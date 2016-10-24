#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "logging/logging.h"
#include "objects/objects.h"

using namespace std;
using namespace objects;
using namespace objects::impl;

static logging::logger diag("objects");

typedef unordered_map<string, factory_t>            factory_map_t;
typedef unordered_map< string, shared_ptr<object> > instance_map_t;

/**
 * We have to hide all the static containers in functions to avoid static initialization order fiasco.
 */

factory_map_t & factories()
{
    static factory_map_t * factories_ = new factory_map_t;
    return *factories_;
}

factory_map_t & mocks()
{
    static factory_map_t * mocks_ = new factory_map_t;
    return *mocks_;
}

instance_map_t & instances()
{
    static instance_map_t * instances_ = new instance_map_t;
    return *instances_;
}

shared_ptr<object> get_instance(string const & itf)
{
    if ( !instances().count(itf) ) {
        // We have yet to instantiate this object, so we do that now
        auto factory = factories().find(itf);
        if ( factory == factories().end() ) {
            return nullptr;
        }
        instances()[itf] = (factory->second)();
    }
    return instances()[itf];
}

void objects::impl::object_registration(string const & name, factory_t factory)
{
    if ( factories().count(name) ) {
        diag.fail("Object {} registered twice.", name);
    }
    factories()[name] = factory;
}

void objects::impl::mock_object_registration(string const & name, factory_t factory)
{
    if ( !factories().count(name) ) {
        diag.fail("Mock registered for unknown object {}.");
    }
    if (  mocks().count(name) ) {
        diag.fail("Mock object {} registered twice.", name);
    }
    mocks()[name] = factory;
}

shared_ptr<object> objects::impl::get(string const & itf)
{
    auto obj = get_instance(itf);
    if (!obj) {
        diag.fail("Requested unregistered object {}.", itf);
    }
    return obj;
}

void objects::impl::replace(string const & itf, factory_t factory)
{
    if ( !factories().count(itf) ) {
        diag.fail("Replacing unknown object {}.", itf);
    }

    if ( instances().count(itf) ) {
        // In this case, we'll reset our reference to the instance to point to a new instance of the
        // replacement object. Anyone with a reference to the original instance will still be using
        // that instance, but subsequent calls to get will get a pointer to the new instance.
        diag.warn("Creating new instance of {} over existing instance.", itf);
        instances()[itf] = factory();
    }

    factories()[itf] = factory;
}

void objects::impl::mock(string const & itf)
{
    if ( !mocks().count(itf) ) {
        diag.trace("No mock {} registered, falling back to real implementation.", itf);
        return;
    }

    diag.trace("Using mock implementation of {}.", itf);
    replace(itf, mocks()[itf]);
}
