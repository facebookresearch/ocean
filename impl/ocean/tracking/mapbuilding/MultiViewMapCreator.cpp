/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/MultiViewMapCreator.h"

#include "ocean/base/Utilities.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Box3.h"
#include "ocean/math/PinholeCamera.h"

#ifdef _WINDOWS
	#include "ocean/cv/FrameConverter.h"
	#include "ocean/cv/Canvas.h"

	#include "ocean/platform/win/Utilities.h"
	#include "ocean/tracking/Utilities.h"
#endif

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

MultiViewMapCreator::Feature::Feature(const Vector2& imagePoint, const ObservationPair& observationPair, const CV::Detector::FREAKDescriptor32& descriptor) :
	observations_(1, Observation(imagePoint, observationPair, descriptor))
{
	// nothing to do here
}

MultiViewMapCreator::Feature::Feature(const Vector2& imagePointA, const Vector2& imagePointB, const ObservationPair& observationPairA, const ObservationPair& observationPairB, const CV::Detector::FREAKDescriptor32& descriptorA, const CV::Detector::FREAKDescriptor32& descriptorB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Vector3& objectPoint) :
	Feature(imagePointA, observationPairA, descriptorA)
{
	observations_.emplace_back(imagePointB, observationPairB, descriptorB);

	objectPoint_ = objectPoint;

	ocean_assert(world_T_cameraA.isValid());
	ocean_assert(world_T_cameraB.isValid());

	const Scalar sqrDistanceA = world_T_cameraA.translation().sqrDistance(objectPoint_);
	const Scalar sqrDistanceB = world_T_cameraB.translation().sqrDistance(objectPoint_);

	minimalObservationSqrDistance_ = std::min(sqrDistanceA, sqrDistanceB);
	maximalObservationSqrDistance_ = std::max(sqrDistanceA, sqrDistanceB);

	ocean_assert(minimalObservationSqrDistance_ <= maximalObservationSqrDistance_);

	static_assert(observationThreshold_ > 0 && observationThreshold_ < 1, "Invalid threshold");
	minimalObservationSqrDistance_ *= Numeric::sqr(Scalar(1) - observationThreshold_); // e.g., 0.25 = 75% and 125%
	maximalObservationSqrDistance_ *= Numeric::sqr(Scalar(1) + observationThreshold_);

	sqrBaseline_ = Box3(world_T_cameraA.translation(), world_T_cameraB.translation()).sqrDiagonal();
}

void MultiViewMapCreator::Feature::addObservation(const Vector2& imagePoint, const ObservationPair& observationPair, const CV::Detector::FREAKDescriptor32& descriptor, RandomGenerator& randomGenerator)
{
	static_assert(maxObservations_ > 0, "Invalid parameter");

	while (observations_.size() >= maxObservations_)
	{
		const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(observations_.size() - 1));

		observations_[index] = observations_.back();
		observations_.pop_back();
	}

	++observationIterations_;

	observations_.emplace_back(imagePoint, observationPair, descriptor);

	timeSinceLastObservation_ = 0.0;
}

MultiViewMapCreator::Feature::LocalizationResult MultiViewMapCreator::Feature::localizeObjectPoint(const std::vector<HomogenousMatrices4>& world_T_cameraGroups, const std::vector<SharedAnyCameras>& cameraGroups)
{
	if (observations_.size() < nextLocalizationObservationIterations_)
	{
		return LR_SKIPPED;
	}

	HomogenousMatrices4 world_T_cameras;
	world_T_cameras.reserve(observations_.size());

	SharedAnyCameras cameras;
	cameras.reserve(observations_.size());

	Vectors2 imagePoints;
	imagePoints.reserve(observations_.size());

	Box3 boundingBox;
	for (const Observation& observation : observations_)
	{
		const ObservationPair& observationPair = observation.observationPair_;

		const Index32& frameIndex = observationPair.frameIndex_;
		const Index32& cameraIndex = observationPair.cameraIndex_;

		ocean_assert(frameIndex < world_T_cameraGroups.size());
		ocean_assert(cameraIndex < world_T_cameraGroups[frameIndex].size());

		const HomogenousMatrix4& world_T_camera = world_T_cameraGroups[frameIndex][cameraIndex];

		boundingBox += world_T_camera.translation();

		world_T_cameras.emplace_back(world_T_camera);

		ocean_assert(frameIndex < cameraGroups.size());
		ocean_assert(cameraIndex < cameraGroups[frameIndex].size());
		cameras.emplace_back(cameraGroups[frameIndex][cameraIndex]);

		imagePoints.emplace_back(observation.imagePoint_);
	}

	const Scalar boundingBoxDiagonalSqr = boundingBox.sqrDiagonal();

	if (boundingBoxDiagonalSqr < Numeric::sqr(Scalar(0.1)))
	{
		return LR_SKIPPED;
	}

	RandomGenerator randomGenerator;

	Vector3 objectPoint;
	Indices32 usedIndices;
	if (!Geometry::RANSAC::objectPoint(SharedPointerConstArrayAccessor<AnyCamera>(cameras), ConstArrayAccessor<HomogenousMatrix4>(world_T_cameras), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator, objectPoint, 20u, Scalar(3 * 3), 2u, true, Geometry::Estimator::ET_SQUARE, nullptr, &usedIndices))
	{
		return LR_FAILED;
	}

	if (usedIndices.size() < cameras.size() * 75 / 100)
	{
		return LR_FAILED;
	}

	sqrBaseline_ = boundingBoxDiagonalSqr;

	if (usedIndices.size() != cameras.size())
	{
		observations_ = Subset::subset(observations_, usedIndices);
	}

	minimalObservationSqrDistance_ = Numeric::maxValue();
	maximalObservationSqrDistance_ = Numeric::minValue();

	for (const Index32& usedIndex : usedIndices)
	{
		const Scalar sqrDistance = world_T_cameras[usedIndex].translation().sqrDistance(objectPoint);

		if (sqrDistance < minimalObservationSqrDistance_)
		{
			minimalObservationSqrDistance_ = sqrDistance;
		}

		if (sqrDistance > maximalObservationSqrDistance_)
		{
			maximalObservationSqrDistance_ = sqrDistance;
		}
	}

	ocean_assert(minimalObservationSqrDistance_ <= maximalObservationSqrDistance_);

	minimalObservationSqrDistance_ *= Numeric::sqr(Scalar(1) - observationThreshold_); // e.g., 0.25 = 75% and 125%
	maximalObservationSqrDistance_ *= Numeric::sqr(Scalar(1) + observationThreshold_);

	objectPoint_ = objectPoint;

	if (boundingBoxDiagonalSqr >= Numeric::sqr(2))
	{
		// we have reached a stable state of the feature
		nextLocalizationObservationIterations_ = size_t(-1);
	}
	else
	{
		nextLocalizationObservationIterations_ = nextLocalizationObservationIterations_ * 192 / 128; // = 1.5
	}

	return LR_SUCCEEDED;
}

bool MultiViewMapCreator::Feature::failedObservation(const Scalar sqrDistance, const double secondsPerFrame)
{
	ocean_assert(sqrDistance > 0.0);
	ocean_assert(secondsPerFrame > 0.0);

	if (sqrDistance >= minimalObservationSqrDistance_  && sqrDistance <= maximalObservationSqrDistance_)
	{
		timeSinceLastObservation_ += secondsPerFrame;

		return true;
	}

	return false;
}

void MultiViewMapCreator::Feature::copyObservations(const Feature& feature)
{
	observationIterations_ += feature.observationIterations_;

	observations_.insert(observations_.cend(), feature.observations_.cbegin(), feature.observations_.cend());

	if (feature.isLocalized())
	{
		if (isLocalized())
		{
			objectPoint_ = (objectPoint_ + feature.objectPoint_) * Scalar(0.5);
		}
		else
		{
			objectPoint_ = feature.objectPoint_;
		}
	}

	nextLocalizationObservationIterations_ = observationIterations_;

	timeSinceLastObservation_ = std::min(timeSinceLastObservation_, feature.timeSinceLastObservation_);
}

bool MultiViewMapCreator::processFrame(const Frames& yFrames, const SharedAnyCameras& cameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras)
{
	ocean_assert(!yFrames.empty());
	ocean_assert(yFrames.size() == cameras.size());
	ocean_assert(yFrames.size() == device_T_cameras.size());

	ocean_assert(world_T_device.isValid());

	if (yFrames.empty())
	{
		return false;
	}

	HomogenousMatrices4 world_T_currentCameras;
	world_T_currentCameras.reserve(device_T_cameras.size());

	for (const HomogenousMatrix4& device_T_camera : device_T_cameras)
	{
		world_T_currentCameras.emplace_back(world_T_device * device_T_camera);
	}

	const ScopedLock scopedLock(lock_);

	currentImagePointGroups_.resize(yFrames.size());
	currentDescriptorGroups_.resize(yFrames.size());
	currentCornerPyramidLevelGroups_.resize(yFrames.size());

	std::vector<Geometry::SpatialDistribution::DistributionArray> currentDistributionArrays(yFrames.size());

	determineImageFeatures(yFrames, cameras, currentImagePointGroups_, currentDescriptorGroups_, currentCornerPyramidLevelGroups_, currentDistributionArrays, WorkerPool::get().scopedWorker()());

	const Timestamp timestamp = yFrames.front().timestamp();

#ifdef OCEAN_DEBUG
	for (const Frame& yFrame : yFrames)
	{
		ocean_assert(yFrame.timestamp() == timestamp);
	}
#endif

	const Index32 currentFrameIndex = Index32(cameraGroups_.size());

	// let's first check whether a 2D image point is the observation of a localized 3D feature

	determineObservations(currentFrameIndex, timestamp, world_T_device, world_T_currentCameras, cameras, currentDistributionArrays);

	// let's determine new stereo-based features

	IndexPair32 stereoCameraIndices;
	if (determineLowerStereoCameras(device_T_cameras, stereoCameraIndices))
	{
		const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameras[stereoCameraIndices.first];
		const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameras[stereoCameraIndices.second];

		const Vectors2& imagePointsA = currentImagePointGroups_[stereoCameraIndices.first];
		const Vectors2& imagePointsB = currentImagePointGroups_[stereoCameraIndices.second];

		const CV::Detector::FREAKDescriptors32& descriptorsA = currentDescriptorGroups_[stereoCameraIndices.first];
		const CV::Detector::FREAKDescriptors32& descriptorsB = currentDescriptorGroups_[stereoCameraIndices.second];

		const Indices32& pyramidLevelsA = currentCornerPyramidLevelGroups_[stereoCameraIndices.first];
		const Indices32& pyramidLevelsB = currentCornerPyramidLevelGroups_[stereoCameraIndices.second];

		Vectors3 stereoObjectPoints;
		const IndexPairs32 stereoMatches = matchStereoFeatures(*cameras[stereoCameraIndices.first], *cameras[stereoCameraIndices.second], world_T_cameraA, world_T_cameraB, imagePointsA, imagePointsB, descriptorsA, descriptorsB, pyramidLevelsA, pyramidLevelsB, stereoObjectPoints);

		for (size_t n = 0; n < stereoMatches.size(); ++n)
		{
			const IndexPair32& stereoMatch = stereoMatches[n];
			const Vector3& stereoObjectPoint = stereoObjectPoints[n];

			const Vector2& imagePointA = imagePointsA[stereoMatch.first];
			const Vector2& imagePointB = imagePointsB[stereoMatch.second];

			features_.emplace_back(imagePointA, imagePointB, ObservationPair(currentFrameIndex, stereoCameraIndices.first), ObservationPair(currentFrameIndex, stereoCameraIndices.second), descriptorsA[stereoMatch.first], descriptorsB[stereoMatch.second], world_T_cameraA, world_T_cameraB, stereoObjectPoint);
		}

#ifdef DEBUG_OUTPUT_ON_WINDOWS
#ifdef _WINDOWS
		Frame rgbFrameA;
		CV::FrameConverter::Comfort::convert(yFrames[stereoCameraIndices.first], FrameType::FORMAT_RGB24, rgbFrameA);

		Frame rgbFrameB;
		CV::FrameConverter::Comfort::convert(yFrames[stereoCameraIndices.second], FrameType::FORMAT_RGB24, rgbFrameB);

		Frame rgbFrame(FrameType(rgbFrameA, rgbFrameA.width() * 2u, rgbFrameA.height()));

		rgbFrame.copy(0u, 0u, rgbFrameA);
		rgbFrame.copy(rgbFrameA.width(), 0u, rgbFrameB);

		for (const IndexPair32& stereoMatch : stereoMatches)
		{
			CV::Canvas::line<1u>(rgbFrame, imagePointsA[stereoMatch.first], imagePointsB[stereoMatch.second] + Vector2(640, 0), CV::Canvas::green());
		}

		Platform::Win::Utilities::desktopFrameOutput(0, int(rgbFrame.height()), rgbFrame);
#endif
#endif
	}

	ocean_assert(cameraGroups_.size() == world_T_cameraGroups_.size());

	world_T_cameraGroups_.emplace_back(std::move(world_T_currentCameras));
	cameraGroups_.emplace_back(cameras);

	for (size_t nFeature = 0; nFeature < features_.size(); /*noop*/)
	{
		if (features_[nFeature].localizeObjectPoint(world_T_cameraGroups_, cameraGroups_) == Feature::LR_FAILED)
		{
			features_[nFeature] = std::move(features_.back());
			features_.pop_back();
		}
		else
		{
			++nFeature;
		}
	}

	lastTimestamp_ = timestamp;

	return true;
}


bool MultiViewMapCreator::latestFeatureMap(Vectors3& objectPoints, std::vector<CV::Detector::FREAKDescriptors32>* multiDescriptors, Scalars* stabilityFactors, const size_t minimalObservations, const size_t maximalDescriptorsPerFeaturePoint) const
{
	ocean_assert(minimalObservations >= 2);
	ocean_assert(maximalDescriptorsPerFeaturePoint >= 1);

	const ScopedLock scopedLock(lock_);

	if (features_.empty())
	{
		return false;
	}

	objectPoints.clear();
	objectPoints.reserve(features_.size());

	if (multiDescriptors != nullptr)
	{
		multiDescriptors->clear();
		multiDescriptors->reserve(features_.size());
	}

	if (stabilityFactors != nullptr)
	{
		stabilityFactors->clear();
		stabilityFactors->reserve(features_.size());
	}

	for (const Feature& feature : features_)
	{
		if (feature.isLocalized() && feature.observations().size() >= minimalObservations)
		{
			objectPoints.emplace_back(feature.objectPoint());

			if (multiDescriptors != nullptr)
			{
				CV::Detector::FREAKDescriptors32 multiDescriptor;
				multiDescriptor.reserve(std::min(feature.observations().size(), maximalDescriptorsPerFeaturePoint));

				if (feature.observations().size() <= maximalDescriptorsPerFeaturePoint)
				{
					for (const Feature::Observation& observation : feature.observations())
					{
						multiDescriptor.emplace_back(observation.descriptor_);
					}
				}
				else
				{
					for (size_t n = 0; n < maximalDescriptorsPerFeaturePoint; ++n)
					{
						const size_t observationIndex = n * feature.observations().size() / maximalDescriptorsPerFeaturePoint;
						ocean_assert(observationIndex < feature.observations().size());

						multiDescriptor.emplace_back(feature.observations()[observationIndex].descriptor_);
					}
				}

				multiDescriptors->emplace_back(std::move(multiDescriptor));
			}

			if (stabilityFactors != nullptr)
			{
				stabilityFactors->emplace_back(feature.stabilityFactor());
			}
		}
	}

	return true;
}

bool MultiViewMapCreator::determineLowerStereoCameras(const HomogenousMatrices4& device_T_cameras, IndexPair32& stereoCameraIndices)
{
	if (device_T_cameras.size() < 2)
	{
		return false;
	}

	stereoCameraIndices = IndexPair32(0u, 1u);

	Scalar yMinValue0 = device_T_cameras[0].translation().y();
	Scalar yMinValue1 = device_T_cameras[1].translation().y();
	Ocean::Utilities::sortLowestToFront2(yMinValue0, yMinValue1, stereoCameraIndices.first, stereoCameraIndices.second);

	for (size_t n = 2; n < device_T_cameras.size(); ++n)
	{
		const Scalar value = device_T_cameras[n].translation().y();

		if (value < yMinValue0)
		{
			yMinValue1 = yMinValue0;
			stereoCameraIndices.second = stereoCameraIndices.first;

			yMinValue0 = value;
			stereoCameraIndices.first = Index32(n);
		}
		else if (value < yMinValue1)
		{
			yMinValue1 = value;
			stereoCameraIndices.second = Index32(n);
		}
	}

	return true;
}

void MultiViewMapCreator::determineObservations(const Index32 currentFrameIndex, const Timestamp& currentTimestamp, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& world_T_currentCameras, const SharedAnyCameras& currentCameras, std::vector<Geometry::SpatialDistribution::DistributionArray>& currentDistributionArrays)
{
	ocean_assert(currentTimestamp.isValid());
	ocean_assert(world_T_currentCameras.size() == currentCameras.size());

	HomogenousMatrices4 flippedCurrentCameras_T_world;
	flippedCurrentCameras_T_world.reserve(world_T_currentCameras.size());
	for (const HomogenousMatrix4& world_T_currentCamera : world_T_currentCameras)
	{
		flippedCurrentCameras_T_world.emplace_back(PinholeCamera::standard2InvertedFlipped(world_T_currentCamera));
	}

	std::vector<Indices32> currentMatchStatementGroups;
	currentMatchStatementGroups.reserve(currentImagePointGroups_.size());

	// the inner core ensure that features points close to the border don't be expected be matched
	Scalar visibilityInnerCoreSize = Numeric::maxValue();
	for (const SharedAnyCamera& camera : currentCameras)
	{
		visibilityInnerCoreSize = std::min(visibilityInnerCoreSize, Scalar(std::min(camera->width(), camera->height())) * Scalar(0.135));
	}

	constexpr unsigned int maxDescriptorDistance = 256u * 10u / 100u;

	const double secondsPerFrame = lastTimestamp_.isValid() ? double(currentTimestamp - lastTimestamp_) : 1.0 / 30.0;
	ocean_assert(secondsPerFrame > 0.0);

	for (size_t cameraIndex = 0; cameraIndex < currentCameras.size(); ++cameraIndex)
	{
		const ObservationPair observationPair(currentFrameIndex, Index32(cameraIndex));

		const AnyCamera& camera = *currentCameras[cameraIndex];
		const HomogenousMatrix4& flippedCurrentCamera_T_world = flippedCurrentCameras_T_world[cameraIndex];

		const Vectors2& currentImagePoints = currentImagePointGroups_[cameraIndex];
		const Geometry::SpatialDistribution::DistributionArray& currentDistributionArray = currentDistributionArrays[cameraIndex];
		const CV::Detector::FREAKDescriptors32& currentDescriptors = currentDescriptorGroups_[cameraIndex];

		currentMatchStatementGroups.emplace_back(currentImagePoints.size(), 0u);

		Indices32& currentMatchStatements = currentMatchStatementGroups.back();

		for (size_t nFeature = 0; nFeature < features_.size(); /*noop*/)
		{
			Feature& feature = features_[nFeature];
			ocean_assert(feature.isLocalized());

			bool deleteFeaturePoint = false;

			bool featureHasBeenMatched = false;
			bool featureCouldHaveBeenVisible = false;

			if (PinholeCamera::isObjectPointInFrontIF(flippedCurrentCamera_T_world, feature.objectPoint()))
			{
				const Vector2 projectedImagePoint = camera.projectToImageIF(flippedCurrentCamera_T_world, feature.objectPoint());

				if (camera.isInside(projectedImagePoint))
				{
					featureCouldHaveBeenVisible = featureCouldHaveBeenVisible || camera.isInside(projectedImagePoint, visibilityInnerCoreSize);

					unsigned int bestDistance = maxDescriptorDistance + 1u;
					unsigned int bestPointCurrent = (unsigned int)(-1);

					const int xBin = currentDistributionArray.horizontalBin(projectedImagePoint.x());
					const int yBin = currentDistributionArray.horizontalBin(projectedImagePoint.y());

					for (int y = std::max(0, yBin - 1); y < std::min(yBin + 2, int(currentDistributionArray.verticalBins())); ++y)
					{
						for (int x = std::max(0, xBin - 1); x < std::min(xBin + 2, int(currentDistributionArray.horizontalBins())); ++x)
						{
							for (const Index32& nCurrentPoint : currentDistributionArray(x, y))
							{
								if (projectedImagePoint.sqrDistance(currentImagePoints[nCurrentPoint]) <= Scalar(5 * 5))
								{
									for (const Feature::Observation& previousObservation : feature.observations())
									{
										const unsigned int distance = previousObservation.descriptor_.distance(currentDescriptors[nCurrentPoint]);

										if (distance < bestDistance)
										{
											bestDistance = distance;
											bestPointCurrent = (unsigned int)(nCurrentPoint);
										}
									}
								}
							}
						}
					}

					if (bestPointCurrent != (unsigned int)(-1))
					{
						featureHasBeenMatched = true;

						feature.addObservation(currentImagePoints[bestPointCurrent], observationPair, currentDescriptors[bestPointCurrent], randomGenerator_);

						ocean_assert(bestPointCurrent < currentMatchStatements.size());
						Index32& currentMatchStatement = currentMatchStatements[bestPointCurrent];

						if (currentMatchStatement != 0u)
						{
							// the image points has already been matched with another feature point, we need to join the two features

							ocean_assert(currentMatchStatement != Index32(nFeature));

							assert(currentMatchStatement < nFeature);
							features_[currentMatchStatement].copyObservations(feature);

							deleteFeaturePoint = true;
						}
						else
						{
							// we store the information that the image point has been matched with a particular feature point
							currentMatchStatement = Index32(nFeature);
						}
					}
				}
			}

			if (!deleteFeaturePoint)
			{
				if (!featureHasBeenMatched && featureCouldHaveBeenVisible)
				{
					const Scalar sqrDistance = features_[nFeature].objectPoint().sqrDistance(world_T_device.translation()); // we can use device instead of camera due to generous threshold

					if (features_[nFeature].failedObservation(sqrDistance, secondsPerFrame))
					{
						if (features_[nFeature].isInstable())
						{
							// we have not seen the feature in a while, so we remove it

							deleteFeaturePoint = true;
						}
					}
				}
			}

			if (deleteFeaturePoint)
			{
				features_[nFeature] = std::move(features_.back());
				features_.pop_back();
			}
			else
			{
				++nFeature;
			}
		}
	}

	for (size_t cameraIndex = 0; cameraIndex < currentMatchStatementGroups.size(); ++cameraIndex)
	{
		const Indices32& currentMatchStatements = currentMatchStatementGroups[cameraIndex];

		Vectors2& imagePoints = currentImagePointGroups_[cameraIndex];
		CV::Detector::FREAKDescriptors32& currentDescriptors = currentDescriptorGroups_[cameraIndex];
		Indices32& currentCornerPyramidLevel = currentCornerPyramidLevelGroups_[cameraIndex];

		for (size_t nPoint = currentMatchStatements.size() - 1; nPoint < currentMatchStatements.size(); --nPoint)
		{
			if (currentMatchStatements[nPoint] != 0u)
			{
				imagePoints[nPoint] = imagePoints.back();
				imagePoints.pop_back();

				currentDescriptors[nPoint] = currentDescriptors.back();
				currentDescriptors.pop_back();

				currentCornerPyramidLevel[nPoint] = currentCornerPyramidLevel.back();
				currentCornerPyramidLevel.pop_back();
			}
		}
	}

	// from now on the distribution arrays are not valid anymore
	currentDistributionArrays.clear();
}

void MultiViewMapCreator::determineImageFeatures(const Frames& yFrames, const SharedAnyCameras& cameras, std::vector<Vectors2>& imagePointGroups, std::vector<CV::Detector::FREAKDescriptors32>& descriptorGroups, std::vector<Indices32>& cornerPyramidLevelGroups, std::vector<Geometry::SpatialDistribution::DistributionArray>& distributionArrays, Worker* worker)
{
	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(&MultiViewMapCreator::determineImageFeaturesSubset, yFrames.data(), cameras.data(), imagePointGroups.data(), descriptorGroups.data(), cornerPyramidLevelGroups.data(), distributionArrays.data(), 0u, 0u), 0u, (unsigned int)(yFrames.size()));
	}
	else
	{
		determineImageFeaturesSubset(yFrames.data(), cameras.data(), imagePointGroups.data(), descriptorGroups.data(), cornerPyramidLevelGroups.data(), distributionArrays.data(), 0u, (unsigned int)(yFrames.size()));
	}

#ifdef DEBUG_OUTPUT_ON_WINDOWS
#ifdef _WINDOWS
	{
		for (size_t cameraIndex = 0; cameraIndex < yFrames.size(); ++cameraIndex)
		{
			Frame rgbFrame;
			if (CV::FrameConverter::Comfort::convert(yFrames[cameraIndex], FrameType::FORMAT_RGB24, rgbFrame))
			{
				CV::Canvas::points<3u>(rgbFrame, imagePointGroups[cameraIndex], CV::Canvas::green());

				Platform::Win::Utilities::desktopFrameOutput(int(rgbFrame.width() * cameraIndex), 0, rgbFrame);
			}
		}
	}
#endif
#endif // DEBUG_OUTPUT_ON_WINDOWS
}

void MultiViewMapCreator::determineImageFeaturesSubset(const Frame* yFrames, const SharedAnyCamera* cameras, Vectors2* imagePointGroups, CV::Detector::FREAKDescriptors32* descriptorGroups, Indices32* cornerPyramidLevelGroups, Geometry::SpatialDistribution::DistributionArray* distributionArrays, const unsigned int firstGroup, const unsigned int numberGroups)
{
	ocean_assert(yFrames != nullptr && cameras != nullptr);
	ocean_assert(imagePointGroups != nullptr && descriptorGroups != nullptr && cornerPyramidLevelGroups != nullptr);
	ocean_assert(distributionArrays != nullptr);

	for (unsigned int nGroup = firstGroup; nGroup < firstGroup + numberGroups; ++nGroup)
	{
		const Frame& yFrame = yFrames[nGroup];
		const SharedAnyCamera& camera = cameras[nGroup];

		ocean_assert(yFrame.width() == camera->width() && yFrame.height() == camera->height());

		const unsigned int maxFrameArea = yFrame.width() * yFrame.height();
		const unsigned int minFrameArea = std::max(40u * 40u, maxFrameArea / 64u);

		constexpr unsigned int expectedHarrisCorners640x480 = 1000u;
		constexpr Scalar harrisCornersReductionScale = Scalar(0.4);
		constexpr unsigned int harrisCornerThreshold = 5u;

		const float inverseFocalLength = float(camera->inverseFocalLengthX());

		const CV::Detector::FREAKDescriptor32::AnyCameraDerivativeFunctor cameraFunctor(camera, 8u); // **TODO** avoid fixed layer number

		CV::Detector::HarrisCorners harrisCorners;

		Indices32& cornerPyramidLevels = cornerPyramidLevelGroups[nGroup];
		cornerPyramidLevels.clear();

		CV::Detector::FREAKDescriptors32& descriptors = descriptorGroups[nGroup];
		descriptors.clear();

		constexpr bool removeInvalid = true;
		constexpr Scalar border = Scalar(20);
		constexpr bool determineExactHarrisCornerPositions = true;
		const bool yFrameIsUndistorted = false;

		CV::Detector::FREAKDescriptor32::extractHarrisCornersAndComputeDescriptors(yFrame, maxFrameArea, minFrameArea, expectedHarrisCorners640x480, harrisCornersReductionScale, harrisCornerThreshold, inverseFocalLength, cameraFunctor, harrisCorners, cornerPyramidLevels, descriptors, removeInvalid, border, determineExactHarrisCornerPositions, yFrameIsUndistorted);

		ocean_assert(harrisCorners.size() == cornerPyramidLevels.size());

		Vectors2& imagePoints = imagePointGroups[nGroup];
		imagePoints.clear();

		for (size_t nFeature = 0; nFeature < harrisCorners.size(); ++nFeature)
		{
			const Scalar levelFactor = Scalar(1u << cornerPyramidLevels[nFeature]);
			imagePoints.emplace_back(harrisCorners[nFeature].observation() * levelFactor);
		}

		const Scalar maxPixelDistance = Scalar(std::max(yFrame.width(), yFrame.height())) * maxPixelDistanceFactor_;

		distributionArrays[nGroup] = Geometry::SpatialDistribution::distributeToArray(imagePoints.data(), imagePoints.size(), Scalar(0), Scalar(0), Scalar(yFrame.width()), Scalar(yFrame.height()), (unsigned int)(Scalar(yFrame.width() * 2u) / maxPixelDistance), (unsigned int)(Scalar(yFrame.height() * 2u) / maxPixelDistance));
	}
}

IndexPairs32 MultiViewMapCreator::matchStereoFeatures(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Vectors2& imagePointsA, const Vectors2& imagePointsB, const CV::Detector::FREAKDescriptors32& descriptorsA, const CV::Detector::FREAKDescriptors32 descriptorsB, const Indices32& pyramidLevelsA, const Indices32& pyramidLevelsB, Vectors3& objectPoints)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());
	ocean_assert(world_T_cameraA.isValid() && world_T_cameraB.isValid());

	ocean_assert(imagePointsA.size() == descriptorsA.size());
	ocean_assert(imagePointsB.size() == descriptorsB.size());

	const HomogenousMatrix4 flippedCameraA_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameraA));
	const HomogenousMatrix4 flippedCameraB_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameraB));

	const HomogenousMatrix4 cameraA_T_cameraB(world_T_cameraA.inverted() * world_T_cameraB);
	const Scalar cameraRotationAB = Numeric::atan2(cameraA_T_cameraB.xAxis().y(), cameraA_T_cameraB.xAxis().x());

	Lines3 raysB;
	raysB.reserve(imagePointsB.size());

	for (const Vector2& imagePointB : imagePointsB)
	{
		raysB.emplace_back(cameraB.ray(imagePointB, world_T_cameraB));
	}

	constexpr Scalar minSqrObjectPointDistance = Scalar(0.4 * 0.4); // 40cm
	constexpr Scalar maxSqrRayDistance = Scalar(0.05 * 0.05); // 5cm

	constexpr Scalar maxSqrDistance = Scalar(2 * 2);

	constexpr unsigned int maxDescriptorDistance = 256u * 10u / 100u;

	IndexPairs32 matches;
	matches.reserve(64);

	Vector3 nearestPointA;
	Vector3 nearestPointB;

	for (size_t nPointA = 0; nPointA < imagePointsA.size(); ++nPointA)
	{
		const Vector2& imagePointA = imagePointsA[nPointA];

		const Line3 rayA = cameraA.ray(imagePointA, world_T_cameraA);

		const Index32& pyramidLevelA = pyramidLevelsA[nPointA];

		unsigned int bestDistance = maxDescriptorDistance + 1u;
		unsigned int bestPointB = (unsigned int)(-1);
		Vector3 bestObjectPoint;

		for (size_t nPointB = 0; nPointB < imagePointsB.size(); ++nPointB)
		{
			// ensuring that both features have been determined on the same pyramid layer
			if (pyramidLevelA != pyramidLevelsB[nPointB])
			{
				continue;
			}

			// the orientations of the descriptors need similar orientations
			if (!Numeric::angleIsEqual(descriptorsA[nPointA].orientation() + cameraRotationAB, descriptorsB[nPointB].orientation(), Numeric::deg2rad(30)))
			{
				continue;
			}

			const Vector2& imagePointB = imagePointsB[nPointB];

			const Line3& rayB = raysB[nPointB];

			if (rayA.nearestPoints(rayB, nearestPointA, nearestPointB) && nearestPointA.sqrDistance(nearestPointB) <= maxSqrRayDistance)
			{
				if (rayA.point().sqrDistance(nearestPointA) >= minSqrObjectPointDistance && rayB.point().sqrDistance(nearestPointB) >= minSqrObjectPointDistance)
				{
					const Vector3 objectPoint = (nearestPointA + nearestPointB) * Scalar(0.5);

					if (PinholeCamera::isObjectPointInFrontIF(flippedCameraA_T_world, objectPoint)
							&& PinholeCamera::isObjectPointInFrontIF(flippedCameraB_T_world, objectPoint)
							&& cameraA.projectToImageIF(flippedCameraA_T_world, objectPoint).sqrDistance(imagePointA) <= maxSqrDistance
							&& cameraB.projectToImageIF(flippedCameraB_T_world, objectPoint).sqrDistance(imagePointB) <= maxSqrDistance)
					{
						const unsigned int distance = descriptorsA[nPointA].distance(descriptorsB[nPointB]);

						if (distance < bestDistance)
						{
							bestDistance = distance;
							bestPointB = (unsigned int)(nPointB);

							bestObjectPoint = objectPoint;
						}
					}
				}
			}
		}

		if (bestPointB != (unsigned int)(-1))
		{
			matches.emplace_back(Index32(nPointA), Index32(bestPointB));

			objectPoints.emplace_back(bestObjectPoint);
		}
	}

	return matches;
}

IndexPairs32 MultiViewMapCreator::matchMonoFeatures(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Vectors2& imagePointsA, const Vectors2& imagePointsB, const CV::Detector::FREAKDescriptors32& descriptorsA, const CV::Detector::FREAKDescriptors32 descriptorsB, const Indices32& pyramidLevelsA, const Indices32& pyramidLevelsB)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());
	ocean_assert(world_T_cameraA.isValid() && world_T_cameraB.isValid());

	ocean_assert(imagePointsA.size() == descriptorsA.size());
	ocean_assert(imagePointsB.size() == descriptorsB.size());

	const HomogenousMatrix4 cameraA_T_cameraB(world_T_cameraA.inverted() * world_T_cameraB);
	const Scalar cameraRotationAB = Numeric::atan2(cameraA_T_cameraB.xAxis().y(), cameraA_T_cameraB.xAxis().x());

	Lines3 raysB;
	raysB.reserve(imagePointsB.size());

	for (const Vector2& imagePointB : imagePointsB)
	{
		raysB.emplace_back(cameraB.ray(imagePointB, world_T_cameraB));
	}

	const unsigned int frameWidth = cameraB.width();
	const unsigned int frameHeight = cameraB.height();
	ocean_assert_and_suppress_unused(frameWidth == cameraA.width() && frameHeight == cameraA.height(), cameraA);

	const Scalar maxPixelDistance = Scalar(std::max(frameWidth, frameHeight)) * maxPixelDistanceFactor_;
	constexpr unsigned int maxDescriptorDistance = 256u * 20u / 100u;

	const Geometry::SpatialDistribution::DistributionArray distributionArrayB = Geometry::SpatialDistribution::distributeToArray(imagePointsB.data(), imagePointsB.size(), Scalar(0), Scalar(0), Scalar(frameWidth), Scalar(cameraB.height()), (unsigned int)(Scalar(frameWidth * 2u) / maxPixelDistance), (unsigned int)(Scalar(frameHeight * 2u) / maxPixelDistance));

	IndexPairs32 matches;
	matches.reserve(64);

	Vector3 nearestPointA;
	Vector3 nearestPointB;

	for (size_t nPointA = 0; nPointA < imagePointsA.size(); ++nPointA)
	{
		const Index32& pyramidLevelA = pyramidLevelsA[nPointA];

		unsigned int bestDistance = maxDescriptorDistance + 1u;
		unsigned int bestPointB = (unsigned int)(-1);

		const Vector2& imagePointA = imagePointsA[nPointA];

		const int xBin = distributionArrayB.horizontalBin(imagePointA.x());
		const int yBin = distributionArrayB.horizontalBin(imagePointA.y());

		for (int y = std::max(0, yBin - 1); y < std::min(yBin + 2, int(distributionArrayB.verticalBins())); ++y)
		{
			for (int x = std::max(0, xBin - 1); x < std::min(xBin + 2, int(distributionArrayB.horizontalBins())); ++x)
			{
				for (const Index32& nPointB : distributionArrayB(x, y))
				{
					// ensuring that both features have been determined on the same pyramid layer
					if (pyramidLevelA != pyramidLevelsB[nPointB])
					{
						continue;
					}

					// the orientations of the descriptors need similar orientations
					if (!Numeric::angleIsEqual(descriptorsA[nPointA].orientation() + cameraRotationAB, descriptorsB[nPointB].orientation(), Numeric::deg2rad(30)))
					{
						continue;
					}

					const unsigned int distance = descriptorsA[nPointA].distance(descriptorsB[nPointB]);

					if (distance < bestDistance)
					{
						bestDistance = distance;
						bestPointB = (unsigned int)(nPointB);
					}
				}
			}
		}

		if (bestPointB != (unsigned int)(-1))
		{
			matches.emplace_back(Index32(nPointA), Index32(bestPointB));
		}
	}

	return matches;
}

}

}

}
