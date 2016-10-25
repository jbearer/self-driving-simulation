#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "hardware/raspi.h"
#include "system/threading.h"

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
        typedef raspi::digital_val_t              trigger_t;

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
         * @param[in]  name     Used by the logger to identify the window when tracking its progress.
         * @param[in]  size     The size of the window in microseconds.
         * @param[in]  callback Function to be passed the window when it is ready.
         */
        void fill_window(std::string const & name, long size, window_callback_t const & callback);

        /**
         * @brief      Obtain a window of samples starting from a time dependent on trigger:
         *             trigger == HIGH: start sampling the next time the voltage changes from low to
         *             high.
         *             trigger == LOW: start sampling the next time the voltage changes from high to
         *             low.
         */
        void fill_window(std::string const & name, long size, trigger_t trigger,
                         window_callback_t const & callback);

        /**
         * @brief      The sampling rate in hertz.
         */
        long sampling_rate() const;

        // output_device
        void write(raspi::digital_val_t value);

    private:
        // Sample from the raspberry pi and fill out window requests
        void sample();

        // Return requested windows to callers as they are finished
        void flush();

        struct window_req
        {
            std::string         name;
            window_t            data;
            size_t              num_samples;
            window_callback_t   callback;

            window_req(std::string const & name_,
                       window_t const & data_,
                       size_t num_samples_,
                       window_callback_t const & callback_)
                : name(name_)
                , data(data_)
                , num_samples(num_samples_)
                , callback(callback_)
            {}

            // We don't want to be copying these, because copying data can be very expensive
            window_req(window_req const &) = delete;
            window_req(window_req &&) = default;
            window_req & operator=(window_req const &) = delete;
            window_req & operator=(window_req && other)
            {
                name = std::move(other.name);
                data = std::move(other.data);
                num_samples = std::move(other.num_samples);
                callback = std::move(other.callback);
                return *this;
            }
        };

        typedef std::list<window_req>       window_list_t;
        typedef std::queue<window_req>      window_queue_t;

        // Windows which will be requested when next triggered
        window_list_t                       high_triggers;
        window_list_t                       low_triggers;

        // Windows which have been requested and need to be filled
        window_list_t                       requested_windows;
        std::mutex                          requests_lock;

        // Windows which have been filled and are waiting to be returned to the caller
        window_queue_t                      filled_windows;
        std::mutex                          filled_lock;
        std::condition_variable             filled_signal; // Signal that a window needs flushing

        // The most recent reading from the raspberry pi
        std::atomic<raspi::digital_val_t>   value;

        // The previous sample from the raspberry pi
        raspi::digital_val_t                last_sample;

        // The sampling rate
        long                                sr;

        // Sample and fill requested windows
        std::unique_ptr<sys::thread>        sampler;

        // Return filled windows to clients
        std::unique_ptr<sys::thread>        flusher;
    };
}
