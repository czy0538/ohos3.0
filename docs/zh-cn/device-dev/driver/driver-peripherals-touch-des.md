# TOUCHSCREEN<a name="ZH-CN_TOPIC_0000001052857350"></a>

-   [概述](#section175431838101617)
    -   [接口说明](#section17667171301711)

-   [开发指导](#section65745222184)
    -   [开发步骤](#section865734181916)

-   [开发实例](#section263714411191)
    -   [设备描述配置](#section18249155619195)
    -   [板级配置及器件私有配置](#section3571192072014)
    -   [添加器件驱动](#section6356758162015)


## 概述<a name="section175431838101617"></a>

-   **Touchscreen驱动主要任务**

    Touchscreen驱动用于驱动触摸屏使其正常工作，该驱动主要完成如下工作：对触摸屏驱动IC进行上电、配置硬件管脚并初始化其状态、注册中断、配置通信接口（I2C或SPI）、设定input相关配置、下载及更新固件等操作。


-   **Touchscreen驱动层次说明**

    本节主要介绍基于input驱动模型开发touchscreen器件驱动，其整体的框架模型如[图1](#fig6251184817261)。

    Input驱动模型基于HDF驱动框架、PLATFORM接口、OSAL接口进行开发，向上对接规范化的驱动接口HDI（Hardware Driver Interface）层，通过Input-HDI层对外提供硬件能力，即上层input service可以通过HDI接口层获取相应的驱动能力，进而操控touchscreen等输入设备。


**图 1**  基于HDF驱动框架的input驱动模型<a name="fig6251184817261"></a>  
![](figure/基于HDF驱动框架的input驱动模型.png "基于HDF驱动框架的input驱动模型")

-   **Input驱动模型介绍**

    Input驱动模型核心部分由设备管理层、公共驱动层、器件驱动层组成。器件产生的数据借助平台数据通道能力从内核传递到用户态，驱动模型通过配置文件适配不同器件及硬件平台，提高开发者的器件驱动开发效率。如下部分为模型各部分的说明：

    （1）input设备管理：为各类输入设备驱动提供input设备的注册、注销接口，同时统一管理input设备列表；

    （2）input平台驱动：指各类input设备的公共抽象驱动（例如触摸屏的公共驱动），负责对板级硬件进行初始化、硬件中断处理、向manager注册input设备等；

    （3）input器件驱动：指各器件厂家的差异化驱动，通过适配平台驱动预留的差异化接口，实现器件驱动开发量最小化；

    （4）input数据通道：提供一套通用的数据上报通道，各类别的input设备驱动均可用此通道上报input事件；

    （5）input配置解析：负责对input设备的板级配置及器件私有配置进行解析及管理。


-   **基于HDF驱动框架开发器件驱动的优势**

    在HDF（Hardware Driver Foundation）[驱动管理框架](driver-hdf-development.md)的基础上，input驱动模型调用OSAL接口层和Platfom接口层提供的基础接口进行开发，包括bus通信接口、操作系统原生接口（memory、lock、thread、timer等）。由于OSAL接口和Platform接口屏蔽了芯片平台差异，所以基于input驱动模型实现的touchscreen驱动可以进行跨平台、跨OS迁移，以便逐步实现驱动的一次开发，多端部署。


### 接口说明<a name="section17667171301711"></a>

Touchscreen器件的硬件接口相对简单，根据PIN脚的属性，可以简单分为如下三类:

-   电源接口
-   IO控制接口
-   通信接口

**图 2**  Touchscreen器件常用管脚<a name="fig1290384314416"></a>  
![](figure/Touchscreen器件常用管脚.png "Touchscreen器件常用管脚")

如上图所示的三类接口，分别做简要说明如下：

1.  **电源接口**
    -   LDO\_1P8：1.8v数字电路
    -   LDO\_3P3：3.3v模拟电路

        通常情况下，touchscreen驱动IC和LCD驱动IC是相互分离的，这种情况下，touchscreen驱动IC一般同时需要1.8v和3.3v两路供电。随着芯片演进，业内已有touchscreen驱动IC和LCD驱动IC集成在一颗IC中的芯片案例，对touchscreen而言，只需要关注1.8v供电即可，其内部需要的3.3v电源，会在驱动IC内部从LCD的VSP电源（典型值5.5V）中分出来。

2.  **IO控制接口**
    -   Reset：reset管脚，用于在系统休眠、唤醒时，由主机侧对驱动IC进行复位操作。
    -   INT：中断管脚，需要在驱动初始化时，配置为输入上拉状态。在驱动IC检测到外部触摸信号后，通过操作中断管脚来触发中断，器件驱动则会在中断处理函数中进行报点数据读取等操作。

3.  **通信接口**
    -   I2C：由于touchscreen的报点数据量相对较少，所以一般选用I2C方式传输数据。I2C的具体协议及对应操作接口，可以参考Platform接口层中的[“I2C”使用指南](driver-platform-i2c-des.md#section1695201514281)。
    -   SPI：部分厂商，由于需要传递的数据不止报点坐标，而是需要获取基础容值，数据量较大，所以会选用SPI通信方式。SPI的具体协议及对应操作接口，可以参考Platform接口层中的[“SPI” 使用指南](driver-platform-spi-des.md#section71363452477)。


## 开发指导<a name="section65745222184"></a>

Input驱动模型是基于HDF框架、Platform接口和OSAL接口开发，不区分操作系统和芯片平台，为touchscreen等输入器件提供统一的驱动开发架构。

-   如下以touchscreen器件驱动为例，说明input驱动模型的完整加载流程：

    （1）设备描述配置：由开发者参考已有模板进行设备描述配置，包括驱动加载顺序、板级硬件信息、器件私有数据信息等。

    （2）加载input设备管理驱动：input设备管理驱动由HDF驱动加载，完成设备manager的创建并对其初始化。

    （3）加载平台驱动：平台驱动由HDF框架加载，主要完成板级配置解析及硬件初始化，并提供器件注册接口。

    （4）加载器件驱动：器件驱动也由HDF框架加载，完成器件设备的实例化，包括器件私有配置解析和平台预留的差异化接口适配。

    （5）器件设备向平台驱动注册：将实例化的器件设备向平台驱动注册，实现设备和驱动的绑定，并完成中断注册、上下电等器件初始化工作。

    （6）input设备注册：在器件初始化完成后，实例化input设备，并将其注册到input manager进行管理。


### 开发步骤<a name="section865734181916"></a>

1.  设备描述配置

    目前Input驱动基于HDF驱动框架编写，驱动的加载启动由HDF驱动管理框架统一处理。首先需要在对应的配置文件中，将驱动信息注册进去，如是否加载、加载优先级，此后HDF驱动框架会逐一启动注册过的驱动模块。驱动的相关配置请参考[HDF驱动框架配置指导](driver-hdf-development.md#section1969312275533)。

2.  板级配置及Touchscreen器件私有配置

    配置对应的IO管脚功能，例如对单板上为touchscreen设计预留的I2C Pin脚，需设置对应的寄存器，使其选择I2C的通信功能。

3.  实现器件差异化适配接口

    根据硬件单板设计的通信接口，使用Platform接口层提供的管脚操作接口配置对应的复位管脚、中断管脚以及电源操作，对于GPIO的操作，可参考[GPIO操作接口指导](driver-platform-gpio-des.md#section259614242196)


## 开发实例<a name="section263714411191"></a>

本实例提供touchscreen驱动开发示例，并简要对具体关键点进行开发说明。

### 设备描述配置<a name="section18249155619195"></a>

如下配置主要包含input驱动模型各模块层级信息，具体原理可参考[HDF驱动开发指南](driver-hdf-development.md)，HDF框架依据该配置信息实现对Input模型各模块的依次加载等。

```
input :: host {
            hostName = "input_host";
            priority = 100;
            device_input_manager :: device {
                device0 :: deviceNode {
                    policy = 2;        // 向外发布服务
                    priority = 100;    // 加载优先级，在input模块内，manager模块优先级应为最高
                    preload = 0;       // 加载该驱动 0:加载 1:不加载
                    permission = 0660;
                    moduleName = "HDF_INPUT_MANAGER";
                    serviceName = "input_dev_manager";
                    deviceMatchAttr = "";
                }
            }
            device_hdf_touch :: device {
                device0 :: deviceNode {
                    policy = 2;
                    priority = 120;
                    preload = 0;
                    permission = 0660;
                    moduleName = "HDF_TOUCH";
                    serviceName = "event1";
                    deviceMatchAttr = "touch_device1";
                }
            }

            device_touch_chip :: device {
                device0 :: deviceNode {
                    policy = 0;
                    priority = 130;
                    preload = 0;
                    permission = 0660;
                    moduleName = "HDF_TOUCH_SAMPLE";
                    serviceName = "hdf_touch_sample_service";
                    deviceMatchAttr = "zsj_sample_5p5";
                }
            }
}
```

### 板级配置及器件私有配置<a name="section3571192072014"></a>

如下配置包含板级硬件配置及器件私有数据配置，实际业务开发时，可根据具体需求增删及修改如下配置文件信息。

```
root {
    input_config {
        touchConfig {
            touch0 {
                boardConfig {
                    match_attr = "touch_device1";
                    inputAttr {
                        inputType = 0;           // 0代表触摸屏
                        solutionX = 480; 
                        solutionY = 960;
                        devName = "main_touch";  // 设备名称
                    }
                    busConfig {
                        busType = 0;             // 0代表I2C
                        busNum = 6;
                        clkGpio = 86;
                        dataGpio = 87;
                        i2cClkIomux = [0x114f0048, 0x403];  // i2c_clk对应pin的寄存器配置
                        i2cDataIomux = [0x114f004c, 0x403]; // i2c_data对应pin的寄存器配置
                    }
                    pinConfig {
                        rstGpio = 3;
                        intGpio = 4;
                        rstRegCfg = [0x112f0094, 0x400];  // reset对应pin的寄存器配置
                        intRegCfg = [0x112f0098, 0x400];  // interrupt对应pin的寄存器配置
                    }
                    powerConfig {
                        vccType = 2;       // 1代表LDO、2代表GPIO、3代表PMIC
                        vccNum = 20;       // GPIO号为20
                        vccValue = 1800;   // 电压幅值为1800mV
                        vciType = 1;
                        vciNum = 12;
                        vciValue = 3300;
                    }
                    featureConfig {
                        capacitanceTest = 0;
                        gestureMode = 0;
                        gloverMOde = 0;
                        coverMode = 0;
                        chargerMode = 0;
                        knuckleMode = 0;
                    }
                }
                chipConfig {
                    template touchChip {
                        match_attr = "";
                        chipName = "sample";
                        vendorName = "zsj";
                        chipInfo = "AAAA11222";  // 1~4字符代表产品名，5~6字符代表IC型号，7~9字符代表模型型号
                        busType = 0;
                        deviceAddr = 0x5D;
                        irqFlag = 2;             // 1代表上升沿触发，2代表下降沿触发，4代表高电平触发，8代表低电平触发
                        maxSpeed = 400;
                        chipVersion = 0;
                        powerSequence {
                            /* 上电时序的配置含义说明：
                               [类型, 状态, 方向 , 延时]
                               <type> 0代表空，1代表vcc电源（1.8V），2代表VCI电源（3.3V），3代表复位管脚，4代表中断管脚
                               <status> 0代表下电或拉低，1代表上电或拉高，2代表无操作
                               <dir> 0代表输入方向，1代表输出方向，2代表无操作
                               <delay> 代表延时多少毫秒, 例如20代表延时20ms
                             */
                            powerOnSeq = [4, 0, 1, 0,
                                         3, 0, 1, 10,
                                         3, 1, 2, 60,
                                         4, 2, 0, 0];
                            suspendSeq = [3, 0, 2, 10];
                            resumeSeq = [3, 1, 2, 10];
                            powerOffSeq = [3, 0, 2, 10,
                                           1, 0, 2, 20];
                        }
                    }
                    chip0 :: touchChip {
                        match_attr = "zsj_sample_5p5";
                        chipInfo = "ZIDN45100";
                        chipVersion = 0;
                    }
                }
            }
        }
    }
}
```

### 添加器件驱动<a name="section6356758162015"></a>

在器件驱动中，主要实现了平台预留的差异化接口，以器件数据获取及解析进行示例说明。具体开发过程，需要根据实际使用的单板及器件进行适配。

```
/* 将从器件中读取到的报点数据解析为坐标 */
static void ParsePointData(ChipDevice *device, FrameData *frame, uint8_t *buf, uint8_t pointNum)
{
    int32_t resX = device->driver->boardCfg->attr.resolutionX;
    int32_t resY = device->driver->boardCfg->attr.resolutionY;

    for (int32_t i = 0; i < pointNum; i++) {
        frame->fingers[i].y = (buf[GT_POINT_SIZE * i + GT_X_LOW] & ONE_BYTE_MASK) |
                              ((buf[GT_POINT_SIZE * i + GT_X_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
        frame->fingers[i].x = (buf[GT_POINT_SIZE * i + GT_Y_LOW] & ONE_BYTE_MASK) |
                              ((buf[GT_POINT_SIZE * i + GT_Y_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
        frame->fingers[i].valid = true;
    }
}
/* 从器件中获取报点数据 */
static int32_t ChipDataHandle(ChipDevice *device)
{
    int32_t ret;
    uint8_t touchStatus = 0;
    uint8_t pointNum;
    uint8_t buf[GT_POINT_SIZE * MAX_SUPPORT_POINT] = {0};
    InputI2cClient *i2cClient = &device->driver->i2cClient;
    uint8_t reg[GT_ADDR_LEN] = {0};
    FrameData *frame = &device->driver->frameData;
    reg[0] = (GT_BUF_STATE_ADDR >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    reg[1] = GT_BUF_STATE_ADDR & ONE_BYTE_MASK;
    ret = InputI2cRead(i2cClient, reg, GT_ADDR_LEN, &touchStatus, 1);
    if (ret < 0 || touchStatus == GT_EVENT_INVALID) {
        return HDF_FAILURE;
    }
    OsalMutexLock(&device->driver->mutex);
    (void)memset_s(frame, sizeof(FrameData), 0, sizeof(FrameData));
    if (touchStatus == GT_EVENT_UP) {
        frame->realPointNum = 0;
        frame->definedEvent = TOUCH_UP;
        goto exit;
    }
    reg[0] = (GT_X_LOW_BYTE_BASE >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    reg[1] = GT_X_LOW_BYTE_BASE & ONE_BYTE_MASK;
    pointNum = touchStatus & GT_FINGER_NUM_MASK;
    if (pointNum <= 0 || pointNum > MAX_SUPPORT_POINT) {
        HDF_LOGE("%s: pointNum is invalid, %d", __func__, pointNum);
        (void)ChipCleanBuffer(i2cClient);
        OsalMutexUnlock(&device->driver->mutex);
        return HDF_FAILURE;
    }
    frame->realPointNum = pointNum;
    frame->definedEvent = TOUCH_DOWN;
    /* 从寄存器中读取报点值 */
    (void)InputI2cRead(i2cClient, reg, GT_ADDR_LEN, buf, GT_POINT_SIZE * pointNum);
    /* 解析报点值 */
    ParsePointData(device, frame, buf, pointNum);
exit:
    OsalMutexUnlock(&device->driver->mutex);
    if (ChipCleanBuffer(i2cClient) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct TouchChipOps g_sampleChipOps = {
    .Init = ChipInit,
    .Detect = ChipDetect,
    .Resume = ChipResume,
    .Suspend = ChipSuspend,
    .DataHandle = ChipDataHandle,
};

static TouchChipCfg *ChipConfigInstance(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = (TouchChipCfg *)OsalMemAlloc(sizeof(TouchChipCfg));
    if (chipCfg == NULL) {
        HDF_LOGE("%s: instance chip config failed", __func__);
        return NULL;
    }
    (void)memset_s(chipCfg, sizeof(TouchChipCfg), 0, sizeof(TouchChipCfg));
    /* 解析器件私有配置 */
    if (ParseTouchChipConfig(device->property, chipCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: parse chip config failed", __func__);
        OsalMemFree(chipCfg);
        chipCfg = NULL;
    }
    return chipCfg;
}

static ChipDevice *ChipDeviceInstance(void)
{
    ChipDevice *chipDev = (ChipDevice *)OsalMemAlloc(sizeof(ChipDevice));
    if (chipDev == NULL) {
        HDF_LOGE("%s: instance chip device failed", __func__);
        return NULL;
    }
    (void)memset_s(chipDev, sizeof(ChipDevice), 0, sizeof(ChipDevice));
    return chipDev;
}

static void FreeChipConfig(TouchChipCfg *config)
{
    if (config->pwrSeq.pwrOn.buf != NULL) {
        OsalMemFree(config->pwrSeq.pwrOn.buf);
    }
    if (config->pwrSeq.pwrOff.buf != NULL) {
        OsalMemFree(config->pwrSeq.pwrOff.buf);
    }
    OsalMemFree(config);
}

static int32_t HdfSampleChipInit(struct HdfDeviceObject *device)
{
    TouchChipCfg *chipCfg = NULL;
    ChipDevice *chipDev = NULL;
    HDF_LOGE("%s: enter", __func__);
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    /* 器件私有配置解析 */
    chipCfg = ChipConfigInstance(device);
    if (chipCfg == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    /* 器件设备实例化 */
    chipDev = ChipDeviceInstance();
    if (chipDev == NULL) {
        goto freeCfg;
    }
    chipDev->chipCfg = chipCfg;
    chipDev->ops = &g_sampleChipOps;
    chipDev->chipName = chipCfg->chipName;
    chipDev->vendorName = chipCfg->vendorName;

   /* 器件设备注册到平台驱动 */
    if (RegisterChipDevice(chipDev) != HDF_SUCCESS) {
        goto freeDev;
    }
    HDF_LOGI("%s: exit succ, chipName = %s", __func__, chipCfg->chipName);
    return HDF_SUCCESS;

freeDev:
    OsalMemFree(chipDev);
freeCfg:
    FreeChipConfig(chipCfg);
    return HDF_FAILURE;
}

struct HdfDriverEntry g_touchSampleChipEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_TOUCH_SAMPLE",
    .Init = HdfSampleChipInit,
};

HDF_INIT(g_touchSampleChipEntry);
```

