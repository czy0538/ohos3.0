# 应用验签开发指导<a name="ZH-CN_TOPIC_0000001058671627"></a>

-   [场景介绍](#section18502174174019)
-   [验签流程](#section554632717226)
-   [接口说明](#section1633115419401)
-   [开发步骤](#section4207112818418)
    -   [生成OpenHarmony签名应用](#section167151429133312)


## 场景介绍<a name="section18502174174019"></a>

为了确保应用的完整性和来源可靠，OpenHarmony需要对应用进行签名和验签。

-   应用开发阶段：开发者完成开发并生成安装包后，需要开发者对安装包进行签名，以证明安装包发布到设备的过程中没有被篡改。OpenHarmony的应用完整性校验模块提供了签名工具、签名证书生成规范，以及签名所需的公钥证书等完整的机制，支撑开发者对应用安装包签名。为了方便开源社区开发者，版本中预置了公钥证书和对应的私钥，为开源社区提供离线签名和校验能力；在OpenHarmony商用版本中应替换此公钥证书和对应的私钥。
-   应用安装阶段：OpenHarmony用户程序框架子系统负责应用的安装。在接收到应用安装包之后，应用程序框架子系统需要解析安装包的签名数据，然后使用应用完整性校验模块的API对签名进行验证，只有校验成功之后才允许安装此应用. 应用完整性校验模块在校验安装包签名数据时，会使用系统预置的公钥证书进行验签。

## 验签流程<a name="section554632717226"></a>

未经签名的Hap包的压缩方式是ZIP格式，简单分为文件块，中心目录（Central directory）块，中心目录结尾（EOCD，End of central directory record）块。

经过签名的Hap包，在文件块，和中心目录块之间，插入了签名块。签名块由整包签名数据块（data sign block）、授权文件签名数据块（profile sign block）和签名头（sign head）组成，如下图所示。

**图 1**  经过签名的Hap包结构<a name="fig699855043"></a>  


![](figure/zh-cn_image_0000001181934155.png)

整个验签流程，主要分为三部分：整包验签、授权文件验签，以及授权文件内容校验。

**整包验签**

整包签名数据块是一个PKCS7格式的签名块（signed data），验签过程包括PKSC7签名验证、哈希比较、证书链验证以及证书链与设备预置根证书的匹配校验。

**授权文件验签**

授权文件数据块是一个PKCS7格式的签名块（signed data），其中PKCS7签名块的内容信息\(contentinfo\)是授权文件的内容。验签过程包括：PKCS7签名验证、哈希比较、证书链验证以及签发授权文件证书的合法性校验。

**授权文件内容校验**

验签模块将对授权文件内容进行合法性检查。如果授权文件是调试类型，则会比对本机UDID是否在授权文件授权调试的UDID列表中，如果本机UDID在授权文件授权调试的UDID列表中，则会进一步比较授权文件中的调试证书和整包签名使用的证书是否相同，如果相同，则验证通过。

## 接口说明<a name="section1633115419401"></a>

验签组件当前提供innerkits接口，仅供系统应用调用，相关接口及功能描述如下：

**表 1**  轻量级系统验签组件API接口功能介绍

<a name="table1731550155318"></a>
<table><thead align="left"><tr id="row4419501537"><th class="cellrowborder" valign="top" width="37.66%" id="mcps1.2.3.1.1"><p id="p54150165315"><a name="p54150165315"></a><a name="p54150165315"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="62.339999999999996%" id="mcps1.2.3.1.2"><p id="p941150145313"><a name="p941150145313"></a><a name="p941150145313"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row34145016535"><td class="cellrowborder" valign="top" width="37.66%" headers="mcps1.2.3.1.1 "><p id="p122711391160"><a name="p122711391160"></a><a name="p122711391160"></a>int APPVERI_AppVerify(const char *filePath, VerifyResult *verifyRst)</p>
</td>
<td class="cellrowborder" valign="top" width="62.339999999999996%" headers="mcps1.2.3.1.2 "><p id="p13562171015712"><a name="p13562171015712"></a><a name="p13562171015712"></a>主入口函数，输入文件路径，进行验签，并将从描述文件中获取的数据通过verifyRst返回给调用者</p>
</td>
</tr>
<tr id="row1746172917474"><td class="cellrowborder" valign="top" width="37.66%" headers="mcps1.2.3.1.1 "><p id="p2131144717619"><a name="p2131144717619"></a><a name="p2131144717619"></a>int APPVERI_SetDebugMode(bool mode)</p>
</td>
<td class="cellrowborder" valign="top" width="62.339999999999996%" headers="mcps1.2.3.1.2 "><p id="p1577114614282"><a name="p1577114614282"></a><a name="p1577114614282"></a>设置测试模式，设置mode为true，则支持基于测试根密钥的证书链校验，设置mode为false，则关闭基于测试根密钥的证书链校验。</p>
<p id="p2431455765"><a name="p2431455765"></a><a name="p2431455765"></a>注：当前没有基于现有测试根密钥的证书，开发者可根据自身需要，替换测试根密钥并进行相关验证。</p>
</td>
</tr>
<tr id="row10992232154714"><td class="cellrowborder" valign="top" width="37.66%" headers="mcps1.2.3.1.1 "><p id="p685215538611"><a name="p685215538611"></a><a name="p685215538611"></a>void APPVERI_FreeVerifyRst(VerifyResult *verifyRst)</p>
</td>
<td class="cellrowborder" valign="top" width="62.339999999999996%" headers="mcps1.2.3.1.2 "><p id="p126575774517"><a name="p126575774517"></a><a name="p126575774517"></a>释放verifyRst中申请的内存</p>
</td>
</tr>
</tbody>
</table>

**表 2**  标准系统验签组件API接口功能介绍

<a name="table10383348161613"></a>
<table><thead align="left"><tr id="row9384114813161"><th class="cellrowborder" valign="top" width="37.66%" id="mcps1.2.3.1.1"><p id="p038411483162"><a name="p038411483162"></a><a name="p038411483162"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="62.339999999999996%" id="mcps1.2.3.1.2"><p id="p9384124871618"><a name="p9384124871618"></a><a name="p9384124871618"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row11384194871614"><td class="cellrowborder" valign="top" width="37.66%" headers="mcps1.2.3.1.1 "><p id="p138411487168"><a name="p138411487168"></a><a name="p138411487168"></a>nt HapVerify(const std::string&amp; filePath, HapVerifyResult&amp; hapVerifyResult)</p>
</td>
<td class="cellrowborder" valign="top" width="62.339999999999996%" headers="mcps1.2.3.1.2 "><p id="p0384184810169"><a name="p0384184810169"></a><a name="p0384184810169"></a>校验应用完整性，识别应用来源。</p>
</td>
</tr>
</tbody>
</table>

## 开发步骤<a name="section4207112818418"></a>

### 生成OpenHarmony签名应用<a name="section167151429133312"></a>

参考文档：https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/%E9%85%8D%E7%BD%AEOpenHarmony%E5%BA%94%E7%94%A8%E7%AD%BE%E5%90%8D%E4%BF%A1%E6%81%AF.md

