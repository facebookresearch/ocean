// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/oceanharris/OceanHarris.h"
#include "ocean/unifiedfeatures/oceanharris/OHFeatureContainer.h"
#include "ocean/unifiedfeatures/oceanharris/OHFeatureDetector.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace  OceanHarris
{

std::string nameOceanHarrisLibrary()
{
	return std::string("OceanHarris");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerOceanHarrisUnifiedObjects()
{
	Manager::get().registerUnifiedObject(OHFeatureContainer::nameOHFeatureContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&OHFeatureContainer::create));
	Manager::get().registerUnifiedObject(OHFeatureDetector::nameOHFeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&OHFeatureDetector::create));
}

bool unregisterOceanHarrisUnifiedObjects()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(OHFeatureContainer::nameOHFeatureContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(OHFeatureDetector::nameOHFeatureDetector()) && result;

	return result;
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace OceanHarris

} // namespace UnifiedFeatures

} // namespace Ocean
