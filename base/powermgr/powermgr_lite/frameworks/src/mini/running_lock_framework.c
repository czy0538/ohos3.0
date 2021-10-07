/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "running_lock_framework.h"

#include <stdint.h>

#include <pthread.h>

#include "hilog_wrapper.h"
#include "running_lock_interface.h"

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static RunningLockInterface *g_intf = NULL;

static RunningLockInterface *GetRunningLockInterface(void)
{
    if (g_intf != NULL) {
        return g_intf;
    }
    pthread_mutex_lock(&g_mutex);
    if (g_intf != NULL) {
        pthread_mutex_unlock(&g_mutex);
        return g_intf;
    }
    IUnknown *iUnknown = GetRunningLockIUnknown();
    if (iUnknown == NULL) {
        POWER_HILOGE("Failed to get running lock iUnknown");
        return NULL;
    }

    int ret = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&g_intf);
    if ((ret != EC_SUCCESS) || (g_intf == NULL)) {
        POWER_HILOGE("Failed to query running lock interface");
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }
    pthread_mutex_unlock(&g_mutex);

    return g_intf;
}

void InitIdentity(RunningLockEntry *entry)
{
    if (entry == NULL) {
        return;
    }
    entry->identity.pid = 0;
    entry->identity.token = (uint64_t)(uintptr_t)entry;
}

BOOL AcquireRunningLockEntry(RunningLockEntry *entry, int32_t timeoutMs)
{
    int32_t ret = EC_FAILURE;
    RunningLockInterface *intf = GetRunningLockInterface();
    if ((intf != NULL) && (intf->AcquireRunningLockEntryFunc != NULL)) {
        ret = intf->AcquireRunningLockEntryFunc((IUnknown *)intf, entry, timeoutMs);
    }
    return (ret == EC_SUCCESS) ? TRUE : FALSE;
}

BOOL ReleaseRunningLockEntry(RunningLockEntry *entry)
{
    int32_t ret = EC_FAILURE;
    RunningLockInterface *intf = GetRunningLockInterface();
    if ((intf != NULL) && (intf->ReleaseRunningLockEntryFunc != NULL)) {
        ret = intf->ReleaseRunningLockEntryFunc((IUnknown *)intf, entry);
    }
    return (ret == EC_SUCCESS) ? TRUE : FALSE;
}

BOOL IsAnyRunningLockHolding()
{
    BOOL ret = FALSE;
    RunningLockInterface *intf = GetRunningLockInterface();
    if ((intf != NULL) && (intf->IsAnyRunningLockHoldingFunc != NULL)) {
        ret = intf->IsAnyRunningLockHoldingFunc((IUnknown *)intf);
    }
    return ret;
}