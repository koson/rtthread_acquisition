#include "rtthread.h"
#include "spi_flash.h"

uint8_t s[10] = "abcdefghij";
uint8_t a[4096];

void spi_flash_test(void)
{

    rt_spi_flash_device_t rt_spi_flash;
    rt_spi_flash = (rt_spi_flash_device_t)rt_device_find("w25q64");
    rt_kprintf("found w25q64!\r\n");
    if(rt_spi_flash == RT_NULL)
    {
        rt_kprintf("can not found w25q64!\r\n");
    }
	
//     rt_spi_flash->flash_device.open(&(rt_spi_flash->flash_device), RT_DEVICE_FLAG_RDWR);
// 	rt_kprintf("1111111\r\n");
//     rt_spi_flash->flash_device.write(&(rt_spi_flash->flash_device), 0, s, 1);
// 	rt_kprintf("222222\r\n");
//     rt_spi_flash->flash_device.read(&(rt_spi_flash->flash_device), 0, a, 1);
// 	rt_kprintf("333333\r\n");
//     rt_kprintf("%s\r\n",a);
// 	rt_kprintf("4444444\r\n");
}
