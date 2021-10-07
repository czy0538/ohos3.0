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

#include "verification.h"

#include "ecmascript/js_tagged_value-inl.h"
#include "heap_roots-inl.h"
#include "slots.h"

namespace panda::ecmascript {
// Verify the object body
void VerifyObjectVisitor::VisitAllObjects(TaggedObject *obj)
{
    auto jsHclass = obj->GetClass();
    rootManager_.MarkObjectBody<GCType::OLD_GC>(
        obj, jsHclass, [this]([[maybe_unused]] TaggedObject *root, ObjectSlot start, ObjectSlot end) {
            for (ObjectSlot slot = start; slot < end; slot++) {
                JSTaggedValue value(slot.GetTaggedType());
                if (value.IsWeak()) {
                    if (!HEAP->IsLive(value.GetTaggedWeakRef())) {
                        LOG(ERROR, RUNTIME) << "Heap verify detected a dead object at " << value.GetTaggedObject();
                        ++(*FAIL_COUNT);
                    }
                } else if (value.IsHeapObject()) {
                    if (!HEAP->IsLive(value.GetTaggedObject())) {
                        LOG(ERROR, RUNTIME) << "Heap verify detected a dead object at " << value.GetTaggedObject();
                        ++(*FAIL_COUNT);
                    }
                }
            }
        });
}

size_t Verification::VerifyRoot() const
{
    size_t failCount = 0;
    RootVisitor visit1 = [this, &failCount]([[maybe_unused]] Root type, ObjectSlot slot) {
        JSTaggedValue value(slot.GetTaggedType());
        if (value.IsWeak()) {
            VerifyObjectVisitor(heap_, &failCount)(value.GetTaggedWeakRef());
        } else if (value.IsHeapObject()) {
            VerifyObjectVisitor(heap_, &failCount)(value.GetTaggedObject());
        }
    };
    RootRangeVisitor visit2 = [this, &failCount]([[maybe_unused]] Root type, ObjectSlot start, ObjectSlot end) {
        for (ObjectSlot slot = start; slot < end; slot++) {
            JSTaggedValue value(slot.GetTaggedType());
            if (value.IsWeak()) {
                VerifyObjectVisitor(heap_, &failCount)(value.GetTaggedWeakRef());
            } else if (value.IsHeapObject()) {
                VerifyObjectVisitor(heap_, &failCount)(value.GetTaggedObject());
            }
        }
    };
    rootManager_.VisitVMRoots(visit1, visit2);
    if (failCount > 0) {
        LOG(ERROR, RUNTIME) << "VerifyRoot detects deadObject count is " << failCount;
    }

    return failCount;
}

size_t Verification::VerifyHeap() const
{
    size_t failCount = heap_->VerifyHeapObjects();
    if (failCount > 0) {
        LOG(ERROR, RUNTIME) << "VerifyHeap detects deadObject count is " << failCount;
    }
    return failCount;
}

bool Verification::IsHeapAddress(void *addr) const
{
    return heap_->ContainObject(reinterpret_cast<TaggedObject *>(addr));
}
}  // namespace panda::ecmascript
