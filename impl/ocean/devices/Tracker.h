/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_TRACKER_H
#define META_OCEAN_DEVICES_TRACKER_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"
#include "ocean/devices/Measurement.h"

#include "ocean/math/Math.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class Tracker;

/**
 * Definition of a smart object reference for a tracker.
 * @see Tracker.
 * @ingroup devices
 */
typedef SmartDeviceRef<Tracker> TrackerRef;

/**
 * This class implements the base class for all tracker devices.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT Tracker : virtual public Measurement
{
	public:

		/**
		 * Definition of different minor device types.
		 */
		enum TrackerType : uint32_t
		{
			/// Invalid tracker.
			TRACKER_INVALID = MINOR_INVALID,
			/// 3DOF orientation tracker.
			TRACKER_ORIENTATION_3DOF = (1u << 0u),
			/// 3DOF position tracker.
			TRACKER_POSITION_3DOF = (1u << 1u),
			/// 6DOF orientation and position tracker.
			TRACKER_6DOF = TRACKER_ORIENTATION_3DOF | TRACKER_POSITION_3DOF,
			/// 6DOF scene tracker.
			SCENE_TRACKER_6DOF = (1u << 2u) | TRACKER_6DOF,
			/// GPS tracker.
			TRACKER_GPS = (1u << 3u),
			/// Tracker using a magnetic measurements.
			TRACKER_MAGNETIC = (1u << 4u),
			/// Tracker using a visual input for their measurements.
			TRACKER_VISUAL = (1u << 5u),
			/// Tracker allowing to register tracking objects at runtime.
			TRACKER_OBJECT = (1u << 6u)
		};

		/**
		 * Definition of different tracking reference system.
		 * Beware: Both reference systems are mutual inverse.
		 */
		enum ReferenceSystem
		{
			/// Returning tracking values are given in the coordinate system of the device.
			RS_OBJECT_IN_DEVICE,
			/// Returning tracking values are given in the coordinate system of the object.
			RS_DEVICE_IN_OBJECT
		};

		/**
		 * Definition of a tracker frequency in Hz.
		 */
		typedef float Frequency;

		/**
		 * Definition of a sample holding one measurement values of a tracker.
		 */
		class OCEAN_DEVICES_EXPORT TrackerSample : virtual public Measurement::Sample
		{
			public:

				/**
				 * Returns the reference system of the tracker.
				 * @return Tracking reference system
				 */
				inline ReferenceSystem referenceSystem() const;

			protected:

				/**
				 * Creates a new tracker sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Object ids corresponding to different measurement units of one tracker
				 * @param metadata Optional metadata of the new sample
				 */
				TrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Metadata& metadata);

				/**
				 * Creates a new tracker sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Object ids corresponding to different measurement units of one tracker
				 * @param metadata Optional metadata of the new sample
				 */
				TrackerSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Metadata&& metadata);

			protected:

				/// Tracking reference system.
				ReferenceSystem referenceSystem_;
		};

		/**
		 * Definition of a smart object reference for tracker samples.
		 */
		typedef SmartObjectRef<TrackerSample, Sample> TrackerSampleRef;

		/**
		 * Definition of a callback function to subscribe for lost and found tracker events.
		 * The first parameter is the tracker producing the event.<br>
		 * The second parameter determines whether a tracker object has been found (true) or lost (false).
		 * The third parameter hold the ids of the tracker object.
		 * The last parameter hold the event timestamp.
		 */
		typedef Callback<void, const Tracker*, const bool, const ObjectIdSet&, const Timestamp&> TrackerObjectCallback;

		/**
		 * This class manages the lifetime of an event subscription for tracker object events.
		 */
		class OCEAN_DEVICES_EXPORT TrackerObjectEventSubscription
		{
			friend class Tracker;

			public:

				/**
				 * Default constructor for a not active subscription.
				 */
				TrackerObjectEventSubscription() = default;

				/**
				 * Move constructor.
				 * @param trackerObjectEventSubscription The object to be moved
				 */
				inline TrackerObjectEventSubscription(TrackerObjectEventSubscription&& trackerObjectEventSubscription);

				/**
				 * Destructs the subscription object and unsubscribes the object event.
				 */
				inline ~TrackerObjectEventSubscription();

				/**
				 * Makes this subscription object weak so that is does not hold a reference to the actual measurement object.
				 */
				inline void makeWeak();

				/**
				 * Explicitly releases the subscription.
				 */
				void release();

				/**
				 * Returns whether this subscription object holds an active subscription.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

				/**
				 * Replaces the current event subscription with a new event subscription.
				 * @param trackerObjectEventSubscription The subscription object to assign
				 * @return Reference to this object
				 */
				TrackerObjectEventSubscription& operator=(TrackerObjectEventSubscription&& trackerObjectEventSubscription);

			protected:

				/**
				 * Creates an active subscription object.
				 * @param tracker The tracker to which the event subscription belongs
				 * @param subscriptionId The subscription id of the event, must be valid
				 */
				TrackerObjectEventSubscription(const Tracker& tracker, const SubscriptionId subscriptionId);

				/**
				 * Disabled assign operator.
				 * @param trackerObjectEventSubscription The subscription object to assign
				 * @return Reference to this object
				 */
				TrackerObjectEventSubscription& operator=(const TrackerObjectEventSubscription& trackerObjectEventSubscription) = delete;

			protected:

				/// The tracker to which the event subscription belongs.
				TrackerRef tracker_;

				/// The pointer to the tracker object to which the event subscription belongs.
				Tracker* weakTracker_ = nullptr;

				/// The subscription id.
				SubscriptionId subscriptionId_ = invalidSubscriptionId();
		};

	protected:

		/**
		 * Definition of a map mapping subscription ids to tracker object event callback functions.
		 */
		typedef std::unordered_map<SubscriptionId, TrackerObjectCallback> TrackerObjectSubscriptionMap;

	public:

		/**
		 * Returns the frequency of this tracker.
		 * @return The tracker's frequency in Hz, with range (0, infinity), -1 if unknown
		 */
		virtual Frequency frequency() const;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @param objectId The id of the object to be checked, must be valid
		 * @return True, if so
		 */
		virtual bool isObjectTracked(const ObjectId& objectId) const;

		/**
		 * Subscribes a callback event function for tracker object (found or lost) events.
		 * Do not subscribe or unsubscribe from inside an event thread.
		 * @param callback The callback function receiving the event calls, must be valid
		 * @return The resulting subscription object, dispose the object to unsubscribe from the event call
		 */
		[[nodiscard]] TrackerObjectEventSubscription subscribeTrackerObjectEvent(TrackerObjectCallback&& callback);

		/**
		 * Returns the reference coordinate system of this tracker.
		 * Whenever a reference coordinate system is defined, all tracker samples are defined in relation to this reference coordinate system.
		 * @return The tracker's reference coordinate system, invalid if the tracker does not use a reference system
		 */
		[[nodiscard]] virtual HomogenousMatrixD4 reference() const;

		/**
		 * Translates the tracker type to a readable string.
		 * @param trackerType The tracker type to translate
		 * @return The readable string, empty if the tracker type is unknown
		 */
		static std::string translateTrackerType(const TrackerType trackerType);

		/**
		 * Translates the tracker type from a readable string to a value.
		 * @param trackerType The tracker type to translate
		 * @return The translated value
		 */
		static TrackerType translateTrackerType(const std::string& trackerType);

		/**
		 * Definition of a constant as unknown frequency.
		 */
		static constexpr Frequency unknownFrequency();

	protected:

		/**
		 * Creates a new tracker object.
		 * @param name The name of the tracker, must be valid
		 * @param type Major and minor device type of the device, must be valid
		 */
		Tracker(const std::string& name, const DeviceType type);

		/**
		 * Destructs a tracker object.
		 */
		~Tracker() override;

		/**
		 * Posts a new found tracker objects event.
		 * @param objectIds The ids of all objects which have been found recently, nothing will be done if empty
		 * @param timestamp Event timestamp
		 */
		void postFoundTrackerObjects(const ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Posts a new lost tracker objects event.
		 * @param objectIds The ids of all objects which have been lost recently, nothing will be done if empty
		 * @param timestamp Event timestamp
		 */
		void postLostTrackerObjects(const ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Unsubscribes a tracker object event callback function.
		 * @param subscriptionId The id of the event subscription to unsubscribe
		 */
		void unsubscribeTrackerObjectEvent(const SubscriptionId subscriptionId);

		/**
		 * Determines the ids which were not tracked in the previous iteration but tracked in the current iteration
		 * @param previousObjects The ids of all objects tracked in the previous iteration
		 * @param currentObjects The ids of all objects tracked in the current iteration
		 * @return The ids of the objects which have been found in the current iteration
		 */
		static ObjectIdSet determineFoundObjects(const ObjectIdSet& previousObjects, const ObjectIdSet& currentObjects);

		/**
		 * Determines the ids which were tracked in the previous iteration but not tracked in the current iteration
		 * @param previousObjects The ids of all objects tracked in the previous iteration
		 * @param currentObjects The ids of all objects tracked in the current iteration
		 * @return The ids of the objects which have been lost in the current iteration
		 */
		static ObjectIdSet determineLostObjects(const ObjectIdSet& previousObjects, const ObjectIdSet& currentObjects);

	protected:

		/// Map holding all tracker object event subscriptions.
		TrackerObjectSubscriptionMap trackerObjectSubscriptionMap_;

		/// The subscription id of the next event subscription.
		SubscriptionId nextTrackerObjectSubscriptionId_ = SubscriptionId(invalidSubscriptionId() + 1u);
};

inline Tracker::ReferenceSystem Tracker::TrackerSample::referenceSystem() const
{
	return referenceSystem_;
}

inline Tracker::TrackerObjectEventSubscription::TrackerObjectEventSubscription(TrackerObjectEventSubscription&& trackerObjectEventSubscription)
{
	*this = std::move(trackerObjectEventSubscription);
}

inline Tracker::TrackerObjectEventSubscription::~TrackerObjectEventSubscription()
{
	release();
}

inline void Tracker::TrackerObjectEventSubscription::makeWeak()
{
	tracker_.release();
}


inline Tracker::TrackerObjectEventSubscription::operator bool() const
{
	return weakTracker_ != nullptr;
}

constexpr Tracker::Frequency Tracker::unknownFrequency()
{
	return -1.0f;
}

}

}

#endif // META_OCEAN_DEVICES_TRACKER_H
