#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "hardware/pi_emulator.h"
#include "logging/logging.h"
#include "system/system.h"

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/test.oscilloscope");

static const long           SAMPLING_RATE = 2e6;
static const long           WINDOW_SIZE   = 2000;
static const size_t         NUM_SAMPLES   = SAMPLING_RATE * WINDOW_SIZE * 1e-6;
static const int            PIN           = 0;

static sys::useconds_t      window_requested;

static bool                 callback_called = false;

static mutex                callback_lock;
static condition_variable   callback_signal;

void window_filled(oscilloscope::window_t && window)
{
    lock_guard<mutex> lock(callback_lock);

    diag.info("Window filled in {} seconds.", sys::seconds_since(window_requested));

    if (window.size() != NUM_SAMPLES) {
        diag.fail("Window did not contain expected number of samples.\n"
                  "Expected: {}\n"
                  "Actual: {}\n",
                  NUM_SAMPLES, window.size());
    }

    for (auto sample : window) {
        assert(sample == raspi::HIGH);
    }
    callback_called = true;

    callback_signal.notify_one();
}

int main()
{
    auto pi = emulate_raspi();
    pi->pin_mode(PIN, raspi::OUTPUT);
    auto scope = make_shared<oscilloscope>(SAMPLING_RATE);
    pi->connect_device(PIN, scope);

    pi->digital_write(PIN, raspi::HIGH);
    window_requested = sys::now();
    scope->fill_window(WINDOW_SIZE, &window_filled);

    unique_lock<mutex> lock(callback_lock);
    callback_signal.wait_for(lock, chrono::seconds(5), []() {
        return callback_called;
    });

    assert(callback_called);

    return 0;
}
