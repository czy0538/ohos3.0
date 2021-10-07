# 常见问题<a name="ZH-CN_TOPIC_0000001128311064"></a>

-   [烧写选择串口后提示失败](#section1498892119619)
-   [Windows电脑与单板网络连接失败](#section8512971816)
-   [烧写失败](#section1767804111198)
-   [编译构建过程中，提示找不到“python”](#zh-cn_topic_0000001053466255_section1039835245619)
-   [串口无回显](#zh-cn_topic_0000001053466255_section14871149155911)

## 烧写选择串口后提示失败<a name="section1498892119619"></a>

-   **现象描述**

    点击烧写并选择串口后，出现Error: Opening COMxx: Access denied。

    **图 1**  打开串口失败图<a name="zh-cn_topic_0000001053466255_fig066333283916"></a>  
    ![](figure/打开串口失败图-15.png "打开串口失败图-15")

-   **可能原因**

    串口已经被占用。

-   **解决办法**

1.  按图依次选择下拉框，查找带有serial-xx的终端

    **图 2**  查找是否存在占用串口的终端<a name="zh-cn_topic_0000001053466255_fig165994164420"></a>  
    ![](figure/查找是否存在占用串口的终端-16.png "查找是否存在占用串口的终端-16")

2.  点击标号中的垃圾桶图标，关闭串口。

    **图 3**  关闭串口终端<a name="zh-cn_topic_0000001053466255_fig7911282453"></a>  
    ![](figure/关闭串口终端-17.png "关闭串口终端-17")

3.  重新点击烧写，选择串口并开始烧写程序

    **图 4**  重新启动烧写任务<a name="fig87139519227"></a>  
    ![](figure/重新启动烧写任务-18.png "重新启动烧写任务-18")


## Windows电脑与单板网络连接失败<a name="section8512971816"></a>

-   **现象描述**

    点击烧写并选择串口后，无法获取文件。

    **图 5**  网络不通，单板无法获取文件图<a name="zh-cn_topic_0000001053466255_fig5218920223"></a>  
    ![](figure/网络不通-单板无法获取文件图-19.png "网络不通-单板无法获取文件图-19")

-   **可能原因**

    单板网络与Windows电脑不联通。

    Windows电脑防火墙未允许Visual Studio Code联网。

-   **解决方法**

1.  检查网线是否连接。
2.  点击Windows防火墙。

    **图 6**  网络防火墙设置图<a name="fig954464811215"></a>  
    ![](figure/网络防火墙设置图-20.png "网络防火墙设置图-20")

3.  点击“允许应用通过防火墙”。

    **图 7**  防火墙和网络保护界面图<a name="fig158731526101312"></a>  
    ![](figure/防火墙和网络保护界面图-21.png "防火墙和网络保护界面图-21")

4.  查找Visual Studio Code应用。

    **图 8**  查找Visual Studio Code应用图<a name="fig96572537139"></a>  
    ![](figure/查找Visual-Studio-Code应用图-22.png "查找Visual-Studio-Code应用图-22")

5.  勾选Visual Studio Code的专用和公用网络的访问权限。

    **图 9**  允许Visual Studio Code应用访问网络<a name="fig18201217161411"></a>  
    ![](figure/允许Visual-Studio-Code应用访问网络-23.png "允许Visual-Studio-Code应用访问网络-23")


## 烧写失败<a name="section1767804111198"></a>

-   **现象描述**

    点击烧写并选择串口后，出现无法烧写的情况。

-   **可能原因**

    安装IDE插件DevEco后未重启。

-   **解决方法**

    重启IDE。


## 编译构建过程中，提示找不到“python”<a name="zh-cn_topic_0000001053466255_section1039835245619"></a>

-   **现象描述**

    ![](figure/error.png)


-   **可能原因1**

    没有装python。

-   **解决办法**

    请按照[安装Python环境](quickstart-lite-env-setup-linux.md#section1238412211211)。

-   **可能原因2**

    ![](figure/error2.png)

-   **解决办法**

    usr/bin目录下没有python软链接，请运行以下命令：

    ```
    # cd /usr/bin/
    # which python3
    # ln -s /usr/local/bin/python3 python
    # python --version
    ```

    例：

    ![](figure/solution-24.png)


## 串口无回显<a name="zh-cn_topic_0000001053466255_section14871149155911"></a>

-   **现象描述**

    串口显示已连接，重启单板后，回车无任何回显。

-   **可能原因1**

    串口连接错误。

-   **解决办法**

    修改串口号。

    请查看设备管理器，确认连接单板的串口与终端中连接串口是否一致，若不一致，请按镜像运行内[步骤1](#section1498892119619)修改串口号。


-   **可能原因2**

    单板U-boot被损坏。

-   **解决办法**

    烧写U-boot。

    若上述步骤依旧无法连接串口，可能由于单板U-boot损坏，按下述步骤烧写U-boot。


1.  获取引导文件U-boot。

    >![](../public_sys-resources/icon-notice.gif) **须知：** 
    >单板的U-boot文件请在开源包中获取：
    >Hi3516DV300：device\\hisilicon\\hispark\_taurus\\sdk\_liteos\\uboot\\out\\boot\\u-boot-hi3516dv300.bin
    >Hi3518EV300：device\\hisilicon\\hispark\_aries\\sdk\_liteos\\uboot\\out\\boot\\u-boot-hi3518ev300.bin

2.  根据USB烧写步骤烧写U-boot文件。

    按照[Hi3516系列USB烧写步骤](https://device.harmonyos.com/cn/docs/ide/user-guides/hi3516_upload-0000001052148681)/[Hi3518系列USB烧写步骤](https://device.harmonyos.com/cn/docs/ide/user-guides/hi3518_upload-0000001057313128)中描述的USB烧写方法，选择对应单板的U-boot文件进行烧写。

3.  烧写完成后，登录串口如下图所示。

    ![](figure/login-serial-port.png)


