#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "logging/logging.h"
#include "hardware/raspi.h"
#include "hardware/sensor.h"
#include "objects/objects.h"

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/sensor");

// Mapping from track_id_s to pins
static unordered_map<int, int> pins = {
    {0, 0},
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {5, 5}
};

struct sensor_impl
    : sensor
{
    explicit sensor_impl(int pin_)
        : pi( objects::get<raspi>() )
        , pin_id(pin_)
    {
        pi->pin_mode(pin_id, raspi::INPUT);
    }

    int pin() const
    {
        return pin_id;
    }

    bool is_triggered() const
    {
        return pi->digital_read(pin_id) == raspi::HIGH;
    }

private:
    shared_ptr<raspi>   pi;
    int                 pin_id;
};

struct mock_sensor
    : sensor
{
    int pin() const
    {
        return 0;
    }

    bool is_triggered() const
    {
        return false;
    }
};

struct sensor_factory_impl
    : sensor_factory
{
    unique_ptr<sensor> create(int track_id)
    {
        int pin = pins[track_id];
        return unique_ptr<sensor>( static_cast<sensor *>( new sensor_impl(pin) ) );
    }
};

struct mock_sensor_factory
    : sensor_factory
{
    unique_ptr<sensor> create(int)
    {
        return unique_ptr<sensor>( static_cast<sensor *>(new mock_sensor) );
    }
};

register_object(sensor_factory, sensor_factory_impl);
register_mock_object(sensor_factory, mock_sensor_factory);
