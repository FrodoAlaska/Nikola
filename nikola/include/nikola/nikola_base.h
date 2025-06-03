#pragma once

#include <cstddef>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola
 
/// ----------------------------------------------------------------------
/// *** Typedefs ***

/// char
typedef char  i8; 

/// short
typedef short i16;

/// int
typedef int   i32;

/// long
typedef long  i64;

/// unsigned char
typedef unsigned char  u8;

/// unsigned short
typedef unsigned short u16;   

/// unsigned int
typedef unsigned int   u32;   

/// unsigned long
typedef unsigned long  u64;   

/// size_t
typedef size_t         sizei;

/// float
typedef float  f32;   

/// double
typedef double f64;   

/// *** Typedefs ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** DEFS ***

/// Nikola only supports OpenGL versions greater than these.
#define NIKOLA_GL_MINIMUM_MAJOR_VERSION 4
#define NIKOLA_GL_MINIMUM_MINOR_VERSION 2

// Exports
#ifdef NIKOLA_EXPORT 

  // Window exports
  #ifdef _MSC_VER
    #define NIKOLA_API __declspec(dllexport) 
  // Linux exports
  #else
    #define NIKOLA_API __attribute__((visibility("default"))) 
  #endif

#endif

// Window imports
#ifdef _MSC_VER 
  #define NIKOLA_API __declspec(dllexport) 
// Linux exports
#else
  #define NIKOLA_API __attribute__((visibility("default"))) 
#endif

/// *** DEFS ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Macros ***

/// Returns `true` if `other` is set in `bit`
#define IS_BIT_SET(bit, other) ((bit & other) == other)

/// Sets/adds `other` into `bit`
#define SET_BIT(bit, other)    (bit |= other)

/// Unsets/removes `other` from `bit`
#define UNSET_BIT(bit, other)  (bit &= ~(other))

/// *** Macros ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Build Types ***

#if defined(DEBUG) || defined(_DEBUG)
  #define NIKOLA_BUILD_DEBUG   1 
  #define NIKOLA_BUILD_RELEASE 0
#else
  #define NIKOLA_BUILD_DEBUG   0 
  #define NIKOLA_BUILD_RELEASE 1
#endif

/// *** Build Types ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Platform detection ***

/// Windows
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define NIKOLA_PLATFORM_WINDOWS 1
#define NIKOLA_GFX_CONTEXT_OPENGL
#ifndef _WIN64 
#error "[NIKOLA-FATAL]: Only support 64-bit machines\n"
#endif 

/// Linux
#elif defined(__linux__) || defined(__gnu_linux__)
#define NIKOLA_PLATFORM_LINUX   1 
#define NIKOLA_GFX_CONTEXT_OPENGL
#endif

/// *** Platform detection ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Library init ***

///---------------------------------------------------------------------------------------------------------------------
/// Library functions

/// Initialze various different subsystems of Nikola. 
NIKOLA_API const bool init();

/// Shutdown subsystems of the Nikola.
NIKOLA_API void shutdown();

/// Library functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Library init ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Memory ***

///---------------------------------------------------------------------------------------------------------------------
/// Memory callbacks

/// A callback that allocates and returns a block of memory with size `size`. 
/// Used for custom allocation purposes.
using AllocateMemoryFn = void*(*)(const sizei size);

/// A callback that frees/reclaims a block of memory `ptr`.
/// Used for custom allocation purposes.
using FreeMemoryFn = void(*)(void* ptr);

/// Memory callbacks
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Memory functions 

/// Allocate a memory block of size `size`.
/// 
/// @NOTE: This function will assert if there's no suffient memory left.
NIKOLA_API void* memory_allocate(const sizei size);

/// Re-allocate a block of memory `ptr` with a new size of `new_size`.
/// 
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void* memory_reallocate(void* ptr, const sizei new_size);

/// Set the value of the memory block `ptr` with a size of `ptr_size` to `value`.
/// 
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void* memory_set(void* ptr, const i32 value, const sizei ptr_size);

/// Set the value of the memory block `ptr` of size `ptr_size` to 0.
/// 
/// @NOTE: This is equivalent to `memory_set(ptr, 0, ptr_size)`.
///
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void* memory_zero(void* ptr, const sizei ptr_size);

/// Allocate `count` blocks of memory each with the size of `block_size`.
/// 
/// @NOTE: This is equivalent to `memory_allocate(block_size * count)`.
NIKOLA_API void* memory_blocks_allocate(const sizei count, const sizei block_size);

/// Copy `src_size` bytes of `src` to the memory block `dest`. 
/// 
/// @NOTE: This function will assert if `dest` or `src` are a `nullptr`.
NIKOLA_API void* memory_copy(void* dest, const void* src, const sizei src_size);

/// Free/reclaim the memory of the given `ptr`.
/// 
/// @NOTE: This function will assert if `ptr` is a `nullptr`.
NIKOLA_API void memory_free(void* ptr);

/// Retrieve the amount of allocations made so far.
NIKOLA_API const sizei memory_get_allocations_count();

/// Retrieve the amount of frees made so far. 
NIKOLA_API const sizei memory_get_frees_count();

/// Retrieve how many bytes have been allocated so far. 
NIKOLA_API const sizei memory_get_allocation_bytes();

/// Memory functions 
///---------------------------------------------------------------------------------------------------------------------

/// *** Memory ***
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// *** Logger ***

#define NIKOLA_LOG_INFO_ACTIVE 1 
#define NIKOLA_LOG_WARN_ACTIVE 1 

// Only activate trace and debug logs on debug builds
#if NIKOLA_BUILD_RELEASE == 1
#define NIKOLA_LOG_TRACE_ACTIVE 0
#define NIKOLA_LOG_DEBUG_ACTIVE 0
#else 
#define NIKOLA_LOG_TRACE_ACTIVE 1
#define NIKOLA_LOG_DEBUG_ACTIVE 1
#endif

///---------------------------------------------------------------------------------------------------------------------
/// Log level
enum LogLevel {
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG, 
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR, 
  LOG_LEVEL_FATAL,
};
/// Log level
///---------------------------------------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------------------------------------
/// Logger functions

/// Log an assertion with the given information.
NIKOLA_API void logger_log_assert(const char* expr, const char* msg, const char* file, const u32 line_num);

/// Log a specific log level `lvl` with the given `msg` and any other parametars.
NIKOLA_API void logger_log(const LogLevel lvl, const char* msg, ...);

/// Logger functions
///---------------------------------------------------------------------------------------------------------------------

/// Trace log
#if NIKOLA_LOG_TRACE_ACTIVE == 1
#define NIKOLA_LOG_TRACE(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_TRACE(msg, ...)
#endif
/// Trace log

/// Debug log
#if NIKOLA_LOG_DEBUG_ACTIVE == 1
#define NIKOLA_LOG_DEBUG(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_DEBUG(msg, ...)
#endif
/// Debug log

/// Info log
#if NIKOLA_LOG_INFO_ACTIVE == 1
#define NIKOLA_LOG_INFO(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_INFO(msg, ...)
#endif
/// Info log

/// Warn log
#if NIKOLA_LOG_WARN_ACTIVE == 1
#define NIKOLA_LOG_WARN(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define NIKOLA_LOG_WARN(msg, ...)
#endif
/// Warn log

/// Error log
#define NIKOLA_LOG_ERROR(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
/// Error log

/// Fatal log
#define NIKOLA_LOG_FATAL(msg, ...) nikola::logger_log(nikola::LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
/// Fatal log

/// *** Logger ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Asserts ***

#define NIKOLA_ASSERTS_ENABLED // @TEMP (Base): Maybe would be a better idea to have a "Debug" assert and a "Release" assert instead of just one.

#ifdef NIKOLA_ASSERTS_ENABLED

/// Windows debug break
#if NIKOLA_PLATFORM_WINDOWS == 1
  #define DEBUG_BREAK() __debugbreak()
/// Windows debug break

/// Linux debug break
#elif NIKOLA_PLATFORM_LINUX == 1
  #define DEBUG_BREAK() __builtin_trap()
/// Linux debug break

#endif

/// Assert if `expr` is false, with the information in `msg`. 
  #define NIKOLA_ASSERT(expr, msg)                                      \
        {                                                               \
          if(expr) {                                                    \
          }                                                             \
          else {                                                        \
            nikola::logger_log_assert(#expr, msg, __FILE__, __LINE__);  \
            DEBUG_BREAK();                                              \
          }                                                             \
        }

#else 
  #define NIKOLA_ASSERT(expr, msg)
#endif

/// *** Asserts ***
/// ---------------------------------------------------------------------

/// ---------------------------------------------------------------------
/// *** Clock ***

///---------------------------------------------------------------------------------------------------------------------
/// Clock functions

/// @NOTE: Every function has an "ni" prefix to avoid any confusion with the C clock library.

/// Updates the values of the time, FPS (frames per second), and the delta time.
/// 
/// @NOTE: This must be called every frame.
NIKOLA_API void niclock_update();

/// Retrieve the time passed since the application was initialzed.
NIKOLA_API const f64 niclock_get_time(); 

/// Retrieve the current FPS (frames per second) of the application
NIKOLA_API const f64 niclock_get_fps();

/// Retrieve the time passed between each frame. 
NIKOLA_API const f64 niclock_get_delta_time();

/// Clock functions
///---------------------------------------------------------------------------------------------------------------------

/// *** Clock ***
/// ---------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
