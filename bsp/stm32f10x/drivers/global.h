// This is available in all editors.
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "rtthread.h"

#define USING_TXD6_ARB1
#ifndef USING_TXD6_ARB1
#define USING_VG035P
#endif
#define USING_JHT_IA

#define TXD6_ARB1_RX_BEGIN  0xDD
#define TXD6_ARB1_RX_SIZE   8
#define TXD6_ARB1_RX_END

#define VG035P_RX_BEGIN     0xDD
#define VG035P_RX_SIZE      12
#define VG035P_RX_END

#define JHT_IA_RX_BEGIN     0xDD
#define JHT_IA_RX_SIZE      12
#define JHT_IA_RX_END

#define UART1_RX_EVENT      ((rt_uint32_t) (0x00000001<<1))
#define UART2_RX_EVENT      ((rt_uint32_t) (0x00000001<<2))
#define UART3_RX_EVENT      ((rt_uint32_t) (0x00000001<<3))
#define UART4_RX_EVENT      ((rt_uint32_t) (0x00000001<<4))
#define UART5_RX_EVENT      ((rt_uint32_t) (0x00000001<<5))
#define UART6_RX_EVENT      ((rt_uint32_t) (0x00000001<<6))

#define RX_EVENT    (UART2_RX_EVENT|UART3_RX_EVENT|UART4_RX_EVENT|UART5_RX_EVENT)//(UART4_RX_EVENT|UART5_RX_EVENT) //(UART2_RX_EVENT|UART3_RX_EVENT|UART4_RX_EVENT|UART5_RX_EVENT)

enum rx_stat
{
    WAIT_BEGIN,
    WAIT_END,
};

enum rx_checksum
{
    CHECKSUM,
    NONCHECKSUM,
};



#ifndef RT_SERIAL_RB_BUFSZ
#define RT_SERIAL_RB_BUFSZ	64
#endif
/***
 * T0 -> UART4
 * T1 -> UART5
 * (T2 -> UART1)
 * J0 -> UART3
 * J1 -> UART2
 * (J2 -> UART6)
 ***/

#define RT_USING_T0
#define RT_USING_T1
//#define RT_USING_T2
#define RT_USING_J0
#define RT_USING_J1
//#define RT_USING_J2

struct rt_uart_rx
{
    struct rt_semaphore rx_sem;
    enum rx_stat stat;
    uint8_t index;
    enum rx_checksum checksum;
    uint8_t rx_begin;
    uint8_t rx_size;
    char rx_buffer[RT_SERIAL_RB_BUFSZ];
    char previous_rx_buffer[RT_SERIAL_RB_BUFSZ];
};

extern struct rt_uart_rx uart2_rx;
extern struct rt_uart_rx uart3_rx;
extern struct rt_uart_rx uart4_rx;
extern struct rt_uart_rx uart5_rx;


struct rt_TXD6_ARB1_sensor_data
{
    float angular_speed;
};

#pragma pack(push)
#pragma pack(1)
struct rt_VG035P_sensor_data
{
    float angular_speed;
    float angular;
    float temperature;
    rt_uint8_t index;
};
struct rt_JHT_IA_sensor_data
{
    float acceleration;
    float velocity;
    float temperature;
    rt_uint8_t index;
};
#pragma pack(pop)

struct rt_uart_rx_sensor_data
{
#ifdef  USING_TXD6_ARB1
//    struct rt_TXD6_ARB1_sensor_data T0;
//    struct rt_TXD6_ARB1_sensor_data T1;
    struct rt_VG035P_sensor_data T0;
    struct rt_VG035P_sensor_data T1;
#endif
#ifdef  USING_VG035P
    struct rt_VG035P_sensor_data T0;
    struct rt_VG035P_sensor_data T1;
#endif
#ifdef  USING_JHT_IA
    struct rt_JHT_IA_sensor_data J0;
    struct rt_JHT_IA_sensor_data J1;
#endif
    float laser_distance_sensor_left;
};

extern struct rt_uart_rx_sensor_data previous_ALL_DATA, ALL_DATA;

rt_err_t uart_rx_check_data(struct rt_uart_rx* uartx_rx, rt_uint8_t ch);
rt_err_t uart_rx_customer(rt_device_t device, struct rt_uart_rx* uartx_rx);
uint8_t crc_sum(uint8_t *data, uint8_t length);
void int24_to_int32(char* buf, rt_int32_t* data);
void TXD6_ARB1_decode(char* buf, struct rt_VG035P_sensor_data* sensor_data);
void VG035P_decode(char* buf, struct rt_VG035P_sensor_data* sensor_data);
void JHT_IA_decode(char* buf, struct rt_JHT_IA_sensor_data* sensor_data);


#endif





































