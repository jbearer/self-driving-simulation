#include <memory>

#include "logging/logging.h"
#include "hardware/button.h"
#include "hardware/raspi.h"
#include "objects/objects.h"

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/button");

// Mapping from track_id_s to pins
static unordered_map<int, int> pins = {
    {0, 0},
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {5, 5}
};

struct button_impl
    : button
{
    explicit button_impl(int pin_)
        : pi( objects::get<raspi>() )
        , pin_id(pin_)
    {
        pi->pin_mode(pin_id, raspi::INPUT);
    }

    int pin() const
    {
        return pin_id;
    }

    bool is_pushed() const
    {
        return pi->digital_read(pin_id) == raspi::HIGH;
    }

private:
    shared_ptr<raspi>   pi;
    int                 pin_id;
};

struct mock_button
    : button
{
    bool is_pushed() const
    {
        return false;
    }

    int pin() const
    {
        return 0;
    }
};

struct button_factory_impl
    : button_factory
{
    unique_ptr<button> create(int track_id)
    {
        return unique_ptr<button>( static_cast<button *>( new button_impl(pins[track_id]) ) );
    }
};

struct mock_button_factory
    : button_factory
{
    unique_ptr<button> create(int)
    {
        return unique_ptr<button>( static_cast<button *>(new mock_button) );
    }
};

register_object(button_factory, button_factory_impl);
register_mock_object(button_factory, mock_button_factory);
