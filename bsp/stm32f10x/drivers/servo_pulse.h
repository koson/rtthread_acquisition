/*
 * File      : servo_pulse.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     mgcheng      the first version
 */

#ifndef __SERVO_PULSE_H__
#define __SERVO_PULSE_H__

#include <rtthread.h>

#define SEVER_PULSE_PER_ROUND	1000	//pulses_per round
#define DISTANCE_BETWEEN_CALIBRATIONS 125	//distance between two calibrations(mm)
#define CALIBRATION_PULES	DISTANCE_BETWEEN_CALIBRATIONS*SEVER_PULSE_PER_ROUND/314	//¾àÀëµ¥Î»Îªmm
#define DISTANCE_BETWEEN_SLEEPERS 600	//mm
#define DISTANCE_ERROR_RANGE	200		//error correction distance (mm)


void rt_servo_pulse_init(void);

#endif
