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

#ifndef OHOS_AAFWK_ABILITY_MANAGER_CLIENT_H
#define OHOS_AAFWK_ABILITY_MANAGER_CLIENT_H

#include <mutex>

#include "ability_connect_callback_interface.h"
#include "ability_manager_errors.h"
#include "ability_scheduler_interface.h"
#include "ability_manager_interface.h"
#include "want.h"

#include "iremote_object.h"

namespace OHOS {
namespace AAFwk {
/**
 * @class AbilityManagerClient
 * AbilityManagerClient is used to access ability manager services.
 */
class AbilityManagerClient {
public:
    AbilityManagerClient();
    virtual ~AbilityManagerClient();
    static std::shared_ptr<AbilityManagerClient> GetInstance();

    /**
     * AttachAbilityThread, ability call this interface after loaded.
     *
     * @param scheduler,.the interface handler of kit ability.
     * @param token,.ability's token.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AttachAbilityThread(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token);

    /**
     * AbilityTransitionDone, ability call this interface after lift cycle was changed.
     *
     * @param token,.ability's token.
     * @param state,.the state of ability lift cycle.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode AbilityTransitionDone(const sptr<IRemoteObject> &token, int state);

    /**
     * ScheduleConnectAbilityDone, service ability call this interface while session was connected.
     *
     * @param token,.service ability's token.
     * @param remoteObject,.the session proxy of service ability.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode ScheduleConnectAbilityDone(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject);

    /**
     * ScheduleDisconnectAbilityDone, service ability call this interface while session was disconnected.
     *
     * @param token,.service ability's token.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token);

    /**
     * ScheduleCommandAbilityDone, service ability call this interface while session was commanded.
     *
     * @param token,.service ability's token.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token);

    /**
     * AddWindowInfo, add windowToken to AbilityRecord.
     *
     * @param token, the token of the ability.
     * @param windowToken, window id of the ability.
     */
    void AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken);

    /**
     * StartAbility with want, send want to ability manager service.
     *
     * @param want Ability want.
     * @param requestCode Ability request code.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode StartAbility(const Want &want, int requestCode = DEFAULT_INVAL_VALUE);

    /**
     * StartAbility with want, send want to ability manager service.
     *
     * @param want Ability want.
     * @param callerToken, caller ability token.
     * @param requestCode Ability request code.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode StartAbility(
        const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode = DEFAULT_INVAL_VALUE);

    /**
     * Starts a new ability with specific start settings.
     *
     * @param want Indicates the ability to start.
     * @param requestCode the resultCode of the ability to start.
     * @param abilityStartSetting Indicates the setting ability used to start.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode StartAbility(const Want &want, const AbilityStartSetting &abilityStartSetting,
        const sptr<IRemoteObject> &callerToken, int requestCode = DEFAULT_INVAL_VALUE);

    /**
     * TerminateAbility with want, return want from ability manager service.
     *
     * @param token Ability token.
     * @param resultCode resultCode.
     * @param Want Ability want returned.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant);

    /**
     * TerminateAbility, terminate the special ability.
     *
     * @param callerToken, caller ability token.
     * @param requestCode Ability request code.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode TerminateAbility(const sptr<IRemoteObject> &callerToken, int requestCode);

    /**
     * Destroys this Service ability if the number of times it
     * has been started equals the number represented by
     * the given startId.
     *
     * @param token ability's token.
     * @param startId is incremented by 1 every time this ability is started.
     * @return Returns true if the startId matches the number of startup times
     * and this Service ability will be destroyed; returns false otherwise.
     */
    ErrCode TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId);
    /**
     * ConnectAbility, connect session with service ability.
     *
     * @param want, Special want for service type's ability.
     * @param connect, Callback used to notify caller the result of connecting or disconnecting.
     * @param callerToken, caller ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode ConnectAbility(
        const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken);

    /**
     * DisconnectAbility, disconnect session with service ability.
     *
     * @param connect, Callback used to notify caller the result of connecting or disconnecting.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode DisconnectAbility(const sptr<IAbilityConnection> &connect);

    /**
     * AcquireDataAbility, acquire a data ability by its authority, if it not existed,
     * AMS loads it synchronously.
     *
     * @param uri, data ability uri.
     * @param tryBind, true: when a data ability is died, ams will kill this client, or do nothing.
     * @param callerToken, specifies the caller ability token.
     * @return returns the data ability ipc object, or nullptr for failed.
     */
    sptr<IAbilityScheduler> AcquireDataAbility(const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken);

    /**
     * ReleaseDataAbility, release the data ability that referenced by 'dataAbilityToken'.
     *
     * @param dataAbilityToken, specifies the data ability that will be released.
     * @param callerToken, specifies the caller ability token.
     * @return returns ERR_OK if succeeded, or error codes for failed.
     */
    ErrCode ReleaseDataAbility(sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken);

    /**
     * dump ability stack info, about userID, mission stack info,
     * mission record info and ability info.
     *
     * @param state Ability stack info.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode DumpState(const std::string &args, std::vector<std::string> &state);

    /**
     * Connect ability manager service.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode Connect();

    /**
     * Get all stack info from ability manager service.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetAllStackInfo(StackInfo &stackInfo);

    /**
     * Destroys this Service ability by Want.
     *
     * @param want, Special want for service type's ability.
     * @return Returns true if this Service ability will be destroyed; returns false otherwise.
     */
    ErrCode StopServiceAbility(const Want &want);

    /**
     * Get the list of the missions that the user has recently launched,
     * with the most recent being first and older ones after in order.
     *
     * @param recentList recent mission info
     * @param numMax The maximum number of entries to return in the list. The
     * actual number returned may be smaller, depending on how many tasks the
     * user has started and the maximum number the system can remember.
     * @param falgs Information about what to return.  May be any combination
     * of {@link #RECENT_WITH_EXCLUDED} and {@link #RECENT_IGNORE_UNAVAILABLE}.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetRecentMissions(const int32_t numMax, const int32_t flags, std::vector<AbilityMissionInfo> &recentList);

    /**
     * Get mission snapshot by mission id
     *
     * @param missionId the id of the mission to retrieve the sAutoapshots
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot);

    /**
     * Ask that the mission associated with a given mission ID be moved to the
     * front of the stack, so it is now visible to the user.
     *
     * @param missionId.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode MoveMissionToTop(int32_t missionId);

    /**
     * Requires that tasks associated with a given capability token be moved to the background
     *
     * @param token ability token
     * @param nonFirst If nonfirst is false and not the lowest ability of the mission, you cannot move mission to end
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode MoveMissionToEnd(const sptr<IRemoteObject> &token, const bool nonFirst);

    /**
     * Remove the specified mission from the stack by missionid
     *
     * @param id.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode RemoveMissions(std::vector<int> missionId);

    /**
     * Remove the specified mission stack by stack id
     *
     * @param id.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode RemoveStack(int id);

    /**
     * Kill the process immediately.
     *
     * @param bundleName.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode KillProcess(const std::string &bundleName);

    /**
     * @brief Checks whether this ability is the first ability in a mission.
     *
     * @return Returns true is first in Mission.
     */
    ErrCode IsFirstInMission(const sptr<IRemoteObject> &token);

    ErrCode CompelVerifyPermission(const std::string &permission, int pid, int uid, std::string &message);

    /**
     * Save the top ability States and move them to the background
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode PowerOff();

    /**
     * Restore the state before top ability poweroff
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode PowerOn();

    /**
     * Sets the application to start its ability in lock mission mode.
     * @param missionId luck mission id
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode LockMission(int missionId);

    /**
     * Unlocks this ability by exiting the lock mission mode.
     * @param missionId unluck mission id
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode UnlockMission(int missionId);

    /**
     * Sets description information about the mission containing this ability.
     *
     * @param description Indicates the object containing information about the
     *                    mission. This parameter cannot be null.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode SetMissionDescriptionInfo(
        const sptr<IRemoteObject> &token, const MissionDescriptionInfo &description);

    /**
     * get current system mission lock mode state.
     *
     * @return Returns 0: LOCK_MISSION_STATE_NONE, 1: LOCK_MISSION_STATE_LOCKED
     */
    int GetMissionLockModeState();

    int UpdateConfiguration(const DummyConfiguration &config);

    sptr<IWantSender> GetWantSender(const WantSenderInfo &wantSenderInfo, const sptr<IRemoteObject> &callerToken);

    ErrCode SendWantSender(const sptr<IWantSender> &target, const SenderInfo &senderInfo);

    void CancelWantSender(const sptr<IWantSender> &sender);

    ErrCode GetPendingWantUid(const sptr<IWantSender> &target, int32_t &uid);

    ErrCode GetPendingWantUserId(const sptr<IWantSender> &target, int32_t &userId);

    ErrCode GetPendingWantBundleName(const sptr<IWantSender> &target, std::string &bundleName);

    ErrCode GetPendingWantCode(const sptr<IWantSender> &target, int32_t &code);

    ErrCode GetPendingWantType(const sptr<IWantSender> &target, int32_t &type);

    void RegisterCancelListener(const sptr<IWantSender> &sender, const sptr<IWantReceiver> &recevier);

    void UnregisterCancelListener(const sptr<IWantSender> &sender, const sptr<IWantReceiver> &recevier);

    ErrCode GetPendingRequestWant(const sptr<IWantSender> &target, std::shared_ptr<Want> &want);

    /**
     * Moving mission to the specified stack by mission option(Enter floating window mode).
     * @param missionOption, target mission option
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode MoveMissionToFloatingStack(const MissionOption &missionOption);

    /**
     * Moving mission to the specified stack by mission option(Enter floating window mode).
     * @param missionOption, target mission option
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode MoveMissionToSplitScreenStack(const MissionOption &missionOption);

    /**
     * minimize multiwindow by mission id.
     * @param missionId, the id of target mission
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode MinimizeMultiWindow(int missionId);

    /**
     * maximize multiwindow by mission id.
     * @param missionId, the id of target mission
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode MaximizeMultiWindow(int missionId);

    /**
     * Change the focus of ability in the mission stack.
     * @param lostToken, the token of lost focus ability
     * @param getToken, the token of get focus ability
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode ChangeFocusAbility(const sptr<IRemoteObject> &lostFocusToken, const sptr<IRemoteObject> &getFocusToken);

    /**
     * get missions info of floating mission stack.
     * @param list, mission info.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode GetFloatingMissions(std::vector<AbilityMissionInfo> &list);

    /**
     * close multiwindow by mission id.
     * @param missionId, the id of target mission.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode CloseMultiWindow(int missionId);

    /**
     * set special mission stack default settings.
     * @param stackSetting, mission stack default settings.
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode SetMissionStackSetting(const StackSetting &stackSetting);

private:
    static std::mutex mutex_;
    static std::shared_ptr<AbilityManagerClient> instance_;
    sptr<IRemoteObject> remoteObject_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_AAFWK_ABILITY_MANAGER_H
