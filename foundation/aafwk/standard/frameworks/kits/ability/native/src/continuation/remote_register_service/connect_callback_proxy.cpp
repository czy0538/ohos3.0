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
#include "connect_callback_proxy.h"
#include "extra_params.h"
#include "string_ex.h"
#include "app_log_wrapper.h"
namespace OHOS {
namespace AppExecFwk {

/**
 * @brief Remote device sends connection request.
 * @param deviceId indicators id of connection device.
 * @param deviceType indicators type of connection device.
 * @return none
 */
void ConnectCallbackProxy::Connect(const string &deviceId, const string &deviceType)
{
    APP_LOGI("%{public}s called begin", __func__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(IConnectCallback::GetDescriptor()) || !data.WriteString(deviceId) ||
        data.WriteString(deviceType)) {
        APP_LOGE("%{public}s params is wrong", __func__);
        return;
    }
    RemoteRequest(data, COMMAND_CONNECT);
    APP_LOGI("%{public}s called end", __func__);
}
/**
 * @brief Remote device sends disconnection request.
 * @param deviceId indicators id of disconnection device.
 * @return none
 */
void ConnectCallbackProxy::Disconnect(const string &deviceId)
{
    APP_LOGI("%{public}s called begin", __func__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(IConnectCallback::GetDescriptor()) || !data.WriteString(deviceId)) {
        APP_LOGE("%{public}s params is wrong", __func__);
        return;
    }
    RemoteRequest(data, COMMAND_DISCONNECT);
    APP_LOGI("%{public}s called end", __func__);
}
void ConnectCallbackProxy::RemoteRequest(MessageParcel &data, int commandDisconnect)
{
    APP_LOGI("%{public}s called begin", __func__);
    MessageParcel reply;
    MessageOption option;
    if (Remote() == nullptr) {
        return;
    }
    Remote()->SendRequest(commandDisconnect, data, reply, option);
    APP_LOGI("%{public}s called begin", __func__);
}
}  // namespace AppExecFwk
}  // namespace OHOS