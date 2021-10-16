#include "discovery_service.h"
#include "session.h"
#include "softbus_common.h"
#include <stdio.h>
#include <string.h>

// 定义相关名称
const char *g_pkgName = "czypkgName";
const char *g_sessionName = "czysessionName";
const char *moduleName = "czymoduleName";

static SubscribeInfo g_sInfo = {
    .subscribeId = 233,
    .medium = COAP,
    .mode = DISCOVER_MODE_ACTIVE,
    .freq = HIGH,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"a",
    .dataLen = sizeof("a"),
    .isSameAccount = false,
    .isWakeRemote = true};

static void TestDeviceFound(const DeviceInfo *device)
{
    printf("[czy_test]TestDeviceFound\n");
}

static void TestDiscoverFailed(int subscribeId, DiscoveryFailReason failReason)
{
    printf("[czy_test]TestDiscoverFailed\n");
}

static void TestDiscoverySuccess(int subscribeId)
{
    printf("[czy_test]TestDiscoverySuccess\n");
}

static IDiscoveryCallback g_subscribeCb = {
    .OnDeviceFound = TestDeviceFound,
    .OnDiscoverFailed = TestDiscoverFailed,
    .OnDiscoverySuccess = TestDiscoverySuccess};

int main()
{
    printf("start discoveryTask\n");
    int ret = StartDiscovery(g_pkgName, &g_sInfo, &g_subscribeCb);
    printf("[czy_test]ret is:%d\n",ret);
    while(ret!=233)
    {
        printf("[czy_test]waiting!!!\n");
        scanf("%d", &ret);
    }
    return 0;
}
