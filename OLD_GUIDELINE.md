# Moxa Kernel Repository Guidelines

## Table Of Contents
* Getting Started
* How to update kernel
* Products List
* Appendix

---
## Getting Started

### Check out the kernel source:

#### If you want to build the latest ls1021a-linux-4.4 kernel source code. Please make sure you are at develop branch.
```
# git branch -a
```

#### If you want to build the specific version of ls1021a-linux-4.4 kernel source code. Please checkout the kernel source using tag.

##### To show all the tags.
```
# git tag
```

##### Checkout kernel source by using tag:
```
# git checkout <product>_V<version>
```
### Steps to compile the kernel:

The detail model support list, default kernel configuration, dts file, its file and kernel type information can be found at "Product List" below.


#### 1. Set default config.
```
# make <product>_defconfig
```
Example:
```
# make ls102xa_defconfig
```

#### 2. Compile kernel and create uImage file.
```
# make uImage -j[jobs]
```
Example:
```
# make uImage -j16
```

Our kernel source code will build dtb file automatically. If you want to re-build dtb file by your self.
```
# make <product dts name>.dtb
```
Example:
```
# make ls1021a-moxa-uc-8410a.dtb
```

#### 4. make kernel modules:
```
# make modules
```

#### 5. Strip and install kernel modules:
```
# make INSTALL_MOD_STRIP=1 modules_install INSTALL_MOD_PATH=<YOUR DIRECTORY>
```
Example:
```
# make INSTALL_MOD_STRIP=1 modules_install INSTALL_MOD_PATH=/tmp
```

---
## How to update kernel
### Steps to update kernel

#### 1. Upload all your compiled results to the device
```
# scp -r <Username of device system>@<IP address of device>:<File path>
```

To upload kernel modules:
Example:
```
# scp -r /tmp/lib/modules/4.4.0-cip-rt-moxa-ls102xa/kernel/ moxa@192.168.3.100:/tmp
# scp /tmp/lib/modules/4.4.0-cip-rt-moxa-ls102xa/modules.* moxa@192.168.3.100:/tmp
```

To upload uImage and dtb files:
```
# scp arch/arm/boot/uImage moxa@192.168.3.100:/tmp
# scp arch/arm/boot/ls1021a-moxa-uc-8410a.dtb moxa@192.168.3.100:/tmp
```

#### 2. Replace the kernel modules on the device with new files

Backup the original kernel modules and update kernel modules with new files

Example:
```
# mv /lib/modules/4.4.0-cip-rt-uc8400a+/ /lib/modules/4.4.0-cip-rt-uc8400a+_bak/
# mkdir -p /lib/modules/4.4.0-cip-rt-moxa-ls102xa/
# cp -arf /tmp/kernel/ /lib/modules/4.4.0-cip-rt-moxa-ls102xa/
# cp -arf /tmp/modules.* /lib/modules/4.4.0-cip-rt-moxa-ls102xa/
# sync
```

#### 3. Replace the kernel file on the device with new file

The kernel file is placed in operation system storage partition 1.
The storage device name can be checked by `lsblk` command:

![lsblk](https://github.com/Moxa-Linux/resize-image/blob/develop/lsblk.PNG?raw=true)

In this example, root is mounted at "/dev/mmcblk0p2", so the system storage device is "/dev/mmcblk0". Partition 1 is "/dev/mmcblk0p1".

To mount operation system storage partition 1

Example:
```
# mount /dev/mmcblk0p1 /mnt
# cd /mnt
```

Backup the original kernel file and update it with new file

Example:
```
# mv ls1021a-moxa-uc-8410a.dtb ls1021a-moxa-uc-8410a.dtb_bak
# mv uImage uImage_bak
# cp /tmp/ls1021a-moxa-uc-8410a.dtb .
# cp /tmp/uImage .
# sync
```

#### 3. Reboot the device
```
# reboot
```

---
## Products List
### Product kernel configuration:
There following are the list of product kernel configuration files. defconfig is placed in the arch/arm/configs folder, dts file is placed in arch/arm/boot/dts folder, its file is placed in its folder.

##### UC-8410A series
* models: UC-8410A-LX
* defconfig: uc8410a_defconfig
* dts: moxa-uc8410a.dts

---
## Appendix

### How to create a customized image?
To create a customized image, please refer to the following link and see "Steps to create a customized image" section.
[https://github.com/Moxa-Linux/resize-image](https://github.com/Moxa-Linux/resize-image)

### About the kernel version magic name
For standard Linux kernel source code, if the source is maintained in the git repository, the plus sign "+" will be automatically appended to the kerenl version magic.

Example:
	4.1.0-ltsi+

If you switch the kernel source code to a tag or a source build that has never been maintained in git, the plus sign "+" will not be appended.

Example:
	4.1.0-ltsi
