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

    #define MANGO_ASSERT(expr) \
    if(!(expr)) { \
        RHI_FATAL("Mango Assert Failed {} -> {}", __FILE__, __LINE__) \
        throw ::std::runtime_error("Mango Assert Failed"); \
    }
#else
    #define MANGO_RELEASE

    #define MANGO_ASSERT(expr)
#endif

#include <stdint.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <stb/stb_image.h>

#if defined (USE_EASTL)
    #define STL_IMPL eastl
    #include <EASTL/vector.h>
    #include <EASTL/set.h>
    #include <EASTL/unordered_map.h>
#else
    #define STL_IMPL ::std
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
    #define BIT(x) (1 << x)

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
        eNeedToRecreate,
        eRecreating,
    };
    std::string to_string(Result res);

    class RuntimeComponent {
    public:
        virtual Result create() = 0;
        virtual Result destroy() = 0;
        virtual Bool is_destroyed() final { return destroyed; }
        
        virtual Result recreate();
    protected:
        Bool destroyed = MG_TRUE;
    };

    #define __MANGO_NULL_MACRO
    #define __MANGO_READONLY const
    #define __MANGO_READWRITE __MANGO_NULL_MACRO

    #define __define_member(is_readonly, type, is_pointer, member_name, ...) \
    private: \
        is_readonly type is_pointer member_name { __VA_ARGS__ };

    #define __define_member_getter(type, type_descriptor, is_const, member_name) \
    public: \
        is_const type type_descriptor get_##member_name() is_const { return member_name; }
    
    #define __define_member_setter(is_readonly, type, type_descriptor, is_override, member_name) \
    public: \
        void set_##member_name(is_readonly type type_descriptor member_name) is_override { this->member_name = member_name; }
    
    #define __define_member_setter_with_translator(SrcType, is_override, member_name, translator) \
    public: \
        void set_##member_name(SrcType member_name) is_override { this->member_name = translator(member_name); }
    
    #define define_private_member(type, member_name, ...) \
    __define_member(__MANGO_READWRITE, type, __MANGO_NULL_MACRO, member_name, __VA_ARGS__)

    #define define_private_readonly_member(type, member_name, ...) \
    __define_member(__MANGO_READONLY, type, __MANGO_NULL_MACRO, member_name, __VA_ARGS__)

    #define define_member(getter, setter, type, member_name, ...) \
    define_private_member(type, member_name, __VA_ARGS__) \
    getter(type, &, member_name) \
    setter(__MANGO_READWRITE, type, &, member_name)

    #define define_member_with_translator(getter, setter, SrcType, DstType, member_name, translator, ...) \
    define_private_member(DstType, member_name, translator(__VA_ARGS__)) \
    getter(DstType, &, member_name) \
    setter(SrcType, member_name, translator)

    #define define_readonly_member(getter, setter, type, member_name, ...) \
    define_private_readonly_member(type, member_name, __VA_ARGS__) \
    getter(type, &, member_name) \
    setter(__MANGO_READONLY, type, &, member_name)

    #define define_private_pointer(type, member_name, ...) \
    __define_member(__MANGO_READWRITE, type, *, member_name, __VA_ARGS__)

    #define define_private_readonly_pointer(type, member_name, ...) \
    __define_member(__MANGO_READONLY, type, *, member_name, __VA_ARGS__)

    #define define_pointer(getter, setter, type, member_name, ...) \
    define_private_pointer(type, member_name, __VA_ARGS__) \
    getter(type, *, member_name) \
    setter(__MANGO_READWRITE, type, *, member_name)

    #define define_readonly_pointer(getter, setter, type, member_name, ...) \
    define_private_readonly_pointer(type, member_name, __VA_ARGS__) \
    getter(type, *, member_name) \
    setter(__MANGO_READONLY, type, *, member_name)

    #define MANGO_NO_INIT_VAULE __MANGO_NULL_MACRO
    
    #define MANGO_NO_GETTER(...) __MANGO_NULL_MACRO

    #define MANGO_CONST_GETTER(type, type_descriptor, member_name) \
    __define_member_getter(type, type_descriptor, const, member_name)

    #define MANGO_MUTABLE_GETTER(type, type_descriptor, member_name) \
    __define_member_getter(type, type_descriptor, const, member_name) \
    __define_member_getter(type, type_descriptor, __MANGO_NULL_MACRO, member_name)

    #define MANGO_NO_SETTER(...) __MANGO_NULL_MACRO

    #define MANGO_SETTER(is_readonly, type, type_descriptor, member_name) \
    __define_member_setter(is_readonly, type, type_descriptor, __MANGO_NULL_MACRO, member_name)

    #define MANGO_SETTER_BASIC(is_readonly, type, type_descriptor, member_name) \
    __define_member_setter(is_readonly, type, __MANGO_NULL_MACRO, __MANGO_NULL_MACRO, member_name)

    #define MANGO_SETTER_OVERRIDE(is_readonly, type, type_descriptor, member_name) \
    __define_member_setter(is_readonly, type, type_descriptor, override, member_name)

    #define MANGO_SETTER_BASIC_OVERRIDE(is_readonly, type, type_descriptor, member_name) \
    __define_member_setter(is_readonly, type, __MANGO_NULL_MACRO, override, member_name)

    #define MANGO_SETTER_WITH_TRANSLATOR(SrcType, member_name, translator) \
    __define_member_setter_with_translator(SrcType, __MANGO_NULL_MACRO, member_name, translator)

    #define MANGO_SETTER_WITH_TRANSLATOR_OVERRIDE(SrcType, member_name, translator) \
    __define_member_setter_with_translator(SrcType, override, member_name, translator)

    #define declare_component_cls(cls_name) \
    public: \
        cls_name() = default; \
    private: \
        cls_name(const cls_name &) = delete; \
        cls_name &operator=(const cls_name &) = delete; \
        cls_name(cls_name &&) = delete; \
        cls_name &operator=(cls_name &&) = delete;

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

    #define define_flags(EnumClass) \
    struct EnumClass##Flags { EnumClass##Flags(u32 n) : bits(n) {} EnumClass##Flags(EnumClass n) : bits((u32)n) {} u32 bits; }; \
    inline EnumClass##Flags operator|(EnumClass left, EnumClass right) { return ((u32)left) | ((u32)right); } \
    inline EnumClass##Flags operator|(EnumClass##Flags left, EnumClass right) { return left.bits | ((u32)right); } \
    inline EnumClass##Flags operator|(EnumClass left, EnumClass##Flags right) { return ((u32)left) | right.bits; } \
    inline EnumClass##Flags operator|(EnumClass##Flags left, EnumClass##Flags right) { return left.bits | right.bits; }

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
        eColor,
        eDepth,
    };

    enum class PipelineBindPoint : u32 {
        eGraphicsPipeline,
        eComputePipeline,
    };
    
    enum class PipelineStage : u32 {
        eNone = BIT(0),
        eColorOutput = BIT(1),
        eEarlyFragmentTest = BIT(2),
    };
    define_flags(PipelineStage)

    enum class Access : u32 {
        eNone = BIT(0),
        eColorRenderTargetWrite = BIT(1),
        eDepthStencilRenderTargetWrite = BIT(2),
    };
    define_flags(Access)

    struct SubpassStageInfo {
        const char *name;
        PipelineStageFlags stage;
        AccessFlags access;
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

    enum class DepthCompareOp : u32 {
        eLess,
        eLessOrEqual,
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
        eNone = BIT(0),
        eVertex = BIT(1),
        eFragment = BIT(2),
    };
    define_flags(DescriptorStage)

    enum class SamplerFilter : u32 {
        eNearest,
        eLinear,
    };

    enum class SamplerAddressMode : u32 {
        eRepeat,
        eMirroredRepeat,
        eClampToEdge,
        eClampToBorder,
    };

    enum class SamplerBorderColor : u32 {
        eFloatTransparentBlack,
        eIntTransparentBlack,
        eFloatOpaqueBlack,
        eIntOpaqueBlack,
        eFloatOpaqueWhite,
        eIntOpaqueWhite,
    };

    #undef define_flags

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

    class Context;

    MangoRHI_API Result initialize(API api);
    MangoRHI_API Result quit();
    MangoRHI_API Context *get_context();
    STL_IMPL::vector<char> read_binary_file(const char *filename);
}

#include "logger.hpp"
