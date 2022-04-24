cmake_minimum_required(VERSION 3.10)
# set(PROJECT_NAME "ocvdemo")
project(ocvdemo)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})


include(ExternalProject)
ExternalProject_Add(
    opencv-contrib
    URL ${CMAKE_SOURCE_DIR}/pkgs/opencv_contrib-4.5.0.zip
    # GIT_REPOSITORY https://github.com/opencv/opencv_contrib.git
    # GIT_TAG 4.5.3
    PREFIX opencv-contrib
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "Skipping configuring step."
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping building step."
    INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step."
    UPDATE_COMMAND ""	
)
set(OPENCV_CONTRIB_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/opencv-contrib/src/opencv-contrib/modules)

ExternalProject_Add(
    opencv
    URL ${CMAKE_SOURCE_DIR}/pkgs/opencv-4.5.0.zip
    # GIT_REPOSITORY https://github.com/opencv/opencv.git
    # GIT_TAG 4.5.3
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/thirdparty/opencv
               -DBUILD_DOCS:BOOL=OFF
               -DBUILD_EXAMPLES:BOOL=OFF
               -DBUILD_NEW_PYTHON_SUPPORT:BOOL=OFF
               -DBUILD_PACKAGE:BOOL=OFF
               -DBUILD_SHARED_LIBS:BOOL=ON
               -DBUILD_TESTS:BOOL=OFF
               -DBUILD_PERF_TESTS:BOOL=OFF
               -DBUILD_opencv_apps:BOOL=OFF
               -DCMAKE_BUILD_TYPE:STRING=Release
               -DWITH_GTK:BOOL=ON
               -DWITH_V4L:BOOL=ON
               -DOPENCV_EXTRA_MODULES_PATH=${OPENCV_CONTRIB_MODULE_PATH}
    PREFIX opencv
    UPDATE_COMMAND ""
)
add_dependencies(opencv opencv-contrib)
file(GLOB OPENCV_LIBS ${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib/${CMAKE_STATIC_LIBRARY_PREFIX}*.so)
# set(OPENCV_LIBS ${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib/${CMAKE_STATIC_LIBRARY_PREFIX}opencv_core${CMAKE_SHARED_LIBRARY_SUFFIX}
#                 ${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib/${CMAKE_STATIC_LIBRARY_PREFIX}opencv_highgui${CMAKE_SHARED_LIBRARY_SUFFIX}
#                 ${CMAKE_SOURCE_DIR}/thirdparty/opencv/lib/${CMAKE_STATIC_LIBRARY_PREFIX}opencv_imgcodecs${CMAKE_SHARED_LIBRARY_SUFFIX}
# )
set(OPENCV_INCLUDE ${CMAKE_SOURCE_DIR}/thirdparty/opencv/include/opencv4)


aux_source_directory(. DIR_SRCS)
message("-----------------------------------")
message(${DIR_SRCS})
message(${PROJECT_NAME})
message("-----------------------------------")
 
INCLUDE_DIRECTORIES(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/include
)
#------------------------ src  --------------------
add_subdirectory(src)
set(ncnn_DIR 
${CMAKE_SOURCE_DIR}/thirdparty/ncnn/lib/cmake/ncnn
 CACHE PATH "Directory that contains ncnnConfig.cmake"
)
find_package(ncnn REQUIRED)
# set(NCNN_INCLUDE
# ${CMAKE_SOURCE_DIR}/thirdparty/ncnn/include
# ${CMAKE_SOURCE_DIR}/thirdparty/ncnn/include/ncnn
# ${CMAKE_SOURCE_DIR}/thirdparty/ncnn/include/glslang
# ${CMAKE_SOURCE_DIR}/thirdparty/ncnn/include/glslang/include
# )
# file(GLOB NCNN_HEADS_FILE ${CMAKE_SOURCE_DIR}/thirdparty/ncnn/include/ncnn/*.h)
# include_directories(${PROJECT_NAME} ${NCNN_INCLUDE}
# ${NCNN_HEADS_FILE}
# )

 
# add_executable(ocvdemo ${PROJECT_SOURCE_DIR}/main.cpp)
add_executable(${PROJECT_NAME} 
${DIR_SRCS}
)
include_directories(${PROJECT_NAME} ${OPENCV_INCLUDE})
target_link_libraries(${PROJECT_NAME}
    ${OPENCV_LIBS} 
    ncnn
)

# add_dependencies(${PROJECT_NAME} opencv)
# include_directories(ocvdemo ${OPENCV_INCLUDE})
# target_link_libraries(ocvdemo ${OPENCV_LIBS})
