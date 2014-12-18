/*
 * mqueue.c
 *
 * This module contains the POSIX wrapper for Semaphore.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "semaphore.h"
#include "queue.h"
void queInit(que_t *q)
{
    q->head = q->tail = NULL;
    q->qSize = 0;
}
int32_t receive(que_t *q,uint8_t *buffer, uint32_t maxLength)
{
    int32_t rLength = -1;
    if(q->qSize > 0 && q->head != NULL)
    {

        q->temp = q->head->ptr;
        memcpy(buffer, q->head->data, q->head->length);
        rLength = (int32_t)q->head->length;

        if (q->temp != NULL)
        {
            free(q->head->data);
            free(q->head);
            q->head = q->temp;
        }
        else
        {
            free(q->head->data);
            free(q->head);
            q->head = NULL;
            q->tail = NULL;
        }

        q->qSize--;

    }
    return rLength;
}
int32_t timedReceive(que_t *q, uint8_t *buffer, uint32_t maxLength,const struct timespec * abs_timeout)
{
    time_t timeout;
    int32_t rLength = -1;
  // calculate timeout
  timeout = ((abs_timeout->tv_sec -time(0)) * (1000UL)) +(abs_timeout->tv_nsec/(1000000UL));
  timeout = timeout/3;

    do
    {

        usleep(3);
        rLength = receive(q,buffer,maxLength);
    }while(timeout-- > (time_t)0 && rLength == -1);
    return rLength;
}
void addToHead(que_t *q,uint8_t *data, uint32_t length)
{
    if (q->head == NULL)
    {
        q->head = (node_t *)malloc(1*sizeof(node_t));
        q->head->ptr = NULL;
        q->head->data = (uint8_t *)malloc(length*(sizeof(uint8_t)));
        q->head->length = length;
        memcpy(q->head->data, data, length);
        q->tail = q->head;
    }
    else
    {
        q->temp=(node_t *)malloc(1*sizeof(node_t));
        q->temp->ptr = q->head;
        q->temp->data = (uint8_t *)malloc(length*(sizeof(uint8_t)));
        q->temp->length = length;
        memcpy(q->temp->data, data, length);

        q->head = q->temp;
    }
    q->qSize++;
}
void addToTail(que_t *q,uint8_t *data, uint32_t length)
{

    if (q->tail == NULL)
    {
        q->tail = (node_t *)malloc(1*sizeof(node_t));
        q->tail->ptr = NULL;
        q->tail->data = (uint8_t *)malloc(length*(sizeof(uint8_t)));
        q->tail->length = length;
        memcpy(q->tail->data, data, length);
        q->head = q->tail;
    }
    else
    {
        q->temp=(node_t *)malloc(1*sizeof(node_t));
        q->tail->ptr = q->temp;
        q->temp->data = (uint8_t *)malloc(length*(sizeof(uint8_t)));
        q->temp->length = length;
        memcpy(q->temp->data, data, length);
        q->temp->ptr = NULL;

        q->tail = q->temp;
    }
    q->qSize++;
}
uint8_t isEmpty(que_t *q)
{

}

uint32_t queuesize(que_t *q)
{
    return q->qSize;
}

