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

//Case 0: 
void main_fsm_case_0(void)
{
	slave_comm_trig = 1;

	//test_comm_rw_master_v2(systick_100us_timeshare);
}

//Case 1: 
void main_fsm_case_1(void)
{

}

//Case 2:
void main_fsm_case_2(void)
{
	//Test code 03/22/2016:

	/*
	static uint8_t xmit_toggle = 0;
	int tx_byte = 0, commstrlen = 0;
	uint8_t test_payload[PAYLOAD_BUF_LEN];

	 xmit_toggle ^= 1;	//500Hz
	 if(xmit_toggle)
	 {
	 tx_byte = tx_cmd_ctrl_special_1(FLEXSEA_EXECUTE_1, CMD_READ, test_payload, PAYLOAD_BUF_LEN, 0, 0, 0, 0, 0, 0);
	 commstrlen = comm_gen_str(test_payload, comm_str_485_1, tx_byte);
	 commstrlen = COMM_STR_BUF_LEN;
	 flexsea_send_serial_slave(PORT_RS485_1, comm_str_485_1, commstrlen);

	 slaves_485_1.xmit.listen = 1;
	 }
	 */
	//test_comm_rw_master_v1();
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
	//Master-Slave communications
	slave_comm(&slave_comm_trig);

	//RGB:
	rgbLedRefresh();
}

//Asynchronous time slots:
//========================

void main_fsm_asynchronous(void)
{

}

//****************************************************************************
// Private Function(s)
//****************************************************************************

