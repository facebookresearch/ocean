// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/DescriptorGenerator.h"

namespace Ocean
{

namespace UnifiedFeatures
{

std::shared_ptr<DescriptorGenerator> FeatureDetector::getBestAvailableDescriptorGenerator()
{
	// Derived classes should override this function.
	return nullptr;
}

} // namespace UnifiedFeatures

} // namespace Ocean
