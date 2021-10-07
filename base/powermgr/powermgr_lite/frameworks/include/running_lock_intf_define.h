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

#ifndef POWERMGR_RUNNING_LOCK_INTF_DEFINE_H
#define POWERMGR_RUNNING_LOCK_INTF_DEFINE_H

#include <iunknown.h>

#include "running_lock_entry.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define POWER_RUNNING_LOCK_FEATURE        "runninglock"

#define INHERIT_RUNNINGLOCK_INTERFACE                                                                           \
    int32_t (*AcquireRunningLockEntryFunc)(IUnknown *iUnknown, RunningLockEntry *entry, int32_t timeoutMs);     \
    int32_t (*ReleaseRunningLockEntryFunc)(IUnknown *iUnknown, RunningLockEntry *entry);                        \
    BOOL (*IsAnyRunningLockHoldingFunc)(IUnknown *iUnknown)

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_RUNNING_LOCK_INTF_DEFINE_H
