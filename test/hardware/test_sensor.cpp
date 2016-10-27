#include <memory>

#include "diagnostics/diag.h"
#include "hardware/pi_emulator.h"
#include "hardware/sensor.h"
#include "diagnostics/diag.h"
#include "objects/objects.h"
#include "system/system.h"

#include "testing.h"

using namespace std;
using namespace hardware;

static diagnostics::logger diag("test/sensor");

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

void setup(shared_ptr<sensor_emulator> & sensor_emu, unique_ptr<sensor> & sensor)
{
    auto pi = emulate_raspi();
    auto factory = objects::get<sensor_factory>();
    sensor = move( factory->create(0) );

    // Connect a sensor to the emulator
    sensor_emu.reset(new sensor_emulator);
    pi->connect_device(sensor->pin(), sensor_emu);
    while ( !sensor_emu->connected() ) {
        sys::sleep(1e6);
    }
}

test_case(hardware.sensor.trigger)
{
    shared_ptr<sensor_emulator> sensor_emu;
    unique_ptr<sensor>          sensor;
    setup(sensor_emu, sensor);

    diag_assert( !sensor->is_triggered() );
    sensor_emu->trigger();
    diag_assert( sensor->is_triggered() );
}

test_case(hardware.sensor.untrigger)
{
    shared_ptr<sensor_emulator> sensor_emu;
    unique_ptr<sensor>          sensor;
    setup(sensor_emu, sensor);

    sensor_emu->trigger();
    diag_assert( sensor->is_triggered() );
    sensor_emu->untrigger();
    diag_assert( !sensor->is_triggered() );
}
