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
    #include <EASTL/set.h>
    #include <EASTL/unordered_map.h>
#else
    #define STL_IMPL std
    #include <vector>
    #include <set>
    #include <unordered_map>
    #include <optional>
#endif

#include <fstream>

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

    enum class API : u32 {
        eNone,
        eOpenGL,
        eVulkan,
        eDirectX,
        eMetal,
    };

    enum class LogLevel : u32 {
        eTrace,
        eDebug,
        eInfo,
        eWarn,
        eError,
        eFatal,
    };

    enum class Result : u32 {
        eSuccess,
        eFailed,
        eAlreadyDestroyed,
        eNotImplemented,
        eDeviceNotFound,
    };
    std::string to_string(Result res);

    class RuntimeComponent {
    public:
        virtual Result create() = 0;
        virtual Result destroy() = 0;
        virtual Bool is_destroyed() final { return destroyed; }
    protected:
        Bool destroyed = MG_TRUE;
    };

    #define MANGO_NULL_MACRO
    #define MANGO_NO_INIT_VAULE MANGO_NULL_MACRO
    #define MANGO_READONLY const
    #define MANGO_READWRITE MANGO_NULL_MACRO


    #define __define_member_getter(type, type_descriptor, is_const, member_name) \
    public: \
        is_const type type_descriptor get_##member_name() is_const { return member_name; }

    #define __define_private_member(type, is_readonly, is_pointer, member_name, value) \
    private: \
        is_readonly type is_pointer member_name { value };

    #define __define_member(type, is_readonly, is_pointer, is_refrence, member_name, value) \
    __define_private_member(type, is_readonly, is_pointer, member_name, value) \
    __define_member_getter(type, is_pointer is_refrence, const, member_name)

    #define define_private_member(type, member_name, value) \
    __define_private_member(type, MANGO_READWRITE, MANGO_NULL_MACRO, member_name, value)

    #define define_member(type, member_name, value) \
    __define_member(type, MANGO_READWRITE, MANGO_NULL_MACRO, &, member_name, value)

    #define define_private_readonly_member(type, member_name, value) \
    __define_private_member(type, MANGO_READONLY, MANGO_NULL_MACRO, member_name, value)

    #define define_readonly_member(type, member_name, value) \
    __define_member(type, MANGO_READONLY, MANGO_NULL_MACRO, &, member_name, value)

    #define define_extern_writeable_member(type, member_name, value) \
    define_member(type, member_name, value) \
    __define_member_getter(type, &, MANGO_NULL_MACRO, member_name)

    #define define_private_pointer(type, member_name, value) \
    __define_private_member(type, MANGO_READWRITE, *, member_name, value)

    #define define_pointer(type, member_name, value) \
    __define_member(type, MANGO_READWRITE, *, MANGO_NULL_MACRO, member_name, value)

    #define define_private_readonly_pointer(type, member_name, value) \
    __define_private_member(type, MANGO_READONLY, *, member_name, value)

    #define define_readonly_pointer(type, member_name, value) \
    __define_member(type, MANGO_READONLY, *, MANGO_NULL_MACRO, member_name, value)

    #define define_extern_writeable_pointer(type, member_name, value) \
    define_pointer(type, member_name, value) \
    __define_member_getter(type, *, MANGO_NULL_MACRO, member_name)


    #define component_create() \
    if (destroyed == ::MangoRHI::MG_FALSE) { \
        RHI_WARN("{} --> {}", __FILE__, __LINE__) \
        RHI_WARN("Create a runtime component without destroy the old one") \
        destroy(); \
    } \
    destroyed = ::MangoRHI::MG_FALSE;

    #define component_destroy() \
    if (destroyed == ::MangoRHI::MG_TRUE) { \
        return ::MangoRHI::Result::eAlreadyDestroyed; \
    } \
    destroyed = ::MangoRHI::MG_TRUE;


    struct ColorClearValue {
        f32 r, g, b, a;
    };

    struct DepthStencilClearValue {
        f32 depth;
        u32 stencil;
    };

    union ClearValue {
        ColorClearValue color;
        DepthStencilClearValue depth_stencil;
    };

    enum class RenderTargetUsage : u32 {
        eColor,
        eDepth,
    };

    enum class RenderTargetLayout : u32 {
        eUndefined,
        eColor
    };

    enum class PipelineBindPoint : u32 {
        eGraphicsPipeline,
        eComputePipeline,
    };
    
    enum class PipelineStage : u32 {
        eColorOutput,
    };

    enum class Access : u32 {
        eNone,
        eColorRenderTargetWrite,
    };

    struct SubpassStageInfo {
        const char *name;
        PipelineStage stage;
        Access access;
    };

    enum class CommandLevel : u32 {
        ePrimary,
        eSecondary,
    };

    enum class Topology : u32 {
        ePointList,
        eLineList,
        eTriangleList,
    };

    enum class PolygonMode : u32 {
        ePoint,
        eLine,
        eFill,
    };

    enum class FrontFace : u32 {
        eCouterClockwise,
        eClockwise,
    };

    enum class CullMode : u32 {
        eNone,
        eBack,
        eFront,
        eAll,
    };

    enum class VertexInputType : u32 {
        eInt2,
        eInt3,
        eInt4,
        eFloat2,
        eFloat3,
        eFloat4,
    };

    enum class VertexInputRate : u32 {
        ePerVertex,
        ePerInstance,
    };

    enum class DescriptorStage : u32 {
        eVertex,
        eFragment,
        eVertexAndFragment,
    };

    struct Viewport {
        f32 x, y;
        f32 width, height;
        f32 min_depth, max_depth;
    };

    struct Scissor {
        i32 x, y;
        u32 width, height;
    };

    #define MANGORHI_SURFACE_RENDER_TARGET_NAME "surface"
    #define MANGORHI_EXTERNAL_SUBPASS_NAME "external"

    #define __default_construction(cls) \
    public: \
        cls() = default;

    #define __no_copy_construction(cls) \
    private: \
        cls(const cls &) = delete; \
        cls &operator=(const cls &) = delete;
    
    #define __no_move_construction(cls) \
    private: \
        cls(cls &&) = delete; \
        cls &operator=(cls &&) = delete;


    #define no_copy_construction(cls) \
    __default_construction(cls) \
    __no_copy_construction(cls)

    #define no_move_construction(cls) \
    __default_construction(cls) \
    __no_move_construction(cls)

    #define no_copy_and_move_construction(cls) \
    __default_construction(cls) \
    __no_copy_construction(cls) \
    __no_move_construction(cls) \

    class Context;

    MangoRHI_API Result initialize(API api);
    MangoRHI_API Result quit();
    MangoRHI_API Context *get_context();
    STL_IMPL::vector<char> read_binary_file(const char *filename);
}

#include "logger.hpp"
