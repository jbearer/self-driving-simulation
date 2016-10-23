#pragma once

#include <memory>

namespace hardware
{
    struct button
    {
        virtual int pin() const = 0;
        virtual bool is_pushed() const = 0;
        virtual ~button() {}
    };

    struct button_factory
    {
        virtual std::unique_ptr<button> create(int track_id) = 0;
    };
}
