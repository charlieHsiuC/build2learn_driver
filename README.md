# build2learn_driver

Build to learn Linux driver

## Prerequisites

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r) bear clangd
```

## Build

```bash
make
```

### Clangd

```bash
bear -- make
```

## Lesson Learned

### 01-hello-world

- `module_init` and `module_exit`
  - `module_init` and `module_exit` are used to register the module initialization and exit functions.
- `__init` and `__exit`
  - `__init` and `__exit` are used to mark the functions that are used to initialize and exit the module.
- `MODULE_LICENSE`, `MODULE_AUTHOR`, `MODULE_DESCRIPTION`
  - `MODULE_LICENSE`, `MODULE_AUTHOR`, `MODULE_DESCRIPTION` are used to mark the license, author, and description of the module.
- `pr_info`
  - `pr_info` is used to print information to the kernel log.
  - `dmesg` to view the kernel log.

Run `01-hello-world`:

```bash
sudo insmod src/01-hello-world/hello-world.ko
sudo dmesg | tail -n 10
sudo rmmod hello-world
```

### 02-module-param

- `module_param`, `module_param_string`: used to define module parameters.
  - these macros expand to functions and variables that are used to store the module parameters, so they should be declared outside of functions.
- `MODULE_PARM_DESC`: used to describe the module parameters.
  - `modinfo <module_name>.ko` to view the module parameters.
- read module parameters from sysfs
  - `cat /sys/module/<module_name>/parameters/<parameter_name>` to read the module parameters.
- write module parameters to sysfs
  - `echo <value> | sudo tee /sys/module/<module_name>/parameters/<parameter_name>` to write the module parameters.
  - `cat /sys/module/<module_name>/parameters/<parameter_name>` to read the module parameters to check if the parameters are set correctly.

Run `02-module-param`:

```bash
sudo insmod src/02-module-param/module-param.ko int_param=10 bool_param=1 charp_param=a string_param=hello
sudo dmesg | tail -n 10
sudo rmmod module-param
```

### 03-char-dev

Build simple char device driver:

- function pointers
  - file operations
    - `open`
    - `release`
    - `read`
    - `write`
- userspace data transfer
  - `copy_from_user`
  - `copy_to_user`
- device registration
  - `alloc_chrdev_region`: allocate a range of device numbers for the device
  - `cdev_init`: initialize a cdev structure with the file operations
  - `cdev_add`: add a cdev to the system with the device number
  - `class_create`: create a class for the device
  - `device_create`: create a device with the class and the device number, kernel will create a device node in sysfs and send uevent to userspace
- device number
  - `MAJOR`: major number of the device, represents the device type.
  - `MINOR`: minor number of the device, represents the device instance.
- error handling
  - `IS_ERR`: check if the pointer is an error.
  - `PTR_ERR`: get the error code from the pointer.
- unload module flow: reverse order of module initialization.
  - `cdev_del`: delete the cdev.
  - `device_destroy`: delete the device.
  - `class_destroy`: delete the class.
  - `unregister_chrdev_region`: unregister the device number.
- print error message
  - `pr_err`: print error message to the kernel log.

Run `03-char-dev`:

```bash
sudo insmod src/03-char-dev/char-dev.ko
sudo dmesg | tail -n 10
sudo rmmod char-dev
```

Test char device driver by userspace program:

```bash
sudo insmod src/03-char-dev/char-dev.ko
./src/03-char-dev/test-char-dev
sudo rmmod char-dev
```
