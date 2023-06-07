project(GBuffer)
add_executable(GBuffer main.cpp)
target_link_directories(GBuffer PUBLIC ../../build)
target_link_libraries(GBuffer PUBLIC MangoRHI glfw)
