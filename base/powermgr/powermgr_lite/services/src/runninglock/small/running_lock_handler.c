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

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hilog_wrapper.h"

static int32_t WriteLock(int32_t fd, const char *name)
{
    if (name == NULL) {
        POWER_HILOGE("Invalid lock name");
        return -1;
    }
    int32_t ret = write(fd, name, strlen(name));
    if (ret < 0) {
        POWER_HILOGE("Failed to write lock: %s, error: %s", name, strerror(errno));
        return -1;
    }
    return 0;
}

void RunningLockHandlerLock(const char *name)
{
    static int32_t fd = -1;

    if (fd < 0) {
        fd = open("/proc/power/power_lock", O_RDWR);
        if (fd < 0) {
            POWER_HILOGE("Failed to open power_lock: %s", strerror(errno));
            return;
        }
    }
    int32_t ret = WriteLock(fd, name);
    POWER_HILOGI("Write %s to power_lock, ret: %d", name, ret);
}

void RunningLockHandlerUnlock(const char *name)
{
    static int32_t fd = -1;

    if (fd < 0) {
        fd = open("/proc/power/power_unlock", O_RDWR);
        if (fd < 0) {
            POWER_HILOGE("Failed to open power_unlock: %s", strerror(errno));
            return;
        }
    }
    int32_t ret = WriteLock(fd, name);
    POWER_HILOGI("Write %s to power_unlock, ret: %d", name, ret);
}
