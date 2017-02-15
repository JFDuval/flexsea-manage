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
	[This file] main_fsm: Contains all the case() code for the main FSM
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "main_fsm.h"
#include "fm_master_slave_comm.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t new_cmd_led = 0;

//Test code only - ToDo integrate or remove:
//#define BT_SLAVE_STREAM
#ifdef BT_SLAVE_STREAM
uint8_t info[2] = {PORT_WIRELESS, PORT_WIRELESS};
uint16_t delay = 0;
#endif

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

//1kHz time slots:
//================

//Case 0: I2C1 + slaveComm
void main_fsm_case_0(void)
{
	slaveTransmit(PORT_RS485_1);
}

//Case 1: I2C2
void main_fsm_case_1(void)
{
	i2c1_fsm();
}

//Case 2:
void main_fsm_case_2(void)
{
	i2c2_fsm();
}

//Case 3:
void main_fsm_case_3(void)
{
	slaveTransmit(PORT_RS485_2);
}

//Case 4: User Functions
void main_fsm_case_4(void)
{
	#if(RUNTIME_FSM1 == ENABLED)
	user_fsm_1();
	#endif //RUNTIME_FSM1 == ENABLED
}

//Case 5:
void main_fsm_case_5(void)
{
	#ifdef BT_SLAVE_STREAM

		//No transmission for the first 5s:
		delay++;
		if(delay < 5000)
			return;

		//Send data at 100Hz:
		static uint8_t cnt = 0;
		cnt++;
		cnt %= 10;
		if(!cnt)
		{
			tx_cmd_data_read_all_w(TX_N_DEFAULT);
			//delayUsBlocking(500);	//ToDo remove, test only! ************
			packAndSend(P_AND_S_DEFAULT, FLEXSEA_PLAN_1, info, SEND_TO_MASTER);
		}

	#endif	//BT_SLAVE_STREAM
}

//Case 6:
void main_fsm_case_6(void)
{
	//User switch:
	manag1.sw1 = read_sw1();
}

//Case 7:
void main_fsm_case_7(void)
{

}

//Case 8: User functions
void main_fsm_case_8(void)
{
	#if(RUNTIME_FSM2 == ENABLED)
	user_fsm_2();
	#endif //RUNTIME_FSM2 == ENABLED
}

//Case 9: User Interface
void main_fsm_case_9(void)
{
	//UI RGB LED
	rgbLedRefreshFade();
	rgb_led_ui(0, 0, 0, new_cmd_led);    //ToDo add error codes
	if(new_cmd_led)
	{
		new_cmd_led = 0;
	}
}

//10kHz time slot:
//================

void main_fsm_10kHz(void)
{
	#ifdef USE_COMM_TEST

		comm_test();

	#endif	//USE_COMM_TEST

	//RGB:
	rgbLedRefresh();

	//Communication with our Master & Slave(s):
	//=========================================

	//SPI or USB reception from a Plan board:
	flexsea_receive_from_master();

	//RS-485 reception from an Execute board:
	flexsea_receive_from_slave();

	//Did we receive new commands? Can we parse them?
	parseMasterCommands(&new_cmd_led);
	parseSlaveCommands(&new_cmd_led);
}

//Asynchronous time slots:
//========================

void main_fsm_asynchronous(void)
{

}

//****************************************************************************
// Private Function(s)
//****************************************************************************
