cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

# Default Environment

option(UPDATE_PACKAGE_MANIFESTS "" 0)
if(NOT DEFINED EXTERNAL_DIR)
	set(EXTERNAL_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
endif()

set(PACKAGE_DIR "${EXTERNAL_DIR}/packages")
set(MANIFEST_DIR "${EXTERNAL_DIR}/manifests")
set(TOOLCHAIN_DIR "${EXTERNAL_DIR}/toolchains")

# Project Definition

project(external)


# Platform Detection

if(CMAKE_COMPILER_IS_GNUCXX)
	set(PLATFORM "gnu64")
endif()
if(MSVC)
	set(PLATFORM "msvc64")
endif()


# cmakepp

set(FILE_CMAKEPP_CMAKE "${EXTERNAL_DIR}/cmakepp.cmake")
if(NOT EXISTS ${FILE_CMAKEPP_CMAKE})
	message("Downloading cmakepp.cmake...")
	file(DOWNLOAD "https://github.com/toeb/cmakepp/releases/download/v0.3.1-alpha/cmakepp.cmake" ${FILE_CMAKEPP_CMAKE} EXPECTED_MD5 "c47e1c919aad86a447655e74ae31b6cf" SHOW_PROGRESS)
endif()
include(${FILE_CMAKEPP_CMAKE})


# Process Manifest

set(FILE_MANIFEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/manifest.json")

message("Reading manifest.json...")
json_read(${FILE_MANIFEST_JSON})
ans(packages)

message("Updating package manifests...")
set(PACKAGE_LIST "")
foreach(pkg ${packages})
	map_get(${pkg} "name")
	ans(pkg_name)

	map_get(${pkg} "platform")
	ans(pkg_platform_map)

	map_get(${pkg_platform_map} "${PLATFORM}")
	ans(pkg_platform)

	map_get(${pkg_platform} "version")
	ans(pkg_version)

	map_get(${pkg_platform} "toolchain")
	ans(pkg_toolchain)

	map_get(${pkg} "container")
	ans(pkg_container)

	set(FILE_PKG_MANIFEST "${MANIFEST_DIR}/${pkg_name}.json")
	set(URI_PKG_MANIFEST "${pkg_container}/${pkg_name}/manifest.json")

	if(UPDATE_PACKAGE_MANIFESTS)
		# Update package manifest if md5 differs
		file(DOWNLOAD ${URI_PKG_MANIFEST}.md5 ${EXTERNAL_DIR}/tmp/md5)
		execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${EXTERNAL_DIR}/tmp/md5 ${FILE_PKG_MANIFEST}.md5 RESULT_VARIABLE cmp OUTPUT_QUIET ERROR_QUIET)
		if(${cmp})
			message("  ${pkg_name}:")
			file(DOWNLOAD ${URI_PKG_MANIFEST} ${FILE_PKG_MANIFEST} SHOW_PROGRESS)
			file(MD5 ${FILE_PKG_MANIFEST} md5)
			file(WRITE ${FILE_PKG_MANIFEST}.md5 ${md5}\n)
		endif()
	else()
		# Download package manifest if missing
		if(NOT EXISTS ${FILE_PKG_MANIFEST})
			message("======== ${pkg_name} =========")
			file(DOWNLOAD ${URI_PKG_MANIFEST} ${FILE_PKG_MANIFEST} SHOW_PROGRESS)
			file(MD5 ${FILE_PKG_MANIFEST} md5)
			file(WRITE ${FILE_PKG_MANIFEST}.md5 ${md5}\n)
		endif()
	endif()

	# Read Manifest
	json_read("${FILE_PKG_MANIFEST}")
	ans(man)

	map_get(${man} "platform")
	ans(man_platform_map)

	map_get(${man_platform_map} "${PLATFORM}")
	ans(man_platform)

	set(PKG_DISPLAY "${pkg_name}-${pkg_version}-${PLATFORM}")

	# Generate install dir
	set(PKG_INSTALL_DIR "${PACKAGE_DIR}/${PKG_DISPLAY}")

	# Generate simple name
	set(PKG_SIMPLE_NAME "${pkg_name}")
	string(REPLACE "-" "_" PKG_SIMPLE_NAME "${PKG_SIMPLE_NAME}")
	string(TOUPPER "${PKG_SIMPLE_NAME}" PKG_SIMPLE_NAME)

	map_get(${man_platform} "fileFormat")
	ans(man_file_format)
	string(REPLACE "$v" "${pkg_version}" PKG_DL_FILE "${man_file_format}")

	map_get(${man_platform} "downloadFormat")
	ans(man_download_format)
	string(REPLACE "$v" "${pkg_version}" PKG_DL_URI "${man_download_format}")

	# TODO: Determine if we want to store a copy per package version or toolchain version
	set(FILE_PKG_TOOLCHAIN "${TOOLCHAIN_DIR}/${pkg_name}-${pkg_version}-${PLATFORM}.cmake")
	if(NOT EXISTS ${FILE_PKG_TOOLCHAIN})
		set(URI_PKG_TOOLCHAIN "${pkg_container}/${pkg_name}/toolchains/${PLATFORM}/${pkg_toolchain}.cmake")
		message("  Downloading ${PKG_DISPLAY} toolchain <${URI_PKG_TOOLCHAIN}>...")
		file(DOWNLOAD ${URI_PKG_TOOLCHAIN} ${FILE_PKG_TOOLCHAIN} SHOW_PROGRESS)
	endif()

	include("${FILE_PKG_TOOLCHAIN}")

	# TODO: Move this execution to the dependency solver
#	set(FILE_PKG_TOOLCHAIN_VARS "${TOOLCHAIN_DIR}/${pkg_name}-${pkg_version}-${PLATFORM}.vars.cmake")
#	if(EXISTS ${FILE_PKG_TOOLCHAIN_VARS})
#		include("${FILE_PKG_TOOLCHAIN_VARS}")
#	endif()
	# TODO: Use ${${NAME}_INCLUDE_DIRS} and ${${NAME}_LIBRARIES}

	list(APPEND PACKAGE_LIST ${PKG_DISPLAY})
endforeach(p)


# Dependency Solver


# Export vars for parent list to execute
file(WRITE ${EXTERNAL_DIR}/vars.cmake "\
set(INCLUDE \"${INCLUDE}\")\n\
set(LIB \"${LIB}\")\n\
")

# Ideal Pipeline:
#
# Pre Pass
# Solve master package list
# Create aliases for simple names to working names
#
# First Pass
# Pkg -> Append to list for dep, on dep finished, put append list in 2nd queue
# If no deps, append to 2nd queue
# Second Pass
# Append include/deps
#
#
# Version System:
#   "luajit": [{ minver, maxver }, { minver, maxver }, { minver, maxver }]
#	Calc common: Find latest version using ranges from N sources
#      Use an AND for each range


# TODO: Handle dependencies

# Each package has a local stable history
# Each repository has a stable history with current package versions
# We need to download a list of the current history for all the packages we care about
# We also need to give an error if a child requires a package that the master requires that is more up to date than the on the master refers to if the child's version of the package has a different toolchain than the master's version [NEEDS MORE THOUGHT]
# After building this list, cache it so we can use it again, unless the list of packages changes

# For each package toolchain, add a custom target to execute it in the external directory
# Accumulate and sort any outputted libraries
# Accumulate includes


# If the state of the packages has changed, output parsable information for CI to conduct a test


# Testing:
# Try latest release, if fails, attempt to set up from last stable release until a build fails, then use the last stable
# Only do this for one package at a time
# Try to prioritize packages that are depended on by others
