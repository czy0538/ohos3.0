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

#include "frameworks/bridge/pa_backend/engine/quickjs/qjs_pa_engine_loader.h"

#include "base/utils/macros.h"
#include "frameworks/bridge/pa_backend/engine/quickjs/qjs_pa_engine.h"

namespace OHOS::Ace::Framework {
QjsPaEngineLoader::QjsPaEngineLoader() = default;
QjsPaEngineLoader::~QjsPaEngineLoader() = default;

RefPtr<JsBackendEngine> QjsPaEngineLoader::CreateJsBackendEngine(int32_t instanceId) const
{
    return AceType::MakeRefPtr<QjsPaEngine>(instanceId);
}

#if defined(BUILT_IN_JS_ENGINE)
JsBackendEngineLoader& JsBackendEngineLoader::Get()
{
    return QjsPaEngineLoader::GetInstance();
}
#else
extern "C" ACE_EXPORT void* OHOS_ACE_GetJsBackendEngineLoader()
{
    return &QjsPaEngineLoader::GetInstance();
}
#endif
} // namespace OHOS::Ace::Framework
