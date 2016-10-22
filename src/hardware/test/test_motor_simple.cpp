#include <memory>
#include <unistd.h>

#include "hardware/motor.h"
#include "logging/logging.h"
#include "objects/objects.h"

using namespace std;
using namespace hardware;

static logging::logger diag("test/motor.simple");

int main()
{
    logging::set_level(logging::log_level::trace);

    diag.info("Initializing motor");
    auto factory = objects::get<motor_factory>();
    auto motor = factory->create(0);

    diag.info("Sleeping for 2 seconds with motors idle.");
    usleep(2e6);

    diag.info("Accelerating motor 0.");
    motor->set_acceleration(0.2);
    usleep(2e6);
    diag.info("Decelerating motor 0.");
    motor->change_acceleration(-0.4);
    usleep(2e6);

    diag.info("Sleeping for 2 seconds with motors once again idle.");
    usleep(2e6);

    diag.info("Stopping.");

    return 0;
}
