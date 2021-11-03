#include "discovery_service.h"
#include "softbus_common.h"
#include "softbus_bus_center.h"
#include "session.h"

#include "softbus_discovery.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 定义相关名称
const char *g_pkgName = "czypkgName3516";
const char *g_sessionName = "czysessionName3516";
const char *moduleName = "czymoduleName3516";

static ConnectionAddr * connectionAddrPtr=NULL;

static SubscribeInfo SubInfo = {
    .subscribeId = 233,
    .medium = COAP,
    .mode = DISCOVER_MODE_ACTIVE,
    .freq = HIGH,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"a",
    .dataLen = sizeof("a"),
    .isSameAccount = false,
    .isWakeRemote = true};

static IDiscoveryCallback DiscCb = {
    .OnDeviceFound = TestDeviceFound,
    .OnDiscoverFailed = TestDiscoverFailed,
    .OnDiscoverySuccess = TestDiscoverySuccess};

static ISessionListener SessionCb =
    {
        .OnBytesReceived = onBytesReceivedFunc,
        .OnSessionOpened = onSessionOpenedFunc,
        .OnSessionClosed = onSessionClosedFunc,
        .OnMessageReceived=onMessageReceivedFunc,
        .OnStreamReceived=onStreamReceivedFunc
    };


int main()
{
    printf("start discoveryTask\n");
    int ret = StartDiscovery(g_pkgName, &SubInfo, &DiscCb);
    printf("[czy_test]ret is:%d\n",ret);
    while(ret!=233)
    {
        printf("[czy_test]waiting!!!\n");
        scanf("%d", &ret);
    }
    printf("[czy_test]stop discoveryTask\n");
    StopDiscovery(g_pkgName,233);
    return 0;
}

// 接收对方通过SendBytes发送的数据
void onBytesReceivedFunc(int sessionId, const void *data, unsigned int dataLen)
{
    printf("[czy_test_session]onBytesReceived\n");
    printf("[czy_test_session]Recv Data: %s\n", (char *)data);
    printf("[czy_test_session]Recv Data dataLen: %d\n", dataLen);
    char *testSendData = "[czy_test_session]Hello World!";
    SendBytes(sessionId, testSendData, strlen(testSendData));
    return;
}

//会话关闭后的相关业务操作
void onSessionClosedFunc(int sessionId)
{
    printf("[czy_test_session]Close session successfully, sessionId=%d\n", sessionId);
}

//会话打开后的相关业务操作
int onSessionOpenedFunc(int sessionId)
{
    printf("[czy_test_session]Open session successfully, sessionId=%d\n", sessionId);
    return 0;
}

int StartSessionServer()
{
    int ret = CreateSessionServer(g_pkgName, g_sessionName, &SessionCb);
    if (ret < 0)
    {
        printf("[czy_test_session]Failed to create session server!\n");
    }
    return ret;
}
void TestDeviceFound(const DeviceInfo *device)
{
    printf("[czy_test_DeviceFound]TestDeviceFound,here are some info about Device:\r\n");
    printf("[czy_test_DeviceFound] devId %s:\r\n",device->devId);
    printf("[czy_test_DeviceFound] devType %d\r\n",device->devType);
    printf("[czy_test_DeviceFound] devName %s\r\n",device->devName);
    printf("[czy_test_DeviceFound] addrNum %d\r\n",device->addrNum);
   // printf("[czy_test_DeviceFound] addr %s\r\n",device->addr.info.ip.ip);
    connectionAddrPtr=(ConnectionAddr *)malloc(sizeof(ConnectionAddr));
    if(!connectionAddrPtr)
    {
        printf("[czy_test_DeviceFound] malloc error\r\n");
        return;
    }
    memcpy(connectionAddrPtr,&(device->addr),sizeof(ConnectionAddr));
    int ret=JoinLNN(g_pkgName,connectionAddrPtr,onJoinLNNResultFunc);
    if(ret==0)
    {
        printf("[czy_test] %d JoinLNN success, \r\n");
    }
    else
    {
        printf("[czy_test] %d JoinLNN fail,ret is %d,ret \r\n");
    }
}

void TestDiscoverFailed(int subscribeId, DiscoveryFailReason failReason)
{
    printf("[czy_test]%d TestDiscoverFailed\r\n",subscribeId);
}

void TestDiscoverySuccess(int subscribeId)
{
    printf("[czy_test] %d TestDiscoverySuccess\r\n",subscribeId);
}

void onJoinLNNResultFunc(ConnectionAddr *addr, const char *networkId, int32_t retCode)
{
    printf("[czy_test] JoinLNN retCode is %d \r\n",retCode);
}

void onMessageReceivedFunc(int sessionId, const void *data, unsigned int dataLen)
{
    printf("onMessageReceivedFunc\r\n");

}
void onStreamReceivedFunc(int sessionId, const StreamData *data, const StreamData *ext, const FrameInfo *param)
{
    printf("onStreamReceivedFunc\r\n");
}