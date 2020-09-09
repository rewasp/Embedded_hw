#ifndef __DEV__
#define __DEV__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/ioctl.h>
#include<linux/timer.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/uaccess.h>
#include <linux/io.h>



#define DEV_MAJOR 242 // major number
#define DEV_NAME "dev_driver" // driver name

#define DEV_IOCTL_BASE 'p' // ioctl magic number
#define DEVIOC_WRITE    _IOW(DEV_IOCTL_BASE, 0, int)

#define LED_ADDRESS 0x08000016
#define FND_ADDRESS 0x08000004
#define DOT_ADDRESS 0x08000210
#define TEXT_ADDRESS 0x08000090


#endif