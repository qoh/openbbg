# Locals
set(TARGET "OpenBBG")
set(fileSources "${CMAKE_CURRENT_LIST_DIR}/sources.txt")
set(fileTarget "${CMAKE_CURRENT_LIST_DIR}/target.cmake")
set(pathBase "${CMAKE_CURRENT_SOURCE_DIR}")

# Vulkan
if(WITH_VULKAN)
	find_package(Vulkan)
	if(Vulkan_FOUND)
		list(APPEND INCLUDE ${Vulkan_INCLUDE_DIRS})
		list(APPEND LIB ${Vulkan_LIBRARIES})
	endif()
endif()

# Config
set(OPENBBG_WITH_VULKAN ${WITH_VULKAN})
configure_file("${pathBase}/src/OpenBBG/InternalConfig.h.in" "${pathBase}/src/OpenBBG/InternalConfig.h")

_ParseTargetFileTreeIfChanged(${TARGET} ${fileSources} ${fileTarget} ${pathBase} 1)

if(UNIX)
	list(APPEND LIB X11 Xxf86vm Xrandr pthread Xi m dl Xinerama Xcursor)
endif()
if(MSVC)
	list(APPEND LIB Shcore ws2_32)
endif()

# Define target
add_executable(${TARGET} ${TARGET_${TARGET}_ABSOLUTE_SOURCES})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
set_target_properties(${TARGET} PROPERTIES DEBUG_POSTFIX -d)
target_include_directories(${TARGET} PUBLIC ${pathBase}/src ${INCLUDE})

target_link_libraries(${TARGET} ${LIB})
target_compile_features(${TARGET} PRIVATE cxx_range_for)
if(MSVC)
	set_source_files_properties(${pathBase}/src/stdafx.cpp PROPERTIES COMPILE_FLAGS /Yc"stdafx.h")
	target_compile_options(${TARGET} PUBLIC /Od /openmp /Yu"stdafx.h")
endif()
