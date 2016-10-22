#include <fcntl.h>
#include <sys/mman.h>

#include "hardware/raspi.h"
#include "logging/logging.h"

#define GPFSEL   ((volatile unsigned int *) (gpio + 0))
#define GPSET    ((volatile unsigned int *) (gpio + 7))
#define GPCLR    ((volatile unsigned int *) (gpio + 10))
#define GPLEV    ((volatile unsigned int *) (gpio + 13))

#define BCM2836_PERI_BASE 0x3F000000
#define GPIO_BASE         BCM2836_PERI_BASE + 0x200000
#define BLOCK_SIZE           (4*1024)

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

register_object(raspi, raspi_impl);
