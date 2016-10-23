#pragma once

#include <functional>
#include <memory>
#include <string>
#include <type_traits>

#include "logging/logging.h"

using namespace std;

namespace objects {

    namespace impl {

        typedef function< shared_ptr<object>(void) > factory_t;

        template<typename any_t>
        std::string name_of()
        {
            return typeid(any_t).name();
        }

        void object_registration(std::string const & name, factory_t factory);
        void mock_object_registration(std::string const & name, factory_t factory);
        std::shared_ptr<object> get(std::string const & name);
        void mock(std::string const & name);
    }

    template<typename itf_t, typename impl_t>
    objects::object_registration<itf_t, impl_t>::object_registration()
    {
        static_assert(is_base_of<object, itf_t>::value,
                      "register_object: interface type must inherit from object");
        static_assert(is_base_of<itf_t, impl_t>::value,
                      "register_object: implementation type must inherit from interface type");

        impl::object_registration( impl::name_of<itf_t>(), []() {
            return std::shared_ptr<object>( static_cast<object *>(new impl_t) );
        });
    }

    template<typename itf_t, typename impl_t>
    objects::mock_object_registration<itf_t, impl_t>::mock_object_registration()
    {
        static_assert(is_base_of<object, itf_t>::value,
                      "register_mock_object: interface type must inherit from object");
        static_assert(is_base_of<itf_t, impl_t>::value,
                      "register_mock_object: implementation type must inherit from interface type");

        impl::mock_object_registration( impl::name_of<itf_t>(), []() {
            return std::shared_ptr<object>( static_cast<object *>(new impl_t) );
        });
    }

    template<typename itf_t>
    shared_ptr<itf_t> get()
    {
        auto untyped_ptr = impl::get( impl::name_of<itf_t>() );
        return std::shared_ptr<itf_t>( untyped_ptr, static_cast<itf_t*>( untyped_ptr.get() ) );
    }

    template<typename itf_t>
    void mock()
    {
        impl::mock( impl::name_of<itf_t>() );
    }
}
