#pragma once

#if defined (__WIN32)
    #define MangoRHI_Platform_Windows
    #error Not impl for windows.
#elif defined (__unix__)
    #define MangoRHI_Platform_Unix
    #define MangoRHI_API
#elif defined (__APPLE__)
    #define MangoRHI_Platform_Apple
    #error Not impl for mac and ios.
#elif defined (__ANDROID__)
    #define MangoRHI_Platform_Android
    #error Not impl for android
#else
    #error Unknown Platform.
#endif

#if defined (NDEBUG)
    #define MANGO_DEBUG
#else
    #define MANGO_RELEASE
#endif

#include <stdint.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined (USE_EASTL)
    #define STL_IMPL eastl
    #include <EASTL/vector.h>
    #include <EASTL/vector_set.h>
#else
    #define STL_IMPL std
    #include <vector>
    #include <set>
    namespace std {
        template<typename T>
        using vector_set = std::set<T>;
    }
#endif

namespace MangoRHI {
    typedef int8_t i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    typedef float f32;
    typedef double d32;
    typedef u8 Bool;
    constexpr Bool MG_FALSE = 0;
    constexpr Bool MG_TRUE = 1;
    typedef u64 AddrType;

    enum class API {
        eNone,
        eOpenGL,
        eVulkan,
        eDirectX,
        eMetal,
    };

    enum class LogLevel {
        eTrace,
        eDebug,
        eInfo,
        eWarn,
        eError,
        eFatal,
    };

    enum class Result {
        eSuccess,
        eFailed,
        eAlreadyDestroyed,
        eNotImplemented,
        eDeviceNotFound,
    };

    class RuntimeComponent {
    public:
        virtual Result create() = 0;
        virtual Result destroy() = 0;
        virtual Bool is_destroyed() final { return destroyed; }
    protected:
        Bool destroyed = MG_TRUE;
    };

    #define define_readonly_member(type, member_name) \
    private: \
        type member_name{}; \
    public: \
        const type &get_##member_name() const { return member_name; }

    #define define_readonly_pointer(type, member_name) \
    private: \
        type *member_name{}; \
    public: \
        const type *get_##member_name() const { return member_name; }

    #define define_readonly_const_pointer(type, member_name) \
    private: \
        const type *member_name{}; \
    public: \
        const type *get_##member_name() const { return member_name; }

    #define define_readonly_member_value(type, member_name, value) \
    private: \
        type member_name = value; \
    public: \
        const type &get_##member_name() const { return member_name; }

    #define define_readonly_pointer_value(type, member_name, value) \
    private: \
        type *member_name = value; \
    public: \
        const type *get_##member_name() const { return member_name; }

    #define define_readonly_const_pointer_value(type, member_name, value) \
    private: \
        const type *member_name = value; \
    public: \
        const type *get_##member_name() const { return member_name; }

    #define component_create() \
    if (destroyed == ::MangoRHI::MG_FALSE) { \
        destroy(); \
    } \
    destroyed = ::MangoRHI::MG_FALSE;

    #define component_destroy() \
    if (destroyed == ::MangoRHI::MG_TRUE) { \
        return ::MangoRHI::Result::eAlreadyDestroyed; \
    } \
    destroyed = ::MangoRHI::MG_TRUE;

    class Context;

    MangoRHI_API Result initialize(API api);
    MangoRHI_API Result quit();
    MangoRHI_API Context* create_context();
}

#include "logger.hpp"
