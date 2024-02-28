// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/base/Base.h"
#include "ocean/base/Frame.h"

namespace Ocean
{

namespace UnifiedFeatures
{

/**
 * @defgroup unifiedfeatures Ocean Unified Features Library
 * @{
 * The Ocean Unified Features Library provides a unified interface for feature detectors and descriptors.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::UnifiedFeatures Namespace of the Ocean Feature Library.<p>
 * The Namespace Ocean::UnifiedFeatures is used in the entire Ocean Feature Library.
 */

// Defines OCEAN_UNIFIEDFEATURES_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_UNIFIEDFEATURES_EXPORT
		#define OCEAN_UNIFIEDFEATURES_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_UNIFIEDFEATURES_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_UNIFIEDFEATURES_EXPORT
#endif

/// Typedef for a set of pixel formats
typedef std::unordered_set<FrameType::PixelFormat> PixelFormatSet;

/**
 * Definition of different categories of features
 * @ingroup unifiedfeatures
 */
enum class FeatureCategory : uint32_t
{
	/// Unknown type
	UNKNOWN = 0u,
	/// Indicates that the features are keypoints
	KEYPOINTS,
};

/**
 * Definition of different categories of descriptors
 * @ingroup unifiedfeatures
 */
enum class DescriptorCategory : uint32_t
{
	/// Unknown type
	UNKNOWN = 0u,
	/// A binary descriptor
	BINARY_DESCRIPTOR,
	/// A descriptor consisting for floating-point descriptors
	FLOAT_DESCRIPTOR,
	/// A descriptor consisting for uint8 descriptors
	UINT8_DESCRIPTOR,
};

/**
 * Definition of the base class for parameters
 * @ingroup unifiedfeatures
 */
class Parameters
{
	public:

		/**
		 * Default destructor
		 */
		virtual ~Parameters() = default;
};

} // namespace UnifiedFeatures

} // namespace Ocean
