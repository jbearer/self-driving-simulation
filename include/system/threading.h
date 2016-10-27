#pragma once

#include <functional>
#include <memory>
#include <string>

#include "system/time.h"

namespace sys {

    struct thread
    {
        virtual std::string name() const = 0;
        virtual void start() = 0;
        virtual void join() = 0;
        virtual void detach() = 0;
        virtual ~thread() {}
    };

    /**
     * @brief      Create a thread that repeats the given function for the duration of its life
     *             cycle. The loop will be interrupted if join is called, or if the thread object is
     *             disposed when detach has not been called.
     *
     * @param      name         Used by the logger to identify the thread.
     * @param      main         The function to repeat while the thread is alive.
     * @param      delay        The interval at which main should be repeated. If delay == 0, main
     *                          will be repeated as frequently as possible.
     */
    std::unique_ptr<thread> loop(
        std::string const & name, std::function<void()> const & main, useconds_t delay = 0);

    /**
     * @brief      Execute the given function and then return after a specified elapsed time.
     *             If more than the specified time elapses, a warning is logged, but the function
     *             returns normally.
     *
     * @param      name       An identifier used by the logger.
     * @param[in]  duration   The duration in microseconds.
     * @param[in]  func       The function to execute.
     *
     * @return                The duration in microseconds that the function actually took.
     */
    useconds_t timed_exec(std::string const & name, useconds_t duration, std::function<void()> func);
}
