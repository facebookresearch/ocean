// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace BoltHarris
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesboltharris Ocean Unified Features Bolt Harris Library
 * @{
 * The Ocean Unified Features Bolt Harris Library provides the Bolt Harris corner detector (reference implementation).
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::BoltHarris Namespace of the Bolt Harris Library.<p>
 * The Namespace Ocean::UnifiedFeatures::BoltHarris is used in the Bolt Harris Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_BH_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_BH_EXPORT
		#define OCEAN_UNIFIEDFEATURES_BH_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_BH_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_BH_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesboltharris
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_BH_EXPORT std::string nameBoltHarrisLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesoceanharris
 */
void registerUnifiedObjectsBoltHarris();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesoceanharris
 */
bool unregisterUnifiedObjectsBoltHarris();

#endif // OCEAN_RUNTIME_STATIC
} // namespace BoltHarris

} // namespace UnifiedFeatures

} // namespace Ocean
