#include <memory>
#include <sys/time.h>

#include "logging/logging.h"
#include "hardware/motor.h"

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/motor");

struct mock_motor
    : motor
{
    mock_motor()
        : accel(0)
        , position_offset(0)
        , velocity_offset(0)
    {}

    void set_acceleration(double acceleration_)
    {
        // Things are changing, so we store our offsets for position and velocity
        calibrate( position() );
        velocity_offset = velocity();
        last_velocity_calibration = now();

        accel = acceleration_;
    }

    void change_acceleration(double delta)
    {
        set_acceleration(acceleration() + delta);
    }

    double position() const
    {
        return position_offset + velocity() * seconds_since(last_position_calibration);
    }

    double velocity() const
    {
        return velocity_offset + acceleration() * seconds_since(last_velocity_calibration);
    }

    double acceleration() const
    {
        return accel;
    }

    void calibrate(double new_position)
    {
        position_offset = new_position;
        last_position_calibration = now();
    }

private:
    suseconds_t now() const
    {
        timeval tv;
        gettimeofday(&tv, 0);
        return tv.tv_usec;
    }

    double seconds_since(suseconds_t time) const
    {
        return double(now() - time) / 10e6;
    }

    double accel;
    double position_offset;
    double velocity_offset;

    suseconds_t last_position_calibration;
    suseconds_t last_velocity_calibration;
};

struct mock_motor_factory
    : motor_factory
{
    unique_ptr<motor> create(int)
    {
        return unique_ptr<motor>( static_cast<motor *>(new mock_motor) );
    }
};

//register_object(motor_factory, motor_factory_impl);
register_mock_object(motor_factory, mock_motor_factory);
