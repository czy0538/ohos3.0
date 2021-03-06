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

#ifndef AIE_INFO_DEFINE_H
#define AIE_INFO_DEFINE_H

#include <sys/types.h>

typedef struct ConfigInfo {
    const char *description; // max length is 200, limit by ipc.
} ConfigInfo;

typedef struct ClientInfo { // initialized by AieClientInit, don't init manually.
    long long clientVersion; // reserved field

    // generated by server in AieClientInit, valid value > 0, INVALID_CLIENT_ID(-1) is invalid, 0 is undefined.
    int clientId;

    // generated by client in AieClientInit, valid value > 0, INVALID_SESSION_ID(-1) is invalid, 0 is undefined.
    int sessionId;
    uid_t serverUid; // server's UID, for the sake of share memory transfer.
    uid_t clientUid; // client's UID, for the sake of share memory transfer.
    int extendLen; // reserved field
    unsigned char *extendMsg; // reserved field
} ClientInfo;

typedef struct AlgorithmInfo {
    long long clientVersion; // reserved field
    bool isAsync; // indicate asynchronous

    // determine ALGORITHM_TYPE_ID_LIST[algorithmType]
    int algorithmType;

    // algorithmType+algorithmVersion determines section in
    // services/common/protocol/plugin_config/plugin_config_ini/*.ini,
    // where the FullPath is given.
    long long algorithmVersion;

    bool isCloud; // reserved field
    int operateId; // reserved field
    int requestId; // identity of algorithm when ClientCallback::OnResult is called. valid only for async algorithm.
    int extendLen; // reserved field
    unsigned char *extendMsg; // reserved field
} AlgorithmInfo;

typedef struct DataInfo {
    unsigned char *data;
    int length;
} DataInfo;

#endif // AIE_INFO_DEFINE_H
