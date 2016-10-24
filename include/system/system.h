#pragma once

#include <sys/time.h>
#include <unistd.h>

#include "logging/logging.h"

#ifdef __arm__
#define R_PI
#endif

namespace sys {

    typedef suseconds_t useconds_t;

    static logging::logger diag("system");

    inline void sleep(double us)
    {
        if ( usleep(us) < 0)
            diag.error("usleep: {}", strerror(errno));
    }

    inline useconds_t now()
    {
        timeval tv;
        gettimeofday(&tv, 0);
        return tv.tv_usec;
    }

    inline double seconds_since(useconds_t time)
    {
        return double(now() - time) / 1e6;
    }

}
