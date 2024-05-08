/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/PowerMonitor.h"
#include "ocean/platform/android/Battery.h"

#include "ocean/base/Median.h"

#include "ocean/math/Variance.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

PowerMonitor::PowerMonitor(const double updateFrequency)
{
	if (updateFrequency > 0.0)
	{
		updateFrequency_ = updateFrequency;
	}
}

PowerMonitor::~PowerMonitor()
{
	stopThreadExplicitly();
}

bool PowerMonitor::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return updateFrequency_ > 0.0;
}

double PowerMonitor::updateFrequency() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(updateFrequency_ > 0.0);

	return updateFrequency_;
}

bool PowerMonitor::reset()
{
	const ScopedLock scopedLock(lock_);

	if (isActive_)
	{
		return false;
	}

	measurements_.clear();

	return true;
}

PowerMonitor::ScopedSubscription PowerMonitor::startMeasurement()
{
	const ScopedLock scopedLock(lock_);

	if (isActive_)
	{
		ocean_assert(false && "The measurement is already active!");
		return ScopedSubscription();
	}

	isActive_ = true;

	if (!isThreadActive())
	{
		startThread();
	}

	return ScopedSubscription(true /*unused*/, std::bind(&PowerMonitor::stopMeasurement, this, std::placeholders::_1));
}

bool PowerMonitor::averagePower(double& power, double* standardDeviation, size_t* numberMeasurements) const
{
	const ScopedLock scopedLock(lock_);

	if (measurements_.empty())
	{
		return false;
	}

	VarianceD variance;

	for (const Measurement& measurement : measurements_)
	{
		variance.add(measurement.current_ * measurement.voltage_);
	}

	power = variance.average();

	if (standardDeviation != nullptr)
	{
		*standardDeviation = variance.deviation();
	}

	if (numberMeasurements != nullptr)
	{
		*numberMeasurements = measurements_.size();
	}

	return true;
}

bool PowerMonitor::medianPower(double& power, size_t* numberMeasurements) const
{
	const ScopedLock scopedLock(lock_);

	if (measurements_.empty())
	{
		return false;
	}

	std::vector<double> values;
	values.reserve(measurements_.size());

	for (const Measurement& measurement : measurements_)
	{
		values.emplace_back(measurement.current_ * measurement.voltage_);
	}

	power = Median::median(values.data(), values.size());

	if (numberMeasurements != nullptr)
	{
		*numberMeasurements = measurements_.size();
	}

	return true;
}

double PowerMonitor::determineIdealUpdateFrequency(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	if (testDuration <= 0.0)
	{
		return -1.0;
	}

	double currentUpdateFrequency = -1.0;
	if (!determineIdealUpdateFrequency<true>(currentUpdateFrequency, testDuration))
	{
		return -1.0;
	}

	double voltageUpdateFrequency = -1.0;
	if (!determineIdealUpdateFrequency<false>(voltageUpdateFrequency, testDuration))
	{
		return -1.0;
	}

	const double updateFrequency = std::max(currentUpdateFrequency, voltageUpdateFrequency);

	Log::debug() << "PowerMonitor: Combined update frequency is " << updateFrequency << " Hz";

	return updateFrequency;
}

void PowerMonitor::stopMeasurement(const bool&)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isActive_);

	isActive_ = false;
}

void PowerMonitor::threadRun()
{
	ocean_assert(updateFrequency_ > 0.0);

	const double interval = 1.0 / updateFrequency_;

	double current;
	double voltage;

	Timestamp nextTimestamp(false);

	while (!shouldThreadStop())
	{
		const Timestamp timestamp(true);

		if (timestamp >= nextTimestamp)
		{
			nextTimestamp = timestamp + interval;

			if (Battery::currentCurrent(current) && Battery::currentVoltage(voltage))
			{
				const ScopedLock scopedLock(lock_);

				if (isActive_)
				{
					measurements_.emplace_back(timestamp, current, voltage);
				}
			}
		}

		sleep(1u);
	}
}

template <bool tForCurrent>
bool PowerMonitor::determineIdealUpdateFrequency(double& updateFrequency, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const Timestamp startTimestamp(true);

	double value = NumericD::minValue();

	if constexpr (tForCurrent)
	{
		if (!Battery::currentCurrent(value))
		{
			return false;
		}
	}
	else
	{
		if (!Battery::currentVoltage(value))
		{
			return false;
		}
	}

	double lastValue = value;

	Timestamps timestamps;
	timestamps.reserve(1024);

	timestamps.emplace_back(startTimestamp);

	while (true)
	{
		const Timestamp timestamp(true);

		if (timestamp > startTimestamp + testDuration)
		{
			break;
		}

		if constexpr (tForCurrent)
		{
			if (!Battery::currentCurrent(value))
			{
				return false;
			}
		}
		else
		{
			if (!Battery::currentVoltage(value))
			{
				return false;
			}
		}

		if (value != lastValue)
		{
			timestamps.emplace_back(timestamp);

			lastValue = value;
			continue;
		}

		Thread::sleep(0u);
	}

	if constexpr (tForCurrent)
	{
		Log::debug() << "PowerMonitor: Got " << timestamps.size() << " current measurement to determine update frequency";
	}
	else
	{
		Log::debug() << "PowerMonitor: Got " << timestamps.size() << " voltage measurements to determine update frequency";
	}

	return determineIdealUpdateFrequency(timestamps, updateFrequency);
}

bool PowerMonitor::determineIdealUpdateFrequency(const Timestamps& timestamps, double& updateFrequency)
{
	if (timestamps.size() < 2)
	{
		return false;
	}

	std::vector<double> intervals;
	intervals.reserve(timestamps.size());

	for (size_t n = 1; n < timestamps.size(); ++n)
	{
		intervals.emplace_back(timestamps[n] - timestamps[n - 1]);
	}

	std::sort(intervals.begin(), intervals.end());

	ocean_assert(intervals.size() * 99 / 100 < intervals.size());
	const double interval = intervals[intervals.size() * 99 / 100];

	if (interval <= NumericD::eps())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// we samples twice as often to capture an interval
	updateFrequency = 1.0 / (interval * 0.5);

	Log::debug() << "PowerMonitor: P99 interval is " << interval << "sec, update frequency is " << updateFrequency << " Hz";

	return true;
}

}

}

}
