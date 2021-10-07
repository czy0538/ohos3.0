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

#ifndef PANDA_RUNTIME_TOOLING_PT_METHOD_PRIVATE_H_
#define PANDA_RUNTIME_TOOLING_PT_METHOD_PRIVATE_H_

#include "runtime/include/tooling/pt_method.h"

namespace panda {
class Method;
}  // namespace panda

namespace panda::tooling {
inline Method *PtMethodToMethod(PtMethod method)
{
    return reinterpret_cast<Method *>(method.GetData());
}

inline PtMethod MethodToPtMethod(Method *method)
{
    return PtMethod(reinterpret_cast<void *>(method));
}

}  // namespace panda::tooling

#endif  // PANDA_RUNTIME_TOOLING_PT_METHOD_PRIVATE_H_
