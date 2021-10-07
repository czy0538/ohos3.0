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

#include "avmetadatahelper_server.h"
#include "media_log.h"
#include "media_errors.h"
#include "engine_factory_repo.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AVMetadataHelperServer"};
}

namespace OHOS {
namespace Media {
std::shared_ptr<IAVMetadataHelperService> AVMetadataHelperServer::Create()
{
    std::shared_ptr<AVMetadataHelperServer> server = std::make_shared<AVMetadataHelperServer>();
    CHECK_AND_RETURN_RET_LOG(server != nullptr, nullptr, "Failed to new AVMetadataHelperServer");
    return server;
}

AVMetadataHelperServer::AVMetadataHelperServer()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

AVMetadataHelperServer::~AVMetadataHelperServer()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
    std::lock_guard<std::mutex> lock(mutex_);
    avMetadataHelperEngine_ = nullptr;
}

int32_t AVMetadataHelperServer::SetSource(const std::string &uri, int32_t usage)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MEDIA_LOGD("Current uri is : %{public}s %{public}u", uri.c_str(), usage);
    auto engineFactory = EngineFactoryRepo::Instance().GetEngineFactory(IEngineFactory::Scene::SCENE_AVMETADATA, uri);
    CHECK_AND_RETURN_RET_LOG(engineFactory != nullptr, MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED,
        "Failed to get engine factory");
    avMetadataHelperEngine_ = engineFactory->CreateAVMetadataHelperEngine();
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED,
        "Failed to create avmetadatahelper engine");

    int32_t ret = avMetadataHelperEngine_->SetSource(uri, usage);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "SetSource failed!");

    return MSERR_OK;
}

std::string AVMetadataHelperServer::ResolveMetadata(int32_t key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MEDIA_LOGD("Key is %{public}d", key);
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, "",
        "avMetadataHelperEngine_ is nullptr");
    return avMetadataHelperEngine_->ResolveMetadata(key);
}

std::unordered_map<int32_t, std::string> AVMetadataHelperServer::ResolveMetadata()
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, {},
        "avMetadataHelperEngine_ is nullptr");
    return avMetadataHelperEngine_->ResolveMetadata();
}

std::shared_ptr<AVSharedMemory> AVMetadataHelperServer::FetchFrameAtTime(int64_t timeUs, int32_t option,
    OutputConfiguration param)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_AND_RETURN_RET_LOG(avMetadataHelperEngine_ != nullptr, nullptr,
        "avMetadataHelperEngine_ is nullptr");
    return avMetadataHelperEngine_->FetchFrameAtTime(timeUs, option, param);
}

void AVMetadataHelperServer::Release()
{
    std::lock_guard<std::mutex> lock(mutex_);
    avMetadataHelperEngine_ = nullptr;
}
}
}