#pragma once

#include <memory>

#include "objects/objects.h"

namespace hardware
{
    struct button
    {
        virtual bool is_pushed() const = 0;

        virtual ~button() {}
    };

    struct button_factory
        : objects::object
    {
        virtual std::unique_ptr<button> create(int track_id);
    };
}
