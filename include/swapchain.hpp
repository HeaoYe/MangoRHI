#pragma once

#include "commons.hpp"

namespace MangoRHI {
    class Swapchain : public RuntimeComponent {
    public:
        virtual Result recreate() = 0;
        virtual Result acquire_next_frame() = 0;
        virtual Result present() = 0;
    };
}
