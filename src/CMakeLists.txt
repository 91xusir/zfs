# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)

project (suxian)

# set (CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/src/libs/linux)

#SET (BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}" CACHE BOOL
#	"Set to OFF to build static libraries" FORCE )

SET(BUILD_SHARED_LIBS ON)

# Default build type is debug
if (NOT CMAKE_BUILD_TYPE )
	set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Set install dir to project root
set (CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")

# Add pre compiler macro
add_definitions (-DLINUX -DENABLE_OPERATOR_NEW -fpermissive -Wno-deprecated)

if (${CMAKE_BUILD_FINAL} MATCHES True)
	add_definitions (-D_FINAL)
endif(${CMAKE_BUILD_FINAL} MATCHES True)

# Set build paramaters by each build type
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
	SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -fstack-protector-all")
	add_definitions (-DDEBUG -D_DEBUG)
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
	SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
	remove_definitions (-DDEBUG -D_DEBUG)
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

# Make sure the compiler can find include files from our Hello library.
include_directories (${PROJECT_SOURCE_DIR}/../engine/head)
include_directories (${PROJECT_SOURCE_DIR}/shared)
#include_directories (${PROJECT_SOURCE_DIR}/../engine/head/core)
#include_directories (${PROJECT_SOURCE_DIR}/../engine/head/net)
#include_directories (${PROJECT_SOURCE_DIR}/../engine/head/filepack)
link_directories (${PROJECT_SOURCE_DIR}/../engine/libs)

# Recurse into the "Hello" and "Demo" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
# add_subdirectory (zfs-engine)
add_subdirectory (login_server)
add_subdirectory (gameworld_server)
add_subdirectory (region_server)

MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE}" )
MESSAGE( STATUS "CMAKE_BUILD_FINAL = ${CMAKE_BUILD_FINAL}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "${PROJECT_NAME}_DEPENDS = \"${${PROJECT_NAME}_DEPENDS}\"" )
MESSAGE( STATUS "BUILD_WITH = \"${BUILD_WITH}\"" )
MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}" )
MESSAGE( STATUS "COMPILE_DEFINITIONS = ${COMPILE_DEFINITIONS_DEBUG}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS ) 
