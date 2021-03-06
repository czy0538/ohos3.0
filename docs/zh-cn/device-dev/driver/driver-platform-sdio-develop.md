# SDIO

- [概述](#1)
- [开发步骤](#2)
- [开发实例](#3)

## 概述 <a name="1"></a>

SDIO由SD卡发展而来，被统称为mmc（MultiMediaCard），相关技术差别不大,在HDF框架中，
SDIO的接口适配模式采用独立服务模式，在这种模式下，每一个设备对象会独立发布一个设备服务来处理外部访问，设备管理器收到API的访问请求之后，通过提取该请求的参数，达到调用实际设备对象的相应内部方法的目的。

独立服务模式可以直接借助HDFDeviceManager的服务管理能力，但需要为每个设备单独配置设备节点，增加内存占用。

图 1 独立服务模式结构图
![image1](figure/独立服务模式.png)

## 开发步骤 <a name="2"></a>

SDIO模块适配HDF框架的三个环节是配置属性文件，实例化驱动入口，以及填充核心层接口函数。


1. **实例化驱动入口：**   
    - 实例化HdfDriverEntry结构体成员。
    - 调用HDF_INIT将HdfDriverEntry实例化对象注册到HDF框架中。

2. **配置属性文件：**   
   
    - 在device_info.hcs文件中添加deviceNode描述。
    - 【可选】添加sdio_config.hcs器件属性文件。

3. **实例化SDIO控制器对象：**   
   
    - 初始化SdioDevice成员。
    - 实例化SdioDevice成员SdioDeviceOps，其定义和成员说明见下
    
4. **驱动调试：**
    - 【可选】针对新增驱动程序，建议验证驱动基本功能，例如SDIO控制状态，中断响应情况等。

> ![](../public_sys-resources/icon-note.gif) **说明：** 
> 
> SdioDeviceOps定义
> 
> ```c
> // 函数模板
> struct SdioDeviceOps {
>   int32_t (*incrAddrReadBytes)(struct SdioDevice *dev, uint8_t *data, uint32_t addr, uint32_t size);
>   int32_t (*incrAddrWriteBytes)(struct SdioDevice *dev, uint8_t *data, uint32_t addr, uint32_t size);
>   int32_t (*fixedAddrReadBytes)(struct SdioDevice *dev, uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen);
>   int32_t (*fixedAddrWriteBytes)(struct SdioDevice *dev, uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen);
>   int32_t (*func0ReadBytes)(struct SdioDevice *dev, uint8_t *data, uint32_t addr, uint32_t size);
>   int32_t (*func0WriteBytes)(struct SdioDevice *dev, uint8_t *data, uint32_t addr, uint32_t size);
>   int32_t (*setBlockSize)(struct SdioDevice *dev, uint32_t blockSize);
>   int32_t (*getCommonInfo)(struct SdioDevice *dev, SdioCommonInfo *info, uint32_t infoType);
>   int32_t (*setCommonInfo)(struct SdioDevice *dev, SdioCommonInfo *info, uint32_t infoType);
>   int32_t (*flushData)(struct SdioDevice *dev);
>   int32_t (*enableFunc)(struct SdioDevice *dev);
>   int32_t (*disableFunc)(struct SdioDevice *dev);
>   int32_t (*claimIrq)(struct SdioDevice *dev, SdioIrqHandler *irqHandler);
>   int32_t (*releaseIrq)(struct SdioDevice *dev);
>   int32_t (*findFunc)(struct SdioDevice *dev, struct SdioFunctionConfig *configData);
>   int32_t (*claimHost)(struct SdioDevice *dev);
>   int32_t (*releaseHost)(struct SdioDevice *dev);
> };
> ```
> 
> 表1 SdioDeviceOps结构体成员的回调函数功能说明
> 
> |函数|入参|出参|返回值|功能|
> |-|-|-|-|-|
> |incrAddrReadBytes,  |**dev**: 结构体指针,SDIO设备控制器;<br />**addr**: uint32_t,地址值;<br />**size**: uint32_t,大小 |**data**: uint8_t指针,传出值;|HDF_STATUS相关状态| 从指定的SDIO地址增量读取给定长度的数据 |
> |incrAddrWriteBytes, |**dev**: 结构体指针,SDIO设备控制器;<br />**data**: uint8_t指针,传入值;<br />**addr**: uint32_t,地址值;<br />**size**: uint32_t,大小 |无|HDF_STATUS相关状态| 将给定长度的数据增量写入指定的SDIO地址 |
> |fixedAddrReadBytes, |**dev**: 结构体指针,SDIO设备控制器;<br />**addr**: uint32_t,地址值;<br />**size**: uint32_t,大小;<br />**scatterLen**: uint32_t,数据长度；|**data**: uint8_t指针,传出值;|HDF_STATUS相关状态| 从固定SDIO地址读取给定长度的数据。 |
> |fixedAddrWriteBytes,|**dev**: 结构体指针,SDIO设备控制器;<br />**data**: uint8_t指针,传入值;<br />**addr**: uint32_t,地址值;<br />**size**: uint32_t,大小;<br />**scatterLen**: uint32_t,数据长度;|无|HDF_STATUS相关状态| 将给定长度的数据写入固定SDIO地址 |
> |func0ReadBytes,     |**dev**: 结构体指针,SDIO设备控制器;<br />**addr**: uint32_t,地址值;<br />**size**: uint32_t,大小; |**data**: uint8_t指针,传出值;|HDF_STATUS相关状态| 从SDIO函数0的地址空间读取给定长度的数据。 |
> |func0WriteBytes,    |**dev**: 结构体指针,SDIO设备控制器;<br />**data**: uint8_t指针,传入值;<br />**addr**: uint32_t,地址值;<br />**size**: uint32_t,大小; |无|HDF_STATUS相关状态| 将给定长度的数据写入SDIO函数0的地址空间。 |
> |setBlockSize,       |**dev**: 结构体指针,SDIO设备控制器;<br />**blockSize**: uint32_t,Block大小 |无|HDF_STATUS相关状态| 设置block大小|
> |getCommonInfo,      |**dev**: 联合体指针,SDIO设备控制器;<br />**infoType**: uint32_t,info类型; |**info**:  结构体指针,传出SdioFuncInfo信息;|HDF_STATUS相关状态| 获取CommonInfo，说明见下 |
> |setCommonInfo,      |**dev**: 结构体指针,SDIO设备控制器; <br />**info**: 联合体指针，SdioFuncInfo信息传入;<br />**infoType**: uint32_t,info类型; |无|HDF_STATUS相关状态| 设置CommonInfo，说明见下 |
> |flushData,          |**dev**: 结构体指针,SDIO设备控制器; |无|HDF_STATUS相关状态|当SDIO需要重新初始化或发生意外错误时调用的函数|
> |enableFunc,         |**dev**: 结构体指针,SDIO设备控制器; |无|HDF_STATUS相关状态|使能SDIO设备 |
> |disableFunc,        |**dev**: 结构体指针,SDIO设备控制器; |无|HDF_STATUS相关状态|去使能SDIO设备 |
> |claimIrq,           |**dev**: 结构体指针,SDIO设备控制器;<br />**irqHandler**: void函数指针; |无|HDF_STATUS相关状态|注册SDIO中断 |
> |releaseIrq,         |**dev**: 结构体指针,SDIO设备控制器; |无|HDF_STATUS相关状态|释放SDIO中断|
> |findFunc,           |**dev**: 结构体指针,SDIO设备控制器;<br />**configData**: 结构体指针, SDIO函数关键信息 |无|HDF_STATUS相关状态|寻找匹配的funcNum|
> |claimHost,          |**dev**: 结构体指针,SDIO设备控制器; |无|HDF_STATUS相关状态|独占HOST |
> |releaseHost,        |**dev**: 结构体指针,SDIO设备控制器; |无|HDF_STATUS相关状态|释放HOST |
>
> >![](../public_sys-resources/icon-note.gif) **CommonInfo说明：** 
> > 包括maxBlockNum(单个request中最大block数), maxBlockSize(单个block最大字节数), maxRequestSize(单个Request最大字节数), enTimeout(最大超时时间,毫秒), funcNum(功能编号1~7), irqCap(IRQ capabilities), (void \*)data.

## 开发实例 <a name="3"></a>

下方将以sdio_adapter.c为示例，展示需要厂商提供哪些内容来完整实现设备功能。

1. 驱动开发首先需要实例化驱动入口，驱动入口必须为HdfDriverEntry（在 hdf_device_desc.h 中定义）类型的全局变量，且moduleName要和device_info.hcs中保持一致。HDF框架会将所有加载的驱动的HdfDriverEntry对象首地址汇总，形成一个类似数组的段地址空间，方便上层调用。
   
    一般在加载驱动时HDF会先调用Bind函数，再调用Init函数加载该驱动。当Init调用异常时，HDF框架会调用Release释放驱动资源并退出。

- SDIO 驱动入口参考

    ```c 
    struct HdfDriverEntry g_sdioDriverEntry = {
        .moduleVersion = 1,
        .Bind = Hi35xxLinuxSdioBind,      //见Bind参考
        .Init = Hi35xxLinuxSdioInit,      //见Init参考
        .Release = Hi35xxLinuxSdioRelease,//见Release参考
        .moduleName = "HDF_PLATFORM_SDIO",//【必要 且与 HCS文件中里面的moduleName匹配】
    };
    //调用HDF_INIT将驱动入口注册到HDF框架中 
    HDF_INIT(g_sdioDriverEntry);
    ```

2. 完成驱动入口注册之后，下一步请在device_info.hcs文件中添加deviceNode信息，并在 sdio_config.hcs 中配置器件属性。deviceNode信息与驱动入口注册相关，器件属性值与核心层SdioDevice成员的默认值或限制范围有密切关系。
   
    **本例只有一个SDIO控制器，如有多个器件信息，则需要在device_info文件增加deviceNode信息，以及在sdio_config文件中增加对应的器件属性**。

- device_info.hcs 配置参考

    ```c
     root {
      device_info {
        match_attr = "hdf_manager";
        platform :: host {
          hostName = "platform_host";
          priority = 50;
          device_sdio :: device {
            device0 :: deviceNode {
              policy = 1;
              priority = 70;
              permission = 0644;
              moduleName = "HDF_PLATFORM_SDIO";   //【必要】用于指定驱动名称，需要与驱动Entry中的moduleName一致；
              serviceName = "HDF_PLATFORM_MMC_2"; //【必要】驱动对外发布服务的名称，必须唯一
              deviceMatchAttr = "hisilicon_hi35xx_sdio_0";//【必要】用于配置控制器私有数据，要与sdio_config.hcs中对应控制器保持一致
            }
          }
        }
      }
    }
    ```
    
- sdio_config.hcs 配置参考

    ```c
    root {
      platform {
        sdio_config {
          template sdio_controller {
            match_attr = "";
            hostId = 2;  //【必要】模式固定为2，在mmc_config.hcs有介绍
            devType = 2; //【必要】模式固定为2，在mmc_config.hcs有介绍
          }
          controller_0x2dd1 :: sdio_controller {
            match_attr = "hisilicon_hi35xx_sdio_0";//【必要】需要和device_info.hcs中的deviceMatchAttr值一致
        }
      }
    }
    ```

3. 完成驱动入口注册之后，最后一步就是以核心层SdioDevice对象的初始化为核心，包括厂商自定义结构体（传递参数和数据），实例化SdioDevice成员SdioDeviceOps（让用户可以通过接口来调用驱动底层函数），实现HdfDriverEntry成员函数（Bind，Init，Release）

- 自定义结构体参考 

    > 从驱动的角度看，自定义结构体是参数和数据的载体，而且sdio_config.hcs文件中的数值会被HDF读入通过DeviceResourceIface来初始化结构体成员，一些重要数值也会传递给核心层对象。
    
    ```c
    typedef struct {
        uint32_t maxBlockNum;    // 单个request最大的block个数
        uint32_t maxBlockSize;   // 单个block最大的字节数1~2048
        uint32_t maxRequestSize; // 单个request最大的字节数 1~2048
        uint32_t enTimeout;      // 最大超时时间，单位毫秒，且不能超过一秒
        uint32_t funcNum;        // 函数编号1~7
        uint32_t irqCap;         // 中断能力
        void *data;              // 私有数据
    } SdioFuncInfo;
    
    //SdioDevice是核心层控制器结构体，其中的成员在Bind函数中会被赋值
    struct SdioDevice {
        struct SdDevice sd;
        struct SdioDeviceOps *sdioOps;
        struct SdioRegister sdioReg;
        uint32_t functions;
        struct SdioFunction *sdioFunc[SDIO_MAX_FUNCTION_NUMBER];
        struct SdioFunction *curFunction;
        struct OsalThread thread;  /* irq thread */
        struct OsalSem sem;
        bool irqPending;
        bool threadRunning;
    };
    ```

- **【重要】** SdioDevice成员回调函数结构体SdioDeviceOps的实例化，其他成员在Init函数中初始化

    ```c 
    static struct SdioDeviceOps g_sdioDeviceOps = {
        .incrAddrReadBytes   = Hi35xxLinuxSdioIncrAddrReadBytes,
        .incrAddrWriteBytes  = Hi35xxLinuxSdioIncrAddrWriteBytes,
        .fixedAddrReadBytes  = Hi35xxLinuxSdioFixedAddrReadBytes,
        .fixedAddrWriteBytes = Hi35xxLinuxSdioFixedAddrWriteBytes,
        .func0ReadBytes  = Hi35xxLinuxSdioFunc0ReadBytes,
        .func0WriteBytes = Hi35xxLinuxSdioFunc0WriteBytes,
        .setBlockSize    = Hi35xxLinuxSdioSetBlockSize,
        .getCommonInfo   = Hi35xxLinuxSdioGetCommonInfo,
        .setCommonInfo   = Hi35xxLinuxSdioSetCommonInfo,
        .flushData       = Hi35xxLinuxSdioFlushData,
        .enableFunc      = Hi35xxLinuxSdioEnableFunc,
        .disableFunc = Hi35xxLinuxSdioDisableFunc,
        .claimIrq    = Hi35xxLinuxSdioClaimIrq,
        .releaseIrq  = Hi35xxLinuxSdioReleaseIrq,
        .findFunc    = Hi35xxLinuxSdioFindFunc,
        .claimHost   = Hi35xxLinuxSdioClaimHost,
        .releaseHost = Hi35xxLinuxSdioReleaseHost,
    };
    ```

- **Bind函数参考**

    > **入参：** 
    >  HdfDeviceObject 是整个驱动对外暴露的接口参数，具备 HCS 配置文件的信息 
    > 
    > **返回值：**
    > HDF_STATUS相关状态 （下表为部分展示，如需使用其他状态，可见//drivers/framework/include/utils/hdf_base.h中HDF_STATUS 定义）
    > 
    > |状态(值)|问题描述|
    > |:-|:-:|
    > |HDF_ERR_INVALID_OBJECT|控制器对象非法|
    > |HDF_ERR_MALLOC_FAIL   |内存分配失败|
    > |HDF_ERR_INVALID_PARAM |参数非法|
    > |HDF_ERR_IO           |I/O 错误|
    > |HDF_SUCCESS            |初始化成功|
    > |HDF_FAILURE           |初始化失败|
    > 
    > **函数说明：**
    > 将 MmcCntlr 对象同 HdfDeviceObject 进行了关联 
    > 
    > **函数说明：**
    > 初始化自定义结构体对象，初始化SdioCntlr成员，调用核心层SdioCntlrAdd函数，以及其他厂商自定义初始化操作
    
    ```c
    static int32_t Hi35xxLinuxSdioBind(struct HdfDeviceObject *obj)
    {
        struct MmcCntlr *cntlr = NULL;
        int32_t ret;
        ...
        cntlr = (struct MmcCntlr *)OsalMemCalloc(sizeof(struct MmcCntlr));// 分配内存
        ...
        cntlr->ops = &g_sdioCntlrOps;  //【必要】struct MmcCntlrOps g_sdioCntlrOps={
                                       // .rescanSdioDev = Hi35xxLinuxSdioRescan,};
        cntlr->hdfDevObj = obj;        //【必要】使HdfDeviceObject与MmcCntlr可以相互转化的前提
        obj->service = &cntlr->service;//【必要】使HdfDeviceObject与MmcCntlr可以相互转化的前提    
        ret = Hi35xxLinuxSdioCntlrParse(cntlr, obj);//【必要】初始化cntlr 的 index, devType, 失败则 goto _ERR;
        ...
        ret = MmcCntlrAdd(cntlr);	   //【必要】调用核心层mmc_core.c的函数， 失败则 goto _ERR;
        ...
        ret = MmcCntlrAllocDev(cntlr, (enum MmcDevType)cntlr->devType);//【必要】调用核心层mmc_core.c的函数， 失败则 goto _ERR;
        ...
        
        MmcDeviceAddOps(cntlr->curDev, &g_sdioDeviceOps);//【必要】调用核心层mmc_core.c的函数， 钩子函数挂载
        HDF_LOGD("Hi35xxLinuxSdioBind: Success!");
        return HDF_SUCCESS;
    
    _ERR:
        Hi35xxLinuxSdioDeleteCntlr(cntlr);
        HDF_LOGE("Hi35xxLinuxSdioBind: Fail!");
        return HDF_FAILURE;
    }
    ```


- **Init函数参考**

    > **入参：** 
    > HdfDeviceObject 是整个驱动对外暴露的接口参数，具备 HCS 配置文件的信息 
    > 
    > **返回值：**
    > HDF_STATUS相关状态
    > 
    > **函数说明：**
    > 无操作，可根据厂商需要添加
    
    ```c 
    static int32_t Hi35xxLinuxSdioInit(struct HdfDeviceObject *obj)
    {
        (void)obj;//无操作，可根据厂商需要添加
        HDF_LOGD("Hi35xxLinuxSdioInit: Success!");
        return HDF_SUCCESS;
    }
    ```

- **Release函数参考**

    > **入参：** 
    > HdfDeviceObject 是整个驱动对外暴露的接口参数，具备 HCS 配置文件的信息 
    > 
    > **返回值：**
    > 无
    > 
    > **函数说明：**
    > 释放内存和删除控制器，该函数需要在驱动入口结构体中赋值给 Release 接口， 当HDF框架调用Init函数初始化驱动失败时，可以调用 Release 释放驱动资源。所有强制转换获取相应对象的操作**前提**是在Bnit函数中具备对应赋值的操作。
    
    ```c
    static void Hi35xxLinuxSdioRelease(struct HdfDeviceObject *obj)
    {
        if (obj == NULL) {
            return;
        }
        Hi35xxLinuxSdioDeleteCntlr((struct MmcCntlr *)obj->service);//【必要】自定义的内存释放函数,这里有HdfDeviceObject到MmcCntlr的强制转化
    }
    ```

