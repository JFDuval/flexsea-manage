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
	[This file] user: User Projects & Functions
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "user.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

//MIT Ankle 2-DoF:
#if(ACTIVE_PROJECT == PROJECT_ANKLE_2DOF)

struct ankle2dof_s ankle2dof_left, ankle2dof_right;

#endif	//PROJECT_ANKLE_2DOF

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************	

//****************************************************************************
// Public Function(s)
//****************************************************************************

//Initialization function - call once in main.c, before while()
void init_user(void)
{	
	//RIC/NU Knee:
	#if(ACTIVE_PROJECT == PROJECT_RICNU_KNEE)
	init_ricnu_knee();
	#endif	//PROJECT_RICNU_KNEE
	
	//MIT Ankle 2-DoF:
	#if(ACTIVE_PROJECT == PROJECT_ANKLE_2DOF)
	init_ankle_2dof();
	#endif	//PROJECT_ANKLE_2DOF
	
	//MIT d'Arbeloff Dual-Speed Dual-Motor:
	#if(ACTIVE_PROJECT == PROJECT_DSDM)
	init_dsdm();
	#endif	//PROJECT_DSDM  
}

//Call this function in one of the main while time slots.
void user_fsm_1(void)
{
	
	//RIC/NU Knee code
	#if(ACTIVE_PROJECT == PROJECT_RICNU_KNEE)				                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	ricnu_knee_fsm();						
	#endif	//PROJECT_RICNU_KNEE

	//MIT Ankle 2-DoF:
	#if(ACTIVE_PROJECT == PROJECT_ANKLE_2DOF)
	ankle_2dof_fsm_1();
	#endif	//PROJECT_ANKLE_2DOF
	
	//MIT d'Arbeloff Dual-Speed Dual-Motor:
	#if(ACTIVE_PROJECT == PROJECT_DSDM)				                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	dsdm_fsm();						
	#endif	//PROJECT_DSDM	
}

//Optional second FSM
//Call this function in one of the main while time slots.
void user_fsm_2(void)
{
	//MIT Ankle 2-DoF:
	#if(ACTIVE_PROJECT == PROJECT_ANKLE_2DOF)
	ankle_2dof_fsm_2();
	#endif	//PROJECT_ANKLE_2DOF
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

