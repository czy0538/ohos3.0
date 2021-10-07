# 开发指导<a name="ZH-CN_TOPIC_0000001079036458"></a>

-   [接口说明](#section158501652121514)
-   [开发流程](#section11841123033618)
-   [编程实例](#section460018317164)
    -   [结果验证](#section1048572415182)


当产生中断请求时，CPU暂停当前的任务，转而去响应外设请求。用户可以根据需要注册对应的中断处理程序，指定CPU响应中断请求时所执行的具体操作。

## 接口说明<a name="section158501652121514"></a>

OpenHarmony LiteOS-M内核的中断模块提供下面几种功能，接口详细信息可以查看API参考。

**表 1**  中断模块接口

<a name="table1415203765610"></a>
<table><thead align="left"><tr id="row134151837125611"><th class="cellrowborder" valign="top" width="12.85128512851285%" id="mcps1.2.4.1.1"><p id="p16415637105612"><a name="p16415637105612"></a><a name="p16415637105612"></a>功能分类</p>
</th>
<th class="cellrowborder" valign="top" width="29.8029802980298%" id="mcps1.2.4.1.2"><p id="p11415163718562"><a name="p11415163718562"></a><a name="p11415163718562"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="57.34573457345735%" id="mcps1.2.4.1.3"><p id="p1641533755612"><a name="p1641533755612"></a><a name="p1641533755612"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row0415737175610"><td class="cellrowborder" rowspan="2" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id="p4917132105710"><a name="p4917132105710"></a><a name="p4917132105710"></a>创建、删除中断</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id="p341513372561"><a name="p341513372561"></a><a name="p341513372561"></a>HalHwiCreate</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id="p74151037185617"><a name="p74151037185617"></a><a name="p74151037185617"></a>中断创建，注册中断号、中断触发模式、中断优先级、中断处理程序。中断被触发时，会调用该中断处理程序。</p>
</td>
</tr>
<tr id="row1841519376561"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p64151837155618"><a name="p64151837155618"></a><a name="p64151837155618"></a>HalHwiDelete</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p441516379562"><a name="p441516379562"></a><a name="p441516379562"></a>根据指定的中断号，删除中断。</p>
</td>
</tr>
<tr id="row1141513373562"><td class="cellrowborder" rowspan="3" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id="p956643365710"><a name="p956643365710"></a><a name="p956643365710"></a>打开、关闭中断</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id="p17765212416"><a name="p17765212416"></a><a name="p17765212416"></a>LOS_IntUnLock</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id="p1972971913115"><a name="p1972971913115"></a><a name="p1972971913115"></a>开中断，使能当前处理器所有中断响应。</p>
</td>
</tr>
<tr id="row1541513745611"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1332013246116"><a name="p1332013246116"></a><a name="p1332013246116"></a>LOS_IntLock</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1692710331219"><a name="p1692710331219"></a><a name="p1692710331219"></a>关中断，关闭当前处理器所有中断响应。</p>
</td>
</tr>
<tr id="row14167379561"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1438619410117"><a name="p1438619410117"></a><a name="p1438619410117"></a>LOS_IntRestore</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p254691522"><a name="p254691522"></a><a name="p254691522"></a>恢复到使用LOS_IntLock、LOS_IntUnLock操作之前的中断状态。</p>
</td>
</tr>
</tbody>
</table>

## 开发流程<a name="section11841123033618"></a>

1.  调用中断创建接口HalHwiCreate创建中断。
2.  调用TestHwiTrigger接口触发指定中断（该接口在测试套中定义，通过写中断控制器的相关寄存器模拟外部中断，一般的外设设备，不需要执行这一步）。
3.  调用HalHwiDelete接口删除指定中断，此接口根据实际情况使用，判断是否需要删除中断。

>![](../public_sys-resources/icon-note.gif) **说明：** 
>-   根据具体硬件，配置支持的最大中断数及可设置的中断优先级个数。
>-   中断处理程序耗时不能过长，否则会影响CPU对中断的及时响应。
>-   中断响应过程中不能直接、间接执行引起调度的LOS\_Schedule等函数。
>-   中断恢复LOS\_IntRestore\(\)的入参必须是与之对应的LOS\_IntLock\(\)的返回值（即关中断之前的CPSR值）。Cortex-M系列处理器中0-15中断为内部使用，因此不建议用户去申请和创建。

## 编程实例<a name="section460018317164"></a>

本实例实现如下功能：

1.  创建中断。
2.  触发中断。
3.  删除中断。

代码实现如下，演示如何创建中断和删除中断，当指定的中断号HWI\_NUM\_TEST产生中断时，会调用中断处理函数：

```
#include "los_interrupt.h"

/*创建中断*/
#define HWI_NUM_TEST 7

STATIC VOID HwiUsrIrq(VOID)
{
    printf("in the func HwiUsrIrq \n"); 
}

static UINT32 Example_Interrupt(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 3;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;
  
    /*创建中断*/
    ret = HalHwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiUsrIrq, arg);
    if(ret == LOS_OK){
        printf("Hwi create success!\n");
    } else {
        printf("Hwi create failed!\n");
        return LOS_NOK;
    }

    /* 延时50个Ticks， 当有硬件中断发生时，会调用函数HwiUsrIrq*/
    LOS_TaskDelay(50);

    /*删除中断*/
    ret = HalHwiDelete(HWI_NUM_TEST);    
    if(ret == LOS_OK){
        printf("Hwi delete success!\n");
    } else {
        printf("Hwi delete failed!\n");
        return LOS_NOK;
    }
    return LOS_OK;
}
```

### 结果验证<a name="section1048572415182"></a>

编译运行得到的结果为：

```
Hwi create success!
Hwi delete success!.
```

