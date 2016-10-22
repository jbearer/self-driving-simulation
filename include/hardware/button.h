#pragma once

namespace hardware
{
    class button
    {
        virtual bool is_pushed() const = 0;

        virtual ~button() {}
    };
}
