/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_OBJECT_TRACKER_H
#define META_OCEAN_DEVICES_OBJECT_TRACKER_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Tracker.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class ObjectTracker;

/**
 * Definition of a smart object reference for an ObjectTracker.
 * @see ObjectTracker.
 * @ingroup devices
 */
typedef SmartDeviceRef<ObjectTracker> ObjectTrackerRef;

/**
 * This class is the base class for all tracker allowing to track an object or location.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT ObjectTracker : virtual public Tracker
{
	public:

		/**
		 * Registers (adds) a new object to be tracked.
		 * The object can be e.g., a unique mark, an image, a feature map, or a location.
		 * @param description The information describing the object, e.g., a filename, or an id, or a location
		 * @param dimension The dimension of the object if known, e.g. a 1D or 2D sizeof of a marker, can be zero if unknown
		 * @return Object id associated with the defined object, an invalid object id if the function failed
		 * @see unregisterObject().
		 */
		virtual ObjectId registerObject(const std::string& description, const Vector3& dimension = Vector3(0, 0, 0));

		/**
		 * Unregisters (removes) a previously registered object.
		 * @param objectId Unique object id of the object to be removed
		 * @return True, if succeeded
		 * @see registerObject().
		 */
		virtual bool unregisterObject(const ObjectId objectId);

	protected:

		/**
		 * Creates a new object tracker object.
		 * @param name The name of the tracker
		 * @param type Major and minor device type of the device
		 */
		ObjectTracker(const std::string& name, const DeviceType type);

		/**
		 * Destructs an object tracker object.
		 */
		~ObjectTracker() override;
};

}

}

#endif // META_OCEAN_DEVICES_OBJECT_TRACKER_H
