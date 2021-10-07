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

#include "vsync_manager_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "return_value_tester.h"
#include "vsync_log.h"

namespace OHOS {
namespace Vsync {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncManagerProxy" };
}

VsyncManagerProxy::VsyncManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IVsyncManager>(impl)
{
}

VsyncError VsyncManagerProxy::ListenVsync(sptr<IVsyncCallback>& cb)
{
    if (cb == nullptr) {
        VLOG_FAILURE_NO(VSYNC_ERROR_NULLPTR);
        return VSYNC_ERROR_NULLPTR;
    }

    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    auto reval = arg.WriteInterfaceToken(GetDescriptor());
    if (!ReturnValueTester::Get<bool>(reval)) {
        VLOGE("write interface token failed");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }

    arg.WriteRemoteObject(cb->AsObject());

    int result = Remote()->SendRequest(IVSYNC_MANAGER_LISTEN_VSYNC, arg, ret, opt);
    if (ReturnValueTester::Get<int>(result)) {
        VLOG_ERROR_API(result, SendRequest);
        return VSYNC_ERROR_BINDER_ERROR;
    }
    int res = ret.ReadInt32();
    VsyncError err = (VsyncError)ReturnValueTester::Get<int>(res);
    if (err != VSYNC_ERROR_OK) {
        VLOG_FAILURE_NO(err);
        return err;
    }

    return VSYNC_ERROR_OK;
}

VsyncError VsyncManagerProxy::GetVsyncFrequency(uint32_t &freq)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    auto reval = arg.WriteInterfaceToken(GetDescriptor());
    if (!ReturnValueTester::Get<bool>(reval)) {
        VLOGE("write interface token failed");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }

    int32_t result = Remote()->SendRequest(IVSYNC_MANAGER_GET_VSYNC_FREQUENCY, arg, ret, opt);
    if (ReturnValueTester::Get<int>(result)) {
        VLOG_ERROR_API(result, SendRequest);
        return VSYNC_ERROR_BINDER_ERROR;
    }

    int res = ret.ReadInt32();
    VsyncError err = (VsyncError)ReturnValueTester::Get<int>(res);
    if (err != VSYNC_ERROR_OK) {
        VLOG_FAILURE_NO(err);
        return err;
    }

    freq = ret.ReadUint32();
    return VSYNC_ERROR_OK;
}
} // namespace Vsync
} // namespace OHOS
