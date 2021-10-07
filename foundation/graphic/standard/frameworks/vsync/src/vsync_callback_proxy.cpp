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

#include "vsync_callback_proxy.h"

#include "return_value_tester.h"
#include "vsync_log.h"

namespace OHOS {
namespace Vsync {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncCallbackProxy" };
}

VsyncCallbackProxy::VsyncCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IVsyncCallback>(impl)
{
}

VsyncError VsyncCallbackProxy::OnVsync(int64_t timestamp)
{
    MessageOption opt;
    MessageParcel arg;
    MessageParcel ret;

    auto reval = arg.WriteInterfaceToken(GetDescriptor());
    if (!ReturnValueTester::Get<bool>(reval)) {
        VLOGE("write interface token failed");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }

    bool retval = arg.WriteInt64(timestamp);
    if (!ReturnValueTester::Get<bool>(retval)) {
        VLOGE("arg.WriteInt64 failed");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }

    int res = Remote()->SendRequest(IVSYNC_CALLBACK_ON_VSYNC, arg, ret, opt);
    if (ReturnValueTester::Get<int>(res)) {
        VLOG_ERROR_API(res, SendRequest);
        return VSYNC_ERROR_BINDER_ERROR;
    }

    int result = ret.ReadInt32();
    VsyncError err = (VsyncError)ReturnValueTester::Get<int>(result);
    if (err != VSYNC_ERROR_OK) {
        VLOG_FAILURE_NO(err);
        return err;
    }
    return VSYNC_ERROR_OK;
}
} // namespace Vsync
} // namespace OHOS
