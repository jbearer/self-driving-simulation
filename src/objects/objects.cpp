#include "objects/objects.h"

namespace objects {

    object_map_t * real_objects()
    {
        static object_map_t * real_objects_ = new object_map_t;
        return real_objects_;
    }

    object_map_t * mock_objects()
    {
        static object_map_t * mock_objects_ = new object_map_t;
        return mock_objects_;
    }

    unordered_set<string> * mocks()
    {
        static unordered_set<string> * mocks_ = new unordered_set<string>;
        return mocks_;
    }
}
