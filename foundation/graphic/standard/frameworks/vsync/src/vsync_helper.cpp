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

#include <vsync_helper.h>

#include "vsync_helper_impl.h"

namespace OHOS {
sptr<VsyncHelper> VsyncHelper::Current()
{
    return Vsync::VsyncHelperImpl::Current();
}

sptr<VsyncHelper> VsyncHelper::FromHandler(std::shared_ptr<AppExecFwk::EventHandler>& handler)
{
    sptr<VsyncHelper> helper = new Vsync::VsyncHelperImpl(handler);
    return helper;
}
} // namespace OHOS
