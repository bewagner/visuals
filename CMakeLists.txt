cmake_minimum_required(VERSION 3.0)
project(visuals LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        src/main.cpp
        src/FaceDetector.cpp)

# Cinder setup
#add_subdirectory(Cinder)
#get_filename_component(CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Cinder" ABSOLUTE)
#
#include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")
#ci_make_app(
#        APP_NAME "${PROJECT_NAME}"
#        CINDER_PATH ${CINDER_PATH}
#        SOURCES ${SOURCES}
#        INCLUDES include/
#)

# OpenCV setup
find_package(OpenCV REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Copy network weights and configuration files to binary directory
set(CONFIGURATION_FILE "deploy.prototxt")
set(WEIGHTS_FILE "res10_300x300_ssd_iter_140000_fp16.caffemodel")

foreach (file "${CONFIGURATION_FILE}" "${WEIGHTS_FILE}")
    configure_file(
            ${PROJECT_SOURCE_DIR}/assets/${file}
            ${PROJECT_BINARY_DIR}
            COPYONLY
    )
endforeach (file)
target_compile_definitions(${PROJECT_NAME} PRIVATE MODEL_CONFIGURATION_FILE="${PROJECT_BINARY_DIR}/${CONFIGURATION_FILE}")
target_compile_definitions(${PROJECT_NAME} PRIVATE MODEL_WEIGHTS_FILE="${PROJECT_BINARY_DIR}/${WEIGHTS_FILE}")



## DLib setup
#find_package(dlib REQUIRED)
#find_package(Threads REQUIRED)
#target_link_libraries(${PROJECT_NAME} dlib::dlib)


## Pytorch setup
#set(Torch_DIR "${PROJECT_SOURCE_DIR}/libtorch/share/cmake/Torch")
#find_package(Torch REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
#target_link_libraries(${PROJECT_NAME} "${TORCH_LIBRARIES}")
#target_include_directories(${PROJECT_NAME} PUBLIC "${TORCH_INCLUDE_DIRS}")

