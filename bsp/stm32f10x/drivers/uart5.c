#include "uart5.h"
#include "global.h"
#include "rtthread.h"
#include "stdio.h"

extern struct rt_event uart_all_rx_event;

rt_err_t uart5_input(rt_device_t dev, rt_size_t size)
{
    rt_uint8_t i = 0;
    char ch;
    ch = 0;

    if(rt_device_read(dev, 0, &ch, 1) != 1)
        return -1;

    uart_rx_check_data(&uart5_rx, ch);
    return RT_EOK;
}

void uart5_thread_entry(void * parameter)
{
    char s[6] = "uart5";

    rt_device_t device;

    device = rt_device_find("uart5");
    if(device == RT_NULL)
    {
        rt_kprintf("UART5_DATA: can not find device: uart5\n");
        return;
    }

    if(rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("uart5: can not open device: uart5\n");
        return;
    }

//    rt_device_set_rx_indicate(device, uart5_input);
//    rt_device_write(device, 0, uartx_rx->rx_buffer, uartx_rx->index-1);

    while(1)
    {
        rt_sem_take(&uart5_rx.rx_sem, RT_WAITING_FOREVER);

//#ifdef  USING_RX_CRC_SUM
//        if(uartx_rx->previous_rx_buffer[RX_SIZE-2] == crc_sum(uartx_rx->previous_rx_buffer, RX_SIZE-2))
//#endif

        {
#ifdef USING_TXD6_ARB1
            TXD6_ARB1_decode(uart5_rx.previous_rx_buffer, &ALL_DATA.T1);
            sprintf(s,"%f", ALL_DATA.T0.angular_speed);
#endif
#ifdef USING_VG035P
            VG035P_decode(uart5_rx.previous_rx_buffer, &ALL_DATA.T1);
//            sprintf(s,"%f %f %f %d", ALL_DATA.T1.angular_speed, ALL_DATA.T1.angular, ALL_DATA.T1.temperature, ALL_DATA.T1.index);
#endif
            rt_event_send(&uart_all_rx_event, UART5_RX_EVENT);
//            rt_device_write(device, 0, uart5_rx.previous_rx_buffer, uart5_rx.rx_size);
//            rt_kprintf("uart5\r\n");
//            rt_kprintf("uart5:%s\r\n",s);
//            device = rt_device_find("uart1");
//            rt_device_write(device, 0, s, sizeof(s));
        }
//        rt_thread_delay(2);
    }
}

void rt_uart5_init(void)
{
    rt_thread_t thread;

    rt_err_t result;

    result = rt_sem_init(&uart5_rx.rx_sem, "uart5_rx_sem", 0, RT_IPC_FLAG_FIFO);

    if(result != RT_EOK)
    {
        rt_kprintf("init message queue failed.\r\n");
        return;
    }

    thread = rt_thread_create("UART5_DATA", uart5_thread_entry, RT_NULL, 1024, 15, 2);
    if(thread != RT_NULL)
        rt_thread_startup(thread);
}
