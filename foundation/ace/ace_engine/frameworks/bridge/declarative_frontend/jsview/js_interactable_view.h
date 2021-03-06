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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_INTERACTABLE_VIEW_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_INTERACTABLE_VIEW_H

#include "frameworks/bridge/declarative_frontend/jsview/js_pan_handler.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_touch_handler.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "frameworks/core/pipeline/base/component.h"
#include "frameworks/core/gestures/tap_gesture.h"

namespace OHOS::Ace::Framework {

class JSInteractableView {
public:
    static void JsOnTouch(const JSCallbackInfo& args);
    static void JsOnPan(const JSCallbackInfo& args);
    static void JsOnClick(const JSCallbackInfo& info);
    static EventMarker GetClickEventMarker(const JSCallbackInfo& info);
    static RefPtr<Gesture> GetTapGesture(const JSCallbackInfo& info);
    static void JsOnKey(const JSCallbackInfo& args);
    static void SetFocusable(bool focusable);
    static void SetFocusNode(bool isFocusNode);

    static void JsOnAppear(const JSCallbackInfo& info);
    static void JsOnDisAppear(const JSCallbackInfo& info);

    static void JsOnDelete(const JSCallbackInfo& info);
    static void JsOnAccessibility(const JSCallbackInfo& info);

private:
    static EventMarker GetEventMarker(const JSCallbackInfo& info, const std::vector<std::string>& keys);
}; // class JSInteractableView

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_INTERACTABLE_VIEW_H
