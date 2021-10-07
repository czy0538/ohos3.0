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

#include "wmclient_native_test_9.h"

#include <cstdio>
#include <securec.h>

#include <display_type.h>
#include <window_manager.h>

#include "native_test_class.h"
#include "util.h"
#include "wmclient_native_test_1.h"

using namespace OHOS;

namespace {
class WMClientNativeTest9 : public WMClientNativeTest1 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "resize";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 9;
        return id;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        WMClientNativeTest1::Run(argc, argv);
        std::vector<struct WMDisplayInfo> displays;
        WindowManager::GetInstance()->GetDisplays(displays);
        if (displays.size() <= 0) {
            printf("GetDisplays return no screen\n");
            ExitTest();
            return;
        }
        maxWidth = displays[0].width;
        maxHeight = displays[0].height;
        AfterRun();
    }

    void AfterRun()
    {
        constexpr int32_t step = 37;
        constexpr int32_t minWidthHeight = 100;
        static int32_t width = maxWidth;
        static int32_t height = maxHeight;
        width -= step;
        height -= step;
        if (width < minWidthHeight) {
            width += maxWidth;
        }
        if (height < minWidthHeight) {
            height += maxHeight;
        }
        window->Resize(width, height);

        constexpr uint32_t delayTime = 100;
        PostTask(std::bind(&WMClientNativeTest9::AfterRun, this), delayTime);
    }

private:
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
} g_autoload;
} // namespace
