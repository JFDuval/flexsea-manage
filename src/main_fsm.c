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

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t slave_comm_trig = 0;
uint8_t new_cmd_led = 0;

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
	slave_comm_trig = 1;

	i2c1_fsm();
}

//Case 1: I2C2
void main_fsm_case_1(void)
{
	i2c2_fsm();
}

//Case 2:
void main_fsm_case_2(void)
{
}

//Case 3:
void main_fsm_case_3(void)
{

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
	slave_comm_trig = 2;
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
	/*
	//Test code: reset USB reception every second:
	//ToDo ***Remove***
	static int cnt = 0;
	cnt++;
	cnt %= 1000;
	if(!cnt)
	{
		USBD_CDC_SetRxBuffer(hUsbDevice_0, UserRxBufferFS);
		USBD_CDC_ReceivePacket(hUsbDevice_0);
	}
	*/
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
	DEBUG_OUT_DIO4(1);

	#ifdef USE_COMM_TEST

		comm_test();

	#endif	//USE_COMM_TEST

	//Master-Slave communications
	slave_comm(&slave_comm_trig);

	//RGB:
	rgbLedRefresh();

	//Communication with our Master & Slave(s):
	//=========================================

	//SPI or USB reception from a Plan board:
	DEBUG_OUT_DIO5(1);
	flexsea_receive_from_master();
	DEBUG_OUT_DIO5(0);

	//RS-485 reception from an Execute board:
	flexsea_receive_from_slave();

	//Did we receive new commands? Can we parse them?
	parse_master_slave_commands(&new_cmd_led);

	DEBUG_OUT_DIO4(0);
}

//Asynchronous time slots:
//========================

void main_fsm_asynchronous(void)
{

}

//****************************************************************************
// Private Function(s)
//****************************************************************************

