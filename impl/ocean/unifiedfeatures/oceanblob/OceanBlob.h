// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

/**
 * @ingroup unifiedfeatures
 * @defgroup unifiedfeaturesoceanblob Ocean Unified Features Ocean Blob Library
 * @{
 * The Ocean Unified Features Ocean Blob Library provides the Ocean Blob features.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures::OceanBlob Namespace of the Ocean Blob Library.<p>
 * The Namespace Ocean::UnifiedFeatures::OceanBlob is used in the Ocean Blob Library.
 */

// Defines USE_OCEAN_UNIFIEDFEATURES_OB_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_OB_EXPORT
		#define OCEAN_UNIFIEDFEATURES_OB_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_OB_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_OB_EXPORT
#endif

/**
 * Returns the name of this library.
 * @ingroup unifiedfeaturesoceanblob
 * @return The name of this library
 */
OCEAN_UNIFIEDFEATURES_OB_EXPORT std::string nameOceanBlobLibrary();

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

/**
 * Registers the unified objects of this library with the global library manager.
 * @ingroup unifiedfeaturesoceanblob
 */
void registerOceanBlobUnifiedObjects();

/**
 * Unregisters the unified objects of this library from the global library manager.
 * @return True, if succeeded
 * @ingroup unifiedfeaturesoceanblob
 */
bool unregisterOceanBlobUnifiedObjects();

#endif // OCEAN_RUNTIME_STATIC

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
