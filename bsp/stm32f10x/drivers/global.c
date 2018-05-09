#include "global.h"

#ifdef USING_TXD6_ARB1
//T0
struct rt_uart_rx uart4_rx={
                                   {0},
                                   WAIT_BEGIN,
                                   0,NONCHECKSUM,
                                   TXD6_ARB1_RX_BEGIN,
                                   TXD6_ARB1_RX_SIZE,{0},{0}};
//T1
struct rt_uart_rx uart5_rx={
                                    {0},
                                    WAIT_BEGIN,
                                    0,NONCHECKSUM,
                                    TXD6_ARB1_RX_BEGIN,
                                    TXD6_ARB1_RX_SIZE,{0},{0}};
//T2
struct rt_uart_rx uart1_rx={
                                    {0},
                                    WAIT_BEGIN,
                                    0,NONCHECKSUM,
                                    TXD6_ARB1_RX_BEGIN,
                                    TXD6_ARB1_RX_SIZE,{0},{0}};
#endif

#ifdef USING_VG035P
//T0
struct rt_uart_rx uart4_rx={
                                   {0},
                                   WAIT_BEGIN,
                                   0,CHECKSUM,
                                   VG035P_RX_BEGIN,
                                   VG035P_RX_SIZE,{0},{0}};
//T1
struct rt_uart_rx uart5_rx={
                                    {0},
                                    WAIT_BEGIN,
                                    0,CHECKSUM,
                                    VG035P_RX_BEGIN,
                                    VG035P_RX_SIZE,{0},{0}};
//T2
struct rt_uart_rx uart1_rx={
                                    {0},
                                    WAIT_BEGIN,
                                    0,CHECKSUM,
                                    VG035P_RX_BEGIN,
                                    VG035P_RX_SIZE,{0},{0}};
#endif

#ifdef  USING_JHT_IA
//JO
struct rt_uart_rx uart3_rx={
                                   {0},
                                   WAIT_BEGIN,
                                   0,CHECKSUM,
                                   JHT_IA_RX_BEGIN,
                                   JHT_IA_RX_SIZE,{0},{0}};
//J1
struct rt_uart_rx uart2_rx={
                                    {0},
                                    WAIT_BEGIN,
                                    0,CHECKSUM,
                                    JHT_IA_RX_BEGIN,
                                    JHT_IA_RX_SIZE,{0},{0}};
//J2
struct rt_uart_rx uart6_rx={
                                    {0},
                                    WAIT_BEGIN,
                                    0,CHECKSUM,
                                    JHT_IA_RX_BEGIN,
                                    JHT_IA_RX_SIZE,{0},{0}};
#endif

 /* crc sum check */
 uint8_t crc_sum(uint8_t *data, uint8_t length)
 {
     uint8_t sum = 0;
     uint8_t i;

     for(i=0;i<length;i++)
     {
         sum += *data;
         data++;
     }
     return sum;
 }

void int24_to_int32(char* buf, rt_int32_t* data)
{
    rt_int32_t temp = 0;
    temp = (rt_int32_t)((buf[1]<<16) | (buf[2]<<8) | buf[0]);
    if(temp >= 0x00800000)  //negative
    {
        temp |= 0xFF000000;
    }
    *data = temp;
}

void TXD6_ARB1_decode(char* buf, struct rt_VG035P_sensor_data* sensor_data)
{
    rt_int32_t temp = 0;
    int24_to_int32(buf+1, &temp);
//    rt_kprintf("temp : %x\r\n",temp);
    sensor_data->angular_speed = (float)temp / 117015.375;
}

void VG035P_decode(char* buf, struct rt_VG035P_sensor_data* sensor_data)
{
    rt_int32_t temp = 0;
    int24_to_int32(buf+1, &temp);
    sensor_data->angular_speed = (float)temp * 2500 / 0x800000;

    int24_to_int32(buf+4, &temp);
    sensor_data->angular = (float)temp * 1.00424 / 100;

    int24_to_int32(buf+7, &temp);
    sensor_data->temperature = (float)temp * 250 / 0x800000 - 50;

    sensor_data->index = *(buf+10);
}

void JHT_IA_decode(char* buf, struct rt_JHT_IA_sensor_data* sensor_data)
{
    rt_int32_t temp = 0;
    int24_to_int32(buf+1, &temp);
    sensor_data->acceleration = (float)temp * 2500 / 0x800000;

    int24_to_int32(buf+4, &temp);
    sensor_data->velocity = (float)temp / 100;

    int24_to_int32(buf+7, &temp);
    sensor_data->temperature = (float)temp * 500 / 0x800000 - 273.2;

    sensor_data->index = *(buf+10);
}


rt_err_t uart_rx_check_data(struct rt_uart_rx* uartx_rx, rt_uint8_t ch)
{
	rt_uint8_t i;
    if(uartx_rx->stat == WAIT_BEGIN)      //start one cmd
    {
        if(ch != uartx_rx->rx_begin)    //not begin, wait for begin
            return RT_EOK;

        uartx_rx->stat = WAIT_END;
        uartx_rx->index = 0;
        uartx_rx->rx_buffer[uartx_rx->index] = ch;
        uartx_rx->index ++;
    }
    else if(uartx_rx->stat == WAIT_END)
    {

        uartx_rx->rx_buffer[uartx_rx->index] = ch;
        uartx_rx->index ++;
        if(uartx_rx->index >= (uartx_rx->rx_size))
        {
            if(((uartx_rx->checksum == CHECKSUM)  &&
                (uartx_rx->rx_buffer[uartx_rx->rx_size-1] == crc_sum(uartx_rx->rx_buffer+1, uartx_rx->rx_size-2)))  //need check and check ok
                || (uartx_rx->checksum == NONCHECKSUM))//no need check

            {   //check sum right
                rt_memcpy(uartx_rx->previous_rx_buffer, uartx_rx->rx_buffer, uartx_rx->rx_size);
                rt_sem_release(&uartx_rx->rx_sem);
                uartx_rx->stat = WAIT_BEGIN;
                return RT_EOK;
            }
            {   //check sum wrong
                for(i=1;i<uartx_rx->rx_size;i++)
                {
                    if(uartx_rx->rx_buffer[i] == uartx_rx->rx_begin)
                        break;
                }
                if(i == uartx_rx->rx_size)    //no begin flag
                {
                    uartx_rx->stat = WAIT_BEGIN;
                    uartx_rx->index = 0;
                    return RT_EOK;
                }
                else        //found begin flag
                {
                    rt_memcpy(uartx_rx->rx_buffer, uartx_rx->rx_buffer+i, uartx_rx->rx_size-i);
                    uartx_rx->index -= i;
                }
            }
        }
    }
    return RT_EOK;
}



rt_err_t uart_rx_customer(rt_device_t device, struct rt_uart_rx* uartx_rx)
{
    rt_int16_t i;
    rt_device_t uart;

    uartx_rx->index += rt_device_read(device, 0, uartx_rx->rx_buffer+uartx_rx->index, uartx_rx->rx_size);//RT_SERIAL_RB_BUFSZ);

//    uart = rt_device_find("uart1");
//    rt_device_write(uart, 0, uartx_rx->rx_buffer, uartx_rx->index);
//    rt_kprintf("\r\n%d\r\n",uartx_rx->index);

    if(uartx_rx->index >= uartx_rx->rx_size)        //more than one data
    {
        i = uartx_rx->index - uartx_rx->rx_size;
        for(;i>=0;i--)
        {
            if(uartx_rx->rx_buffer[i] == uartx_rx->rx_begin)                // found rx begin flag
            {
                if(((uartx_rx->checksum == CHECKSUM)  &&
                    (uartx_rx->rx_buffer[i+uartx_rx->rx_size-1] == crc_sum(uartx_rx->rx_buffer+i+1, uartx_rx->rx_size-2)))  //need check and check ok
                    || (uartx_rx->checksum == NONCHECKSUM))//no need check
                {   //check sum right
                    rt_memcpy(uartx_rx->previous_rx_buffer, uartx_rx->rx_buffer+i, uartx_rx->rx_size);
                    rt_memcpy(uartx_rx->rx_buffer, uartx_rx->rx_buffer+i+uartx_rx->rx_size, uartx_rx->index-i-uartx_rx->rx_size);
                    rt_sem_release(&uartx_rx->rx_sem);
                    uartx_rx->index = uartx_rx->index-i-uartx_rx->rx_size;
//                    rt_kprintf("\r\nrx:%d\r\n",uartx_rx->index);
//                    rt_device_write(uart, 0, uartx_rx->rx_buffer, uartx_rx->index);
//                    rt_kprintf("\r\nprx:%d\r\n",uartx_rx->rx_size);
//                    rt_device_write(uart, 0, uartx_rx->previous_rx_buffer, uartx_rx->rx_size);
                    return RT_EOK;
                }
                else
                {
                    rt_memcpy(uartx_rx->rx_buffer, uartx_rx->rx_buffer+i+uartx_rx->rx_size, uartx_rx->index-i-uartx_rx->rx_size);
                    uartx_rx->index = uartx_rx->index-i-uartx_rx->rx_size;
                }
//                rt_kprintf("\r\nrx:%d\r\n",uartx_rx->index);
//                rt_device_write(uart, 0, uartx_rx->rx_buffer, uartx_rx->index);
//                rt_kprintf("\r\nprx:%d\r\n",uartx_rx->rx_size);
//                rt_device_write(uart, 0, uartx_rx->previous_rx_buffer, uartx_rx->rx_size);
                return -RT_ERROR;
            }
        }
//        rt_kprintf("\r\nrx:%d\r\n",uartx_rx->index);
//        rt_device_write(uart, 0, uartx_rx->rx_buffer, uartx_rx->index);
        rt_memcpy(uartx_rx->rx_buffer, uartx_rx->rx_buffer+(uartx_rx->index-uartx_rx->rx_size)+1, uartx_rx->rx_size-1);
        uartx_rx->index = uartx_rx->rx_size;
//        rt_kprintf("\r\nrx:%d\r\n",uartx_rx->index);
//        rt_device_write(uart, 0, uartx_rx->rx_buffer, uartx_rx->index);
    }
    return -RT_ERROR;
}





















