list(APPEND loupe_test_files
	"arrays.cpp"
	"blob.cpp"
	"enums.cpp"
	"fundamentals.cpp"
	"lifetimes.cpp"
	"main.cpp"
	"maps.cpp"
	"pointers.cpp"
	"serialization.cpp"
	"structures.cpp"
	"test_data.cpp"
	"test_data.h"
	"variants.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${loupe_test_files})

add_executable(unit_tests WIN32 ${loupe_test_files})
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(unit_tests
	PUBLIC
		catch
	PRIVATE
		loupe
)
target_compile_options(unit_tests
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/MP>
)
target_compile_definitions(unit_tests
	PRIVATE
		CATCH_CONFIG_FAST_COMPILE
		CATCH_CONFIG_DISABLE_EXCEPTIONS
		_CRT_SECURE_NO_WARNINGS
)
loupe_set_warnings(unit_tests)
loupe_disable_rtti(unit_tests OPTIONAL)
loupe_disable_exceptions(unit_tests OPTIONAL)

# Disable warning: local variable is initialized but not referenced.
target_compile_options(unit_tests PRIVATE /wd4189)
