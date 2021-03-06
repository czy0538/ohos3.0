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

#include "It_posix_pthread.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

static const INT32 g_keyValue1 = 100;
static const INT32 g_keyValue2 = 200;

static VOID *g_specific1;
static VOID *g_specific2;

static VOID *PthreadF01(VOID *argument)
{
    INT32 ret;

    /* Bind a value to g_key for this thread (this will be different from the value
     * that we bind for the main thread) */
    ret = pthread_setspecific(g_key, (void *)(g_keyValue2));
    ICUNIT_TRACK_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    /* Get the bound value of the g_key that we just set. */
    g_specific2 = pthread_getspecific(g_key);

    pthread_exit(0);

    return NULL;
}

static UINT32 Testcase(VOID)
{
    pthread_t newTh;
    INT32 ret;

    ret = pthread_key_create(&g_key, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    ret = pthread_setspecific(g_key, (void *)(g_keyValue1));
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    ret = pthread_create(&newTh, NULL, PthreadF01, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    ret = pthread_join(newTh, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    g_specific1 = pthread_getspecific(g_key);

    return PTHREAD_NO_ERROR;
}

VOID ItPosixPthread226(VOID)
{
    TEST_ADD_CASE("ItPosixPthread226", Testcase, TEST_POSIX, TEST_PTHREAD, TEST_LEVEL2, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
