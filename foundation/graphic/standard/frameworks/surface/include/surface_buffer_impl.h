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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
#define FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H

#include <any>
#include <map>

#include <buffer_extra_data.h>
#include <buffer_handle_parcel.h>
#include <buffer_handle_utils.h>
#include <surface_buffer.h>

#include "buffer_extra_data_impl.h"
#ifdef ACE_ENABLE_GL
#include "egl_data.h"
#endif

namespace OHOS {
enum ExtraDataType {
    EXTRA_DATA_TYPE_MIN,
    EXTRA_DATA_TYPE_INT32,
    EXTRA_DATA_TYPE_INT64,
    EXTRA_DATA_TYPE_MAX,
};

typedef struct {
    std::any value;
    ExtraDataType type;
} ExtraData;

class MessageParcel;
class SurfaceBufferImpl : public SurfaceBuffer {
public:
    SurfaceBufferImpl();
    SurfaceBufferImpl(int seqNum);
    virtual ~SurfaceBufferImpl();

    static SurfaceBufferImpl *FromBase(const sptr<SurfaceBuffer>& buffer);

    BufferHandle *GetBufferHandle() const override;
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    int32_t GetFormat() const override;
    int64_t GetUsage() const override;
    uint64_t GetPhyAddr() const override;
    int32_t GetKey() const override;
    void *GetVirAddr() const override;
    int32_t GetFileDescriptor() const override;
    uint32_t GetSize() const override;
#ifdef ACE_ENABLE_GL
    sptr<EglData> GetEglData() const;
    void SetEglData(const sptr<EglData>& data);
#endif

    int32_t GetSeqNum();
    SurfaceError SetInt32(uint32_t key, int32_t val) override;
    SurfaceError GetInt32(uint32_t key, int32_t &val) override;
    SurfaceError SetInt64(uint32_t key, int64_t val) override;
    SurfaceError GetInt64(uint32_t key, int64_t &val) override;

    void SetExtraData(const BufferExtraData &bedata);
    void GetExtraData(BufferExtraData &bedata) const;
    virtual SurfaceError ExtraGet(std::string key, int32_t &value) const override;
    virtual SurfaceError ExtraGet(std::string key, int64_t &value) const override;
    virtual SurfaceError ExtraGet(std::string key, double &value) const override;
    virtual SurfaceError ExtraGet(std::string key, std::string &value) const override;
    virtual SurfaceError ExtraSet(std::string key, int32_t value) override;
    virtual SurfaceError ExtraSet(std::string key, int64_t value) override;
    virtual SurfaceError ExtraSet(std::string key, double value) override;
    virtual SurfaceError ExtraSet(std::string key, std::string value) override;

    void SetBufferHandle(BufferHandle *handle);

    void WriteToMessageParcel(MessageParcel &parcel);

private:
    SurfaceError SetData(uint32_t key, ExtraData data);
    SurfaceError GetData(uint32_t key, ExtraData &data);
    std::map<uint32_t, ExtraData> extraDatas_;

    BufferHandle *handle_ = nullptr;
    int32_t sequenceNumber = -1;
    BufferExtraDataImpl bedataimpl;
#ifdef ACE_ENABLE_GL
    sptr<EglData> eglData_ = nullptr;
#endif
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
