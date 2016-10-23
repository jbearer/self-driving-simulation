#include <memory>

#include "hardware/raspi.h"

namespace hardware {

    struct output_device;
    struct input_device;

    struct pi_emu
        : raspi
    {
        struct input_handle
        {
            virtual void write(digital_val_t) = 0;
            virtual ~input_handle() {}
        };

        virtual void connect_device(int pin, std::shared_ptr<output_device> dev) = 0;
        virtual void connect_device(int pin, std::shared_ptr<input_device> dev) = 0;

        virtual void digital_write(int pin, digital_val_t value) = 0;
        virtual digital_val_t digital_read(int pin) const = 0;
        virtual void pin_mode(int pin, pin_mode_t mode) = 0;
    };

    struct output_device
    {
        virtual void write(raspi::digital_val_t) = 0;
        virtual ~output_device() {}
    };

    struct input_device
    {
        virtual void connected(std::unique_ptr<pi_emu::input_handle>) = 0;
        virtual ~input_device() {}
    };

    std::shared_ptr<pi_emu> emulate_raspi();
}
