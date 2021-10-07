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

#ifndef PANDA_RUNTIME_TOOLING_PT_REFERENCE_PRIVATE_H_
#define PANDA_RUNTIME_TOOLING_PT_REFERENCE_PRIVATE_H_

#include "runtime/include/tooling/pt_reference.h"

namespace panda {
class ObjectHeader;
}  // namespace panda

namespace panda::tooling {
void PtPushLocalFrameFromNative();
void PtPopLocalFrameFromNative();

PtLocalReference *PtCreateLocalReference(ObjectHeader *objectHeader);
void PtDestroyLocalReference(const PtLocalReference *localRef);

ObjectHeader *PtGetObjectHeaderByReference(const PtReference *ref);

PtGlobalReference *PtCreateGlobalReference(const ObjectHeader *objectHeader);
PtGlobalReference *PtCreateGlobalReference(const PtReference *ref);
void PtDestroyGlobalReference(const PtGlobalReference *ref);
}  // namespace panda::tooling

#endif  // PANDA_RUNTIME_TOOLING_PT_REFERENCE_PRIVATE_H_
