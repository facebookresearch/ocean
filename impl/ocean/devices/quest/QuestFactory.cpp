// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/QuestFactory.h"
#include "ocean/devices/quest/QuestGPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

QuestFactory::QuestFactory() :
	Factory(nameQuestLibrary())
{
	registerDevice(QuestGPSTracker::deviceNameQuestGPSTracker(), QuestGPSTracker::deviceTypeQuestGPSTracker(), InstanceFunction::createStatic(&QuestFactory::createQuestGPSTracker));
}

bool QuestFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new QuestFactory()));
}

bool QuestFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameQuestLibrary());
}

Device* QuestFactory::createQuestGPSTracker(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert(name == QuestGPSTracker::deviceNameQuestGPSTracker());
	ocean_assert(deviceType == QuestGPSTracker::deviceTypeQuestGPSTracker());

	return new QuestGPSTracker();
}

}

}

}
