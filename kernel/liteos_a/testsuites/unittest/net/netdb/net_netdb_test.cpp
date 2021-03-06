/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "stdio.h"
#include <climits>
#include <gtest/gtest.h>

#include "lt_net_netdb.h"

using namespace testing::ext;
namespace OHOS {
class NetDbTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        struct sched_param param = { 0 };
        int currThreadPolicy, ret;
        ret = pthread_getschedparam(pthread_self(), &currThreadPolicy, &param);
        param.sched_priority = TASK_PRIO_TEST;
        ret = pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    }
    static void TearDownTestCase(void) {}
};

#if defined(LOSCFG_USER_TEST_SMOKE) && defined(LOSCFG_USER_TEST_NET_NETDB)
/* *
 * @tc.name: NetNetDbTest001
 * @tc.desc: function for NetDbTest
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(NetDbTest, NetNetDbTest001, TestSize.Level0)
{
    NetNetDbTest001();
}

/* *
 * @tc.name: NetNetDbTest013
 * @tc.desc: function for NetDbTest
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(NetDbTest, NetNetDbTest013, TestSize.Level0)
{
    NetNetDbTest013();
}

#endif
}
