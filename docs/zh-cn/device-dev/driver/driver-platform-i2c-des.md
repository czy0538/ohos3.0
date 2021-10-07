# I2C<a name="ZH-CN_TOPIC_0000001052778273"></a>

-   [概述](#section5361140416)
    -   [接口说明](#section459052019177)

-   [使用指导](#section1695201514281)
    -   [使用流程](#section1338373417288)
    -   [打开I2C控制器](#section13751110132914)
    -   [进行I2C通信](#section9202183372916)
    -   [关闭I2C控制器](#section19481164133018)

-   [使用实例](#section5302202015300)

## 概述<a name="section5361140416"></a>

-   I2C\(Inter Integrated Circuit\)总线是由Philips公司开发的一种简单、双向二线制同步串行总线。
-   I2C以主从方式工作，通常有一个主设备和一个或者多个从设备，主从设备通过SDA\(SerialData\)串行数据线以及SCL\(SerialClock\)串行时钟线两根线相连，如[图1 ](#fig1135561232714)所示。

-   I2C数据的传输必须以一个起始信号作为开始条件，以一个结束信号作为传输的停止条件。数据传输以字节为单位，高位在前，逐个bit进行传输。
-   I2C总线上的每一个设备都可以作为主设备或者从设备，而且每一个设备都会对应一个唯一的地址，当主设备需要和某一个从设备通信时，通过广播的方式，将从设备地址写到总线上，如果某个从设备符合此地址，将会发出应答信号，建立传输。

-   I2C接口定义了完成I2C传输的通用方法集合，包括：

    -   I2C控制器管理:  打开或关闭I2C控制器
    -   I2C消息传输：通过消息传输结构体数组进行自定义传输

    **图 1**  I2C物理连线示意图<a name="fig1135561232714"></a>  
    ![](figure/I2C物理连线示意图.png "I2C物理连线示意图")


### 接口说明<a name="section459052019177"></a>

**表 1**  I2C驱动API接口功能介绍

<a name="table1731550155318"></a>
<table><thead align="left"><tr id="row4419501537"><th class="cellrowborder" valign="top" width="18.63%" id="mcps1.2.4.1.1"><p id="p641050105320"><a name="p641050105320"></a><a name="p641050105320"></a>功能分类</p>
</th>
<th class="cellrowborder" valign="top" width="28.03%" id="mcps1.2.4.1.2"><p id="p54150165315"><a name="p54150165315"></a><a name="p54150165315"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="53.339999999999996%" id="mcps1.2.4.1.3"><p id="p941150145313"><a name="p941150145313"></a><a name="p941150145313"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row34145016535"><td class="cellrowborder" rowspan="2" valign="top" width="18.63%" headers="mcps1.2.4.1.1 "><p id="p229610227124"><a name="p229610227124"></a><a name="p229610227124"></a>I2C控制器管理接口</p>
</td>
<td class="cellrowborder" valign="top" width="28.03%" headers="mcps1.2.4.1.2 "><p id="p19389143041518"><a name="p19389143041518"></a><a name="p19389143041518"></a>I2cOpen</p>
</td>
<td class="cellrowborder" valign="top" width="53.339999999999996%" headers="mcps1.2.4.1.3 "><p id="p8738101941716"><a name="p8738101941716"></a><a name="p8738101941716"></a>打开I2C控制器</p>
</td>
</tr>
<tr id="row5632152611414"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p143890309153"><a name="p143890309153"></a><a name="p143890309153"></a>I2cClose</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p573815197171"><a name="p573815197171"></a><a name="p573815197171"></a>关闭I2C控制器</p>
</td>
</tr>
<tr id="row15108165391412"><td class="cellrowborder" valign="top" width="18.63%" headers="mcps1.2.4.1.1 "><p id="p91084533141"><a name="p91084533141"></a><a name="p91084533141"></a>I2c消息传输接口</p>
</td>
<td class="cellrowborder" valign="top" width="28.03%" headers="mcps1.2.4.1.2 "><p id="p13901730101511"><a name="p13901730101511"></a><a name="p13901730101511"></a>I2cTransfer</p>
</td>
<td class="cellrowborder" valign="top" width="53.339999999999996%" headers="mcps1.2.4.1.3 "><p id="p12738111912171"><a name="p12738111912171"></a><a name="p12738111912171"></a>自定义传输</p>
</td>
</tr>
</tbody>
</table>

>![](../public_sys-resources/icon-note.gif) **说明：** 
>本文涉及的所有接口，仅限内核态使用，不支持在用户态使用。

## 使用指导<a name="section1695201514281"></a>

### 使用流程<a name="section1338373417288"></a>

使用I2C设备的一般流程如[图2](#fig166181128151112)所示。

**图 2**  I2C设备使用流程图<a name="fig166181128151112"></a>  


![](figure/zh-cn_image_0000001123509750.png)

### 打开I2C控制器<a name="section13751110132914"></a>

在进行I2C通信前，首先要调用I2cOpen打开I2C控制器。

DevHandle I2cOpen\(int16\_t number\);

**表 2**  I2cOpen参数和返回值描述

<a name="table7603619123820"></a>
<table><thead align="left"><tr id="row1060351914386"><th class="cellrowborder" valign="top" width="20.66%" id="mcps1.2.3.1.1"><p id="p14603181917382"><a name="p14603181917382"></a><a name="p14603181917382"></a><strong id="b16510829133012"><a name="b16510829133012"></a><a name="b16510829133012"></a>参数</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.34%" id="mcps1.2.3.1.2"><p id="p36031519183819"><a name="p36031519183819"></a><a name="p36031519183819"></a><strong id="b65222293309"><a name="b65222293309"></a><a name="b65222293309"></a>参数描述</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1960431983813"><td class="cellrowborder" valign="top" width="20.66%" headers="mcps1.2.3.1.1 "><p id="p3604719123817"><a name="p3604719123817"></a><a name="p3604719123817"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="79.34%" headers="mcps1.2.3.1.2 "><p id="p221392414442"><a name="p221392414442"></a><a name="p221392414442"></a>I2C控制器号</p>
</td>
</tr>
<tr id="row11410612183019"><td class="cellrowborder" valign="top" width="20.66%" headers="mcps1.2.3.1.1 "><p id="p460381915385"><a name="p460381915385"></a><a name="p460381915385"></a><strong id="b4349113243013"><a name="b4349113243013"></a><a name="b4349113243013"></a>返回值</strong></p>
</td>
<td class="cellrowborder" valign="top" width="79.34%" headers="mcps1.2.3.1.2 "><p id="p96031619153812"><a name="p96031619153812"></a><a name="p96031619153812"></a><strong id="b63502322308"><a name="b63502322308"></a><a name="b63502322308"></a>返回值描述</strong></p>
</td>
</tr>
<tr id="row15410111273017"><td class="cellrowborder" valign="top" width="20.66%" headers="mcps1.2.3.1.1 "><p id="p1060418195389"><a name="p1060418195389"></a><a name="p1060418195389"></a>NULL</p>
</td>
<td class="cellrowborder" valign="top" width="79.34%" headers="mcps1.2.3.1.2 "><p id="p760471912388"><a name="p760471912388"></a><a name="p760471912388"></a>打开I2C控制器失败</p>
</td>
</tr>
<tr id="row1241081213303"><td class="cellrowborder" valign="top" width="20.66%" headers="mcps1.2.3.1.1 "><p id="p5604719133811"><a name="p5604719133811"></a><a name="p5604719133811"></a>设备句柄</p>
</td>
<td class="cellrowborder" valign="top" width="79.34%" headers="mcps1.2.3.1.2 "><p id="p3604181933818"><a name="p3604181933818"></a><a name="p3604181933818"></a>打开的I2C控制器设备句柄</p>
</td>
</tr>
</tbody>
</table>

假设系统中存在8个I2C控制器，编号从0到7，那么我们现在获取3号控制器

```
DevHandle i2cHandle = NULL;  /* I2C控制器句柄 /

/* 打开I2C控制器 */
i2cHandle = I2cOpen(3);
if (i2cHandle == NULL) {
    HDF_LOGE("I2cOpen: failed\n");
    return;
}
```

### 进行I2C通信<a name="section9202183372916"></a>

消息传输

int32\_t I2cTransfer\(DevHandle handle, struct I2cMsg \*msgs, int16\_t count\);

**表 3**  I2cTransfer参数和返回值描述

<a name="table1934414174212"></a>
<table><thead align="left"><tr id="row1134415176216"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p13295152320217"><a name="p13295152320217"></a><a name="p13295152320217"></a><strong id="b17389641205115"><a name="b17389641205115"></a><a name="b17389641205115"></a>参数</strong></p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p1295112352115"><a name="p1295112352115"></a><a name="p1295112352115"></a><strong id="b19401541175118"><a name="b19401541175118"></a><a name="b19401541175118"></a>参数描述</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row5344101702113"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p19295132382111"><a name="p19295132382111"></a><a name="p19295132382111"></a>handle</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1051172572919"><a name="p1051172572919"></a><a name="p1051172572919"></a>I2C控制器设备句柄</p>
</td>
</tr>
<tr id="row17344171722117"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p9295122332113"><a name="p9295122332113"></a><a name="p9295122332113"></a>msgs</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p202951238218"><a name="p202951238218"></a><a name="p202951238218"></a>待传输数据的消息结构体数组</p>
</td>
</tr>
<tr id="row45812466213"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1659246112117"><a name="p1659246112117"></a><a name="p1659246112117"></a>count</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p259124622119"><a name="p259124622119"></a><a name="p259124622119"></a>消息数组长度</p>
</td>
</tr>
<tr id="row04701426105110"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p17295142322113"><a name="p17295142322113"></a><a name="p17295142322113"></a><strong id="b2159044145115"><a name="b2159044145115"></a><a name="b2159044145115"></a>返回值</strong></p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p142959232211"><a name="p142959232211"></a><a name="p142959232211"></a><strong id="b16160044135114"><a name="b16160044135114"></a><a name="b16160044135114"></a>返回值描述</strong></p>
</td>
</tr>
<tr id="row74701226125110"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p929532313211"><a name="p929532313211"></a><a name="p929532313211"></a>正整数</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p829512237217"><a name="p829512237217"></a><a name="p829512237217"></a>成功传输的消息结构体数目</p>
</td>
</tr>
<tr id="row204701126195115"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p12958234217"><a name="p12958234217"></a><a name="p12958234217"></a>负数</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1295192312112"><a name="p1295192312112"></a><a name="p1295192312112"></a>执行失败</p>
</td>
</tr>
</tbody>
</table>

I2C传输消息类型为I2cMsg，每个传输消息结构体表示一次读或写，通过一个消息数组，可以执行若干次的读写组合操作。

```
int32_t ret;
uint8_t wbuff[2] = { 0x12, 0x13 };
uint8_t rbuff[2] = { 0 };
struct I2cMsg msgs[2]; /* 自定义传输的消息结构体数组 */
msgs[0].buf = wbuff;    /* 写入的数据 */
msgs[0].len = 2;        /* 写入数据长度为2 */
msgs[0].addr = 0x5A;    /* 写入设备地址为0x5A */
msgs[0].flags = 0;      /* 传输标记为0，默认为写 */
msgs[1].buf = rbuff;    /* 要读取的数据 */
msgs[1].len = 2;        /* 读取数据长度为2 */
msgs[1].addr = 0x5A;    /* 读取设备地址为0x5A */
msgs[1].flags = I2C_FLAG_READ /* I2C_FLAG_READ置位 */
/* 进行一次自定义传输，传输的消息个数为2 */
ret = I2cTransfer(i2cHandle, msgs, 2);
if (ret != 2) {
    HDF_LOGE("I2cTransfer: failed, ret %d\n", ret);
    return;
}
```

>![](../public_sys-resources/icon-caution.gif) **注意：** 
>-   I2cMsg结构体中的设备地址不包含读写标志位，读写信息由flags成员变量的读写控制位传递。
>-   本函数不对消息结构体个数count做限制，其最大个数度由具体I2C控制器决定。
>-   本函数也不对每个消息结构体中的数据长度做限制，同样由具体I2C控制器决定。
>-   本函数可能会引起系统休眠，不允许在中断上下文调用

### 关闭I2C控制器<a name="section19481164133018"></a>

I2C通信完成之后，需要关闭I2C控制器，关闭函数如下所示：

void I2cClose\(DevHandle handle\); 

**表 4**  I2cClose参数和返回值描述

<a name="table72517953115"></a>
<table><thead align="left"><tr id="row1525793312"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p115402031153111"><a name="p115402031153111"></a><a name="p115402031153111"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p65406313319"><a name="p65406313319"></a><a name="p65406313319"></a>参数描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1926109193116"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p105419317318"><a name="p105419317318"></a><a name="p105419317318"></a>handle</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1213245577"><a name="p1213245577"></a><a name="p1213245577"></a>I2C控制器设备句柄</p>
</td>
</tr>
</tbody>
</table>

```
I2cClose(i2cHandle); /* 关闭I2C控制器 */
```

## 使用实例<a name="section5302202015300"></a>

本例程以操作开发板上的I2C设备为例，详细展示I2C接口的完整使用流程。

本例拟对Hi3516DV300某开发板上TouchPad设备进行简单的寄存器读写访问，基本硬件信息如下：

-   SOC：hi3516dv300。

-   Touch IC：I2C地址为0x38, IC内部寄存器位宽为1字节。

-   原理图信息：TouchPad设备挂接在3号I2C控制器下；IC的复位管脚为3号GPIO。

本例程首先对Touch IC进行复位操作（开发板上电默认会给TouchIC供电，本例程不考虑供电），然后对其内部寄存器进行随机读写，测试I2C通路是否正常。

>![](../public_sys-resources/icon-note.gif) **说明：** 
>本例程重点在于展示I2C设备访问流程，并验证I2C通路，所以对于设备寄存器读写值不做关注，读写寄存器导致的行为由设备自身决定。

示例如下：

```
#include "i2c_if.h"          /* I2C标准接口头文件 */
#include "gpio_if.h"         /* GPIO标准接口头文件 */
#include "hdf_log.h"         /* 标准日志打印头文件 */
#include "osal_io.h"         /* 标准IO读写接口头文件 */
#include "osal_time.h"       /* 标准延迟&睡眠接口头文件 */

/* 定义一个表示TP设备的结构体，存储i2c及gpio相关硬件信息 */
struct TpI2cDevice {
    uint16_t rstGpio;             /* 复位管脚 */
    uint16_t busId;               /* I2C总线号 */ 
    uint16_t addr;                /* I2C设备地址 */ 
    uint16_t regLen;              /* 寄存器字节宽度 */ 
    DevHandle i2cHandle;  /* I2C控制器句柄 */ 
};

/* I2C管脚io配置，需要查阅SOC寄存器手册  */
#define I2C3_DATA_REG_ADDR 0x112f008c /* 3号I2C控制器SDA管脚配置寄存器地址 */
#define I2C3_CLK_REG_ADDR 0x112f0090  /* 3号I2C控制器SCL管脚配置寄存器地址 */
#define I2C_REG_CFG 0x5f1             /* 3号I2C控制器SDA及SCL管脚配置值 */

static void TpSocIoCfg(void)
{
    /* 将3号I2C控制器对应两个管脚的IO功能设置为I2C */
    OSAL_WRITEL(I2C_REG_CFG, IO_DEVICE_ADDR(I2C3_DATA_REG_ADDR));
    OSAL_WRITEL(I2C_REG_CFG, IO_DEVICE_ADDR(I2C3_CLK_REG_ADDR));
}

/* 对TP的复位管脚进行初始化， 拉高维持20ms, 再拉底维持50ms，最后再拉高维持20ms， 完成复位动作 */
static int32_t TestCaseGpioInit(struct TpI2cDevice *tpDevice)
{
    int32_t ret;

    /* 设置复位管脚方向为输出 */ 
    ret = GpioSetDir(tpDevice->rstGpio, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set rst dir fail!:%d", __func__, ret);
        return ret;
    }

    ret = GpioWrite(tpDevice->rstGpio, GPIO_VAL_HIGH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set rst hight fail!:%d", __func__, ret);
        return ret;
    }
    OsalMSleep(20);

    ret = GpioWrite(tpDevice->rstGpio, GPIO_VAL_LOW);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set rst low fail!:%d", __func__, ret);
        return ret;
    }
    OsalMSleep(50);

    ret = GpioWrite(tpDevice->rstGpio, GPIO_VAL_HIGH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set rst high fail!:%d", __func__, ret);
        return ret;
    }
    OsalMSleep(20);

    return HDF_SUCCESS;
}

/* 基于I2cTransfer方法封装一个寄存器读写的辅助函数， 通过flag表示读或写 */
static int TpI2cReadWrite(struct TpI2cDevice *tpDevice, unsigned int regAddr,
    unsigned char *regData, unsigned int dataLen, uint8_t flag)
{
    int index = 0;
    unsigned char regBuf[4] = {0};
    struct I2cMsg msgs[2] = {0};

    /* 单双字节寄存器长度适配 */
    if (tpDevice->regLen == 1) { 
        regBuf[index++] = regAddr & 0xFF;
    } else {
        regBuf[index++] = (regAddr >> 8) & 0xFF;
        regBuf[index++] = regAddr & 0xFF;
    }

    /* 填充I2cMsg消息结构 */ 
    msgs[0].addr = tpDevice->addr;
    msgs[0].flags = 0; /* 标记为0，表示写入 */ 
    msgs[0].len = tpDevice->regLen;
    msgs[0].buf = regBuf;

    msgs[1].addr = tpDevice->addr;
    msgs[1].flags = (flag == 1) ? I2C_FLAG_READ : 0; /* 添加读标记位，表示读取 */ 
    msgs[1].len = dataLen;
    msgs[1].buf = regData;

    if (I2cTransfer(tpDevice->i2cHandle, msgs, 2) != 2) {
        HDF_LOGE("%s: i2c read err", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

/* TP寄存器读函数 */ 
static inline int TpI2cReadReg(struct TpI2cDevice *tpDevice, unsigned int regAddr,
    unsigned char *regData, unsigned int dataLen)
{
    return TpI2cReadWrite(tpDevice, regAddr, regData, dataLen, 1);
}

/* TP寄存器写函数 */ 
static inline int TpI2cWriteReg(struct TpI2cDevice *tpDevice, unsigned int regAddr,
    unsigned char *regData, unsigned int dataLen)
{
    return TpI2cReadWrite(tpDevice, regAddr, regData, dataLen, 0);
}

/* I2C例程总入口 */ 
static int32_t TestCaseI2c(void)
{
    int32_t i;
    int32_t ret;
    unsigned char bufWrite[7] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xA, 0xB, 0xC };
    unsigned char bufRead[7] = {0};
    static struct TpI2cDevice tpDevice;

    /* IO管脚功能配置 */
    TpSocIoCfg();

    /* TP设备信息初始化 */ 
    tpDevice.rstGpio = 3;
    tpDevice.busId = 3;
    tpDevice.addr = 0x38;
    tpDevice.regLen = 1;
    tpDevice.i2cHandle = NULL;

    /* GPIO管脚初始化 */ 
    ret = TestCaseGpioInit(&tpDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: gpio init fail!:%d", __func__, ret);
        return ret;
    }

    /* 打开I2C控制器 */ 
    tpDevice.i2cHandle = I2cOpen(tpDevice.busId);
    if (tpDevice.i2cHandle == NULL) {
        HDF_LOGE("%s: Open I2c:%u fail!", __func__, tpDevice.busId);
        return -1;
    }

    /* 向TP-IC的0xD5寄存器连续写7字节数据 */ 
    ret = TpI2cWriteReg(&tpDevice, 0xD5, bufWrite, 7);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: tp i2c write reg fail!:%d", __func__, ret);
        I2cClose(tpDevice.i2cHandle);
        return -1;
    }
    OsalMSleep(10);

    /* 从TP-IC的0xD5寄存器连续读7字节数据 */ 
    ret = TpI2cReadReg(&tpDevice, 0xD5, bufRead, 7);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: tp i2c read reg fail!:%d", __func__, ret);
        I2cClose(tpDevice.i2cHandle);
        return -1;
    }

    HDF_LOGE("%s: tp i2c write&read reg success!", __func__);
    for (i = 0; i < 7; i++) {
        HDF_LOGE("%s: bufRead[%d] = 0x%x", __func__, i, bufRead[i]);
    }

    /* 访问完毕关闭I2C控制器 */ 
    I2cClose(tpDevice.i2cHandle);
    return ret;
}
```

