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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_COMPONENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_COMPONENT_V2_H

#include "core/components/popup/popup_component.h"
#include "core/components/popup/popup_theme.h"

namespace OHOS::Ace {

struct ButtonProperties {
    bool showButton = false;
    std::string value;
    EventMarker actionId;
};

class ACE_EXPORT PopupComponentV2 : public PopupComponent {
    DECLARE_ACE_TYPE(PopupComponentV2, PopupComponent);

public:
    PopupComponentV2(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : PopupComponent(id, name, child) {}
    PopupComponentV2(const ComposeId& id, const std::string& name) : PopupComponent(id, name) {}
    ~PopupComponentV2() override = default;

    RefPtr<Element> CreateElement() override;

    void SetMessage(const std::string& message)
    {
        message_ = message;
    }

    const EventMarker& GetOnStateChange() const
    {
        return onStateChange_;
    }

    void SetOnStateChange(const EventMarker& onStateChange)
    {
        onStateChange_ = onStateChange;
    }

    void Initialization(
        const RefPtr<ThemeManager>& themeManager, const WeakPtr<PipelineContext>& context);

    void SetPrimaryButtonProperties(const ButtonProperties& properties)
    {
        primaryButtonProperties_ = properties;
    }

    void SetSecondaryButtonProperties(const ButtonProperties& properties)
    {
        secondaryButtonProperties_ = properties;
    }

    void SetPlacementOnTop(bool placementOnTop)
    {
        placementOnTop_ = placementOnTop;
    }

private:
    const RefPtr<Component> CreateChild();
    const RefPtr<Component> CreateMessage();
    const RefPtr<Component> CreateButtons();
    const RefPtr<Component> CreateButton(const ButtonProperties& buttonProperties);
    const RefPtr<Component> SetPadding(const RefPtr<Component>& component, const Edge& edge);

    std::string message_;
    bool hasInitialization_ = false;
    bool placementOnTop_ = false;
    EventMarker onStateChange_;
    RefPtr<PopupParam> popupParam_;
    RefPtr<PopupController> popupController_;
    RefPtr<ThemeManager> themeManager_;
    WeakPtr<PipelineContext> context_;

    ButtonProperties primaryButtonProperties_; // first button.
    ButtonProperties secondaryButtonProperties_; // second button.
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_COMPONENT_V2_H
