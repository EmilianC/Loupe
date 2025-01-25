cmake_minimum_required(VERSION 3.17.1)

project(loupe LANGUAGES CXX)

include(cmake/Utilities.cmake)
loupe_is_top_level(LOUPE_TOP_LEVEL)
if (${LOUPE_TOP_LEVEL})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
	# Hide Cmake projects under 'CMakePredefinedTargets'
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	# Check to make sure that we are not building in source
	if (${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
	message("In source build are not supported. use a build/ subdirectory:")
	message("    > cmake . <OPTIONS> -Bbuild")
	message(FATAL_ERROR "Stopping build.")
	endif()
endif()

option(LOUPE_TESTS "Build Loupe Test Executable" ${LOUPE_TOP_LEVEL})
option(LOUPE_ARCHIVER_CEREAL "Include an adapter archive for serialization with cereal." ON)
option(LOUPE_ENABLE_ASSERTS "Whether or not errors are signaled with a c-asserts (in debug builds)." ON)

add_subdirectory(external)
add_subdirectory(loupe)

if (LOUPE_TESTS)
	add_subdirectory(tests)
endif()
