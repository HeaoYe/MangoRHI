#include "MangoRHI/commons.hpp"
#include "vulkan/vulkan_context.hpp"

namespace MangoRHI {
    Result RuntimeComponent::recreate() {
        if (is_destroyed()) {
            return Result::eAlreadyDestroyed;
        }
        Result res;
        if ((res = destroy()) != Result::eSuccess) {
            RHI_ERROR("Vulkan runtime component recreate error when destroy -- {}", to_string(res))
            return res;
        }
        if ((res = create()) != Result::eSuccess) {
            RHI_ERROR("Vulkan runtime component recreate error when create -- {}", to_string(res))
            return res;
        }
        return Result::eSuccess;
    }

    #define invoke_backend_func(backend, func) \
    { \
        auto res = func##_##backend(); \
        if (res != Result::eSuccess) { \
            return res; \
        } \
    }

    static API g_api;

    MANGORHI_API Result initialize(API api) {
        g_logger.create();

        g_api = api;
        switch (api) {
        case API::eNone:
            return Result::eNotImplemented;
        case API::eOpenGL:
            return Result::eNotImplemented;
        case API::eVulkan:
            invoke_backend_func(vulkan, initialize)
            break;
        case API::eDirectX:
            return Result::eNotImplemented;
        case API::eMetal:
            return Result::eNotImplemented;
        };

        return Result::eSuccess;
    }

    MANGORHI_API Result quit() {
        switch (g_api) {
        case API::eNone:
            return Result::eNotImplemented;
        case API::eOpenGL:
            return Result::eNotImplemented;
        case API::eVulkan:
            invoke_backend_func(vulkan, quit)
            break;
        case API::eDirectX:
            return Result::eNotImplemented;
        case API::eMetal:
            return Result::eNotImplemented;
        };

        g_logger.destroy();

        return Result::eSuccess;
    }

    MANGORHI_API Reference<Context> get_context() {
        switch (g_api) {
        case API::eNone:
            return Reference<Context>();
        case API::eOpenGL:
            return Reference<Context>();
        case API::eVulkan:
            return *vulkan_context;
        case API::eDirectX:
            return Reference<Context>();
        case API::eMetal:
            return Reference<Context>();
        };
    }

    #undef invoke_backend_func

    #define case_result(value) \
    case Result::e##value: \
        return #value;

    std::string to_string(Result res) {
        switch (res) {
        case_result(Success)
        case_result(Failed)
        case_result(AlreadyDestroyed)
        case_result(NotImplemented)
        case_result(DeviceNotFound)
        case_result(NeedToRecreate)
        case_result(Recreating)
        }
    }

    #undef case_result

    STL_IMPL::vector<char> read_binary_file(const char *filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        STL_IMPL::vector<char> buffer(0);
        if (!file.is_open()) {
            RHI_ERROR("Failed open binary file {}", filename)
            return STL_IMPL::move(buffer);
        }
        u32 size = file.tellg();
        buffer.resize(size);
        file.seekg(0);
        file.read(buffer.data(), size);
        file.close();
        return STL_IMPL::move(buffer);
    }
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION
