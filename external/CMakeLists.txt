# Append to any IDE folders already set be calling scripts.
set(PARENT_FOLDER ${CMAKE_FOLDER})

# Catch #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external/catch")
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE catch/)
target_compile_definitions(catch
	INTERFACE
		CATCH_CONFIG_FAST_COMPILE
		CATCH_CONFIG_DISABLE_EXCEPTIONS
)

# Cereal #
set(CMAKE_FOLDER "${PARENT_FOLDER}/external")
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "")
set(JUST_INSTALL_CEREAL         ON CACHE BOOL "")
add_subdirectory(cereal)
