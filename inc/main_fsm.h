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
	
#ifndef INC_MAIN_FSM_H
#define INC_MAIN_FSM_H

//****************************************************************************
// Include(s)
//****************************************************************************		
	
#include "main.h"
	
//****************************************************************************
// Shared variable(s)
//****************************************************************************

extern uint8_t new_cmd_led;

//****************************************************************************
// Public Function Prototype(s):
//****************************************************************************

void main_fsm_case_0(void);
void main_fsm_case_1(void);
void main_fsm_case_2(void);
void main_fsm_case_3(void);
void main_fsm_case_4(void);
void main_fsm_case_5(void);
void main_fsm_case_6(void);
void main_fsm_case_7(void);
void main_fsm_case_8(void);
void main_fsm_case_9(void);

void main_fsm_10kHz(void);
void main_fsm_asynchronous(void);

//****************************************************************************
// Definition(s):
//****************************************************************************	

//****************************************************************************
// Structure(s)
//****************************************************************************	

	
#endif	//INC_MAIN_FSM_H
	
