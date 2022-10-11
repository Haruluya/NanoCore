#pragma once
//
#include <memory>
//
#include "PlatformDetection.h"

#ifdef RA_DEBUG
#if defined(RA_PLATFORM_WINDOWS)
#define RA_DEBUGBREAK() __debugbreak()
#elif defined(RA_PLATFORM_LINUX)
#include <signal.h>
#define RA_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define RA_ENABLE_ASSERTS
#else
#define RA_DEBUGBREAK()
#endif

#define RA_EXPAND_MACRO(x) x
#define RA_STRINGIFY_MACRO(x) #x
//
#define BIT(x) (1 << x)
