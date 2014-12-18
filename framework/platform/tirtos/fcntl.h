/*
 * fcntl.h
 *
 * This module contains the implementation of POSIX fcntl (manipulate
 * file descriptor) on TI-RTOS.
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

#ifndef FCNTL_H
#define FCNTL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * MACROS
 */

/* Specifiy one of these flags to define the access mode. */
#define	_O_RDONLY	0
#define _O_WRONLY	1
#define _O_RDWR		2

/* Mask for access mode bits in the _open flags. */
#define _O_ACCMODE	(_O_RDONLY|_O_WRONLY|_O_RDWR)

#define	_O_APPEND	0x0008	/* Writes will add to the end of the file. */

#define	_O_RANDOM	0x0010
#define	_O_SEQUENTIAL	0x0020
#define	_O_TEMPORARY	0x0040	/* Make the file dissappear after closing.
				 * WARNING: Even if not created by _open! */
#define	_O_NOINHERIT	0x0080

#define	_O_CREAT	0x0100	/* Create the file if it does not exist. */
#define	_O_TRUNC	0x0200	/* Truncate the file if it does exist. */
#define	_O_EXCL		0x0400	/* Open only if the file does not exist. */

#define _O_SHORT_LIVED  0x1000

/* NOTE: Text is the default even if the given _O_TEXT bit is not on. */
#define	_O_TEXT		0x4000	/* CR-LF in file becomes LF in memory. */
#define	_O_BINARY	0x8000	/* Input and output is not translated. */
#define	_O_RAW		_O_BINARY

#ifndef	_NO_OLDNAMES

/* POSIX/Non-ANSI names for increased portability */
#define	O_RDONLY	_O_RDONLY
#define O_WRONLY	_O_WRONLY
#define O_RDWR		_O_RDWR
#define O_ACCMODE	_O_ACCMODE
#define	O_APPEND	_O_APPEND
#define	O_CREAT		_O_CREAT
#define	O_TRUNC		_O_TRUNC
#define	O_EXCL		_O_EXCL
#define	O_TEXT		_O_TEXT
#define	O_BINARY	_O_BINARY
#define	O_TEMPORARY	_O_TEMPORARY
#define O_NOINHERIT	_O_NOINHERIT
#define O_SEQUENTIAL	_O_SEQUENTIAL
#define	O_RANDOM	_O_RANDOM
#define O_RAW		_O_RAW

#endif	/* Not _NO_OLDNAMES */

/*********************************************************************
 * CONSTANTS
 */

/************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * API FUNCTIONS
 */

#ifdef __cplusplus
}
#endif

#endif /* FCNTL_H */
