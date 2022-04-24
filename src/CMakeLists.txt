
aux_source_directory(. DIR_SUB_SRCS)
include_directories(base_static_lib ${NCNN_INCLUDE})
ADD_LIBRARY(base_static_lib STATIC ${DIR_SUB_SRCS})