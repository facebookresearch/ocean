/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_PATTERN_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_PATTERN_TRACKER_6DOF_H

#include "ocean/devices/pattern/Pattern.h"
#include "ocean/devices/pattern/PatternDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/OrientationTracker3DOF.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/tracking/pattern/PatternTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

/**
 * This class implements a Pattern feature-based tracker.
 * @ingroup devicespattern
 */
class OCEAN_DEVICES_PATTERN_EXPORT PatternTracker6DOF :
	virtual public PatternDevice,
	virtual public Tracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker,
	protected Thread
{
	friend class PatternFactory;

	public:

		/**
		 * Adds a new tracking pattern.
		 * For this pattern feature based tracker the pattern must be the url of an image.
		 * @see ObjectTracker::registerObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

		/**
		 * Returns whether this device is active.
		 * @see Devices::isStarted().
		 */
		 bool isStarted() const override;

		/**
		 * Starts the device.
		 * @see Device::start().
		 */
		bool start() override;

		/**
		 * Stops the device.
		 * @see Device::stop().
		 */
		bool stop() override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Sets a parameter  for this tracker.
		 * Supprted parameters 'noFrameToFrameTracking', 'noDownsamplingOnAndroid'.
		 * @see Device::setParameter().
		 */
		bool setParameter(const std::string& parameter, const Value& value) override;

		/**
		 * Returns a parameter of this device.
		 * Supprted parameters 'noFrameToFrameTracking', 'noDownsamplingOnAndroid'.
		 * @see Device::parameter().
		 */
		bool parameter(const std::string& parameter, Value& value) override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNamePatternTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypePatternTracker6DOF();

	private:

		/**
		 * Creates a new Pattern feature based 6DOF tracker object.
		 */
		PatternTracker6DOF();

		/**
		 * Destructs an Pattern feature based 6DOF tracker object.
		 */
		~PatternTracker6DOF() override;

		/**
		 * Ensures that the tracker exists, if the tracker does not yet exist, it will be created.
		 */
		void ensureTrackerExists();

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// Frame timestamp.
		Timestamp frameTimestamp_;

		/// Holds the 6DOF tracker object.
		Tracking::VisualTrackerRef visualTracker_;

		/// Set holding all visible marker from the most recent frame.
		ObjectIdSet visiblePatterns_;

		/// The 3DOF orientation tracker which is used to support the tracker.
		OrientationTracker3DOFRef orientationTracker3DOF_;

		/// The mapper between internal and external object ids.
		ObjectMapper<unsigned int> objectIdMapper_;

		/// True, to skip frame-to-frame tracking.
		bool noFrameToFrameTracking_ = false;

		/// True, to avoid downsampling the input image on Android devices.
		bool noDownsamplingOnAndroid_ = false;
};

inline std::string PatternTracker6DOF::deviceNamePatternTracker6DOF()
{
	return std::string("Pattern 6DOF Tracker");
}

inline PatternTracker6DOF::DeviceType PatternTracker6DOF::deviceTypePatternTracker6DOF()
{
	return PatternTracker6DOF::DeviceType(PatternTracker6DOF::deviceTypeTracker6DOF(), TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_PATTERN_TRACKER_6DOF_H
