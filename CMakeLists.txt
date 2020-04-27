cmake_minimum_required(VERSION 3.5)
project(visuals LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
        src/NVidiaComputeParticlesApp.cpp
        src/CameraHandler.cpp
        src/ParticleSystem.cpp
        src/detector/Detector.cpp
        src/detector/FaceDetector.cpp
        src/detector/FaceKeypoints.cpp
        src/detector/KeypointDetector.cpp

        include/detector/PairOfEyes.h
        include/ScopedBufferBase.h
        include/Ssbo.h
       // src/main.cpp
        )

 # TODO Make cinder installable via cmake
 # Cinder setup
 add_subdirectory(Cinder)
 get_filename_component(CINDER_PATH "${PROJECT_SOURCE_DIR}/Cinder" ABSOLUTE)

 include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")
 ci_make_app(
         APP_NAME "${PROJECT_NAME}"
         CINDER_PATH ${CINDER_PATH}
         SOURCES ${SOURCES}
         INCLUDES include/
)


add_executable(${PROJECT_NAME} ${SOURCES})

find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Introduce preprocessor variables to keep paths of asset files
set(FACE_DETECTION_CONFIGURATION "${PROJECT_SOURCE_DIR}/assets/deploy.prototxt")
set(FACE_DETECTION_WEIGHTS "${PROJECT_SOURCE_DIR}/assets/res10_300x300_ssd_iter_140000_fp16.caffemodel")
set(KEYPOINT_DETECTION_MODEL "${PROJECT_SOURCE_DIR}/assets/lbfmodel.yaml")

target_compile_definitions(${PROJECT_NAME} PRIVATE FACE_DETECTION_CONFIGURATION="${FACE_DETECTION_CONFIGURATION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE FACE_DETECTION_WEIGHTS="${FACE_DETECTION_WEIGHTS}")
target_compile_definitions(${PROJECT_NAME} PRIVATE KEYPOINT_DETECTION_MODEL="${KEYPOINT_DETECTION_MODEL}")

