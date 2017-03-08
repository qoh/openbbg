# Locals
set(TARGET "OpenBBG")
set(fileSources "${CMAKE_CURRENT_LIST_DIR}/sources.txt")
set(fileTarget "${CMAKE_CURRENT_LIST_DIR}/target.cmake")
set(pathBase "${CMAKE_CURRENT_SOURCE_DIR}")

_ParseTargetFileTreeIfChanged(${TARGET} ${fileSources} ${fileTarget} ${pathBase})

if(UNIX)
	list(APPEND LIB pthread)
endif()
if(MSVC)
	list(APPEND LIB ws2_32)
endif()

# Define target
add_executable(${TARGET} ${TARGET_${TARGET}_ABSOLUTE_SOURCES})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES DEBUG_POSTFIX -d)
target_include_directories(${TARGET} PUBLIC ${INCLUDE})
target_link_libraries(${TARGET} ${LIB})
target_compile_features(${TARGET} PRIVATE cxx_range_for)
