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

#include "core/animation/animatable_data.h"

namespace OHOS::Ace {

template<>
Dimension AnimatableData<Dimension>::Blend(
    const Dimension& start, const Dimension& end, float process)
{
    float scaleA = process;
    float scaleB = 1.0f - process;
    return start * scaleB + end * scaleA;
}

template<>
Color AnimatableData<Color>::Blend(
    const Color& start, const Color& end, float process)
{
    float startA = start.GetAlpha();
    float startR = start.GetRed();
    float startG = start.GetGreen();
    float startB = start.GetBlue();
    int32_t alpha = startA + (end.GetAlpha() - startA) * process;
    int32_t red = startR + (end.GetRed() - startR) * process;
    int32_t green = startG + (end.GetGreen() - startG) * process;
    int32_t blue = startB + (end.GetBlue() - startB) * process;
    return Color::FromARGB(alpha, red, green, blue);
}

template<>
BorderStyle AnimatableData<BorderStyle>::Blend(const BorderStyle& start, const BorderStyle& end, float process)
{
    if (process > 0.5f) {
        return end;
    }
    return start;
}

template<>
Shadow AnimatableData<Shadow>::Blend(const Shadow& start, const Shadow& end, float process)
{
    return Shadow::Blend(end, start, process);
}

template<>
BackgroundImageSize AnimatableData<BackgroundImageSize>::Blend(
    const BackgroundImageSize& start, const BackgroundImageSize& end, float process)
{
    float scaleA = process;
    float scaleB = 1.0f - process;
    return start * scaleB + end * scaleA;
}

} // namespace OHOS::Ace