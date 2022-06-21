#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

extern AVCodecContext *video_dec_ctx;
extern AVCodecContext *audio_dec_ctx;
extern AVFrame *frame;
extern int width, height;
extern enum AVPixelFormat pix_fmt;

int decode_packet(AVCodecContext *dec, const AVPacket *pkt);

int open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);
