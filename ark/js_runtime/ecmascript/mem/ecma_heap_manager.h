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

#ifndef ECMASCRIPT_MEM_ECMA_HEAP_MANAGER_H
#define ECMASCRIPT_MEM_ECMA_HEAP_MANAGER_H

#include "ecmascript/mem/allocator-inl.h"
#include "ecmascript/js_hclass.h"

namespace panda::ecmascript {
class Heap;

class EcmaHeapManager {
public:
    explicit EcmaHeapManager(Heap *heap);
    ~EcmaHeapManager() = default;

    NO_COPY_SEMANTIC(EcmaHeapManager);
    NO_MOVE_SEMANTIC(EcmaHeapManager);

    inline TaggedObject *AllocateYoungGenerationOrHugeObject(JSHClass *hclass);
    inline TaggedObject *TryAllocateYoungGeneration(size_t size);
    inline TaggedObject *AllocateYoungGenerationOrHugeObject(JSHClass *hclass, size_t size);

    inline TaggedObject *AllocateNonMovableOrHugeObject(JSHClass *hclass, size_t size);
    inline TaggedObject *AllocateNonMovableOrHugeObject(JSHClass *hclass);
    inline TaggedObject *AllocateHugeObject(JSHClass *hclass, size_t size);
    inline TaggedObject *AllocateOldGenerationOrHugeObject(JSHClass *hclass, size_t size);
    inline TaggedObject *AllocateMachineCodeSpaceObject(JSHClass *hclass, size_t size);
    inline uintptr_t AllocateSnapShotSpace(size_t size);

    inline void SetClass(TaggedObject *header, JSHClass *hclass);

    const Heap *GetHeap() const
    {
        return heap_;
    }

    FreeListAllocator &GetOldSpaceAllocator()
    {
        return oldSpaceAllocator_;
    }

    BumpPointerAllocator &GetNewSpaceAllocator()
    {
        return newSpaceAllocator_;
    }

    FreeListAllocator &GetNonMovableSpaceAllocator()
    {
        return nonMovableAllocator_;
    }

    const BumpPointerAllocator &GetSnapShotSpaceAllocator() const
    {
        return snapshotSpaceAllocator_;
    }

    FreeListAllocator &GetMachineCodeSpaceAllocator()
    {
        return machineCodeSpaceAllocator_;
    }

private:
    Heap *heap_{nullptr};
    BumpPointerAllocator newSpaceAllocator_;
    FreeListAllocator nonMovableAllocator_;
    FreeListAllocator oldSpaceAllocator_;
    BumpPointerAllocator snapshotSpaceAllocator_;
    FreeListAllocator machineCodeSpaceAllocator_;
};
}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_MEM_ECMA_HEAP_MANAGER_H
