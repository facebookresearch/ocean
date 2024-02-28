// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/quest/Quest.h"
#include "ocean/media/quest/QuestLibrary.h"

namespace Ocean
{

namespace Media
{

namespace Quest
{

std::string nameQuestLibrary()
{
	return std::string("Quest");
}

#ifdef OCEAN_RUNTIME_STATIC

void registerQuestLibrary()
{
	QuestLibrary::registerLibrary();
}

bool unregisterQuestLibrary()
{
	return QuestLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
