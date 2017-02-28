# Locals
set(TARGET "OpenBBG")
set(fileSources "${CMAKE_CURRENT_LIST_DIR}/sources.txt")
set(fileTarget "${CMAKE_CURRENT_LIST_DIR}/target.cmake")
set(pathBase "${CMAKE_CURRENT_SOURCE_DIR}")

_ParseTargetFileTreeIfChanged(${TARGET} ${fileSources} ${fileTarget} ${pathBase})

# Define target
add_executable(${TARGET} ${TARGET_${TARGET}_ABSOLUTE_SOURCES})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUT_DIR})
set_target_properties(${TARGET} PROPERTIES DEBUG_POSTFIX -d)
#target_include_directories(${TARGET} PUBLIC ${INCLUDE})
#target_link_libraries(${TARGET} ${LIB})
#add_dependencies(${TARGET} external)
