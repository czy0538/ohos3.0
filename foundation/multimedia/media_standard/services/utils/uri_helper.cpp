/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "uri_helper.h"
#include <cstring>
#include <climits>
#include "media_errors.h"
#include "media_log.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "UriHelper"};
}

namespace OHOS {
namespace Media {
static bool PathToRealPath(const std::string_view &path, std::string &realPath)
{
    if (path.empty()) {
        MEDIA_LOGE("path is empty!");
        return false;
    }

    if ((path.length() >= PATH_MAX)) {
        MEDIA_LOGE("path len is error, the len is: [%{public}zu]", path.length());
        return false;
    }

    char tmpPath[PATH_MAX] = {0};
    if (realpath(path.data(), tmpPath) == nullptr) {
        MEDIA_LOGE("path to realpath error, %{public}s", path.data());
        return false;
    }

    realPath = tmpPath;
    if (access(realPath.c_str(), F_OK) != 0) {
        MEDIA_LOGE("check realpath (%{private}s) error", realPath.c_str());
        return false;
    }
    return true;
}

UriHelper::UriHelper(UriHelper &&rhs) noexcept
{
    uri_.swap(rhs.uri_);
    formattedUri_.swap(rhs.formattedUri_);
    type_ = rhs.type_;
}

UriHelper &UriHelper::operator=(UriHelper &&rhs) noexcept
{
    if (&rhs == this) {
        return *this;
    }

    uri_.swap(rhs.uri_);
    formattedUri_.swap(rhs.formattedUri_);
    type_ = rhs.type_;

    return *this;
}

UriHelper &UriHelper::FormatMe()
{
    static const std::map<std::string_view, uint8_t> VALID_URI_HEAD_MAP = {
        {"file", URI_TYPE_FILE}, {"fd", URI_TYPE_FD}, {"http", URI_TYPE_HTTP}
    };

    if (!formattedUri_.empty()) {
        return *this;
    }

    std::string_view::size_type start = uri_.find_first_not_of(' ');
    std::string_view::size_type end = uri_.find_last_not_of(' ') + sizeof(char);
    formattedUri_ = uri_.substr(start, end);
    std::string_view rawUri = formattedUri_;
    type_ = URI_TYPE_UNKNOWN;

    std::string_view delimiter = "://";
    std::string_view::size_type pos = rawUri.find(delimiter);
    if (pos == std::string_view::npos) {
        return *this;
    }

    std::string_view head = rawUri.substr(0, pos);
    rawUri = rawUri.substr(pos + delimiter.size());
    if (VALID_URI_HEAD_MAP.count(head) == 0) {
        return *this;
    }

    type_ = VALID_URI_HEAD_MAP.at(head);
    if (type_ == URI_TYPE_FILE) {
        if (PathToRealPath(rawUri, formattedUri_)) {
            (void)formattedUri_.insert(0, "file://");
        }
    }
    return *this;
}

uint8_t UriHelper::UriType() const
{
    return type_;
}

std::string UriHelper::FormattedUri() const
{
    return formattedUri_;
}

bool UriHelper::AccessCheck(uint8_t flag) const
{
    if (type_ == URI_TYPE_FILE) {
        uint32_t mode = (flag & URI_READ) ? R_OK : 0;
        mode |= (flag & URI_WRITE) ? W_OK : 0;
        std::string_view rawUri = formattedUri_;
        rawUri = rawUri.substr(strlen("file://"));
        int ret = access(rawUri.data(), static_cast<int>(mode));
        if (ret != 0) {
            return false;
        }
        return true;
    }

    return false; // Not implemented
}
}
}