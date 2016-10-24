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

    /**
     * @brief      Logging mechanism. Every file should have one logger, declared like so:
     *                  static logger diag("short, descriptive name");
     *             Used:
     *                  diag->info("Formatted message with {} param", 1);
     */
    class logger
    {
    public:

        logger(std::string const & name)
            : inner( spdlog::get(name) )
        {
            inner = inner ? inner : spdlog::basic_logger_mt(name, LOG_FILE, false);
            inner->flush_on( inner->level() );
        }

        template <typename... args_t> void trace(char const * fmt, const args_t&... args) {
            inner->trace(fmt, args...);
        }

        template <typename... args_t> void debug(char const * fmt, const args_t&... args) {
            inner->debug(fmt, args...);
        }

        template <typename... args_t> void info(char const * fmt, const args_t&... args) {
            inner->info(fmt, args...);
        }

        template <typename... args_t> void warn(char const * fmt, const args_t&... args) {
            inner->warn(fmt, args...);
        }

        template <typename... args_t> void error(char const * fmt, const args_t&... args) {
            inner->error(fmt, args...);
        }

        template <typename... args_t> void critical(char const * fmt, const args_t&... args) {
            inner->critical(fmt, args...);
        }

        /**
         * @brief      Used when a fatal condition is encountered.
         *
         * @detail     A messaged is logged with critical level, and program execution is aborted.
         *
         */
        template <typename... args_t> void fail(char const * fmt, const args_t&... args) {
            std::string msg("FATAL: ");
            msg.append(fmt);
            inner->critical(msg.c_str(), args...);
            std::abort();
        }

        void set_level(log_level level)
        {
            inner->set_level(level);
            inner->flush_on(level);
        }

    private:
        std::shared_ptr<spdlog::logger> inner;
    };
}
