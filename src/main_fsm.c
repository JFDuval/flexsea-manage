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
#include "flexsea_cmd_stream.h"
#include "flexsea_global_structs.h"
#include "flexsea_board.h"
#include "fm_dio.h"
#include "fm_i2c.h"
#include "fm_adc.h"
#include "fm_ui.h"
#include "rgb_led.h"
#include "user-mn.h"
#include "fm_imu.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t new_cmd_led = 0;
uint16_t servoPos = SERVO_MIN;
uint8_t servoExtTrigger = 0;

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

void servo(uint8_t pos, uint8_t *extTrigger)
{
	static uint16_t period = 0;
	static uint16_t switchTimeout = 0;
	static int32_t servoOnDelay = 0;
	period++;
	period %= 200;

	if(servoOnDelay > 0)
		servoOnDelay--;

	if((period < pos) && (servoOnDelay > 0))
	{
		DEBUG_OUT_DIO7(1);
	}
	else
	{
		DEBUG_OUT_DIO7(0);
	}

	//Switch:
	if(switchTimeout > 0)
		switchTimeout--;

	if(!switchTimeout)
	{
		if((HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_13) == GPIO_PIN_SET) || (*extTrigger))
		{
			switchTimeout = 2000;

			if(*extTrigger)
				(*extTrigger) = 0;

			if(servoPos == SERVO_MAX)
				servoPos = SERVO_MIN;
			else
				servoPos = SERVO_MAX;
			servoOnDelay = 30000;
		}
	}
}

void KnockDetection(void)
{
    static int32_t acc, avgacc, normacc, rectacc,difftog,knocktog, lastacc, diffacc;
    static int32_t csl = 0,ktmr=0;
    static int64_t cntr =0;
    static int64_t accs;
    acc = imu.accel.z;
    cntr++;
    csl++;
    ktmr++;

    accs+= acc;
    avgacc = accs/cntr;

    normacc = acc-avgacc;
    lastacc = rectacc;
    rectacc = normacc;
    if (rectacc<0) {rectacc = -rectacc;}

    diffacc = rectacc-lastacc;
    if (diffacc<0) {diffacc = 0;}

    static int32_t mintime = 100, mindiff = 800;

    gvar0 = rectacc;

    if (csl>mintime && rectacc>mindiff)
    {
        difftog^=1;
        csl = 0;
    }

    static int32_t knocks[10] = {350,220,165,370,705,395,0,0,0,0};
    static int32_t knockindx = 0;

    if (csl == 0)
    {
        if (ktmr<knocks[knockindx]-knocks[knockindx]/2 || ktmr>knocks[knockindx]+knocks[knockindx]/2) {knockindx = 0;}
        else {knockindx++;}
        ktmr = 0;
    }

    if (knocks[knockindx]== 0)
    {
        knocktog^=1;
        knockindx = 0;
        servoExtTrigger = 1;
    }
    gvar1 = knockindx;
    gvar2 = servoExtTrigger;
    gvar3 = diffacc;
}

//1kHz time slots:
//================

//Case 0: I2C1 + slaveComm
void mainFSM0(void)
{
	slaveTransmit(PORT_RS485_1);
}

//Case 1: I2C2
void mainFSM1(void)
{
	i2c1_fsm();
}

//Case 2:
void mainFSM2(void)
{
	i2c2_fsm();
}

//Case 3:
void mainFSM3(void)
{
	slaveTransmit(PORT_RS485_2);
}

//Case 4: User Functions
void mainFSM4(void)
{
	#if(RUNTIME_FSM1 == ENABLED)
	user_fsm_1();
	#endif //RUNTIME_FSM1 == ENABLED
}

//Case 5:
void mainFSM5(void)
{

}

//Case 6:
void mainFSM6(void)
{
	//User switch:
	manag1.sw1 = read_sw1();

	//ADC:
	startAdcConversion();
}

//Case 7:
void mainFSM7(void)
{
	static int sinceLastStreamSend = 0;
	if(isStreaming)
	{
		if(!sinceLastStreamSend)
		{
			//hopefully this works ok
			uint8_t cp_str[256] = {0};
			cp_str[P_XID] = streamReceiver;
			(*flexsea_payload_ptr[streamCmd][RX_PTYPE_READ]) (cp_str, &streamPortInfo);
		}
		sinceLastStreamSend++;
		sinceLastStreamSend%=streamPeriod;
	}
}

//Case 8: User functions
void mainFSM8(void)
{
	KnockDetection();
	//#if(RUNTIME_FSM2 == ENABLED)
	//user_fsm_2();
	//#endif //RUNTIME_FSM2 == ENABLED
}

//Case 9: User Interface
void mainFSM9(void)
{
	//UI RGB LED
	rgbLedRefreshFade();
	rgb_led_ui(0, 0, 0, new_cmd_led);    //ToDo add error codes
	if(new_cmd_led) {new_cmd_led = 0;}
}

//10kHz time slot:
//================

void mainFSM10kHz(void)
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

	servo(servoPos, &servoExtTrigger);
}

//Asynchronous time slots:
//========================

void mainFSMasynchronous(void)
{

}



