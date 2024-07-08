/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/ApplicationInterface.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

const ApplicationInterface::EventId ApplicationInterface::invalidEventId(0xFFFFFFFF);

ApplicationInterface::~ApplicationInterface()
{
	ocean_assert(!contentAddCallback && "The add content callback function must be released before.");
	ocean_assert(!contentRemoveCallback && "The remove content callback function must be released before.");
	ocean_assert(contentEventCallbacks.isEmpty() && "The event content callback functions must be removed before.");
}

ApplicationInterface::EventIds ApplicationInterface::addContent(const StringVector& content)
{
	TemporaryScopedLock scopedLock(lock);

	const ContentCallback callback(contentAddCallback);

	scopedLock.release();

	if (contentAddCallback)
		return contentAddCallback(content);

	Log::warning() << "The application does not support content adding!";
	return EventIds();
}

ApplicationInterface::EventIds ApplicationInterface::removeContent(const StringVector& content)
{
	TemporaryScopedLock scopedLock(lock);

	const ContentCallback callback(contentRemoveCallback);

	scopedLock.release();

	if (callback)
		return callback(content);

	Log::warning() << "The application does not support content removing!";
	return EventIds();
}

void ApplicationInterface::contentAdded(const EventId eventId, const bool state)
{
	contentEventCallbacks(eventId, true, state);
}

void ApplicationInterface::contentRemoved(const EventId eventId, const bool state)
{
	contentEventCallbacks(eventId, false, state);
}

void ApplicationInterface::setContentAddCallbackFunction(const ContentCallback& callback)
{
	const ScopedLock scopedLock(lock);
	contentAddCallback = callback;
}

void ApplicationInterface::setContentRemoveCallbackFunction(const ContentCallback& callback)
{
	const ScopedLock scopedLock(lock);
	contentRemoveCallback = callback;
}

void ApplicationInterface::addEventCallbackFunction(const EventCallback& callback)
{
	contentEventCallbacks.addCallback(callback);
}

void ApplicationInterface::removeEventCallbackFunction(const EventCallback& callback)
{
	contentEventCallbacks.removeCallback(callback);
}

}
