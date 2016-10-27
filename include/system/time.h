#pragma once

#include <sys/time.h>
#include <unistd.h>

#include "diagnostics/diag.h"

namespace sys {

    typedef suseconds_t useconds_t;

    static diagnostics::logger diag("system");

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

    inline useconds_t useconds_since(useconds_t time)
    {
        return now() - time;
    }

    inline double seconds_since(useconds_t time)
    {
        return double( useconds_since(time) ) / 1e6;
    }

}
