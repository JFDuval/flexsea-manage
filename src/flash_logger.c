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

#include <stdint.h>
#include "flash_driver.h"
#include "flash_logger.h"

//Head and tail pointers of the circular buffer
uint32_t log_head;
uint32_t log_tail;

//Write log record to the flash device at log_head
int flashLogWriteStructX(struct myCustomStructX *myStructX)
{
	uint8_t *p = (void*)myStructX;
	int res;

	#ifdef IN_RECORD_STATE_STORE

		// mark the record as written
		*p = (*p & ~1)|2;
		res = flashWriteArray(log_head, p, LOG_RECORD_SIZE);

	#else

		// mark the record as written
		res = flashWriteByte(log_head, ~1);
		if (res)
		{
			return res;
		}

		res = flashWriteArray(log_head+1, p, LOG_RECORD_SIZE-1);

	#endif//IN_RECORD_STATE_STORE

	if (res)
	{
		return res;
	}

	// Check if there is a room for next record in the current sector
	if (log_head + LOG_RECORD_SIZE*2-1 <= (log_head | (FLASH_SECTOR_SIZE-1)))
	{
		log_head += LOG_RECORD_SIZE;
		return 0;
	}

	// Otherwise position the head to the next sector beginning
	log_head = (log_head + FLASH_SECTOR_SIZE) & ~(FLASH_SECTOR_SIZE-1) & (FLASH_SIZE-1);

	// Check if the tail is in the same sector (it must be ahead of us) and
	// advance it to the next sector if necessary as we are going to reset the sector
	if ((log_head & ~(FLASH_SECTOR_SIZE-1)) == (log_tail & ~(FLASH_SECTOR_SIZE-1)))
	{
		log_tail = (log_tail + FLASH_SECTOR_SIZE) & ~(FLASH_SECTOR_SIZE-1) & (FLASH_SIZE-1);
	}

	// Finally check if the sector has to be erased (if the first record
	// has never been written, then the rest of records in a sector are
	// clean as well
	res = flashReadByte(log_head);
	if (res < 0)
	{
		return res;
	}

	return (res & 3) == 3 ? 0 : flashReadByte(log_head);
}

//Read log record from the flash device at tail
int flashLogReadStructX(struct myCustomStructX *myStructX)
{
	int res;

	// check if there is something in the log
	if (log_head == log_tail)
	{
		return ERR_NO_LOG;
	}

	#ifdef IN_RECORD_STATE_STORE

		// Read in the log record
		res = flashReadArray(log_tail, (void*)myStructX, LOG_RECORD_SIZE);

	#else

		// Read in the log record
		res = flashReadArray(log_tail+1, (void*)myStructX, LOG_RECORD_SIZE-1);

	#endif//IN_RECORD_STATE_STORE

	if (res)
	{
		return res;
	}

	// Check if this is the last record in a sector and erase a sector;
	// As we do wait for chip not busy condition before we do something,
	// the erase latency would be hidden here; the head could not be
	// in the same page in this case (see flashLogWriteStructX)
	if ((FLASH_SECTOR_SIZE % LOG_RECORD_SIZE) ?
		((log_tail & ~(FLASH_SECTOR_SIZE-1)) != ((log_tail+LOG_RECORD_SIZE*2) & ~(FLASH_SECTOR_SIZE-1))) :
		(((log_tail+LOG_RECORD_SIZE) & (LOG_RECORD_SIZE-1)) == 0))
	{

		// Erase sector
		res = flashEraseSector(log_tail & ~(FLASH_SECTOR_SIZE-1));
		// Advance tail
		log_tail = (log_tail + FLASH_SECTOR_SIZE) & ~(FLASH_SECTOR_SIZE-1) & (FLASH_SIZE-1);
	}
	else
	{
		// Mark a record as read otherwise
		res = flashWriteByte(log_tail,0);
		// Advance tail
		log_tail += LOG_RECORD_SIZE;
	}

	return res;
}

//flashLogInit init function. Has to be called at system startup to recover head and tail pointers.
int flashLogInit(void)
{
	uint32_t current = 0;

	log_head = log_tail = ~0;

	// First, locate the head which is always a transition from written or read record to a clean one
	int b, e, prev = flashReadByte((FLASH_SIZE / FLASH_SECTOR_SIZE - 1) * FLASH_SECTOR_SIZE +
		(FLASH_SECTOR_SIZE / LOG_RECORD_SIZE - 1) * LOG_RECORD_SIZE);

	if (prev < 0)
	{
		return prev;
	}

	prev &= 3;

	while (current < FLASH_SIZE)
	{
		// Read the status of the first record in a sector
		b = flashReadByte(current);
		if (b < 0)
			return b;

		// check if a sector is clean and if there is a transition on its boundary
		if ((b &= 3) == 3)
		{
			if (b != prev)
			{
				log_head = current;
				break;
			}
			current += FLASH_SECTOR_SIZE;
			continue;
		}

		// Read the status of the last record in a sector to check if we need to descend into
		e = flashReadByte(current + (FLASH_SECTOR_SIZE/LOG_RECORD_SIZE-1)*LOG_RECORD_SIZE);
		if (e < 0)
		{
			return e;
		}

		// The last record is not clean as well, so skip the block;
		if ((e &= 3) != 3)
		{
			prev = e;
			current += FLASH_SECTOR_SIZE;
			continue;
		}

		// There is a transition between b & e, so traverse the records
		do
		{
			current += LOG_RECORD_SIZE;
			b = flashReadByte(current);
			if (b < 0)
			{
				return b;
			}
		}
		while ((b & 3) != 3);

		// as we get here, the current points to the first clean record
		log_head = current;
		break;
	}

	// we did not find any records, just start from scratch
	if (log_head == 0)
	{
		log_head = log_tail = 0;
		return 0;
	}

	// Now locate the tail record. The tail record is a transition from read to written or clean;

	// First seek for the first unclean sector starting from the next after the current
	do
	{
		current = (current + FLASH_SECTOR_SIZE) & ~(FLASH_SECTOR_SIZE-1) & (FLASH_SIZE-1);
		// Read the status of the first record in a sector
		b = flashReadByte(current);
		if (b < 0)
		{
			return b;
		}
	}
	while ((b & 3) == 3);

	// Next locate the transition from read to anything else
	while ((b & 3) == 0)
	{
		current = (current & ~(FLASH_SECTOR_SIZE-1)) == ((current + LOG_RECORD_SIZE) & ~(FLASH_SECTOR_SIZE-1)) ?
			(current + LOG_RECORD_SIZE) : (current + FLASH_SECTOR_SIZE) & ~(FLASH_SECTOR_SIZE-1) & (FLASH_SIZE-1);
		b = flashReadByte(current);
		if (b < 0)
		{
			return b;
		}
	}
	log_tail = current;
	return 0;
}

#define TEST_VAL_LEN 	5
int testFlashLog(void)
{
	int i = 0;
	uint16_t testVal[TEST_VAL_LEN] = {1111, 2222, 3333, 4444, 5555};
	struct myCustomStructX myData, myDataRet;
	flashEraseAll();
	flashLogInit();

	//Save 5 data points in memory:
	for(i = 0; i < TEST_VAL_LEN; i++)
	{
		myData.a = testVal[i];
		flashLogWriteStructX(&myData);
	}

	//Read values back, and make sure they are right:
	for(i = 0; i < TEST_VAL_LEN; i++)
	{
		flashLogReadStructX(&myDataRet);
		if(myDataRet.a != testVal[i])
		  {
			return -1; //Error
		  }
	}
	if (flashLogReadStructX(&myDataRet) != ERR_NO_LOG)
	{
		return -1;
	}
	//Save 5 data points in memory:
	for(i = 0; i < TEST_VAL_LEN; i++)
	{
		myData.a = testVal[i];
		flashLogWriteStructX(&myData);
	}

	// Reinit the log
	flashLogInit();
	//Read values back, and make sure they are right:
	for(i = 0; i < TEST_VAL_LEN; i++)
	{
		flashLogReadStructX(&myDataRet);
		if(myDataRet.a != testVal[i])
		  {
			return -1; //Error
		  }
	}
	if (flashLogReadStructX(&myDataRet) != ERR_NO_LOG)
	{
		return -1;
	}

	return 0; //All good
}
