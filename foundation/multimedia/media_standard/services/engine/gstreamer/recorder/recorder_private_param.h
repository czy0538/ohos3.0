/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef RECORDER_PRIVATE_PARAM_H
#define RECORDER_PRIVATE_PARAM_H

#include <memory>
#include <refbase.h>
#include "recorder_param.h"
#include "surface.h"

namespace OHOS {
namespace Media {
enum RecorderPrivateParamType : uint32_t {
    PRIVATE_PARAM_TYPE_BEGIN = PRIVATE_PARAM_SECTION_START,
    SURFACE,
    OUTPUT_FORMAT,
};

struct SurfaceParam : public RecorderParam {
    SurfaceParam() : RecorderParam(RecorderPrivateParamType::SURFACE), surface_(nullptr) {}
    ~SurfaceParam() = default;
    sptr<Surface> surface_;
};

struct OutputFormat : public RecorderParam {
    explicit OutputFormat(int32_t format) : RecorderParam(RecorderPrivateParamType::OUTPUT_FORMAT), format_(format) {}
    ~OutputFormat() = default;
    int32_t format_;
};
}
}
#endif
