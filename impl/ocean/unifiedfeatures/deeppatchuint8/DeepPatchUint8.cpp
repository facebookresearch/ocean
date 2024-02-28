// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/deeppatchuint8/DeepPatchUint8.h"
#include "ocean/unifiedfeatures/deeppatchuint8/DP8DescriptorContainer.h"
#include "ocean/unifiedfeatures/deeppatchuint8/DP8FeatureDetector.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

std::string nameDeepPatchUint8Library()
{
	return std::string("DeepPatchUint8");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerUnifiedObjectsDeepPatchUint8()
{
	Manager::get().registerUnifiedObject(DP8DescriptorContainer::nameDP8DescriptorContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&DP8DescriptorContainer::create));
	Manager::get().registerUnifiedObject(DP8FeatureDetector::nameDP8FeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&DP8FeatureDetector::create));
}

bool unregisterUnifiedObjectsDeepPatchUint8()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(DP8DescriptorContainer::nameDP8DescriptorContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(DP8FeatureDetector::nameDP8FeatureDetector()) && result;

	return result;
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
