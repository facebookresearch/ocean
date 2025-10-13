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

Timestamp Timestamp::TimestampConverter::toUnix(const int64_t domainTimestampNs)
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return Timestamp(false);
	}

	int64_t domainToUnixOffsetNs = domainToUnixOffsetNs_;

	if (domainToUnixOffsetNs == invalidValue_)
	{
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

	#ifdef OCEAN_BASE_TIMESTAMP_BOOTTIME_AVAILABLE
		ocean_assert(timeDomain_ == TD_MONOTONIC || timeDomain_ == TD_BOOTTIME);
		const clockid_t domainClockId = timeDomain_ == TD_MONOTONIC ? CLOCK_MONOTONIC : CLOCK_BOOTTIME;
	#else
		ocean_assert(timeDomain_ == TD_MONOTONIC);
		constexpr clockid_t domainClockId = CLOCK_MONOTONIC;
	#endif

		struct timespec domainTimestampSpec;
		struct timespec unixTimestampSpec;

		if (measurements_ % 2 == 0)
		{
			const int resultDomain = clock_gettime(domainClockId, &domainTimestampSpec);
			const int resultUnix = clock_gettime(CLOCK_REALTIME, &unixTimestampSpec);

			ocean_assert_and_suppress_unused(resultDomain == 0, resultDomain);
			ocean_assert_and_suppress_unused(resultUnix == 0, resultUnix);
		}
		else
		{
			const int resultUnix = clock_gettime(CLOCK_REALTIME, &unixTimestampSpec);
			const int resultDomain = clock_gettime(domainClockId, &domainTimestampSpec);

			ocean_assert_and_suppress_unused(resultUnix == 0, resultUnix);
			ocean_assert_and_suppress_unused(resultDomain == 0, resultDomain);
		}

		const int64_t domainNs = domainTimestampSpec.tv_sec * 1000000000ll + domainTimestampSpec.tv_nsec;
		const int64_t unixNs = unixTimestampSpec.tv_sec * 1000000000ll + unixTimestampSpec.tv_nsec;

#endif // OCEAN_PLATFORM_BUILD_WINDOWS

		/*
		 * We calculate the unix timestamp of the equivalent domain timestamp by gathering the unix and domain timestamp at the same point in time
		 * unix =
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

		domainToUnixOffsetNs = initialDomainToUnixOffsetNs + averageRelativeDomainToUnixOffsetNs;

		if (measurements_ >= necessaryMeasurements_)
		{
			domainToUnixOffsetNs_ = domainToUnixOffsetNs;
		}

#ifdef OCEAN_INTENSIVE_DEBUG
		Log::debug() << " ";
		Log::debug() << "TimestampConverter:";
		Log::debug() << "Measurement iteration: " << measurements_;
		Log::debug() << "Input time: " << domainTimestampNs;
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
	}

	const int64_t unixTimestampNs = domainTimestampNs + domainToUnixOffsetNs;

	return Timestamp(Timestamp::nanoseconds2seconds(unixTimestampNs));
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
	}

	return *this;
}

int64_t Timestamp::TimestampConverter::currentTimestampNs(const TimeDomain timeDomain)
{
#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

	ocean_assert(timeDomain == TD_MONOTONIC);

	const std::chrono::steady_clock::time_point domainTimestampTimePoint = std::chrono::steady_clock::now();

	return std::chrono::duration_cast<std::chrono::nanoseconds>(domainTimestampTimePoint.time_since_epoch()).count();

#else

	#ifdef OCEAN_BASE_TIMESTAMP_BOOTTIME_AVAILABLE
		ocean_assert(timeDomain == TD_MONOTONIC || timeDomain == TD_BOOTTIME);
		const clockid_t domainClockId = timeDomain == TD_MONOTONIC ? CLOCK_MONOTONIC : CLOCK_BOOTTIME;
	#else
		ocean_assert(timeDomain == TD_MONOTONIC);
		constexpr clockid_t domainClockId = CLOCK_MONOTONIC;
	#endif

	struct timespec timestampSpec;
	const int result = clock_gettime(domainClockId, &timestampSpec);
	ocean_assert_and_suppress_unused(result == 0, result);

	return timestampSpec.tv_sec * 1000000000ll + timestampSpec.tv_nsec;

#endif // OCEAN_PLATFORM_BUILD_WINDOWS
}

#ifndef OCEAN_PLATFORM_BUILD_WINDOWS

int64_t Timestamp::TimestampConverter::currentTimestampNs(const int posixClockId)
{
	struct timespec timestampSpec;
	if (clock_gettime(clockid_t(posixClockId), &timestampSpec) != 0)
	{
		return invalidValue_;
	}

	return timestampSpec.tv_sec * 1000000000ll + timestampSpec.tv_nsec;
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
