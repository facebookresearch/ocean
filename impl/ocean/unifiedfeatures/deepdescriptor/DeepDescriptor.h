// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

#include <perception/features/deep/Deep.h>

#include <array>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesdeepdescriptor Ocean Unified Features Deep Descriptor Library
 * @{
 * The Ocean Unified Features Deep Descriptor Library provides an implementation of deep descriptors.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::DeepDescriptor Namespace of the Deep Descriptor Library.<p>
 * The Namespace Ocean::UnifiedFeatures::DeepDescriptor is used in the Deep Descriptor Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_DD_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_DD_EXPORT
		#define OCEAN_UNIFIEDFEATURES_DD_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_DD_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_DD_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesdeepdescriptor
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_DD_EXPORT std::string nameDeepDescriptorLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesdeepdescriptor
 */
void registerUnifiedObjectsDeepDescriptor();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesdeepdescriptor
 */
bool unregisterUnifiedObjectsDeepDescriptor();

#endif // OCEAN_RUNTIME_STATIC

/// Typedef for deep descriptor dimensionality.
static constexpr size_t deepPatchFloatDescriptorSizeInElements = 128;

/// Typedef for a deep descriptor
typedef std::array<float, deepPatchFloatDescriptorSizeInElements> DeepPatchFloatDescriptor;

/// Typedef for a vector of deep descriptors
typedef std::vector<DeepPatchFloatDescriptor> DeepPatchFloatDescriptors;

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
