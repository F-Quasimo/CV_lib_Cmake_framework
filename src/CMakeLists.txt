cmake_minimum_required(VERSION 3.14)
aux_source_directory(. DIR_SUB_SRCS)
ADD_LIBRARY(base_static_lib STATIC ${DIR_SUB_SRCS})