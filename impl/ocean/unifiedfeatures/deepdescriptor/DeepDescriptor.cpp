// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/deepdescriptor/DeepDescriptor.h"
#include "ocean/unifiedfeatures/deepdescriptor/DDFloatDescriptorContainer.h"
#include "ocean/unifiedfeatures/deepdescriptor/DDHarrisDeepPatchFloatFeatureDetector.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

std::string nameDeepDescriptorLibrary()
{
	return std::string("DeepDescriptor");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerUnifiedObjectsDeepDescriptor()
{
	Manager::get().registerUnifiedObject(DDFloatDescriptorContainer::nameDDFloatDescriptorContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&DDFloatDescriptorContainer::create));
	Manager::get().registerUnifiedObject(DDHarrisDeepPatchFloatFeatureDetector::nameDDHarrisDeepPatchFloatFeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&DDHarrisDeepPatchFloatFeatureDetector::create));
}

bool unregisterUnifiedObjectsDeepDescriptor()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(DDFloatDescriptorContainer::nameDDFloatDescriptorContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(DDHarrisDeepPatchFloatFeatureDetector::nameDDHarrisDeepPatchFloatFeatureDetector()) && result;

	return result;
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
