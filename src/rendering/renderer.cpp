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
#include <GLFW/glfw3.h>
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


void render(RECT frameSize, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv) {
    int width = (frameSize.xMax - frameSize.xMin) / 20;
    int height = (frameSize.yMax - frameSize.yMin) / 20;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    GLFWwindow* window = glfwCreateWindow(width, height, "Skia", nullptr, nullptr);
    glfwMakeContextCurrent(window);

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

    SkCanvas* canvas = surface->getCanvas();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        std::unique_lock<std::mutex> renderLock(renderStreamMutex);
        renderCv.wait(renderLock, [&] { return !renderStream.empty(); });
        if (renderStream.empty()) continue;

        rendererInstruction instruction = renderStream.back();
        renderStream.pop_back();
        renderLock.unlock();

        switch (instruction.instructionCode) {
            case 0:
            case 1:
                canvas->clear(SkColorSetRGB(instruction.red, instruction.green, instruction.blue));
                break;

            case 2: {
                if (!instruction.yPlane || !instruction.uPlane || !instruction.vPlane) break;

                SkYUVAInfo yuvaInfo(
                    {instruction.videoFrameWidth, instruction.videoFrameHeight},
                    SkYUVAInfo::PlaneConfig::kY_U_V,
                    SkYUVAInfo::Subsampling::k420,
                    kRec601_Limited_SkYUVColorSpace
                );

                // Build one pixmap per plane with correct stride
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
                    context.get(), yuvaPixmaps,
                    skgpu::Mipmapped::kNo,
                    false, nullptr
                );
                if (!image) break;

                SkSamplingOptions sampling = instruction.useSmoothing
                    ? SkSamplingOptions(SkFilterMode::kLinear)
                    : SkSamplingOptions(SkFilterMode::kNearest);

                canvas->drawImageRect(
                    image,
                    SkRect::MakeXYWH(instruction.x, instruction.y,
                                     instruction.videoFrameWidth, instruction.videoFrameHeight),
                    sampling
                );
                break;
            }
        }

        context->flush();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void pushRendererInstruction(rendererInstruction instruction, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv) {
    std::lock_guard<std::mutex> renderLock(renderStreamMutex);
    renderStream.push_front(instruction);
    renderCv.notify_one();
}