/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MEASUREMENT_H
#define META_OCEAN_DEVICES_MEASUREMENT_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/DeviceRef.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Value.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class Measurement;

/**
 * Definition of a smart object reference for a measurement.
 * @see Measurement.
 * @ingroup devices
 */
typedef SmartDeviceRef<Measurement> MeasurementRef;

/**
 * This class implements the base class for all devices providing measurement samples.
 * Each measurement holds a container holding several most recent samples provided by the device.<br>
 * Thus, depending on the number of stored samples a specific sample can be requested.<br>
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT Measurement : virtual public Device
{
	public:

		/**
		 * Definition of an object id.
		 */
		typedef unsigned int ObjectId;

		/**
		 * Definition of a vector holding object ids.
		 */
		typedef std::vector<ObjectId> ObjectIds;

		/**
		 * Definition of an unordered set holding object ids.
		 */
		typedef std::unordered_set<ObjectId> ObjectIdSet;

		/**
		 * Definition of an unordered map mapping keys to values.
		 */
		typedef std::unordered_map<std::string, Value> Metadata;

		/**
		 * Definition of a sample holding a measurement.
		 */
		class OCEAN_DEVICES_EXPORT Sample
		{
			friend class ObjectRef<Sample>;

			public:

				/**
				 * Returns the sample timestamp.
				 * @return Sample timestamp
				 */
				inline const Timestamp& timestamp() const;

				/**
				 * Returns the sample object ids specifying possible different measurement units.
				 * @return Object ids
				 */
				inline const ObjectIds& objectIds() const;

				/**
				 * Returns the metadata of this sample.
				 * @return The sample's meta data
				 */
				inline const Metadata& metadata() const;

			protected:

				/**
				 * Creates a new measurement sample.
				 * @param timestamp Sample timestamp
				 * @param objectIds Object ids corresponding to different measurement units of one tracker
				 * @param metadata Optional metadata of the new sample
				 */
				Sample(const Timestamp& timestamp, const ObjectIds& objectIds, const Metadata& metadata = Metadata());

				/**
				 * Creates a new measurement sample.
				 * @param timestamp Sample timestamp
				 * @param objectIds Object ids corresponding to different measurement units of one tracker
				 * @param metadata Optional metadata of the new sample
				 */
				Sample(const Timestamp& timestamp, ObjectIds&& objectIds, Metadata&& metadata = Metadata());

				/**
				 * Destructs a sample.
				 */
				virtual ~Sample();

			protected:

				/// Sample timestamp
				Timestamp timestamp_;

				/// Measurement unit object ids.
				ObjectIds objectIds_;

				/// The metadata of this sample.
				Metadata metadata_;
		};

		/**
		 * Definition of an object reference for samples.
		 */
		typedef ObjectRef<Sample> SampleRef;

		/**
		 * Definition of a callback function to subscribe for new measurement sample events.
		 * The first parameter is the Measurement object sending the sample
		 * The second parameter is the sample
		 */
		typedef Callback<void, const Measurement*, const SampleRef&> SampleCallback;

		/**
		 * Definition of individual interpolation strategies for samples.
		 */
		enum InterpolationStrategy
		{
			/// An invalid strategy.
			IS_INVALID,
			/// The sample with nearest/closest timestamp is used.
			IS_TIMESTAMP_NEAREST,
			/// The sample is interpolated based on two samples.
			IS_TIMESTAMP_INTERPOLATE
		};

		/**
		 * This class manages the lifetime of an event subscription for sample events.
		 */
		class OCEAN_DEVICES_EXPORT SampleEventSubscription
		{
			friend class Measurement;

			public:

				/**
				 * Default constructor for a not active subscription.
				 */
				SampleEventSubscription() = default;

				/**
				 * Move constructor.
				 * @param sampleEventSubscription The object to be moved
				 */
				inline SampleEventSubscription(SampleEventSubscription&& sampleEventSubscription);

				/**
				 * Destructs the subscription object and unsubscribes the object event.
				 */
				inline ~SampleEventSubscription();

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
				 * @param sampleEventSubscription The subscription object to assign
				 * @return Reference to this object
				 */
				SampleEventSubscription& operator=(SampleEventSubscription&& sampleEventSubscription);

			protected:

				/**
				 * Creates an active subscription object.
				 * @param measurement The measurement object to which the event subscription belongs
				 * @param subscriptionId The subscription id of the event, must be valid
				 */
				SampleEventSubscription(const Measurement& measurement, const SubscriptionId subscriptionId);

				/**
				 * Disabled assign operator.
				 * @param sampleEventSubscription The subscription object to assign
				 * @return Reference to this object
				 */
				SampleEventSubscription& operator=(const SampleEventSubscription& sampleEventSubscription) = delete;

			protected:

				/// The measurement object to which the event subscription belongs.
				MeasurementRef measurement_;

				/// The pointer to the measurement object to which the event subscription belongs.
				Measurement* weakMeasurement_ = nullptr;

				/// The subscription id.
				SubscriptionId subscriptionId_ = invalidSubscriptionId();
		};

	protected:

		/**
		 * Definition of a map holding the most recent samples
		 */
		typedef std::map<Timestamp, SampleRef> SampleMap;

		/**
		 * This class implements a helper class to simplify the mapping between internal object ids (of the actual tracking implementation) and extern object ids (of the device system).
		 * @tparam TInternalId The data type of the internal object id.
		 */
		template <typename TInternalId>
		class ObjectMapper
		{
			protected:

				/**
				 * Definition of an unordered map mapping internal object ids to external object ids.
				 */
				typedef std::unordered_map<TInternalId, ObjectId> InternalObjectIdMap;

				/**
				 * Definition of an unordered map mapping external object ids to internal object ids.
				 */
				typedef std::unordered_map<ObjectId, TInternalId> ExternalObjectIdMap;

			public:

				/**
				 * Default constructor.
				 */
				explicit inline ObjectMapper(Measurement& owner);

				/**
				 * Returns whether this mapping object holds a specific internal object.
				 * @param internalObjectId The id of the internal object to be checked
				 * @return True, if so
				 */
				bool hasInternalObject(const TInternalId& internalObjectId) const;

				/**
				 * Returns whether this mapping object holds a specific external object.
				 * @param externalObjectId The id of the external object to be checked, must be valid
				 * @return True, if so
				 */
				bool hasExternalObject(const Measurement::ObjectId& externalObjectId) const;

				/**
				 * Adds a new internal pattern id.
				 * @param internalObjectId The new internal object id to add
				 * @param description The description of the new pattern
				 * @return The corresponding external object id
				 */
				ObjectId newInternalObjectId(const TInternalId& internalObjectId, const std::string& description);

				/**
				 * Removes a mapping between internal and external object.
				 * @param internalObjectId The id of the internal object for which the mapping will be removed
				 */
				void removeInternalObject(const TInternalId& internalObjectId);

				/**
				 * Converts the tracker's internal object id to an external object id.
				 * @param internalObjectId The internal object id to translate
				 * @return The resulting external object id
				 */
				ObjectId externalObjectIdFromInternalObjectId(const TInternalId& internalObjectId) const;

				/**
				 * Converts an external object id to the tracker tracker's internal object id.
				 * @param externalObjectId The external object id to translate
				 * @param invalidInternalId An invalid internal id
				 * @return The resulting internal object id
				 */
				TInternalId internalObjectIdFromExternalObjectId(const ObjectId externalObjectId, const TInternalId& invalidInternalId) const;

			protected:

				/**
				 * Disabled copy constructor.
				 * @param objectMapper Object which would be copied
				 */
				ObjectMapper(const ObjectMapper& objectMapper) = delete;

				/**
				 * Disabled copy operator.
				 * @param objectMapper Object which would be copied
				 * @return Reference to this object
				 */
				ObjectMapper& operator=(const ObjectMapper& objectMapper) = delete;

			protected:

				/// The owner of this mapper.
				Measurement& owner_;

				/// The map mapping internal object ids to external object ids.
				InternalObjectIdMap internalObjectIdMap_;

				/// The map mapping external object ids to internal object ids.
				ExternalObjectIdMap externalObjectIdMap_;
		};

		/**
		 * Definition of an unordered map mapping descriptions to unique object ids.
		 */
		typedef std::unordered_map<std::string, ObjectId> ObjectDescriptionToIdMap;

		/**
		 * Definition of an unordered map mapping unique object ids to descriptions.
		 */
		typedef std::unordered_map<ObjectId, std::string> ObjectIdToDescriptionMap;

		/**
		 * Definition of a map mapping subscription ids to event sample callback functions.
		 */
		typedef std::unordered_map<SubscriptionId, SampleCallback> SampleSubscriptionMap;

	public:

		/**
		 * Returns the capacity of the internal sample container.
		 * @return The sample container's capacity, with range [2, infinity)
		 * @see setSampleCapacity().
		 */
		inline size_t sampleCapacity() const;

		/**
		 * Sets the capacity of the internal sample container.
		 * @param capacity The number maximal number of samples to be stored, with range [2, infinity)
		 * @return True, if succeeded
		 */
		bool setSampleCapacity(const size_t capacity);

		/**
		 * Returns the most recent sample.
		 * @return Most recent sample as object reference
		 */
		virtual SampleRef sample() const;

		/**
		 * Returns the sample with a specific timestamp.
		 * If no sample exists with the given timestamp the most recent sample is returned.
		 * @param timestamp The timestamp of the sample to be returned
		 * @return Requested sample
		 */
		virtual SampleRef sample(const Timestamp timestamp) const;

		/**
		 * Returns the sample best matching with a specified timestamp.
		 * In case, the given timestamp does not fit to an existing sample, the resulting sample will be based on the specified interpolation strategy.
		 * @param timestamp The timestamp for which a sample will be determined, must be valid
		 * @param interpolationStrategy The interpolation strategy to be applied in case the timestamp does not fit with an existing sample
		 * @return The resulting sample, invalid if no sample could be determined
		 */
		virtual SampleRef sample(const Timestamp& timestamp, const InterpolationStrategy interpolationStrategy) const;

		/**
		 * Subscribes a callback event function for new measurement sample events.
		 * Do not subscribe or unsubscribe from inside an event thread.
		 * @param callback The callback function receiving the event calls, must be valid
		 * @return The resulting subscription object, dispose the object to unsubscribe from the event call
		 */
		[[nodiscard]] SampleEventSubscription subscribeSampleEvent(SampleCallback&& callback);

		/**
		 * Returns the object id for an object description.
		 * @param description The description for which the object id will be returned
		 * @return The object id, an invalid id if the description is not known
		 * @see objectDescriptions().
		 */
		ObjectId objectId(const std::string& description) const;

		/**
		 * Returns descriptions of all objects currently available.
		 * @return All object descriptions
		 * @see objectId().
		 */
		Strings objectDescriptions() const;

		/**
		 * Returns the description of one object of this measurement.
		 * @param objectId The id of the object for which the description will be returned, must be valid
		 * @return The description of the object, empty if the object is unknown
		 */
		std::string objectDescription(const ObjectId objectId) const;

		/**
		 * Returns an invalid object id.
		 * @return Id of an invalid object
		 */
		static constexpr ObjectId invalidObjectId();

	protected:

		/**
		 * Creates a new measurement object.
		 * @param name The name of the measurement device
		 * @param type Major and minor device type of the device
		 */
		Measurement(const std::string& name, const DeviceType type);

		/**
		 * Destructs a measurement object.
		 */
		~Measurement() override;

		/**
		 * Posts a new measurement sample.
		 * @param newSample New sample to post
		 */
		void postNewSample(const SampleRef& newSample);

		/**
		 * Creates a unique object id for a new object (e.g., a tracking object like an image, a marker, or a location).
		 * @param description The description of the new object, must be valid
		 * @return The unique id which is unique across all devices
		 */
		ObjectId addUniqueObjectId(const std::string& description);

		/**
		 * Unsubscribes a sample event callback function.
		 * @param subscriptionId The id of the event subscription to unsubscribe
		 */
		void unsubscribeSampleEvent(const SubscriptionId subscriptionId);

	protected:

		/// Sample lock.
		mutable Lock samplesLock_;

		/// Subscription lock.
		Lock subscriptionLock_;

	private:

		/// The most recent measurement samples.
		SampleMap sampleMap_;

		/// The maximal number of samples this measurement object can hold.
		size_t sampleCapacity_;

		/// Map holding all sample event subscriptions.
		SampleSubscriptionMap sampleSubscriptionMap_;

		/// The subscription id of the next event subscription.
		SubscriptionId nextSampleSubscriptionId_ = SubscriptionId(invalidSubscriptionId() + 1u);

		/// The map mapping object descriptions to object ids.
		ObjectDescriptionToIdMap objectDescriptionToIdMap_;

		/// The map mapping object ids to descriptions.
		ObjectIdToDescriptionMap objectIdToDescriptionMap_;
};

inline const Timestamp& Measurement::Sample::timestamp() const
{
	return timestamp_;
}

inline const Measurement::ObjectIds& Measurement::Sample::objectIds() const
{
	return objectIds_;
}

inline const Measurement::Metadata& Measurement::Sample::metadata() const
{
	return metadata_;
}

template <typename TInternalId>
inline Measurement::ObjectMapper<TInternalId>::ObjectMapper(Measurement& owner) :
	owner_(owner)
{
	// nothing to do here
}

template <typename TInternalId>
bool Measurement::ObjectMapper<TInternalId>::hasInternalObject(const TInternalId& internalObjectId) const
{
	return internalObjectIdMap_.find(internalObjectId) != internalObjectIdMap_.cend();
}

template <typename TInternalId>
bool Measurement::ObjectMapper<TInternalId>::hasExternalObject(const Measurement::ObjectId& externalObjectId) const
{
	ocean_assert(externalObjectId != invalidObjectId());

	return externalObjectIdMap_.find(externalObjectId) != externalObjectIdMap_.cend();
}

template <typename TInternalId>
Measurement::ObjectId Measurement::ObjectMapper<TInternalId>::newInternalObjectId(const TInternalId& internalObjectId, const std::string& description)
{
	const ObjectId externalObjectId = owner_.addUniqueObjectId(description);

	internalObjectIdMap_.emplace(internalObjectId, externalObjectId);
	externalObjectIdMap_.emplace(externalObjectId, internalObjectId);

	return externalObjectId;
}

template <typename TInternalId>
void Measurement::ObjectMapper<TInternalId>::removeInternalObject(const TInternalId& internalObjectId)
{
	const ObjectId externalObjectId = externalObjectIdFromInternalObjectId(internalObjectId);

	internalObjectIdMap_.erase(internalObjectId);
	externalObjectIdMap_.erase(externalObjectId);
}

template <typename TInternalId>
Measurement::ObjectId Measurement::ObjectMapper<TInternalId>::externalObjectIdFromInternalObjectId(const TInternalId& internalObjectId) const
{
	const typename InternalObjectIdMap::const_iterator i = internalObjectIdMap_.find(internalObjectId);

	if (i == internalObjectIdMap_.cend())
	{
		ocean_assert(false && "This must never happen!");
		return invalidObjectId();
	}

	return i->second;
}

template <typename TInternalId>
TInternalId Measurement::ObjectMapper<TInternalId>::internalObjectIdFromExternalObjectId(const ObjectId externalObjectId, const TInternalId& invalidInternalId) const
{
	const typename ExternalObjectIdMap::const_iterator i = externalObjectIdMap_.find(externalObjectId);

	if (i == externalObjectIdMap_.cend())
	{
		ocean_assert(false && "This must never happen!");
		return invalidInternalId;
	}

	return i->second;
}

inline Measurement::SampleEventSubscription::SampleEventSubscription(SampleEventSubscription&& sampleEventSubscription)
{
	*this = std::move(sampleEventSubscription);
}

inline Measurement::SampleEventSubscription::~SampleEventSubscription()
{
	release();
}

inline void Measurement::SampleEventSubscription::makeWeak()
{
	measurement_.release();
}

inline Measurement::SampleEventSubscription::operator bool() const
{
	return weakMeasurement_ != nullptr;
}

inline size_t Measurement::sampleCapacity() const
{
	const ScopedLock scopedLock(samplesLock_);

	return sampleCapacity_;
}

constexpr Measurement::ObjectId Measurement::invalidObjectId()
{
	return ObjectId(-1);
}

}

}

#endif // META_OCEAN_DEVICES_MEASUREMENT_H
