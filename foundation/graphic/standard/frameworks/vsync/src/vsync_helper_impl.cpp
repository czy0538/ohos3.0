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

#include "vsync_helper_impl.h"

#include <algorithm>
#include <chrono>
#include <list>
#include <sys/time.h>
#include <unistd.h>

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "static_call.h"
#include "vsync_log.h"

namespace OHOS {
namespace Vsync {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncHelperImpl" };
std::list<std::shared_ptr<AppExecFwk::EventHandler>> g_handlers;
std::mutex g_handlersMutex;

int64_t GetNowTime()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}
}

sptr<VsyncClient> VsyncClient::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new VsyncClient();
        }
    }
    return instance;
}

VsyncError VsyncClient::InitService()
{
    std::lock_guard<std::mutex> lock(serviceMutex_);
    if (service_ == nullptr) {
        auto sam = StaticCall::GetInstance()->GetSystemAbilityManager();
        if (sam == nullptr) {
            VLOG_FAILURE_RET(VSYNC_ERROR_SAMGR);
        }

        auto remoteObject = StaticCall::GetInstance()->GetSystemAbility(sam, VSYNC_MANAGER_ID);
        if (remoteObject == nullptr) {
            VLOG_FAILURE_RET(VSYNC_ERROR_SERVICE_NOT_FOUND);
        }

        sptr<IRemoteObject::DeathRecipient> deathRecipient = new VsyncManagerDeathRecipient();
        if (remoteObject->IsProxyObject() == true && remoteObject->AddDeathRecipient(deathRecipient) == false) {
            VLOGW("Failed to add death recipient");
        }

        if (service_ == nullptr) {
            service_ = StaticCall::GetInstance()->GetCast(remoteObject);
        }

        if (service_ == nullptr) {
            VLOG_FAILURE_RET(VSYNC_ERROR_PROXY_NOT_INCLUDE);
        }
        VLOG_SUCCESS("service_ = iface_cast");
    }
    return VSYNC_ERROR_OK;
}

VsyncError VsyncClient::InitVsyncFrequency()
{
    if (vsyncFrequency_ == 0) {
        VsyncError vret;
        {
            std::lock_guard<std::mutex> lock(serviceMutex_);
            vret = StaticCall::GetInstance()->GetVsyncFrequency(service_, vsyncFrequency_);
            if (vret == VSYNC_ERROR_BINDER_ERROR) {
                service_ = nullptr;
                listener_ = nullptr;
            }
        }
        if (vret != VSYNC_ERROR_OK) {
            VLOG_FAILURE_RET(vret);
        }
        if (vsyncFrequency_ == 0) {
            VLOG_FAILURE_RET(VSYNC_ERROR_INNER);
        }
        VLOG_SUCCESS("Get Frequency: %{public}u", vsyncFrequency_);
    }
    return VSYNC_ERROR_OK;
}

VsyncError VsyncClient::Init(bool restart)
{
    if (restart == true) {
        std::lock_guard<std::mutex> lock(serviceMutex_);
        service_ = nullptr;
        listener_ = nullptr;
    }

    while (true) {
        VsyncError vret;
        if (service_ == nullptr) {
            vret = InitService();
            if (vret == VSYNC_ERROR_SERVICE_NOT_FOUND && restart == true) {
                constexpr int sleepTime = 5 * 1000;
                usleep(sleepTime);
                continue;
            }
            if (vret != VSYNC_ERROR_OK) {
                return vret;
            }
        }

        vret = InitVsyncFrequency();
        if (vret == VSYNC_ERROR_BINDER_ERROR) {
            restart = true;
            continue;
        } else if (vret != VSYNC_ERROR_OK) {
            return vret;
        }

        if (listener_ == nullptr) {
            listener_ = new VsyncCallback();
            {
                std::lock_guard<std::mutex> lock(serviceMutex_);
                vret = StaticCall::GetInstance()->ListenVsync(service_, listener_);
                if (vret == VSYNC_ERROR_BINDER_ERROR) {
                    service_ = nullptr;
                    listener_ = nullptr;
                    restart = true;
                    continue;
                }
            }
            if (vret == VSYNC_ERROR_OK) {
                VLOG_SUCCESS("ListenVsync");
            } else {
                VLOG_FAILURE_API(ListenVsync, vret);
                return vret;
            }
        }
        break;
    }
    return VSYNC_ERROR_OK;
}

VsyncError VsyncClient::RequestFrameCallback(const struct FrameCallback &cb)
{
    VsyncError ret = Init();
    if (ret != VSYNC_ERROR_OK) {
        return ret;
    }

    if (cb.callback_ == nullptr) {
        VLOG_FAILURE_RET(VSYNC_ERROR_NULLPTR);
    }

    uint32_t frequency = cb.frequency_;
    if (frequency == 0) {
        frequency = vsyncFrequency_;
    }

    if (vsyncFrequency_ % frequency != 0) {
        VLOGW("cb.frequency_ is invalid arguments");
        VLOG_FAILURE_RET(VSYNC_ERROR_INVALID_ARGUMENTS);
    }

    int64_t delayTime = cb.timestamp_;
    uint32_t vsyncID = lastID_ + vsyncFrequency_ / frequency;
    struct VsyncElement ele = {
        .callback_ = cb.callback_,
        .activeTime_ = cb.timestamp_ + GetNowTime(),
        .userdata_ = cb.userdata_,
    };

    {
        std::lock_guard<std::mutex> lockGuard(callbacksMapMutex_);
        callbacksMap_[vsyncID].push(ele);
    }

    VLOG_SUCCESS("RequestFrameCallback time: " VPUBI64 ", id: %{public}u", delayTime, vsyncID);
    return VSYNC_ERROR_OK;
}

VsyncError VsyncClient::GetSupportedVsyncFrequencys(std::vector<uint32_t>& freqs)
{
    VsyncError ret = Init();
    if (ret != VSYNC_ERROR_OK) {
        return ret;
    }

    freqs.clear();
    for (uint32_t i = 1; i * i <= vsyncFrequency_; i++) {
        if (vsyncFrequency_ % i == 0) {
            if (i * i != vsyncFrequency_) {
                freqs.push_back(vsyncFrequency_ / i);
            }
            freqs.push_back(i);
        }
    }

    auto compare = [](int a, int b) { return a > b; };
    std::sort(freqs.begin(), freqs.end(), compare);
    return VSYNC_ERROR_OK;
}

void VsyncClient::DispatchFrameCallback(int64_t timestamp)
{
    std::lock_guard<std::mutex> lock(g_handlersMutex);
    if (g_handlers.empty() == true) {
        VLOGD("g_handlers is empty, cannot exec");
        return;
    }

    auto func = std::bind(&VsyncClient::DispatchMain, this, timestamp);
    g_handlers.front()->PostTask(func, "VsyncClient::DispatchFrameCallback");
}

void VsyncClient::DispatchMain(int64_t timestamp)
{
    uint32_t id = ++lastID_;
    int64_t now = GetNowTime();

    std::list<struct VsyncElement> vsyncElements;
    {
        std::lock_guard<std::mutex> lockGuard(callbacksMapMutex_);
        for (auto &[vid, eles] : callbacksMap_) {
            if (vid > id) {
                continue;
            }

            while (eles.empty() != true) {
                if (eles.top().activeTime_ <= now) {
                    vsyncElements.push_back(eles.top());
                    eles.pop();
                } else {
                    break;
                }
            }
        }
    }

    if (vsyncElements.size()) {
        VLOGI("DispatchFrameCallback id: %{public}u, time: " VPUBI64 ", timestamp: " VPUBI64,
                id, now, timestamp);
    }

    for (const auto &ele : vsyncElements) {
        ele.callback_(timestamp, ele.userdata_);
    }

    {
        std::lock_guard<std::mutex> lockGuard(callbacksMapMutex_);
        bool haveEmpty;
        do {
            haveEmpty = false;
            for (auto it = callbacksMap_.begin(); it != callbacksMap_.end(); it++) {
                if (it->second.empty() == true) {
                    callbacksMap_.erase(it);
                    haveEmpty = true;
                    break;
                }
            }
        } while (haveEmpty == true);
    }
}

sptr<VsyncHelperImpl> VsyncHelperImpl::Current()
{
    if (currentHelper_ == nullptr) {
        auto handler = StaticCall::GetInstance()->Current();
        if (handler == nullptr) {
            VLOG_FAILURE("AppExecFwk::EventHandler::Current() return nullptr");
            return nullptr;
        }
        currentHelper_ = new VsyncHelperImpl(handler);
        VLOG_SUCCESS("new VsyncHelperImpl");
    }
    return currentHelper_;
}

VsyncHelperImpl::VsyncHelperImpl(std::shared_ptr<AppExecFwk::EventHandler>& handler)
{
    handler_ = handler;
    std::lock_guard<std::mutex> lock(g_handlersMutex);
    g_handlers.push_back(handler_);
}

VsyncHelperImpl::~VsyncHelperImpl()
{
    std::lock_guard<std::mutex> lock(g_handlersMutex);
    for (auto it = g_handlers.begin(); it != g_handlers.end(); it++) {
        if (handler_ == *it) {
            g_handlers.erase(it);
            break;
        }
    }
}

VsyncError VsyncHelperImpl::RequestFrameCallback(const struct FrameCallback &cb)
{
    return VsyncClient::GetInstance()->RequestFrameCallback(cb);
}

VsyncError VsyncHelperImpl::GetSupportedVsyncFrequencys(std::vector<uint32_t>& freqs)
{
    return VsyncClient::GetInstance()->GetSupportedVsyncFrequencys(freqs);
}

VsyncError VsyncCallback::OnVsync(int64_t timestamp)
{
    VsyncClient::GetInstance()->DispatchFrameCallback(timestamp);
    return VSYNC_ERROR_OK;
}

void VsyncManagerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    VLOGD("IS DEAD");
    VsyncClient::GetInstance()->Init(true);
}
} // namespace Vsync
} // namespace OHOS
