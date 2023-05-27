#include "commons.hpp"
#include "vulkan/vulkan_context.hpp"

namespace MangoRHI {
    #define invoke_backend_func(backend, func) \
    { \
        auto res = func##_##backend(); \
        if (res != Result::eSuccess) { \
            return res; \
        } \
    }

    static API g_api;

    MangoRHI_API Result initialize(API api) {
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

    MangoRHI_API Result quit() {
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

    MangoRHI_API Context *get_context() {
        switch (g_api) {
        case API::eNone:
            return nullptr;
        case API::eOpenGL:
            return nullptr;
        case API::eVulkan:
            return vulkan_context;
        case API::eDirectX:
            return nullptr;
        case API::eMetal:
            return nullptr;
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
