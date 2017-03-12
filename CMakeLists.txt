cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

project(OpenBBG)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Paths

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(BIN_EXT_DIR ${BIN_DIR}/external)
set(SRC_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(OUT_DIR ${PROJECT_SOURCE_DIR}/bin)


# External

get_filename_component(MSVC_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
set(MSVC_DIR ${MSVC_DIR}/../../..)

if(MSVC)
	execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_EXT_DIR})
	execute_process(WORKING_DIRECTORY ${BIN_EXT_DIR} COMMAND ${CMAKE_COMMAND} -Wno-dev -G ${CMAKE_GENERATOR}
				-DEXTERNAL_DIR=${BIN_EXT_DIR}
				${SRC_EXT_DIR})
	execute_process(WORKING_DIRECTORY ${BIN_EXT_DIR} COMMAND cmd.exe /C "cd ${MSVC_DIR} & cd Common7/Tools & VsDevCmd.bat & cd ../../VC & vcvarsall.bat amd64 & msbuild ${BIN_EXT_DIR}/ALL_BUILD.vcxproj /m /v:n /p:Configuration=Release /t:Build")
endif()

if(UNIX)
	execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_EXT_DIR})
	execute_process(WORKING_DIRECTORY ${BIN_EXT_DIR} COMMAND ${CMAKE_COMMAND} -Wno-dev -G ${CMAKE_GENERATOR}
				-DEXTERNAL_DIR=${BIN_EXT_DIR}
				${SRC_EXT_DIR})
	execute_process(WORKING_DIRECTORY ${BIN_EXT_DIR} COMMAND make -j)
endif()

include("${BIN_EXT_DIR}/vars.cmake")


# Targets

set(TARGET_OPENBBG OpenBBG)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Macros.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Targets.cmake")
