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

#include "avmeta_meta_collector.h"
#include "avmetadatahelper.h"
#include "media_errors.h"
#include "media_log.h"
#include "gst_utils.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AVMetaCollector"};
}

namespace OHOS {
namespace Media {
enum GstElemType : uint8_t {
    TYPEFIND,
    DEMUXER,
    PARSER,
    DECODER,
    UNKNOWN,
};

struct GstElemMetaMatchDesc {
    std::string_view metaKey;
    std::vector<std::string_view> expectedFields;
};

static const std::unordered_map<GstElemType, GstElemMetaMatchDesc> GST_ELEM_META_MATCH_DESC = {
    { GstElemType::TYPEFIND, { GST_ELEMENT_METADATA_LONGNAME, { "TypeFind" } } },
    { GstElemType::DEMUXER, { GST_ELEMENT_METADATA_KLASS, { "Codec", "Demuxer" } } },
    { GstElemType::PARSER, { GST_ELEMENT_METADATA_KLASS, { "Codec", "Parser" } } },
    { GstElemType::DECODER, { GST_ELEMENT_METADATA_KLASS, { "Codec", "Decoder" } } },
};

AVMetaMetaCollector::AVMetaMetaCollector()
    : currSetupedElemType_(GstElemType::UNKNOWN)
{
    MEDIA_LOGD("enter ctor, instance: 0x%{public}06" PRIXPTR "", FAKE_POINTER(this));
}

AVMetaMetaCollector::~AVMetaMetaCollector()
{
    MEDIA_LOGD("enter dtor, instance: 0x%{public}06" PRIXPTR "", FAKE_POINTER(this));

    {
        decltype(blockers_) temp;
        temp.swap(blockers_);
        for (auto &[type, blockerVec] : temp) {
            for (auto &blocker : blockerVec) {
                if (blocker != nullptr) {
                    // Rather than cancel, just clear the blocks, for speed up the destroy process
                    blocker->Clear();
                }
            }
        }
    }

    for (auto &[elem, signalId] : signalIds_) {
        g_signal_handler_disconnect(elem, signalId);
    }

    stopCollecting_ = true;
    cond_.notify_all();
}

void AVMetaMetaCollector::Start()
{
    MEDIA_LOGD("start collecting...");
}

void AVMetaMetaCollector::AddMetaSource(GstElement &source)
{
    uint8_t srcType = ProbeElemType(source);
    AddElemCollector(source, srcType);
    AddElemBlocker(source, srcType);
}

void AVMetaMetaCollector::Stop()
{
    MEDIA_LOGD("stop collecting...");

    {
        decltype(blockers_) temp;
        temp.swap(blockers_); // will cancel all blocks
    }
    {
        decltype(elemCollectors_) temp;
        temp.swap(elemCollectors_);
    }

    stopCollecting_ = true;
    cond_.notify_all();
}

std::unordered_map<int32_t, std::string> AVMetaMetaCollector::GetMetadata()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]() { return CheckCollectCompleted() || stopCollecting_; });

    AdjustMimeType();

    return allMeta_.tbl_;
}

bool AVMetaMetaCollector::CheckCollectCompleted() const
{
    if (elemCollectors_.size() == 0 || blockers_.size() == 0) {
        return false;
    }

    if (trackMetaCollected_.count(AVMETA_TRACK_NUMBER_FILE) == 0) {
        return false;
    }

    for (auto &collector : elemCollectors_) {
        if (collector == nullptr) {
            continue;
        }

        if (collector->GetType() == AVMetaSourceType::TYPEFIND) {
            continue;
        }

        int32_t trackCount = collector->GetTrackCount();
        for (auto trackId = 0; trackId < trackCount; trackId++) {
            if (trackMetaCollected_.count(trackId) == 0) {
                return false;
            }
        }
    }

    for (auto &[type, blockerVec] : blockers_) {
        for (auto &blocker : blockerVec) {
            if (blocker == nullptr) {
                continue;
            }
            if (!blocker->CheckBufferRecieved()) {
                return false;
            }
        }
    }

    return true;
}

uint8_t AVMetaMetaCollector::ProbeElemType(GstElement &source)
{
    for (const auto &[srcType, matchDesc] : GST_ELEM_META_MATCH_DESC) {
        bool matchResult = MatchElementByMeta(source, matchDesc.metaKey, matchDesc.expectedFields);
        if (!matchResult) {
            continue;
        }

        std::string detailLog = "metaKey: ";
        detailLog += matchDesc.metaKey;
        detailLog += ", expected field: ";
        for (auto &fields : matchDesc.expectedFields) {
            detailLog += fields;
            detailLog += " ";
        }
        MEDIA_LOGD("find %{public}s, %{public}s", ELEM_NAME(&source), detailLog.c_str());

        if (hasSrcType_.count(srcType) == 0) {
            (void)hasSrcType_.emplace(srcType, 0);
        }
        hasSrcType_[srcType] += 1;
        return srcType;
    }

    return GstElemType::UNKNOWN;
}

void AVMetaMetaCollector::AdjustMimeType()
{
    std::string mimeType;
    if (allMeta_.GetMeta(AV_KEY_MIME_TYPE, mimeType)) {
        if ((mimeType.compare(FILE_MIMETYPE_VIDEO_MP4) == 0) && !allMeta_.HasMeta(AV_KEY_HAS_VIDEO)) {
            if (allMeta_.HasMeta(AV_KEY_HAS_AUDIO)) {
                allMeta_.SetMeta(AV_KEY_MIME_TYPE, std::string(FILE_MIMETYPE_AUDIO_MP4));
            }
        }
    }
}

void AVMetaMetaCollector::UpdataMeta(int32_t trackId, const Metadata &metadata)
{
    MEDIA_LOGD("trackId = %{public}d", trackId);
    std::unique_lock<std::mutex> lock(mutex_);

    for (auto &[key, value] : metadata.tbl_) {
        allMeta_.SetMeta(key, value);
    }

    (void)trackMetaCollected_.emplace(trackId);
    cond_.notify_all();
}

void AVMetaMetaCollector::AddElemCollector(GstElement &source, uint8_t type)
{
    if (type != GstElemType::TYPEFIND && type != GstElemType::DEMUXER && type != GstElemType::PARSER) {
        return;
    }

    for (auto &collector : elemCollectors_) {
        if (collector->GetType() == static_cast<AVMetaSourceType>(type)) {
            collector->AddMetaSource(source);
            return;
        }
    }

    // already has demuxer, reject to create parser's collector
    if ((hasSrcType_.count(GstElemType::DEMUXER) != 0) &&
        (type != GstElemType::DEMUXER) &&
        (type != GstElemType::TYPEFIND)) {
        return;
    }

    auto metaUpdateCb = std::bind(&AVMetaMetaCollector::UpdataMeta,
                                  this, std::placeholders::_1, std::placeholders::_2);
    auto result = AVMetaElemMetaCollector::Create(static_cast<AVMetaSourceType>(type), metaUpdateCb);
    result->AddMetaSource(source);
    elemCollectors_.push_back(std::move(result));
}

void AVMetaMetaCollector::AddElemBlocker(GstElement &source, uint8_t type)
{
    /**
     * After the demuxer or parser plugin of gstreamer complete the metadata resolve work,
     * them will send one frame buffer to downstream. If there is decoder at the downstream,
     * the decode will happened, which is unneccesary and wastefully for metadata resolving.
     * We can block the demuxer or parser's sinkpads to prevent the decode process happened.
     * However, as long as the conditions for preventing the buffer from reaching the decoder
     * are met, the blocked plug-in should be more downstream in the entire pipeline as far
     * as possible, so that the upstream plugin of the decoder is not affected by the block
     * operation as much as possible.
     *
     * One kind of possible sequence of element setuped to the pipeline is :
     * Demuxer1 --> Demuxer2 ---> Parser1 --> Decoder1
     *                      |\
     *                      | `-> Parser2 --> Parser3 --> Decoder2
     *                      \
     *                       `--> Decoder3
     * Or:
     * Parser1 --> Decoder1
     * Or:
     * Parser1 -->
     *
     * Therefore, we will process the block by referring to these order.
     */

#define PUSH_NEW_BLOCK(type, blocker)                                         \
    do {                                                                      \
        auto typeBlockersIter = blockers_.find(type);                         \
        if (typeBlockersIter == blockers_.end()) {                            \
            auto ret = blockers_.emplace(type, BufferBlockerVec {});          \
            typeBlockersIter = ret.first;                                     \
        }                                                                     \
        (blocker)->Init();                                                      \
        (void)typeBlockersIter->second.emplace_back(std::move(blocker));      \
    } while (0)

    if (type == GstElemType::TYPEFIND || type == GstElemType::UNKNOWN) {
        return;
    }

    auto notifier = [this]() { cond_.notify_all(); };

    if (type == GstElemType::DEMUXER || type == GstElemType::PARSER) {
        auto blocker = std::make_unique<AVMetaBufferBlocker>(source, true, notifier);
        PUSH_NEW_BLOCK(type, blocker);
        UpdateElemBlocker(source, type);
        return;
    }

    if (type == GstElemType::DECODER) {
        auto blocker = std::make_unique<AVMetaBufferBlocker>(source, false, notifier);
        PUSH_NEW_BLOCK(type, blocker);
        UpdateElemBlocker(source, type);
    }
}

void AVMetaMetaCollector::UpdateElemBlocker(GstElement &source, uint8_t elemType)
{
    /**
     * When the new element is setuped, we need to update the block. The update
     * strategy is: cancel the upstream block. Because a block has been added to
     * the current element, after an upstream block is canceled, the block is
     * successfully moved downstream.
     *
     * When the demuxer is setuped, we always need to set the pad-added listener to
     * the demuxer so that we can figure out which element is upstream of the element
     * currently installed.
     */
    if (elemType == GstElemType::DEMUXER) {
        auto signalId = g_signal_connect(&source, "pad-added", G_CALLBACK(PadAdded), this);
        if (signalId == 0) {
            MEDIA_LOGE("add pad-added signal tp %{public}s failed", ELEM_NAME(&source));
            return;
        }
        (void)signalIds_.emplace_back(std::pair<GstElement *, gulong>{&source, signalId});
    }

    MEDIA_LOGD("update blocker when elem %{public}s setup, elemType: %{public}hhu",
               ELEM_NAME(&source), elemType);

    std::unique_lock<std::mutex> lock(mutex_);
    do {
        if (currSetupedElemType_ == GstElemType::UNKNOWN) {
            break;
        }
        auto typeBlockersIter = blockers_.find(currSetupedElemType_);
        if (typeBlockersIter == blockers_.end() || typeBlockersIter->second.size() <= currSetupedElemIdx_) {
            break;
        }
        auto &currBlocker = typeBlockersIter->second[currSetupedElemIdx_];
        if (currBlocker == nullptr) {
            break;
        }
        if (currBlocker->GetStreamCount() == 0) {
            break;
        }
        currBlocker->CancelBlock(currBlocker->GetStreamCount() - 1);
        if (currSetupedElemType_ != GstElemType::DEMUXER) {
            currBlocker = nullptr;
        }
    } while (0);

    currSetupedElemType_ = elemType;
    // when this function invoked, the blocker had already been setuped for curr element.
    currSetupedElemIdx_ = blockers_.at(elemType).size() - 1;
    MEDIA_LOGD("currType = %{public}hhu, currIdx = %{public}zu", currSetupedElemType_, currSetupedElemIdx_);

    /**
     * Is there any such situation: there is no parser or decoder at the downstream of
     * a certain outstream of the demuxer.
     */
}

void AVMetaMetaCollector::PadAdded(GstElement *elem, GstPad *pad, gpointer userdata)
{
    (void)elem;
    (void)pad;
    CHECK_AND_RETURN_LOG(userdata != nullptr, "userdata is nullptr");
    auto collector = reinterpret_cast<AVMetaMetaCollector *>(userdata);
    std::unique_lock<std::mutex> lock(collector->mutex_);
    collector->currSetupedElemType_ = GstElemType::DEMUXER;
    // when padadded notify, the demuxer had already been setup blocker
    collector->currSetupedElemIdx_ = collector->blockers_.at(GstElemType::DEMUXER).size() - 1;
}
}
}
