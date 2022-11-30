#pragma once

#include <string_view>

namespace anworker
{
    enum class item_type
    {
        unknown,
        sonarr,
        radarr
    };

    inline std::string_view to_string(item_type item)
    {
        using namespace std::string_view_literals;
        switch(item)
        {
            using enum item_type;
            default:
            case unknown:
                return "unknown"sv;
            case sonarr:
                return "sonarr"sv;
            case radarr:
                return "radarr"sv;
        }
    }
}