/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/RateCalculator.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

void RateCalculator::addOccurance(const Timestamp& timestamp, const double quantity)
{
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	// we remove all occurrences far outside of the sliding window
	timestampMap_.erase(timestampMap_.begin(), timestampMap_.upper_bound(Timestamp(timestamp - window_ * 5.0)));

	// we allow several occurrences at the same time, so we just increase the counter
	timestampMap_[timestamp] += quantity;
}

double RateCalculator::rate(const Timestamp& timestamp) const
{
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	const Timestamp oldestTimestamp(timestamp - window_); // exclusive

	const TimestampMap::const_iterator iEnd = timestampMap_.upper_bound(timestamp); // exclusive

	double rangeSum = 0.0;
	for (TimestampMap::const_iterator i = timestampMap_.upper_bound(oldestTimestamp); i != iEnd; ++i)
	{
		rangeSum += i->second;
	}

	ocean_assert(NumericD::isNotEqualEps(window_));
	return rangeSum / window_;
}

bool RateCalculator::rateEveryNSeconds(const Timestamp& rateTimestamp, double& rate, const double interval, const Timestamp& requestTimestamp)
{
	ocean_assert(rateTimestamp.isValid());
	ocean_assert(interval >= 0.0);

	const Timestamp& timestampToUse = requestTimestamp.isValid() ? requestTimestamp : rateTimestamp;

	const ScopedLock scopedLock(lock_);

	if (lastRequestTimestamp_.isInvalid())
	{
		lastRequestTimestamp_ = timestampToUse;
	}

	if (timestampToUse < lastRequestTimestamp_ + interval)
	{
		return false;
	}

	lastRequestTimestamp_ = timestampToUse;
	rate = RateCalculator::rate(rateTimestamp);

	return true;
}

}
