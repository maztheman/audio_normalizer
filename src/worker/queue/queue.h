#pragma once

#include "queue_item.h"

#include <concurrentqueue.h>

namespace anworker
{
    using queue = moodycamel::ConcurrentQueue<queue_item>;

    inline queue& worker_queue()
    {
        //long live the queue.
        static queue instance;
        return instance;
    }
}
