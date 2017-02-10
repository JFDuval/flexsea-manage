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

#include "main.h"
#include "fm_master_slave_comm.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t tmp_rx_command_spi[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_usb[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_wireless[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_1[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_2[PAYLOAD_BUF_LEN];

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

void initSlaveComm(void)
{
	//...
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
	volatile uint32_t i = 0;
	uint8_t info[2] = {0,0};

	//Valid communication from SPI?
	if(cmd_ready_spi != 0)
	{
		cmd_ready_spi = 0;

		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_spi[i] = rx_command_spi[0][i];
		}
		// parse the command and execute it
		info[0] = PORT_SPI;
		payload_parse_str(tmp_rx_command_spi, info);

		//LED:
		*new_cmd = 1;
	}

	//Valid communication from USB?
	if(cmd_ready_usb != 0)
	{
		cmd_ready_usb = 0;

		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_usb[i] = rx_command_usb[0][i];
		}
		// parse the command and execute it
		info[0] = PORT_USB;
		payload_parse_str(tmp_rx_command_usb, info);

		//LED:
		*new_cmd = 1;
	}

	//Valid communication from Wireless?
	if(masterComm[2].rx.cmdReady > 0)
	{
		masterComm[2].rx.cmdReady = 0;

		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_wireless[i] = rx_command_wireless[0][i];
		}
		// parse the command and execute it
		info[0] = PORT_WIRELESS;
		payload_parse_str(tmp_rx_command_wireless, info);

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

		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_485_1[i] = rx_command_485_1[0][i];
		}
		// parse the command and execute it
		info[0] = PORT_485_1;
		payload_parse_str(tmp_rx_command_485_1, info);
	}

	//Valid communication from RS-485 #2?
	if(slaveComm[1].rx.cmdReady > 0)
	{
		slaveComm[1].rx.cmdReady = 0;

		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_485_2[i] = rx_command_485_2[0][i];
		}
		// parse the command and execute it
		info[0] = PORT_485_2;
		payload_parse_str(tmp_rx_command_485_2, info);
	}
}

//Slave Communication function. Call at 1kHz.
void slaveTransmit(uint8_t port)
{
	if(port == PORT_485_1)
	{
		/*Note: this is only a demonstration. In the final application, we want
		 * to send the commands accumulated on a ring buffer here.*/

		//Packet injection:
		if(slaveComm[0].tx.inject == 1)
		{
			slaveComm[0].tx.inject = 0;
			if(IS_CMD_RW(slaveComm[0].tx.cmd) == READ)
			{
				slaveComm[0].transceiverState = TRANS_STATE_TX_THEN_RX;
			}
			else
			{
				slaveComm[0].transceiverState = TRANS_STATE_TX;
			}

			flexsea_send_serial_slave(port, slaveComm[0].tx.txBuf, slaveComm[0].tx.len);
		}

		//Debugging test: ***ToDo Remove***
		static uint8_t toggle1 = 0;
		toggle1 ^= 1;
		DEBUG_OUT_DIO4(toggle1);
	}
	else if(port == PORT_485_2)
	{
		/*Note: this is only a demonstration. In the final application, we want
		 * to send the commands accumulated on a ring buffer here.*/

		//Packet injection:
		if(slaveComm[1].tx.inject == 1)
		{
			slaveComm[1].tx.inject = 0;
			if(IS_CMD_RW(slaveComm[1].tx.cmd) == READ)
			{
				slaveComm[1].transceiverState = TRANS_STATE_TX_THEN_RX;
			}
			else
			{
				slaveComm[1].transceiverState = TRANS_STATE_TX;
			}

			flexsea_send_serial_slave(port, slaveComm[1].tx.txBuf, slaveComm[1].tx.len);
		}
	}
}

//****************************************************************************
// Private Function(s)
//****************************************************************************
