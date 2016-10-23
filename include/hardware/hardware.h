#pragma once

namespace hardware
{
    struct device
    {
        virtual int pin() const = 0;
    };
}
