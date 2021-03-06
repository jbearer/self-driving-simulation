#pragma once

#include <memory>

#include "hardware/hardware.h"

namespace hardware
{
    struct button
        : device
    {
        virtual bool is_pushed() const = 0;
        virtual ~button() {}
    };

    struct button_factory
    {
        virtual std::unique_ptr<button> create(int track_id) = 0;
    };
}
