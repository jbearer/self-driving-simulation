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

typedef unordered_map<string, factory_t>            object_map_t;
typedef unordered_map< string, shared_ptr<object> > instance_map_t;

/**
 * We have to hide all the static containers in functions to avoid static initialization order fiasco.
 */

object_map_t & real_objects()
{
    static object_map_t * real_objects_ = new object_map_t;
    return *real_objects_;
}

instance_map_t & real_instances()
{
    static instance_map_t * real_instances_ = new instance_map_t;
    return *real_instances_;
}

object_map_t & mock_objects()
{
    static object_map_t * mock_objects_ = new object_map_t;
    return *mock_objects_;
}

instance_map_t & mock_instances()
{
    static instance_map_t * mock_instances_ = new instance_map_t;
    return *mock_instances_;
}

unordered_set<string> & mocks()
{
    static unordered_set<string> * mocks_ = new unordered_set<string>;
    return *mocks_;
}

shared_ptr<object> get_instance(string const & itf,
                                instance_map_t & instances,
                                object_map_t const & factories)
{
    if ( !instances.count(itf) ) {
        // We have yet to instantiate this object, so we do that now
        auto factory = factories.find(itf);
        if ( factory == factories.end() ) {
            return nullptr;
        }
        instances[itf] = (factory->second)();
    }
    return instances[itf];
}

shared_ptr<object> get_real(string const & itf)
{
    auto obj = get_instance(itf, real_instances(), real_objects());
    if (!obj) {
        diag.fail("Requested unregistered object {}.", itf);
    }
    return obj;
}

shared_ptr<object> get_mock(string const & itf)
{
    auto mock = get_instance(itf, mock_instances(), mock_objects());
    if (mock) {
        diag.trace("Using mock implementation of {}.", itf);
        return mock;
    } else {
        diag.trace("No mock {} registered, falling back to real implementation.", itf);
        return get_real(itf);
    }
}

void objects::impl::object_registration(string const & name, factory_t factory)
{
    if ( real_objects().count(name) ) {
        diag.fail("Object {} registered twice.", name);
    }
    real_objects()[name] = factory;
}

void objects::impl::mock_object_registration(string const & name, factory_t factory)
{
    if ( !real_objects().count(name) ) {
        diag.fail("Mock registered for unknown object {}.");
    }
    if (  mock_objects().count(name) ) {
        diag.fail("Mock object {} registered twice.", name);
    }
    mock_objects()[name] = factory;
}

shared_ptr<object> objects::impl::get(string const & itf)
{
    if ( mocks().count(itf) ) {
        return get_mock(itf);
    } else {
        return get_real(itf);
    }
}

void objects::impl::mock(string const & itf)
{
    mocks().insert(itf);
}
