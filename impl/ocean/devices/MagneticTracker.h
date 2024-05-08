/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MAGNETIC_TRACKER_H
#define META_OCEAN_DEVICES_MAGNETIC_TRACKER_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Tracker.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class MagneticTracker;

/**
 * Definition of a smart object reference for a magnetic tracker.
 * @see MagneticTracker.
 * @ingroup devices
 */
typedef SmartDeviceRef<MagneticTracker> MagneticTrackerRef;

/**
 * This class is the base class for all tracker using a magnetic filed as input measurements.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT MagneticTracker : virtual public Tracker
{
	public:

		/**
		 * Resets the magnetic field.
		 * @return True, if succeeded
		 * @exception NotSupportedException is thrown if this function is not supported.
		 */
		virtual bool resetMagneticField();

	protected:

		/**
		 * Creates a new magnetic tracker object.
		 * @param name The name of the tracker
		 * @param type Major and minor device type of the device
		 */
		MagneticTracker(const std::string& name, const DeviceType type);

		/**
		 * Destructs a magnetic tracker object.
		 */
		virtual ~MagneticTracker();
};

}

}

#endif // META_OCEAN_DEVICES_MAGNETIC_TRACKER_H
