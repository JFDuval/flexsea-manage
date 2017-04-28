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
	[This file] main: FlexSEA-Manage
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-23 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_

//****************************************************************************
// Include(s)
//****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "cortexm/ExceptionHandlers.h"

/*
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_tim.h"

#include "main_fsm.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "fm_timer.h"
#include "fm_misc.h"
#include "fm_ui.h"
#include "rgb_led.h"
#include "fm_uarts.h"
#include "fm_adc.h"
#include "fm_adv_analog.h"
#include "fm_dio.h"
#include "fm_spi.h"
#include "fm_i2c.h"
#include "fm_imu.h"
#include "fm_pwr_out.h"
#include "fm_batt.h"
#include "flash_driver.h"
#include "flash_logger.h"
#include "comm_test.h"
#include "flexsea_board.h"
#include "../flexsea-user/inc/user-mn.h"
#include "../flexsea-system/inc/flexsea_system.h"
#include "../flexsea-comm/inc/flexsea.h"
#include "../flexsea-system/test/flexsea-system_test-all.h"
#include "../flexsea-comm/test/flexsea-comm_test-all.h"
*/


//****************************************************************************
// Prototype(s):
//****************************************************************************

int main(void);

//****************************************************************************
// Definition(s):
//****************************************************************************

extern int16_t gvar0, gvar1, gvar2, gvar3;

#endif // MAIN_H_
