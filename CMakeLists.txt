cmake_minimum_required(VERSION 3.5)
project(visuals LANGUAGES CXX)

set(SOURCES
                src/main.cpp
        )

# Cinder setup
#add_subdirectory(Cinder)
#get_filename_component(CINDER_PATH "${PROJECT_SOURCE_DIR}/Cinder" ABSOLUTE)
#
#include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")
#ci_make_app(
#        APP_NAME "${PROJECT_NAME}"
#        CINDER_PATH ${CINDER_PATH}
#        SOURCES ${SOURCES}
#        INCLUDES include/
#)


add_executable(${PROJECT_NAME} ${SOURCES})




find_package(Git REQUIRED)
include(ExternalProject)

# OpenCV
set(OPENCV_CONTRIB_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/opencv_contrib)
ExternalProject_Add(opencv_contrib
        GIT_REPOSITORY https://github.com/opencv/opencv_contrib
        GIT_TAG e6f32c6a69043456a806a4e802ee3ce7b7059c93 # 4.3.0
        SOURCE_DIR opencv_contrib
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        )

set(OPENCV_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/opencv)
ExternalProject_Add(opencv
        GIT_REPOSITORY https://github.com/opencv/opencv
        GIT_TAG 01b2c5a77ca6dbef3baef24ebc0a5984579231d9 # 4.3.0
        SOURCE_DIR opencv
        BINARY_DIR opencv-build
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${OPENCV_INSTALL_LOCATION} -DBUILD_LIST="core,imgproc,highgui,dnn" -DOPENCV_EXTRA_MODULES_PATH=${CMAKE_BINARY_DIR}/opencv_contrib/modules
        )
add_dependencies(opencv opencv_contrib)
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENCV_INSTALL_LOCATION}/include/opencv4)
link_directories(${OPENCV_INSTALL_LOCATION}/lib) # target_link_directories would be better here, but the cmake version on TravisCI does not support this yet
target_link_libraries(${PROJECT_NAME} opencv_core)
add_dependencies(${PROJECT_NAME} opencv)

# Set C++ standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

# Introduce preprocessor variables to keep paths of asset files
set(FACE_DETECTION_CONFIGURATION "${PROJECT_SOURCE_DIR}/assets/deploy.prototxt")
set(FACE_DETECTION_WEIGHTS "${PROJECT_SOURCE_DIR}/assets/res10_300x300_ssd_iter_140000_fp16.caffemodel")
set(KEYPOINT_DETECTION_MODEL "${PROJECT_SOURCE_DIR}/assets/lbfmodel.yaml")

target_compile_definitions(${PROJECT_NAME} PRIVATE FACE_DETECTION_CONFIGURATION="${FACE_DETECTION_CONFIGURATION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE FACE_DETECTION_WEIGHTS="${FACE_DETECTION_WEIGHTS}")
target_compile_definitions(${PROJECT_NAME} PRIVATE KEYPOINT_DETECTION_MODEL="${KEYPOINT_DETECTION_MODEL}")
