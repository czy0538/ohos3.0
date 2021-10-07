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

#include "ecmascript/builtins/builtins_arraybuffer.h"
#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"

#include "ecmascript/js_arraybuffer.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::builtins;

namespace panda::test {
class BuiltinsArrayBufferTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "TearDownCase";
    }

    void SetUp() override
    {
        TestHelper::CreateEcmaVMWithScope(instance, thread, scope);
    }

    void TearDown() override
    {
        TestHelper::DestroyEcmaVMWithScope(instance, scope);
    }

    PandaVM *instance {nullptr};
    EcmaHandleScope *scope {nullptr};
    JSThread *thread {nullptr};
};

JSTaggedValue CreateBuiltinsArrayBuffer(JSThread *thread, int32_t length)
{
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> arrayBuffer(thread, env->GetArrayBufferFunction().GetTaggedValue());
    JSHandle<JSObject> globalObject(thread, env->GetGlobalObject());
    // 6 : test case
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, arrayBuffer.GetTaggedValue(), 6);
    ecmaRuntimeCallInfo->SetFunction(arrayBuffer.GetTaggedValue());
    ecmaRuntimeCallInfo->SetThis(globalObject.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(length));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsArrayBuffer::ArrayBufferConstructor(ecmaRuntimeCallInfo.get());
    return result;
}

// new ArrayBuffer(8)
HWTEST_F_L0(BuiltinsArrayBufferTest, Constructor1)
{
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> arrayBuffer(thread, env->GetArrayBufferFunction().GetTaggedValue());
    JSHandle<JSObject> globalObject(thread, env->GetGlobalObject());
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, arrayBuffer.GetTaggedValue(), 6);
    ecmaRuntimeCallInfo->SetFunction(arrayBuffer.GetTaggedValue());
    ecmaRuntimeCallInfo->SetThis(globalObject.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(static_cast<int32_t>(8)));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsArrayBuffer::ArrayBufferConstructor(ecmaRuntimeCallInfo.get());
    ASSERT_TRUE(result.IsECMAObject());
}

// (new ArrayBuffer(5)).byteLength
HWTEST_F_L0(BuiltinsArrayBufferTest, byteLength1)
{
    JSTaggedValue tagged = CreateBuiltinsArrayBuffer(thread, 5);
    JSHandle<JSArrayBuffer> arrBuf(thread, JSArrayBuffer::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(arrBuf.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsArrayBuffer::GetByteLength(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(5).GetRawData());
}

// (new ArrayBuffer(10)).slice(1, 5).bytelength
HWTEST_F_L0(BuiltinsArrayBufferTest, slice1)
{
    JSTaggedValue tagged = CreateBuiltinsArrayBuffer(thread, 10);
    JSHandle<JSArrayBuffer> arrBuf(thread, JSArrayBuffer::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(arrBuf.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(static_cast<int32_t>(1)));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(static_cast<int32_t>(5)));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result1 = BuiltinsArrayBuffer::Slice(ecmaRuntimeCallInfo.get());
    TestHelper::TearDownFrame(thread, prev);
    JSHandle<JSArrayBuffer> arrBuf1(thread,
                                     JSArrayBuffer::Cast(reinterpret_cast<TaggedObject *>(result1.GetRawData())));
    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(arrBuf1.GetTaggedValue());
    prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo1.get());
    JSTaggedValue result2 = BuiltinsArrayBuffer::GetByteLength(ecmaRuntimeCallInfo1.get());

    ASSERT_EQ(result2.GetRawData(), JSTaggedValue(4).GetRawData());
}
}  // namespace panda::test
