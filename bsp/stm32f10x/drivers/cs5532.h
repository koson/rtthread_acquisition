#ifndef __CS5532_H__
#define __CS5532_H__

#include    "stm32f10x.h"

/* Const Macro Defination */
#define SYNC1       ((uint8_t) 0xFF)
#define SYNC0       ((uint8_t) 0xFE)
#define NULL_BYTE   ((uint8_t) 0x00)

/* Configer Register Command Defination */
#define CMD_WRITE   ((uint8_t)(0x00<<3))
#define CMD_READ    ((uint8_t)(0X01<<3))

/* Channel Select Bits */
#define channel_0   ((uint8_t)(0x00<<4))
#define channel_1   ((uint8_t)(0x01<<4))
#define channel_2   ((uint8_t)(0x02<<4))
#define channel_3   ((uint8_t)(0x03<<4))

#define REG_OFFSET  ((uint8_t) 0x01)//all channel   ((uint8_t) 0x01)    //offset register 1
#define REG_GAIN    ((uint8_t) 0x02)//all channel   ((uint8_t) 0x02)    //gain register 1
#define REG_CONFIG  ((uint8_t) 0x03)
#define REG_CHN_STP ((uint8_t) 0x05)//all channel   ((uint8_t) 0x05)    //channel-setup register1

/* configer mode & start conversion */
#define SINGLE      ((uint8_t)(0x00<<6))
#define CONTINUOUS  ((uint8_t)(0x01<<6))
#define PERFORM_CONVERSION  ((uint8_t)(0x80))

/* start offset calibration */
#define SYSTEM_OFFSET_CAL   ((uint8_t)(0x80+0x05))
#define SYSTEM_GAIN_CAL     ((uint8_t)(0x80+0x06))

/* configuration register defination */
//bufore config register, must to rest the serial port sunchroization
#define STANDBY_MODE        ((uint32_t)(0x00000000<<31))    //save mode
#define SLEEP_MODE          ((uint32_t)(0x00000001<<31))
#define POWER_SAVE_MODE     ((uint32_t)(0x00000001<<30))

#define NORMAL_MODE         ((uint32_t)(0x00000000<<29))    //normal operation
#define SYSTEM_RESET        ((uint32_t)(0x00000001<<29))    //reset the system when set, and then wait for 8clk and clear.
#define RESET_STATUS        ((uint32_t)(0x00000001<<28))    //setted when system reset, clear by read.

#define SHORT_INPUTS        ((uint32_t)(0x00000001<<27))    //short the input pins to ground internal

#define VREF_HIGH           ((uint32_t)(0x00000000<<25))    //Vref: 2.5~((VA+)-(VA-))V
#define VREF_LOW            ((uint32_t)(0x00000001<<25))    //Vref:1~2.5V

#define CR_A1_0             ((uint32_t)(0x00000000<<24))    //logic out control pins
#define CR_A1_1             ((uint32_t)(0x00000001<<24))
#define CR_A0_0             ((uint32_t)(0x00000000<<23))
#define CR_A0_1             ((uint32_t)(0x00000001<<23))

#define LINE_FREQ_60        ((uint32_t)(0x00000000<<19))    //default
#define LINE_FREQ_50        ((uint32_t)(0x00000001<<19))

#define DATARATE_100        ((uint32_t)(0x00000000<<23))    //word out rate
#define DATARATE_50         ((uint32_t)(0x00000001<<23))
#define DATARATE_25         ((uint32_t)(0x00000002<<23))
#define DATARATE_12P5       ((uint32_t)(0x00000003<<23))
#define DATARATE_6P25       ((uint32_t)(0x00000004<<23))
#define DATARATE_3200       ((uint32_t)(0x00000008<<23))
#define DATARATE_1600       ((uint32_t)(0x00000009<<23))
#define DATARATE_800        ((uint32_t)(0x0000000a<<23))
#define DATARATE_400        ((uint32_t)(0x0000000b<<23))
#define DATARATE_200        ((uint32_t)(0x0000000c<<23))

/* define the channel setup register */
#define BIPOLAR_MODE        ((uint32_t)(0x00000000<<22))
#define UNIPOLAR_MODE       ((uint32_t)(0x00000001<<22))

#define TURN_OFF_300NA      ((uint32_t)(0x00000000<<9))
#define TURN_ON_300NA       ((uint32_t)(0x00000001<<9)) //open circuit detection, 300na

#define OVER_RANGE          ((uint8_t) (0x01)<<2)

void rt_cs5532_init(void);

#endif


















