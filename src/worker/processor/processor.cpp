#include "processor.h"

#include "queue/queue.h"

#include <fmt/format.h>
#include "process_sonarr.h"

//TODO: add tracing to the processing

namespace anworker
{

void processor_run(std::atomic_bool& interrupter)
{
    for(;!interrupter.load();)
    {
        queue_item item;
        if (worker_queue().try_dequeue(item))
        {
            fmt::print(stderr, "Picked up a job type [{}]\n", to_string(item.type));
            if (item.type == item_type::sonarr)
            {
                processor::process_sonarr(item.data);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

}