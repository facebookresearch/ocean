/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_IOS_MOTION_MANAGER_H
#define META_OCEAN_DEVICES_IOS_MOTION_MANAGER_H

#include "ocean/devices/ios/IOS.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <CoreMotion/CoreMotion.h>

namespace Ocean
{

namespace Devices
{

namespace IOS
{

/**
 * This class implements a simple wrapper for the CMMotionManager object as an application must not create more than one instance of a CMMotionManager object.
 * This class is implemented as a singleton and the actual CMMotionManager object can be set explicitly (if already existing in the application), or will be created automatically.<br>
 * The manager is thread-safe.
 * @ingroup devicesios
 */
class OCEAN_DEVICES_IOS_EXPORT MotionManager : public Singleton<MotionManager>
{
	friend class Singleton<MotionManager>;

	public:

		/**
		 * Definition of a listener id for a registered callbacks.
		 */
		typedef unsigned int ListenerId;

		/**
		 * Definition of a callback function to subscribe for new device motion samples.
		 */
		typedef Callback<void, CMDeviceMotion*> DeviceMotionCallback;

	protected:

		/**
		 * Type of an unordered map mapping listener ids to callback functions.
		 */
		typedef std::unordered_map<ListenerId, DeviceMotionCallback> DeviceMotionListenerMap;

	public:

		/**
		 * Returns the instance of the CMMotionManager object hold by this singleton.
		 * In the case the object hasn't been set explicitly an object will be created the first time this function is called.
		 * @return The CMMotionManager object
		 * @see setObject();
		 */
		CMMotionManager* object();

		/**
		 * Sets the instance of the CMMotionManager object explicitly as the application holds the instance already.
		 * Do not call this function twice.
		 * @param object The CMMotionManager object to be set explicitly, must be valid
		 * @return True, if this singleton does not hold an own object already and if the provided object is valids
		 */
		bool setObject(CMMotionManager* object);

		/**
		 * Register a new listener.
		 * @param callback Callback function that occurs each time a new DeviceMotion sample is provided
		 * @return Unique id for the registered callback
		 */
		ListenerId addListener(const DeviceMotionCallback& callback);

		/**
		 * Unregister a given listener.
		 * @param listenerId Id returned from addListener, must be valid
		 */
		void removeListener(const ListenerId listenerId);

		/**
		 * Returns an invalid listener id.
		 * @return Invalid listener id
		 */
		static constexpr inline ListenerId invalidListenerId();

	protected:

		/**
		 * Protected default constructor.
		 */
		MotionManager() = default;

		/**
		 * Internally starts the main DeviceMotion callback with CMMotionMap.
		 */
		void start();

	protected:

		/// The instance of the CMMotionManager object this singleton is responsible for.
		CMMotionManager* motionManager_ = nullptr;

		/// Map storing the set of registered listeners.
		DeviceMotionListenerMap deviceMotionListenerMap_;

		/// Counter used to produce a new id when addListener is called.
		unsigned int nextListenerId_ = invalidListenerId();

		/// The manager's lock.
		Lock lock_;
};

inline constexpr MotionManager::ListenerId MotionManager::invalidListenerId()
{
	return 0u;
}

}

}

}

#endif // META_OCEAN_DEVICES_IOS_MOTION_MANAGER_H
