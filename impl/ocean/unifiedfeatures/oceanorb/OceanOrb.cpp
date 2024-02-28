// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanorb/OceanOrb.h"
#include "ocean/unifiedfeatures/oceanorb/OODescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanorb/OODescriptorGenerator.h"
#include "ocean/unifiedfeatures/oceanorb/OONonBijectiveMatcher.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

std::string nameOceanOrbLibrary()
{
	return std::string("OceanOrb");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerOceanOrbUnifiedObjects()
{
	Manager::get().registerUnifiedObject(OODescriptorContainer::nameOODescriptorContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&OODescriptorContainer::create));
	Manager::get().registerUnifiedObject(OODescriptorGenerator::nameOODescriptorGenerator(), Manager::CreateUnifiedObjectCallback::createStatic(&OODescriptorGenerator::create));
	Manager::get().registerUnifiedObject(OONonBijectiveMatcher::nameOONonBijectiveMatcher(), Manager::CreateUnifiedObjectCallback::createStatic(&OONonBijectiveMatcher::create));
}

bool unregisterOceanOrbUnifiedObjects()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(OODescriptorContainer::nameOODescriptorContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(OODescriptorGenerator::nameOODescriptorGenerator()) && result;
	result = Manager::get().unregisterUnifiedObject(OONonBijectiveMatcher::nameOONonBijectiveMatcher()) && result;

	return result;
}

#endif // defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
