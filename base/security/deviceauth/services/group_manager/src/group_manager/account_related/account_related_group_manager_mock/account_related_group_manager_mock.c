/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "account_related_group_manager.h"
#include "hc_log.h"

AccountRelatedGroupManager *GetAccountRelatedGroupManager()
{
    return NULL;
}

bool IsAccountRelatedGroupManagerSupported()
{
    return false;
}

bool IsAccountRelatedGroupTypeSupported(int32_t groupType)
{
    (void)groupType;
    return false;
}