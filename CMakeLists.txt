cmake_minimum_required(VERSION 3.14)
# set(PROJECT_NAME "ocvdemo")
project(ocvdemo)
# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LINK_OPENCV_SHARED_LIB OFF)
set(thirdparty_path ${CMAKE_SOURCE_DIR}/thirdparty)
message("-----------------------\nplatform:${CMAKE_GENERATOR_PLATFORM}\tCMAKE_ANDROID_ARCH_ABI:${CMAKE_ANDROID_ARCH_ABI}\nCMAKE_ARCHIVE_OUTPUT_DIRECTORY:${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
#set(CMAKE_BUILD_TYPE "Debug")

# ------------------------------ PLATFORM CONFIG ---------
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
# message(FATAL_ERROR
#     "DO NOT WASTE TIME ON WINDOWS"
# )
set(ncnn_name_platform ncnn_20220420_win_vs2019_no_vulkan)
set(OpenCV_DIR 
${CMAKE_SOURCE_DIR}/thirdparty/vs2019opencvlib/x64/vc16/staticlib
CACHE PATH "Directory that contains OpenCVConfig.cmake"
)
set(RE_CMAKE_OPENCV OFF)
	if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
    endif()

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")  
    set(ncnn_name_platform ncnn_20220420_ubuntu_1804)
    set(OpenCV_DIR 
    ${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib/cmake/opencv4
    CACHE PATH "Directory that contains OpenCVConfig.cmake"
    )
    set(RE_CMAKE_OPENCV ON)
else()
    set(ncnn_name_platform other_unknown)
endif()

# -----------------------  root dir  ------------------------
aux_source_directory(. DIR_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(thirdparty)

# ---------------------- config OpenCV ------------------
# https://answers.opencv.org/question/14611/example-of-makefile/
# set(OpenCV_DIR 
# ${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib/cmake/opencv4
#  CACHE PATH "Directory that contains OpenCVConfig.cmake"
# )
set(OpenCV_SHARED OFF)
set(OpenCV_STATIC ON)
find_package( OpenCV )
if( OpenCV_FOUND )
list( APPEND ThirdParty_LIBS ${OpenCV_LIBS} )
    include_directories( ${OpenCV_INCLUDE_DIRS} )
    message("***************OpenCV_INCLUDE_DIRS:${OpenCV_INCLUDE_DIRS}")
    LINK_DIRECTORIES()
endif( OpenCV_FOUND )

# ----------------------------- ADD NCNN -------------------
# https://github.com/Tencent/ncnn/wiki/use-ncnn-with-own-project#use-ncnn-with-own-project
set(ncnn_DIR 
${CMAKE_SOURCE_DIR}/thirdparty/ncnn/${ncnn_name_platform}/lib/cmake/ncnn
 CACHE PATH "Directory that contains ncnnConfig.cmake"
)
message("ncnn_DIR:${ncnn_DIR}")
find_package(ncnn REQUIRED)

# ----------------------------- ADD Eigen3--------------------
set(Eigen3_DIR ${CMAKE_SOURCE_DIR}/thirdparty/eigen/share/eigen3/cmake)
find_package(Eigen3)
set(Eigen3_INCLUDE ${thirdparty_path}/eigen/include/eigen3)
message("root:Eigen: ${Eigen3_LIBRARY}")

#------------------------ src and include --------------------

INCLUDE_DIRECTORIES(${PROJECT_NAME}
    # ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/yolox
)
# include_directories(${PROJECT_NAME} ${OPENCV_INCLUDE}
# #${CMAKE_SOURCE_DIR}/thirdparty/eigen/include/eigen3
# )

add_subdirectory(src)

add_executable(${PROJECT_NAME} 
    ${DIR_SRCS}
)

message("root:DIR_SRCS: ${DIR_SRCS}")
# link_directories(${PROJECT_NAME}
# ${OpenCV_DIR}
# )
message("-----OPENCV_LIBS_lib:${OPENCV_LIBS_lib}")
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS} 
    LIB_YOLOX
)
