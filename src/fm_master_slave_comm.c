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
#include <fm_block_allocator.h>
#include <flexsea_payload.h>
#include <stdbool.h>
//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t tmp_rx_command_spi[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_usb[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_wireless[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_1[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_2[PAYLOAD_BUF_LEN];

MsgQueue slave_queue;

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

void initSlaveComm(void)
{
	//...
	fm_queue_init(&slave_queue, 10);
}

//Prepares the structures:
void init_master_slave_comm(void)
{
	//Slave Port #1:
	slaveComm[0].port = PORT_RS485_1;
	slaveComm[0].rx.bytesReady = 0;
	slaveComm[0].rx.cmdReady = 0;
	slaveComm[0].rx.commStr = comm_str_485_1;
	//slaveComm[0].rx.rxBuf = rx_buf_1;
	slaveComm[0].rx.rxCmd = rx_command_485_1;

	//Slave Port #2:
	slaveComm[1].port = PORT_RS485_2;
	slaveComm[1].rx.bytesReady = 0;
	slaveComm[1].rx.cmdReady = 0;
	slaveComm[1].rx.commStr = comm_str_485_1;
	//slaveComm[0].rx.rxBuf = rx_buf_1;
	slaveComm[1].rx.rxCmd = rx_command_485_1;

	//Master Port #3:
	masterComm[2].port = PORT_WIRELESS;
	masterComm[2].rx.bytesReady = 0;
	masterComm[2].rx.cmdReady = 0;
	masterComm[2].rx.commStr = comm_str_wireless;
	//slaveComm[2].rx.rxBuf = rx_buf_1;
	masterComm[2].rx.rxCmd = rx_command_wireless;

	//ToDo: use this approach for USB & SPI
}

//Did we receive new commands? Can we parse them?
void parseMasterCommands(uint8_t *new_cmd)
{
	uint8_t info[2] = {0,0};

	//Valid communication from any port?
	PacketWrapper* p = fm_queue_get(&unpacked_packet_queue);
	if(p != NULL)
	{
		info[0] = p->port;
		payload_parse_str(p);

		//LED:
		*new_cmd = 1;
	}

}

//Did we receive new commands? Can we parse them?
void parseSlaveCommands(uint8_t *new_cmd)
{
	volatile uint32_t i = 0;
	uint8_t info[2] = {0,0};

	//Valid communication from RS-485 #1?
	if(slaveComm[0].rx.cmdReady > 0)
	{
		slaveComm[0].rx.cmdReady = 0;
		PacketWrapper* p = fm_pool_allocate_block();
		if (p == NULL)
			return;

		memcpy(p->unpaked, &rx_command_485_1[0], COMM_STR_BUF_LEN);
		memcpy(p->packed, rx_buf_1, COMM_STR_BUF_LEN);
		/*
		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_485_1[i] = rx_command_485_1[0][i];
		}*/

		p->port = slaveComm[0].reply_port;
		payload_parse_str(p);
	}

	//Valid communication from RS-485 #2?
	if(slaveComm[1].rx.cmdReady > 0)
	{
		slaveComm[1].rx.cmdReady = 0;
		PacketWrapper* p = fm_pool_allocate_block();
		if (p == NULL)
			return;

		memcpy(p->unpaked, &rx_command_485_2[0], COMM_STR_BUF_LEN);
		memcpy(p->packed, rx_buf_2, COMM_STR_BUF_LEN);
		// parse the command and execute it
		p->port = slaveComm[1].reply_port;
		payload_parse_str(p);
	}
}

//Slave Communication function. Call at 1kHz.
void slaveTransmit(uint8_t port)
{
	PacketWrapper* p = fm_queue_get(&slave_queue);

	if (p == NULL)
		return;

	//Send to slave port:
	if((p->port == PORT_RS485_1) || (p->port == PORT_RS485_2))
	{
		flexsea_send_serial_slave(p);
	}
}

//****************************************************************************
// Private Function(s)
//****************************************************************************
