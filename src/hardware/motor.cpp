#include <atomic>
#include <condition_variable>
#include <errno.h>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "logging/logging.h"
#include "hardware/motor.h"
#include "hardware/raspi.h"
#include "objects/objects.h"
#include "system/system.h"

#define PI 3.1415926535897

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/motor");

// Mapping from track_id_s to pins
static unordered_map<int, int> pins = {
    {0, 18},
    {1, 23},
    {2, 4},
    {3, 5},
    {4, 6},
    {5, 12}
};

struct motor_impl
    : motor
{
    explicit motor_impl(int pin_)
        : pi( objects::get<raspi>() )
        , pin_id(pin_)
        , curr(0)
        , next(STOP_THRESHOLD + 1)     // This ensures we are stopped at first
        , accel(0.0)
        , pos(0.0)
        , velocity_offset( velocity() )
        , halt(false)
    {
        diag.info("Initializing motor at pin {}.", pin_id);
        pi->pin_mode(pin_id, raspi::OUTPUT);
        motor_thread = thread( bind(&motor_impl::idle, this) );
    }

    ~motor_impl()
    {
        // Stop the motors and join the thread
        {
            lock_guard<mutex> lock(data_lock);
            halt = true;
        }
        motor_thread.join();
    }

    int pin() const
    {
        return pin_id;
    }

    void set_acceleration(double acceleration_)
    {
        lock_guard<mutex> lock(data_lock);

        // If we were stopped, we may not be anymore
        if ( stopped() ) {
            curr = 0;
            next = STOP_THRESHOLD;  // This ensures we start moving again
        }

        // Things are changing, so we store our velocity offset
        velocity_offset = lockless_velocity();
        curr = 0;
        next = lockless_delay();
        accel = acceleration_;

        start.notify_all();
    }

    void change_acceleration(double delta)
    {
        set_acceleration(acceleration() + delta);
    }

    double position() const
    {
        lock_guard<mutex> lock(data_lock);
        return pos;
    }

    double velocity() const
    {
        lock_guard<mutex> lock(data_lock);
        return lockless_velocity();
    }

    double acceleration() const
    {
        lock_guard<mutex> lock(data_lock);
        return accel;
    }

    void calibrate(double new_position)
    {
        lock_guard<mutex> lock(data_lock);
        pos = new_position;
    }

private:

    // Sit still and wait for an acceleration
    void idle()
    {
        {
            unique_lock<mutex> l(data_lock);

            if (halt) {
                diag.info("Halt requested, exiting.");
                return;
            }

            diag.trace("Idling, waiting for acceleration.");
            start.wait(l, [this]() { return !stopped(); });
        }

        diag.trace("Acceleration detected, beginning pulse wave modulation.");
        pwm();
    }

    void pwm()
    {
        double delay;
        while( !stopped() ) {
            {
                // Update delay so that we accelerate
                lock_guard<mutex> lock(data_lock);
                delay = lockless_delay();
                diag.info("delay = {}", delay);
            }

            pi->digital_write(pin_id, raspi::HIGH);
            sys::sleep(delay);
            pi->digital_write(pin_id, raspi::LOW);
            sys::sleep(delay);

            {
                lock_guard<mutex> lock(data_lock);

                double v_0 = (velocity_offset / RADIUS) * 60 / (2 * PI);
                double a   = (accel / RADIUS) * 60 / (2 * PI); // Linear to angular
                curr = next;
                next = 4688.0 / (a * 1e-6 * curr + v_0) + curr;

                pos += RADIANS_PER_TURN * RADIUS;

                // While we have the lock, check if we've been asked to quit
                if (halt) {
                    diag.info("Halt requested, exiting.");
                    return;
                }
            }
        }

        idle();
    }

    bool stopped() const
    {
        return lockless_delay() > STOP_THRESHOLD;
    }

    // We sometimes need to calculate velocity when we already have the lock, and thus can't call velocity()
    double lockless_velocity() const
    {
        return stopped() ? 0.0 : (1e6 / lockless_delay()) * RADIANS_PER_TURN * RADIUS;
    }

    double lockless_delay() const
    {
        return next - curr;
    }

    // Hardware
    shared_ptr<raspi>   pi;
    int                 pin_id;

    // Kinematics
    double              curr;
    double              next;
    double              accel;
    double              pos;
    double              velocity_offset;

    // Synchronization
    mutable mutex       data_lock;      // Protects the kinematics data
    condition_variable  start;          // Used to signal that we have started moving

    bool                halt;
    thread              motor_thread;

    // The radius of the wheel in meters. Used to convert between angular and linear units.
    static constexpr double RADIUS = 0.075;

    static constexpr double RADIANS_PER_TURN = PI / 100;

    // If delay is longer than this, the motor is considered to be stopped.
    static constexpr double STOP_THRESHOLD = 4000;
};

struct mock_motor
    : motor
{
    mock_motor()
        : accel(0)
        , position_offset(0)
        , velocity_offset(0)
    {}

    int pin() const
    {
        return 0;
    }

    void set_acceleration(double acceleration_)
    {
        // Things are changing, so we store our offsets for position and velocity
        calibrate( position() );
        velocity_offset = velocity();
        last_velocity_calibration = sys::now();

        accel = acceleration_;
    }

    void change_acceleration(double delta)
    {
        set_acceleration(acceleration() + delta);
    }

    double position() const
    {
        return position_offset + velocity() * sys::seconds_since(last_position_calibration);
    }

    double velocity() const
    {
        return velocity_offset + acceleration() * sys::seconds_since(last_velocity_calibration);
    }

    double acceleration() const
    {
        return accel;
    }

    void calibrate(double new_position)
    {
        position_offset = new_position;
        last_position_calibration = sys::now();
    }

private:
    double accel;
    double position_offset;
    double velocity_offset;

    sys::useconds_t last_position_calibration;
    sys::useconds_t last_velocity_calibration;
};

struct motor_factory_impl
    : motor_factory
{
    unique_ptr<motor> create(int track_id)
    {
        int pin = pins[track_id];
        return unique_ptr<motor>( static_cast<motor*>( new motor_impl(pin) ) );
    }
};

struct mock_motor_factory
    : motor_factory
{
    unique_ptr<motor> create(int)
    {
        return unique_ptr<motor>( static_cast<motor *>(new mock_motor) );
    }
};

register_object(motor_factory, motor_factory_impl);
register_mock_object(motor_factory, mock_motor_factory);
