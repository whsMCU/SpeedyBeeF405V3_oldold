/*
 * ring_buffer.h
 *
 *  Created on: 2020. 12. 16.
 *      Author: WANG
 */

#ifndef SRC_COMMON_CORE_RING_BUFFER_H_
#define SRC_COMMON_CORE_RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

#define MAX_SIZE 255

typedef struct queue {
  volatile uint8_t *buffer;
  volatile uint8_t head;
  volatile uint8_t tail;
  volatile uint8_t size;
  volatile uint8_t temp;
  volatile int cnt;
} Queue_t;

void     Init_Q(void);
bool     QueueCreate(Queue_t *p_node, uint8_t *p_buf, uint32_t length);
uint8_t  Q_full(Queue_t *Q);
uint8_t  Q_empty(Queue_t *Q);
bool     Q_write(Queue_t *p_node, uint8_t *p_data, uint32_t length);
bool     Q_read(Queue_t *p_node, uint8_t *p_data, uint32_t length);
uint32_t QueueAvailable(Queue_t *Q);
void     QueueFlush(Queue_t *Q);

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_CORE_RING_BUFFER_H_ */
