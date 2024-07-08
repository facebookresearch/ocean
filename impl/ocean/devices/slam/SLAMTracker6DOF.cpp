/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/slam/SLAMTracker6DOF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Median.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/String.h"
#include "ocean/base/Subset.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/StereoscopicGeometry.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/tracking/VisualTracker.h"
#include "ocean/tracking/Utilities.h"

#ifdef OCEAN_DEBUG_ON_WINDOWS
	#include "ocean/platform/win/Utilities.h"
#endif

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

SLAMTracker6DOF::SLAMTracker6DOF() :
	Device(deviceNameSLAMTracker6DOF(),  deviceTypeSLAMTracker6DOF()),
	SLAMDevice(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF()),
	Measurement(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF()),
	Tracker(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF()),
	OrientationTracker3DOF(deviceNameSLAMTracker6DOF()),
	PositionTracker3DOF(deviceNameSLAMTracker6DOF()),
	Tracker6DOF(deviceNameSLAMTracker6DOF()),
	VisualTracker(deviceNameSLAMTracker6DOF(), deviceTypeSLAMTracker6DOF())
{
	uniqueObjectId_ = addUniqueObjectId("SLAM World");
}

SLAMTracker6DOF::~SLAMTracker6DOF()
{
	stopThreadExplicitly();
}

bool SLAMTracker6DOF::isStarted() const
{
	const ScopedLock scopedLock(deviceLock);

	return isThreadActive();
}

bool SLAMTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
	{
		return false;
	}

	if (isThreadActive())
	{
		return true;
	}

	startThread();

	return true;
}

bool SLAMTracker6DOF::stop()
{
	const ScopedLock scopedLock(deviceLock);

	stopThread();
	return true;
}

bool SLAMTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	return previousPose_.isValid() && objectId == uniqueObjectId_;
}

void SLAMTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	Log::info() << deviceNameSLAMTracker6DOF() << " started...";

	RandomGenerator randomGenerator;

	try
	{
		while (shouldThreadStop() == false)
		{
			const FrameRef frame = frameMedium->frame();

			if (frame.isNull() || !frame->isValid() || frame->timestamp() <= frameTimestamp_)
			{
				sleep(1u);
				continue;
			}

			frameTimestamp_ = frame->timestamp();

			if (*frame != recentFrameType_)
			{
				recentFrameType_ = *frame;

				IO::CameraCalibrationManager::Quality quality;
				camera_ = IO::CameraCalibrationManager::get().camera(frameMedium->url(), frame->width(), frame->height(), &quality);

				if (quality == IO::CameraCalibrationManager::QUALITY_DEFAULT)
					Log::warning() << "No valid camera calibration has been found for \"" << frameMedium->url() << "\" a default calibration with 45 degree FOVX is used instead.";
				else if (quality == IO::CameraCalibrationManager::QUALITY_INTERPOLATED)
					Log::info() << "No exact camera calibration has been found for \"" << frameMedium->url() << "\" with resolution " << frame->width() << "x" << frame->height() << " an interpolated calibration is used instead.";
			}

			const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

			Frame yFrame;
			if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
			{
				continue;
			}

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
			const unsigned int framePyramidLayers = previousFramePyramid_ ? previousFramePyramid_.layers() : CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), 50u, 50u, 2u, 64u, 2u);
			const unsigned int binsSize = 80u;
#else
			const unsigned int framePyramidLayers = previousFramePyramid_ ? previousFramePyramid_.layers() : CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), 50u, 50u, 2u, 128u, 2u);
			const unsigned int binsSize = 50u;
#endif

			if (framePyramidLayers == 0u)
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			if (!currentFramePyramid_.replace8BitPerChannel11(yFrame, framePyramidLayers, true /*coypFirstLayer*/, scopedWorker()))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			if (objectPoints_.empty())
			{
				// we do not have valid 3D object points, so we have to determine their locations first

				if (initializationFirstImagePoints_.empty())
				{
					// we do not have any stereo image points for the initialization

					ocean_assert(initializationImagePointsDetermined_ == 0);

					if (initializationTimestamp_.isInvalid())
						initializationTimestamp_ = Timestamp(true) + 1.0;

					if (frameTimestamp_ >= initializationTimestamp_)
					{
						// now the time is right for the first stereo image

						Vectors2 imagePoints;
						determineFeaturePoints(yFrame, Vectors2(), imagePoints, binsSize, scopedWorker());

						if (imagePoints.size() >= 20)
						{
							initializationFirstImagePoints_ = std::move(imagePoints);
							initializationImagePointsDetermined_ = initializationFirstImagePoints_.size();

							initializationRecentImagePoints_ = initializationFirstImagePoints_;

							Log::info() << "Started with " << initializationRecentImagePoints_.size() << " initial image points";
						}
					}
				}
				else
				{
					// we have image points initially determined in a frame which now must be tracked from frame to frame so that we finally can determine 3D object point locations for them

					ocean_assert(initializationImagePointsDetermined_ != 0);
					ocean_assert(initializationRecentImagePoints_.size() >= 5);
					ocean_assert(initializationFirstImagePoints_.size() == initializationRecentImagePoints_.size());

					Vectors2 newImagePoints;
					Indices32 validIndices;
					trackPoints<7u>(previousFramePyramid_, currentFramePyramid_, initializationRecentImagePoints_, newImagePoints, validIndices, scopedWorker());

					if (validIndices.size() < 10)
					{
						initializationFirstImagePoints_.clear();
						initializationImagePointsDetermined_ = 0;
						initializationTimestamp_.toInvalid();
					}
					else
					{
						initializationFirstImagePoints_ = Subset::subset(initializationFirstImagePoints_, validIndices);
						initializationRecentImagePoints_ = Subset::subset(newImagePoints, validIndices);

						ocean_assert(initializationFirstImagePoints_.size() == initializationRecentImagePoints_.size());

						Log::info() << "Now we have " << initializationFirstImagePoints_.size() << " image points";

						// now we check whether we should start the determination of the initial pose

						bool determineInitialPose = initializationFirstImagePoints_.size() < initializationImagePointsDetermined_ * 50 / 100;

						if (!determineInitialPose)
						{
							Scalars positionOffsets(initializationFirstImagePoints_.size());

							for (size_t n = 0; n < initializationFirstImagePoints_.size(); ++n)
								positionOffsets[n] = initializationFirstImagePoints_[n].sqrDistance(initializationRecentImagePoints_[n]);

							const Scalar medianOffset = Numeric::sqrt(Median::median(positionOffsets.data(), positionOffsets.size()));

							Log::info() << "Median offset: " << medianOffset << " / " << Scalar(yFrame.width()) * Scalar(0.2);

							determineInitialPose = medianOffset >= Scalar(yFrame.width()) * Scalar(0.2);
						}

						if (determineInitialPose)
						{
							Log::info() << "Initial pose determination";

							HomogenousMatrix4 pose(false);
							Vectors3 objectPoints;
							validIndices.clear();

							if (determineInitialObjectPoints(camera_, initializationFirstImagePoints_, initializationRecentImagePoints_, pose, objectPoints, validIndices))
							{
								// we accept the initialization result only if we have enough valid object points, otherwise we result the initialization process

								if (objectPoints.size() >= 10)
								{
									imagePoints_ = Subset::subset(initializationRecentImagePoints_, validIndices);
									objectPoints_ = std::move(objectPoints);

									previousPose_ = pose;
									postPose(previousPose_, frameTimestamp_);
								}

								initializationFirstImagePoints_.clear();
								initializationRecentImagePoints_.clear();
								initializationImagePointsDetermined_ = 0;
							}
						}
					}
				}
			}
			else
			{
				// we have known 3D object point locations (for our previously determined image points) so that we now need to determine the camera pose for the current frame (by tracking the image points from the previous frame)

				static HighPerformanceStatistic performance;
				performance.start();

				static HighPerformanceStatistic performancePointTracking;
				performancePointTracking.start();

				Vectors2 newFeaturePointCandidates;

				if (imagePoints_.size() < 25)
				{
					// we try to detect new feature points in empty areas of the camera frame
					determineFeaturePoints(yFrame, combineImagePointGroups(imagePoints_, observationGroups_, Vectors2()), newFeaturePointCandidates, binsSize, scopedWorker());
				}

				const Vectors2 combinedPreviousImagePoints = combineImagePointGroups(imagePoints_, observationGroups_, newFeaturePointCandidates);

				Vectors2 combinedCurrentImagePoints;
				Indices32 validIndices;
				trackPoints<7u>(previousFramePyramid_, currentFramePyramid_, combinedPreviousImagePoints, combinedCurrentImagePoints, validIndices, scopedWorker());

				performancePointTracking.stop();

				const size_t numberLocatedPreviousImagePoints = imagePoints_.size();
				const Indices32 validTrackingIndices(extractLocatedImagePointIndices(numberLocatedPreviousImagePoints, validIndices)); // indices which can be used for tracking

				if (validTrackingIndices.size() < 5)
				{
					objectPoints_.clear();
					imagePoints_.clear();
					observationGroups_.clear();

					previousPose_.toNull();
					initializationTimestamp_.toInvalid();
				}
				else
				{
					objectPoints_ = Subset::subset(objectPoints_, validTrackingIndices);
					imagePoints_ = Subset::subset(combinedCurrentImagePoints, validTrackingIndices);

					HomogenousMatrix4 pose(false);

					if (Geometry::NonLinearOptimizationPose::optimizePose(camera_, previousPose_, ConstArrayAccessor<Vector3>(objectPoints_), ConstArrayAccessor<Vector2>(imagePoints_), camera_.hasDistortionParameters(), pose, 20u, Geometry::Estimator::ET_HUBER))
					{
						// first we post the tracking result so that the connected components have this information as early as possible

						previousPose_ = pose;
						postPose(previousPose_, frameTimestamp_);

						// now we extend our database for new feature points
						extractUnlocatedImagePoints(combinedCurrentImagePoints, numberLocatedPreviousImagePoints, validIndices, pose, observationGroups_);

						extendTrackingDatabase(camera_, observationGroups_, objectPoints_, imagePoints_, 20u);

#ifdef OCEAN_DEBUG_ON_WINDOWS
						{
							Frame rgbFrame;
							CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, scopedWorker());

							const uint8_t lineColor[3] = {0xAA, 0xAA, 0xAA};

							const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));
							const HomogenousMatrix4 planeTransformation = HomogenousMatrix4(Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()));

							Tracking::Utilities::paintPlaneIF(rgbFrame, poseIF, trackerCamera, planeTransformation, Scalar(1.0), 10u, trackerCamera.hasDistortionParameters(), lineColor, nullptr);

							const uint8_t black[3] = {0x00, 0x00, 0x00};
							const uint8_t white[3] = {0xFF, 0xFF, 0xFF};
							const uint8_t red[3] = {0xFF, 0x00, 0x00};
							const uint8_t blue[3] = {0x00, 0x00, 0xFF};

							Tracking::Utilities::paintCorrespondences<7u, 3u>(rgbFrame, AnyCameraPinhole(trackerCamera), pose, trackerObjectPoints.data(), trackerImagePoints.data(), trackerObjectPoints.size(), Scalar(3 * 3), black, white, red, blue, true, true, true, scopedWorker());

							Platform::Win::Utilities::desktopFrameOutput(0, 0, rgbFrame);
						}
#endif // OCEAN_DEBUG_ON_WINDOWS
					}
					else
					{
						objectPoints_.clear();
						imagePoints_.clear();
						observationGroups_.clear();

						previousPose_.toNull();
						initializationTimestamp_.toInvalid();
					}
				}

				performance.stop();

				if (performancePointTracking.measurements() % 50u == 0u)
					Log::info() << "Point Tracking: " << performancePointTracking.averageMseconds();

				if (performance.measurements() % 50u == 0u)
					Log::info() << "Tracker performance: " << performance.averageMseconds();
			}

			std::swap(currentFramePyramid_, previousFramePyramid_);

			if (!previousPose_.isValid())
			{
				postLostTrackerObjects({uniqueObjectId_}, frameTimestamp_);
			}
		}
	}
	catch(const Exception& exception)
	{
		Log::error() << "Exception during SLAM feature tracker: " << exception.what();
	}
	catch(...)
	{
		Log::error() << "Unknown exception during SLAM feature tracker!";
	}

	postLostTrackerObjects({uniqueObjectId_}, Timestamp(true));

	Log::info() << deviceNameSLAMTracker6DOF() << " stopped...";
}

void SLAMTracker6DOF::postPose(const HomogenousMatrix4& pose, const Timestamp& timestamp)
{
	const ObjectIds objectIds(1, uniqueObjectId_);
	const Tracker6DOFSample::Positions positions(1, pose.translation());
	const Tracker6DOFSample::Orientations orientations(1, pose.rotation());

	postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, objectIds, orientations, positions)));
}

void SLAMTracker6DOF::determineFeaturePoints(const Frame& frame, const Vectors2& alreadyKnownFeaturePoints, Vectors2& newFeaturePoints, const unsigned int binSize, Worker* worker)
{
	ocean_assert(frame && frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(binSize != 0u);

	const unsigned int horizontalBins = frame.width() / binSize;
	const unsigned int verticalBins = frame.height() / binSize;

	const Vectors2 newPointCandidates = CV::Detector::FeatureDetector::determineHarrisPoints(frame, CV::SubRegion(), horizontalBins, verticalBins, 10u, worker);

	Geometry::SpatialDistribution::OccupancyArray occupancyArray(Geometry::SpatialDistribution::createOccupancyArray(alreadyKnownFeaturePoints.data(), alreadyKnownFeaturePoints.size(), Scalar(0), Scalar(0), Scalar(frame.width()), Scalar(frame.height()), horizontalBins, verticalBins));

	ocean_assert(newFeaturePoints.empty());
	newFeaturePoints.reserve(newPointCandidates.size());

	for (Vectors2::const_iterator i = newPointCandidates.begin(); i != newPointCandidates.end(); ++i)
	{
		if (occupancyArray.addPoint(*i))
		{
			newFeaturePoints.push_back(*i);
		}
	}
}

template <unsigned int tSize>
bool SLAMTracker6DOF::trackPoints(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const Vectors2& previousImagePoints, Vectors2& currentImagePoints, Indices32& validIndices, Worker* worker)
{
	Vectors2 previousPointsCopy(previousImagePoints);

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
	const Scalar maximalSqrError = Scalar(1.5 * 1.5);
	const unsigned int subPixelIterations = 1u;
#else
	const Scalar maximalSqrError = Scalar(0.9 * 0.9);
	const unsigned int subPixelIterations = 4u;
#endif

	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<tSize>(previousFramePyramid, currentFramePyramid, 2u, previousPointsCopy, currentImagePoints, maximalSqrError, worker, &validIndices, subPixelIterations))
		return false;

	return true;
}

bool SLAMTracker6DOF::determineInitialObjectPoints(const PinholeCamera& pinholeCamera, const Vectors2& firstImagePoints, const Vectors2& secondImagePoints, HomogenousMatrix4& pose, Vectors3& objectPoints, Indices32& validImagePoints)
{
	ocean_assert(firstImagePoints.size() == secondImagePoints.size());
	ocean_assert(firstImagePoints.size() >= 5);

	ocean_assert(objectPoints.empty() && validImagePoints.empty());

	RandomGenerator randomGenerator;

	HomogenousMatrix4 world_T_camera;
	if (!Geometry::StereoscopicGeometry::cameraPose(pinholeCamera, ConstArrayAccessor<Vector2>(firstImagePoints), ConstArrayAccessor<Vector2>(secondImagePoints), randomGenerator, world_T_camera, &objectPoints, &validImagePoints))
		return false;

	if (world_T_camera.translation().isNull())
	{
		Log::info() << "We have a pure rotation so far, so we try it again later";
		return false;
	}

	if (objectPoints.size() < 5)
		return false;

	// we determine the most prominent 3D plane from the determined 3D object point locations

	Plane3 plane;
	if (!Geometry::RANSAC::plane(ConstArrayAccessor<Vector3>(objectPoints), randomGenerator, plane))
		return false;

	// now we need to determine the reference coordinate system lying in/on the 3D plane

	const Line3 rayPrincipalPoint(pinholeCamera.ray(Vector2(Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height())) * Scalar(0.5), world_T_camera));

	Vector3 planePrincipalObjectPoint;
	if (!plane.intersection(rayPrincipalPoint, planePrincipalObjectPoint) || !pinholeCamera.isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), planePrincipalObjectPoint))
		return false;

	const Line3 rayRightPoint(pinholeCamera.ray(Vector2(Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()) * Scalar(0.5)), world_T_camera));

	Vector3 planeRightObjectPoint;
	if (!plane.intersection(rayRightPoint, planeRightObjectPoint) || !pinholeCamera.isObjectPointInFrontIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), planeRightObjectPoint))
		return false;

	Vector3 xAxis = planeRightObjectPoint - planePrincipalObjectPoint;
	if (!xAxis.normalize())
		return false;

	Vector3 yAxis = plane.normal();
	if (yAxis * (world_T_camera.translation() - planePrincipalObjectPoint) < 0)
		yAxis = -yAxis;

	const Vector3 zAxis = xAxis.cross(yAxis);
	ocean_assert(Numeric::isEqual(zAxis.length(), 1));

	const HomogenousMatrix4 worldTplane(xAxis, yAxis, zAxis, planePrincipalObjectPoint);
	ocean_assert(worldTplane.rotationMatrix().isOrthonormal(Numeric::weakEps()));
	const HomogenousMatrix4 planeTworld(worldTplane.inverted());

	for (size_t n = 0; n < objectPoints.size(); ++n)
		objectPoints[n] = planeTworld * objectPoints[n];

	pose = planeTworld * world_T_camera;

	return true;
}

Vectors2 SLAMTracker6DOF::combineImagePointGroups(const Vectors2& locatedPreviousImagePoints, const ObservationGroups& unlocatedObservationGroups, const Vectors2& newObservations)
{
	Vectors2 result(locatedPreviousImagePoints);
	result.reserve(result.size() + unlocatedObservationGroups.size() + newObservations.size());

	for (ObservationGroups::const_iterator i = unlocatedObservationGroups.begin(); i != unlocatedObservationGroups.end(); ++i)
	{
		ocean_assert(!i->empty());
		result.push_back(i->back().second);
	}

	result.insert(result.end(), newObservations.begin(), newObservations.end());

	return result;
}

Indices32 SLAMTracker6DOF::extractLocatedImagePointIndices(const size_t numberLocatedPreviousImagePoints, const Indices32& validIndices)
{
	Indices32 result;
	result.reserve(numberLocatedPreviousImagePoints);

	for (Indices32::const_iterator i = validIndices.cbegin(); i != validIndices.cend(); ++i)
	{
		if (*i >= (unsigned int)numberLocatedPreviousImagePoints)
			break;

		result.push_back(*i);
	}

	return result;
}

Vectors2 SLAMTracker6DOF::extractLocatedImagePoints(const Vectors2& combinedImagePoints, const size_t numberLocatedPreviousImagePoints, const Indices32& validIndices)
{
	return Subset::subset(combinedImagePoints, extractLocatedImagePointIndices(numberLocatedPreviousImagePoints, validIndices));
}

void SLAMTracker6DOF::extractUnlocatedImagePoints(const Vectors2& combinedImagePoints, const size_t numberLocatedPreviousImagePoints, const Indices32& validIndices, const HomogenousMatrix4& pose, ObservationGroups& observationGroups)
{
	ocean_assert(pose.isValid());

	// first we calculate the index of valid image points which will produce a new observation group
	const unsigned int newObservationGroupIndex = (unsigned int)(observationGroups.size() + numberLocatedPreviousImagePoints);

	ObservationGroups tempObservationGroups;
	tempObservationGroups.reserve(observationGroups.size() * 50 / 100);

	for (Indices32::const_iterator i = validIndices.cbegin(); i != validIndices.cend(); ++i)
		if (*i >= (unsigned int)numberLocatedPreviousImagePoints)
		{
			// now all indices belong to our observation groups or even will produce a new observation group

			const Vector2& imagePoint = combinedImagePoints[*i];

			if (*i < newObservationGroupIndex)
			{
				ocean_assert(*i - numberLocatedPreviousImagePoints < observationGroups.size());

				tempObservationGroups.push_back(std::move(observationGroups[*i - numberLocatedPreviousImagePoints]));
				tempObservationGroups.back().push_back(Observation(pose, imagePoint));
			}
			else
			{
				tempObservationGroups.push_back(Observations(1, Observation(pose, imagePoint)));
			}
		}

	observationGroups = std::move(tempObservationGroups);
}

void SLAMTracker6DOF::extendTrackingDatabase(const PinholeCamera& pinholeCamera, ObservationGroups& observationGroups, Vectors3& objectPoints, Vectors2& imagePoints, const unsigned int minimalObservations)
{
	ObservationGroups tempObservationGroups;
	tempObservationGroups.reserve(observationGroups.size());

	HomogenousMatrices4 observationPoses;
	Vectors2 observationImagePoints;
	Indices32 usedIndices;

	RandomGenerator localRandomGenerator;

	for (size_t n = 0; n < observationGroups.size(); ++n)
	{
		if (observationGroups[n].size() >= minimalObservations)
		{
			const Observations& observations = observationGroups[n];

			// we first check whether the object point has been seen from several different viewing angles

			const Scalar angle = medianObservationAngle(pinholeCamera, observations);

			if (angle >= Numeric::deg2rad(3))
			{
				observationPoses.clear();
				observationImagePoints.clear();
				usedIndices.clear();

				observationPoses.reserve(observations.size());
				observationImagePoints.reserve(observations.size());

				for (size_t i = 0; i < observations.size(); ++i)
				{
					observationPoses.push_back(observations[i].first);
					observationImagePoints.push_back(observations[i].second);
				}

				Vector3 objectPoint;
				if (Geometry::RANSAC::objectPoint(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<HomogenousMatrix4>(observationPoses), ConstArrayAccessor<Vector2>(observationImagePoints), localRandomGenerator, objectPoint, 20u, Scalar(3 * 3), 5u, true, Geometry::Estimator::ET_SQUARE, nullptr, &usedIndices) && usedIndices.size() == observationPoses.size())
				{
					// we have a new 3D object point

					objectPoints.push_back(objectPoint);
					imagePoints.push_back(observationImagePoints.back());
				}
				else
				{
					// we failed to determine a valid 3D object point location, so we will not use the observation group anymore
				}
			}
			else
			{
				tempObservationGroups.push_back(std::move(observationGroups[n]));
			}
		}
		else
		{
			tempObservationGroups.push_back(std::move(observationGroups[n]));
		}
	}

	observationGroups = std::move(tempObservationGroups);
}

Scalar SLAMTracker6DOF::medianObservationAngle(const PinholeCamera& pinholeCamera, const Observations& observations)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(!observations.empty());

	Lines3 rays(observations.size());
	Vector3 meanDirection(0, 0, 0);

	for (size_t n = 0; n < observations.size(); ++n)
	{
		rays[n] = pinholeCamera.ray(pinholeCamera.undistortDamped(observations[n].second), observations[n].first);
		meanDirection += rays[n].direction();
	}

	meanDirection = meanDirection.normalizedOrZero();

	Scalars angles(observations.size());

	for (size_t n = 0; n < observations.size(); ++n)
		angles[n] = meanDirection * rays[n].direction();

	return Numeric::acos(Median::median(angles.data(), angles.size()));
}

}

}

}
