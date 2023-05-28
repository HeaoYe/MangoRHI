#pragma once

#include "commons.hpp"

namespace MangoRHI {
    class DescriptorSet : public RuntimeComponent {
    public:
        virtual void add_descriptor(DescriptorUsage usage, DescriptorStage stage, u32 count) = 0;
    };
}
