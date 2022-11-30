#pragma once

#include "item_type.h"

#include <string>

namespace anworker
{
    struct queue_item
    {
        item_type type;
        std::string data;
    };
}