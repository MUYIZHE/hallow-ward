#include "linux/miscdevice.h"
#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/gpio.h"
#include "linux/fs.h"
#include "linux/cdev.h"
#include "linux/device.h"
dev_t myleds_devnum[4]={0};
struct cdev  myleds_cdev;
struct file_operations ops;

struct class * cls = NULL;

//我打开 myled1 myled2 myled3 myled4 都会到此处调用！
int myleds_open(struct inode * i, struct file * f)
{
	//想要实现四个 设备文件独立性 关键在哪？
	//我如何判断 是哪个设备文件在调用 这个函数！
	//如何在此函数中 判断 不同的设备文件呢？
	//* 设备文件区别 ->设备号！  myleds_devnum[4]
	// struct inode * i -> i_rdev == 设备号！ 

	if(i->i_rdev == myleds_devnum[0])//LED1 
	{
		gpio_set_value(EXYNOS4X12_GPM4(0),0);
	}else if(i->i_rdev == myleds_devnum[1])//LED2
	{
		gpio_set_value(EXYNOS4X12_GPM4(1),0);
	}else if(i->i_rdev == myleds_devnum[2])//LED3
	{
		gpio_set_value(EXYNOS4X12_GPM4(2),0);
	}else if(i->i_rdev == myleds_devnum[3])//LED4
	{
		gpio_set_value(EXYNOS4X12_GPM4(3),0);
	}
	
	return 0;
}
int myleds_close(struct inode * i, struct file * f)
{
	if(i->i_rdev == myleds_devnum[0])//LED1 
	{
		gpio_set_value(EXYNOS4X12_GPM4(0),1);
	}else if(i->i_rdev == myleds_devnum[1])//LED2
	{
		gpio_set_value(EXYNOS4X12_GPM4(1),1);
	}else if(i->i_rdev == myleds_devnum[2])//LED3
	{
		gpio_set_value(EXYNOS4X12_GPM4(2),1);
	}else if(i->i_rdev == myleds_devnum[3])//LED4
	{
		gpio_set_value(EXYNOS4X12_GPM4(3),1);
	}
	return 0;
}


static int __init myleds_init()
{
//1: 申请四个 Linux2.6的 设备号
	alloc_chrdev_region(myleds_devnum,0,4,"myleds");
	myleds_devnum[1] = myleds_devnum[0] + 1;
	myleds_devnum[2] = myleds_devnum[0] + 2;
	myleds_devnum[3] = myleds_devnum[0] + 3;
	
//2: 初始化Linux2.6的核心结构体 
	ops.owner = THIS_MODULE;
	ops.open = myleds_open;
	ops.release = myleds_close;

	cdev_init(&myleds_cdev,&ops);
//3: 添加设备 
	cdev_add(&myleds_cdev,myleds_devnum[0],4);
//4: 生成类结构体 
	cls = class_create(THIS_MODULE, "myled_class");
//5: 生成设备文件 
	device_create(cls, NULL,myleds_devnum[0],NULL,"myled1");
	device_create(cls, NULL,myleds_devnum[1],NULL,"myled2");
	device_create(cls, NULL,myleds_devnum[2],NULL,"myled3");
	device_create(cls, NULL,myleds_devnum[3],NULL,"myled4");

	return 0;
}

static void __exit myleds_exit()
{
	//6:销毁设备 
	device_destroy(cls, myleds_devnum[3]);
	device_destroy(cls, myleds_devnum[2]);
	device_destroy(cls, myleds_devnum[1]);
	device_destroy(cls, myleds_devnum[0]);

	//7:销毁类
	class_destroy(cls);
	//8:取消注册设备
	cdev_del(&myleds_cdev);
	
}

module_init(myleds_init);
module_exit(myleds_exit);
MODULE_LICENSE("GPL");












