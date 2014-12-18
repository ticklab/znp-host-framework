/*
 * hostConsole.c
 *
 * This module contains the API for console support.
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
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include "hostConsole.h"
#include "UARTConsole.h"

/*********************************************************************
 * MACROS
 */
#define MAX_STRING_LEN 128

/*********************************************************************
 * LOCAL VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * API FUNCTIONS
 */

/**************************************************************************************************
 * @fn          consoleGetCh
 *
 * @brief       This function gets a char from the console
 *
 * input parameters
 *
 * @param
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void consolePrint(const char *fmt, ...)
{
	char strBuff[MAX_STRING_LEN];
	int32_t strLen;

	va_list argp;
	va_start(argp, fmt);
	strLen = vsnprintf(strBuff, MAX_STRING_LEN, fmt, argp);

	if (strLen > 0)
	{
		UARTConsole_write((const char *) strBuff, strLen);
	}

	va_end(argp);
}

/**************************************************************************************************
 * @fn          consoleGetCh
 *
 * @brief       This function gets a char from the console
 *
 * input parameters
 *
 * @param
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
char consoleGetCh()
{
	char c;

	UARTConsole_getch(&c);

	return c;
}

/**************************************************************************************************
 * @fn          consoleGetLine
 *
 * @brief       This function gets a line from the console.
 *
 * input parameters
 *
 * @param
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
int consoleGetLine(char *ln, int maxLen)
{
	int chIdx;

	for (chIdx = 0; chIdx < maxLen - 1; chIdx++)
	{
		if ((ln[chIdx] = consoleGetCh()) == '\n')
		{
			break;
		}
	}

	//null terminate
	ln[chIdx] = '\0';

	return chIdx;
}
