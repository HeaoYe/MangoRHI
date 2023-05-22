#pragma once

#include "commons.hpp"


namespace MangoRHI {
    spdlog::level::level_enum level2spdlog(LogLevel level);
    MangoRHI_API void set_logger_level(LogLevel level);

    class Logger final : public RuntimeComponent {
    public:
        Result create() override;
        Result destroy() override;
        void set_level(LogLevel level);

        template<typename... Args>
        void trace(Args &&... args) {
            spd_logger->trace(std::forward<Args>(args)...);
        }

        template<typename... Args>
        void debug(Args &&... args) {
            spd_logger->debug(std::forward<Args>(args)...);
        }

        template<typename... Args>
        void info(Args &&... args) {
            spd_logger->info(std::forward<Args>(args)...);
        }

        template<typename... Args>
        void warn(Args &&... args) {
            spd_logger->warn(std::forward<Args>(args)...);
        }

        template<typename... Args>
        void error(Args &&... args) {
            spd_logger->error(std::forward<Args>(args)...);
        }

        template<typename... Args>
        void fatal(Args &&... args) {
            spd_logger->critical(std::forward<Args>(args)...);
        }
    private:
        std::shared_ptr<spdlog::logger> spd_logger;
    };

    extern Logger g_logger;
}

#define RHI_TRACE(...) ::MangoRHI::g_logger.trace(__VA_ARGS__);
#define RHI_DEBUG(...) ::MangoRHI::g_logger.debug(__VA_ARGS__);
#define RHI_INFO(...) ::MangoRHI::g_logger.info(__VA_ARGS__);
#define RHI_WARN(...) ::MangoRHI::g_logger.warn(__VA_ARGS__);
#define RHI_ERROR(...) ::MangoRHI::g_logger.error(__VA_ARGS__);
#define RHI_FATAL(...) ::MangoRHI::g_logger.fatal(__VA_ARGS__);
