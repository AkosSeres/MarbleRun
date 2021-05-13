cmake_minimum_required(VERSION 3.0.0)
project(marblerun)

if(POLICY CMP0072)
    cmake_policy(SET CMP0072 OLD)
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)

configure_file(base.scene base.scene COPYONLY)

add_executable(marblerun src/main.cpp src/Ball.cpp src/Camera.cpp src/Matrix.cpp src/Model.cpp src/ObjModel.cpp src/Scene3D.cpp src/Shaders.cpp src/SphereModel.cpp src/Vec3.cpp)
if(WIN32)
    target_link_libraries(marblerun SDL2::SDL2-static ${SDL2_LIBRARIES})
else()
    target_link_libraries(marblerun ${SDL2_LIBRARIES})
endif()

target_link_libraries(marblerun GLESv2)
