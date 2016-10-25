#pragma once

#include <cstdlib>
#include <memory>
#include <string>

#include "spdlog/common.h"
#include "spdlog/spdlog.h"

namespace logging
{
    typedef spdlog::level::level_enum log_level;

    const spdlog::filename_t LOG_FILE = "logs/main.log";

    inline void set_level(log_level level)
    {
        spdlog::set_level(level);
        spdlog::apply_all([level](std::shared_ptr<spdlog::logger> l) {
            l->flush_on( l->level() );
        });
    }

    template<typename impl_t>
    struct logger_impl
    {
        logger_impl(std::string const & name)
            : inner( spdlog::get(name) )
        {
            inner = inner ? inner : spdlog::basic_logger_mt(name, LOG_FILE, false);
            inner->flush_on( inner->level() );
        }

#define impl_log(lvl) static_cast<impl_t *>(this)->log(log_level::lvl, fmt, args...)

        template <typename... args_t> void trace(char const * fmt, const args_t&... args) {
            impl_log(trace);
        }

        template <typename... args_t> void debug(char const * fmt, const args_t&... args) {
            impl_log(debug);
        }

        template <typename... args_t> void info(char const * fmt, const args_t&... args) {
            impl_log(info);
        }

        template <typename... args_t> void warn(char const * fmt, const args_t&... args) {
            impl_log(warn);
        }

        template <typename... args_t> void error(char const * fmt, const args_t&... args) {
            impl_log(err);
        }

        template <typename... args_t> void critical(char const * fmt, const args_t&... args) {
            impl_log(critical);
        }

#undef impl_log

        /**
         * @brief      Used when a fatal condition is encountered.
         *
         * @detail     A messaged is logged with critical level, and program execution is aborted.
         *
         */
        template <typename... args_t> void fail(char const * fmt, const args_t&... args) {
            std::string msg("FATAL: ");
            msg.append(fmt);
            critical(msg.c_str(), args...);
            std::abort();
        }

        void set_level(log_level level)
        {
            inner->set_level(level);
            inner->flush_on(level);
        }

        std::string name() const
        {
            return inner->name();
        }

    protected:
        template <typename... args_t>
        void log(log_level level, const char* fmt, const args_t&... args)
        {
            inner->log(level, fmt, args...);
        }

    private:
        std::shared_ptr<spdlog::logger> inner;
    };

    /**
     * @brief      Logging mechanism. Every file should have one logger, declared like so:
     *                  static logger diag("short, descriptive name");
     *             Used:
     *                  diag->info("Formatted message with {} param", 1);
     */
    struct logger
        : logger_impl<logger>
    {
        logger(std::string const & name)
            : logger_impl(name)
        {}
    };

    struct prefix_logger
        : logger_impl<prefix_logger>
    {
        prefix_logger(std::string const & name, std::string const & prefix_)
            : logger_impl(name)
            , prefix(prefix_)
        {}

        template<typename... args_t>
        void log(log_level level, const char * fmt, const args_t&... args)
        {
            std::string new_fmt = prefix;
            new_fmt.append(": ");
            new_fmt.append(fmt);
            logger_impl::log(level, new_fmt.c_str(), args...);
        }

    private:

        std::string prefix;
    };
}
