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

#include "core/components_v2/inspector/grid_item_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/grid_layout/grid_layout_item_element.h"
#include "core/components/grid_layout/render_grid_layout_item.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const GridItemComposedElement&)>> CREATE_JSON_MAP {
    { "rowStart", [](const GridItemComposedElement& inspector) { return inspector.GetRowStart(); } },
    { "rowEnd", [](const GridItemComposedElement& inspector) { return inspector.GetRowEnd(); } },
    { "columnStart", [](const GridItemComposedElement& inspector) { return inspector.GetColumnStart(); } },
    { "columnEnd", [](const GridItemComposedElement& inspector) { return inspector.GetColumnEnd(); } },
    { "forceRebuild", [](const GridItemComposedElement& inspector) { return inspector.GetForceRebuild(); } }
};

}

void GridItemComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("rowStart: ").append(GetRowStart()));
    DumpLog::GetInstance().AddDesc(
        std::string("rowEnd: ").append(GetRowEnd()));
    DumpLog::GetInstance().AddDesc(
        std::string("columnStart: ").append(GetColumnStart()));
    DumpLog::GetInstance().AddDesc(
        std::string("columnEnd: ").append(GetColumnEnd()));
    DumpLog::GetInstance().AddDesc(
        std::string("forceRebuild: ").append(GetForceRebuild()));
}

std::unique_ptr<JsonValue> GridItemComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string GridItemComposedElement::GetRowStart() const
{
    auto node = GetInspectorNode(GridLayoutItemElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderGripItem = AceType::DynamicCast<RenderGridLayoutItem>(node);
    if (renderGripItem) {
        return std::to_string(renderGripItem->GetRowStart());
    }
    return "-";
}

std::string GridItemComposedElement::GetRowEnd() const
{
    auto node = GetInspectorNode(GridLayoutItemElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderGripItem = AceType::DynamicCast<RenderGridLayoutItem>(node);
    if (renderGripItem) {
        return std::to_string(renderGripItem->GetRowEnd());
    }
    return "-";
}

std::string GridItemComposedElement::GetColumnStart() const
{
    auto node = GetInspectorNode(GridLayoutItemElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderGripItem = AceType::DynamicCast<RenderGridLayoutItem>(node);
    if (renderGripItem) {
        return std::to_string(renderGripItem->GetColumnStart());
    }
    return "-";
}


std::string GridItemComposedElement::GetColumnEnd() const
{
    auto node = GetInspectorNode(GridLayoutItemElement::TypeId());
    if (!node) {
        return "-";
    }
    auto renderGripItem = AceType::DynamicCast<RenderGridLayoutItem>(node);
    if (renderGripItem) {
        return std::to_string(renderGripItem->GetColumnEnd());
    }
    return "-";
}

std::string GridItemComposedElement::GetForceRebuild() const
{
    auto node = GetInspectorNode(GridLayoutItemElement::TypeId());
    if (!node) {
        return "false";
    }
    auto renderGripItem = AceType::DynamicCast<RenderGridLayoutItem>(node);
    if (renderGripItem) {
        return renderGripItem->GetForceRebuild() ? "true" : "false";
    }
    return "false";
}

} // namespace OHOS::Ace::V2