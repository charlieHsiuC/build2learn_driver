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

## Run

```bash
sudo insmod src/01-hello-world/hello-world.ko
sudo dmesg | tail -n 10
sudo rmmod hello-world
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
