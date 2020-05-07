# Check to make sure that we are not building in source
if (${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
	message("In source build are not supported. use a build/ subdirectory:")
	message("    > cmake . <OPTIONS> -Bbuild")
	message(FATAL_ERROR "Stopping build.")
endif()

# Hide Cmake projects under 'CMakePredefinedTargets'
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Omit ZERO_CHECK project
set(CMAKE_SUPPRESS_REGENERATION true)
# Disable MinSizeRel and RelWithDebInfo
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

	# Set the possible values of build type for cmake-gui, ccmake
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

find_program(CCACHE ccache)
if(CCACHE)
	message("using ccache")
	set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_LTO "Enable Link Time Optimization" ON)
if(ENABLE_LTO)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT result OUTPUT output)
	if(result)
		set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
	else()
		message(SEND_ERROR "LTO is not supported: ${output}")
	endif()
endif()

function(check_master_project is_project)
	if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
		set(${is_project} ON PARENT_SCOPE)
	else()
		set(${is_project} OFF PARENT_SCOPE)
	endif()
endfunction()
