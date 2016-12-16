/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'flexsea-manage' Mid-level computing, and networking
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Contractor
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors] Jean-Francois (JF) Duval, jfduval at dephy dot com.
*****************************************************************************
	[This file] flash_logger: High-level driver, FLASH memory
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-16 | jfduval | Initial
	*
****************************************************************************/

#ifndef FLASHLOGGER_H
#define FLASHLOGGER_H

/**
 * To avoid wearing out some dedicated location to store head & tail
 * pointers in flash memory each record requires two extra bits:
 * one bit to mark a record as stored and one bit to mark a stored record
 * as already read in (consumed)
 * This achieved by a) prepending a record with extra byte;
 * b) reserving two lowest bits in the first byte of the record by the
 * upper layer.
 * To enable in-record store of the state bits uncomment the following define
 */
//#define IN_RECORD_STATE_STORE

#ifdef IN_RECORD_STATE_STORE
#define LOG_RECORD_SIZE (sizeof(struct myCustomStructX))
#else
#define LOG_RECORD_SIZE (sizeof(struct myCustomStructX) + 1)
#endif//IN_RECORD_STATE_STORE

/// The error code returned by flashLogReadStructX when there is no records
#define ERR_NO_LOG 1
/// The error code returned when log corruption is detected
#define ERR_LOG_CORRUPTED -1

struct myInnerStruct1
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct myCustomStructX
{
	struct myInnerStruct1 innerSA;
	struct myInnerStruct1 innerSB;
	uint16_t a;
	uint16_t b[8];
	int16_t c;
	int32_t d;
	uint8_t e;
	uint8_t f;
};

/// Init Flash Log driver
extern int flashLogInit(void);

/// Write head log record to the flash device
extern int flashLogWriteStructX(struct myCustomStructX *myStructX);

/// Read tail log record into the RAM
extern int flashLogReadStructX(struct myCustomStructX *myStructX);

int testFlashLog(void);

#endif//FLASHLOGGER_H
