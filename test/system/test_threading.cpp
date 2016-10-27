#include "diagnostics/diag.h"
#include "system/threading.h"

#include "testing.h"

static diagnostics::logger diag("test.system.threading");

static void test_timed_exec(sys::useconds_t delay)
{
    sys::useconds_t error = 0;

    // Do a lot of work at times varying from very short to the maximum allowed time
    for (sys::useconds_t work_length = 0; work_length < delay; work_length += delay / 100)
    {
        error += sys::timed_exec("test.system.threading.timed_exec", delay, [work_length]() {
            sys::sleep(work_length);
        }) - work_length;
    }

    double avg_error = double(error) / delay;

    diag_assert(abs(avg_error) < 0.01 * delay,
                "timed_exec ({} us) exceeded maximum average error: {} us.", delay, avg_error);

    diag.info("timed_exec ({} us) passed with average error {} us.", delay, avg_error);
}

test_case(system.threading.timed_exec.short)
{
    test_timed_exec(500);
}

test_case(system.threading.timed_exec.long)
{
    test_timed_exec(5000);
}

static void calculate_timed_exec_overhead(sys::useconds_t delay)
{
    sys::useconds_t overhead = sys::timed_exec("test.system.threading.timed_exec.overhead", delay, [delay]() {
        sys::sleep(delay);
    }) - delay;

    diag.info("timed_exec ({} us) overhead = {}.", delay, overhead);
}

// Doesn't actually test anything, just calculates the overhead involved in calling timed_exec
test_case(system.threading.timed_exec.calculate_overhead)
{
    calculate_timed_exec_overhead(100);
    calculate_timed_exec_overhead(500);
    calculate_timed_exec_overhead(1000);
    calculate_timed_exec_overhead(5000);
}
