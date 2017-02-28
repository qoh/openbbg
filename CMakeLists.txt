cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

# cmakepp
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmakepp.cmake")
  file(DOWNLOAD "https://github.com/toeb/cmakepp/releases/download/v0.3.1-alpha/cmakepp.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmakepp.cmake" EXPECTED_MD5 "c47e1c919aad86a447655e74ae31b6cf")
endif()
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmakepp.cmake")

project(OpenBBG)


# Paths

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(BIN_EXT_DIR ${BIN_DIR}/external)
set(SRC_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(OUT_DIR ${PROJECT_SOURCE_DIR}/bin)


# Targets

set(TARGET_OPENBBG OpenBBG)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Macros.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Targets.cmake")
