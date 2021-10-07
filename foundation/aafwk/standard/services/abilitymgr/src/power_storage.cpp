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

#include "power_storage.h"
#include "ability_util.h"
#include "mission_record.h"

namespace OHOS {
namespace AAFwk {
void PowerStorage::SetPowerOffInActiveRecord(const std::shared_ptr<AbilityRecord> &ability)
{
    CHECK_POINTER(ability);
    PowerOffRecord record;
    record.ability = ability;
    record.StackId = ability->GetMissionRecord()->GetMissionStack()->GetMissionStackId();
    record.missionId = ability->GetMissionRecord()->GetMissionRecordId();
    record.state = ability->GetAbilityState();
    inActiveRecord_.emplace_back(record);
}

std::vector<PowerOffRecord> PowerStorage::GetPowerOffInActiveRecord() const
{
    return inActiveRecord_;
}

void PowerStorage::SetPowerOffActiveRecord(const std::shared_ptr<AbilityRecord> &ability)
{
    CHECK_POINTER(ability);
    PowerOffRecord record;
    record.ability = ability;
    record.StackId = ability->GetMissionRecord()->GetMissionStack()->GetMissionStackId();
    record.missionId = ability->GetMissionRecord()->GetMissionRecordId();
    record.state = ability->GetAbilityState();
    activeRecord_.emplace_back(record);
}

std::vector<PowerOffRecord> PowerStorage::GetPowerOffActiveRecord() const
{
    return activeRecord_;
}
}  // namespace AAFwk
}  // namespace OHOS
