// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/boltharris/BoltHarris.h"
#include "ocean/unifiedfeatures/boltharris/BHFeatureContainer.h"
#include "ocean/unifiedfeatures/boltharris/BHFeatureDetector.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace BoltHarris
{

std::string nameBoltHarrisLibrary()
{
	return std::string("BoltHarris");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerUnifiedObjectsBoltHarris()
{
	Manager::get().registerUnifiedObject(BHFeatureContainer::nameBHFeatureContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&BHFeatureContainer::create));
	Manager::get().registerUnifiedObject(BHFeatureDetector::nameBHFeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&BHFeatureDetector::create));
}

bool unregisterUnifiedObjectsBoltHarris()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(BHFeatureContainer::nameBHFeatureContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(BHFeatureDetector::nameBHFeatureDetector()) && result;

	return result;
}

#endif // defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

} // namespace BoltHarris

} // namespace UnifiedFeatures

} // namespace Ocean
