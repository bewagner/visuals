cmake_minimum_required(VERSION 3.0)
project(OpenCVKeyPointDetector LANGUAGES CXX)

set(SOURCES
        src/main.cpp
        src/FaceDetector.cpp
        src/KeyPointDetector.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

# OpenCV setup
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Introduce preprocessor variables to keep paths of asset files
set(FACE_DETECTION_CONFIGURATION "${PROJECT_SOURCE_DIR}/assets/deploy.prototxt")
set(FACE_DETECTION_WEIGHTS "${PROJECT_SOURCE_DIR}/assets/res10_300x300_ssd_iter_140000_fp16.caffemodel")
set(KEY_POINT_DETECTION_MODEL "${PROJECT_SOURCE_DIR}/assets/lbfmodel.yaml")

target_compile_definitions(${PROJECT_NAME} PRIVATE FACE_DETECTION_CONFIGURATION="${FACE_DETECTION_CONFIGURATION}")
target_compile_definitions(${PROJECT_NAME} PRIVATE FACE_DETECTION_WEIGHTS="${FACE_DETECTION_WEIGHTS}")
target_compile_definitions(${PROJECT_NAME} PRIVATE KEY_POINT_DETECTION_MODEL="${KEY_POINT_DETECTION_MODEL}")

