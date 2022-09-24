list(APPEND loupe_files
	"loupe.cpp"
	"loupe.h"
	"loupe.inl"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${loupe_files})

add_library(loupe STATIC ${loupe_files})
target_compile_features(loupe PUBLIC cxx_std_20)
target_include_directories(loupe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
loupe_set_warnings(loupe)

option(LOUPE_PCH "Use a precompiled header file." OFF)
if (LOUPE_PCH)
	target_precompile_headers(loupe
		PRIVATE
			<string_view>
			<type_traits>
			<unordered_map>
			<vector>
	)
endif()
