#pragma once

/**
 * Fake object interface for use in testing objects library.
 */

namespace testing {
    namespace objects {

        enum impl_t
        {
            MOCK,
            REAL,
            REPLACEMENT
        };

        struct widget
        {
            virtual impl_t stat() const = 0;
        };

    }
}
