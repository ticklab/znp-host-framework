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
/** ============================================================================
 *  @file       UARTConsole.h
 *
 *  @brief      UART Console related utilities
 *
 *
 *  ============================================================================
 */

#ifndef __UARTConsole_H
#define __UARTConsole_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <xdc/std.h>
#include <stdio.h>

/* TI-RTOS Header files */
#include <ti/drivers/UART.h>

/*!
 *  @brief  Opens the UART console
 *
 *  @param  binary Open the UART in binary mode
 *  @return        UART_Handle to the opened UART
 */
extern UART_Handle UARTConsole_open(bool binary);

/*!
 *  @brief  Closes the UART console
 *
 *  @param  None
 *  @return None
 */
extern void UARTConsole_close();

/*!
 *  @brief  reads buffer from the UART Console
 *
 *  @param  	buffer - buffer to read in to
 *  @param  	size - number of bytes to read
 *  @return     bytes read
 */
extern int UARTConsole_read(char *buffer, unsigned size);

/*!
 *  @brief  writes a buffer to the UART Console
 *
 *  @param  	buffer - buffer to write
 *  @param  	size - number of bytes to write
 *  @return     bytes written
 */
extern int UARTConsole_write(const char *buffer, unsigned size);

/*!
 *  @brief  writes a char to the UART Console
 *
 *  @param  	a - char to write
 *  @return     None
 */
extern Void UARTConsole_putch(Char a);

/*!
 *  @brief  gets a char from the UART Console
 *
 *  @param  	a - char to read into
 *  @return     None
 */
extern Void UARTConsole_getch(Char *a);

#ifdef __cplusplus
}
#endif

#endif /* __UARTConsole_H */
