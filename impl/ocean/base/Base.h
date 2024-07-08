/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_BASE_H
#define META_OCEAN_BASE_BASE_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

namespace Ocean
{

/**
 * @defgroup base Ocean Base Library
 * @{
 * The Ocean Base Library provides all base functionalities needed in the Ocean framework.
 * The library is platform independent.
 *
 * The DateTime, Timestamp and HighPerformanceTimer classes provide date and time functionalities.<br>
 *
 * The Thread, Lock, Signal, Thread, ThreadPool, Worker and WorkerPool classes provide threading, mutex, signal and critical section functionalities.<br>
 *
 * Callback and Caller realize callback functions and the Scheduler can be applied to implement regular timer events.<br>
 *
 * The StaticBuffer, StaticVector, ShiftVector, KdTree, HashSet, HasMap, RingMap and Triple classes provide possibilities to store and access data elements within individual structures.<br>
 *
 * The RandomI and RandomGenerator classes provide capabilities to create random numbers.<br>
 *
 * The ObjectRef and SmartObjectRef classes encapsulate individual objects by object references with reference counters.<br>
 *
 * Use the Frame class to store arbitrary 2D (image) information.<br>
 *
 * Singletons can be realized by application of the Singleton class.<br>
 *
 * Several helper classes exist providing template-based solutions to e.g., create data types with specific size or to create data types larger than a specific data type.
 * Some of this classes are: DataType, SquareValueTyper, DifferenceValueTyper, NextLargerTyper, UnsignedTyper, TypeNamer.
 * @}
 */

/**
 * @namespace Ocean The namespace covering the entire Ocean framework.<p>
 * Ocean is the base namespace for the Ocean framework.<br>
 * The Namespace Ocean is used in the Base and Math library directly.
 */

// Do not use windows min-max functionality
#ifndef NOMINMAX
	#define NOMINMAX
#endif

/**
 * Using min from std.
 */
using std::min;

/**
 * Using max from std.
 */
using std::max;

/**
 * Definition of a 32 bit index value.
 * @ingroup base
 */
typedef uint32_t Index32;

/**
 * Definition of a 64 bit index value.
 * @ingroup base
 */
typedef uint64_t Index64;

/**
 * Definition of a vector holding 32 bit index values.
 * @ingroup base
 */
typedef std::vector<Index32> Indices32;

/**
 * Definition of a vector holding 32 bit indices, so we have groups of indices.
 * @ingroup base
 */
typedef std::vector<Indices32> IndexGroups32;

/**
 * Definition of a vector holding 64 bit index values.
 * @ingroup base
 */
typedef std::vector<Index64> Indices64;

/**
 * Definition of a set holding 32 bit indices.
 * @ingroup base
 */
typedef std::set<Index32> IndexSet32;

/**
 * Definition of a set holding 64 bit indices.
 * @ingroup base
 */
typedef std::set<Index64> IndexSet64;

/**
 * Definition of an unordered_set holding 32 bit indices.
 * @ingroup base
 */
typedef std::unordered_set<Index32> UnorderedIndexSet32;

/**
 * Definition of an unordered_set holding 64 bit indices.
 * @ingroup base
 */
typedef std::unordered_set<Index64> UnorderedIndexSet64;

/**
 * Definition of a pair holding 32 bit indices.
 * @ingroup base
 */
typedef std::pair<Index32, Index32> IndexPair32;

/**
 * Definition of a vector holding 32 bit index pairs.
 * @ingroup base
 */
typedef std::vector<IndexPair32> IndexPairs32;

/**
 * Definition of a pair holding 64 bit indices.
 * @ingroup base
 */
typedef std::pair<Index64, Index64> IndexPair64;

/**
 * Definition of a vector holding 64 bit index pairs.
 * @ingroup base
 */
typedef std::vector<IndexPair64> IndexPairs64;

/**
 * Definition of a vector holding strings.
 * @ingroup base
 */
typedef std::vector<std::string> Strings;

/**
 * Definition of a vector holding strings.
 * @ingroup base
 */
typedef std::vector<std::wstring> WStrings;



// Define OCEAN_DEACTIVATED_MESSENGER to disable the messenger capabilities in the entire framework.
// Disabling the messenger in the release build also ensures that message strings will not be part of the release binaries.
// ENABLE_LOGGING should be enabled in local, rc, and master (but not in profile) builds
// PROFILE should be enabled in profile builds
// Thus, we use Ocean's messenger in local, profile, rc and master - but not in production
// OCEAN_ACTIVATE_MESSENGER can be defined to explicitly avoid deactivating Ocean's messenger
#if (!defined(ENABLE_LOGGING) || ENABLE_LOGGING != 1) && (!defined(OCEAN_ACTIVATE_MESSENGER) || OCEAN_ACTIVATE_MESSENGER != 1) && (!defined(PROFILE) || PROFILE != 1)
	#ifndef OCEAN_DEACTIVATED_MESSENGER
		#define OCEAN_DEACTIVATED_MESSENGER
	#endif
#else
  // #define OCEAN_DEACTIVATED_MESSENGER
#endif




// we ensure that OCEAN_DEBUG is also defined as long as DEBUG is defined or _DEBUG is defined
#if !defined(OCEAN_DEBUG) && (defined(DEBUG) || defined(_DEBUG))
	#define OCEAN_DEBUG
#endif




// Define OCEAN_INTENSIVE_DEBUG to activate code blocks intensively checking the correctness/accuracy of some specific function which is disabled by default.
#ifdef OCEAN_DEBUG
	#ifndef OCEAN_INTENSIVE_DEBUG
		//#define OCEAN_INTENSIVE_DEBUG
	#endif
#endif




// we ensure that _ANDROID is also defined as long as __ANDROID__ is defined
#if defined(__ANDROID__) && !defined(_ANDROID)
	#define _ANDROID
#endif




// Defines OCEAN_RUNTIME_STATIC for static runtimes
#ifndef OCEAN_RUNTIME_SHARED
	#ifndef OCEAN_RUNTIME_STATIC
		#define OCEAN_RUNTIME_STATIC
	#endif
#endif




// Defines OCEAN_HARDWARE_REDUCED_PERFORMANCE to allow the usage of specific code for hardware with reduced computational power.
#if defined(_ANDROID) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1)
	#ifndef OCEAN_HARDWARE_REDUCED_PERFORMANCE
		#define OCEAN_HARDWARE_REDUCED_PERFORMANCE
	#endif
#else
	//#define OCEAN_HARDWARE_REDUCED_PERFORMANCE
#endif





// Defines OCEAN_HARDWARE_NEON_VERSION for platforms supporting NEON instructions
#ifndef OCEAN_HARDWARE_NEON_VERSION
	#if defined(__ARM_NEON__) || defined(__ARM_NEON)
		#define OCEAN_HARDWARE_NEON_VERSION 10
	#endif
#endif

#ifndef OCEAN_HARDWARE_NEON_VERSION
	#define OCEAN_HARDWARE_NEON_VERSION 0
#endif




// Defines OCEAN_GCC_VERSION according to the given GCC version
#ifdef __GNUC__
	#define OCEAN_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#else
	#define OCEAN_GCC_VERSION 0
#endif

// Defines OCEAN_CLANG_VERSION according to the given Clang version
#ifdef __clang__
	#define OCEAN_CLANG_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100)
#else
	#define OCEAN_CLANG_VERSION 0
#endif

// Defines OCEAN_MSC_VERSION according to the given _MSC_VER version
// Beware: _MSC_VER may be defined although the Microsoft compiler is not applied e.g., when using clang for Windows platforms.
#ifdef _MSC_VER
	#define OCEAN_MSC_VERSION (_MSC_VER)
#else
	#define OCEAN_MSC_VERSION 0
#endif




/**
 * Ocean supports the following compilers:
 * GCC, Clang, Microsoft
 * As macros like `_MSC_VER` can be defined by the Clang compiler (when compiling for Windows platforms),
 * Ocean defines an own macro `OCEAN_COMPILER_***` to identify the individual compiler.
 * The macro is defined as follows:
 * <pre>
 * Active compiler macro:        __GNUC__:         __clang__:        _MSC_VER:
 * OCEAN_COMPILER_CLANG          do not care       defined           do not care
 * OCEAN_COMPILER_GCC            defined           undefined         undefined
 * OCEAN_COMPILER_MSC            undefined         undefined         defined
 * </pre>
 * @ingroup base
 */

// Defines OCEAN_COMPILER_CLANG if the clang compiler is used
#if defined(OCEAN_CLANG_VERSION) && OCEAN_CLANG_VERSION > 0
	/// \cond DOXYGEN_DO_NOT_DOCUMENT
	#define OCEAN_COMPILER_CLANG 1
	/// \endcond

	#if defined(OCEAN_COMPILER_GCC) || defined(OCEAN_COMPILER_MSC)
		#error Invalid compiler configuration.
	#endif
#endif

// Defines OCEAN_COMPILER_GCC if the gcc compiler is used
#if defined(OCEAN_GCC_VERSION) && OCEAN_GCC_VERSION > 0

	#if !defined(OCEAN_CLANG_VERSION) || OCEAN_CLANG_VERSION == 0
		/// \cond DOXYGEN_DO_NOT_DOCUMENT
		#define OCEAN_COMPILER_GCC 1
		/// \endcond

		#if defined(OCEAN_COMPILER_CLANG) || defined(OCEAN_COMPILER_MSC)
			#error Invalid compiler configuration.
		#endif
	#endif

	#if defined(OCEAN_MSC_VERSION) && OCEAN_MSC_VERSION > 0
		#error The GCC compiler is used already, either the usage of gcc or msc is wrong.
	#endif
#endif

// Defines OCEAN_COMPILER_MSC if the Microsoft compiler is used
#if defined(OCEAN_MSC_VERSION) && OCEAN_MSC_VERSION > 0

	#if !defined(OCEAN_CLANG_VERSION) || OCEAN_CLANG_VERSION == 0
		#if !defined(OCEAN_GCC_VERSION) || OCEAN_GCC_VERSION == 0
			/// \cond DOXYGEN_DO_NOT_DOCUMENT
			#define OCEAN_COMPILER_MSC 1
			/// \endcond

			#if defined(OCEAN_COMPILER_GCC) || defined(OCEAN_COMPILER_CLANG)
				#error Invalid compiler configuration.
			#endif
		#endif
	#endif
#endif

#if !defined(OCEAN_COMPILER_GCC) && !defined(OCEAN_COMPILER_CLANG) && !defined(OCEAN_COMPILER_MSC)
	// we ensure that one of the compilers is defined
	#error Unknown compiler, either GCC, Clang or the Microsoft compiler must be used.
#endif


#if !defined(OCEAN_POSIX_AVAILABLE)
	// Clang on Windows uses standard library from MSVC.
	#if defined(OCEAN_COMPILER_GCC) || (defined(OCEAN_COMPILER_CLANG) && !defined(_MSC_VER))
		/// \cond DOXYGEN_DO_NOT_DOCUMENT
		#define OCEAN_POSIX_AVAILABLE
		/// \endcond
	#endif
#endif


// Defines OCEAN_HARDWARE_SSE_VERSION for platforms supporting SSE instructions
#ifndef OCEAN_HARDWARE_SSE_VERSION
	#if defined(_MSC_VER) && !defined(__clang__)

		#if defined(_M_IX86_FP) && _M_IX86_FP >= 1
			#define OCEAN_HARDWARE_SSE_VERSION 41
		#endif

		// The 64 bit compiler provides SSE support by default
		#if defined(_WIN64)
			#define OCEAN_HARDWARE_SSE_VERSION 41
		#endif

	#elif defined(_MSC_VER) && defined(__clang__)

		#if defined(__SSE4_1__)
			#define OCEAN_HARDWARE_SSE_VERSION 41
		#endif

	#elif defined(__APPLE__)

		#if defined(__SSE4_1__)
			#define OCEAN_HARDWARE_SSE_VERSION 41
		#endif

	#elif defined(__linux__) && !defined(_ANDROID)

		#if defined(__SSE4_1__)
			#define OCEAN_HARDWARE_SSE_VERSION 41
		#endif

	#endif
#endif

#ifndef OCEAN_HARDWARE_SSE_VERSION
	#define OCEAN_HARDWARE_SSE_VERSION 0
#endif




// Defines OCEAN_HARDWARE_AVX_VERSION for platforms supporting AVX instructions
#if !defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_SSE_VERSION > 0
	#if defined(_MSC_VER) || defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

		#if defined(__AVX2__)
			#define OCEAN_HARDWARE_AVX_VERSION 20
		#elif defined(__AVX__)
			#define OCEAN_HARDWARE_AVX_VERSION 10
		#endif

	#endif
#endif

#ifndef OCEAN_HARDWARE_AVX_VERSION
	#define OCEAN_HARDWARE_AVX_VERSION 0
#endif




// Defines OCEAN_LITTLE_ENDIAN on platforms with little endian byte order, do not define it on big endian platforms
#if defined(_WINDOWS) || defined(_ANDROID) || defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)
	#ifndef OCEAN_LITTLE_ENDIAN
		#define OCEAN_LITTLE_ENDIAN
	#endif
#endif




// Defines OCEAN_BASE_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_BASE_EXPORT
		#define OCEAN_BASE_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_BASE_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_BASE_EXPORT
#endif




// Defines OCEAN_REDIRECT_ASSERT_TO_MESSENGER on platforms not providing useful assert handling
#if defined(_ANDROID)
	//#define OCEAN_REDIRECT_ASSERT_TO_MESSENGER
#endif

// Defines the ocean_assert() macro
//
// The 'ocean_assert' or 'ocean_assert_accuracy' macro should be used instead of the standard C assert() directly.
//
// ocean_assert will be active as long as OCEAN_DEBUG is not defined
// In case, OCEAN_DEBUG is not defined, the expression of the assert will disappear, so that compilers may come up with warnings regarding unused variables.
//
// Use 'ocean_assert' to ensure that a program state is correct e.g., ocean_assert(pointer != nullptr);
// Use 'ocean_assert_accuracy' to ensure that the accuracy of a parameter is good enough for the following code
#ifndef ocean_assert
	#ifdef OCEAN_DEBUG

		// Overwrites the assert for platforms not allowing for senseful debugging
		#ifdef OCEAN_REDIRECT_ASSERT_TO_MESSENGER

			/**
			 * Error message function for redirected asserts.
			 * @param file Source file in that the assert occurred
			 * @param line Number of the line in that the assert occurred
			 * @param message The message to be redirected
			 * @ingroup base
			 */
			extern OCEAN_BASE_EXPORT void assertErrorMessage(const char* file, const int line, const char* message);

			#define ocean_assert(e) ((bool(e) != true) ? Ocean::assertErrorMessage(__FILE__, __LINE__, #e) : ((void)0))

		#else

			#define ocean_assert(e) assert(e)

		#endif
	#else
		#define ocean_assert(e) ((void)0)
	#endif
#endif

// Defines the ocean_assert_accuracy() macro
//
// The 'ocean_assert' or 'ocean_assert_accuracy' macro should be used instead of the standard C assert() directly.
//
// ocean_assert_accuracy will be active as long as OCEAN_DEBUG is not defined
// In case, OCEAN_DEBUG is not defined, the expression of the assert will disappear, so that compilers may come up with warnings regarding unused variables.
//
// Use 'ocean_assert' to ensure that a program state is correct e.g., ocean_assert(pointer != nullptr);
// Use 'ocean_assert_accuracy' to ensure that the accuracy of a parameter is good enough for the following code
#ifndef ocean_assert_accuracy
	#ifdef OCEAN_DEBUG
		#define ocean_assert_accuracy(e) if (bool(e) != true) std::cout << "ocean_assert_accuracy(" << #e << "): in file " << __FILE__ << ", line " << __LINE__ << std::endl
	#else
		#define ocean_assert_accuracy(e) ((void)0)
	#endif
#endif

// Defines the ocean_assert_and_suppress_unused() macro
//
// ocean_assert_and_suppress_unused behaves like ocean_assert.
// In addition, in case, OCEAN_DEBUG is not defined, ocean_assert_and_suppress_unused suppressed the warning of an unused variable.
// Thus, ocean_assert_and_suppress_unused(expression, variable) does mainly `ocean_assert(expression), OCEAN_SUPPRESS_UNUSED_WARNING(variable)`
#ifndef ocean_assert_and_suppress_unused
	#ifdef OCEAN_DEBUG
		#define ocean_assert_and_suppress_unused(expression, variable) ocean_assert(expression)
	#else
		#define ocean_assert_and_suppress_unused(expression, variable) ((void)(variable))
	#endif
#endif



// Defines OCEAN_SUPPORT_RTTI state for compiler configurations supporting e.g., the usage of typeid.
#ifndef OCEAN_SUPPORT_RTTI
	#if defined(OCEAN_COMPILER_MSC)
		#ifdef _CPPRTTI
			#define OCEAN_SUPPORT_RTTI
		#endif
	#elif defined(OCEAN_COMPILER_GCC) && OCEAN_GCC_VERSION >= 40302
		#ifdef __GXX_RTTI
			#define OCEAN_SUPPORT_RTTI
		#endif
	#elif defined(OCEAN_COMPILER_CLANG) && OCEAN_CLANG_VERSION >= 20700
		#if __has_feature(cxx_rtti)
			#define OCEAN_SUPPORT_RTTI
		#endif
	#endif
#endif // OCEAN_SUPPORT_RTTI




// Defines OCEAN_SUPPORT_EXCEPTIONS state for compiler configurations supporting exceptions.
#ifndef OCEAN_SUPPORT_EXCEPTIONS
	#if defined(OCEAN_COMPILER_MSC)
		#ifdef _CPPUNWIND
			#define OCEAN_SUPPORT_EXCEPTIONS
		#endif
	#elif defined(OCEAN_COMPILER_GCC) && OCEAN_GCC_VERSION > 0
		#if defined(__EXCEPTIONS) && __EXCEPTIONS
			#define OCEAN_SUPPORT_EXCEPTIONS
		#endif
	#elif defined(OCEAN_COMPILER_CLANG) && OCEAN_CLANG_VERSION > 0
		#if defined(__EXCEPTIONS) && __EXCEPTIONS && __has_feature(cxx_exceptions)
			#define OCEAN_SUPPORT_EXCEPTIONS
		#endif
	#endif
#endif // OCEAN_SUPPORT_EXCEPTIONS




// Define OCEAN_COMPILETIME_WARNING_MISSING_IMPLEMENTATION to create compile time warnings for missing implementations
// #define OCEAN_COMPILETIME_WARNING_MISSING_IMPLEMENTATION

// Defines the OCEAN_WARNING_MISSING_IMPLEMENTATION state either creating a compile time warning or a runtime warning (an assert).
#ifndef OCEAN_WARNING_MISSING_IMPLEMENTATION
	#ifdef OCEAN_COMPILETIME_WARNING_MISSING_IMPLEMENTATION
		#ifdef OCEAN_COMPILER_MSC
			#define OCEAN_WARNING_MISSING_IMPLEMENTATION __pragma(message("Missing implementation!")); assert(false && "Missing implementation!")
		#else
			#define OCEAN_WARNING_MISSING_IMPLEMENTATION _Pragma ("message \"Missing implementation\""); assert(false && "Missing implementation!")
		#endif
	#else
		#define OCEAN_WARNING_MISSING_IMPLEMENTATION assert(false && "Missing implementation!")
	#endif
#endif // OCEAN_WARNING_MISSING_IMPLEMENTATION




// Defines the OCEAN_SUPPRESS_UNUSED_WARNING state that allows to suppress warnings regarding unused variables.
#ifndef OCEAN_SUPPRESS_UNUSED_WARNING
	#define OCEAN_SUPPRESS_UNUSED_WARNING(e) ((void)e)
#endif




/**
 * This function is a helper function returning false any time.
 * @return False, any the time
 * @tparam T Template parameter that can be used, however it does not have any impact
 * @ingroup base
 */
template <typename T>
inline constexpr bool oceanFalse()
{
	return false;
}




// Definition of OCEAN_PLATFORM_BUILD_XXX for individual platforms
#if defined(_WINDOWS)

	#define OCEAN_PLATFORM_BUILD_WINDOWS

#elif defined(__APPLE__)

	#define OCEAN_PLATFORM_BUILD_APPLE

	#if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR == 1

		#define OCEAN_PLATFORM_BUILD_APPLE_IOS_SUMULATOR

	 #elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

		#define OCEAN_PLATFORM_BUILD_APPLE_IOS

	#elif defined(TARGET_OS_MAC) && TARGET_OS_MAC == 1

		#define OCEAN_PLATFORM_BUILD_APPLE_MACOS

	#else

		#error Missing implementation

	#endif

	#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_SUMULATOR) || defined(OCEAN_PLATFORM_BUILD_APPLE_IOS)

		#define OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	#endif

#elif defined(_ANDROID)

	#define OCEAN_PLATFORM_BUILD_ANDROID

#elif defined(__linux__) || defined(__EMSCRIPTEN__)

	#define OCEAN_PLATFORM_BUILD_LINUX

#else

	#error Missing implementation

#endif

#if defined(OCEAN_PLATFORM_BUILD_ANDROID) || defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)

	#define OCEAN_PLATFORM_BUILD_MOBILE

#endif




// The OCEAN_APPLY_IF_DEBUG allows to invoke an expression in debug builds.
#ifndef OCEAN_APPLY_IF_DEBUG
	#ifdef OCEAN_DEBUG
		#define OCEAN_APPLY_IF_DEBUG(e) e
	#else
		#define OCEAN_APPLY_IF_DEBUG(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_WINDOWS allows to invoke an expression on windows platforms only.
#ifndef OCEAN_APPLY_IF_WINDOWS
	#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
		#define OCEAN_APPLY_IF_WINDOWS(e) e
	#else
		#define OCEAN_APPLY_IF_WINDOWS(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_APPLE allows to invoke an expression on apple platforms only.
#ifndef OCEAN_APPLY_IF_APPLE
	#ifdef OCEAN_PLATFORM_BUILD_APPLE
		#define OCEAN_APPLY_IF_APPLE(e) e
	#else
		#define OCEAN_APPLY_IF_APPLE(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_LINUX allows to invoke an expression on apple platforms only.
#ifndef OCEAN_APPLY_IF_LINUX
	#ifdef OCEAN_PLATFORM_BUILD_LINUX
		#define OCEAN_APPLY_IF_LINUX(e) e
	#else
		#define OCEAN_APPLY_IF_LINUX(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_ANDROID allows to invoke an expression on android platforms only.
#ifndef OCEAN_APPLY_IF_ANDROID
	#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		#define OCEAN_APPLY_IF_ANDROID(e) e
	#else
		#define OCEAN_APPLY_IF_ANDROID(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_IPHONE allows to invoke an expression on iOS platforms only.
#ifndef OCEAN_APPLY_IF_IPHONE
	#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS) || defined(TARGET_IPHONE_SIMULATOR)
		#define OCEAN_APPLY_IF_IPHONE(e) e
	#else
		#define OCEAN_APPLY_IF_IPHONE(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_GCC allows to invoke an expression with GCC compilers only.
#ifndef OCEAN_APPLY_IF_GCC
	#if defined(OCEAN_GCC_VERSION) && OCEAN_GCC_VERSION > 0
		#define OCEAN_APPLY_IF_GCC(e) e
	#else
		#define OCEAN_APPLY_IF_GCC(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_SSE allows to invoke an expression if any valid SSE version is defined.
#ifndef OCEAN_APPLY_IF_SSE
	#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION > 0
		#define OCEAN_APPLY_IF_SSE(e) e
	#else
		#define OCEAN_APPLY_IF_SSE(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_AVX allows to invoke an expression if any valid AVX version is defined.
#ifndef OCEAN_APPLY_IF_AVX
	#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION > 0
		#define OCEAN_APPLY_IF_AVX(e) e
	#else
		#define OCEAN_APPLY_IF_AVX(e) {}
	#endif
#endif

// The OCEAN_APPLY_IF_NEON allows to invoke an expression if any valid NEON version is defined.
#ifndef OCEAN_APPLY_IF_NEON
	#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION > 0
		#define OCEAN_APPLY_IF_NEON(e) e
	#else
		#define OCEAN_APPLY_IF_NEON(e) {}
	#endif
#endif





// The OCEAN_FORCE_INLINE macro allows to overwrite the compiler's inline analysis and forces a function to be inlined in any situation, as long as supported by the compiler
#ifndef OCEAN_FORCE_INLINE
	#if defined(OCEAN_COMPILER_MSC)
		#define OCEAN_FORCE_INLINE __forceinline
	#elif defined(OCEAN_COMPILER_CLANG)
		#if __has_attribute(always_inline)
			#define OCEAN_FORCE_INLINE inline __attribute__((always_inline))
		#else
			#define OCEAN_FORCE_INLINE inline
		#endif
	#elif defined(OCEAN_COMPILER_GCC)
		#define OCEAN_FORCE_INLINE inline __attribute__((always_inline))
	#else
		#define OCEAN_FORCE_INLINE inline
	#endif
#endif




// The OCEAN_PREVENT_INLINE macro allows to overwrite the compiler's inline analysis and forces a function from being inlined in any situation, as long as supported by the compiler
#ifndef OCEAN_PREVENT_INLINE
	#if defined(OCEAN_COMPILER_MSC)
		#define OCEAN_PREVENT_INLINE __declspec(noinline)
	#elif defined(OCEAN_COMPILER_CLANG)
		#if __has_attribute(noinline)
			#define OCEAN_PREVENT_INLINE __attribute__((noinline))
		#else
			#define OCEAN_PREVENT_INLINE
		#endif
	#elif defined(OCEAN_COMPILER_GCC)
		#define OCEAN_PREVENT_INLINE __attribute__((noinline))
	#else
		#define OCEAN_PREVENT_INLINE
	#endif
#endif




// The OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION macro allows to disable loop optimizations in the subsequent loop
#ifndef OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION
	#if defined(OCEAN_COMPILER_MSC)
		#define OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION _Pragma("loop(no_vector)")
	#elif defined(OCEAN_COMPILER_CLANG)
		#define OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION _Pragma("clang loop vectorize(disable) interleave(disable)")
	#elif defined(OCEAN_COMPILER_GCC)
		#define OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION _Pragma("GCC unroll 1")
	#else
		#define OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION
	#endif
#endif

// The OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION_IF_NEON macro allows to disable loop optimizations in the subsequent loop in case NEON is supported
#ifndef OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION_IF_NEON
	#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION > 0
		#define OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION_IF_NEON OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION
	#else
		#define OCEAN_DISABLE_SUBSEQUENT_LOOP_OPTIMIZATION_IF_NEON
	#endif
#endif




// The OCEAN_ALIGN_DATA macro allows to force the alignment of data to a specified byte boundary
#ifndef OCEAN_ALIGN_DATA
	#if defined(OCEAN_COMPILER_MSC)
		#define OCEAN_ALIGN_DATA(x) __declspec(align(x))
	#else
		#define OCEAN_ALIGN_DATA(x) __attribute__ ((aligned(x)))
	#endif
#endif





#if (defined(DEBUG) || defined(OCEAN_DEBUG)) && defined(NDEBUG)
	#error You cannot define both flags (DEBUG and NDEBUG) concurrently
#endif

#if defined(OCEAN_INTENSIVE_DEBUG) && (!defined(DEBUG) || !defined(OCEAN_DEBUG))
	#error You cannot define OCEAN_INTENSIVE_DEBUG without defining DEBUG/OCEAN_DEBUG
#endif




#ifndef OCEAN_NDEBUG
	#if !defined(OCEAN_DEBUG) || defined(NDEBUG)
		#define OCEAN_NDEBUG
	#endif
#endif




// The OCEAN_DISABLE_DOCUMENTATION_DIAGNOSTIC macro allows to disable Clang's code documentation diagnostic, must be balanced with a following usage of `OCEAN_RE_ENABLE_DOCUMENTATION_DIAGNOSTIC`
#ifndef OCEAN_DISABLE_DOCUMENTATION_DIAGNOSTIC
	#if defined(OCEAN_CLANG_VERSION) && OCEAN_CLANG_VERSION > 0
		#define OCEAN_DISABLE_DOCUMENTATION_DIAGNOSTIC _Pragma("clang diagnostic push") \
														_Pragma("clang diagnostic ignored \"-Wdocumentation\"")
	#else
		#define OCEAN_DISABLE_DOCUMENTATION_DIAGNOSTIC
	#endif
#endif

// The OCEAN_RE_ENABLE_DOCUMENTATION_DIAGNOSTIC macro allows to re-enabled Clang's code documentation diagnostic, must be balanced with earlier usage of `OCEAN_DISABLE_DOCUMENTATION_DIAGNOSTIC`
#ifndef OCEAN_RE_ENABLE_DOCUMENTATION_DIAGNOSTIC
	#if defined(OCEAN_CLANG_VERSION) && OCEAN_CLANG_VERSION > 0
		#define OCEAN_RE_ENABLE_DOCUMENTATION_DIAGNOSTIC _Pragma("clang diagnostic pop")
	#else
		#define OCEAN_RE_ENABLE_DOCUMENTATION_DIAGNOSTIC
	#endif
#endif




// The OCEAN_PRINT_MACRO_VALUE macro allows to print a macro value at compile time, usage e.g., `OCEAN_PRINT_MACRO_VALUE("This is a version number: ", OCEAN_CLANG_VERSION)`
#ifndef OCEAN_PRINT_MACRO_VALUE
	#define OCEAN_PRINT_MACRO_VALUE_INTERNAL_INTERNAL(value) #value
	#define OCEAN_PRINT_MACRO_VALUE_INTERNAL(value) OCEAN_PRINT_MACRO_VALUE_INTERNAL_INTERNAL(value)

	#define OCEAN_PRAGMA_INTERNAL(value) _Pragma(#value)
	#define OCEAN_PRAGMA(value) OCEAN_PRAGMA_INTERNAL(value)

	#define OCEAN_PRINT_MACRO_VALUE(text, value) OCEAN_PRAGMA(message text OCEAN_PRINT_MACRO_VALUE_INTERNAL(value))
#endif

}

#endif // META_OCEAN_BASE_BASE_H
