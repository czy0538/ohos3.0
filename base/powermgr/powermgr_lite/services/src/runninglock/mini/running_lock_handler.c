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

#include <stdint.h>

#include <los_pm.h>

#include "hilog_wrapper.h"

void RunningLockHandlerLock(const char *name)
{
    if (name == NULL) {
        POWER_HILOGE("Invalid lock name");
        return;
    }
    uint32_t ret = LOS_PmLockRequest(name);
    POWER_HILOGI("Lock request, name: %s, ret: %u", name, ret);
}

void RunningLockHandlerUnlock(const char *name)
{
    if (name == NULL) {
        POWER_HILOGE("Invalid lock name");
        return;
    }
    uint32_t ret = LOS_PmLockRelease(name);
    POWER_HILOGI("Lock release, name: %s, ret: %u", name, ret);
}
