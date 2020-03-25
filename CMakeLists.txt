cmake_minimum_required(VERSION 3.0)
project(visuals)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
        src/main.cpp
        )

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





