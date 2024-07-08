/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AC_DEVICE_H
#define META_OCEAN_DEVICES_ARCORE_AC_DEVICE_H

#include "ocean/devices/arcore/ARCore.h"

#include "ocean/devices/Device.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/media/FrameMedium.h"

#include <arcore_c_api.h>

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

/**
 * This class implements a device for the ARCore library.
 * @ingroup devicearcore
 */
class OCEAN_DEVICES_ARCORE_EXPORT ACDevice :
	virtual public Device,
	virtual public VisualTracker
{
	friend class ACFactory;
	friend class ARSessionManager;

	protected:

		/**
		 * Definition of individual capabilities.
		 */
		enum TrackerCapabilities : uint32_t
		{
			/// An invalid capability.
			TC_INVALID = 0u,
			/// The tracker provides basic SLAM.
			TC_SLAM = 1u << 0u,
			/// The tracker provide plane detection.
			TC_PLANE_DETECTION = 1u << 1u,
			/// The tracker provides depth information.
			TC_DEPTH = 1u << 2u
		};

	public:

		/**
		 * Returns the name of the owner library.
		 * @see Device::library().
		 */
		const std::string& library() const override;

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Pauses the device.
		 * @see Device::pause().
		 */
		bool pause() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @see VisualTracker::setInput().
		 */
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Returns the capabilities of the tracker necessary for this device.
		 * @return The tracker capabilities
		 */
		inline TrackerCapabilities trackerCapabilities() const;

	protected:

		/**
		 * Creates a new device by is name.
		 * @param trackerCapabilities The capabilities of the tracker necessary for this device
		 * @param name The name of the device
		 * @param type Major and minor device type of the device
		 */
		ACDevice(const TrackerCapabilities trackerCapabilities, const std::string& name, const DeviceType type);

		/**
		 * Destructs this tracker.
		 */
		~ACDevice() override;

	protected:

		/// The capabilities of the tracker for this device.
		TrackerCapabilities trackerCapabilities_ = TC_INVALID;

		/// True, if this tracker has been registered with the session mananager.
		bool hasBeenRegistered_ = false;

		/// The id of the world object (the world coodinate system).
		ObjectId worldObjectId_ = invalidObjectId();

		/// True, if the world is currently tracked.
		bool worldIsTracked_ = false;
};

inline ACDevice::TrackerCapabilities ACDevice::trackerCapabilities() const
{
	return trackerCapabilities_;
}

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AC_DEVICE_H
