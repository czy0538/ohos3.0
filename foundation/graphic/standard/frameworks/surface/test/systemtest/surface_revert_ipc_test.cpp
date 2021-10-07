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

#include "surface_revert_ipc_test.h"

#include <display_type.h>

namespace OHOS {
void SurfaceRevertIPCTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid();
    requestConfig = {
        .width = 0x100,  // small
        .height = 0x100, // small
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    flushConfig = { .damage = {
        .w = 0x100,
        .h = 0x100,
    } };
}

void SurfaceRevertIPCTest::OnBufferAvailable()
{
}

pid_t SurfaceRevertIPCTest::ChildProcessMain()
{
    pipe(pipeFd);
    auto pid = fork();
    if (pid != 0) {
        return pid;
    }

    GTEST_LOG_(INFO) << getpid();
    auto csurface = Surface::CreateSurfaceAsConsumer("test");
    csurface->RegisterConsumerListener(this);
    auto producer = csurface->GetProducer();
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->AddSystemAbility(ipcSystemAbilityID, producer->AsObject());
    auto psurface = Surface::CreateSurfaceAsProducer(producer);

    int64_t data;
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence;
    auto sret = psurface->RequestBuffer(buffer, fence, requestConfig);
    if (sret != SURFACE_ERROR_OK) {
        data = sret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    buffer->ExtraSet("123", 0x123);
    buffer->ExtraSet("345", (int64_t)0x345);
    buffer->ExtraSet("567", "567");

    sret = psurface->FlushBuffer(buffer, -1, flushConfig);
    if (sret != SURFACE_ERROR_OK) {
        data = sret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    Rect damage;
    sret = csurface->AcquireBuffer(buffer, fence, data, damage);
    if (sret != SURFACE_ERROR_OK) {
        data = sret;
        write(pipeFd[1], &data, sizeof(data));
        exit(0);
    }

    sret = csurface->ReleaseBuffer(buffer, -1);
    data = sret;
    write(pipeFd[1], &data, sizeof(data));
    sleep(0);
    read(pipeFd[0], &data, sizeof(data));
    sam->RemoveSystemAbility(ipcSystemAbilityID);
    exit(0);
    return pid;
}

namespace {
HWTEST_F(SurfaceRevertIPCTest, Fork, testing::ext::TestSize.Level0)
{
    auto pid = ChildProcessMain();
    ASSERT_GE(pid, 0);
    int64_t data;
    read(pipeFd[0], &data, sizeof(data));
    GTEST_LOG_(INFO) << getpid();
    ASSERT_EQ(data, SURFACE_ERROR_OK);

    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto robj = sam->GetSystemAbility(ipcSystemAbilityID);
    auto producer = iface_cast<IBufferProducer>(robj);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);

    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t fence;
    auto sret = psurface->RequestBuffer(buffer, fence, requestConfig);
    EXPECT_EQ(sret, SURFACE_ERROR_OK);
    EXPECT_NE(buffer, nullptr);
    if (buffer != nullptr) {
        int32_t int32;
        int64_t int64;
        std::string str;
        buffer->ExtraGet("123", int32);
        buffer->ExtraGet("345", int64);
        buffer->ExtraGet("567", str);

        EXPECT_EQ(int32, 0x123);
        EXPECT_EQ(int64, 0x345);
        EXPECT_EQ(str, "567");
    }
    write(pipeFd[1], &data, sizeof(data));
    waitpid(pid, nullptr, NULL);
}
} // namespace
} // namespace OHOS
