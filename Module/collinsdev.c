#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>

#define MAX_DEV 2
static int read_data =  1;
module_param(read_data,int,0660);

static int collinsdev_open(struct inode *inode, struct file *file);
static int collinsdev_release(struct inode *inode, struct file *file);
static ssize_t collinsdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t collinsdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations collinsdev_fops = {
    .owner      = THIS_MODULE,
    .open       = collinsdev_open,
    .release    = collinsdev_release,
    .read       = collinsdev_read,
    .write       = collinsdev_write
};

struct collins_device_data {
    struct cdev cdev;
};

static int dev_major = 0;
static struct class *collinsdev_class = NULL;
static struct collins_device_data collinsdev_data[MAX_DEV];

static int collinsdev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init collinsdev_init(void)
{
    int err, i;
    dev_t dev;

    printk("Inside collinsdev_init() \n");
    err = alloc_chrdev_region(&dev, 0, MAX_DEV, "collinsdev");

    dev_major = MAJOR(dev);

    collinsdev_class = class_create(THIS_MODULE, "collinsdev");
    collinsdev_class->dev_uevent = collinsdev_uevent;

    for (i = 0; i < MAX_DEV; i++) {
        cdev_init(&collinsdev_data[i].cdev, &collinsdev_fops);
        collinsdev_data[i].cdev.owner = THIS_MODULE;

        cdev_add(&collinsdev_data[i].cdev, MKDEV(dev_major, i), 1);

        device_create(collinsdev_class, NULL, MKDEV(dev_major, i), NULL, "collinsdev-%d", i);
    }

    return 0;
}

static void __exit collinsdev_exit(void)
{
    int i;

    printk("Inside collinsdev_exit() \n");

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(collinsdev_class, MKDEV(dev_major, i));
    }

    class_unregister(collinsdev_class);
    class_destroy(collinsdev_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}

static int collinsdev_open(struct inode *inode, struct file *file)
{
    printk("COLLINSDEV: Device open\n");
    return 0;
}

static int collinsdev_release(struct inode *inode, struct file *file)
{
    printk("COLLINSDEV: Device close\n");
    return 0;
}

static ssize_t collinsdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{

    uint8_t *data;
    size_t datalen;

    printk("Reading device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (read_data == 1) {
    	data = "Hello from the Kernel World 1!\n";
    } else {
	data = "Hello from the Kernel World 2!\n"; 
    }

    datalen = strlen(data);

    if (count > datalen) {
        count = datalen;
    }

    if (copy_to_user(buf, data, count)) {
        return -EFAULT;
    }

    return count;
}

static ssize_t collinsdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    size_t maxdatalen = 30, ncopied;
    uint8_t databuf[maxdatalen];

    printk("Writing device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count < maxdatalen) {
        maxdatalen = count;
    }

    ncopied = copy_from_user(databuf, buf, maxdatalen);

    if (ncopied == 0) {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        printk("Could't copy %zd bytes from the user\n", ncopied);
    }

    databuf[maxdatalen] = 0;

    printk("Data from the user: %s\n", databuf);

    return count;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ashish Yadav <ashish.yadav@collins.com>");
MODULE_DESCRIPTION("Simple Char Driver");
MODULE_VERSION("0.1");

module_init(collinsdev_init);
module_exit(collinsdev_exit);
