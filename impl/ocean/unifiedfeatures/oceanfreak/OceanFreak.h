// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesoceanfreak Ocean Unified Features Ocean FREAK Library
 * @{
 * The Ocean Unified Features Ocean FREAK Library provides the Ocean FREAK descriptor.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::OceanFreak Namespace of the Ocean FREAK Library.<p>
 * The Namespace Ocean::UnifiedFeatures::OceanFreak is used in the Ocean FREAK Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_OF_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_OF_EXPORT
		#define OCEAN_UNIFIEDFEATURES_OF_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_OF_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_OF_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesoceanfreak
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_OF_EXPORT std::string nameOceanFreakLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesoceanharris
 */
void registerUnifiedObjectsOceanFreak();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesoceanharris
 */
bool unregisterUnifiedObjectsOceanFreak();

#endif // OCEAN_RUNTIME_STATIC

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
