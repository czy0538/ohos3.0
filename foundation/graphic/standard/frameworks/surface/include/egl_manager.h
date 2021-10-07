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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_EGL_MANAGER_H
#define FRAMEWORKS_SURFACE_INCLUDE_EGL_MANAGER_H

#include <mutex>
#include <thread>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <gbm.h>
#include <refbase.h>
#include "surface_type.h"

using EglCreateImageFunc = PFNEGLCREATEIMAGEKHRPROC;
using EglDestroyImageFunc = PFNEGLDESTROYIMAGEKHRPROC;
using EglImageTargetTexture2DFunc = PFNGLEGLIMAGETARGETTEXTURE2DOESPROC;
using EglCreateSyncFunc = PFNEGLCREATESYNCKHRPROC;
using EglDestroySyncFunc = PFNEGLDESTROYSYNCKHRPROC;
using EglClientWaitSyncFunc = PFNEGLCLIENTWAITSYNCKHRPROC;
using EglDupNativeFenceFdFunc = PFNEGLDUPNATIVEFENCEFDANDROIDPROC;
using EglWaitSyncFunc = PFNEGLWAITSYNCKHRPROC;
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;

namespace OHOS {
class EglManager : public RefBase {
public:
    static sptr<EglManager> GetInstance();

    EGLDisplay GetEGLDisplay() const;
    EGLContext GetEGLContext() const;

    EGLImageKHR EglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attribList);
    EGLImageKHR EglCreateImage(EGLenum target, EGLClientBuffer buffer, const EGLint *attribList);
    EGLBoolean EglDestroyImage(EGLImageKHR image);
    void EglImageTargetTexture2D(GLenum target, GLeglImageOES image);
    EGLSyncKHR EglCreateSync(EGLenum type, const EGLint *attribList);
    EGLint EglWaitSync(EGLSyncKHR sync, EGLint flags);
    EGLint EglClientWaitSync(EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
    EGLBoolean EglDestroySync(EGLSyncKHR sync);
    EGLint EglDupNativeFenceFd(EGLSyncKHR sync);
    EGLBoolean EglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx);
    EGLBoolean EglMakeCurrent(EGLSurface draw, EGLSurface read);
    EGLBoolean EglMakeCurrent();

    SurfaceError Init(EGLContext ctx = EGL_NO_CONTEXT);
    bool IsInit() const;

private:
    EglManager();
    virtual ~EglManager();
    void Deinit();
    SurfaceError GbmInit();
    SurfaceError EglInit(EGLContext ctx = EGL_NO_CONTEXT);
    SurfaceError EglCheckExt();
    SurfaceError EglFuncInit();

    thread_local static inline sptr<EglManager> instance_ = nullptr;
    bool initFlag_ = false;
    int drmFd_ = -1;
    struct gbm_device *device_ = nullptr;

    EGLDisplay display_;
    EGLContext context_;

    std::map<uint32_t, EGLContext> sharedContextCache;

    bool ctxReleaseFlg_ = false;
    EGLConfig conf_;
    EglCreateImageFunc createImage_;
    EglDestroyImageFunc destroyImage_;
    EglImageTargetTexture2DFunc imageTargetTexture2d_;
    EglCreateSyncFunc createSync_;
    EglDestroySyncFunc destroySync_;
    EglWaitSyncFunc waitSync_;
    EglClientWaitSyncFunc clientWaitSync_;
    EglDupNativeFenceFdFunc dupNativeFenceFd_;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_EGL_MANAGER_H
