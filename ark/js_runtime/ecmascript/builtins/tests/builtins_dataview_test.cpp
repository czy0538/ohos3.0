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

#include "ecmascript/builtins/builtins_dataview.h"
#include "ecmascript/builtins/builtins_arraybuffer.h"
#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_arraybuffer.h"
#include "ecmascript/js_dataview.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::builtins;

namespace panda::test {
using DataViewType = ecmascript::DataViewType;
class BuiltinsDataViewTest : public testing::Test {
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
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue(*arrayBuffer), 6);
    ecmaRuntimeCallInfo->SetFunction(arrayBuffer.GetTaggedValue());
    ecmaRuntimeCallInfo->SetThis(globalObject.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(length));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsArrayBuffer::ArrayBufferConstructor(ecmaRuntimeCallInfo.get());
    return result;
}

JSTaggedValue CreateBuiltinsDataView(JSThread *thread, int32_t length, int32_t byte_offset)
{
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> dataView(thread, env->GetDataViewFunction().GetTaggedValue());
    JSHandle<JSObject> globalObject(thread, env->GetGlobalObject());
    JSTaggedValue tagged = CreateBuiltinsArrayBuffer(thread, length);
    JSHandle<JSArrayBuffer> arrBuf(thread, JSArrayBuffer::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    // 8 : test case
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue(*dataView), 8);
    ecmaRuntimeCallInfo->SetFunction(dataView.GetTaggedValue());
    ecmaRuntimeCallInfo->SetThis(globalObject.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, arrBuf.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(byte_offset));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::DataViewConstructor(ecmaRuntimeCallInfo.get());
    return result;
}

void SetUint8(JSThread *thread, const JSHandle<JSDataView> &view, int32_t offset, JSTaggedValue value)
{
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(offset));
    ecmaRuntimeCallInfo->SetCallArg(1, value);

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    BuiltinsDataView::SetUint8(ecmaRuntimeCallInfo.get());
}

// new DataView(new ArrayBuffer(10), 1)
HWTEST_F_L0(BuiltinsDataViewTest, Constructor)
{
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> dataView(thread, env->GetDataViewFunction().GetTaggedValue());
    JSHandle<JSObject> globalObject(thread, env->GetGlobalObject());
    JSTaggedValue tagged = CreateBuiltinsArrayBuffer(thread, 10);
    JSHandle<JSArrayBuffer> arrBuf(thread, JSArrayBuffer::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue(*dataView), 8);
    ecmaRuntimeCallInfo->SetFunction(dataView.GetTaggedValue());
    ecmaRuntimeCallInfo->SetThis(globalObject.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, arrBuf.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(1));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::DataViewConstructor(ecmaRuntimeCallInfo.get());
    ASSERT_TRUE(result.IsECMAObject());
}

// new DataView(new ArrayBuffer(10), 1).byteOffset
HWTEST_F_L0(BuiltinsDataViewTest, byteOffset)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 10, 1);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetOffset(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(1).GetRawData());
}

// new DataView(new ArrayBuffer(10), 2).byteLength
HWTEST_F_L0(BuiltinsDataViewTest, byteLength)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 10, 2);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetByteLength(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(8).GetRawData());
}

// new DataView(new ArrayBuffer(10), 1).buffer
HWTEST_F_L0(BuiltinsDataViewTest, buffer)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 10, 1);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetBuffer(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.IsArrayBuffer(), true);
}

// new DataView(new ArrayBuffer(8), 0).SetUint16/GetUint16
HWTEST_F_L0(BuiltinsDataViewTest, getUint16)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(-1870724872));
    ecmaRuntimeCallInfo->SetCallArg(2, JSTaggedValue::False());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::SetUint16(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::VALUE_UNDEFINED);

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo1->SetCallArg(1, JSTaggedValue::True());

    JSTaggedValue result1 = BuiltinsDataView::GetUint16(ecmaRuntimeCallInfo1.get());
    ASSERT_EQ(result1.GetRawData(), JSTaggedValue(63488).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetInt16/GetInt16
HWTEST_F_L0(BuiltinsDataViewTest, getInt16)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(-1870724872));
    ecmaRuntimeCallInfo->SetCallArg(2, JSTaggedValue::False());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::SetInt16(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::VALUE_UNDEFINED);

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo1->SetCallArg(1, JSTaggedValue::True());

    JSTaggedValue result1 = BuiltinsDataView::GetInt16(ecmaRuntimeCallInfo1.get());
    ASSERT_EQ(result1.GetRawData(), JSTaggedValue(-2048).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetUint8/GetUint32
HWTEST_F_L0(BuiltinsDataViewTest, GetUint32)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    SetUint8(thread, view, 0, JSTaggedValue(127));
    SetUint8(thread, view, 1, JSTaggedValue(255));
    SetUint8(thread, view, 2, JSTaggedValue(255));
    SetUint8(thread, view, 3, JSTaggedValue(255));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue::False());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetUint32(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(2147483647).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetUint8/GetInt32
HWTEST_F_L0(BuiltinsDataViewTest, GetInt32)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    SetUint8(thread, view, 0, JSTaggedValue(127));
    SetUint8(thread, view, 1, JSTaggedValue(255));
    SetUint8(thread, view, 2, JSTaggedValue(255));
    SetUint8(thread, view, 3, JSTaggedValue(255));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue::False());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetInt32(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(2147483647).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetUint8/GetInt8
HWTEST_F_L0(BuiltinsDataViewTest, GetInt8)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    SetUint8(thread, view, 0, JSTaggedValue(255));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetInt8(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(-1).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetUint8/GetUint8
HWTEST_F_L0(BuiltinsDataViewTest, GetUint8)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    SetUint8(thread, view, 0, JSTaggedValue(127));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetUint8(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(127).GetRawData());
}

// new DataView(new ArrayBuffer(8), 4).SetUint8/GetFloat32
HWTEST_F_L0(BuiltinsDataViewTest, GetFloat32)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    SetUint8(thread, view, 4, JSTaggedValue(75));
    SetUint8(thread, view, 5, JSTaggedValue(75));
    SetUint8(thread, view, 6, JSTaggedValue(75));
    SetUint8(thread, view, 7, JSTaggedValue(75));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(4));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue::False());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetFloat32(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(static_cast<double>(13323083)).GetRawData());
}

// new DataView(new ArrayBuffer(12), 4).SetUint8/GetFloat64
HWTEST_F_L0(BuiltinsDataViewTest, GetFloat64)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 12, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    SetUint8(thread, view, 4, JSTaggedValue(67));
    SetUint8(thread, view, 5, JSTaggedValue(67));
    SetUint8(thread, view, 6, JSTaggedValue(68));
    SetUint8(thread, view, 7, JSTaggedValue(68));
    SetUint8(thread, view, 8, JSTaggedValue(67));
    SetUint8(thread, view, 9, JSTaggedValue(67));
    SetUint8(thread, view, 10, JSTaggedValue(68));
    SetUint8(thread, view, 11, JSTaggedValue(68));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(4));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue::False());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::GetFloat64(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue(static_cast<double>(10846169068898440)).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetUint32/GetUint32
HWTEST_F_L0(BuiltinsDataViewTest, SetUint32)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(0x907f00f8));
    ecmaRuntimeCallInfo->SetCallArg(2, JSTaggedValue::True());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::SetUint32(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::VALUE_UNDEFINED);

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo1->SetCallArg(1, JSTaggedValue::False());

    JSTaggedValue result1 = BuiltinsDataView::GetUint32(ecmaRuntimeCallInfo1.get());
    ASSERT_EQ(result1.GetRawData(), JSTaggedValue(static_cast<double>(0xf8007f90)).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetInt32/GetInt32
HWTEST_F_L0(BuiltinsDataViewTest, SetInt32)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(-1870724872));
    ecmaRuntimeCallInfo->SetCallArg(2, JSTaggedValue::True());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::SetInt32(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::VALUE_UNDEFINED);

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo1->SetCallArg(1, JSTaggedValue::False());

    JSTaggedValue result1 = BuiltinsDataView::GetInt32(ecmaRuntimeCallInfo1.get());
    ASSERT_EQ(result1.GetRawData(), JSTaggedValue(-134185072).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetInt8/GetUint8
HWTEST_F_L0(BuiltinsDataViewTest, SetInt8)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(-1));
    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::SetInt8(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::VALUE_UNDEFINED);

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(0, JSTaggedValue(0));
    JSTaggedValue result1 = BuiltinsDataView::GetUint8(ecmaRuntimeCallInfo1.get());
    ASSERT_EQ(result1.GetRawData(), JSTaggedValue(255).GetRawData());
}

// new DataView(new ArrayBuffer(4), 0).SetFloat32/GetFloat32
HWTEST_F_L0(BuiltinsDataViewTest, SetFloat32)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 4, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(42));
    ecmaRuntimeCallInfo->SetCallArg(2, JSTaggedValue::True());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::SetFloat32(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::VALUE_UNDEFINED);

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo1->SetCallArg(1, JSTaggedValue::False());

    JSTaggedValue result1 = BuiltinsDataView::GetFloat32(ecmaRuntimeCallInfo1.get());
    ASSERT_EQ(result1.GetRawData(), JSTaggedValue(static_cast<double>(1.4441781973331565e-41)).GetRawData());
}

// new DataView(new ArrayBuffer(8), 0).SetFloat64/GetFloat64
HWTEST_F_L0(BuiltinsDataViewTest, SetFloat64)
{
    JSTaggedValue tagged = CreateBuiltinsDataView(thread, 8, 0);
    JSHandle<JSDataView> view(thread, JSDataView::Cast(reinterpret_cast<TaggedObject *>(tagged.GetRawData())));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo->SetCallArg(1, JSTaggedValue(42));
    ecmaRuntimeCallInfo->SetCallArg(2, JSTaggedValue::True());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo.get());
    JSTaggedValue result = BuiltinsDataView::SetFloat64(ecmaRuntimeCallInfo.get());
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::VALUE_UNDEFINED);

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(view.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(0, JSTaggedValue(0));
    ecmaRuntimeCallInfo1->SetCallArg(1, JSTaggedValue::False());

    JSTaggedValue result1 = BuiltinsDataView::GetFloat64(ecmaRuntimeCallInfo1.get());
    ASSERT_EQ(result1.GetRawData(), JSTaggedValue(static_cast<double>(8.759e-320)).GetRawData());
}
}  // namespace panda::test
