# 常见问题<a name="ZH-CN_TOPIC_0000001166804465"></a>

-   **现象描述**

    Linux编译服务器终端输入不识别的命令时，提示"ImportError: No module named apt\_pkg"


-   **可能原因**

    python3 apt安装兼容性问题。

-   **解决办法**

    执行如下命令重新安装python3-apt。

    ```
    sudo apt-get remove  python3-apt
    sudo apt-get install python3-apt
    ```


