# Building Moxa Linux Kernel *ls102xa-linux-4.4*
Follow the instructions below to build the *ls102xa-linux-4.4* kernel and update your device.

## Table Of Contents

- [Building Moxa Linux Kernel *ls102xa-linux-4.4*](#building-moxa-linux-kernel-ls102xa-linux-44)
- [Table Of Contents](#table-of-contents)
- [Build Instructions Table](#build-instructions-table)
- [Build Kernel Packages](#build-kernel-packages)
    - [1. Download source](#1-download-source)
    - [2. Check out the kernel source](#2-check-out-the-kernel-source)
    - [3. Download dockerfile](#3-download-dockerfile)
    - [4. Create docker container](#4-create-docker-container)
    - [5. Build kernel packages](#5-build-kernel-packages)
- [Update Kernel Packages](#update-kernel-packages)
    - [1. Upload kernel packages](#1-upload-kernel-packages)
    - [2. (Optional) Backup the original kernel](#2-optional-backup-the-original-kernel)
    - [3. Install kernel packages](#3-install-kernel-packages)
---

## Build Instructions Table

| Branch / Tags | Build Instructions |
| ------------- | ------------------ |
| UC-8410A <br> UC-8540 <br> UC-8580 | [Past Instructions](OLD_GUIDELINE.md) |
| UC-8410A <br> UC-8540 <br> UC-8580 <br> 4.4.201-cip39-rt26/jessie/master | Below Instructions (`Latest`) |

## Build Kernel Packages

### 1. Download source

Obtain *ls102xa-linux-4.4* sources:

```bash
git clone git@gitlab.syssw.moxa.com:MXcore-Kernel/ls102xa-linux-4.4.git
```

### 2. Check out the kernel source

Go to `/your/path/to/ls102xa-linux-4.4` and check the branch. If you want to build the latest ls102xa-linux-4.4 kernel, please make sure you are in the develop branch.

```bash
# check branch
git branch -a
# * 4.4.112-cip18-rt11/stretch/develop
# ...
```

If you want to build the specific version of *ls102xa-linux-4.4* kernel source code, please check out the kernel source using tag.

```bash
# show tags
git tag
# 4.4.201-cip39-rt26/upstream
# UC-8410A_3.0
# UC-8410A_3.1
# ...
```

Check out kernel source by tag:
```bash
git checkout <product>_V<version>
```

### 3. Download dockerfile

Moxa provides [moxa-dockerfiles](http://gitlab.syssw.moxa.com/MXcore-Tool/moxa-dockerfiles) to create build environment.

### 4. Create docker container

Create a docker container and execute the following commands from the directory.

```bash
sudo docker run -d -it -v ${PWD}:/workspace moxa-package-builder:1.0.0 bash
# output <container-id>
d103e6df5f719f9430056f9c23cf4e3e518d4a4f8b5b65e55889b90c258886c6
```

After executing commands, you will get a `<container-id>` like this `d103e6df5f719f9430056f9c23cf4e3e518d4a4f8b5b65e55889b90c258886c6`.

### 5. Build kernel packages

Execute the following commands to build kernel package.
```bash
# go into the docker container
docker start -ia <container-id>
cd /workspace/ls102xa-linux-4.4
apt-get build-dep -aarmhf .
dpkg-buildpackage -us -uc -b -aarmhf
```
You can find `.deb` files under `/workspace` directory after the build process completes.

---
## Update Kernel Packages

After building the kernel packages, you can update your device.
Below are instructions to update the kernel packages on `UC-8410A`.

### 1. Upload kernel packages
```bash
# upload to /tmp
scp uc8410a-kernel*.deb uc8410a-modules*.deb moxa@<ip-addresso-of-your-device>:/tmp
```
### 2. Install kernel packages

```bash
cd /tmp
dpkg -i *.deb
sync
```

**NOTE: Remember to reboot the device after installing the kernel package!**
