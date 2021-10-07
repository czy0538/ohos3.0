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

#include "running_lock_feature.h"

#include <feature.h>
#include <ohos_init.h>
#include <samgr_lite.h>

#include "hilog_wrapper.h"
#include "power_mgr.h"
#include "running_lock_framework.h"
#include "running_lock_mgr.h"

const char *GetRunningLockFeatureName(Feature *feature)
{
    (void)feature;
    return POWER_RUNNING_LOCK_FEATURE;
}

void OnRunningLockFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    if (feature == NULL) {
        POWER_HILOGE("Invalid feature");
        return;
    }

    RunningLockFeature *f = (RunningLockFeature *)feature;
    f->identity = identity;
    POWER_HILOGI("Init running lock feature done");
}

void OnRunningLockFeatureStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
    RunningLockFeature *f = GetRunningLockFeatureImpl();
    if (f != NULL) {
        f->identity.queueId = NULL;
        f->identity.featureId = -1;
        f->identity.serviceId = -1;
    }
}

BOOL OnRunningLockFeatureMessage(Feature *feature, Request *request)
{
    return ((feature != NULL) && (request != NULL)) ? TRUE : FALSE;
}

int32_t OnAcquireRunningLockEntry(IUnknown *iUnknown, RunningLockEntry *entry, int32_t timeoutMs)
{
    (void)iUnknown;
    return RunningLockMgrAcquireEntry(entry, timeoutMs);
}

int32_t OnReleaseRunningLockEntry(IUnknown *iUnknown, RunningLockEntry *entry)
{
    (void)iUnknown;
    return RunningLockMgrReleaseEntry(entry);
}

BOOL OnIsAnyRunningLockHolding(IUnknown *iUnknown)
{
    (void)iUnknown;
    return RunningLockMgrIsAnyLockHolding();
}

static void Init(void)
{
    SamgrLite *sam = SAMGR_GetInstance();
    if (sam == NULL) {
        POWER_HILOGE("Failed to get samgr");
        return;
    }

    RunningLockFeature *feature = GetRunningLockFeatureImpl();
    if (feature == NULL) {
        POWER_HILOGE("Failed to get running lock feature");
        return;
    }
    BOOL result = sam->RegisterFeature(POWER_MANAGE_SERVICE, (Feature *)feature);
    if (result == FALSE) {
        POWER_HILOGE("Failed to register running lock feature");
        return;
    }
    result = sam->RegisterFeatureApi(POWER_MANAGE_SERVICE, POWER_RUNNING_LOCK_FEATURE, GET_IUNKNOWN(*feature));
    if (result == FALSE) {
        POWER_HILOGE("Failed to register running lock feature api");
        return;
    }
    RunningLockMgrInit();
    POWER_HILOGI("Succeed to register running lock feature");
}
SYS_FEATURE_INIT(Init);
