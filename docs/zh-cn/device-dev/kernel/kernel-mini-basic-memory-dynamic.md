# 动态内存<a name="ZH-CN_TOPIC_0000001123763647"></a>

-   [运行机制](#section328282013571)
-   [开发指导](#section7921151015814)
    -   [使用场景](#section326917198583)
    -   [接口说明](#section1032331584)
    -   [开发流程](#section07271773592)
    -   [编程实例](#section84931234145913)
    -   [结果验证](#section165233233917)


## 运行机制<a name="section328282013571"></a>

动态内存管理，即在内存资源充足的情况下，根据用户需求，从系统配置的一块比较大的连续内存（内存池，也是堆内存）中分配任意大小的内存块。当用户不需要该内存块时，又可以释放回系统供下一次使用。与静态内存相比，动态内存管理的优点是按需分配，缺点是内存池中容易出现碎片。

OpenHarmony LiteOS-M动态内存在TLSF算法的基础上，对区间的划分进行了优化，获得更优的性能，降低了碎片率。动态内存核心算法框图如下：

**图 1**  动态内存核心算法<a name="fig14558185217397"></a>  
![](figure/动态内存核心算法.png "动态内存核心算法")

根据空闲内存块的大小，使用多个空闲链表来管理。根据内存空闲块大小分为两个部分：\[4, 127\]和\[2<sup>7</sup>, 2<sup>31</sup>\]，如上图size class所示：

1.  对\[4,127\]区间的内存进行等分，如上图下半部分所示，分为31个小区间，每个小区间对应内存块大小为4字节的倍数。每个小区间对应一个空闲内存链表和用于标记对应空闲内存链表是否为空的一个比特位，值为1时，空闲链表非空。\[4,127\]区间的31个小区间内存对应31个比特位进行标记链表是否为空。
2.  大于127字节的空闲内存块，按照2的次幂区间大小进行空闲链表管理。总共分为24个小区间，每个小区间又等分为8个二级小区间，见上图上半部分的Size Class和Size SubClass部分。每个二级小区间对应一个空闲链表和用于标记对应空闲内存链表是否为空的一个比特位。总共24\*8=192个二级小区间，对应192个空闲链表和192个比特位进行标记链表是否为空。

例如，当有40字节的空闲内存需要插入空闲链表时，对应小区间\[40,43\]，第10个空闲链表，位图标记的第10比特位。把40字节的空闲内存挂载第10个空闲链表上，并判断是否需要更新位图标记。当需要申请40字节的内存时，根据位图标记获取存在满足申请大小的内存块的空闲链表，从空闲链表上获取空闲内存节点。如果分配的节点大于需要申请的内存大小，进行分割节点操作，剩余的节点重新挂载到相应的空闲链表上。当有580字节的空闲内存需要插入空闲链表时，对应二级小区间\[2^9,2^9+2^6\]，第31+2\*8=47个空闲链表，并使用位图的第47个比特位来标记链表是否为空。把580字节的空闲内存挂载第47个空闲链表上，并判断是否需要更新位图标记。当需要申请580字节的内存时，根据位图标记获取存在满足申请大小的内存块的空闲链表，从空闲链表上获取空闲内存节点。如果分配的节点大于需要申请的内存大小，进行分割节点操作，剩余的节点重新挂载到相应的空闲链表上。如果对应的空闲链表为空，则向更大的内存区间去查询是否有满足条件的空闲链表，实际计算时，会一次性查找到满足申请大小的空闲链表。

内存管理结构如下图所示：

**图 2**  动态内存管理结构图<a name="fig5395115964114"></a>  
![](figure/动态内存管理结构图.png "动态内存管理结构图")

-   内存池池头部分

    内存池池头部分包含内存池信息、位图标记数组和空闲链表数组。内存池信息包含内存池起始地址及堆区域总大小，内存池属性。位图标记数组有7个32位无符号整数组成，每个比特位标记对应的空闲链表是否挂载空闲内存块节点。空闲内存链表包含223个空闲内存头节点信息，每个空闲内存头节点信息维护内存节点头和空闲链表中的前驱、后继空闲内存节点。

-   内存池节点部分

    包含3种类型节点：未使用空闲内存节点，已使用内存节点和尾节点。每个内存节点维护一个前序指针，指向内存池中上一个内存节点，还维护内存节点的大小和使用标记。空闲内存节点和已使用内存节点后面的内存区域是数据域，尾节点没有数据域。

一些芯片片内RAM大小无法满足要求，需要使用片外物理内存进行扩充。对于这样的多段非连续性内存，OpenHarmony LiteOS-M内核支持把多个非连续性内存逻辑上合一，用户不感知底层的多段非连续性内存区域。OpenHarmony LiteOS-M内核内存模块把不连续的内存区域作为空闲内存结点插入到空闲内存节点链表，把不同内存区域间的不连续部分标记为虚拟的已使用内存节点，从逻辑上把多个非连续性内存区域实现为一个统一的内存池。下面通过示意图说明下多段非连续性内存的运行机制：

**图 3**  非连续性内存合一示意图<a name="figMultiMemRegionsPrincipal"></a>
![](figure/非连续性内存合一示意图.png "非连续性内存合一示意图")

结合上述示意图，非连续性内存合并为一个统一的内存池的步骤如下：
1. 把多段非连续性内存区域的第一块内存区域通过调用LOS_MemInit接口进行初始化。
2. 获取下一个内存区域的开始地址和长度，计算该内存区域和上一块内存区域的间隔大小gapSize。
3. 把内存区域间隔部分视为虚拟的已使用节点，使用上一个内存区域的尾节点，设置其大小为gapSize+ OS_MEM_NODE_HEAD_SIZE。
4. 把当前内存区域划分为一个空闲内存节点和一个尾节点，把空闲内存节点插入到空闲链表，并设置各个节点的前后链接关系。
5. 如果有更多的非连续内存区域，重复上述步骤2-4。


## 开发指导<a name="section7921151015814"></a>

### 使用场景<a name="section326917198583"></a>

动态内存管理的主要工作是动态分配并管理用户申请到的内存区间。动态内存管理主要用于用户需要使用大小不等的内存块的场景，当用户需要使用内存时，可以通过操作系统的动态内存申请函数索取指定大小的内存块，一旦使用完毕，通过动态内存释放函数归还所占用内存，使之可以重复使用。

### 接口说明<a name="section1032331584"></a>

OpenHarmony LiteOS-M的动态内存管理主要为用户提供以下功能，接口详细信息可以查看API参考。

**表 1**  动态内存模块接口

<a name="table1415203765610"></a>
<table><thead align="left"><tr id="row134151837125611"><th class="cellrowborder" valign="top" width="12.85128512851285%" id="mcps1.2.4.1.1"><p id="p16415637105612"><a name="p16415637105612"></a><a name="p16415637105612"></a>功能分类</p>
</th>
<th class="cellrowborder" valign="top" width="29.8029802980298%" id="mcps1.2.4.1.2"><p id="p11415163718562"><a name="p11415163718562"></a><a name="p11415163718562"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="57.34573457345735%" id="mcps1.2.4.1.3"><p id="p1641533755612"><a name="p1641533755612"></a><a name="p1641533755612"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row0415737175610"><td class="cellrowborder" rowspan="2" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id="p6485848217"><a name="p6485848217"></a><a name="p6485848217"></a>初始化和删除内存池</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id="p1448511481314"><a name="p1448511481314"></a><a name="p1448511481314"></a>LOS_MemInit</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id="p94857483110"><a name="p94857483110"></a><a name="p94857483110"></a>初始化一块指定的动态内存池，大小为size。</p>
</td>
</tr>
<tr id="row1841519376561"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p20485134810119"><a name="p20485134810119"></a><a name="p20485134810119"></a>LOS_MemDeInit</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p154851348113"><a name="p154851348113"></a><a name="p154851348113"></a>删除指定内存池，仅打开LOSCFG_MEM_MUL_POOL时有效。</p>
</td>
</tr>
<tr id="row1187514443616"><td class="cellrowborder" rowspan="4" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id="p19661710214"><a name="p19661710214"></a><a name="p19661710214"></a>申请、释放动态内存</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id="p66111714213"><a name="p66111714213"></a><a name="p66111714213"></a>LOS_MemAlloc</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id="p4661715214"><a name="p4661715214"></a><a name="p4661715214"></a>从指定动态内存池中申请size长度的内存。</p>
</td>
</tr>
<tr id="row1745415527441"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p116111718218"><a name="p116111718218"></a><a name="p116111718218"></a>LOS_MemFree</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1569175218"><a name="p1569175218"></a><a name="p1569175218"></a>释放从指定动态内存中申请的内存。</p>
</td>
</tr>
<tr id="row19101718144518"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p161417225"><a name="p161417225"></a><a name="p161417225"></a>LOS_MemRealloc</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p56181718219"><a name="p56181718219"></a><a name="p56181718219"></a>按size大小重新分配内存块，并将原内存块内容拷贝到新内存块。如果新内存块申请成功，则释放原内存块。</p>
</td>
</tr>
<tr id="row1346314166464"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10610171528"><a name="p10610171528"></a><a name="p10610171528"></a>LOS_MemAllocAlign</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p26171714214"><a name="p26171714214"></a><a name="p26171714214"></a>从指定动态内存池中申请长度为size且地址按boundary字节对齐的内存。</p>
</td>
</tr>
<tr id="row28531740101112"><td class="cellrowborder" rowspan="4" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id="p39818810129"><a name="p39818810129"></a><a name="p39818810129"></a>获取内存池信息</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id="p16981986123"><a name="p16981986123"></a><a name="p16981986123"></a>LOS_MemPoolSizeGet</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id="p129820881212"><a name="p129820881212"></a><a name="p129820881212"></a>获取指定动态内存池的总大小。</p>
</td>
</tr>
<tr id="row34281341171114"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p0987831214"><a name="p0987831214"></a><a name="p0987831214"></a>LOS_MemTotalUsedGet</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1598889124"><a name="p1598889124"></a><a name="p1598889124"></a>获取指定动态内存池的总使用量大小。</p>
</td>
</tr>
<tr id="row112644551119"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p398208201215"><a name="p398208201215"></a><a name="p398208201215"></a>LOS_MemInfoGet</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p139815820126"><a name="p139815820126"></a><a name="p139815820126"></a>获取指定内存池的内存结构信息，包括空闲内存大小、已使用内存大小、空闲内存块数量、已使用的内存块数量、最大的空闲内存块大小。</p>
</td>
</tr>
<tr id="row1357684518110"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p3981383125"><a name="p3981383125"></a><a name="p3981383125"></a>LOS_MemPoolList</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p998148101218"><a name="p998148101218"></a><a name="p998148101218"></a>打印系统中已初始化的所有内存池，包括内存池的起始地址、内存池大小、空闲内存总大小、已使用内存总大小、最大的空闲内存块大小、空闲内存块数量、已使用的内存块数量。仅打开LOSCFG_MEM_MUL_POOL时有效。</p>
</td>
</tr>
<tr id="row14824879135"><td class="cellrowborder" rowspan="2" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id="p1058973361319"><a name="p1058973361319"></a><a name="p1058973361319"></a>获取内存块信息</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id="p989115206287"><a name="p989115206287"></a><a name="p989115206287"></a>LOS_MemFreeNodeShow</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id="p437618158141"><a name="p437618158141"></a><a name="p437618158141"></a>打印指定内存池的空闲内存块的大小及数量。</p>
</td>
</tr>
<tr id="row525917259143"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p0260025151415"><a name="p0260025151415"></a><a name="p0260025151415"></a>LOS_MemUsedNodeShow</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p19976124812146"><a name="p19976124812146"></a><a name="p19976124812146"></a>打印指定内存池的已使用内存块的大小及数量。</p>
</td>
</tr>
<tr id="row0715201211155"><td class="cellrowborder" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id="p13599202711513"><a name="p13599202711513"></a><a name="p13599202711513"></a>检查指定内存池的完整性</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id="p5784183891513"><a name="p5784183891513"></a><a name="p5784183891513"></a>LOS_MemIntegrityCheck</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id="p15644611153"><a name="p15644611153"></a><a name="p15644611153"></a>对指定内存池做完整性检查，仅打开LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK时有效。</p>
</td>
</tr>
<tr id=""><td class="cellrowborder" valign="top" width="12.85128512851285%" headers="mcps1.2.4.1.1 "><p id=""><a name=""></a><a name=""></a>增加非连续性内存区域</p>
</td>
<td class="cellrowborder" valign="top" width="29.8029802980298%" headers="mcps1.2.4.1.2 "><p id=""><a name=""></a><a name=""></a>LOS_MemRegionsAdd</p>
</td>
<td class="cellrowborder" valign="top" width="57.34573457345735%" headers="mcps1.2.4.1.3 "><p id=""><a name=""></a><a name=""></a>支持多段非连续性内存区域，把非连续性内存区域逻辑上整合为一个统一的内存池。仅打开LOSCFG_MEM_MUL_REGIONS时有效。如果内存池指针参数pool为空，则使用多段内存的第一个初始化为内存池，其他内存区域，作为空闲节点插入；如果内存池指针参数pool不为空，则把多段内存作为空闲节点，插入到指定的内存池。</p>
</td>
</tr>

</tbody>
</table>

>![](../public_sys-resources/icon-note.gif) **说明：** 
>-   由于动态内存管理需要管理控制块数据结构来管理内存，这些数据结构会额外消耗内存，故实际用户可使用内存总量小于配置项OS\_SYS\_MEM\_SIZE的大小。
>-   对齐分配内存接口LOS\_MemAllocAlign/LOS\_MemMallocAlign因为要进行地址对齐，可能会额外消耗部分内存，故存在一些遗失内存，当系统释放该对齐内存时，同时回收由于对齐导致的遗失内存。
>-   非连续性内存区域接口LOS\_MemRegionsAdd的LosMemRegion数组参数传入的非连续性内存区域需要按各个内存区域的内存开始地址升序，且内存区域不能重叠。

### 开发流程<a name="section07271773592"></a>

本节介绍使用动态内存的典型场景开发流程。

1.  初始化LOS\_MemInit。

    初始一个内存池后生成一个内存池控制头、尾节点EndNode，剩余的内存被标记为FreeNode内存节点。注：EndNode作为内存池末尾的节点，size为0。


1.  申请任意大小的动态内存LOS\_MemAlloc。

    判断动态内存池中是否存在大于申请量大小的空闲内存块空间，若存在，则划出一块内存块，以指针形式返回，若不存在，返回NULL。如果空闲内存块大于申请量，需要对内存块进行分割，剩余的部分作为空闲内存块挂载到空闲内存链表上。


1.  释放动态内存LOS\_MemFree。

    回收内存块，供下一次使用。调用LOS\_MemFree释放内存块，则会回收内存块，并且将其标记为FreeNode。在回收内存块时，相邻的FreeNode会自动合并。


### 编程实例<a name="section84931234145913"></a>

本实例执行以下步骤：

1.  初始化一个动态内存池。
2.  从动态内存池中申请一个内存块。
3.  在内存块中存放一个数据。
4.  打印出内存块中的数据。
5.  释放该内存块。

示例代码如下：

```
#include "los_memory.h"

#define TEST_POOL_SIZE (2*1024)
__attribute__((aligned(4))) UINT8 g_testPool[TEST_POOL_SIZE];

VOID Example_DynMem(VOID)
{
    UINT32 *mem = NULL;
    UINT32 ret;

    /*初始化内存池*/
    ret = LOS_MemInit(g_testPool, TEST_POOL_SIZE);
    if (LOS_OK  == ret) {
        printf("Memory pool initialized.\n");
    } else {
        printf("Memory pool initialization failed.\n");
        return;
    }

    /*分配内存*/
    mem = (UINT32 *)LOS_MemAlloc(g_testPool, 4);
    if (NULL == mem) {
        printf("Memory allocation failed.\n");
        return;
    }
    printf("Memory allocated.\n");

    /*赋值*/
    *mem = 828;
    printf("*mem = %d\n", *mem);

    /*释放内存*/
    ret = LOS_MemFree(g_testPool, mem);
    if (LOS_OK == ret) {
        printf("Memory released.\n");
    } else {
        printf("Memory release failed.\n");
    }

    return;
}
```

### 结果验证<a name="section165233233917"></a>

输出结果如下：

```
Memory pool initialized.
Memory allocated.
*mem = 828
Memory released.
```

