// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanblob/OceanBlob.h"
#include "ocean/unifiedfeatures/oceanblob/OBDescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanblob/OBDescriptorGenerator.h"
#include "ocean/unifiedfeatures/oceanblob/OBFeatureContainer.h"
#include "ocean/unifiedfeatures/oceanblob/OBFeatureDetector.h"
#include "ocean/unifiedfeatures/oceanblob/OBNonBijectiveMatcher.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

std::string nameOceanBlobLibrary()
{
	return std::string("OceanBlob");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerOceanBlobUnifiedObjects()
{
	Manager::get().registerUnifiedObject(OBDescriptorContainer::nameOBDescriptorContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&OBDescriptorContainer::create));
	Manager::get().registerUnifiedObject(OBDescriptorGenerator::nameOBDescriptorGenerator(), Manager::CreateUnifiedObjectCallback::createStatic(&OBDescriptorGenerator::create));
	Manager::get().registerUnifiedObject(OBNonBijectiveMatcher::nameOBNonBijectiveMatcher(), Manager::CreateUnifiedObjectCallback::createStatic(&OBNonBijectiveMatcher::create));
	Manager::get().registerUnifiedObject(OBFeatureContainer::nameOBFeatureContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&OBFeatureContainer::create));
	Manager::get().registerUnifiedObject(OBFeatureDetector::nameOBFeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&OBFeatureDetector::create));
}

bool unregisterOceanBlobUnifiedObjects()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(OBDescriptorContainer::nameOBDescriptorContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(OBDescriptorGenerator::nameOBDescriptorGenerator()) && result;
	result = Manager::get().unregisterUnifiedObject(OBNonBijectiveMatcher::nameOBNonBijectiveMatcher()) && result;
	result = Manager::get().unregisterUnifiedObject(OBFeatureContainer::nameOBFeatureContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(OBFeatureDetector::nameOBFeatureDetector()) && result;

	return result;
}

#endif // defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
