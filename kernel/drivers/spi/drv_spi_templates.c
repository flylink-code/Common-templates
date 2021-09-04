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

#include "drv_spitodsp.h"

#define SPI_TODSP_USE_DEBUG
#undef UDEBUG            
#ifdef SPI_TODSP_USE_DEBUG
#define UDEBUG(fmt,args...) printk(KERN_DEBUG "scull: " fmt, ## args)
#else
#define UDEBUG(fmt, args...) /* not debugging: nothing */
#endif 


#define DEVICE_NAME ("drv_spitodsp")

struct drv_spitodsp_dev {
    dev_t               devid;          // 设备号
    struct cdev         cdev;           // CDEV
    struct class        *class;         // 类
    struct device       *device;        // 设备
    struct device_node  *nd;            // 设备节点
    int                 major;          // 主设备号
    void                *private_data;  // 私有数据
};

static struct drv_spitodsp_dev sg_spitodevdev_t;

/************************************************************
*
* Function name	: drv_spitodsp_read_regs
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_spitodsp_read_regs(struct drv_spitodsp_dev *dev,uint8_t reg,uint8_t *buf,int len, uint8_t cs)
{
    int ret = 0;
    struct spi_device *spi = (struct spi_device *)dev->private_data;
    struct spi_message message;
    struct spi_transfer *transfer;
    uint8_t index = 0;
    uint8_t *rxdata;
    uint8_t *txdata;


    /* 数据输入检查 */
    if(dev == NULL || buf == NULL || len == 0) {
        return -1;
    }

    /* 申请内存 */
    transfer = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);
    txdata = kzalloc(len+1,GFP_KERNEL);
    rxdata = kzalloc(len+1,GFP_KERNEL);

    if(transfer == NULL || txdata == NULL || rxdata == NULL) {
        return -1;
    }

    txdata[0] = (reg); // 根据实际内容修改写入寄存器信息
    for(index=0;index<len;index++)
    {
        txdata[1+index] = 0xff;        
    }
    transfer->tx_buf = txdata; // 要发送的数据
    transfer->len    = 1+len; 
    transfer->rx_buf = rxdata; // 读取到的数据
    transfer->len    = 1+len; // 要读取的数据长度

    spi->chip_select = cs;

    spi_message_init(&message); // 初始化spi_message
    spi_message_add_tail(transfer, &message); // 将spi_transfer添加到spi_message
    ret = spi_sync(spi ,&message); // 同步发送
    /* 判断结果 */

    for(index=0;index<len;index++)
    {
        buf[index] = rxdata[1+index];        
    }

    /* 释放内存 */
    kfree(rxdata);
    kfree(txdata);
    kfree(transfer);

    return ret;
}

/************************************************************
*
* Function name	: drv_spitodsp_write_regs
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_spitodsp_write_regs(struct drv_spitodsp_dev *dev,uint8_t reg,uint8_t *buf,int len, uint8_t cs)
{
    int ret = 0;
    struct spi_device *spi = (struct spi_device *)dev->private_data;
    struct spi_message message;
    struct spi_transfer *transfer;
    uint8_t index = 0;
    uint8_t *txdata;

    /* 数据输入检查 */
    if(dev == NULL || buf == NULL || len == 0) {
        return -1;
    }

    /* 申请内存 */
    transfer = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);
    txdata = kzalloc(len+1,GFP_KERNEL);

    if(transfer == NULL || txdata == NULL) {
        return -1;
    }

    txdata[0] = (reg); // 根据实际内容修改写入寄存器信息
    for(index=0;index<len;index++)
    {
        txdata[1+index] = 0xff;        
    }
    transfer->tx_buf = txdata; // 要发送的数据
    transfer->len    = 1+len; 
    spi_message_init(&message); // 初始化spi_message
    spi_message_add_tail(transfer, &message);   // 将spi_transfer添加到spi_message
    ret = spi_sync(spi ,&message);  // 同步发送


    /* 释放内存 */
    kfree(txdata);
    kfree(transfer);

    return ret;
}

/************************************************************
*
* Function name	: drv_spitodsp_open
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_spitodsp_open(struct inode *inode, struct file *filp)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_spitodsp_read
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static ssize_t drv_spitodsp_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_spitodsp_write
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static ssize_t drv_spitodsp_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *off)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_spitodsp_release
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_spitodsp_release(struct inode *inode, struct file *filp)
{

    return 0;
}



static const struct file_operations drv_spitodsp_ops = {
    .owner   = THIS_MODULE,
    .open    = drv_spitodsp_open,
    .read    = drv_spitodsp_read,
    .write   = drv_spitodsp_write,
    .release = drv_spitodsp_release,
};



static const struct spi_device_id drv_spitodsp_id[] = {
    {"ti,drv_spitodsp",0},
    {}
};



/* 设备树匹配列表 */
static const struct of_device_id drv_spitodsp_of_match[] = {
    { .compatible = "ti,drv_spitodsp"},
    { /* Sentinel */ }
};

static int drv_spitodsp_probe(struct spi_device *spi)
{


    /* 1.构建设备号 */
    printk("drv_spitodsp probe \r\n");
    if(sg_spitodevdev_t.major)
    {
        sg_spitodevdev_t.devid = MKDEV(sg_spitodevdev_t.major,0); // 静态
        register_chrdev_region(sg_spitodevdev_t.devid, 1,DEVICE_NAME);
    }
    else
    {
        alloc_chrdev_region(&sg_spitodevdev_t.devid, 0, 1, DEVICE_NAME);
        sg_spitodevdev_t.major = MAJOR(sg_spitodevdev_t.devid);
    }

    /* 2.注册设备 */
    UDEBUG("drv_spitodsp add cdev! \r\n");
    cdev_init(&sg_spitodevdev_t.cdev, &drv_spitodsp_ops);
    cdev_add(&sg_spitodevdev_t.cdev, sg_spitodevdev_t.devid, 1);

    /* 3.创建类 */
    UDEBUG("drv_spitodsp create class! \r\n");
    sg_spitodevdev_t.class = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(sg_spitodevdev_t.class))
    {
        return PTR_ERR(sg_spitodevdev_t.class);
    }

    /* 4.创建设备 */
    UDEBUG("drv_spitodsp create device! \r\n");
    sg_spitodevdev_t.device = device_create(sg_spitodevdev_t.class, NULL, sg_spitodevdev_t.devid, NULL, DEVICE_NAME);
    if(IS_ERR(sg_spitodevdev_t.device))
    {
        return PTR_ERR(sg_spitodevdev_t.device);
    }

    /* 初始化spi_device */
    spi->mode = SPI_MODE_0;
    spi_setup(spi);
    sg_spitodevdev_t.private_data = spi;

    /* 初始化模块 */
    printk("drv_spitodsp probe success!\r\n");

    return 0;
}

static int drv_spitodsp_remove(struct spi_device *spi)
{
    /* 删除设备 */
    cdev_del(&sg_spitodevdev_t.cdev);
    unregister_chrdev_region(sg_spitodevdev_t.devid, 1);

    /* 注销类和设备 */
    device_destroy(sg_spitodevdev_t.class, sg_spitodevdev_t.devid);
    class_destroy(sg_spitodevdev_t.class);

    return 0;
}



/* SPI驱动结构体 */
static struct spi_driver spi_driver = {
    .probe  = drv_spitodsp_probe,
    .remove = drv_spitodsp_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "drv_spitodsp",
        .of_match_table = drv_spitodsp_of_match,
    },
    .id_table = drv_spitodsp_id,
};


/************************************************************
*
* Function name	: 
* Description	: 
* Parameter		:
* Return		:
*
************************************************************/
static int __init drv_spitodsp_init(void)
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
static void __exit drv_spitodsp_exit(void)
{
    spi_unregister_driver(&spi_driver);
}



module_init(drv_spitodsp_init);
module_exit(drv_spitodsp_exit);

MODULE_LICENSE("GPL");


