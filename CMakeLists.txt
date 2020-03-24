cmake_minimum_required(VERSION 3.15)
project(visual)

add_subdirectory(Cinder)
add_subdirectory(dlib)
add_subdirectory(opencv)
set(OPENCV_ENABLE_NONFREE ON)



add_executable(app src/main.cpp)
target_link_libraries(app dlib::dlib)

#add_subdirectory(Cinder)

# get_filename_component(CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Cinder" ABSOLUTE)
# include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

# ci_make_app(
#         CINDER_PATH ${CINDER_PATH}
#         SOURCES src/main.cpp
#         INCLUDES include/
# )




