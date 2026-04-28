#include <iostream>
#include <cstdint>
#include <vector>
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
}
#endif
#include "audioProcessor.h"

AVCodecContext* audio_codec_ctx = NULL;
AVFrame* audio_frame = NULL;
AVPacket* audio_pkt = NULL;
SwrContext* swr_ctx = NULL;
static uint8_t current_channels = 1;
static uint8_t current_compression = 0;
static AVSampleFormat target_format = AV_SAMPLE_FMT_S16;
static int target_sample_rate = 44100;

static void releaseAudioDecoder() {
    if (audio_pkt != NULL) av_packet_free(&audio_pkt);
    if (audio_frame != NULL) av_frame_free(&audio_frame);
    if (audio_codec_ctx != NULL) avcodec_free_context(&audio_codec_ctx);
    if (swr_ctx != NULL) { swr_free(&swr_ctx); swr_ctx = NULL; }
    audio_codec_ctx = NULL;
    audio_frame = NULL;
    audio_pkt = NULL;
}

int mapSwfSampleRate(uint8_t rate) {
    switch (rate) {
        case 0: return 5500;
        case 1: return 11025;
        case 2: return 22050;
        case 3: return 44100;
        default: return 44100;
    }
}

static AVCodecID mapSwfAudioCodec(uint8_t compression) {
    switch (compression) {
        case 0: return AV_CODEC_ID_PCM_U8;
        case 1: return AV_CODEC_ID_ADPCM_SWF;
        case 2: return AV_CODEC_ID_MP3;
        case 3: return AV_CODEC_ID_PCM_S16LE;
        case 5: return AV_CODEC_ID_NELLYMOSER;
        case 6: return AV_CODEC_ID_NELLYMOSER;
        case 11: return AV_CODEC_ID_SPEEX;
        default: return AV_CODEC_ID_NONE;
    }
}

bool initializeAudioDecoder(uint8_t streamSoundCompression, uint8_t streamSoundRate, uint8_t streamSoundSize, uint8_t streamSoundType, int16_t latencySeek) {
    releaseAudioDecoder();

    AVCodecID codec_id = mapSwfAudioCodec(streamSoundCompression);
    if (codec_id == AV_CODEC_ID_NONE) return false;

    const AVCodec* codec = avcodec_find_decoder(codec_id);
    if (codec == NULL) return false;

    audio_codec_ctx = avcodec_alloc_context3(codec);
    if (audio_codec_ctx == NULL) return false;

    target_sample_rate = mapSwfSampleRate(streamSoundRate);
    audio_codec_ctx->sample_rate = target_sample_rate;
    audio_codec_ctx->ch_layout.nb_channels = streamSoundType + 1;
    audio_codec_ctx->bits_per_coded_sample = streamSoundSize ? 16 : 8;
    current_channels = streamSoundType + 1;
    current_compression = streamSoundCompression;

    target_format = (streamSoundCompression == 0) ? AV_SAMPLE_FMT_U8 : AV_SAMPLE_FMT_S16;

    if (avcodec_open2(audio_codec_ctx, codec, nullptr) < 0) {
        releaseAudioDecoder();
        return false;
    }

    audio_frame = av_frame_alloc();
    audio_pkt = av_packet_alloc();
    if (audio_frame == NULL || audio_pkt == NULL) {
        releaseAudioDecoder();
        return false;
    }

    return true;
}

std::vector<uint8_t> decodeAudioFrame(const std::vector<uint8_t>& data) {
    if (data.empty() || audio_codec_ctx == NULL || audio_pkt == NULL || audio_frame == NULL) return {};

    av_packet_unref(audio_pkt);
    audio_pkt->data = (uint8_t*)data.data();
    audio_pkt->size = static_cast<int>(data.size());

    int ret = avcodec_send_packet(audio_codec_ctx, audio_pkt);
    if (ret < 0 && ret != AVERROR(EAGAIN)) return {};

    std::vector<uint8_t> result;
    while (true) {
        ret = avcodec_receive_frame(audio_codec_ctx, audio_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
        if (ret < 0) break;

        if (swr_ctx == NULL) {
            AVChannelLayout ch_layout;
            av_channel_layout_default(&ch_layout, current_channels);

            swr_alloc_set_opts2(&swr_ctx,
                &ch_layout, target_format,                          target_sample_rate,
                &ch_layout, (AVSampleFormat)audio_frame->format,   audio_frame->sample_rate,
                0, NULL);

            if (swr_init(swr_ctx) < 0) {
                swr_free(&swr_ctx);
                swr_ctx = NULL;
                break;
            }
        }

        int outSamples = swr_get_out_samples(swr_ctx, audio_frame->nb_samples);
        int bytesPerSample = av_get_bytes_per_sample(target_format);
        int bufSize = outSamples * current_channels * bytesPerSample;

        std::vector<uint8_t> buf(bufSize);
        uint8_t* outPtr = buf.data();

        int converted = swr_convert(swr_ctx, &outPtr, outSamples,
                                    (const uint8_t**)audio_frame->data, audio_frame->nb_samples);
        if (converted > 0) {
            int actualSize = converted * current_channels * bytesPerSample;
            result.insert(result.end(), buf.begin(), buf.begin() + actualSize);
        }
    }

    return result;
}