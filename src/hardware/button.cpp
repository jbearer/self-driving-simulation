#include <memory>

#include "logging/logging.h"
#include "hardware/button.h"

using namespace std;
using namespace hardware;

static logging::logger diag("hardware/button");

struct mock_button
    : button
{
    bool is_pushed() const
    {
        return false;
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

register_object(button_factory, mock_button_factory);
