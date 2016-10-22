#include <cstdlib>
#include <unistd.h>

#include "hardware/button.h"
#include "logging/logging.h"
#include "objects/objects.h"

using namespace hardware;

static logging::logger diag("test/button");

int main(int argc, char ** argv)
{
    if (argc != 2) {
        diag.fail("Usage: {} <track>", argv[0]);
    }

    int track = atoi(argv[1]);
    auto factory = objects::get<button_factory>();
    auto butt = factory->create(track);
    bool state = butt->is_pushed();

    diag.info("Initial state is {}.", state ? "pushed" : "unpushed");

    while (true) {
        bool new_state = butt->is_pushed();
        if (new_state != state) {
            diag.info("Button was {}.", new_state ? "pushed": "unpushed");
            state = new_state;
        }
        usleep(1000);
    }

    return 0;
}