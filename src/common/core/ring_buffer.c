/*
 * ring_buffer.c
 *
 *  Created on: 2020. 12. 16.
 *      Author: WANG
 */

#include "ring_buffer.h"

void Init_Q(void)
{

}

bool QueueCreate(Queue_t *Q, uint8_t *Q_buf, uint32_t length)
{
	bool ret = true;

	Q->buffer = Q_buf;
	Q->head = 0;
	Q->tail = 0;
	Q->size = MAX_SIZE;
	Q->temp = 0;
	Q->cnt = 0;

	return ret;
}

uint8_t Q_full(Queue_t *Q)
{
	if((Q->head+1)%MAX_SIZE == Q->tail){
	   return true;
	} return false;
}
uint8_t Q_empty(Queue_t *Q){
	if(Q->head == Q->tail){
		return true;
	} return false;
}

bool Q_write(Queue_t *Q, uint8_t *p_data, uint32_t length){
	if(Q_full(Q)){
		return false;
	}
	for (int i=0; i<length; i++)
	{
		Q->buffer[Q->head] = p_data[i];
		Q->head = (Q->head+1) % MAX_SIZE;
	}
	return true;
}

bool Q_read(Queue_t *Q, uint8_t *p_data, uint32_t length){
	if(Q_empty(Q)){
		return false;
	}
	for (int i=0; i<length; i++)
	{
		p_data[i] = Q->buffer[Q->tail];
		Q->tail = (Q->tail+1) % MAX_SIZE;
	}

	return true;
}

uint32_t QueueAvailable(Queue_t *Q)
{
  uint32_t length;

  length = (Q->head - Q->tail) % Q->size;

  return length;
}

void QueueFlush(Queue_t *Q)
{
  Q->head  = 0;
  Q->tail = 0;
}


