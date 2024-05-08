/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_EVENT_H
#define META_OCEAN_BASE_EVENT_H

#include "ocean/base/Base.h"
#include "ocean/base/Callback.h"
#include "ocean/base/ObjectRef.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include <queue>

namespace Ocean
{

// Forward declaration.
class Event;

/**
 * Definition of an object reference holding an event object.
 * @see Event.
 * @ingroup base
 */
typedef ObjectRef<Event> EventRef;

/**
 * This class implements the base class for all event classes.
 * Events can be used to share, to forward or to distribute arbitrary information from one component to another component while dependencies can be avoided.<br>
 * Further, events may be used to synchronize asynchronous tasks.<br>
 *
 * @see EventRef, EventManager.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Event
{
	public:

		/**
		 * Destructs an event object.
		 */
		virtual ~Event();

		/**
		 * Returns the type of this event.
		 * @return Event type
		 */
		inline unsigned int type() const;

		/**
		 * Returns the sub-type of this event.
		 * @return Event sub-type
		 */
		inline unsigned int subtype() const;

		/**
		 * Casts this event to an event with specified type.
		 * @return Casted event object
		 * @tparam T Data type of the resulting event
		 */
		template <typename T> const T& cast() const;

		/**
		 * Casts this event to an event with specified type.
		 * @return Casted event object
		 * @tparam T Data type of the resulting event
		 */
		template <typename T> T& cast();

	protected:

		/**
		 * Creates a new event object.
		 * @param type The type of the event
		 * @param subtype The subtype of the event
		 */
		inline Event(const unsigned int type, const unsigned int subtype);

	protected:

		/// The type of the event.
		unsigned int eventType;

		/// The sub-type of the event.
		unsigned int eventSubtype;
};

/**
 * This class implements a scoped event.
 * A scoped event stores three individual events.<br>
 * One event is invoked when the scope is entered.<br>
 * One event is invoked when the scope is left.<br>
 * One optional event is invoked when the scope is paused.
 * @tparam T Event type that will be handled
 * @see Event.
 * @ingroup base
 */
template <typename T>
class ScopedEvent
{
	public:

		/**
		 * Definition of a callback function providing the event as single parameter.
		 */
		typedef Callback<void, const T&> EventCallback;

		/**
		 * Definition of a list of callback functions.
		 */
		typedef Callbacks<EventCallback> EventCallbacks;

	public:

		/**
		 * Creates a new scoped event object.
		 * @param enterEvent Event that will be invoked during the creation of this event
		 * @param breakEvent Event that will be invoked when this object is disposed as long as the leave() function hasn't been called
		 * @param leaveEvent Event that will be invoked when this object is disposed as long as the leave() function has been called
		 * @param callbacks List of callback functions to those the events will be sent
		 */
		inline ScopedEvent(const T& enterEvent, T& breakEvent, T& leaveEvent, const EventCallbacks& callbacks);

		/**
		 * Destructs this scoped event object.
		 */
		inline ~ScopedEvent();

		/**
		 * Activates the leave event of this object.
		 * If the leave event is activated, the leave event will be invoked instead of the break event in the moment this object is disposed.
		 */
		inline void leave() const;

		/**
		 * Returns the break-event of this object, nothing else will be done.
		 * @return Event object
		 */
		inline const T& breakEvent() const;

		/**
		 * Returns the break-event of this object, nothing else will be done.
		 * @return Event object
		 */
		inline T& breakEvent();

		/**
		 * Returns the leave-event of this object, nothing else will be done.
		 * @return Event object
		 */
		inline const T& leaveEvent() const;

		/**
		 * Returns the leave-event of this object, nothing else will be done.
		 * @return Event object
		 */
		inline T& leaveEvent();

	protected:

		/// Break event which will be invoked if the leave() function has not been called before this object is disposed.
		T& scopedBreakEvent;

		/// Leave event which will be invoked if the leave() function has been called before this object is disposed.
		T& scopedLeaveEvent;

		/// A list of callback functions to those the events will be sent.
		const EventCallbacks& scopedCallbacks;

		/// Leave statement of this object.
		mutable bool scopedEventLeave;
};

/**
 * This class implements a central manager for events that can be pushed and received from/in individual components of an application or a software package.
 * The manager has an own thread distributing the events.
 * @see Event.
 * @ingroup base
 */
class EventManager :
	public Singleton<EventManager>,
	public Thread
{
	friend class Singleton<EventManager>;

	public:

		/**
		 * Definition of a callback function for events.
		 */
		typedef Callback<void, const EventRef&> EventCallback;

	protected:

		/**
		 * Definition of a set of callback functions.
		 */
		typedef Callbacks<EventCallback> EventCallbacks;

		/**
		 * Definition of a map mapping event types to callback functions.
		 */
		typedef std::map<unsigned int, EventCallbacks> EventCallbacksMap;

		/**
		 * Definition of a class encapsulating a registration request.
		 */
		class RegistrationRequest
		{
			public:

				/**
				 * Creates a new registration request.
				 * @param registerCallback True, to register the callback function; False, to unregister the callback function
				 * @param id The registration request id of the new object
				 * @param eventTypes The event type(s) for which the callback function will be registered or unregistered
				 * @param callback The callback function
				 */
				inline RegistrationRequest(const bool registerCallback, const unsigned int id, const unsigned int eventTypes, const EventCallback& callback);

				/**
				 * Returns whether the request intends an register or unregister execution.
				 * @return True, to register the callback function; False, to unregister the callback function
				 */
				inline bool registerCallback() const;

				/**
				 * Returns the registration request id of this object.
				 * @return The request id
				 */
				inline unsigned int id() const;

				/**
				 * Returns the event types for which the callback function will be registered or unregistered
				 * @return The event types
				 */
				inline unsigned int eventTypes() const;

				/**
				 * Returns the callback function of this request object.
				 * @return The callback function
				 */
				inline const EventCallback& callback() const;

			protected:

				/// True, to register the callback function; False, to unregister the callback function.
				bool requestRegisterCallback;

				/// The registration request id of the new object.
				unsigned int requestId;

				/// The event types for which the callback function will be registered or unregistered.
				unsigned int requestEventTypes;

				/// The callback function of this request.
				EventCallback requestCallback;
		};

		/**
		 * Definition of a vector holding registration requests.
		 */
		typedef std::vector<RegistrationRequest> RegistrationRequests;

		/**
		 * Definition of a queue holding events.
		 */
		typedef std::queue<EventRef> EventQueue;

	public:

		/**
		 * Pushes a new event to the event queue.
		 * @param eventObject The new event to push
		 */
		void pushEvent(const EventRef& eventObject);

		/**
		 * Registers a callback function for a specific type or types of events.
		 * Each registered callback function must be unregistered later.
		 * @param eventTypes The type(s) of events which will be forwarded to the provided callback function
		 * @param eventCallback The callback function which will receive the events with the defined type
		 * @see unregisterEventFunction().
		 */
		void registerEventFunction(const unsigned int eventTypes, const EventCallback& eventCallback);

		/**
		 * Unregisters a callback function for a specific type or types of events.
		 * The callback function must have been registered before.
		 * @param eventTypes The type(s) of events which have been registered
		 * @param eventCallback The callback function which has been registered
		 * @see registerEventFunction().
		 */
		void unregisterEventFunction(const unsigned int eventTypes, const EventCallback& eventCallback);

	protected:

		/**
		 * Creates a new event manager object.
		 */
		EventManager();

		/**
		 * Destructs an event manger object.
		 */
		~EventManager();

		/**
		 * The internal thread function.
		 * @see Thread::threadRun().
		 */
		virtual void threadRun();

	private:

		/**
		 * Adds an event function.
		 * @param eventTypes The type(s) of events which will be forwarded to the provided callback function
		 * @param eventCallback The callback function which will receive the events with the defined type
		 */
		void addEventFunction(const unsigned int eventTypes, const EventCallback& eventCallback);

		/**
		 * Removes an event function.
		 * @param eventTypes The type(s) of events which have been added
		 * @param eventCallback The callback function which has been added
		 */
		void removeEventFunction(const unsigned int eventTypes, const EventCallback& eventCallback);

	protected:

		/// The queue of events waiting to be distributed.
		EventQueue managerEventQueue;

		/// The map of callback functions.
		EventCallbacksMap managerEventCallbacksMap;

		/// The registration requests which are used to modify the map of callback functions.
		RegistrationRequests managerRegistrationRequests;

		/// The counter for registration requests.
		unsigned int managerRequestCounter;

		/// The set of pending registration requests.
		IndexSet32 managerRequestSet;

		/// The lock for the entire manager.
		Lock managerLock;

		/// The lock for the registration requests.
		Lock managerRegistrationRequestLock;
};

inline Event::Event(const unsigned int type, const unsigned int subtype) :
	eventType(type),
	eventSubtype(subtype)
{
	// nothing to do here
}

inline unsigned int Event::type() const
{
	return eventType;
}

inline unsigned int Event::subtype() const
{
	return eventSubtype;
}

template <typename T>
const T& Event::cast() const
{
	ocean_assert(dynamic_cast<const T*>(this));
	return dynamic_cast<const T&>(*this);
}

template <typename T>
T& Event::cast()
{
	ocean_assert(dynamic_cast<const T*>(this));
	return dynamic_cast<T&>(*this);
}

template <typename T>
inline ScopedEvent<T>::ScopedEvent(const T& enterEvent, T& breakEvent, T& leaveEvent, const EventCallbacks& callbacks) :
	scopedBreakEvent(breakEvent),
	scopedLeaveEvent(leaveEvent),
	scopedCallbacks(callbacks),
	scopedEventLeave(false)
{
	scopedCallbacks(enterEvent);
}

template <typename T>
inline ScopedEvent<T>::~ScopedEvent()
{
	if (scopedEventLeave)
		scopedCallbacks(scopedLeaveEvent);
	else
		scopedCallbacks(scopedBreakEvent);
}

template <typename T>
inline void ScopedEvent<T>::leave() const
{
	scopedEventLeave = true;
}

template <typename T>
inline const T& ScopedEvent<T>::breakEvent() const
{
	return scopedBreakEvent;
}

template <typename T>
inline T& ScopedEvent<T>::breakEvent()
{
	return scopedBreakEvent;
}

template <typename T>
inline const T& ScopedEvent<T>::leaveEvent() const
{
	return scopedLeaveEvent;
}

template <typename T>
inline T& ScopedEvent<T>::leaveEvent()
{
	return scopedLeaveEvent;
}

inline EventManager::RegistrationRequest::RegistrationRequest(const bool registerCallback, const unsigned int id, const unsigned int eventTypes, const EventCallback& callback) :
	requestRegisterCallback(registerCallback),
	requestId(id),
	requestEventTypes(eventTypes),
	requestCallback(callback)
{
	// nothing to do here
}

inline bool EventManager::RegistrationRequest::registerCallback() const
{
	return requestRegisterCallback;
}

inline unsigned int EventManager::RegistrationRequest::id() const
{
	return requestId;
}

inline unsigned int EventManager::RegistrationRequest::eventTypes() const
{
	return requestEventTypes;
}

inline const EventManager::EventCallback& EventManager::RegistrationRequest::callback() const
{
	return requestCallback;
}

}

#endif // META_OCEAN_BASE_EVENT_H
