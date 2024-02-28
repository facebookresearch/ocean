// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

#include <perception/features/deep/Deep.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesdeeppatchuint8 Ocean Unified Features Deep Patch Uint8 Library
 * @{
 * The Ocean Unified Features Deep Patch Uint8 Library provides an implementation of DeepPatchUint8.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::DeepDescriptor Namespace of the Deep Patch Uint8 Library.<p>
 * The Namespace Ocean::UnifiedFeatures::DeepDescriptor is used in the Deep Patch Uint8 Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_DP8_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_DP8_EXPORT
		#define OCEAN_UNIFIEDFEATURES_DP8_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_DP8_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_DP8_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesdeeppatchuint8
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_DP8_EXPORT std::string nameDeepPatchUint8Library();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
void registerUnifiedObjectsDeepPatchUint8();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
bool unregisterUnifiedObjectsDeepPatchUint8();

#endif // OCEAN_RUNTIME_STATIC

/**
 * The size of the DeepPatchUint8 descriptor in elements
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
constexpr size_t dp8Descriptor96Size = 96;

/**
 * Typedef for a descriptor of DeepPatchUint8
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
typedef std::array<uint8_t, dp8Descriptor96Size> DP8Descriptor96;

/**
 * Typedef for a vector of descriptors of DeepPatchUint8
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
typedef std::vector<DP8Descriptor96> DP8Descriptors96;

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
