# vendor_ohos

## 介绍

该仓库托管OpenHarmony社区开发的，而非某个厂商单独开发的典型产品样例代码，主要
包括类似QEMU(Quick Emulator)的图形、软总线等特性产品的开发。

## 软件架构

支持QEMU的ARMv7-a架构的virt平台的图形显示产品样例，支持RISC-V架构的virt平台的
样例，以及Cortex-M4架构的样例。

代码路径：

```
device/qemu/                          --- device_qemu 仓库路径
├── arm_mps2_an386                    --- Cortex-M4 架构的 MPS2-AN386 单板模拟
├── arm_virt                          --- ARMv7-a 架构的 virt 单板模拟
├── drivers                           --- virt驱动目录
└── riscv32_virt                      --- RISC-V 架构的 virt 单板模拟
vendor/ohos/                          --- vendor_ohos 仓库路径
├── qemu_small_system_demo            --- 小型系统的样例
├── qemu_mini_system_demo             --- 微型系统的样例
└── qemu_riscv_mini_system_demo       --- 微型系统的 RISC-V 架构的样例
```

## 安装教程

[QEMU的安装参考链接](https://gitee.com/openharmony/device_qemu#qemu%E5%AE%89%E8%A3%85)

## 使用说明

1. 运行`hb set`命令选择对应的产品样例。
```
ohos
 > qemu_small_system_demo
   qemu_mini_system_demo
```

其中，

`qemu_small_system_demo`表示小型系统的样例，它会集成OpenHarmony上层组件，例如：
graphic、foundation等。

`qemu_mini_system_demo`表示微型系统的样例，它会集成OpenHarmony上层组件，例如：
samgr、hilog等。

2. 运行`hb build`进行编译。

3. 将编译出的镜像基于qemu来运行，方法如下：

3.1 `qemu_small_system_demo`的运行方法是

```
./qemu-init
./qemu-run
```

再用`vnc-client`连接该主机的5920端口

3.2 `qemu_mini_system_demo`的运行方法是

```
cd device/qemu/arm_mps2_an386
./qemu_run.sh ../../../out/arm_mps2_an386/bin/liteos
```

## 贡献

[如何参与](https://gitee.com/openharmony/docs/blob/HEAD/zh-cn/contribute/%E5%8F%82%E4%B8%8E%E8%B4%A1%E7%8C%AE.md)

[Commit message规范](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83?sort_id=4042860)

## 相关仓

[device\_qemu](https://gitee.com/openharmony/device_qemu/blob/HEAD/README_zh.md)

