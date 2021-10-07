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

#include "window_manager_service_proxy.h"

#include "window_manager_hilog.h"

namespace OHOS {
using PromiseWMError = Promise<WMError>;
using PromiseWMSImageInfo = Promise<WMSImageInfo>;

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMServiceProxy"};
}

WindowManagerServiceProxy::WindowManagerServiceProxy(struct wms *wms, struct wl_display *display)
{
    this->wms = wms;
    this->display = display;
}

void WindowManagerServiceProxy::OnDisplayChange(uint32_t did,
    const char *name, wms_screen_status state, int32_t width, int32_t height)
{
    WMLOGFI("state: %{public}d, did: %{public}d, %{public}s(%{public}dx%{public}d)",
        state, did, name, width, height);

    auto match = [did](const auto &info) { return info.id == static_cast<int32_t>(did); };
    auto ret = std::find_if(displays.begin(), displays.end(), match);

    if (state == WMS_SCREEN_STATUS_ADD) {
        if (ret == displays.end()) {
            constexpr int32_t tmpVsyncFreq = 60;
            struct WMDisplayInfo info = {
                .id = did,
                .width = width,
                .height = height,
                .phyWidth = width,
                .phyHeight = height,
                .vsync = tmpVsyncFreq,
            };
            displays.push_back(info);
        }
        if (displayListener != nullptr) {
            displayListener->OnScreenPlugin(did);
        }
    }

    if (state == WMS_SCREEN_STATUS_REMOVE) {
        if (ret != displays.end()) {
            displays.erase(ret);
        }
        if (displayListener != nullptr) {
            displayListener->OnScreenPlugout(did);
        }
    }
}

void WindowManagerServiceProxy::OnDisplayModeChange(uint32_t mode)
{
    WMLOGFI("mode: %{public}u", mode);
    displayModes = mode;
}

void WindowManagerServiceProxy::OnReply(wms_error error)
{
    WMLOGFD("reply: %{public}d", error);
    if (promiseQueue.empty() == false) {
        promiseQueue.front()->Resolve(static_cast<WMError>(error));
        promiseQueue.pop();
    } else {
        WMLOGFW("OnReply promiseQueue is empty");
    }
}

void WindowManagerServiceProxy::OnScreenShot(wms_error reply,
    uint32_t did, int32_t fd, int32_t width, int32_t height,
    int32_t stride, uint32_t format, uint32_t timestampSec, uint32_t timestampNanoSec)
{
    WMLOGFI("reply: %{public}u, did: %{public}d, fd: %{public}d, %{public}dx%{public}d, stride: %{public}d, "
        "format: %{public}u, timestampSec: %{public}u, timestampNanoSec: %{public}u",
        reply, did, fd, width, height, stride, format, timestampSec, timestampNanoSec);

    {
        std::lock_guard<std::mutex> lock(screenShotPromisesMutex);
        if (screenShotPromises.find(did) == screenShotPromises.end()) {
            WMLOGFW("cannot found %{public}d 's promise", did);
            return;
        }

        if (screenShotPromises[did] == nullptr) {
            WMLOGFW("cannot resolve %{public}d screenshot promise, it's nullptr", did);
            return;
        }

        constexpr int64_t SEC_TO_MSEC = 1000;
        constexpr int64_t NSEC_TO_MSEC = 1000 * 1000;
        int64_t timestampMillSec = timestampSec * SEC_TO_MSEC + timestampNanoSec / NSEC_TO_MSEC;
        WMSImageInfo info = {
            .wret = static_cast<WMError>(reply),
            .fd = fd,
            .width = width,
            .height = height,
            .stride = stride,
            .format = format,
            .timestamp = timestampMillSec,
        };
        screenShotPromises[did]->Resolve(info);
        screenShotPromises.erase(did);
    }
}

void WindowManagerServiceProxy::OnWindowShot(wms_error reply,
    uint32_t wid, int32_t fd, int32_t width, int32_t height,
    int32_t stride, uint32_t format, uint32_t timestampSec, uint32_t timestampNanoSec)
{
    WMLOGFI("reply: %{public}u, wid: %{public}d, fd: %{public}d, %{public}dx%{public}d, stride: %{public}d, "
        "format: %{public}u, timestampSec: %{public}u, timestampNanoSec: %{public}u",
        reply, wid, fd, width, height, stride, format, timestampSec, timestampNanoSec);

    {
        std::lock_guard<std::mutex> lock(windowShotPromisesMutex);
        if (windowShotPromises.find(wid) == windowShotPromises.end()) {
            WMLOGFW("cannot found %{public}d 's promise", wid);
            return;
        }

        if (windowShotPromises[wid] == nullptr) {
            WMLOGFW("cannot resolve %{public}d windowshot promise, it's nullptr", wid);
            return;
        }

        constexpr int64_t SEC_TO_MSEC = 1000;
        constexpr int64_t NSEC_TO_MSEC = 1000 * 1000;
        int64_t timestampMillSec = timestampSec * SEC_TO_MSEC + timestampNanoSec / NSEC_TO_MSEC;
        WMSImageInfo info = {
            .wret = static_cast<WMError>(reply),
            .fd = fd,
            .width = width,
            .height = height,
            .stride = stride,
            .format = format,
            .timestamp = timestampMillSec,
        };
        windowShotPromises[wid]->Resolve(info);
        windowShotPromises.erase(wid);
    }
}

void WindowManagerServiceProxy::OnGlobalWindowStatus(uint32_t pid, uint32_t wid, uint32_t status)
{
    WMLOGFI("global window status: pid=%{public}u status=%{public}u wid=%{public}u", pid, status, wid);
    if (globalWindowChangeListener != nullptr) {
        if (status == WMS_WINDOW_STATUS_CREATED) {
            globalWindowChangeListener->OnWindowCreate(pid, wid);
        }
        if (status == WMS_WINDOW_STATUS_DESTROYED) {
            globalWindowChangeListener->OnWindowDestroy(pid, wid);
        }
    }
}

WMError WindowManagerServiceProxy::GetDisplays(std::vector<struct WMDisplayInfo> &displays)
{
    displays = this->displays;
    return WM_OK;
}

WMError WindowManagerServiceProxy::GetDisplayModes(uint32_t &displayModes)
{
    displayModes = this->displayModes;
    return WM_OK;
}

sptr<PromiseWMError> WindowManagerServiceProxy::SetDisplayMode(WMSDisplayMode modes)
{
    WMLOGFI("modes: %{public}d", modes);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_display_mode(wms, modes);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

WMError WindowManagerServiceProxy::SetDisplayDirection(WMSDisplayDirection direction)
{
    return WM_ERROR_NOT_SUPPORT;
}

WMError WindowManagerServiceProxy::OnDisplayDirectionChange(DisplayDirectionChangeFunc func)
{
    return WM_ERROR_NOT_SUPPORT;
}

sptr<Promise<WMError>> WindowManagerServiceProxy::OnWindowListChange(IWindowChangeListenerClazz *listener)
{
    sptr<PromiseWMError> promise = new PromiseWMError();
    globalWindowChangeListener = listener;
    promiseQueue.push(promise);
    wms_config_global_window_status(wms, (listener == nullptr) ? 0 : 1);
    wl_display_flush(display);
    return promise;
}

sptr<PromiseWMSImageInfo> WindowManagerServiceProxy::ShotScreen(int32_t did)
{
    WMLOGFI("did: %{public}d", did);
    sptr<PromiseWMSImageInfo> promise = new PromiseWMSImageInfo();
    {
        std::lock_guard<std::mutex> lock(screenShotPromisesMutex);
        screenShotPromises[did] = promise;
    }
    wms_screenshot(wms, did);
    wl_display_flush(display);
    return promise;
}

sptr<PromiseWMSImageInfo> WindowManagerServiceProxy::ShotWindow(int32_t wid)
{
    WMLOGFI("wid: %{public}d", wid);
    sptr<PromiseWMSImageInfo> promise = new PromiseWMSImageInfo();
    {
        std::lock_guard<std::mutex> lock(windowShotPromisesMutex);
        windowShotPromises[wid] = promise;
    }
    wms_windowshot(wms, wid);
    wl_display_flush(display);
    return promise;
}

WMError WindowManagerServiceProxy::AddDisplayChangeListener(IWindowManagerDisplayListenerClazz *listener)
{
    WMLOGFI("listener: %{public}s", (listener != nullptr) ? "Yes" : "No");
    displayListener = listener;
    return WM_OK;
}

sptr<PromiseWMError> WindowManagerServiceProxy::DestroyWindow(int32_t wid)
{
    WMLOGFI("wid: %{public}d", wid);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_destroy_window(wms, wid);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::SwitchTop(int32_t wid)
{
    WMLOGFI("wid: %{public}d", wid);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_top(wms, wid);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::Show(int32_t wid)
{
    WMLOGFI("wid: %{public}d", wid);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_visibility(wms, wid, WMS_VISIBILITY_TRUE);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::Hide(int32_t wid)
{
    WMLOGFI("wid: %{public}d", wid);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_visibility(wms, wid, WMS_VISIBILITY_FALSE);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::Move(int32_t wid, int32_t x, int32_t y)
{
    WMLOGFI("wid: %{public}d, (%{public}d, %{public}d)", wid, x, y);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_position(wms, wid, x, y);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::Resize(int32_t wid, uint32_t width, uint32_t height)
{
    WMLOGFI("wid: %{public}d, %{public}dx%{public}d", wid, width, height);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_size(wms, wid, width, height);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::ScaleTo(int32_t wid, uint32_t width, uint32_t height)
{
    WMLOGFI("wid: %{public}d, %{public}dx%{public}d", wid, width, height);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_scale(wms, wid, width, height);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::SetWindowType(int32_t wid, WindowType type)
{
    WMLOGFI("wid: %{public}d, type: %{public}d", wid, type);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_type(wms, wid, type);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}

sptr<PromiseWMError> WindowManagerServiceProxy::SetWindowMode(int32_t wid, WindowMode mode)
{
    WMLOGFI("wid: %{public}d, mode: %{public}d", wid, mode);
    sptr<PromiseWMError> ret = new PromiseWMError();
    promiseQueue.push(ret);
    wms_set_window_mode(wms, wid, mode);
    wms_commit_changes(wms);
    wl_display_flush(display);
    return ret;
}
} // namespace OHOS
