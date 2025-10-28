/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Timestamp.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Messenger.h"

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
	#include <chrono>
#else
	#include <time.h>
#endif

namespace Ocean
{

Timestamp::TimestampConverter::TimestampConverter(const TimeDomain timeDomain, const size_t necessaryMeasurements) :
	timeDomain_(timeDomain),
	necessaryMeasurements_(necessaryMeasurements)
{
#ifndef OCEAN_PLATFORM_BUILD_WINDOWS
	domainPosixClockId_ = posixClockId(timeDomain_);
#endif
}

Timestamp Timestamp::TimestampConverter::toUnix(const int64_t domainTimestampNs)
{
	const int64_t domainToUnixOffsetNs = domainToUnixOffset();

	ocean_assert(domainToUnixOffsetNs != invalidValue_);
	if (domainToUnixOffsetNs == invalidValue_)
	{
		return Timestamp(false);
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	if (domainToUnixOffsetNs_ == invalidValue_)
	{
		Log::debug() << "Input time: " << domainTimestampNs;
	}
#endif

	const int64_t unixTimestampNs = domainTimestampNs + domainToUnixOffsetNs;

	return Timestamp(Timestamp::nanoseconds2seconds(unixTimestampNs));
}

Timestamp Timestamp::TimestampConverter::toUnix(const double domainTimestampSeconds)
{
	const int64_t domainToUnixOffsetNs = domainToUnixOffset();

	ocean_assert(domainToUnixOffsetNs != invalidValue_);
	if (domainToUnixOffsetNs == invalidValue_)
	{
		return Timestamp(false);
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	if (domainToUnixOffsetNs_ == invalidValue_)
	{
		Log::debug() << "Input time: " << domainTimestampNs;
	}
#endif

	const double unixTimestampSeconds = domainTimestampSeconds + Timestamp::nanoseconds2seconds(domainToUnixOffsetNs);

	return Timestamp(unixTimestampSeconds);
}

bool Timestamp::TimestampConverter::isWithinRange(const int64_t domainTimestampNs, const double maxDistance, double* distance)
{
	ocean_assert(maxDistance >= 0.0);

	const int64_t currentDomainTimestampNs = currentTimestampNs(timeDomain_);

	const int64_t distanceNs = domainTimestampNs - currentDomainTimestampNs;

	const int64_t absDistanceNs = std::abs(distanceNs);

	const double absDistanceSeconds = Timestamp::nanoseconds2seconds(absDistanceNs);

	if (distance != nullptr)
	{
		*distance = Timestamp::nanoseconds2seconds(distanceNs);
	}

	return absDistanceSeconds <= maxDistance;
}

int64_t Timestamp::TimestampConverter::domainToUnixOffset()
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return invalidValue_;
	}

	const int64_t offsetNs = domainToUnixOffsetNs_;

	if (offsetNs != invalidValue_)
	{
		return offsetNs;
	}

	const ScopedLock scopedLock(lock_);

	// First, we determine the unix timestamp and the equivalent domain timestamp (a pair of both timestamps)
	// then we determine the offset (averaged over several measurements)
	// finally, we convert the provided domain timestamp to the corresponding unix timestamp by applying the determined (averaged) offset

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

	ocean_assert(timeDomain_ == TD_MONOTONIC);

	// On Windows, steady_clock wraps the QueryPerformanceCounter function.
	// https://learn.microsoft.com/en-us/cpp/standard-library/steady-clock-struct

	std::chrono::steady_clock::time_point domainTimestampTimePoint;
	std::chrono::system_clock::time_point unixTimestampTimePoint;

	if (measurements_ % 2 == 0)
	{
		domainTimestampTimePoint = std::chrono::steady_clock::now();
		unixTimestampTimePoint = std::chrono::system_clock::now();

	}
	else
	{
		unixTimestampTimePoint = std::chrono::system_clock::now();
		domainTimestampTimePoint = std::chrono::steady_clock::now();
	}

	const int64_t domainNs = std::chrono::duration_cast<std::chrono::nanoseconds>(domainTimestampTimePoint.time_since_epoch()).count();
	const int64_t unixNs = std::chrono::duration_cast<std::chrono::nanoseconds>(unixTimestampTimePoint.time_since_epoch()).count();

#else

	ocean_assert(domainPosixClockId_ != -1);

	struct timespec domainTimestampSpec;
	struct timespec unixTimestampSpec;

	if (measurements_ % 2 == 0)
	{
		const int resultDomain = clock_gettime(clockid_t(domainPosixClockId_), &domainTimestampSpec);
		const int resultUnix = clock_gettime(CLOCK_REALTIME, &unixTimestampSpec);

		ocean_assert_and_suppress_unused(resultDomain == 0, resultDomain);
		ocean_assert_and_suppress_unused(resultUnix == 0, resultUnix);
	}
	else
	{
		const int resultUnix = clock_gettime(CLOCK_REALTIME, &unixTimestampSpec);
		const int resultDomain = clock_gettime(clockid_t(domainPosixClockId_), &domainTimestampSpec);

		ocean_assert_and_suppress_unused(resultUnix == 0, resultUnix);
		ocean_assert_and_suppress_unused(resultDomain == 0, resultDomain);
	}

	const int64_t domainNs = domainTimestampSpec.tv_sec * nanosecondsPerSecond_ + domainTimestampSpec.tv_nsec;
	const int64_t unixNs = unixTimestampSpec.tv_sec * nanosecondsPerSecond_ + unixTimestampSpec.tv_nsec;

#endif // OCEAN_PLATFORM_BUILD_WINDOWS

	/*
	 * We calculate the unix timestamp of the equivalent domain timestamp by gathering the unix and domain timestamp at the same point in time
	 * unix = domain + offset
	 */

	if (initialDomainNs_ == invalidValue_)
	{
		ocean_assert(initialUnixNs_ == invalidValue_);
		ocean_assert(sumDomainToUnixOffsetNs_ == 0ll);
		ocean_assert(measurements_ == 0);

		initialDomainNs_ = domainNs;
		initialUnixNs_ = unixNs;
	}

	// we use the relative timestamps (in relation to the first measurement) to avoid that the summed offsets are getting out of bounds

	const int64_t relativeDomain = domainNs - initialDomainNs_;
	const int64_t relativeUnix = unixNs - initialUnixNs_;

	sumDomainToUnixOffsetNs_ += relativeUnix - relativeDomain;
	++measurements_;

	int64_t averageRelativeDomainToUnixOffsetNs;

	const int64_t measurements = int64_t(measurements_);

	if (sumDomainToUnixOffsetNs_ >= 0ll)
	{
		averageRelativeDomainToUnixOffsetNs = (sumDomainToUnixOffsetNs_ + (measurements / 2)) / measurements;
	}
	else
	{
		averageRelativeDomainToUnixOffsetNs = (sumDomainToUnixOffsetNs_ - (measurements / 2)) / measurements;
	}

	const int64_t initialDomainToUnixOffsetNs = initialUnixNs_ - initialDomainNs_;

	const int64_t domainToUnixOffsetNs = initialDomainToUnixOffsetNs + averageRelativeDomainToUnixOffsetNs;

	if (measurements_ >= necessaryMeasurements_)
	{
		domainToUnixOffsetNs_ = domainToUnixOffsetNs;
	}

#ifdef OCEAN_INTENSIVE_DEBUG
		Log::debug() << " ";
		Log::debug() << "TimestampConverter:";
		Log::debug() << "Measurement iteration: " << measurements_;
		Log::debug() << "Current domain time: " << domainNs;
		Log::debug() << "Current unix time: " << unixNs;
		Log::debug() << "Domain to unix offset: " << unixNs - domainNs;
		Log::debug() << "Average relative domain to unix offset: " << averageRelativeDomainToUnixOffsetNs;
		Log::debug() << "Averaged domain to unix offset: " << domainToUnixOffsetNs;

		if (domainToUnixOffsetNs_ != invalidValue_)
		{
			Log::info() << "Final domain to unix offset: " << domainToUnixOffsetNs_;
		}
#endif

	return domainToUnixOffsetNs;
}

Timestamp::TimestampConverter& Timestamp::TimestampConverter::operator=(TimestampConverter&& converter)
{
	if (this != &converter)
	{
		timeDomain_ = converter.timeDomain_;
		converter.timeDomain_ = TD_INVALID;

		domainToUnixOffsetNs_ = converter.domainToUnixOffsetNs_.load();
		converter.domainToUnixOffsetNs_ = invalidValue_;

		initialDomainNs_ = converter.initialDomainNs_;
		converter.initialDomainNs_ = invalidValue_;

		initialUnixNs_ = converter.initialUnixNs_;
		converter.initialUnixNs_ = invalidValue_;

		sumDomainToUnixOffsetNs_ = converter.sumDomainToUnixOffsetNs_;
		converter.sumDomainToUnixOffsetNs_ = 0;

		measurements_ = converter.measurements_;
		converter.measurements_ = 0;

		necessaryMeasurements_ = converter.necessaryMeasurements_;
		converter.necessaryMeasurements_ = 0;

#ifndef OCEAN_PLATFORM_BUILD_WINDOWS
		domainPosixClockId_ = converter.domainPosixClockId_;
		converter.domainPosixClockId_ = -1;
#endif
	}

	return *this;
}

int64_t Timestamp::TimestampConverter::currentTimestampNs(const TimeDomain timeDomain)
{
#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

	ocean_assert_and_suppress_unused(timeDomain == TD_MONOTONIC, timeDomain);

	const std::chrono::steady_clock::time_point domainTimestampTimePoint = std::chrono::steady_clock::now();

	return std::chrono::duration_cast<std::chrono::nanoseconds>(domainTimestampTimePoint.time_since_epoch()).count();

#else

	const int domainClockId = posixClockId(timeDomain);
	ocean_assert(domainClockId != -1);

	struct timespec timestampSpec;
	const int result = clock_gettime(clockid_t(domainClockId), &timestampSpec);
	ocean_assert_and_suppress_unused(result == 0, result);

	return timestampSpec.tv_sec * nanosecondsPerSecond_ + timestampSpec.tv_nsec;

#endif // OCEAN_PLATFORM_BUILD_WINDOWS
}

int64_t Timestamp::TimestampConverter::timestampInNs(const int64_t timeValue, const int64_t timeDenominator)
{
	ocean_assert(timeDenominator != 0);

	if (timeDenominator == nanosecondsPerSecond_)
	{
		return timeValue;
	}

	const int64_t seconds = timeValue / timeDenominator;
	const int64_t remainingInTimeScale = timeValue % timeDenominator;

	const int64_t adjustedSeconds = seconds * nanosecondsPerSecond_;
	const int64_t adjustedRemainingInTimeScale = (remainingInTimeScale * nanosecondsPerSecond_) / timeDenominator;

	const int64_t timeValueNs = adjustedSeconds + adjustedRemainingInTimeScale;

#ifdef OCEAN_DEBUG
	{
		const double input = double(timeValue) / double(timeDenominator);
		const double output = double(timeValueNs) / double(nanosecondsPerSecond_);
		const double difference = std::abs(input - output);
		ocean_assert(difference < 0.0001);
	}
#endif // OCEAN_DEBUG

	return timeValueNs;
}

#ifndef OCEAN_PLATFORM_BUILD_WINDOWS

int64_t Timestamp::TimestampConverter::currentTimestampNs(const int posixClockId)
{
	struct timespec timestampSpec;
	if (clock_gettime(clockid_t(posixClockId), &timestampSpec) != 0)
	{
		return invalidValue_;
	}

	return timestampSpec.tv_sec * nanosecondsPerSecond_ + timestampSpec.tv_nsec;
}

int Timestamp::TimestampConverter::posixClockId(const TimeDomain timeDomain)
{
	switch (timeDomain)
	{
		case TD_MONOTONIC:
			return CLOCK_MONOTONIC;

	#ifdef OCEAN_BASE_TIMESTAMP_BOOTTIME_AVAILABLE
		case TD_BOOTTIME:
			return CLOCK_BOOTTIME;
	#endif

	#ifdef OCEAN_BASE_TIMESTAMP_UPTIMERAW_AVAILABLE
		case TD_UPTIME_RAW:
			return CLOCK_UPTIME_RAW;
	#endif

		case TD_INVALID:
			break;
	}

	ocean_assert(false && "Invalid time domain!");
	return -1;
}

#endif // OCEAN_PLATFORM_BUILD_WINDOWS

Timestamp::Timestamp(const bool toNow) :
	value_(toNow ? DateTime::timestamp() : invalidTimestampValue())
{
	// nothing to do here
}

Timestamp& Timestamp::toNow()
{
	value_ = DateTime::timestamp();

	return *this;
}

}
