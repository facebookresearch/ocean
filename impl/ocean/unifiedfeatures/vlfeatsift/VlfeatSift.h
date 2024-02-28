// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesvlfeatsift Ocean Unified Features VLFeat SIFT Library
 * @{
 * The Ocean Unified Features VLFeat SIFT Library provides the VLFeat SIFT corner detector.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::VlfeatSift Namespace of the VLFeat SIFT Library.<p>
 * The Namespace Ocean::UnifiedFeatures::VlfeatSift is used in the VLFeat SIFT Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT
		#define OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesvlfeatsift
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT std::string nameVlfeatSiftLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesvlfeatsift
 */
void registerVlfeatSiftUnifiedObjects();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesvlfeatsift
 */
bool unregisterVlfeatSiftUnifiedObjects();

#endif // OCEAN_RUNTIME_STATIC

/// Number of elements in a SIFT descriptor.
static constexpr size_t kSiftDescriptorSizeInElements = 128;

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
