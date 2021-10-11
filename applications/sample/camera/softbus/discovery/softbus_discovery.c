#include "discovery_service.h"
#include "session.h"
#include "softbus_common.h"
#include <stdio.h>
#include <string.h>

// 定义相关名称
const char *g_pkgName = "czy";
const char *g_sessionName = "czy";
const char *moduleName = "czy";

static SubscribeInfo g_sInfo = {
    .subscribeId = 1,
    .medium = COAP,
    .mode = DISCOVER_MODE_ACTIVE,
    .freq = MID,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"capdata",
    .dataLen = sizeof("capdata"),
    .isSameAccount = true,
    .isWakeRemote = false};

static void TestDeviceFound(const DeviceInfo *device)
{
    printf("[client]TestDeviceFound\n");
}

static void TestDiscoverFailed(int subscribeId, DiscoveryFailReason failReason)
{
    printf("[client]TestDiscoverFailed\n");
}

static void TestDiscoverySuccess(int subscribeId)
{
    printf("[client]TestDiscoverySuccess\n");
}

static IDiscoveryCallback g_subscribeCb = {
    .OnDeviceFound = TestDeviceFound,
    .OnDiscoverFailed = TestDiscoverFailed,
    .OnDiscoverySuccess = TestDiscoverySuccess};

int main()
{
    printf("start discoveryTask\n");
    int ret = StartDiscovery(g_pkgName, &g_sInfo, &g_subscribeCb);
    printf("ret is:%d",ret);
    return 0;
}
