cmake_minimum_required(VERSION 3.14)
# set(PROJECT_NAME "ocvdemo")
project(ocvdemo)
# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# -----------------------  root dir  ------------------------
aux_source_directory(. DIR_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(thirdparty)

# ---------------------- config OpenCV ------------------
# https://answers.opencv.org/question/14611/example-of-makefile/
set(OpenCV_DIR 
${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib/cmake/opencv4
 CACHE PATH "Directory that contains OpenCVConfig.cmake"
)

find_package( OpenCV )
if( OpenCV_FOUND )
list( APPEND ThirdParty_LIBS ${OpenCV_LIBS} )
    include_directories( ${OpenCV_INCLUDE_DIRS} )
endif( OpenCV_FOUND )

# ----------------------------- ADD NCNN -------------------
# https://github.com/Tencent/ncnn/wiki/use-ncnn-with-own-project#use-ncnn-with-own-project
set(ncnn_DIR 
${CMAKE_SOURCE_DIR}/thirdparty/ncnn/lib/cmake/ncnn
 CACHE PATH "Directory that contains ncnnConfig.cmake"
)
find_package(ncnn REQUIRED)

#------------------------ src and include --------------------

INCLUDE_DIRECTORIES(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/include
)
include_directories(${PROJECT_NAME} ${OPENCV_INCLUDE}
#${CMAKE_SOURCE_DIR}/thirdparty/eigen/include/eigen3
)
set(Eigen3_DIR ${CMAKE_SOURCE_DIR}/thirdparty/eigen/share/eigen3/cmake)
find_package(Eigen3)
message("root:Eigen: ${Eigen3_LIBRARY}")

add_subdirectory(src)


add_executable(${PROJECT_NAME} 
    ${DIR_SRCS}
)
message("root:DIR_SRCS: ${DIR_SRCS}")
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS} 
    ncnn
)
