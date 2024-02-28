// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/plane/PlaneTracker.h"

#include <vplib/line_detection/ULFLineSegmentDetector.h>
#include <vplib/plane_normal_detection/VerticalPlaneNormalDetectorRelaxed.h>
#include <vplib/vp_fitting/VerticalHorizontalVpFitterEM.h>
#include <vplib/vp_initialization/HorizontalVpBinningInitializer.h>

using namespace facebook::coreai;

namespace Ocean
{

namespace Tracking
{

namespace Plane
{

PlaneTracker::PlaneTracker() :
	planeDetectionInterval_(0.15),
	previousProcessFrameTimestamp_(false),
	maximalIntervalForPredictedPose_(0.5),
	previousCameraOrientation_(false),
	planeIdCounter_(0u),
	maximalAngleForForcingGravityVector_(Numeric::deg2rad(30)),
	maximalAngleForUsingGravityVectorAsBackup_(Numeric::deg2rad(45)),
	maximalAngleForUsingGravityVector_(Numeric::deg2rad(80))
{
	ocean_assert(maximalAngleForForcingGravityVector_ >= 0);
	ocean_assert(maximalAngleForForcingGravityVector_ < maximalAngleForUsingGravityVectorAsBackup_);
	ocean_assert(maximalAngleForUsingGravityVectorAsBackup_ < Numeric::pi_2());

#ifdef OCEAN_DEBUG
	const double maxTimeToLastProcessedSeconds = 10.0;
#else
	const double maxTimeToLastProcessedSeconds = planeDetectionInterval_ * 3.0;
#endif

	liveVerticalPlaneFinderWithIMU_ = createVerticalPlaneFinder(maxTimeToLastProcessedSeconds);

	startThread();
}

PlaneTracker::~PlaneTracker()
{
	stopThread();

	const Timestamp startTimestamp(true);
	while (isThreadActive() && startTimestamp + 2 > Timestamp(true))
	{
		sleep(1u);
	}

	ocean_assert(!isThreadActive());
}

PlaneTracker::PlaneIds PlaneTracker::addPlanes(const PinholeCamera& pinholeCamera, const Frame& yFrame, const Vectors2& planeLocations, const Quaternion& cameraOrientation, const PlaneProperties& planeProperties, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid() && yFrame.isValid());
	ocean_assert(pinholeCamera.width() == yFrame.width() && pinholeCamera.height() == yFrame.height());
	ocean_assert(yFrame.timestamp().isValid());
	return addPlanes(pinholeCamera, yFrame.timestamp(), planeLocations, cameraOrientation, planeProperties, worker);
}

PlaneTracker::PlaneIds PlaneTracker::addPlanes(const PinholeCamera& pinholeCamera, const Timestamp& timestamp, const Vectors2& planeLocations, const Quaternion& cameraOrientation, const PlaneProperties& planeProperties, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(!planeLocations.empty());
	ocean_assert(planeLocations.size() == planeProperties.size());

	const ScopedLock scopedLock(trackerLock_);

	ocean_assert(liveVerticalPlaneFinderWithIMU_);
	if (!liveVerticalPlaneFinderWithIMU_)
	{
		return PlaneIds(planeLocations.size(), invalidPlaneId());
	}

	ocean_assert(cameraOrientation.isValid() && "Missing implementation!");

	const Scalar cosAngleWithGravityVector = Vector3(0, -1, 0) * (cameraOrientation * Vector3(0, 0, -1));

	const Scalar cosForForcingGravityVector = Numeric::cos(maximalAngleForForcingGravityVector_);
	const Scalar cosForUsingGravityVectorAsBackup = Numeric::cos(maximalAngleForUsingGravityVectorAsBackup_);
	const Scalar cosForUsingGravityVector = Numeric::cos(maximalAngleForUsingGravityVector_);

	// the VP normal detector is using the inverted and flipped coordinate system
	const Quaternion cameraOrientationIF = PinholeCamera::standard2InvertedFlipped(cameraOrientation);

	PlaneIds newPlaneIds;
	newPlaneIds.reserve(planeLocations.size());

	for (size_t n = 0; n < planeLocations.size(); ++n)
	{
		const Vector2& planeLocation = planeLocations[n];
		const PlaneProperty desiredPlaneProperty = n < planeProperties.size() ? planeProperties[n] : PP_HORIZONTAL_OR_VERTICAL;

		PlaneId newPlaneId = invalidPlaneId();
		PlaneProperty newPlaneProperty = PP_INVALID;

		Vector3 planeNormal(0, 0, 0);

		if ((desiredPlaneProperty & PP_VERTICAL) != 0 && Numeric::abs(cosAngleWithGravityVector) < cosForForcingGravityVector) // angleWithGravityVector > angleForForcingGravityVector
		{
			// the angle between viewing ray and gravity vector is larger than the threshold for forcing using the gravity vector as normal
			// therefore, we try to get VP-based normal

			Vector3 verticalPlaneNormalIF;
			if (liveVerticalPlaneFinderWithIMU_->findVerticalPlaneNormalAtPoint(planeLocation, pinholeCamera, cameraOrientationIF, double(timestamp), verticalPlaneNormalIF))
			{
				// we need the normal in the (standard) world coordinate system (not in the inverted flipped camera coordinate system)
				planeNormal = cameraOrientationIF.inverted() * verticalPlaneNormalIF; // TODO ? flip

				newPlaneProperty = PP_VERTICAL;
			}
		}

		if (planeNormal.isNull() && (desiredPlaneProperty & PP_HORIZONTAL) != PP_INVALID)
		{
			// some kind of horizontal plane is desired

			if (desiredPlaneProperty == PP_HORIZONTAL_FLOOR)
			{
				// we force a floor plane, with a very generous gravity angle

				if (cosAngleWithGravityVector >= cosForUsingGravityVector) // angleWithGravityVector <= angleForUsingGravityVector
				{
					newPlaneProperty = PP_HORIZONTAL_FLOOR;
				}
			}
			else if (desiredPlaneProperty == PP_HORIZONTAL_CEILING)
			{
				// we force a ceiling plane, with a very generous gravity angle

				if (-cosAngleWithGravityVector >= cosForUsingGravityVector) // -angleWithGravityVector <= angleForUsingGravityVector
				{
					newPlaneProperty = PP_HORIZONTAL_CEILING;
				}
			}
			else if (desiredPlaneProperty == PP_HORIZONTAL)
			{
				// we force either a floor plane or a ceiling plane, with a very generous gravity angle
				// the sign of the cos-value will determine whether we have a floor or a ceiling plane

				if (Numeric::abs(cosAngleWithGravityVector) >= cosForUsingGravityVector) // angleWithGravityVector <= angleForUsingGravityVector
				{
					newPlaneProperty = cosAngleWithGravityVector >= 0 ? PP_HORIZONTAL_FLOOR : PP_HORIZONTAL_CEILING;
				}
			}
			else if (Numeric::abs(cosAngleWithGravityVector) >= cosForUsingGravityVectorAsBackup) // angleWithGravityVector <= angleForUsingGravityVectorAsBackup
			{
				// the user accepts a horizontal plane (and the gravity angle was within the threshold)

				newPlaneProperty = cosAngleWithGravityVector >= 0 ? PP_HORIZONTAL_FLOOR : PP_HORIZONTAL_CEILING;
			}

			ocean_assert(newPlaneProperty == PP_INVALID || newPlaneProperty == PP_HORIZONTAL_FLOOR || newPlaneProperty == PP_HORIZONTAL_CEILING);

			if (newPlaneProperty == PP_HORIZONTAL_FLOOR)
			{
				// we define a floor/ground plane (with normal equal to the y-axis)
				planeNormal = Vector3(0, 1, 0);
			}
			else if (newPlaneProperty == PP_HORIZONTAL_CEILING)
			{
				// we define a ceiling plane (with normal equal to the negative y-axis)
				planeNormal = Vector3(0, -1, 0);
			}
		}

		if (!planeNormal.isNull())
		{
			const unsigned int minDimension = std::min(pinholeCamera.width(), pinholeCamera.height());
			const Scalar regionSize = std::max(Scalar(10), Scalar(minDimension) * Scalar(0.45));

			const Box2 region(planeLocation, regionSize, regionSize);

			// we create a new homography tracker for each new plane

			std::shared_ptr<Point::HomographyTracker> homographyTracker = std::make_shared<Point::HomographyTracker>();
			ocean_assert(homographyTracker);

			HomogenousMatrix4 pose;
			HomogenousMatrix4 planeTransformation;

			if (homographyTracker && homographyTracker->resetRegion(pinholeCamera, region, cameraOrientation, planeNormal, &pose, &planeTransformation))
			{
				// we create a new unique id for the plane
				newPlaneId = ++planeIdCounter_;

				ocean_assert(newPlaneProperty != PP_INVALID);

				ocean_assert(planeMap_.find(newPlaneId) == planeMap_.cend());
				planeMap_.insert(std::make_pair(newPlaneId, Plane(pose, planeTransformation, newPlaneProperty)));

				ocean_assert(homographyTrackerMap_.find(newPlaneId) == homographyTrackerMap_.cend());
				homographyTrackerMap_.insert(std::make_pair(newPlaneId, std::move(homographyTracker)));
			}
		}

		newPlaneIds.push_back(newPlaneId);
	}

	ocean_assert(planeLocations.size() == newPlaneIds.size());
	return newPlaneIds;
}

bool PlaneTracker::movePlane(const PlaneId planeId, const PinholeCamera& pinholeCamera, const Vector2& newPlaneLocation)
{
	ocean_assert(planeId != invalidPlaneId());
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(pinholeCamera.isInside(newPlaneLocation));

	if (!pinholeCamera.isValid() || !pinholeCamera.isInside(newPlaneLocation))
	{
		return false;
	}

	const ScopedLock scopedLock(trackerLock_);

	PlaneMap::iterator iPlane = planeMap_.find(planeId);
	ocean_assert(iPlane != planeMap_.end());

	Plane& plane = iPlane->second;

	if (!plane.pose().isValid() || !plane.object().isValid())
	{
		return false;
	}

	const Line3 ray = pinholeCamera.ray(newPlaneLocation, plane.pose());
	const Plane3 infinitePlane(plane.object().translation(), plane.object().zAxis());

	Vector3 pointOnPlane;
	if (infinitePlane.intersection(ray, pointOnPlane))
	{
		plane.object_.setTranslation(pointOnPlane);

		return true;
	}

	return false;
}

bool PlaneTracker::removePlane(const PlaneId planeId)
{
	ocean_assert(planeId != invalidPlaneId());

	const ScopedLock scopedLock(trackerLock_);

	PlaneMap::iterator i = planeMap_.find(planeId);

	if (i == planeMap_.cend())
	{
		ocean_assert(homographyTrackerMap_.find(planeId) == homographyTrackerMap_.cend());
		return false;
	}

	planeMap_.erase(i);

	ocean_assert(homographyTrackerMap_.find(planeId) != homographyTrackerMap_.cend());
	homographyTrackerMap_.erase(planeId);

	return true;
}

void PlaneTracker::removePlanes()
{
	const ScopedLock scopedLock(trackerLock_);

	planeMap_.clear();
	homographyTrackerMap_.clear();
}

bool PlaneTracker::trackPlanes(const PinholeCamera& pinholeCamera, const Frame& yFrame, const Quaternion& cameraOrientation, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid() && yFrame.isValid());
	ocean_assert(pinholeCamera.width() == yFrame.width() && pinholeCamera.height() == yFrame.height());
	ocean_assert(FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(yFrame.timestamp().isValid());

	if (yFrame.isNull() || !yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	const ScopedLock scopedLock(trackerLock_);

	ocean_assert(liveVerticalPlaneFinderWithIMU_);
	if (!liveVerticalPlaneFinderWithIMU_)
	{
		return false;
	}

	// the VP normal detector is using the inverted and flipped coordinate system
	const Quaternion cameraOrientationIF = PinholeCamera::standard2InvertedFlipped(cameraOrientation);

	if (previousProcessFrameTimestamp_.isInvalid() || yFrame.timestamp() > previousProcessFrameTimestamp_ + planeDetectionInterval_)
	{
		const ScopedLock lock(asynchronousDataLock_);

		if (!planeFinderAsynchronousData_)
		{
			planeFinderAsynchronousData_ = std::make_unique<PlaneFinderAsynchronousData>(Frame(yFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT), pinholeCamera, cameraOrientationIF);

			previousProcessFrameTimestamp_ = yFrame.timestamp();
		}
	}

	Scalar cameraMotionAngle = 0;

	if (cameraOrientation.isValid() && previousCameraOrientation_.isValid())
	{
		// (wTp)^-1 * wTc = pTc
		cameraMotionAngle = previousCameraOrientation_.smallestAngle(cameraOrientation);
	}

	for (HomographyTrackerMap::iterator iHomographyTracker = homographyTrackerMap_.begin(); iHomographyTracker != homographyTrackerMap_.end(); ++iHomographyTracker)
	{
		const PlaneId planeId = iHomographyTracker->first;
		std::shared_ptr<Point::HomographyTracker>& homographyTracker = iHomographyTracker->second;
		ocean_assert(homographyTracker);

		PlaneMap::iterator iPlane = planeMap_.find(planeId);
		ocean_assert(iPlane != planeMap_.cend());

		Plane& plane = iPlane->second;

		SquareMatrix3 homography(false);
		HomogenousMatrix4 pose(false);

		if (homographyTracker->determineHomography(pinholeCamera, yFrame, homography, &pose, cameraOrientation, worker) && pose.isValid())
		{
			plane = Plane(pose, plane.object(), plane.planeProperty(), cameraOrientation, yFrame.timestamp());
		}
		else
		{
			if (cameraMotionAngle > Numeric::deg2rad(1.5) && plane.lastAccuratePoseTimestamp_.isValid() && double(yFrame.timestamp() - plane.lastAccuratePoseTimestamp_) <= maximalIntervalForPredictedPose_)
			{
				// lTc = wTl^1 * wTc

				const Quaternion predictedOrientation(plane.lastAccuratePose_.rotation() * plane.lastAccuratePoseCameraOrientation_.inverted() * cameraOrientation);
				const HomogenousMatrix4 predictedPose(plane.lastAccuratePose_.translation(), predictedOrientation);

				plane.pose_ = predictedPose;
			}
			else
			{
				plane.pose_.toNull();
			}
		}
	}

	previousCameraOrientation_ = cameraOrientation;

	return true;
}

void PlaneTracker::threadRun()
{
	while (!shouldThreadStop())
	{
		TemporaryScopedLock lock(asynchronousDataLock_);

		if (planeFinderAsynchronousData_)
		{
			lock.release();

			// now we can use the asynchronous data, as nobody will touch it until we free it

			const Frame& yFrame = planeFinderAsynchronousData_->yFrame();
			ocean_assert(yFrame);

			const PinholeCamera& pinholeCamera = planeFinderAsynchronousData_->camera();
			ocean_assert(pinholeCamera);

			const Quaternion& cameraOrientationIF = planeFinderAsynchronousData_->cameraOrientationIF();
			ocean_assert(cameraOrientationIF.isValid());

			liveVerticalPlaneFinderWithIMU_->processFrame(yFrame, pinholeCamera, cameraOrientationIF, double(yFrame.timestamp()));

			const ScopedLock finalLock(asynchronousDataLock_);

			planeFinderAsynchronousData_.reset(nullptr);
		}
		else
		{
			lock.release();

			Thread::sleep(1u);
		}
	}
}

std::unique_ptr<vplib::LiveVerticalPlaneFinderWithIMU> PlaneTracker::createVerticalPlaneFinder(const double maxTimeToLastProcessedSeconds)
{
	// creating the line segment detector
	vplib::ULFLineSegmentDetector::Params ulfParams;
	ulfParams.minLength = 40;
	ulfParams.numPyramidLevels = 3u;
#ifndef _ANDROID
	ulfParams.collinearityParams.enabled = true;
	ulfParams.collinearityParams.maxLineGap = Scalar(15.0);
	ulfParams.collinearityParams.cosAngle =	Numeric::cos(Numeric::deg2rad(Scalar(1.0)));
#endif
	vplib::ULFLineSegmentDetectorPtr ulfLineSegmentDetector = std::make_unique<vplib::ULFLineSegmentDetector>(ulfParams);

	// creating the binning initializer
	vplib::HorizontalVpBinningInitializer::Params binningParams;
	vplib::HorizontalVpBinningInitializerPtr vpInitializer = std::make_unique<vplib::HorizontalVpBinningInitializer>(binningParams);

	// creating the horizontal vanishing point fitter
	vplib::VerticalHorizontalVpFitterEM::Params fittingParams;
	vplib::VerticalHorizontalVpFitterEMPtr vpFitter = std::make_unique<vplib::VerticalHorizontalVpFitterEM>(fittingParams);

	// creating the normal detector
	vplib::VerticalPlaneNormalDetectorRelaxed::Params planeDetectionParams;
	vplib::VerticalPlaneNormalDetectorRelaxedPtr planeNormalDetector = std::make_unique<vplib::VerticalPlaneNormalDetectorRelaxed>(planeDetectionParams);

	// finally, we can create the vertical plane finder

	vplib::LiveVerticalPlaneFinderWithIMU::Params planeFinderParams;
	planeFinderParams.maxTimeToLastProcessedSeconds = maxTimeToLastProcessedSeconds;
	return std::make_unique<vplib::LiveVerticalPlaneFinderWithIMU>(planeFinderParams, std::move(ulfLineSegmentDetector), std::move(vpInitializer), std::move(vpFitter), std::move(planeNormalDetector));
}

}

}

}
