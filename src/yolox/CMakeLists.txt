cmake_minimum_required(VERSION 3.14)
aux_source_directory(. DIR_SUB_SRCS)

# set(OpenCV_SHARED OFF)
# set(OpenCV_STATIC ON)

# find_package( OpenCV )
# if( OpenCV_FOUND )
# list( APPEND ThirdParty_LIBS ${OpenCV_LIBS} )
#     include_directories(LIB_YOLOX ${OpenCV_INCLUDE_DIRS} )
# endif( OpenCV_FOUND )

include_directories(LIB_YOLOX ${OpenCV_INCLUDE_DIRS} )

message( "-yolox-------${DIR_SUB_SRCS}\n${OpenCV_INCLUDE_DIRS}\n ${OpenCV_LIBS}\nOpenCV_DIR:${OpenCV_DIR}")
message("---------\n------OpenCV:${OpenCV}")

include_directories(LIB_YOLOX 
yolox.h
)


ADD_LIBRARY(LIB_YOLOX STATIC 
    yolox.cpp
)

target_link_libraries(LIB_YOLOX
    ${OpenCV_LIBS}
    ncnn
)
SET_TARGET_PROPERTIES (LIB_YOLOX PROPERTIES OUTPUT_NAME "yolox")
GET_TARGET_PROPERTY (OUTPUT_VALUE LIB_YOLOX OUTPUT_NAME)
MESSAGE (STATUS "This is the LIB_YOLOX OUTPUT_NAME: " ${OUTPUT_VALUE})

SET_TARGET_PROPERTIES (LIB_YOLOX PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES (LIB_YOLOX PROPERTIES VERSION 1.0 SOVERSION 1)