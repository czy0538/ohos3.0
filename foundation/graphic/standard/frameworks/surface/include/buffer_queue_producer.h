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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_PRODUCER_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_PRODUCER_H

#include <vector>
#include <mutex>
#include <refbase.h>
#include <iremote_stub.h>
#include <message_parcel.h>
#include <message_option.h>

#include <surface_type.h>
#include <ibuffer_producer.h>

#include "buffer_queue.h"

namespace OHOS {
class BufferQueueProducer : public IRemoteStub<IBufferProducer> {
public:
    BufferQueueProducer(sptr<BufferQueue>& bufferQueue);
    virtual ~BufferQueueProducer();

    virtual int OnRemoteRequest(uint32_t code, MessageParcel &arguments,
                                MessageParcel &reply, MessageOption &option) override;

    virtual SurfaceError RequestBuffer(const BufferRequestConfig &config, BufferExtraData &bedata,
                                       RequestBufferReturnValue &retval) override;

    SurfaceError CancelBuffer(int32_t sequence, BufferExtraData &bedata) override;

    SurfaceError FlushBuffer(int32_t sequence, BufferExtraData &bedata,
                             int32_t fence, BufferFlushConfig &config) override;

    uint32_t     GetQueueSize() override;
    SurfaceError SetQueueSize(uint32_t queueSize) override;

    SurfaceError GetName(std::string &name) override;

    int32_t      GetDefaultWidth() override;
    int32_t      GetDefaultHeight() override;
    uint32_t     GetDefaultUsage() override;

    SurfaceError CleanCache() override;

private:
    int32_t RequestBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t CancelBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t FlushBufferRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t SetQueueSizeRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetNameRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetDefaultWidthRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetDefaultHeightRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t GetDefaultUsageRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);
    int32_t CleanCacheRemote(MessageParcel &arguments, MessageParcel &reply, MessageOption &option);

    using BufferQueueProducerFunc = int32_t (BufferQueueProducer::*)(MessageParcel &arguments,
        MessageParcel &reply, MessageOption &option);
    std::map<uint32_t, BufferQueueProducerFunc> memberFuncMap_;

    sptr<BufferQueue> bufferQueue_ = nullptr;
    std::string name_ = "not init";
};
}; // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_QUEUE_PRODUCER_H
