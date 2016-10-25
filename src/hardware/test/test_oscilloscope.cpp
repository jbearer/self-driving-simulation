#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>

#include "hardware/pi_emulator.h"
#include "logging/logging.h"
#include "system/system.h"

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/test.oscilloscope");

static const long           SAMPLING_RATE = 1e4;
static const long           WINDOW_SIZE   = 2000;
static const size_t         NUM_SAMPLES   = SAMPLING_RATE * WINDOW_SIZE * 1e-6;
static const int            PIN           = 0;

template<raspi::digital_val_t expected_value>
void window_filled(oscilloscope::window_t && window)
{
    if (window.size() != NUM_SAMPLES) {
        diag.fail("Window did not contain expected number of samples.\n"
                  "Expected: {}\n"
                  "Actual: {}\n",
                  NUM_SAMPLES, window.size());
    }

    for (auto sample : window) {
        assert(sample == expected_value);
    }
}

struct callback
{
    explicit callback(oscilloscope::window_callback_t inner_)
        : inner(inner_)
        , called(false)
        , requested(0)
    {}

    void wait_for(int milliseconds)
    {
        unique_lock<mutex> l(lock);

        requested = sys::now();

        signal.wait_for(l, chrono::milliseconds(milliseconds), [this]() {
            return called;
        });
        assert(called);
    }

    oscilloscope::window_callback_t bind()
    {
        return [this](oscilloscope::window_t && window) {
            lock_guard<mutex> l(lock);

            if (requested) {
                double wait_time = sys::seconds_since(requested);
                diag.info("Callback called. Waited {} seconds / {} windows.",
                          wait_time, wait_time * 1e6 / WINDOW_SIZE);
            }

            inner( move(window) );
            called = true;
            signal.notify_one();
        };
    }

    bool was_called() const
    {
        lock_guard<mutex> l(lock);
        return called;
    }

private:
    oscilloscope::window_callback_t     inner;
    bool                                called;
    mutable mutex                       lock;
    condition_variable                  signal;
    sys::useconds_t                     requested;
};

int main()
{
    logging::set_level(logging::log_level::trace);

    auto pi = emulate_raspi();
    pi->pin_mode(PIN, raspi::OUTPUT);
    auto scope = make_shared<oscilloscope>(SAMPLING_RATE);
    pi->connect_device(PIN, scope);

    pi->digital_write(PIN, raspi::HIGH);

    callback immediate(window_filled<raspi::HIGH>);
    callback trigger(window_filled<raspi::LOW>);

    diag.info("Requesting immediate window.");
    scope->fill_window("immediate", WINDOW_SIZE, immediate.bind());
    immediate.wait_for(5000);

    assert( !trigger.was_called() );

    diag.info("Requesting trigger window.");
    scope->fill_window("trigger", WINDOW_SIZE, raspi::LOW, trigger.bind());

    // Wait a bit while still writing high and make sure we don't get triggered spuriously
    sys::sleep(1e6);
    assert( !trigger.was_called() );

    diag.info("Activating trigger.");
    pi->digital_write(PIN, raspi::LOW);
    trigger.wait_for(5000);

    return 0;
}
