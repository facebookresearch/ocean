/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_VISUAL_TRACKER_H
#define META_OCEAN_DEVICES_VISUAL_TRACKER_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Tracker.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/MediumRef.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class VisualTracker;

/**
 * Definition of a smart object reference for a visual tracker.
 * @see VisualTracker.
 * @ingroup devices
 */
typedef SmartDeviceRef<VisualTracker> VisualTrackerRef;

/**
 * This class is the base class for all tracker using visual input to create the tracking results.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT VisualTracker : virtual public Tracker
{
	public:

		/**
		 * Returns the medium objects of this tracker used as visual input.
		 * The majority of tracker will be based on mono camera inputs, while some tracker may use stereo or multi-view vision.
		 * @return Visual input media object
		 */
		inline Media::FrameMediumRefs input() const;

		/**
		 * Sets the mono visual input of this tracker.
		 * @param frameMedium The visual input medium object to set
		 */
		inline void setInput(const Media::FrameMediumRef& frameMedium);

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @param frameMediums The visual input medium objects to set
		 */
		virtual void setInput(Media::FrameMediumRefs&& frameMediums);

	protected:

		/**
		 * Creates a new visual tracker object.
		 * @param name The name of the tracker
		 * @param type Major and minor device type of the device
		 */
		VisualTracker(const std::string& name, const DeviceType type);

		/**
		 * Destructs a visual tracker object.
		 */
		~VisualTracker() override;

	protected:

		/// Visual input media objects.
		Media::FrameMediumRefs frameMediums_;
};

inline Media::FrameMediumRefs VisualTracker::input() const
{
	const ScopedLock scopedLock(deviceLock);

	return frameMediums_;
}

inline void VisualTracker::setInput(const Media::FrameMediumRef& frameMedium)
{
	setInput(Media::FrameMediumRefs(1, frameMedium));
}

}

}

#endif // META_OCEAN_DEVICES_VISUAL_TRACKER_H
