#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "logging/logging.h"
#include "system/threading.h"
#include "system/time.h"

using std::atomic;
using std::atomic_load;
using std::atomic_store;
using std::bind;
using std::function;
using std::make_unique;
using std::string;
using std::unique_ptr;

using namespace sys;

struct loop_thread
        : sys::thread
{
    loop_thread(string const & name_, function<void()> const & main_, sys::useconds_t delay_time)
        : diag("system.threading", name_)
        , work(main_)
        , halt(false)
    {
        // Set up a delay function
        if (delay_time == 0) {
            // No delay, loop as fast as we can
            delay = [](sys::useconds_t){};
        } else {
            // Get the elapsed time, and delay for the remainder of the delay time
            delay = [this, delay_time](sys::useconds_t started) {
                register sys::useconds_t delay_time_left = delay_time - useconds_since(started);
                if (delay_time_left >= 0) {
                    sys::sleep(delay_time_left);
                } else {
                    diag.warn("Delay time exceeded.");
                }
            };
        }
    }

    ~loop_thread()
    {
        join();
    }

    string name() const
    {
        return diag.name();
    }

    void start()
    {
        inner = std::thread( bind(&loop_thread::main, this) );
    }

    void join()
    {
        atomic_store(&halt, true);
        inner.join();
    }

    void detach()
    {
        inner.detach();
    }

private:
    void main()
    {
        while ( !atomic_load(&halt) ) {
            sys::useconds_t started = sys::now();
            work();
            delay(started);
        }
    }

    logging::prefix_logger      diag;
    function<void()>            work;
    std::thread                 inner;
    atomic<bool>                halt;

    // Accept a start time and wait until the time elapsed since start is 1/frequency.
    function<void(sys::useconds_t)>  delay;
};

unique_ptr<sys::thread> sys::loop(
    string const & name, function<void()> const & main, sys::useconds_t delay_time)
{
    return make_unique<loop_thread>(name, main, delay_time);
}
