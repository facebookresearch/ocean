// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesoceanorb Ocean Unified Features Ocean ORB Library
 * @{
 * The Ocean Unified Features Ocean ORB Library provides the Ocean ORB features.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::OceanOrb Namespace of the Ocean ORB Library.<p>
 * The Namespace Ocean::UnifiedFeatures::OceanOrb is used in the Ocean ORB Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_OO_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_OO_EXPORT
		#define OCEAN_UNIFIEDFEATURES_OO_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_OO_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_OO_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesoceanorb
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_OO_EXPORT std::string nameOceanOrbLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesoceanorb
 */
void registerOceanOrbUnifiedObjects();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesoceanorb
 */
bool unregisterOceanOrbUnifiedObjects();

#endif // OCEAN_RUNTIME_STATIC

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
