#include "define_container.h"

#include "context.h"
#include "sonarr_type.h"

#include <fmt/format.h>

#include <filesystem>
namespace fs = std::filesystem;

namespace anworker::task
{

static std::string &&to_lower(std::string &&value)
{
    for (auto &c : value)
    {
        c = static_cast<char>(::tolower(c));
    }
    return std::move(value);
}


void define_container(sonarr_type &type, context &context)
{
    fs::path fullPath = type.series.path / type.episodeFile.relativePath;

    if (!fs::exists(fullPath))
    {
        context.containerType = container_type::none;
        fmt::print(stderr, "file {} doesn't exist\n", fullPath.generic_string());
        return;
    }

    if (fullPath.has_extension())
    {
        if (auto ext = to_lower(fullPath.extension().generic_string()); ext == ".mkv")
        {
            // we could try executing ffprobe to verify...?
            context.containerType = container_type::mkv;
        }
        else if (ext == ".mp4")
        {
            context.containerType = container_type::mp4;
        }
        else if (ext == ".mpeg" || ext == ".mpg")
        {
            context.containerType = container_type::mpeg;
        }
        else
        {
            context.containerType = container_type::other;
        }
    }
    else
    {
        context.containerType = container_type::none;
    }
}


}// namespace anworker::task