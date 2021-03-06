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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/form/resource/form_request_data.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

class ACE_EXPORT FormComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(FormComponent, ComposedComponent);

public:
    FormComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child);
    FormComponent(const ComposeId& id, const std::string& name);
    ~FormComponent() override = default;

    RefPtr<Element> CreateElement() override;

    void SetFormRequestionInfo(const RequestFormInfo& info)
    {
        info_ = info;
    }

    RequestFormInfo GetFormRequestionInfo() const
    {
        return info_;
    }

    void SetAllowUpdate(bool allow)
    {
        info_.allowUpate = allow;
    }

    bool IsAllowUpdate() const
    {
        return info_.allowUpate;
    }

    void SetDimension(int32_t dimension)
    {
        info_.dimension = dimension;
    }

    void SetModuleName(const std::string& name)
    {
        info_.moduleName = name;
    }

    void SetCardSize(const Dimension& width, const Dimension& height)
    {
        width_ = width;
        height_ = height;
        if (mountBox_) {
            mountBox_->SetWidth(width.Value(), width.Unit());
            mountBox_->SetHeight(height.Value(), height.Unit());
        }
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    void SetOnAcquireFormEventId(const EventMarker& event)
    {
        onAcquireForm_ = event;
    }

    const EventMarker& GetOnAcquireFormEventId() const
    {
        return onAcquireForm_;
    }

    void SetOnErrorEventId(const EventMarker& event)
    {
        onError_ = event;
    }

    const EventMarker& GetOnErrorEvent() const
    {
        return onError_;
    }

    void SetOnRouterEventId(const EventMarker& event)
    {
        onRouter_ = event;
    }

    const EventMarker& GetOnRouterEvent() const
    {
        return onRouter_;
    }

private:
    void CreateChildren();

    RequestFormInfo info_;
    RefPtr<BoxComponent> mountBox_;
    EventMarker onAcquireForm_;
    EventMarker onError_;
    EventMarker onRouter_;
    Dimension width_;
    Dimension height_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_COMPONENT_H
