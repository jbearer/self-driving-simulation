#include <cassert>

#include "objects/objects.h"

using namespace objects;

enum impl_t
{
    MOCK,
    REAL,
    REPLACEMENT
};

struct widget
{
    virtual impl_t stat() const = 0;
};

struct widget_impl
    : widget
{
    impl_t stat() const
    {
        return REAL;
    }
};

struct mock_widget
    : widget
{
    impl_t stat() const
    {
        return MOCK;
    }
};

struct replacement_widget
    : widget
{
    impl_t stat() const
    {
        return REPLACEMENT;
    }
};

register_object(widget, widget_impl);
register_mock_object(widget, mock_widget);

int main()
{
    assert( get<widget>()->stat() == REAL );
    mock<widget>();
    assert( get<widget>()->stat() == MOCK );
    replace<widget, replacement_widget>();
    assert( get<widget>()->stat() == REPLACEMENT );
    return 0;
}
