/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PANDA_LIBPANDABASE_OS_UNIX_LIBRARY_LOADER_H_
#define PANDA_LIBPANDABASE_OS_UNIX_LIBRARY_LOADER_H_

#include "macros.h"

#include <dlfcn.h>

namespace panda::os::unix::library_loader {

class LibraryHandle {
public:
    explicit LibraryHandle(void *handle) : handle_(handle) {}

    LibraryHandle(LibraryHandle &&handle) noexcept
    {
        handle_ = handle.handle_;
        handle.handle_ = nullptr;
    }

    LibraryHandle &operator=(LibraryHandle &&handle) noexcept
    {
        handle_ = handle.handle_;
        handle.handle_ = nullptr;
        return *this;
    }

    bool IsValid() const
    {
        return handle_ != nullptr;
    }

    void *GetNativeHandle() const
    {
        return handle_;
    }

    ~LibraryHandle()
    {
        if (handle_ != nullptr) {
            dlclose(handle_);
        }
    }

private:
    void *handle_;

    NO_COPY_SEMANTIC(LibraryHandle);
};

}  // namespace panda::os::unix::library_loader

#endif  // PANDA_LIBPANDABASE_OS_UNIX_LIBRARY_LOADER_H_
