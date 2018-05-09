/*
 * File      : fs_test.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2011, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-01-02     aozima       the first version.
 * 2011-03-17     aozima       fix some bug.
 * 2011-03-18     aozima       to dynamic thread.
 */

#include <rtthread.h>
//#include <dfs_posix.h>

static rt_uint32_t stop_flag = 0;
static rt_thread_t fsrw1_thread = RT_NULL;
static rt_thread_t fsrw2_thread = RT_NULL;

#define fsrw1_fn                   "/test1.dat"
#define fsrw1_data_len             120               /* Less than 256 */
static void fsrw1_thread_entry(void* parameter)
{
 
}

#define fsrw2_fn                   "/test2.dat"
#define fsrw2_data_len             180              /* Less than 256 */
static void fsrw2_thread_entry(void* parameter)
{
  
}


/** \brief startup filesystem read/write test(multi thread).
 *
 * \param arg rt_uint32_t [0]startup thread1,[1]startup thread2.
 * \return void
 *
 */
void test(rt_uint32_t arg)
{
    rt_kprintf("arg is : 0x%02X ",arg);

    if(arg & 0x01)
    {
        if( fsrw1_thread != RT_NULL )
        {
            rt_kprintf("fsrw1_thread already exists!\r\n");
        }
        else
        {
            fsrw1_thread = rt_thread_create( "fsrw1",
                                             fsrw1_thread_entry,
                                             RT_NULL,
                                             2048,
                                             RT_THREAD_PRIORITY_MAX-2,
                                             1);
            if ( fsrw1_thread != RT_NULL)
            {
                rt_thread_startup(fsrw1_thread);
            }
        }
    }

    if( arg & 0x02)
    {
        if( fsrw2_thread != RT_NULL )
        {
            rt_kprintf("fsrw2_thread already exists!\r\n");
        }
        else
        {
            fsrw2_thread = rt_thread_create( "fsrw2",
                                             fsrw2_thread_entry,
                                             RT_NULL,
                                             2048,
                                             RT_THREAD_PRIORITY_MAX-2,
                                             1);
            if ( fsrw2_thread != RT_NULL)
            {
                rt_thread_startup(fsrw2_thread);
            }
        }
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(test, file system R/W test. e.g: test(3));
#endif
