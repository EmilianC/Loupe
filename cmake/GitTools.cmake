find_package(Git)
if(NOT GIT_FOUND)
	message(FATAL_ERROR "Git executable not found.")
endif()

function(git_clone_repo)
	set(oneValueArgs URL TAG DIRECTORY)
	cmake_parse_arguments(ARGS "" "${oneValueArgs}" "" ${ARGN})

	if(NOT ARGS_URL OR "URL" IN_LIST ARGS_KEYWORDS_MISSING_VALUES)
		message("A URL must be specified")
		return()
	endif()

	if(NOT ARGS_DIRECTORY OR "DIRECTORY" IN_LIST ARGS_KEYWORDS_MISSING_VALUES)
		message("A directory must be specified")
		return()
	endif()

	if(NOT ARGS_TAG OR "TAG" IN_LIST ARGS_KEYWORDS_MISSING_VALUES)
		set(ARGS_TAG "origin/master")
	endif()

	if(EXISTS "${ARGS_DIRECTORY}/.git/")
		return()
	endif()

	message("Cloning ${ARGS_URL}@${ARGS_TAG} into: \"${ARGS_DIRECTORY}\"...")

	file(MAKE_DIRECTORY "${ARGS_DIRECTORY}")

	# Create the repo
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" init
		WORKING_DIRECTORY "${ARGS_DIRECTORY}"
		RESULT_VARIABLE error_code)

	if(error_code)
		return()
	endif()

	# Link to the remote
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" remote add origin "${ARGS_URL}"
		WORKING_DIRECTORY "${ARGS_DIRECTORY}"
		RESULT_VARIABLE error_code)

	if(error_code)
		return()
	endif()

	# Fetch tag heads
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" fetch --tags --depth=1
		WORKING_DIRECTORY "${ARGS_DIRECTORY}"
		RESULT_VARIABLE error_code)

	if(error_code)
		return()
	endif()

	# Reset to target tag
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" reset --hard "${ARGS_TAG}"
		WORKING_DIRECTORY "${ARGS_DIRECTORY}"
		RESULT_VARIABLE error_code)

endfunction()
