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

#ifndef ECMASCRIPT_WRAPPER_OBJECT_H
#define ECMASCRIPT_WRAPPER_OBJECT_H

#include "ecmascript/ecma_macros.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/js_hclass.h"

namespace panda::ecmascript {
class ObjectWrapper : public TaggedObject {
public:
    static ObjectWrapper *Cast(ObjectHeader *object)
    {
        return static_cast<ObjectWrapper *>(object);
    }

    static constexpr size_t VALUE_OFFSET = TaggedObjectSize();
    ACCESSORS(Value, VALUE_OFFSET, SIZE)

    DECL_VISIT_OBJECT(VALUE_OFFSET, SIZE)
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_WRAPPER_OBJECT_H