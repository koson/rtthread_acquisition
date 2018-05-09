#include "cs5532.h"
#include "rtthread.h"
#include "drivers/spi.h"
#include "drivers/pin.h"
#include "rt_stm32f10x_spi.h"

struct rt_semaphore cs5532_sem;

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t cs5532_stack[ 512 ];
static struct rt_thread cs5532_thread;

// static void cs5532_take_cs(struct rt_spi_device *device);
// static void cs5532_release_cs(struct rt_spi_device *device);

// // static void cs5532_transfer(struct rt_spi_device *device, rt_uint8_t *send_buf, rt_uint8_t *recv_buf, rt_size_t size);
// // static void cs5532_write_word(struct rt_spi_device *device, rt_uint32_t write_data);
// // static void cs5532_read_word(struct rt_spi_device *device, rt_uint32_t* read_data);

// static void cs5532_take_cs(struct rt_spi_device *device)
// {
//     struct rt_spi_message message;

//     /* initial message */
//     message.send_buf = RT_NULL;
//     message.recv_buf = RT_NULL;
//     message.length   = 0;
//     message.cs_take  = 1;
//     message.cs_release = 0;

//     /* transfer message */
//     device->bus->ops->xfer(device, &message);
// }

// static void cs5532_release_cs(struct rt_spi_device *device)
// {
//     struct rt_spi_message message;

//     /* initial message */
//     message.send_buf = RT_NULL;
//     message.recv_buf = RT_NULL;
//     message.length   = 0;
//     message.cs_take  = 0;
//     message.cs_release = 1;

//     /* transfer message */
//     device->bus->ops->xfer(device, &message);
// }

 static void cs5532_transfer(struct rt_spi_device *device,
                             rt_uint8_t *send_buf,
                             rt_uint8_t *recv_buf,
                             rt_size_t size)
 {
     rt_err_t result;
     struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
     struct stm32_spi_cs * stm32_spi_cs = device->parent.user_data;

     RT_ASSERT(device != RT_NULL);
     RT_ASSERT(device->bus != RT_NULL);

     result = rt_mutex_take(&(device->bus->lock), RT_WAITING_FOREVER);
     if (result == RT_EOK)
     {
         if (device->bus->owner != device)
         {
             /* not the same owner as current, re-configure SPI bus */
             result = device->bus->ops->configure(device, &device->config);
             if (result == RT_EOK)
             {
                 /* set SPI bus owner */
                 device->bus->owner = device;
             }
             else
             {
                 /* configure SPI bus failed */
                 rt_set_errno(-RT_EIO);
                 result = 0;
                 goto __exit;
             }
         }

         /* transfer message */

         GPIO_ResetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
         while(size--)
         {
             rt_uint8_t data = 0xFF;
             if(send_buf != RT_NULL)
             {
                 data = *send_buf++;
             }

             //Wait until the transmit buffer is empty
             while (SPI_I2S_GetFlagStatus(stm32_spi_bus->SPI, SPI_I2S_FLAG_TXE) == RESET);
             // Send the byte
             SPI_I2S_SendData(stm32_spi_bus->SPI, data);

             //Wait until a data is received
             while (SPI_I2S_GetFlagStatus(stm32_spi_bus->SPI, SPI_I2S_FLAG_RXNE) == RESET);
             // Get the received data
             data = SPI_I2S_ReceiveData(stm32_spi_bus->SPI);

             if(recv_buf != RT_NULL)
             {
                 *recv_buf++ = data;
             }
         }

         GPIO_SetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);

     }
     else
     {
         rt_set_errno(-RT_EIO);

         return;
     }

 __exit:
     rt_mutex_release(&(device->bus->lock));

     return;
 }

uint32_t read_cfg;
uint32_t regdata;
uint32_t Av_cfg;
uint16_t CONV1,CONV2;
float read_Votage;
float read_Distance;
float read_Pressure;

/* Read Word */
//uint32_t  CS5530_Read_Word (SPI_TypeDef* SPIX)
static void cs5532_read_word(struct rt_spi_device *device, rt_uint32_t* read_data)
{
         rt_uint8_t recv_buf[4];


         cs5532_transfer(device, RT_NULL, recv_buf, 4);
         *read_data = (rt_uint32_t)(recv_buf[0]<<24) | (recv_buf[1]<<16) | (recv_buf[2]<<8) | recv_buf[3];
}

/* Write Word */
//void CS5530_Write_Word (SPI_TypeDef* SPIX, uint32_t data)
static void cs5532_write_word(struct rt_spi_device *device, rt_uint32_t write_data)
{
    uint16_t  i;


         rt_uint8_t send_buf[4];


                   for(i=0;i<4;i++)
                   {
                       send_buf[i] = 0xFF & (write_data >> (8*(3-i)));
                   }
                   cs5532_transfer(device, send_buf, RT_NULL, 4);
}

/* Reset Serial Port */
//void CS5530_Serial_Reset(SPI_TypeDef* SPIX )
static void cs5532_serial_reset(struct rt_spi_device *device)
{
    uint16_t i = 31;
    rt_uint8_t send_buf[33];
    for(i=0;i<31;i++)
        send_buf[i] = SYNC1;
    send_buf[31] = SYNC0;
    cs5532_transfer(device, send_buf, RT_NULL, 32);
}

/* System Reset:
Configuration Register: 00000000(H)
Offset Registers: 00000000(H)
Gain Registers: 01000000(H)
Channel Setup Registers: 00000000(H)*/
//void CS5530_System_Reset (SPI_TypeDef* SPIX )
static void cs5532_system_reset(struct rt_spi_device *device)
{

    rt_uint8_t send_buf[4];
    rt_uint32_t read_buf;

    send_buf[0] = CMD_WRITE + REG_CONFIG;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_write_word(device, SYSTEM_RESET) ;//Reset system, configration Register at 0x20000000

    rt_thread_delay(RT_TICK_PER_SECOND/20);

    send_buf[0] = CMD_WRITE + REG_CONFIG;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_write_word(device, NORMAL_MODE );//0x00000000

    send_buf[0] = CMD_READ + REG_CONFIG;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);

    send_buf[0] = CMD_READ + REG_CONFIG;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);

    send_buf[0] = CMD_READ + REG_OFFSET;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);

    send_buf[0] = CMD_READ + REG_GAIN;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);

    send_buf[0] = CMD_READ + REG_CHN_STP;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);
}

//void CS5530_Config (SPI_TypeDef* SPIX)  //3
static void cs5532_config(struct rt_spi_device *device)
{
    rt_uint8_t send_buf[4];
    rt_uint32_t read_buf;

// 	if(SPI1 ==SPIX){
    /* / Gain = 1 / unipolar / REF = 0 */
    //channel1
    send_buf[0] = CMD_WRITE + REG_GAIN;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_write_word(device, 0x01000000);//0x00000000
    send_buf[0] = CMD_READ + REG_GAIN;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);
    rt_kprintf("%x\r\n",read_buf);

    send_buf[0] = CMD_WRITE + REG_CHN_STP;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_write_word(device, UNIPOLAR_MODE+DATARATE_800);//0x00000000
    send_buf[0] = CMD_READ + REG_CHN_STP;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);
    rt_kprintf("%x\r\n",read_buf);

    send_buf[0] = CMD_WRITE + REG_CONFIG;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_write_word(device, VREF_LOW+LINE_FREQ_50);//0x00000000
    send_buf[0] = CMD_READ + REG_CONFIG;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);
    rt_kprintf("%x\r\n",read_buf);

    send_buf[0] = CMD_WRITE + REG_OFFSET;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_write_word(device, 0XFFFD0000);//0x00000000
    rt_thread_delay(RT_TICK_PER_SECOND/10);//Delay_ms(100);
    send_buf[0] = CMD_READ + REG_OFFSET;
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    cs5532_read_word(device, &read_buf);
    rt_kprintf("%x\r\n",read_buf);


//    //channel2
//    send_buf[0] = CMD_WRITE + REG_GAIN + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_write_word(device, 0x01000000);//0x00000000
//    send_buf[0] = CMD_READ + REG_GAIN + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_read_word(device, &read_buf);
//    rt_kprintf("%x\r\n",read_buf);

//    send_buf[0] = CMD_WRITE + REG_CHN_STP + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_write_word(device, UNIPOLAR_MODE+DATARATE_800 + (0x00000001<<30));//0x00000000
//    send_buf[0] = CMD_READ + REG_CHN_STP + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_read_word(device, &read_buf);
//    rt_kprintf("%x\r\n",read_buf);

//    send_buf[0] = CMD_WRITE + REG_CONFIG + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_write_word(device, VREF_LOW+LINE_FREQ_50);//0x00000000
//    send_buf[0] = CMD_READ + REG_CONFIG + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_read_word(device, &read_buf);
//    rt_kprintf("%x\r\n",read_buf);

//    send_buf[0] = CMD_WRITE + REG_OFFSET + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_write_word(device, 0XFFFD0000);//0x00000000
//    rt_thread_delay(RT_TICK_PER_SECOND/10);//Delay_ms(100);
//    send_buf[0] = CMD_READ + REG_OFFSET + channel_1;
//    cs5532_transfer(device, send_buf, RT_NULL, 1);
//    cs5532_read_word(device, &read_buf);
//    rt_kprintf("%x\r\n",read_buf);
}

//float CS5530_Perform ( SPI_TypeDef* SPIX )
float cs5532_perform(struct rt_spi_device *device)
 {
    uint16_t	continuous = 1;
    rt_uint8_t send_buf[4];

    read_cfg = 0;
    Av_cfg = 0;

    if(continuous )
    {
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)); 	//a6
        send_buf[0] = 0x00;
        cs5532_transfer(device, send_buf, RT_NULL, 1);
        cs5532_read_word(device, &regdata);
        Av_cfg = ((regdata)&0xffffffff) ;//AD Result
        read_Votage  = ((Av_cfg) * ((float)5/0xffFFFFFF));
        read_Distance = ((135-(read_Votage*((float)70/5))));  //160401 24.7   27.7
//         rt_kprintf("%x %x %x\r\n",regdata,Av_cfg,a);
//         rt_kprintf("%d \r\n",(uint16_t)(read_Votage*100));
//        rt_kprintf("a");
        return	read_Distance;
    }
    else
    {
        send_buf[0] = 0x00;
        cs5532_transfer(device, send_buf, RT_NULL, 1);
        cs5532_read_word(device, &regdata);
        Av_cfg = ((regdata)>>8) ;//AD Result
        read_Votage  = ((Av_cfg) * ((float)5/0xFFFFFF));
        read_Distance = ((135-(read_Votage*((float)70/5))));  //160401 24.7   27.7
//         rt_kprintf("%x %x\r\n",regdata,Av_cfg);
//         rt_kprintf("%d \r\n",(uint16_t)(read_Votage*100));
        return	read_Distance;
    }
}

#include "global.h"
static void cs5532_thread_entry(void* parameter)
{
    struct rt_spi_device* device;
    rt_uint8_t send_buf[4];

    device = (struct rt_spi_device *)rt_device_find("spi21");
    if (device == RT_NULL)
    {
        rt_kprintf("spi21 %s not found!\r\n");
        return ;
    }

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
        cfg.max_hz = 400 * 1000; /* 400k */
        rt_spi_configure(device, &cfg);
    }

    cs5532_serial_reset(device);
    rt_thread_delay(RT_TICK_PER_SECOND/10);
    cs5532_system_reset( device);
    rt_thread_delay(RT_TICK_PER_SECOND/10);
    cs5532_config (device);
    rt_thread_delay(RT_TICK_PER_SECOND/10);
	
	send_buf[0] = PERFORM_CONVERSION+CONTINUOUS+(0x00<<3);
    cs5532_transfer(device, send_buf, RT_NULL, 1);
    rt_thread_delay(RT_TICK_PER_SECOND / 100);

    rt_sem_init(&cs5532_sem, "cs5532_sem", 0, RT_IPC_FLAG_FIFO);

    while(1)
    {     
        rt_sem_take(&cs5532_sem, RT_WAITING_FOREVER);
        ALL_DATA.laser_distance_sensor_left = (cs5532_perform(device)*100);

//        rt_thread_delay(RT_TICK_PER_SECOND / 200);

//		send_buf[0] = PERFORM_CONVERSION+SINGLE+(0x02<<3);
//        cs5532_transfer(device, send_buf, RT_NULL, 1);
//        rt_thread_delay(RT_TICK_PER_SECOND / 100);
//        ALL_DATA.laser_distance_sensor_right = (cs5532_perform(device)*100);
    }
}

void rt_cs5532_init(void)
{
    rt_err_t result;

    result = rt_thread_init(&cs5532_thread,
                            "cs5532",
                            cs5532_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&cs5532_stack[0],
                             sizeof(cs5532_stack),
                             16,20);

    if(result == RT_EOK)
    {
        rt_thread_startup(&cs5532_thread);
        return ;
    }
    rt_kprintf("cs5532 thread startup failed!");
}


















