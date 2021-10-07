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

#ifndef OHOS_AAFWK_MISSION_RECORD_H
#define OHOS_AAFWK_MISSION_RECORD_H

#include <list>

#include "ability_record.h"
#include "mission_stack.h"
#include "mission_description_info.h"
#include "mission_option.h"

namespace OHOS {
namespace AAFwk {
class MissionStack;
/**
 * @class MissionRecord
 * MissionRecord records mission info and ability records.
 */
class MissionRecord : public ConfigurationHolder {
public:
    MissionRecord(const std::string &bundleName = "");
    MissionRecord(const std::shared_ptr<MissionRecord> &mission);
    virtual ~MissionRecord();

    /**
     * calculate next mission id
     *
     * @return mission id.
     */
    static int GetNextMissionId();

    /**
     * get this mission record id
     *
     * @return mission record id.
     */
    int GetMissionRecordId() const;

    /**
     * get the number of abilities owned by this mission
     *
     * @return count.
     */
    int GetAbilityRecordCount() const;

    /**
     * get the bottom ability of this mission
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetBottomAbilityRecord() const;

    /**
     * get the top ability of this mission
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetTopAbilityRecord() const;

    /**
     * get the last top ability of this mission
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetLastTopAbility() const;

    /**
     * get the ability by token
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByToken(const sptr<IRemoteObject> &token) const;

    /**
     * get the ability by id
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordById(const int64_t recordId) const;

    /**
     * get the ability record by caller token and request code
     *
     * @return AbilityRecord.
     */
    std::shared_ptr<AbilityRecord> GetAbilityRecordByCaller(
        const std::shared_ptr<AbilityRecord> &caller, int requestCode);

    /**
     * put the ability at the top of the stack
     *
     * @param AbilityRecord.
     */
    void AddAbilityRecordToTop(std::shared_ptr<AbilityRecord> ability);

    /**
     * remove the ability from the stack
     *
     * @param AbilityRecord.
     */
    bool RemoveAbilityRecord(std::shared_ptr<AbilityRecord> ability);

    /**
     * remove the ability at the top of the stack
     *
     * @param AbilityRecord.
     */
    bool RemoveTopAbilityRecord();

    /**
     * remove all ability record from stack
     *
     * @param AbilityRecord.
     */
    void RemoveAll();

    /**
     * dump the mission and ability stack information
     *
     * @param info
     */
    void Dump(std::vector<std::string> &info);

    /**
     * check whether it is the same mission by bundleName
     *
     * @param bundleName
     */
    bool IsSameMissionRecord(const std::string &bundleName) const;

    /**
     * Get all the ability information in this mission
     *
     * @param abilityInfos
     */
    void GetAllAbilityInfo(std::vector<AbilityRecordInfo> &abilityInfos);

    /**
     * check whether it is the top ability by abilityName
     *
     * @param abilityName
     */
    bool IsTopAbilityRecordByName(const std::string &abilityName);

    /**
     * set whether it is created by luncher
     *
     * @param isCreate
     */
    void SetIsLauncherCreate();

    /**
     * is luncher create
     *
     * @return is create
     */
    bool IsLauncherCreate() const;

    /**
     * set mission's previous mission record.
     *
     * @param record , previous mission record
     */
    void SetPreMissionRecord(const std::shared_ptr<MissionRecord> &record);

    /**
     * get mission's previous mission record.
     *
     * @return previous mission record
     */
    std::shared_ptr<MissionRecord> GetPreMissionRecord() const;

    /**
     * Check whether it exists through record ID
     *
     * @param id: ability record id
     */
    bool IsExistAbilityRecord(int32_t id);

    /**
     * set parent mission stack.
     *
     * @param missionStack: the parent mission stack
     */
    void SetMissionStack(const std::shared_ptr<MissionStack> &missionStack, int stackId);
    std::shared_ptr<MissionStack> GetMissionStack() const;

    std::string GetName() const
    {
        return bundleName_;
    };

    void SetMissionDescriptionInfo(std::shared_ptr<MissionDescriptionInfo> &description)
    {
        missionDescriptionInfo_ = description;
    };

    std::shared_ptr<MissionDescriptionInfo> GetMissionDescriptionInfo() const
    {
        return missionDescriptionInfo_;
    };

    bool SupportMultWindow() const;

    void SetMissionOption(const MissionOption &option);
    const MissionOption &GetMissionOption() const;

    bool IsEmpty();
    void Resume(const std::shared_ptr<MissionRecord> &backup);

protected:
    virtual std::shared_ptr<ConfigurationHolder> GetParent() override;
    virtual unsigned int GetChildSize() override;
    virtual std::shared_ptr<ConfigurationHolder> FindChild(unsigned int index) override;

private:
    static int nextMissionId_;
    int missionId_;
    std::string bundleName_;
    std::list<std::shared_ptr<AbilityRecord>> abilities_;
    bool isLauncherCreate_ = false;
    std::weak_ptr<MissionRecord> preMissionRecord_;
    std::weak_ptr<MissionStack> parentMissionStack_;

    std::shared_ptr<MissionDescriptionInfo> missionDescriptionInfo_ = nullptr;
    MissionOption option_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_MISSION_RECORD_H