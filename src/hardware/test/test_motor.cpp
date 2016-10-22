#include <algorithm>
#include <memory>
#include <numeric>
#include <unistd.h>
#include <vector>

#include "hardware/motor.h"
#include "logging/logging.h"
#include "objects/objects.h"

using namespace std;
using namespace hardware;

static logging::logger diag("test/motor");

int main()
{
    diag.info("Initializing motors");
    vector<int> tracks(6);
    vector<unique_ptr<motor>> motors(6);
    auto factory = objects::get<motor_factory>();
    iota(tracks.begin(), tracks.end(), 0);
    transform(tracks.begin(), tracks.end(), motors.begin(), [factory](int track_id) {
        return factory->create(track_id);
    });

    diag.info("Sleeping for 2 seconds with motors idle.");
    usleep(2e6);

    diag.info("Accelerating motor 0.");
    motors[0]->set_acceleration(0.2);
    usleep(2e6);
    diag.info("Decelerating motor 0.");
    motors[0]->change_acceleration(-0.2);
    usleep(2e6);

    diag.info("Sleeping for 2 seconds with motors once again idle.");
    usleep(2e6);

    diag.info("Accelerating motor 0 slowly and motor 1 quickly.");
    motors[0]->set_acceleration(0.1);
    motors[1]->set_acceleration(1);
    usleep(2e6);

    diag.info("Stopping.");

    return 0;
}
