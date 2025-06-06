# Toggle LTO
option(LOUPE_ENABLE_LTO "Enable Link Time Optimization" ON)
if (LOUPE_ENABLE_LTO)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT result OUTPUT output)
	if(result)
		set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
	else()
		message(SEND_ERROR "LTO is not supported: ${output}")
	endif()
endif()


# Check if the project is being included by another
function(loupe_is_top_level project)
	if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		set(${project} ON PARENT_SCOPE)
	else()
		set(${project} OFF PARENT_SCOPE)
	endif()
endfunction()


# Set up standard C++ warnings
function(loupe_set_warnings target_name)
	set(MSVC_WARNINGS
		/W4          # Baseline reasonable warnings
		/w14242      # 'identifier': conversion from 'type1' to 'type1', possible loss of data
		/w14254      # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
		/w14263      # 'function': member function does not override any base class virtual member function
		/w14265      # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not be destructed correctly
		/w14287      # 'operator': unsigned/negative constant mismatch
		/we4289      # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
		/w14296      # 'operator': expression is always 'boolean_value'
		/w14311      # 'variable': pointer truncation from 'type1' to 'type2'
		/w14545      # expression before comma evaluates to a function which is missing an argument list
		/w14546      # function call before comma missing argument list
		/w14547      # 'operator': operator before comma has no effect; expected operator with side-effect
		/w14549      # 'operator': operator before comma has no effect; did you intend 'operator'?
		/w14555      # expression has no effect; expected expression with side- effect
		/w14619      # pragma warning: there is no warning number 'number'
		/w14640      # Enable warning on thread un-safe static member initialization
		/w14826      # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
		/w14905      # wide string literal cast to 'LPSTR'
		/w14906      # string literal cast to 'LPWSTR'
		/w14928      # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
		/permissive- # standards conformance mode for MSVC compiler.
	)

	set(CLANG_WARNINGS
		-Wall
		-Wextra              # reasonable and standard
		-Wshadow             # warn the user if a variable declaration shadows one from a parent context
		-Wnon-virtual-dtor   # warn the user if a class with virtual functions has a non-virtual destructor.
		-Wold-style-cast     # warn for c-style casts
		-Wcast-align         # warn for potential performance problem casts
		-Wunused             # warn on anything being unused
		-Woverloaded-virtual # warn if you overload (not override) a virtual function
		-Wpedantic           # warn if non-standard C++ is used
		-Wconversion         # warn on type conversions that may lose data
		-Wsign-conversion    # warn on sign conversions
		-Wnull-dereference   # warn if a null dereference is detected
		-Wdouble-promotion   # warn if float is implicit promoted to double
		-Wformat=2           # warn on security issues around functions that format output (ie printf)
	)

	set(GCC_WARNINGS
		${CLANG_WARNINGS}
		-Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
		-Wduplicated-cond        # warn if if / else chain has duplicated conditions
		-Wduplicated-branches    # warn if if / else branches have duplicated code
		-Wlogical-op             # warn about logical operations being used where bitwise were probably wanted
		-Wuseless-cast           # warn if you perform a cast to the same type
	)

	set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
	set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)

	if(MSVC)
		set(PROJECT_WARNINGS ${MSVC_WARNINGS})
	elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
		set(PROJECT_WARNINGS ${CLANG_WARNINGS})
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(PROJECT_WARNINGS ${GCC_WARNINGS})
	else()
		message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
	endif()

	target_compile_options(${target_name} PRIVATE ${PROJECT_WARNINGS})
endfunction()


# Disables Run-Time Type Information (RTTI) for the target. Accepts the [OPTIONAL] flag.
# Changes made to the target are PRIVATE.
function(loupe_disable_rtti target_name)
  cmake_parse_arguments(THIS "OPTIONAL" "" "" ${ARGN})

  if (THIS_OPTIONAL)
    option(LOUPE_DISABLE_RTTI "Disables Run-Time Type Information generation." TRUE)
  endif()

  if (LOUPE_DISABLE_RTTI OR NOT THIS_OPTIONAL)
    if (MSVC)
      target_compile_options(${target_name} PRIVATE /GR-)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
      target_compile_options(${target_name} PRIVATE -fno-rtti)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(${target_name} PRIVATE -fno-rtti)
    else()
      message(AUTHOR_WARNING "RTTI could not be disabled for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif()
  endif()
endfunction()


# Disables exceptions for the target. Accepts the [OPTIONAL] flag.
# Changes made to the target are PRIVATE.
function(loupe_disable_exceptions target_name)
  cmake_parse_arguments(THIS "OPTIONAL" "" "" ${ARGN})

  if (THIS_OPTIONAL)
    option(LOUPE_DISABLE_EXCEPTIONS "Disables C++ exceptions." TRUE)
  endif()

  if (LOUPE_DISABLE_EXCEPTIONS OR NOT THIS_OPTIONAL)
    target_compile_definitions(${target_name} PRIVATE _HAS_EXCEPTIONS=0)

    if (MSVC)
      target_compile_options(${target_name} PRIVATE /EHs-c-a-)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
      target_compile_options(${target_name} PRIVATE -fno-exceptions)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(${target_name} PRIVATE -fno-exceptions)
    else()
      message(AUTHOR_WARNING "Exceptions could not be disabled for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif()
  endif()
endfunction()
