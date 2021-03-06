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

#include "ecmascript/ecma_vm.h"
#include "ecmascript/napi/include/jsnapi.h"
#include "ecmascript/tests/test_helper.h"
#include "ecmascript/tooling/test/test_list.h"

namespace panda::tooling::ecmascript::test {
using panda::ecmascript::EcmaHandleScope;
using panda::ecmascript::EcmaVM;
using panda::ecmascript::JSThread;
using panda::test::TestHelper;

class EcmaScriptDebugApiTest : public testing::TestWithParam<const char *> {
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

protected:
    void RunEcmaScriptTest(const char *testName)
    {
        std::cout << "Running " << testName << std::endl;
        SetCurrentTestName(testName);
        EcmaVM *vm = EcmaVM::Cast(instance);
        ASSERT_NE(vm, nullptr);
        auto [pandaFile, entryPoint] = GetTestEntryPoint(testName);

        auto fileNameRef = StringRef::NewFromUtf8(vm, pandaFile.c_str(), pandaFile.size());
        auto entryRef = StringRef::NewFromUtf8(vm, entryPoint.c_str(), entryPoint.size());
        auto res = JSNApi::Execute(vm, fileNameRef, entryRef);
        ASSERT_TRUE(res);
    }
};

HWTEST_P_L0(EcmaScriptDebugApiTest, EcmaScriptSuite)
{
    const char *testName = GetParam();
    ASSERT_NE(testName, nullptr);
    RunEcmaScriptTest(testName);
}

INSTANTIATE_TEST_CASE_P(EcmaDebugApiTest, EcmaScriptDebugApiTest,
                        testing::ValuesIn(GetTestList(panda::panda_file::SourceLang::ECMASCRIPT)));
}  // namespace panda::tooling::ecmascript::test
