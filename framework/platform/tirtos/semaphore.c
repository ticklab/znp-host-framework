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

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/heaps/HeapMem.h>

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
static int sem_pend(sem_t * sem, UInt timeout);

/*********************************************************************
 * API FUNCTIONS
 */

sem_t *sem_open(const char *name, int oflag)
{
	sem_t* sem;
	Error_Block eb;

	// init error block
	Error_init(&eb);

	// allocate memory for handle
	sem = (sem_t *) Memory_alloc(NULL, sizeof(Semaphore_Handle), 0, &eb);
	if (sem == NULL)
	{
		// set error
		sem = SEM_FAILED;

		// set errno
		errno = ENOMEM;
	}

	return sem;
}

int sem_close(sem_t *sem)
{
	int ret = -1;

	if (sem != SEM_FAILED)
	{
		// free memory
		Memory_free(NULL, sem, sizeof(Semaphore_Handle));

		// return success
		ret = 0;
	}
	else
	{
		// set errno
		errno = EINVAL;
	}

	return ret;
}

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
	int ret = 0;
	Semaphore_Params params;
	Error_Block eb;

	// init params
	Semaphore_Params_init(&params);
	Error_init(&eb);

	// create semaphore instance
	(*sem) = Semaphore_create((Int) value, &params, &eb);
	if (*sem == NULL)
	{
		// error
		ret = -1;

		// set errno
		errno = ENOMEM;
	}

	return ret;
}

int sem_destroy(sem_t *sem)
{
	int ret = -1;

	if (sem != SEM_FAILED)
	{
		// just delete semaphore
		Semaphore_delete(sem);

		// return success
		ret = 0;
	}
	else
	{
		// set errno
		errno = EINVAL;
	}

	return ret;
}

int sem_wait(sem_t * sem)
{
	return (sem_pend(sem, BIOS_WAIT_FOREVER));
}

int sem_timedwait(sem_t * sem, const struct timespec * abs_timeout)
{
	UInt timeout;

	// calculate timeout
	timeout = ((abs_timeout->tv_sec - time(0)) * (1000000UL / Clock_tickPeriod))
	        + (abs_timeout->tv_nsec / (Clock_tickPeriod * 1000UL));

	return (sem_pend(sem, timeout));
}

int sem_post(sem_t * sem)
{
	int ret = -1;

	if (sem != SEM_FAILED)
	{
		//consolePrint("Semaphore_post++\n");

		// post semaphore
		Semaphore_post(*sem);

		// return success
		ret = 0;
	}
	else
	{
		// set errno
		errno = EINVAL;
	}

	return ret;
}

/*********************************************************************
 * LOCAL FUNCTIONS DEFINITIONS
 */
static int sem_pend(sem_t * sem, UInt timeout)
{
	int ret = -1;

	if (sem != SEM_FAILED)
	{
		//consolePrint("Semaphore_pend(%d)++\n", timeout);
		// wait for semaphore forever
		if (Semaphore_pend(*sem, timeout) == TRUE)
		{
			//consolePrint("Semaphore_pend(%d)--\n");
			// return success
			ret = 0;
		}
	}
	else
	{
		// set errno
		errno = EINVAL;
	}

	//consolePrint("Semaphore_pend--: error\n");

	return ret;
}
