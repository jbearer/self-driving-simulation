#include <functional>
#include <memory>
#include <string>

#include "logging/logging.h"
#include "hardware/sensor.h"

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/sensor");

struct mock_sensor
    : sensor
{
    void add_callback(string, double, function<void()>) {}
    void remove_callback(string) {}
};

struct mock_sensor_factory
    : sensor_factory
{
    unique_ptr<sensor> create(int)
    {
        return unique_ptr<sensor>( static_cast<sensor *>(new mock_sensor) );
    }
};

register_object(sensor_factory, mock_sensor_factory);
