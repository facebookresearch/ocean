/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SAMPLE_MAP_H
#define META_OCEAN_MATH_SAMPLE_MAP_H

#include "ocean/math/Math.h"
#include "ocean/math/Interpolation.h"
#include "ocean/math/Numeric.h"

#include "ocean/base/Lock.h"

namespace Ocean
{

/**
 * This class stores samples of e.g., sensor or tracking data in a map.
 * Samples are stored in a round robin method.<br>
 * Whenever the map does not have an empty spot left, the oldest sample will be replaced by the newest sample.<br>
 * The implementation is thread-safe.
 * @tparam T The type of the sample to be stored
 * @ingroup math
 */
template <typename T>
class SampleMap
{
	public:

		/**
		 * Definition of a map mapping timestamps to samples.
		 */
		typedef std::map<double, T> Map;

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

	public:

		/**
		 * Creates a new map with default capacity.
		 */
		SampleMap() noexcept;

		/**
		 * Creates a new amp with specified capacity.
		 * @param capacity The number of samples the new map can store, with range [1, infinity), -1 to create a map without capacity restrictions
		 */
		explicit SampleMap(const size_t capacity) noexcept;

		/**
		 * Copy constructor.
		 * @param sampleMap The map to be copied
		 */
		SampleMap(const SampleMap& sampleMap) noexcept;

		/**
		 * Move constructor.
		 * @param sampleMap The map to be moved
		 */
		SampleMap(SampleMap&& sampleMap) noexcept;

		/**
		 * Inserts a new sample with corresponding timestamp.
		 * In case, a sample with same timestamp exists already, the given sample will replace the existing sample.<br>
		 * In case, the map does not have an empty spot left, the oldest sample will be replaced by the given sample.
		 * @param sample The new sample to be inserted, must be valid
		 * @param timestamp The timestamp of the sample, must be valid
		 */
		void insert(const T& sample, const double& timestamp);

		/**
		 * Returns the most recent sample.
		 * @param value The resulting sample
		 * @param sampleTimestamp The timestamp of the returning sample; nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool sample(T& value, double* sampleTimestamp = nullptr) const;

		/**
		 * Returns the sample with a specific timestamp.
		 * @param timestamp Timestamp of the sample to be returned
		 * @param value The resulting sample with the requested timestamp
		 * @return True, if succeeded (if a sample with the timestamp existed)
		 */
		bool sample(const double& timestamp, T& value) const;

		/**
		 * Returns the sample best matching with a specified timestamp.
		 * In case, the given timestamp does not fit to an existing sample, the resulting sample will be based on the specified interpolation strategy.
		 * @param timestamp The timestamp for which a sample will be determined, must be valid
		 * @param interpolationStrategy The interpolation strategy to be applied in case the timestamp does not fit with an existing sample
		 * @param value The resulting sample
		 * @param timestampDistance Optional resulting minimal time distance between the requested timestamp and the sample(s) used to create the resulting value, with range [0, infinity)
		 * @param sampleTimestamp The timestamp of the returning sample; nullptr if not of interest
		 * @return True, if a sample could be determined
		 */
		bool sample(const double& timestamp, const InterpolationStrategy interpolationStrategy, T& value, double* timestampDistance = nullptr, double* sampleTimestamp = nullptr) const;

		/**
		 * Returns all samples stores in this map.
		 * @return All samples as vector of pairs
		 */
		std::vector<std::pair<double, T>> samples() const;

		/**
		 * Returns all samples stores in this map.
		 * @return All samples as a standard map
		 */
		Map data() const;

		/**
		 * Returns the number of samples currently stored in this map.
		 * @return The map's samples, with range [0, capacity()]
		 */
		inline size_t size() const;

		/**
		 * Returns the capacity of this map (the number of samples this map can store).
		 * The capacity of this map, with range [1, infinity)
		 */
		inline size_t capacity() const;

		/**
		 * Returns whether this map is empty.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Removes all samples from this map.
		 */
		inline void clear();

		/**
		 * Copy operator.
		 * @param sampleMap The map to be copied
		 * @return Reference to this object
		 */
		SampleMap<T>& operator=(const SampleMap<T>& sampleMap) noexcept;

		/**
		 * Move operator.
		 * @param sampleMap The map to be moved
		 * @return Reference to this object
		 */
		SampleMap<T>& operator=(SampleMap<T>&& sampleMap) noexcept;

	protected:

		/// The map holding the actual samples.
		Map map_;

		/// The capacity of this map, with range [1, infinity).
		size_t capacity_;

		/// The lock of this map.
		mutable Lock lock_;
};

template <typename T>
SampleMap<T>::SampleMap() noexcept :
	capacity_(100)
{
	// nothing to do here
}

template <typename T>
SampleMap<T>::SampleMap(const size_t capacity) noexcept :
	capacity_(capacity)
{
	ocean_assert(capacity_ >= 1);
}

template <typename T>
SampleMap<T>::SampleMap(const SampleMap& sampleMap) noexcept :
	map_(sampleMap.map_),
	capacity_(sampleMap.capacity_)
{
	// nothing to do here
}

template <typename T>
SampleMap<T>::SampleMap(SampleMap&& sampleMap) noexcept
{
	*this = std::move(sampleMap);
}

template <typename T>
void SampleMap<T>::insert(const T& sample, const double& timestamp)
{
	const ScopedLock scopedLock(lock_);

	if (map_.size() >= capacity_)
	{
		typename Map::iterator i = map_.begin();
		ocean_assert(i != map_.end());

		map_.erase(i);
	}

	map_[timestamp] = sample; // not using insert to ensure that identical timestamps overwrite previous entries
}

template <typename T>
bool SampleMap<T>::sample(T& value, double* sampleTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	// looking for the most recent sample
	typename Map::const_reverse_iterator i = map_.rbegin();

	if (i == map_.rend())
	{
		return false;
	}

	value = i->second;

	if (sampleTimestamp)
	{
		*sampleTimestamp = i->first;
	}

	return true;
}

template <typename T>
bool SampleMap<T>::sample(const double& timestamp, T& value) const
{
	const ScopedLock scopedLock(lock_);

	const typename Map::const_iterator i = map_.find(timestamp);

	if (i == map_.cend())
	{
		return false;
	}

	value = i->second;

	return true;
}

template <typename T>
bool SampleMap<T>::sample(const double& timestamp, const InterpolationStrategy interpolationStrategy, T& value, double* timestampDistance, double* sampleTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	if (map_.empty())
	{
		return false;
	}

	if (map_.size() == 1)
	{
		// we just have one sample, so we return this one without any further handling

		ocean_assert(map_.rbegin() != map_.rend());
		value = map_.rbegin()->second;

		if (timestampDistance)
		{
			// distance is |timestamp - sample.timestamp|
			*timestampDistance = NumericD::abs(timestamp - map_.rbegin()->first);
		}

		if (sampleTimestamp)
		{
			*sampleTimestamp = map_.rbegin()->first;
		}

		return true;
	}

	// let's find the sample with timestamp bigger (younger) than the specified timestamp
	const typename Map::const_iterator iUpper = map_.upper_bound(timestamp);

	if (iUpper == map_.cend())
	{
		// our timestamp is too new so that we simply return the sample of the most recent timestamp

		ocean_assert(map_.rbegin() != map_.rend());
		value = map_.rbegin()->second;

		if (timestampDistance)
		{
			// distance is |timestamp - newestSample.timestamp|
			*timestampDistance = NumericD::abs(timestamp - map_.rbegin()->first);
		}

		if (sampleTimestamp)
		{
			*sampleTimestamp = map_.rbegin()->first;
		}

		return true;
	}

	ocean_assert(iUpper->first > timestamp);

	if (iUpper == map_.cbegin())
	{
		// our timestamp is too old so that we simply return the oldest sample we have

		ocean_assert(map_.begin() != map_.end());

		value = map_.begin()->second;

		if (timestampDistance)
		{
			// distance is |timestamp - oldestSample.timestamp|
			*timestampDistance = NumericD::abs(timestamp - map_.begin()->first);
		}

		if (sampleTimestamp)
		{
			*sampleTimestamp = map_.begin()->first;
		}

		return true;
	}

	typename Map::const_iterator iLower(iUpper);

	ocean_assert(iLower != map_.begin());
	iLower--;

	ocean_assert(iLower->first <= timestamp);

	const double lowerDelta = timestamp - iLower->first;
	const double upperDelta = iUpper->first - timestamp;
	ocean_assert(lowerDelta >= 0.0 && upperDelta >= 0.0);

	if (interpolationStrategy == IS_TIMESTAMP_INTERPOLATE)
	{
		const double delta = lowerDelta + upperDelta;

		if (NumericD::isEqualEps(delta))
		{
			// both samples are almost identical, so that we return the sample from the past (not from the future)
			value = iLower->second;

			if (timestampDistance)
			{
				// distance is 0
				*timestampDistance = 0.0;
			}

			if (sampleTimestamp)
			{
				*sampleTimestamp = iLower->first;
			}

			return true;
		}

		const double interpolationFactor = lowerDelta / delta;
		ocean_assert(interpolationFactor >= 0.0 && interpolationFactor <= 1.0);

		const T& lowerSample = iLower->second;
		const T& upperSample = iUpper->second;

		value = Interpolation::linear(lowerSample, upperSample, interpolationFactor);

		if (timestampDistance)
		{
			// distance is min(|upperTimestamp - timestamp|, |timestamp - lowerTimestamp|)
			*timestampDistance = std::min(iUpper->first - timestamp, timestamp - iLower->first);
		}

		if (sampleTimestamp)
		{
			*sampleTimestamp = iLower->first * interpolationFactor + iUpper->first * (1.0 - interpolationFactor);
		}

		return true;
	}

	ocean_assert(interpolationStrategy == IS_TIMESTAMP_NEAREST);

	// let's return the sample with timestamp closest to the specified timestamp

	if (lowerDelta < upperDelta)
	{
		value = iLower->second;

		if (timestampDistance)
		{
			// distance is min(|upperTimestamp - timestamp|, |timestamp - lowerTimestamp|)
			*timestampDistance = std::min(iUpper->first - timestamp, timestamp - iLower->first);
		}

		if (sampleTimestamp)
		{
			*sampleTimestamp = iLower->first;
		}

		return true;
	}

	value = iUpper->second;

	if (timestampDistance)
	{
		// distance is min(|upperTimestamp - timestamp|, |timestamp - lowerTimestamp|)
		*timestampDistance = std::min(iUpper->first - timestamp, timestamp - iLower->first);
	}

	if (sampleTimestamp)
	{
		*sampleTimestamp = iUpper->first;
	}

	return true;
}

template <typename T>
std::vector<std::pair<double, T>> SampleMap<T>::samples() const
{
	const ScopedLock scopedLock(lock_);

	std::vector<std::pair<double, T>> result;
	result.reserve(map_.size());

	for (typename Map::const_iterator i = map_.cbegin(); i != map_.cend(); ++i)
	{
		result.emplace_back(i->first, i->second);
	}

	return result;
}

template <typename T>
typename SampleMap<T>::Map SampleMap<T>::data() const
{
	const ScopedLock scopedLock(lock_);

	Map result(map_);

	return result;
}

template <typename T>
inline size_t SampleMap<T>::size() const
{
	const ScopedLock scopedLock(lock_);

	return map_.size();
}

template <typename T>
inline size_t SampleMap<T>::capacity() const
{
	const ScopedLock scopedLock(lock_);

	return capacity_;
}

template <typename T>
inline bool SampleMap<T>::isEmpty() const
{
	const ScopedLock scopedLock(lock_);

	return map_.empty();
}

template <typename T>
inline void SampleMap<T>::clear()
{
	const ScopedLock scopedLock(lock_);

	map_.clear();
}

template <typename T>
SampleMap<T>& SampleMap<T>::operator=(const SampleMap<T>& sampleMap) noexcept
{
	map_ = sampleMap.map_;
	capacity_ = sampleMap.capacity_;

	return *this;
}

template <typename T>
SampleMap<T>& SampleMap<T>::operator=(SampleMap<T>&& sampleMap) noexcept
{
	if (this  != &sampleMap)
	{
		map_ = std::move(sampleMap.map_);

		capacity_ = sampleMap.capacity_;
		sampleMap.capacity_ = 0;
	}

	return *this;
}

}

#endif // META_OCEAN_MATH_SAMPLE_MAP_H
