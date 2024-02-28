// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_QUEST_QUEST_H
#define META_OCEAN_MEDIA_QUEST_QUEST_H

#include "ocean/media/Media.h"

#ifndef _ANDROID
	#error This library is available on Quest/Android platforms only!
#endif

namespace Ocean
{

namespace Media
{

namespace Quest
{

/**
 * @ingroup media
 * @defgroup mediaquest Ocean Media Quest Library
 * @{
 * The Ocean Media Quest Library provides camera access functionalities on Quest platforms via Sensor Data API Unification (SDAU).
 * The library is available on Quest/Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Media::Quest Namespace of the Media Quest library.<p>
 * The Namespace Ocean::Media::Quest is used in the entire Ocean Media Quest Library.
 */

// Defines OCEAN_MEDIA_QUEST_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_QUEST_EXPORT
		#define OCEAN_MEDIA_QUEST_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_QUEST_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_QUEST_EXPORT
#endif

/**
 * Returns the name of this media library.
 * @ingroup mediaquest
 */
OCEAN_MEDIA_QUEST_EXPORT std::string nameQuestLibrary();

#ifdef OCEAN_RUNTIME_STATIC

/**
 * Registers this media library at the global library manager.
 * This function calls QuestLibrary::registerLibrary() only.
 * @ingroup mediaquest
 */
void registerQuestLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls QuestLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediaquest
 */
bool unregisterQuestLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_MEDIA_QUEST_QUEST_H
