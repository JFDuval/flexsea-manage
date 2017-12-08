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
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.	If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] Test code - use (with care) with PROJECT_DEV
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-05-10 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "dev.h"
#include "cmd-RICNU_Knee_v1.h"
#include "flexsea_system.h"
#include "flexsea_sys_def.h"
#include "flexsea.h"
#include "cmd-ActPack.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

struct devCtrl_s devCtrl;

#ifdef SPI_MASTER
uint8_t info[2] = {PORT_EXP, PORT_EXP};
#else
uint8_t info[2] = {PORT_RS485_1, PORT_RS485_1};
#endif

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

void dev_fsm_1(void)
{
	#if(ACTIVE_PROJECT == PROJECT_DEV)

	static uint32_t time = 0;
	static int8_t state = 0;

	//Increment time (1 tick = 1ms)
	time++;

	//Before going to a state we refresh values:
	//...

	switch(state)
	{
		case 0:	//Wait for 11 seconds to let everything load

			if (time >= 11000)
			{
				time = 0;
				state = 1;
			}

			break;

		case 1:	//Set to Position Control mode

			devCtrl.ctrl = CTRL_POSITION;
			devCtrl.g0 = 10;
			devCtrl.gains = CHANGE;

			if (time >= 5)
			{
				time = 0;
				state = 2;
				devCtrl.gains = KEEP;
			}

			break;

		case 2:	//Position setpoint moves up and down

			devCtrl.setp = 10 * time;
			if (time >= 4000){state = 3;}

			break;

		case 3:	//Position setpoint moves up and down

			devCtrl.setp = 10 * (8000-time);
			if (time >= 8000)
			{
				time = 0;
				state = 2;
			}

			break;

		default:
			//Handle exceptions here
			break;
	}

	#endif 	//(ACTIVE_PROJECT == PROJECT_DEV)
}

//Second state machine for the Dev project:
//This function is called at 1kHz. We divide it down to 250Hz for now
void dev_fsm_2(void)
{
	#if(ACTIVE_PROJECT == PROJECT_DEV)

	static uint8_t ex_refresh_fsm_state = 0;
	static uint32_t timer = 0;

	//This FSM talks to the slaves at 250Hz each
	switch(ex_refresh_fsm_state)
	{
		case 0:		//Power-up

			if(timer < 10000)
			{
				//We wait 10s before sending the first commands
				timer++;
			}
			else
			{
				//Ready to start transmitting
				ex_refresh_fsm_state = 1;
			}

			break;

		case 1:	//Communicating with Rigid-Mn #1 - ActPack

			tx_cmd_actpack_rw(TX_N_DEFAULT, 0, devCtrl.ctrl, \
					devCtrl.setp, devCtrl.gains, devCtrl.g0, devCtrl.g1, 0, 0, 0);
			packAndSend(P_AND_S_DEFAULT, FLEXSEA_MANAGE_1, info, SEND_TO_SLAVE);
			ex_refresh_fsm_state++;

			break;

		case 2:	//

			//Skipping one cycle
			ex_refresh_fsm_state++;

			break;

		case 3:	//

			//Skipping one cycle
			ex_refresh_fsm_state++;

			break;

		case 4:	//

			//Skipping one cycle
			ex_refresh_fsm_state = 1;

			break;
	}

	#endif	//ACTIVE_PROJECT == PROJECT_DEV
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

//...
