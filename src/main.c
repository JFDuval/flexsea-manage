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

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "fm_master_slave_comm.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

//****************************************************************************
// Function(s)
//****************************************************************************

int main(void)
{
	//Variables:
	uint8_t toggle_led0 = 0;

	//Prepare FlexSEA stack:
	init_flexsea_payload_ptr();

	initSlaveComm();

	//Initialize queues (comm)
	fm_pool_init();	//ToDo: this is called init_flexsea_payload_ptr, do I need it here?

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

			switch(tb_100us_timeshare)
			{
				case 0:
					main_fsm_case_0();
					break;
				case 1:
					main_fsm_case_1();
					break;
				case 2:
					main_fsm_case_2();
					break;
				case 3:
					main_fsm_case_3();
					break;
				case 4:
					main_fsm_case_4();
					break;
				case 5:
					main_fsm_case_5();
					break;
				case 6:
					main_fsm_case_6();
					break;
				case 7:
					main_fsm_case_7();
					break;
				case 8:
					main_fsm_case_8();
					break;
				case 9:
					main_fsm_case_9();
					break;
				default:
					break;
			}

			//Increment value, limits to 0-9
			tb_100us_timeshare++;
			tb_100us_timeshare %= 10;

			//The code below is executed every 100us, after the previous slot.
			//Keep it short!
			main_fsm_10kHz();
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

			//...
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
