#pragma once

#include <functional>
#include <string>

namespace hardware
{
    class sensor
    {
        /**
         * @brief      Register a callback to execute when the sensor passes a certain threshold.
         *
         * @param[in]  name       A unique name identifying the callback.
         */
        virtual void add_callback(std::string name, double threshold, std::function<void(void)>) = 0;

        /**
         * @brief      Removes a callback.
         *
         * @param[in]  name  The name passed to add_callback.
         */
        virtual void remove_callback(std::string name) = 0;
    }
}
