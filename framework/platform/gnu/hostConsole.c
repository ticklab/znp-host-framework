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
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "hostConsole.h"
#include "dbgPrint.h"

/*********************************************************************
 * MACROS
 */

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
char consoleGetCh()
{
	//struct termios old, new;
	char c;

	struct termios old =
		{ 0 };

	fflush(stdout);
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	//old.c_lflag&=~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");

	if (read(0, &c, 1) < 0)
		perror("read()");

	old.c_lflag |= ICANON;
	//old.c_lflag|=ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");

	return c;
}

int idxpHist = 0;
int pHistIsRollover = 0;
int pHistMax = 256;
char pHist[256][256];
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
	int chIdx, isDir, cPos, currpHist;
	char ch;
	char tempStr[256];
	char temppHist[256];
	chIdx = isDir = cPos = currpHist = 0;
	ln[0] = '\0';
	while (chIdx < maxLen)
	{
		ch = consoleGetCh();
		if ((ch == '\n') || (chIdx == (maxLen - 1)))
		{

			if ((idxpHist == 0 && pHistIsRollover == 0)
			        || strcmp(pHist[(pHistMax + idxpHist - 1) % pHistMax], ln)
			                != 0)
			{
				strcpy(pHist[idxpHist], ln);
				idxpHist = ((idxpHist + 1) % pHistMax);
			}
			if (idxpHist == 0)
			{
				pHistIsRollover = 1;
			}

			break;
		}
		switch (ch)
		{
		case 127:
			//backspace was press
			//clear line
			if (chIdx > 0)
			{
				if (cPos < chIdx)
				{
					int x;
					for (x = 0; x < cPos; x++)
					{
						ln[chIdx - cPos + x - 1] = tempStr[cPos - x - 1];
					}
					ln[--chIdx] = '\0';
				}
				consoleClearLn();
				printf("\r%s", ln);

			}
			else
			{
				consoleClearLn();
				printf("\r");
			}
			break;
		case 27:
			isDir = 1;
			if (chIdx > 0)
			{
				consoleClearLn();
				printf("\r%s", ln);

			}
			else
			{
				consoleClearLn();
				printf("\r");
			}

			break;
		default:
			if (isDir)
			{
				if (ch == '[')
				{
					isDir = 1;
				}
				else if (ch == 'A')
				{
					if (currpHist < idxpHist)
					{
						cPos = 0;
						if (currpHist)
						{
							strcpy(pHist[idxpHist - currpHist], ln);
						}
						if (currpHist == 0)
							strcpy(temppHist, ln);
						currpHist++;
						strcpy(ln, pHist[idxpHist - currpHist]);
						chIdx = strlen(ln);

					}

					isDir = 0;
				}
				else if (ch == 'B')
				{
					if (currpHist)
					{
						strcpy(pHist[idxpHist - currpHist], ln);
						currpHist--;
						if (currpHist)
						{
							strcpy(ln, pHist[idxpHist - currpHist]);
						}
						else
						{
							strcpy(ln, temppHist);
						}
						chIdx = strlen(ln);

					}
					isDir = 0;
				}
				else if (ch == 'C')
				{
					if (cPos > 0)
					{
						cPos--;
					}

					isDir = 0;
				}
				else if (ch == 'D')
				{
					if (cPos < chIdx)
					{
						tempStr[cPos] = ln[chIdx - cPos - 1];
						cPos++;
					}
					isDir = 0;
				}
				else
				{
					isDir = 0;
				}
				//consoleClearLn();
				//printf("\r%s", ln);
			}
			else
			{
				if (cPos != 0)
				{
					ln[chIdx - cPos] = ch;
					int x;
					for (x = 1; x <= cPos; x++)
					{
						ln[chIdx - cPos + x] = tempStr[cPos - x];
					}
					ln[++chIdx] = '\0';
				}
				else
				{
					ln[chIdx++] = ch;
					ln[chIdx] = '\0';
				}

			}
			consoleClearLn()
			;
			printf("\r%s", ln);
			break;
		}
		int x;
		for (x = 1; x <= cPos; x++)
		{
			consolePrint("\b");
		}
	}

	return chIdx;
}
