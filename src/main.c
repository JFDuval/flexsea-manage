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
	[This file] main: FlexSEA-Manage
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

// FlexSEA: Flexible & Scalable Electronics Architecture

//****************************************************************************
// Include(s)
//****************************************************************************

#include <ui.h>
#include "main.h"
#include "main_fsm.h"
#include "fm_timer.h"
#include "fm_master_slave_comm.h"
#include "fm_misc.h"
#include "user-mn.h"
#include "flexsea_board.h"
#include "flexsea_system.h"
#include "flexsea_global_structs.h"

#include "spi.h"
#include "cmd-RICNU_Knee_v1.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

//Map fsm case to an index:
void (*fsmCases[10])(void) = {&mainFSM0, &mainFSM1, &mainFSM2, &mainFSM3, \
			&mainFSM4, &mainFSM5, &mainFSM6, &mainFSM7,	&mainFSM8, &mainFSM9};

//Test:
//uint8_t txData[100] = {237,4,10,20,1,45,76,238,0,0,0};	//Read All Mn1
uint8_t txData[100] = {237,4,10,20,1,45,76,238,0,0,0};	//Read All Mn1
uint16_t wait = 0;
uint8_t info[2];

//****************************************************************************
// Function(s)
//****************************************************************************

int main(void)
{
	//Variables:
	uint8_t toggle_led0 = 0;

	//Prepare FlexSEA stack:
	init_flexsea_payload_ptr();

	//Initialize all the peripherals
	init_peripherals();

	initializeGlobalStructs();

	//Start receiving from master via interrupts
	flexsea_start_receiving_from_master();

	//Test code, use with care. Normal code might NOT run when enabled!
	//test_code_blocking();
	//test_code_non_blocking();

	init_user();

	//Infinite loop
	while(1)
	{
		//Time bases:
		//===========

		if(tb_100us_flag == 1)
		{
			tb_100us_flag = 0;

			//Timing FSM:
			fsmCases[tb_100us_timeshare]();

			//Increment value, limits to 0-9
			tb_100us_timeshare++;
			tb_100us_timeshare %= 10;

			//The code below is executed every 100us, after the previous slot.
			//Keep it short!
			mainFSM10kHz();
		}

		//1ms
		if(tb_1ms_flag)
		{
			tb_1ms_flag = 0;

		}

		//10ms
		if(tb_10ms_flag)
		{
			tb_10ms_flag = 0;

			#ifdef SPI_MASTER

			/*
			//Test code: transmit packet from SPI6
			if(wait < 250)
			{
				LED1(0);
				wait++;
			}
			else
			{
				spi6Transmit(txData, 48);
				LED1(1);
			}
			*/

			#endif

			/*
			//Test code: transmit packet from SPI6
			if(wait < 250)
			{
				LED1(0);
				wait++;
			}
			else
			{
				info[0] = PORT_RS485_1;
				//tx_cmd_ricnu_rw(TX_N_DEFAULT, 1, CTRL_NONE, 0, KEEP, 0, 0, 0, 0);
				tx_cmd_data_read_all_r(TX_N_DEFAULT);
				packAndSend(P_AND_S_DEFAULT, FLEXSEA_EXECUTE_1, info, SEND_TO_SLAVE);
				LED1(1);
			}
			*/

		}

		//100ms
		if(tb_100ms_flag)
		{
			tb_100ms_flag = 0;

			//Constant LED0 flashing while the code runs
			toggle_led0 ^= 1;
			LED0(toggle_led0);
		}

		//1000ms
		if(tb_1000ms_flag)
		{
			tb_1000ms_flag = 0;
		}
	}
}
