cmake_minimum_required(VERSION 3.17.1)

project(loupe CXX)
include(cmake/StandardProjectSettings.cmake)
include(cmake/Filters.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
	if (ENABLE_BUILD_WITH_TIME_TRACE)
		add_compile_definitions(project_options INTERFACE -ftime-trace)
	endif()
endif()

# Standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_options)

# Sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

check_master_project(LOUPE_IS_MASTER)
option(ENABLE_TESTING "Enable Test Builds" ${LOUPE_IS_MASTER})
option(ENABLE_SAMPLES "Enable Sample Builds" ${LOUPE_IS_MASTER})

option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if(ENABLE_PCH)
	# This sets a global PCH parameter, each project will build its own PCH,
	# which is a good idea if any #define's change
	target_precompile_headers(project_options INTERFACE <type_traits>)
endif()

if(ENABLE_TESTING)
	enable_testing()
	message("Building Test Project")
	add_subdirectory(tests)
endif()

if(ENABLE_SAMPLES)
	message("Building Sample Project")
	add_subdirectory(samples)
endif()

add_subdirectory(loupe)
