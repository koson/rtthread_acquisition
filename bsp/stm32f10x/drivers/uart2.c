#include "uart2.h"
#include "global.h"
#include "rtthread.h"
#include "stdio.h"

extern struct rt_event uart_all_rx_event;

rt_err_t uart2_input(rt_device_t dev, rt_size_t size)
{
    rt_uint8_t i = 0;
    char ch;
    ch = 0;

    if(rt_device_read(dev, 0, &ch, 1) != 1)
        return -1;

    uart_rx_check_data(&uart2_rx, ch);
//    uart_rx_customer(dev, &uart2_rx);
    return RT_EOK;
}

void uart2_thread_entry(void * parameter)
{
    char s[6] = "uart2";

    rt_device_t device;

    device = rt_device_find("uart2");
    if(device == RT_NULL)
    {
        rt_kprintf("UART2_DATA: can not find device: uart2\n");
        return;
    }

    if(rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("uart2: can not open device: uart2\n");
        return;
    }

//    rt_device_set_rx_indicate(device, uart2_input);
//    rt_device_write(device, 0, uartx_rx->rx_buffer, uartx_rx->index-1);

    while(1)
    {
        rt_sem_take(&uart2_rx.rx_sem, RT_WAITING_FOREVER);

//#ifdef  USING_RX_CRC_SUM
//        if(uartx_rx->previous_rx_buffer[RX_SIZE-2] == crc_sum(uartx_rx->previous_rx_buffer, RX_SIZE-2))
//#endif

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
//        rt_thread_delay(2);
    }
}

void rt_uart2_init(void)
{
    rt_thread_t thread;

    rt_err_t result;

    result = rt_sem_init(&uart2_rx.rx_sem, "uart2_rx_sem", 0, RT_IPC_FLAG_FIFO);

    if(result != RT_EOK)
    {
        rt_kprintf("init message queue failed.\r\n");
        return;
    }

    thread = rt_thread_create("UART2_DATA", uart2_thread_entry, RT_NULL, 1024, 15, 2);
    if(thread != RT_NULL)
        rt_thread_startup(thread);
}
