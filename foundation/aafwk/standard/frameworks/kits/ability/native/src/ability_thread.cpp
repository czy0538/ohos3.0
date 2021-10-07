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

#include "ability_thread.h"
#include <functional>
#include "ohos_application.h"
#include "ability_loader.h"
#include "ability_state.h"
#include "ability_impl_factory.h"
#include "page_ability_impl.h"
#include "application_impl.h"
#include "app_log_wrapper.h"
#include "context_deal.h"
#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "values_bucket.h"
#include "dataobs_mgr_client.h"

namespace OHOS {
namespace AppExecFwk {
using AbilityManagerClient = OHOS::AAFwk::AbilityManagerClient;
using DataObsMgrClient = OHOS::AAFwk::DataObsMgrClient;
constexpr static char ACE_ABILITY_NAME[] = "AceAbility";
constexpr static char ACE_SERVICE_ABILITY_NAME[] = "AceServiceAbility";
constexpr static char ACE_DATA_ABILITY_NAME[] = "AceDataAbility";
constexpr static char ACE_FORM_ABILITY_NAME[] = "AceFormAbility";

/**
 * @brief Default constructor used to create a AbilityThread instance.
 */
AbilityThread::AbilityThread()
    : abilityImpl_(nullptr), token_(nullptr), currentAbility_(nullptr), abilityHandler_(nullptr), runner_(nullptr)
{}

AbilityThread::~AbilityThread()
{
    DelayedSingleton<AbilityImplFactory>::DestroyInstance();
}

/**
 * @description: Attach The ability thread to the main process.
 *
 * @param abilityRecord Indicates the abilityRecord.
 *
 * @return Returns the abilityName.
 *
 */
std::string AbilityThread::CreateAbilityName(const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    std::string abilityName;
    APP_LOGI("AbilityThread::CreateAbilityName begin");
    if (abilityRecord == nullptr) {
        APP_LOGE("AbilityThread::CreateAbilityName failed,abilityRecord is nullptr");
        return abilityName;
    }

    std::shared_ptr<AbilityInfo> abilityInfo = abilityRecord->GetAbilityInfo();
    if (abilityInfo == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,abilityInfo is nullptr");
        return abilityName;
    }

    APP_LOGI("AbilityThread::ability attach the ability type is %{public}d", abilityInfo->type);
    APP_LOGI("AbilityThread::ability attach the ability is Native %{public}d", abilityInfo->isNativeAbility);

    if (abilityInfo->isNativeAbility == false) {
        if (abilityInfo->type == AbilityType::PAGE) {
            abilityName = ACE_ABILITY_NAME;
        } else if (abilityInfo->type == AbilityType::SERVICE) {
            if (abilityInfo->formEnabled == true) {
                abilityName = ACE_FORM_ABILITY_NAME;
            } else {
                abilityName = ACE_SERVICE_ABILITY_NAME;
            }
        } else if (abilityInfo->type == AbilityType::DATA) {
            abilityName = ACE_DATA_ABILITY_NAME;
        } else {
            abilityName = abilityInfo->name;
        }
    } else {
        abilityName = abilityInfo->name;
    }

    APP_LOGI("AbilityThread::CreateAbilityName end");
    return abilityName;
}

/**
 * @description: Create and init contextDeal.
 *
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 * @param abilityObject Indicates the abilityObject.
 *
 * @return Returns the contextDeal.
 *
 */
std::shared_ptr<ContextDeal> AbilityThread::CreateAndInitContextDeal(std::shared_ptr<OHOSApplication> &application,
    const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<Context> &abilityObject)
{
    APP_LOGI("AbilityThread::CreateAndInitContextDeal begin");
    std::shared_ptr<ContextDeal> contextDeal = nullptr;
    APP_LOGI("AbilityThread::CreateAndInitContextDeal called");
    if ((application == nullptr) || (abilityRecord == nullptr) || (abilityObject == nullptr)) {
        APP_LOGE("AbilityThread::ability attach failed,context or record or abilityObject is nullptr");
        return contextDeal;
    }

    contextDeal = std::make_shared<ContextDeal>();
    if (contextDeal == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,contextDeal  is nullptr");
        return contextDeal;
    }

    contextDeal->SetAbilityInfo(abilityRecord->GetAbilityInfo());
    contextDeal->SetApplicationInfo(application->GetApplicationInfo());
    contextDeal->SetProcessInfo(application->GetProcessInfo());

    std::shared_ptr<Context> tmpContext = application->GetApplicationContext();
    contextDeal->SetApplicationContext(tmpContext);

    contextDeal->SetBundleCodePath(abilityRecord->GetAbilityInfo()->codePath);
    contextDeal->SetContext(abilityObject);
    contextDeal->SetRunner(abilityHandler_->GetEventRunner());
    APP_LOGI("AbilityThread::CreateAndInitContextDeal end");
    return contextDeal;
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 * @param mainRunner The runner which main_thread holds.
 */
void AbilityThread::Attach(std::shared_ptr<OHOSApplication> &application,
    const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<EventRunner> &mainRunner)
{
    APP_LOGI("AbilityThread::Attach begin");
    if ((application == nullptr) || (abilityRecord == nullptr) || (mainRunner == nullptr)) {
        APP_LOGE("AbilityThread::ability attach failed,context or record is nullptr");
        return;
    }

    // 1.new AbilityHandler
    std::string abilityName = CreateAbilityName(abilityRecord);
    abilityHandler_ = std::make_shared<AbilityHandler>(mainRunner, this);
    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,abilityHandler_ is nullptr");
        return;
    }

    // 2.new ability
    auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityName);
    if (ability == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,load ability failed");
        return;
    }

    APP_LOGI("AbilityThread::new ability success.");
    currentAbility_.reset(ability);
    token_ = abilityRecord->GetToken();
    abilityRecord->SetEventHandler(abilityHandler_);
    abilityRecord->SetEventRunner(mainRunner);
    abilityRecord->SetAbilityThread(this);
    std::shared_ptr<Context> abilityObject = currentAbility_;
    std::shared_ptr<ContextDeal> contextDeal = CreateAndInitContextDeal(application, abilityRecord, abilityObject);
    ability->AttachBaseContext(contextDeal);

    // 3.new abilityImpl
    abilityImpl_ =
        DelayedSingleton<AbilityImplFactory>::GetInstance()->MakeAbilityImplObject(abilityRecord->GetAbilityInfo());
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ability abilityImpl_ == nullptr");
        return;
    }
    APP_LOGI("AbilityThread::Attach before abilityImpl_->Init");
    abilityImpl_->Init(application, abilityRecord, currentAbility_, abilityHandler_, token_, contextDeal);
    APP_LOGI("AbilityThread::Attach after abilityImpl_->Init");
    // 4. ability attach : ipc
    APP_LOGI("AbilityThread::Attach before AttachAbilityThread");
    ErrCode err = AbilityManagerClient::GetInstance()->AttachAbilityThread(this, token_);
    APP_LOGI("AbilityThread::Attach after AttachAbilityThread");
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: attach success faile err = %{public}d", err);
        return;
    }

    APP_LOGI("AbilityThread::Attach end");
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 */
void AbilityThread::Attach(
    std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    APP_LOGI("AbilityThread::Attach begin");
    if ((application == nullptr) || (abilityRecord == nullptr)) {
        APP_LOGE("AbilityThread::ability attach failed,context or record is nullptr");
        return;
    }
    // 1.new AbilityHandler
    std::string abilityName = CreateAbilityName(abilityRecord);
    runner_ = EventRunner::Create(abilityName);
    if (runner_ == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,create runner failed");
        return;
    }
    abilityHandler_ = std::make_shared<AbilityHandler>(runner_, this);
    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,abilityHandler_ is nullptr");
        return;
    }

    // 2.new ability
    auto ability = AbilityLoader::GetInstance().GetAbilityByName(abilityName);
    if (ability == nullptr) {
        APP_LOGE("AbilityThread::ability attach failed,load ability failed");
        return;
    }

    APP_LOGI("AbilityThread::new ability success.");
    currentAbility_.reset(ability);
    token_ = abilityRecord->GetToken();
    abilityRecord->SetEventHandler(abilityHandler_);
    abilityRecord->SetEventRunner(runner_);
    abilityRecord->SetAbilityThread(this);
    std::shared_ptr<Context> abilityObject = currentAbility_;
    std::shared_ptr<ContextDeal> contextDeal = CreateAndInitContextDeal(application, abilityRecord, abilityObject);
    ability->AttachBaseContext(contextDeal);

    // 3.new abilityImpl
    abilityImpl_ =
        DelayedSingleton<AbilityImplFactory>::GetInstance()->MakeAbilityImplObject(abilityRecord->GetAbilityInfo());
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ability abilityImpl_ == nullptr");
        return;
    }
    APP_LOGI("AbilityThread::Attach before abilityImpl_->Init");
    abilityImpl_->Init(application, abilityRecord, currentAbility_, abilityHandler_, token_, contextDeal);
    APP_LOGI("AbilityThread::Attach after abilityImpl_->Init");
    // 4. ability attach : ipc
    APP_LOGI("AbilityThread::Attach before AttachAbilityThread");
    ErrCode err = AbilityManagerClient::GetInstance()->AttachAbilityThread(this, token_);
    APP_LOGI("AbilityThread::Attach after AttachAbilityThread");
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: attach success faile err = %{public}d", err);
        return;
    }

    APP_LOGI("AbilityThread::Attach end");
}

/**
 * @description:  Handle the life cycle of Ability.
 * @param want  Indicates the structure containing lifecycle information about the ability.
 * @param lifeCycleStateInfo  Indicates the lifeCycleStateInfo.
 */
void AbilityThread::HandleAbilityTransaction(const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo)
{
    APP_LOGI("AbilityThread::HandleAbilityTransaction begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleAbilityTransaction abilityImpl_ == nullptr");
        return;
    }

    APP_LOGI("AbilityThread::HandleAbilityTransaction before abilityImpl_->SetCallingContext");
    abilityImpl_->SetCallingContext(lifeCycleStateInfo.caller.deviceId,
        lifeCycleStateInfo.caller.bundleName,
        lifeCycleStateInfo.caller.abilityName);
    APP_LOGI("AbilityThread::HandleAbilityTransaction after abilityImpl_->SetCallingContext");
    APP_LOGI("AbilityThread::HandleAbilityTransaction before abilityImpl_->HandleAbilityTransaction");
    abilityImpl_->HandleAbilityTransaction(want, lifeCycleStateInfo);
    APP_LOGI("AbilityThread::HandleAbilityTransaction after abilityImpl_->HandleAbilityTransaction");
    APP_LOGI("AbilityThread::HandleAbilityTransaction end");
}

/**
 * @description:  Handle the current connection of Ability.
 * @param want  Indicates the structure containing connection information about the ability.
 */
void AbilityThread::HandleConnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::HandleConnectAbility begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleConnectAbility abilityImpl_ == nullptr");
        return;
    }

    APP_LOGI("AbilityThread::HandleConnectAbility before abilityImpl_->ConnectAbility");
    sptr<IRemoteObject> service = abilityImpl_->ConnectAbility(want);
    APP_LOGI("AbilityThread::HandleConnectAbility after abilityImpl_->ConnectAbility");
    APP_LOGI("AbilityThread::HandleConnectAbility before ScheduleConnectAbilityDone");
    ErrCode err = AbilityManagerClient::GetInstance()->ScheduleConnectAbilityDone(token_, service);
    APP_LOGI("AbilityThread::HandleConnectAbility after ScheduleConnectAbilityDone");
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: HandleConnectAbility faile err = %{public}d", err);
    }
    APP_LOGI("AbilityThread::HandleConnectAbility end");
}

/**
 * @description:  Handle the current disconnection of Ability.
 */
void AbilityThread::HandleDisconnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::HandleDisconnectAbility begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleDisconnectAbility abilityImpl_ == nullptr");
        return;
    }

    APP_LOGI("AbilityThread::HandleDisconnectAbility before abilityImpl_->DisconnectAbility");
    abilityImpl_->DisconnectAbility(want);
    APP_LOGI("AbilityThread::HandleDisconnectAbility after abilityImpl_->DisconnectAbility");
    APP_LOGI("AbilityThread::HandleDisconnectAbility before ScheduleDisconnectAbilityDone");
    ErrCode err = AbilityManagerClient::GetInstance()->ScheduleDisconnectAbilityDone(token_);
    APP_LOGI("AbilityThread::HandleDisconnectAbility after ScheduleDisconnectAbilityDone");
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: HandleDisconnectAbility faile err = %{public}d", err);
    }
    APP_LOGI("AbilityThread::HandleDisconnectAbility end");
}

/**
 * @brief Handle the current commadn of Ability.
 *
 * @param want The Want object to command to.
 *
 * * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
 * destroyed, and the value false indicates a normal startup.
 *
 * @param startId Indicates the number of times the Service ability has been started. The startId is incremented by 1
 * every time the ability is started. For example, if the ability has been started for six times, the value of startId
 * is 6.
 */
void AbilityThread::HandleCommandAbility(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityThread::HandleCommandAbility begin");
    APP_LOGI("AbilityThread::HandleCommandAbility before abilityImpl_->CommandAbility");
    abilityImpl_->CommandAbility(want, restart, startId);
    APP_LOGI("AbilityThread::HandleCommandAbility after abilityImpl_->CommandAbility");
    APP_LOGI("AbilityThread::HandleCommandAbility before ScheduleCommandAbilityDone");
    ErrCode err = AbilityManagerClient::GetInstance()->ScheduleCommandAbilityDone(token_);
    APP_LOGI("AbilityThread::HandleCommandAbility after ScheduleCommandAbilityDone");
    if (err != ERR_OK) {
        APP_LOGE("AbilityThread:: HandleCommandAbility  faile err = %{public}d", err);
    }
    APP_LOGI("AbilityThread::HandleCommandAbility end");
}

/**
 * @description: Handle the SaveAbility state.
 * @param state Indicates save ability state used to dispatchSaveAbilityState.
 */
void AbilityThread::HandleSaveAbilityState(PacMap &state)
{
    APP_LOGI("AbilityThread::HandleSaveAbilityState begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleSaveAbilityState abilityImpl_ == nullptr");
        return;
    }

    APP_LOGI("AbilityThread::HandleSaveAbilityState before abilityImpl_->DispatchSaveAbilityState");
    abilityImpl_->DispatchSaveAbilityState(state);
    APP_LOGI("AbilityThread::HandleSaveAbilityState after abilityImpl_->DispatchSaveAbilityState");
    APP_LOGI("AbilityThread::HandleSaveAbilityState end");
}

/**
 * @description: Handle the restoreAbility state.
 * @param state  Indicates save ability state used to dispatchRestoreAbilityState.
 */
void AbilityThread::HandleRestoreAbilityState(const PacMap &state)
{
    APP_LOGI("AbilityThread::HandleRestoreAbilityState begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleRestoreAbilityState abilityImpl_ == nullptr");
        return;
    }

    APP_LOGI("AbilityThread::HandleRestoreAbilityState before abilityImpl_->DispatchRestoreAbilityState");
    abilityImpl_->DispatchRestoreAbilityState(state);
    APP_LOGI("AbilityThread::HandleRestoreAbilityState after abilityImpl_->DispatchRestoreAbilityState");
    APP_LOGI("AbilityThread::HandleRestoreAbilityState end");
}

/**
 * @description: Provide operating system SaveabilityState information to the observer
 * @param state Indicates save ability state used to dispatch.
 */
void AbilityThread::ScheduleSaveAbilityState(PacMap &state)
{
    APP_LOGI("AbilityThread::ScheduleSaveAbilityState begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleSaveAbilityState abilityImpl_ == nullptr");
        return;
    }

    APP_LOGI("AbilityThread::ScheduleSaveAbilityState before abilityImpl_->DispatchSaveAbilityState");
    abilityImpl_->DispatchSaveAbilityState(state);
    APP_LOGI("AbilityThread::ScheduleSaveAbilityState after abilityImpl_->DispatchSaveAbilityState");
    APP_LOGI("AbilityThread::ScheduleSaveAbilityState end");
}

/**
 * @description:  Provide operating system RestoreAbilityState information to the observer
 * @param state Indicates resotre ability state used to dispatchRestoreAbilityState.
 */
void AbilityThread::ScheduleRestoreAbilityState(const PacMap &state)
{
    APP_LOGI("AbilityThread::ScheduleRestoreAbilityState begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleRestoreAbilityState abilityImpl_ == nullptr");
        return;
    }
    APP_LOGI("AbilityThread::ScheduleRestoreAbilityState before abilityImpl_->DispatchRestoreAbilityState");
    abilityImpl_->DispatchRestoreAbilityState(state);
    APP_LOGI("AbilityThread::ScheduleRestoreAbilityState after abilityImpl_->DispatchRestoreAbilityState");
    APP_LOGI("AbilityThread::ScheduleRestoreAbilityState end");
}

/*
 * @brief ScheduleUpdateConfiguration, scheduling update configuration.
 */
void AbilityThread::ScheduleUpdateConfiguration(const DummyConfiguration &config)
{
    APP_LOGI("AbilityThread::ScheduleUpdateConfiguration begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleUpdateConfiguration abilityImpl_ is nullptr");
        return;
    }

    auto task = [abilitThread = this, config]() { abilitThread->HandleUpdateConfiguration(config); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleUpdateConfiguration abilityHandler_ is nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleUpdateConfiguration PostTask error");
    }
    APP_LOGI("AbilityThread::ScheduleUpdateConfiguration end");
}

/*
 * @brief Handle the scheduling update configuration.
 */
void AbilityThread::HandleUpdateConfiguration(const DummyConfiguration &config)
{
    APP_LOGI("AbilityThread::HandleUpdateConfiguration begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::HandleUpdateConfiguration abilityImpl_ is nullptr");
        return;
    }

    APP_LOGI("AbilityThread::HandleUpdateConfiguration before abilityImpl_->ScheduleUpdateConfiguration");
    abilityImpl_->ScheduleUpdateConfiguration(config);
    APP_LOGI("AbilityThread::HandleUpdateConfiguration after abilityImpl_->ScheduleUpdateConfiguration");
    APP_LOGI("AbilityThread::HandleUpdateConfiguration end");
}

/**
 * @description:  Provide operating system AbilityTransaction information to the observer
 * @param want Indicates the structure containing Transaction information about the ability.
 * @param lifeCycleStateInfo Indicates the lifecycle state.
 */
void AbilityThread::ScheduleAbilityTransaction(const Want &want, const LifeCycleStateInfo &lifeCycleStateInfo)
{
    APP_LOGI("ScheduleAbilityTransaction begin: targeState = %{public}d, isNewWant = %{public}d",
        lifeCycleStateInfo.state,
        lifeCycleStateInfo.isNewWant);

    want.DumpInfo(0);

    if ((token_ == nullptr) || abilityImpl_ == nullptr) {
        APP_LOGE("ScheduleAbilityTransaction::failed");
        return;
    }
    auto task = [abilityThread = this, want, lifeCycleStateInfo]() {
        abilityThread->HandleAbilityTransaction(want, lifeCycleStateInfo);
    };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleAbilityTransaction abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleAbilityTransaction PostTask error");
    }
    APP_LOGI("ScheduleAbilityTransaction end");
}

/**
 * @description:  Provide operating system ConnectAbility information to the observer
 * @param  want Indicates the structure containing connect information about the ability.
 */
void AbilityThread::ScheduleConnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::ScheduleConnectAbility begin");
    auto task = [abilityThread = this, want]() { abilityThread->HandleConnectAbility(want); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleConnectAbility abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleConnectAbility PostTask error");
    }
    APP_LOGI("AbilityThread::ScheduleConnectAbility end");
}

/**
 * @description: Provide operating system ConnectAbility information to the observer
 * @return  None
 */
void AbilityThread::ScheduleDisconnectAbility(const Want &want)
{
    APP_LOGI("AbilityThread::ScheduleDisconnectAbility begin");
    auto task = [abilityThread = this, want]() { abilityThread->HandleDisconnectAbility(want); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleDisconnectAbility abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleDisconnectAbility PostTask error");
    }
    APP_LOGI("AbilityThread::ScheduleDisconnectAbility end");
}

/**
 * @description: Provide operating system CommandAbility information to the observer
 *
 * @param want The Want object to command to.
 *
 * * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
 * destroyed, and the value false indicates a normal startup.
 *
 * @param startId Indicates the number of times the Service ability has been started. The startId is incremented by 1
 * every time the ability is started. For example, if the ability has been started for six times, the value of startId
 * is 6.
 */
void AbilityThread::ScheduleCommandAbility(const Want &want, bool restart, int startId)
{
    APP_LOGI("AbilityThread::ScheduleCommandAbility begin");
    auto task = [abilityThread = this, want, restart, startId]() {
        abilityThread->HandleCommandAbility(want, restart, startId);
    };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleCommandAbility abilityHandler_ == nullptr");
        return;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleCommandAbility PostTask error");
    }
    APP_LOGI("AbilityThread::ScheduleCommandAbility end");
}

/**
 * @brief Send the result code and data to be returned by this Page ability to the caller.
 * When a Page ability is destroyed, the caller overrides the AbilitySlice#onAbilityResult(int, int, Want) method to
 * receive the result set in the current method. This method can be called only after the ability has been initialized.
 *
 * @param requestCode Indicates the request code for send.
 * @param resultCode Indicates the result code returned after the ability is destroyed. You can define the result code
 * to identify an error.
 * @param want Indicates the data returned after the ability is destroyed. You can define the data returned. This
 * parameter can be null.
 */
void AbilityThread::SendResult(int requestCode, int resultCode, const Want &want)
{
    APP_LOGI("AbilityThread::SendResult begin");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::SendResult abilityImpl_ == nullptr");
        return;
    }

    if (requestCode != -1) {
        APP_LOGI("AbilityThread::SendResult before abilityImpl_->SendResult");
        abilityImpl_->SendResult(requestCode, resultCode, want);
        APP_LOGI("AbilityThread::SendResult after abilityImpl_->SendResult");
    }
    APP_LOGI("AbilityThread::SendResult end");
}

/**
 * @brief Obtains the MIME types of files supported.
 *
 * @param uri Indicates the path of the files to obtain.
 * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
 *
 * @return Returns the matched MIME types. If there is no match, null is returned.
 */
std::vector<std::string> AbilityThread::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    APP_LOGI("AbilityThread::GetFileTypes begin");
    std::vector<std::string> types;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::GetFileTypes abilityImpl_ is nullptr");
        return types;
    }

    APP_LOGI("AbilityThread::GetFileTypes before abilityImpl_->GetFileTypes");
    types = abilityImpl_->GetFileTypes(uri, mimeTypeFilter);
    APP_LOGI("AbilityThread::GetFileTypes after abilityImpl_->GetFileTypes");
    APP_LOGI("AbilityThread::GetFileTypes end");
    return types;
}

/**
 * @brief Opens a file in a specified remote path.
 *
 * @param uri Indicates the path of the file to open.
 * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
 * (erasing whatever data is currently in the file), "wt" for write access that truncates any existing file,
 * "wa" for write-only access to append to any existing data, "rw" for read and write access on any existing data,
 *  or "rwt" for read and write access that truncates any existing file.
 *
 * @return Returns the file descriptor.
 */
int AbilityThread::OpenFile(const Uri &uri, const std::string &mode)
{
    APP_LOGI("AbilityThread::OpenFile begin");
    int fd = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::OpenFile abilityImpl_ is nullptr");
        return fd;
    }

    APP_LOGI("AbilityThread::OpenFile before abilityImpl_->OpenFile");
    fd = abilityImpl_->OpenFile(uri, mode);
    APP_LOGI("AbilityThread::OpenFile after abilityImpl_->OpenFile");
    APP_LOGI("AbilityThread::OpenFile end");
    return fd;
}

/**
 * @brief This is like openFile, open a file that need to be able to return sub-sections of files，often assets
 * inside of their .hap.
 *
 * @param uri Indicates the path of the file to open.
 * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
 * (erasing whatever data is currently in the file), "wt" for write access that truncates any existing file,
 * "wa" for write-only access to append to any existing data, "rw" for read and write access on any existing
 * data, or "rwt" for read and write access that truncates any existing file.
 *
 * @return Returns the RawFileDescriptor object containing file descriptor.
 */
int AbilityThread::OpenRawFile(const Uri &uri, const std::string &mode)
{
    APP_LOGI("AbilityThread::OpenRawFile begin");
    int fd = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::OpenRawFile abilityImpl_ is nullptr");
        return fd;
    }

    APP_LOGI("AbilityThread::OpenRawFile before abilityImpl_->OpenRawFile");
    fd = abilityImpl_->OpenRawFile(uri, mode);
    APP_LOGI("AbilityThread::OpenRawFile after abilityImpl_->OpenRawFile");
    APP_LOGI("AbilityThread::OpenRawFile end");
    return fd;
}

/**
 * @brief Inserts a single data record into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
 *
 * @return Returns the index of the inserted data record.
 */
int AbilityThread::Insert(const Uri &uri, const NativeRdb::ValuesBucket &value)
{
    APP_LOGI("AbilityThread::Insert begin");
    int index = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Insert abilityImpl_ is nullptr");
        return index;
    }

    APP_LOGI("AbilityThread::Insert before abilityImpl_->Insert");
    index = abilityImpl_->Insert(uri, value);
    APP_LOGI("AbilityThread::Insert after abilityImpl_->Insert");
    APP_LOGI("AbilityThread::Insert end");
    return index;
}

/**
 * @brief Updates data records in the database.
 *
 * @param uri Indicates the path of data to update.
 * @param value Indicates the data to update. This parameter can be null.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the number of data records updated.
 */
int AbilityThread::Update(
    const Uri &uri, const NativeRdb::ValuesBucket &value, const NativeRdb::DataAbilityPredicates &predicates)
{
    APP_LOGI("AbilityThread::Update begin");
    int index = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Update abilityImpl_ is nullptr");
        return index;
    }

    APP_LOGI("AbilityThread::Update before abilityImpl_->Update");
    index = abilityImpl_->Update(uri, value, predicates);
    APP_LOGI("AbilityThread::Update after abilityImpl_->Update");
    APP_LOGI("AbilityThread::Update end");
    return index;
}

/**
 * @brief Deletes one or more data records from the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the number of data records deleted.
 */
int AbilityThread::Delete(const Uri &uri, const NativeRdb::DataAbilityPredicates &predicates)
{
    APP_LOGI("AbilityThread::Delete begin");
    int index = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Delete abilityImpl_ is nullptr");
        return index;
    }
    APP_LOGI("AbilityThread::Delete before abilityImpl_->Delete");
    index = abilityImpl_->Delete(uri, predicates);
    APP_LOGI("AbilityThread::Delete after abilityImpl_->Delete");
    APP_LOGI("AbilityThread::Delete end");
    return index;
}

/**
 * @brief Deletes one or more data records from the database.
 *
 * @param uri Indicates the path of data to query.
 * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the query result.
 */
std::shared_ptr<NativeRdb::AbsSharedResultSet> AbilityThread::Query(
    const Uri &uri, std::vector<std::string> &columns, const NativeRdb::DataAbilityPredicates &predicates)
{
    APP_LOGI("AbilityThread::Query begin");
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Query abilityImpl_ is nullptr");
        return resultSet;
    }

    APP_LOGI("AbilityThread::Query before abilityImpl_->Query");
    resultSet = abilityImpl_->Query(uri, columns, predicates);
    APP_LOGI("AbilityThread::Query after abilityImpl_->Query");
    APP_LOGI("AbilityThread::Query end");
    return resultSet;
}

/**
 * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
 * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
 *
 * @param uri Indicates the URI of the data.
 *
 * @return Returns the MIME type that matches the data specified by uri.
 */
std::string AbilityThread::GetType(const Uri &uri)
{
    APP_LOGI("AbilityThread::GetType begin");
    std::string type;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::GetType abilityImpl_ is nullptr");
        return type;
    }

    APP_LOGI("AbilityThread::GetType before abilityImpl_->GetType");
    type = abilityImpl_->GetType(uri);
    APP_LOGI("AbilityThread::GetType after abilityImpl_->GetType");
    APP_LOGI("AbilityThread::GetType end");
    return type;
}

/**
 * @brief Reloads data in the database.
 *
 * @param uri Indicates the position where the data is to reload. This parameter is mandatory.
 * @param extras Indicates the PacMap object containing the additional parameters to be passed in this call. This
 * parameter can be null. If a custom Sequenceable object is put in the PacMap object and will be transferred across
 * processes, you must call BasePacMap.setClassLoader(ClassLoader) to set a class loader for the custom object.
 *
 * @return Returns true if the data is successfully reloaded; returns false otherwise.
 */
bool AbilityThread::Reload(const Uri &uri, const PacMap &extras)
{
    APP_LOGI("AbilityThread::Reload begin");
    bool ret = false;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::Reload abilityImpl_ is nullptr");
        return ret;
    }
    APP_LOGI("AbilityThread::Reload before abilityImpl_->Reload");
    ret = abilityImpl_->Reload(uri, extras);
    APP_LOGI("AbilityThread::Reload after abilityImpl_->Reload");
    APP_LOGI("AbilityThread::Reload end");
    return ret;
}

/**
 * @brief Inserts multiple data records into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param values Indicates the data records to insert.
 *
 * @return Returns the number of data records inserted.
 */
int AbilityThread::BatchInsert(const Uri &uri, const std::vector<NativeRdb::ValuesBucket> &values)
{
    APP_LOGI("AbilityThread::BatchInsert begin");
    int ret = -1;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::BatchInsert​ abilityImpl_ is nullptr");
        return ret;
    }

    APP_LOGI("AbilityThread::BatchInsert before abilityImpl_->BatchInsert");
    ret = abilityImpl_->BatchInsert(uri, values);
    APP_LOGI("AbilityThread::BatchInsert after abilityImpl_->BatchInsert");
    APP_LOGI("AbilityThread::BatchInsert end");
    return ret;
}

void AbilityThread::NotifyMultiWinModeChanged(int32_t winModeKey, bool flag)
{
    APP_LOGI("NotifyMultiWinModeChanged.key:%{public}d,flag:%{public}d", winModeKey, flag);
    sptr<Window> window = currentAbility_->GetWindow();
    if (window == nullptr) {
        APP_LOGE("NotifyMultiWinModeChanged window == nullptr");
        return;
    }

    return;
}

void AbilityThread::NotifyTopActiveAbilityChanged(bool flag)
{
    APP_LOGI("NotifyTopActiveAbilityChanged,flag:%{public}d", flag);
    sptr<Window> window = currentAbility_->GetWindow();
    if (window == nullptr) {
        APP_LOGE("NotifyMultiWinModeChanged window == nullptr");
        return;
    }
    if (flag) {
        window->SwitchTop();
    }
    return;
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 * @param mainRunner The runner which main_thread holds.
 */
void AbilityThread::AbilityThreadMain(std::shared_ptr<OHOSApplication> &application,
    const std::shared_ptr<AbilityLocalRecord> &abilityRecord, const std::shared_ptr<EventRunner> &mainRunner)
{
    APP_LOGI("AbilityThread::AbilityThreadMain begin");
    sptr<AbilityThread> thread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    if (thread == nullptr) {
        APP_LOGE("AbilityThread::AbilityThreadMain failed,thread  is nullptr");
        return;
    }
    thread->Attach(application, abilityRecord, mainRunner);
    APP_LOGI("AbilityThread::AbilityThreadMain end");
}

/**
 * @description: Attach The ability thread to the main process.
 * @param application Indicates the main process.
 * @param abilityRecord Indicates the abilityRecord.
 */
void AbilityThread::AbilityThreadMain(
    std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &abilityRecord)
{
    APP_LOGI("AbilityThread::AbilityThreadMain begin");
    sptr<AbilityThread> thread = sptr<AbilityThread>(new (std::nothrow) AbilityThread());
    if (thread == nullptr) {
        APP_LOGE("AbilityThread::AbilityThreadMain failed,thread  is nullptr");
        return;
    }
    thread->Attach(application, abilityRecord);
    APP_LOGI("AbilityThread::AbilityThreadMain end");
}
/**
 * @brief Converts the given uri that refer to the Data ability into a normalized URI. A normalized URI can be used
 * across devices, persisted, backed up, and restored. It can refer to the same item in the Data ability even if the
 * context has changed. If you implement URI normalization for a Data ability, you must also implement
 * denormalizeUri(ohos.utils.net.Uri) to enable URI denormalization. After this feature is enabled, URIs passed to any
 * method that is called on the Data ability must require normalization verification and denormalization. The default
 * implementation of this method returns null, indicating that this Data ability does not support URI normalization.
 *
 * @param uri Indicates the Uri object to normalize.
 *
 * @return Returns the normalized Uri object if the Data ability supports URI normalization; returns null otherwise.
 */
Uri AbilityThread::NormalizeUri(const Uri &uri)
{
    APP_LOGI("AbilityThread::NormalizeUri begin");
    Uri urivalue("");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("DataAbilityHelper::normalizeUri failed dataAbility == nullptr");
        return urivalue;
    }

    APP_LOGI("AbilityThread::NormalizeUri before abilityImpl_->NormalizeUri");
    urivalue = abilityImpl_->NormalizeUri(uri);
    APP_LOGI("AbilityThread::NormalizeUri after abilityImpl_->NormalizeUri");
    APP_LOGI("AbilityThread::NormalizeUri end");
    return urivalue;
}

/**
 * @brief Converts the given normalized uri generated by normalizeUri(ohos.utils.net.Uri) into a denormalized one.
 * The default implementation of this method returns the original URI passed to it.
 *
 * @param uri uri Indicates the Uri object to denormalize.
 *
 * @return Returns the denormalized Uri object if the denormalization is successful; returns the original Uri passed to
 * this method if there is nothing to do; returns null if the data identified by the original Uri cannot be found in the
 * current environment.
 */
Uri AbilityThread::DenormalizeUri(const Uri &uri)
{
    APP_LOGI("AbilityThread::DenormalizeUri begin");
    Uri urivalue("");
    if (abilityImpl_ == nullptr) {
        APP_LOGE("DataAbilityHelper::denormalizeUri failed dataAbility == nullptr");
        return urivalue;
    }

    APP_LOGI("AbilityThread::DenormalizeUri before abilityImpl_->DenormalizeUri");
    urivalue = abilityImpl_->DenormalizeUri(uri);
    APP_LOGI("AbilityThread::DenormalizeUri after abilityImpl_->DenormalizeUri");
    APP_LOGI("AbilityThread::DenormalizeUri end");
    return urivalue;
}

/**
 * @brief Registers an observer to DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the IDataAbilityObserver object.
 */
bool AbilityThread::HandleRegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        APP_LOGE("%{public}s obsMgrClient is nullptr", __func__);
        return false;
    }

    ErrCode ret = obsMgrClient->RegisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        APP_LOGE("%{public}s obsMgrClient->RegisterObserver error return %{public}d", __func__, ret);
        return false;
    }
    return true;
}

/**
 * @brief Deregisters an observer used for DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the IDataAbilityObserver object.
 */
bool AbilityThread::HandleUnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        APP_LOGE("%{public}s obsMgrClient is nullptr", __func__);
        return false;
    }

    ErrCode ret = obsMgrClient->UnregisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        APP_LOGE("%{public}s obsMgrClient->UnregisterObserver error return %{public}d", __func__, ret);
        return false;
    }
    return true;
}

/**
 * @brief Notifies the registered observers of a change to the data resource specified by Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 */
bool AbilityThread::HandleNotifyChange(const Uri &uri)
{
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        APP_LOGE("%{public}s obsMgrClient is nullptr", __func__);
        return false;
    }

    ErrCode ret = obsMgrClient->NotifyChange(uri);
    if (ret != ERR_OK) {
        APP_LOGE("%{public}s obsMgrClient->NotifyChange error return %{public}d", __func__, ret);
        return false;
    }
    return true;
}

/**
 * @brief Access authority verification.
 *
 * @return Returns true on success, others on failure.
 */
bool AbilityThread::CheckObsPermission()
{
    APP_LOGI("%{public}s CheckObsPermission() run Permission Checkout", __func__);
    return true;
}

/**
 * @brief Registers an observer to DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the IDataAbilityObserver object.
 */
bool AbilityThread::ScheduleRegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    APP_LOGI("%{public}s called", __func__);

    if (!CheckObsPermission()) {
        APP_LOGE("%{public}s CheckObsPermission() return false", __func__);
        return false;
    }

    auto task = [abilityThread = this, uri, dataObserver]() {
        abilityThread->HandleRegisterObserver(uri, dataObserver);
    };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleRegisterObserver abilityHandler_ == nullptr");
        return false;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleRegisterObserver PostTask error");
    }
    return ret;
}

/**
 * @brief Deregisters an observer used for DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the IDataAbilityObserver object.
 */
bool AbilityThread::ScheduleUnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    APP_LOGI("%{public}s called", __func__);

    if (!CheckObsPermission()) {
        APP_LOGE("%{public}s CheckObsPermission() return false", __func__);
        return false;
    }

    auto task = [abilityThread = this, uri, dataObserver]() {
        abilityThread->HandleUnregisterObserver(uri, dataObserver);
    };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleUnregisterObserver abilityHandler_ == nullptr");
        return false;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleUnregisterObserver PostTask error");
    }
    return ret;
}

/**
 * @brief Notifies the registered observers of a change to the data resource specified by Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 */
bool AbilityThread::ScheduleNotifyChange(const Uri &uri)
{
    APP_LOGI("%{public}s called", __func__);

    if (!CheckObsPermission()) {
        APP_LOGE("%{public}s CheckObsPermission() return false", __func__);
        return false;
    }

    auto task = [abilityThread = this, uri]() { abilityThread->HandleNotifyChange(uri); };

    if (abilityHandler_ == nullptr) {
        APP_LOGE("AbilityThread::ScheduleNotifyChange abilityHandler_ == nullptr");
        return false;
    }

    bool ret = abilityHandler_->PostTask(task);
    if (!ret) {
        APP_LOGE("AbilityThread::ScheduleNotifyChange PostTask error");
    }
    return ret;
}

std::vector<std::shared_ptr<DataAbilityResult>> AbilityThread::ExecuteBatch(
    const std::vector<std::shared_ptr<DataAbilityOperation>> &operations)
{

    APP_LOGI("AbilityThread::ExecuteBatch start");
    std::vector<std::shared_ptr<DataAbilityResult>> results;
    if (abilityImpl_ == nullptr) {
        APP_LOGE("AbilityThread::ExecuteBatch abilityImpl_ is nullptr");
        results.clear();
        return results;
    }
    APP_LOGI("AbilityThread::ExecuteBatch before abilityImpl_->ExecuteBatch");
    results = abilityImpl_->ExecuteBatch(operations);
    APP_LOGI("AbilityThread::ExecuteBatch after abilityImpl_->ExecuteBatch");
    APP_LOGI("AbilityThread::ExecuteBatch end");
    return results;
}

}  // namespace AppExecFwk
}  // namespace OHOS