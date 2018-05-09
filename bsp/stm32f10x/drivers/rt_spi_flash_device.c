#include <rtthread.h>
#include <board.h>

#ifdef RT_USING_LWIP
#include "stm32_eth.h"
#endif /* RT_USING_LWIP */

#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"
#include "spi_flash_w25qxx.h"

#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
#include "msd.h"
#endif /* RT_USING_DFS */

/*
 * SPI1_MOSI: PA7
 * SPI1_MISO: PA6
 * SPI1_SCK : PA5
 *
 * CS0: PA4  SPI FLASH.
 * CS1: PA2  SD card.
 * SDPOWER:PA0
 * SDDETECT:PA1
*/
#define FLASH_CS_Pin        GPIO_Pin_12
#define FLASH_CS_PORT       GPIOB
#define FLASH_CS_RCC        RCC_APB2Periph_GPIOB

#define CS5532_CS_Pin       GPIO_Pin_15
#define CS5532_CS_PORT      GPIOA
#define CS5532_CS_RCC       RCC_APB2Periph_GPIOA

static int rt_hw_spi_init(void)
{
#ifdef RT_USING_SPI2
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
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

        stm32_spi_register(SPI2, &stm32_spi, "spi2");
    }

    /* attach SPI FLASH cs */
#ifdef RT_USING_SPI_FLASH
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

        /* spi10: PA4 */
        spi_cs.GPIOx = FLASH_CS_PORT;
        spi_cs.GPIO_Pin = FLASH_CS_Pin;
        RCC_APB2PeriphClockCmd(FLASH_CS_RCC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;        
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);

        rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", (void*)&spi_cs);
    }
#endif

    /* attach CS5532 cs */
#ifdef RT_USING_CS5532
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

        /* spi21: PA15 */
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//disable the JTAG,only for A15->JTDI
        spi_cs.GPIOx = CS5532_CS_PORT;
        spi_cs.GPIO_Pin = CS5532_CS_Pin;
        RCC_APB2PeriphClockCmd(CS5532_CS_RCC|RCC_APB2Periph_AFIO, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);
		GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);

        rt_spi_bus_attach_device(&spi_device, "spi21", "spi2", (void*)&spi_cs);
    }
#endif
#endif /* RT_USING_SPI1 */

	return 0;
}
INIT_DEVICE_EXPORT(rt_hw_spi_init);
#endif /* RT_USING_SPI */

void SPI_SD_to_PC(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;   


}

void SPI_PC_to_SD(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;   


}

//#include <dfs_posix.h>
//#define TEST_FN		"/SYSTERM/GKC_1.TXT"
//static char test_data[120],buffer[120];
void rt_dfs_test(void)
{

}

#ifdef	RT_USING_FINSH
#include "finsh.h"
FINSH_FUNCTION_EXPORT(rt_dfs_test, PERFORM FILE READ AND WRITE TEST);
#endif

void rt_spi_flash_device_init(void)
{
#ifdef RT_USING_SPI
//    rt_hw_spi_init();


#if defined(RT_USING_W25QXX)
    w25qxx_init("w25q64","spi20");
#endif

#ifdef RT_USING_CS5532
    /* init sdcard driver */
    {
        extern void rt_cs5532_init(void);

        rt_thread_delay(2);

        rt_cs5532_init();
    }
#endif /* RT_USING_DFS && RT_USING_DFS_ELMFAT */

#endif // RT_USING_SPI

#ifdef RT_USING_LWIP
    /* initialize eth interface */
    rt_hw_stm32_eth_init();
#endif /* RT_USING_LWIP */

}
