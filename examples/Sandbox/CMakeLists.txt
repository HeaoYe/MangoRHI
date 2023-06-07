project(Sandbox)
add_executable(Sandbox sandbox.cpp)
target_link_directories(Sandbox PUBLIC ../../build/MangoRHI)
target_link_libraries(Sandbox PUBLIC MangoRHI glfw)
