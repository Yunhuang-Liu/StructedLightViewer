cmake_minimum_required(VERSION 3.4)

project(StructedLightViewer CXX CUDA)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Core OpenGLExtensions Quick)
find_package(CUDA REQUIRED)
if(CUDA_FOUND)
    add_definitions(-DCUDA)
endif()
find_package(StructedLightCamera REQUIRED)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5OpenGLExtensions_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CUDA_INCLUDE_DIRS}
    ${StructedLightCamera_INCLUDE_DIRS}
    )

file(GLOB_RECURSE CURRENT_HEADER ./include/*.h ./ui/*.h)
source_group("Header" FILES ${CURRENT_HEADER})
file(GLOB_RECURSE CURRENT_SOURCE ./main.cpp ./src/*.c ./src/*.cpp ./ui/*.cpp)
source_group("Source" FILES ${CURRENT_SOURCE})
file(GLOB_RECURSE CURRENT_Qrc ./*.qrc)
source_group("Qrc" FILES ${CURRENT_Qrc})
file(GLOB_RECURSE CURRENT_CUDASource  ./include/*.cuh ./src/*.cu ./src/*.cuh)
source_group("CUDASource" FILES ${CURRENT_CUDASource})
file(GLOB_RECURSE CURRENT_Qml ./ui/*.qml)
source_group("Qml" FILES ${CURRENT_Qml})

link_directories(
    ./libs)

add_executable(
    ${PROJECT_NAME}
    ${CURRENT_Qrc}
    ${CURRENT_HEADER}
    ${CURRENT_SOURCE}
    ${CURRENT_CUDASource}
    ${CURRENT_Qml}
    )

target_link_libraries(
    ${PROJECT_NAME}
    Qt5::Core
    Qt5::OpenGLExtensions
    Qt5::Quick
    ${OpenCV_LIBRARIES}
    ${CUDA_LIBRARIES}
    ${StructedLightCamera_LIBRARIES})
