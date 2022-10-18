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

#include "drv_i2c.h"

#define DRV_I2C_USE_DEBUG
#undef UDEBUG            
#ifdef DRV_I2C_USE_DEBUG
#define UDEBUG(fmt,args...) printk(KERN_DEBUG "scull: " fmt, ## args)
#else
#define UDEBUG(fmt, args...) /* not debugging: nothing */
#endif 


#define DEVICE_CNT	1
#define DEVICE_NAME "drv_i2c"

struct drv_i2c_dev {
	dev_t devid;			// 设备号
	struct cdev cdev;		// cdev
	struct class *class;	// 类 
	struct device *device;	// 设备
	struct device_node	*nd; // 设备节点
	int major;			// 主设备号
	void *private_data;	// 私有数据
};

static struct drv_i2c_dev device_dev_t;

static int drv_i2c_read_regs(struct drv_i2c_dev *dev, u8 reg, void *val, int len)
{
	int ret;
	struct i2c_msg msg[2];
	struct i2c_client *client = (struct i2c_client *)dev->private_data;

	/* msg[0]为发送要读取的首地址 */
	msg[0].addr = client->addr;			/* drv_i2c地址 */
	msg[0].flags = 0;					/* 标记为发送数据 */
	msg[0].buf = &reg;					/* 读取的首地址 */
	msg[0].len = 1;						/* reg长度*/

	/* msg[1]读取数据 */
	msg[1].addr = client->addr;			/* drv_i2c地址 */
	msg[1].flags = I2C_M_RD;			/* 标记为读取数据*/
	msg[1].buf = val;					/* 读取数据缓冲区 */
	msg[1].len = len;					/* 要读取的数据长度*/

	ret = i2c_transfer(client->adapter, msg, 2);
	if(ret == 2) {
		ret = 0;
	} else {
		printk("i2c rd failed=%d reg=%06x len=%d\n",ret, reg, len);
		ret = -EREMOTEIO;
	}
	return ret;
}

/*
 * @description	: 向drv_i2c多个寄存器写入数据
 * @param - dev:  drv_i2c设备
 * @param - reg:  要写入的寄存器首地址
 * @param - val:  要写入的数据缓冲区
 * @param - len:  要写入的数据长度
 * @return 	  :   操作结果
 */
static s32 drv_i2c_write_regs(struct drv_i2c_dev *dev, u8 reg, u8 *buf, u8 len)
{
	u8 b[256];
	struct i2c_msg msg;
	struct i2c_client *client = (struct i2c_client *)dev->private_data;
	
	b[0] = reg;					/* 寄存器首地址 */
	memcpy(&b[1],buf,len);		/* 将要写入的数据拷贝到数组b里面 */
		
	msg.addr = client->addr;	/* drv_i2c地址 */
	msg.flags = 0;				/* 标记为写数据 */

	msg.buf = b;				/* 要写入的数据缓冲区 */
	msg.len = len + 1;			/* 要写入的数据长度 */

	return i2c_transfer(client->adapter, &msg, 1);
}

/*
 * @description	: 读取drv_i2c指定寄存器值，读取一个寄存器
 * @param - dev:  drv_i2c设备
 * @param - reg:  要读取的寄存器
 * @return 	  :   读取到的寄存器值
 */
static unsigned char drv_i2c_read_reg(struct drv_i2c_dev *dev, u8 reg)
{
	u8 data = 0;

	drv_i2c_read_regs(dev, reg, &data, 1);
	return data;

#if 0
	struct i2c_client *client = (struct i2c_client *)dev->private_data;
	return i2c_smbus_read_byte_data(client, reg);
#endif
}

/*
 * @description	: 向drv_i2c指定寄存器写入指定的值，写一个寄存器
 * @param - dev:  drv_i2c设备
 * @param - reg:  要写的寄存器
 * @param - data: 要写入的值
 * @return   :    无
 */
static void drv_i2c_write_reg(struct drv_i2c_dev *dev, u8 reg, u8 data)
{
	u8 buf = 0;
	buf = data;
	drv_i2c_write_regs(dev, reg, &buf, 1);
}


/************************************************************
*
* Function name	: drv_i2c_open
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_i2c_open(struct inode *inode, struct file *filp)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_i2c_read
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static ssize_t drv_i2c_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_i2c_write
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static ssize_t drv_i2c_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *off)
{

    return 0;
}

/************************************************************
*
* Function name	: drv_i2c_release
* Description	: 
* Parameter		:
* Return		: 
*
************************************************************/
static int drv_i2c_release(struct inode *inode, struct file *filp)
{

    return 0;
}

static const struct file_operations drv_i2c_ops = {
	.owner = THIS_MODULE,
	.open = drv_i2c_open,
    .write = drv_i2c_write,
	.read = drv_i2c_read,
	.release = drv_i2c_release,
};

 /*
  * @description     : i2c驱动的probe函数，当驱动与
  *                    设备匹配以后此函数就会执行
  * @param - client  : i2c设备
  * @param - id      : i2c设备ID
  * @return          : 0，成功;其他负值,失败
  */
static int drv_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	/* 1、构建设备号 */
    UDEBUG("drv_i2c probe! \r\n");
	if (device_dev_t.major) {
		device_dev_t.devid = MKDEV(device_dev_t.major, 0);
		register_chrdev_region(device_dev_t.devid, DEVICE_CNT, DEVICE_NAME);
	} else {
		alloc_chrdev_region(&device_dev_t.devid, 0, DEVICE_CNT, DEVICE_NAME);
		device_dev_t.major = MAJOR(device_dev_t.devid);
	}

	/* 2、注册设备 */
	cdev_init(&device_dev_t.cdev, &drv_i2c_ops);
	cdev_add(&device_dev_t.cdev, device_dev_t.devid, DEVICE_CNT);

	/* 3、创建类 */
	device_dev_t.class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(device_dev_t.class)) {
		return PTR_ERR(device_dev_t.class);
	}

	/* 4、创建设备 */
	device_dev_t.device = device_create(device_dev_t.class, NULL, device_dev_t.devid, NULL, DEVICE_NAME);
	if (IS_ERR(device_dev_t.device)) {
		return PTR_ERR(device_dev_t.device);
	}

	device_dev_t.private_data = client;

	return 0;
}

/*
 * @description     : i2c驱动的remove函数，移除i2c驱动的时候此函数会执行
 * @param - client 	: i2c设备
 * @return          : 0，成功;其他负值,失败
 */
static int drv_i2c_remove(struct i2c_client *client)
{
	/* 删除设备 */
	cdev_del(&device_dev_t.cdev);
	unregister_chrdev_region(device_dev_t.devid, DEVICE_CNT);

	/* 注销掉类和设备 */
	device_destroy(device_dev_t.class, device_dev_t.devid);
	class_destroy(device_dev_t.class);
	return 0;
}


/* 传统匹配方式ID列表 */
static const struct i2c_device_id drv_i2c_id[] = {
	{"eant,drv_i2c", 0},  
	{}
};


/* 设备树匹配列表 */
static const struct of_device_id drv_i2c_of_match[] = {
	{ .compatible = "eant,drv_i2c" },
	{ /* Sentinel */ }
};

/* i2c驱动结构体 */	
static struct i2c_driver drv_i2c_driver = {
	.probe = drv_i2c_probe,
	.remove = drv_i2c_remove,
	.driver = {
			.owner = THIS_MODULE,
		   	.name = "drv_i2c",
		   	.of_match_table = drv_i2c_of_match, 
		   },
	.id_table = drv_i2c_id,
};

static int __init drv_i2c_init(void)
{
	int ret = 0;

	ret = i2c_add_driver(&drv_i2c_driver);
	return ret;
}

static void __exit drv_i2c_exit(void)
{
	i2c_del_driver(&drv_i2c_driver);
}

module_init(drv_i2c_init);
module_exit(drv_i2c_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zl");