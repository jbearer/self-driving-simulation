#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unordered_map>
#include <vector>

#include "hardware/pi_emulator.h"
#include "hardware/raspi.h"
#include "logging/logging.h"
#include "objects/objects.h"

#define GPFSEL   ((volatile unsigned int *) (gpio + 0))
#define GPSET    ((volatile unsigned int *) (gpio + 7))
#define GPCLR    ((volatile unsigned int *) (gpio + 10))
#define GPLEV    ((volatile unsigned int *) (gpio + 13))

#define BCM2836_PERI_BASE 0x3F000000
#define GPIO_BASE         BCM2836_PERI_BASE + 0x200000
#define BLOCK_SIZE           (4*1024)

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/raspi");

struct raspi_impl
    : raspi
{
    raspi_impl()
    {
        pio_init();
        if (!gpio)
            diag.fail("Failed to initialize GPIO.");
    }

    void pin_mode(int pin, pin_mode_t fn)
    {
        diag.info("Set pin {} to mode {}.", pin, fn);

        int reg = pin / 10;                             // determines which register, 0-5
        int offset = (pin % 10) * 3;                    // determines offset, 0-9
        GPFSEL[reg] &= ~( (0b111 & fn) << offset);      // clear bits
        GPFSEL[reg] |=  ( ( 0b111 & fn) << offset);     // set bits
    }

    void digital_write(int pin, digital_val_t value)
    {
        int reg = pin / 32;                             // determines i for GPSET[i] and GPCLR[i]
        int offset = pin % 32;                          // determines offset in GPSET[i]/GPCLR[i]

        switch(value) {
            case HIGH:
                GPSET[reg] |= (0x1 << offset); break;
            case LOW:
                GPCLR[reg] |= (0x1 << offset); break;
        }
    }

    digital_val_t digital_read(int pin) const
    {
        int reg = pin / 32;
        int offset = pin % 32;
        return static_cast<digital_val_t>( (GPLEV[reg] >> offset) & 0x1 );
    }

private:

    // setup access to GPIO and system_timer, navigating physcial/virtual memory mapping
    void pio_init()
    {
        int     mem_fd;
        void *  reg_map;

        // /dev/mem is a psuedo-driver for accessing memory in the Linux filesystem
        if ( (mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
              diag.fail("can't open /dev/mem");
        }

        reg_map = mmap(
          NULL,                     // Address at which to start local mapping (null means don't-care)
          BLOCK_SIZE,               // Size of mapped memory block
          PROT_READ|PROT_WRITE,     // Enable both reading and writing to the mapped memory
          MAP_SHARED,               // This program does not have exclusive access to this memory
          mem_fd,                   // Map to /dev/mem
          GPIO_BASE);               // Offset to GPIO peripheral

        if (reg_map == MAP_FAILED) {
          diag.fail("gpio mmap error {}", reg_map);
        }

        gpio = static_cast<volatile unsigned *>(reg_map);
    }

    //pointer to base of gpio
    volatile unsigned int * gpio = nullptr;
};

struct mock_raspi;

struct input_handle_impl
    : pi_emu::input_handle
{
    input_handle_impl(mock_raspi & parent_, int pin_)
        : parent(parent_)
        , pin(pin_)
    {}

    void write(raspi::digital_val_t value);

private:
    mock_raspi &            parent;
    int                     pin;
};

struct mock_raspi
    : pi_emu
{
    void connect_device(int pin, std::shared_ptr<output_device> dev)
    {
        if (inputs.count(pin) || outputs.count(pin)) {
            diag.fail("A device is already connected to pin {}.", pin);
        }
        if (pin_modes.count(pin) == 0 || pin_modes[pin] != OUTPUT) {
            diag.warn("Connected output device to pin {}, which is not configured for output.", pin);
        }
        outputs[pin] = dev;
    }

    void connect_device(int pin, std::shared_ptr<input_device> dev)
    {
        if (inputs.count(pin) || outputs.count(pin)) {
            diag.fail("A device is already connected to pin {}.", pin);
        }
        if (pin_modes.count(pin) == 0 || pin_modes[pin] != INPUT) {
            diag.warn("Connected input device to pin {}, which is not configured for input.", pin);
        }
        inputs[pin] = LOW;  // Pins are low until explicitly written to
        dev->connected( unique_ptr<input_handle>(
            static_cast<input_handle *>( new input_handle_impl(*this, pin) ) ) );
    }

    void digital_write(int pin, digital_val_t value)
    {
        if ( !pin_modes.count(pin) ) {
            diag.fail("Attempted to write to pin {} before configuring for output.", pin);
        }
        if (pin_modes[pin] != OUTPUT) {
            diag.fail("Attempted to write to input pin {}.", pin);
        }

        if ( outputs.count(pin) ) {
            outputs[pin]->write(value);
        } else {
            diag.warn("Writing to unconnected pin {}.", pin);
        }
    }

    digital_val_t digital_read(int pin) const
    {
        auto it = pin_modes.find(pin);
        if ( it == pin_modes.end() ) {
            diag.fail("Attempted to read pin {} before configuring for input.", pin);
        }
        if (it->second != OUTPUT) {
            diag.fail("Attempted to read output pin {}.", pin);
        }

        auto reading = inputs.find(pin);
        if ( reading != inputs.end() ) {
            return reading->second;
        } else {
            diag.warn("Reading from unconnected pin {}.", pin);
            return LOW;
        }
    }

    void pin_mode(int pin, pin_mode_t mode)
    {
        pin_modes[pin] = mode;
    }

private:
    template<typename val_t>
    using pin_map = unordered_map<int, val_t>;

    pin_map<pin_mode_t>                     pin_modes;
    pin_map<digital_val_t>                  inputs;     // Last value seen from input device
    pin_map< shared_ptr<output_device> >    outputs;    // Output devices

    friend struct input_handle_impl;
};

shared_ptr<pi_emu> hardware::emulate_raspi()
{
    objects::mock<raspi>();
    auto itf_ptr = objects::get<raspi>();
    return shared_ptr<pi_emu>( itf_ptr, static_cast<pi_emu*>(itf_ptr.get()) );
}

void input_handle_impl::write(raspi::digital_val_t value)
{
     parent.inputs[pin] = value;
}

register_object(raspi, raspi_impl);
register_mock_object(raspi, mock_raspi);
