#include <cassert>

#include "objects/objects.h"

#include "testing.h"
#include "widget.h"

using namespace objects;
using namespace testing::objects;

struct replacement_widget
    : widget
{
    impl_t stat() const
    {
        return REPLACEMENT;
    }
};

test_case(objects.real)
{
    assert( get<widget>()->stat() == REAL );
}

test_case(objects.mock)
{
    mock<widget>();
    assert( get<widget>()->stat() == MOCK );
}

test_case(objects.mock.over)
{
    assert( get<widget>()->stat() == REAL );
    mock<widget>();
    assert( get<widget>()->stat() == MOCK );
}

test_case(objects.replace)
{
    replace<widget, replacement_widget>();
    assert( get<widget>()->stat() == REPLACEMENT );
}

test_case(objects.replace.over)
{
    assert( get<widget>()->stat() == REAL );
    replace<widget, replacement_widget>();
    assert( get<widget>()->stat() == REPLACEMENT );
}

test_case(objects.reset)
{
    mock<widget>();
    assert( get<widget>()->stat() == MOCK );
    reset();
    assert( get<widget>()->stat() == REAL );
}
