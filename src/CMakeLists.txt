add_compile_definitions(_POSIX_C_SOURCE=200809L _XOPEN_SOURCE=700)

if(APPLE)
    add_definitions(-D_DARWIN_C_SOURCE)
endif()

find_program(LINT "clang-tidy")
IF (LINT)
    set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=*,-llvmlibc-restrict-system-libc-headers,-cppcoreguidelines-init-variables,-clang-analyzer-security.insecureAPI.strcpy,-concurrency-mt-unsafe,-android-cloexec-accept,-android-cloexec-dup,-google-readability-todo,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cert-dcl03-c,-hicpp-static-assert,-misc-static-assert,-altera-struct-pack-align,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling;--quiet")
ENDIF ()

function(find_libxcrypt)
    if (TARGET COMP8005_assignment2)
        return()
    endif ()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBXCRYPT IMPORTED_TARGET libxcrypt)

    if (TARGET PkgConfig::LIBXCRYPT)
        add_library(COMP8005_assignment2 INTERFACE)
        target_link_libraries(COMP8005_assignment2 INTERFACE PkgConfig::LIBXCRYPT)
        target_compile_definitions(COMP8005_assignment2 INTERFACE HAVE_CRYPT_H)
    endif ()
endfunction()

function(find_sys_crypt)
    if (TARGET COMP8005_assignment2)
        return()
    endif ()

    include(CheckIncludeFile)
    include(CheckSymbolExists)

    set(defs "")
    set(CMAKE_REQUIRED_INCLUDES "")
    set(CMAKE_REQUIRED_LIBRARIES "")

    check_include_file("crypt.h" HAVE_CRYPT_H)
    if (HAVE_CRYPT_H)
        list(APPEND defs HAVE_CRYPT_H)
        list(APPEND CMAKE_REQUIRED_INCLUDES "crypt.h")
    endif ()

    check_symbol_exists(crypt "unistd.h" HAVE_CRYPT)
    if (NOT HAVE_CRYPT)
        list(APPEND CMAKE_REQUIRED_LIBRARIES crypt)
        check_symbol_exists(crypt "unistd.h" HAVE_CRYPT_WITH_LIB)
    endif ()

    if (HAVE_CRYPT OR HAVE_CRYPT_WITH_LIB)
        add_library(COMP8005_assignment2 INTERFACE)
        target_link_libraries(COMP8005_assignment2 INTERFACE ${CMAKE_REQUIRED_LIBRARIES})
        target_compile_definitions(COMP8005_assignment2 INTERFACE ${defs})
    endif ()
endfunction()

option(PREFER_LIBXCRYPT "Prefer libxcrypt for crypt()" OFF)
if (PREFER_LIBXCRYPT)
    find_libxcrypt()
    find_sys_crypt()
else ()
    find_sys_crypt()
    find_libxcrypt()
endif ()

if (NOT TARGET COMP8005_assignment2)
    message(FATAL_ERROR "Could not find a suitable crypt() implementation")
endif ()

find_package(OpenMP REQUIRED)

# Make an executable
add_executable(MAIN ${COMMON_SOURCE_LIST}  ${MAIN_SOURCE_LIST} ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(MAIN PRIVATE ../include)
target_include_directories(MAIN PRIVATE /usr/include)
target_include_directories(MAIN PRIVATE /usr/local/include)
target_link_directories(MAIN PRIVATE /usr/lib)
target_link_directories(MAIN PRIVATE /usr/local/lib)

# All users of this library will need at least C17
target_compile_features(MAIN PUBLIC c_std_17)
target_compile_options(MAIN PRIVATE -g)
target_compile_options(MAIN PRIVATE -fstack-protector-all -ftrapv)
target_compile_options(MAIN PRIVATE -Wpedantic -Wall -Wextra)
target_compile_options(MAIN PRIVATE -Wdouble-promotion -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wnull-dereference -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused-local-typedefs -Wstrict-overflow=5 -Wmissing-noreturn -Walloca -Wfloat-equal -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wabsolute-value -Wundef -Wexpansion-to-defined -Wunused-macros -Wno-endif-labels -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wconversion -Wdangling-else -Wdate-time -Wempty-body -Wsign-conversion -Wfloat-conversion -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Winvalid-pch -Wlong-long -Wvariadic-macros -Wdisabled-optimization -Wstack-protector -Woverlength-strings)

find_library(LIBM m REQUIRED)
find_library(LIBSOCKET socket)

target_link_libraries(MAIN PRIVATE ${LIBM} OpenMP::OpenMP_C)
target_link_libraries(MAIN PRIVATE COMP8005_assignment2)



set_target_properties(MAIN PROPERTIES OUTPUT_NAME "main")

install(TARGETS MAIN DESTINATION bin)


# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)

add_custom_target(
        format
        COMMAND clang-format
        -i
        ${HEADER_LIST}
        ${COMMON_SOURCE_LIST}
        ${MAIN_SOURCE_LIST}
)