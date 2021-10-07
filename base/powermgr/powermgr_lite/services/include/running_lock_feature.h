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

#ifndef POWERMGR_RUNNING_LOCK_FEATURE_H
#define POWERMGR_RUNNING_LOCK_FEATURE_H

#include <feature.h>

#include "running_lock_interface.h"
#include "running_lock_mgr.h"

typedef struct {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(RunningLockInterface);
    Identity identity;
} RunningLockFeature;

const char *GetRunningLockFeatureName(Feature *feature);
void OnRunningLockFeatureInitialize(Feature *feature, Service *parent, Identity identity);
void OnRunningLockFeatureStop(Feature *feature, Identity identity);
BOOL OnRunningLockFeatureMessage(Feature *feature, Request *request);
int32_t OnAcquireRunningLockEntry(IUnknown *iUnknown, RunningLockEntry *entry, int32_t timeoutMs);
int32_t OnReleaseRunningLockEntry(IUnknown *iUnknown, RunningLockEntry *entry);
BOOL OnIsAnyRunningLockHolding(IUnknown *iUnknown);

#define RUNNING_LOCK_FEATURE_INTERFACE_IMPL                         \
    .GetName = GetRunningLockFeatureName,                                      \
    .OnInitialize = OnRunningLockFeatureInitialize,                            \
    .OnStop = OnRunningLockFeatureStop,                                        \
    .OnMessage = OnRunningLockFeatureMessage

#define RUNNING_LOCK_INTERFACE_IMPL                                 \
    .AcquireRunningLockEntryFunc = OnAcquireRunningLockEntry,       \
    .ReleaseRunningLockEntryFunc = OnReleaseRunningLockEntry,       \
    .IsAnyRunningLockHoldingFunc = OnIsAnyRunningLockHolding

RunningLockFeature *GetRunningLockFeatureImpl(void);

#endif // POWERMGR_RUNNING_LOCK_FEATURE_H
