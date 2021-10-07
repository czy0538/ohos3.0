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

#ifndef FRAMEWORKS_WM_INCLUDE_SUBWINDOW_NORMAL_IMPL_H
#define FRAMEWORKS_WM_INCLUDE_SUBWINDOW_NORMAL_IMPL_H

#include <subwindow.h>

#include <mutex>

#include "window_attribute.h"
#include "wl_subsurface.h"
#include "wl_surface.h"
#include "window_manager_impl.h"

namespace OHOS {
class SubwindowNormalImpl : public Subwindow, public IBufferConsumerListenerClazz {
public:
    static WMError Create(sptr<Subwindow> &subwindow,
                          const sptr<Window> &window,
                          const sptr<SubwindowOption> &option);

    virtual sptr<Surface> GetSurface() const override;

    virtual WMError Move(int32_t x, int32_t y) override;
    virtual WMError Resize(uint32_t width, uint32_t height) override;
    virtual WMError Destroy() override;

    virtual void OnPositionChange(WindowPositionChangeFunc func) override;
    virtual void OnSizeChange(WindowSizeChangeFunc func) override;

private:
    SubwindowNormalImpl() = default;
    virtual ~SubwindowNormalImpl() = default;

    virtual void OnBufferAvailable() override;

    static WMError CheckAndNew(sptr<SubwindowNormalImpl> &si,
                               const sptr<Window> &window,
                               const sptr<SubwindowOption> &option,
                               sptr<WlSurface> &parent);

    static WMError CreateWlSurface(sptr<SubwindowNormalImpl> &si,
                               const sptr<WlSurface> &parentWlSurface);

    static WMError CreateConsumerSurface(sptr<SubwindowNormalImpl> &si,
                                         const sptr<SubwindowOption> &option);

    // base attribute
    std::mutex publicMutex;
    bool isDestroy = false;
    WindowAttribute attr;

    // functional member
    sptr<Surface> csurface = nullptr;
    sptr<Surface> psurface = nullptr;
    sptr<WlSubsurface> wlSubsurface = nullptr;
    sptr<WlSurface> wlSurface = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_SUBWINDOW_NORMAL_IMPL_H
