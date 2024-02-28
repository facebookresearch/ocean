// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/DescriptorGenerator.h"

namespace Ocean
{

namespace UnifiedFeatures
{

bool DescriptorGenerator::isSupportedFeatureDetector(const FeatureDetector& featureDetector) const
{
	// Derived classes will have to override this function
	return false;
}

} // namespace UnifiedFeatures

} // namespace Ocean
