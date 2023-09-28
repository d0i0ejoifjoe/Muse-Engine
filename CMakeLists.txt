cmake_minimum_required(VERSION 3.19)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(muse VERSION 0.0.1 LANGUAGES C CXX)

include(FetchContent)

add_library(${PROJECT_NAME} STATIC)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_raw_string_literals)

option(MUSE_DEBUG "Whether build muse in debug" ON)
option(MUSE_BUILD_SAMPLES "Whether build sample projects" ON)

set(ASSIMP_BUILD_TESTS FALSE CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL FALSE CACHE BOOL "" FORCE)

set(MUSE_PLATFORM "")
set(MUSE_COMPILER "")

set(MUSE_WORKSPACE "${CMAKE_CURRENT_LIST_DIR}")
set(MUSE_SOURCE_PATH "${MUSE_WORKSPACE}/src")

set(MUSE_LIBRARIES "")
set(MUSE_INCLUDE_DIRS "")
set(MUSE_ARGS "")
set(MUSE_DEFS "")

macro(setup_msvc_runtime_library target)
    if(MUSE_DEBUG)
        set_target_properties(${target} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        set_target_properties(${target} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
endmacro()

macro(fetch_content_macro name repo tag)
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${repo}
        GIT_TAG ${tag}
    )

    FetchContent_GetProperties(${name})
    FetchContent_MakeAvailable(${name})
endmacro()

macro(parse_dir)
    file(GLOB_RECURSE files "./*.cpp" "./*.c" "./*.h")

    target_sources(muse PRIVATE ${files})
endmacro()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(MUSE_PLATFORM "WIN32")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(MUSE_PLATFORM "LINUX")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MUSE_PLATFORM "MACOS")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(MUSE_COMPILER "GCC")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(MUSE_COMPILER "CLANG")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "AppleClang")
    set(MUSE_COMPILER "CLANG")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    set(MUSE_COMPILER "MSVC")
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

list(APPEND MUSE_INCLUDE_DIRS "${MUSE_SOURCE_PATH}")

fetch_content_macro(sdl https://github.com/libsdl-org/SDL.git release-2.28.2)

list(APPEND MUSE_INCLUDE_DIRS "${sdl_SOURCE_DIR}/include")
list(APPEND MUSE_LIBRARIES SDL2)

fetch_content_macro(fmt https://github.com/fmtlib/fmt.git 10.1.1)

list(APPEND MUSE_INCLUDE_DIRS "${fmt_SOURCE_DIR}/include")
list(APPEND MUSE_LIBRARIES fmt)

fetch_content_macro(glm https://github.com/g-truc/glm 0.9.9.8)

list(APPEND MUSE_INCLUDE_DIRS "${glm_SOURCE_DIR}")

fetch_content_macro(assimp https://github.com/assimp/assimp.git v5.3.1)

list(APPEND MUSE_INCLUDE_DIRS "${assimp_SOURCE_DIR}/include")
list(APPEND MUSE_LIBRARIES assimp)

if(${MUSE_COMPILER} MATCHES "MSVC")
    list(APPEND MUSE_ARGS "/WX" "/Wall" "/O2")

    setup_msvc_runtime_library(${PROJECT_NAME})
    setup_msvc_runtime_library(SDL2)
    setup_msvc_runtime_library(FMT)
else()
    list(APPEND MUSE_ARGS "-static" "-O3" "-Wall" "-Werror")
endif()

if(NOT MUSE_DEBUG)
    list(APPEND MUSE_DEFS "NDEBUG=1")
endif()

list(APPEND MUSE_DEFS "MUSE_PLATFORM_${MUSE_PLATFORM}=1")
list(APPEND MUSE_DEFS "MUSE_COMPILER_${MUSE_COMPILER}=1")

message(STATUS "Defines: ${MUSE_DEFS}")
message(STATUS "Compile args: ${MUSE_ARGS}")
message(STATUS "Libraries: ${MUSE_LIBRARIES}")
message(STATUS "Include directories: ${MUSE_INCLUDE_DIRS}")

target_compile_definitions(${PROJECT_NAME} PUBLIC "${MUSE_DEFS}")
target_compile_options(${PROJECT_NAME} PUBLIC "${MUSE_ARGS}")
target_link_libraries(${PROJECT_NAME} PUBLIC "${MUSE_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PUBLIC "${MUSE_INCLUDE_DIRS}")

add_subdirectory("src")

if(MUSE_BUILD_SAMPLES)
    add_subdirectory("samples")
endif()