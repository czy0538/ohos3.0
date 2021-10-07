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

#ifndef I_MEDIA_SERVICE_H
#define I_MEDIA_SERVICE_H

#include <memory>
#include "i_recorder_service.h"
#include "i_player_service.h"
#include "i_avmetadatahelper_service.h"

namespace OHOS {
namespace Media {
class IMediaService {
public:
    virtual ~IMediaService() = default;

    /**
     * @brief Create a recorder service.
     *
     * All recorder functions must be created and obtained first.
     *
     * @return Returns a valid pointer if the setting is successful;
     * @since 1.0
     * @version 1.0
     */
    virtual std::shared_ptr<IRecorderService> CreateRecorderService() = 0;

    /**
     * @brief Create a player service.
     *
     * All player functions must be created and obtained first.
     *
     * @return Returns a valid pointer if the setting is successful;
     * @since 1.0
     * @version 1.0
     */
    virtual std::shared_ptr<IPlayerService> CreatePlayerService() = 0;

    /**
     * @brief Create an avmetadatahelper service.
     *
     * All player functions must be created and obtained first.
     *
     * @return Returns a valid pointer if the setting is successful;
     * @since 1.0
     * @version 1.0
     */
    virtual std::shared_ptr<IAVMetadataHelperService> CreateAVMetadataHelperService() = 0;

    /**
     * @brief Destroy a recorder service.
     *
     * call the API to destroy the recorder service.
     *
     * @param pointer to the recorder service.
     * @return Returns a valid pointer if the setting is successful;
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t DestroyRecorderService(std::shared_ptr<IRecorderService> recorder) = 0;

    /**
     * @brief Destroy a player service.
     *
     * call the API to destroy the player service.
     *
     * @param pointer to the player service.
     * @return Returns a valid pointer if the setting is successful;
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t DestroyPlayerService(std::shared_ptr<IPlayerService> player) = 0;

    /**
     * @brief Destroy a avmetadatahelper service.
     *
     * call the API to destroy the avmetadatahelper service.
     *
     * @param pointer to the avmetadatahelper service.
     * @return Returns a valid pointer if the setting is successful;
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t DestroyAVMetadataHelperService(std::shared_ptr<IAVMetadataHelperService> avMetadataHelper) = 0;
};

class __attribute__((visibility("default"))) MeidaServiceFactory {
public:
    /**
     * @brief IMediaService singleton
     *
     * Create Recorder Service and Player Service Through the Media Service.
     *
     * @return Returns IMediaService singleton;
     * @since 1.0
     * @version 1.0
     */
    static IMediaService &GetInstance();
private:
    MeidaServiceFactory() = delete;
    ~MeidaServiceFactory() = delete;
};
} // namespace Media
} // namespace OHOS
#endif // I_MEDIA_SERVICE_H
