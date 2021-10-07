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

#ifndef ECMASCRIPT_COMPILER_FASTSTUB_DEFINE_H
#define ECMASCRIPT_COMPILER_FASTSTUB_DEFINE_H

namespace kungfu {
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXTERNAL_RUNTIMESTUB_LIST(V) \
    V(AddElementInternal, 5)         \
    V(CallSetter, 2)                 \
    V(ThrowTypeError, 2)             \
    V(JSProxySetProperty, 6)         \
    V(GetHash32, 2)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FAST_RUNTIME_STUB_LIST(V)   \
    V(FastAdd, 2)                   \
    V(FastSub, 2)                   \
    V(FastMul, 2)                   \
    V(FastDiv, 2)                   \
    V(FastMod, 2)                   \
    V(FastEqual, 2)                 \
    V(FastTypeOf, 2)                \
    V(FastStrictEqual, 2)           \
    V(IsSpecialIndexedObjForSet, 1) \
    V(IsSpecialIndexedObjForGet, 1) \
    V(GetPropertyByName, 3)         \
    V(GetElement, 2)                \
    V(SetElement, 5)                \
    V(SetPropertyByName, 5)         \
    V(SetGlobalOwnProperty, 5)      \
    V(GetGlobalOwnProperty, 3)      \
    V(SetOwnPropertyByName, 4)      \
    V(SetOwnElement, 4)             \
    V(FastSetProperty, 5)           \
    V(FastGetProperty, 3)           \
    V(FindOwnProperty, 6)           \
    V(FindOwnElement, 2)            \
    V(NewLexicalEnvDyn, 4)          \
    V(FindOwnProperty2, 6)          \
    V(FindOwnElement2, 6)

#define CALL_STUB_LIST(V)     \
    FAST_RUNTIME_STUB_LIST(V) \
    EXTERNAL_RUNTIMESTUB_LIST(V)

enum CallStubId {
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEF_FAST_STUB(name, counter) NAME_##name,
    FAST_RUNTIME_STUB_LIST(DEF_FAST_STUB) FAST_STUB_MAXCOUNT,
    EXTERNAL_RUNTIME_STUB_BEGIN = FAST_STUB_MAXCOUNT - 1,
    EXTERNAL_RUNTIMESTUB_LIST(DEF_FAST_STUB) EXTERN_RUNTIME_STUB_MAXCOUNT,
#undef DEF_FAST_STUB
    CALL_STUB_MAXCOUNT = EXTERN_RUNTIME_STUB_MAXCOUNT,
};
}  // namespace kungfu
#endif  // ECMASCRIPT_COMPILER_FASTSTUB_DEFINE_H