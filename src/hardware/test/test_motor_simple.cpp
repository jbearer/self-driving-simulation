#include <memory>

#include "hardware/motor.h"
#include "logging/logging.h"
#include "objects/objects.h"
#include "system/time.h"

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
    sys::sleep(2e6);

    diag.info("Accelerating motor 0.");
    motor->set_acceleration(0.2);
    sys::sleep(5e6);
    diag.info("Decelerating motor 0.");
    motor->change_acceleration(-0.4);
    sys::sleep(5e6);

    diag.info("Sleeping for 2 seconds with motors once again idle.");
    sys::sleep(2e6);

    diag.info("Stopping.");

    return 0;
}
