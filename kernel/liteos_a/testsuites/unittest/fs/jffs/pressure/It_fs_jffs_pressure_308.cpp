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

#include "It_vfs_jffs.h"

static UINT32 TestCase(VOID)
{
    INT32 ret, len;
    INT32 fd1 = -1;
    INT32 i = 0;
    CHAR filebuf[20] = "abcdeabcde";
    CHAR readbuf[JFFS_STANDARD_NAME_LENGTH] = "liteos";
    CHAR pathname1[JFFS_STANDARD_NAME_LENGTH] = { JFFS_PATH_NAME0 };
    CHAR pathname2[JFFS_STANDARD_NAME_LENGTH] = { JFFS_PATH_NAME0 };
    CHAR pathname3[JFFS_STANDARD_NAME_LENGTH] = { JFFS_PATH_NAME0 };

    ret = mkdir(pathname1, HIGHEST_AUTHORITY);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT);

    strcat_s(pathname1, JFFS_STANDARD_NAME_LENGTH, "/test");
    fd1 = open(pathname1, O_NONBLOCK | O_CREAT | O_RDWR | O_EXCL, HIGHEST_AUTHORITY);
    ICUNIT_GOTO_NOT_EQUAL(fd1, -1, fd1, EXIT1);

    len = write(fd1, filebuf, strlen(filebuf));
    ICUNIT_GOTO_EQUAL(len, JFFS_SHORT_ARRAY_LENGTH, len, EXIT1);

    ret = close(fd1);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT1);

    for (i = 0; i < JFFS_MAX_NUM_TEST; i++) {
        ret = chdir("/");
        ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT2);

        ret = umount(JFFS_MAIN_DIR0);
        ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT2);

        ret = mount(JFFS_DEV_PATH0, JFFS_MAIN_DIR0, JFFS_FILESYS_TYPE, 0, NULL);
        ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT2);

        ret = chdir(JFFS_MAIN_DIR0);
        ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT2);
    }

    fd1 = open(pathname1, O_NONBLOCK | O_RDWR, HIGHEST_AUTHORITY);
    ICUNIT_GOTO_NOT_EQUAL(fd1, -1, fd1, EXIT1);

    len = read(fd1, readbuf, JFFS_STANDARD_NAME_LENGTH);
    ICUNIT_GOTO_EQUAL(len, JFFS_SHORT_ARRAY_LENGTH, len, EXIT1);
    ICUNIT_GOTO_STRING_EQUAL(readbuf, filebuf, readbuf, EXIT1);

    ret = close(fd1);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT1);

    ret = remove(pathname1);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT1);

    ret = remove(JFFS_PATH_NAME0);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT);

    return JFFS_NO_ERROR;
EXIT2:
    mount(JFFS_DEV_PATH0, JFFS_MAIN_DIR0, JFFS_FILESYS_TYPE, 0, NULL);
EXIT1:
    close(fd1);
    remove(pathname1);
EXIT:
    remove(JFFS_PATH_NAME0);
    return JFFS_NO_ERROR;
}

VOID ItFsJffsPressure308(VOID)
{
    TEST_ADD_CASE("ItFsJffsPressure308", TestCase, TEST_VFS, TEST_JFFS, TEST_LEVEL3, TEST_PRESSURE);
}
