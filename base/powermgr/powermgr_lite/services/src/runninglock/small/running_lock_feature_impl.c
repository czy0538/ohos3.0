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

#include <ohos_errno.h>
#include <iproxy_server.h>

#include <pthread.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "running_lock_feature.h"

typedef int32_t (*InvokeFunc)(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply);
static int32_t AcquireInvoke(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply);
static int32_t ReleaseInvoke(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply);
static int32_t IsAnyHoldingInvoke(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply);
static int32_t FeatureInvoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply);

static RunningLockFeature g_feature = {
    RUNNING_LOCK_FEATURE_INTERFACE_IMPL,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = FeatureInvoke,
    RUNNING_LOCK_INTERFACE_IMPL,
    IPROXY_END,
    .identity = { -1, -1, NULL },
};

static InvokeFunc g_invokeFuncs[RUNNINGLOCK_FUNCID_BUTT] = {
    AcquireInvoke,
    ReleaseInvoke,
    IsAnyHoldingInvoke,
};

RunningLockFeature *GetRunningLockFeatureImpl(void)
{
    return &g_feature;
}

static int32_t FeatureInvoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    if ((iProxy == NULL) || (origin == NULL) || (req == NULL)) {
        POWER_HILOGE("Invalid parameter");
        return EC_INVALID;
    }
    POWER_HILOGD("Running lock feature invoke function id: %d", funcId);
    return (funcId >= 0 && funcId < RUNNINGLOCK_FUNCID_BUTT) ? g_invokeFuncs[funcId](iProxy, origin, req, reply) :
        EC_FAILURE;
}

static int32_t AcquireInvoke(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply)
{
    size_t len = 0;
    void *data = (void *)IpcIoPopFlatObj(req, &len);
    int32_t timeoutMs = IpcIoPopInt32(req);
    int32_t ret = OnAcquireRunningLockEntry((IUnknown *)iProxy, (RunningLockEntry *)data, timeoutMs);
    IpcIoPushInt32(reply, ret);
    return EC_SUCCESS;
}

static int32_t ReleaseInvoke(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply)
{
    size_t len = 0;
    void *data = (void *)IpcIoPopFlatObj(req, &len);
    int32_t ret = OnReleaseRunningLockEntry((IUnknown *)iProxy, (RunningLockEntry *)data);
    IpcIoPushInt32(reply, ret);
    return EC_SUCCESS;
}

static int32_t IsAnyHoldingInvoke(IServerProxy *iProxy, void *origin, IpcIo *req, IpcIo *reply)
{
    BOOL ret = OnIsAnyRunningLockHolding((IUnknown *)iProxy);
    IpcIoPushBool(reply, ret == TRUE);
    return EC_SUCCESS;
}
