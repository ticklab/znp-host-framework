/*
 * time.c
 *
 * This module contains the wrapper for the POSIX timespec structure.
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
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
#include <time.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/System.h>

#include <time.h>

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
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * API FUNCTIONS
 */

time_t time(time_t * timer)
{
	time_t curTime;

	// get current timer ticks
	curTime = (time_t) ((Clock_getTicks() * Clock_tickPeriod) / 1000000UL);

	if (timer != NULL)
	{
		*timer = curTime;
	}

	return curTime;
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	(void) tz;
	//Clock_tickPeriod is num ticks per us
	tv->tv_sec = ((Clock_getTicks() * Clock_tickPeriod) / 1000000UL);
	tv->tv_usec = (Clock_getTicks() * Clock_tickPeriod);

	return 0;
}

int nanosleep(timespec* req, timespec* rem)
{

	/* seconds:
	 * sleep_ticks = s * ticks_per_s;
	 * nano seconds (do in period so it as period is >1):
	 * sleep_ticks = ns / period_in_ns;
	 */

	// Clock_tickPeriod is us per period
	uint32_t ticks_per_s = Clock_tickPeriod * 1000;
	uint32_t period_in_ns = Clock_tickPeriod * 1000;

	uint32_t sleepTicks = (uint32_t) req->tv_sec * (ticks_per_s);
	sleepTicks += (uint32_t)(req->tv_nsec / period_in_ns);

	//consolePrint("nanosleep: req->tv_sec=%d, req->tv_nsec=%d \n", req->tv_sec, req->tv_nsec);
	//consolePrint("sleepTicks=%d, Clock_tickPeriod=%d, sleep time=%dus\n", sleepTicks, Clock_tickPeriod, sleepTicks * Clock_tickPeriod);

	Task_sleep(sleepTicks);

	//consolePrint("nanosleep--\n");
	return 0;
}
