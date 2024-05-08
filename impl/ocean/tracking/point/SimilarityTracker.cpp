/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/point/SimilarityTracker.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/SpatialDistribution.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

bool SimilarityTracker::determineSimilarity(const Frame& yFrame, const CV::PixelBoundingBox& previousSubRegion, SquareMatrix3* current_T_previous, Vector2* translation, Scalar* rotation, Scalar* scale, const Vector2& predictedTranslation, TrackerConfidence* trackerConfidence, RegionTextureness* regionTextureness, Worker* worker)
{
	if (current_T_previous != nullptr)
	{
		current_T_previous->toIdentity();
	}

	if (translation != nullptr)
	{
		*translation = Vector2(0, 0);
	}

	if (rotation != nullptr)
	{
		*rotation = Scalar(0);
	}

	if (scale != nullptr)
	{
		*scale = Scalar(1);
	}

	if (trackerConfidence != nullptr)
	{
		*trackerConfidence = TC_NONE;
	}

	if (regionTextureness != nullptr)
	{
		*regionTextureness = RT_UNKNOWN;
	}

	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.width() >= 40u && yFrame.height() >= 40u);
	ocean_assert(previousSubRegion.isValid());
	ocean_assert(previousSubRegion.right() < yFrame.width() && previousSubRegion.bottom() < yFrame.height());

	ocean_assert(yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(!keyFramePyramid_.isValid() || keyFramePyramid_.frameType().isFrameTypeCompatible(yFrame, false));

	if (!yFrame.isValid() || yFrame.width() < 40u || yFrame.height() < 40u || !yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8) || (keyFramePyramid_ && !keyFramePyramid_.frameType().isFrameTypeCompatible(yFrame, false)) || !previousSubRegion.isValid())
	{
		return false;
	}

	TrackerConfidence internalTrackerConfidence = TC_NONE;
	RegionTextureness internalRegionTextureness = RT_LOW;

	constexpr unsigned int coarsestLayerRadius = 2u;
	constexpr unsigned int subPixelIterations = 2u;

	const unsigned int maxSize = std::max(yFrame.width(), yFrame.height());

	// we want to ensure that corresponding feature points can have an offset of 2.5% between to successive video frames
	const unsigned int pyramidLayers = keyFramePyramid_ ? keyFramePyramid_.layers() : CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), 20u, 20u, 2u, maxSize * 25u / 1000u, coarsestLayerRadius);

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	constexpr bool copyFirstLayer = true; // we need to make a copy of the first layer, as this pyramid will be used as 'previousPyramid' in the next call of determineSimilarity()

	currentFramePyramid_.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), copyFirstLayer, worker);

	constexpr size_t minimalFeaturePoints = 20;
	constexpr size_t desiredFeaturePoints = 80;
	constexpr size_t maximalFeaturePoints = 150;

	if (keyFramePyramid_)
	{
		ocean_assert(keyFramePyramid_.layers() == currentFramePyramid_.layers());

		unsigned int firstPyramidLayerIndex = (unsigned int)(-1);

		if (keyFramePoints_.empty())
		{
			// we do not have any key-frame points, so that we have to determine new feature points

			Vectors2 keyFramePoints;
			if (!determineFeaturePoints(keyFramePyramid_, previousSubRegion, minimalFeaturePoints, desiredFeaturePoints, maximalFeaturePoints, keyFramePoints, firstPyramidLayerIndex, internalRegionTextureness, worker))
			{
				reset();
				return false;
			}

			// we will try to reuse the feature points in several next iterations

			ocean_assert(keyFramePoints_.empty());
			keyFramePoints_ = std::move(keyFramePoints);

			keyFramePointsLayerIndex_ = firstPyramidLayerIndex;
		}
		else
		{
			ocean_assert(keyFramePointsLayerIndex_ < keyFramePyramid_.layers());
			firstPyramidLayerIndex = keyFramePointsLayerIndex_;
		}

		ocean_assert(keyFramePoints_.size() <= maximalFeaturePoints);

		unsigned int hierarchyLayers = keyFramePyramid_.layers() - firstPyramidLayerIndex;
		ocean_assert(hierarchyLayers >= 1u && hierarchyLayers <= keyFramePyramid_.layers());

		Vectors2 roughCurrentPoints;

		if (predictedTranslation != Vector2(0, 0) || !previous_T_key_.isIdentity())
		{
			roughCurrentPoints.reserve(keyFramePoints_.size());

			const Scalar invLayerSizeFactor = Scalar(1) / Scalar(CV::FramePyramid::sizeFactor(firstPyramidLayerIndex));

			// the predicted translation was provided for the finest pyramid resolution, so that we need to adjust it if we do not track on the finest resolution

			const SquareMatrix3 predicted_current_T_previous(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(predictedTranslation * invLayerSizeFactor, 1));

			const SquareMatrix3 predicted_currentTkey(predicted_current_T_previous * previous_T_key_);

			const Scalar maximalWidth = Scalar(keyFramePyramid_.width(firstPyramidLayerIndex) - 1u);
			const Scalar maximalHeight = Scalar(keyFramePyramid_.height(firstPyramidLayerIndex) - 1u);

			for (const Vector2& keyFramePoint : keyFramePoints_)
			{
				const Vector2 predictedPoint = predicted_currentTkey * keyFramePoint;

				roughCurrentPoints.emplace_back(minmax(Scalar(0), predictedPoint.x(), maximalWidth), minmax(Scalar(0), predictedPoint.y(), maximalHeight));
			}
		}

		SquareMatrix3 currentTkey(false);

		// way may apply a second tracking iteration trying to improve the tracking result
		bool applyAnotherTrackingIteration = true;

		while (applyAnotherTrackingIteration)
		{
			applyAnotherTrackingIteration = false;

			// let's create new frame pyramids, starting with level 'firstPyramidLayerIndex' - while we do not copy the data
			const CV::FramePyramid hierarchyKey(keyFramePyramid_, firstPyramidLayerIndex, hierarchyLayers, false /*copyData*/);
			const CV::FramePyramid hierarchyCurrent(currentFramePyramid_, firstPyramidLayerIndex, hierarchyLayers, false /*copyData*/);

			Indices32 validCorrespondences;

			if (determineSimilarityTransformation(hierarchyKey, hierarchyCurrent, keyFramePoints_, roughCurrentPoints, randomGenerator_, currentTkey, validCorrespondences, coarsestLayerRadius, subPixelIterations, worker) && validCorrespondences.size() >= minimalFeaturePoints)
			{
				ocean_assert(previous_T_key_.isSimilarity() && currentTkey.isSimilarity());
				const SquareMatrix3 layer_current_T_previous = currentTkey * previous_T_key_.inverted();

				// we need the similarity transformation in the coordinate system of the finest image resolutions
				const SquareMatrix3 fine_current_T_previous = Geometry::Homography::toFinestHomography(layer_current_T_previous, firstPyramidLayerIndex);

				if (current_T_previous != nullptr)
				{
					*current_T_previous = fine_current_T_previous;
				}

				// let's extract the scale from the similarity transformation

				// similarity transformation:
				// Ra  -Rb  Tx
				// Rb   Ra  Ty
				//  0    0   1

				if (translation != nullptr)
				{
					const Vector2 layerTranslation(fine_current_T_previous.zAxis().xy());

					*translation = layerTranslation;
				}

				if (rotation != nullptr)
				{
					const Vector2 axis(fine_current_T_previous.xAxis().xy());

					*rotation = Numeric::atan2(axis.y(), axis.x());
				}

				if (scale != nullptr)
				{
					const Vector2 axis(fine_current_T_previous.xAxis().xy());
					const Scalar axisLength = axis.length();

					*scale = axisLength;
				}

				// we can use the following parameter to determine the confidence
				// - number of found feature points
				// - number of feature points supporting the similarity
				// - sum of error of all feature points (currently not used due to additional computation cost)

				if (keyFramePoints_.size() <= minimalFeaturePoints + 1)
				{
					// we have a very small number of feature points only
					internalTrackerConfidence = TC_BAD;
				}
				else
				{
					// let's rate the ratio between valid correspondences and all correspondences

					ocean_assert(keyFramePoints_.size() >= 1);
					const float ratioValidCorrespondences = float(validCorrespondences.size()) / float(keyFramePoints_.size());

					if (keyFramePoints_.size() >= desiredFeaturePoints && ratioValidCorrespondences >= 0.85f)
					{
						internalTrackerConfidence = TC_VERY_GOOD;
					}
					else if (keyFramePoints_.size() >= desiredFeaturePoints && ratioValidCorrespondences >= 0.60f)
					{
						internalTrackerConfidence = TC_GOOD;
					}
					else if (keyFramePoints_.size() >= desiredFeaturePoints / 2 && ratioValidCorrespondences >= 0.35f)
					{
						internalTrackerConfidence = TC_MODERATE;

						if (roughCurrentPoints.empty())
						{
							// as we have a moderate tracking result, we try to improve the result by !one! additional tracking iteration
							// we use the current transformation to predict the locations of the key-frame feature points

							roughCurrentPoints.clear();
							roughCurrentPoints.reserve(keyFramePoints_.size());

							const Scalar maximalWidth = Scalar(keyFramePyramid_.width(firstPyramidLayerIndex) - 1u);
							const Scalar maximalHeight = Scalar(keyFramePyramid_.height(firstPyramidLayerIndex) - 1u);

							for (const Vector2& keyFramePoint : keyFramePoints_)
							{
								const Vector2 roughCurrentPoint = currentTkey * keyFramePoint;

								roughCurrentPoints.emplace_back(minmax(Scalar(0), roughCurrentPoint.x(), maximalWidth), minmax(Scalar(0), roughCurrentPoint.y(), maximalHeight));
							}

							hierarchyLayers = std::min(hierarchyLayers, 3u);
							applyAnotherTrackingIteration = true;
						}
					}
					else
					{
						internalTrackerConfidence = TC_BAD;
					}
				}
			}
		}

		if (!currentTkey.isNull())
		{
			previous_T_key_ = currentTkey;
		}
	}

	if (!keyFramePyramid_.isValid() || internalTrackerConfidence < TC_GOOD)
	{
		// whenever the tracker's confidence is quite low, we avoid to improve persistence
		// this may increase drift errors but will also improve tracking quality

		std::swap(keyFramePyramid_, currentFramePyramid_);

		previous_T_key_.toIdentity();
		keyFramePoints_.clear();
		keyFramePointsLayerIndex_ = (unsigned int)(-1);
	}

	if (trackerConfidence != nullptr)
	{
		*trackerConfidence = internalTrackerConfidence;
	}

	if (regionTextureness != nullptr)
	{
		*regionTextureness = internalRegionTextureness;
	}

	return true;
}

bool SimilarityTracker::determineFeaturePoints(const CV::FramePyramid& framePyramid, const CV::PixelBoundingBox& subRegion, const size_t minimalFeaturePoints, const size_t desiredFeaturePoints, const size_t maximalFeaturePoints, Vectors2& featurePoints, unsigned int &usedLayerIndex, RegionTextureness& regionTextureness, Worker* worker)
{
	ocean_assert(framePyramid.isValid() && FrameType::formatIsGeneric(framePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));
	ocean_assert(subRegion.isValid());

	ocean_assert(minimalFeaturePoints <= desiredFeaturePoints);
	ocean_assert(desiredFeaturePoints <= maximalFeaturePoints);

	featurePoints.clear();
	usedLayerIndex = (unsigned int)(-1);

	// we define four individual configurations (combinations of feature strength and pyramid layers)
	// the individual configurations ensure that we get the best tracking result with highest performance possible
	const std::vector<unsigned int> firstPyramidLayerIndices = {1u, 1u, 0u, 0u};
	const std::vector<unsigned int> featureStrengths = {16u, 8u, 4u, 2u};

	// in case the provided sub-region is too small, we cannot start on the 2nd pyramid layer, in this case we start on a finer layer
	unsigned int maximalFirstPyramidLayerIndex = firstPyramidLayerIndices.front();

	if (subRegion.size() <= 60u * 60u)
	{
		maximalFirstPyramidLayerIndex = 0u;
	}
	else if (subRegion.size() <= 100u * 100u)
	{
		maximalFirstPyramidLayerIndex = 1u;
	}

	// in any case, we cannot start with a layer we do not have
	maximalFirstPyramidLayerIndex = std::min(maximalFirstPyramidLayerIndex, framePyramid.layers() - 1u);

	CV::Detector::HarrisCorners corners;

	for (size_t nConfiguration = 0; nConfiguration < firstPyramidLayerIndices.size(); ++nConfiguration)
	{
		ocean_assert(firstPyramidLayerIndices.size() == featureStrengths.size());

		usedLayerIndex = std::min(firstPyramidLayerIndices[nConfiguration], maximalFirstPyramidLayerIndex);

		ocean_assert(usedLayerIndex < framePyramid.layers());

		// we have a valid frame pyramid of a previous frame

		const Frame& previousPyramidLayer = framePyramid[usedLayerIndex];

		const unsigned int layerSizeFactor = CV::FramePyramid::sizeFactor(usedLayerIndex);

		// we scale the sub-region for the first pyramid layer
		const CV::PixelBoundingBox layerSubRegion = (subRegion / layerSizeFactor) && CV::PixelBoundingBox(0, 0u, previousPyramidLayer.width() - 1u, previousPyramidLayer.height() - 1u);

		if (layerSubRegion)
		{
			const unsigned int featureStrength = featureStrengths[nConfiguration];

			corners.clear();
			if (!CV::Detector::HarrisCornerDetector::detectCorners(previousPyramidLayer.constdata<uint8_t>(), previousPyramidLayer.width(), previousPyramidLayer.height(), previousPyramidLayer.paddingElements(), layerSubRegion.left(), layerSubRegion.top(), layerSubRegion.width(), layerSubRegion.height(), featureStrength, true, corners, true, worker))
			{
				// 	the sub-region is too small
				return false;
			}

			if (corners.size() >= desiredFeaturePoints)
			{
				if (nConfiguration == 0)
				{
					regionTextureness = RT_HIGH;
				}
				else if (nConfiguration <= 2)
				{
					regionTextureness = RT_MODERATE;
				}

				break;
			}
		}
	}

	if (corners.size() < minimalFeaturePoints)
	{
		return false;
	}

	if (corners.size() > maximalFeaturePoints)
	{
		// we have too many feature points, so we have to filter them

		std::sort(corners.begin(), corners.end());

		const Vectors2 points = CV::Detector::HarrisCorner::corners2imagePoints(corners);

		const Scalar invLayerSizeFactor = Scalar(1) / Scalar(CV::FramePyramid::sizeFactor(usedLayerIndex));

		const Scalar left = Scalar(subRegion.left()) * invLayerSizeFactor;
		const Scalar top = Scalar(subRegion.top()) * invLayerSizeFactor;
		const Scalar width = Scalar(subRegion.width()) * invLayerSizeFactor;
		const Scalar height = Scalar(subRegion.height()) * invLayerSizeFactor;

		// bin size should be 10px, while we must not use more than 100 bins to ensure that all feature points are spread around
		const unsigned int horizontalBins = minmax(1u, (unsigned int)(width * Scalar(0.1)), 10u);
		const unsigned int verticalBins = minmax(1u, (unsigned int)(height * Scalar(0.1)), 10u);

		const Vectors2 filteredPoints = Geometry::SpatialDistribution::distributeAndFilter(points.data(), points.size(), left, top, width, height, horizontalBins, verticalBins, maximalFeaturePoints);

		featurePoints.reserve(filteredPoints.size());

		for (const Vector2& filteredPoint : filteredPoints)
		{
			featurePoints.emplace_back(filteredPoint.x(), filteredPoint.y());
			ocean_assert(subRegion.isInside(CV::PixelPosition::vector2pixelPosition(featurePoints.back()) * CV::FramePyramid::sizeFactor(usedLayerIndex)));
		}
	}
	else
	{
		featurePoints.reserve(corners.size());

		for (const CV::Detector::HarrisCorner& corner : corners)
		{
			featurePoints.emplace_back(corner.observation().x(), corner.observation().y());
			ocean_assert(subRegion.isInside(CV::PixelPosition::vector2pixelPosition(featurePoints.back()) * CV::FramePyramid::sizeFactor(usedLayerIndex)));
		}
	}

	ocean_assert(usedLayerIndex < framePyramid.layers());
	ocean_assert(featurePoints.size() >= minimalFeaturePoints);

	return true;
}

bool SimilarityTracker::determineSimilarityTransformation(const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, const Vectors2& previousPoints, const Vectors2& roughCurrentPoints, RandomGenerator& randomGenerator, SquareMatrix3& current_T_previous, Indices32& validCorrespondences, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, Worker* worker)
{
	ocean_assert(yPreviousFramePyramid.frameType() == yCurrentFramePyramid.frameType());
	ocean_assert(FrameType::formatIsGeneric(yPreviousFramePyramid.frameType().pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	ocean_assert(roughCurrentPoints.empty() || roughCurrentPoints.size() == previousPoints.size());

	ocean_assert(coarsestLayerRadius >= 1u);
	ocean_assert(validCorrespondences.empty());

	Vectors2 currentPoints(previousPoints.size());
	if (CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<1u, 7u>(yPreviousFramePyramid, yCurrentFramePyramid, previousPoints, roughCurrentPoints.empty() ? previousPoints : roughCurrentPoints, currentPoints, coarsestLayerRadius, subPixelIterations, worker))
	{
		ocean_assert(previousPoints.size() == currentPoints.size());

		const Scalar layerPixelError = 1.5; // constant for every layer

		// we determine a similarity transformation based on the tracked feature points via a RANSAC-based approach

		SquareMatrix3 ransacSimilarity;
		if (Geometry::RANSAC::similarityMatrix(previousPoints.data(), currentPoints.data(), previousPoints.size(), randomGenerator, ransacSimilarity, 2u, 100u, Numeric::sqr(layerPixelError), &validCorrespondences) && validCorrespondences.size() >= 4)
		{
			const Vectors2 validPreviousPoints(Subset::subset(previousPoints, validCorrespondences));
			const Vectors2 validCurrentPoints(Subset::subset(currentPoints, validCorrespondences));

			// now, we optimize the RANSAC-based similarity

			return Geometry::NonLinearOptimizationHomography::optimizeSimilarity<Geometry::Estimator::ET_SQUARE>(ransacSimilarity, validPreviousPoints.data(), validCurrentPoints.data(), validPreviousPoints.size(), current_T_previous, 20u, Scalar(0.001), Scalar(5));
		}
	}

	return false;
}

}

}

}
