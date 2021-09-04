#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "drv_spi_templates.h"

#define SPI_TODSP_USE_DEBUG
#undef UDEBUG            
#ifdef SPI_TODSP_USE_DEBUG
#define UDEBUG(fmt,args...) printk(KERN_DEBUG "scull: " fmt, ## args)
#else
#define UDEBUG(fmt, args...) /* not debugging: nothing */
#endif 



#define DEVICE_NAME ("drv_spi_templates")

struct drv_spi_templates_dev {
    dev_t               devid;          // 设备号
    struct cdev         cdev;           // CDEV
    struct class        *class;         // 类
    struct device       *device;        // 设备
    struct device_node  *nd;            // 设备节点
    int                 major;          // 主设备号
    void                *private_data;  // 私有数据
    int                 cs_gpio;        // 片选所使用的GPIO编号
};

static struct drv_spi_templates_dev sg_spitemplates_t;

/************************************************************
*
* Function name	: drv_spi_templates_open
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_spi_templates_open(struct inode *inode, struct file *filp)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_spi_templates_read
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static ssize_t drv_spi_templates_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_spi_templates_write
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static ssize_t drv_spi_templates_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *off)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_spi_templates_release
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_spi_templates_release(struct inode *inode, struct file *filp)
{

    return 0;
}



static const struct file_operations drv_spi_templates_ops = {
    .owner   = THIS_MODULE,
    .open    = drv_spi_templates_open,
    .read    = drv_spi_templates_read,
    .write   = drv_spi_templates_write,
    .release = drv_spi_templates_release,
};



static const struct spi_device_id drv_spi_templates_id[] = {
    {"ti,drv_spi_templates",0},
    {}
};



/* 设备树匹配列表 */
static const struct of_device_id drv_spi_templates_of_match[] = {
    { .compatible = "ti,drv_spi_templates"},
    { /* Sentinel */ }
};

static int drv_spi_templates_probe(struct spi_device *spi)
{


    /* 1.构建设备号 */
    printk("drv_spi_templates probe \r\n");
    if(sg_spitemplates_t.major)
    {
        sg_spitemplates_t.devid = MKDEV(sg_spitemplates_t.major,0); // 静态
        register_chrdev_region(sg_spitemplates_t.devid, 1,DEVICE_NAME);
    }
    else
    {
        alloc_chrdev_region(&sg_spitemplates_t.devid, 0, 1, DEVICE_NAME);
        sg_spitemplates_t.major = MAJOR(sg_spitemplates_t.devid);
    }

    /* 2.注册设备 */
    UDEBUG("drv_spi_templates add cdev! \r\n");
    cdev_init(&sg_spitemplates_t.cdev, &drv_spi_templates_ops);
    cdev_add(&sg_spitemplates_t.cdev, sg_spitemplates_t.devid, 1);

    /* 3.创建类 */
    UDEBUG("drv_spi_templates create class! \r\n");
    sg_spitemplates_t.class = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(sg_spitemplates_t.class))
    {
        return PTR_ERR(sg_spitemplates_t.class);
    }

    /* 4.创建设备 */
    UDEBUG("drv_spi_templates create device! \r\n");
    sg_spitemplates_t.device = device_create(sg_spitemplates_t.class, NULL, sg_spitemplates_t.devid, NULL, DEVICE_NAME);
    if(IS_ERR(sg_spitemplates_t.device))
    {
        return PTR_ERR(sg_spitemplates_t.device);
    }

    /* 获取设备树中CS片选信号 */
    UDEBUG("drv_spi_templates get gpio! \r\n");
    sg_spitemplates_t.nd = of_find_node_by_path("/../../xxx_spi@xx");// 名称需要根据设备树进行定义
    if(sg_spitemplates_t.nd == NULL)
    {
        UDEBUG("drv_spi_templates spi4 node not find!\r\n");
        printk("drv_spi_templates probe fail!\r\n");
        return -EINVAL;
    }

    /* 获取设备树中的GPIO属性，得到对应编号 */
    sg_spitemplates_t.cs_gpio = of_get_named_gpio(sg_spitemplates_t.nd, "cs-gpio", 0);
    if(sg_spitemplates_t.cs_gpio < 0)
    {
        UDEBUG("drv_spi_templates can't get cs-gpio");
        return -EINVAL;
    }

    gpio_request(sg_spitemplates_t.cs_gpio,"spi4-cs-gpio");

    gpio_direction_output(sg_spitemplates_t.cs_gpio,1);

    /* 初始化spi_device */
    spi->mode = SPI_MODE_0;
    spi_setup(spi);
    sg_spitemplates_t.private_data = spi;

    /* 初始化模块 */
    printk("drv_spi_templates probe success!\r\n");

    return 0;
}

static int drv_spi_templates_remove(struct spi_device *spi)
{
    /* 是否申请的io资源 */
    gpio_free(sg_spitemplates_t.cs_gpio);

    /* 删除设备 */
    cdev_del(&sg_spitemplates_t.cdev);
    unregister_chrdev_region(sg_spitemplates_t.devid, 1);

    /* 注销类和设备 */
    device_destroy(sg_spitemplates_t.class, sg_spitemplates_t.devid);
    class_destroy(sg_spitemplates_t.class);

    return 0;
}



/* SPI驱动结构体 */
static struct spi_driver spi_driver = {
    .probe  = drv_spi_templates_probe,
    .remove = drv_spi_templates_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "drv_spi_templates",
        .of_match_table = drv_spi_templates_of_match,
    },
    .id_table = drv_spi_templates_id,
};


/************************************************************
*
* Function name	: 
* Description	: 
* Parameter		:
* Return		:
*
************************************************************/
static int __init drv_spi_templates_init(void)
{
    return spi_register_driver(&spi_driver);

}

/************************************************************
*
* Function name	: 
* Description	: 
* Parameter		:
* Return		:
*
************************************************************/
static void __exit drv_spi_templates_exit(void)
{
    spi_unregister_driver(&spi_driver);
}



module_init(drv_spi_templates_init);
module_exit(drv_spi_templates_exit);

MODULE_LICENSE("GPL");


