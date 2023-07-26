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

#pragma once

#include "def.h"
#include "hw_def.h"
#include "time.h"
#include "config/config.h"
#include "pg/scheduler.h"

#define TASK_PERIOD_HZ(hz) (1000000 / (hz))
#define TASK_PERIOD_MS(ms) ((ms) * 1000)
#define TASK_PERIOD_US(us) (us)

#define TASK_STATS_MOVING_SUM_COUNT     8

#define LOAD_PERCENTAGE_ONE             100

#define SCHED_TASK_DEFER_MASK           0x07 // Scheduler loop count is masked with this and when 0 long running tasks are processed

#define SCHED_START_LOOP_MIN_US         1   // Wait at start of scheduler loop if gyroTask is nearly due
#define SCHED_START_LOOP_MAX_US         12
#define SCHED_START_LOOP_DOWN_STEP      50  // Fraction of a us to reduce start loop wait
#define SCHED_START_LOOP_UP_STEP        1   // Fraction of a us to increase start loop wait

#define TASK_GUARD_MARGIN_MIN_US        3   // Add an amount to the estimate of a task duration
#define TASK_GUARD_MARGIN_MAX_US        6
#define TASK_GUARD_MARGIN_DOWN_STEP     50  // Fraction of a us to reduce task guard margin
#define TASK_GUARD_MARGIN_UP_STEP       1   // Fraction of a us to increase task guard margin

#define CHECK_GUARD_MARGIN_US           2   // Add a margin to the amount of time allowed for a check function to run

// Some tasks have occasional peaks in execution time so normal moving average duration estimation doesn't work
// Decay the estimated max task duration by 1/(1 << TASK_EXEC_TIME_SHIFT) on every invocation
#define TASK_EXEC_TIME_SHIFT            7

#define TASK_AGE_EXPEDITE_RX            25  // Make RX tasks more schedulable if it's failed to be scheduled this many times
#define TASK_AGE_EXPEDITE_OSD           25  // Make OSD tasks more schedulable if it's failed to be scheduled this many times
#define TASK_AGE_EXPEDITE_COUNT         1   // Make aged tasks more schedulable
#define TASK_AGE_EXPEDITE_SCALE         0.9 // By scaling their expected execution time

// Gyro interrupt counts over which to measure loop time and skew
#define GYRO_RATE_COUNT 25000
#define GYRO_LOCK_COUNT 50

#define TASK_GYROPID_DESIRED_PERIOD     125 // 125us = 8kHz
#define SCHEDULER_DELAY_LIMIT           10


typedef enum {
    TASK_PRIORITY_REALTIME = -1, // Task will be run outside the scheduler logic
    TASK_PRIORITY_LOWEST = 1,
    TASK_PRIORITY_LOW = 2,
    TASK_PRIORITY_MEDIUM = 3,
    TASK_PRIORITY_MEDIUM_HIGH = 4,
    TASK_PRIORITY_HIGH = 5,
    TASK_PRIORITY_MAX = 255
} taskPriority_e;

typedef struct {
    uint32_t     maxExecutionTimeUs;
    uint32_t     totalExecutionTimeUs;
    uint32_t     averageExecutionTimeUs;
    uint32_t     averageDeltaTimeUs;
} cfCheckFuncInfo_t;

typedef struct {
    const char * taskName;
    const char * subTaskName;
    bool         isEnabled;
    int8_t       staticPriority;
    int32_t  desiredPeriodUs;
    int32_t  latestDeltaTimeUs;
    uint32_t     maxExecutionTimeUs;
    uint32_t     totalExecutionTimeUs;
    uint32_t     averageExecutionTime10thUs;
    uint32_t     averageDeltaTime10thUs;
    float        movingAverageCycleTimeUs;
#if defined(USE_LATE_TASK_STATISTICS)
    uint32_t     runCount;
    uint32_t     lateCount;
    uint32_t     execTime;
#endif
} taskInfo_t;

typedef enum {
    /* Actual tasks */
    TASK_SYSTEM = 0,
    TASK_MAIN,
    TASK_GYRO,
    TASK_FILTER,
    TASK_PID,
    TASK_ACCEL,
    TASK_ATTITUDE,
    TASK_RX,
    TASK_SERIAL,
    TASK_LED,
    TASK_DEBUG,
    TASK_BATTERY_VOLTAGE,
    TASK_BATTERY_CURRENT,
    // TASK_BATTERY_ALERTS,
#ifdef USE_BEEPER
    TASK_BEEPER,
#endif
    TASK_GPS,
    TASK_COMPASS,
    TASK_BARO,
    TASK_ALTITUDE,
#ifdef USE_TELEMETRY
    TASK_TELEMETRY,
#endif
#ifdef USE_LED_STRIP
    TASK_LEDSTRIP,
#endif
#ifdef USE_STACK_CHECK
    TASK_STACK_CHECK,
#endif
#ifdef USE_ADC_INTERNAL
    TASK_ADC_INTERNAL,
#endif

    /* Count of real tasks */
    TASK_COUNT,

    /* Service task IDs */
    TASK_NONE = TASK_COUNT,
    TASK_SELF
} taskId_e;

typedef struct {
    // Configuration
    const char * taskName;
    const char * subTaskName;
    bool (*checkFunc)(uint32_t currentTimeUs, int32_t currentDeltaTimeUs);
    void (*taskFunc)(uint32_t currentTimeUs);
    int32_t desiredPeriodUs;        // target period of execution
    const int8_t staticPriority;        // dynamicPriority grows in steps of this size
} task_attribute_t;

typedef struct {
    // Task static data
    task_attribute_t *attribute;

    // Scheduling
    uint16_t dynamicPriority;           // measurement of how old task was last executed, used to avoid task starvation
    uint16_t taskAgePeriods;
    int32_t taskLatestDeltaTimeUs;
    uint32_t lastExecutedAtUs;          // last time of invocation
    uint32_t lastSignaledAtUs;          // time of invocation event for event-driven tasks
    uint32_t lastDesiredAt;             // time of last desired execution

    // Statistics
    float    movingAverageCycleTimeUs;
    uint32_t anticipatedExecutionTime;  // Fixed point expectation of next execution time
    uint32_t movingSumDeltaTime10thUs;  // moving sum over 64 samples
    uint32_t movingSumExecutionTime10thUs;
    uint32_t maxExecutionTimeUs;
    uint32_t totalExecutionTimeUs;      // total time consumed by task since boot
    uint32_t lastStatsAtUs;             // time of last stats gathering for rate calculation
#if defined(USE_LATE_TASK_STATISTICS)
    uint32_t runCount;
    uint32_t lateCount;
    uint32_t execTime;
#endif
} task_t;

void getCheckFuncInfo(cfCheckFuncInfo_t *checkFuncInfo);
void getTaskInfo(taskId_e taskId, taskInfo_t *taskInfo);
void rescheduleTask(taskId_e taskId, int32_t newPeriodUs);
void setTaskEnabled(taskId_e taskId, bool newEnabledState);
int32_t getTaskDeltaTimeUs(taskId_e taskId);
void schedulerIgnoreTaskStateTime();
void schedulerIgnoreTaskExecRate();
void schedulerIgnoreTaskExecTime();
bool schedulerGetIgnoreTaskExecTime();
void schedulerResetTaskStatistics(taskId_e taskId);
void schedulerResetTaskMaxExecutionTime(taskId_e taskId);
void schedulerResetCheckFunctionMaxExecutionTime(void);
void schedulerSetNextStateTime(int32_t nextStateTime);
int32_t schedulerGetNextStateTime();
void schedulerInit(void);
void scheduler(void);
uint32_t schedulerExecuteTask(task_t *selectedTask, uint32_t currentTimeUs);
void taskSystemLoad(uint32_t currentTimeUs);
void schedulerEnableGyro(void);
uint16_t getAverageSystemLoadPercent(void);
float schedulerGetCycleTimeMultiplier(void);
