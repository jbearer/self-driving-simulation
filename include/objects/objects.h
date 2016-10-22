#pragma once

#include <memory>

namespace objects
{
    /**
     * @brief      All registered types must inherit from object
     */
    struct object {};

    /**
     * @brief      Register a singleton object.
     *
     * @tparam     itf_t   The interface represented by this registration.
     * @tparam     impl_t  The implementation to use when creating the singleton.
     * *                   Must inherit from itf_t.
     */
    template<typename itf_t, typename impl_t>
    struct object_registration
    {
        object_registration();
    };

    /**
     * @brief      Register a mock implementation of a singleton object.
     *
     * @tparam     itf_t   The interface being mocked out.
     * @tparam     impl_t  The mock implementation.
     * *                   Must inherit from itf_t.
     */
    template<typename itf_t, typename impl_t>
    struct mock_object_registration
    {
        mock_object_registration();
    };

    /**
     * @brief      Get a reference to a singleton object registered by creating an object_registration.
     */
    template<typename itf_t>
    std::shared_ptr<itf_t> get();

    /**
     * @brief      Substitute the mock implementation of a class or object for the real implementation.
     * @detail     Subsequent calls to get<itf_t> or create<itf_t> will return an instance of the
     *             mock implementation, rather than the real implementation. If no mock is
     *             registered, the real implementation is still used.
     */
    template<typename itf_t>
    void mock();
}

#define register_object(itf_t, impl_t) static objects::object_registration<itf_t, impl_t> itf_t##_registation
#define register_mock_object(itf_t, impl_t) static objects::mock_object_registration<itf_t, impl_t> mock_##itf_t##registation

#include "../../src/objects/objects_impl.h"
