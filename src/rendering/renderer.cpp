#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include "renderer.h"
#include "../base/rect.h"
#define SK_GANESH
#define SK_GL
#include "../../include/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "../../include/skia/include/gpu/ganesh/GrDirectContext.h"
#include "../../include/skia/include/gpu/ganesh/gl/GrGLInterface.h"
#include "../../include/skia/include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "../../include/skia/include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "../../include/skia/include/gpu/ganesh/SkSurfaceGanesh.h"
#include "../../include/skia/include/gpu/ganesh/SkImageGanesh.h"
#include "../../include/skia/include/core/SkCanvas.h"
#include "../../include/skia/include/core/SkSurface.h"
#include "../../include/skia/include/core/SkColorSpace.h"
#include "../../include/skia/include/core/SkYUVAPixmaps.h"
#include "../../include/skia/include/core/SkYUVAInfo.h"
#include "../../include/skia/include/gpu/ganesh/SkImageGanesh.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/threading.h>
#include <GLES3/gl3.h>
#else
#include <GLFW/glfw3.h>
#endif

struct RenderState {
    std::deque<rendererInstruction>* renderStream;
    std::mutex* renderStreamMutex;
    std::condition_variable* renderCv;
    sk_sp<GrDirectContext> context;
    SkCanvas* canvas;
    sk_sp<SkSurface> surface;
#ifndef __EMSCRIPTEN__
    GLFWwindow* window;
#endif
};

static RenderState gRenderState;

static void renderFrame() {
    auto& rs = gRenderState;

    // Never block in the main loop callback — just check and return if empty
    std::unique_lock<std::mutex> renderLock(*rs.renderStreamMutex);
    if (rs.renderStream->empty()) return;

    rendererInstruction instruction = rs.renderStream->back();
    rs.renderStream->pop_back();
    renderLock.unlock();

    switch (instruction.instructionCode) {
        case 0:
        case 1:
            rs.canvas->clear(SkColorSetRGB(instruction.red, instruction.green, instruction.blue));
            break;

        case 2: {
            if (!instruction.yPlane || !instruction.uPlane || !instruction.vPlane) break;

            SkYUVAInfo yuvaInfo(
                {instruction.videoFrameWidth, instruction.videoFrameHeight},
                SkYUVAInfo::PlaneConfig::kY_U_V,
                SkYUVAInfo::Subsampling::k420,
                kRec601_Limited_SkYUVColorSpace
            );

            SkPixmap yPm(SkImageInfo::MakeA8(instruction.videoFrameWidth, instruction.videoFrameHeight),
                         instruction.yPlane->data(), instruction.yStride);
            SkPixmap uPm(SkImageInfo::MakeA8(instruction.videoFrameWidth / 2, instruction.videoFrameHeight / 2),
                         instruction.uPlane->data(), instruction.uStride);
            SkPixmap vPm(SkImageInfo::MakeA8(instruction.videoFrameWidth / 2, instruction.videoFrameHeight / 2),
                         instruction.vPlane->data(), instruction.vStride);
            SkPixmap planes[SkYUVAPixmaps::kMaxPlanes] = {yPm, uPm, vPm};
            SkYUVAPixmaps yuvaPixmaps = SkYUVAPixmaps::FromExternalPixmaps(yuvaInfo, planes);
            if (!yuvaPixmaps.isValid()) break;

            sk_sp<SkImage> image = SkImages::TextureFromYUVAPixmaps(
                rs.context.get(), yuvaPixmaps,
                skgpu::Mipmapped::kNo,
                false, nullptr
            );
            if (!image) break;

            SkSamplingOptions sampling = instruction.useSmoothing
                ? SkSamplingOptions(SkFilterMode::kLinear)
                : SkSamplingOptions(SkFilterMode::kNearest);

            rs.canvas->drawImageRect(
                image,
                SkRect::MakeXYWH(instruction.x, instruction.y,
                                 instruction.videoFrameWidth, instruction.videoFrameHeight),
                sampling
            );
            break;
        }
    }

    rs.context->flush();

#ifndef __EMSCRIPTEN__
    glfwSwapBuffers(rs.window);
#endif
}

#ifdef __EMSCRIPTEN__
// This runs on the main browser thread via emscripten_sync_run_in_main_runtime_thread
static void initWebGL(void* arg) {
    int* dims = (int*)arg;
    int width = dims[0];
    int height = dims[1];

    // Resize canvas to match SWF frame size
    emscripten_set_canvas_element_size("#canvas", width, height);

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.stencil = 1;
    attrs.antialias = 0;
    attrs.renderViaOffscreenBackBuffer = 1; // needed for pthread usage
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attrs);
    emscripten_webgl_make_context_current(ctx);

    auto interface = GrGLMakeNativeInterface();
    auto context = GrDirectContexts::MakeGL(interface);

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = 0;
    fbInfo.fFormat = GL_RGBA8;

    GrBackendRenderTarget target = GrBackendRenderTargets::MakeGL(width, height, 0, 8, fbInfo);
    auto surface = SkSurfaces::WrapBackendRenderTarget(
        context.get(), target,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        nullptr, nullptr
    );

    gRenderState.context = context;
    gRenderState.surface = surface;
    gRenderState.canvas = surface->getCanvas();

    // Start the main loop on the main thread
    emscripten_set_main_loop(renderFrame, 0, 0);
}
#endif

void render(RECT frameSize, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv) {
    int width = (frameSize.xMax - frameSize.xMin) / 20;
    int height = (frameSize.yMax - frameSize.yMin) / 20;

    gRenderState.renderStream = &renderStream;
    gRenderState.renderStreamMutex = &renderStreamMutex;
    gRenderState.renderCv = &renderCv;

#ifdef __EMSCRIPTEN__
    // WebGL context must be created on the main browser thread, not a pthread
    int dims[2] = {width, height};
    emscripten_sync_run_in_main_runtime_thread(EM_FUNC_SIG_VI, initWebGL, dims);
    // render thread can now exit — main loop keeps firing renderFrame
#else
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    GLFWwindow* window = glfwCreateWindow(width, height, "Skia", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gRenderState.window = window;

    auto interface = GrGLMakeNativeInterface();
    auto context = GrDirectContexts::MakeGL(interface);

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = 0;
    fbInfo.fFormat = GL_RGBA8;

    GrBackendRenderTarget target = GrBackendRenderTargets::MakeGL(width, height, 0, 8, fbInfo);
    auto surface = SkSurfaces::WrapBackendRenderTarget(
        context.get(), target,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        nullptr, nullptr
    );

    gRenderState.context = context;
    gRenderState.surface = surface;
    gRenderState.canvas = surface->getCanvas();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderFrame();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
#endif
}

void pushRendererInstruction(rendererInstruction instruction, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv) {
    std::lock_guard<std::mutex> renderLock(renderStreamMutex);
    renderStream.push_front(instruction);
    renderCv.notify_one();
}