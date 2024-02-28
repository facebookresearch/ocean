// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/Quest.h"
#include "ocean/devices/quest/QuestFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

std::string nameQuestLibrary()
{
	return std::string("Quest Devices");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerQuestLibrary()
{
	QuestFactory::registerFactory();
}

bool unregisterQuestLibrary()
{
	return QuestFactory::unregisterFactory();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
