#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/module.h> 
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>

struct sysfs_demo {
	dev_t devid;			// 设备号
	struct cdev cdev;		// cdev
	struct class *class;
	struct device *device;	// 设备
    struct device_node *nd;
	struct kobject *k_obj;
	int major;
};

#define DEV_CONTROL_NAME ("sysfs_demo")
struct sysfs_demo sysfsdemo;

static int sysfs_demo_open(struct inode *inode, struct file *file)
{
	return 0;	
}

static int sysfs_demo_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t sysfs_demo_read (struct file *file, char __user *user, size_t size, loff_t *buff)
{
    return 0;
}

static ssize_t sysfs_demo_write (struct file *file, const char __user *user, size_t size, loff_t *buff)
{
    return 0;
}


static struct file_operations sysfs_demo_fops = {
    .owner  =   THIS_MODULE,   
    .open   =   sysfs_demo_open,  
	.release =  sysfs_demo_release,      
    .read = sysfs_demo_read,
    .write = sysfs_demo_write,
};


static ssize_t sysfs_demo_show(struct kobject *kobj, struct kobj_attribute *attr, char * buf)
{
	return strlcpy(buf, "0\n", 3);
}

static ssize_t sysfs_demo_set(struct kobject *kobj, struct kobj_attribute *attr, const char * buf, size_t n)
{
    return n;
}

#define KOBJ_ATTR(_name, _mode, _show, _store) \
	struct kobj_attribute kobj_attr_##_name = __ATTR(_name, _mode, _show, _store)

static KOBJ_ATTR(demo,0644,sysfs_demo_show,sysfs_demo_set);

static struct attribute *demo_attributes[] = {
    &kobj_attr_demo.attr,
    NULL
};


static const struct attribute_group atrr_demo_g = {
	.attrs = demo_attributes,
};

static int sysfs_demo_probe(struct platform_device *pdev)
{
	int ret = 0; 
	int value = 0;

	printk("sysfs demo probe\n");
	/* 1、创建设备号 */
	if (sysfsdemo.major)
	{
		/* 定义了设备号 */
		sysfsdemo.devid = MKDEV(sysfsdemo.major, 0);
		register_chrdev_region(sysfsdemo.devid, 1, DEV_CONTROL_NAME);
	}
	else
	{
		/* 没有定义设备号 */
		alloc_chrdev_region(&sysfsdemo.devid, 0, 1, DEV_CONTROL_NAME); /* 申请设备号 */
		sysfsdemo.major = MAJOR(sysfsdemo.devid);
	}

	/* 2、注册设备 */
	cdev_init(&sysfsdemo.cdev, &sysfs_demo_fops);
	cdev_add(&sysfsdemo.cdev, sysfsdemo.devid, 1);
	printk("sysfs demo devid:%d", sysfsdemo.major);

	/* 3、创建类 */
	sysfsdemo.class = class_create(THIS_MODULE, DEV_CONTROL_NAME);
	if (IS_ERR(sysfsdemo.class))
	{
		printk("sysfs demo create class fail\r\n");
		return PTR_ERR(sysfsdemo.class);
	}

	/* 4、创建设备 */
	sysfsdemo.device = device_create(sysfsdemo.class, NULL, sysfsdemo.devid, NULL, DEV_CONTROL_NAME);
	if (IS_ERR(sysfsdemo.device))
	{
		printk("sysfs demo create device fail\r\n");
		return PTR_ERR(sysfsdemo.device);
	}

	/* 查找结点 */
	sysfsdemo.nd = of_find_node_by_path("/sysfsdmoe");
	if(sysfsdemo.nd == NULL)
    {
        printk("sysfs demo node not find!\r\n");
        return -EINVAL;
    } else {
		printk("sysfs demo node find success!\r\n");
	}
	
	if ((sysfsdemo.k_obj = kobject_create_and_add("sysfsdmoe", NULL)) == NULL ) {
        printk("sysfsdmoe sys node create error \n");
        goto out;
    }

	ret = sysfs_create_group(sysfsdemo.k_obj, &atrr_demo_g);
	if (!ret)
	{
		printk(KERN_INFO "sysfs-demo sysfs create success\r\n");
	}
	else
	{
		printk(KERN_INFO "sysfs-demo sysfs create fail\r\n");
		goto out;
	}


	printk(KERN_INFO "sysfs demo sucess\n");
	return 0;

out:
	cdev_del(&sysfsdemo.cdev);
	unregister_chrdev_region(sysfsdemo.devid, 1); /* 注销设备号 */
	device_destroy(sysfsdemo.class, sysfsdemo.devid);
	class_destroy(sysfsdemo.class);
	printk(KERN_INFO "sysfs demo fail\n");
	return -1;
}
 

static int sysfs_demo_remove(struct platform_device *pdev)
{
	printk("sysfs demo remove !\n");
	sysfs_remove_group(&pdev->dev.kobj, &atrr_demo_g);

	cdev_del(&sysfsdemo.cdev);
	unregister_chrdev_region(sysfsdemo.devid, 1); /* 注销设备号 */
	device_destroy(sysfsdemo.class, sysfsdemo.devid);
	class_destroy(sysfsdemo.class);

	return 0;
}

static struct of_device_id sysfs_dmoe_of_match[] = {
        { .compatible = "sysfs-demo"},
        {}
};

static struct platform_driver sysfs_demo_driver={
    .driver = {
		.name           ="sysfs-demo",
		.owner          = THIS_MODULE,
		.of_match_table = sysfs_dmoe_of_match,
    },
    .probe  = sysfs_demo_probe,
    .remove = sysfs_demo_remove,
};
static int __init sysfs_demo_init(void)
{
    printk(KERN_INFO "Enter %s\n", __FUNCTION__);
    return platform_driver_register(&sysfs_demo_driver);
    return 0;
}
static void __exit sysfs_demo_exit(void)
{
	platform_driver_unregister(&sysfs_demo_driver);
    printk(KERN_INFO "Exit demo driver\n");
}

module_init(sysfs_demo_init);
module_exit(sysfs_demo_exit);
MODULE_LICENSE("GPL");