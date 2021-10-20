#include "discovery_service.h"
#include "session.h"
#include "softbus_common.h"
#include "nstackx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NAME_LENGTH 64
#define TRANS_FAILED -1

// 定义业务⾃身的业务名称，会话名称及相关回调
const char *g_pkgName = "czyPkgName";
const char *g_sessionName = "czySessionName";
const char *g_demoModuleName = "czyModuleName";

static PublishInfo g_publishInfo = {
    .capabilityData = (unsigned char *)"a",
    .capability = "dvKit",
    .dataLen = sizeof("a"),
    .publishId = 233,
    .mode = DISCOVER_MODE_ACTIVE,
    .medium = COAP,
    .freq = HIGH,
};

// 回调实现：接收对方通过SendBytes发送的数据，此示例实现是接收到对端发送的数据后回复固定消息
void OnBytesReceivedTest(int sessionId, const void *data, unsigned int dataLen)
{
    printf("OnBytesReceivedTest\n");
    printf("Recv Data: %s\n", (char *)data);
    printf("Recv Data dataLen: %d\n", dataLen);
    char *testSendData = "Hello World, Hello!";
    SendBytes(sessionId, testSendData, strlen(testSendData));
    return;
}

// 回调实现：用于处理会话关闭后的相关业务操作，如释放当前会话相关的业务资源，会话无需业务主动释放
void OnSessionClosedEventTest(int sessionId)
{
    printf("Close session successfully, sessionId=%d\n", sessionId);
}

// 回调实现：用于处理会话打开后的相关业务操作。返回值为0，表示接收；反之，非0表示拒绝。此示例表示只接受其他设备的同名会话连接
int OnSessionOpenedEventTest(int sessionId)
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

ISessionListener s_sessionCallback =
{
        .OnBytesReceived = OnBytesReceivedTest,
        .OnSessionOpened = OnSessionOpenedEventTest,
        .OnSessionClosed = OnSessionClosedEventTest};
// 向SoftBus注册业务会话服务及其回调
int StartSessionServer()
{
    int ret = CreateSessionServer(g_pkgName, g_sessionName, &s_sessionCallback);
    if (ret < 0)
    {
        printf("Failed to create session server!\n");
    }
    return ret;
}

// 从SoftBus中删除业务会话服务及其回调
void StopSessionServer()
{
    int ret = RemoveSessionServer(g_pkgName, g_sessionName);
    if (ret < 0)
    {
        printf("Failed to remove session server!\n");
        return;
    }
}

// 回调函数声明：
void onSuccess(int publishId)
{
    printf("publish succeeded, publishId = %d\r\n", publishId);
    if (StartSessionServer() != -1)
        printf("-----StartSessionServer successed!-----\n");
}
void onFail(int publishId, PublishFailReason reason)
{
    printf("publish failed, publishId = %d, reason = %d\r\n", publishId, reason);
}


static IPublishCallback g_publishCallback = {
    .OnPublishSuccess = onSuccess,
    .OnPublishFail = onFail,
};
int main()
{
    printf("[czy_test]----------enter publish service--------\n");
    int ret = PublishService(g_pkgName, &g_publishInfo, &g_publishCallback);
    if (ret != 0)
    {
        printf("[czy_test]PublishService init failed\n");
    }
    else
    {
        printf("[czy_test]PublishService init success\n");
    }
    while(ret!=233)
    {
        printf("[czy_test]waiting!!!\n");
        scanf("%d", &ret);
    }
    printf("[czy_test]stop discoveryTask\n");
    UnPublishService(g_pkgName,233);
    return 0;
}
