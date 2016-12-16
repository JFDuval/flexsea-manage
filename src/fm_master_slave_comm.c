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

//ToDo Update this block of comments
//Manage can be in slave_comm_mode = TRANSPARENT or slave_comm_mode = AUTOSAMPLING
//TRANSPARENT: Manage routes communication between the Master and the Slaves. All
//	the timings come from the Master (Manage routes as fast as possible)
//AUTOSAMPLING: Manage will do periodic Write/Read to n slaves and update a local structure
//	Master can Write/Read to that structure at any point. The timing of the communications with
//	the Master doesn't influence the timing of the communications between Manage and its slave(s)
//	other than the fact that a new packet from Master will have priority for the next slot.
//The 2 slave busses can have a different slave_comm_mode.

#include "main.h"
#include "fm_master_slave_comm.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t tmp_rx_command_spi[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_usb[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_1[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_485_2[PAYLOAD_BUF_LEN];

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

static void slave_comm_single(struct slave_comm_s *slave, uint8_t *trig);
static void write_to_slave_xmit(struct slave_comm_s *slave);
static void write_to_slave_autosample(struct slave_comm_s *slave);
static void slaves_485_1_autosample(void);
static void slaves_485_2_autosample(void);

//****************************************************************************
// Public Function(s)
//****************************************************************************

//Prepares the structures:
void init_master_slave_comm(void)
{
	//Port #1:
	slaves_485_1.mode = SC_TRANSPARENT;
	slaves_485_1.port = PORT_RS485_1;
	slaves_485_1.xmit.flag = 0;
	slaves_485_1.xmit.length = COMM_STR_BUF_LEN;
	slaves_485_1.xmit.cmd = 0;
	slaves_485_1.xmit.listen = 0;
	slaves_485_1.autosample.flag = 0;
	slaves_485_1.autosample.length = COMM_STR_BUF_LEN;
	slaves_485_1.autosample.cmd = 0;
	slaves_485_1.autosample.listen = 0;

	//Port #2:
	slaves_485_2.mode = SC_TRANSPARENT;
	slaves_485_2.port = PORT_RS485_2;
	slaves_485_2.xmit.flag = 0;
	slaves_485_2.xmit.length = COMM_STR_BUF_LEN;
	slaves_485_2.xmit.cmd = 0;
	slaves_485_2.xmit.listen = 0;
	slaves_485_2.autosample.flag = 0;
	slaves_485_2.autosample.length = COMM_STR_BUF_LEN;
	slaves_485_2.autosample.cmd = 0;
	slaves_485_2.autosample.listen = 0;
}

//This function gets called at a much higher frequency than the slave communication to
//make sure that we don't add delays in Transparent mode. autosample_trig will clock the
//autosampling state machine(s)
void slave_comm(uint8_t *autosample_trig)
{
	//Slave bus #1:
	slave_comm_single(&slaves_485_1, autosample_trig);

	//Slave bus #2:
	slave_comm_single(&slaves_485_2, autosample_trig);
}

//Did we receive new commands? Can we parse them?
void parse_master_slave_commands(uint8_t *new_cmd)
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

	//Valid communication from RS-485 #1?
	if(slaves_485_1.cmd_ready != 0)
	{
		slaves_485_1.cmd_ready = 0;

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
	if(slaves_485_2.cmd_ready != 0)
	{
		slaves_485_2.cmd_ready = 0;

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

//****************************************************************************
// Private Function(s)
//****************************************************************************

//slave_comm takes care of 2 slave busses. To simplify code it will call slave_comm_single twice.
static void slave_comm_single(struct slave_comm_s *slave, uint8_t *trig)
{
	if(slave->mode == SC_TRANSPARENT)
	{
		//In Transparent mode we do our best to route packets as fast as possible

		//New data ready to be transmitted?
		if(slave->xmit.flag == 1)
		{
			//Transmit data:
			write_to_slave_xmit(slave);

			//We are done, lower the flag
			slave->xmit.flag = 0;
		}
	}
	else if(slave->mode == SC_AUTOSAMPLING)
	{
		//Autosampling is enabled. Will be bypassed is xmit_flag is set.

		//New data ready to be transmitted?
		if(slave->xmit.flag == 1)
		{
			//Transmit data:
			write_to_slave_xmit(slave);

			//We are done, lower the flag
			slave->xmit.flag = 0;
		}
		else
		{
			//No bypassing

			if((*trig) == 1)
			{
				//Time to send a new packet:
				slaves_485_1_autosample();

				(*trig) = 0;
			}
			else if((*trig) == 2)
			{
				//Time to send a new packet:
				slaves_485_2_autosample();

				(*trig) = 0;
			}
		}
	}
}

//Sends a packet to RS-485, xmit mode. Sets the Listen flag if needed.
static void write_to_slave_xmit(struct slave_comm_s *slave)
{
	//Transmit data:
	flexsea_send_serial_slave(slave->port, slave->xmit.str, slave->xmit.length);

	//Are we trying to read?
	if(IS_CMD_RW(slave->xmit.cmd) == READ)
	{
		//We expect an answer, start listening:
		slave->xmit.listen = 1;
	}
}

//Sends a packet to RS-485, autosample mode. Sets the Listen flag if needed.
static void write_to_slave_autosample(struct slave_comm_s *slave)
{
	//Transmit data:
	flexsea_send_serial_slave(slave->port, slave->autosample.str,
			slave->autosample.length);

	//Are we trying to read?
	if(IS_CMD_RW(slave->autosample.cmd) == READ)
	{
		//We expect an answer, start listening:
		slave->autosample.listen = 1;
	}
}

//State-machine for the autosampling on bus #1
static void slaves_485_1_autosample(void)
{
	uint8_t numb = 0;

	//Experiment #1: Send Special1

	/* Disabled due to the deletion of spc4_ex1
	numb = tx_cmd_ctrl_special_1(FLEXSEA_EXECUTE_1, CMD_READ, payload_str,
			PAYLOAD_BUF_LEN, spc4_ex1.ctrl_w, spc4_ex1.ctrl, spc4_ex1.encoder_w,
			spc4_ex1.encoder, spc4_ex1.current, spc4_ex1.open_spd);
			*/
	numb = comm_gen_str(payload_str, comm_str_485_1, PAYLOAD_BUF_LEN);
	numb = COMM_STR_BUF_LEN;

	flexsea_send_serial_slave(PORT_RS485_1, comm_str_485_1, numb);
	slaves_485_1.autosample.listen = 1;
}

//State-machine for the autosampling on bus #2
static void slaves_485_2_autosample(void)
{
	uint8_t numb = 0;

	//Experiment #1: Send Special1

	/* Disabled due to the deletion of spc4_ex1
	numb = tx_cmd_ctrl_special_1(FLEXSEA_EXECUTE_2, CMD_READ, payload_str,
			PAYLOAD_BUF_LEN, spc4_ex2.ctrl_w, spc4_ex2.ctrl, spc4_ex2.encoder_w,
			spc4_ex2.encoder, spc4_ex2.current, spc4_ex2.open_spd);
			*/
	numb = comm_gen_str(payload_str, comm_str_485_2, PAYLOAD_BUF_LEN);
	numb = COMM_STR_BUF_LEN;

	flexsea_send_serial_slave(PORT_RS485_2, comm_str_485_2, numb);
	slaves_485_2.autosample.listen = 1;
}
