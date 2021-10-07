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

#include "continuation_manager.h"
#include "ability.h"
#include "ability_continuation_interface.h"
#include "continuation_handler.h"
#include "app_log_wrapper.h"
#include "want.h"
#include "continuation_scheduler.h"
#include "distributed_client.h"
#include "string_ex.h"
#include "operation_builder.h"

namespace OHOS {
namespace AppExecFwk {
const int ContinuationManager::TIMEOUT_MS_WAIT_DMS_SCHEDULE_START_CONTINUATION = 5000;
const int ContinuationManager::TIMEOUT_MS_WAIT_DMS_NOTIFY_CONTINUATION_COMPLETE = 6000;
const int ContinuationManager::TIMEOUT_MS_WAIT_REMOTE_NOTIFY_BACK = 6000;

ContinuationManager::ContinuationManager()
{
    progressState_ = ProgressState::INITIAL;
}

bool ContinuationManager::Init(const std::shared_ptr<Ability> &ability, const sptr<IRemoteObject> &continueToken,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ContinuationHandler> &continuationHandler)
{
    APP_LOGI("%{public}s called begin", __func__);
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::Init failed. ability is nullptr");
        return false;
    }
    ability_ = ability;

    std::shared_ptr<Ability> abilityTmp = nullptr;
    abilityTmp = ability_.lock();
    if (abilityTmp == nullptr) {
        APP_LOGE("ContinuationManager::Init failed. get ability is nullptr");
        return false;
    }

    if (abilityTmp->GetAbilityInfo() == nullptr) {
        APP_LOGE("ContinuationManager::Init failed. abilityInfo is nullptr");
        return false;
    }
    abilityInfo_ = abilityTmp->GetAbilityInfo();

    if (continueToken == nullptr) {
        APP_LOGE("ContinuationManager::Init failed. continueToken is nullptr");
        return false;
    }
    continueToken_ = continueToken;

    continuationHandler_ = continuationHandler;
    APP_LOGI("%{public}s called end", __func__);
    return true;
}

ContinuationState ContinuationManager::GetContinuationState()
{
    return continuationState_;
}

std::string ContinuationManager::GetOriginalDeviceId()
{
    return originalDeviceId_;
}

void ContinuationManager::ContinueAbility(bool reversible, const std::string &deviceId)
{
    APP_LOGI("%{public}s called begin", __func__);
    if (CheckContinuationIllegal()) {
        APP_LOGE("ContinuationManager::ContinueAbility failed. Ability not available to continueAbility.");
        return;
    }

    if (progressState_ != ProgressState::INITIAL) {
        APP_LOGE("ContinuationManager::ContinueAbility failed. Another request in progress. progressState_: %{public}d",
            progressState_);
        return;
    }

    if (continuationState_ != ContinuationState::LOCAL_RUNNING) {
        APP_LOGE("ContinuationManager::ContinueAbility failed. Illegal continuation state. Current state is %{public}d",
            continuationState_);
        return;
    }

    if (HandleContinueAbility(reversible, deviceId)) {
        reversible_ = reversible;
        ChangeProcessState(ProgressState::WAITING_SCHEDULE);
        // Wait state change timeout. Restore state
        RestoreStateWhenTimeout(TIMEOUT_MS_WAIT_DMS_SCHEDULE_START_CONTINUATION, ProgressState::WAITING_SCHEDULE);
    }
    APP_LOGI("%{public}s called end", __func__);
}

bool ContinuationManager::ReverseContinueAbility()
{
    APP_LOGI("%{public}s called begin", __func__);
    if (progressState_ != ProgressState::INITIAL) {
        APP_LOGE("ContinuationManager::ReverseContinueAbility failed. progressState_ is %{public}d.", progressState_);
        return false;
    }

    if (continuationState_ != ContinuationState::REMOTE_RUNNING) {
        APP_LOGE("ContinuationManager::ReverseContinueAbility failed. continuationState_ is %{public}d.",
            continuationState_);
        return false;
    }

    std::shared_ptr<ContinuationHandler> continuationHandler = continuationHandler_.lock();
    if (continuationHandler == nullptr) {
        APP_LOGE("ContinuationManager::ReverseContinueAbility failed. continuationHandler_ is nullptr.");
        return false;
    }

    bool requestSuccess = continuationHandler->ReverseContinueAbility();
    if (requestSuccess) {
        ChangeProcessState(ProgressState::WAITING_SCHEDULE);
        RestoreStateWhenTimeout(TIMEOUT_MS_WAIT_REMOTE_NOTIFY_BACK, ProgressState::WAITING_SCHEDULE);
    }
    APP_LOGI("%{public}s called end", __func__);
    return requestSuccess;
}

bool ContinuationManager::StartContinuation()
{
    APP_LOGI("%{public}s called begin", __func__);
    ChangeProcessState(ProgressState::IN_PROGRESS);
    bool result = DoScheduleStartContinuation();
    if (!result) {
        ChangeProcessState(ProgressState::INITIAL);
    }
    APP_LOGI("%{public}s called end", __func__);
    return result;
}

bool ContinuationManager::SaveData(WantParams &saveData)
{
    APP_LOGI("%{public}s called begin", __func__);
    bool result = DoScheduleSaveData(saveData);
    if (!result) {
        ChangeProcessState(ProgressState::INITIAL);
    } else {
        RestoreStateWhenTimeout(TIMEOUT_MS_WAIT_DMS_NOTIFY_CONTINUATION_COMPLETE, ProgressState::IN_PROGRESS);
    }
    APP_LOGI("%{public}s called end", __func__);
    return result;
}

bool ContinuationManager::RestoreData(
    const WantParams &restoreData, bool reversible, const std::string &originalDeviceId)
{
    APP_LOGI("%{public}s called begin", __func__);
    ChangeProcessState(ProgressState::IN_PROGRESS);
    bool result = DoScheduleRestoreData(restoreData);
    if (reversible) {
        continuationState_ = ContinuationState::REPLICA_RUNNING;
    }
    originalDeviceId_ = originalDeviceId;
    ChangeProcessState(ProgressState::INITIAL);
    APP_LOGI("%{public}s called end", __func__);
    return result;
}

void ContinuationManager::NotifyCompleteContinuation(
    const std::string &originDeviceId, int sessionId, bool success, const sptr<IRemoteObject> &reverseScheduler)
{
    APP_LOGI("%{public}s called begin", __func__);
    InitDistSchedulerHost();
    DistributedClient::GetInstance()->NotifyCompleteContinuation(
        Str8ToStr16(originDeviceId), sessionId, success, reverseScheduler);
    APP_LOGI("%{public}s called end", __func__);
}

void ContinuationManager::CompleteContinuation(int result)
{
    APP_LOGI("%{public}s called begin", __func__);
    if (CheckContinuationIllegal()) {
        APP_LOGE("ContinuationManager::CompleteContinuation failed. Ability not available to complete continuation.");
        return;
    }

    std::shared_ptr<Ability> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::CheckContinuationIllegal failed. ability is nullptr");
        return;
    }

    if (result == 0 && reversible_) {
        continuationState_ = ContinuationState::REMOTE_RUNNING;
    }
    ChangeProcessState(ProgressState::INITIAL);

    ability->OnCompleteContinuation(result);

    if (!reversible_) {
        ability->TerminateAbility();
    }
    APP_LOGI("%{public}s called end", __func__);
}

bool ContinuationManager::RestoreFromRemote(const WantParams &restoreData)
{
    APP_LOGI("%{public}s called begin", __func__);
    ChangeProcessState(ProgressState::IN_PROGRESS);
    bool result = DoRestoreFromRemote(restoreData);
    /*
     * No matter what the result is, we should reset the status. Because even it fail, we can do
     * nothing but let the user send another reverse continuation request again.
     */
    ChangeProcessState(ProgressState::INITIAL);
    if (result) {
        continuationState_ = ContinuationState::LOCAL_RUNNING;
    }
    APP_LOGI("%{public}s called end", __func__);
    return result;
}

bool ContinuationManager::NotifyRemoteTerminated()
{
    APP_LOGI("%{public}s called begin", __func__);
    continuationState_ = ContinuationState::LOCAL_RUNNING;
    ChangeProcessState(ProgressState::INITIAL);

    std::shared_ptr<Ability> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::NotifyRemoteTerminated failed. ability is nullptr");
        return false;
    }

    ability->OnRemoteTerminated();
    APP_LOGI("%{public}s called end", __func__);
    return true;
}

bool ContinuationManager::CheckContinuationIllegal()
{
    APP_LOGI("%{public}s called begin", __func__);
    std::shared_ptr<Ability> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::CheckContinuationIllegal failed. ability is nullptr");
        return false;
    }

    if (ability->GetState() >= AbilityLifecycleExecutor::LifecycleState::UNINITIALIZED) {
        APP_LOGE("ContinuationManager::CheckContinuationIllegal failed. ability state is wrong: %{public}d",
            ability->GetState());
        return true;
    }
    APP_LOGI("%{public}s called end", __func__);
    return false;
}

bool ContinuationManager::HandleContinueAbility(bool reversible, const std::string &deviceId)
{
    APP_LOGI("%{public}s called begin", __func__);

    if (!CheckAbilityToken()) {
        APP_LOGE("ContinuationManager::HandleContinueAbility failed. CheckAbilityToken failed");
        return false;
    }

    if (!RegisterAbilityTokenIfNeed(continueToken_)) {
        APP_LOGE("ContinuationManager::HandleContinueAbility failed. RegisterAbilityTokenIfNeed failed");
        return false;
    }

    sptr<IRemoteObject> continueToken = continueToken_;
    std::shared_ptr<ContinuationHandler> continuationHandler = continuationHandler_.lock();
    if (continuationHandler == nullptr) {
        APP_LOGE("ContinuationManager::HandleContinueAbility failed. continuationHandler is nullptr");
        return false;
    }
    continuationHandler->SetReversible(reversible);

    auto task = [continuationHandler, continueToken, deviceId]() {
        continuationHandler->HandleStartContinuation(continueToken, deviceId);
    };

    if (!mainHandler_->PostTask(task)) {
        APP_LOGE("ContinuationManager::HandleContinueAbility failed.PostTask failed");
        return false;
    }

    APP_LOGI("%{public}s called end", __func__);
    return true;
}

ContinuationManager::ProgressState ContinuationManager::GetProcessState()
{
    return progressState_;
}

void ContinuationManager::ChangeProcessState(const ProgressState &newState)
{
    APP_LOGI("%{public}s called begin. progressState_: %{public}d, newState: %{public}d",
        __func__,
        progressState_,
        newState);

    progressState_ = newState;
}

void ContinuationManager::RestoreStateWhenTimeout(long timeoutInMs, const ProgressState &preState)
{
    APP_LOGI("%{public}s called begin", __func__);
    InitMainHandlerIfNeed();

    auto timeoutTask = [continuationManager = shared_from_this(), preState]() {
        APP_LOGI(
            "ContinuationManager::RestoreStateWhenTimeout called. preState = %{public}d, currentState = %{public}d.",
            preState,
            continuationManager->GetProcessState());
        if (preState == continuationManager->GetProcessState()) {
            continuationManager->ChangeProcessState(ProgressState::INITIAL);
        }
    };
    mainHandler_->PostTask(timeoutTask, "Restore_State_When_Timeout", timeoutInMs);
    APP_LOGI("%{public}s called end", __func__);
}

void ContinuationManager::InitMainHandlerIfNeed()
{
    APP_LOGI("%{public}s called begin", __func__);
    if (mainHandler_ == nullptr) {
        APP_LOGI("Try to init main handler.");
        std::lock_guard<std::mutex> lock_l(lock_);
        if ((mainHandler_ == nullptr) && (EventRunner::GetMainEventRunner() != nullptr)) {
            mainHandler_ = std::make_shared<EventHandler>(EventRunner::GetMainEventRunner());
        }
    }
    APP_LOGI("%{public}s called end", __func__);
}

bool ContinuationManager::CheckAbilityToken()
{
    APP_LOGI("%{public}s called", __func__);
    if (continueToken_ == nullptr) {
        APP_LOGI("%{public}s called failed", __func__);
        return false;
    }
    APP_LOGI("%{public}s called success", __func__);
    return true;
}

bool ContinuationManager::UnregisterAbilityTokenIfNeed()
{
    APP_LOGI("%{public}s called begin", __func__);

    if (continueToken_ == nullptr) {
        APP_LOGE("%{public}s called failed continueToken_ is nullptr", __func__);
        return false;
    }

    if (!tokenRegistered_) {
        APP_LOGI("%{public}s called no need to :Unregister", __func__);
        return true;
    }

    std::lock_guard<std::mutex> lock_l(lockForRegist_);

    if (!tokenRegistered_) {
        APP_LOGI("%{public}s called no need to :Unregister", __func__);
        return true;
    }

    bool unregisterSuccess = UnregisterAbilityToken(continueToken_);
    if (unregisterSuccess) {
        tokenRegistered_ = false;
    }

    APP_LOGI("%{public}s called end", __func__);
    return unregisterSuccess;
}

bool ContinuationManager::UnregisterAbilityToken(const sptr<IRemoteObject> &token)
{
    APP_LOGI("%{public}s called begin", __func__);
    int result = -1;

    if (distSchedulerHost_ == nullptr) {
        APP_LOGE("%{public}s called failed. distSchedulerHost_ is nullptr", __func__);
        return false;
    }

    result = DistributedClient::GetInstance()->UnregisterAbilityToken(token, distSchedulerHost_);
    if (result != -1) {
        std::string interfaceName = "unregisterAbilityToken";
        CheckDmsInterfaceResult(result, interfaceName);
    }

    APP_LOGI("%{public}s called end", __func__);
    return (result == NO_ERROR);
}

bool ContinuationManager::RegisterAbilityTokenIfNeed(const sptr<IRemoteObject> &token)
{
    APP_LOGI("%{public}s called begin", __func__);
    if (tokenRegistered_) {
        return true;
    }

    std::lock_guard<std::mutex> lock_l(lockForRegist_);

    if (tokenRegistered_) {
        return true;
    }

    bool registerSuccess = RegisterAbilityToken(token);
    if (registerSuccess) {
        tokenRegistered_ = true;
    }
    APP_LOGI("%{public}s called end", __func__);
    return registerSuccess;
}

bool ContinuationManager::RegisterAbilityToken(const sptr<IRemoteObject> &token)
{
    APP_LOGI("%{public}s called begin", __func__);
    int result = -1;

    InitDistSchedulerHost();

    result = DistributedClient::GetInstance()->RegisterAbilityToken(token, distSchedulerHost_);
    if (result != -1) {
        std::string interfaceName = "registerAbilityToken";
        CheckDmsInterfaceResult(result, interfaceName);
    }
    APP_LOGI("%{public}s called end", __func__);
    return (result == NO_ERROR);
}

void ContinuationManager::InitDistSchedulerHost()
{
    APP_LOGI("%{public}s called begin", __func__);
    if (distSchedulerHost_ == nullptr) {
        InitMainHandlerIfNeed();
        std::weak_ptr<IDistributeScheduleHandler> continuationHandler = continuationHandler_;
        distSchedulerHost_ =
            sptr<ContinuationScheduler>(new (std::nothrow) ContinuationScheduler(continuationHandler, mainHandler_));
        if (distSchedulerHost_ == nullptr) {
            APP_LOGE("AbilityThread::AbilityThreadMain failed,thread  is nullptr");
            return;
        }
    }
    APP_LOGI("%{public}s called end", __func__);
}

void ContinuationManager::CheckDmsInterfaceResult(int result, const std::string &interfaceName)
{
    APP_LOGI("ContinuationManager::CheckDmsInterfaceResult called. interfaceName: %{public}s, result: %{public}d.",
        interfaceName.c_str(),
        result);
}

bool ContinuationManager::DoScheduleStartContinuation()
{
    APP_LOGI("%{public}s called begin", __func__);
    if (CheckContinuationIllegal()) {
        APP_LOGE(
            "ContinuationManager::DoScheduleStartContinuation called. Ability not available to startContinuation.");
        return false;
    }

    std::shared_ptr<Ability> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::DoScheduleStartContinuation failed. ability is nullptr");
        return false;
    }
    if (!ability->OnStartContinuation()) {
        APP_LOGI("%{public}s called failed to StartContinuation", __func__);
        return false;
    }
    APP_LOGI("%{public}s called end", __func__);
    return true;
}

bool ContinuationManager::DoScheduleSaveData(WantParams &saveData)
{
    APP_LOGI("%{public}s called begin", __func__);
    if (CheckContinuationIllegal()) {
        APP_LOGE("ContinuationManager::DoScheduleSaveData failed. Ability not available to save data.");
        return false;
    }

    std::shared_ptr<Ability> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::DoScheduleSaveData failed. ability is nullptr");
        return false;
    }

    WantParams abilitySaveData;
    bool ret = ability->OnSaveData(abilitySaveData);
    for (std::string key : abilitySaveData.KeySet()) {
        saveData.SetParam(key, abilitySaveData.GetParam(key).GetRefPtr());
    }

    if (!ret) {
        APP_LOGE("ContinuationManager::DoScheduleSaveData failed. Ability save data failed.");
    }
    APP_LOGI("%{public}s called end", __func__);
    return ret;
}

bool ContinuationManager::DoScheduleRestoreData(const WantParams &restoreData)
{
    APP_LOGI("%{public}s called begin", __func__);
    if (CheckContinuationIllegal()) {
        APP_LOGE("ContinuationManager::DoScheduleRestoreData failed. Ability not available to restore data.");
        return false;
    }

    std::shared_ptr<Ability> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::DoScheduleRestoreData failed. ability is nullptr");
        return false;
    }

    WantParams abilityRestoreData;
    for (std::string key : restoreData.KeySet()) {
        abilityRestoreData.SetParam(key, restoreData.GetParam(key).GetRefPtr());
    }

    bool ret = ability->OnRestoreData(abilityRestoreData);
    if (!ret) {
        APP_LOGE("ContinuationManager::DoScheduleRestoreData failed. Ability restore data failed.");
    }
    APP_LOGI("%{public}s called end", __func__);
    return ret;
}

bool ContinuationManager::DoRestoreFromRemote(const WantParams &restoreData)
{
    APP_LOGI("%{public}s called begin", __func__);
    std::shared_ptr<Ability> ability = nullptr;
    ability = ability_.lock();
    if (ability == nullptr) {
        APP_LOGE("ContinuationManager::DoRestoreFromRemote failed. ability is nullptr");
        return false;
    }

    WantParams abilityRestoreData;
    for (std::string key : restoreData.KeySet()) {
        abilityRestoreData.SetParam(key, restoreData.GetParam(key).GetRefPtr());
    }

    bool ret = ability->OnRestoreData(abilityRestoreData);
    if (!ret) {
        APP_LOGE("ContinuationManager::DoRestoreFromRemote failed. Ability restore data failed.");
    }
    APP_LOGI("%{public}s called end", __func__);
    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS