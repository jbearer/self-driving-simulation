#pragma once

#include <memory>
#include <functional>
#include <string>

#include "hardware/hardware.h"

namespace hardware
{
    struct sensor
        : device
    {
        virtual bool is_triggered() const = 0;

        virtual ~sensor() {};
    };

    struct sensor_factory
    {
        virtual std::unique_ptr<sensor> create(int track_id) = 0;
    };
}
