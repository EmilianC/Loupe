list(APPEND loupe_files
	"${CMAKE_CURRENT_LIST_DIR}/loupe.h")

add_library(LOUPE_LIB INTERFACE)

target_link_libraries(LOUPE_LIB INTERFACE project_options)
target_sources(LOUPE_LIB INTERFACE ${loupe_files})
target_include_directories(LOUPE_LIB INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

# For IDE only
add_custom_target(loupe SOURCES ${loupe_files})
target_source_group(loupe)

#target_source_group(loupe DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
#target_include_directories(loupe "${CMAKE_CURRENT_LIST_DIR}")
#set_target_properties(loupe PROPERTIES LINKER_LANGUAGE CXX)
