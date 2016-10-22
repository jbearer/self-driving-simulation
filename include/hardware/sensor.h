#pragma once

#include <memory>
#include <functional>
#include <string>

#include "objects/objects.h"

namespace hardware
{
    struct sensor
    {
        virtual bool is_triggered() const = 0;

        virtual ~sensor() {};
    };

    struct sensor_factory
        : objects::object
    {
        virtual std::unique_ptr<sensor> create(int track_id) = 0;
    };
}
