#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_device.h"

#include "discovery_service.h"
#include "session.h"
#include "coap_discover.h"
#include "tcp_session_manager.h"
#include "nstackx.h"

#include "wifi_connect_demo.c"

#include <stdio.h>
#include <string.h>

#define NAME_LENGTH 64
#define TRANS_FAILED -1

void onSuccess(int publishId);
void onFail(int publishId, PublishFailReason reason);
int StartSessionServer(void);
void onBytesReceivedFunc(int sessionId, const void *data, unsigned int dataLen);
void onSessionClosedFunc(int sessionId);
int onSessionOpenedFunc(int sessionId);

// 定义相关名称
const char *g_pkgName = "czypkgName3861";
const char *g_sessionName = "czysessionName3861";
const char *moduleName = "czymoduleName3861";

//定义参数
static PublishInfo info =
    {
        .publishId = 256,
        .mode = DISCOVER_MODE_PASSIVE,
        .medium = COAP,
        .freq = HIGH,
        .capability = "dvKit",
        .capabilityData = (unsigned char *)"a",
        .dataLen = sizeof("a")};

static IPublishCallback cb =
    {
        .onPublishSuccess = onSuccess,
        .onPublishFail = onFail};

struct ISessionListener ISLcb =
    {
        .onBytesReceived = onBytesReceivedFunc,
        .onSessionOpened = onSessionOpenedFunc,
        .onSessionClosed = onSessionClosedFunc};
void onSuccess(int publishId)
{
    printf("publish succeeded, publishId = %d\r\n", publishId);

}

void onFail(int publishId, PublishFailReason reason)
{
    printf("publish failed, publishId = %d, reason = %d\r\n", publishId, reason);
}

// 向SoftBus注册业务会话服务及其回调
int StartSessionServer()
{
    int ret = CreateSessionServer(g_pkgName, g_sessionName, &ISLcb);
    if (ret < 0)
    {
        printf("Failed to create session server!\n");
    }
    return ret;
}

// 接收对方通过SendBytes发送的数据
void onBytesReceivedFunc(int sessionId, const void *data, unsigned int dataLen)
{
    printf("onBytesReceived\n");
    printf("Recv Data: %s\n", (char *)data);
    printf("Recv Data dataLen: %d\n", dataLen);
    char *testSendData = "Hello World!";
    SendBytes(sessionId, testSendData, strlen(testSendData));
    return;
}

//会话关闭后的相关业务操作
void onSessionClosedFunc(int sessionId)
{
    printf("Close session successfully, sessionId=%d\n", sessionId);
}

//会话打开后的相关业务操作
int onSessionOpenedFunc(int sessionId)
{
    char sessionNameBuffer[NAME_LENGTH + 1];
    if (GetPeerSessionName(sessionId, sessionNameBuffer, NAME_LENGTH) == TRANS_FAILED)
    {
        printf("GetPeerSessionName faild, which sessionId = %d\n", sessionId);
        return -1;
    }
    if (strcmp(sessionNameBuffer, g_sessionName) != 0)
    {
        printf("Reject the session which name is different from mine, sessionId=%d\n", sessionId);
        return -1;
    }
    printf("Open session successfully, sessionId=%d\n", sessionId);
    return 0;
}

void liteSoftBusTask()
{
    WifiConnectTask();

    //初始化软总线
    printf("enter SoftBus Task\r\n");
    int ret = PublishService(moduleName, &info, &cb);
    if (ret != 0)
    {
        printf("PublishService init failed\n");
    }
    else
    {
        printf("PublishService init success\n");
    }
    //启动传输服务
    char ipbuff[NSTACKX_MAX_IP_STRING_LEN] = {"0.0.0.0"};
    CoapGetIp(ipbuff, NSTACKX_MAX_IP_STRING_LEN, 0);
    printf("CoapGetIp = %s\n", ipbuff);
    if (StartSessionServer() != -1)
        printf("StartSessionServer successed!\n");

    // sleep(60);
    // printf("try to  UnPublishService= %d\r\n", 233);
    // UnPublishService(moduleName,233);
    // printf("UnPublishService successed~\n");
}

SYS_SERVICE_INIT_PRI(liteSoftBusTask, 4);