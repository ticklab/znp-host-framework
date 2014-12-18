/*
 * rpcTransportUart.c
 *
 * This module contains the API for the zll SoC Host Interface.
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
#include <windows.h>
//include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
//#include <sys/ioctl.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#include <commdlg.h>

//#include "rpc.h"

#include "dbgPrint.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
/*******************************/
/*** Bootloader Commands ***/
/*******************************/
#define SB_FORCE_BOOT               0xF8
#define SB_FORCE_RUN               (SB_FORCE_BOOT ^ 0xFF)

/************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
uint8_t uartDebugPrintsEnabled = 0;

/*********************************************************************
 * LOCAL VARIABLES
 */

HANDLE hSerial;
COMMTIMEOUTS timeouts;
COMMCONFIG dcbSerialParams;
DWORD dwBytesWritten, dwBytesRead;
/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      rpcTransportOpen
 *
 * @brief   opens the serial port to the CC253x.
 *
 * @param   devicePath - path to the UART device
 *
 * @return  status
 */
int32_t rpcTransportOpen(char *_devicePath, uint32_t port)
{
    hSerial = CreateFile(_devicePath, GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		perror(_devicePath);
		dbg_print(PRINT_LEVEL_ERROR, "rpcOpen: %s device open failed\n",
				_devicePath);
		return (-1);
	}
    if (!GetCommState(hSerial, &dcbSerialParams.dcb))
    {
        dbg_print(PRINT_LEVEL_ERROR, "rpcOpen: get device state failed\n");
    }

    dcbSerialParams.dcb.DCBlength = sizeof(dcbSerialParams.dcb);


    dcbSerialParams.dcb.BaudRate = CBR_115200;
    dcbSerialParams.dcb.ByteSize = 8;
    dcbSerialParams.dcb.StopBits = ONESTOPBIT;
    dcbSerialParams.dcb.Parity = NOPARITY;

    dcbSerialParams.dcb.fBinary = TRUE;
    dcbSerialParams.dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcbSerialParams.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
    dcbSerialParams.dcb.fOutxCtsFlow = TRUE;
    dcbSerialParams.dcb.fOutxDsrFlow = FALSE;
    dcbSerialParams.dcb.fDsrSensitivity= FALSE;
    dcbSerialParams.dcb.fAbortOnError = TRUE;

    if (!SetCommState(hSerial, &dcbSerialParams.dcb))
    {
        dbg_print(PRINT_LEVEL_ERROR, "rpcOpen: error setting serial port state\n");
    }


    GetCommTimeouts(hSerial,&timeouts);
    //COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout = 500;
    timeouts.ReadTotalTimeoutConstant = 500;
    timeouts.ReadTotalTimeoutMultiplier = 100;
    timeouts.WriteTotalTimeoutConstant = 500;
    timeouts.WriteTotalTimeoutMultiplier= 100;

    if(!SetCommTimeouts(hSerial, &timeouts))
    {
        dbg_print(PRINT_LEVEL_ERROR, "rpcOpen: error setting serial port timeouts\n");
    }



	return hSerial;
}

/*********************************************************************
 * @fn      rpcTransportClose
 *
 * @brief   closes the serial port to the CC253x.
 *
 * @param   fd - file descriptor of the UART device
 *
 * @return  status
 */
void rpcTransportClose(void)
{
    //tcflush(serialPortFd, TCOFLUSH);
    //close(serialPortFd);
    CloseHandle(hSerial);
    return;
}

/*********************************************************************
 * @fn      rpcTransportWrite
 *
 * @brief   Write to the the serial port to the CC253x.
 *
 * @param   fd - file descriptor of the UART device
 *
 * @return  status
 */
void rpcTransportWrite(uint8_t* buf, uint8_t len)
{
    int remain = len;
    int offset = 0;
    dwBytesWritten = 0;
#if 0
    dbg_print(PRINT_LEVEL_VERBOSE, "rpcTransportWrite : len = %d\n", len);

    while (remain > 0)
    {
        int sub = (remain >= 8 ? 8 : remain);
        dbg_print(PRINT_LEVEL_VERBOSE, "writing %d bytes (offset = %d, remain = %d)\n", sub, offset, remain);
        //write(serialPortFd, buf + offset, sub);
        WriteFile(hSerial, buf + offset, sub, &dwBytesWritten, NULL);
        //tcflush(serialPortFd, TCOFLUSH);
        usleep(1000);
        remain -= 8;
        offset += 8;
    }
#else
    dbg_print(PRINT_LEVEL_VERBOSE, "rpcTransportWrite : len = %d\n", len);
    if(!WriteFile(hSerial, buf , len, &dwBytesWritten, NULL))
    {
        printf("Write Error");
    }
    dbg_print(PRINT_LEVEL_VERBOSE, "rpcTransportWrite : %d Bytes written\n", dwBytesWritten);

    //write (serialPortFd, buf, len);
    //tcflush(serialPortFd, TCOFLUSH);

#endif
    return;
}

/*********************************************************************
 * @fn      rpcTransportRead
 *
 * @brief   Reads from the the serial port to the CC253x.
 *
 * @param   fd - file descriptor of the UART device
 *
 * @return  status
 */
uint8_t rpcTransportRead(uint8_t* buf, uint8_t len)
{
    //uint8_t *buffer = malloc(len*sizeof(uint8_t));
    dwBytesRead = 0;
    do
    {
        ReadFile(hSerial, buf, len, &dwBytesRead, NULL);
        usleep(1);
    }while(dwBytesRead == 0);

    //*buf = (uint8_t)(buffer);
    if(dwBytesRead > 0)
    {
    	dbg_print(PRINT_LEVEL_VERBOSE, "rpcTransportRead: read %d bytes\n", dwBytesRead);
    }
    return (dwBytesRead);

}
