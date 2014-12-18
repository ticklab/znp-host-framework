/*
 * semaphore.h
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

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <time.h>
#include <ti/sysbios/knl/Semaphore.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/* Value returned if `sem_open' failed.  */
#define SEM_FAILED      ((sem_t *) 0)

/************************************************************
 * TYPEDEFS
 */

typedef Semaphore_Handle sem_t;

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      sem_open
 *
 * @brief   creates a new POSIX semaphore or opens an existing semaphore.
 *
 * @param   name - pointer to semaphore name
 * @param   oflag - control operation flag
 *
 * @return  address of the new semaphore or SEM_FAILED if fails
 */
extern sem_t *sem_open(const char *name, int oflag);

/*********************************************************************
 * @fn      sem_close
 *
 * @brief   creates a new POSIX semaphore or opens an existing semaphore.
 *
 * @param   name - pointer to semaphore name
 * @param   oflag - control operation flag
 *
 * @return  address of the new semaphore or SEM_FAILED if fails
 */
extern int sem_close(sem_t *sem);

/*********************************************************************
 * @fn      sem_init
 *
 * @brief   populates the Frame Check Sequence of the RPC payload.
 *
 * @param   msg - pointer to the RPC message
 *
 * @return  none
 */
extern int sem_init(sem_t *sem, int pshared, unsigned int value);

extern int sem_destroy(sem_t *sem);

extern int sem_wait(sem_t * sem);

extern int sem_timedwait(sem_t * sem, const struct timespec * abs_timeout);

extern int sem_post(sem_t * sem);

#ifdef __cplusplus
}
#endif

#endif /* SEMAPHORE_H */
