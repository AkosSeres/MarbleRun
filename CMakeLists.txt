cmake_minimum_required(VERSION 3.0.0)
project(marblerun)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
