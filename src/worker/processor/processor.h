#pragma once

#include <atomic>

namespace anworker
{
   /**
     * main program loop for the processor
    */
    void processor_run(std::atomic_bool& interrupter);
}