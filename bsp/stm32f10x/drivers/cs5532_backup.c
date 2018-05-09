#include "cs5532.h"
#include "rtthread.h"
#include "drivers/spi.h"
#include "drivers/pin.h"
#include "rt_stm32f10x_spi.h"

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

// static void cs5532_transfer(struct rt_spi_device *device,
//                             rt_uint8_t *send_buf,
//                             rt_uint8_t *recv_buf,
//                             rt_size_t size)
// {
//     rt_err_t result;
//     struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
//     struct stm32_spi_cs * stm32_spi_cs = device->parent.user_data;

//     RT_ASSERT(device != RT_NULL);
//     RT_ASSERT(device->bus != RT_NULL);

//     result = rt_mutex_take(&(device->bus->lock), RT_WAITING_FOREVER);
//     if (result == RT_EOK)
//     {
//         if (device->bus->owner != device)
//         {
//             /* not the same owner as current, re-configure SPI bus */
//             result = device->bus->ops->configure(device, &device->config);
//             if (result == RT_EOK)
//             {
//                 /* set SPI bus owner */
//                 device->bus->owner = device;
//             }
//             else
//             {
//                 /* configure SPI bus failed */
//                 rt_set_errno(-RT_EIO);
//                 result = 0;
//                 goto __exit;
//             }
//         }

//         /* transfer message */

//         GPIO_ResetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
//         while(size--)
//         {
//             rt_uint8_t data = 0xFF;
//             if(send_buf != RT_NULL)
//             {
//                 data = *send_buf++;
//             }

//             //Wait until the transmit buffer is empty
//             while (SPI_I2S_GetFlagStatus(stm32_spi_bus->SPI, SPI_I2S_FLAG_TXE) == RESET);
//             // Send the byte
//             SPI_I2S_SendData(stm32_spi_bus->SPI, data);

//             //Wait until a data is received
//             while (SPI_I2S_GetFlagStatus(stm32_spi_bus->SPI, SPI_I2S_FLAG_RXNE) == RESET);
//             // Get the received data
//             data = SPI_I2S_ReceiveData(stm32_spi_bus->SPI);

//             if(recv_buf != RT_NULL)
//             {
//                 *recv_buf++ = data;
//             }
//         }

//         GPIO_SetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);

//     }
//     else
//     {
//         rt_set_errno(-RT_EIO);

//         return;
//     }

// __exit:
//     rt_mutex_release(&(device->bus->lock));

//     return;
// }

// uint16_t SPIX_Basic_RW(SPI_TypeDef* SPIX, uint16_t Tdata)
// {
//     uint16_t Rdata = 0;
//
//     while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
//     SPI_I2S_SendData(SPI2, Tdata);//TXD
//     while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
//     Rdata = SPI_I2S_ReceiveData(SPI2);//RXD

//     return(Rdata);//RXD
// }

// //static void cs5532_read_word(struct rt_spi_device *device, rt_uint32_t* read_data)
// rt_uint32_t cs5532_read_word(void)
// {
// //    rt_uint8_t recv_buf[4];

// //    cs5532_transfer(device, RT_NULL, recv_buf, 4);
// //    *read_data = (rt_uint32_t)(recv_buf[0]<<24) | (recv_buf[1]<<16) | (recv_buf[2]<<8) | recv_buf[0];

//     uint16_t i;
//     uint32_t R_data = 0;

//     for( i = 4; i > 0; i-- ){
//         R_data |= SPIX_Basic_RW(SPI2, NULL_BYTE);

//         if(i > 1)R_data <<= 8;
//     }

//     return (R_data);
// }

// //static void cs5532_write_word(struct rt_spi_device *device, rt_uint32_t write_data)
// void cs5532_write_word(rt_uint32_t data)
// {
// //    rt_uint8_t i;
// //    rt_uint8_t send_buf[4];

// //    for(i=0;i<4;i++)
// //    {
// //        send_buf[i] = 0xFF & (write_data >> (8*(3-i)));
// //    }
// //    rt_spi_transfer(device, send_buf, RT_NULL, 4);

//     uint16_t  i;
//     typedef union
//     {
//         uint8_t ch[4];
//         uint32_t lon;
//     } W_type ;
//     W_type temp = {0};
//     temp.lon = data;
// GPIO_ResetBits(GPIOA, GPIO_Pin_15);
//     for( i = 4; i > 0; i-- ){
//         SPIX_Basic_RW(SPI2, temp.ch[i-1]);
//     }
// }

// static void cs5532_serial_reset(struct rt_spi_device *device)
// {
// //    rt_uint8_t i;
// //    rt_uint8_t send_buf[33];
// //    for(i=0;i<31;i++)
// //        send_buf[i] = SYNC1;
// //    send_buf[32] = SYNC0;

// //    rt_spi_transfer(device, send_buf, RT_NULL, 32);
//     uint16_t i = 31;

//     while ( i-- ){
//         SPIX_Basic_RW (SPI2, SYNC1);
//     }
//     SPIX_Basic_RW (SPI2, SYNC0);
// }

// static void cs5532_system_reset(struct rt_spi_device *device)
// {
// //    rt_uint8_t send_buf[4],read_buf[4];

// //    send_buf[0] = CMD_WRITE + REG_CONFIG;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 1);
// //    cs5532_write_word(device, SYSTEM_RESET);

// //    rt_thread_delay(RT_TICK_PER_SECOND / 20);

// //    send_buf[0] = CMD_WRITE + REG_CONFIG;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 1);
// //    cs5532_write_word(device, NORMAL_MODE);

// //    send_buf[0] = CMD_READ + REG_CONFIG;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_read_word(device, (rt_uint32_t*)read_buf);

// //    send_buf[0] = CMD_WRITE + REG_OFFSET;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_write_word(device, SYSTEM_RESET);

// //    send_buf[0] = CMD_WRITE + REG_GAIN;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_write_word(device, SYSTEM_RESET);

// //    send_buf[0] = CMD_WRITE + REG_CHN_STP;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_write_word(device, SYSTEM_RESET);
//     SPIX_Basic_RW (SPI2, CMD_WRITE + REG_CONFIG);
//     cs5532_write_word ( SYSTEM_RESET) ;//Reset system, configration Register at 0x20000000

//     rt_thread_delay(RT_TICK_PER_SECOND / 20); //Delay for 1ms

//     SPIX_Basic_RW (SPI2, CMD_WRITE + REG_CONFIG);//CMD
//     cs5532_write_word (NORMAL_MODE );//0x00000000

//     SPIX_Basic_RW(SPI2, CMD_READ + REG_CONFIG);//CMD: Read Configration Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     cs5532_read_word ( );   //0x10000000

//     SPIX_Basic_RW(SPI2, CMD_READ + REG_CONFIG);//CMD: Clear RV bit
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     cs5532_read_word ( );   //0x00000000
//     SPIX_Basic_RW(SPI2, CMD_READ + REG_OFFSET);//CMD: Read Offset Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     cs5532_read_word ( );   //0x00000000
//     SPIX_Basic_RW(SPI2, CMD_READ + REG_GAIN);//CMD: Read Gain Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     cs5532_read_word ( );   //0x01000000
//     SPIX_Basic_RW(SPI2, CMD_READ + REG_CHN_STP);//CMD: Read Channel-Setup Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     cs5532_read_word ( );   //0x00000000
// }

// static void cs5532_config(struct rt_spi_device *device)
// {
// //    rt_uint8_t send_buf[4];
// //    rt_uint32_t read_buf;

// //    /* / Gain = 1 / unipolar / REF = 0 */
// //    send_buf[0] = CMD_WRITE + REG_GAIN;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 1);
// //    cs5532_write_word(device, 0x01000000);

// //    send_buf[0] = CMD_READ + REG_GAIN;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_read_word(device, &read_buf);
// //    rt_kprintf("REG_GAIN:%x\r\n",read_buf);

// //    /* REG_CHN_STP */
// //    send_buf[0] = CMD_WRITE + REG_CHN_STP;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 1);
// //    cs5532_write_word(device, UNIPOLAR_MODE+DATARATE_200);

// //    send_buf[0] = CMD_READ + REG_CHN_STP;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_read_word(device, &read_buf);
// //    rt_kprintf("REG_CHN_STP:%x\r\n",read_buf);

// //    /* REG_CONFIG */
// //    send_buf[0] = CMD_WRITE + REG_CONFIG;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 1);
// //    cs5532_write_word(device, VREF_LOW+LINE_FREQ_50);

// //    send_buf[0] = CMD_READ + REG_CONFIG;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_read_word(device, &read_buf);
// //    rt_kprintf("REG_CONFIG:%x\r\n",read_buf);

// //    /* REG_OFFSET */
// //    send_buf[0] = CMD_WRITE + REG_OFFSET;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 1);
// //    cs5532_write_word(device, 0xFFFD0000);

// //    send_buf[0] = CMD_READ + REG_OFFSET;
// //    rt_spi_transfer(device, send_buf, RT_NULL, 2);
// //    cs5532_read_word(device, &read_buf);
// //    rt_kprintf("REG_OFFSET:%x\r\n",read_buf);
//     SPIX_Basic_RW (SPI2, CMD_WRITE + REG_GAIN );
//     cs5532_write_word ( 0x01000000);//0x00000000
//     SPIX_Basic_RW(SPI2, CMD_READ + REG_GAIN);//CMD: Read Configration Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     rt_kprintf("%x\r\n",cs5532_read_word ( ));
//     cs5532_read_word ( );   //0x00800000


//     SPIX_Basic_RW (SPI2, CMD_WRITE + REG_CHN_STP );
//     cs5532_write_word (UNIPOLAR_MODE+DATARATE_200);//0x00400000
//     SPIX_Basic_RW(SPI2, CMD_READ+REG_CHN_STP);//CMD: Read Configration Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     rt_kprintf("%x\r\n",cs5532_read_word ( ));
//     cs5532_read_word ( );   //0x00400000

//     SPIX_Basic_RW (SPI2, CMD_WRITE + REG_CONFIG );
//     cs5532_write_word (VREF_LOW+LINE_FREQ_50);//0x00000000
//     SPIX_Basic_RW(SPI2, CMD_READ+REG_CONFIG);//CMD: Read Configration Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     rt_kprintf("%x\r\n",cs5532_read_word ( ));
//     cs5532_read_word ();   //0x00000000

//     SPIX_Basic_RW (SPI2, CMD_WRITE + REG_OFFSET );
//     cs5532_write_word (0XFFFD0000);// 5v	  0XFFF9AA00
//     rt_thread_delay(RT_TICK_PER_SECOND / 10);
//     SPIX_Basic_RW(SPI2, CMD_READ+REG_OFFSET);//CMD: Read Configration Register
//     SPIX_Basic_RW (SPI2, NULL_BYTE) ;//Wait for 8 MCLK
//     rt_kprintf("%x\r\n",cs5532_read_word ( ));
//     cs5532_read_word ();   //0x00000000
// }

// uint32_t read_cfg;
// uint32_t regdata;
// uint32_t Av_cfg;
// uint16_t CONV1,CONV2;
// float read_Votage;
// float read_Distance;
// float read_Pressure;
// float cs5532_perform(struct rt_spi_device *device)
// {
// //    rt_uint16_t continuous = 1;     //0:single,1:continuous
// //    rt_uint8_t send_buf[4],read_buf[4];
// //    rt_uint32_t ad_data = 0;

// //    read_cfg = 0;
// //    Av_cfg = 0;

// //    if(continuous)
// //    {
// //        //spi2_miso->b14

// //        //while(rt_pin_read(35));
// //        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14));

// //        send_buf[0] = 0xFF;
// //        rt_spi_transfer(device, send_buf, RT_NULL, 1);

// //        cs5532_read_word(device, &ad_data);

// //        read_Votage = ((ad_data & 0xffffff) * ((float)5/0xffffff));
// //        read_Distance = (135-(read_Votage * (float)70/5));
// //        rt_kprintf("5532:%x\r\n", ad_data);
// //        return read_Distance;
// //    }
// //    else
// //    {		//single
// //        send_buf[0] = PERFORM_CONVERSION+SINGLE;
// //        rt_spi_transfer(device, send_buf, RT_NULL, 1);
// //        rt_thread_delay(RT_TICK_PER_SECOND/200);
// //        send_buf[0] = 0xFF;
// //        rt_spi_transfer(device, send_buf, RT_NULL, 1);//Wait for 8 MCLK
// //        rt_spi_transfer(device, send_buf, RT_NULL, 1);//Wait for 8 MCLK

// //        cs5532_read_word(device, &ad_data);
// //        Av_cfg = (ad_data>>8) ;//AD Result
// //        read_Votage  = ((Av_cfg) * ((float)5/0xFFFFFF));
// //        read_Distance = ((135-(read_Votage*((float)70/5))));  //160401 24.7   27.7

// //        return	read_Distance;
// //    }

//     uint16_t	continuous = 1;		//0,µ¥´Î×ª»»;1,Á¬Ðø×ª»»
//     read_cfg = 0;
//     Av_cfg = 0;
// //	regdata = 0;
//     if(continuous ){		//Á¬Ðø×ª»»

//             while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)); 	//a6

//             SPIX_Basic_RW (SPI2, NULL_BYTE);//Wait for 8 MCLK
// //			SPIX_Basic_RW (SPIX, NULL_BYTE);//Wait for 8 MCLK

// // printf("%x\r\n",cs5532_read_word (SPIX) & 0xffffff);
//             Av_cfg = ((regdata = cs5532_read_word()) & 0xffffff) ;//AD Result
//         CONV2 = Av_cfg>>8;

//             read_Votage  = ((Av_cfg) * ((float)5/0xFFFFFF));
//             read_Distance = ((135-(read_Votage*((float)70/5))));  //160401 24.7   27.7
// 			rt_kprintf("5532:%x\r\n", Av_cfg);
//             return	read_Distance;
//     }
//         else{		//µ¥´Î×ª»»
//             SPIX_Basic_RW (SPI2, PERFORM_CONVERSION+SINGLE);
//             rt_thread_delay(RT_TICK_PER_SECOND / 200);
//             SPIX_Basic_RW (SPI2, NULL_BYTE);//Wait for 8 MCLK
//             SPIX_Basic_RW (SPI2, NULL_BYTE);//Wait for 8 MCLK

//             Av_cfg = ((regdata = cs5532_read_word ())>>8) ;//AD Result
//             read_Votage  = ((Av_cfg) * ((float)5/0xFFFFFF));
//             read_Distance = ((135-(read_Votage*((float)70/5))));  //160401 24.7   27.7
//             return	read_Distance;
//         }



// }

uint32_t read_cfg;
uint32_t regdata;
uint32_t Av_cfg;
uint16_t CONV1,CONV2;
float read_Votage;
float read_Distance;
float read_Pressure;

extern void Delay_10us( float nCount);
extern void Delay_ms(float nCount);
/* SPI_Basic_R&W */
uint16_t SPIX_Basic_RW(SPI_TypeDef* SPIX, uint16_t Tdata)
{
    uint16_t Rdata = 0;
//	SPI_I2S_ReceiveData(SPIX);
    while(SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET);
//	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    SPI_I2S_SendData(SPIX, Tdata);//TXD
    Rdata = 0;
// 	GPIO_SetBits(GPIOA, GPIO_Pin_4);
    while(SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE) == RESET);
    Rdata = SPI_I2S_ReceiveData(SPIX);//RXD

//     while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
// 	USART_SendData( USART1, Rdata);
    return(Rdata);//RXD
}

/* Read Word */
uint32_t  CS5530_Read_Word (SPI_TypeDef* SPIX)
{
    uint16_t i;
    uint32_t R_data = 0;
GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    for( i = 4; i > 0; i-- ){
        R_data |= SPIX_Basic_RW(SPIX, NULL_BYTE);

        if(i > 1)R_data <<= 8;
    }

    return (R_data);
}

/* Write Word */
void CS5530_Write_Word (SPI_TypeDef* SPIX, uint32_t data)
{
    uint16_t  i;
    typedef union
    {
        uint8_t ch[4];
        uint32_t lon;
    } W_type ;
    W_type temp = {0};
    temp.lon = data;
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    for( i = 4; i > 0; i-- ){
        SPIX_Basic_RW(SPIX, temp.ch[i-1]);
    }
}

/* Reset Serial Port */
void CS5530_Serial_Reset(SPI_TypeDef* SPIX )
{
    uint16_t i = 31;

    while ( i-- ){
        SPIX_Basic_RW (SPIX, SYNC1);
    }
    SPIX_Basic_RW (SPIX, SYNC0);
}

/* System Reset:
Configuration Register: 00000000(H)
Offset Registers: 00000000(H)
Gain Registers: 01000000(H)
Channel Setup Registers: 00000000(H)*/
void CS5530_System_Reset (SPI_TypeDef* SPIX )
{
    SPIX_Basic_RW (SPIX, CMD_WRITE + REG_CONFIG);
    CS5530_Write_Word (SPIX, SYSTEM_RESET) ;//Reset system, configration Register at 0x20000000

    rt_thread_delay(RT_TICK_PER_SECOND/20);
//	Delay_ms(50); //Delay for 1ms

    SPIX_Basic_RW (SPIX, CMD_WRITE + REG_CONFIG);//CMD
    CS5530_Write_Word (SPIX, NORMAL_MODE );//0x00000000

    SPIX_Basic_RW(SPIX, CMD_READ + REG_CONFIG);//CMD: Read Configration Register
    SPIX_Basic_RW (SPIX, NULL_BYTE) ;//Wait for 8 MCLK
    CS5530_Read_Word (SPIX );   //0x10000000

    SPIX_Basic_RW(SPIX, CMD_READ + REG_CONFIG);//CMD: Clear RV bit
    SPIX_Basic_RW (SPIX, NULL_BYTE) ;//Wait for 8 MCLK
    CS5530_Read_Word (SPIX );   //0x00000000
    SPIX_Basic_RW(SPIX, CMD_READ + REG_OFFSET);//CMD: Read Offset Register
    SPIX_Basic_RW (SPIX, NULL_BYTE) ;//Wait for 8 MCLK
    CS5530_Read_Word (SPIX );   //0x00000000
    SPIX_Basic_RW(SPIX, CMD_READ + REG_GAIN);//CMD: Read Gain Register
    SPIX_Basic_RW (SPIX, NULL_BYTE) ;//Wait for 8 MCLK
    CS5530_Read_Word (SPIX );   //0x01000000
    SPIX_Basic_RW(SPIX, CMD_READ + REG_CHN_STP);//CMD: Read Channel-Setup Register
    SPIX_Basic_RW (SPIX, NULL_BYTE) ;//Wait for 8 MCLK
    CS5530_Read_Word (SPIX );   //0x00000000
}

void CS5530_Config (SPI_TypeDef* SPIX)  //3
{

// 	if(SPI1 ==SPIX){
    /* / Gain = 1 / unipolar / REF = 0 */
    //channel1
        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_GAIN );
        CS5530_Write_Word (SPIX, 0x01000000);//0x00000000
        SPIX_Basic_RW(SPIX, CMD_READ + REG_GAIN);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX );   //0x00800000


        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_CHN_STP );
        CS5530_Write_Word (SPIX,UNIPOLAR_MODE+DATARATE_800);//0x00400000
        SPIX_Basic_RW(SPIX, CMD_READ+REG_CHN_STP);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX );   //0x00400000

        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_CONFIG );
        CS5530_Write_Word (SPIX,VREF_LOW+LINE_FREQ_50);//0x00000000
        SPIX_Basic_RW(SPIX, CMD_READ+REG_CONFIG);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX);   //0x00000000

        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_OFFSET );
        CS5530_Write_Word (SPIX,0XFFFD0000);// 5v	  0XFFF9AA00
        rt_thread_delay(RT_TICK_PER_SECOND/10);//Delay_ms(100);
        SPIX_Basic_RW(SPIX, CMD_READ+REG_OFFSET);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX);   //0x00000000

        //channel2
        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_GAIN + channel_1);
        CS5530_Write_Word (SPIX, 0x01000000);//0x00000000
        SPIX_Basic_RW(SPIX, CMD_READ + REG_GAIN +channel_1);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX );   //0x00800000


        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_CHN_STP + channel_1);
        CS5530_Write_Word (SPIX,UNIPOLAR_MODE+DATARATE_800 + (0x00000001<<30));//0x00400000	channel_1:(01<<30)
        SPIX_Basic_RW(SPIX, CMD_READ+REG_CHN_STP + channel_1);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX );   //0x00400000

        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_CONFIG + channel_1);
        CS5530_Write_Word (SPIX,VREF_LOW+LINE_FREQ_50);//0x00000000
        SPIX_Basic_RW(SPIX, CMD_READ+REG_CONFIG + channel_1);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX);   //0x00000000

        SPIX_Basic_RW (SPIX, CMD_WRITE + REG_OFFSET + channel_1);
        CS5530_Write_Word (SPIX,0XFFFD0000);// 5v	  0XFFF9AA00
        rt_thread_delay(RT_TICK_PER_SECOND/20);//Delay_ms(100);
        SPIX_Basic_RW(SPIX, CMD_READ+REG_OFFSET + channel_1);//CMD: Read Configration Register
        rt_kprintf("%x\r\n",CS5530_Read_Word (SPIX ));
        CS5530_Read_Word (SPIX);   //0x00000000
//	}
}

float CS5530_Perform ( SPI_TypeDef* SPIX )
 {
    uint16_t	continuous = 0;		//0,µ¥´Î×ª»»;1,Á¬Ðø×ª»»
    uint8_t a=0;
    static uint8_t b = 0;
    read_cfg = 0;
    Av_cfg = 0;
//	regdata = 0;
    if(continuous ){		//Á¬Ðø×ª»»
        if(SPI1 == SPIX)		//SPI1
        {
            while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)); 	//a6
        }
        else
        {
            while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)); 	//a6
        }
            SPIX_Basic_RW (SPIX, NULL_BYTE);//Wait for 8 MCLK
            regdata = CS5530_Read_Word(SPIX);

        Av_cfg = ((regdata)&0xffffffff) ;//AD Result


            read_Votage  = ((Av_cfg) * ((float)5/0xffFFFFFF));
            read_Distance = ((135-(read_Votage*((float)70/5))));  //160401 24.7   27.7
            rt_kprintf("%x %x %x\r\n",regdata,Av_cfg,a);
            rt_kprintf("%d \r\n",(uint16_t)(read_Votage*100));
            return	read_Distance;
    }
        else{		//µ¥´Î×ª»»

//        rt_thread_delay(500);
//             if(SPI1 == SPIX)		//SPI1
//         {
//             while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)); 	//a6
//         }
//         else
//         {
//             while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)); 	//a6
//         }
            SPIX_Basic_RW (SPIX, NULL_BYTE);//Wait for 8 MCLK

            Av_cfg = ((regdata = CS5530_Read_Word (SPIX))>>8) ;//AD Result
            read_Votage  = ((Av_cfg) * ((float)5/0xFFFFFF));
            read_Distance = ((135-(read_Votage*((float)70/5))));  //160401 24.7   27.7
            rt_kprintf("%x %x\r\n",regdata,Av_cfg);
            rt_kprintf("%d \r\n",(uint16_t)(read_Votage*100));
            return	read_Distance;
        }
}

static void cs5532_thread_entry(void* parameter)
{
//     struct rt_spi_device* device;
//     rt_uint8_t send_buf[4],read_buf[4];

//     device = (struct rt_spi_device *)rt_device_find("spi21");
//     if (device == RT_NULL)
//     {
//         rt_kprintf("spi21 %s not found!\r\n");
//         return ;
//     }

//     /* config spi */
//     {
//         struct rt_spi_configuration cfg;
//         cfg.data_width = 8;
//         cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
//         cfg.max_hz = 400 * 1000; /* 400k */
//         rt_spi_configure(device, &cfg);
//     }

//     cs5532_serial_reset(device);
//     rt_thread_delay(RT_TICK_PER_SECOND/10);
//     cs5532_system_reset(device);
//     rt_thread_delay(RT_TICK_PER_SECOND/10);
//     cs5532_config(device);
//     rt_thread_delay(RT_TICK_PER_SECOND/10);



//     /* / Gain = 1 / unipolar / REF = 0 */
//     send_buf[0] = PERFORM_CONVERSION+CONTINUOUS;
//     rt_spi_transfer(device, send_buf, RT_NULL, 1);

//     rt_thread_delay(RT_TICK_PER_SECOND/10);
SPI_InitTypeDef SPI_InitStructure;

       GPIO_InitTypeDef GPIO_InitStructure;

               /* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,
        ENABLE);

        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);


        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

        /* spi21: PA15 */
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//disable the JTAG,only for A15->JTDI
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_ResetBits(GPIOA, GPIO_Pin_15);




    SPI_I2S_DeInit( SPI2 );


    GPIO_ResetBits(GPIOA, GPIO_Pin_15);

    SPI_StructInit(&SPI_InitStructure);

    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);

    CS5530_Serial_Reset( SPI2 );
    rt_thread_delay(RT_TICK_PER_SECOND/10);
    CS5530_System_Reset( SPI2);
    rt_thread_delay(RT_TICK_PER_SECOND/10);
    CS5530_Config ( SPI2);
    rt_thread_delay(RT_TICK_PER_SECOND/10);

//	SPIX_Basic_RW (SPI2, PERFORM_CONVERSION+CONTINUOUS);
    rt_kprintf("111111\r\n");
    while(1)
    {
        SPIX_Basic_RW (SPI2, PERFORM_CONVERSION+SINGLE+(0x02<<3));
        rt_thread_delay(RT_TICK_PER_SECOND / 50);
        (uint16_t)(CS5530_Perform( SPI2)*100);
        rt_thread_delay(RT_TICK_PER_SECOND / 1);
        SPIX_Basic_RW (SPI2, PERFORM_CONVERSION+SINGLE+(0x00<<3));
        rt_thread_delay(RT_TICK_PER_SECOND / 50);
        (uint16_t)(CS5530_Perform( SPI2)*100);
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
                             18,35);

    if(result == RT_EOK)
    {
        rt_thread_startup(&cs5532_thread);
        return ;
    }
    rt_kprintf("cs5532 thread startup failed!");
}


















