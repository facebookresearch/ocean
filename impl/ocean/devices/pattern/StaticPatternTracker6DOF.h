/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_PATTERN_STATIC_PATTERN_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_PATTERN_STATIC_PATTERN_TRACKER_6DOF_H

#include "ocean/devices/pattern/Pattern.h"
#include "ocean/devices/pattern/PatternDevice.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/math/HomogenousMatrix4.h"

#include <unordered_map>

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

/**
 * This class implements a static Pattern feature-based tracker.
 * @ingroup devicespattern
 */
class OCEAN_DEVICES_PATTERN_EXPORT StaticPatternTracker6DOF :
	virtual public PatternDevice,
	virtual public Tracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker
{
	friend class PatternFactory;

	protected:

		/**
		 * Definition of a sorted map combining timestamp with transformations.
		 */
		typedef std::map<Timestamp, HomogenousMatrix4> TransformationMap;

		/**
		 * This class stores necessary information for one pattern.
		 */
		class PatternTransformations
		{
			public:

				/**
				 * Creates a new object.
				 * @param maximalDistance The distance between pattern and camera in which the Pattern tracker can be trusted, in meter, with range (0, infinity)
				 */
				explicit inline PatternTransformations(const Scalar maximalDistance);

				/**
				 * Adds a new transformation between camera and pattern.
				 * @param pattern_T_camera The new transformation to be added, must be valid
				 * @param timestamp The timestamp of the transformation
				 * @param maximalNumberForAlignment The maximal number of transformations that kept stored to determine the aligned transformation between world and pattern, with range [1, infinity)
				 * @param maximalIntervalForAlignment The maximal time interval of transformations that kept stored to determine the aligned transformation between world and pattern, in seconds, with range (0, infinity)
				 * @return True, if the transformation has been added; False, if the transformation was not trusted
				 */
				bool addTransformation(const HomogenousMatrix4& pattern_T_camera, const Timestamp& timestamp, const size_t maximalNumberForAlignment, const double maximalIntervalForAlignment);

				/**
				 * Returns the current transformation between world and pattern, may be invalid.
				 * @return Current transformation
				 */
				inline HomogenousMatrix4& pattern_T_world();

				/**
				 * Returns the transformations associated with this pattern.
				 * @return The transformations
				 */
				inline TransformationMap& transformations();

			protected:

				/// The distance between pattern and camera in which the Pattern tracker can be trusted, in meter, with range (0, infinity).
				Scalar maximalDistance_;

				/// The most current transformation between world and pattern, invalid if unknown.
				HomogenousMatrix4 pattern_T_world_;

				/// The recent transformations associated with this pattern.
				TransformationMap transformationMap_;
		};

		/**
		 * Definition of an unsorted map combining object ids with pattern transformations.
		 */
		typedef std::unordered_map<ObjectId, PatternTransformations> PatternTransformationsMap;

	public:

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @see VisualTracker::setInput().
		 */
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

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
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameStaticPatternTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeStaticPatternTracker6DOF();

	private:

		/**
		 * Creates a new Static Pattern feature based 6DOF tracker object.
		 */
		StaticPatternTracker6DOF();

		/**
		 * Destructs an Static Pattern feature based 6DOF tracker object.
		 */
		~StaticPatternTracker6DOF() override;

		/**
		 * Event function for new tracking samples from the pattern tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from pattern tracker
		 */
		void onPatternTrackerSample(const Measurement* measurement, const SampleRef& sample);

		/**
		 * Event function for new tracking samples from the world tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from world tracker
		 */
		void onWorldTrackerSample(const Measurement* measurement, const SampleRef& sample);

		/**
		 * Event function for new tracking object event from the pattern tracker.
		 * @param tracker The sender of the event, must be valid
		 * @param found True, if the object ids were found; False if the object ids were lost
		 * @param objectIds The ids of the object which are found or lost, at least one
		 * @param timestamp The timestamp of the event
		 */
		void onPatternTrackerObject(const Tracker* tracker, const bool found, const ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Event function for new tracking object event from the world tracker.
		 * @param tracker The sender of the event, must be valid
		 * @param found True, if the object ids were found; False if the object ids were lost
		 * @param objectIds The ids of the object which are found or lost, at least one
		 * @param timestamp The timestamp of the event
		 */
		void onWorldTrackerObject(const Tracker* tracker, const bool found, const ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Reports all pattern poses without aligning the poses with the world tracker.
		 * @param timestamp The current timestamp
		 */
		void reportNotAlignedPoses(const Timestamp& timestamp);

		/**
		 * Reports all pattern poses by aligning them with the world tracker.
		 * @param timestamp The current timestamp
		 */
		void reportAlignedPoses(const Timestamp& timestamp);

	private:

		/// The 6-DOF pattern tracker.
		Tracker6DOFRef patternTracker_;

		/// The 6-DOF world tracker.
		Tracker6DOFRef worldTracker_;

		/// The subscription object for samples events from the pattern tracker.
		SampleEventSubscription patternTrackerSampleEventSubscription_;

		/// The subscription object for samples events from the world tracker.
		SampleEventSubscription worldTrackerSampleEventSubscription_;

		/// The subscription object for tracker object events from the pattern tracker.
		TrackerObjectEventSubscription patternTrackerObjectSubscription_;

		/// The subscription object for tracker object events from the world tracker.
		TrackerObjectEventSubscription worldTrackerObjectSubscription_;

		/// The ids of all objects currently connected with a world pose.
		ObjectIdSet worldTrackedPatternObjects_;

		/// The ids of all objects detected and currently actively tracked.
		ObjectIdSet foundPatternTrackerObjects_;

		/// The ids of all objects currently not actively tracked.
		ObjectIdSet lostPatternTrackerObjects_;

		/// Individual transformations (the history of recent poses) of all (pattern) objects.
		PatternTransformationsMap patternTrackerTransformationsMap_;

		/// The transformation map (the history of recent poses) of the world tracker.
		TransformationMap worldTrackerTransformationMap_;

		/// The look for all transformation maps.
		Lock sampleMapLock_;

		/// The maximal number of transformations that kept stored to determine the aligned transformation between world and pattern, with range [1, infinity)
		size_t maximalNumberForAlignment_ = 5;

		/// The maximal time interval of transformations that kept stored to determine the aligned transformation between world and pattern, in seconds, with range (0, infinity)
		double maximalIntervalForAlignment_ = 10.0;

		/// The mapper between internal and external object ids.
		ObjectMapper<ObjectId> objectIdMapper_;
};

inline StaticPatternTracker6DOF::PatternTransformations::PatternTransformations(const Scalar maximalDistance) :
	maximalDistance_(maximalDistance),
	pattern_T_world_(false)
{
	ocean_assert(maximalDistance_ > Numeric::eps());
}

inline HomogenousMatrix4& StaticPatternTracker6DOF::PatternTransformations::pattern_T_world()
{
	return pattern_T_world_;
}

inline StaticPatternTracker6DOF::TransformationMap& StaticPatternTracker6DOF::PatternTransformations::transformations()
{
	return transformationMap_;
}

inline std::string StaticPatternTracker6DOF::deviceNameStaticPatternTracker6DOF()
{
	return std::string("Static Pattern 6DOF Tracker");
}

inline StaticPatternTracker6DOF::DeviceType StaticPatternTracker6DOF::deviceTypeStaticPatternTracker6DOF()
{
	return StaticPatternTracker6DOF::DeviceType(StaticPatternTracker6DOF::deviceTypeTracker6DOF(), TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_PATTERN_STATIC_PATTERN_TRACKER_6DOF_H
