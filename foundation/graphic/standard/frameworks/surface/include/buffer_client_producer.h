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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_CLIENT_PRODUCER_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_CLIENT_PRODUCER_H

#include <map>
#include <vector>

#include <iremote_proxy.h>
#include <iremote_object.h>

#include <ibuffer_producer.h>

#include "surface_buffer_impl.h"

namespace OHOS {
class BufferClientProducer : public IRemoteProxy<IBufferProducer> {
public:
    BufferClientProducer(const sptr<IRemoteObject>& impl);
    virtual ~BufferClientProducer();

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
    static inline BrokerDelegator<BufferClientProducer> delegator_;
    std::string name_ = "not init";
};
}; // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_CLIENT_PRODUCER_H
