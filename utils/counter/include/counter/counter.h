//
// Created by tripack on 17-3-26.
//

#ifndef VE490_LOOP_COUNTER_H
#define VE490_LOOP_COUNTER_H

#include <string>
#include <cinttypes>

namespace ECTL::Utils {

    /* This is a side-effect based counter
     * This abstracts interface to use a number of mapped
     * counters. Counters are globally shared, referenced by name
     * This counter, on construction, will increase an internal
     * counter by 1.
     * Can be used easily to keep track of number of loops
     * Counter is none-copyable or moveable */

    class LoopCounter {
        std::string name;

    public:
        explicit LoopCounter(std::string name, bool inc=true);

        LoopCounter(const LoopCounter&)            = delete;
        LoopCounter(LoopCounter&&)                 = delete;
        LoopCounter& operator=(const LoopCounter&) = delete;
        LoopCounter& operator=(LoopCounter&&)      = delete;

        int32_t     step()  const; // Increase counter and return new value
        int32_t     value() const; // Return current value
        void        reset() const; // Sets current counter value to zero

        static int32_t     step (const std::string& name); // Increase counter and return new value
        static int32_t     value(const std::string& name); // Return current value
        static void        reset(const std::string& name); // Sets current counter value to zero
        static void        clear(const std::string& name); // Clears current counter to none-exist

        // Check existence of a counter
        // Prints if the counter does not exist
        static bool        exist(const std::string& name);

        // Pretty-prints one specific counter value / all counters
        static void        show    (const std::string& name);
        static void        showall (const std::string& name);

        // Support usage 'if ( a == b && counter())'
        // Always returns TRUE
        operator bool()     const;
    };

}

#endif //VE490_RESOURCE_H
