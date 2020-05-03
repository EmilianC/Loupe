# Source: https://github.com/EdenEast/scaffold/blob/master/cmake/filters.cmake
function(target_source_group target)
	set(single_args "DIRECTORY")
	set(multi_args "SOURCE_LIST")
	cmake_parse_arguments(THIS "" "${single_args}" "${multi_args}" ${ARGN})

	# Checking to see what type of target it is. It is an INTERFACE
	# target then it will need a differnet source property
	# https://stackoverflow.com/a/44064705
	get_target_property(target_type ${target} TYPE)

	# If it is a interface target then define IS_INTERFACE
	# and this can be used for the rest to check if it is or not
	if(${target_type} STREQUAL "INTERFACE_LIBRARY")
		set(IS_INTERFACE ON)
	endif()

	# Check to see if the DIRECTORY argument was passed. If so then use that
	# as the directory and dont check the target for it.
	if(THIS_DIRECTORY)
		set(target_source_directory ${THIS_DIRECTORY})
	else()
	if(IS_INTERFACE)
		get_target_property(target_source_directory ${target} INTERFACE_SOURCE_DIR)
			if(${target_source_directory} MATCHES ".*-NOTFOUND")
				message("Error! Cannot find source directory from an INTERFACE library.")
				message("Make sure that you have set the property INTERFACE_SOURCE_DIR")
				message("in target: ${target}. You can use DIRECTORY instead.")
				message(FATAL_ERROR "Cannot find DIRECTORY in target: ${target}")
			endif()
		else()
			get_target_property(target_source_directory ${target} SOURCE_DIR)
		endif()
	endif()

	# Getting the list of sources from the target. There must be a list
	# of sources for the funciton to apply filters to them
	if(IS_INTERFACE)
		get_target_property(target_source_list ${target} INTERFACE_SOURCES)
	else()
		get_target_property(target_source_list ${target} SOURCES)
	endif()

	set(last_dir "")
	foreach(source ${target_source_list})
		get_filename_component(source_directory ${source} DIRECTORY)

		file(RELATIVE_PATH dir "${target_source_directory}" "${source_directory}")
		string(REPLACE "/"  "\\" dir "${dir}")

		if(NOT "${dir}" STREQUAL "${last_dir}")
			if(files)
				source_group("${last_dir}" FILES ${files})
			endif()
			set(files "")
		endif()

		set(files ${files} ${source})
		set(last_dir ${dir})
	endforeach()

	if(files)
		source_group("${last_dir}" FILES ${files})
	endif()
endfunction()
