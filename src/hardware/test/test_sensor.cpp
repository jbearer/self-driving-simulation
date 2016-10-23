#include <cassert>
#include <memory>
#include <unistd.h>

#include "hardware/pi_emulator.h"
#include "hardware/sensor.h"
#include "logging/logging.h"
#include "objects/objects.h"

using namespace std;
using namespace hardware;

static logging::logger diag("test/sensor");

struct sensor_emulator
    : input_device
{

    void connected(unique_ptr<pi_emu::input_handle> handle_)
    {
        handle = move(handle_);
    }

    void trigger()
    {
        assert(handle);
        handle->write(raspi::HIGH);
    }

    void untrigger()
    {
        assert(handle);
        handle->write(raspi::LOW);
    }

    bool connected() const
    {
        return bool(handle);
    }

private:
    unique_ptr<pi_emu::input_handle> handle;
};

int main()
{
    auto pi = emulate_raspi();
    auto factory = objects::get<sensor_factory>();
    auto sensor = factory->create(0);

    // Connect a sensor to the emulator
    shared_ptr<sensor_emulator> sensur_emu(new sensor_emulator);
    pi->connect_device(sensor->pin(), sensur_emu);
    while ( !sensur_emu->connected() ) {
        usleep(1e6);
    }

    // Push the button and see if the software responds accordingly
    sensur_emu->trigger();
    assert( sensor->is_triggered() );
    usleep(1e6);
    assert( sensor->is_triggered() );
    sensur_emu->untrigger();
    assert( !sensor->is_triggered() );
    usleep(1e6);
    assert( !sensor->is_triggered() );

    return 0;
}
