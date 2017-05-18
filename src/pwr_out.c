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
	[This file] fm_pwr_out: Deals with the 2 power outputs
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include <pwr_out.h>
#include "main.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

//Outputs: //PWROUT0 on PD11, PWROUT1 on PD12
void init_pwr_out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIO Peripheral clock on port D
	__GPIOD_CLK_ENABLE();

	// Configure pin in output push/pull mode
	GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
}

//Writes to the power outputs. For now it's on or off.
//ToDo Could be PWM 
void write_pwr_out(unsigned int pwr0, unsigned int pwr1)
{
	//No PWM support yet, we make sure it's 0 or 1:
	if(pwr0 > 1)
		pwr0 = 1;
	if(pwr1 > 1)
		pwr1 = 1;

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, pwr0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, pwr1);
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

//...
