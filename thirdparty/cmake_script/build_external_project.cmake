
# Source: https://github.com/myarichuk/OpenCV.CMake
# This function is used to force a build on a dependant project at cmake configuration phase.
# credit: https://stackoverflow.com/a/23570741/320103
function (build_external_project target prefix url) #FOLLOWING ARGUMENTS are the CMAKE_ARGS of ExternalProject_Add
    set(trigger_base_build_dir ${CMAKE_SOURCE_DIR}/build)
    set(trigger_build_target ${trigger_base_build_dir}/tmp_${target})
    set(trigger_build_dir ${trigger_build_target}/build)
    message("TAG:thirdparty cmake_script build_external_project--target:${target}, prefix:${prefix}")
    #mktemp dir in build tree
    file(MAKE_DIRECTORY ${trigger_build_target} ${trigger_build_dir})

    #generate false dependency project
    set(CMAKE_LIST_CONTENT "
        cmake_minimum_required(VERSION 3.0)
        project(ExternalProjectCustom)
        include(ExternalProject)
        ExternalProject_add(${target}
            PREFIX ${target}
            URL  ${url}
            CMAKE_ARGS ${ARGN})
        add_custom_target(trigger_${target})
        add_dependencies(trigger_${target} ${target})
    ")

    file(WRITE ${trigger_build_target}/CMakeLists.txt "${CMAKE_LIST_CONTENT}")
    message("TAG:thirdparty cmake_script build_external_project-CMakelists--\n${CMAKE_LIST_CONTENT}")

    execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ..
        WORKING_DIRECTORY ${trigger_build_target}/build
        )
    execute_process(COMMAND ${CMAKE_COMMAND} --build . -j8
        WORKING_DIRECTORY ${trigger_build_target}/build
        )

endfunction()