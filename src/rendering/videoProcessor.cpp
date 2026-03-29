#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <unordered_map>
#include <chrono>
#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif
#include "renderer.h"
#include "videoProcessor.h"
#include "../tags/tags.h"

AVCodecContext* codec_ctx = NULL;
AVFrame* frame = NULL;
AVPacket* pkt = NULL;
int configured_width = 0;
int configured_height = 0;
AVCodecID configured_codec = AV_CODEC_ID_NONE;

static void releaseVideoDecoder() {
    if (pkt != NULL) av_packet_free(&pkt);
    if (frame != NULL) av_frame_free(&frame);
    if (codec_ctx != NULL) avcodec_free_context(&codec_ctx);

    configured_width = 0;
    configured_height = 0;
    configured_codec = AV_CODEC_ID_NONE;
}

static AVCodecID mapSwfCodecToFFmpeg(uint8_t swfCodecId) {
    switch (swfCodecId) {
        case 2: return AV_CODEC_ID_FLV1;
        case 4: return AV_CODEC_ID_VP6F;
        case 5: return AV_CODEC_ID_VP6A;
        default: return AV_CODEC_ID_NONE;
    }
}

bool initializeVideoDecoder(uint8_t swfCodecId, int width, int height) {
    AVCodecID codec_id = mapSwfCodecToFFmpeg(swfCodecId);
    if (codec_id == AV_CODEC_ID_NONE || width <= 0 || height <= 0) return false;

    if (codec_ctx != NULL && configured_codec == codec_id &&
        configured_width == width && configured_height == height) {
        return true;
    }

    releaseVideoDecoder();

    const AVCodec* codec = avcodec_find_decoder(codec_id);
    if (codec == NULL) return false;

    codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx == NULL) return false;

    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->thread_count = 0;
    codec_ctx->thread_type = FF_THREAD_SLICE;

    AVDictionary* opts = NULL;
    av_dict_set(&opts, "flags2", "fast", 0);

    if (avcodec_open2(codec_ctx, codec, &opts) < 0) {
        av_dict_free(&opts);
        releaseVideoDecoder();
        return false;
    }
    av_dict_free(&opts);

    frame = av_frame_alloc();
    pkt = av_packet_alloc();
    if (frame == NULL || pkt == NULL) {
        releaseVideoDecoder();
        return false;
    }

    configured_width = width;
    configured_height = height;
    configured_codec = codec_id;
    return true;
}

rendererInstruction getVideoInstruction(
    int width, int height, int xPos, int yPos, bool useSmoothing,
    std::shared_ptr<std::vector<uint8_t>> yPlane,
    std::shared_ptr<std::vector<uint8_t>> uPlane,
    std::shared_ptr<std::vector<uint8_t>> vPlane,
    int yStride, int uStride, int vStride
) {
    rendererInstruction binOut;
    binOut.instructionCode = 2;
    binOut.videoFrameWidth = width;
    binOut.videoFrameHeight = height;
    binOut.x = xPos;
    binOut.y = yPos;
    binOut.useSmoothing = useSmoothing;
    binOut.yPlane = yPlane;
    binOut.uPlane = uPlane;
    binOut.vPlane = vPlane;
    binOut.yStride = yStride;
    binOut.uStride = uStride;
    binOut.vStride = vStride;
    return binOut;
}

YUVFrame decodeFrame(const std::vector<uint8_t>& data) {
    if (data.empty() || codec_ctx == NULL || pkt == NULL || frame == NULL) return {};

    av_packet_unref(pkt);
    pkt->data = (uint8_t*)data.data();
    pkt->size = static_cast<int>(data.size());

    int ret = avcodec_send_packet(codec_ctx, pkt);
    if (ret < 0) return {};

    ret = avcodec_receive_frame(codec_ctx, frame);
    if (ret == AVERROR(EAGAIN)) {
        ret = avcodec_send_packet(codec_ctx, NULL);
        if (ret < 0) return {};
        ret = avcodec_receive_frame(codec_ctx, frame);
    }
    if (ret < 0) return {};
    if (frame->width <= 0 || frame->height <= 0) return {};

    int ySize = frame->linesize[0] * frame->height;
    int uSize = frame->linesize[1] * (frame->height / 2);
    int vSize = frame->linesize[2] * (frame->height / 2);

    YUVFrame out;
    out.width   = frame->width;
    out.height  = frame->height;
    out.yStride = frame->linesize[0];
    out.uStride = frame->linesize[1];
    out.vStride = frame->linesize[2];
    out.yPlane  = std::make_shared<std::vector<uint8_t>>(frame->data[0], frame->data[0] + ySize);
    out.uPlane  = std::make_shared<std::vector<uint8_t>>(frame->data[1], frame->data[1] + uSize);
    out.vPlane  = std::make_shared<std::vector<uint8_t>>(frame->data[2], frame->data[2] + vSize);
    return out;
}