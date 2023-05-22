#include "MangoRHI.hpp"

int main() {
    MangoRHI::set_logger_level(MangoRHI::LogLevel::eDebug);
    MangoRHI::initialize(MangoRHI::API::eVulkan);

    MangoRHI::Context *ctx = MangoRHI::create_context();
    ctx->resize(640, 640);
    ctx->set_application_name("MangoRHI Sanbox App");
    ctx->set_device_name("Nvidia GeForce RTX 4090");
    ctx->create();
    ctx->destroy();

    MangoRHI::quit();
    return 0;
}
