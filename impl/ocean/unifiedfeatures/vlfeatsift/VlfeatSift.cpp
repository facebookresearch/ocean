// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSift.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftBruteForceDescriptorMatcher.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftFeatureContainer.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftFeatureDetector.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftDescriptorContainer.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftDescriptorGenerator.h"

#include "ocean/unifiedfeatures/Manager.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

std::string nameVlfeatSiftLibrary()
{
	return std::string("VlfeatSift");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerVlfeatSiftUnifiedObjects()
{
	Manager::get().registerUnifiedObject(VlfeatSiftBruteForceDescriptorMatcher::nameVlfeatSiftBruteForceDescriptorMatcher(), Manager::CreateUnifiedObjectCallback::createStatic(&VlfeatSiftBruteForceDescriptorMatcher::create));
	Manager::get().registerUnifiedObject(VlfeatSiftFeatureContainer::nameVlfeatSiftFeatureContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&VlfeatSiftFeatureContainer::create));
	Manager::get().registerUnifiedObject(VlfeatSiftFeatureDetector::nameVlfeatSiftFeatureDetector(), Manager::CreateUnifiedObjectCallback::createStatic(&VlfeatSiftFeatureDetector::create));
	Manager::get().registerUnifiedObject(VlfeatSiftDescriptorContainer::nameVlfeatSiftDescriptorContainer(), Manager::CreateUnifiedObjectCallback::createStatic(&VlfeatSiftDescriptorContainer::create));
	Manager::get().registerUnifiedObject(VlfeatSiftDescriptorGenerator::nameVlfeatSiftDescriptorGenerator(), Manager::CreateUnifiedObjectCallback::createStatic(&VlfeatSiftDescriptorGenerator::create));
}

bool unregisterVlfeatSiftUnifiedObjects()
{
	bool result = true;

	result = Manager::get().unregisterUnifiedObject(VlfeatSiftBruteForceDescriptorMatcher::nameVlfeatSiftBruteForceDescriptorMatcher()) && result;
	result = Manager::get().unregisterUnifiedObject(VlfeatSiftFeatureContainer::nameVlfeatSiftFeatureContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(VlfeatSiftFeatureDetector::nameVlfeatSiftFeatureDetector()) && result;
	result = Manager::get().unregisterUnifiedObject(VlfeatSiftDescriptorContainer::nameVlfeatSiftDescriptorContainer()) && result;
	result = Manager::get().unregisterUnifiedObject(VlfeatSiftDescriptorGenerator::nameVlfeatSiftDescriptorGenerator()) && result;

	return result;
}

#endif // OCEAN_RUNTIME_STATIC

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
