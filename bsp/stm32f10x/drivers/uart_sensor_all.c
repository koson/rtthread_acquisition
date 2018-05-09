#include "global.h"
#include "rtthread.h"
#include "stdio.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t uart_sensor_all_stack[ 2048 ];
static struct rt_thread uart_sensor_all_thread;

extern void rt_uart2_init(void);
extern void rt_uart3_init(void);
extern void rt_uart4_init(void);
extern void rt_uart5_init(void);
extern void rt_uart_all_init(void);
extern struct rt_semaphore cs5532_sem;

struct rt_event uart_all_rx_event;
struct rt_uart_rx_sensor_data previous_ALL_DATA =
{
    {0},
    {0},
    {0},
    {0},
     0,
};
struct rt_uart_rx_sensor_data ALL_DATA =
{
    {0},
    {0},
    {0},
    {0},
     0,
};


static void uart_sensor_all_thread_entry(void* parameter)
{
    rt_uint32_t e;
    rt_uint8_t	a=0;
    char str[300],s[6] = "all\n";

    rt_device_t device;
    device = rt_device_find("uart1");
    if(device == RT_NULL)
    {
        rt_kprintf("UART_ALL_DATA: can not find device: uart1\n");
        return;
    }
//    if(rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_DMA_TX) != RT_EOK)
//    {
//        rt_kprintf("uart2: can not open device: uart2\n");
//        return;
//    }

    while(1)
    {
        if(rt_event_recv(&uart_all_rx_event, RX_EVENT, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                      RT_WAITING_FOREVER, &e)== RT_EOK)
        {
            rt_memcpy(&previous_ALL_DATA, &ALL_DATA, sizeof(struct rt_uart_rx_sensor_data));

            a = 0;
#ifdef USING_TXD6_ARB1
//             a = sprintf(str+a,"T0:%f\r\n", previous_ALL_DATA.T0.angular_speed);
//             a += sprintf(str+a,"T1:%f\r\n", previous_ALL_DATA.T1.angular_speed);
//             a += sprintf(str+a,"J0:%f %f %f %d\r\n", previous_ALL_DATA.J0.acceleration, previous_ALL_DATA.J0.velocity, previous_ALL_DATA.J0.temperature, previous_ALL_DATA.J0.index);
//             a += sprintf(str+a,"J1:%f %f %f %d\r\n", previous_ALL_DATA.J1.acceleration, previous_ALL_DATA.J1.velocity, previous_ALL_DATA.J1.temperature, previous_ALL_DATA.J1.index);
//             a += sprintf(str+a,"LL:%f\r\n", previous_ALL_DATA.laser_distance_sensor_left);
#endif
#ifdef USING_VG035P
//            a = sprintf(str+a,"T0:%f %f %f %d\r\n", previous_ALL_DATA.T0.angular_speed, previous_ALL_DATA.T0.angular, previous_ALL_DATA.T0.temperature, previous_ALL_DATA.T0.index);
//            a += sprintf(str+a,"T1:%f %f %f %d\r\n", previous_ALL_DATA.T1.angular_speed, previous_ALL_DATA.T1.angular, previous_ALL_DATA.T1.temperature, previous_ALL_DATA.T1.index);
//            a += sprintf(str+a,"J0:%f %f %f %d\r\n", previous_ALL_DATA.J0.acceleration, previous_ALL_DATA.J0.velocity, previous_ALL_DATA.J0.temperature, previous_ALL_DATA.J0.index);
//            a += sprintf(str+a,"J1:%f %f %f %d\r\n", previous_ALL_DATA.J1.acceleration, previous_ALL_DATA.J1.velocity, previous_ALL_DATA.J1.temperature, previous_ALL_DATA.J1.index);
//            a += sprintf(str+a,"LL:%f\r\nLR:%f\r\n", previous_ALL_DATA.laser_distance_sensor_left, previous_ALL_DATA.laser_distance_sensor_right);
#endif

//            rt_device_write(device, 0, &ALL_DATA, sizeof(struct rt_uart_rx_sensor_data));
//			rt_kprintf("uartall:\r\n");
//            rt_kprintf("uartall:\r\n%s\r\n",str);
//            s[0] = previous_ALL_DATA.T0.index;
//            s[1] = previous_ALL_DATA.T1.index;
//            s[2] = previous_ALL_DATA.J0.index;
//            s[3] = previous_ALL_DATA.J1.index;
//            s[4] = '\r';
//            s[5] = '\n';
//            rt_device_write(device, 0, s, sizeof(s));

           str[0] = 0xDD;
           rt_memcpy(str+1, &previous_ALL_DATA, sizeof(struct rt_uart_rx_sensor_data));
           str[sizeof(struct rt_uart_rx_sensor_data)+1] = '\r';
           str[sizeof(struct rt_uart_rx_sensor_data)+2] = '\n';
           rt_device_write(device, 0, str, sizeof(struct rt_uart_rx_sensor_data)+3);

//             a = sprintf(str+a,"%d %d %d %d ", previous_ALL_DATA.T0.index, previous_ALL_DATA.T1.index, previous_ALL_DATA.J0.index, previous_ALL_DATA.J1.index);
//             a += sprintf(str+a,"%f\r\n", previous_ALL_DATA.laser_distance_sensor_left);
// //            a += sprintf(str+a," %d %d\r\n", a, sizeof(struct rt_uart_rx_sensor_data)+3);

//             rt_device_write(device, 0, str, a);

            if(previous_ALL_DATA.T0.index % 10 == 0)
                rt_sem_release(&cs5532_sem);


        }
    }
}

void rt_uart_sensor_all_init(void)
{
    rt_err_t result;
	
	rt_event_init(&uart_all_rx_event, "all_rx_event", RT_IPC_FLAG_FIFO);
	
    rt_uart2_init();
    rt_uart3_init();
    rt_uart4_init();
    rt_uart5_init();
//    rt_uart_all_init();

    result = rt_thread_init(&uart_sensor_all_thread,
                            "uart_sensor_all",
                            uart_sensor_all_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&uart_sensor_all_stack[0],
                             sizeof(uart_sensor_all_stack),
                             14,2);

    if(result == RT_EOK)
    {
        rt_thread_startup(&uart_sensor_all_thread);
        return;
    }
    rt_kprintf("uart_sensor_all thread startup failed!");
    return;
}



