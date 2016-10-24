#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

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

    struct oscilloscope
        : output_device
    {
        typedef std::vector<raspi::digital_val_t> window_t;
        typedef std::function< void(window_t&&) > window_callback_t;

        /**
         * @brief      Create a new oscilloscope with a fixed sampling rate.
         *
         * @param[in]  src      The sampling rate in hertz.
         */
        oscilloscope(long sr_ = 44100);

        /**
         * @brief      Obtain a window of samples starting from the moment this function is called.
         *
         * @detail     The window is filled with samples start from the moment it is requested and
         *             ending when it is full. This process happens asynchronously, so fill_window
         *             returns immediately. When the window is ready, the function callback will be
         *             called with the filled window as an argument.
         *
         * @param[in]  size     The size of the window in microseconds.
         * @param[in]  callback Function to be passed the window when it is ready.
         */
        void fill_window(long size, window_callback_t callback);

        /**
         * @brief      The sampling rate in hertz.
         */
        long sampling_rate() const;

        // output_device
        void write(raspi::digital_val_t value);

        ~oscilloscope();

    private:
        // Sample from the raspberry pi and fill out window requests
        void sample();

        // Return requested windows to callers as they are finished
        void flush();

        typedef std::pair<window_t, window_callback_t> window_pair_t;

        // Windows which have been requested and need to be filled
        std::vector<window_pair_t>          requested_windows;
        std::mutex                          requests_lock;

        // Windows which have been filled and are waiting to be returned to the caller
        std::queue<window_pair_t>           filled_windows;
        std::mutex                          filled_lock;
        std::condition_variable             filled_signal; // Signal that a window needs flushing

        // The last known reading from the raspberry pi
        std::atomic<raspi::digital_val_t>   value;

        // The sampling rate
        long                                sr;

        // Used to signal threads to stop
        std::atomic<bool>                   halt;

        // Sample and fill requested windows
        std::thread                         sampler;

        // Return filled windows to clients
        std::thread                         flusher;
    };
}
