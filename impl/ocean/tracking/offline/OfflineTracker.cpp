/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/OfflineTracker.h"

#include "ocean/base/Median.h"

#include "ocean/tracking/MotionModel.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

unsigned int OfflineTracker::idCounter_ = 0u;

Lock& OfflineTracker::idCounterLock()
{
	static Lock idCounterLock;
	return idCounterLock;
}

OfflineTracker::OfflineTracker() :
	Thread("OfflineTracker Thread")
{
	const ScopedLock scopedLock(idCounterLock());

	id_ = ++idCounter_;
}

OfflineTracker::~OfflineTracker()
{
	ocean_assert(eventCallbacks_.isEmpty());
}

bool OfflineTracker::running() const
{
	return isThreadActive();
}

bool OfflineTracker::finished() const
{
	return finished_;
}

OfflinePoses OfflineTracker::poses() const
{
	const ScopedLock scopedLock(lock_);
	return offlinePoses_;
}

OfflinePose OfflineTracker::pose(const unsigned int index) const
{
	const ScopedLock scopedLock(lock_);

	if (offlinePoses_.isValidIndex(OfflinePoses::Index(index)))
	{
		return offlinePoses_[OfflinePoses::Index(index)];
	}

	return OfflinePose();
}

bool OfflineTracker::start()
{
	const ScopedLock scopedLock(lock_);

	previousProcessProgress_ = Scalar(0);

	finished_ = false;
	shouldStop_ = false;

	if (isThreadInvokedToStart())
	{
		return true;
	}

	return startThread();
}

bool OfflineTracker::stop(const unsigned int timeout)
{
	shouldStop_ = true;
	stopThread();

	if (timeout == 0u)
	{
		return true;
	}

	return joinThread(timeout);
}

bool OfflineTracker::isPlausibleCamera(const PinholeCamera& pinholeCamera, const CameraModel model)
{
	ocean_assert(pinholeCamera.isValid());

	// the principal point should lie close to the ideal principal point
	const Scalar idealPrincipalX = Scalar(pinholeCamera.width()) * Scalar(0.5);
	const Scalar idealPrincipalY = Scalar(pinholeCamera.height()) * Scalar(0.5);
	const Scalar principalOffsetX = Numeric::abs(pinholeCamera.principalPointX() - idealPrincipalX);
	const Scalar principalOffsetY = Numeric::abs(pinholeCamera.principalPointY() - idealPrincipalY);
	const Scalar principalOffsetRelativeX = principalOffsetX / Scalar(pinholeCamera.width());
	const Scalar principalOffsetRelativeY = principalOffsetY / Scalar(pinholeCamera.height());

	// the horizontal focal length should lie close to the vertical focal length
	const Scalar focalLengthFactor = Scalar(1) - ((pinholeCamera.focalLengthX() > pinholeCamera.focalLengthY()) ? (pinholeCamera.focalLengthY() / pinholeCamera.focalLengthX()) : (pinholeCamera.focalLengthX() / pinholeCamera.focalLengthY()));

	switch (model)
	{
		case CM_LOW_QUALITY:
			return principalOffsetRelativeX <= Scalar(0.2) && principalOffsetRelativeY <= Scalar(0.2) && focalLengthFactor <= Scalar(0.2);

		case CM_MEDIUM_QUALITY:
			return principalOffsetRelativeX <= Scalar(0.1) && principalOffsetRelativeY <= Scalar(0.1) && focalLengthFactor <= Scalar(0.1);

		case CM_HIGH_QUALITY:
			return principalOffsetRelativeX <= Scalar(0.05) && principalOffsetRelativeY <= Scalar(0.05) && focalLengthFactor <= Scalar(0.05);

		default:
			break;
	}

	ocean_assert(false && "Invalid camera model!");
	return false;
}

void OfflineTracker::updateTrackerProgress(const Scalar localProgress)
{
	ocean_assert(localProgress >= Scalar(0) && localProgress <= Scalar(1));

	Scalar progress = localProgress;

	TemporaryScopedLock scopedLock(eventStackLock_);

	if (eventStack_.empty())
	{
		return;
	}

	for (EventStack::const_reverse_iterator i = eventStack_.rbegin(); i != eventStack_.rend(); ++i)
	{
		progress = i->startProgress() + (i->stopProgress() - i->startProgress()) * progress;
	}

	if (Numeric::isEqual(previousProcessProgress_, progress))
	{
		return;
	}

	previousProcessProgress_ = progress;

	scopedLock.release();

	ocean_assert(progress >= Scalar(0) && progress <= Scalar(1));
	const unsigned int iProgress = (unsigned int)(progress * Scalar(100) + Scalar(0.5));

	ocean_assert(iProgress >= 0u && iProgress <= 100u);

	if (iProgress <= 100u)
	{
		eventCallbacks_(TrackerProcessProgressEvent(id_, iProgress));
	}
}

void OfflineTracker::pushProgressEventStackLayer(const EventStackLayer& layer)
{
	const ScopedLock scopedLock(eventStackLock_);
	eventStack_.push_back(layer);
}

void OfflineTracker::popProgressEventStackLayer(const EventStackLayer& layer)
{
	const ScopedLock scopedLock(eventStackLock_);

	ocean_assert(!eventStack_.empty());
	if (!eventStack_.empty())
	{
		ocean_assert_and_suppress_unused(eventStack_.back() == layer, layer);
		eventStack_.pop_back();
	}
}

void OfflineTracker::updatePoses(const OfflinePoses& poses)
{
	const ScopedLock scopedLock(lock_);

	offlinePoses_ = poses;
	eventCallbacks_(TrackerPosesStateEvent(id_, offlinePoses_));
}

void OfflineTracker::removeIrregularPoses(const Scalar factor)
{
	ocean_assert(factor > Numeric::eps());

	Scalars angles;
	angles.reserve(offlinePoses_.size());

	for (OfflinePoses::Index n = offlinePoses_.firstIndex(); n < offlinePoses_.endIndex() - 1; ++n)
	{
		if (offlinePoses_[n].isValid() && offlinePoses_[n + 1].isValid())
		{
			angles.push_back(offlinePoses_[n].transformation().rotation().angle(offlinePoses_[n + 1].transformation().rotation()));
		}
	}

	if (angles.empty())
	{
		return;
	}

	const Scalar angleThreshold = Median::median(angles.data(), angles.size()) * factor;

	// forward pass
	for (OfflinePoses::Index n = offlinePoses_.firstIndex(); n < offlinePoses_.endIndex() - 1; ++n)
	{
		if (offlinePoses_[n].isValid() && offlinePoses_[n + 1].isValid() && (n == offlinePoses_.firstIndex() || !offlinePoses_[n - 1].isValid()))
		{
			const Scalar localAngle = offlinePoses_[n].transformation().rotation().angle(offlinePoses_[n + 1].transformation().rotation());

			if (localAngle > angleThreshold)
			{
				offlinePoses_[n].setTransformation(HomogenousMatrix4(false));
				offlinePoses_[n].setQuality(Scalar(-1));
			}
		}
	}

	// backward pass
	for (OfflinePoses::Index n = offlinePoses_.lastIndex(); n > offlinePoses_.firstIndex(); --n)
	{
		if (offlinePoses_[n].isValid() && offlinePoses_[n - 1].isValid() && (n == offlinePoses_.lastIndex() || !offlinePoses_[n + 1].isValid()))
		{
			const Scalar localAngle = offlinePoses_[n].transformation().rotation().angle(offlinePoses_[n - 1].transformation().rotation());

			if (localAngle > angleThreshold)
			{
				offlinePoses_[n].setTransformation(HomogenousMatrix4(false));
				offlinePoses_[n].setQuality(Scalar(-1));
			}
		}
	}
}

void OfflineTracker::extrapolatePoses(const unsigned int number, const unsigned int base)
{
	ocean_assert(number != 0u);

	OfflinePoses extrapolatedPoses(offlinePoses_);

	for (OfflinePoses::Index i = offlinePoses_.firstIndex(); i < offlinePoses_.endIndex(); ++i)
	{
		if (!offlinePoses_[i].isValid())
		{
			// check for a single invalid pose enclose by two valid poses
			if (i > offlinePoses_.firstIndex() && i < offlinePoses_.lastIndex() && offlinePoses_[i - 1].isValid() && offlinePoses_[i + 1].isValid())
			{
				extrapolateSinglePose(i, extrapolatedPoses);
				continue;
			}

			// check for at least two valid poses on the right with a large gap of invalid poses on the left
			if (i + 2 <= offlinePoses_.lastIndex() && offlinePoses_[i + 1].isValid() && offlinePoses_[i + 2].isValid())
			{
				bool largeGap = true;

				for (OfflinePoses::Index n = i - 1; largeGap && n >= offlinePoses_.firstIndex() && n > i - OfflinePoses::Index(number * 2); --n)
				{
					if (offlinePoses_[n].isValid())
					{
						largeGap = false;
					}
				}

				if (largeGap)
				{
					extrapolateLeftPoses(i, number, base, extrapolatedPoses);
					continue;
				}
			}

			// check for at least two valid poses on the left with a large gap of invalid poses on the right
			if (i - 2 >= offlinePoses_.firstIndex() && offlinePoses_[i - 1].isValid() && offlinePoses_[i - 2].isValid())
			{
				bool largeGap = true;

				for (OfflinePoses::Index n = i + 1; largeGap && n <= offlinePoses_.lastIndex() && n < i + OfflinePoses::Index(number * 2); ++n)
				{
					if (offlinePoses_[n].isValid())
					{
						largeGap = false;
					}
				}

				if (largeGap)
				{
					extrapolateRightPoses(i, number, base, extrapolatedPoses);
					continue;
				}
			}

			// check for at least two valid poses on the left or two valid poses on the right
			if (!extrapolatedPoses[i].isValid() && ((i + 2 <= offlinePoses_.lastIndex() && offlinePoses_[i + 1].isValid() && offlinePoses_[i + 2].isValid())
					|| (i - 2 >= offlinePoses_.firstIndex() && offlinePoses_[i - 1].isValid() && offlinePoses_[i - 2].isValid())))
			{
				extrapolateCenterPoses(i, number, base, extrapolatedPoses);
				continue;
			}
		}
	}

	offlinePoses_ = extrapolatedPoses;
}

void OfflineTracker::extrapolateSinglePose(const OfflinePoses::Index invalidIndex, OfflinePoses& offlinePoses)
{
	ocean_assert(offlinePoses.isValidIndex(invalidIndex));
	ocean_assert(!offlinePoses[invalidIndex].isValid());

	ocean_assert(invalidIndex > offlinePoses.firstIndex());
	ocean_assert(invalidIndex < offlinePoses.lastIndex());

	ocean_assert(offlinePoses[invalidIndex - 1].isValid());
	ocean_assert(offlinePoses[invalidIndex + 1].isValid());

	const Vector3 translation0(offlinePoses[invalidIndex - 1].transformation().translation());
	const Vector3 translation1(offlinePoses[invalidIndex + 1].transformation().translation());

	const Quaternion rotation0(offlinePoses[invalidIndex - 1].transformation().rotation());
	const Quaternion rotation1(offlinePoses[invalidIndex + 1].transformation().rotation());

	offlinePoses[invalidIndex] = OfflinePose((unsigned int)invalidIndex, HomogenousMatrix4(Interpolation::linear(std::make_pair(translation0, rotation0), std::make_pair(translation1, rotation1), Scalar(0.5))));
}

void OfflineTracker::extrapolateLeftPoses(const OfflinePoses::Index invalidIndex, const unsigned int number, const unsigned int base, OfflinePoses& offlinePoses)
{
	ocean_assert(base > 0u && number > 0u);

	ocean_assert(offlinePoses.isValidIndex(invalidIndex));
	ocean_assert(!offlinePoses[invalidIndex].isValid());

	ocean_assert(invalidIndex + 2 <= offlinePoses.lastIndex());
	ocean_assert(offlinePoses[invalidIndex + 1].isValid());
	ocean_assert(offlinePoses[invalidIndex + 2].isValid());

	// check how many poses can be used for extrapolation
	OfflinePoses::Index lastValidIndex = invalidIndex;

	for (OfflinePoses::Index n = invalidIndex + 1; n < offlinePoses.endIndex() && n < invalidIndex + OfflinePoses::Index(base); ++n)
	{
		if (offlinePoses[n].isValid())
		{
			lastValidIndex = n;
		}
		else
		{
			break;
		}
	}

	ocean_assert(lastValidIndex > invalidIndex);

	HomogenousMatrices4 basePoses;
	basePoses.reserve(base);

	for (OfflinePoses::Index i = invalidIndex; i >= offlinePoses.firstIndex() && i > invalidIndex - OfflinePoses::Index(number); --i)
	{
		basePoses.clear();

		for (OfflinePoses::Index n = i + 1; n <= lastValidIndex && basePoses.size() < base; ++n)
		{
			ocean_assert(offlinePoses[n].isValid());
			basePoses.push_back(offlinePoses[n].transformation());
		}

		ocean_assert(!basePoses.empty());
		offlinePoses[i] = OfflinePose((unsigned int)i, MotionModel::predictPose(basePoses));
	}
}

void OfflineTracker::extrapolateRightPoses(const OfflinePoses::Index invalidIndex, const unsigned int number, const unsigned int base, OfflinePoses& offlinePoses)
{
	ocean_assert(base > 0u && number > 0u);

	ocean_assert(offlinePoses.isValidIndex(invalidIndex));
	ocean_assert(!offlinePoses[invalidIndex].isValid());

	ocean_assert(invalidIndex - 2 >= offlinePoses.firstIndex());
	ocean_assert(offlinePoses[invalidIndex - 1].isValid());
	ocean_assert(offlinePoses[invalidIndex - 2].isValid());

	// check how many poses can be used for extrapolation
	OfflinePoses::Index lastValidIndex = invalidIndex;

	for (OfflinePoses::Index n = invalidIndex - 1; n >= offlinePoses.firstIndex() && n >= invalidIndex - OfflinePoses::Index(base); --n)
	{
		if (offlinePoses[n].isValid())
		{
			lastValidIndex = n;
		}
		else
		{
			break;
		}
	}

	ocean_assert(lastValidIndex < invalidIndex);

	HomogenousMatrices4 basePoses;
	basePoses.reserve(base);

	for (OfflinePoses::Index i = invalidIndex; i <= offlinePoses.lastIndex() && i < invalidIndex + OfflinePoses::Index(number); ++i)
	{
		basePoses.clear();

		for (OfflinePoses::Index n = i - 1; n >= lastValidIndex && basePoses.size() < base; --n)
		{
			ocean_assert(offlinePoses[n].isValid());
			basePoses.push_back(offlinePoses[n].transformation());
		}

		ocean_assert(!basePoses.empty());
		offlinePoses[i] = OfflinePose((unsigned int)i, MotionModel::predictPose(basePoses));
	}
}

void OfflineTracker::extrapolateCenterPoses(const OfflinePoses::Index invalidIndex, const unsigned int number, const unsigned int base, OfflinePoses& offlinePoses)
{
	ocean_assert(offlinePoses.isValidIndex(invalidIndex));
	ocean_assert(!offlinePoses[invalidIndex].isValid());

	ocean_assert(invalidIndex > offlinePoses.firstIndex());
	ocean_assert(invalidIndex < offlinePoses.lastIndex());

	OfflinePoses::Index leftValidIndex = -1;
	OfflinePoses::Index rightValidIndex = -1;

	// check whether we are at the left border or whether we are at the right border
	if (offlinePoses[invalidIndex - 1].isValid())
	{
		leftValidIndex = invalidIndex - 1;

		for (OfflinePoses::Index i = invalidIndex; i < invalidIndex + OfflinePoses::Index(number * 2u); ++i)
		{
			if (offlinePoses[i].isValid())
			{
				rightValidIndex = i;
				break;
			}
		}

		ocean_assert(rightValidIndex != -1);
	}
	else
	{
		ocean_assert(offlinePoses[invalidIndex + 1].isValid());

		rightValidIndex = invalidIndex + 1;

		for (OfflinePoses::Index i = invalidIndex; i > invalidIndex - OfflinePoses::Index(number * 2u); --i)
		{
			if (offlinePoses[i].isValid())
			{
				leftValidIndex = i;
				break;
			}
		}

		ocean_assert(leftValidIndex != -1);
	}

	ocean_assert(leftValidIndex != -1);
	ocean_assert(rightValidIndex != -1);

	ocean_assert(offlinePoses[leftValidIndex].isValid());
	ocean_assert(offlinePoses[rightValidIndex].isValid());

	ocean_assert(leftValidIndex - 1 >= offlinePoses.firstIndex());
	ocean_assert(rightValidIndex + 1 <= offlinePoses.lastIndex());

	const size_t invalidPoses = rightValidIndex - leftValidIndex - 1;

	OfflinePoses leftPoses(leftValidIndex + 1, invalidPoses), rightPoses(leftValidIndex + 1, invalidPoses);

	// check whether we have two valid poses on the left
	if (offlinePoses[leftValidIndex - 1].isValid())
	{
		// check how many poses can be used for extrapolation
		OfflinePoses::Index lastValidIndex = leftValidIndex - 1;

		for (OfflinePoses::Index n = lastValidIndex - 1; n >= offlinePoses.firstIndex() && n >= leftValidIndex - OfflinePoses::Index(base) + 1; --n)
		{
			if (offlinePoses[n].isValid())
			{
				lastValidIndex = n;
			}
			else
			{
				break;
			}
		}

		ocean_assert(lastValidIndex < leftValidIndex);

		HomogenousMatrices4 basePoses;
		basePoses.reserve(base);

		for (OfflinePoses::Index i = leftValidIndex + 1; i <= offlinePoses.lastIndex() && i < leftValidIndex + OfflinePoses::Index(number) + 1 && i < rightValidIndex; ++i)
		{
			basePoses.clear();

			for (OfflinePoses::Index n = i - 1; n >= lastValidIndex && basePoses.size() < base; --n)
			{
				if (offlinePoses[n].isValid())
				{
					basePoses.emplace_back(offlinePoses[n].transformation());
				}
				else
				{
					ocean_assert(leftPoses.isValidIndex(n) && leftPoses[n].isValid());
					basePoses.emplace_back(leftPoses[n].transformation());
				}
			}

			ocean_assert(!basePoses.empty());
			leftPoses[i] = OfflinePose((unsigned int)i, MotionModel::predictPose(basePoses));
		}
	}

	// check whether we have two valid poses on the right
	if (offlinePoses[rightValidIndex + 1].isValid())
	{
		// check how many poses can be used for extrapolation
		OfflinePoses::Index lastValidIndex = rightValidIndex + 1;

		for (OfflinePoses::Index n = lastValidIndex + 1; n < offlinePoses.endIndex() && n <= rightValidIndex + OfflinePoses::Index(base) - 1; ++n)
		{
			if (offlinePoses[n].isValid())
			{
				lastValidIndex = n;
			}
			else
			{
				break;
			}
		}

		ocean_assert(lastValidIndex > rightValidIndex);

		HomogenousMatrices4 basePoses;
		basePoses.reserve(base);

		for (OfflinePoses::Index i = rightValidIndex - 1; i >= offlinePoses.firstIndex() && i > rightValidIndex - OfflinePoses::Index(number) - 1 && i > leftValidIndex; --i)
		{
			basePoses.clear();

			for (OfflinePoses::Index n = i + 1; n <= lastValidIndex && basePoses.size() < base; ++n)
			{
				if (offlinePoses[n].isValid())
				{
					basePoses.emplace_back(offlinePoses[n].transformation());
				}
				else
				{
					ocean_assert(rightPoses.isValidIndex(n) && rightPoses[n].isValid());
					basePoses.emplace_back(rightPoses[n].transformation());
				}
			}

			ocean_assert(!basePoses.empty());
			rightPoses[i] = OfflinePose((unsigned int)i, MotionModel::predictPose(basePoses));
		}
	}

	for (OfflinePoses::Index i = leftPoses.firstIndex(); i < leftPoses.endIndex(); ++i)
	{
		ocean_assert(leftPoses[i].isValid() || rightPoses[i].isValid());

		if (leftPoses[i].isValid() && rightPoses[i].isValid())
		{
			ocean_assert(leftPoses.size() > 1);
			const Scalar interpolationFactor = Scalar(i - leftPoses.firstIndex()) / Scalar(leftPoses.size() - 1);
			ocean_assert(interpolationFactor >= 0 && interpolationFactor <= 1);

			const Vector3 translation0(leftPoses[i].transformation().translation());
			const Vector3 translation1(rightPoses[i].transformation().translation());

			const Quaternion rotation0(leftPoses[i].transformation().rotation());
			const Quaternion rotation1(rightPoses[i].transformation().rotation());

			offlinePoses[i] = OfflinePose((unsigned int)(i), HomogenousMatrix4(Interpolation::linear(std::make_pair(translation0, rotation0), std::make_pair(translation1, rotation1), interpolationFactor)));
		}
		else if (leftPoses[i].isValid())
		{
			offlinePoses[i] = leftPoses[i];
		}
		else
		{
			ocean_assert(rightPoses[i].isValid());
			offlinePoses[i] = rightPoses[i];
		}
	}
}

}

}

}
