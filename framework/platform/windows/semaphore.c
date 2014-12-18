/*
 * semaphore.c
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

/*********************************************************************
 * INCLUDES
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



#include "semaphore.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * CONSTANTS
 */

/************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS DECLARATIONS
 */


/*********************************************************************
 * API FUNCTIONS
 */

sem_t *sem_open(const char *name, int oflag)
{
  sem_t* sem;


  // allocate memory for handle
  sem = (sem_t *) malloc(sizeof(sem_t));
  if(sem == NULL)
  {
    // set error
	sem = SEM_FAILED;
  }

  return sem;
}

int sem_close(sem_t *sem)
{
  int ret = 0;



  return ret;
}

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
  int ret = 0;

  // create semaphore instance
  (*sem) = (sem_t)value;
  if(*sem == NULL)
  {
    // error
    ret = -1;

    // set errno - TODO
  }

  return ret;
}

int sem_destroy (sem_t *sem)
{
  int ret = -1;



  return ret;
}

int sem_wait(sem_t * sem)
{

    while(*sem != (sem_t)1);
    *sem = (sem_t)0;
  return 0;
}

int sem_timedwait(sem_t * sem, const struct timespec * abs_timeout)
{
  time_t timeout;

  // calculate timeout
  timeout = ((abs_timeout->tv_sec -time(0)) * (1000UL)) +(abs_timeout->tv_nsec/(1000UL));
    while(timeout-- > 0 && *sem != (sem_t)1)
    {
        usleep(1);
    }
    if(*sem == (sem_t)1)
    {
        *sem = (sem_t)0;
        return 0;
    }

  return -1;
}

int sem_post(sem_t * sem)
{
  int ret = -1;

  if(*sem == (sem_t)0)
  {
	// post semaphore
	*sem = (sem_t)1;

	// return success
	ret = 0;
  }


  return ret;
}


/*********************************************************************
 * LOCAL FUNCTIONS DEFINITIONS
 */

