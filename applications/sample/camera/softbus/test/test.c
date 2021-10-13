#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "softbus_common.h"
#include "discovery_service.h"

static int g_subscribeId = 0;
static int g_publishId = 0;
static const char *g_pkgName = "Softbus_czy";

static int GetSubscribeId(void)
{
    g_subscribeId++;
    return g_subscribeId;
}

static int GetPublishId(void)
{
    g_publishId++;
    return g_publishId;
}

static SubscribeInfo g_sInfo = {
    .subscribeId = 1,
    .medium = COAP,
    .mode = DISCOVER_MODE_ACTIVE,
    .freq = MID,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"capdata3",
    .dataLen = sizeof("capdata3"),
    .isSameAccount = true,
    .isWakeRemote = false};

static PublishInfo g_pInfo = {
    .publishId = 1,
    .medium = COAP,
    .mode = DISCOVER_MODE_ACTIVE,
    .freq = MID,
    .capability = "dvKit",
    .capabilityData = (unsigned char *)"capdata4",
    .dataLen = sizeof("capdata4")};

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

static void TestPublishSuccess(int publishId)
{
    printf("[czy_test]TestPublishSuccess\n");
}

static void TestPublishFail(int publishId, PublishFailReason reason)
{
    printf("[czy_test]TestPublishFail\n");
}

static IDiscoveryCallback g_subscribeCb = {
    .OnDeviceFound = TestDeviceFound,
    .OnDiscoverFailed = TestDiscoverFailed,
    .OnDiscoverySuccess = TestDiscoverySuccess};

static IPublishCallback g_publishCb = {
    .OnPublishSuccess = TestPublishSuccess,
    .OnPublishFail = TestPublishFail};

void PublishServiceTest()
{
    int ret;

    g_pInfo.publishId = GetPublishId();
    ret = PublishService(g_pkgName, &g_pInfo, &g_publishCb);
    if (ret == 0)
    {
        printf("[czy_test]PublishServiceTest success\n");
    }
    else
    {
        printf("[czy_test]PublishServiceTest fail\n");
    }
}

void StartDiscoveryTest()
{
    int ret;
    g_sInfo.subscribeId = GetSubscribeId();
    ret = StartDiscovery(g_pkgName, &g_sInfo, &g_subscribeCb);
    if (ret == 0)
    {
        printf("[czy_test]StartDiscoveryTest success\n");
    }
    else
    {
        printf("[czy_test]StartDiscoveryTest fail\n");
    }
}

void UnPublishServiceTest()
{
    int ret;
    ret = UnPublishService(g_pkgName, g_pInfo.publishId);
    if (ret == 0)
    {
        printf("[czy_test]UnPublishServiceTest success\n");
    }
    else
    {
        printf("[czy_test]UnPublishServiceTest fail\n");
    }
}

void StopDiscoveryTest()
{
    int ret;
    ret = StopDiscovery(g_pkgName, g_pInfo.publishId);
    if (ret == 0)
    {
        printf("[czy_test]StopDiscoveryTest success\n");
    }
    else
    {
        printf("[czy_test]StopDiscoveryTest fail\n");
    }
}

int main()
{
    int num;
    printf("[czy_test]start test\n");
    while (1)
    {
        printf("[czy_test]please input num 1~4\n");
        scanf("%d", &num);
        switch (num)
        {
        case 1:
            PublishServiceTest();
            break;
        case 2:
            UnPublishServiceTest();
            break;
        case 3:
            StartDiscoveryTest();
            break;
        case 4:
            StopDiscoveryTest();
            break;
        default:
            printf("[czy_test]exit\n");
            return 0;
            break;
        }
    }
    return 0;
}