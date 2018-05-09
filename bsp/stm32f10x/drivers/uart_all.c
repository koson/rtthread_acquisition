#include "uart_all.h"
#include "global.h"
#include "rtthread.h"
#include "stdio.h"

extern struct rt_event uart_all_rx_event;

/***
 * T0 -> UART4
 * T1 -> UART5
 * (T2 -> UART1)
 * J0 -> UART3
 * J1 -> UART2
 * (J2 -> UART6)
 ***/
void uart_all_thread_entry(void * parameter)
{
    char s[6] = "uart2";

    rt_device_t device_uart1,device_uart2,device_uart3,device_uart4,device_uart5,device_uart6;

#ifdef  RT_USING_T0
    device_uart4 = rt_device_find("uart4");
    if(device_uart4 == RT_NULL)
    {
        rt_kprintf("T0: can not find device: uart4\n");
        return;
    }
    if(rt_device_open(device_uart4, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("T0: can not open device: uart4\n");
        return;
    }
#endif

#ifdef  RT_USING_T1
    device_uart5 = rt_device_find("uart5");
    if(device_uart5 == RT_NULL)
    {
        rt_kprintf("T1: can not find device: uart5\n");
        return;
    }
    if(rt_device_open(device_uart5, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("T1: can not open device: uart5\n");
        return;
    }
#endif

#ifdef  RT_USING_T2
    device_uart1 = rt_device_find("uart1");
    if(device_uart1 == RT_NULL)
    {
        rt_kprintf("T2: can not find device: uart1\n");
        return;
    }
    if(rt_device_open(device_uart1, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("T2: can not open device: uart1\n");
        return;
    }
#endif

#ifdef  RT_USING_J0
    device_uart3 = rt_device_find("uart3");
    if(device_uart3 == RT_NULL)
    {
        rt_kprintf("J0: can not find device: uart3\n");
        return;
    }
    if(rt_device_open(device_uart3, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("J0: can not open device: uart3\n");
        return;
    }
#endif

#ifdef  RT_USING_J1
    device_uart2 = rt_device_find("uart2");
    if(device_uart2 == RT_NULL)
    {
        rt_kprintf("J1: can not find device: uart2\n");
        return;
    }
    if(rt_device_open(device_uart2, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("J1: can not open device: uart2\n");
        return;
    }
#endif

#ifdef  RT_USING_J2
    device_uart6 = rt_device_find("uart6");
    if(device_uart6 == RT_NULL)
    {
        rt_kprintf("J2: can not find device: uart6\n");
        return;
    }
    if(rt_device_open(device_uart6, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("J2: can not open device: uart6\n");
        return;
    }
#endif

    while(1)
    {
#ifdef  RT_USING_T0
//        if(0 == uart_rx_customer(device_uart4, &uart4_rx))
        if(RT_EOK == rt_sem_take(&uart4_rx.rx_sem, 0))
        {
#ifdef USING_TXD6_ARB1
            TXD6_ARB1_decode(uart4_rx.previous_rx_buffer, &ALL_DATA.T0);
//            sprintf(s,"%f", ALL_DATA.T0.angular_speed);
#endif
#ifdef USING_VG035P
            VG035P_decode(uart4_rx.previous_rx_buffer, &ALL_DATA.T0);
//            sprintf(s,"%f %f %f %d", ALL_DATA.T0.angular_speed, ALL_DATA.T0.angular, ALL_DATA.T0.temperature, ALL_DATA.T0.index);
#endif
            rt_event_send(&uart_all_rx_event, UART4_RX_EVENT);
//            rt_device_write(device_uart4, 0, uart4_rx.previous_rx_buffer, uart4_rx.rx_size);
//            rt_kprintf("uart4\r\n");
//            rt_kprintf("uart4:%s\r\n",s);
//            device = rt_device_find("uart1");
//            rt_device_write(device, 0, s, sizeof(s));
        }
#endif

#ifdef  RT_USING_T1
//        if(0 == uart_rx_customer(device_uart5, &uart5_rx))
        if(RT_EOK == rt_sem_take(&uart5_rx.rx_sem, 0))
        {
#ifdef USING_TXD6_ARB1
            TXD6_ARB1_decode(uart5_rx.previous_rx_buffer, &ALL_DATA.T1);
//            sprintf(s,"%f", ALL_DATA.T1.angular_speed);
#endif
#ifdef USING_VG035P
            VG035P_decode(uart5_rx.previous_rx_buffer, &ALL_DATA.T1);
//            sprintf(s,"%f %f %f %d", ALL_DATA.T1.angular_speed, ALL_DATA.T1.angular, ALL_DATA.T1.temperature, ALL_DATA.T1.index);
#endif
            rt_event_send(&uart_all_rx_event, UART5_RX_EVENT);
//            rt_device_write(device_uart5, 0, uart5_rx.previous_rx_buffer, uart5_rx.rx_size);
//            rt_kprintf("uart5\r\n");
//            rt_kprintf("uart5:%s\r\n",s);
//            device = rt_device_find("uart1");
//            rt_device_write(device, 0, s, sizeof(s));
        }
#endif

#ifdef  RT_USING_J0
//        if(0 == uart_rx_customer(device_uart3, &uart3_rx))
        if(RT_EOK == rt_sem_take(&uart3_rx.rx_sem, 0))
        {
            JHT_IA_decode(uart3_rx.previous_rx_buffer, &ALL_DATA.J0);
            rt_event_send(&uart_all_rx_event, UART3_RX_EVENT);
//            sprintf(s,"%f %f %f %d", ALL_DATA.J0.acceleration, ALL_DATA.J0.velocity, ALL_DATA.J0.temperature, ALL_DATA.J0.index);
//            rt_device_write(device_uart3, 0, uart3_rx.previous_rx_buffer, uart3_rx.rx_size);
//            rt_kprintf("uart3\r\n");
//            rt_kprintf("uart3:%s\r\n",s);
//            device = rt_device_find("uart1");
//            rt_device_write(device, 0, s, sizeof(s));
        }
#endif

#ifdef  RT_USING_J1
//        if(0 == uart_rx_customer(device_uart2, &uart2_rx))
        if(RT_EOK == rt_sem_take(&uart2_rx.rx_sem, 0))
        {
            JHT_IA_decode(uart2_rx.previous_rx_buffer, &ALL_DATA.J1);
            rt_event_send(&uart_all_rx_event, UART2_RX_EVENT);
//            sprintf(s,"%f %f %f %d", ALL_DATA.J1.acceleration, ALL_DATA.J1.velocity, ALL_DATA.J1.temperature, ALL_DATA.J1.index);
//            rt_device_write(device, 0, uart2_rx.previous_rx_buffer, uart2_rx.rx_size);
//            rt_kprintf("uart2\r\n");
//            rt_kprintf("uart2:%s\r\n",s);
//            device = rt_device_find("uart1");
//            rt_device_write(device, 0, s, sizeof(s));
        }
#endif

        rt_thread_delay(RT_TICK_PER_SECOND/300);
    }
}

void rt_uart_all_init(void)
{
    rt_thread_t thread;

    rt_err_t result;

    result = rt_sem_init(&uart2_rx.rx_sem, "uart2_rx_sem", 0, RT_IPC_FLAG_FIFO);
    result = rt_sem_init(&uart3_rx.rx_sem, "uart3_rx_sem", 0, RT_IPC_FLAG_FIFO);
    result = rt_sem_init(&uart4_rx.rx_sem, "uart4_rx_sem", 0, RT_IPC_FLAG_FIFO);
    result = rt_sem_init(&uart5_rx.rx_sem, "uart5_rx_sem", 0, RT_IPC_FLAG_FIFO);

    thread = rt_thread_create("uart_all_data", uart_all_thread_entry, RT_NULL, 1024, 15, 3);
    if(thread != RT_NULL)
        rt_thread_startup(thread);
}
