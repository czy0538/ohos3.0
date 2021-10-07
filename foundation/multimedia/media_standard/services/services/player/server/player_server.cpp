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

#include "player_server.h"
#include "media_log.h"
#include "media_errors.h"
#include "engine_factory_repo.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "PlayerServer"};
}

namespace OHOS {
namespace Media {
const std::string START_TAG = "PlayerCreate->Start";
const std::string STOP_TAG = "PlayerStop->Destroy";
std::shared_ptr<IPlayerService> PlayerServer::Create()
{
    std::shared_ptr<PlayerServer> server = std::make_shared<PlayerServer>();
    CHECK_AND_RETURN_RET_LOG(server != nullptr, nullptr, "failed to new PlayerServer");

    (void)server->Init();
    return server;
}

PlayerServer::PlayerServer()
    : startTimeMonitor_(START_TAG),
      stopTimeMonitor_(STOP_TAG)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

PlayerServer::~PlayerServer()
{
    (void)Release();
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

int32_t PlayerServer::Init()
{
    return MSERR_OK;
}

int32_t PlayerServer::SetSource(const std::string &uri)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = InitPlayEngine(uri);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetSource Failed!");
    return ret;
}

int32_t PlayerServer::SetSource(const std::shared_ptr<IMediaDataSource> &dataSrc)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(dataSrc != nullptr, MSERR_INVALID_VAL, "data source is nullptr");
    dataSrc_ = dataSrc;
    std::string uri = "MediaDataSource";
    int32_t ret = InitPlayEngine(uri);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetObs Failed!");

    return ret;
}

int32_t PlayerServer::InitPlayEngine(const std::string &uri)
{
    if (status_ != PLAYER_IDLE) {
        MEDIA_LOGE("current state is: %{public}d, not support SetSource", status_);
        return MSERR_INVALID_OPERATION;
    }
    startTimeMonitor_.StartTime();
    MEDIA_LOGD("current uri is : %{public}s", uri.c_str());
    auto engineFactory = EngineFactoryRepo::Instance().GetEngineFactory(IEngineFactory::Scene::SCENE_PLAYBACK, uri);
    CHECK_AND_RETURN_RET_LOG(engineFactory != nullptr, MSERR_CREATE_PLAYER_ENGINE_FAILED,
        "failed to get engine factory");
    playerEngine_ = engineFactory->CreatePlayerEngine();
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_CREATE_PLAYER_ENGINE_FAILED,
        "failed to create player engine");
    int32_t ret = MSERR_OK;
    if (dataSrc_ == nullptr) {
        ret = playerEngine_->SetSource(uri);
    } else {
        ret = playerEngine_->SetSource(dataSrc_);
    }
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetSource Failed!");

    std::shared_ptr<IPlayerEngineObs> obs = shared_from_this();
    ret = playerEngine_->SetObs(obs);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "SetObs Failed!");

    status_ = PLAYER_INITIALIZED;
    return MSERR_OK;
}

int32_t PlayerServer::Prepare()
{
    return OnPrepare(false);
}

int32_t PlayerServer::OnPrepare(bool async)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != PLAYER_INITIALIZED && status_ != PLAYER_STOPPED && status_ != PLAYER_PREPARED) {
        MEDIA_LOGE("Can not Prepare, currentState is %{public}d", status_);
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_PREPARED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");
    int32_t ret = MSERR_OK;
    if (surface_ != nullptr) {
        ret = playerEngine_->SetVideoSurface(surface_);
        CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine SetVideoSurface Failed!");
    }

    status_ = PLAYER_PREPARING;
    if (async) {
        ret = playerEngine_->PrepareAsync();
    } else {
        ret = playerEngine_->Prepare();
    }
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Prepare Failed!");
    return MSERR_OK;
}

int32_t PlayerServer::Play()
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PLAYBACK_COMPLETE &&
        status_ != PLAYER_PAUSED && status_ != PLAYER_STARTED) {
        MEDIA_LOGE("Can not Play, currentState is %{public}d", status_);
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_STARTED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    int32_t ret = playerEngine_->Play();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Play Failed!");

    startTimeMonitor_.FinishTime();
    status_ = PLAYER_STARTED;
    return MSERR_OK;
}

int32_t PlayerServer::PrepareAsync()
{
    return OnPrepare(true);
}

int32_t PlayerServer::Pause()
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not Pause, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_PAUSED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    if (status_ != PLAYER_STARTED) {
        MEDIA_LOGE("Can not Pause, status_ is %{public}d", status_);
        return MSERR_INVALID_OPERATION;
    }

    int32_t ret = playerEngine_->Pause();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Pause Failed!");
    status_ = PLAYER_PAUSED;
    return MSERR_OK;
}

int32_t PlayerServer::Stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not Stop, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    if (status_ == PLAYER_STOPPED) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    if ((status_ != PLAYER_PREPARED) && (status_ != PLAYER_STARTED) &&
        (status_ != PLAYER_PLAYBACK_COMPLETE) && (status_ != PLAYER_PAUSED)) {
        MEDIA_LOGE("current state: %{public}d, can not stop", status_);
        return MSERR_INVALID_OPERATION;
    }

    stopTimeMonitor_.StartTime();

    int32_t ret = playerEngine_->Stop();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Stop Failed!");
    status_ = PLAYER_STOPPED;
    return MSERR_OK;
}

int32_t PlayerServer::Reset()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return OnReset();
}

int32_t PlayerServer::OnReset()
{
    if (status_ == PLAYER_IDLE) {
        Format format;
        OnInfo(INFO_TYPE_STATE_CHANGE, status_, format);
        return MSERR_OK;
    }

    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");
    int32_t ret = playerEngine_->Reset();
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Reset Failed!");
    playerEngine_ = nullptr;
    dataSrc_ = nullptr;
    Format format;
    OnInfo(INFO_TYPE_STATE_CHANGE, PLAYER_IDLE, format);
    stopTimeMonitor_.FinishTime();
    return MSERR_OK;
}

int32_t PlayerServer::Release()
{
    std::lock_guard<std::mutex> lock(mutex_);
    {
        std::lock_guard<std::mutex> lockCb(mutexCb_);
        playerCb_ = nullptr;
    }
    (void)OnReset();
    return MSERR_OK;
}

int32_t PlayerServer::SetVolume(float leftVolume, float rightVolume)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not SetVolume, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    const float maxVolume = 1.0f;
    if ((leftVolume < 0) || (leftVolume > maxVolume) || (rightVolume < 0) || (rightVolume > maxVolume)) {
        MEDIA_LOGE("SetVolume failed, the volume should be set to a value ranging from 0 to 5");
        return MSERR_INVALID_OPERATION;
    }

    int32_t ret = playerEngine_->SetVolume(leftVolume, rightVolume);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine SetVolume Failed!");
    return ret;
}

bool PlayerServer::IsValidSeekMode(PlayerSeekMode mode)
{
    switch (mode) {
        case SEEK_PREVIOUS_SYNC:
        case SEEK_NEXT_SYNC:
        case SEEK_CLOSEST_SYNC:
        case SEEK_CLOSEST:
            break;
        default:
            MEDIA_LOGE("Unknown seek mode %{public}d", mode);
            return false;
    }
    return true;
}

int32_t PlayerServer::Seek(int32_t mSeconds, PlayerSeekMode mode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ != PLAYER_PREPARED && status_ != PLAYER_PAUSED &&
        status_ != PLAYER_STARTED && status_ != PLAYER_PLAYBACK_COMPLETE) {
        MEDIA_LOGE("Can not Seek, currentState is %{public}d", status_);
        return MSERR_INVALID_OPERATION;
    }

    if (IsValidSeekMode(mode) != true) {
        MEDIA_LOGE("Seek failed, inValid mode");
        return MSERR_INVALID_VAL;
    }

    mSeconds = std::max(0, mSeconds);
    int32_t ret = playerEngine_->Seek(mSeconds, mode);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine Seek Failed!");
    return ret;
}

int32_t PlayerServer::GetCurrentTime(int32_t &currentTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not GetCurrentTime, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    MEDIA_LOGI("PlayerServer::GetCurrentTime");
    int32_t ret = playerEngine_->GetCurrentTime(currentTime);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine GetCurrentTime Failed!");
    return MSERR_OK;
}

int32_t PlayerServer::GetDuration(int32_t &duration)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_IDLE || status_ == PLAYER_INITIALIZED || status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not GetDuration, currentState is %{public}d", status_);
        return MSERR_INVALID_OPERATION;
    }

    int ret = playerEngine_->GetDuration(duration);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine GetDuration Failed!");
    return MSERR_OK;
}

int32_t PlayerServer::SetPlaybackSpeed(PlaybackRateMode mode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if ((status_ != PLAYER_STARTED) && (status_ != PLAYER_PREPARED) &&
        (status_ != PLAYER_PAUSED) && (status_ != PLAYER_PLAYBACK_COMPLETE)) {
        MEDIA_LOGE("Can not SetPlaybackSpeed, currentState is %{public}d", status_);
        return MSERR_INVALID_OPERATION;
    }

    int ret = playerEngine_->SetPlaybackSpeed(mode);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine SetPlaybackSpeed Failed!");
    return MSERR_OK;
}

int32_t PlayerServer::GetPlaybackSpeed(PlaybackRateMode &mode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not GetDuration, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }

    int ret = playerEngine_->GetPlaybackSpeed(mode);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine GetPlaybackSpeed Failed!");
    return MSERR_OK;
}

int32_t PlayerServer::SetVideoSurface(sptr<Surface> surface)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(surface != nullptr, MSERR_INVALID_VAL, "surface is nullptr");

    if (status_ != PLAYER_INITIALIZED) {
        MEDIA_LOGE("current state: %{public}d, can not SetVideoSurface", status_);
        return MSERR_INVALID_OPERATION;
    }

    surface_ = surface;
    return MSERR_OK;
}

bool PlayerServer::IsPlaying()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not judge IsPlaying, currentState is PLAYER_STATE_ERROR");
        return false;
    }

    return status_ == PLAYER_STARTED;
}

bool PlayerServer::IsLooping()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not judge IsLooping, currentState is PLAYER_STATE_ERROR");
        return false;
    }

    return looping_;
}

int32_t PlayerServer::SetLooping(bool loop)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(playerEngine_ != nullptr, MSERR_NO_MEMORY, "playerEngine_ is nullptr");

    if (status_ == PLAYER_STATE_ERROR) {
        MEDIA_LOGE("Can not SetLooping, currentState is PLAYER_STATE_ERROR");
        return MSERR_INVALID_OPERATION;
    }
    looping_ = loop;

    int32_t ret = playerEngine_->SetLooping(loop);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, MSERR_INVALID_OPERATION, "Engine SetLooping Failed!");
    return ret;
}

int32_t PlayerServer::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(callback != nullptr, MSERR_INVALID_VAL, "callback is nullptr");

    if (status_ != PLAYER_IDLE && status_ != PLAYER_INITIALIZED) {
        MEDIA_LOGE("Can not SetPlayerCallback, currentState is %{public}d", status_);
        return MSERR_INVALID_OPERATION;
    }

    {
        std::lock_guard<std::mutex> lockCb(mutexCb_);
        playerCb_ = callback;
    }
    return MSERR_OK;
}

void PlayerServer::OnError(PlayerErrorType errorType, int32_t errorCode)
{
    std::lock_guard<std::mutex> lockCb(mutexCb_);
    if (playerCb_ != nullptr) {
        playerCb_->OnError(errorType, errorCode);
    }
}

void PlayerServer::OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody)
{
    std::lock_guard<std::mutex> lockCb(mutexCb_);

    if (type == INFO_TYPE_STATE_CHANGE) {
        status_ = static_cast<PlayerStates>(extra);
        MEDIA_LOGI("Callback State change, currentState is %{public}d", status_);
    }

    if (playerCb_ != nullptr) {
        playerCb_->OnInfo(type, extra, infoBody);
    }
}
} // Media
} // OHOS
