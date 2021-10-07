# WatchDog

- [概述](#1)
- [开发步骤](#2)
- [开发实例](#3)

## 概述 <a name="1"></a>

看门狗（watchdog），又叫看门狗计时器（watchdog timer），是一种硬件的计时设备，在HDF框架中，watchdog接口适配模式采用独立服务模式，在这种模式下，每一个设备对象会独立发布一个设备服务来处理外部访问，设备管理器收到API的访问请求之后，通过提取该请求的参数，达到调用实际设备对象的相应内部方法的目的。

独立服务模式可以直接借助HDFDeviceManager的服务管理能力，但需要为每个设备单独配置设备节点，增加内存占用。

图 1 独立服务模式结构图
![image1](figure/独立服务模式.png)

## 开发步骤 <a name="2"></a>

watchdog模块适配HDF框架的三个环节是配置属性文件，实例化驱动入口，以及填充核心层接口函数。

1. **实例化驱动入口：**   
    - 实例化HdfDriverEntry结构体成员。
    - 调用HDF_INIT将HdfDriverEntry实例化对象注册到HDF框架中。

2. **配置属性文件：**   
   
    - 在device_info.hcs文件中添加deviceNode描述。
    - 【可选】添加watchdog_config.hcs器件属性文件。

3. **实例化Watchdog控制器对象：**   
   
    - 初始化WatchdogCntlr成员。
    - 实例化WatchdogCntlr成员WatchdogMethod，其定义和成员**说明**见下
    
4. **驱动调试：**
    - 【可选】针对新增驱动程序，建议验证驱动基本功能，例如挂载后的信息反馈，超时时间设置的成功与否等。

> ![](../public_sys-resources/icon-note.gif) **说明：** 
> WatchdogMethod定义
> 
> ```c
> struct WatchdogMethod {
>   int32_t (*getStatus)(struct WatchdogCntlr *wdt, int32_t *status);
>   int32_t (*setTimeout)(struct WatchdogCntlr *wdt, uint32_t seconds);
>   int32_t (*getTimeout)(struct WatchdogCntlr *wdt, uint32_t *seconds);
>   int32_t (*start)(struct WatchdogCntlr *wdt);
>   int32_t (*stop)(struct WatchdogCntlr *wdt);
>   int32_t (*feed)(struct WatchdogCntlr *wdt);
>   int32_t (*getPriv)(struct WatchdogCntlr *wdt); //【可选】如果WatchdogCntlr 中的priv成员存在，则按需填充 
>   void (*releasePriv)(struct WatchdogCntlr *wdt);//【可选】
> };
> ```
> 
> 表1 WatchdogMethod成员的回调函数功能说明
> |成员函数|入参|出参|返回值|功能|
> |-|-|-|-|-|
> |getStatus |**wdt**: 结构体指针,核心层WDG控制器; |**status**: int32_t指针,<br />表示狗的状态（打开或关闭）; |HDF_STATUS相关状态|获取看门狗所处的状态|
> |start     |**wdt**: 结构体指针,核心层WDG控制器; |无 |HDF_STATUS相关状态|打开开门狗 |
> |stop      |**wdt**: 结构体指针,核心层WDG控制器; |无 |HDF_STATUS相关状态|关闭开门狗  |
> |setTimeout|**wdt**: 结构体指针,核心层WDG控制器;<br />**seconds**: uint32_t,时间传入值;|无|HDF_STATUS相关状态|设置超时时间值，<br />需要与设置的时间相对应，<br />与厂商看门狗的时钟周期相关 |
> |getTimeout|**wdt**: 结构体指针,核心层WDG控制器; |**seconds**: uint32_t,<br />传出的时间值|HDF_STATUS相关状态|回读设置的超时时间值  |
> |feed      |**wdt**: 结构体指针,核心层WDG控制器; |无 |HDF_STATUS相关状态|喂狗  |


## 开发实例 <a name="3"></a>

下方将以watchdog_hi35xx.c为示例，展示需要厂商提供哪些内容来完整实现设备功能。

1. 驱动开发首先需要实例化驱动入口，驱动入口必须为HdfDriverEntry（在 hdf_device_desc.h 中定义）类型的全局变量，且moduleName要和device_info.hcs中保持一致。HDF框架会将所有加载的驱动的HdfDriverEntry对象首地址汇总，形成一个类似数组的段地址空间，方便上层调用。
    
    一般在加载驱动时HDF会先调用Bind函数，再调用Init函数加载该驱动。当Init调用异常时，HDF框架会调用Release释放驱动资源并退出。

- watchdog驱动入口参考

    ```c
    struct HdfDriverEntry g_watchdogDriverEntry = {
    .moduleVersion = 1,
    .Bind = Hi35xxWatchdogBind,           //见Bind参考
    .Init = Hi35xxWatchdogInit,           //见Init参考
    .Release = Hi35xxWatchdogRelease,     //见Release参考
    .moduleName = "HDF_PLATFORM_WATCHDOG",//【必要且与HCS文件中里面的moduleName匹配】
    };
    HDF_INIT(g_watchdogDriverEntry);//调用HDF_INIT将驱动入口注册到HDF框架中
    ```

2. 完成驱动入口注册之后，下一步请在device_info.hcs文件中添加deviceNode信息，并在 watchdog_config.hcs 中配置器件属性。deviceNode信息与驱动入口注册相关，器件属性值与核心层WatchdogCntlr 成员的默认值或限制范围有密切关系。
    
    **本例只有一个Watchdog控制器，如有多个器件信息，则需要在device_info文件增加deviceNode信息，以及在watchdog_config文件中增加对应的器件属性**。

- device_info.hcs 配置参考

    ```c
    root {
      device_info {
        match_attr = "hdf_manager";
        device_watchdog :: device { // 设备节点
          device0 :: deviceNode {   // 驱动的DeviceNode节点
            policy = 1;             // policy字段是驱动服务发布的策略,如果需要面向用户态，则为2
            priority = 20;          // 驱动启动优先级
            permission = 0644;      // 驱动创建设备节点权限
            moduleName = "HDF_PLATFORM_WATCHDOG";             
            // 【必要】驱动名称，该字段的值必须和驱动入口结构的moduleName值一致
            serviceName = "HDF_PLATFORM_WATCHDOG_0";          
            // 【必要且唯一】驱动对外发布服务的名称
            deviceMatchAttr = "hisilicon_hi35xx_watchdog_0";  
            // 【必要】驱动私有数据匹配的关键字，必须和驱动私有数据配置表中的match_attr值相等
          } 
        }
      }
    }
    ```

- watchdog_config.hcs 配置参考

    ```c
    root {
      platform {
        template watchdog_controller {//【必要】模板配置，继承该模板的节点如果使用模板中的默认值，则节点字段可以缺省
          id = 0;
          match_attr = "";
          regBase = 0x12050000;       //【必要】地址映射需要
          regStep = 0x1000;           //【必要】地址映射需要
        }
        controller_0x12050000 :: watchdog_controller {//【必要】是作为设备驱动私有数据匹配的关键字
          match_attr = "hisilicon_hi35xx_watchdog_0"; //【必要】必须和device_info.hcs中的deviceMatchAttr值一致
          }
          //存在多个 watchdog 时【必须】添加，否则不用
          ...  
        }
    }
    ```

3. 完成驱动入口注册之后，最后一步就是以核心层WatchdogCntlr对象的初始化为核心，包括厂商自定义结构体（传递参数和数据），实例化WatchdogCntlr成员WatchdogMethod（让用户可以通过接口来调用驱动底层函数），实现HdfDriverEntry成员函数（Bind，Init，Release）

- 自定义结构体参考 

    > 从驱动的角度看，自定义结构体是参数和数据的载体，而且watchdog_config.hcs文件中的数值会被HDF读入通过DeviceResourceIface来初始化结构体成员，其中一些重要数值也会传递给核心层WatchdogCntlr对象，例如索引、管脚数等。
    
    ```c
    struct Hi35xxWatchdog {
      struct WatchdogCntlr wdt;       //【必要】是链接上下层的载体，具体描述见下面
      OsalSpinlock lock;              //【可选】可挂载到 WatchdogCntlr 的lock成员上，两个是相同的定义
      volatile unsigned char *regBase;//【必要】地址映射需要
      uint32_t phyBase;               //【必要】地址映射需要
      uint32_t regStep;               //【必要】地址映射需要
    };
    //WatchdogCntlr是核心层控制器结构体，其中的成员在Init函数中会被赋值
    struct WatchdogCntlr {
      struct IDeviceIoService service;//驱动服务，【无需挂载】
      struct HdfDeviceObject *device; //驱动设备，需要挂载 bind 函数的入参：struct HdfDeviceObject *device
      OsalSpinlock lock;              //在HDF核心层调用时从系统代码实现了一个自旋锁的机制，挂载的变量需要是相同的变量，不建议挂载
      struct WatchdogMethod *ops;     //接口回调函数
      int16_t wdtId;                  //WDG设备的识别id
      void *priv;                     //存储指针
    };
    ```

- **【重要】** WatchdogCntlr成员回调函数结构体WatchdogMethod的实例化，其他成员在Init和Bind函数中初始化

    ```c
    static struct WatchdogMethod g_method = {
        .getStatus = Hi35xxWatchdogGetStatus,
        .start = Hi35xxWatchdogStart,
        .stop = Hi35xxWatchdogStop,
        .setTimeout = Hi35xxWatchdogSetTimeout,
        .getTimeout = Hi35xxWatchdogGetTimeout,
        .feed = Hi35xxWatchdogFeed,
    };
    ```


- **Init函数和Bind函数参考**

    > **入参：** 
    > HdfDeviceObject :HDF框架给每一个驱动创建的设备对象，用来保存设备相关的私有数据和服务接口
    > 
    > **返回值：**
    > HDF_STATUS相关状态 （下表为部分展示，如需使用其他状态，可见//drivers/framework/include/utils/hdf_base.h中HDF_STATUS 定义）
    > 
    > |状态(值)|问题描述|
    > |:-|:-:|
    > |HDF_ERR_INVALID_OBJECT|找不到 WDG 设备|
    > |HDF_ERR_MALLOC_FAIL   |内存分配失败|
    > |HDF_ERR_IO           |I/O 错误|
    > |HDF_SUCCESS            |初始化成功|
    > |HDF_FAILURE           |初始化失败|
    > 
    > **函数说明：**
    > 初始化自定义结构体对象，初始化WatchdogCntlr成员，调用核心层WatchdogCntlrAdd函数。
    
    ```c
    //一般而言，初始化函数需要根据传入设备的属性实现 Hi35xxWatchdog 结构的填充，
    //但此示例中的这部分集成在 bind 函数中
    static int32_t Hi35xxWatchdogInit(struct HdfDeviceObject *device)
    {
    (void)device;
    return HDF_SUCCESS;
    }
    
    static int32_t Hi35xxWatchdogBind(struct HdfDeviceObject *device)
    {
    int32_t ret;
    struct Hi35xxWatchdog *hwdt = NULL;
    ...
    hwdt = (struct Hi35xxWatchdog *)OsalMemCalloc(sizeof(*hwdt));//Hi35xxWatchdog 结构体的内存申请
    ...
    hwdt->regBase = OsalIoRemap(hwdt->phyBase, hwdt->regStep);   //地址映射
    ...
    //最重要的是这个挂载的过程
    hwdt->wdt.priv = (void *)device->property;//【可选】此处填充的是设备属性，但后续没有调用 priv 成员，
                                                // 如果需要用到 priv 成员，需要实现对应的钩子函数填充 WatchdogMethod 
                                                // 结构体的 getPriv 和 releasePriv 成员函数
    hwdt->wdt.ops = &g_method;                //【必要】WatchdogMethod的实例化对象的挂载
    hwdt->wdt.device = device;                //【必要】使HdfDeviceObject与WatchdogcCntlr可以相互转化的前提
    ret = WatchdogCntlrAdd(&hwdt->wdt);       //【必要】调用此函数填充核心层结构体，返回成功信号后驱动才完全接入平台核心层
    if (ret != HDF_SUCCESS) {                 //不成功的话，需要反向执行初始化相关函数
        OsalIoUnmap((void *)hwdt->regBase);
        OsalMemFree(hwdt);
        return ret;
    }    
    return HDF_SUCCESS;
    }
    ```

- **Release函数参考**

    > **入参：** 
    >   HdfDeviceObject :HDF框架给每一个驱动创建的设备对象，用来保存设备相关的私有数据和服务接口
    > 
    > **返回值：**
    > 无
    > 
    > **函数说明：**
    > 该函数需要在驱动入口结构体中赋值给Release，当HDF框架调用Init函数初始化驱动失败时，可以调用Release释放驱动资源。该函数中需包含释放内存和删除控制器等操作。所有强制转换获取相应对象的操作**前提**是在Init函数中具备对应赋值的操作。
    
    ```c 
    static void Hi35xxWatchdogRelease(struct HdfDeviceObject *device)
    {
    struct WatchdogCntlr *wdt = NULL;
    struct Hi35xxWatchdog *hwdt = NULL;
    ...
    wdt = WatchdogCntlrFromDevice(device);//这里有HdfDeviceObject到MmcCntlr的强制转化，通过service成员(第一个成员)，赋值见Bind函数
                                            //return (device == NULL) ? NULL : (struct WatchdogCntlr *)device->service;
    if (wdt == NULL) {
        return;
    }
    WatchdogCntlrRemove(wdt);				//核心层函数，实际执行wdt->device->service = NULL以及cntlr->lock的释放
    hwdt = (struct Hi35xxWatchdog *)wdt;  //这里有MmcCntlr到HimciHost的强制转化
    if (hwdt->regBase != NULL) {			//地址反映射
        OsalIoUnmap((void *)hwdt->regBase);
        hwdt->regBase = NULL;
    }
    OsalMemFree(hwdt);					//厂商自定义对象的内存释放
    }
    ```
