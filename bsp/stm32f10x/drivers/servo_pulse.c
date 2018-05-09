#include "servo_pulse.h"
#include <stm32f10x.h>
#include "drivers/pin.h"

//  OA->C7  PIN(38)
#define OA_GPIO_RCC     RCC_APB2Periph_GPIOC
#define OA_GPIO_PORT    GPIOC
#define OA_GPIO_PIN     GPIO_Pin_7

//  OB->C6  PIN(37)
#define OB_GPIO_RCC     RCC_APB2Periph_GPIOC
#define OB_GPIO_PORT    GPIOC
#define OB_GPIO_PIN     GPIO_Pin_6

//  OSW1->C9  PIN(40)
#define OSW1_GPIO_RCC     RCC_APB2Periph_GPIOC
#define OSW1_GPIO_PORT    GPIOC
#define OSW1_GPIO_PIN     GPIO_Pin_9

//  OSW2->C8  PIN(39)
#define OSW2_GPIO_RCC     RCC_APB2Periph_GPIOC
#define OSW2_GPIO_PORT    GPIOC
#define OSW2_GPIO_PIN     GPIO_Pin_8
rt_int8_t oa_pin = 38,ob_pin = 37, osw1_pin = 40, osw2_pin = 39;

void oa_ob_irq(void* parameter);
void osw_irq(void* parameter);

uint32_t servo_pulse = 0;
uint32_t	 subsection = 0;        //number of calibration
uint32_t	oa = 0,ob = 0;
uint8_t calibration_point_flag = 0;       //arrive calibration point
uint32_t sleeper_distance = 0;          //distance from sleeper
uint32_t servo_distance = 0;            //distance from servo pulse
static uint8_t over_first_sleeper_flag = 0;	//already over first sleeper
uint32_t sleeper_number = 0;		//number of sleeper

struct rt_semaphore calibration_point_sem;
extern struct rt_semaphore cs5532_sem;

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t servo_pulse_stack[ 512 ];
static struct rt_thread servo_pulse_thread;

void rt_hw_servo_pulse_init(void)
{

    rt_pin_mode(oa_pin, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(ob_pin, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(osw1_pin, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(osw2_pin, PIN_MODE_INPUT_PULLUP);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);//OA
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);//OB
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);//OSW1
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);//OSW2

    rt_pin_attach_irq(oa_pin, PIN_IRQ_MODE_RISING_FALLING, oa_ob_irq, RT_NULL);
    rt_pin_attach_irq(ob_pin, PIN_IRQ_MODE_RISING_FALLING, oa_ob_irq, RT_NULL);
    rt_pin_attach_irq(osw1_pin, PIN_IRQ_MODE_RISING_FALLING, osw_irq, RT_NULL);
    rt_pin_attach_irq(osw2_pin, PIN_IRQ_MODE_RISING_FALLING, osw_irq, RT_NULL);

    rt_pin_irq_enable(oa_pin, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(ob_pin, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(osw1_pin, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(osw2_pin, PIN_IRQ_ENABLE);
}

void oa_ob_irq(void* parameter)
{
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)	//OA
    {
        servo_pulse++;
        oa++;
//         rt_kprintf("oa\r\n");
        EXTI_ClearITPendingBit(EXTI_Line7);
    }

    if(EXTI_GetITStatus(EXTI_Line6) != RESET)	//OB
    {
        servo_pulse++;
        ob++;
//         rt_kprintf("ob\r\n");
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if((servo_pulse >= (CALIBRATION_PULES+1)*2) && over_first_sleeper_flag)	//go throw 0.125m
    {
        calibration_point_flag = 1;
        rt_sem_release(&cs5532_sem);
        calibration_point_flag = 0;
        subsection ++;
        if(subsection > 1)	//calculate the distance from servo pulse
        {
            servo_distance += DISTANCE_BETWEEN_CALIBRATIONS;
        }
        servo_pulse = 0;
    }
}

void osw_irq(void* parameter)
{
    static uint8_t sleeper_check=1;	//check on sleeper
    uint8_t	osw1 = 0,osw2 = 0;	//osw status

    /* osw status*/
    osw1 = rt_pin_read(osw1_pin);
    osw2 = rt_pin_read(osw2_pin);

   if(EXTI_GetITStatus(EXTI_Line9) != RESET)
   {
       EXTI_ClearITPendingBit(EXTI_Line9);
//         rt_kprintf("osw1:%d\r\n",osw1);
   }

    if(EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line8);
//         rt_kprintf("osw2:%d\r\n",osw2);
    }

    if(sleeper_check && osw1 && osw2)	//there is a sleeper
    {
        sleeper_check = 0;
        over_first_sleeper_flag = 1;	//over first sleeper
       sleeper_number++;	//sleeper number ++
        if(sleeper_number>1)	//check distance from second sleeper
        {
            sleeper_distance += DISTANCE_BETWEEN_SLEEPERS;

            if((sleeper_distance > (((subsection-1)*DISTANCE_BETWEEN_CALIBRATIONS) + (servo_pulse*DISTANCE_BETWEEN_CALIBRATIONS/2/CALIBRATION_PULES) + DISTANCE_ERROR_RANGE))
                || (sleeper_distance < (((subsection-1)*DISTANCE_BETWEEN_CALIBRATIONS) + (servo_pulse*DISTANCE_BETWEEN_CALIBRATIONS/2/CALIBRATION_PULES) - DISTANCE_ERROR_RANGE)))	//error overrun
            {
                subsection = sleeper_distance/DISTANCE_BETWEEN_CALIBRATIONS;
                if(sleeper_distance%DISTANCE_BETWEEN_CALIBRATIONS != 0)	subsection++;
            }
        }
    }

    if((!sleeper_check) && (!osw1) && (!osw2))	//leave sleeper
    {
        sleeper_check = 1;
    }
}

void servo_pulse_thread_entry(void * parameter)
{
    rt_hw_servo_pulse_init();
    while(1)
    {
//         rt_kprintf("oa:%d,ob:%d,servo_pulse:%d,calibration_point_flag:%d,subsection:%d,servo_distance:%d,sleeper_distance:%d,over_first_sleeper_flag:%d\r\n",
//                    oa,ob,servo_pulse,calibration_point_flag,subsection,servo_distance,sleeper_distance,over_first_sleeper_flag);
        rt_thread_delay(RT_TICK_PER_SECOND *2);
    }
}

void rt_servo_pulse_init(void)
{
    rt_err_t result;

    result = rt_sem_init(&calibration_point_sem, "calibration_point_sem", 0, RT_IPC_FLAG_FIFO);
    if(result != RT_EOK)
    {
        rt_kprintf("init calibration point sem failed.\r\n");
        return;
    }

    result = rt_thread_init(&servo_pulse_thread,
                            "servo_pulse",
                            servo_pulse_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&servo_pulse_stack[0],
                             sizeof(servo_pulse_stack),
                             16,2);
    if(result == RT_EOK)
    {
        rt_thread_startup(&servo_pulse_thread);
    }
}
