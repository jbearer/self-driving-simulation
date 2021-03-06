#pragma once

#include <memory>

namespace objects
{
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
     * @brief      Substitute impl_t for the registered implementation of itf_t. Future calls to
     *             get<itf_t> will return an instance of impl_t.
     */
    template<typename itf_t, typename impl_t>
    void replace();

    /**
     * @brief      Substitute the registered mock implementation of am object for the real
     *             implementation.
     *
     * @detail     Subsequent calls to get<itf_t> will return an instance of the mock
     *             implementation, rather than the real implementation. If no mock is registered,
     *             the real implementation is still used.
     */
    template<typename itf_t>
    void mock();

    /**
     * @brief      Detach all instances of objects and replace all mocked out objects with their
     *             original implementations.
     *
     * @warning    Continuing to use references to previously instantiated
     *             instances of objects is undefined behavior.
     */
    void reset();
}

#define register_object(itf_t, impl_t) static objects::object_registration<itf_t, impl_t> itf_t##_registation
#define register_mock_object(itf_t, impl_t) static objects::mock_object_registration<itf_t, impl_t> mock_##itf_t##registation

#include "../../src/objects/objects_impl.h"
