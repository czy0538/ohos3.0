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

#ifndef PLAYBIN_CTRLER_BASE_H
#define PLAYBIN_CTRLER_BASE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <gst/gst.h>
#include "nocopyable.h"
#include "i_playbin_ctrler.h"
#include "state_machine.h"
#include "gst_msg_processor.h"
#include "task_queue.h"

namespace OHOS {
namespace Media {
class PlayBinCtrlerBase
    : public IPlayBinCtrler,
      public StateMachine,
      public std::enable_shared_from_this<PlayBinCtrlerBase> {
public:
    explicit PlayBinCtrlerBase(const PlayBinMsgNotifier &notifier);
    virtual ~PlayBinCtrlerBase();

    DISALLOW_COPY_AND_MOVE(PlayBinCtrlerBase);

    int32_t Init();
    int32_t SetSinkProvider(std::shared_ptr<PlayBinSinkProvider> sinkProvider) override;
    int32_t SetScene(PlayBinScene scene) override;
    int32_t SetSource(const std::string &uri)  override;
    int32_t Prepare() override;
    int32_t PrepareAsync() override;
    int32_t Play() override;
    int32_t Pause() override;
    int32_t Seek(int64_t timeUs, int32_t seekOption) override;
    int32_t Stop() override;
    void SetElemSetupListener(ElemSetupListener listener) final;

protected:
    virtual int32_t OnInit() = 0;

    GstPipeline *playbin_ = nullptr;

private:
    class BaseState;
    class IdleState;
    class InitializedState;
    class PreparingState;
    class PreparedState;
    class PlayingState;
    class PausedState;
    class StoppedState;

    std::string GetSource();
    int32_t EnterInitializedState();
    void SetupCustomElement();
    int32_t SetupSignalMessage();
    void DeferTask(const std::shared_ptr<TaskHandler<void>> &task, int64_t delayNs = 0);
    static void ElementSetup(const GstElement *playbin, GstElement *elem, gpointer userdata);
    void OnElementSetup(GstElement &elem);
    void OnMessageReceived(const InnerMessage &msg);
    void ReportMessage(const PlayBinMessage &msg);
    void Reset() noexcept;

    std::mutex mutex_;
    std::unique_ptr<TaskQueue> taskQueue_;
    std::unique_ptr<TaskQueue> msgQueue_;
    PlayBinMsgNotifier notifier_;
    ElemSetupListener elemSetupListener_;
    std::shared_ptr<PlayBinSinkProvider> sinkProvider_;
    std::unique_ptr<GstMsgProcessor> msgProcessor_;
    PlayBinScene currScene_ = PlayBinScene::UNKNOWN;
    std::string uri_;
    bool isInitialized = false;
    std::shared_ptr<TaskHandler<int32_t>> preparedTask_;

    std::shared_ptr<IdleState> idleState_;
    std::shared_ptr<InitializedState> initializedState_;
    std::shared_ptr<PreparingState> preparingState_;
    std::shared_ptr<PreparedState> preparedState_;
    std::shared_ptr<PlayingState> playingState_;
    std::shared_ptr<PausedState> pausedState_;
    std::shared_ptr<StoppedState> stoppedState_;
};
}
}
#endif