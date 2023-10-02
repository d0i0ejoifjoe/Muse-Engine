#pragma once

#if defined(MUSE_COMPILER_GCC)
#define PRAGMA(expr) _Pragma(#expr)
#define PUSH() PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING(expr) PRAGMA(GCC diagnostic ignored #expr)
#define POP() PRAGMA(GCC diagnostic pop)
#elif defined(MUSE_COMPILER_CLANG)
#define PRAGMA(expr) _Pragma(#expr)
#define PUSH() PRAGMA(clang diagnostic push)
#define DISABLE_WARNING(expr) PRAGMA(clang diagnostic ignored #expr)
#define POP() PRAGMA(clang diagnostic pop)
#elif defined(MUSE_COMPILER_MSVC)
#define PUSH() __pragma(push)
#define DISABLE_WARNING(expr) __pragma(warning(disable : expr))
#define POP() __pragma(pop)
#endif

#if !defined(MUSE_COMPILER_MSVC)
PUSH()
DISABLE_WARNING(-Wvolatile)
#endif

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>

#if !defined(MUSE_COMPILER_MSVC)
POP()
#endif