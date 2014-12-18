/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== UARTConsole.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* TI-RTOS Header files */
#include <ti/drivers/UART.h>

#include "Board.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static UART_Handle handle; /* Handle for UART */

/*!
 *  @brief  Opens the UART console
 *
 *  @param  binary Open the UART in binary mode
 *  @return        UART_Handle to the opened UART
 */
UART_Handle UARTConsole_open(bool binary)
{
	UART_Params uartParams;

	/* Create a UART with the parameters below. */
	UART_Params_init(&uartParams);
	if (binary == true)
	{
		uartParams.readEcho = UART_ECHO_OFF;
		uartParams.writeDataMode = UART_DATA_BINARY;
	}
	else
	{
		uartParams.baudRate = 115200;
	}

	handle = UART_open(Board_UART0, &uartParams);

	return (handle);
}

/*!
 *  @brief  Closes the UART console
 *
 *  @param  None
 *  @return None
 */
void UARTConsole_close()
{
	UART_close(handle);
}

/*!
 *  @brief  reads buffer from the UART Console
 *
 *  @param  	buffer - buffer to read in to
 *  @param  	size - number of bytes to read
 *  @return     bytes read
 */
int UARTConsole_read(char *buffer, unsigned size)
{
	int ret;

	/* Return if a UART not open. */
	if (handle == NULL)
	{
		return (-1);
	}

	/* Read character from the UART and block until a newline is received. */
	ret = UART_read(handle, (uint8_t *) buffer, size - 1);

	return (ret);
}

/*!
 *  @brief  writes a buffer to the UART Console
 *
 *  @param  	buffer - buffer to write
 *  @param  	size - number of bytes to write
 *  @return     bytes written
 */
int UARTConsole_write(const char *buffer, unsigned size)
{
	int ret;

	/* Return if a UART not open. */
	if (handle == NULL)
	{
		return (-1);
	}

	/* Write to the UART and block until the transfer is finished. */
	ret = UART_write(handle, (uint8_t *) buffer, size);

	return (ret);
}

/*!
 *  @brief  writes a char to the UART Console
 *
 *  @param  	a - char to write
 *  @return     None
 */
Void UARTConsole_putch(Char a)
{
	/* Make sure UART is initialized */
	if (handle)
	{
		UART_writePolling(handle, (void *) &a, 1);
	}
}

/*!
 *  @brief  gets a char from the UART Console
 *
 *  @param  	a - char to read into
 *  @return     None
 */
Void UARTConsole_getch(Char *a)
{
	/* Make sure UART is initialized */
	if (handle)
	{
		UART_read(handle, a, 1);
	}
}
