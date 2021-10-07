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

#include "egl_manager.h"

#include <fcntl.h>
#include <mutex>
#include <unistd.h>

#include "buffer_log.h"
// #include "shared/platform.h"

#define CHECK_NOT_INIT(ret)                 \
    if (!initFlag_) {                       \
        BLOGE("EglManager is not init.");   \
        return ret;                         \
    }

#define CHECK_NOT_INIT_VOID()               \
    if (!initFlag_) {                       \
        BLOGE("EglManager is not init.");   \
        return;                             \
    }

#define CHECK_PARAM_NULLPTR(param)          \
    if (param == nullptr) {                 \
        BLOGE("param is nullptr.");         \
        return SURFACE_ERROR_INIT;          \
    }

#define CHECK_PARAM_NULLPTR_RET(param, ret) \
    if (param == nullptr) {                 \
        BLOGE("param is nullptr.");         \
        return ret;                         \
    }

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "EglManager" };
constexpr const char *GBM_DEVICE_PATH = "/dev/dri/card0";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char *CHARACTER_STRING_WHITESPACE = " ";
constexpr const char *EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char *EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr const char *EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
constexpr const char *EGL_EXT_IMAGE_DMA_BUF_IMPORT = "EGL_EXT_image_dma_buf_import";
constexpr const char *EGL_KHR_SURFACELESS_CONTEXT = "EGL_KHR_surfaceless_context";
constexpr const char *EGL_KHR_NO_CONFIG_CONTEXT = "EGL_KHR_no_config_context";
constexpr const char *EGL_CREATE_IMAGE_KHR = "eglCreateImageKHR";
constexpr const char *EGL_DESTROY_IMAGE_KHR = "eglDestroyImageKHR";
constexpr const char *EGL_IMAGE_TARGET_TEXTURE2DOES = "glEGLImageTargetTexture2DOES";
constexpr const char *EGL_KHR_FENCE_SYNC = "EGL_KHR_fence_sync";
constexpr const char *EGL_CREATE_SYNC_KHR = "eglCreateSyncKHR";
constexpr const char *EGL_DESTROY_SYNC_KHR = "eglDestroySyncKHR";
constexpr const char *EGL_CLIENT_WAIT_SYNC_KHR = "eglClientWaitSyncKHR";
constexpr const char *EGL_DUP_NATIVE_FENCE_FD_ANDROID = "eglDupNativeFenceFDANDROID";
constexpr const char *EGL_KHR_WAIT_SYNC = "EGL_KHR_wait_sync";
constexpr const char *EGL_WAIT_SYNC_KHR = "eglWaitSyncKHR";
constexpr const char *GL_OES_EGL_IMAGE = "GL_OES_EGL_image";

} // namespace

EglManager::EglManager()
{
    BLOGD("EglManager");
}

EglManager::~EglManager()
{
    BLOGD("~EglManager");
    Deinit();
}

sptr<EglManager> EglManager::GetInstance()
{
    if (instance_ == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance_ == nullptr) {
            instance_ = new EglManager();
        }
    }
    return instance_;
}

bool EglManager::IsInit() const
{
    return initFlag_;
}

SurfaceError EglManager::Init(EGLContext context)
{
    if (initFlag_) {
        BLOGW("already init.");
        return SURFACE_ERROR_OK;
    }

    if (GbmInit() != SURFACE_ERROR_OK) {
        BLOGE("GbmInit failed.");
        return SURFACE_ERROR_INIT;
    }

    if (EglInit(context) != SURFACE_ERROR_OK) {
        BLOGE("EglInit failed.");
        return SURFACE_ERROR_INIT;
    }

    initFlag_ = true;
    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::GbmInit()
{
    drmFd_ = open(GBM_DEVICE_PATH, O_RDWR);
    if (drmFd_ < 0) {
        BLOGE("Failed to open drm render node.");
        return SURFACE_ERROR_INIT;
    }

    device_ = gbm_create_device(drmFd_);

    if (device_ == nullptr) {
        BLOGE("Failed to create gbm device.");
        return SURFACE_ERROR_INIT;
    }

    return SURFACE_ERROR_OK;
}

namespace {
static bool CheckEglExtension(const char *extensions, const char *extension)
{
	size_t extlen = strlen(extension);
	const char *end = extensions + strlen(extensions);

	while (extensions < end) {
		size_t n = 0;

		/* Skip whitespaces, if any */
		if (*extensions == CHARACTER_WHITESPACE) {
			extensions++;
			continue;
		}

		n = strcspn(extensions, CHARACTER_STRING_WHITESPACE);

		/* Compare strings */
		if (n == extlen && strncmp(extension, extensions, n) == 0) {
            return true; /* Found */
        }
		extensions += n;
	}
	/* Not found */
	return false;
}

static EGLDisplay GetPlatformEglDisplay(EGLenum platform, void *native_display, const EGLint *attrib_list)
{
	static GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

	if (!eglGetPlatformDisplayExt) {
        const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (extensions &&
            (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
             CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
            eglGetPlatformDisplayExt =
                (GetPlatformDisplayExt)eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT);
        }
	}

	if (eglGetPlatformDisplayExt) {
        return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
    }
	return eglGetDisplay((EGLNativeDisplayType) native_display);
}
}

SurfaceError EglManager::EglCheckExt()
{
    const char *eglExtensions = eglQueryString(display_, EGL_EXTENSIONS);
    CHECK_PARAM_NULLPTR(eglExtensions);

    if (!CheckEglExtension(eglExtensions, EGL_EXT_IMAGE_DMA_BUF_IMPORT)) {
        BLOGE("EGL_EXT_image_dma_buf_import not supported");
        return SURFACE_ERROR_INIT;
    }

    if (!CheckEglExtension(eglExtensions, EGL_KHR_SURFACELESS_CONTEXT)) {
        BLOGE("EGL_KHR_surfaceless_context not supported");
        return SURFACE_ERROR_INIT;
    }

    if (CheckEglExtension(eglExtensions, EGL_KHR_NO_CONFIG_CONTEXT)) {
        conf_ = EGL_NO_CONFIG_KHR;
    } else {
        BLOGW("EGL_KHR_no_config_context not supported");
        EGLint ret, count;
        EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 1,
            EGL_GREEN_SIZE, 1,
            EGL_BLUE_SIZE, 1,
            EGL_ALPHA_SIZE, 1,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };
        ret = eglChooseConfig(display_, config_attribs, &conf_, 1, &count);
        if (!(ret && count >= 1)) {
            BLOGE("Failed to eglChooseConfig");
            return SURFACE_ERROR_INIT;
        }
    }
    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::EglFuncInit()
{
    const char *eglExtensions = eglQueryString(display_, EGL_EXTENSIONS);
    CHECK_PARAM_NULLPTR(eglExtensions);

    createImage_ = (EglCreateImageFunc)eglGetProcAddress(EGL_CREATE_IMAGE_KHR);
    CHECK_PARAM_NULLPTR(createImage_);

    destroyImage_ = (EglDestroyImageFunc)eglGetProcAddress(EGL_DESTROY_IMAGE_KHR);
    CHECK_PARAM_NULLPTR(destroyImage_);

    imageTargetTexture2d_ = (EglImageTargetTexture2DFunc)eglGetProcAddress(EGL_IMAGE_TARGET_TEXTURE2DOES);
    CHECK_PARAM_NULLPTR(imageTargetTexture2d_);
    
    if (CheckEglExtension(eglExtensions, EGL_KHR_FENCE_SYNC)) {
        createSync_ = (EglCreateSyncFunc)eglGetProcAddress(EGL_CREATE_SYNC_KHR);
        CHECK_PARAM_NULLPTR(createSync_);
        
        destroySync_ = (EglDestroySyncFunc)eglGetProcAddress(EGL_DESTROY_SYNC_KHR);
        CHECK_PARAM_NULLPTR(destroySync_);

        clientWaitSync_ = (EglClientWaitSyncFunc)eglGetProcAddress(EGL_CLIENT_WAIT_SYNC_KHR);
        CHECK_PARAM_NULLPTR(clientWaitSync_);

        dupNativeFenceFd_ = (EglDupNativeFenceFdFunc)eglGetProcAddress(EGL_DUP_NATIVE_FENCE_FD_ANDROID);
        CHECK_PARAM_NULLPTR(dupNativeFenceFd_);
    }

    if (CheckEglExtension(eglExtensions, EGL_KHR_WAIT_SYNC)) {
        waitSync_ = (EglWaitSyncFunc)eglGetProcAddress(EGL_WAIT_SYNC_KHR);
        CHECK_PARAM_NULLPTR(waitSync_);
    }
    return SURFACE_ERROR_OK;
}

SurfaceError EglManager::EglInit(EGLContext ctx)
{
    display_ = GetPlatformEglDisplay(EGL_PLATFORM_GBM_KHR, device_, NULL);
    if (display_ == EGL_NO_DISPLAY) {
        BLOGE("Failed to create EGLDisplay");
        return SURFACE_ERROR_INIT;
    }

    EGLint major, minor;
    if (eglInitialize(display_, &major, &minor) == EGL_FALSE) {
        BLOGE("Failed to initialize EGLDisplay");
        return SURFACE_ERROR_INIT;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        BLOGE("Failed to bind OpenGL ES API");
        return SURFACE_ERROR_INIT;
    }

    if (EglCheckExt() != SURFACE_ERROR_OK) {
        BLOGE("EglCheckExt failed");
        return SURFACE_ERROR_INIT;
    }

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM,
        EGL_NONE
    };

    if (ctx == EGL_NO_CONTEXT) {
        context_ = eglCreateContext(display_, conf_, EGL_NO_CONTEXT, context_attribs);
        if (context_ == EGL_NO_CONTEXT) {
            BLOGE("Failed to create EGLContext");
            return SURFACE_ERROR_INIT;
        }
        ctxReleaseFlg_ = true;
    } else {
        context_ = ctx;
    }

    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);

    const char *glExtensions = (const char *) glGetString(GL_EXTENSIONS);
    CHECK_PARAM_NULLPTR(glExtensions);

    if (!CheckEglExtension(glExtensions, GL_OES_EGL_IMAGE)) {
        BLOGE("GL_OES_EGL_image not supported");
        return SURFACE_ERROR_INIT;
    }

    if (EglFuncInit() != SURFACE_ERROR_OK) {
        BLOGE("EglFuncInit failed");
        return SURFACE_ERROR_INIT;
    }

    return SURFACE_ERROR_OK;
}

void EglManager::Deinit()
{
    initFlag_ = false;

    if (device_) {
        gbm_device_destroy(device_);
    }

    if (drmFd_ >= 0) {
        close(drmFd_);
    }

    if (context_ != EGL_NO_CONTEXT && ctxReleaseFlg_) {
        eglDestroyContext(display_, context_);
    }

    if (display_ != EGL_NO_DISPLAY) {
        eglTerminate(display_);
    }
}

EGLDisplay EglManager::GetEGLDisplay() const
{
    CHECK_NOT_INIT(EGL_NO_DISPLAY);
    return display_;
}

EGLContext EglManager::GetEGLContext() const
{
    CHECK_NOT_INIT(EGL_NO_CONTEXT);
    return context_;
}

EGLImageKHR EglManager::EglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attribList)
{
    CHECK_NOT_INIT(EGL_NO_IMAGE);
    return createImage_(display_, ctx, target, buffer, attribList);
}

EGLImageKHR EglManager::EglCreateImage(EGLenum target, EGLClientBuffer buffer, const EGLint *attribList)
{
    CHECK_NOT_INIT(EGL_NO_IMAGE);
    return createImage_(display_, context_, target, buffer, attribList);
}

EGLBoolean EglManager::EglDestroyImage(EGLImageKHR image)
{
    CHECK_NOT_INIT(EGL_FALSE);
    return destroyImage_(display_, image);
}

void EglManager::EglImageTargetTexture2D(GLenum target, GLeglImageOES image)
{
    CHECK_NOT_INIT_VOID();
    return imageTargetTexture2d_(target, image);
}

EGLSyncKHR EglManager::EglCreateSync(EGLenum type, const EGLint *attribList)
{
    CHECK_PARAM_NULLPTR_RET(createSync_, EGL_NO_SYNC_KHR);
    return createSync_(display_, type, attribList);
}

EGLint EglManager::EglWaitSync(EGLSyncKHR sync, EGLint flags)
{
    CHECK_PARAM_NULLPTR_RET(waitSync_, EGL_FALSE);
    return waitSync_(display_, sync, flags);
}

EGLint EglManager::EglClientWaitSync(EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout)
{
    CHECK_PARAM_NULLPTR_RET(clientWaitSync_, EGL_FALSE);
    return clientWaitSync_(display_, sync, flags, timeout);
}

EGLBoolean EglManager::EglDestroySync(EGLSyncKHR sync)
{
    CHECK_PARAM_NULLPTR_RET(destroySync_, EGL_FALSE);
    return destroySync_(display_, sync);
}

EGLint EglManager::EglDupNativeFenceFd(EGLSyncKHR sync)
{
    CHECK_PARAM_NULLPTR_RET(dupNativeFenceFd_, EGL_DONT_CARE);
    return dupNativeFenceFd_(display_, sync);
}

EGLBoolean EglManager::EglMakeCurrent()
{
    CHECK_NOT_INIT(EGL_FALSE);
    return eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);
}

EGLBoolean EglManager::EglMakeCurrent(EGLSurface draw, EGLSurface read)
{
    CHECK_NOT_INIT(EGL_FALSE);
    return eglMakeCurrent(display_, draw, read, context_);
}

EGLBoolean EglManager::EglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    CHECK_NOT_INIT(EGL_FALSE);
    return eglMakeCurrent(display_, draw, read, ctx);
}

} // namespace OHOS
