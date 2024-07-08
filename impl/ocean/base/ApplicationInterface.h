/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_APPLICATION_INTERFACE_H
#define META_OCEAN_BASE_APPLICATION_INTERFACE_H

#include "ocean/base/Base.h"
#include "ocean/base/Callback.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <vector>

namespace Ocean
{

/**
 * This class implements a very light application interface.
 * The interface can be used to access basic application functionalities.<br>
 * Define the callback function of each feature your application supports.<br>
 * This interface is implemented as a singleton.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT ApplicationInterface : public Singleton<ApplicationInterface>
{
	friend class Singleton<ApplicationInterface>;

	public:

		/**
		 * Definition of an event id.
		 */
		typedef unsigned int EventId;

		/**
		 * Definition of an invalid event id.
		 */
		static const EventId invalidEventId;

		/**
		 * Definition of a vector holding event ids.
		 */
		typedef std::vector<EventId> EventIds;

		/**
		 * Definition of a vector holding strings.
		 */
		typedef std::vector<std::string> StringVector;

		/**
		 * Definition of a content add or remove callback function.
		 * The return parameter is the corresponding event id.<br>
		 * The first parameter holds the content to be added.<br>
		 */
		typedef Callback<EventIds, const StringVector&> ContentCallback;

		/**
		 * Definition of a content event callback function.
		 * The first parameter hold the event id.<br>
		 * The second parameter defines whether the event has been loaded (true) or unloaded (false)
		 * The third parameter defined the event state.<br>
		 */
		typedef Callback<void, EventId, bool, bool> EventCallback;

	private:

		/**
		 * Definition of an vector holding event callbacks.
		 */
		typedef Callbacks<EventCallback> EventCallbacks;

	public:

		/**
		 * Tells the application to add content.
		 * @param content Content to be loaded (may be e.g. urls of files)
		 * @return Corresponding event ids, one for each file
		 */
		EventIds addContent(const StringVector& content);

		/**
		 * Tells the application to close all existing files.
		 * @param content Content to be unloaded (may be e.g. urls of files), if this parameter is empty the entire content will be removed
		 * @return Corresponding event ids, one for each file
		 */
		EventIds removeContent(const StringVector& content);

		/**
		 * Event function for added content.
		 * @param eventId Event id of the added content
		 * @param state State of the add event
		 */
		void contentAdded(const EventId eventId, const bool state);

		/**
		 * Event function for removed content.
		 * @param eventId Event id of the removed content
		 * @param state State of the remove event
		 */
		void contentRemoved(const EventId eventId, const bool state);

		/**
		 * Sets or replaces the add content callback function.
		 * @param callback Callback function to be set
		 */
		void setContentAddCallbackFunction(const ContentCallback& callback);

		/**
		 * Sets or replaces the remove content callback function.
		 * @param callback Callback function to be set
		 */
		void setContentRemoveCallbackFunction(const ContentCallback& callback);

		/**
		 * Adds an event callback function.
		 * @param callback Callback to be added
		 */
		void addEventCallbackFunction(const EventCallback& callback);

		/**
		 * Removes an event callback function.
		 * @param callback Callback to be removed
		 */
		void removeEventCallbackFunction(const EventCallback& callback);

	protected:

		/**
		 * Destructs an application interface object.
		 */
		virtual ~ApplicationInterface();

	private:

		/// Content add callback.
		ContentCallback contentAddCallback;

		/// Content remove callback.
		ContentCallback contentRemoveCallback;

		/// Event callbacks
		EventCallbacks contentEventCallbacks;

		/// Interface lock.
		Lock lock;
};

}

#endif // META_OCEAN_BASE_APPLICATION_INTERFACE_H
