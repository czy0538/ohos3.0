/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_ROUTER_MODULE_H
#define OHOS_ACELITE_ROUTER_MODULE_H

#include "jsi.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
/**
 * @brief: The module of router
 */
class RouterModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(RouterModule);
    RouterModule() {}
    ~RouterModule() {}
    static JSIValue Replace(const JSIValue thisVal, const JSIValue* args, uint8_t argsNum);
};

void InitRouterModule(JSIValue exports);
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_ROUTER_MODULE_H
