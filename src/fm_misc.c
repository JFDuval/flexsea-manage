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
	[This file] fm_misc: when it doesn't belong in any another file, it 
	ends up here...
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "fm_misc.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8_t test_payload[PAYLOAD_BUF_LEN];
uint8_t usb_test_string[40] = "[FlexSEA-Manage 0.1 USB]\n";

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************

//****************************************************************************
// Public Function(s)
//****************************************************************************

//Initialize all the peripherals
void init_peripherals(void)
{
	//Extra Power ON delay:
	HAL_Delay(100);

	//Hardware modules:
	init_systick_timer();		//SysTick timer (1kHz)
	init_timer_7();				//10kHz timebase
	init_usart1(2000000);		//USART1 (RS-485 #1)
	init_usart6(2000000);		//USART6 (RS-485 #2)
	init_rs485_outputs();
	init_leds();
	init_switches();
	init_dio();					//All inputs by default
	init_adc1();
	init_spi4();				//Plan
	//init_spi5();				//FLASH
	//init_spi6();				//Expansion
	init_i2c1();
	init_imu();
	init_adva_fc_pins();
	init_pwr_out();

	//USB
	#ifdef USE_USB
	MX_USB_DEVICE_Init();
	#endif	//USE_USB

	//Software:
	init_master_slave_comm();

	//All RGB LEDs OFF
	LEDR(0);
	LEDG(0);
	LEDB(0);

	//Default analog input states:
	set_default_analog();
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

//...
