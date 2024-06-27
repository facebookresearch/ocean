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

#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"
#include "ocean/cv/detector/blob/BlobFeatureDetector.h"

#include "ocean/geometry/Error.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/SuccessionSubset.h"

#include "ocean/media/Utilities.h"

#include "ocean/tracking/blob/BlobTracker6DOF.h"
#include "ocean/tracking/blob/UnidirectionalCorrespondences.h"

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

PatternTrackerCore6DOF::Pattern::Pattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, Worker* worker) :
	patternFeatureMap(yFrame, width, height, yFramePaddingElements, Vector2(dimension.x(), dimension.y()), Scalar(6.5), true, 0u, worker),
	patternPyramid(yFrame, width, height, 1u, FrameType::ORIGIN_UPPER_LEFT, CV::FramePyramid::idealLayers(width, height, 15u, 15u), yFramePaddingElements, true /*copyFirstLayer*/, worker),
	patternDimension(dimension),
	patternPreviousPose(false),
	patternPoseGuess(false)
{
	ocean_assert(patternDimension.x() > 0 && patternDimension.y() > 0);

	patternPyramidReferencePoints.resize(patternPyramid.layers());

	for (unsigned int n = 0; n < patternPyramid.layers(); ++n)
	{
		patternPyramidReferencePoints[n] = CV::Detector::FeatureDetector::determineHarrisPoints(patternPyramid[n].constdata<uint8_t>(), patternPyramid[n].width(), patternPyramid[n].height(), patternPyramid[n].paddingElements(), CV::SubRegion(), 0u, 0u, 25u, worker);
	}
}

PatternTrackerCore6DOF::Pattern::Pattern(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, CV::Detector::Blob::BlobFeatures&& representativeFeatures, const Vector2& dimension, Worker* worker) :
	patternFeatureMap(Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_USE_KEEP_LAYOUT, yFramePaddingElements), Vector2(dimension.x(), dimension.y()), Scalar(6.5), true, 0u, worker),
	patternRepresentativeFeatures(std::move(representativeFeatures)),
	patternPyramid(yFrame, width, height, 1u, FrameType::ORIGIN_UPPER_LEFT, CV::FramePyramid::idealLayers(width, height, 15u, 15u), yFramePaddingElements, true /*copyFirstLayer*/, worker),
	patternDimension(dimension),
	patternPreviousPose(false),
	patternPoseGuess(false)
{
	ocean_assert(patternDimension.x() > 0 && patternDimension.y() > 0);
	ocean_assert(patternRepresentativeFeatures.size() >= 7);

	patternPyramidReferencePoints.resize(patternPyramid.layers());

	for (unsigned int n = 0; n < patternPyramid.layers(); ++n)
	{
		patternPyramidReferencePoints[n] = CV::Detector::FeatureDetector::determineHarrisPoints(patternPyramid[n].constdata<uint8_t>(), patternPyramid[n].width(), patternPyramid[n].height(), patternPyramid[n].paddingElements(), CV::SubRegion(), 0u, 0u, 25u, worker);
	}
}

bool PatternTrackerCore6DOF::Pattern::recognizePattern(const PinholeCamera& pinholeCamera, const CV::Detector::Blob::BlobFeatures& features, RandomGenerator& randomGenerator, HomogenousMatrix4& recognitionPose, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(features.size() >= 7);

	// we take the representative feature points if existing, otherwise we have to take the entire feature map of this pattern
	const CV::Detector::Blob::BlobFeatures& patternFeatures = patternRepresentativeFeatures.size() >= 7 ? patternRepresentativeFeatures : patternFeatureMap.features();

	const Blob::Correspondences::CorrespondencePairs correspondenceCandidates(Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(features, patternFeatures, features.size(), Scalar(0.1), Scalar(0.9), worker));

	if (correspondenceCandidates.size() < 7)
	{
		return false;
	}

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	Blob::UnidirectionalCorrespondences::extractCorrespondingPoints(features, patternFeatures, correspondenceCandidates, imagePoints, objectPoints);

	const unsigned int ransacIterations = imagePoints.size() <= 50 ? 30u : 50u;

	Indices32 usedIndices;
	if (!Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, recognitionPose, 5u, true, ransacIterations, Scalar(15 * 15), &usedIndices))
	{
		return false;
	}

	if (usedIndices.size() < 7)
	{
		return false;
	}

	return true;
}

void PatternTrackerCore6DOF::Pattern::reset()
{
	patternPreviousPose.toNull();
	patternObjectPoints.clear();
	patternImagePoints.clear();
	patternPoseGuess.toNull();
	patternPoseGuessTimestamp.toInvalid();
}

PatternTrackerCore6DOF::PatternTrackerCore6DOF(const Options& options) :
	options_(options),
	world_R_previousCamera(false),
	trackerPatternMapIdCounter(0u),
	trackerTimestampPreviousFrame(false),
	lastRecognitionAttemptTimestamp_(false),
	trackerLastRecognitionPatternId(0u)
{
	// If any overriding values are provided, use them and set any missing values.
	if (options_.maxRecognitionTime <= 0.0)
	{
#ifdef OCEAN_DEBUG
		options_.maxRecognitionTime = 0.5;
#else
		options_.maxRecognitionTime = 0.075;
#endif
	}

	if (options_.recognitionCadenceWithTrackedPatterns <= 0.0)
	{
		options_.recognitionCadenceWithTrackedPatterns = 0.5;
	}

	options_.recognitionCadenceWithoutTrackedPatterns = std::max(options_.recognitionCadenceWithoutTrackedPatterns, 0.0);

	ocean_assert(options_.maxRecognitionTime > 0.0);
	ocean_assert(options_.recognitionCadenceWithTrackedPatterns >= 0.0);
	ocean_assert(options_.recognitionCadenceWithoutTrackedPatterns >= 0.0);
	ocean_assert(options_.recognitionRansacIterations > 0);
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

	const ScopedLock scopedLock(trackerLock);

	ocean_assert(trackerPatternMap.find(trackerPatternMapIdCounter) == trackerPatternMap.end());

	Vector2 patternDimension(dimension);
	if (patternDimension.y() <= 0)
	{
		patternDimension.y() = patternDimension.x() * Scalar(height) / Scalar(width);
	}

	const unsigned int patternId = trackerPatternMapIdCounter++;
	trackerPatternMap[patternId] = Pattern(yFrame, width, height, yFramePaddingElements, patternDimension, worker);

	trackerLastRecognitionPatternId = patternId;

	return patternId;
}

unsigned int PatternTrackerCore6DOF::addPattern(const std::string& filename, const Vector2& dimension, Worker* worker)
{
	if (filename.empty() || dimension.x() <= 0)
	{
		return (unsigned int)(-1);
	}

	const std::string fileExtension = filename.substr(filename.rfind('.'));

	if (String::toLower(fileExtension) != ".opfm")
	{
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

	Frame patternFrame;
	CV::Detector::Blob::BlobFeatures representativeFeatures;

	if (!readFeatureMap(filename, patternFrame, representativeFeatures))
	{
		return false;
	}

	ocean_assert(patternFrame.isValid());

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(patternFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return (unsigned int)(-1);
	}

	// we need to adjust the positions of the representative features so that they match with the provided dimension
	// as currently, the points are defined in the pixel domain of the pattern

	ocean_assert(yFrame.isValid());

	Vector2 patternDimension(dimension);
	if (patternDimension.y() <= 0)
	{
		patternDimension.y() = patternDimension.x() * Scalar(patternFrame.height()) / Scalar(patternFrame.width());
	}

	const Scalar xFactor = patternDimension.x() / Scalar(yFrame.width());
	const Scalar zFactor = patternDimension.y() / Scalar(yFrame.height());

	for (CV::Detector::Blob::BlobFeatures::iterator i = representativeFeatures.begin(); i != representativeFeatures.end(); ++i)
	{
		ocean_assert(Numeric::isEqualEps(i->position().y()));
		i->setPosition(Vector3(i->position().x() * xFactor, i->position().y(), i->position().z() * zFactor));
	}

	const ScopedLock scopedLock(trackerLock);

	ocean_assert(trackerPatternMap.find(trackerPatternMapIdCounter) == trackerPatternMap.end());

	const unsigned int patternId = trackerPatternMapIdCounter++;

	if (representativeFeatures.empty())
	{
		trackerPatternMap[patternId] = Pattern(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), patternDimension, worker);
	}
	else
	{
		trackerPatternMap[patternId] = Pattern(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), std::move(representativeFeatures), patternDimension, worker);
	}

	trackerLastRecognitionPatternId = patternId;

	return patternId;
}

bool PatternTrackerCore6DOF::removePattern(const unsigned int patternId)
{
	ocean_assert(patternId != (unsigned int)(-1));

	const ScopedLock scopedLock(trackerLock);

	if (patternId >= trackerPatternMapIdCounter)
	{
		ocean_assert(false && "This id is invalid and has never been used in this tracker!");
		return false;
	}

	ocean_assert(trackerPatternMap.find(patternId) != trackerPatternMap.end());

	return trackerPatternMap.erase(patternId) == 1;
}

bool PatternTrackerCore6DOF::removePatterns()
{
	const ScopedLock scopedLock(trackerLock);

	trackerPatternMap.clear();

	return true;
}

bool PatternTrackerCore6DOF::determinePoses(const bool allowRecognition, const uint8_t* yFrame, const PinholeCamera& pinholeCamera, const unsigned yFramePaddingElements, const bool frameIsUndistorted, const Timestamp& timestamp, VisualTracker::TransformationSamples& transformations, const Quaternion& world_R_camera, Worker* worker)
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

	const ScopedLock scopedLock(trackerLock);

	if (trackerPatternMap.empty())
	{
		return false;
	}

	if (options_.noFrameToFrameTracking)
	{
		trackerTimestampPreviousFrame.toInvalid();

		for (PatternMap::iterator i = trackerPatternMap.begin(); i != trackerPatternMap.end(); ++i)
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
	if (world_R_previousCamera.isValid() && world_R_camera.isValid())
	{
		previousCamera_R_camera = world_R_previousCamera.inverted() * world_R_camera;
	}

	ocean_assert(timestamp.isValid());

	const Frame frame(FrameType(pinholeCamera.width(), pinholeCamera.height(), FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_USE_KEEP_LAYOUT, yFramePaddingElements, timestamp);

	if (determinePoses(allowRecognition, frame, pinholeCamera, previousCamera_R_camera, worker))
	{
		for (PatternMap::iterator i = trackerPatternMap.begin(); i != trackerPatternMap.end(); ++i)
		{
			if (i->second.previousPose().isValid()) // previousPose as we are done with the current frame already
			{
				transformations.push_back(VisualTracker::TransformationSample(i->second.previousPose(), i->first));
			}
		}
	}

	world_R_previousCamera = world_R_camera;

	trackerTimestampPreviousFrame = timestamp;

	return true;
}

void PatternTrackerCore6DOF::reset()
{
	const ScopedLock scopedLock(trackerLock);

	trackerCurrentFramePyramid.clear();
	trackerPreviousFramePyramid.clear();

	for (PatternMap::iterator i = trackerPatternMap.begin(); i != trackerPatternMap.end(); ++i)
	{
		i->second.reset();
	}

	world_R_previousCamera = Quaternion(false);

	trackerTimestampPreviousFrame.toInvalid();

	lastRecognitionAttemptTimestamp_.toInvalid();

	// trackerLastRecognitionPatternId, we keep the value
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

bool PatternTrackerCore6DOF::writeFeatureMap(const std::string& filename, const Frame& pattern, const CV::Detector::Blob::BlobFeatures& representativeFeatures)
{
	ocean_assert(!filename.empty() && pattern);

	Media::Utilities::Buffer patternBuffer;
	if (!Media::Utilities::saveImage(pattern, "png", patternBuffer) || patternBuffer.empty())
	{
		return false;
	}

	std::ofstream stream;
	stream.open(filename, std::ios::binary);
	if (!stream.good())
	{
		return false;
	}

	IO::OutputBitstream bitstream(stream);

	// write the tag for identification
	if (!IO::Tag::writeTag(bitstream, Blob::BlobTracker6DOF::trackerTagFeatureMap()))
	{
		return false;
	}

	// write version 1
	if (!bitstream.write<unsigned long long>(1ull))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>((unsigned int)patternBuffer.size()))
	{
		return false;
	}

	if (!bitstream.write(patternBuffer.data(), patternBuffer.size()))
	{
		return false;
	}

	if (!Blob::BlobTracker6DOF::writeFeatures(representativeFeatures, bitstream))
	{
		return false;
	}

	return true;
}

bool PatternTrackerCore6DOF::readFeatureMap(const std::string& filename, Frame& pattern, CV::Detector::Blob::BlobFeatures& representativeFeatures)
{
	ocean_assert(!filename.empty());
	ocean_assert(!pattern.isValid() && representativeFeatures.empty());

	std::ifstream stream;
	stream.open(filename, std::ios::binary);
	if (!stream.good())
	{
		return false;
	}

	IO::InputBitstream bitstream(stream);

	// check for the correct tag
	if (!IO::Tag::readAndCheckTag(bitstream, Blob::BlobTracker6DOF::trackerTagFeatureMap()))
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version))
	{
		return false;
	}

	if (version != 1ull)
	{
		return false;
	}

	unsigned int patternBufferSize = 0u;
	if (!bitstream.read<unsigned int>(patternBufferSize) || patternBufferSize > 32u * 1024u * 1024u)
	{
		return false;
	}

	Media::Utilities::Buffer patternBuffer(patternBufferSize);

	if (!bitstream.read(patternBuffer.data(), patternBuffer.size()))
	{
		return false;
	}

	pattern = Media::Utilities::loadImage(patternBuffer.data(), patternBuffer.size());

	if (!pattern.isValid())
	{
		return false;
	}

	if (!Blob::BlobTracker6DOF::readFeatures(bitstream, representativeFeatures))
	{
		return false;
	}

	return true;
}

bool PatternTrackerCore6DOF::determinePoses(const bool allowRecognition, const Frame& yFrame, const PinholeCamera& pinholeCamera, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	ocean_assert(yFrame && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(yFrame.width() == pinholeCamera.width() && yFrame.height() == pinholeCamera.height());

	// we ensure that we do not handle too much pixel information, therefore we may use a downsampled image for tracking

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

	if (options_.downsampleInputImageOnAndroid)
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

	const unsigned int pyramidLayers = trackerPreviousFramePyramid.isValid() ? trackerPreviousFramePyramid.layers() : CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), 15u, 15u);

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	trackerCurrentFramePyramid.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.channels(), yFrame.pixelOrigin(), pyramidLayers,  yFrame.paddingElements(), true /*copyFirstLayer*/, worker, yFrame.pixelFormat(), yFrame.timestamp());

	for (PatternMap::iterator i = trackerPatternMap.begin(); i != trackerPatternMap.end(); ++i)
	{
		Pattern& pattern = i->second;

		// check whether we have 3D object points (and corresponding 2D image points) from the previous frame so that we can find the new image points in the current frame
		if (pattern.objectPoints().size() > 15)
		{
			ocean_assert(pattern.previousPose().isValid());

			// try to determine the current pose by application of the previous correspondences
			if (!determinePoseWithPreviousCorrespondences(pinholeCamera, trackerPreviousFramePyramid, trackerCurrentFramePyramid, pattern, previousCamera_R_camera, worker))
			{
				pattern.objectPoints().clear();
				pattern.imagePoints().clear();
				pattern.previousPose().toNull();
			}

			// check whether we should try to add new correspondences / reset the old ones - this is better than an entire brute-force retracking
			if (pattern.previousPose().isValid() && pattern.objectPoints().size() <= 20)
			{
				Geometry::SpatialDistribution::OccupancyArray occupancyArray;
				optimizePoseByRectification(pinholeCamera, trackerCurrentFramePyramid, HomogenousMatrix4(pattern.previousPose()), pattern, pattern.previousPose(), worker, &occupancyArray);

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

					if (pattern.isPlanar())
					{
						pattern.objectPoints() = Geometry::Utilities::backProjectImagePoints(pinholeCamera, pattern.previousPose(), Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0)), pattern.imagePoints().data(), pattern.imagePoints().size(), pinholeCamera.hasDistortionParameters());
					}
					else
					{
						ocean_assert(false && "General pattern types are not currently supported!");
					}
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

	// For patterns that lost tracking since the last frame, attempt to relocalize with a more robust procedure.
	if (trackerTimestampPreviousFrame.isValid() && (currentlyVisiblePattern == 0u || currentlyVisiblePattern < internalMaxConcurrentlyVisiblePattern()))
	{
		for (PatternMap::iterator iP = trackerPatternMap.begin(); iP != trackerPatternMap.end(); ++iP)
		{
			Pattern& pattern = iP->second;

			if (pattern.previousPose().isValid() == false)
			{
				Timestamp poseGuessTimestamp;
				const HomogenousMatrix4& poseGuess = pattern.poseGuess(&poseGuessTimestamp);

				if (poseGuess.isValid() && poseGuessTimestamp == trackerTimestampPreviousFrame)
				{
					determinePoseWithDriftErrors(pinholeCamera, trackerPreviousFramePyramid, trackerCurrentFramePyramid, pattern, previousCamera_R_camera, worker);
				}
			}
		}

		currentlyVisiblePattern = internalNumberVisiblePattern();
	}

	// Attempt to track new patterns.
	if (allowRecognition && (currentlyVisiblePattern == 0u || currentlyVisiblePattern < internalMaxConcurrentlyVisiblePattern()) && (!lastRecognitionAttemptTimestamp_.isValid() || yFrame.timestamp() > lastRecognitionAttemptTimestamp_ + maximumDurationBetweenRecognitionAttempts()))
	{
		determinePosesWithoutKnowledge(pinholeCamera, yFrame, trackerCurrentFramePyramid, previousCamera_R_camera, worker);
		lastRecognitionAttemptTimestamp_ = yFrame.timestamp();
	}

	// we use the current frame pyramid as previous frame pyramid in the next tracking iteration
	std::swap(trackerPreviousFramePyramid, trackerCurrentFramePyramid);

	// at this moment the previous pose is also the pose for the current frame
	for (PatternMap::iterator i = trackerPatternMap.begin(); i != trackerPatternMap.end(); ++i)
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

			Vectors3 objectPoints;
			if (pattern.isPlanar())
			{
				objectPoints = Geometry::Utilities::backProjectImagePoints(hierarchyCamera, poseGuess, Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0)), previousFeaturePoints.data(), previousFeaturePoints.size(), hierarchyCamera.hasDistortionParameters());
			}
			else
			{
				ocean_assert(false && "General pattern types are not currently supported!");
			}

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

	ocean_assert(pattern.isPlanar());

	const HomogenousMatrix4 roughPoseIF(PinholeCamera::standard2InvertedFlipped(roughPose));

	// Find the pattern layer best matching with the size of the rectified image.
	unsigned int patternPyramidLayer = static_cast<unsigned int>(-1);

	if (pattern.isPlanar())
	{
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
	}
	else
	{
		return false; // not supported!
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

	if (pattern.isPlanar())
	{
		const Frame& pyramidLayer = currentFramePyramid.finestLayer();

		// Perform a planar rectification with a lookup table.
		CV::Advanced::FrameRectification::planarRectangleObjectMaskIF8BitPerChannel<1u>(pyramidLayer.constdata<uint8_t>(), pyramidLayer.paddingElements(), pyramidLayer.pixelOrigin(), AnyCameraPinhole(pinholeCamera), roughPoseIF, pattern.corner0(), pattern.corner3(), pattern.corner1(), rectifiedFrame.data<uint8_t>(), rectifiedFrameMask.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), rectifiedFrameMask.paddingElements(), worker, 0xFF, 50u);
	}

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
		if (pattern.isPlanar())
		{
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
					if (pattern.isPlanar())
					{
						for (ImagePoints::const_iterator i = invalidPatternPoints.begin(); i != invalidPatternPoints.end(); ++i)
						{
							tmpArray.removePoint(CV::Advanced::FrameRectification::planarRectangleObjectRectifiedPosition2cameraPositionIF<true>(AnyCameraPinhole(pinholeCamera), optimizedPoseIF, pattern.corner0(), pattern.corner3(), pattern.corner1(), rectifiedFrame.width(), rectifiedFrame.height(), *i));
						}
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

	const unsigned int* integralImage = createIntegralImage(yFrame);
	ocean_assert(integralImage);

	// although we may have downsampled the original input image,
	// we still have the simple possibility to selected the sampling density for the Blob detector
	// Thus, as we do not downsample 1280x720 input images, we simply reduce the sampling density instead;
	// (for images having more pixels than 640x480)
	const CV::Detector::Blob::BlobFeatureDetector::SamplingDense samplingDense = yFrame.pixels() > 640u * 480u ? CV::Detector::Blob::BlobFeatureDetector::SAMPLING_SPARSE : CV::Detector::Blob::BlobFeatureDetector::SAMPLING_NORMAL;

	CV::Detector::Blob::BlobFeatures features;
	CV::Detector::Blob::BlobFeatureDetector::detectFeatures(integralImage, yFrame.width(), yFrame.height(), samplingDense, 10, true, features, worker);

	if (features.size() < 10)
	{
		return false;
	}

	const Quaternion previousCamera_R_camera_OrIdentity = previousCamera_R_camera.isValid() ? previousCamera_R_camera : Quaternion(true);

	Triangles2 projectedTriangles;

	for (PatternMap::const_iterator i = trackerPatternMap.begin(); i != trackerPatternMap.end(); ++i)
	{
		if (i->second.previousPose().isValid())
		{
			const Triangles2 triangles = i->second.triangles2(pinholeCamera, i->second.previousPose() * previousCamera_R_camera_OrIdentity);
			projectedTriangles.insert(projectedTriangles.end(), triangles.begin(), triangles.end());
		}
	}

	// remove all feature points lying inside a pattern
	if (!projectedTriangles.empty())
	{
		const CV::SubRegion subRegion(projectedTriangles);

		CV::Detector::Blob::BlobFeatures subsetFeatures;
		subsetFeatures.reserve(features.size());

		for (CV::Detector::Blob::BlobFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
		{
			if (!subRegion.isInside(i->observation()))
			{
				subsetFeatures.push_back(*i);
			}
		}

		features = std::move(subsetFeatures);
	}

	CV::Detector::Blob::BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImage, yFrame.width(), yFrame.height(), FrameType::ORIGIN_UPPER_LEFT, CV::Detector::Blob::BlobFeature::ORIENTATION_SLIDING_WINDOW, features, (unsigned int)(features.size()), false, worker);

	CV::Detector::Blob::BlobFeatures subsetFeatures;
	Vectors2 strongHarrisCorners;

	// Run detection in a round-robin manner.
	// If detection time exceeds 'options_.maxRecognitionTime', then we'll stop recognition early.

	PatternMap::iterator iPattern = trackerPatternMap.upper_bound(trackerLastRecognitionPatternId);

	if (iPattern == trackerPatternMap.end())
	{
		iPattern = trackerPatternMap.begin();
	}

	ocean_assert(iPattern != trackerPatternMap.end());

	for (size_t index = 0; index < trackerPatternMap.size(); ++index, ++iPattern)
	{
		if (double(Timestamp(true) - recognitionStartTimestamp) > options_.maxRecognitionTime)
		{
			return true;
		}

		if (iPattern == trackerPatternMap.end())
		{
			iPattern = trackerPatternMap.begin();
		}

		trackerLastRecognitionPatternId = iPattern->first;

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

		if (!poseGuess.isValid() && pattern.hasRepresentativeFeatures())
		{
			if (!pattern.recognizePattern(pinholeCamera, features, trackerRandomGenerator, poseGuess, worker))
			{
				continue;
			}
		}

		CV::Detector::Blob::BlobFeatures* featureCandidates = &features;

		if (poseGuess.isValid())
		{
			subsetFeatures.clear();
			subsetFeatures.reserve(features.size());

			const Triangles2 triangles(pattern.triangles2(pinholeCamera, poseGuess));

			const CV::SubRegion subRegion(triangles2subRegion(triangles, pinholeCamera.width(), pinholeCamera.height()));

			for (CV::Detector::Blob::BlobFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
			{
				if (subRegion.isInside(i->observation()))
				{
					subsetFeatures.push_back(*i);
				}
			}

			featureCandidates = &subsetFeatures;
		}

		// we apply a brute-force feature matching to determine candidates

		Blob::Correspondences::CorrespondencePairs correspondenceCandidates(Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithQualityEarlyReject(*featureCandidates, pattern.featureMap().features(), featureCandidates->size(), Scalar(0.1), Scalar(0.7), worker));

		if (correspondenceCandidates.size() < 12)
		{
			continue;
		}

		Geometry::ImagePoints imagePoints;
		Geometry::ObjectPoints objectPoints;
		Blob::Correspondences::extractCorrespondingPoints(*featureCandidates, pattern.featureMap().features(), correspondenceCandidates, imagePoints, objectPoints);
		ocean_assert(objectPoints.size() == imagePoints.size());

		HomogenousMatrix4 pose;

		if (!Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), trackerRandomGenerator, pose, 10u, true, options_.recognitionRansacIterations, Scalar(5 * 5)))
		{
			continue;
		}

		// let's apply another iteration of feature matching, now guided with the known pose - this will increase the number of feature correspondences significantly

		correspondenceCandidates = Blob::UnidirectionalCorrespondences::determineFeatureCorrespondencesWithPose(AnyCameraPinhole(pinholeCamera), pose, *featureCandidates, pattern.featureMap().features(), featureCandidates->size(), Scalar(10), Scalar(0.1), Scalar(0.7));

		imagePoints.clear();
		objectPoints.clear();
		Blob::Correspondences::extractCorrespondingPoints(*featureCandidates, pattern.featureMap().features(), correspondenceCandidates, imagePoints, objectPoints);
		ocean_assert(objectPoints.size() == imagePoints.size());

		Indices32 resultingValidCorrespondences;
		if (!Geometry::RANSAC::p3p(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), trackerRandomGenerator, pose, 10u, true, options_.recognitionRansacIterations, Scalar(5 * 5), &resultingValidCorrespondences))
		{
			continue;
		}

		if (resultingValidCorrespondences.size() < 10)
		{
			continue;
		}

		ocean_assert(pose.isValid());
		pattern.previousPose() = pose;

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

			if (pattern.isPlanar())
			{
				pattern.objectPoints() = Geometry::Utilities::backProjectImagePoints(pinholeCamera, pattern.previousPose(), Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0)), pattern.imagePoints().data(), pattern.imagePoints().size(), pinholeCamera.hasDistortionParameters());
			}
			else
			{
				ocean_assert(false && "General pattern types are not currently supported!");
			}

			// now we remove all Blob features lying in the current subset

			subsetFeatures.clear();
			subsetFeatures.reserve(features.size());

			for (CV::Detector::Blob::BlobFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
			{
				if (!subRegion.isInside(i->observation()))
				{
					subsetFeatures.push_back(*i);
				}
			}

			features = std::move(subsetFeatures);
		}

		if (internalNumberVisiblePattern() >= internalMaxConcurrentlyVisiblePattern())
		{
			return true;
		}
	}

	return true;
}

const unsigned int* PatternTrackerCore6DOF::createIntegralImage(const Frame& yFrame)
{
	ocean_assert(yFrame && FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	if (!trackerIntegralImage.set(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), /* forceOwner */ true))
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	ocean_assert(trackerIntegralImage);
	ocean_assert(trackerIntegralImage.isContinuous() && "The output of this function is assumed to be continuous.");

	CV::IntegralImage::createLinedImage<uint8_t, unsigned int, 1u>(yFrame.constdata<uint8_t>(), trackerIntegralImage.data<unsigned int>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), trackerIntegralImage.paddingElements());

	return trackerIntegralImage.constdata<unsigned int>();
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

}

}

}
