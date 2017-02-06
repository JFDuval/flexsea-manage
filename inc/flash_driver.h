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

#ifndef FLASHDRIVER_H
#define FLASHDRIVER_H

//The SECTOR_SIZE implicitly defines the chip in use, its capacity
//and the erase sector command to use.

#define FLASH_SECTOR_SIZE (2*(1<<20)/8)		// M25P128
//#define FLASH_SECTOR_SIZE (4*(1<<10))		// W25Q64 4K sector
//#define FLASH_SECTOR_SIZE (32*(1<<10))	// W25Q64 32K sector
//#define FLASH_SECTOR_SIZE (64*(1<<10))  	// W25Q64 64K sector

#define FLASH_BUSY 			0x01	// Busy bit in Status Register
#define FLASH_WEL  			0x02	// Write Enable Latch bit in Status Register
#define FLASH_WREN 			0x06	// Write Enable
#define FLASH_WRDI 			0x04	// Write Disable
#define FLASH_RDID 			0x9F	// Read Identification
#define FLASH_RDSR 			0x05	// Read Status Register (Read S0 SR for W25Q64FV)
#define FLASH_WRSR 			0x01	// Write Status Register
#define FLASH_READ 			0x03	// Read Data Bytes
#define FLASH_FAST_READ 	0x0B	// Read Data Bytes at Higher Speed
#define FLASH_PP			0x02	// Page Program
#define FLASH_BE			0xC7	// Bulk Erase
#define FLASH_PAGE_SIZE 	256		// Page size

#if FLASH_SECTOR_SIZE == (2*(1<<20)/8) // M25P128 only supported sector size
	#define FLASH_SIZE (128*(1<<20)/8) // M25P128 is 128mbit
#else
	#define FLASH_SIZE (64*(1<<20)/8) // W25Q64 is 64mbit
#endif

#if FLASH_SECTOR_SIZE == (2*(1<<20)/8) // M25P128 only supported sector size
	#define FLASH_SE	0xD8		// Sector Erase (M25P128)
#elif FLASH_SECTOR_SIZE == (4*(1<<10))
	#define FLASH_SE	0x20		// 4Kb Sector Erase (W25Q64)
#elif FLASH_SECTOR_SIZE == (32*(1<<10))
	#define FLASH_SE	0x52		// 32Kb Sector Erase (W25Q64)
#elif FLASH_SECTOR_SIZE == (64*(1<<10))
	#define FLASH_SE	0xD8		// 64Kb Sector Erase (W25Q64)
#else
	#error No sector size defined
#endif

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
int spiTransmitReceive(uint8_t *pTxData, uint16_t txSize, uint8_t *pRxData, uint16_t rxSize);

// Full chip erase
int flashEraseAll(void);

// Erase a sector containgng an address
int flashEraseSector(uint32_t address);

// Write byte at address
int flashWriteByte(uint32_t address, uint8_t byte);

// Read byte at address
// the value returned is byte read if non-negative, an error code otherwise
int flashReadByte(uint32_t address);

// Write length bytes from newData at address
int flashWriteArray(uint32_t address, uint8_t *newData, uint32_t length);

// Read length bytes into readData from address
int flashReadArray(uint32_t address, uint8_t *readData, uint32_t length);

int testFlashDriver(void);

#endif	//FLASHDRIVER_H
