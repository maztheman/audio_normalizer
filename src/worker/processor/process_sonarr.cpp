#include "process_sonarr.h"

#include "task/define_container.h"
#include "task/mkv_scan_contents.h"
#include "task/scan_file_for_container.h"

#include "task/context.h"
#include "task/sonarr_type.h"

#include <json/json.h>

#include <fmt/format.h>

#include <string>
#include <string_view>

using namespace std::string_view_literals;

using namespace anworker::task;

using JsonValue = Json::Value;
using JsonReader = Json::Reader;

namespace anworker::processor
{

static bool fromJson(sonarr_type &value, JsonValue &json);
static bool fromJson(series_t &value, JsonValue &json);
static bool fromJson(std::vector<episode_t> &value, JsonValue &json);
static bool fromJson(episode_t &value, JsonValue &json);
static bool fromJson(episode_file_t &value, JsonValue &json);

void process_sonarr(std::string &data)
{
    JsonReader reader;
    JsonValue document;
    if (reader.parse(data, document))
    {
        sonarr_type type;
        if (!fromJson(type, document))
        {
            return;
        }

        fmt::print(stderr, "Step 1: figure out what container this is\n");
        context ctx;
        define_container(type, ctx);

        if (ctx.containerType == container_type::other)
        {
            // call ffprobe
            scan_file_for_container(type, ctx);
        }

        fmt::print(stderr, "Step 2: figure out what this container contains.\n");

        switch (ctx.containerType)
        {
            using enum container_type;
        default:
        case none:
        case other:
            fmt::print(
              stderr, "cant figure out what container type {} is.\n", type.episodeFile.relativePath.generic_string());
            return;
        case mkv:
            fmt::print(stderr, "Using libmatroska to figure out contents\n");
            mkv_scan_contents(type, ctx);
            break;
        case mp4:
        case mpeg:
            fmt::print(stderr, "Using FFProbe to figure out contents\n");
            break;
        }

        fmt::print(stderr, "Step 3: do we need to transcode? prefer H265 or H264\n");

        fmt::print(stderr, "Step 4: Choose best audio that matches language [English]\n");

        // TODO: from codec, decide how to proceed.
        // Probably always use FFMpeg to downstereo it unless there is a better option (dts,hdaudio, etc) from eac3to ?

        // TODO: figure out how to run dynamic audio normlizer;


        // TODO: remux into a mp4 container using probably ffmpeg. Important to remux and not reencode at this point
        // TODO: remux 1 video 1 audio 1 subtitle, in a format that is Chromecast Ultra compatible
    }
    else
    {
        fmt::print(stderr, "Failed to parse sonarr json\n");
    }
}


static bool fromJson(sonarr_type &value, JsonValue &json)
{
    bool rc = true;
    if (auto member = "series"sv; !json.isMember(member.data()) || !json[member.data()].isObject())
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    rc = fromJson(value.series, json["series"]);
    if (auto member = "episodes"sv; !rc || !json.isMember(member.data()) || !json[member.data()].isArray())
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    rc = fromJson(value.episodes, json["episodes"]);
    if (auto member = "episodeFile"sv; !rc || !json.isMember(member.data()) || !json[member.data()].isObject())
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    if (auto member = "isUpgrade"sv; !rc || !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.isUpgrade = json["isUpgrade"].asBool();

    if (auto member = "downloadClient"sv; !rc || !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.downloadClient = json["downloadClient"].asString();
    if (auto member = "downloadClientType"sv; !rc || !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.downloadClientType = json["downloadClientType"].asString();
    if (auto member = "downloadId"sv; !rc || !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.downloadId = json["downloadId"].asString();
    if (auto member = "eventType"sv; !rc || !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.eventType = json["eventType"].asString();
    return true;
}

static bool fromJson(series_t &value, JsonValue &json)
{
    if (auto member = "id"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.id = json["id"].asUInt();
    if (auto member = "title"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.title = json["title"].asString();
    if (auto member = "path"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.path = json["path"].asString();
    if (auto member = "tvdbId"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.tvdbId = json["tvdbId"].asUInt();
    if (auto member = "tvMazeId"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.tvMazeId = json["tvMazeId"].asUInt();
    if (auto member = "imdbId"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.imdbId = json["imdbId"].asString();
    if (auto member = "type"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.type = json["type"].asString();

    return true;
}

static bool fromJson(std::vector<episode_t> &value, JsonValue &json)
{
    for (int index = 0; auto &arrayJson : json)
    {
        if (!arrayJson.isObject())
        {
            fmt::print(stderr, "episode array item {} is not an object\n", index);
            return false;
        }

        if (episode_t item; !fromJson(item, arrayJson))
        {
            return false;
        }
        else
        {
            value.push_back(std::move(item));
        }
        index++;
    }
}

static bool fromJson(episode_t &value, JsonValue &json)
{
    if (auto member = "id"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.id = json["id"].asUInt();
    if (auto member = "episodeNumber"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.episodeNumber = json["episodeNumber"].asUInt();
    if (auto member = "seasonNumber"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.seasonNumber = json["seasonNumber"].asUInt();
    if (auto member = "title"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.title = json["title"].asString();
    if (auto member = "airDate"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.airDate = json["airDate"].asString();
    if (auto member = "airDateUtc"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.airDateUtc = json["airDateUtc"].asString();
    return true;
}

static bool fromJson(episode_file_t &value, JsonValue &json)
{
    if (auto member = "id"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.id = json["id"].asUInt();
    if (auto member = "relativePath"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.relativePath = json["relativePath"].asString();
    if (auto member = "path"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.path = json["path"].asString();
    if (auto member = "quality"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.quality = json["quality"].asString();
    if (auto member = "qualityVersion"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.qualityVersion = json["qualityVersion"].asUInt();
    if (auto member = "releaseGroup"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.releaseGroup = json["releaseGroup"].asString();
    if (auto member = "sceneName"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.sceneName = json["sceneName"].asString();
    if (auto member = "size"sv; !json.isMember(member.data()))
    {
        fmt::print(stderr, "missing {} key\n", member);
        return false;
    }
    value.size = json["size"].asUInt64();
    return true;
}

}// namespace anworker::processor