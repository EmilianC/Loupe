list(APPEND loupe_files
	"archiver.cpp"
	"archiver.h"
	"core_data.cpp"
	"core_data.h"
	"core_data.inl"
	"loupe.cpp"
	"loupe.h"
	"loupe.inl"
	"type_adapters.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${loupe_files})

add_library(loupe STATIC ${loupe_files})
target_compile_features(loupe PUBLIC cxx_std_20)
target_include_directories(loupe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
loupe_set_warnings(loupe)

if (LOUPE_ARCHIVER_CEREAL)
	target_compile_definitions(loupe PUBLIC LOUPE_ARCHIVER_CEREAL)
endif()
