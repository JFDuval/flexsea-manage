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
	[This file] fm_slave_comm: Slave R/W
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************
#include "flexsea_buffers.h"
#include "main.h"
#include "fm_master_slave_comm.h"
//#include <fm_block_allocator.h>
#include <flexsea_payload.h>
#include <flexsea_board.h>
#include <stdbool.h>

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t tmp_rx_command_spi[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_usb[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_wireless[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_1[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_2[PAYLOAD_BUF_LEN];

//MsgQueue slave_queue;
PacketWrapper PWpsc[2];
PacketWrapper PWst[2];

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

void initSlaveComm(void)
{
	//...
	//fm_queue_init(&slave_queue, 10);
}

//Prepares the structures:
void init_master_slave_comm(void)
{
	//Default state:
	initCommPeriph(&masterCommPeriph[0], PORT_USB, MASTER, rx_buf_4, \
			comm_str_4, rx_command_4, &masterInbound[0], &masterOutbound[0]);
	initCommPeriph(&slaveCommPeriph[0], PORT_RS485_1, SLAVE, rx_buf_1, \
			comm_str_1, rx_command_1, &slaveInbound[0], &slaveOutbound[0]);
	initCommPeriph(&slaveCommPeriph[1], PORT_RS485_2, SLAVE, rx_buf_2, \
			comm_str_2, rx_command_2, &slaveInbound[1], &slaveOutbound[1]);

	//Personalize specific fields:
	//...
}

//Initialize CommPeriph to defaults:
void initCommPeriph(CommPeriph *cp, Port port, PortType pt, uint8_t *input, \
					uint8_t *unpacked, uint8_t *packed, \
					PacketWrapper *inbound, PacketWrapper *outbound)
{
	cp->port = port;
	cp->portType = pt;
	cp->transState = TS_UNKNOWN;

	cp->rx.bytesReadyFlag = 0;
	cp->rx.unpackedPacketsAvailable = 0;
	cp->rx.inputBufferPtr = input;
	cp->rx.unpackedPtr = unpacked;
	cp->rx.packedPtr = packed;
	memset(cp->rx.packed, 0, COMM_PERIPH_ARR_LEN);
	memset(cp->rx.unpacked, 0, COMM_PERIPH_ARR_LEN);

	cp->tx.bytesReadyFlag = 0;
	cp->tx.unpackedPacketsAvailable = 0;
	cp->tx.unpackedPtr = cp->tx.unpacked;
	cp->tx.packedPtr = cp->tx.packed;
	memset(cp->tx.packed, 0, COMM_PERIPH_ARR_LEN);
	memset(cp->tx.unpacked, 0, COMM_PERIPH_ARR_LEN);

	linkCommPeriphPacketWrappers(cp, inbound, outbound);
}

void linkCommPeriphPacketWrappers(CommPeriph *cp, PacketWrapper *inbound, \
					PacketWrapper *outbound)
{
	//Force family:
	cp->in = inbound;
	cp->out = outbound;
	inbound->parent = (CommPeriph*)cp;
	outbound->parent = (CommPeriph*)cp;

	//Children inherit from parent:
	if(cp->portType == MASTER)
	{
		inbound->travelDir = DOWNSTREAM;
		inbound->sourcePort = cp->port;
		outbound->travelDir = UPSTREAM;
		outbound->destinationPort = cp->port;
	}
	else
	{
		inbound->travelDir = UPSTREAM;
		inbound->destinationPort = cp->port;
		outbound->travelDir = DOWNSTREAM;
		outbound->sourcePort = cp->port;
	}
}

//Did we receive new commands? Can we parse them?
void parseMasterCommands(uint8_t *new_cmd)
{
	//ToDo *****IMPORTANT***** it's missing SPI & Bluetooth!

	//USB
	if(masterCommPeriph[0].rx.unpackedPacketsAvailable > 0)
	{
		masterCommPeriph[0].rx.unpackedPacketsAvailable = 0;
		payload_parse_str(&masterInbound[0]);
		*new_cmd = 1;
	}
}

//Did we receive new commands? Can we parse them?
void parseSlaveCommands(uint8_t *new_cmd)
{
	//Valid communication from RS-485 #1?
	if(slaveCommPeriph[0].rx.unpackedPacketsAvailable > 0)
	{
		slaveCommPeriph[0].rx.unpackedPacketsAvailable = 0;
		payload_parse_str(&slaveCommPeriph[0]);
	}

	/*
	//Valid communication from RS-485 #1?
	if(slaveComm[0].rx.cmdReady > 0)
	{
		slaveComm[0].rx.cmdReady = 0;

		PWpsc[0].port = slaveComm[0].port;
		PWpsc[0].reply_port = slaveComm[0].reply_port;
		memcpy(PWpsc[0].unpaked, &rx_command_485_1[0], COMM_STR_BUF_LEN);
		memcpy(PWpsc[0].packed, rx_buf_1, COMM_STR_BUF_LEN);

		payload_parse_str(&PWpsc[0]);
	}
	*/

	//Valid communication from RS-485 #2?
	if(slaveComm[1].rx.cmdReady > 0)
	{
		slaveComm[1].rx.cmdReady = 0;

		PWpsc[1].port = slaveComm[1].port;
		PWpsc[1].reply_port = slaveComm[1].reply_port;
		memcpy(PWpsc[1].unpaked, &rx_command_485_2[0], COMM_STR_BUF_LEN);
		memcpy(PWpsc[1].packed, rx_buf_2, COMM_STR_BUF_LEN);

		payload_parse_str(&PWpsc[1]);
	}
}

//Slave Communication function. Call at 1kHz.
void slaveTransmit(uint8_t port)
{
	/*Note: this is only a demonstration. In the final application, we want
			 * to send the commands accumulated on a ring buffer here.*/
	uint8_t slaveIndex = 0;
	PacketWrapper *p;

	if(port == PORT_RS485_1)
	{
		slaveIndex = 0;
	}
	else if(port == PORT_RS485_2)
	{
		slaveIndex = 1;
	}
	p = &slaveOutbound[slaveIndex];

	if(slaveCommPeriph[slaveIndex].tx.packetReady == 1)
	{
		slaveCommPeriph[slaveIndex].tx.packetReady = 0;

		if(IS_CMD_RW(p->cmd) == READ)
		{
			slaveCommPeriph[slaveIndex].transState = TS_TRANSMIT_THEN_RECEIVE;
		}
		else
		{
			slaveCommPeriph[slaveIndex].transState = TS_TRANSMIT;
		}

		flexsea_send_serial_slave(p);
	}
}

//****************************************************************************
// Private Function(s)
//****************************************************************************
