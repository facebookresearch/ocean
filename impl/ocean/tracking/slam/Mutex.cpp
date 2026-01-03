/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/Mutex.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

LockManager::LockManager()
{
	// nothing to do here
}

std::string LockManager::report(const bool skipZero)
{
	if constexpr (!isEnabled_)
	{
		return "Report is disabled";
	}

	const ScopedLock scopedLock(lock_);

	std::string result;

	for (const bool acquisition : {true, false})
	{
		if (acquisition)
		{
			result += "Acquisition delay measurements:\n\n";
		}
		else
		{
			result += "Hold duration measurements:\n\n";
		}

		Map& map = acquisition ? acquisitionDelayMap_ : holdDurationMap_;

		for (Map::value_type& mapPair : map)
		{
			const Key& key = mapPair.first;

			Measurements& measurements = mapPair.second;

			if (measurements.empty())
			{
				continue;
			}

			std::sort(measurements.begin(), measurements.end());

			const double p99 = measurements[measurements.size() * 99 / 100];
			const double p995 = measurements[measurements.size() * 995 / 1000];
			const double p999 = measurements[measurements.size() * 999 / 1000];
			const double worst = measurements.back();

			if (skipZero && worst < 0.01 / 1000.0)
			{
				continue;
			}

			if (key.first)
			{
				result += "Write";
			}
			else
			{
				result += "Read";
			}

			result += " lock '" + key.second + ", with " + String::toAString(measurements.size()) + " measurements:\n";

			result += "P99: " + String::toAString(p99 * 1000.0, 1u) + "ms, P99.5: " + String::toAString(p995 * 1000.0, 1u) + "ms, P99.9: " + String::toAString(p999 * 1000.0, 1u) + "ms, P100: " + String::toAString(worst * 1000.0, 1u) + "ms\n";
			result += "\n";
		}

		if (result.size() >= 2)
		{
			result.pop_back();
			result.pop_back();
		}
	}

	return result;
}

void LockManager::addAcquisitionDelayMeasurement(const std::string& name, const bool isWriteLock, const double duration)
{
	const ScopedLock scopedLock(lock_);

	acquisitionDelayMap_[Key(isWriteLock, name)].push_back(duration);
}

void LockManager::addHoldDurationMeasurement(const std::string& name, const bool isWriteLock, const double duration)
{
	const ScopedLock scopedLock(lock_);

	holdDurationMap_[Key(isWriteLock, name)].push_back(duration);
}

}

}

}
