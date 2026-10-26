#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define DEVICE_NAME "build2learn_char"
#define CLASS_NAME "build2learn"
#define BUF_LEN 128

static dev_t dev_num;
static struct cdev char_cdev;
static struct class *char_class;
static struct device *char_device;

typedef struct {
  char buf[BUF_LEN];
  size_t size;
  struct mutex mtx;
} char_data;

static char_data device_data;

static bool use_lock = true;
module_param(use_lock, bool, 0644);
MODULE_PARM_DESC(use_lock, "Enable mutex lock when read/write");

static int delay_us = 0;
module_param(delay_us, int, 0644);
MODULE_PARM_DESC(delay_us,
                 "Add delay between copy each byte to/from user buffer");

static int char_open(struct inode *inode, struct file *file) {
  pr_info("%s: open\n", DEVICE_NAME);
  return 0;
}

static int char_release(struct inode *inode, struct file *file) {
  pr_info("%s: close\n", DEVICE_NAME);
  return 0;
}

static ssize_t char_read(struct file *file, char __user *buff, size_t count,
                         loff_t *ppos) {
  size_t available;
  size_t to_copy;
  unsigned long rst;

  if (use_lock) {
    if (mutex_lock_interruptible(&device_data.mtx)) {
      return -ERESTARTSYS;
    }
  }

  if (*ppos >= device_data.size) {
    if (use_lock)
      mutex_unlock(&device_data.mtx);
    return 0;
  }

  available = device_data.size - *ppos;
  to_copy = min(available, count);

  rst = copy_to_user(buff, device_data.buf + *ppos, to_copy);

  if (rst) {
  } else {
    pr_info("%s: read %zu bytes\n", DEVICE_NAME, to_copy);
    *ppos += to_copy;
  }

  if (use_lock)
    mutex_unlock(&device_data.mtx);

  if (rst) {
    return -EFAULT;
  } else {
    return to_copy;
  }
}

static ssize_t char_write(struct file *file, const char __user *buff,
                          size_t count, loff_t *ppos) {

  if (use_lock) {
    if (mutex_lock_interruptible(&device_data.mtx)) {
      return -ERESTARTSYS;
    }
  }

  size_t to_copy = min(count, BUF_LEN);

  unsigned long rst = copy_from_user(device_data.buf, buff, to_copy / 2);

  if (rst) {
  } else {
    if (delay_us) {
      usleep_range(delay_us, delay_us + 10);
    }

    rst = copy_from_user(device_data.buf + (to_copy / 2), buff + (to_copy / 2),
                         to_copy - (to_copy / 2));

    if (rst) {
    } else {
      pr_info("%s: write %zu bytes\n", DEVICE_NAME, to_copy);
      device_data.size = to_copy;
      *ppos = to_copy;
    }
  }

  if (use_lock)
    mutex_unlock(&device_data.mtx);

  if (rst) {
    return -EFAULT;
  } else {
    return to_copy;
  }
}

static const struct file_operations char_fops = {.owner = THIS_MODULE,
                                                 .open = char_open,
                                                 .release = char_release,
                                                 .read = char_read,
                                                 .write = char_write};

static int __init char_dev_init(void) {
  int ret;

  mutex_init(&device_data.mtx);

  ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
  if (ret) {
    pr_err("%s: alloc_chrdev_region failed: %d\n", DEVICE_NAME, ret);
    return ret;
  }

  cdev_init(&char_cdev, &char_fops);

  ret = cdev_add(&char_cdev, dev_num, 1);
  if (ret) {
    pr_err("%s: cdev_add failed: %d\n", DEVICE_NAME, ret);
    goto err_unregister;
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
  char_class = class_create(CLASS_NAME);
#else
  char_class = class_create(THIS_MODULE, CLASS_NAME);
#endif

  if (IS_ERR(char_class)) {
    ret = PTR_ERR(char_class);
    pr_err("%s: class_create failed: %d\n", DEVICE_NAME, ret);
    goto err_cdev;
  }

  char_device = device_create(char_class, NULL, dev_num, NULL, DEVICE_NAME);

  if (IS_ERR(char_device)) {
    ret = PTR_ERR(char_device);
    pr_err("%s: device_create failed: %d\n", DEVICE_NAME, ret);
    goto err_class;
  }

  pr_info("%s: loaded major=%d minor=%d /dev/%s\n", DEVICE_NAME, MAJOR(dev_num),
          MINOR(dev_num), DEVICE_NAME);
  return 0;

err_class:
  class_destroy(char_class);
err_cdev:
  cdev_del(&char_cdev);
err_unregister:
  unregister_chrdev_region(dev_num, 1);

  return ret;
}

static void __exit char_dev_exit(void) {
  device_destroy(char_class, dev_num);
  class_destroy(char_class);
  cdev_del(&char_cdev);
  unregister_chrdev_region(dev_num, 1);
  pr_info("%s: module unloaded\n", DEVICE_NAME);
}

module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hsiu-Chi Chang");
MODULE_DESCRIPTION("Practice build char device");