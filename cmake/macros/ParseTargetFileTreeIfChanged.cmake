macro(_ParseTargetFileTreeIfChanged TARGET fileSources fileTarget pathBase)

set(extra "${ARGN}")
if(extra EQUAL 1)
	set(createMissing 1)
else()
	set(createMissing 0)
endif()

file(MD5 ${fileSources} md5Sources)
file(MD5 ${fileTarget} md5Target)

# Check if files have changed
if(NOT TARGET_${TARGET}_SOURCES_MD5 STREQUAL md5Sources
		OR NOT TARGET_${TARGET}_TARGET_MD5 STREQUAL md5Target)
	# Cache new results
	set(TARGET_${TARGET}_SOURCES_MD5 ${md5Sources} CACHE INTERNAL "")
	set(TARGET_${TARGET}_TARGET_MD5 ${md5Target} CACHE INTERNAL "")

	file(READ ${fileSources} fileContent)
	string(REPLACE "\n" ";" fileContent ${fileContent})

	
	# Process lines
	foreach(line ${fileContent})

		# Get last character
		string(LENGTH ${line} lineLength)
		math(EXPR lastCharPos "${lineLength} - 1")
		string(SUBSTRING ${line} ${lastCharPos} 1 lastChar)

		# Get number of tabs
		string(REPLACE "\t" " ;" line ${line})
		list(LENGTH line tabCount)
		math(EXPR tabCount "${tabCount} - 1")

		list(GET line ${tabCount} value)

		if(lastChar STREQUAL "/")
			# If path, save to stack
			set(T___PATHSTACK_${tabCount} ${value})
		else()
			# If file, composite path
			set(T___PATH "")
			set(T ${tabCount})
			while(NOT T EQUAL 0)
				math(EXPR T "${T} - 1")
				string(CONCAT T___PATH ${T___PATHSTACK_${T}} ${T___PATH})
			endwhile()
			set(file "${T___PATH}${value}")
			if(createMissing)
				if(NOT EXISTS ${file})
					file(WRITE "${pathBase}/${file}" "")
				endif()
			endif()
			list(APPEND files "${file}")
		endif()
	endforeach()

	set(TARGET_${TARGET}_SOURCES ${files} CACHE INTERNAL "")
endif()

# Parse sources into source groups
foreach(file ${TARGET_${TARGET}_SOURCES})
	string(FIND "${file}" "/" lastSlash REVERSE)
	if(NOT lastSlash EQUAL 0)
		string(SUBSTRING "${file}" 1 ${lastSlash} relPath)
		string(REPLACE "/" "\\" relPath "${relPath}")
		source_group("${relPath}" FILES "${pathBase}/${file}")
	else()
		source_group("" FILES "${pathBase}/${file}")
	endif()
	list(APPEND TARGET_${TARGET}_ABSOLUTE_SOURCES ${pathBase}/${file})
endforeach()


endmacro()
