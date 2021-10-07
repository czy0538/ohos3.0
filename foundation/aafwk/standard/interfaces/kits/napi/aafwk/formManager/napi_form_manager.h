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

#ifndef FORM_ABILITY_H_
#define FORM_ABILITY_H_

#include "ability.h"
#include "form_info.h"
#include "form_js_info.h"
#include "form_provider_info.h"
#include "hilog_wrapper.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "nlohmann/json.hpp"
#include "want.h"

struct AsyncDelFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    int64_t formId;
    int result;
};

struct AsyncReleaseFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    int64_t formId;
    bool isReleaseCache;
    int result;
};

struct AsyncRequestFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    int64_t formId;
    int result;
};

struct AsyncNextRefreshTimeFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    int64_t formId;
    int32_t time;
    int result;
};

struct AsyncUpdateFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    int64_t formId;
    std::shared_ptr<OHOS::AppExecFwk::FormProviderData> formProviderData;
    int result;
};

struct AsyncCastTempFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    int64_t formId;
    int result;
};

struct AsyncNotifyVisibleFormsCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    std::vector<int64_t> formIds;
    int result;
};

struct AsyncNotifyInvisibleFormsCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    std::vector<int64_t> formIds;
    int result;
};

struct AsyncEnableUpdateFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    std::vector<int64_t> formIds;
    int result;
};

struct AsyncDisableUpdateFormCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    std::vector<int64_t> formIds;
    int result;
};

struct AsyncCheckFMSReadyCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    bool isFMSReady;
};

struct AsyncGetAllFormsCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    std::vector<OHOS::AppExecFwk::FormInfo> formInfos; 
    int result;
};

struct AsyncGetFormsInfoByAppCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    std::vector<OHOS::AppExecFwk::FormInfo> formInfos;
    std::string bundleName;
    int result;
};

struct AsyncGetFormsInfoByModuleCallbackInfo {
    napi_env env;
    OHOS::AppExecFwk::Ability *ability;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback;
    std::vector<OHOS::AppExecFwk::FormInfo> formInfos;
    std::string bundleName;
    std::string moduleName;
    int result;
};

napi_value NAPI_DeleteForm(napi_env env, napi_callback_info info);
napi_value NAPI_ReleaseForm(napi_env env, napi_callback_info info);
napi_value NAPI_RequestForm(napi_env env, napi_callback_info info);
napi_value NAPI_SetFormNextRefreshTime(napi_env env, napi_callback_info info);
napi_value NAPI_UpdateForm(napi_env env, napi_callback_info info);
napi_value NAPI_CastTempForm(napi_env env, napi_callback_info info);
napi_value NAPI_NotifyVisibleForms(napi_env env, napi_callback_info info);
napi_value NAPI_NotifyInvisibleForms(napi_env env, napi_callback_info info);
napi_value NAPI_EnableFormsUpdate(napi_env env, napi_callback_info info);
napi_value NAPI_DisableFormsUpdate(napi_env env, napi_callback_info info);
napi_value NAPI_CheckFMSReady(napi_env env, napi_callback_info info);
napi_value NAPI_GetAllFormsInfo(napi_env env, napi_callback_info info);
napi_value NAPI_GetFormsInfoByApp(napi_env env, napi_callback_info info);
napi_value NAPI_GetFormsInfoByModule(napi_env env, napi_callback_info info);

#endif /* FORM_ABILITY_H_ */
