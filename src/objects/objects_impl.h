#pragma once

#include <functional>
#include <memory>
#include <string>
#include <type_traits>

#include "diagnostics/diag.h"

namespace objects {

    namespace impl {

        // We can't instantiate templates like shared_ptr with void type, so we use this dummy
        // object instead, as a way of erasing type information so we can store all of our objects
        // in one container.
        struct object {};

        typedef std::function< std::shared_ptr<object>(void) > factory_t;

        template<typename any_t>
        std::string name_of()
        {
            return typeid(any_t).name();
        }

        void object_registration(std::string const & name, factory_t factory);
        void mock_object_registration(std::string const & name, factory_t factory);
        std::shared_ptr<object> get(std::string const & name);
        void replace(std::string const & name, factory_t factory);
        void mock(std::string const & name);
    }

    template<typename itf_t, typename impl_t>
    objects::object_registration<itf_t, impl_t>::object_registration()
    {
        static_assert(std::is_base_of<itf_t, impl_t>::value,
                      "register_object: implementation type must inherit from interface type");

        impl::object_registration( impl::name_of<itf_t>(), []() {
            return std::shared_ptr<impl::object>( (impl::object *)(new impl_t) );
        });
    }

    template<typename itf_t, typename impl_t>
    objects::mock_object_registration<itf_t, impl_t>::mock_object_registration()
    {
        static_assert(std::is_base_of<itf_t, impl_t>::value,
                      "register_mock_object: implementation type must inherit from interface type");

        impl::mock_object_registration( impl::name_of<itf_t>(), []() {
            return std::shared_ptr<impl::object>( (impl::object *)(new impl_t) );
        });
    }

    template<typename itf_t>
    std::shared_ptr<itf_t> get()
    {
        auto untyped_ptr = impl::get( impl::name_of<itf_t>() );
        return std::shared_ptr<itf_t>( untyped_ptr, (itf_t *)( untyped_ptr.get() ) );
    }

    template<typename itf_t, typename impl_t>
    void replace()
    {
        static_assert(std::is_base_of<itf_t, impl_t>::value,
                      "replace: implementation type must inherit from interface type");

        impl::replace( impl::name_of<itf_t>(), []() {
            return std::shared_ptr<impl::object>( (impl::object *)(new impl_t) );
        });
    }

    template<typename itf_t>
    void mock()
    {
        impl::mock( impl::name_of<itf_t>() );
    }
}
