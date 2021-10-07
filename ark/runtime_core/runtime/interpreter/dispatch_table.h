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

#ifndef PANDA_RUNTIME_INTERPRETER_DISPATCH_TABLE_H_
#define PANDA_RUNTIME_INTERPRETER_DISPATCH_TABLE_H_

#include <array>
#include <cstddef>
#include <cstdint>

#ifdef PANDA_ENABLE_GLOBAL_REGISTER_VARIABLES
#include "arch/global_regs.h"
#endif

namespace panda::interpreter {

#ifdef PANDA_ENABLE_GLOBAL_REGISTER_VARIABLES

template <size_t N>
ALWAYS_INLINE inline void SetDispatchTable(const std::array<const void *, N> &dispatch_table)
{
    arch::regs::SetDispatchTable(dispatch_table.data());
}

template <size_t N>
ALWAYS_INLINE inline const void *const *GetDispatchTable([
    [maybe_unused]] const std::array<const void *, N> &dispatch_table)
{
    return arch::regs::GetDispatchTable();
}

#else

template <size_t N>
ALWAYS_INLINE inline void SetDispatchTable([[maybe_unused]] const std::array<const void *, N> &dispatch_table)
{
}

template <size_t N>
ALWAYS_INLINE inline const void *const *GetDispatchTable(const std::array<const void *, N> &dispatch_table)
{
    return dispatch_table.data();
}

#endif  // PANDA_ENABLE_GLOBAL_REGISTER_VARIABLES

}  // namespace panda::interpreter

#endif  // PANDA_RUNTIME_INTERPRETER_DISPATCH_TABLE_H_
