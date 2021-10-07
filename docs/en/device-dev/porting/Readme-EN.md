# Development Board Porting
OpenHarmony has organized a Special Interest Group (SIG) [SIG_DevBoard](https://gitee.com/openharmony/community/blob/master/sig/sig-devboard/sig_devboard.md) to provide support for third-party development boards.

Before learning about how to port the code of a development board, take a look at the device classification on OpenHarmony. The porting methods vary according to the device type.

| Device Type| Hardware Requirement| Supported Kernel|
|---------|-------------|----------------|
| Mini-system devices| Memory > 128 KB| LiteOS-M       |
| Small-system devices| Memory > 1 MB, with MMU| LiteOS-A and Linux|
| Standard-system devices| Memory > 128 MB|  Linux       |

## Code Preparation

OpenHarmony has created repositories for vendors in openharmony-sig. To participate in the repository development, you need to use the following method to initialize and download the code.

```shell
repo init -u https://gitee.com/openharmony-sig/manifest.git -b master -m devboard.xml --no-repo-verify
```

The download steps for other resources are the same as those in the mainline version.

## Porting Procedure

- [Mini System SoC Porting Guide](porting-minichip.md)
  - [Porting Preparations](porting-chip-prepare.md)
    - [Before You Start](oem_transplant_chip_prepare_knows.md)
    - [Building Adaptation Process](porting-chip-prepare-process.md)
  - [Kernel Porting](porting-chip-kernel.md)
    - [Overview](porting-chip-kernel-overview.md)
    - [Basic Kernel Adaptation](porting-chip-kernel-adjustment.md)
    - [Kernel Porting Verification](porting-chip-kernel-verify.md)
  - [Board-Level OS Porting](porting-chip-board.md)
    - [Overview](porting-chip-board-overview.md)
    - [Board-Level Driver Adaptation](porting-chip-board-driver.md)
    - [Implementation of APIs at the HAL](porting-chip-board-hal.md)
    - [System Modules](porting-chip-board-component.md)
    - [Third-party Module Adaptation](porting-chip-board-bundle.md)
    - [XTS](porting-chip-board-xts.md)
  - [FAQ](porting-chip-faqs.md)
- [Small System SoC Porting Guide](porting-smallchip.md)
  - [Porting Preparations](porting-smallchip-prepare.md)
    - [Before You Start](porting-smallchip-prepare-needs.md)
    - [Compilation and Building](porting-smallchip-prepare-building.md)
  - [Kernel Porting](porting-smallchip-kernel.md)
    - [LiteOS Cortex-A](porting-smallchip-kernel-a.md)
    - [Linux Kernel](porting-smallchip-kernel-linux.md)
  - [Driver Porting](porting-smallchip-driver.md)
    - [Overview](porting-smallchip-driver-overview.md)
    - [Platform Driver Porting](porting-smallchip-driver-plat.md)
    - [Device Driver Porting](porting-smallchip-driver-oom.md)
- [Standard System Porting Guide](standard-system-porting-guide.md)

# Third-Party Library Porting Guide

- [Overview](porting-thirdparty-overview.md)
- [Porting a Library Built Using CMake](porting-thirdparty-cmake.md)
- [Porting a Library Built Using Makefile](porting-thirdparty-makefile.md)
