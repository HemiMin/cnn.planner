cmake_minimum_required(VERSION 2.6)
project(e-PlaNNer)
set(VERSION_MAJOR 4)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

find_package(GLOG REQUIRED)
if (GLOG_FOUND)
  set (GLOG_LINKING_FLAG "-lglog")
else (GLOG_FOUND)
  message("Google glog need to be installed for logging system.")
endif (GLOG_FOUND)
find_package(Threads REQUIRED)
if (THREADS_HAVE_PTHREAD_ARG)
  set (PTHREAD_LINKING_FALG "-pthread")
else (THREADS_HAVE_PTHREAD_ARG)
  message("Pthread need to be installed.")
endif (THREADS_HAVE_PTHREAD_ARG)

file(GLOB GRAPH_SRC_FILES "src/graph/*.cc")
file(GLOB LOOP_SRC_FILES  "src/loop/*.cc")
file(GLOB ARCH_SRC_FILES  "src/arch/*.cc")
file(GLOB PARAM_SRC_FILES "src/parameter/*.cc")
file(GLOB SIM_SRC_FILES   "src/simulation/*.cc")
file(GLOB GAIA_SRC_FILES  "src/codegen/ir_gaia/*.cc")
file(GLOB ANLYS_SRC_FILES "src/analysis/*.cc")
file(GLOB STATS_SRC_FILES "src/statistic/*.cc")

set(COMPILER "src/compiler.cc")
set(COMPILER_SRC_FILES  ${GRAPH_SRC_FILES}  
                        ${LOOP_SRC_FILES}
                        ${ARCH_SRC_FILES}
                        ${PARAM_SRC_FILES}
                        ${SIM_SRC_FILES}
                        ${GAIA_SRC_FILES}
                        ${COMPILER})
set(PROFILER "src/profiler.cc")
set(PROFILER_SRC_FILES  ${LOOP_SRC_FILES}
                        ${ARCH_SRC_FILES}
                        ${PARAM_SRC_FILES}
                        ${ANLYS_SRC_FILES}
                        ${STATS_SRC_FILES}
                        ${PROFILER})

set(CMAKE_C_COMPILER "g++")

include_directories(include)
link_libraries(glog)
link_libraries(pthread)

add_compile_options(-std=c++11 -Ofast -Wall -Werror ${GLOG_LINKING_FLAG} ${PTHREAD_LINKING_FLAG})

add_executable(compiler ${COMPILER_SRC_FILES})
add_executable(profiler ${PROFILER_SRC_FILES})
# target_compile_definitions(cnn_planner_manual PRIVATE -DMANUAL)
install ( TARGETS compiler profiler
          RUNTIME DESTINATION /usr/local/bin
        )

# Doxygen
option(BUILD_DOC "Create and install the HTML based API
documentation (requires Doxygen)" ${DOXYGEN_FOUND})

find_package(Doxygen)
if (DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

  add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif(DOXYGEN_FOUND)
