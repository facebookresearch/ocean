// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/math/Math.h"
#include "ocean/unifiedfeatures/oceanblob/OceanBlob.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{
/**
 * Definition of parameters for the Blob detector and descriptor generator.
 * @ingroup unifiedfeaturesoceanblob
 */
class OCEAN_UNIFIEDFEATURES_OB_EXPORT OBParameters : public Ocean::UnifiedFeatures::Parameters
{
	public:

		/**
	 	 * Default constructor
	 	 */
		OBParameters() = default;

	public:

		/// Minimum strength for a blob feature to be detected by the detector.
		Scalar minimumStrength = Scalar(6.5);
		/// Maximum number of features returned by the detector. Features with weaker strength will be discarded.
		uint32_t maximumNumberFeatures = 10000u;
};

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
