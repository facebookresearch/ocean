/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/math/Interpolation.h"

namespace Ocean
{

namespace Tracking
{

SmoothedTransformation::SmoothedTransformation(const double smoothingInterval)
{
	reset();

	ocean_assert(smoothingInterval > NumericD::eps());
	smoothingInterval_ = smoothingInterval;
}

void SmoothedTransformation::setTransformation(const HomogenousMatrix4& transformation, const Timestamp& timestamp)
{
	ocean_assert(transformation.isValid() && timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	ocean_assert(oldTimestamp_.isInvalid() || timestamp >= oldTimestamp_);
	ocean_assert(newTimestamp_.isInvalid() || timestamp >= newTimestamp_);

	if (oldTransformation_.isValid())
	{
		// we replace the old transformation by the current (smoothed) transformation

		oldTransformation_ = this->transformation(timestamp);

		newTransformation_ = transformation;
		newTimestamp_ = timestamp;
	}
	else
	{
		ocean_assert(!newTransformation_.isValid());
		ocean_assert(!newTimestamp_.isValid());

		oldTransformation_ = transformation;
		oldTimestamp_ = timestamp;
	}
}

HomogenousMatrix4 SmoothedTransformation::transformation(const Timestamp& timestamp) const
{
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	//                                                                            new transformation
	//       old transformation                 new transformation               + smoothing interval
	//               |                                   |                                 |
	//               V                                   V                                 V
	// ---------------------------------------------------------------------------------------------------------------------
	//              the old transformation               |  the smoothed transformation    |  the new transformation
	//                  will be reported                 |         will be reported        |     will be reported

	if (!newTimestamp_.isValid() || timestamp <= newTimestamp_)
	{
		return oldTransformation_;
	}

	if (timestamp >= newTimestamp_ + smoothingInterval_)
	{
		return newTransformation_;
	}

	ocean_assert(NumericD::isNotEqualEps(smoothingInterval_));
	const Scalar interpolationFactor = Scalar(double(timestamp - newTimestamp_) / smoothingInterval_);
	ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);

	// we apply a 12-DOF linear interpolation
	// 3D translation, 3D orientation (slerp), 3D scale, 3D shear

	return Interpolation::linear(oldTransformation_, newTransformation_, interpolationFactor);
}

void SmoothedTransformation::reset()
{
	const ScopedLock scopedLock(lock_);

	oldTransformation_.toNull();
	newTransformation_.toNull();

	oldTimestamp_.toInvalid();
	newTimestamp_.toInvalid();
}

}

}
