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
	[This file] flash_driver: Low-level driver, FLASH memory
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-12-16 | jfduval | Initial
	*
****************************************************************************/

#include "main.h"
#include "fm_spi.h"
#include "flash_driver.h"
#include "fm_stm32f4xx_hal_spi.h"

static __inline__ int flashWaitReady(void)
{
	uint8_t out[1] = { FLASH_RDSR };
	uint8_t in[1];
	int res;

	while (!(res = spiTransmitReceive(out, sizeof(out), in, sizeof(in))) && (in[0] & FLASH_BUSY))
	{
		// place here any code to be executed during chip RDY state wait
	}
	return res;
}

static __inline__ int flashWriteEnable(void)
{
	uint8_t out[1] = { FLASH_WREN };
	uint8_t in[0];

	int res = flashWaitReady();

	return res ? res : spiTransmitReceive(out, sizeof(out), in, sizeof(in));
}

// Full chip erase
int flashEraseAll(void)
{
	uint8_t out[1] = { FLASH_BE };
	uint8_t in[0];

	int res = flashWriteEnable();

	return res ? res : spiTransmitReceive(out, sizeof(out), in, sizeof(in));
}

// Erase a sector containing an address
int flashEraseSector(uint32_t address)
{
	uint8_t out[4];
	uint8_t in[0];

	address &= ~(FLASH_SECTOR_SIZE - 1);
	out[0] = FLASH_SE;
	out[1] = address >> 16;
	out[2] = address >> 8;
	out[3] = address >> 0;

	int res = flashWriteEnable();

	return res ? res : spiTransmitReceive(out, sizeof(out), in, sizeof(in));
}

// Write byte at address
int flashWriteByte(uint32_t address, uint8_t byte)
{
	uint8_t out[5];
	uint8_t in[0];

	out[0] = FLASH_PP;
	out[1] = address >> 16;
	out[2] = address >> 8;
	out[3] = address >> 0;
	out[4] = byte;

	int res = flashWriteEnable();

	return res ? res : spiTransmitReceive(out, sizeof(out), in, sizeof(in));
}

// Read byte at address
// the value returned is byte read if non-negative, an error code otherwise
int flashReadByte(uint32_t address)
{
	uint8_t out[4];
	uint8_t in[1];

	out[0] = FLASH_READ;
	out[1] = address >> 16;
	out[2] = address >> 8;
	out[3] = address >> 0;

	int res = flashWaitReady();

	if (!res)
	{
		res = spiTransmitReceive(out, sizeof(out), in, sizeof(in));
	}

	return res ? res : in[0];
}

// Write length bytes from newData at address
int flashWriteArray(uint32_t address, uint8_t *newData, uint32_t length)
{
	uint8_t out[4+256];
	uint8_t in[0];

	while (length)
	{
		out[0] = FLASH_PP;
		out[1] = address >> 16;
		out[2] = address >> 8;
		out[3] = address >> 0;

		// number of bytes available in the current page
		uint16_t cb = 256 - (address & (FLASH_PAGE_SIZE - 1));

		if (cb > length)
		{
			cb = length;
		}

		memcpy(out+4, newData, cb);

		int res = flashWriteEnable();
		if (res)
		{
			return res;
		}

		res = spiTransmitReceive(out, cb + 4, in, sizeof(in));
		if (res)
		{
			return res;
		}

		// Advance pointers, decrease length by number of bytes written
		address += cb;
		newData += cb;
		length -= cb;
	}
	return 0;
}

// Read length bytes into readData from address
int flashReadArray(uint32_t address, uint8_t *readData, uint32_t length)
{
	uint8_t out[4];

	out[0] = FLASH_READ;
	out[1] = address >> 16;
	out[2] = address >> 8;
	out[3] = address >> 0;

	int res = flashWaitReady();

	return res ? res : spiTransmitReceive(out, sizeof(out), readData, length);
}

#define TEST_ARRAY_LEN 		10
int testFlashDriver(void)
{
	uint8_t testByte = 0x1E, testByteRet = 0;
	uint8_t testArray[TEST_ARRAY_LEN] = {10, 100, 20, 200, 5, 15, 1, 11, 0, 255};
	uint8_t testArrayRet[TEST_ARRAY_LEN] = {0,0,0,0,0,0,0,0,0};

	//Single byte R/W test:
	flashEraseAll();
	flashWriteByte(0, testByte);
	testByteRet = flashReadByte(0);
	if(testByteRet != testByte)
	{
		return -1; //Error
	}

	//Array R/W test:
	flashEraseAll();
	flashWriteArray(1000, testArray, TEST_ARRAY_LEN);
	flashReadArray(1000, testArrayRet, TEST_ARRAY_LEN);
	if(memcmp(testArray, testArrayRet, TEST_ARRAY_LEN) != 0)
	{
		return -1; //Error
	}
	return 0; //All good
}

//Temporary: wrapper for SPI to avoid modifying all of Sergey's code
/**
 * The SPI bus interface function
 * It transmits txSize bytes at pTxData and then receives rxSize bytes into pRxData
 * in a single transaction, i.e. /CS goes low, then command & data bytes sent to the chip,
 * then response received, then /CS goes high.
 *
 * The return value is expected to be zero or negative number.
 * The zero return value means successful execution of the request.
 * Any negative value is treated as an error and passed back to the caller
 */
int spiTransmitReceive(uint8_t *pTxData, uint16_t txSize, uint8_t *pRxData, uint16_t rxSize)
{
	HAL_StatusTypeDef retVal;

	//Note: only using 1 size, TX's
	retVal = HAL_SPI_TransmitReceive(&spi5_handle, pTxData, pRxData, txSize, 1000);

	return (retVal == HAL_OK ? 0 : -1);
}
