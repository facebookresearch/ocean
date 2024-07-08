/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_RATE_CALCUALTOR_H
#define META_OCEAN_MATH_RATE_CALCUALTOR_H

#include "ocean/math/Math.h"

#include "ocean/base/Timestamp.h"

namespace Ocean
{

/**
 * This class implements a calculate for rates like frame rates.
 * The rates are determined with a sliding windows.<br>
 * The class is thread-safe.
 * @ingroup math
 */
class OCEAN_MATH_EXPORT RateCalculator
{
	protected:

		/**
		 * Definition of an ordered map mapping timestamps to quantities.
		 */
		typedef std::map<Timestamp, double> TimestampMap;

	public:

		/**
		 * Creates a new rate calculator object.
		 * @param window The size of the sliding window, in seconds, with range (0, infinity)
		 */
		explicit inline RateCalculator(const double window = 1.0);

		/**
		 * Adds another occurrence (e.g., a new frame has been processed).
		 * @param timestamp The timestamp at which the occurrence happened, must be valid
		 * @param quantity The quantity of the current occurrence
		 */
		void addOccurance(const Timestamp& timestamp, const double quantity = 1.0);

		/**
		 * Returns the current rate.
		 * @param timestamp The timestamp at which the current rate will be determined, must be valid
		 * @return The current rate, with range [0, infinity)
		 */
		double rate(const Timestamp& timestamp) const;

		/**
		 * Returns the current rate only every n-th second.
		 * @param rateTimestamp The timestamp at which the current rate will be determined, must be valid
		 * @param rate The resulting current rate, with range [0, infinity)
		 * @param interval The number of seconds necessary since the last successful call of this function, with range [0, infinity)
		 * @param requestTimestamp Optional an explicit timestamp when the request happens, e.g., in display time, invalid to use the timestamp for the rate
		 * @return True, if the rate was determined; False, if the interval was not yet reached
		 */
		bool rateEveryNSeconds(const Timestamp& rateTimestamp, double& rate, const double interval = 1.0, const Timestamp& requestTimestamp = Timestamp(false));

		/**
		 * Updates the window of this rate calculator.
		 * @param window The window to be set, in seconds, with range [0, infinity)
		 */
		inline void setWindow(const double window);

		/**
		 * Returns the window of this rate calculator.
		 * @return The rate calculator's window, in seconds, with range [0, infinity)
		 */
		inline double window() const;

		/**
		 * Clears the rate calculator e.g., to start with a completely new measurement
		 */
		inline void clear();

	protected:

		/// The size of the sliding window, in seconds, with range (0, infinity).
		double window_ = 1.0;

		/// The map mapping timestamps to quantities.
		TimestampMap timestampMap_;

		/// The timestamp at which the rate has been sucessfully requested the last time when calling rateEveryNSeconds().
		Timestamp lastRequestTimestamp_;

		/// The lock of this object.
		mutable Lock lock_;
};

inline RateCalculator::RateCalculator(const double window) :
	window_(window),
	lastRequestTimestamp_(false)
{
	ocean_assert(window_ > 0.0);
}

inline void RateCalculator::setWindow(const double window)
{
	ocean_assert(window > 0.0);

	const ScopedLock scopedLock(lock_);

	window_ = window;
}

inline double RateCalculator::window() const
{
	const ScopedLock scopedLock(lock_);

	return window_;
}

inline void RateCalculator::clear()
{
	const ScopedLock scopedLock(lock_);

	timestampMap_.clear();
	lastRequestTimestamp_.toInvalid();
}

}

#endif // META_OCEAN_MATH_RATE_CALCUALTOR_H
