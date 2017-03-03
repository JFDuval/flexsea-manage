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
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] flexsea_board: configuration and functions for this
	particular board
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include <fm_uarts.h>
#include "flexsea_board.h"
#include "../../flexsea-system/inc/flexsea_system.h"
//#include <fm_block_allocator.h>
#include <flexsea_comm.h>
#include <stdbool.h>

//****************************************************************************
// Variable(s)
//****************************************************************************

//<FlexSEA User>
//==============
//Board architecture. Has to be changed in all the flexsea_board files!

uint8_t board_id = FLEXSEA_MANAGE_1;		//This board
uint8_t board_up_id = FLEXSEA_PLAN_1;		//This board's master

//Slave bus #1 (RS-485 #1):
//=========================
uint8_t board_sub1_id[SLAVE_BUS_1_CNT] = {FLEXSEA_EXECUTE_1, FLEXSEA_EXECUTE_3};

//Slave bus #2 (RS-485 #2):
//=========================
uint8_t board_sub2_id[SLAVE_BUS_2_CNT] = {FLEXSEA_EXECUTE_2, FLEXSEA_EXECUTE_4};

//(make sure to update SLAVE_BUS_x_CNT in flexsea_board.h!)

//===============
//</FlexSEA User>

//PacketWrappers associated with our ComPeriph structures:
PacketWrapper masterInbound[3], masterOutbound[3];
PacketWrapper slaveInbound[2], slaveOutbound[2];

//****************************************************************************
// Function(s)
//****************************************************************************
//Wrapper for the specific serial functions. Useful to keep flexsea_network
//platform independent (for example, we don't need need puts_rs485() for Plan)
void flexsea_send_serial_slave(PacketWrapper* p)
{
	uint8_t port = p->destinationPort;
	uint8_t* str = p->packed;
	uint16_t length = COMM_STR_BUF_LEN;

	if(port == PORT_RS485_1)
	{
		puts_rs485_1(str, length);
		slaveCommPeriph[0].transState = TS_TRANSMIT_THEN_RECEIVE;
		//ToDo we do not always want to RX
		slaveComm[0].reply_port = p->reply_port;
	}
	else if(port == PORT_RS485_2)
	{
		puts_rs485_2(str, length);
		slaveCommPeriph[1].transState = TS_TRANSMIT_THEN_RECEIVE;
		//ToDo we do not always want to RX
		slaveComm[1].reply_port = p->reply_port;
	}
	else
	{
		//Unknown port, call flexsea_error()
		flexsea_error(SE_INVALID_SLAVE);
	}
}

void flexsea_send_serial_master(PacketWrapper* p)
{
	Port port = p->destinationPort;
	uint8_t *str = p->packed;
	uint16_t length = COMM_STR_BUF_LEN;
	int i = 0;

	if(port == PORT_SPI)
	{
		for(i = 0; i < length; i++)
		{
			comm_str_spi[i] = str[i];
		}
		//This will be sent during the next SPI transaction
	}
	else if(port == PORT_USB)
	{
		CDC_Transmit_FS(str, length);
	}
	else if(port == PORT_WIRELESS)
	{
		puts_expUart(str, length);
	}
}

void flexsea_receive_from_master(void)
{
	//USB:
	tryUnpacking(&masterCommPeriph[MCP_USB],  &masterInbound[MCP_USB]);

	//Incomplete, ToDo (flag won't be raised)
	tryUnpacking(&masterCommPeriph[MCP_SPI],  &masterInbound[MCP_SPI]);
	tryUnpacking(&masterCommPeriph[MCP_WIRELESS],  &masterInbound[MCP_WIRELESS]);
}

void flexsea_start_receiving_from_master(void)
{
	// start receive over SPI
	if (HAL_SPI_GetState(&spi4_handle) == HAL_SPI_STATE_READY)
	{
		if(HAL_SPI_TransmitReceive_IT(&spi4_handle, (uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, COMM_STR_BUF_LEN) != HAL_OK)
		{
			// Transfer error in transmission process
			flexsea_error(SE_RECEIVE_FROM_MASTER);
		}
	}
}

//Receive data from a slave
void flexsea_receive_from_slave(void)
{
	//Transceiver state:
	//==================

	//We only listen if we requested a reply:
	if(slaveCommPeriph[SCP_RS485_1].transState == TS_PREP_TO_RECEIVE)
	{
		slaveCommPeriph[SCP_RS485_1].transState = TS_RECEIVE;

		reception_rs485_1_blocking();	//Sets the transceiver to Receive
		//From this point on data will be received via the interrupt.
	}

	//We only listen if we requested a reply:
	if(slaveCommPeriph[SCP_RS485_2].transState == TS_PREP_TO_RECEIVE)
	{
		slaveCommPeriph[SCP_RS485_2].transState = TS_RECEIVE;

		reception_rs485_2_blocking();	//Sets the transceiver to Receive
		//From this point on data will be received via the interrupt.
	}

	//Did we get new bytes?
	//=====================

	tryUnpacking(&slaveCommPeriph[SCP_RS485_1], &slaveInbound[SCP_RS485_1]);
	tryUnpacking(&slaveCommPeriph[SCP_RS485_2], &slaveInbound[SCP_RS485_2]);
}

//Copies the generated comm_str to the aTxBuffer
//It will be transfered the next time the master writes to us.
//ToDo generalize, use buffers as arguments
void comm_str_to_txbuffer(void)
{
	uint8_t i = 0;

	for(i = 0; i < COMM_STR_BUF_LEN; i++)
	{
		aTxBuffer[i] = comm_str_spi[i];
	}
}

//Do you have bytes ready? Can they be unpacked? Let's give it a shot.
uint8_t tryUnpacking(CommPeriph *cp, PacketWrapper *pw)
{
	uint8_t retVal = 0;

	if(cp->rx.bytesReadyFlag > 0)
	{
		//Try unpacking. This is the only way to know if we have a packet and
		//not just random bytes, or an incomplete packet.
		cp->rx.unpackedPacketsAvailable = unpack_payload( \
				cp->rx.inputBufferPtr, \
				cp->rx.packedPtr, \
				cp->rx.unpackedPtr);

		if(cp->rx.unpackedPacketsAvailable > 0)
		{
			//Transition from CommInterface to PacketWrapper:
			fillPacketFromCommPeriph(cp, pw);
			retVal = 1;
		}

		//Drop flag
		cp->rx.bytesReadyFlag = 0;
	}

	return retVal;
}
