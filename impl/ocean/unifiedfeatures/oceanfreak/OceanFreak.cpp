// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanfreak/OceanFreak.h"
#include "ocean/unifiedfeatures/oceanfreak/OFDescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanfreak/OFDescriptorGenerator.h"
#include "ocean/unifiedfeatures/oceanfreak/OFHarrisFeatureContainer.h"
#include "ocean/unifiedfeatures/oceanfreak/OFHarrisFreakFeatureDetector.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

std::string nameOceanFreakLibrary()
{
	return std::string("OceanFreak");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerUnifiedObjectsOceanFreak()
{
	Manager::get().registerUnifiedObject(OFDescriptorContainer32::nameOFDescriptorContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&OFDescriptorContainer32::create));
	Manager::get().registerUnifiedObject(OFDescriptorContainer64::nameOFDescriptorContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&OFDescriptorContainer64::create));
	Manager::get().registerUnifiedObject(OFDescriptorGenerator32::nameOFDescriptorGenerator(), Manager::CreateUnifiedObjectCallback::createStatic(&OFDescriptorGenerator32::create));
	Manager::get().registerUnifiedObject(OFDescriptorGenerator64::nameOFDescriptorGenerator(), Manager::CreateUnifiedObjectCallback::createStatic(&OFDescriptorGenerator64::create));
	Manager::get().registerUnifiedObject(OFHarrisFeatureContainer::nameOFHarrisFeatureContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&OFHarrisFeatureContainer::create));
	Manager::get().registerUnifiedObject(OFHarrisFreakFeatureDetector32::nameOFHarrisFreakFeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&OFHarrisFreakFeatureDetector32::create));
	Manager::get().registerUnifiedObject(OFHarrisFreakFeatureDetector64::nameOFHarrisFreakFeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&OFHarrisFreakFeatureDetector64::create));
}

bool unregisterUnifiedObjectsOceanFreak()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(OFDescriptorContainer32::nameOFDescriptorContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(OFDescriptorContainer64::nameOFDescriptorContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(OFDescriptorGenerator32::nameOFDescriptorGenerator()) && result;
	result = Manager::get().unregisterUnifiedObject(OFDescriptorGenerator64::nameOFDescriptorGenerator()) && result;
	result = Manager::get().unregisterUnifiedObject(OFHarrisFeatureContainer::nameOFHarrisFeatureContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(OFHarrisFreakFeatureDetector32::nameOFHarrisFreakFeatureDetector()) && result;
	result = Manager::get().unregisterUnifiedObject(OFHarrisFreakFeatureDetector64::nameOFHarrisFreakFeatureDetector()) && result;

	return result;
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
