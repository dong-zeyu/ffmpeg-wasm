#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavutil/timestamp.h>
#include "wasm_io.h"
#include "decoding.h"

int main() {
    int ret = 0;
    AVFormatContext *fmt_ctx = NULL;
    AVIOContext *avio_ctx = NULL;
    avio_internal bd;
    uint8_t *buffer = NULL;
    size_t buffer_szie = 4096;
    AVCodec *codec;
    memset(&bd, 0, sizeof(bd));
    bd.url = "test.mp4";
    head(&bd);

    fmt_ctx = avformat_alloc_context();
    avio_ctx = avio_alloc_context(buffer, buffer_szie, 0, &bd, read_packet, NULL, seek_packet);
    fmt_ctx->pb = avio_ctx;
    avformat_open_input(&fmt_ctx, "http://", NULL, NULL);
    avformat_find_stream_info(fmt_ctx, NULL);
    av_dump_format(fmt_ctx, 0, "", 0);

    int video_stream_idx = -1, audio_stream_idx = -1;
    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;
        printf("Demuxing video\n");
    }

    if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        printf("Demuxing audio\n");
    }

    if (video_stream_idx == -1 && audio_stream_idx == -1) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    static AVPacket *pkt = NULL;
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate packet\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise
        // skip it
        if (pkt->stream_index == video_stream_idx)
            ret = decode_packet(video_dec_ctx, pkt);
        else if (pkt->stream_index == audio_stream_idx)
            ret = decode_packet(audio_dec_ctx, pkt);
        av_packet_unref(pkt);
        if (ret < 0)
            break;
    }

    /* flush the decoders */
    if (video_dec_ctx)
        decode_packet(video_dec_ctx, NULL);
    if (audio_dec_ctx)
        decode_packet(audio_dec_ctx, NULL);

    printf("Demuxing succeeded.\n");

end:
    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_freep(&avio_ctx->buffer);
    av_freep(&avio_ctx);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    printf("Stop\n");
}
