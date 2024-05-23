/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/SLAMTracker.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Maintenance.h"
#include "ocean/base/Scheduler.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/geometry/Grid.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/io/Utilities.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Random.h"
#include "ocean/math/SuccessionSubset.h"

#include "ocean/media/Utilities.h"

#include "ocean/tracking/Solver3.h"
#include "ocean/tracking/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

SLAMTracker::~SLAMTracker()
{
	stop();
}

bool SLAMTracker::start()
{
	if (!FrameTracker::start())
	{
		return false;
	}

	// we state the function as object as the register and unregister call may be invoked from individual processes/libraries
	if (schedulerCallback_.isNull())
	{
		schedulerCallback_ = Scheduler::Callback::create(*this, &SLAMTracker::onScheduler);
		Scheduler::get().registerFunction(schedulerCallback_);
	}

	return true;
}

bool SLAMTracker::stop(const unsigned int timeout)
{
	if (schedulerCallback_)
	{
		Scheduler::get().unregisterFunction(schedulerCallback_);
		schedulerCallback_ = Scheduler::Callback();
	}

	return FrameTracker::stop(timeout);
}

SLAMTracker::TrackingQuality SLAMTracker::trackingQuality() const
{
	return trackingQuality_;
}

PinholeCamera::OptimizationStrategy SLAMTracker::cameraOptimizationStrategy() const
{
	return cameraOptimizationStrategy_;
}

Scalar SLAMTracker::cameraFieldOfView() const
{
	return cameraFieldOfView_;
}

SLAMTracker::AbstractMotionType SLAMTracker::abstractMotionType() const
{
	return abstractMotionType_;
}

PointPaths::MotionSpeed SLAMTracker::motionSpeed() const
{
	return motionSpeed_;
}

bool SLAMTracker::setTrackingQuality(const TrackingQuality trackingQuality)
{
	if (!finished_)
	{
		return false;
	}

	trackingQuality_ = trackingQuality;
	return true;
}

bool SLAMTracker::setCameraOptimizationStrategy(const PinholeCamera::OptimizationStrategy optimizationStrategy)
{
	if (!finished_)
	{
		return false;
	}

	cameraOptimizationStrategy_ = optimizationStrategy;
	return true;
}

bool SLAMTracker::setCameraFieldOfView(const Scalar fovX)
{
	if (!finished_)
	{
		return false;
	}

	if (fovX <= Numeric::eps() || fovX > Numeric::pi() - Numeric::eps())
	{
		return false;
	}

	cameraFieldOfView_ = fovX;
	return true;
}

bool SLAMTracker::setAbstractMotionType(const AbstractMotionType abstractMotionType)
{
	if (!finished_)
	{
		return false;
	}

	abstractMotionType_ = abstractMotionType;
	return true;
}

bool SLAMTracker::setMotionSpeed(const PointPaths::MotionSpeed motionSpeed)
{
	if (!finished_)
	{
		return false;
	}

	motionSpeed_ = motionSpeed;
	return true;
}

bool SLAMTracker::setRegionOfInterest(const CV::SubRegion& regionOfInterest, const bool soleApplication)
{
	const ScopedLock scopedLock(lock_);

	if (running())
	{
		return false;
	}

	regionOfInterest_ = regionOfInterest;
	soleRegionOfInterestApplication_ = soleApplication;

	return true;
}

bool SLAMTracker::extractPoses(const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, OfflinePoses& offlinePoses, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* finalAverageError, Worker* worker, bool* abort) const
{
	ocean_assert(lowerFrameIndex <= upperFrameIndex);

	RandomGenerator randomGenerator;

	ShiftVector<HomogenousMatrix4> poses;
	if (!Solver3::determinePoses(database_, AnyCameraPinhole(camera_), cameraMotion_, IndexSet32(), false, randomGenerator, lowerFrameIndex, upperFrameIndex, minimalCorrespondences, poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, finalAverageError, worker, abort))
	{
		return false;
	}

	ocean_assert(NumericT<ptrdiff_t>::isInsideValueRange(lowerFrameIndex));
	ocean_assert(NumericT<ptrdiff_t>::isInsideValueRange(upperFrameIndex));

	ocean_assert(poses.firstIndex() == ptrdiff_t(lowerFrameIndex) && poses.lastIndex() == ptrdiff_t(upperFrameIndex));

	offlinePoses = OfflinePoses(lowerFrameIndex, poses.size());
	for (unsigned int n = lowerFrameIndex; n <= upperFrameIndex; ++n)
	{
		offlinePoses[n] = OfflinePose(n, poses[n]);
	}

	return true;
}

bool SLAMTracker::extractPoses(const unsigned int lowerFrameIndex, const unsigned int regionOfInterestIndex, const unsigned int upperFrameIndex, const CV::SubRegion& regionOfInterest, OfflinePoses& offlinePoses, HomogenousMatrix4& planeTransformation, const unsigned int minimalCorrespondences, const unsigned int minimalKeyFrames, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* finalAverageError, Worker* worker, bool* abort) const
{
	ocean_assert(lowerFrameIndex <= regionOfInterestIndex && regionOfInterestIndex <= upperFrameIndex);
	ocean_assert(!regionOfInterest.isEmpty());

	if (regionOfInterestIndex == (unsigned int)(-1) || regionOfInterest.isEmpty())
	{
		return false;
	}

	if (regionOfInterestIndex < lowerFrameIndex_ || regionOfInterestIndex > upperFrameIndex_)
	{
		return false;
	}

	HomogenousMatrix4 regionOfInterestPose;
	if (!database_.hasPose<false>(regionOfInterestIndex, &regionOfInterestPose) || !regionOfInterestPose.isValid())
	{
		return false;
	}

	RandomGenerator randomGenerator;

	Plane3 plane;
	ShiftVector<HomogenousMatrix4> poses;

	if (cameraMotion_ & Solver3::CM_TRANSLATIONAL)
	{
		// we first determine the plane that matches to the object points covered by the region of interest, we are also interested in all object points defining the plane

		Indices32 regionOfInterestObjectPointIds;
		if (!Solver3::determinePlane(database_, camera_, lowerFrameIndex_, regionOfInterestIndex, upperFrameIndex_, regionOfInterest, randomGenerator, plane, camera_.hasDistortionParameters(), Solver3::RelativeThreshold(3u, Scalar(0.5), 20u), Scalar(6), Geometry::Estimator::ET_HUBER, nullptr, &regionOfInterestObjectPointIds))
		{
			return false;
		}

		// now we try to determine as less key frames as possible in which the plane's object points are visible

		HomogenousMatrix4 pose;
		IndexSet32 keyFrameCandidateIdSet;

		// we gather all possible key frames (frames with valid pose)

		for (unsigned int n = lowerFrameIndex_; n <= upperFrameIndex_; ++n)
		{
			if (database_.hasPose<false>(n, &pose) && pose.isValid())
			{
				keyFrameCandidateIdSet.insert(n);
			}
		}

		IndexSet32 majorObjectPointIds(regionOfInterestObjectPointIds.begin(), regionOfInterestObjectPointIds.end());
		IndexSet32 minorObjectPointIds;

		Indices32 foundMajorObjectPointIds;
		Indices32 keyFrameIds;

		while (!majorObjectPointIds.empty())
		{
			Index32 keyFrameId;

			foundMajorObjectPointIds.clear();
			if (!database_.poseWithMostObservations<false>(keyFrameCandidateIdSet, majorObjectPointIds, minorObjectPointIds, keyFrameId, &foundMajorObjectPointIds))
			{
				break;
			}

			keyFrameIds.push_back(keyFrameId);
			keyFrameCandidateIdSet.erase(keyFrameId);

			for (Indices32::const_iterator i = foundMajorObjectPointIds.begin(); i != foundMajorObjectPointIds.end(); ++i)
			{
				majorObjectPointIds.erase(*i);
				minorObjectPointIds.insert(*i);
			}
		}

		Log::info() << "Found " << minorObjectPointIds.size() << " object points from " << regionOfInterestObjectPointIds.size() << " maximal possible in " << keyFrameIds.size() << " key frames!";

		// now we check whether we can spend more key frames
		if (keyFrameIds.size() < minimalKeyFrames)
		{
			Poses keyFrameCandidatePoses;
			keyFrameCandidatePoses.reserve(keyFrameCandidateIdSet.size() + keyFrameIds.size());

			Indices32 keyFrameCandidatePoseIds;
			keyFrameCandidatePoseIds.reserve(keyFrameCandidateIdSet.size() + keyFrameIds.size());

			const unsigned int halfCorrespondences = max(1u, database_.numberObservations<false>(keyFrameIds.front(), regionOfInterestObjectPointIds) / 2);

			// we add the already selected key frames (so that we can select further key frames most different to these)
			for (Indices32::const_iterator i = keyFrameIds.begin(); i != keyFrameIds.end(); ++i)
			{
				keyFrameCandidatePoses.push_back(Pose(database_.pose<false>(*i)));
				keyFrameCandidatePoseIds.push_back(*i);
			}

			for (const Index32 keyFrameCandidateId : keyFrameCandidateIdSet)
			{
				if (database_.numberObservations<false>(keyFrameCandidateId, regionOfInterestObjectPointIds) >= halfCorrespondences)
				{
					keyFrameCandidatePoses.push_back(Pose(database_.pose<false>(keyFrameCandidateId)));
					keyFrameCandidatePoseIds.push_back(keyFrameCandidateId);
				}
			}

			SuccessionSubset<Scalar, 6> successionSubset((SuccessionSubset<Scalar, 6>::Object*)keyFrameCandidatePoses.data(), keyFrameCandidatePoses.size());

			// the already selected key frames have the indices [0, keyFrameIds.size() - 1]
			for (size_t n = 0; n < keyFrameIds.size(); ++n)
			{
				successionSubset.incrementSubset(n);
			}

			const Indices32 subsetIndices(SuccessionSubset<Scalar, 6>::indices2indices32(successionSubset.subset(minimalKeyFrames)));

#ifdef OCEAN_DEBUG
			const Indices32 debugInitialKeyFrameIds(keyFrameIds);
#endif
			keyFrameIds = Subset::subset(keyFrameCandidatePoseIds, subsetIndices);

#ifdef OCEAN_DEBUG
			// we ensure that the first key frames are still those from our initial search
			ocean_assert(debugInitialKeyFrameIds.size() <= keyFrameIds.size());
			for (size_t n = 0; n < debugInitialKeyFrameIds.size(); ++n)
			{
				ocean_assert(debugInitialKeyFrameIds[n] == keyFrameIds[n]);
			}
#endif
		}

		Log::info() << "Finally selected " << keyFrameIds.size() << " key frames for the bundle adjustment of the object points in the region of interest";

		Scalar initialError, finalError;
		Indices32 regionOptimizedObjectPointIds;
		Vectors3 regionOptimizedObjectPoints;
		if (!Solver3::optimizeObjectPointsWithVariablePoses(database_, camera_, keyFrameIds, regionOfInterestObjectPointIds, regionOptimizedObjectPoints, regionOptimizedObjectPointIds, nullptr, 5u, Geometry::Estimator::ET_SQUARE, 50u, &initialError, &finalError))
		{
			Log::info() << "Solver3::optimizeObjectPointsWithVariablePoses() failed!";

			regionOptimizedObjectPointIds = regionOfInterestObjectPointIds;
			regionOptimizedObjectPoints = database_.objectPoints<false>(regionOfInterestObjectPointIds);
		}
		else
		{
			Log::info() << "Bundle adjustment of " << regionOfInterestObjectPointIds.size() << " while " << regionOptimizedObjectPoints.size() << " succeeded: " << initialError << " -> " << finalError;
		}

		Maintenance::get().send("HIGHLIGHT_OBJECT_POINTS", IO::Utilities::encodeVectors3(regionOptimizedObjectPoints.data(), regionOptimizedObjectPoints.size()));

		Database regionDatabase(database_);
		regionDatabase.setObjectPoints<false>(regionOptimizedObjectPointIds.data(), regionOptimizedObjectPoints.data(), regionOptimizedObjectPointIds.size());

		// we have to re-determine the plane due to the changed locations of the object points inside the region of interest
		if (!Solver3::determinePlane(regionDatabase, camera_, lowerFrameIndex_, regionOfInterestIndex, upperFrameIndex_, regionOfInterest, randomGenerator, plane, camera_.hasDistortionParameters(), Solver3::RelativeThreshold(3u, Scalar(0.5), 20u), Scalar(6), Geometry::Estimator::ET_HUBER))
		{
			return false;
		}

		const IndexSet32 regionOptimizedObjectPointIdSet(regionOptimizedObjectPointIds.begin(), regionOptimizedObjectPointIds.end());

		if (!Solver3::determinePoses(regionDatabase, AnyCameraPinhole(camera_), cameraMotion_, regionOptimizedObjectPointIdSet, false, randomGenerator, lowerFrameIndex, upperFrameIndex, minimalCorrespondences, poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, finalAverageError, worker, abort))
		{
			return false;
		}

#if 0

		ShiftVector<HomogenousMatrix4> regionPoses;
		if (!Solver3::determinePoses(regionDatabase, trackerCamera, trackerCameraMotion, regionOptimizedObjectPointIdSet, true, randomGenerator, trackerLowerFrameIndex, trackerUpperFrameIndex, minimalCorrespondences, regionPoses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, finalAverageError, worker, abort))
			return false;

		ocean_assert(regionPoses.firstIndex() == trackerLowerFrameIndex && regionPoses.lastIndex() == trackerUpperFrameIndex);
		regionDatabase.setPoses<false>(regionPoses);

		const Indices32 nonRegionNonOptimizedObjectPointIds(regionDatabase.objectPointIds<false, false>(regionOptimizedObjectPointIdSet, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())));

#ifdef OCEAN_DEBUG
		for (const Indices32::const_iterator i = nonRegionObjectPointIds.begin(); i != nonRegionObjectPointIds.end(); ++i)
			ocean_assert(regionOptimizedObjectPointIdSet.find(*i) == regionOptimizedObjectPointIdSet.end());
#endif

		Vectors3 nonRegionOptimizedObjectPoints;
		Indices32 nonRegionOptimizedObjectPointIds;
		if (!Solver3::optimizeObjectPointsWithFixedPoses(regionDatabase, trackerCamera, trackerCameraMotion, nonRegionNonOptimizedObjectPointIds, nonRegionOptimizedObjectPoints, nonRegionOptimizedObjectPointIds, 2u, estimator, Scalar(3.5 * 3.5), WorkerPool::get().scopedWorker()()))
			return false;

		Log::info() << "Non region object points: " << nonRegionNonOptimizedObjectPointIds.size() << " and optimized: " << nonRegionOptimizedObjectPoints.size();

		regionDatabase.setObjectPoints<false>(nonRegionNonOptimizedObjectPointIds.data(), nonRegionNonOptimizedObjectPointIds.size(), Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
		regionDatabase.setObjectPoints<false>(nonRegionOptimizedObjectPointIds.data(), nonRegionOptimizedObjectPoints.data(), nonRegionOptimizedObjectPointIds.size());

		// we determine the final precise plane due to the optimized 3D object point locations

		plane = Plane3();
		regionOfInterestObjectPointIds.clear();
		if (!Solver3::determinePlane(regionDatabase, trackerCamera, lowerFrameIndex, regionOfInterestIndex, upperFrameIndex, regionOfInterest, randomGenerator, plane, trackerCamera.hasDistortionParameters(), Solver3::RelativeThreshold(3u, Scalar(0.5), 20u), Scalar(6), Geometry::Estimator::ET_HUBER, nullptr, &regionOfInterestObjectPointIds))
			return false;

		Log::info() << "Finally we determined " << regionOfInterestObjectPointIds.size() << " object points for pose determination (not including remaining object points)";

		Scalar averageError;
		if (!Solver3::determinePoses(regionDatabase, trackerCamera, trackerCameraMotion, IndexSet32(regionOfInterestObjectPointIds.begin(), regionOfInterestObjectPointIds.end()), false, randomGenerator, lowerFrameIndex, upperFrameIndex, minimalCorrespondences, poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, &averageError, worker, abort))
			return false;

		Log::info() << "And determined poses for the specific region with an average error of " << averageError;

		if (finalAverageError)
			*finalAverageError = averageError;

#endif
	}
	else
	{
		// **TODO** we have to apply:
		// - a bundle adjustment for all roi object points
		// - a re-determination of the locations of all 3D object points not belonging to roi (so that the pose determination still will work if the roi object points disappear)
		// **TODO**

		if (!Solver3::determinePoses(database_, AnyCameraPinhole(camera_), cameraMotion_, IndexSet32(), false, randomGenerator, lowerFrameIndex, upperFrameIndex, minimalCorrespondences, poses, estimator, minimalValidCorrespondenceRatio, ransacMaximalSqrError, maximalRobustError, finalAverageError, worker, abort))
		{
			return false;
		}

		if (!Solver3::determinePerpendicularPlane(database_, camera_, startFrameIndex_, regionOfInterest.boundingBox().center(), 1, plane, camera_.hasDistortionParameters()))
		{
			return false;
		}
	}

	ocean_assert(plane.isValid());

	const Line3 ray = camera_.ray(regionOfInterest.boundingBox().center(), regionOfInterestPose);

	Vector3 pointOnPlane;
	if (!plane.intersection(ray, pointOnPlane) || !plane.transformation(pointOnPlane, Vector3(0, 1, 0), planeTransformation))
	{
		return false;
	}

	if (Maintenance::get().isActive())
	{
		Maintenance::get().send("PLANE_TRANSFORMATION_16", IO::Utilities::encodeHomogenousMatrix4(planeTransformation));
	}

	ocean_assert(NumericT<ptrdiff_t>::isInsideValueRange(lowerFrameIndex));
	ocean_assert(NumericT<ptrdiff_t>::isInsideValueRange(upperFrameIndex));

	ocean_assert(poses.firstIndex() == ptrdiff_t(lowerFrameIndex) && poses.lastIndex() == ptrdiff_t(upperFrameIndex));

	offlinePoses = OfflinePoses(lowerFrameIndex, poses.size());
	for (unsigned int n = lowerFrameIndex; n <= upperFrameIndex; ++n)
	{
		offlinePoses[n] = OfflinePose(n, poses[n]);
	}

	return true;
}

bool SLAMTracker::applyFrameTracking(const FrameType& frameType)
{
	ocean_assert(shouldStop_ == false);
	ocean_assert(!soleRegionOfInterestApplication_ || (!regionOfInterest_.isEmpty() && startFrameIndex_ != (unsigned int)(-1)));

	localProgress_ = -1;
	cameraMotion_ = Solver3::CM_INVALID;

	RandomGenerator randomGenerator;

	Log::info() << " ";
	Log::info() << "*** START TRACKING ***";

	if (frameProviderInterface_.isNull() || lowerFrameIndex_ > upperFrameIndex_)
	{
		return false;
	}

	const unsigned int frameRangeNumber = upperFrameIndex_ - lowerFrameIndex_ + 1u;

	// check whether the initial camera object matches with the current frame type
	if (camera_.isValid() && (camera_.width() != frameType.width() || camera_.height() != frameType.height()))
	{
		Log::warning() << "This given camera object does not match the frame type!";
		camera_ = PinholeCamera();
	}

	// use a default camera object when no camera object is given // **TODO** merged with the optimization strategy and field of view
	if (!camera_.isValid())
	{
		ocean_assert(optimizeCamera_);

		if (cameraFieldOfView_ > 0 && cameraFieldOfView_ < Numeric::pi())
			camera_ = PinholeCamera(frameType.width(), frameType.height(), cameraFieldOfView_);
		else
			camera_ = PinholeCamera(frameType.width(), frameType.height(), Numeric::deg2rad(60));
	}

	if (frameRangeNumber == 1u)
	{
		// we cannot track any reliable information, thus we simply create a default camera pose and return a plane perpendicular to the camera

		ocean_assert(!regionOfInterest_.isEmpty() && startFrameIndex_ != (unsigned int)(-1));

		abstractMotionType_ = AMT_PURE_ROTATIONAL;

		const OfflinePoses offlinePoses(upperFrameIndex_, 1, OfflinePose(startFrameIndex_, HomogenousMatrix4(true)));

		Plane3 plane;
		Vector3 pointOnPlane;
		if (!Tracking::Solver3::determinePerpendicularPlane(camera_, HomogenousMatrix4(true), regionOfInterest_.boundingBox().center(), 1, plane, camera_.hasDistortionParameters(), &pointOnPlane))
			return false;

		HomogenousMatrix4 planeTransformation;
		if (!plane.transformation(pointOnPlane, Vector3(0, 1, 0), planeTransformation))
			return false;

		// the plane is a suggestion - so that the user has the chance to adjust the plane by application of the virtual plane, which is simpler than using the base plane
		eventCallbacks_(TrackerTransformationStateEvent(id_, planeTransformation, true));
		updateCamera(camera_);
		updatePoses(offlinePoses);

		return true;
	}

	Log::info() << "Starting point path determination";

	PointPaths::TrackingConfiguration regionOfInterestTrackingConfiguration, frameTrackingConfiguration;

	localProgress_ = 0;
	ScopedEventStackLayer scopedProgress(*this, Scalar(0.00), Scalar(0.01));

	const bool useRegionOfInterest = soleRegionOfInterestApplication_ || (startFrameIndex_ != (unsigned int)(-1) && regionOfInterest_.size() >= 50  * 50);

	if (trackingQuality_ == TQ_AUTOMATIC)
	{
		if (useRegionOfInterest)
		{
			ocean_assert(startFrameIndex_ != (unsigned int)(-1));
			if (!PointPaths::determineAutomaticTrackingConfiguration(*frameProviderInterface_, FrameType::ORIGIN_UPPER_LEFT, motionSpeed_, startFrameIndex_, regionOfInterest_, soleRegionOfInterestApplication_ ? nullptr : &frameTrackingConfiguration, &regionOfInterestTrackingConfiguration, WorkerPool::get().scopedWorker()(), &shouldStop_))
				return false;
		}
		else
		{
			if (!PointPaths::determineAutomaticTrackingConfiguration(*frameProviderInterface_, FrameType::ORIGIN_UPPER_LEFT, motionSpeed_, lowerFrameIndex_, upperFrameIndex_, frameTrackingConfiguration, 5u, WorkerPool::get().scopedWorker()(), &shouldStop_))
				return false;
		}
	}
	else
	{
		if (useRegionOfInterest)
		{
			ocean_assert(startFrameIndex_ != (unsigned int)(-1));
			if (!PointPaths::determineTrackingConfiguration(*frameProviderInterface_, regionOfInterest_, trackingQuality_, motionSpeed_, soleRegionOfInterestApplication_ ? nullptr : &frameTrackingConfiguration, &regionOfInterestTrackingConfiguration, &shouldStop_))
				return false;
		}
		else
		{
			if (!PointPaths::determineTrackingConfiguration(*frameProviderInterface_, CV::SubRegion(), trackingQuality_, motionSpeed_, &frameTrackingConfiguration, nullptr, &shouldStop_))
				return false;
		}
	}

	scopedProgress.modify(Scalar(0.75));
	localProgress_ = 0;

	// track the points inside the specified sub-region
	if (useRegionOfInterest)
	{
		ocean_assert(regionOfInterestTrackingConfiguration.isValid());

		const ScopedEventStackLayer internalScopedProgress(*this, 0, soleRegionOfInterestApplication_ ?  1 : Scalar(0.25));

		// we select a border size of 20 pixels at the frame's border/boundary, re-tracked points in this border area count as invalid
		const unsigned int invalidBorderSize = frameType.width() >= 100u && frameType.height() >= 100u ? 20u : 0u;

		if (soleRegionOfInterestApplication_ && lowerFrameIndex_ != upperFrameIndex_)
		{
			// as the tracker relies on the area of interest only, we must ensure that we have enough point paths
			// thus, we test the current tracker configuration for the first neighboring frames (lower and upper) and weaken the tracker configuration as long as necessary

			Log::info() << "The tracker relies on the area of interest only, so we ensure that we use enough tracking points by weakening the tracker configuration as long as necessary.";

			ocean_assert(startFrameIndex_ != (unsigned int)(-1));
			const unsigned int lowerTestFrameIndex = max(int(lowerFrameIndex_), int(startFrameIndex_) - 1);
			const unsigned int upperTestFrameIndex = min(startFrameIndex_ + 1u, upperFrameIndex_);

			unsigned int weakeningIterations = 0u;
			while (weakeningIterations++ < 5u)
			{
				Database testDatabase;
				if (!PointPaths::determinePointPaths(*frameProviderInterface_, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, regionOfInterestTrackingConfiguration, lowerTestFrameIndex, regionOfInterest_, startFrameIndex_, upperTestFrameIndex, invalidBorderSize, true, testDatabase, WorkerPool::get().scopedWorker()(), &shouldStop_))
				{
					Log::error() << "determinePointPaths() FAILED!";
					return false;
				}

				// we use the maximal number of correspondences and not the minimal number to ensure that a keyframe (with different image content) between the selection frame and one neighboring frame does not create an insane tracker configuration
				unsigned int maximalPointCorrespondences = 0u;

				for (unsigned int n = lowerTestFrameIndex; n <= upperTestFrameIndex; ++n)
				{
					const unsigned int correspondences = testDatabase.numberCorrespondences<false, true, false>(startFrameIndex_, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

					maximalPointCorrespondences = max(maximalPointCorrespondences, correspondences);
				}

				Log::info() << "We tracked " << maximalPointCorrespondences << " points in the region of interest towards one neighboring frames, we are happy with 30.";

				if (maximalPointCorrespondences >= 30u)
					break;

				if (!regionOfInterestTrackingConfiguration.weakenConfiguration())
					break;

				Log::info() << "We weaken the tracker configuration";
			}
		}

		Log::info() << "Determining point paths in region of interest with " << regionOfInterestTrackingConfiguration.horizontalBinSize() << "x" << regionOfInterestTrackingConfiguration.verticalBinSize() << " bins " << regionOfInterestTrackingConfiguration.strength() << " minimal strength and " << regionOfInterestTrackingConfiguration.trackingMethod() << " as tracking method";

		if (!PointPaths::determinePointPaths(*frameProviderInterface_, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, regionOfInterestTrackingConfiguration, lowerFrameIndex_, regionOfInterest_, startFrameIndex_, upperFrameIndex_, invalidBorderSize, true, database_, WorkerPool::get().scopedWorker()(), &shouldStop_, &localProgress_))
		{
			Log::error() << "determinePointPaths() FAILED!";
			return false;
		}
	}

	localProgress_ = 0;

	// track the points in the remaining areas
	if (!soleRegionOfInterestApplication_)
	{
		ocean_assert(frameTrackingConfiguration.isValid());

		const ScopedEventStackLayer internalScopedProgress(*this, useRegionOfInterest ? Scalar(0.25) : 0, 1);

		// we select a border size of 20 pixels at the frame's border/boundary, re-tracked points in this border area count as invalid
		const unsigned int invalidBorderSize = frameType.width() >= 100u && frameType.height() >= 100u ? 20u : 0u;

		Log::info() << "Determining point paths in entire area with " << frameTrackingConfiguration.horizontalBinSize() << "x" << frameTrackingConfiguration.verticalBinSize() << " bins " << frameTrackingConfiguration.strength() << " minimal strength and " << frameTrackingConfiguration.trackingMethod() << " as tracking method";

		if (!PointPaths::determinePointPaths(*frameProviderInterface_, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, frameTrackingConfiguration, lowerFrameIndex_, (useRegionOfInterest && startFrameIndex_ != (unsigned int)(-1)) ? startFrameIndex_ : lowerFrameIndex_, upperFrameIndex_, invalidBorderSize, true, database_, WorkerPool::get().scopedWorker()(), &shouldStop_, &localProgress_))
		{
			Log::error() << "determinePointPaths() FAILED!";
			return false;
		}
	}

	Log::info() << "Finished point path determination";

	Log::info() << "Starting SLAM Tracker with a camera with " << Numeric::rad2deg(camera_.fovX()) << "deg field of view:";

	scopedProgress.modify(Scalar(0.80));
	localProgress_ = 0;

	unsigned int lowerPoseBorder, upperPoseBorder;
	if (!determineInitialObjectPoints(camera_, database_, randomGenerator, lowerFrameIndex_, startFrameIndex_ != (unsigned int)(-1) ? &startFrameIndex_ : nullptr, upperFrameIndex_, useRegionOfInterest ? regionOfInterest_ : CV::SubRegion(), soleRegionOfInterestApplication_, &lowerPoseBorder, &upperPoseBorder, &shouldStop_, &localProgress_))
	{
		Log::error() << "determineInitialObjectPoints() FAILED!";
		return false;
	}

	maintenanceSendEnvironment();

	scopedProgress.modify(Scalar(0.85));
	localProgress_ = 0;

	unsigned int maximalValidInitialCorrespondences = 0u;
	database_.poseWithMostCorrespondences<false, false, true>(lowerFrameIndex_, upperFrameIndex_, nullptr, &maximalValidInitialCorrespondences);
	ocean_assert(maximalValidInitialCorrespondences != 0u);

	// normally 10 would be a good lower boundary; however, in extreme situations we cannot use more than the maximal number of correspondences, but 5 is the absolute minimum
	const unsigned int correspondenceThresholdLowerBoundary = max(5u, min(10u, maximalValidInitialCorrespondences));

	ocean_assert(correspondenceThresholdLowerBoundary >= 5u);
	ocean_assert(maximalValidInitialCorrespondences < 10u || correspondenceThresholdLowerBoundary == 10u);

	if (!extendInitialObjectPoints(camera_, database_, lowerFrameIndex_, upperFrameIndex_, Solver3::RelativeThreshold(correspondenceThresholdLowerBoundary, Scalar(0.3), 25u), &lowerPoseBorder, &upperPoseBorder, &shouldStop_))
	{
		Log::error() << "extendInitialObjectPoints() FAILED!";
		return false;
	}

	maintenanceSendEnvironment();

	const bool findInitialFieldOfView = cameraOptimizationStrategy_ != PinholeCamera::OS_NONE && cameraFieldOfView_ < 0;

	PinholeCamera optimizedCamera;
	Database optimizedDatabase;
	Solver3::CameraMotion cameraMotion = Solver3::CM_UNKNOWN;

	scopedProgress.modify(Scalar(0.90));
	localProgress_ = 0;

	Scalar optimizedCameraFinalSqrError;
	if (optimizeCamera(camera_, database_, lowerFrameIndex_, upperFrameIndex_, findInitialFieldOfView, cameraOptimizationStrategy_, min(25u, frameRangeNumber), optimizedCamera, optimizedDatabase, &cameraMotion, &shouldStop_, &optimizedCameraFinalSqrError))
	{
		camera_ = optimizedCamera;
		database_ = std::move(optimizedDatabase);

		maintenanceSendEnvironment();

		Log::info() << "Database and camera profile updated with final error: " << optimizedCameraFinalSqrError;
	}
	else
	{
		if (Solver3::removeSparseObjectPoints(database_, Scalar(1e+7), Scalar(100), Scalar(0.10)))
		{
			Log::info() << "We retry to optimize the profile of the camera as we have modified the database";

			localProgress_ = 0;

			if (optimizeCamera(camera_, database_, lowerFrameIndex_, upperFrameIndex_, findInitialFieldOfView, cameraOptimizationStrategy_, 25u, optimizedCamera, optimizedDatabase, &cameraMotion, &shouldStop_, &optimizedCameraFinalSqrError))
			{
				camera_ = optimizedCamera;
				database_ = std::move(optimizedDatabase);

				maintenanceSendEnvironment();

				Log::info() << "Database and camera profile updated with final error: " << optimizedCameraFinalSqrError;
			}
		}
	}

	scopedProgress.modify(Scalar(0.95));
	localProgress_ = 0;

	if (!extendStableObjectPoints(camera_, database_, randomGenerator, lowerFrameIndex_, upperFrameIndex_, cameraMotion, Solver3::RelativeThreshold(10u, Scalar(0.4), 25u), &lowerPoseBorder, &upperPoseBorder, &cameraMotion_, &shouldStop_, &localProgress_))
	{
		Log::error() << "extendStableObjectPoints() FAILED!";
		return false;
	}

	Index32 validLowerPoseIndex, validUpperPoseIndex;
	if (database_.largestValidPoseRange<false>(lowerFrameIndex_, upperFrameIndex_, validLowerPoseIndex, validUpperPoseIndex))
	{
		Log::info() << "Final largest valid pose ranges: [" << validLowerPoseIndex << ", " << validUpperPoseIndex << "]";
	}

	maintenanceSendEnvironment();

	Log::info() << " ";
	Log::info() << "Ended with camera field of view: " << Numeric::rad2deg(camera_.fovX()) << "deg";
	Log::info() << "Camera Intrinsic: " << camera_.focalLengthX() << ", " << camera_.focalLengthY() << ", " << camera_.principalPointX() << ", " << camera_.principalPointY();
	Log::info() << "Camera Distortion: " << camera_.radialDistortion().first << ", " << camera_.radialDistortion().second << ", " << camera_.tangentialDistortion().first << ", " << camera_.tangentialDistortion().second;
	Log::info() << " ";
	Log::info() << "*** FINISHED TRACKING ***";

	if (Maintenance::get().isActive())
	{
		const Frame frame = CV::CVUtilities::visualizeDistortion(camera_, true, 20u, 20u, WorkerPool::get().scopedWorker()());
		Maintenance::get().send("FRAME", Media::Utilities::encodeFrame(frame, "png"));
	}

	scopedProgress.modify(Scalar(1));
	localProgress_ = 0;

	// now we update the abstract motion of this tracker due to the internal motion type which has been determined during the tracking process

	if (cameraMotion_ & Solver3::CM_TRANSLATIONAL)
	{
		abstractMotionType_ = AMT_COMPLEX;
	}
	else
	{
		abstractMotionType_ = AMT_PURE_ROTATIONAL;
	}

	// **TODO** **HACK** ->

	if (useRegionOfInterest)
	{
		Log::info() << "Determine final poses optimized for the region of interest";

		Plane3 roughRegionOfInterestPlane;
		if (addUnknownObjectPointsInPlanarRegionOfInterest(camera_, database_, randomGenerator, lowerFrameIndex_, startFrameIndex_, upperFrameIndex_, regionOfInterest_, cameraMotion_, 15u, &roughRegionOfInterestPlane, &shouldStop_))
		{
			Scalar meanCorrespondences = 0;
			unsigned int medianCorrespondences = 0u;
			if (determineNumberImagePointsInPlanarRegionOfInterest(camera_, database_, lowerFrameIndex_, startFrameIndex_, upperFrameIndex_, regionOfInterest_, roughRegionOfInterestPlane, nullptr, &meanCorrespondences, &medianCorrespondences, &shouldStop_))
			{
				Log::info() << "Median region image points: " << medianCorrespondences;
				Log::info() << "Mean region image points: " << meanCorrespondences;

				// **TODO** need determination of new point paths for planar region if too less image points a visible in some frames...

				maintenanceSendEnvironment();

				OfflinePoses offlinePoses;
				HomogenousMatrix4 planeTransformation(false);

				if (extractPoses(lowerFrameIndex_, startFrameIndex_, upperFrameIndex_, regionOfInterest_, offlinePoses, planeTransformation, 5u, 50u, Geometry::Estimator::ET_SQUARE, Scalar(1.0), Scalar(3.5 * 3.5), Scalar(10 * 10), nullptr, WorkerPool::get().scopedWorker()(), &shouldStop_))
				{
					if (offlinePoses.isValidIndex(startFrameIndex_) && offlinePoses[startFrameIndex_].transformation().isValid())
					{
						if ((cameraMotion_ & Solver3::CM_TRANSLATIONAL) == 0)
						{
							const Triangles2 roiTriangles = regionOfInterest_.triangles();

							if (roiTriangles.size() == 2)
							{
								Vectors2 rectangleCornersImageCoordinate;
								rectangleCornersImageCoordinate.reserve(4);

								rectangleCornersImageCoordinate.push_back(roiTriangles[0][0]);
								rectangleCornersImageCoordinate.push_back(roiTriangles[0][1]);
								rectangleCornersImageCoordinate.push_back(roiTriangles[0][2]);
								rectangleCornersImageCoordinate.push_back(roiTriangles[1][2]);

								const Geometry::Grid grid(camera_, offlinePoses[startFrameIndex_].transformation(), rectangleCornersImageCoordinate);

								if (grid.isValid())
								{
									planeTransformation = grid.plane2worldTransformation();
								}
							}
						}

						adjustPlaneTransformationToRegionOfInterest(camera_, offlinePoses[startFrameIndex_].transformation(), regionOfInterest_, planeTransformation);
					}

					eventCallbacks_(TrackerTransformationStateEvent(id_, planeTransformation));
					updateCamera(camera_);
					updatePoses(offlinePoses);

					Log::info() << "Everything is fine!";

					return true;
				}
			}
		}
	}

	// <- **TODO** **HACK**

	Log::info() << "Determine final poses for entire set of 3D object points";

	OfflinePoses offlinePoses;
	if (!extractPoses(lowerFrameIndex_, upperFrameIndex_, offlinePoses, 5u, Geometry::Estimator::ET_SQUARE, Scalar(1.0), Scalar(3.5 * 3.5), Scalar(10 * 10), nullptr, WorkerPool::get().scopedWorker()(), &shouldStop_))
		return false;

	ocean_assert(!regionOfInterest_.isEmpty() && startFrameIndex_ != (unsigned int)(-1));

	Plane3 plane;
	Vector3 pointOnPlane(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

	if (!offlinePoses.isValidIndex(startFrameIndex_) || !offlinePoses[startFrameIndex_].transformation().isValid() || !Tracking::Solver3::determinePerpendicularPlane(camera_, offlinePoses[startFrameIndex_].transformation(), regionOfInterest_.boundingBox().center(), 1, plane, camera_.hasDistortionParameters(), &pointOnPlane))
	{
		// the determination of the perpendicular plane failed as e.g., no valid camera pose is known for the selection frame
		// first, we try to solve this issue by weakening the conditions/thresholds for pose determination

		Log::info() << "The tracker selection frame is outside the range of valid poses, thus we try to determine an inaccurate pose for the selection frame";

		const HomogenousMatrix4 inaccuratePose = Solver3::determinePose(database_, AnyCameraPinhole(camera_), randomGenerator, startFrameIndex_, HomogenousMatrix4(false), 5u, Geometry::Estimator::ET_SQUARE, Scalar(1.0), Scalar(20 * 20));

		if (inaccuratePose.isValid())
		{
			Log::info() << "We could determine an inaccurate pose so that we now determine the perpendicular plane for this pose";

			Tracking::Solver3::determinePerpendicularPlane(camera_, inaccuratePose, regionOfInterest_.boundingBox().center(), 1, plane, camera_.hasDistortionParameters(), &pointOnPlane);
		}

		if (!plane.isValid())
		{
			// we still could not determine a very inaccurate pose so that we now have to determine a perpendicular plane for any image region in the very first valid pose

			Log::info() << "We could not determine an inaccurate pose so that we now take the backup method";

			for (OfflinePoses::Index n = offlinePoses.firstIndex(); n < offlinePoses.endIndex(); ++n)
			{
				if (offlinePoses[n].transformation().isValid())
				{
					if (Tracking::Solver3::determinePerpendicularPlane(camera_, offlinePoses[n].transformation(), Vector2(Scalar(camera_.width()), Scalar(camera_.height())) * Scalar(0.5), 1, plane, camera_.hasDistortionParameters(), &pointOnPlane))
					{
						break;
					}
				}
			}
		}

		ocean_assert(plane.isValid());

		if (!plane.isValid())
		{
			return false;
		}
	}

	HomogenousMatrix4 planeTransformation;
	if (!plane.transformation(pointOnPlane, Vector3(0, 1, 0), planeTransformation))
	{
		return false;
	}

	eventCallbacks_(TrackerTransformationStateEvent(id_, planeTransformation, true));
	updateCamera(camera_);
	updatePoses(offlinePoses);

	return true;
}

void SLAMTracker::onScheduler()
{
	if (localProgress_ >= 0 && localProgress_ <= 1)
	{
		updateTrackerProgress(localProgress_);
	}
}

bool SLAMTracker::determineInitialObjectPoints(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int* startFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const bool soleRegionOfInterest, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort, Scalar* progress)
{
	ocean_assert(pinholeCamera.isValid() && lowerFrame <= upperFrame);
	ocean_assert(!startFrame || (*startFrame >= lowerFrame && *startFrame <= upperFrame));
	ocean_assert(regionOfInterest.isEmpty() || (!regionOfInterest.isEmpty() && startFrame));

	if (!pinholeCamera.isValid() || lowerFrame > upperFrame || (startFrame != nullptr && (*startFrame < lowerFrame || *startFrame > upperFrame)))
	{
		return false;
	}

	setProgress(progress, 0);

	// We apply several RANSAC iterations and key-frame selections steps to identify a subset of all frames (key frames) having most stable/accurate initial 3D object points.

	const unsigned int numberFrames = upperFrame - lowerFrame + 1u;

	Log::info() << "Starting initial object point determination step for frame range [" << lowerFrame << ", " << upperFrame << "]:";
	Log::info() << " ";

	// check if we have only one frame so that we must back project all visible image points as we do not have more information
	if (numberFrames == 1u)
	{
		Indices32 objectPointIds;
		database.imagePointsWithObjectPoints<false>(lowerFrame, objectPointIds);

		const HomogenousMatrix4 pose(true);
		const Vectors2 imagePoints = database.imagePointsFromObjectPoints<false>(lowerFrame, objectPointIds);
		const Vectors3 objectPoints = Geometry::Utilities::createObjectPoints(pinholeCamera, pose, ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), 1);

		database.setPose<false>(lowerFrame, pose);

		if (!objectPointIds.empty())
		{
			database.setObjectPoints<false>(objectPointIds.data(), objectPoints.data(), objectPointIds.size());
		}

		Log::info() << "We have only one frame, so that we can stop here directly";

		if (finalLowerValidPoseRange)
		{
			*finalLowerValidPoseRange = lowerFrame;
		}

		if (finalUpperValidPoseRange)
		{
			*finalUpperValidPoseRange = lowerFrame;
		}

		return true;
	}

	HighPerformanceTimer timer;

	const unsigned int minimalKeyframes = min(numberFrames, 3u);
	const unsigned int maximalKeyframes = 20u;

	Vectors3 objectPoints;
	Indices32 objectPointIds, poseIds;

	if (startFrame)
	{
		// if we have a start frame we use the dense point determination function, if the determination fails we take the sparse determination function as fallback

		if (regionOfInterest)
		{
			Log::info() << "Dense initial point determination with region of interest:";
		}
		else
		{
			Log::info() << "Dense initial point determination with start frame:";
		}

		if (!Solver3::determineInitialObjectPointsFromDenseFrames(database, pinholeCamera, randomGenerator, lowerFrame, *startFrame, upperFrame, regionOfInterest, Scalar(0.1), objectPoints, objectPointIds, Solver3::RelativeThreshold(20u, Scalar(0.4), 100u), Scalar(0.1), minimalKeyframes, maximalKeyframes, Scalar(3.5 * 3.5), &poseIds, nullptr, abort))
		{
			// the initial object determination failed, thus we either have to less object points or/and our threshold were too strict

			Log::info() << "Dense initial point determination failed";

			if (regionOfInterest)
			{
				if (soleRegionOfInterest)
				{
					// as we have a defined region of interest and this region is the sole information we must use for tracking we must weaken the threshold

					Log::info() << "Restarting dense initial point determination with weakened thresholds:";
					Solver3::determineInitialObjectPointsFromDenseFrames(database, pinholeCamera, randomGenerator, lowerFrame, *startFrame, upperFrame, regionOfInterest, Scalar(0.1), objectPoints, objectPointIds, Solver3::RelativeThreshold(8u, Scalar(0.4), 100u), Scalar(0.1), minimalKeyframes, maximalKeyframes, Scalar(3.5 * 3.5), &poseIds, nullptr, abort);
				}
				else
				{
					// if the determination of initial points failed (with region of interest) we restart the determination without the region

					Log::info() << "Restarting dense initial point determination without roi:";
					Solver3::determineInitialObjectPointsFromDenseFrames(database, pinholeCamera, randomGenerator, lowerFrame, *startFrame, upperFrame, CV::SubRegion(), Scalar(0.1), objectPoints, objectPointIds, Solver3::RelativeThreshold(20u, Scalar(0.4), 100u), Scalar(0), minimalKeyframes, maximalKeyframes, Scalar(3.5 * 3.5), &poseIds, nullptr, abort);
				}
			}
		}
	}

	setProgress(progress, Scalar(0.25));

	if (poseIds.empty())
	{
		// we use the sparse point determination function if the dense point determination function was not invoked or failed

		Log::info() << "Sparse initial point determination:";

		objectPoints.clear();
		objectPointIds.clear();
		poseIds.clear();

		// we take a step at each 50th frame
		const unsigned int steps = max(min(numberFrames, 3u), numberFrames / 50u);

		if (!Solver3::determineInitialObjectPointsFromSparseKeyFramesBySteps(database, steps, pinholeCamera, randomGenerator, lowerFrame, upperFrame, Scalar(0.1), objectPoints, objectPointIds, Solver3::RelativeThreshold(20u, Scalar(0.4), 100u), minimalKeyframes, maximalKeyframes, Scalar(3.5 * 3.5), &poseIds, WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}
	}

	setProgress(progress, Scalar(0.50));

	std::sort(poseIds.begin(), poseIds.end());

	Log::info() << "Determined initial " << objectPointIds.size() << " object points in " << poseIds.size() << " keyframes within the frame range [" << poseIds.front() << ", " << poseIds.back() << "] = " << poseIds.back() - poseIds.front() + 1 << " poses";
	Log::info() << timer.mseconds() << "ms";
	Log::info() << " ";

	timer.start();

	const Index32 startPoseId = poseIds.front();
	poseIds.clear();

	Vectors3 optimizedObjectPoints;
	Indices32 optimizedObjectPointIds;
	Scalar initialSqrError, finalSqrError;
	if (!Solver3::optimizeInitialObjectPoints(database, AnyCameraPinhole(pinholeCamera), randomGenerator, lowerFrame, startPoseId, upperFrame, objectPoints, objectPointIds, optimizedObjectPoints, optimizedObjectPointIds, (unsigned int)(objectPointIds.size()), minimalKeyframes, maximalKeyframes, Scalar(3.5 * 3.5), &poseIds, &initialSqrError, &finalSqrError, abort))
	{
		return false;
	}

	setProgress(progress, Scalar(0.75));

	Log::info() << "Bundle adjustment optimization of " << optimizedObjectPoints.size() << " object points within " << poseIds.size() << " keyframes with error improvement: " << initialSqrError << " -> " << finalSqrError;
	Log::info() << timer.mseconds() << "ms";
	Log::info() << " ";

	objectPoints = std::move(optimizedObjectPoints);
	objectPointIds = std::move(optimizedObjectPointIds);

	// now we set initial 3D object point in the database
	ocean_assert(objectPoints.size() == objectPointIds.size());
	database.setObjectPoints<false>(objectPointIds.data(), objectPoints.data(), objectPoints.size());

	// we determine the minimal number of 2D/3D point correspondences which are neccessary for a valid pose:
	// if we have less than 20 initial object points: number of initial object points; else: 75% of the object points but at least 20
	unsigned int minimalCorrespondences = max(min((unsigned int)objectPoints.size(), 20u), (unsigned int)objectPoints.size() * 3u / 4u);
	ocean_assert(minimalCorrespondences >= 5u);

	timer.start();

	size_t validPoses;
	Scalar aveError;
	unsigned int lowerValidPose = (unsigned int)(-1), upperValidPose = (unsigned int)(-1);

	// we determine the initial poses
	if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_UNKNOWN, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Geometry::Estimator::ET_SQUARE, Scalar(1), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), &aveError, &validPoses, WorkerPool::get().scopedWorker()(), abort)
			|| !database.validPoseRange<false>(lowerFrame, poseIds.front(), upperFrame, lowerValidPose, upperValidPose))
	{
		return false;
	}

	setProgress(progress, Scalar(0.85));

	Log::info() << "Determined " << validPoses << " initial poses with " << minimalCorrespondences << " minimal correspondences (75% of " << objectPoints.size() << " object points) with accuracies: " << aveError;
	Log::info() << "Valid pose range [" << lowerValidPose << "; " << upperValidPose << "] = " << upperValidPose - lowerValidPose + 1u;
	Log::info() << timer.mseconds() << "ms";
	Log::info() << " ";

	ocean_assert(optimizedObjectPoints.empty());
	ocean_assert(optimizedObjectPointIds.empty());

	timer.start();

	const unsigned int minimalObservations = upperValidPose - lowerValidPose + 1u;

	// we apply an optimization of all 3D object points individually (without optimizing the camera poses concurrently)
	if (!Solver3::optimizeObjectPointsWithFixedPoses(database, pinholeCamera, Solver3::CM_UNKNOWN, objectPointIds, optimizedObjectPoints, optimizedObjectPointIds, minimalObservations, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), WorkerPool::get().scopedWorker()(), abort))
	{
		return false;
	}

	setProgress(progress, Scalar(0.95));

	Log::info() << "Optimized the 3D object points individually now holding " << optimizedObjectPointIds.size() << " initial object points";
	Log::info() << timer.mseconds() << "ms";
	Log::info() << " ";

	objectPoints = std::move(optimizedObjectPoints);
	objectPointIds = std::move(optimizedObjectPointIds);

	// remove all object points and set the new optimized object points
	database.setObjectPoints<false>();
	database.setObjectPoints<false>(objectPointIds.data(), objectPoints.data(), objectPointIds.size());

	// we have to update the minimal number of correspondences as the number of object points may be smaller now
	minimalCorrespondences = max(min((unsigned int)objectPoints.size(), 20u), (unsigned int)objectPoints.size() * 3u / 4u);
	ocean_assert(minimalCorrespondences >= 5u);

	timer.start();

	// we determine the initial poses now for the new initial object points
	if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_UNKNOWN, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Geometry::Estimator::ET_SQUARE, Scalar(1), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), &aveError, &validPoses, WorkerPool::get().scopedWorker()(), abort)
			|| !database.validPoseRange<false>(lowerFrame, poseIds.front(), upperFrame, lowerValidPose, upperValidPose))
	{
		return false;
	}

	Log::info() << "Determined " << validPoses << " initial poses with " << minimalCorrespondences << " minimal correspondences (75% of " << objectPoints.size() << " object points) with accuracies: " << aveError;
	Log::info() << "Valid pose range [" << lowerValidPose << "; " << upperValidPose << "] = " << upperValidPose - lowerValidPose + 1u;
	Log::info() << timer.mseconds() << "ms";

	// we do not remove inaccurate initial object points as we still may have too less object points so that we need any initial object point as long as it is almost accurate

	if (finalLowerValidPoseRange)
	{
		*finalLowerValidPoseRange = lowerValidPose;
	}

	if (finalUpperValidPoseRange)
	{
		*finalUpperValidPoseRange = upperValidPose;
	}

	setProgress(progress, 1);

	return !abort || !*abort;
}

bool SLAMTracker::extendInitialObjectPoints(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort)
{
	ocean_assert(pinholeCamera.isValid() && lowerFrame <= upperFrame);

	if (!pinholeCamera.isValid() || lowerFrame > upperFrame)
	{
		return false;
	}

	Log::info() << "Starting extension of initial object points step for frame range [" << lowerFrame << ", " << upperFrame << "]:";
	Log::info() << " ";

	HighPerformanceTimer timer;

	RandomGenerator randomGenerator;

	// we add the first set of new unknown object (object points with unknown 3D locations)

	// we determine the ids of all un-located object points and sort them according their number of observations
	IndexPairs32 objectPointPairs(database.objectPointIdsWithNumberOfObservations<false, true>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(-1), WorkerPool::get().scopedWorker()()));
	if (objectPointPairs.empty())
	{
		return false;
	}

	std::sort(objectPointPairs.rbegin(), objectPointPairs.rend(), Ocean::Utilities::sortPairSecond<Index32, Index32>);

	// we identify the number of minimal observations: best 100 object points (or less if there are less than 100)
	const unsigned int minimalObservations = objectPointPairs[min((unsigned int)objectPointPairs.size() - 1u, 100u)].second;

	// now we extract all ids of object points with more or equal observations
	Indices32 unknownObjectPointIds;
	unknownObjectPointIds.reserve(100);

	for (const IndexPair32& objectPointPair : objectPointPairs)
	{
		if (objectPointPair.second >= minimalObservations)
		{
			unknownObjectPointIds.push_back(objectPointPair.first);
		}
		else
		{
			break;
		}
	}

	ocean_assert(!unknownObjectPointIds.empty());

	Vectors3 newObjectPoints;
	Indices32 newObjectPointIds, newObjectPointObservations;
	if (!Solver3::determineUnknownObjectPoints(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_UNKNOWN, unknownObjectPointIds, newObjectPoints, newObjectPointIds, randomGenerator, nullptr, 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort))
	{
		return false;
	}

	if (!newObjectPointIds.empty())
	{
		database.setObjectPoints<false>(newObjectPointIds.data(), newObjectPoints.data(), newObjectPoints.size());

		Log::info() << "Added the first set of unknown 3D object points:";
		Log::info() << "Added " << newObjectPointIds.size() << " with at least " << minimalObservations << " observations in valid poses";
		Log::info() << timer.mseconds() << "ms";
		Log::info() << " ";
	}
	else
	{
		Log::info() << "Could not any further unknown 3D object point";
	}

	timer.start();

	// we update all poses as we have new 3D object points which might change the current poses a little bit
	// therefore we determine the pose with maximal valid correspondences so that we can define the minimal number of correspondences necessary for pose determination in neighboring frames

	Index32 poseId;
	unsigned int bestCorrespondences;
	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, &poseId, &bestCorrespondences, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())))
	{
		return false;
	}

	unsigned int minimalCorrespondences = max(min(bestCorrespondences, 20u), bestCorrespondences / 2u);

	size_t validPoses;
	Scalar aveError;
	unsigned int lowerValidPose, upperValidPose;
	if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_UNKNOWN, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Geometry::Estimator::ET_SQUARE, Scalar(0.95), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), &aveError, &validPoses, WorkerPool::get().scopedWorker()(), abort))
	{
		return false;
	}

	if (!database.validPoseRange<false>(lowerFrame, poseId, upperFrame, lowerValidPose, upperValidPose) && !database.largestValidPoseRange<false>(lowerFrame, upperFrame, lowerValidPose, upperValidPose))
	{
		return false;
	}

	Log::info() << "Determined " << validPoses << " initial poses with " << minimalCorrespondences << " minimal correspondences (50% of best correspondences (" << bestCorrespondences << ")) with accuracies: " << aveError;
	Log::info() << "Valid pose range [" << lowerValidPose << "; " << upperValidPose << "] = " << upperValidPose - lowerValidPose + 1u;
	Log::info() << timer.mseconds() << "ms";
	Log::info() << " ";

	timer.start();

	const unsigned int minimalKeyFrames = min(upperFrame - lowerFrame + 1u, 3u);
	const unsigned int maximalKeyFrames = 20u;

	Vectors3 optimizedObjectPoints;
	Indices32 optimizedObjectPointIds;
	Scalar initialError, finalError;
	if (Solver3::optimizeObjectPointsWithVariablePoses(database, pinholeCamera, optimizedObjectPoints, optimizedObjectPointIds, nullptr, nullptr, minimalKeyFrames, maximalKeyFrames, min(upperFrame - lowerFrame + 1u, 10u), Geometry::Estimator::ET_SQUARE, 50u, &initialError, &finalError))
	{
		Log::info() << "Bundle Adjustment succeeded: " << initialError << " --> " << finalError << " in " << timer.mseconds() << "ms";

		database.setObjectPoints<false>();
		database.setObjectPoints<false>(optimizedObjectPointIds.data(), optimizedObjectPoints.data(), optimizedObjectPointIds.size());
	}
	else
	{
		Log::info() << "Bundle Adjustment failed, this may happen if we have e.g., near object points and very far object points within the same scene...";

		if (Solver3::removeSparseObjectPoints(database, Scalar(1e+7), Scalar(100), Scalar(0.05)))
		{
			optimizedObjectPoints.clear();
			optimizedObjectPointIds.clear();
			if (Solver3::optimizeObjectPointsWithVariablePoses(database, pinholeCamera, optimizedObjectPoints, optimizedObjectPointIds, nullptr, nullptr, minimalKeyFrames, maximalKeyFrames, min(upperFrame - lowerFrame + 1u, 10u), Geometry::Estimator::ET_SQUARE, 50u, &initialError, &finalError))
			{
				Log::info() << "Now the Bundle Adjustment succeeded: " << initialError << " --> " << finalError << " in " << timer.mseconds() << "ms";

				database.setObjectPoints<false>();
				database.setObjectPoints<false>(optimizedObjectPointIds.data(), optimizedObjectPoints.data(), optimizedObjectPointIds.size());
			}
			else
			{
				Log::info() << "The Bundle Adjustment failed again!";
			}
		}
	}

	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, &poseId, &bestCorrespondences, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())))
	{
		return false;
	}

	minimalCorrespondences = max(min(bestCorrespondences, 20u), bestCorrespondences / 2u);

	timer.start();
	if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_UNKNOWN, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Geometry::Estimator::ET_SQUARE, Scalar(0.95), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), nullptr, nullptr, WorkerPool::get().scopedWorker()(), abort))
	{
		return false;
	}

	if (!database.validPoseRange<false>(lowerFrame, poseId, upperFrame, lowerValidPose, upperValidPose) && !database.largestValidPoseRange<false>(lowerFrame, upperFrame, lowerValidPose, upperValidPose))
	{
		return false;
	}

	Log::info() << "Final pose update: " << timer.mseconds() << "ms";

	Log::info() << " ";

	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &bestCorrespondences))
	{
		return false;
	}

	minimalCorrespondences = correspondenceThreshold.threshold(bestCorrespondences);

	if (!removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_UNKNOWN, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(0.95), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), finalLowerValidPoseRange, finalUpperValidPoseRange, abort))
	{
		return false;
	}

	Log::info() << "Finished extension of initial object points step " << timer.mseconds() << "ms";

	return !abort || !*abort;
}

bool SLAMTracker::optimizeCamera(const PinholeCamera& pinholeCamera, const Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const bool findInitialFieldOfView, const PinholeCamera::OptimizationStrategy optimizationStrategy, const unsigned int maximalKeyFrames, PinholeCamera& optimizedCamera, Database& optimizedDatabase, Solver3::CameraMotion* cameraMotion, bool* abort, Scalar* finalMeanSqrError)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);

	ocean_assert(maximalKeyFrames >= 1u && maximalKeyFrames <= upperFrame - lowerFrame + 1u);

	if (!pinholeCamera.isValid() || lowerFrame > upperFrame)
	{
		return false;
	}

	if (cameraMotion)
	{
		*cameraMotion = Solver3::CM_UNKNOWN;
	}

	const unsigned int frameNumber = upperFrame - lowerFrame + 1u;

	Log::info() << " ";
	Log::info() << "Trying to suppose a pure rotational camera motion:";

	if (Solver3::supposeRotationalCameraMotion(database, pinholeCamera, lowerFrame, upperFrame, findInitialFieldOfView, optimizationStrategy, optimizedCamera, optimizedDatabase, 0u, min(frameNumber, 3u), maximalKeyFrames, Numeric::deg2rad(20), Numeric::deg2rad(140), Scalar(1.5 * 1.5), WorkerPool::get().scopedWorker()(), abort, finalMeanSqrError))
	{
		Log::info() << "We have a pure rotational camera motion!";
		Log::info() << "Camera field of view: " << Numeric::rad2deg(optimizedCamera.fovX()) << "deg";
		Log::info() << " ";

		if (cameraMotion)
		{
			*cameraMotion = Solver3::CM_ROTATIONAL;
		}
	}
	else
	{
		Log::info() << "We do NOT have a pure rotation camera motion!";

		if (cameraMotion)
		{
			*cameraMotion = Solver3::CM_TRANSLATIONAL;
		}

		if (findInitialFieldOfView || optimizationStrategy != PinholeCamera::OS_NONE)
		{
			Log::info() << " ";
			Log::info() << "Thus, we optimize the camera profile for a translational camera motion!";

			if (!Solver3::optimizeCamera(database, pinholeCamera, lowerFrame, upperFrame, findInitialFieldOfView, optimizationStrategy, optimizedCamera, optimizedDatabase, 2u, min(frameNumber, 3u), maximalKeyFrames, Numeric::deg2rad(20), Numeric::deg2rad(140), WorkerPool::get().scopedWorker()(), abort, finalMeanSqrError))
			{
				Log::error() << "optimizeCamera() FAILED!";
				return false;
			}

			Log::info() << "Camera field of view: " << Numeric::rad2deg(optimizedCamera.fovX()) << "deg";
			Log::info() << " ";
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool SLAMTracker::extendStableObjectPoints(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::CameraMotion cameraMotion, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, Solver3::CameraMotion* finalCameraMotion, bool* abort, Scalar* progress)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);

	Log::info() << " ";
	Log::info() << "Extending stable object points and camera poses";

	// check whether we have at least one valid pose
	unsigned int validLowerFrame = (unsigned int)(-1);
	unsigned int validUpperFrame = (unsigned int)(-1);
	if (!database.largestValidPoseRange<false>(lowerFrame, upperFrame, validLowerFrame, validUpperFrame))
	{
		return false;
	}

	Log::info() << "Largest valid pose range: [" << validLowerFrame << ", " << validUpperFrame << "] from [" << lowerFrame << ", " << upperFrame << "] maximal possible pose range";

	Solver3::CameraMotion internalCameraMotion = cameraMotion != Solver3::CM_UNKNOWN ? cameraMotion : Solver3::determineCameraMotion(database, pinholeCamera, validLowerFrame, validUpperFrame, true, WorkerPool::get().scopedWorker()());

	Log::info() << "We have the following camera motion: " << Solver3::translateCameraMotion(internalCameraMotion);
	Log::info() << " ";

	if (internalCameraMotion == Solver3::CM_INVALID)
	{
		return false;
	}

	if (validLowerFrame == lowerFrame && validUpperFrame == upperFrame)
	{
		// if we have camera poses for the maximal possible pose range we can apply a more 'relaxed' strategy as we seem to have a simple scene which could be coverd within the initialization step already
		if (!extendStableObjectPointsFull(pinholeCamera, database, internalCameraMotion, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress))
		{
			return false;
		}
	}
	else
	{
		// if we have camera poses only for a small subset of the entire pose range we need to extend the remaining environment (3D area) and the remaining camera poses very carefully
		if (!extendStableObjectPointsPartially(pinholeCamera, database, internalCameraMotion, lowerFrame, upperFrame, correspondenceThreshold, &validLowerFrame, &validUpperFrame, abort, progress))
		{
			return false;
		}

		if (finalLowerValidPoseRange)
		{
			*finalLowerValidPoseRange = validLowerFrame;
		}

		if (finalUpperValidPoseRange)
		{
			*finalUpperValidPoseRange = validUpperFrame;
		}

		// we have to determine the motion of the camera again as it might have changed, **TODO** perhaps this should be done in the specific extension function above?
		internalCameraMotion = Solver3::determineCameraMotion(database, pinholeCamera, validLowerFrame, validUpperFrame, true, WorkerPool::get().scopedWorker()());
	}

	if (internalCameraMotion == Solver3::CM_INVALID)
	{
		return false;
	}

	unsigned int bestCorrespondences;
	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &bestCorrespondences))
	{
		return false;
	}

	const unsigned int minimalCorrespondences = correspondenceThreshold.threshold(bestCorrespondences);

	if (!removeInaccurateObjectPoints(pinholeCamera, internalCameraMotion, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(1), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), finalLowerValidPoseRange, finalUpperValidPoseRange, abort))
	{
		return false;
	}

	if (finalCameraMotion)
	{
		*finalCameraMotion = internalCameraMotion;
	}

	return true;
}

bool SLAMTracker::removeInaccurateObjectPoints(const PinholeCamera& pinholeCamera, const Solver3::CameraMotion cameraMotion, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalAverageSqrError, const Scalar maximalWorstSqrError, const unsigned int iterations, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort)
{
	ocean_assert(cameraMotion != Solver3::CM_INVALID);
	ocean_assert(iterations != 0u);
	ocean_assert(lowerFrame <= upperFrame);

	Log::info() << " ";
	Log::info() << "Removing inaccurate object points";

	size_t inaccurateObjectPoints = 0;

	unsigned int iteration = 0u;
	while ((!abort || !*abort) && iteration++ < iterations)
	{
		// extracting all valid object points from the entire database
		const Indices32 objectPointIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		// determine the projection errors for each object points (object points having no observation within [lowerFrame, upperFrame] receive Numeric::maxValue()
		Scalars averageErrors(objectPointIds.size()), worstErrors(objectPointIds.size());
		if (!Solver3::determineProjectionErrors(database, pinholeCamera, objectPointIds, pinholeCamera.hasDistortionParameters(), lowerFrame, upperFrame, nullptr, averageErrors.data(), worstErrors.data(), nullptr, WorkerPool::get().scopedWorker()()))
		{
			return false;
		}

		// we separated the object points
		Indices32 accurateObjectPointIds;
		Indices32 inaccurateObjectPointIds;
		for (size_t n = 0; n < objectPointIds.size(); ++n)
		{
			if (averageErrors[n] <= maximalAverageSqrError && worstErrors[n] <= maximalWorstSqrError)
			{
				accurateObjectPointIds.push_back(objectPointIds[n]);
			}
			else
			{
				inaccurateObjectPointIds.push_back(objectPointIds[n]);
			}
		}

		// we can stop if no invalid object point could be found
		if (inaccurateObjectPointIds.empty())
		{
			break;
		}

		// set all inaccurate object points (and also valid object points without observation) to an invalid object position
		database.setObjectPoints<false>(inaccurateObjectPointIds.data(), inaccurateObjectPointIds.size(), Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		inaccurateObjectPoints += inaccurateObjectPointIds.size();

		// now we apply one pose update step followed by one object point update step and followed again by a pose update step
		// we avoid a bundle adjustment due to performance reasons although the quality and accuracy of a bundle adjustment would be better

		if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), cameraMotion, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Geometry::Estimator::ET_SQUARE, minimalValidCorrespondenceRatio, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), nullptr, nullptr, WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}

		// **TODO** thresholds
		Vectors3 optimizedObjectPoints;
		Indices32 optimizedObjectPointIds;
		if (!Solver3::optimizeObjectPointsWithFixedPoses(database, pinholeCamera, cameraMotion, accurateObjectPointIds, optimizedObjectPoints, optimizedObjectPointIds, 3u, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}

		// we update the database with the new locations of all valid 3D object points
		if (optimizedObjectPointIds.size() == accurateObjectPointIds.size())
		{
			ocean_assert(IndexSet32(accurateObjectPointIds.begin(), accurateObjectPointIds.end()) == IndexSet32(optimizedObjectPointIds.begin(), optimizedObjectPointIds.end()));
			database.setObjectPoints<false>(optimizedObjectPointIds.data(), optimizedObjectPoints.data(), optimizedObjectPointIds.size());
		}
		else
		{
			ocean_assert(accurateObjectPointIds.size() > optimizedObjectPointIds.size());
			inaccurateObjectPoints += accurateObjectPointIds.size() - optimizedObjectPointIds.size();

			database.setObjectPoints<false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
			database.setObjectPoints<false>(optimizedObjectPointIds.data(), optimizedObjectPoints.data(), optimizedObjectPointIds.size());
		}

		if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), cameraMotion, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Geometry::Estimator::ET_SQUARE, minimalValidCorrespondenceRatio, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), nullptr, nullptr, WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}
	}

	unsigned int validLowerFrame, validUpperFrame;
	if (!database.largestValidPoseRange<false>(lowerFrame, upperFrame, validLowerFrame, validUpperFrame))
	{
		return false;
	}

	Log::info() << "Identified " << inaccurateObjectPoints << " inaccurate object points (and removed them) within " << iteration - 1u << " iterations: [" << validLowerFrame << ", " << validUpperFrame << "]";
	Log::info() << " ";

	if (finalLowerValidPoseRange != nullptr)
	{
		*finalLowerValidPoseRange = validLowerFrame;
	}
	if (finalUpperValidPoseRange != nullptr)
	{
		*finalUpperValidPoseRange = validUpperFrame;
	}

	return true;
}

bool SLAMTracker::optimizeObjectPointsAndPosesIndividuallyIteratively(const PinholeCamera& pinholeCamera, const Solver3::CameraMotion cameraMotion, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, Indices32& objectPointIds, const unsigned int minimalCorrespondences, const bool beginWithPoseUpdate, const unsigned int iterations, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalRobustError, Scalar* initialAverageError, Scalar* finalAverageError, bool* abort)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(cameraMotion != Solver3::CM_INVALID);

	Vectors3 optimizedObjectPoints;
	Indices32 optimizedObjectPointIds;

	Scalar firstError = Numeric::maxValue();
	Scalar finalError = Numeric::maxValue();
	Scalar previousError = Numeric::maxValue();

	if (beginWithPoseUpdate)
	{
		// **TODO** thresholds
		if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), cameraMotion, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, Scalar(3.5 * 3.5), maximalRobustError, &firstError, nullptr, WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}
	}

	for (unsigned int n = 0u; (!abort || !*abort) && n < iterations; ++n)
	{
		optimizedObjectPoints.clear();
		optimizedObjectPointIds.clear();

		if (!Solver3::optimizeObjectPointsWithFixedPoses(database, pinholeCamera, cameraMotion, objectPointIds, optimizedObjectPoints, optimizedObjectPointIds, 3u, estimator, maximalRobustError, WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}

		// we reset all object points which were intended to be optimized
		database.setObjectPoints<false>(objectPointIds.data(), objectPointIds.size(), Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
		// now we set all optimized object points (so that we loose all object points which could not be optimized)
		database.setObjectPoints<false>(optimizedObjectPointIds.data(), optimizedObjectPoints.data(), optimizedObjectPointIds.size());

		objectPointIds = optimizedObjectPointIds;

		Scalar localAverageError;

		// **TODO** thresholds
		if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), cameraMotion, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, estimator, minimalValidCorrespondenceRatio, Scalar(3.5 * 3.5), maximalRobustError, &localAverageError, nullptr, WorkerPool::get().scopedWorker()(), abort))
		{
			return false;
		}

		if (firstError == Numeric::maxValue())
		{
			ocean_assert(n == 0u);
			firstError = localAverageError;
			finalError = localAverageError;
			previousError = localAverageError;
		}
		else
		{
			if (localAverageError >= previousError)
			{
				break;
			}
		}

		finalError = localAverageError;
		previousError = localAverageError;
	}

	ocean_assert(firstError >= finalError && finalError != Numeric::maxValue());

	if (initialAverageError != nullptr)
	{
		*initialAverageError = firstError;
	}

	if (finalAverageError != nullptr)
	{
		*finalAverageError = finalError;
	}

	return (!abort || !*abort);
}

bool SLAMTracker::addUnknownObjectPointsInRegionOfInterest(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Solver3::CameraMotion cameraMotion, const unsigned int minimalObservations, bool* abort)
{
	ocean_assert(lowerFrame <= regionOfInterestFrame && regionOfInterestFrame <= upperFrame);
	ocean_assert(!regionOfInterest.isEmpty());

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	Indices32 objectPointIds;
	database.imagePointsObjectPoints<false, true>(regionOfInterestFrame, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), minimalObservations, nullptr, &objectPointIds);

	// we have to identify all 2D image points lying inside the specified region of interest

	Indices32 validIndices;
	validIndices.reserve(imagePoints.size());

	for (unsigned int n = 0u; n < imagePoints.size(); ++n)
	{
		if (regionOfInterest.isInside(imagePoints[n]))
		{
			validIndices.push_back(n);
		}
	}

	if (validIndices.size() != imagePoints.size())
	{
		imagePoints = Subset::subset(imagePoints, validIndices);
		objectPoints = Subset::subset(objectPoints, validIndices);
		objectPointIds = Subset::subset(objectPointIds, validIndices);
	}

	// if we do not have any object point we return True, as we do not have any error - we simply could not determine any new object point location
	if (objectPointIds.empty())
	{
		return true;
	}

	// **TODO** Numeric::maxValue() ->

	Vectors3 newObjectPoints;
	Indices32 newObjectPointIds;
	if (!Solver3::determineUnknownObjectPoints(database, AnyCameraPinhole(pinholeCamera), cameraMotion, objectPointIds, newObjectPoints, newObjectPointIds, randomGenerator, nullptr, 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort))
		return false;

	// we determine the minimal number of correspondences before we add the new object point locations

	unsigned int minimalCorrespondences;
	if (!database.poseWithLeastCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &minimalCorrespondences, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())))
	{
		return false;
	}

	database.setObjectPoints<false>(newObjectPointIds.data(), newObjectPoints.data(), newObjectPointIds.size());

	Log::info() << "Added " << newObjectPointIds.size() << " new object points in the specified region";

	// now we optimize all object points having a valid 3D location
	objectPointIds = database.objectPointIds<false, false, false>(lowerFrame, upperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

	if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, cameraMotion, database, randomGenerator, lowerFrame, upperFrame, objectPointIds, minimalCorrespondences, true, 5u, Geometry::Estimator::ET_SQUARE, Scalar(1.0), Scalar(3.5 * 3.5), nullptr, nullptr, abort))
	{
		return false;
	}

	// finally we remove all object points not accurate enough
	return removeInaccurateObjectPoints(pinholeCamera, cameraMotion, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(1.0), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), nullptr, nullptr, abort);
}

bool SLAMTracker::addUnknownObjectPointsInPlanarRegionOfInterest(const PinholeCamera& pinholeCamera, Database& database, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Solver3::CameraMotion cameraMotion, const unsigned int minimalObservations, Plane3* resultingPlane, bool* abort)
{
	ocean_assert(lowerFrame <= regionOfInterestFrame && regionOfInterestFrame <= upperFrame);
	ocean_assert(!regionOfInterest.isEmpty());

	// first we try to determine the 3D object point locations for all object points which are visible in the region of interest in the corresponding frame

	HomogenousMatrix4 regionOfInterestPose;
	if (!database.hasPose<false>(regionOfInterestFrame, &regionOfInterestPose) || !regionOfInterestPose.isValid())
	{
		return false;
	}

	if (!addUnknownObjectPointsInRegionOfInterest(pinholeCamera, database, randomGenerator, lowerFrame, regionOfInterestFrame, upperFrame, regionOfInterest, cameraMotion, minimalObservations, abort))
		return false;

	// now we determine the plane which is covered by the region of interest

	Plane3 plane;

	if (cameraMotion & Solver3::CM_TRANSLATIONAL)
	{
		if (!Solver3::determinePlane(database, pinholeCamera, lowerFrame, regionOfInterestFrame, upperFrame, regionOfInterest, randomGenerator, plane, pinholeCamera.hasDistortionParameters(), Solver3::RelativeThreshold(3u, Scalar(0.5), 20u), Scalar(6), Geometry::Estimator::ET_HUBER))
		{
			return false;
		}
	}
	else
	{
		if (!Solver3::determinePerpendicularPlane(database, pinholeCamera, regionOfInterestFrame, regionOfInterest.boundingBox().center(), 1, plane, pinholeCamera.hasDistortionParameters()))
		{
			return false;
		}
	}

	ocean_assert(plane.isValid());

	if (resultingPlane)
	{
		*resultingPlane = plane;
	}

	// now we identify all object point ids of object points which have no location and which are visible in the region of interest

	IndexSet32 objectPointIdSet;

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	Indices32 objectPointIds;

	for (unsigned int frameIndex = lowerFrame; frameIndex <= upperFrame; ++frameIndex)
	{
		HomogenousMatrix4 framePose;
		if (database.hasPose<false>(frameIndex, &framePose) && framePose.isValid())
		{
			const SquareMatrix3 homography = Geometry::Homography::homographyMatrix(framePose, regionOfInterestPose, pinholeCamera, pinholeCamera, plane);
			const SquareMatrix3 normalizedHomography(pinholeCamera.invertedIntrinsic() * homography * pinholeCamera.intrinsic());

			imagePoints.clear();
			objectPoints.clear();
			objectPointIds.clear();
			database.imagePointsObjectPoints<false, true>(frameIndex, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), 0, nullptr, &objectPointIds);

			for (size_t n = 0; n < objectPointIds.size(); ++n)
			{
				if (objectPointIdSet.find(objectPointIds[n]) == objectPointIdSet.end())
				{
					if (pinholeCamera.hasDistortionParameters())
					{
						const Vector2 undistortedNormalizedImagePoint(pinholeCamera.imagePoint2normalizedImagePoint<true>(imagePoints[n], true));
						const Vector2 undistortedNormalizedSubRegionFrameImagePoint(normalizedHomography * undistortedNormalizedImagePoint);
						const Vector2 regionOfInterestFrameImagePoint(pinholeCamera.normalizedImagePoint2imagePoint<true>(undistortedNormalizedSubRegionFrameImagePoint, true));

						if (regionOfInterest.isInside(regionOfInterestFrameImagePoint))
						{
							objectPointIdSet.insert(objectPointIds[n]);
						}
					}
					else
					{
						if (regionOfInterest.isInside(homography * imagePoints[n]))
						{
							objectPointIdSet.insert(objectPointIds[n]);
						}
					}
				}
			}
		}
	}

	// if we do not have any object point we return True, as we do not have any error - we simply could not determine any new object point location
	if (objectPointIdSet.empty())
	{
		return true;
	}

	// now we determine the locations of all identified object points

	objectPointIds = Indices32(objectPointIdSet.begin(), objectPointIdSet.end());

	// **TODO** Numeric::maxValue() ->

	Vectors3 newObjectPoints;
	Indices32 newObjectPointIds;
	if (!Solver3::determineUnknownObjectPoints(database, AnyCameraPinhole(pinholeCamera), cameraMotion, objectPointIds, newObjectPoints, newObjectPointIds, randomGenerator, nullptr, minimalObservations, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort))
	{
		return false;
	}

	// we determine the minimal number of correspondences before we add the new object point locations

	unsigned int minimalCorrespondences;
	if (!database.poseWithLeastCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &minimalCorrespondences, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())))
	{
		return false;
	}

	if (!newObjectPointIds.empty())
	{
		database.setObjectPoints<false>(newObjectPointIds.data(), newObjectPoints.data(), newObjectPointIds.size());

		Log::info() << "Added " << newObjectPointIds.size() << " new object points in the specified region(s)";

		objectPointIds = database.objectPointIds<false, false, false>(lowerFrame, upperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, cameraMotion, database, randomGenerator, lowerFrame, upperFrame, objectPointIds, minimalCorrespondences, true, 5u, Geometry::Estimator::ET_SQUARE, Scalar(1.0), Scalar(3.5 * 3.5), nullptr, nullptr, abort))
		{
			return false;
		}
	}

	return removeInaccurateObjectPoints(pinholeCamera, cameraMotion, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(1.0), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), nullptr, nullptr, abort);
}

bool SLAMTracker::determineValidCorrespondencesInPlanarRegionOfInterest(const PinholeCamera& pinholeCamera, const Database& database, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Plane3& plane, Indices32* validCorrespondences, Scalar* meanCorrespondences, unsigned int* medianCorrespondences, bool* /*abort*/)
{
	ocean_assert(pinholeCamera.isValid() && regionOfInterest.isEmpty());
	ocean_assert(lowerFrame <= regionOfInterestFrame && regionOfInterestFrame <= upperFrame);

	HomogenousMatrix4 regionOfInterestPose;
	if (!database.hasPose<false>(regionOfInterestFrame, &regionOfInterestPose) || !regionOfInterestPose.isValid())
	{
		return false;
	}

	Indices32 correspondences;
	correspondences.reserve(upperFrame - lowerFrame + 1u);

	Vectors2 imagePoints;
	Vectors3 objectPoints;

	for (unsigned int frameIndex = lowerFrame; frameIndex <= upperFrame; ++frameIndex)
	{
		unsigned int count = 0u;

		HomogenousMatrix4 framePose;
		if (database.hasPose<false>(frameIndex, &framePose) && framePose.isValid())
		{
			const SquareMatrix3 homography = Geometry::Homography::homographyMatrix(framePose, regionOfInterestPose, pinholeCamera, pinholeCamera, plane);
			const SquareMatrix3 normalizedHomography(pinholeCamera.invertedIntrinsic() * homography * pinholeCamera.intrinsic());

			imagePoints.clear();
			objectPoints.clear();
			database.imagePointsObjectPoints<false, false>(frameIndex, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

			for (Vectors2::const_iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
			{
				if (pinholeCamera.hasDistortionParameters())
				{
					const Vector2 undistortedNormalizedImagePoint(pinholeCamera.imagePoint2normalizedImagePoint<true>(*i, true));
					const Vector2 undistortedNormalizedSubRegionFrameImagePoint(normalizedHomography * undistortedNormalizedImagePoint);
					const Vector2 regionOfInterestFrameImagePoint(pinholeCamera.normalizedImagePoint2imagePoint<true>(undistortedNormalizedSubRegionFrameImagePoint, true));

					if (regionOfInterest.isInside(regionOfInterestFrameImagePoint))
					{
						count++;
					}
				}
				else
				{
					if (regionOfInterest.isInside(homography * *i))
					{
						count++;
					}
				}
			}
		}

		correspondences.push_back(count);
	}

	if (medianCorrespondences)
	{
		*medianCorrespondences = Median::constMedian(correspondences.data(), correspondences.size());
	}

	if (meanCorrespondences)
	{
		Scalar sum = 0;
		for (Indices32::const_iterator i = correspondences.begin(); i != correspondences.end(); ++i)
		{
			sum += Scalar(*i);
		}

		*meanCorrespondences = correspondences.empty() ? 0 : Scalar(sum) / Scalar(correspondences.size());
	}

	if (validCorrespondences)
	{
		*validCorrespondences = std::move(correspondences);
	}

	return true;
}

bool SLAMTracker::determineNumberImagePointsInPlanarRegionOfInterest(const PinholeCamera& pinholeCamera, const Database& database, const unsigned int lowerFrame, const unsigned int regionOfInterestFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Plane3& plane, Indices32* numberImagePoints, Scalar* meanNumberImagePoints, unsigned int* medianNumberImagePoints, bool* /*abort*/)
{
	ocean_assert(pinholeCamera.isValid() && !regionOfInterest.isEmpty());
	ocean_assert(lowerFrame <= regionOfInterestFrame && regionOfInterestFrame <= upperFrame);

	HomogenousMatrix4 regionOfInterestPose;
	if (!database.hasPose<false>(regionOfInterestFrame, &regionOfInterestPose) || !regionOfInterestPose.isValid())
	{
		return false;
	}

	Indices32 points;
	points.reserve(upperFrame - lowerFrame + 1u);

	Vectors2 imagePoints;

	for (unsigned int frameIndex = lowerFrame; frameIndex <= upperFrame; ++frameIndex)
	{
		unsigned int count = 0u;

		HomogenousMatrix4 framePose;
		if (database.hasPose<false>(frameIndex, &framePose) && framePose.isValid())
		{
			const SquareMatrix3 homography = Geometry::Homography::homographyMatrix(framePose, regionOfInterestPose, pinholeCamera, pinholeCamera, plane);
			const SquareMatrix3 normalizedHomography(pinholeCamera.invertedIntrinsic() * homography * pinholeCamera.intrinsic());

			imagePoints = database.imagePoints<false>(frameIndex);

			for (Vectors2::const_iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
			{
				if (pinholeCamera.hasDistortionParameters())
				{
					const Vector2 undistortedNormalizedImagePoint(pinholeCamera.imagePoint2normalizedImagePoint<true>(*i, true));
					const Vector2 undistortedNormalizedSubRegionFrameImagePoint(normalizedHomography * undistortedNormalizedImagePoint);
					const Vector2 regionOfInterestFrameImagePoint(pinholeCamera.normalizedImagePoint2imagePoint<true>(undistortedNormalizedSubRegionFrameImagePoint, true));

					if (regionOfInterest.isInside(regionOfInterestFrameImagePoint))
					{
						count++;
					}
				}
				else
				{
					if (regionOfInterest.isInside(homography * *i))
					{
						count++;
					}
				}
			}
		}

		points.push_back(count);
	}

	if (medianNumberImagePoints)
	{
		*medianNumberImagePoints = Median::constMedian(points.data(), points.size());
	}

	if (meanNumberImagePoints)
	{
		Scalar sum = 0;
		for (Indices32::const_iterator i = points.begin(); i != points.end(); ++i)
		{
			sum += Scalar(*i);
		}

		*meanNumberImagePoints = points.empty() ? 0 : Scalar(sum) / Scalar(points.size());
	}

	if (numberImagePoints)
	{
		*numberImagePoints = std::move(points);
	}

	return true;
}

bool SLAMTracker::adjustPosesToCameraWithoutDistortion(const PinholeCamera& oldCamera, const TransformationMap& oldPoses, const PinholeCamera& newCamera, TransformationMap& newPoses, TransformationMap& newObjectTransformations)
{
	ocean_assert(oldCamera.isValid() && !oldPoses.empty() && newCamera.isValid());
	ocean_assert(!oldCamera.hasDistortionParameters() && !newCamera.hasDistortionParameters());

	if (oldCamera == newCamera)
	{
		newPoses = oldPoses;
		return true;
	}

	// Camera_old * Pose_old = Camera_new * Pose_new
	// Pose_new = Camera_new^-1 * Camera_old * Pose_old

	// Pose_new will contain scale and shear factors
	// Thus, we extract the scale and shear to an explicit object transformation changing for each pose

	ocean_assert((newCamera.invertedIntrinsic() * newCamera.intrinsic()).isIdentity());
	const SquareMatrix3 intrinsicDelta(newCamera.invertedIntrinsic() * oldCamera.intrinsic());

	ocean_assert(newPoses.empty() && newObjectTransformations.empty());

	newPoses.clear();
	newObjectTransformations.clear();

	for (TransformationMap::const_iterator i = oldPoses.begin(); i != oldPoses.end(); ++i)
	{
		const HomogenousMatrix4& oldPose = i->second;
		ocean_assert(oldPose.isValid());

		const HomogenousMatrix4 oldPoseIF(PinholeCamera::standard2InvertedFlipped(oldPose));
		ocean_assert(oldPoseIF.rotationMatrix().isOrthonormal());

		const SquareMatrix3 oldRotationIF(oldPoseIF.rotationMatrix());

		// 'rotation' containing scale and shear (transformation from scale-shear coordinate system to world coordinate system)
		const SquareMatrix3 wRs(intrinsicDelta * oldRotationIF);

		// real rotation without scale and shear (transformation from normal coordinate system to world coordinate system)
		const SquareMatrix3 wRn = wRs.orthonormalMatrix();

		// transformation transforming the scale-shear coordinate system to the normal coordinate systeem
		const SquareMatrix3 nRs = wRn.inverted() * wRs;

		// the translation is not affected by the scale-shear problem
		const Vector3 translation = intrinsicDelta * oldPoseIF.translation();

		const HomogenousMatrix4 newPoseIF(translation, wRn);
		const HomogenousMatrix4 objectTransformation(nRs);

#ifdef OCEAN_DEBUG
		{
			const Vector2 oldImagePoint(Random::scalar(Scalar(0.25), Scalar(0.75)) * Scalar(oldCamera.width()), Random::scalar(Scalar(0.25), Scalar(0.75)) * Scalar(oldCamera.height()));

			const Vector3 objectPoint(oldCamera.ray(oldImagePoint, PinholeCamera::invertedFlipped2Standard(oldPoseIF)).point(Random::scalar(1, 10)));
			ocean_assert(oldImagePoint.isEqual(oldCamera.projectToImageIF<true>(oldPoseIF, objectPoint, false), Scalar(0.01)));

			const Vector2 newImagePoint(newCamera.projectToImageIF<true>(newPoseIF, objectTransformation * objectPoint, false));
			ocean_assert(oldImagePoint.isEqual(newImagePoint, Scalar(0.1)));
		}
#endif

		newPoses[i->first] = PinholeCamera::invertedFlipped2Standard(newPoseIF);
		newObjectTransformations[i->first] = objectTransformation;
	}

	return true;
}

bool SLAMTracker::adjustPosesAndPlaneToCamera(const PinholeCamera& oldCamera, const OfflinePoses& oldPoses, const HomogenousMatrix4& oldPlaneTransformation, const unsigned int selectionFrameIndex, const PinholeCamera& newCamera, OfflinePoses& newPoses, HomogenousMatrix4& newPlaneTransformation, const unsigned int iterations, Scalar* finalError)
{
	ocean_assert(oldCamera.isValid() && !oldPoses.isEmpty() && oldPlaneTransformation.isValid() && newCamera.isValid() && iterations >= 1u);

	if (!oldCamera.isValid() || !newCamera.isValid() || !oldPlaneTransformation.isValid() || iterations == 0u)
	{
		return false;
	}

	if (oldCamera == newCamera || oldPoses.size() <= 1)
	{
		newPoses = oldPoses;
		newPlaneTransformation = oldPlaneTransformation;
		return true;
	}

	// we ensure that the provided selection frame (if provided) lies inside the boundaries of the provided poses
	ocean_assert(selectionFrameIndex == (unsigned int)(-1) || oldPoses.isValidIndex(OfflinePoses::Index(selectionFrameIndex)));
	if (selectionFrameIndex != (unsigned int)(-1) && !oldPoses.isValidIndex(OfflinePoses::Index(selectionFrameIndex)))
	{
		return false;
	}

	ocean_assert(oldPoses.firstIndex() >= 0 && oldPoses.lastIndex() >= 0);
	if (oldPoses.firstIndex() < 0 || oldPoses.lastIndex() < 0)
	{
		return false;
	}

	// we determine the size of the environment by determining the bounding box of the camera locations
	Box3 environmentBoundingBox;

	// we ensure whether each of the provided poses is valid
	for (OfflinePoses::Index n = oldPoses.firstIndex(); n < oldPoses.endIndex(); ++n)
	{
		ocean_assert(oldPoses[n].transformation().isValid());
		if (!oldPoses[n].transformation().isValid())
		{
			return false;
		}

		environmentBoundingBox += oldPoses[n].transformation().translation();
	}

	const Scalar enviornmentSize = environmentBoundingBox.diagonal();
	const Scalar nonPlaneObjectPointDistance = max(Scalar(5), enviornmentSize * Scalar(5));

	const Scalar minimalDistanceToCamera = Numeric::eps() * 100;

	const Plane3 oldPlane(oldPlaneTransformation);
	ocean_assert(oldPlane.isValid());
	ocean_assert(oldPlane.isInPlane(oldPlaneTransformation.translation()));

	// now we specify object points so that in each camera frame enough object points are visible
	Vectors3 objectPoints;
	Indices32 planeObjectPointIds;

	unsigned int oldPlanePointVisibleCount = 0u;
	for (OfflinePoses::Index n = oldPoses.firstIndex(); oldPlanePointVisibleCount < 3u && n < oldPoses.endIndex(); ++n)
	{
		if (oldCamera.isInside(oldCamera.projectToImageDamped(oldPoses[n].transformation(), oldPlaneTransformation.translation(), true)))
		{
			if (oldPlanePointVisibleCount == 0u)
			{
				objectPoints.push_back(oldPlaneTransformation.translation());
			}

			oldPlanePointVisibleCount++;
		}
	}

	ocean_assert(objectPoints.size() <= 1);

	if (selectionFrameIndex != (unsigned int)(-1))
	{
		const HomogenousMatrix4& oldPose = oldPoses[selectionFrameIndex].transformation();
		const HomogenousMatrix4 oldPoseIF(PinholeCamera::standard2InvertedFlipped(oldPose));

		// we select 16 image points at location (0.2)x(0.2) -> (0.8)x(0.8) in relation to the frame dimension
		for (unsigned int y = 0u; y < 4u; ++y)
		{
			for (unsigned int x = 0u; x < 4u; ++x)
			{
				const Vector2 distortedImagePoint(Scalar(oldCamera.width()) * Scalar(x + 1u) * Scalar(0.2), Scalar(oldCamera.height()) * Scalar(y + 1u) * Scalar(0.2));
				const Vector2 undistortedImagePoint(oldCamera.undistortDamped(distortedImagePoint));

				if (oldCamera.isInside(undistortedImagePoint))
				{
					const Line3 ray(oldCamera.ray(undistortedImagePoint, oldPose));

					Vector3 objectPoint;
					if (oldPlane.intersection(ray, objectPoint) && (oldPoseIF * objectPoint).z() > minimalDistanceToCamera)
					{
						planeObjectPointIds.push_back(Index32(objectPoints.size()));

						objectPoints.push_back(objectPoint);
					}
				}
			}
		}
	}

	// now we check all camera frames with at least 10 object points will project into the camera frames, otherwise we add new object points

	for (OfflinePoses::Index n = oldPoses.firstIndex(); n < oldPoses.endIndex(); ++n)
	{
		const HomogenousMatrix4& oldPose = oldPoses[n].transformation();
		const HomogenousMatrix4 oldPoseIF(PinholeCamera::standard2InvertedFlipped(oldPose));

		unsigned int validObjectPoints = 0u;

		for (size_t i = 0; i < objectPoints.size(); ++i)
		{
			if ((oldPoseIF * objectPoints[i]).z() > minimalDistanceToCamera && oldCamera.isInside(oldCamera.projectToImageDampedIF(oldPoseIF, objectPoints[i], true)))
			{
				validObjectPoints++;
			}
		}

		if (validObjectPoints < 10u)
		{
			// we add new object points

			// we select 16 image points at location (0.2)x(0.2) -> (0.8)x(0.8) in relation to the frame dimension
			for (unsigned int y = 0u; y < 4u; ++y)
			{
				for (unsigned int x = 0u; x < 4u; ++x)
				{
					const Vector2 distortedImagePoint(Scalar(oldCamera.width()) * Scalar(x + 1u) * Scalar(0.2), Scalar(oldCamera.height()) * Scalar(y + 1u) * Scalar(0.2));
					const Vector2 undistortedImagePoint(oldCamera.undistortDamped(distortedImagePoint));

					if (oldCamera.isInside(undistortedImagePoint))
					{
						const Line3 ray(oldCamera.ray(undistortedImagePoint, oldPose));

						Vector3 objectPoint;
						if (oldPlane.intersection(ray, objectPoint) && (oldPoseIF * objectPoint).z() > minimalDistanceToCamera)
						{
							planeObjectPointIds.push_back(Index32(objectPoints.size()));

							objectPoints.push_back(objectPoint);
							validObjectPoints++;
						}
					}
				}
			}

			// if we still do not have enough object points we avoid the application of the plane-intersection
			if (validObjectPoints < 10u)
			{
				for (unsigned int y = 0u; y < 4u; ++y)
				{
					for (unsigned int x = 0u; x < 4u; ++x)
					{
						const Vector2 distortedImagePoint(Scalar(oldCamera.width()) * Scalar(x + 1u) * Scalar(0.2), Scalar(oldCamera.height()) * Scalar(y + 1u) * Scalar(0.2));
						const Vector2 undistortedImagePoint(oldCamera.undistortDamped(distortedImagePoint));

						if (oldCamera.isInside(undistortedImagePoint))
						{
							const Line3 ray(oldCamera.ray(undistortedImagePoint, oldPose));
							const Vector3 objectPoint(ray.point(nonPlaneObjectPointDistance));

							ocean_assert((oldPose * objectPoint).z() <= Numeric::eps());

							planeObjectPointIds.push_back(Index32(objectPoints.size()));
							objectPoints.push_back(objectPoint);
							validObjectPoints++;
						}
					}
				}
			}

			ocean_assert(validObjectPoints >= 10u);
		}
	}

	// now we (again) determine the projected object point locations and fill the database

	Database database;

	Indices32 objectPointIds(objectPoints.size());

	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		const Index32 objectPointId = database.addObjectPoint<false>(objectPoints[n]);
		ocean_assert_and_suppress_unused(objectPointId == Index32(n), objectPointId);

		objectPointIds[n] = Index32(n);
	}

	for (OfflinePoses::Index n = oldPoses.firstIndex(); n < oldPoses.endIndex(); ++n)
	{
		const Index32 poseId = Index32(n);

		const HomogenousMatrix4& oldPose = oldPoses[n].transformation();
		const HomogenousMatrix4 oldPoseIF(PinholeCamera::standard2InvertedFlipped(oldPose));

		const bool result = database.addPose<false>(poseId, oldPose);
		ocean_assert_and_suppress_unused(result, result);

		for (Index32 objectPointId = 0u; objectPointId < objectPoints.size(); ++objectPointId)
		{
			const Vector3& objectPoint = objectPoints[objectPointId];

			if ((oldPoseIF * objectPoint).z() > minimalDistanceToCamera)
			{
				const Vector2 distortedImagePoint(oldCamera.projectToImageDampedIF(oldPoseIF, objectPoint, true));

				if (oldCamera.isInside(distortedImagePoint))
				{
					const Index32 imagePointId = database.addImagePoint<false>(distortedImagePoint);

					database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
					database.attachImagePointToPose<false>(imagePointId, Index32(poseId));
				}
			}
		}
	}

	const Index32 lowerPoseId = Index32(oldPoses.firstIndex());
	const Index32 upperPoseId = Index32(oldPoses.lastIndex());

	Indices32 usedKeyFrameIds;
	Vectors3 optimizedObjectPoints;

	if (!Solver3::optimizeObjectPointsWithVariablePoses(database, newCamera, lowerPoseId, upperPoseId, objectPointIds, usedKeyFrameIds, optimizedObjectPoints, 10u, min(upperPoseId - lowerPoseId + 1u, 20u), Geometry::Estimator::ET_SQUARE, iterations, nullptr, finalError))
	{
		return false;
	}

	if (finalError)
	{
		*finalError = Numeric::sqrt(*finalError);
	}

	// we invalidate the locations of all 3D object points and set the new locations afterwards
	database.setObjectPoints<false>();
	database.setObjectPoints<false>(objectPointIds.data(), optimizedObjectPoints.data(), objectPointIds.size());

	// **TODO** as we are sure that all point correspondences are valid we should use a simple optimization approach without RANSAC
	RandomGenerator randomGenerator1(0u);
	if (!Solver3::updatePoses(database, AnyCameraPinhole(newCamera), Solver3::CM_UNKNOWN, randomGenerator1, (unsigned int)oldPoses.firstIndex(), (unsigned int)oldPoses.lastIndex(), 5u, Geometry::Estimator::ET_SQUARE, Scalar(1), Numeric::maxValue(), Numeric::maxValue(), nullptr, nullptr))
	{
		return false;
	}

	unsigned int lowerValidPose = (unsigned int)(-1);
	unsigned int upperValidPose = (unsigned int)(-1);
	if (!database.validPoseRange<false>((unsigned int)oldPoses.firstIndex(), selectionFrameIndex, (unsigned int)oldPoses.lastIndex(), lowerValidPose, upperValidPose) && !database.largestValidPoseRange<false>((unsigned int)oldPoses.firstIndex(), (unsigned int)oldPoses.lastIndex(), lowerValidPose, upperValidPose))
	{
		return false;
	}

	lowerValidPose = max(lowerValidPose, (unsigned int)oldPoses.firstIndex());
	upperValidPose = min(upperValidPose, (unsigned int)oldPoses.lastIndex());

	if (lowerValidPose > upperValidPose)
	{
		return false;
	}

	newPoses.clear();
	newPoses.setFirstIndex(lowerValidPose);

	for (unsigned int n = lowerValidPose; n <= upperValidPose; ++n)
	{
		newPoses.insert(n, OfflinePose(n, database.pose<false>(n)));
	}

	RandomGenerator randomGenerator2(0u);

	Plane3 newPlane;
	if (!Geometry::RANSAC::plane(ConstArrayAccessor<Vector3>(database.objectPoints<false>(planeObjectPointIds)), randomGenerator2, newPlane, 100u, Scalar(-1), 3u, Geometry::Estimator::ET_SQUARE))
	{
		return false;
	}

	newPlaneTransformation.toNull();

	if (oldPlanePointVisibleCount >= 3u)
	{
		const Vector3& newObjectPoint = optimizedObjectPoints.front();
		const Vector3 newObjectPointOnPlane(newPlane.projectOnPlane(newObjectPoint));

		const bool result = newPlane.transformation(newObjectPointOnPlane, Vector3(0, 0, 1), newPlaneTransformation);
		ocean_assert_and_suppress_unused(result, result);
	}
	else
	{
		// we take the average offset between the old object points and the new object points

		Vector3 offset(0, 0, 0);
		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			offset += optimizedObjectPoints[n] - objectPoints[n];
		}

		ocean_assert(objectPoints.size() != 0);
		offset /= Scalar(objectPoints.size());

		const Vector3 newObjectPoint(oldPlaneTransformation.translation() + offset);
		const Vector3 newObjectPointOnPlane(newPlane.projectOnPlane(newObjectPoint));

		const bool result = newPlane.transformation(newObjectPointOnPlane, Vector3(0, 0, 1), newPlaneTransformation);
		ocean_assert_and_suppress_unused(result, result);
	}

	if (newPlaneTransformation.isNull())
	{
		ocean_assert(false && "This should never happen!");

		if (!newPlane.transformation(newPlane.pointOnPlane(), Vector3(0, 1, 0), newPlaneTransformation))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	// as the plane's normal may point towards the camera or may point back from the camera we may have to switch the plane's normal

	const Vector3 poseTranslation = newPoses[newPoses.firstIndex()].transformation().translation();

	const Vector3 pointInPlane(Plane3(newPlaneTransformation).projectOnPlane(poseTranslation));
	ocean_assert(Plane3(newPlaneTransformation).isInPlane(pointInPlane));

	const Vector3 directionToPose(poseTranslation - pointInPlane); // may be the zero vector

	if (newPlaneTransformation.zAxis() * directionToPose < 0)
	{
		newPlaneTransformation = PinholeCamera::flippedTransformationRightSide(newPlaneTransformation);
	}

	// now we finally need to rotate the plane transformation around the z-axis so that it matches with the old transformation

	const Vector3 targetVectorXY = Vector3(1, 1, 0) * Scalar(0.70710678118654752440084436210485);
	Vector3 sourceVectorXY = newPlaneTransformation.inverted() * newPlane.projectOnPlane(oldPlaneTransformation * Vector3(1, 1, 0));

	if (sourceVectorXY.normalize())
	{
		ocean_assert(Numeric::isEqual(targetVectorXY.length(), 1));

		const Quaternion sRt(targetVectorXY, sourceVectorXY);

		ocean_assert(sRt * Vector3(0, 0, 1) == Vector3(0, 0, 1));
		ocean_assert(sourceVectorXY == sRt * targetVectorXY);

		newPlaneTransformation = HomogenousMatrix4(newPlaneTransformation.translation(), newPlaneTransformation.rotationMatrix() * SquareMatrix3(sRt));
	}
	else
	{
		ocean_assert(false && "This should never happen!");
	}

	ocean_assert(newPlaneTransformation.isValid());
	return true;
}

bool SLAMTracker::extendStableObjectPointsFull(const PinholeCamera& pinholeCamera, Database& database, const Solver3::CameraMotion cameraMotion, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort, Scalar* progress)
{
	ocean_assert(cameraMotion != Solver3::CM_INVALID);
	ocean_assert(cameraMotion != Solver3::CM_UNKNOWN);

	if (cameraMotion & Solver3::CM_TRANSLATIONAL)
	{
		// we have a translational camera motion (can also have rotational motion)
		ocean_assert((cameraMotion & Solver3::CM_TRANSLATIONAL) != 0 || (cameraMotion & Solver3::CM_TRANSLATIONAL_TINY) != 0
					|| (cameraMotion & Solver3::CM_TRANSLATIONAL_MODERATE) != 0 || (cameraMotion & Solver3::CM_TRANSLATIONAL_SIGNIFICANT) != 0);

		return extendStableObjectPointsFullTranslational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress);
	}
	else
	{
		// we have a rotational camera motion, or the camera motion is static
		ocean_assert(cameraMotion == Solver3::CM_STATIC || cameraMotion == Solver3::CM_ROTATIONAL || cameraMotion == Solver3::CM_ROTATIONAL_TINY
					|| cameraMotion == Solver3::CM_ROTATIONAL_MODERATE || cameraMotion == Solver3::CM_ROTATIONAL_SIGNIFICANT);

		if (extendStableObjectPointsFullRotational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress))
		{
			return true;
		}

		// if the extension of the database for rotational camera motion failes we apply the extension for the translation camera motion as backup
		Log::info() << "Extension for rotational camera motion failed, thus we try a translational camera motion as backup.";

		// **TODO**
		ocean_assert(false && "Translational pose update necessary before!");

		return extendStableObjectPointsFullTranslational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress);
	}

	ocean_assert(false && "Invalid camera motion");
	return false;
}

bool SLAMTracker::extendStableObjectPointsPartially(const PinholeCamera& pinholeCamera, Database& database, const Solver3::CameraMotion cameraMotion, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort, Scalar* progress)
{
	ocean_assert(cameraMotion != Solver3::CM_INVALID);
	ocean_assert(cameraMotion != Solver3::CM_UNKNOWN);

	if (cameraMotion & Solver3::CM_TRANSLATIONAL)
	{
		unsigned int validLowerFrame = 0u;
		unsigned int validUpperFrame = 0u;

		// first we carefully stabilize/add new 3D objects which are visible in the frame range with valid poses

		if (!stabilizeStableObjectPointsPartiallyTranslational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, &validLowerFrame, &validUpperFrame, abort, progress))
		{
			return false;
		}

		if (finalLowerValidPoseRange)
		{
			*finalLowerValidPoseRange = validLowerFrame;
		}
		if (finalUpperValidPoseRange)
		{
			*finalUpperValidPoseRange = validUpperFrame;
		}

		if (validLowerFrame != lowerFrame || validUpperFrame != upperFrame)
		{
			// if we still do not have a valid pose for every frame we have to investigate object points which are visible in the 'stall' frames

			if (!extendStableObjectPointsPartiallyTranslational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress))
			{
				return false;
			}

			// **TODO** special handling as we can have inaccurate object points due to the stall frames..., we need an advanced bundle adjustment
		}

		if (validLowerFrame == lowerFrame && validUpperFrame == upperFrame)
		{
			// if we have now valid poses for every frame we can simply add new object points visible in the entire frame range

			if (!extendStableObjectPointsFullTranslational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress))
			{
				return false;
			}
		}

		Log::info() << "Finished stabilization/extension of 3D object points";

		return true;
	}
	else
	{
		// we have a rotational camera motion, or the camera motion is static
		ocean_assert(cameraMotion == Solver3::CM_STATIC || cameraMotion == Solver3::CM_ROTATIONAL || cameraMotion == Solver3::CM_ROTATIONAL_TINY
					|| cameraMotion == Solver3::CM_ROTATIONAL_MODERATE || cameraMotion == Solver3::CM_ROTATIONAL_SIGNIFICANT);

		if (extendStableObjectPointsPartiallyRotational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress))
			return true;

		// if the extension of the database for rotational camera motion failes we apply the extension for the translation camera motion as backup
		Log::info() << "Extension for rotational camera motion failed, thus we try a translational camera motion as backup.";

		// **TODO**
		ocean_assert(false && "Translational pose update necessary before!");

		return extendStableObjectPointsPartiallyTranslational(pinholeCamera, database, lowerFrame, upperFrame, correspondenceThreshold, finalLowerValidPoseRange, finalUpperValidPoseRange, abort, progress);
	}

	return false;
}

bool SLAMTracker::extendStableObjectPointsFullRotational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort, Scalar* /*progress*/)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);

	RandomGenerator randomGenerator;

	/**
	 * we have camera poses for every camera frame, so we seem to have a very stable tracking
	 * thus, we can add a large set of 3D object points
	 * as we have rotational camera motion only we cannot determine the real depth positions of 3D object points
	 * thus we do not need to measure the accuracy of 3D object points regarding the observation angles (observation directions)
	 *
	 * in a first step we add all stable object points which are visible in !all! camera frames
	 * afterwards we add object points visible in most frames: 80%, 60% and 40%
	 *
	 * between the steps we apply successive optimizations of object points and camera poses (not as bundle adjustment)
	 */

	const unsigned int allFrames = upperFrame - lowerFrame + 1u;

	Vectors3 newObjectPoints;
	Indices32 newObjectPointIds;

	Vectors3 optimizedObjectPoints;
	Indices32 optimizedObjectPointIds;

	for (const unsigned int minimalObservationPercent : {100u, 80u, 60u, 40u})
	{
		if (abort != nullptr && *abort)
		{
			break;
		}

		newObjectPoints.clear();
		newObjectPointIds.clear();

		// check whether we can stop here
		if (allFrames * minimalObservationPercent / 100u < 2u)
		{
			break;
		}

		if (Solver3::determineUnknownObjectPoints(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_ROTATIONAL, newObjectPoints, newObjectPointIds, randomGenerator, nullptr, Scalar(-1), allFrames * minimalObservationPercent / 100u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort)
				&& !newObjectPoints.empty())
		{
			// we do not need to measure the accuracy of the new object points as the accuracy has been measured by the determination function already

#ifdef OCEAN_DEBUG
			ocean_assert(newObjectPoints.size() == newObjectPointIds.size());
			for (Vectors3::const_iterator i = newObjectPoints.begin(); i != newObjectPoints.end(); ++i)
			{
				ocean_assert(*i != Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
			}
#endif
			database.setObjectPoints<false>(newObjectPointIds.data(), newObjectPoints.data(), newObjectPointIds.size());

			Log::info() << "Added " << newObjectPointIds.size() << " stable and " << minimalObservationPercent << "% visible new object points";

			Indices32 allObjectPointIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

			Scalar initialAverageError, finalAverageError;
			if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, Solver3::CM_ROTATIONAL, database, randomGenerator, lowerFrame, upperFrame, allObjectPointIds, 10u, true, 5u, Geometry::Estimator::ET_SQUARE, Scalar(0.8), Scalar(3.5 * 3.5), &initialAverageError, &finalAverageError, abort))
			{
				return false;
			}

			Log::info() << "Pose updates: " << initialAverageError << " -> " << finalAverageError;
		}
	}

	unsigned int bestCorrespondences;
	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &bestCorrespondences))
	{
		return false;
	}

	const unsigned int minimalCorrespondences = correspondenceThreshold.threshold(bestCorrespondences);

	return removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_ROTATIONAL, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(1), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), finalLowerValidPoseRange, finalUpperValidPoseRange, abort);
}

bool SLAMTracker::extendStableObjectPointsFullTranslational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort, Scalar* progress)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);

	if (progress)
	{
		*progress = 0;
	}

	RandomGenerator randomGenerator;

	/**
	 * we have camera poses for every camera frame, so we seem to have a very stable tracking
	 * thus, we can add a large set of 3D object points
	 * as we have translational camera motion we need/must/can determine precise depth information of 3D object points
	 * the precision of the depth values are determined by the observation angles (observation directions)
	 */

	const std::vector<ReliabilityPair> reliabilityThresholds =
	{
		ReliabilityPair(Numeric::cos(Numeric::deg2rad(Scalar(5.0))), Scalar(0.8)),
		ReliabilityPair(Numeric::cos(Numeric::deg2rad(Scalar(2.0))), Scalar(0.6)),
		ReliabilityPair(Numeric::cos(Numeric::deg2rad(Scalar(1.5))), Scalar(0.4)),
		ReliabilityPair(Numeric::cos(Numeric::deg2rad(Scalar(1.0))), Scalar(0.2)),
		ReliabilityPair(Numeric::cos(Numeric::deg2rad(Scalar(0.5))), Scalar(0.3)),
		ReliabilityPair(Numeric::cos(Numeric::deg2rad(Scalar(0.2))), Scalar(0.2))
	};

	const unsigned int allFrames = upperFrame - lowerFrame + 1u;

	Vectors3 newObjectPointCandidates;
	Indices32 newObjectPointCandidateIds;

	Vectors3 newObjectPoints;
	Indices32 newObjectPointIds;

	size_t totalNewObjectPoints = 0;

	for (size_t iteration = 0; (abort == nullptr || !*abort) && iteration < reliabilityThresholds.size(); ++iteration)
	{
		// check whether we can/should skip the last iterations (as the last iteration is very generous
		if (iteration + 1 == reliabilityThresholds.size() && totalNewObjectPoints >= 50)
		{
			break;
		}

		newObjectPointCandidates.clear();
		newObjectPointCandidateIds.clear();

		if (Solver3::determineUnknownObjectPoints(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_TRANSLATIONAL, newObjectPointCandidates, newObjectPointCandidateIds, randomGenerator, nullptr, Scalar(-1), reliabilityThresholds[iteration].minimalObservations<2u>(allFrames), true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Scalar(7.0 * 7.0), WorkerPool::get().scopedWorker()(), abort)
				&& !newObjectPointCandidates.empty())
		{
			const Scalar maximalCosine = reliabilityThresholds[iteration].maximalCosine();

			Log::info() << "Found " << newObjectPointCandidateIds.size() << " new object point candidates with at least " << reliabilityThresholds[iteration].minimalObservationRatio() * 100 << "% visibility";

			const Scalars newObjectPointCandidateAccuracies = Solver3::determineObjectPointsAccuracy(database, pinholeCamera, newObjectPointCandidateIds, Solver3::AM_MEAN_DIRECTION_MEDIAN_COSINE, lowerFrame, upperFrame, WorkerPool::get().scopedWorker()());
			ocean_assert(newObjectPointCandidateIds.size() == newObjectPointCandidateAccuracies.size());

			newObjectPoints.clear();
			newObjectPointIds.clear();

			for (size_t i = 0; i < newObjectPointCandidateAccuracies.size(); ++i)
			{
				if (newObjectPointCandidateAccuracies[i] > -1 && newObjectPointCandidateAccuracies[i] <= maximalCosine)
				{
					newObjectPoints.push_back(newObjectPointCandidates[i]);
					newObjectPointIds.push_back(newObjectPointCandidateIds[i]);
				}
			}

			if (!newObjectPointIds.empty())
			{
				database.setObjectPoints<false>(newObjectPointIds.data(), newObjectPoints.data(), newObjectPointIds.size());
				totalNewObjectPoints += newObjectPointIds.size();

				Log::info() << "Added " << newObjectPointIds.size() << " new 3D object points (which have more than " << Numeric::rad2deg(Numeric::acos(maximalCosine)) << "deg of observation accuracy)";

				Indices32 allObjectPointIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

				Log::info() << "Now we have overall " << allObjectPointIds.size() << " valid object points in the database";

				/*{
					// **TODO** remove later
					unsigned int minimalCorrespondences = 0u, medianCorrespondences = 0u, maximalCorrespondences = 0u;
					Scalar averageCorrespondences = 0;
					Solver3::determineNumberCorrespondences(database, true, lowerFrame, upperFrame, &minimalCorrespondences, &averageCorrespondences, &medianCorrespondences, &maximalCorrespondences);
					Log::info() << "With the following distribution of correspondences: [" << minimalCorrespondences << ", (" << averageCorrespondences << ", " << medianCorrespondences << "), " << maximalCorrespondences << "]";
				}*/

				// we apply a minimalValidCorrespondenceRatio of 0.90 to support scenes in which up to 10% of the point paths of 3D object points 'jump' to an invalid position e.g, due to areas in the frame with almost identical appearance (regular patterns)

				Scalar initialAverageError, finalAverageError;
				if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, allObjectPointIds, 10u, true, 5u, Geometry::Estimator::ET_SQUARE, Scalar(0.9), Scalar(3.5 * 3.5), &initialAverageError, &finalAverageError, abort))
				{
					return false;
				}

				Log::info() << "Pose updates: " << initialAverageError << " -> " << finalAverageError;

				unsigned int validLowerFrame, validUpperFrame;
				if (!removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, 10u, Scalar(0.9), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), (unsigned int)(-1), &validLowerFrame, &validUpperFrame, abort))
				{
					return false;
				}

				Log::info() << "Now we have a valid pose range: [" << validLowerFrame << ", " << validUpperFrame << "]";

				unsigned int minimalCorrespondences = 0u, medianCorrespondences = 0u, maximalCorrespondences = 0u;
				Scalar averageCorrespondences = 0;

				Solver3::determineNumberCorrespondences(database, true, lowerFrame, upperFrame, &minimalCorrespondences, &averageCorrespondences, &medianCorrespondences, &maximalCorrespondences, WorkerPool::get().scopedWorker()());
				Log::info() << "With the following distribution of correspondences: [" << minimalCorrespondences << ", (" << averageCorrespondences << ", " << medianCorrespondences << "), " << maximalCorrespondences << "]";

				// now we check whether we can stop here as we have enough correspondences in each frame
				if (minimalCorrespondences >= 200u && averageCorrespondences >= 300)
				{
					break;
				}
			}
		}

		if (progress != nullptr)
		{
			*progress = Scalar(iteration + 1u) / Scalar(reliabilityThresholds.size() + 1);
		}
	}

	if (progress != nullptr)
	{
		*progress = Scalar(reliabilityThresholds.size()) / Scalar(reliabilityThresholds.size() + 1);
	}

	// **TODO**
	{
		newObjectPointCandidates.clear();
		newObjectPointCandidateIds.clear();

		if (Solver3::determineUnknownObjectPoints(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_TRANSLATIONAL, newObjectPointCandidates, newObjectPointCandidateIds, randomGenerator, nullptr, Scalar(-1), 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Scalar(7.0 * 7.0), WorkerPool::get().scopedWorker()(), abort)
				&& !newObjectPointCandidates.empty())
		{
			Log::info() << "Found " << newObjectPointCandidateIds.size() << " new object point candidates with arbitrary visibility";

			const Scalars newObjectPointCandidateAccuracies = Solver3::determineObjectPointsAccuracy(database, pinholeCamera, newObjectPointCandidateIds, Solver3::AM_MEAN_DIRECTION_MEDIAN_COSINE, lowerFrame, upperFrame, WorkerPool::get().scopedWorker()());
			ocean_assert(newObjectPointCandidateIds.size() == newObjectPointCandidateAccuracies.size());

			newObjectPoints.clear();
			newObjectPointIds.clear();

			const Scalar maximalCosine = Numeric::cos(Numeric::deg2rad(1.5));

			for (size_t i = 0; i < newObjectPointCandidateAccuracies.size(); ++i)
			{
				if (newObjectPointCandidateAccuracies[i] > -1 && newObjectPointCandidateAccuracies[i] <= maximalCosine)
				{
					newObjectPoints.push_back(newObjectPointCandidates[i]);
					newObjectPointIds.push_back(newObjectPointCandidateIds[i]);
				}
			}

			database.setObjectPoints<false>(newObjectPointIds.data(), newObjectPoints.data(), newObjectPointIds.size());

			Log::info() << "Added " << newObjectPointIds.size() << " new 3D object points (which have more than " << Numeric::rad2deg(Numeric::acos(maximalCosine)) << "deg of observation accuracy)";

			Indices32 allObjectPointIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

			// we apply a minimalValidCorrespondenceRatio of 0.90 to support scenes in which up to 10% of the point paths of 3D object points 'jump' to an invalid position e.g, due to areas in the frame with almost identical appearance (regular patterns)

			Scalar initialAverageError, finalAverageError;
			if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, allObjectPointIds, 10u, true, 5u, Geometry::Estimator::ET_SQUARE, Scalar(0.9), Scalar(3.5 * 3.5), &initialAverageError, &finalAverageError, abort))
			{
				return false;
			}

			Log::info() << "Pose updates: " << initialAverageError << " -> " << finalAverageError;

			unsigned int validLowerFrame, validUpperFrame;
			if (!removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, 10u, Scalar(0.90), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), (unsigned int)(-1), &validLowerFrame, &validUpperFrame, abort))
			{
				return false;
			}

			Log::info() << "Now we have a valid pose range: [" << validLowerFrame << ", " << validUpperFrame << "]";
		}
	}

	unsigned int bestCorrespondences;
	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &bestCorrespondences))
	{
		return false;
	}

	const unsigned int minimalCorrespondences = correspondenceThreshold.threshold(bestCorrespondences);

	return removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(1), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), (unsigned int)(-1), finalLowerValidPoseRange, finalUpperValidPoseRange, abort);
}

bool SLAMTracker::extendStableObjectPointsPartiallyRotational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* /*finalLowerValidPoseRange*/, unsigned int* /*finalUpperValidPoseRange*/, bool* abort, Scalar* /*progress*/)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);

	RandomGenerator randomGenerator;

	/**
	 * we do not have camera poses for every camera frame, so we seem to have complex camera movements, complex scene structures or bad tracking results
	 * thus, we have to add new 3D object points very carefully
	 * as we have rotational camera motion only we cannot determine any depth information of 3D object points
	 * we simply have to back project image points with a specified distance to the camera's center of projection
	 *
	 * further we may receive a translational motion if we add more frames, so we have to handle this situation also
	 */

	Vectors3 newLowerObjectPointCandidates;
	Indices32 newLowerObjectPointCandidateIds;
	Indices32 newLowerObjectPointCandidateObservations;

	Vectors3 newUpperObjectPointCandidates;
	Indices32 newUpperObjectPointCandidateIds;
	Indices32 newUpperObjectPointCandidateObservations;

	unsigned int validLowerFrame = (unsigned int)(-1);
	unsigned int validUpperFrame = (unsigned int)(-1);
	if (!database.largestValidPoseRange<false>(lowerFrame, upperFrame, validLowerFrame, validUpperFrame))
	{
		return false;
	}

	unsigned int previousValidFrameNumber = validUpperFrame - validLowerFrame + 1u;

	const unsigned int lookaheadStart = 128u;

	unsigned int lowerLookaheadStart = lookaheadStart;
	unsigned int upperLookaheadStart = lookaheadStart;

	unsigned int lowerLookaheadEnd = (unsigned int)(-1);
	unsigned int upperLookaheadEnd = (unsigned int)(-1);

	const AnyCameraPinhole camera(pinholeCamera);

	while (true)
	{
		newLowerObjectPointCandidates.clear();
		newLowerObjectPointCandidateIds.clear();
		newLowerObjectPointCandidateObservations.clear();

		newUpperObjectPointCandidates.clear();
		newUpperObjectPointCandidateIds.clear();
		newUpperObjectPointCandidateObservations.clear();

#ifdef OCEAN_DEBUG
		unsigned int debugValidLowerFrame = (unsigned int)(-1);
		unsigned int debugValidUpperFrame = (unsigned int)(-1);
		ocean_assert(database.largestValidPoseRange<false>(lowerFrame, upperFrame, debugValidLowerFrame, debugValidUpperFrame));
		ocean_assert(debugValidLowerFrame == validLowerFrame && debugValidUpperFrame == validUpperFrame);
#endif

		// lower stall frame
		if (validLowerFrame > lowerFrame)
		{
			// we identify all unknown 3D object points which are visible in the lowest frame with valid pose
			// we try to add as much object points as possible as long as their position is reliable and accurate

			const unsigned int correspondencesLowerFrame = database.numberCorrespondences<false, false, true>(validLowerFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
			ocean_assert(correspondencesLowerFrame >= 10u);

			Log::info() << " ";
			Log::info() << "We have a lower stall frame at " << validLowerFrame << " (which is the lowest valid frame) with " << correspondencesLowerFrame << " valid correspondences";

			unsigned int lookahead = lowerLookaheadStart;

			while (lookahead != 0u)
			{
				ocean_assert(validLowerFrame != 0u);
				const unsigned int lowerObservationFrame = max(int(lowerFrame), int(validLowerFrame) - int(lookahead));
				const unsigned int upperObservationFrame = min(validLowerFrame + lookahead, validUpperFrame);

				if (!Solver3::determineUnknownObjectPoints<true>(database, camera, Solver3::CM_ROTATIONAL, lowerObservationFrame, upperObservationFrame, newLowerObjectPointCandidates, newLowerObjectPointCandidateIds, randomGenerator, &newLowerObjectPointCandidateObservations, Scalar(-1), 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort)
						|| newLowerObjectPointCandidates.empty()
						|| (lookahead > 2u && newLowerObjectPointCandidates.size() < correspondencesLowerFrame * 2u)
						|| (lookahead > 1u && newLowerObjectPointCandidates.size() < correspondencesLowerFrame))
				{
					ocean_assert(lookahead / 2u != lookahead);
					lookahead /= 2u;
					continue;
				}

				Log::info() << "Found " << newLowerObjectPointCandidateIds.size() << " new object point candidates within the range [" << lowerObservationFrame << ", " << upperObservationFrame << "]";

#ifdef OCEAN_DEBUG
				{
					for (unsigned int n = lowerObservationFrame; n <= upperObservationFrame; ++n)
					{
						for (Indices32::const_iterator i = newLowerObjectPointCandidateIds.begin(); i != newLowerObjectPointCandidateIds.end(); ++i)
						{
							ocean_assert(database.hasObservation<false>(n, *i));
						}
					}

					Database copyDatabase(database);
					copyDatabase.setObjectPoints<false>(newLowerObjectPointCandidateIds.data(), newLowerObjectPointCandidates.data(), newLowerObjectPointCandidateIds.size());

					Scalars averagedErrors(newLowerObjectPointCandidateIds.size());
					if (!Solver3::determineProjectionErrors(copyDatabase, pinholeCamera, newLowerObjectPointCandidateIds, pinholeCamera.hasDistortionParameters(), lowerFrame, upperFrame, nullptr, averagedErrors.data(), nullptr, nullptr, WorkerPool::get().scopedWorker()()))
					{
						return false;
					}

					for (Scalars::const_iterator i = averagedErrors.begin(); i != averagedErrors.end(); ++i)
					{
						ocean_assert(*i <= Scalar(3.5 * 3.5));
					}
				}
#endif // OCEAN_DEBUG

				break;
			}

			lowerLookaheadEnd = lookahead;
		}

		// upper stall frame
		if (upperFrame > validUpperFrame)
		{
			// we identify all unknown 3D object points which are visible in the highest frame with valid pose
			// we try to add as much object points as possible as long as their position is reliable and accurate

			const unsigned int correspondencesUpperFrame = database.numberCorrespondences<false, false, true>(validUpperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
			ocean_assert(correspondencesUpperFrame >= 10u);

			Log::info() << " ";
			Log::info() << "We have an upper stall frame at " << validUpperFrame << " (which is the highest valid frame) with " << correspondencesUpperFrame << " valid correspondences";

			unsigned int lookahead = upperLookaheadStart;

			while (lookahead != 0u)
			{
				ocean_assert(validLowerFrame != 0u);
				const unsigned int lowerObservationFrame = max(int(validLowerFrame), int(validUpperFrame) - int(lookahead));
				const unsigned int upperObservationFrame = min(validUpperFrame + lookahead, upperFrame);

				if (!Solver3::determineUnknownObjectPoints<true>(database, camera, Solver3::CM_ROTATIONAL, lowerObservationFrame, upperObservationFrame, newUpperObjectPointCandidates, newUpperObjectPointCandidateIds, randomGenerator, &newUpperObjectPointCandidateObservations, Scalar(-1), 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort)
						|| newUpperObjectPointCandidates.empty()
						|| (lookahead > 2u && newUpperObjectPointCandidates.size() < correspondencesUpperFrame * 2u)
						|| (lookahead > 1u && newUpperObjectPointCandidates.size() < correspondencesUpperFrame))
				{
					ocean_assert(lookahead / 2u != lookahead);
					lookahead /= 2u;
					continue;
				}

				Log::info() << "Found " << newUpperObjectPointCandidateIds.size() << " new object point candidates within the range [" << lowerObservationFrame << ", " << upperObservationFrame << "]";

#ifdef OCEAN_DEBUG
				{
					for (unsigned int n = lowerObservationFrame; n <= upperObservationFrame; ++n)
					{
						for (Indices32::const_iterator i = newUpperObjectPointCandidateIds.begin(); i != newUpperObjectPointCandidateIds.end(); ++i)
						{
							ocean_assert(database.hasObservation<false>(n, *i));
						}
					}

					Database copyDatabase(database);
					copyDatabase.setObjectPoints<false>(newUpperObjectPointCandidateIds.data(), newUpperObjectPointCandidates.data(), newUpperObjectPointCandidateIds.size());

					Scalars averagedErrors(newUpperObjectPointCandidateIds.size());
					if (!Solver3::determineProjectionErrors(copyDatabase, pinholeCamera, newUpperObjectPointCandidateIds, pinholeCamera.hasDistortionParameters(), lowerFrame, upperFrame, nullptr, averagedErrors.data(), nullptr, nullptr, WorkerPool::get().scopedWorker()()))
					{
						return false;
					}

					for (Scalars::const_iterator i = averagedErrors.begin(); i != averagedErrors.end(); ++i)
					{
						ocean_assert(*i <= Scalar(3.5 * 3.5));
					}
				}
#endif // OCEAN_DEBUG

				break;
			}

			upperLookaheadEnd = lookahead;
		}

		if (newLowerObjectPointCandidateIds.empty() && newUpperObjectPointCandidateIds.empty())
		{
			break;
		}

		// **TODO** there can be  identical lower and upper object points...

		if (!newLowerObjectPointCandidates.empty())
		{
			database.setObjectPoints<false>(newLowerObjectPointCandidateIds.data(), newLowerObjectPointCandidates.data(), newLowerObjectPointCandidateIds.size());
			Log::info() << "Added " << newLowerObjectPointCandidateIds.size() << " new object points at the lower stall frame";
		}

		if (!newUpperObjectPointCandidates.empty())
		{
			database.setObjectPoints<false>(newUpperObjectPointCandidateIds.data(), newUpperObjectPointCandidates.data(), newUpperObjectPointCandidateIds.size());
			Log::info() << "Added " << newUpperObjectPointCandidateIds.size() << " new object points at the upper stall frame";
		}

		const unsigned int lowerValidFrameCorrespondences = database.numberCorrespondences<false, false, true>(validLowerFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
		const unsigned int upperValidFrameCorrespondences = database.numberCorrespondences<false, false, true>(validUpperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		const unsigned int minimalCorrespondences = correspondenceThreshold.threshold(min(lowerValidFrameCorrespondences, upperValidFrameCorrespondences));

		Indices32 allObjectPointIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
		if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, Solver3::CM_ROTATIONAL, database, randomGenerator, lowerFrame, upperFrame, allObjectPointIds, minimalCorrespondences, true, 4u, Geometry::Estimator::ET_SQUARE, Scalar(0.8), Scalar(3.5 * 3.5), nullptr, nullptr, abort))
		{
			return false;
		}

		if (!removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_ROTATIONAL, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(0.8), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), &validLowerFrame, &validUpperFrame, abort))
		{
			return false;
		}

		// check whether we could not improve the tracking result (currently not based on the frame range but on the frame number which might not be the best idea
		if (validUpperFrame - validLowerFrame + 1u <= previousValidFrameNumber)
		{
			if (lowerLookaheadEnd >= 2u || upperLookaheadEnd >= 2u)
			{
				lowerLookaheadStart = lowerLookaheadEnd / 2u;
				upperLookaheadStart = upperLookaheadEnd / 2u;
				continue;
			}

			Log::info() << "We could not improve the pose range so that we have to stop here.";
			break;
		}

		lowerLookaheadStart = lookaheadStart;
		upperLookaheadStart = lookaheadStart;

		Log::info() << "Now we have valid poses within the range: [" << validLowerFrame << ", " << validUpperFrame << "]";
		Log::info() << " ";

		previousValidFrameNumber = validUpperFrame - validLowerFrame + 1u;
	}

	return true;
}

bool SLAMTracker::stabilizeStableObjectPointsPartiallyTranslational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* finalLowerValidPoseRange, unsigned int* finalUpperValidPoseRange, bool* abort, Scalar* /*progress*/)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);

	RandomGenerator randomGenerator;

	/**
	 * we do not have camera poses for every camera frame, so we seem to have complex camera movements, complex (or large) scene structures or bad tracking results
	 * thus, we have to add new 3D object points very carefully for all existing poses
	 * as we have translational camera motion we need/must/can determine precise depth information of 3D object points
	 * the precision of the depth values are determined by the observation angles (observation directions)
	 * we add new object points to the environment as long as we have 'enough' object points in every camera frame
	 * thus, we have the guarantee that the pose is stable and we hope that several new object points will lead to further valid poses at the borders
	 * hopefully we finally end with camera poses for the entire frame range, if not we will have to handle the borders explicitly afterwards
	 */

	const std::array<Scalar, 5> maximalCosines =
	{
		Numeric::cos(Numeric::deg2rad(5)),
		Numeric::cos(Numeric::deg2rad(3.5)),
		Numeric::cos(Numeric::deg2rad(2)),
		Numeric::cos(Numeric::deg2rad(1.5)),
		Numeric::cos(Numeric::deg2rad(1))
	};

	Vectors3 newObjectPointCandidates;
	Indices32 newObjectPointCandidateIds;

	Vectors3 newObjectPoints;
	Indices32 newObjectPointIds;

	unsigned int validLowerFrame = (unsigned int)(-1);
	unsigned int validUpperFrame = (unsigned int)(-1);
	if (!database.largestValidPoseRange<false>(lowerFrame, upperFrame, validLowerFrame, validUpperFrame))
	{
		return false;
	}

	Log::info() << "Pre initial pose update: [" << validLowerFrame << ", " << validUpperFrame << "]";

	unsigned int bestCorrespondences;
	if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &bestCorrespondences))
	{
		return false;
	}

	Log::info() << "Stabilizing the stable object points within the pose range [" << validLowerFrame << ", " << validUpperFrame << "]";

	const unsigned int initialMinimalCorrespondences = correspondenceThreshold.threshold(bestCorrespondences);
	if (!Solver3::updatePoses(database, AnyCameraPinhole(pinholeCamera), Solver3::CM_TRANSLATIONAL, randomGenerator, lowerFrame, upperFrame, initialMinimalCorrespondences, Geometry::Estimator::ET_SQUARE, Scalar(0.9), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), nullptr, nullptr, WorkerPool::get().scopedWorker()(), abort))
	{
		return false;
	}

	if (!database.largestValidPoseRange<false>(lowerFrame, upperFrame, validLowerFrame, validUpperFrame))
	{
		return false;
	}

	Log::info() << "After initial pose update: [" << validLowerFrame << ", " << validUpperFrame << "]";

	const AnyCameraPinhole camera(pinholeCamera);

	for (size_t iteration = 0; (abort == nullptr || !*abort) && iteration < maximalCosines.size(); ++iteration)
	{
		newObjectPointCandidates.clear();
		newObjectPointCandidateIds.clear();

		// we try to determine the locations of unknown object points by application of the already known camera poses

		if (Solver3::determineUnknownObjectPoints<false>(database, camera, Solver3::CM_TRANSLATIONAL, lowerFrame, upperFrame, newObjectPointCandidates, newObjectPointCandidateIds, randomGenerator, nullptr, Scalar(-1), 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), WorkerPool::get().scopedWorker()(), abort)
				&& !newObjectPointCandidates.empty())
		{
			Log::info() << "Found " << newObjectPointCandidateIds.size() << " new object point candidates";

			// now we have to filter the reliable/good 3D object points from the unreliable/bad object points (by application of the viewing angle)

			const Scalars newObjectPointCandidateAccuracies = Solver3::determineObjectPointsAccuracy(database, pinholeCamera, newObjectPointCandidateIds, Solver3::AM_MEAN_DIRECTION_MEDIAN_COSINE, lowerFrame, upperFrame, WorkerPool::get().scopedWorker()());
			ocean_assert(newObjectPointCandidateIds.size() == newObjectPointCandidateAccuracies.size());

			newObjectPoints.clear();
			newObjectPointIds.clear();

			Scalar maximalCosine;

			do
			{
				maximalCosine = maximalCosines[iteration];

				for (size_t i = 0; i < newObjectPointCandidateAccuracies.size(); ++i)
				{
					if (newObjectPointCandidateAccuracies[i] > -1 && newObjectPointCandidateAccuracies[i] <= maximalCosine)
					{
						ocean_assert(Numeric::acos(newObjectPointCandidateAccuracies[i]) >= Numeric::acos(maximalCosine));

						newObjectPoints.push_back(newObjectPointCandidates[i]);
						newObjectPointIds.push_back(newObjectPointCandidateIds[i]);
					}
				}

				// if we could found at least one object point for the current angle threshold we break here, otherwise we take the next smaller angle threshold
				if (!newObjectPoints.empty())
				{
					break;
				}
			}
			while ((!abort || !*abort) && ++iteration < maximalCosines.size());

			if (!newObjectPoints.empty())
			{
				database.setObjectPoints<false>(newObjectPointIds.data(), newObjectPoints.data(), newObjectPointIds.size());

				Log::info() << "Added " << newObjectPointIds.size() << " new 3D object points (which have more than " << Numeric::rad2deg(Numeric::acos(maximalCosine)) << "deg of observation accuracy)";

				// we determine the minimal number of correspondences due to the frame with most correspondences (before we added the new points)
				const unsigned int minimalCorrespondences = correspondenceThreshold.threshold(bestCorrespondences);

				// we apply a minimalValidCorrespondenceRatio of 0.90 to support scenes in which up to 10% of the point paths of 3D object points 'jump' to an invalid position e.g, due to areas in the frame with almost identical appearance (regular patterns)

				Indices32 allObjectPointIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

				{
					unsigned int lowerValidPose, upperValidPose;
					database.largestValidPoseRange<false>(lowerFrame, upperFrame, lowerValidPose, upperValidPose);
					Log::info() << "Largest valid pose range: [" << lowerValidPose << ", " << upperValidPose << "]";
				}

				Log::info() << "Updating poses and object point locations individually (with " << allObjectPointIds.size() << " object points) accepting poses with at least " << minimalCorrespondences << " correspondences";

				Scalar initialError, finalError;
				if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, allObjectPointIds, minimalCorrespondences, true, 10u, Geometry::Estimator::ET_SQUARE, Scalar(0.9), Scalar(3.5 * 3.5), &initialError, &finalError, abort))
				{
					return false;
				}

				Log::info() << "Database update: " << initialError << " -> " << finalError;

				{
					unsigned int lowerValidPose, upperValidPose;
					database.largestValidPoseRange<false>(lowerFrame, upperFrame, lowerValidPose, upperValidPose);
					Log::info() << "Largest valid pose range: [" << lowerValidPose << ", " << upperValidPose << "]";
				}

				if (!removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(0.90), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), &validLowerFrame, &validUpperFrame, abort))
				{
					return false;
				}

				unsigned int minimalPoseCorrespondences = 0u, medianPoseCorrespondences = 0u, maximalPoseCorrespondences = 0u;
				Scalar averagePoseCorrespondences = 0;
				if (!Solver3::determineNumberCorrespondences(database, true, validLowerFrame, validUpperFrame, &minimalPoseCorrespondences, &averagePoseCorrespondences, &medianPoseCorrespondences, &maximalPoseCorrespondences, WorkerPool::get().scopedWorker()()))
				{
					return false;
				}

				Log::info() << "Now we have valid poses in the range: [" << validLowerFrame << ", " << validUpperFrame << "] with " << averagePoseCorrespondences << " correspondences in average (" << medianPoseCorrespondences << ", " << minimalPoseCorrespondences << " -> " << maximalPoseCorrespondences << ")";

				// check whether we do not need to find more stable 3D object points for the valid poses
				if (validLowerFrame == lowerFrame && validUpperFrame == upperFrame && averagePoseCorrespondences >= 150 && medianPoseCorrespondences >= 150u)
				{
					Log::info() << "We can skip remaining (stabilization) iterations as the poses have enough valid object points.";
					break;
				}

				// we the frame with most correspondences and store the number for the next iteration
				if (!database.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, nullptr, &bestCorrespondences))
				{
					return false;
				}
			}
		}
	}

	if (finalLowerValidPoseRange)
	{
		*finalLowerValidPoseRange = validLowerFrame;
	}

	if (finalUpperValidPoseRange)
	{
		*finalUpperValidPoseRange = validUpperFrame;
	}

	Log::info() << "Finished stabilizing now having valid frames within [" << validLowerFrame << ", " << validUpperFrame << "]";

	return true;
}

bool SLAMTracker::extendStableObjectPointsPartiallyTranslational(const PinholeCamera& pinholeCamera, Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const Solver3::RelativeThreshold& correspondenceThreshold, unsigned int* /*finalLowerValidPoseRange*/, unsigned int* /*finalUpperValidPoseRange*/, bool* abort, Scalar* /*progress*/)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(lowerFrame <= upperFrame);

	RandomGenerator randomGenerator;

	/**
	 * we do not have camera poses for every camera frame, so we seem to have complex camera movements, complex scene structures or bad tracking results
	 * thus, we have to add new 3D object points very carefully
	 * as we have translational camera motion we need/must/can determine precise depth information of 3D object points
	 * the precision of the depth values are determined by the observation angles (observation directions)
	 */

	Vectors3 newLowerObjectPointCandidates;
	Indices32 newLowerObjectPointCandidateIds;
	Indices32 newLowerObjectPointCandidateObservations;

	Vectors3 newUpperObjectPointCandidates;
	Indices32 newUpperObjectPointCandidateIds;
	Indices32 newUpperObjectPointCandidateObservations;

	Vectors3 objectPoints;
	Indices32 objectPointIds;

	unsigned int validLowerFrame = (unsigned int)(-1);
	unsigned int validUpperFrame = (unsigned int)(-1);
	if (!database.largestValidPoseRange<false>(lowerFrame, upperFrame, validLowerFrame, validUpperFrame))
	{
		return false;
	}

	{
		// **TODO** remove later

		unsigned int bestCorrespondencesPoseId, bestCorrespondences;
		if (!database.poseWithMostCorrespondences<false, false, true>(validLowerFrame, validUpperFrame, &bestCorrespondencesPoseId, &bestCorrespondences))
		{
			return false;
		}

		Log::info() << "Best pose: " << bestCorrespondencesPoseId << " with " << bestCorrespondences << " correspondences";
	}

	const AnyCameraPinhole camera(pinholeCamera);

	unsigned int previousValidFrameNumber = validUpperFrame - validLowerFrame + 1u;

	while (true)
	{
		newLowerObjectPointCandidates.clear();
		newLowerObjectPointCandidateIds.clear();
		newLowerObjectPointCandidateObservations.clear();

		newUpperObjectPointCandidates.clear();
		newUpperObjectPointCandidateIds.clear();
		newUpperObjectPointCandidateObservations.clear();

#ifdef OCEAN_DEBUG
		unsigned int debugValidLowerFrame = (unsigned int)(-1);
		unsigned int debugValidUpperFrame = (unsigned int)(-1);
		ocean_assert(database.largestValidPoseRange<false>(lowerFrame, upperFrame, debugValidLowerFrame, debugValidUpperFrame));
		ocean_assert(debugValidLowerFrame == validLowerFrame && debugValidUpperFrame == validUpperFrame);
#endif

		// lower stall frame
		if (validLowerFrame > lowerFrame)
		{
			// we identify all unknown 3D object points which are visible in the lowest frame with valid pose
			// we try to add as much object points as possible as long as their position is reliable and accurate

			const unsigned int correspondencesLowerFrame = database.numberCorrespondences<false, false, true>(validLowerFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
			ocean_assert(correspondencesLowerFrame >= 10u);

			Log::info() << " ";
			Log::info() << "We have a lower stall frame at " << validLowerFrame << " (which is the lowest valid frame) with " << correspondencesLowerFrame << " valid correspondences";

			unsigned int lookahead = 128u;

			while (lookahead != 0u)
			{
				ocean_assert(validLowerFrame != 0u);
				const unsigned int lowerObservationFrame = max(int(lowerFrame), int(validLowerFrame) - int(lookahead));
				const unsigned int upperObservationFrame = min(validLowerFrame + lookahead, validUpperFrame);

				if (!Solver3::determineUnknownObjectPoints<true>(database, camera, Solver3::CM_TRANSLATIONAL, lowerObservationFrame, upperObservationFrame, newLowerObjectPointCandidates, newLowerObjectPointCandidateIds, randomGenerator, &newLowerObjectPointCandidateObservations, Scalar(-1), 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort)
						|| newLowerObjectPointCandidates.empty()
						|| (lookahead > 2u && newLowerObjectPointCandidates.size() < correspondencesLowerFrame * 2u)
						|| (lookahead > 1u && newLowerObjectPointCandidates.size() < correspondencesLowerFrame))
				{
					ocean_assert(lookahead / 2u != lookahead);
					lookahead /= 2u;
					continue;
				}

				Log::info() << "Found " << newLowerObjectPointCandidateIds.size() << " new object point candidates within the range [" << lowerObservationFrame << ", " << upperObservationFrame << "]";

#ifdef OCEAN_DEBUG
				{
					for (unsigned int n = lowerObservationFrame; n <= upperObservationFrame; ++n)
					{
						for (Indices32::const_iterator i = newLowerObjectPointCandidateIds.begin(); i != newLowerObjectPointCandidateIds.end(); ++i)
						{
							ocean_assert(database.hasObservation<false>(n, *i));
						}
					}

					Database copyDatabase(database);
					copyDatabase.setObjectPoints<false>(newLowerObjectPointCandidateIds.data(), newLowerObjectPointCandidates.data(), newLowerObjectPointCandidateIds.size());

					Scalars averagedErrors(newLowerObjectPointCandidateIds.size());
					if (!Solver3::determineProjectionErrors(copyDatabase, pinholeCamera, newLowerObjectPointCandidateIds, pinholeCamera.hasDistortionParameters(), lowerFrame, upperFrame, nullptr, averagedErrors.data(), nullptr, nullptr, WorkerPool::get().scopedWorker()()))
					{
						return false;
					}

					for (Scalars::const_iterator i = averagedErrors.begin(); i != averagedErrors.end(); ++i)
					{
						ocean_assert(*i <= Scalar(3.5 * 3.5));
					}
				}
#endif // OCEAN_DEBUG

				break;
			}
		}

		// upper stall frame
		if (upperFrame > validUpperFrame)
		{
			// we identify all unknown 3D object points which are visible in the highest frame with valid pose
			// we try to add as much object points as possible as long as their position is reliable and accurate

			const unsigned int correspondencesUpperFrame = database.numberCorrespondences<false, false, true>(validUpperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
			ocean_assert(correspondencesUpperFrame >= 10u);

			Log::info() << " ";
			Log::info() << "We have an upper stall frame at " << validUpperFrame << " (which is the highest valid frame) with " << correspondencesUpperFrame << " valid correspondences";

			unsigned int lookahead = 128u;

			while (lookahead != 0u)
			{
				ocean_assert(validLowerFrame != (unsigned int)(-1));
				const unsigned int lowerObservationFrame = max(int(validLowerFrame), int(validUpperFrame) - int(lookahead));
				const unsigned int upperObservationFrame = min(validUpperFrame + lookahead, upperFrame);

				if (!Solver3::determineUnknownObjectPoints<true>(database, camera, Solver3::CM_TRANSLATIONAL, lowerObservationFrame, upperObservationFrame, newUpperObjectPointCandidates, newUpperObjectPointCandidateIds, randomGenerator, &newUpperObjectPointCandidateObservations, Scalar(-1), 2u, true, Geometry::Estimator::ET_SQUARE, Scalar(3.5 * 3.5), Scalar(3.5 * 3.5), Numeric::maxValue(), WorkerPool::get().scopedWorker()(), abort)
						|| newUpperObjectPointCandidates.empty()
						|| (lookahead > 2u && newUpperObjectPointCandidates.size() < correspondencesUpperFrame * 2u)
						|| (lookahead > 1u && newUpperObjectPointCandidates.size() < correspondencesUpperFrame))
				{
					ocean_assert(lookahead / 2u != lookahead);
					lookahead /= 2u;
					continue;
				}

				Log::info() << "Found " << newUpperObjectPointCandidateIds.size() << " new object point candidates within the range [" << lowerObservationFrame << ", " << upperObservationFrame << "]";

#ifdef OCEAN_DEBUG
				{
					for (unsigned int n = lowerObservationFrame; n <= upperObservationFrame; ++n)
					{
						for (Indices32::const_iterator i = newUpperObjectPointCandidateIds.begin(); i != newUpperObjectPointCandidateIds.end(); ++i)
						{
							ocean_assert(database.hasObservation<false>(n, *i));
						}
					}

					Database copyDatabase(database);
					copyDatabase.setObjectPoints<false>(newUpperObjectPointCandidateIds.data(), newUpperObjectPointCandidates.data(), newUpperObjectPointCandidateIds.size());

					Scalars averagedErrors(newUpperObjectPointCandidateIds.size());
					if (!Solver3::determineProjectionErrors(copyDatabase, pinholeCamera, newUpperObjectPointCandidateIds, pinholeCamera.hasDistortionParameters(), lowerFrame, upperFrame, nullptr, averagedErrors.data(), nullptr, nullptr, WorkerPool::get().scopedWorker()()))
					{
						return false;
					}

					for (Scalars::const_iterator i = averagedErrors.begin(); i != averagedErrors.end(); ++i)
					{
						ocean_assert(*i <= Scalar(3.5 * 3.5));
					}
				}
#endif // OCEAN_DEBUG

				break;
			}
		}

		if (newLowerObjectPointCandidateIds.empty() && newUpperObjectPointCandidateIds.empty())
		{
			break;
		}

		// **TODO** there can be  identical lower and upper object points...

		if (!newLowerObjectPointCandidateIds.empty())
		{
			objectPoints.clear();
			objectPointIds.clear();
			extractObjectPointsWithMostObservations(newLowerObjectPointCandidateIds, newLowerObjectPointCandidates, newLowerObjectPointCandidateObservations, 150, objectPointIds, objectPoints);

			database.setObjectPoints<false>(objectPointIds.data(), objectPoints.data(), objectPointIds.size());
			Log::info() << "Added " << objectPointIds.size() << " new object points at the lower stall frame";
		}

		if (!newUpperObjectPointCandidateIds.empty())
		{
			objectPoints.clear();
			objectPointIds.clear();
			extractObjectPointsWithMostObservations(newUpperObjectPointCandidateIds, newUpperObjectPointCandidates, newUpperObjectPointCandidateObservations, 150, objectPointIds, objectPoints);

			database.setObjectPoints<false>(objectPointIds.data(), objectPoints.data(), objectPointIds.size());
			Log::info() << "Added " << objectPointIds.size() << " new object points at the upper stall frame";
		}

		const unsigned int lowerValidFrameCorrespondences = database.numberCorrespondences<false, false, true>(validLowerFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
		const unsigned int upperValidFrameCorrespondences = database.numberCorrespondences<false, false, true>(validUpperFrame, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		const unsigned int minimalCorrespondences = correspondenceThreshold.threshold(min(lowerValidFrameCorrespondences, upperValidFrameCorrespondences));

		// we apply a minimalValidCorrespondenceRatio of 0.90 to support scenes in which up to 10% of the point paths of 3D object points 'jump' to an invalid position e.g, due to areas in the frame with almost identical appearance (regular patterns)

		Indices32 allObjectPointIds = database.objectPointIds<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
		if (!optimizeObjectPointsAndPosesIndividuallyIteratively(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, allObjectPointIds, minimalCorrespondences, true, 4u, Geometry::Estimator::ET_SQUARE, Scalar(0.9), Scalar(3.5 * 3.5), nullptr, nullptr, abort))
		{
			return false;
		}

		if (!removeInaccurateObjectPoints(pinholeCamera, Solver3::CM_TRANSLATIONAL, database, randomGenerator, lowerFrame, upperFrame, minimalCorrespondences, Scalar(0.9), Scalar(3.5 * 3.5), Scalar(5.5 * 5.5), (unsigned int)(-1), &validLowerFrame, &validUpperFrame, abort))
		{
			return false;
		}

		// check whether we could not improve the tracking result (currently not based on the frame range but on the frame number which might not be the best idea
		if (validUpperFrame - validLowerFrame + 1u <= previousValidFrameNumber)
		{
			Log::info() << "We could not improve the pose range so that we have to stop here.";
			break;
		}

		Log::info() << "Now we have valid poses within the range: [" << validLowerFrame << ", " << validUpperFrame << "]";
		Log::info() << " ";

		previousValidFrameNumber = validUpperFrame - validLowerFrame + 1u;
	}

	return true;
}

void SLAMTracker::extractObjectPointsWithMostObservations(const Indices32& objectPointIds, const Vectors3& objectPoints, const Indices32& objectPointObservations, const size_t subsetSize, Indices32& bestObjectPointIds, Vectors3& bestObjectPoints)
{
	ocean_assert(objectPointIds.size() == objectPoints.size());
	ocean_assert(objectPointIds.size() == objectPointObservations.size());
	ocean_assert(subsetSize >= 1);

	ocean_assert(bestObjectPointIds.empty() && bestObjectPoints.empty());

	if (objectPointIds.size() <= subsetSize)
	{
		bestObjectPointIds = objectPointIds;
		bestObjectPoints = objectPoints;
	}
	else
	{
		Indices32 sortedObservations(objectPointObservations);
		std::sort(sortedObservations.rbegin(), sortedObservations.rend());

		ocean_assert(subsetSize < sortedObservations.size());
		const unsigned int minimalObservations = sortedObservations[subsetSize - 1];

		bestObjectPointIds.clear();
		bestObjectPoints.clear();

		bestObjectPointIds.reserve(subsetSize * 2);
		bestObjectPoints.reserve(subsetSize * 2);

		for (size_t n = 0; n < objectPointObservations.size(); ++n)
		{
			if (objectPointObservations[n] >= minimalObservations)
			{
				bestObjectPointIds.push_back(objectPointIds[n]);
				bestObjectPoints.push_back(objectPoints[n]);
			}
		}

		ocean_assert(bestObjectPointIds.size() >= subsetSize);
	}

	ocean_assert(bestObjectPointIds.size() == bestObjectPoints.size());
}

bool SLAMTracker::adjustPlaneTransformationToRegionOfInterest(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const CV::SubRegion& regionOfInterest, HomogenousMatrix4& planeTransformation)
{
	ocean_assert(pinholeCamera.isValid() && pose.isValid());
	ocean_assert(planeTransformation.isValid());
	ocean_assert(!regionOfInterest.isEmpty());

	HomogenousMatrix4 invPlaneTransformation(planeTransformation);
	if (!invPlaneTransformation.invert())
	{
		return false;
	}

	const Vector2 horizontalDirection(1, 0);

	const Triangles2 roiTriangles = regionOfInterest.triangles();

	Vector2 imagePoint0, imagePoint1;
	Vector2 centerImagePoint;

	if (roiTriangles.size() == 2)
	{
		const Vector2 rectangleCornersImageCoordinate[4] =
		{
			roiTriangles[0][0],
			roiTriangles[0][1],
			roiTriangles[0][2],
			roiTriangles[1][2]
		};

		const FiniteLine2 diagonal0(rectangleCornersImageCoordinate[0], rectangleCornersImageCoordinate[2]);
		const FiniteLine2 diagonal1(rectangleCornersImageCoordinate[1], rectangleCornersImageCoordinate[3]);

		if (!diagonal0.intersection(diagonal1, centerImagePoint))
		{
			return false;
		}

		Vector2 rectangleEdges[4] =
		{
			rectangleCornersImageCoordinate[1] - rectangleCornersImageCoordinate[0],
			rectangleCornersImageCoordinate[2] - rectangleCornersImageCoordinate[1],
			rectangleCornersImageCoordinate[3] - rectangleCornersImageCoordinate[2],
			rectangleCornersImageCoordinate[0] - rectangleCornersImageCoordinate[3]
		};

		unsigned int minIndex = 0u;
		Scalar minAngle = std::min(rectangleEdges[0].angle(horizontalDirection), rectangleEdges[0].angle(-horizontalDirection));

		for (unsigned int i = 1u; i < 4u; ++i)
		{
			const Scalar currentAngle = std::min(rectangleEdges[i].angle(horizontalDirection), rectangleEdges[i].angle(-horizontalDirection));
			if (currentAngle < minAngle)
			{
				minIndex = i;
				minAngle = currentAngle;
			}
		}

		imagePoint0 = rectangleCornersImageCoordinate[minIndex];
		imagePoint1 = rectangleCornersImageCoordinate[(minIndex + 1u) % 4u];

		if ((imagePoint1 - imagePoint0) * horizontalDirection < Numeric::eps())
		{
			std::swap(imagePoint0, imagePoint1);
		}
	}
	else
	{
		const Box2 roiBoundingBox = regionOfInterest.boundingBox();

		imagePoint0 = roiBoundingBox.lower();
		imagePoint1 = Vector2(roiBoundingBox.higher().x(), roiBoundingBox.lower().y());

		centerImagePoint = roiBoundingBox.center();
	}

	const Plane3 plane(planeTransformation);

	const Line3 rayPoint0 = pinholeCamera.ray(pinholeCamera.undistort<true>(imagePoint0), pose);
	const Line3 rayPoint1 = pinholeCamera.ray(pinholeCamera.undistort<true>(imagePoint1), pose);
	const Line3 rayCenterPoint = pinholeCamera.ray(pinholeCamera.undistort<true>(centerImagePoint), pose);

	Vector3 worldCoordinatePoint0, worldCoordinatePoint1, worldCoordinateCenterPoint;
	if (!plane.intersection(rayPoint0, worldCoordinatePoint0) || !plane.intersection(rayPoint1, worldCoordinatePoint1) || !plane.intersection(rayCenterPoint, worldCoordinateCenterPoint))
	{
		return false;
	}

	const Vector3 planeCoordinatePoint0 = invPlaneTransformation * worldCoordinatePoint0;
	const Vector3 planeCoordinatePoint1 = invPlaneTransformation * worldCoordinatePoint1;
	const Vector3 planeCoordinateCenterPoint = invPlaneTransformation * worldCoordinateCenterPoint;

	Vector3 planeHorizontalDirection = planeCoordinatePoint1 - planeCoordinatePoint0;
	if (!planeHorizontalDirection.normalize())
	{
		return false;
	}

	const Rotation rotation(Vector3(1, 0, 0), planeHorizontalDirection);

	ocean_assert((planeTransformation * HomogenousMatrix4(rotation) * HomogenousMatrix4(planeCoordinateCenterPoint)).zAxis() == planeTransformation.zAxis());
	planeTransformation = planeTransformation * HomogenousMatrix4(rotation) * HomogenousMatrix4(planeCoordinateCenterPoint);

	return true;
}

void SLAMTracker::maintenanceSendEnvironment()
{
	if (Maintenance::get().isActive())
	{
		Index32 poseId;
		if (!database_.poseWithMostCorrespondences<false, false, true>(lowerFrameIndex_, upperFrameIndex_, &poseId, nullptr, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())))
		{
			return;
		}

		const FrameRef frame = frameProviderInterface_->synchronFrameRequest(poseId, 10.0, &shouldStop_);

		Vectors2 frameImagePoints;
		Vectors3 frameObjectPoints;
		database_.imagePointsObjectPoints<false, false>(poseId, frameImagePoints, frameObjectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		const Vectors3 objectPoints = database_.objectPoints<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		const HomogenousMatrix4& pose = database_.pose<false>(poseId);

		Maintenance::get().send("ENVIRONMENT", Tracking::Utilities::encodeEnvironment(frame ? *frame : Frame(), frameImagePoints, frameObjectPoints, pose, objectPoints));
	}
}

}

}

}
