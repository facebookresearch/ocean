/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Measurement.h"
#include "ocean/devices/Manager.h"

#include "ocean/math/Interpolation.h"

namespace Ocean
{

namespace Devices
{

Measurement::Sample::Sample(const Timestamp& timestamp, const ObjectIds& objectIds, const Metadata& metadata) :
	timestamp_(timestamp),
	objectIds_(objectIds),
	metadata_(metadata)
{
	// nothing to do here
}

Measurement::Sample::Sample(const Timestamp& timestamp, ObjectIds&& objectIds, Metadata&& metadata) :
	timestamp_(timestamp),
	objectIds_(std::move(objectIds)),
	metadata_(std::move(metadata))
{
	// nothing to do here
}

Measurement::Sample::~Sample()
{
	// nothing to do here
}

Measurement::SampleEventSubscription::SampleEventSubscription(const Measurement& measurement, const SubscriptionId subscriptionId) :
	subscriptionId_(subscriptionId)
{
	measurement_ = DeviceRefManager::get().device(measurement.name());
	ocean_assert(measurement_);

	weakMeasurement_ = &*measurement_;
	ocean_assert(weakMeasurement_ == &measurement);
}

void Measurement::SampleEventSubscription::release()
{
	if (weakMeasurement_ != nullptr)
	{
		weakMeasurement_->unsubscribeSampleEvent(subscriptionId_);
		subscriptionId_ = invalidSubscriptionId();

		weakMeasurement_ = nullptr;
	}

	measurement_.release();
}

Measurement::SampleEventSubscription& Measurement::SampleEventSubscription::operator=(SampleEventSubscription&& sampleEventSubscription)
{
	if (this != &sampleEventSubscription)
	{
		release();

		measurement_ = std::move(sampleEventSubscription.measurement_);
		weakMeasurement_ = sampleEventSubscription.weakMeasurement_;
		sampleEventSubscription.weakMeasurement_ = nullptr;

		subscriptionId_ = sampleEventSubscription.subscriptionId_;
		sampleEventSubscription.subscriptionId_ = invalidSubscriptionId();
	}

	return *this;
}

Measurement::Measurement(const std::string& name, const DeviceType type) :
	Device(name, type)
{
	ocean_assert((type.majorType() & DEVICE_MEASUREMENT) == DEVICE_MEASUREMENT);
}

Measurement::~Measurement()
{
	ocean_assert(sampleSubscriptionMap_.empty());
}

bool Measurement::setSampleCapacity(const size_t capacity)
{
	const ScopedLock scopedLock(samplesLock_);

	if (capacity < 2)
	{
		return false;
	}

	while (sampleMap_.size() > capacity)
	{
		ocean_assert(sampleMap_.size() < 2 || sampleMap_.begin()->first < sampleMap_.rbegin()->first);

		// removing the oldest sample
		sampleMap_.erase(sampleMap_.begin());
	}

	sampleCapacity_ = capacity;
	return true;
}

Measurement::SampleRef Measurement::sample() const
{
	const ScopedLock scopedLock(samplesLock_);

	// looking for the most recent sample
	const SampleMap::const_reverse_iterator i = sampleMap_.crbegin();

	if (i == sampleMap_.crend())
	{
		return SampleRef();
	}

	return i->second;
}

Measurement::SampleRef Measurement::sample(const Timestamp timestamp) const
{
	const ScopedLock scopedLock(samplesLock_);

	const SampleMap::const_iterator i = sampleMap_.find(timestamp);

	if (i == sampleMap_.cend())
	{
		if (sampleMap_.empty())
		{
			return SampleRef();
		}

		ocean_assert(sampleMap_.rbegin() != sampleMap_.rend());
		return sampleMap_.rbegin()->second;
	}

	return i->second;
}

Measurement::SampleRef Measurement::sample(const Timestamp& timestamp, const InterpolationStrategy interpolationStrategy) const
{
	const ScopedLock scopedLock(samplesLock_);

	if (sampleMap_.empty())
	{
		return SampleRef();
	}

	if (sampleMap_.size() == 1)
	{
		// we just have one sample, so we return this one without any further handling

		ocean_assert(sampleMap_.rbegin() != sampleMap_.rend());
		return sampleMap_.rbegin()->second;
	}

	// let's find the sample with timestamp bigger (younger) than the specified timestamp
	const SampleMap::const_iterator iUpper = sampleMap_.upper_bound(timestamp);

	if (iUpper == sampleMap_.end())
	{
		// our timestamp is too new so that we simply return the sample of the most recent timestamp

		ocean_assert(sampleMap_.rbegin() != sampleMap_.rend());
		return sampleMap_.rbegin()->second;
	}

	ocean_assert(iUpper->first > timestamp);

	if (iUpper == sampleMap_.begin())
	{
		// our timestamp is too old so that we simply return the oldest sample we have

		ocean_assert(sampleMap_.begin() != sampleMap_.end());
		return sampleMap_.begin()->second;
	}

	SampleMap::const_iterator iLower(iUpper);

	ocean_assert(iLower != sampleMap_.begin());
	iLower--;

	ocean_assert(iLower->first == iLower->second->timestamp());
	ocean_assert(iUpper->first == iUpper->second->timestamp());

	ocean_assert(iLower->first <= timestamp);

	const double lowerDelta = double(timestamp - iLower->first);
	const double upperDelta = double(iUpper->first - timestamp);
	ocean_assert(lowerDelta >= 0.0 && upperDelta >= 0.0);

	if (interpolationStrategy == IS_TIMESTAMP_INTERPOLATE)
	{
		const double delta = lowerDelta + upperDelta;

		if (NumericD::isEqualEps(delta))
		{
			// both samples are almost identical, so that we return the sample from the past (not from the future)
			return iLower->second;
		}

		ocean_assert(iLower->second->objectIds() == iUpper->second->objectIds()); // this is a restriction that will not hold in any case, we need to handle it if necessary

		const double interpolationFactor = lowerDelta / delta;
		ocean_assert(interpolationFactor >= 0.0 && interpolationFactor <= 1.0);

		const Timestamp interpolatedTimestamp = Timestamp(Interpolation::linear(double(iLower->second->timestamp()), double(iUpper->second->timestamp()), double(interpolationFactor)));
		ocean_assert(iLower->second->timestamp() <= interpolatedTimestamp && interpolatedTimestamp <= iUpper->second->timestamp());
		ocean_assert(NumericD::isEqual(double(interpolatedTimestamp), double(timestamp), NumericD::weakEps()));

		return interpolateSamples(iLower->second, iUpper->second, interpolationFactor, interpolatedTimestamp);
	}

	ocean_assert(interpolationStrategy == IS_TIMESTAMP_NEAREST);

	// let's return the sample with timestamp closest to the specified timestamp

	if (lowerDelta < upperDelta)
	{
		return iLower->second;
	}
	else
	{
		return iUpper->second;
	}
}

Measurement::SampleEventSubscription Measurement::subscribeSampleEvent(SampleCallback&& callback)
{
	if (callback.isNull())
	{
		return SampleEventSubscription();
	}

	const ScopedLock scopedLock(subscriptionLock_);

	const SubscriptionId subscriptionId = nextSampleSubscriptionId_++;

	ocean_assert(subscriptionId != invalidSubscriptionId());
	ocean_assert(sampleSubscriptionMap_.find(subscriptionId) == sampleSubscriptionMap_.end());
	sampleSubscriptionMap_[subscriptionId] = std::move(callback);

	return SampleEventSubscription(*this, subscriptionId);
}

Measurement::ObjectId Measurement::objectId(const std::string& description) const
{
	const ScopedLock scopedLock(deviceLock);

	const ObjectDescriptionToIdMap::const_iterator i = objectDescriptionToIdMap_.find(description);

	if (i == objectDescriptionToIdMap_.cend())
	{
		return invalidObjectId();
	}

	return i->second;
}

Strings Measurement::objectDescriptions() const
{
	const ScopedLock scopedLock(deviceLock);

	Strings descriptions;
	descriptions.reserve(objectDescriptionToIdMap_.size());

	for (ObjectDescriptionToIdMap::const_iterator i = objectDescriptionToIdMap_.cbegin(); i != objectDescriptionToIdMap_.cend(); ++i)
	{
		descriptions.emplace_back(i->first);
	}

	return descriptions;
}

std::string Measurement::objectDescription(const ObjectId objectId) const
{
	ocean_assert(objectId != invalidObjectId());

	const ScopedLock scopedLock(deviceLock);

	const ObjectIdToDescriptionMap::const_iterator i = objectIdToDescriptionMap_.find(objectId);

	if (i == objectIdToDescriptionMap_.cend())
	{
		return std::string();
	}

	return i->second;
}

void Measurement::postNewSample(const SampleRef& newSample)
{
	ocean_assert(newSample);

	{
		const ScopedLock scopedLock(samplesLock_);

		if (sampleMap_.size() >= sampleCapacity_)
		{
			const SampleMap::iterator i = sampleMap_.begin();
			ocean_assert(i != sampleMap_.end());

			sampleMap_.erase(i);
		}

		sampleMap_.emplace(newSample->timestamp(), newSample);
	}

	const ScopedLock scopedLock(subscriptionLock_);

	for (SampleSubscriptionMap::const_iterator i = sampleSubscriptionMap_.cbegin(); i != sampleSubscriptionMap_.cend(); ++i)
	{
		i->second(this, newSample);
	}
}

Measurement::ObjectId Measurement::addUniqueObjectId(const std::string& description)
{
	ocean_assert(!description.empty());

	const ObjectId objectId = Manager::get().createUniqueObjectId(description);

	const ScopedLock scopedLock(deviceLock);

	if (objectDescriptionToIdMap_.find(description) != objectDescriptionToIdMap_.cend())
	{
		ocean_assert(false && "The description has been used already!");
		return invalidObjectId();
	}

	ocean_assert(objectDescriptionToIdMap_.find(description) == objectDescriptionToIdMap_.cend());
	ocean_assert(objectIdToDescriptionMap_.find(objectId) == objectIdToDescriptionMap_.cend());

	objectDescriptionToIdMap_.emplace(description, objectId);
	objectIdToDescriptionMap_.emplace(objectId, description);

	return objectId;
}

void Measurement::unsubscribeSampleEvent(const SubscriptionId subscriptionId)
{
	if (subscriptionId != invalidSubscriptionId())
	{
		const ScopedLock scopedLock(subscriptionLock_);

		ocean_assert(sampleSubscriptionMap_.find(subscriptionId) != sampleSubscriptionMap_.end());
		sampleSubscriptionMap_.erase(subscriptionId);
	}
}

Measurement::SampleRef Measurement::interpolateSamples(const SampleRef& lowerSample, const SampleRef& upperSample, const double /*interpolationFactor*/, const Timestamp& /*interpolatedTimestamp*/) const
{
	ocean_assert(lowerSample && upperSample);

	// Default implementation: no interpolation, just return the lower sample
	// Derived classes should override this method to provide type-specific interpolation
	return lowerSample;
}


}

}
