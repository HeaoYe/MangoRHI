#include "MangoRHI/logger.hpp"

namespace MangoRHI {
    Logger g_logger;
    static LogLevel g_log_level = LogLevel::eInfo;

    spdlog::level::level_enum level2spdlog(LogLevel level) {
        switch (level) {
        case LogLevel::eTrace:
            return spdlog::level::trace;
        case LogLevel::eDebug:
            return spdlog::level::debug;
        case LogLevel::eInfo:
            return spdlog::level::info;
        case LogLevel::eWarn:
            return spdlog::level::warn;
        case LogLevel::eError:
            return spdlog::level::err;
        case LogLevel::eFatal:
            return spdlog::level::critical;
        }
    }

    Result Logger::create() {
        component_create()

        spd_logger = spdlog::stdout_color_mt("MangoRHI");
        spd_logger->set_level(level2spdlog(g_log_level));
        spd_logger->info("Create Logger");

        return Result::eSuccess;
    }

    Result Logger::destroy() {
        component_destroy()

        spd_logger->info("Destroy Logger");
        // spdlog::shutdown();

        component_destroy_end()
    }

    void Logger::set_level(LogLevel level) {
        g_log_level = level;
        spd_logger->set_level(level2spdlog(level));
    }

    MANGORHI_API void set_logger_level(LogLevel level) {
        g_log_level = level;
        if (g_logger.is_destroyed() == MG_FALSE) {
            g_logger.set_level(level);
        }
    }
}
