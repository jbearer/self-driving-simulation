#pragma once

#include "objects/objects.h"

namespace hardware
{
    struct raspi
        : objects::object
    {
        enum digital_val_t
        {
            LOW = 0,
            HIGH = 1
        };

        enum pin_mode_t
        {
            INPUT = 0,
            OUTPUT = 1
        };

        virtual digital_val_t digital_read(int pin) const = 0;
        virtual void digital_write(int pin, digital_val_t val) = 0;
        virtual void pin_mode(int pin, pin_mode_t mode) = 0;
    };
}
