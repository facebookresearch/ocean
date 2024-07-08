/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/pattern/PatternTrackerCore6DOF.h"

#include "ocean/base/Subset.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/advanced/AdvancedMotion.h"
#include "ocean/cv/advanced/FrameRectification.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/SuccessionSubset.h"

#include "ocean/media/Utilities.h"

#include "ocean/tracking/UnidirectionalCorrespondences.h"

namespace Ocean
{

namespace Tracking
{

namespace Pattern
{

PatternTrackerCore6DOF::Options::Options()
{
	// nothing to do here
}

PatternTrackerCore6DOF::FeatureMap::FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker)
{
	const Frame patternFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_USE_KEEP_LAYOUT, yFramePaddingElements);

	const SharedAnyCamera camera = std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, Numeric::deg2rad(60)));

	Vectors2 imagePoints;
	if (!detectAndDescribeFeatures(camera, patternFrame, imagePoints, descriptors_, 20u, worker))
	{
		descriptors_.clear();
		objectPoints_.clear();

		return;
	}

	ocean_assert(imagePoints.size() == descriptors_.size());

	objectPoints_.reserve(imagePoints.size());

	ocean_assert(width >= 1u && height >= 1u);
	const Scalar factorX = dimension.x() / Scalar(width);
	const Scalar factorY = dimension.y() > 0 ? dimension.y() / Scalar(height) : factorX;

	for (const Vector2& imagePoint : imagePoints)
	{
		objectPoints_.emplace_back(imagePoint.x() * factorX, Scalar(0), imagePoint.y() * factorY);
	}
}

PatternTrackerCore6DOF::Pattern::Pattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker) :
	featureMap_(yFrame, width, height, yFramePaddingElements, Vector2(dimension.x(), dimension.y()), worker),
	patternPyramid_(yFrame, width, height, 1u, FrameType::ORIGIN_UPPER_LEFT, CV::FramePyramid::idealLayers(width, height, 15u, 15u), yFramePaddingElements, true /*copyFirstLayer*/, worker),
	dimension_(dimension),
	world_T_previousCamera_(false),
	world_T_guessCamera_(false)
{
	ocean_assert(dimension_.x() > 0 && dimension_.y() > 0);

	pyramidReferencePoints_.resize(patternPyramid_.layers());

	for (unsigned int n = 0; n < patternPyramid_.layers(); ++n)
	{
		pyramidReferencePoints_[n] = CV::Detector::FeatureDetector::determineHarrisPoints(patternPyramid_[n].constdata<uint8_t>(), patternPyramid_[n].width(), patternPyramid_[n].height(), patternPyramid_[n].paddingElements(), CV::SubRegion(), 0u, 0u, 25u, worker);
	}
}

void PatternTrackerCore6DOF::Pattern::reset()
{
	world_T_previousCamera_.toNull();
	objectPoints_.clear();
	imagePoints_.clear();
	world_T_guessCamera_.toNull();
	poseGuessTimestamp_.toInvalid();
}

PatternTrackerCore6DOF::PatternTrackerCore6DOF(const Options& options) :
	options_(options)
{
	// If any overriding values are provided, use them and set any missing values.
	if (options_.maxRecognitionTime_ <= 0.0)
	{
#ifdef OCEAN_DEBUG
		options_.maxRecognitionTime_ = 0.5;
#else
		options_.maxRecognitionTime_ = 0.075;
#endif
	}

	if (options_.recognitionCadenceWithTrackedPatterns_ <= 0.0)
	{
		options_.recognitionCadenceWithTrackedPatterns_ = 0.5;
	}

	options_.recognitionCadenceWithoutTrackedPatterns_ = std::max(options_.recognitionCadenceWithoutTrackedPatterns_, 0.0);

	ocean_assert(options_.maxRecognitionTime_ > 0.0);
	ocean_assert(options_.recognitionCadenceWithTrackedPatterns_ >= 0.0);
	ocean_assert(options_.recognitionCadenceWithoutTrackedPatterns_ >= 0.0);
	ocean_assert(options_.recognitionRansacIterations_ > 0);
}

PatternTrackerCore6DOF::~PatternTrackerCore6DOF()
{
	// nothing to do here
}

unsigned int PatternTrackerCore6DOF::addPattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker)
{
	ocean_assert(yFrame);
	ocean_assert(width >= 1u && height >= 1u);

	if (yFrame == nullptr || width == 0u || height == 0u || dimension.x() <= 0)
	{
		return (unsigned int)(-1);
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(patternMap_.find(patternMapIdCounter_) == patternMap_.end());

	Vector2 patternDimension(dimension);
	if (patternDimension.y() <= 0)
	{
		patternDimension.y() = patternDimension.x() * Scalar(height) / Scalar(width);
	}

	const unsigned int patternId = patternMapIdCounter_++;
	patternMap_[patternId] = Pattern(yFrame, width, height, yFramePaddingElements, patternDimension, worker);

	lastRecognitionPatternId_ = patternId;

	return patternId;
}

unsigned int PatternTrackerCore6DOF::addPattern(const std::string& filename, const Vector2& dimension, Worker* worker)
{
	if (filename.empty() || dimension.x() <= 0)
	{
		return (unsigned int)(-1);
	}

	const Frame patternFrame = Media::Utilities::loadImage(filename);

	if (!patternFrame.isValid())
	{
		return (unsigned int)(-1);
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(patternFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return (unsigned int)(-1);
	}

	return addPattern(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), dimension, worker);
}

bool PatternTrackerCore6DOF::removePattern(const unsigned int patternId)
{
	ocean_assert(patternId != (unsigned int)(-1));

	const ScopedLock scopedLock(lock_);

	if (patternId >= patternMapIdCounter_)
	{
		ocean_assert(false && "This id is invalid and has never been used in this tracker!");
		return false;
	}

	ocean_assert(patternMap_.find(patternId) != patternMap_.end());

	return patternMap_.erase(patternId) == 1;
}

bool PatternTrackerCore6DOF::removePatterns()
{
	const ScopedLock scopedLock(lock_);

	patternMap_.clear();

	return true;
}

bool PatternTrackerCore6DOF::determinePoses(const uint8_t* yFrame, const PinholeCamera& pinholeCamera, const unsigned int yFramePaddingElements, const bool frameIsUndistorted, const Timestamp& timestamp, VisualTracker::TransformationSamples& transformations, const Quaternion& world_R_camera, Worker* worker)
{
	ocean_assert(yFrame && pinholeCamera);
	if (yFrame == nullptr || !pinholeCamera.isValid())
	{
		return false;
	}

	// in general, we should remove 'frameIsUndistorted' from the interface,
	// as this information should be given by providing a camera profile with or without distortion parameters,
	// for now we keep the interface as it is
	ocean_assert_and_suppress_unused(frameIsUndistorted == false || pinholeCamera.hasDistortionParameters() == false, frameIsUndistorted);

	const ScopedLock scopedLock(lock_);

	if (patternMap_.empty())
	{
		return false;
	}

	if (options_.noFrameToFrameTracking_)
	{
		timestampPreviousFrame_.toInvalid();

		for (PatternMap::iterator i = patternMap_.begin(); i != patternMap_.end(); ++i)
		{
			Pattern& pattern = i->second;

			pattern.objectPoints().clear();
			pattern.imagePoints().clear();
			pattern.previousPose().toNull();
		}
	}

	ocean_assert(transformations.empty());
	transformations.clear();

	Quaternion previousCamera_R_camera(false);
	if (world_R_previousCamera_.isValid() && world_R_camera.isValid())
	{
		previousCamera_R_camera = world_R_previousCamera_.inverted() * world_R_camera;
	}

	ocean_assert(timestamp.isValid());

	const Frame frame(FrameType(pinholeCamera.width(), pinholeCamera.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_USE_KEEP_LAYOUT, yFramePaddingElements, timestamp);

	if (determinePoses(true /*allowRecognition*/, frame, pinholeCamera, previousCamera_R_camera, worker))
	{
		for (PatternMap::iterator i = patternMap_.begin(); i != patternMap_.end(); ++i)
		{
			if (i->second.previousPose().isValid()) // previousPose as we are done with the current frame already
			{
				transformations.push_back(VisualTracker::TransformationSample(i->second.previousPose(), i->first));
			}
		}
	}

	world_R_previousCamera_ = world_R_camera;

	timestampPreviousFrame_ = timestamp;

	return true;
}

bool PatternTrackerCore6DOF::recentFeatureCorrespondences(const unsigned int patternId, Vectors2& imagePoints, Vectors3& objectPoints, HomogenousMatrix4* pattern_T_camera) const
{
	const ScopedLock scopedLock(lock_);

	PatternMap::const_iterator iPattern = patternMap_.find(patternId);

	if (iPattern == patternMap_.cend())
	{
		return false;
	}

	imagePoints = iPattern->second.imagePoints();
	objectPoints = iPattern->second.objectPoints();

	if (pattern_T_camera != nullptr)
	{
		*pattern_T_camera = iPattern->second.previousPose();
	}

	return true;
}

void PatternTrackerCore6DOF::reset()
{
	const ScopedLock scopedLock(lock_);

	currentFramePyramid_.clear();
	previousFramePyramid_.clear();

	for (PatternMap::iterator i = patternMap_.begin(); i != patternMap_.end(); ++i)
	{
		i->second.reset();
	}

	world_R_previousCamera_ = Quaternion(false);

	timestampPreviousFrame_.toInvalid();

	lastRecognitionAttemptTimestamp_.toInvalid();

	// lastRecognitionPatternId_, we keep the value
}

bool PatternTrackerCore6DOF::convertPoseForCamera(const PinholeCamera& newCamera, const PinholeCamera& referenceCamera, const HomogenousMatrix4& referencePose, HomogenousMatrix4& newPose)
{
	ocean_assert(newCamera.isValid());
	ocean_assert(referenceCamera.isValid());
	ocean_assert(referencePose.isValid());

	if (newCamera == referenceCamera)
	{
		// both camera profiles are actually identical so that the pose does not change

		newPose = referencePose;
		return true;
	}

	const Scalar cameraWidth = Scalar(newCamera.width());
	const Scalar cameraHeight = Scalar(newCamera.height());

	// we simply project 5 image points back onto the tracked ground plane

	constexpr size_t numberPoints = 5;

	const Vector2 distortedImagePoints[numberPoints] =
	{
		Vector2(cameraWidth * Scalar(0.1), cameraHeight * Scalar(0.1)), // top left
		Vector2(cameraWidth * Scalar(0.1), cameraHeight * Scalar(0.9)), // bottom left
		Vector2(cameraWidth * Scalar(0.9), cameraHeight * Scalar(0.9)), // bottom right
		Vector2(cameraWidth * Scalar(0.9), cameraHeight * Scalar(0.1)), // top right
		Vector2(cameraWidth * Scalar(0.5), cameraHeight * Scalar(0.5)) // center
	};

	const Plane3 plane(Vector3(0, 0, 0), Vector3(0, 1, 0));

	Vector3 objectPoints[numberPoints];
	const HomogenousMatrix4 iFlippedPose = PinholeCamera::standard2InvertedFlipped(referencePose);

	for (size_t n = 0; n < numberPoints; ++n)
	{
		const Vector2& distortedImagePoint = distortedImagePoints[n];
		const Vector2 undistortedImagePoint = referenceCamera.undistortDamped(distortedImagePoint);

		const Line3 ray = referenceCamera.ray(undistortedImagePoint, referencePose);

		if (plane.intersection(ray, objectPoints[n]) == false
				|| !referenceCamera.isObjectPointInFrontIF(iFlippedPose, objectPoints[n]))
		{
			newPose = referencePose;
			return false;
		}
	}

	// and now we use the object points to determine the camera pose for the new camera profile

	return Geometry::NonLinearOptimizationPose::optimizePose(newCamera, referencePose, ConstArrayAccessor<Vector3>(objectPoints, numberPoints), ConstArrayAccessor<Vector2>(distortedImagePoints, numberPoints), true, newPose);
}

bool PatternTrackerCore6DOF::determinePoses(const bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	ocean_assert(yFrame && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(yFrame.width() == pinholeCamera.width() && yFrame.height() == pinholeCamera.height());

	// we ensure that we do not handle too much pixel information, therefore we may use a downsampled image for tracking

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	if (options_.downsampleInputImageOnAndroid_)
	{
		const unsigned int desiredOperationSize = 360u;

		Frame yFrameDownsampled(yFrame, Frame::ACM_USE_KEEP_LAYOUT);

		CV::FramePyramid framePyramid;
		const unsigned int pyramidLayers = CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), desiredOperationSize - 1u);

		if (pyramidLayers >= 2u)
		{
			if (framePyramid.replace8BitPerChannel11(yFrame, pyramidLayers, false /*copyFirstLayer*/, nullptr))
			{
				yFrameDownsampled = Frame(framePyramid.coarsestLayer(), Frame::ACM_USE_KEEP_LAYOUT);
			}
			else
			{
				ocean_assert(false && "This should never happen!");
			}
		}

		ocean_assert(yFrameDownsampled);

		if (std::min(yFrameDownsampled.width(), yFrameDownsampled.height()) > desiredOperationSize * 120u / 100u)
		{
			unsigned int desiredWidth = 0u;
			unsigned int desiredHeight = 0u;

			if (yFrameDownsampled.width() < yFrameDownsampled.height())
			{
				desiredWidth = desiredOperationSize;
				desiredHeight = (yFrameDownsampled.height() * desiredWidth + yFrameDownsampled.width() / 2u) / yFrameDownsampled.width();
			}
			else
			{
				desiredHeight = desiredOperationSize;
				desiredWidth = (yFrameDownsampled.width() * desiredHeight + yFrameDownsampled.height() / 2u) / yFrameDownsampled.height();
			}

			ocean_assert(desiredWidth >= desiredOperationSize);
			ocean_assert(desiredHeight >= desiredOperationSize);

			Frame yDesiredFrame(FrameType(yFrameDownsampled, desiredWidth, desiredHeight));

			CV::FrameInterpolatorBilinear::resize<uint8_t, 1u>(yFrameDownsampled.constdata<uint8_t>(), yDesiredFrame.data<uint8_t>(), yFrameDownsampled.width(), yFrameDownsampled.height(), yDesiredFrame.width(), yDesiredFrame.height(), yFrameDownsampled.paddingElements(), yDesiredFrame.paddingElements(), worker);

			std::swap(yFrameDownsampled, yDesiredFrame);
		}

		yFrameDownsampled.setTimestamp(yFrame.timestamp());

		const PinholeCamera cameraDownsampled(yFrameDownsampled.width(), yFrameDownsampled.height(), pinholeCamera);

		return determinePosesWithDownsampledResolution(allowRecognition, yFrameDownsampled, cameraDownsampled, previousCamera_R_camera, worker);
	}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

	if (yFrame.pixels() > 1280u * 720u)
	{
		Frame yFrameDownsampled(FrameType(yFrame, yFrame.width() / 2u, yFrame.height() / 2u));
		CV::FrameShrinker::downsampleByTwo8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrameDownsampled.data<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.paddingElements(), yFrameDownsampled.paddingElements(), nullptr);

		yFrameDownsampled.setTimestamp(yFrame.timestamp());

		const PinholeCamera cameraDownsampled(yFrameDownsampled.width(), yFrameDownsampled.height(), pinholeCamera);

		return determinePosesWithDownsampledResolution(allowRecognition, yFrameDownsampled, cameraDownsampled, previousCamera_R_camera, worker);
	}
	else
	{
		return determinePosesWithDownsampledResolution(allowRecognition, yFrame, pinholeCamera, previousCamera_R_camera, worker);
	}
}

bool PatternTrackerCore6DOF::determinePosesWithDownsampledResolution(const bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	ocean_assert(yFrame && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(yFrame.width() == pinholeCamera.width() && yFrame.height() == pinholeCamera.height());

	const unsigned int pyramidLayers = previousFramePyramid_.isValid() ? previousFramePyramid_.layers() : CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), 15u, 15u);

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	currentFramePyramid_.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.channels(), yFrame.pixelOrigin(), pyramidLayers,  yFrame.paddingElements(), true /*copyFirstLayer*/, worker, yFrame.pixelFormat(), yFrame.timestamp());

	for (PatternMap::iterator i = patternMap_.begin(); i != patternMap_.end(); ++i)
	{
		Pattern& pattern = i->second;

		// check whether we have 3D object points (and corresponding 2D image points) from the previous frame so that we can find the new image points in the current frame
		if (pattern.objectPoints().size() > 15)
		{
			ocean_assert(pattern.previousPose().isValid());

			// try to determine the current pose by application of the previous correspondences
			if (!determinePoseWithPreviousCorrespondences(pinholeCamera, previousFramePyramid_, currentFramePyramid_, pattern, previousCamera_R_camera, worker))
			{
				pattern.objectPoints().clear();
				pattern.imagePoints().clear();
				pattern.previousPose().toNull();
			}

			// check whether we should try to add new correspondences / reset the old ones - this is better than an entire brute-force re-tracking
			if (pattern.previousPose().isValid() && pattern.objectPoints().size() <= 20)
			{
				Geometry::SpatialDistribution::OccupancyArray occupancyArray;
				optimizePoseByRectification(pinholeCamera, currentFramePyramid_, HomogenousMatrix4(pattern.previousPose()), pattern, pattern.previousPose(), worker, &occupancyArray);

				ocean_assert(pattern.previousPose().isValid());
				const Triangles2 triangles(pattern.triangles2(pinholeCamera));

				const CV::SubRegion subRegion(triangles2subRegion(triangles, pinholeCamera.width(), pinholeCamera.height()));

				const Vectors2 strongHarrisCorners = CV::Detector::FeatureDetector::determineHarrisPoints(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), subRegion, 15u, 15u, 15u, worker);

				Vectors2 validPoints;
				validPoints.reserve(strongHarrisCorners.size() / 2);

				for (size_t n = 0; n < strongHarrisCorners.size(); ++n)
				{
					if (occupancyArray(strongHarrisCorners[n]))
					{
						validPoints.push_back(strongHarrisCorners[n]);
					}
				}

				if (validPoints.empty())
				{
					pattern.imagePoints().clear();
					pattern.objectPoints().clear();
				}
				else
				{
					pattern.imagePoints() = Geometry::SpatialDistribution::distributeAndFilter(validPoints.data(), validPoints.size(), subRegion.boundingBox().left(), subRegion.boundingBox().top(), subRegion.boundingBox().width(), subRegion.boundingBox().height(), 15u, 15u);

					pattern.objectPoints() = Geometry::Utilities::backProjectImagePoints(pinholeCamera, pattern.previousPose(), Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0)), pattern.imagePoints().data(), pattern.imagePoints().size(), pinholeCamera.hasDistortionParameters());
				}
			}
		}
		else
		{
			pattern.objectPoints().clear();
			pattern.imagePoints().clear();
			pattern.previousPose().toNull();
		}
	}

	unsigned int currentlyVisiblePattern = internalNumberVisiblePattern();

	// For patterns that lost tracking since the last frame, attempt to re-localize with a more robust procedure.
	if (timestampPreviousFrame_.isValid() && (currentlyVisiblePattern == 0u || currentlyVisiblePattern < internalMaxConcurrentlyVisiblePattern()))
	{
		for (PatternMap::iterator iP = patternMap_.begin(); iP != patternMap_.end(); ++iP)
		{
			Pattern& pattern = iP->second;

			if (pattern.previousPose().isValid() == false)
			{
				Timestamp poseGuessTimestamp;
				const HomogenousMatrix4& poseGuess = pattern.poseGuess(&poseGuessTimestamp);

				if (poseGuess.isValid() && poseGuessTimestamp == timestampPreviousFrame_)
				{
					determinePoseWithDriftErrors(pinholeCamera, previousFramePyramid_, currentFramePyramid_, pattern, previousCamera_R_camera, worker);
				}
			}
		}

		currentlyVisiblePattern = internalNumberVisiblePattern();
	}

	// Attempt to track new patterns.
	if (allowRecognition && (currentlyVisiblePattern == 0u || currentlyVisiblePattern < internalMaxConcurrentlyVisiblePattern()) && (!lastRecognitionAttemptTimestamp_.isValid() || yFrame.timestamp() > lastRecognitionAttemptTimestamp_ + maximumDurationBetweenRecognitionAttempts()))
	{
		determinePosesWithoutKnowledge(pinholeCamera, yFrame, currentFramePyramid_, previousCamera_R_camera, worker);
		lastRecognitionAttemptTimestamp_ = yFrame.timestamp();
	}

	// we use the current frame pyramid as previous frame pyramid in the next tracking iteration
	std::swap(previousFramePyramid_, currentFramePyramid_);

	// at this moment the previous pose is also the pose for the current frame
	for (PatternMap::iterator i = patternMap_.begin(); i != patternMap_.end(); ++i)
	{
		if (i->second.previousPose().isValid())
		{
			i->second.setPoseGuess(i->second.previousPose(), yFrame.timestamp());
		}
	}

	return internalNumberVisiblePattern() != 0u;
}

bool PatternTrackerCore6DOF::determinePoseWithDriftErrors(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid_, const CV::FramePyramid& currentFramePyramid_, Pattern& pattern, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(currentFramePyramid_);

	if (currentFramePyramid_.frameType() != previousFramePyramid_.frameType())
	{
		// **TODO** supporting changed input image resolutions
		return false;
	}

	ocean_assert(currentFramePyramid_.layers() == previousFramePyramid_.layers());
	ocean_assert(previousFramePyramid_.finestWidth() == pinholeCamera.width() && previousFramePyramid_.finestHeight() == pinholeCamera.height());

	const HomogenousMatrix4& poseGuess = pattern.poseGuess();
	ocean_assert(poseGuess.isValid());

	constexpr unsigned int trackingLayer = 1u;

	const CV::FramePyramid hierarchyPrevious(previousFramePyramid_, trackingLayer, 3u, false /*copyData*/);
	const CV::FramePyramid hierarchyCurrent(currentFramePyramid_, trackingLayer, 3u, false /*copyData*/);

	const PinholeCamera hierarchyCamera(hierarchyPrevious.finestWidth(), hierarchyPrevious.finestHeight(), pinholeCamera);

	const Triangles2 triangles(pattern.triangles2(hierarchyCamera, poseGuess));
	const CV::SubRegion subRegion(triangles2subRegion(triangles, hierarchyCamera.width(), hierarchyCamera.height()));

	Vectors2 previousFeaturePoints = CV::Detector::FeatureDetector::determineHarrisPoints(hierarchyPrevious.finestLayer().constdata<uint8_t>(), hierarchyPrevious.finestWidth(), hierarchyPrevious.finestHeight(), hierarchyPrevious.finestLayer().paddingElements(), subRegion, 20u, 20u, 5u, worker);
	Vectors2 currentFeaturePoints;

	if (previousFeaturePoints.size() > 10)
	{
		Vectors2 roughCurrentFeaturePoints;
		roughCurrentFeaturePoints.reserve(previousFeaturePoints.size());

		if (previousCamera_R_camera.isValid())
		{
			const SquareMatrix3 homography = Geometry::Homography::homographyMatrix(previousCamera_R_camera, hierarchyCamera, hierarchyCamera);

			roughCurrentFeaturePoints.reserve(previousFeaturePoints.size());

			for (const Vector2& previousFeaturePoint : previousFeaturePoints)
			{
				const Vector2 roughCurrentFeaturePoint = homography * previousFeaturePoint;

				roughCurrentFeaturePoints.emplace_back(minmax<Scalar>(0, roughCurrentFeaturePoint.x(), Scalar(hierarchyCamera.width() - 1u)), minmax<Scalar>(0, roughCurrentFeaturePoint.y(), Scalar(hierarchyCamera.height() - 1u)));
			}
		}
		else
		{
			roughCurrentFeaturePoints = previousFeaturePoints;
		}

		ocean_assert(previousFeaturePoints.size() == roughCurrentFeaturePoints.size());

		constexpr size_t kMinNumFeaturePoints = 25u;

		if (CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 7u>(hierarchyPrevious, hierarchyCurrent, 8u, previousFeaturePoints, roughCurrentFeaturePoints, currentFeaturePoints, Scalar(1.9 * 1.9), worker) && previousFeaturePoints.size() > kMinNumFeaturePoints)
		{
			ocean_assert(currentFeaturePoints.size() == previousFeaturePoints.size());

			const Vectors3 objectPoints = Geometry::Utilities::backProjectImagePoints(hierarchyCamera, poseGuess, Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0)), previousFeaturePoints.data(), previousFeaturePoints.size(), hierarchyCamera.hasDistortionParameters());

			ocean_assert(objectPoints.size() == currentFeaturePoints.size());

			if (currentFeaturePoints.size() > kMinNumFeaturePoints)
			{
				RandomGenerator randomGenerator;

				HomogenousMatrix4 pose(false);
				if (Geometry::RANSAC::p3p(AnyCameraPinhole(hierarchyCamera), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(currentFeaturePoints), randomGenerator, pose, 16u, true, 50u, Scalar(2.5 * 2.5)))
				{
					if ((pose * poseGuess.inverted()).rotation().angle() < Numeric::deg2rad(25))
					{
						pattern.previousPose() = pose;
						return true;
					}
				}
			}
		}
	}

	if (previousCamera_R_camera.isValid() && previousCamera_R_camera.angle() >= Numeric::deg2rad(3.5))
	{
		// we have a very extreme camera motion, so let's simply use the IMU data
		pattern.previousPose() = poseGuess * previousCamera_R_camera;
		return true;
	}

	return false;
}

bool PatternTrackerCore6DOF::determinePoseWithPreviousCorrespondences(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, Pattern& pattern, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(currentFramePyramid);

	if (currentFramePyramid.frameType() != previousFramePyramid.frameType())
	{
		// **TODO** supporting changed input image resolutions
		return false;
	}

	ocean_assert(currentFramePyramid.layers() == previousFramePyramid.layers());

	ocean_assert(pattern.objectPoints().size() >= 3);
	ocean_assert(pattern.objectPoints().size() == pattern.imagePoints().size());
	ocean_assert(pattern.previousPose().isValid());

	const Quaternion previousCamera_R_camera_OrIdentity = previousCamera_R_camera.isValid() ? previousCamera_R_camera : Quaternion(true);

	HomogenousMatrix4 currentRoughPose(false);

	// check whether we can apply a frame2frame tracking on a low pyramid resolution to receive a rough pose
	if (currentFramePyramid.layers() >= 3u)
	{
		trackFrame2FrameHierarchy(pinholeCamera, previousFramePyramid, currentFramePyramid, 2u, pattern.previousPose() * previousCamera_R_camera_OrIdentity, pattern.objectPoints(), pattern.imagePoints(), currentRoughPose, worker, 40u, Scalar(1.9 * 1.9));
	}

	Vectors2 currentImagePoints;
	HomogenousMatrix4 pose;

	if (currentRoughPose.isValid())
	{
		const CV::FramePyramid previous(previousFramePyramid, 0u, 3u, false /*copyData*/);
		const CV::FramePyramid current(currentFramePyramid, 0u, 3u, false /*copyData*/);

		if (!trackFrame2Frame(pinholeCamera, previous, current, pattern.previousPose(), pattern.objectPoints(), pattern.imagePoints(), currentImagePoints, pose, currentRoughPose, worker))
		{
			return false;
		}
	}
	else
	{
		if (!trackFrame2Frame(pinholeCamera, previousFramePyramid, currentFramePyramid, pattern.previousPose() * previousCamera_R_camera_OrIdentity, pattern.objectPoints(), pattern.imagePoints(), currentImagePoints, pose, HomogenousMatrix4(false), worker))
		{
			return false;
		}
	}

	pattern.imagePoints() = std::move(currentImagePoints);
	pattern.previousPose() = pose;

	return true;
}

bool PatternTrackerCore6DOF::trackFrame2FrameHierarchy(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const unsigned int trackingLayer, const HomogenousMatrix4& previousPose, const Vectors3& previousObjectPoints, const Vectors2& previousImagePoints, HomogenousMatrix4& roughPose, Worker* worker, const unsigned int numberFeatures, const Scalar maxError)
{
	ocean_assert(previousFramePyramid && currentFramePyramid && previousFramePyramid.frameType() == currentFramePyramid.frameType());
	ocean_assert(pinholeCamera.isValid() && pinholeCamera.width() == previousFramePyramid.finestWidth() && pinholeCamera.height() == previousFramePyramid.finestHeight());
	ocean_assert(previousObjectPoints.size() == previousImagePoints.size() && previousImagePoints.size() >= 3);

	if (previousFramePyramid.layers() <= trackingLayer || previousImagePoints.size() < 3)
	{
		return false;
	}

	const CV::FramePyramid hierarchyPrevious(previousFramePyramid, trackingLayer, 3u, false /*copyData*/);
	const CV::FramePyramid hierarchyCurrent(currentFramePyramid, trackingLayer, 3u, false /*copyData*/);

	const PinholeCamera hierarchyCamera(hierarchyPrevious.finestWidth(), hierarchyPrevious.finestHeight(), pinholeCamera);

	Vectors3 hierarchyPreviousObjectPoints;
	Vectors2 hierarchyPreviousImagePoints;

	hierarchyPreviousObjectPoints.reserve(numberFeatures);
	hierarchyPreviousImagePoints.reserve(numberFeatures);

	SuccessionSubset<Scalar, 2> subset((SuccessionSubset<Scalar, 2>::Object*)previousImagePoints.data(), previousImagePoints.size());

	const Scalar factor = Scalar(1) / Scalar(previousFramePyramid.sizeFactor(trackingLayer));

	while (hierarchyPreviousObjectPoints.size() < numberFeatures)
	{
		const size_t index = subset.incrementSubset();

		if (index == size_t(-1))
		{
			break;
		}

		const Vector2 previousImagePoint(previousImagePoints[index] * factor);

		if (previousImagePoint.x() >= 3 && previousImagePoint.y() >= 3 && previousImagePoint.x() <= Scalar(hierarchyCamera.width() - 3u) && previousImagePoint.y() <= Scalar(hierarchyCamera.height() - 3u))
		{
			hierarchyPreviousObjectPoints.push_back(previousObjectPoints[index]);
			hierarchyPreviousImagePoints.push_back(previousImagePoint);
		}
	}

	if (hierarchyPreviousObjectPoints.size() <= 5)
	{
		return false;
	}

	const Vectors2 hierarchyPreviousImagePointsCopy(hierarchyPreviousImagePoints);

	constexpr unsigned int numberCoarseLayerRadii = 3u;
	const unsigned int coarseLayerRadii[numberCoarseLayerRadii] = {2u, 4u, 8u};

	Indices32 validIndices;
	Vectors2 hierarchyCurrentImagePoints;

	for (unsigned int n = 0u; n < numberCoarseLayerRadii; ++n)
	{
		validIndices.clear();
		hierarchyCurrentImagePoints.clear();

		const unsigned int coarseLayerRadius = coarseLayerRadii[n];

		if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 7u>(hierarchyPrevious, hierarchyCurrent, coarseLayerRadius, hierarchyPreviousImagePoints, hierarchyPreviousImagePointsCopy, hierarchyCurrentImagePoints, maxError, worker, &validIndices))
		{
			return false;
		}

		if (validIndices.size() >= 10)
		{
			break;
		}
	}

	if (validIndices.size() <= 5)
	{
		return false;
	}

	if (hierarchyPreviousImagePoints.size() != validIndices.size())
	{
		hierarchyPreviousObjectPoints = Subset::subset(hierarchyPreviousObjectPoints, validIndices);
		hierarchyCurrentImagePoints = Subset::subset(hierarchyCurrentImagePoints, validIndices);
	}

#ifdef OCEAN_DEBUG
	hierarchyPreviousImagePoints.clear();
#endif

	ocean_assert(hierarchyPreviousObjectPoints.size() == hierarchyCurrentImagePoints.size());

	Scalar initialError, finalError;
	if (!Geometry::NonLinearOptimizationPose::optimizePose(hierarchyCamera, previousPose, ConstArrayAccessor<Vector3>(hierarchyPreviousObjectPoints), ConstArrayAccessor<Vector2>(hierarchyCurrentImagePoints), pinholeCamera.hasDistortionParameters(), roughPose, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), &initialError, &finalError))
	{
		return false;
	}

	return true;
}

bool PatternTrackerCore6DOF::trackFrame2Frame(const PinholeCamera& pinholeCamera, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const HomogenousMatrix4& previousPose, Vectors3& previousObjectPoints, Vectors2& previousImagePoints, Vectors2& currentImagePoints, HomogenousMatrix4& currentPose, const HomogenousMatrix4& roughCurrentPose, Worker* worker)
{
	ocean_assert(pinholeCamera);
	ocean_assert(previousFramePyramid && currentFramePyramid);
	ocean_assert(previousFramePyramid.frameType() == currentFramePyramid.frameType());
	ocean_assert(FrameType::formatIsGeneric(previousFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(FrameType::formatIsGeneric(currentFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(pinholeCamera.width() == previousFramePyramid.finestWidth() && pinholeCamera.height() == previousFramePyramid.finestHeight());

	ocean_assert(previousObjectPoints.size() == previousImagePoints.size() && previousImagePoints.size() >= 3);

#ifdef OCEAN_DEBUG
	for (Vectors2::const_iterator i = previousImagePoints.begin(); i != previousImagePoints.end(); ++i)
	{
		ocean_assert(i->x() >= 0 && i->y() >= 0 && i->x() <= Scalar(previousFramePyramid.finestLayer().width()) && i->y() <= Scalar(previousFramePyramid.finestLayer().height()));
	}
#endif

	constexpr unsigned int numberCoarseLayerRadii = 2u;
	const unsigned int coarseLayerRadii[numberCoarseLayerRadii] = {2u, 4u};

	currentImagePoints.clear();

	const Vectors2 previousImagePointsCopy(previousImagePoints);

	if (roughCurrentPose.isNull())
	{
		Indices32 validIndices;

		for (unsigned int n = 0u; n < numberCoarseLayerRadii; ++n)
		{
			validIndices.clear();
			currentImagePoints.clear();

			const unsigned int coarseLayerRadius = coarseLayerRadii[n];

			if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 7u>(previousFramePyramid, currentFramePyramid, coarseLayerRadius, previousImagePoints, previousImagePointsCopy, currentImagePoints, Scalar(1.9 * 1.9), worker, &validIndices, 2u))
			{
				return false;
			}

			if (validIndices.size() >= 10)
			{
				break;
			}
		}

		if (validIndices.size() != previousObjectPoints.size())
		{
			previousObjectPoints = Subset::subset(previousObjectPoints, validIndices);
			currentImagePoints = Subset::subset(currentImagePoints, validIndices);
		}

		ocean_assert(previousObjectPoints.size() == currentImagePoints.size());

#ifdef OCEAN_DEBUG
		previousImagePoints.clear();
#endif
	}
	else
	{
		Vectors2 roughCurrentImagePoints;
		const HomogenousMatrix4 roughCurrentPoseIF(PinholeCamera::standard2InvertedFlipped(roughCurrentPose));

		Indices32 validIndices;

		roughCurrentImagePoints.reserve(previousImagePoints.size());
		validIndices.reserve(previousImagePoints.size());

		for (size_t n = 0; n < previousObjectPoints.size(); ++n)
		{
			const Vector2 roughPoint = pinholeCamera.projectToImageIF<true>(roughCurrentPoseIF, previousObjectPoints[n], true);

			if (roughPoint.x() >= 3 && roughPoint.y() >= 3 && roughPoint.x() < Scalar(previousFramePyramid.finestLayer().width() - 3u) && roughPoint.y() < Scalar(previousFramePyramid.finestLayer().height() - 3u))
			{
				roughCurrentImagePoints.push_back(roughPoint);
				validIndices.push_back((unsigned int)n);
			}
		}

		if (validIndices.size() < 3)
		{
			return false;
		}

		if (validIndices.size() != previousObjectPoints.size())
		{
			previousObjectPoints = Subset::subset(previousObjectPoints, validIndices);
			previousImagePoints = Subset::subset(previousImagePoints, validIndices);
		}

		ocean_assert(previousObjectPoints.size() == previousImagePoints.size());
		ocean_assert(previousObjectPoints.size() == roughCurrentImagePoints.size());

		for (unsigned int n = 0u; n < numberCoarseLayerRadii; ++n)
		{
			validIndices.clear();
			currentImagePoints.clear();

			const unsigned int coarseLayerRadius = coarseLayerRadii[n];

			if (!CV::Advanced::AdvancedMotionSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 7u>(previousFramePyramid, currentFramePyramid, coarseLayerRadius, previousImagePoints, roughCurrentImagePoints, currentImagePoints, Scalar(1.9 * 1.9), worker, &validIndices, 2u))
			{
				return false;
			}

			if (validIndices.size() >= 10)
			{
				break;
			}
		}

		if (validIndices.size() != previousObjectPoints.size())
		{
			previousObjectPoints = Subset::subset(previousObjectPoints, validIndices);
			currentImagePoints = Subset::subset(currentImagePoints, validIndices);
		}

		ocean_assert(previousObjectPoints.size() == currentImagePoints.size());

#ifdef OCEAN_DEBUG
		previousImagePoints.clear();
#endif
	}

	if (currentImagePoints.size() <= 3)
	{
		return false;
	}

	ocean_assert(previousObjectPoints.size() == currentImagePoints.size());

	Scalar initialError, finalError;
	if (!Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, previousPose, ConstArrayAccessor<Vector3>(previousObjectPoints), ConstArrayAccessor<Vector2>(currentImagePoints), pinholeCamera.hasDistortionParameters(), currentPose, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(10), &initialError, &finalError))
	{
		return false;
	}

	return true;
}

bool PatternTrackerCore6DOF::optimizePoseByRectification(const PinholeCamera& pinholeCamera, const CV::FramePyramid& currentFramePyramid, const HomogenousMatrix4& roughPose, const Pattern& pattern, HomogenousMatrix4& optimizedPose, Worker* worker, Geometry::SpatialDistribution::OccupancyArray* occupancyArray)
{
	ocean_assert(pinholeCamera.isValid() && currentFramePyramid && roughPose.isValid() && pattern.isValid());
	ocean_assert(pinholeCamera.width() == currentFramePyramid.finestWidth() && pinholeCamera.height() == currentFramePyramid.finestHeight());

	ocean_assert(FrameType::formatIsGeneric(currentFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(FrameType::formatIsGeneric(pattern.pyramid().frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(currentFramePyramid.frameType().pixelOrigin() == pattern.pyramid().frameType().pixelOrigin());

	const HomogenousMatrix4 roughPoseIF(PinholeCamera::standard2InvertedFlipped(roughPose));

	// Find the pattern layer best matching with the size of the rectified image.
	unsigned int patternPyramidLayer = static_cast<unsigned int>(-1);

	const Vector2 corner0(pinholeCamera.projectToImageIF<true>(roughPoseIF, pattern.corner0(), pinholeCamera.hasDistortionParameters()));
	const Vector2 corner1(pinholeCamera.projectToImageIF<true>(roughPoseIF, pattern.corner1(), pinholeCamera.hasDistortionParameters()));
	const Vector2 corner2(pinholeCamera.projectToImageIF<true>(roughPoseIF, pattern.corner2(), pinholeCamera.hasDistortionParameters()));
	const Vector2 corner3(pinholeCamera.projectToImageIF<true>(roughPoseIF, pattern.corner3(), pinholeCamera.hasDistortionParameters()));

	const unsigned int currentSize = min(currentFramePyramid.finestWidth(), currentFramePyramid.finestHeight());
	const unsigned int patternSize = min(pattern.pyramid().finestWidth(), pattern.pyramid().finestHeight());

	const unsigned int maximalSize = max(50u, min(currentSize, patternSize));

	const Scalar maxHorizontal(max(corner0.distance(corner3), corner1.distance(corner2)));
	const Scalar maxVertical(max(corner0.distance(corner1), corner2.distance(corner3)));

	if (maxHorizontal > maxVertical)
	{
		// find the pattern pyramid layer best matching to the given horizontal size

		const unsigned int horizontal = minmax(50u, (unsigned int)Numeric::ceil(maxHorizontal), maximalSize);

		for (unsigned int n = 0u; n < pattern.pyramid().layers(); ++n)
		{
			if (horizontal >= pattern.pyramid().width(n) && horizontal <= pattern.pyramid().width(n) * 2u)
			{
				patternPyramidLayer = n;
				break;
			}
		}
	}
	else
	{
		// find the pattern pyramid layer best matching to the given vertical size

		const unsigned int vertical = minmax(50u, (unsigned int)Numeric::ceil(maxVertical), maximalSize);

		for (unsigned int n = 0u; n < pattern.pyramid().layers(); ++n)
		{
			if (vertical >= pattern.pyramid().height(n) && vertical <= pattern.pyramid().height(n) * 2u)
			{
				patternPyramidLayer = n;
				break;
			}
		}
	}

	ocean_assert(patternPyramidLayer != static_cast<unsigned int>(-1));
	if (patternPyramidLayer >= pattern.layers())
	{
		return false;
	}

	// Create an rectified image of the unwrapped cone, sampled from the given input frame.
	const unsigned int referenceWidth = pattern.pyramid().width(patternPyramidLayer);
	const unsigned int referenceHeight = pattern.pyramid().height(patternPyramidLayer);
	ocean_assert(referenceWidth != 0u && referenceHeight != 0u);

	Frame rectifiedFrame(FrameType(currentFramePyramid.frameType(), referenceWidth, referenceHeight));
	rectifiedFrame.setValue(0x00u);

	Frame rectifiedFrameMask(FrameType(rectifiedFrame, FrameType::FORMAT_Y8));

	ocean_assert(rectifiedFrame.isContinuous());
	ocean_assert(rectifiedFrameMask.isContinuous());

	const Frame& pyramidLayer = currentFramePyramid.finestLayer();

	// Perform a planar rectification with a lookup table.
	CV::Advanced::FrameRectification::planarRectangleObjectMaskIF8BitPerChannel<1u>(pyramidLayer.constdata<uint8_t>(), pyramidLayer.paddingElements(), pyramidLayer.pixelOrigin(), AnyCameraPinhole(pinholeCamera), roughPoseIF, pattern.corner0(), pattern.corner3(), pattern.corner1(), rectifiedFrame.data<uint8_t>(), rectifiedFrameMask.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), rectifiedFrameMask.paddingElements(), worker, 0xFF, 50u);

	const CV::FramePyramid rectifiedPyramid(rectifiedFrame, 3u /*layers*/, false /*copyFirstLayer*/, worker);

	// determine visible pattern feature points which should be also visible in the current rectified camera frame
	const Vectors2& patternReferencePoints = pattern.referencePoints(patternPyramidLayer);
	CV::SubRegion subRegion(rectifiedFrameMask, CV::PixelBoundingBox(), 0xFF);

	Vectors2 patternPoints(subRegion.filterPoints(Geometry::SpatialDistribution::distributeAndFilter(patternReferencePoints.data(), patternReferencePoints.size(), 0, 0, Scalar(rectifiedFrameMask.width()), Scalar(rectifiedFrameMask.height()), 15u, 15u)));
	const Vectors2 patternPointsCopy(patternPoints);

	Vectors2 rectifiedPoints;
	Indices32 validPointIndices;
	if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 7u>(CV::FramePyramid(pattern.pyramid(), patternPyramidLayer, 3u, false /*copyData*/), rectifiedPyramid, 2u, patternPoints, patternPointsCopy, rectifiedPoints, Scalar(0.9 * 0.9), worker, &validPointIndices, 2u))
	{
		return false;
	}

	constexpr size_t minNumPoints = 4;

	if (validPointIndices.size() >= minNumPoints)
	{
		const Vectors2 invalidPatternPoints(Subset::subset(patternPoints, Subset::invertedIndices(validPointIndices, patternPoints.size())));
		patternPoints = Subset::subset(patternPoints, validPointIndices);
		rectifiedPoints = Subset::subset(rectifiedPoints, validPointIndices);

		Vectors3 objectPoints;
		objectPoints.reserve(patternPoints.size());
		Vectors2 imagePoints;
		imagePoints.reserve(rectifiedPoints.size());

		// Compute the associated 3D object points for both the pattern and the rectified image.

		const unsigned int patternLayerWidth = pattern.pyramid()[patternPyramidLayer].width();
		const unsigned int patternLayerHeight = pattern.pyramid()[patternPyramidLayer].height();

		ocean_assert(patternLayerWidth != 0u && patternLayerHeight != 0u);

		const Scalar invPatternLayerWidth = Scalar(1) / Scalar(patternLayerWidth);
		const Scalar invPatternLayerHeight = Scalar(1) / Scalar(patternLayerHeight);

		for (Vectors2::const_iterator i = patternPoints.begin(); i != patternPoints.end(); ++i)
		{
			objectPoints.push_back(Vector3(i->x() * pattern.dimension().x() * invPatternLayerWidth, 0, i->y() * pattern.dimension().y() * invPatternLayerHeight));
		}

		for (Vectors2::const_iterator i = rectifiedPoints.begin(); i != rectifiedPoints.end(); ++i)
		{
			imagePoints.push_back(CV::Advanced::FrameRectification::planarRectangleObjectRectifiedPosition2cameraPositionIF<true>(AnyCameraPinhole(pinholeCamera), roughPoseIF, pattern.corner0(), pattern.corner3(), pattern.corner1(), rectifiedFrame.width(), rectifiedFrame.height(), *i));
		}

		// Optimize the given rough pose by application of the new 2D/3D correspondences.
		if (imagePoints.size() >= minNumPoints)
		{
			Scalar initError, finalError;
			if (Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, roughPose, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), pinholeCamera.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5), &initError, &finalError))
			{
				if (occupancyArray)
				{
					const Box2 imagePointsBoundingBox(imagePoints.data(), imagePoints.size());

					// First, mark every bin as valid.
					Geometry::SpatialDistribution::OccupancyArray tmpArray(Geometry::SpatialDistribution::OccupancyArray(imagePointsBoundingBox.left(), imagePointsBoundingBox.top(), imagePointsBoundingBox.width(), imagePointsBoundingBox.height(), 8u, 8u, false));

					// Invalidate the bins for invalid points.
					const HomogenousMatrix4 optimizedPoseIF(PinholeCamera::standard2InvertedFlipped(optimizedPose));

					for (ImagePoints::const_iterator i = invalidPatternPoints.begin(); i != invalidPatternPoints.end(); ++i)
					{
						tmpArray.removePoint(CV::Advanced::FrameRectification::planarRectangleObjectRectifiedPosition2cameraPositionIF<true>(AnyCameraPinhole(pinholeCamera), optimizedPoseIF, pattern.corner0(), pattern.corner3(), pattern.corner1(), rectifiedFrame.width(), rectifiedFrame.height(), *i));
					}

					// Re-validate the bins for valid points.
					for (ImagePoints::const_iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
					{
						tmpArray.addPoint(*i);
					}

					*occupancyArray = tmpArray;

					for (unsigned int yBin = 1u; yBin < tmpArray.verticalBins() - 1u; ++yBin)
					{
						for (unsigned int xBin = 1u; xBin < tmpArray.horizontalBins() - 1u; ++xBin)
						{
							if (!tmpArray(xBin, yBin) && tmpArray.countOccupiedNeighborhood9(xBin, yBin) >= 7)
							{
								(*occupancyArray)(xBin, yBin) = 1u;
							}
						}
					}
				}
			}
		}
	}

	return true;
}

bool PatternTrackerCore6DOF::determinePosesWithoutKnowledge(const PinholeCamera& pinholeCamera, const Frame& yFrame, const CV::FramePyramid& currentFramePyramid, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	if (internalNumberVisiblePattern() >= internalMaxConcurrentlyVisiblePattern())
	{
		return true;
	}

	const Timestamp recognitionStartTimestamp(true);

	ocean_assert(pinholeCamera.isValid() && yFrame.isValid());
	ocean_assert(pinholeCamera.width() == yFrame.width() && pinholeCamera.height() == yFrame.height());

	Vectors2 imagePoints;
	Descriptors imagePointDescriptors;
	if (!detectAndDescribeFeatures(std::make_shared<AnyCameraPinhole>(pinholeCamera), yFrame, imagePoints, imagePointDescriptors, 20u, worker))
	{
		return false;
	}

	if (imagePoints.size() < 10)
	{
		return false;
	}

	const Quaternion previousCamera_R_camera_OrIdentity = previousCamera_R_camera.isValid() ? previousCamera_R_camera : Quaternion(true);

	Triangles2 projectedTriangles;

	for (PatternMap::const_iterator i = patternMap_.begin(); i != patternMap_.end(); ++i)
	{
		if (i->second.previousPose().isValid())
		{
			const Triangles2 triangles = i->second.triangles2(pinholeCamera, i->second.previousPose() * previousCamera_R_camera_OrIdentity);
			projectedTriangles.insert(projectedTriangles.end(), triangles.begin(), triangles.end());
		}
	}

	ocean_assert(imagePoints.size() == imagePointDescriptors.size());

	// remove all feature points lying inside a pattern
	if (!projectedTriangles.empty())
	{
		const CV::SubRegion subRegion(projectedTriangles);

		for (size_t n = 0; n < imagePoints.size(); ++n)
		{
			if (subRegion.isInside(imagePoints[n]))
			{
				imagePoints[n] = imagePoints.back();
				imagePoints.pop_back();

				imagePointDescriptors[n] = imagePointDescriptors.back();
				imagePointDescriptors.pop_back();
			}
		}
	}

	Vectors2 strongHarrisCorners;

	// Run detection in a round-robin manner.
	// If detection time exceeds 'options_.maxRecognitionTime', then we'll stop recognition early.

	PatternMap::iterator iPattern = patternMap_.upper_bound(lastRecognitionPatternId_);

	if (iPattern == patternMap_.end())
	{
		iPattern = patternMap_.begin();
	}

	ocean_assert(iPattern != patternMap_.end());

	Vectors2 guessImagePoints;
	Descriptors guessImagePointDescriptors;

	Vectors2 subsetImagePoints;
	Vectors3 subsetObjectPoints;

	for (size_t index = 0; index < patternMap_.size(); ++index, ++iPattern)
	{
		if (index > 0 && recognitionStartTimestamp.hasTimePassed(options_.maxRecognitionTime_))
		{
			return true;
		}

		if (iPattern == patternMap_.end())
		{
			iPattern = patternMap_.begin();
		}

		lastRecognitionPatternId_ = iPattern->first;

		if (iPattern->second.previousPose().isValid())
		{
			continue;
		}

		Pattern& pattern = iPattern->second;

		HomogenousMatrix4 poseGuess(false);
		if (pattern.hasPoseGuess(poseGuess, 0.05))
		{
			poseGuess *= previousCamera_R_camera_OrIdentity;
		}

		const Vectors2* imagePointCandidates = &imagePoints;
		const Descriptors* imagePointDescriptorCandidates = &imagePointDescriptors;

		if (poseGuess.isValid())
		{
			// in case we have a rough pose, we explicitly select all image features which are visible in the projected area of the pattern when applying the rough pose

			guessImagePoints.clear();
			guessImagePointDescriptors.clear();

			guessImagePoints.reserve(imagePoints.size());
			guessImagePointDescriptors.reserve(imagePoints.size());

			const Triangles2 triangles(pattern.triangles2(pinholeCamera, poseGuess));

			const CV::SubRegion subRegion(triangles2subRegion(triangles, pinholeCamera.width(), pinholeCamera.height()));

			for (size_t n = 0; n < imagePoints.size(); ++n)
			{
				if (subRegion.isInside(imagePoints[n]))
				{
					guessImagePoints.push_back(imagePoints[n]);
					guessImagePointDescriptors.push_back(imagePointDescriptors[n]);
				}
			}

			imagePointCandidates = &guessImagePoints;
			imagePointDescriptorCandidates = &guessImagePointDescriptors;
		}

		// we apply a brute-force feature matching to determine candidates

		UnidirectionalCorrespondences::CorrespondencePairs correspondenceCandidates = UnidirectionalCorrespondences::determineCorrespondingDescriptors<Descriptor, unsigned int, determineDescriptorDistance>(imagePointDescriptorCandidates->data(), imagePointDescriptorCandidates->size(), pattern.featureMap().descriptors().data(), pattern.featureMap().descriptors().size(), maximalDescriptorDistance_, worker);

		if (correspondenceCandidates.size() < 12)
		{
			continue;
		}

		subsetImagePoints.clear();
		subsetObjectPoints.clear();
		UnidirectionalCorrespondences::extractCorrespondenceElements(correspondenceCandidates, imagePointCandidates->data(), imagePointCandidates->size(), pattern.featureMap().objectPoints().data(), pattern.featureMap().objectPoints().size(), subsetImagePoints, subsetObjectPoints);
		ocean_assert(subsetImagePoints.size() == subsetObjectPoints.size());

		HomogenousMatrix4 pattern_T_camera;
			Indices32 toto;
		if (!Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(subsetObjectPoints), ConstArrayAccessor<Vector2>(subsetImagePoints), randomGenerator_, pattern_T_camera, 10u, true, options_.recognitionRansacIterations_, Scalar(5 * 5), &toto))
		{
			continue;
		}

		// let's apply another iteration of feature matching, now guided with the known pose - this will increase the number of feature correspondences significantly

		correspondenceCandidates = UnidirectionalCorrespondences::determineCorrespondingFeatures<Descriptor, unsigned int, determineDescriptorDistance>(AnyCameraPinhole(pinholeCamera), pattern_T_camera, pattern.featureMap().objectPoints().data(), pattern.featureMap().descriptors().data(), pattern.featureMap().objectPoints().size(), imagePointCandidates->data(), imagePointDescriptorCandidates->data(), imagePointCandidates->size(), maximalDescriptorDistance_, Scalar(10));

		subsetImagePoints.clear();
		subsetObjectPoints.clear();
		UnidirectionalCorrespondences::extractCorrespondenceElements(correspondenceCandidates, imagePointCandidates->data(), imagePointCandidates->size(), pattern.featureMap().objectPoints().data(), pattern.featureMap().objectPoints().size(), subsetImagePoints, subsetObjectPoints);
		ocean_assert(subsetImagePoints.size() == subsetObjectPoints.size());

		Indices32 resultingValidCorrespondences;
		if (!Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(subsetObjectPoints), ConstArrayAccessor<Vector2>(subsetImagePoints), randomGenerator_, pattern_T_camera, 10u, true, options_.recognitionRansacIterations_, Scalar(3.5 * 3.5), &resultingValidCorrespondences))
		{
			continue;
		}

		if (resultingValidCorrespondences.size() < 30)
		{
			continue;
		}

		ocean_assert(pattern_T_camera.isValid());
		pattern.previousPose() = pattern_T_camera;

		Geometry::SpatialDistribution::OccupancyArray occupancyArray;
		optimizePoseByRectification(pinholeCamera, currentFramePyramid, HomogenousMatrix4(pattern.previousPose()), pattern, pattern.previousPose(), worker, &occupancyArray);

		if (occupancyArray)
		{
			const Triangles2 triangles(pattern.triangles2(pinholeCamera));
			const CV::SubRegion subRegion(triangles2subRegion(triangles, pinholeCamera.width(), pinholeCamera.height()));

			ocean_assert(pattern.previousPose().isValid());

			if (strongHarrisCorners.empty())
			{
				strongHarrisCorners = CV::Detector::FeatureDetector::determineHarrisPoints(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), CV::SubRegion(), 0u, 0u, 15u, worker);
			}

			Vectors2 validPoints;
			validPoints.reserve(strongHarrisCorners.size() / 2);

			for (size_t n = 0; n < strongHarrisCorners.size(); ++n)
			{
				if (occupancyArray(strongHarrisCorners[n]) && subRegion.isInside(strongHarrisCorners[n]))
				{
					validPoints.push_back(strongHarrisCorners[n]);
				}
			}

			if (validPoints.empty())
			{
				continue;
			}

			pattern.imagePoints() = Geometry::SpatialDistribution::distributeAndFilter(validPoints.data(), validPoints.size(), subRegion.boundingBox().left(), subRegion.boundingBox().top(), subRegion.boundingBox().width(), subRegion.boundingBox().height(), 15u, 15u);

			pattern.objectPoints() = Geometry::Utilities::backProjectImagePoints(pinholeCamera, pattern.previousPose(), Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0)), pattern.imagePoints().data(), pattern.imagePoints().size(), pinholeCamera.hasDistortionParameters());

			if (patternMap_.size() >= 2)
			{
				// now we remove all features lying in the current subset

				for (size_t n = 0; n < imagePoints.size(); ++n)
				{
					if (subRegion.isInside(imagePoints[n]))
					{
						imagePoints[n] = imagePoints.back();
						imagePointDescriptors[n] = imagePointDescriptors.back();

						imagePoints.pop_back();
						imagePointDescriptors.pop_back();
					}
				}
			}
		}

		if (internalNumberVisiblePattern() >= internalMaxConcurrentlyVisiblePattern())
		{
			return true;
		}
	}

	return true;
}

CV::SubRegion PatternTrackerCore6DOF::triangles2subRegion(const Triangles2& triangles, const unsigned int backupWidth, const unsigned int backupHeight)
{
	ocean_assert(!triangles.empty());
	ocean_assert(backupWidth > 0u && backupHeight > 0u);

	// we may get an (or more) invalid triangle due to numerical precision issues (e.g., for 32 bit floating point values)
	// in this case we simply take the entire camera frame as sub-region

	for (size_t n = 0; n < triangles.size(); ++n)
	{
		if (!triangles[n].isValid())
		{
			return CV::SubRegion(Box2(0, 0, Scalar(backupWidth), Scalar(backupHeight)));
		}
	}

	return CV::SubRegion(triangles);
}

bool PatternTrackerCore6DOF::detectAndDescribeFeatures(const SharedAnyCamera& camera, const Frame& yFrame, Vectors2& imagePoints, Descriptors& imagePointDescriptors, const unsigned int harrisCornerThreshold, Worker* worker)
{
	ocean_assert(camera && yFrame.isValid());
	ocean_assert(camera->width() == yFrame.width() && camera->height() == yFrame.height());
	ocean_assert(yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	const unsigned int maxFrameArea = yFrame.pixels();
	const unsigned int minFrameArea = std::max(40u * 40u, maxFrameArea / 64u);

	constexpr unsigned int expectedHarrisCorners640x480 = 1000u;
	constexpr Scalar harrisCornersReductionScale = Scalar(0.4);

	const float inverseFocalLength = float(camera->inverseFocalLengthX());

	const CV::Detector::FREAKDescriptor32::AnyCameraDerivativeFunctor cameraFunctor(camera, 8u); // **TODO** avoid fixed layer number

	CV::Detector::HarrisCorners harrisCorners;
	Indices32 cornerPyramidLevels;

	constexpr bool removeInvalid = true;
	constexpr Scalar border = Scalar(20);
	constexpr bool determineExactHarrisCornerPositions = true;
	const bool yFrameIsUndistorted = false;

	imagePointDescriptors.clear();

	if (!CV::Detector::FREAKDescriptor32::extractHarrisCornersAndComputeDescriptors(yFrame, maxFrameArea, minFrameArea, expectedHarrisCorners640x480, harrisCornersReductionScale, harrisCornerThreshold, inverseFocalLength, cameraFunctor, harrisCorners, cornerPyramidLevels, imagePointDescriptors, removeInvalid, border, determineExactHarrisCornerPositions, yFrameIsUndistorted, worker))
	{
		return false;
	}

	ocean_assert(harrisCorners.size() == imagePointDescriptors.size());
	ocean_assert(harrisCorners.size() == cornerPyramidLevels.size());

	imagePoints.clear();
	imagePoints.reserve(harrisCorners.size());

	for (size_t nFeature = 0; nFeature < harrisCorners.size(); ++nFeature)
	{
		CV::Detector::HarrisCorner& harrisCorner = harrisCorners[nFeature];

		const Scalar levelFactor = Scalar(1u << cornerPyramidLevels[nFeature]);

		const Vector2 imagePoint = harrisCorner.observation() * levelFactor;
		ocean_assert(camera->isInside(imagePoint));

		imagePoints.push_back(imagePoint);
	}

	return true;
}

}

}

}
