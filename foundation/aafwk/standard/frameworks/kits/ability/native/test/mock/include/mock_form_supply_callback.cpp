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

#include "app_log_wrapper.h"
#include "mock_form_supply_callback.h"

namespace OHOS {
namespace AppExecFwk {
sptr<MockFormSupplyCallback> MockFormSupplyCallback::instance = nullptr;
std::mutex MockFormSupplyCallback::mutex;

sptr<MockFormSupplyCallback> MockFormSupplyCallback::GetInstance()
{
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lockl(mutex);
        if (instance == nullptr) {
            instance = new MockFormSupplyCallback();
        }
    }
    return instance;
}

/**
 * onAcquire
 *
 * @param formInfo Form Provider Info.
 * @param want data
 */
int MockFormSupplyCallback::OnAcquire(const FormProviderInfo &formProviderInfo, const Want& want)
{
    APP_LOGI("MockFormSupplyCallback::OnAcquire called.");
    return 1;
}


/**
 * onEventHandle
 *
 * @param want data
 */
int MockFormSupplyCallback::OnEventHandle(const Want& want)
{
    APP_LOGI("MockFormSupplyCallback::OnEventHandle called.");
    return 1;
}
}  // namespace AppExecFwk
}  // namespace OHOS
