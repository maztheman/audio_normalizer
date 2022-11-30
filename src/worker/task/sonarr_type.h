#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace anworker::task
{

struct series_t
{
    uint32_t id;
    std::string title;
    fs::path path;
    uint32_t tvdbId;
    uint32_t tvMazeId;
    std::string imdbId;
    std::string type;
};

struct episode_t
{
    uint32_t id;
    uint32_t episodeNumber;
    uint32_t seasonNumber;
    std::string title;
    std::string airDate;
    std::string airDateUtc;
};

struct episode_file_t
{
    uint32_t id;
    //relative path under the series.path
    fs::path relativePath;
    //download dir
    fs::path path;
    std::string quality;
    uint32_t qualityVersion;
    std::string releaseGroup;
    std::string sceneName;
    size_t size;
};

struct sonarr_type
{
    series_t series;
    std::vector<episode_t> episodes;
    episode_file_t episodeFile;
    bool isUpgrade;
    std::string downloadClient;
    std::string downloadClientType;
    std::string downloadId;
    std::string eventType;
};
}