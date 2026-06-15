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
#include "../base/shapeRecord.h"
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
#include "../../include/skia/include/core/SkPathBuilder.h"
#include "../../include/skia/include/core/SkSurface.h"
#include "../../include/skia/include/core/SkColorSpace.h"
#include "../../include/skia/include/core/SkYUVAPixmaps.h"
#include "../../include/skia/include/core/SkYUVAInfo.h"
#include "../../include/skia/include/gpu/ganesh/SkImageGanesh.h"
#include "../../include/skia/include/core/SkColorFilter.h"
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
    std::deque<rendererInstruction> lastBatch;
    float dpr = 1.0f;
#ifndef __EMSCRIPTEN__
    GLFWwindow* window;
#endif
};

static RenderState gRenderState;

alignas(16) float g_dpr = 1.0f;

extern "C" float* get_dpr_addr() { return &g_dpr; }


static void renderFrame() {
    auto& rs = gRenderState;

    std::unique_lock<std::mutex> renderLock(*rs.renderStreamMutex);
    if (rs.renderStream->empty()) return;

    rs.canvas->save();
    if (rs.dpr != 1.0f) rs.canvas->scale(rs.dpr, rs.dpr);

    bool hasRenderedBackground = false;

    while (!rs.renderStream->empty()) {

        rendererInstruction instruction = rs.renderStream->back();

        if (instruction.instructionCode == 1 && hasRenderedBackground) break;

        rs.renderStream->pop_back();
        renderLock.unlock();

        switch (instruction.instructionCode) {

            case 0:
            break;

            case 1:
                rs.canvas->clear(SkColorSetRGB(instruction.red, instruction.green, instruction.blue));
                rs.canvas->clipRect(SkRect::MakeWH(
                    (float)rs.surface->width()  / rs.dpr,
                    (float)rs.surface->height() / rs.dpr
                ));
                hasRenderedBackground = true;
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
            }
            break;

            case 3:
                if (!instruction.SWFShape || !instruction.SWFShape->picture) break;
                rs.canvas->save();
                rs.canvas->concat(instruction.canvasTransform);
                if (instruction.colorFilter) {
                    SkPaint layerPaint;
                    layerPaint.setColorFilter(instruction.colorFilter);
                    rs.canvas->saveLayer(nullptr, &layerPaint);
                    rs.canvas->drawPicture(instruction.SWFShape->picture);
                    rs.canvas->restore();
                } else {
                    rs.canvas->drawPicture(instruction.SWFShape->picture);
                }
                rs.canvas->restore();
            break;

            case 4:
            {
                if (!instruction.SWFMorphFrame) break;
                const Shape& frame = *instruction.SWFMorphFrame;

                rs.canvas->save();
                rs.canvas->concat(instruction.canvasTransform);

                if (instruction.colorFilter) {
                    SkPaint layerPaint;
                    layerPaint.setColorFilter(instruction.colorFilter);
                    rs.canvas->saveLayer(nullptr, &layerPaint);
                }

                for (int i = 0; i < (int)frame.FillPaths.size(); i++)
                    rs.canvas->drawPath(frame.FillPaths[i], frame.Fills[i]);

                for (int i = 0; i < (int)frame.LinePaths.size(); i++)
                    rs.canvas->drawPath(frame.LinePaths[i], frame.Lines[i]);

                if (instruction.colorFilter)
                    rs.canvas->restore();

                rs.canvas->restore();
            }
            break;

        }

        renderLock.lock();
    }

    renderLock.unlock();

    rs.canvas->restore();

    rs.context->flush();
    rs.context->submit();

#ifndef __EMSCRIPTEN__
    glfwSwapBuffers(rs.window);
#endif
}

#ifdef __EMSCRIPTEN__
static void initWebGL(void* arg) {
    int* dims = (int*)arg;
    int width  = dims[0];
    int height = dims[1];

    float dpr = g_dpr;
    int scaledWidth  = (int)(width  * dpr);
    int scaledHeight = (int)(height * dpr);

    emscripten_set_canvas_element_size("#canvas", scaledWidth, scaledHeight);

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.stencil = 1;
    attrs.antialias = 1;
    attrs.renderViaOffscreenBackBuffer = 1;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attrs);
    emscripten_webgl_make_context_current(ctx);

    auto interface = GrGLMakeNativeInterface();
    auto context = GrDirectContexts::MakeGL(interface);

    context->setResourceCacheLimit(256 * 1024 * 1024);

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = 0;
    fbInfo.fFormat = GL_RGBA8;

    GrBackendRenderTarget target = GrBackendRenderTargets::MakeGL(scaledWidth, scaledHeight, 0, 8, fbInfo);
    auto surface = SkSurfaces::WrapBackendRenderTarget(
        context.get(), target,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        nullptr, nullptr
    );

    gRenderState.context = context;
    gRenderState.surface = surface;
    gRenderState.canvas  = surface->getCanvas();
    gRenderState.dpr     = dpr;

    emscripten_set_main_loop(renderFrame, 0, 0);
}
#endif

void render(RECT frameSize, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv) {
    int width  = (frameSize.xMax - frameSize.xMin) / 20;
    int height = (frameSize.yMax - frameSize.yMin) / 20;

    gRenderState.renderStream      = &renderStream;
    gRenderState.renderStreamMutex = &renderStreamMutex;
    gRenderState.renderCv          = &renderCv;

#ifdef __EMSCRIPTEN__
    int dims[2] = {width, height};
    emscripten_sync_run_in_main_runtime_thread(EM_FUNC_SIG_VI, initWebGL, dims);
#else
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(width, height, "Skia", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gRenderState.window = window;

    auto interface = GrGLMakeNativeInterface();
    auto context   = GrDirectContexts::MakeGL(interface);

    context->setResourceCacheLimit(256 * 1024 * 1024);

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID  = 0;
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
    gRenderState.canvas  = surface->getCanvas();

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