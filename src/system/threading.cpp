#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "diagnostics/diag.h"
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
    typedef function<void(sys::useconds_t)> delay_func_t;

    // RAII wrapper to force a function call to take a certain amount of time
    struct enforce_delay
    {
        explicit enforce_delay(delay_func_t & delay_)
            : started( sys::now() )
            , delay(delay_)
        {}

        ~enforce_delay()
        {
            delay(started);
        }

    private:
        sys::useconds_t started;
        delay_func_t &  delay;
    };

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
            enforce_delay d(delay);
            work();
        }
    }

    diagnostics::prefix_logger      diag;
    function<void()>            work;
    std::thread                 inner;
    atomic<bool>                halt;

    // Accept a start time and wait until the time elapsed since start is 1/frequency.
    delay_func_t                delay;
};

unique_ptr<sys::thread> sys::loop(
    string const & name, function<void()> const & main, sys::useconds_t delay_time)
{
    return make_unique<loop_thread>(name, main, delay_time);
}

sys::useconds_t sys::timed_exec(string const & name, useconds_t duration, function<void()> func)
{
    useconds_t time = sys::now();
    func();
    time = useconds_since(time);
    if (time <= duration) {
        sys::sleep(duration - time);
    } else {
        diag.warn("timed_exec: {} exceeded {} us limit ({} us).", name, duration, time);
    }
    return useconds_since(time);
}
