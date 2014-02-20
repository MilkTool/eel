include(CheckIncludeFiles)
include(CheckFunctionExists)

include_directories(${EEL_SOURCE_DIR})
include_directories(${EEL_SOURCE_DIR}/include)
include_directories(${EEL_SOURCE_DIR}/src/core)
include_directories(${EEL_SOURCE_DIR}/src/core/eelc)
include_directories(${EEL_SOURCE_DIR}/src/core/dir)
include_directories(${EEL_SOURCE_DIR}/src/core/dsp)
include_directories(${EEL_SOURCE_DIR}/src/core/io)
include_directories(${EEL_SOURCE_DIR}/src/core/math)
include_directories(${EEL_SOURCE_DIR}/src/core/system)

set(EEL_DIRSEP	/)

if(WIN32)
	set(EEL_ARCH "WIN32")
	set(EEL_SOEXT ".DLL")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(EEL_ARCH "Linux")
	set(EEL_SOEXT ".so")
else(WIN32)
	set(EEL_ARCH "Unknown")
	set(EEL_SOEXT ".so")
endif(WIN32)

set(CMAKE_EXTRA_INCLUDE_FILES setjmp.h)
check_function_exists(setjmp		HAVE_SETJMP)
check_function_exists(_setjmp		HAVE__SETJMP)
check_function_exists(longjmp		HAVE_LONGJMP)
check_function_exists(_longjmp		HAVE__LONGJMP)
check_function_exists(sigsetjmp		HAVE_SIGSETJMP)
check_function_exists(siglongjmp	HAVE_SIGLONGJMP)

set(CMAKE_EXTRA_INCLUDE_FILES stdio.h)
check_function_exists(snprintf		HAVE_SNPRINTF)
check_function_exists(_snprintf		HAVE__SNPRINTF)

set(CMAKE_EXTRA_INCLUDE_FILES)


#TODO:
#builtin.c:      builtin.eel text2c.sed
#                sed -f text2c.sed builtin.eel > builtin.c


# Core files
set(sources
	e_array.c
	e_cast.c
	e_state.c
	e_table.c
	e_vector.c
	e_dstring.c
	e_function.c
	e_object.c
	e_builtin.c
	e_class.c
	e_register.c
	e_string.c
	e_util.c
	e_vm.c
	e_error.c
	e_module.c
	e_operate.c
)

# Compiler files
set(sources ${sources}
	eelc/ec_bio.c
	eelc/ec_coder.c
	eelc/ec_context.c
	eelc/ec_event.c
	eelc/ec_lexer.c
	eelc/ec_manip.c
	eelc/ec_mlist.c
	eelc/ec_parser.c
	eelc/ec_symtab.c
)

# dir module
set(sources ${sources}
	dir/eel_dir.c
)

# dsp module
set(sources ${sources}
	dsp/eel_dsp.c
	dsp/kfc.c
	dsp/kiss_fft.c
	dsp/kiss_fftr.c
)

# io module
set(sources ${sources}
	io/eel_io.c
)

# math module
set(sources ${sources}
	math/eel_math.c
)

# system module
set(sources ${sources}
	system/eel_system.c
)


#add_library(libeel SHARED ${sources})
add_library(libeel STATIC ${sources})


if(UNIX)
	target_link_libraries(libeel m)
endif(UNIX)

# Until 1.0, we assume there's no ABI compatibility across releases!
set_target_properties(libeel PROPERTIES VERSION ${VERSION_STRING}
	SOVERSION ${VERSION_STRING})

set_target_properties(libeel PROPERTIES OUTPUT_NAME "eel")

configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake
	${CMAKE_SOURCE_DIR}/config.h)
configure_file(${CMAKE_SOURCE_DIR}/include/EEL_version.h.cmake
	${CMAKE_SOURCE_DIR}/include/EEL_version.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/include/EEL_platform.h.cmake
	${CMAKE_SOURCE_DIR}/include/EEL_platform.h @ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include e_config.h")

install(TARGETS libeel DESTINATION lib)