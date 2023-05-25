list(APPEND loupe_files
	"assert.h"
	"core_data.cpp"
	"core_data.h"
	"core_data.inl"
	"loupe.cpp"
	"loupe.h"
	"loupe.inl"
	"private_members.h"
	"property_adapters.h"
)

if (LOUPE_ARCHIVER_CEREAL)
	list(APPEND loupe_files "archiver.cpp" "archiver.h")
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${loupe_files})

add_library(loupe STATIC ${loupe_files})
target_compile_features(loupe PUBLIC cxx_std_23)
target_include_directories(loupe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_compile_definitions(loupe PUBLIC $<$<BOOL:${LOUPE_ENABLE_ASSERTS}>:LOUPE_ASSERTS>)
target_link_libraries(loupe PUBLIC $<$<BOOL:${LOUPE_ARCHIVER_CEREAL}>:cereal>)
loupe_set_warnings(loupe)
loupe_disable_rtti(loupe OPTIONAL)
loupe_disable_exceptions(loupe OPTIONAL)
