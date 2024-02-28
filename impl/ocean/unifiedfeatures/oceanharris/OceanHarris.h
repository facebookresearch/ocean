// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanHarris
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesoceanharris Ocean Unified Features Ocean Harris Library
 * @{
 * The Ocean Unified Features Ocean Harris Library provides the Ocean Harris corner detector.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::OceanHarris Namespace of the Ocean Harris Library.<p>
 * The Namespace Ocean::UnifiedFeatures::OceanHarris is used in the Ocean Harris Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_OH_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_OH_EXPORT
		#define OCEAN_UNIFIEDFEATURES_OH_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_OH_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_OH_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesoceanharris
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_OH_EXPORT std::string nameOceanHarrisLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesoceanharris
 */
void registerOceanHarrisUnifiedObjects();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesoceanharris
 */
bool unregisterOceanHarrisUnifiedObjects();

#endif // OCEAN_RUNTIME_STATIC

} // namespace OceanHarris

} // namespace UnifiedFeatures

} // namespace Ocean
