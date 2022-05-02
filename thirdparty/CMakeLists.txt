cmake_minimum_required(VERSION 3.14)
# make and build opencv immediately
# set(PROJECT_NAME "ocvdemo")
set(ncnn_zip_path "${CMAKE_SOURCE_DIR}/pkgs/ncnn-20220420-ubuntu-1804.zip")
set(thirdparty_path ${CMAKE_SOURCE_DIR}/thirdparty)

message("TAG:thirdparty -----CMAKE_GENERATOR_PLATFORM---${CMAKE_GENERATOR_PLATFORM}------------- tar rm ncnn------------------ over------------")
EXECUTE_PROCESS(
    COMMAND rm -rf ${CMAKE_SOURCE_DIR}/thirdparty/${ncnn_zip_name}
    WORKING_DIRECTORY "${thirdparty_path}"
    RESULT_VARIABLE ncnn
)

EXECUTE_PROCESS(
    COMMAND unzip ${CMAKE_SOURCE_DIR}/pkgs/${ncnn_zip_name}.zip
    WORKING_DIRECTORY "${thirdparty_path}"
    RESULT_VARIABLE ncnn
)
message("TAG:thirdparty ------ncnn-zip-name----${ncnn_zip_name}----------- tar ncnn------------------ over------------")
EXECUTE_PROCESS(
    COMMAND mv ${CMAKE_SOURCE_DIR}/thirdparty/${ncnn_zip_name} ${CMAKE_SOURCE_DIR}/thirdparty/ncnn
    WORKING_DIRECTORY "${thirdparty_path}"
    RESULT_VARIABLE ncnn
)

# ------------------ Eigen --------------------------
# EXECUTE_PROCESS(
#     COMMAND unzip "${CMAKE_SOURCE_DIR}/pkgs/eigen-3.4.0.zip"
#     WORKING_DIRECTORY "${thirdparty_path}"
#     RESULT_VARIABLE Eigen3
# )
# message("TAG:thirdparty --------------------- tar Eigen3 ------------------ over------------")
# EXECUTE_PROCESS(
#     COMMAND mv ${CMAKE_SOURCE_DIR}/thirdparty/eigen-3.4.0 ${CMAKE_SOURCE_DIR}/thirdparty/eigen
#     WORKING_DIRECTORY "${thirdparty_path}"
#     RESULT_VARIABLE eigen
# )

include(FetchContent)
FetchContent_Declare(
	opencv_contrib
    URL ${CMAKE_SOURCE_DIR}/pkgs/opencv_contrib-4.5.0.zip
    # GIT_REPOSITORY https://github.com/opencv/opencv_contrib.git
    # GIT_TAG 4.5.3
    PREFIX opencv_contrib
)
FetchContent_Populate(opencv_contrib)

message("TAG:thirdparty - CMakeLists.txt: CMAKE_CURRENT_SOURCE_DIR:${CMAKE_CURRENT_SOURCE_DIR}")
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_script/build_external_project.cmake)

message("TAG:thirdparty - CMakeLists.txt: CMAKE_SOURCE_DIR:${CMAKE_SOURCE_DIR}\nLINK_OPENCV_SHARED_LIB:${LINK_OPENCV_SHARED_LIB}")

build_external_project(
    opencv 
    ${PROJECT_BINARY_DIR}/external
    ${CMAKE_SOURCE_DIR}/pkgs/opencv-4.5.0.zip
        -DBUILD_opencv_core=ON
        -DBUILD_opencv_highgui=ON
        -DBUILD_opencv_imgproc=ON
        -DBUILD_opencv_contrib=ON
        -DBUILD_DOCS:BOOL=FALSE
        -DBUILD_EXAMPLES:BOOL=FALSE
        -DBUILD_TESTS:BOOL=FALSE
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBUILD_SHARED_LIBS:BOOL=${LINK_OPENCV_SHARED_LIB}
        -DBUILD_NEW_PYTHON_SUPPORT:BOOL=OFF
        -DBUILD_WITH_DEBUG_INFO=OFF
        -DWITH_CUDA:BOOL=FALSE
        -DWITH_FFMPEG:BOOL=TRUE
        -DWITH_GSTREAMER:BOOL=OFF
        -DWITH_MSMF:BOOL=ON
        -DWITH_V4L:BOOL=ON
        -DWITH_IPP:BOOL=FALSE
        -DBUILD_PERF_TESTS:BOOL=FALSE
        -DBUILD_PNG:BOOL=ON
        -DBUILD_JPEG:BOOL=ON
        -DBUILD_WITH_STATIC_CRT:BOOL=OFF
        -DBUILD_FAT_JAVA_LIB=OFF
        -DWITH_EIGEN:BOOL=ON
        -USE_EIGEN=${thirdparty_path}/eigen/include/eigen3
        -DCMAKE_INSTALL_PREFIX:PATH=${thirdparty_path}/opencv
        -DOPENCV_EXTRA_MODULES_PATH=${opencv_contrib_SOURCE_DIR}/modules
)
