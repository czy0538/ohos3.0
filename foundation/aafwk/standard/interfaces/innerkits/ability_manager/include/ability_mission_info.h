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

#ifndef OHOS_AAFWK_INTERFACES_INNERKITS_ABILITY_MISSION_INFO_H
#define OHOS_AAFWK_INTERFACES_INNERKITS_ABILITY_MISSION_INFO_H

#include <string>

#include "ability_record_info.h"
#include "mission_description_info.h"
#include "parcel.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
/**
 * @struct AbilityMissionInfo
 * AbilityMissionInfo is used to save informations about ability mission.
 */
struct AbilityMissionInfo : public Parcelable {
    int32_t id = -1;
    int32_t runingState = -1;
    int32_t missionStackId = -1;
    Want baseWant;
    AppExecFwk::ElementName baseAbility;
    AppExecFwk::ElementName topAbility;
    int32_t size;
    MissionDescriptionInfo missionDescription;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static AbilityMissionInfo *Unmarshalling(Parcel &parcel);
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_INTERFACES_INNERKITS_ABILITY_MISSION_INFO_H