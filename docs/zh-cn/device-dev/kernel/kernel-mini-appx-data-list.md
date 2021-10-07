# 双向链表<a name="ZH-CN_TOPIC_0000001078588390"></a>

-   [基本概念](#section1990715203418)
-   [功能说明](#section848334511411)
-   [开发流程](#section01781261552)
-   [编程实例](#section67569495514)
    -   [实例描述](#section48761994551)
    -   [示例代码](#section1280202685519)
    -   [结果验证](#section5811249105512)


## 基本概念<a name="section1990715203418"></a>

双向链表是指含有往前和往后两个方向的链表，即每个结点中除存放下一个节点指针外，还增加一个指向前一个节点的指针。其头指针head是唯一确定的。

从双向链表中的任意一个结点开始，都可以很方便地访问它的前驱结点和后继结点，这种数据结构形式使得双向链表在查找时更加方便，特别是大量数据的遍历。由于双向链表具有对称性，能方便地完成各种插入、删除等操作，但需要注意前后方向的操作。

## 功能说明<a name="section848334511411"></a>

双向链表模块为用户提供下面几种功能，接口详细信息可以查看API参考。

<a name="table9827162254713"></a>
<table><tbody><tr id="row2089515228470"><td class="cellrowborder" valign="top" width="8.14%"><p id="p4895182214473"><a name="p4895182214473"></a><a name="p4895182214473"></a><strong id="b17895022154712"><a name="b17895022154712"></a><a name="b17895022154712"></a><span>功能分类</span></strong></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p58951922124713"><a name="p58951922124713"></a><a name="p58951922124713"></a><strong id="b15895132214476"><a name="b15895132214476"></a><a name="b15895132214476"></a><span>接口名</span></strong></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p5895122134719"><a name="p5895122134719"></a><a name="p5895122134719"></a><strong id="b5895132214471"><a name="b5895132214471"></a><a name="b5895132214471"></a><span>描述</span></strong></p>
</td>
</tr>
<tr id="row20895152284710"><td class="cellrowborder" rowspan="2" valign="top" width="8.14%"><p id="p18951922144714"><a name="p18951922144714"></a><a name="p18951922144714"></a><span>初始化链表</span></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p789516220474"><a name="p789516220474"></a><a name="p789516220474"></a><span>LOS_ListInit</span></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p38951422144710"><a name="p38951422144710"></a><a name="p38951422144710"></a><span>将指定双向链表节点初始化为双向链表</span></p>
</td>
</tr>
<tr id="row289552216475"><td class="cellrowborder" valign="top"><p id="p11895222194718"><a name="p11895222194718"></a><a name="p11895222194718"></a><span>LOS_DL_LIST_HEAD</span></p>
</td>
<td class="cellrowborder" valign="top"><p id="p3895222184710"><a name="p3895222184710"></a><a name="p3895222184710"></a>定义一个双向链表节点并以该节点初始化为双向链表</p>
</td>
</tr>
<tr id="row1689522210476"><td class="cellrowborder" rowspan="2" valign="top" width="8.14%"><p id="p12896132294720"><a name="p12896132294720"></a><a name="p12896132294720"></a><span>增加节点</span></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p789611221477"><a name="p789611221477"></a><a name="p789611221477"></a><span>LOS_ListAdd</span></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p9896122134715"><a name="p9896122134715"></a><a name="p9896122134715"></a><span>将指定节点插入到双向链表头端</span></p>
</td>
</tr>
<tr id="row188961225475"><td class="cellrowborder" valign="top"><p id="p18961122154718"><a name="p18961122154718"></a><a name="p18961122154718"></a><span>LOS_ListTailInsert</span></p>
</td>
<td class="cellrowborder" valign="top"><p id="p188961322144712"><a name="p188961322144712"></a><a name="p188961322144712"></a><span>将指定节点插入到双向链表尾端</span></p>
</td>
</tr>
<tr id="row5896112264710"><td class="cellrowborder" rowspan="2" valign="top" width="8.14%"><p id="p28961122174717"><a name="p28961122174717"></a><a name="p28961122174717"></a><span>删除节点</span></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p28961422154710"><a name="p28961422154710"></a><a name="p28961422154710"></a><span>LOS_ListDelete</span></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p289610224473"><a name="p289610224473"></a><a name="p289610224473"></a><span>将指定节点从链表中删除</span></p>
</td>
</tr>
<tr id="row3896522124711"><td class="cellrowborder" valign="top"><p id="p0896152213471"><a name="p0896152213471"></a><a name="p0896152213471"></a><span>LOS_ListDelInit</span></p>
</td>
<td class="cellrowborder" valign="top"><p id="p48965226475"><a name="p48965226475"></a><a name="p48965226475"></a><span>将指定节点从链表中删除，并使用该节点初始化链表</span></p>
</td>
</tr>
<tr id="row1689602294714"><td class="cellrowborder" valign="top" width="8.14%"><p id="p18961222164716"><a name="p18961222164716"></a><a name="p18961222164716"></a><span>判断双向链表是否为空</span></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p78961522184715"><a name="p78961522184715"></a><a name="p78961522184715"></a><span>LOS_ListEmpty</span></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p16896172254720"><a name="p16896172254720"></a><a name="p16896172254720"></a><span>判断链表是否为空</span></p>
</td>
</tr>
<tr id="row128977221474"><td class="cellrowborder" rowspan="2" valign="top" width="8.14%"><p id="p138971322174717"><a name="p138971322174717"></a><a name="p138971322174717"></a><span>获取结构体信息</span></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p138971922194712"><a name="p138971922194712"></a><a name="p138971922194712"></a><span>LOS_DL_LIST_ENTRY</span></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p08971022144720"><a name="p08971022144720"></a><a name="p08971022144720"></a><span>获取包含链表的结构体地址，接口的第一个入参表示的是链表中的某个节点，第二个入参是要获取的结构体名称，第三个入参是链表在该结构体中的名称</span></p>
</td>
</tr>
<tr id="row17897102264718"><td class="cellrowborder" valign="top"><p id="p1889792218473"><a name="p1889792218473"></a><a name="p1889792218473"></a><span>LOS_OFF_SET_OF</span></p>
</td>
<td class="cellrowborder" valign="top"><p id="p1897822194713"><a name="p1897822194713"></a><a name="p1897822194713"></a><span>获取指定结构体内的成员相对于结构体起始地址的偏移量</span></p>
</td>
</tr>
<tr id="row4897192254715"><td class="cellrowborder" rowspan="2" valign="top" width="8.14%"><p id="p3897922164714"><a name="p3897922164714"></a><a name="p3897922164714"></a><span>遍历双向链表</span></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p58971222194713"><a name="p58971222194713"></a><a name="p58971222194713"></a><span>LOS_DL_LIST_FOR_EACH</span></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p28971222194714"><a name="p28971222194714"></a><a name="p28971222194714"></a><span>遍历双向链表</span></p>
</td>
</tr>
<tr id="row589792254710"><td class="cellrowborder" valign="top"><p id="p689792264718"><a name="p689792264718"></a><a name="p689792264718"></a><span>LOS_DL_LIST_FOR_EACH_SAFE</span></p>
</td>
<td class="cellrowborder" valign="top"><p id="p148975222479"><a name="p148975222479"></a><a name="p148975222479"></a><span>遍历双向链表，并存储当前节点的后继节点用于安全校验</span></p>
</td>
</tr>
<tr id="row208971622174718"><td class="cellrowborder" rowspan="2" valign="top" width="8.14%"><p id="p168977224474"><a name="p168977224474"></a><a name="p168977224474"></a><span>遍历包含双向链表的结构体</span></p>
</td>
<td class="cellrowborder" valign="top" width="32.09%"><p id="p1489752216479"><a name="p1489752216479"></a><a name="p1489752216479"></a><span>LOS_DL_LIST_FOR_EACH_ENTRY</span></p>
</td>
<td class="cellrowborder" valign="top" width="59.77%"><p id="p10897522194717"><a name="p10897522194717"></a><a name="p10897522194717"></a><span>遍历指定双向链表，获取包含该链表节点的结构体地址</span></p>
</td>
</tr>
<tr id="row10897622104713"><td class="cellrowborder" valign="top"><p id="p2897112215478"><a name="p2897112215478"></a><a name="p2897112215478"></a><span>LOS_DL_LIST_FOR_EACH_ENTRY_SAFE</span></p>
</td>
<td class="cellrowborder" valign="top"><p id="p13898102220475"><a name="p13898102220475"></a><a name="p13898102220475"></a><span>遍历指定双向链表，获取包含该链表节点的结构体地址，并存储包含当前节点的后继节点的结构体地址</span></p>
</td>
</tr>
</tbody>
</table>

## 开发流程<a name="section01781261552"></a>

双向链表的典型开发流程：

1.  调用LOS\_ListInit/LOS\_DL\_LIST\_HEAD初始双向链表。
2.  调用LOS\_ListAdd向链表插入节点。
3.  调用LOS\_ListTailInsert向链表尾部插入节点。
4.  调用LOS\_ListDelete删除指定节点。
5.  调用LOS\_ListEmpty判断链表是否为空。
6.  调用LOS\_ListDelInit删除指定节点并以此节点初始化链表。

>![](../public_sys-resources/icon-note.gif) **说明：** 
>-   需要注意节点指针前后方向的操作。
>-   链表操作接口，为底层接口，不对入参进行判空，需要使用者确保传参合法。
>-   如果链表节点的内存是动态申请的，删除节点时，要注意释放内存。

## 编程实例<a name="section67569495514"></a>

### 实例描述<a name="section48761994551"></a>

本实例实现如下功能：

1.  初始化双向链表。
2.  增加节点。
3.  删除节点。
4.  测试操作是否成功。

### 示例代码<a name="section1280202685519"></a>

示例代码如下:

```
#include "stdio.h"
#include "los_list.h"

static UINT32 ListSample(VOID)
{
    LOS_DL_LIST listHead = {NULL,NULL};
    LOS_DL_LIST listNode1 = {NULL,NULL};
    LOS_DL_LIST listNode2 = {NULL,NULL};

    //首先初始化链表
    printf("Initial head\n");
    LOS_ListInit(&listHead);

    //添加节点1和节点2，并校验他们的相互关系
    LOS_ListAdd(&listHead, &listNode1);
    if (listNode1.pstNext == &listHead && listNode1.pstPrev == &listHead) {
        printf("Add listNode1 success\n");
    }

    LOS_ListTailInsert(&listHead, &listNode2);
    if (listNode2.pstNext == &listHead && listNode2.pstPrev == &listNode1) {
        printf("Tail insert listNode2 success\n");
    }

    //删除两个节点
    LOS_ListDelete(&listNode1);
    LOS_ListDelete(&listNode2);

    //确认链表为空
    if (LOS_ListEmpty(&listHead)) {
        printf("Delete success\n");
    }

    return LOS_OK;
}
```

### 结果验证<a name="section5811249105512"></a>

编译运行得到的结果为：

```
Initial head 
Add listNode1 success 
Tail insert listNode2 success
Delete success 
```

