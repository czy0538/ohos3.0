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

#ifndef FOUNDATION_AAFWK_SERVICES_MOCK_ABILITY_MGR_SERVICE_H
#define FOUNDATION_AAFWK_SERVICES_MOCK_ABILITY_MGR_SERVICE_H

#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "ability_manager_stub.h"

namespace OHOS {
namespace AAFwk {
class MockAbilityMgrService : public AbilityManagerStub {
public:
    MOCK_METHOD2(StartAbility, int(const Want &want, int requestCode));
    MOCK_METHOD3(StartAbility, int(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD2(TerminateAbilityByCaller, int(const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD3(TerminateAbility, int(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant));
    MOCK_METHOD3(ConnectAbility,
        int(const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken));
    MOCK_METHOD1(DisconnectAbility, int(const sptr<IAbilityConnection> &connect));
    MOCK_METHOD3(AcquireDataAbility, sptr<IAbilityScheduler>(const Uri &, bool, const sptr<IRemoteObject> &));
    MOCK_METHOD2(ReleaseDataAbility, int(sptr<IAbilityScheduler>, const sptr<IRemoteObject> &));
    MOCK_METHOD2(AddWindowInfo, void(const sptr<IRemoteObject> &token, int32_t windowToken));
    MOCK_METHOD2(AttachAbilityThread, int(const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token));
    MOCK_METHOD2(AbilityTransitionDone, int(const sptr<IRemoteObject> &token, int state));
    MOCK_METHOD2(
        ScheduleConnectAbilityDone, int(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject));
    MOCK_METHOD1(ScheduleDisconnectAbilityDone, int(const sptr<IRemoteObject> &token));
    MOCK_METHOD1(ScheduleCommandAbilityDone, int(const sptr<IRemoteObject> &));
    MOCK_METHOD2(DumpState, void(const std::string &args, std::vector<std::string> &state));
    MOCK_METHOD2(TerminateAbilityResult, int(const sptr<IRemoteObject> &, int startId));
    MOCK_METHOD1(StopServiceAbility, int(const Want &));
    MOCK_METHOD1(GetAllStackInfo, int(StackInfo &stackInfo));
    MOCK_METHOD3(GetRecentMissions, int(const int32_t, const int32_t, std::vector<AbilityMissionInfo> &));
    MOCK_METHOD2(GetMissionSnapshot, int(const int32_t, MissionSnapshotInfo &));
    MOCK_METHOD1(RemoveMission, int(int));
    MOCK_METHOD1(RemoveStack, int(int));
    MOCK_METHOD1(MoveMissionToTop, int(int32_t));
    MOCK_METHOD1(KillProcess, int(const std::string &));
    MOCK_METHOD1(UninstallApp, int(const std::string &));
    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));
    MOCK_METHOD2(MoveMissionToEnd, int(const sptr<IRemoteObject> &token, const bool nonFirst));
    MOCK_METHOD1(IsFirstInMission, bool(const sptr<IRemoteObject> &token));
    MOCK_METHOD4(CompelVerifyPermission, int(const std::string &permission, int pid, int uid, std::string &message));
    MOCK_METHOD0(PowerOff, int());
    MOCK_METHOD0(PowerOn, int());
    MOCK_METHOD1(LockMission, int(int));
    MOCK_METHOD1(UnlockMission, int(int));
    MOCK_METHOD2(SetMissionDescriptionInfo, int(const sptr<IRemoteObject> &token, const MissionDescriptionInfo &info));
    MOCK_METHOD0(GetMissionLockModeState, int());
    MOCK_METHOD1(UpdateConfiguration, int(const DummyConfiguration &));
    MOCK_METHOD2(
        GetWantSender, sptr<IWantSender>(const WantSenderInfo &wantSenderInfo, const sptr<IRemoteObject> &callerToken));
    MOCK_METHOD2(SendWantSender, int(const sptr<IWantSender> &target, const SenderInfo &senderInfo));
    MOCK_METHOD1(CancelWantSender, void(const sptr<IWantSender> &sender));
    MOCK_METHOD1(GetPendingWantUid, int(const sptr<IWantSender> &target));
    MOCK_METHOD1(GetPendingWantUserId, int(const sptr<IWantSender> &target));
    MOCK_METHOD1(GetPendingWantBundleName, std::string(const sptr<IWantSender> &target));
    MOCK_METHOD1(GetPendingWantCode, int(const sptr<IWantSender> &target));
    MOCK_METHOD1(GetPendingWantType, int(const sptr<IWantSender> &target));
    MOCK_METHOD2(RegisterCancelListener, void(const sptr<IWantSender> &sender, const sptr<IWantReceiver> &receiver));
    MOCK_METHOD2(UnregisterCancelListener, void(const sptr<IWantSender> &sender, const sptr<IWantReceiver> &receiver));
    MOCK_METHOD2(GetPendingRequestWant, int(const sptr<IWantSender> &target, std::shared_ptr<Want> &want));
    MOCK_METHOD4(StartAbility, int(const Want &want, const AbilityStartSetting &abilityStartSetting,
                                   const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD1(MoveMissionToFloatingStack, int(const MissionOption &missionOption));
    MOCK_METHOD1(MoveMissionToSplitScreenStack, int(const MissionOption &missionOption));
    MOCK_METHOD2(
        ChangeFocusAbility, int(const sptr<IRemoteObject> &lostFocusToken, const sptr<IRemoteObject> &getFocusToken));
    MOCK_METHOD1(MinimizeMultiWindow, int(int missionId));
    MOCK_METHOD1(MaximizeMultiWindow, int(int missionId));
    MOCK_METHOD1(GetFloatingMissions, int(std::vector<AbilityMissionInfo> &list));
    MOCK_METHOD1(CloseMultiWindow, int(int missionId));
    MOCK_METHOD1(SetMissionStackSetting, int(const StackSetting &stackSetting));

    void Wait()
    {
        sem_.Wait();
    }

    int Post()
    {
        sem_.Post();
        return 0;
    }

    void PostVoid()
    {
        sem_.Post();
    }
private:
    Semaphore sem_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // FOUNDATION_AAFWK_SERVICES_MOCK_ABILITY_MGR_SERVICE_H
