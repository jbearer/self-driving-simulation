#pragma once

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include "logging/logging.h"

using namespace std;

namespace objects {
    static logging::logger diag("objects");
}

typedef unordered_map< string, shared_ptr<objects::object> > object_map_t;

namespace objects {
    object_map_t * real_objects();
    object_map_t * mock_objects();
    unordered_set<string> * mocks();
}

template<typename any_t>
string name_of()
{
    return typeid(any_t).name();
}

template<typename itf_t, typename impl_t>
void register_object_unchecked(object_map_t & object_map)
{
    object_map[name_of<itf_t>()] = shared_ptr<objects::object>( static_cast<objects::object *>(new impl_t) );
}

template<typename itf_t, typename impl_t>
objects::object_registration<itf_t, impl_t>::object_registration()
{
    string name = name_of<itf_t>();
    if ( objects::real_objects()->count(name) != 0 )
        objects::diag.fail("Object {} registered twice.", name);
    if ( !is_base_of<itf_t, impl_t>::value )
        objects::diag.fail("Interface type {} is not a base class of implementation type {}", name, name_of<impl_t>() );

    register_object_unchecked<itf_t, impl_t>(*objects::real_objects());
}

template<typename itf_t, typename impl_t>
objects::mock_object_registration<itf_t, impl_t>::mock_object_registration()
{
    string name = name_of<itf_t>();
    if ( objects::mock_objects()->count(name) != 0 )
        objects::diag.fail("Mock object {} registered twice.", name);
    if ( !is_base_of<itf_t, impl_t>::value )
        objects::diag.fail("Interface type {} is not a base class of implementation type {}", name, name_of<impl_t>() );

    register_object_unchecked<itf_t, impl_t>(*objects::mock_objects());
}

template<typename itf_t>
shared_ptr<itf_t> get_real()
{
    if ( objects::real_objects()->count( name_of<itf_t>() ) ) {
        auto untyped_ptr = (*objects::real_objects())[name_of<itf_t>()];
        return shared_ptr<itf_t>( untyped_ptr, static_cast<itf_t *>( untyped_ptr.get() ) );
    } else {
        return shared_ptr<itf_t>(nullptr);
    }
}

template<typename itf_t>
shared_ptr<itf_t> get_mock()
{
    if ( objects::mock_objects()->count( name_of<itf_t>() ) ) {
        auto untyped_ptr = (*objects::mock_objects())[name_of<itf_t>()];
        return shared_ptr<itf_t>( untyped_ptr, static_cast<itf_t *>( untyped_ptr.get() ) );
    } else {
        // Fall back to real implementation if we don't see a mock
        return get_real<itf_t>();
    }
}

template<typename itf_t>
shared_ptr<itf_t> get_internal()
{
    if ( objects::mocks()->count( name_of<itf_t>() ) ) {
        return get_mock<itf_t>();
    } else {
        return get_real<itf_t>();
    }
}

template<typename itf_t>
shared_ptr<itf_t> objects::get()
{
    auto option_ptr = get_internal<itf_t>();
    if (!option_ptr)
        objects::diag.fail("Requested unregistered object {}.", name_of<itf_t>());
    return option_ptr;
}

template<typename itf_t>
void objects::mock()
{
    objects::mocks()->insert( name_of<itf_t>() );
}
