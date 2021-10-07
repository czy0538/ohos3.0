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

#include "player_listener_stub.h"
#include "media_log.h"
#include "media_errors.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "PlayerListenerStub"};
}

namespace OHOS {
namespace Media {
PlayerListenerStub::PlayerListenerStub()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

PlayerListenerStub::~PlayerListenerStub()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

int PlayerListenerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    switch (code) {
        case PlayerListenerMsg::ON_ERROR: {
            int32_t errorType = data.ReadInt32();
            int32_t errorCode = data.ReadInt32();
            OnError(static_cast<PlayerErrorType>(errorType), errorCode);
            return MSERR_OK;
        }
        case PlayerListenerMsg::ON_INFO: {
            int32_t type = data.ReadInt32();
            int32_t extra = data.ReadInt32();
            Format format;
            MEDIA_LOGD("0x%{public}06" PRIXPTR " listen stub on info type: %{public}d extra %{public}d",
                       FAKE_POINTER(this), type, extra);
            OnInfo(static_cast<PlayerOnInfoType>(type), extra, format);
            return MSERR_OK;
        }
        default: {
            MEDIA_LOGE("default case, need check PlayerListenerStub");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

void PlayerListenerStub::OnError(PlayerErrorType errorType, int32_t errorCode)
{
    std::shared_ptr<PlayerCallback> cb = callback_.lock();
    if (cb != nullptr) {
        cb->OnError(errorType, errorCode);
    }
}

void PlayerListenerStub::OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody)
{
    std::shared_ptr<PlayerCallback> cb = callback_.lock();
    CHECK_AND_RETURN(cb != nullptr);
    switch (type) {
        case INFO_TYPE_SEEKDONE:
            cb->OnInfo(INFO_TYPE_SEEKDONE, extra, infoBody);
            break;
        case INFO_TYPE_EOS:
            cb->OnInfo(INFO_TYPE_EOS, extra, infoBody);
            break;
        case INFO_TYPE_STATE_CHANGE:
            cb->OnInfo(INFO_TYPE_STATE_CHANGE, extra, infoBody);
            break;
        case INFO_TYPE_POSITION_UPDATE:
            cb->OnInfo(INFO_TYPE_POSITION_UPDATE, extra, infoBody);
            break;
        case INFO_TYPE_MESSAGE:
            cb->OnInfo(INFO_TYPE_MESSAGE, extra, infoBody);
            break;
        case INFO_TYPE_VOLUME_CHANGE:
            cb->OnInfo(INFO_TYPE_VOLUME_CHANGE, extra, infoBody);
            break;
        case INFO_TYPE_EXTRA_FORMAT:
            cb->OnInfo(INFO_TYPE_EXTRA_FORMAT, extra, infoBody);
            break;
        default:
            MEDIA_LOGE("default case, need check PlayerListenerStub");
            break;
    }
}

void PlayerListenerStub::SetPlayerCallback(const std::weak_ptr<PlayerCallback> &callback)
{
    callback_ = callback;
}
} // namespace Media
} // namespace OHOS
