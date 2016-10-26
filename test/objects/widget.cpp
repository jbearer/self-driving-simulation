#include "objects/objects.h"

#include "widget.h"

using namespace testing::objects;

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

register_object(widget, widget_impl);
register_mock_object(widget, mock_widget);
