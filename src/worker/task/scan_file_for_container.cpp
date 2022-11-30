#include "scan_file_for_container.h"

#include "context.h"
#include "sonarr_type.h"

#include <fmt/format.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <filesystem>

namespace fs = std::filesystem;

// https://ffmpeg.org/doxygen/trunk/demuxing_decoding_8c-example.html
// so we have just a small portion before we dont even care about the container just get the stuff inside


namespace anworker::task
{

static int open_codec_context(fs::path srcPath,
  int *stream_idx,
  AVCodecContext **dec_ctx,
  AVFormatContext *fmt_ctx,
  enum AVMediaType type);


void scan_file_for_container(sonarr_type &type, context &context)
{
    fs::path fullPath = type.series.path / type.episodeFile.relativePath;

    auto url = fmt::format("file:{}", fullPath.generic_string());

    AVFormatContext *s = NULL;
    int ret = avformat_open_input(&s, url.c_str(), NULL, NULL);
    if (ret < 0)
    {
        fmt::print(stderr, "libav failed to open file\n");
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(s, NULL) < 0)
    {
        fmt::print(stderr, "Could not find stream information\n");
    }

    int video_stream_idx = -1, audio_stream_idx = -1, subtitle_stream_idx = -1;
    AVCodecContext *video_dec_ctx = nullptr, *audio_dec_ctx = nullptr, *subtitle_dec_ctx = nullptr;

    // we can use this to actually demux which is awsome.

    if (open_codec_context(fullPath, &video_stream_idx, &video_dec_ctx, s, AVMEDIA_TYPE_VIDEO) >= 0)
    {
        auto video_stream = s->streams[video_stream_idx];
    }

    if (open_codec_context(fullPath, &audio_stream_idx, &audio_dec_ctx, s, AVMEDIA_TYPE_AUDIO) >= 0)
    {
        auto audio_stream = s->streams[audio_stream_idx];
    }

    if (open_codec_context(fullPath, &subtitle_stream_idx, &subtitle_dec_ctx, s, AVMEDIA_TYPE_SUBTITLE) >= 0)
    {
        auto subtitle_stream = s->streams[subtitle_stream_idx];
    }

    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avcodec_free_context(&subtitle_dec_ctx);
    avformat_close_input(&s);
}

static int open_codec_context(fs::path srcPath,
  int *stream_idx,
  AVCodecContext **dec_ctx,
  AVFormatContext *fmt_ctx,
  enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    const AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        fmt::print(stderr,
          "Could not find {} stream in input file '{}'\n",
          av_get_media_type_string(type),
          srcPath.generic_string());
        return ret;
    }
    else
    {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec)
        {
            fprintf(stderr, "Failed to find %s codec\n", av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx)
        {
            fprintf(stderr, "Failed to allocate the %s codec context\n", av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0)
        {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n", av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0)
        {
            fprintf(stderr, "Failed to open %s codec\n", av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

}// namespace anworker::task
