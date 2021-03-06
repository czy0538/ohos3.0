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

#ifndef FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_ENGINE_ACE_DIFF_ENGINE_H
#define FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_ENGINE_ACE_DIFF_ENGINE_H

#include <unordered_map>

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine_instance.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_group_js_bridge.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include "native_engine/impl/quickjs/quickjs_native_engine.h"
#endif

namespace OHOS::Ace::Framework {

class QJSDeclarativeEngine : public JsEngine {
public:
    explicit QJSDeclarativeEngine(int32_t instanceId) : instanceId_(instanceId) {}
    ~QJSDeclarativeEngine() override;

    bool Initialize(const RefPtr<FrontendDelegate>& delegate) override;

    void LoadJs(const std::string& url, const RefPtr<JsAcePage>& page, bool isMainPage) override;

    // Update running page
    void UpdateRunningPage(const RefPtr<JsAcePage>& page) override;

    // Update staging page
    void UpdateStagingPage(const RefPtr<JsAcePage>& page) override;

    // Reset staging page
    void ResetStagingPage() override;

    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) override;

    // Fire AsyncEvent on JS
    void FireAsyncEvent(const std::string& eventId, const std::string& param) override;

    // Fire SyncEvent on JS
    void FireSyncEvent(const std::string& eventId, const std::string& param) override;

    void FireExternalEvent(const std::string& componentId, const uint32_t nodeId) override;

    // Timer callback
    virtual void TimerCallback(const std::string& callbackId, const std::string& delay, bool isInterval) override;

    void TimerCallJs(const std::string& callbackId, bool isInterval);

    // destroy page instance
    void DestroyPageInstance(int32_t pageId) override;

    void OnConfigurationUpdated(const std::string& data) override;

    bool OnStartContinuation() override;

    void OnCompleteContinuation(int32_t code) override;

    void OnRemoteTerminated() override;

    void OnSaveData(std::string& data) override;

    bool OnRestoreData(const std::string& data) override;

    void MediaQueryCallback(const std::string& callbackId, const std::string& args) override;

    void RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp) override;

    void JsCallback(const std::string& callbackId, const std::string& args) override;

    JSValue CallAppFunc(std::string appFuncName, int argc, JSValueConst* argv);

    // destroy application instance according packageName
    void DestroyApplication(const std::string& packageName) override;

    void UpdateApplicationState(const std::string& packageName, Frontend::State state) override;

    void OnWindowDisplayModeChanged(bool isShownInMultiWindow, const std::string& data) override;

    void RunGarbageCollection() override;

    RefPtr<GroupJsBridge> GetGroupJsBridge() override;

    virtual FrontendDelegate* GetFrontend() override
    {
        return AceType::RawPtr(engineInstance_->GetDelegate());
    }

private:
    RefPtr<QJSDeclarativeEngineInstance> engineInstance_;
    int32_t instanceId_ = 0;
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    QuickJSNativeEngine* nativeEngine_ = nullptr;
    void SetPostTask(NativeEngine* nativeEngine);
#endif

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM) and defined(ENABLE_WORKER)
    void RegisterWorker();
    void RegisterInitWorkerFunc();
    void RegisterAssetFunc();
#endif

    ACE_DISALLOW_COPY_AND_MOVE(QJSDeclarativeEngine);
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORK_JAVASCRIPT_ENGINE_ACE_DIFF_ENGINE_H
