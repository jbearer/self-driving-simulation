#include <cassert>
#include <memory>
#include <unistd.h>

#include "hardware/button.h"
#include "hardware/pi_emulator.h"
#include "logging/logging.h"
#include "objects/objects.h"

using namespace std;
using namespace hardware;

static logging::logger diag("test/button");

struct button_emulator
    : input_device
{

    void connected(unique_ptr<pi_emu::input_handle> handle_)
    {
        handle = move(handle_);
    }

    void push()
    {
        assert(handle);
        handle->write(raspi::HIGH);
    }

    void unpush()
    {
        assert(handle);
        handle->write(raspi::LOW);
    }

    bool connected() const
    {
        return bool(handle);
    }

private:
    unique_ptr<pi_emu::input_handle> handle;
};

int main()
{
    auto pi = emulate_raspi();
    auto factory = objects::get<button_factory>();
    auto butt = factory->create(0);

    // Connect a button to the emulator
    shared_ptr<button_emulator> button_emu(new button_emulator);
    pi->connect_device(butt->pin(), button_emu);
    while ( !button_emu->connected() ) {
        usleep(1e6);
    }

    // Push the button and see if the software responds accordingly
    button_emu->push();
    assert( butt->is_pushed() );
    usleep(1e6);
    assert( butt->is_pushed() );
    button_emu->unpush();
    assert( !butt->is_pushed() );
    usleep(1e6);
    assert( !butt->is_pushed() );

    return 0;
}