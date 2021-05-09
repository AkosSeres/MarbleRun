cmake_minimum_required(VERSION 3.0.0)
project(phy3d)

if(POLICY CMP0072)
    cmake_policy(SET CMP0072 OLD)
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
find_package(OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

configure_file(base.scene base.scene COPYONLY)

add_executable(phy3d src/main.cpp src/Ball.cpp src/Camera.cpp src/Matrix.cpp src/Model.cpp src/ObjModel.cpp src/Scene3D.cpp src/SDLInstance.cpp src/Shaders.cpp src/SphereModel.cpp src/Vec3.cpp)
if(WIN32)
    target_link_libraries(phy3d SDL2::SDL2-static ${SDL2_LIBRARIES})
else()
    target_link_libraries(phy3d ${SDL2_LIBRARIES})
endif()

target_link_libraries(phy3d GLESv2)
