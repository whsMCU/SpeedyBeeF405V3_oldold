/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "bmi270.h"
#include "gpio.h"
#include "spi.h"
#include "led.h"
#include "cli.h"
#include "axis.h"
#include "gyro_init.h"

#ifdef USE_ACCGYRO_BMI270

// 10 MHz max SPI frequency
#define BMI270_MAX_SPI_CLK_HZ 10000000

#define BMI270_FIFO_FRAME_SIZE 6

#define BMI270_CONFIG_SIZE 328

// Declaration for the device config (microcode) that must be uploaded to the sensor
extern const uint8_t bmi270_maximum_fifo_config_file[BMI270_CONFIG_SIZE];

#define BMI270_CHIP_ID 0x24

// BMI270 registers (not the complete list)
typedef enum {
    BMI270_REG_CHIP_ID = 0x00,
    BMI270_REG_ERR_REG = 0x02,
    BMI270_REG_STATUS = 0x03,
    BMI270_REG_ACC_DATA_X_LSB = 0x0C,
    BMI270_REG_GYR_DATA_X_LSB = 0x12,
    BMI270_REG_SENSORTIME_0 = 0x18,
    BMI270_REG_SENSORTIME_1 = 0x19,
    BMI270_REG_SENSORTIME_2 = 0x1A,
    BMI270_REG_EVENT = 0x1B,
    BMI270_REG_INT_STATUS_0 = 0x1C,
    BMI270_REG_INT_STATUS_1 = 0x1D,
    BMI270_REG_INTERNAL_STATUS = 0x21,
    BMI270_REG_TEMPERATURE_LSB = 0x22,
    BMI270_REG_TEMPERATURE_MSB = 0x23,
    BMI270_REG_FIFO_LENGTH_LSB = 0x24,
    BMI270_REG_FIFO_LENGTH_MSB = 0x25,
    BMI270_REG_FIFO_DATA = 0x26,
    BMI270_REG_ACC_CONF = 0x40,
    BMI270_REG_ACC_RANGE = 0x41,
    BMI270_REG_GYRO_CONF = 0x42,
    BMI270_REG_GYRO_RANGE = 0x43,
    BMI270_REG_AUX_CONF = 0x44,
    BMI270_REG_FIFO_DOWNS = 0x45,
    BMI270_REG_FIFO_WTM_0 = 0x46,
    BMI270_REG_FIFO_WTM_1 = 0x47,
    BMI270_REG_FIFO_CONFIG_0 = 0x48,
    BMI270_REG_FIFO_CONFIG_1 = 0x49,
    BMI270_REG_SATURATION = 0x4A,
    BMI270_REG_INT1_IO_CTRL = 0x53,
    BMI270_REG_INT2_IO_CTRL = 0x54,
    BMI270_REG_INT_LATCH = 0x55,
    BMI270_REG_INT1_MAP_FEAT = 0x56,
    BMI270_REG_INT2_MAP_FEAT = 0x57,
    BMI270_REG_INT_MAP_DATA = 0x58,
    BMI270_REG_INIT_CTRL = 0x59,
    BMI270_REG_INIT_DATA = 0x5E,
    BMI270_REG_ACC_SELF_TEST = 0x6D,
    BMI270_REG_GYR_SELF_TEST_AXES = 0x6E,
    BMI270_REG_PWR_CONF = 0x7C,
    BMI270_REG_PWR_CTRL = 0x7D,
    BMI270_REG_CMD = 0x7E,
} bmi270Register_e;

// BMI270 register configuration values
typedef enum {
    BMI270_VAL_CMD_SOFTRESET = 0xB6,
    BMI270_VAL_CMD_FIFOFLUSH = 0xB0,
    BMI270_VAL_PWR_CTRL = 0x0E,              // enable gyro, acc and temp sensors
    BMI270_VAL_PWR_CONF = 0x02,              // disable advanced power save, enable FIFO self-wake
    BMI270_VAL_ACC_CONF_ODR800 = 0x0B,       // set acc sample rate to 800hz
    BMI270_VAL_ACC_CONF_ODR1600 = 0x0C,      // set acc sample rate to 1600hz
    BMI270_VAL_ACC_CONF_BWP = 0x02,          // set acc filter in normal mode
    BMI270_VAL_ACC_CONF_HP = 0x01,           // set acc in high performance mode
    BMI270_VAL_ACC_RANGE_8G = 0x02,          // set acc to 8G full scale
    BMI270_VAL_ACC_RANGE_16G = 0x03,         // set acc to 16G full scale
    BMI270_VAL_GYRO_CONF_ODR3200 = 0x0D,     // set gyro sample rate to 3200hz
    BMI270_VAL_GYRO_CONF_BWP_OSR4 = 0x00,    // set gyro filter in OSR4 mode
    BMI270_VAL_GYRO_CONF_BWP_OSR2 = 0x01,    // set gyro filter in OSR2 mode
    BMI270_VAL_GYRO_CONF_BWP_NORM = 0x02,    // set gyro filter in normal mode
    BMI270_VAL_GYRO_CONF_NOISE_PERF = 0x01,  // set gyro in high performance noise mode
    BMI270_VAL_GYRO_CONF_FILTER_PERF = 0x01, // set gyro in high performance filter mode

    BMI270_VAL_GYRO_RANGE_2000DPS = 0x08,    // set gyro to 2000dps full scale
                                             // for some reason you have to enable the ois_range bit (bit 3) for 2000dps as well
                                             // or else the gyro scale will be 250dps when in prefiltered FIFO mode (not documented in datasheet!)

    BMI270_VAL_INT_MAP_DATA_DRDY_INT1 = 0x04,// enable the data ready interrupt pin 1
    BMI270_VAL_INT_MAP_FIFO_WM_INT1 = 0x02,  // enable the FIFO watermark interrupt pin 1
    BMI270_VAL_INT1_IO_CTRL_PINMODE = 0x0A,  // active high, push-pull, output enabled, input disabled 
    BMI270_VAL_FIFO_CONFIG_0 = 0x00,         // don't stop when full, disable sensortime frame
    BMI270_VAL_FIFO_CONFIG_1 = 0x80,         // only gyro data in FIFO, use headerless mode
    BMI270_VAL_FIFO_DOWNS = 0x00,            // select unfiltered gyro data with no downsampling (6.4KHz samples)
    BMI270_VAL_FIFO_WTM_0 = 0x06,            // set the FIFO watermark level to 1 gyro sample (6 bytes)
    BMI270_VAL_FIFO_WTM_1 = 0x00,            // FIFO watermark MSB
} bmi270ConfigValues_e;

// Need to see at least this many interrupts during initialisation to confirm EXTI connectivity
#define GYRO_EXTI_DETECT_THRESHOLD 1000

static uint8_t _buffer[16] = {0, };

#ifdef _USE_HW_CLI
static void cliBmi270(cli_args_t *args);
#endif

// Toggle the CS to switch the device into SPI mode.
// Device switches initializes as I2C and switches to SPI on a low to high CS transition
static void bmi270EnableSPI(uint8_t ch)
{
    gpioPinWrite(_PIN_DEF_CS, _DEF_LOW);
    delay(1);
    gpioPinWrite(_PIN_DEF_CS, _DEF_HIGH);
    delay(10);
}

static void bmi270UploadConfig(uint8_t ch)
{
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_PWR_CONF, 0, 1);
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_INIT_CTRL, 0, 1);

    // Transfer the config file
    SPI_ByteWrite(_DEF_SPI1, BMI270_REG_INIT_DATA, (uint8_t *)bmi270_maximum_fifo_config_file, sizeof(bmi270_maximum_fifo_config_file));

    delay(10);
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_INIT_CTRL, 1, 1);
}


static void bmi270Config(void)
{
    // If running in hardware_lpf experimental mode then switch to FIFO-based,
    // 6.4KHz sampling, unfiltered data vs. the default 3.2KHz with hardware filtering
#ifdef USE_GYRO_DLPF_EXPERIMENTAL
    const bool fifoMode = (gyro->hardware_lpf == GYRO_HARDWARE_LPF_EXPERIMENTAL);
#else
    const bool fifoMode = false;
#endif

    // Perform a soft reset to set all configuration to default
    // Delay 100ms before continuing configuration
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_CMD, BMI270_VAL_CMD_SOFTRESET, 100);
    // Toggle the chip into SPI mode
    bmi270EnableSPI(0);

    bmi270UploadConfig(_DEF_SPI1);

    // Configure the FIFO
    if (fifoMode) {
        SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_FIFO_CONFIG_0, BMI270_VAL_FIFO_CONFIG_0, 1);
        SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_FIFO_CONFIG_1, BMI270_VAL_FIFO_CONFIG_1, 1);
        SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_FIFO_DOWNS, BMI270_VAL_FIFO_DOWNS, 1);
        SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_FIFO_WTM_0, BMI270_VAL_FIFO_WTM_0, 1);
        SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_FIFO_WTM_1, BMI270_VAL_FIFO_WTM_1, 1);
    }

    // Configure the accelerometer
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_ACC_CONF, (BMI270_VAL_ACC_CONF_HP << 7) | (BMI270_VAL_ACC_CONF_BWP << 4) | BMI270_VAL_ACC_CONF_ODR800, 1);

    // Configure the accelerometer full-scale range
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_ACC_RANGE, BMI270_VAL_ACC_RANGE_16G, 1);

    // Configure the gyro
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_GYRO_CONF, (BMI270_VAL_GYRO_CONF_FILTER_PERF << 7) | (BMI270_VAL_GYRO_CONF_NOISE_PERF << 6) | (BMI270_VAL_GYRO_CONF_BWP_OSR4 << 4) | BMI270_VAL_GYRO_CONF_ODR3200, 1);

    // Configure the gyro full-range scale
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_GYRO_RANGE, BMI270_VAL_GYRO_RANGE_2000DPS, 1);

    // Configure the gyro data ready interrupt
    if (fifoMode) {
        // Interrupt driven by FIFO watermark level
        SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_INT_MAP_DATA, BMI270_VAL_INT_MAP_FIFO_WM_INT1, 1);
    } else {
        // Interrupt driven by data ready
        SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_INT_MAP_DATA, BMI270_VAL_INT_MAP_DATA_DRDY_INT1, 1);
    }

    // Configure the behavior of the INT1 pin
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_INT1_IO_CTRL, BMI270_VAL_INT1_IO_CTRL_PINMODE, 1);

    // Configure the device for  performance mode
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_PWR_CONF, BMI270_VAL_PWR_CONF, 1);

    // Enable the gyro, accelerometer and temperature sensor - disable aux interface
    SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_PWR_CTRL, BMI270_VAL_PWR_CTRL, 1);

    // Flush the FIFO
    if (fifoMode) {
      SPI_RegisterWrite(_DEF_SPI1, BMI270_REG_CMD, BMI270_VAL_CMD_FIFOFLUSH, 1);
    }
}

static void bmi270SpiAccInit(accDev_t *acc)
{
    // sensor is configured during gyro init
    acc->acc_1G = 512 * 4;   // 16G sensor scale
}

bool bmi270SpiAccDetect(accDev_t *acc)
{

    acc->initFn = bmi270SpiAccInit;
    acc->readFn = bmi270SpiAccRead;

    return true;
}


static bool bmi270SpiGyroDetect(gyroDev_t *gyro)
{
    gyro->initFn = bmi270Config;
    gyro->readFn = bmi270SpiGyroRead;
    gyro->scale = GYRO_SCALE_2000DPS;

    return true;
}

bool bmi270_Init(void)
{
    bool ret = true;

    bmi270EnableSPI(0);
    // Allow 100ms before attempting to access gyro's SPI bus
    // Do this once here rather than in each detection routine to speed boot
    while (millis() < 100);
    delay(35);

    gyroInit();
    gyroSetTargetLooptime(activePidLoopDenom);
    gyroStartCalibration(false);

    bmi270SpiGyroDetect(&gyro.gyroSensor1.gyroDev);

    accInit(gyro.accSampleRateHz);

    accStartCalibration();
    
    //gyro_instace = gyro;

    bmi270Config();

    for(int i = 0; i > 5; i++)
    {
        if (bmi270Detect(_DEF_SPI1))
        {
            bmi270Config();
            break;
        }
        delay(100);
    }

    #ifdef _USE_HW_CLI
        cliAdd("bmi270", cliBmi270);
    #endif

    return ret;
}

bool bmi270Detect(uint8_t ch)
{
    memset(_buffer, 0x00, 7);
    SPI_ByteRead(ch, BMI270_REG_CHIP_ID | 0x80, _buffer, 2);
    if (_buffer[1] == BMI270_CHIP_ID)
    {
        return true;
    }
    return false;
}

bool bmi270SpiAccRead(accDev_t *acc)
{
    HAL_StatusTypeDef status = 0;
    uint8_t data_status[2] = {0, 0};
    static uint32_t test = 0;
    SPI_ByteRead(_DEF_SPI1, BMI270_REG_STATUS | 0x80, data_status, 2);
    //memset(_buffer, 0x00, 7);
    if(data_status[1] & 0x80)
    {
        status = SPI_ByteRead(_DEF_SPI1, BMI270_REG_ACC_DATA_X_LSB | 0x80, _buffer, 7);
        acc->ADCRaw[X] = (int16_t)((uint16_t)_buffer[2]<<8 | (uint16_t)_buffer[1]);
        acc->ADCRaw[Y] = (int16_t)((uint16_t)_buffer[4]<<8 | (uint16_t)_buffer[3]);
        acc->ADCRaw[Z] = (int16_t)((uint16_t)_buffer[6]<<8 | (uint16_t)_buffer[5]);
    }else
    {
        test +=1;
    }
    

    if(status == HAL_OK)
    {
        return true;
    }
    return false;
}
bool bmi270SpiGyroRead(gyroDev_t *gyro)
{
    HAL_StatusTypeDef status = 0;
    //memset(_buffer, 0x00, 7);
    status = SPI_ByteRead(_DEF_SPI1, BMI270_REG_GYR_DATA_X_LSB | 0x80, _buffer, 7);
    gyro->gyroADCRaw[X] = (int16_t)((uint16_t)_buffer[2]<<8 | (uint16_t)_buffer[1]);
    gyro->gyroADCRaw[Y] = (int16_t)((uint16_t)_buffer[4]<<8 | (uint16_t)_buffer[3]);
    gyro->gyroADCRaw[Z] = (int16_t)((uint16_t)_buffer[6]<<8 | (uint16_t)_buffer[5]);
    if(status == HAL_OK)
    {
        return true;
    }
    return false;
}

static void (*frameCallBack)(void) = NULL;

bool bmi270SetCallBack(void (*p_func)(void))
{
  frameCallBack = p_func;

  return true;
}

uint8_t bmi270InterruptStatus(gyroDev_t *gyro)
{
    uint8_t buffer[2] = {0, 0};
    SPI_ByteRead(gyro->gyro_bus_ch, BMI270_REG_INT_STATUS_1 | 0x80, buffer, 2);
    return buffer[1];
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_4)
    {
        gyro.rawSensorDev->dataReady = true;
        //gyro_instace->imuDev.InterruptStatus = bmi270InterruptStatus(gyro_instace);
    }
}

#ifdef _USE_HW_CLI
void cliBmi270(cli_args_t *args)
{
  bool ret = false;

if (args->argc == 1 && args->isStr(0, "gyro_show") == true)
  {
    uint32_t pre_time;
 	pre_time = millis();
    int16_t x=0, y=0, z=0;
    while(cliKeepLoop())
    {
        if (millis()-pre_time >= 1000)
    	{
     		pre_time = millis();
			memset(_buffer, 0x00, 7);
            SPI_ByteRead(_DEF_SPI1, (BMI270_REG_GYR_DATA_X_LSB | 0x80), _buffer, 7);
            x = (uint16_t)_buffer[2]<<8 | (uint16_t)_buffer[1];
            y = (uint16_t)_buffer[4]<<8 | (uint16_t)_buffer[3];
            z = (uint16_t)_buffer[6]<<8 | (uint16_t)_buffer[5];
            cliPrintf("gyro x: %d, y: %d, z: %d\n\r", x, y, z);
    	}
    }
    ret = true;
  }

if (args->argc == 1 && args->isStr(0, "acc_show") == true)
{
    uint32_t pre_time;
    pre_time = millis();
    int16_t x=0, y=0, z=0;
while(cliKeepLoop())
{
    if (millis()-pre_time >= 1000)
    {
        pre_time = millis();
        memset(_buffer, 0x00, 7);
        SPI_ByteRead(_DEF_SPI1, (BMI270_REG_ACC_DATA_X_LSB | 0x80), _buffer, 7);
        x = (uint16_t)_buffer[2]<<8 | (uint16_t)_buffer[1];
        y = (uint16_t)_buffer[4]<<8 | (uint16_t)_buffer[3];
        z = (uint16_t)_buffer[6]<<8 | (uint16_t)_buffer[5];
        cliPrintf("acc x: %d, y: %d, z: %d\n\r", x, y, z);
    }
}
ret = true;
}

  if (args->argc == 3 && args->isStr(0, "mem_read") == true)
  {
    uint8_t ch;
    uint8_t addr;
    uint8_t buffer[2] = {0, 0};
    HAL_StatusTypeDef status;

    ch   = (uint8_t)args->getData(1);
    addr = (uint8_t)args->getData(2);
    addr |= 0x80;

    status = SPI_ByteRead(ch, addr, buffer, 2);

    if(status == HAL_OK)
    {
        cliPrintf("bmi270 mem_read : ch (%d), addr (0x%X), data[0] : (0x%X), data[1] : (0x%X), status (%d)\n", ch, addr, buffer[0], buffer[1], status);
    }else
    {
        cliPrintf("bmi270 read - Fail(%d) \n", status);
    }
    ret = true;
  }

    if (args->argc == 4 && args->isStr(0, "mem_write") == true)
  {
    uint8_t ch;
    uint8_t addr;
    uint8_t buffer;
    HAL_StatusTypeDef status;

    ch     = (uint8_t)args->getData(1);
    addr   = (uint8_t)args->getData(2);
    buffer = (uint8_t)args->getData(3);

    status = SPI_ByteWrite(ch, addr, &buffer, 1);

    if(status == HAL_OK)
    {
        cliPrintf("bmi270 mem_write : ch (%d), addr (0x%X), data : (0x%X), status (%d)\n", ch, addr, buffer, status);
    }else
    {
        cliPrintf("bmi270 write - Fail(%d) \n", status);
    }
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("bmi270 gyro_show \n\r");
    cliPrintf("bmi270 acc_show \n\r");
    cliPrintf("bmi270 mem_read ch0:1, addr \n\r");
    cliPrintf("bmi270 mem_write ch0:1, addr data \n\r");
  }
}
#endif



#endif // USE_ACCGYRO_BMI270
