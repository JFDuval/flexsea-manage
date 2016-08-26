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
	[This file] user_dsdm: Dual-Speed Dual-Motor Actuator Functions
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "user_dsdm.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************	

static void dsdm_refresh_values(void);

//****************************************************************************
// Public Function(s)
//****************************************************************************

//Call this function once in main.c, just before the while()
void init_dsdm(void)
{	
	//...
}

//Ankle 2-DoF Finite State Machine.
//Call this function in one of the main while time slots.
void dsdm_fsm(void)
{
	#if(ACTIVE_PROJECT == PROJECT_DSDM)
    static uint32 time = 0;
    static uint8 state = 0;
	
    //Increment time (1 tick = 1ms)
    time++;
    
	//Before going to a state we refresh values:
	dsdm_refresh_values();
	
	//Nothing programmed yet...
	switch(state)
	{
		case 0:
            break;
		case 1:			
            break;
        case 2:
            break;
        default:
			//Handle exceptions here
			break;
	}
	#endif
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

//Note: 'static' makes them private; they can only called from functions in this
//file. It's safer than making everything global.

//Here's an example function:
static void dsdm_refresh_values(void)
{
	//...
}
//That function can be called from the FSM.
