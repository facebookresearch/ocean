/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Event.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

Event::~Event()
{
	// nothing to do here
}

EventManager::EventManager() :
	Thread("EventManager"),
	managerRequestCounter(0u)
{
	startThread();
}

EventManager::~EventManager()
{
	ocean_assert(managerRegistrationRequests.empty());
	ocean_assert(managerRequestSet.empty());

	stopThreadExplicitly();

	for (EventCallbacksMap::const_iterator i = managerEventCallbacksMap.begin(); i != managerEventCallbacksMap.end(); ++i)
		ocean_assert(i->second.isEmpty());

	managerEventCallbacksMap.clear();
}

void EventManager::pushEvent(const EventRef& eventObject)
{
	const ScopedLock scopedLock(managerLock);

	if (managerEventQueue.size() > 10000000)
	{
		// we prevent an extreme memory usage (e.g., if nobody has registered for the events)
		ocean_assert(false && "Too many events, we will skip very old events!");

		managerEventQueue.pop();
	}

	managerEventQueue.push(eventObject);
}

void EventManager::registerEventFunction(const unsigned int eventType, const EventCallback& eventCallback)
{
	const ScopedLock scopedLock(managerRegistrationRequestLock);

	const unsigned int requestId = ++managerRequestCounter;

	ocean_assert(managerRequestSet.find(requestId) == managerRequestSet.end());
	managerRequestSet.insert(requestId);

	managerRegistrationRequests.push_back(RegistrationRequest(true, requestId, eventType, eventCallback));
}

void EventManager::unregisterEventFunction(const unsigned int eventType, const EventCallback& eventCallback)
{
	TemporaryScopedLock temporalScopedLock(managerRegistrationRequestLock);

		const unsigned int requestId = ++managerRequestCounter;

		ocean_assert(managerRequestSet.find(requestId) == managerRequestSet.end());
		managerRequestSet.insert(requestId);

		managerRegistrationRequests.push_back(RegistrationRequest(false, requestId, eventType, eventCallback));

	temporalScopedLock.release();

	const Timestamp startTimestamp(true);
	while (startTimestamp + 5 > Timestamp(true))
	{
		sleep(1u);

		const ScopedLock scopedLock(managerRegistrationRequestLock);

		if (managerRequestSet.find(requestId) == managerRequestSet.end())
			return;
	}

	ocean_assert(false && "This should never happen!");
}

void EventManager::threadRun()
{

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{

#endif // OCEAN_SUPPORT_EXCEPTIONS

		while (!shouldThreadStop())
		{
			TemporaryScopedLock temporalScopedLock(managerLock);

			TemporaryScopedLock temporalModificationScopedLock(managerRegistrationRequestLock);

			for (RegistrationRequests::const_iterator i = managerRegistrationRequests.begin(); i != managerRegistrationRequests.end(); ++i)
			{
				if (i->registerCallback())
					addEventFunction(i->eventTypes(), i->callback());
				else
					removeEventFunction(i->eventTypes(), i->callback());

				ocean_assert(managerRequestSet.find(i->id()) != managerRequestSet.end());
				managerRequestSet.erase(i->id());
			}
			managerRegistrationRequests.clear();

			temporalModificationScopedLock.release();

			if (managerEventQueue.empty())
			{
				// we need to release the lock before we sleep the thread!
				temporalScopedLock.release();
				sleep(1u);
			}
			else
			{
				const EventRef eventObject = managerEventQueue.front();
				ocean_assert(eventObject);

				managerEventQueue.pop();

				temporalScopedLock.release();

				const ScopedLock scopedLock(managerRegistrationRequestLock);

				for (EventCallbacksMap::const_iterator i = managerEventCallbacksMap.begin(); i != managerEventCallbacksMap.end(); ++i)
					if (i->first & eventObject->type())
						i->second(eventObject);
			}
		}

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	}
	catch(...)
	{
		ocean_assert(false && "This should never happen!");
	}

#endif // OCEAN_SUPPORT_EXCEPTIONS

}

void EventManager::addEventFunction(const unsigned int eventTypes, const EventCallback& eventCallback)
{
	// this is a private function, as the lock of this manager must be invoked

	managerEventCallbacksMap[eventTypes].addCallback(eventCallback);
}

void EventManager::removeEventFunction(const unsigned int eventTypes, const EventCallback& eventCallback)
{
	// this is a private function, as the lock of this manager must be invoked

	managerEventCallbacksMap[eventTypes].removeCallback(eventCallback);
}

}
