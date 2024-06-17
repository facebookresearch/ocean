/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/HomographyImageAlignmentSparse.h"

#include "ocean/base/Subset.h"

#include "ocean/cv/advanced/AdvancedMotion.h"
#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace Tracking
{

bool HomographyImageAlignmentSparse::determineHomographyWithPoints(const Frame& previousFrame, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const CV::SubRegion& previousSubRegion, const size_t numberFeaturePoints, const unsigned int patchSize, const unsigned int coarsestLayerRadius, const bool determineFullHomography, const unsigned int subPixelIterations, SquareMatrix3& homography, Worker* worker, Vectors2* previousPoints, Vectors2* currentPoints, Indices32* validPointIndices, const Scalar squarePixelErrorThreshold, const bool zeroMean)
{
	ocean_assert(previousFramePyramid && currentFramePyramid);
	ocean_assert(previousFramePyramid.frameType() == currentFramePyramid.frameType());
	ocean_assert(currentFramePyramid.layers() == currentFramePyramid.layers());
	ocean_assert(numberFeaturePoints >= 1);

	ocean_assert(coarsestLayerRadius >= 1u);
	ocean_assert(subPixelIterations >= 1u);

    ocean_assert(validPointIndices == nullptr || (previousPoints != nullptr && currentPoints != nullptr));

    ocean_assert(squarePixelErrorThreshold > 0);

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(previousFrame, FrameType::FORMAT_Y8, yFrame, false, worker))
	{
		ocean_assert(false && "Incompatible frame type!");
	}

	// we detect almost any feature points in the frame as we will filter them later
	Vectors2 previousPointCandidates = CV::Detector::FeatureDetector::determineHarrisPoints(yFrame, previousSubRegion, 0u, 0u, 15u, worker);

	if (previousPointCandidates.empty())
	{
		return false;
	}

	// calculate bounding box around all regions
	Box2 boundingBox = previousSubRegion.boundingBox().isValid() ? previousSubRegion.boundingBox() : Box2();
	if (!boundingBox.isValid())
	{
		boundingBox = Box2(0, 0, Scalar(previousFrame.width()), Scalar(previousFrame.height()));
	}

	// calculate clip window by intersecting bounding box with image borders
	unsigned int windowLeft, windowTop, windowWidth, windowHeight;
	if (!boundingBox.box2integer(previousFrame.width(), previousFrame.height(), windowLeft, windowTop, windowWidth, windowHeight))
	{
		return false;
	}

	if (previousPointCandidates.size() > numberFeaturePoints)
	{
		// now we filter the determined feature points by using an array of bins and try to select at least one feature point from each bin

		unsigned int horizontalBins, verticalBins;
		Geometry::SpatialDistribution::idealBins(windowWidth, windowHeight, numberFeaturePoints, horizontalBins, verticalBins, 2u, 2u);

		previousPointCandidates = Geometry::SpatialDistribution::distributeAndFilter(previousPointCandidates.data(), previousPointCandidates.size(), Scalar(windowLeft), Scalar(windowTop), Scalar(windowWidth), Scalar(windowHeight), horizontalBins, verticalBins, numberFeaturePoints);
	}

	Vectors2 currentPointCandidates;

	switch (patchSize)
	{
		case 5u:
		{
			if (zeroMean)
			{
				if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<5u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}
			else
			{
				if (!CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<5u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}

			break;
		}

		case 7u:
		{
			if (zeroMean)
			{
				if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<7u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}
			else
			{
				if (!CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<7u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}

			break;
		}

		case 31u:
		{
			if (zeroMean)
			{
				if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<31u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}
			else
			{
				if (!CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<31u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}

			break;
		}

		default:
		{
			if (zeroMean)
			{
				ocean_assert(patchSize == 15u);
				if (!CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}
			else
			{
				if (!CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, previousPointCandidates, previousPointCandidates, currentPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
				{
					return false;
				}
			}

			break;
		}
	}

	if (currentPointCandidates.size() < 4)
	{
		return false;
	}

	RandomGenerator randomGenerator;

	Indices32 usedIndices;
	Indices32* const usedIndicesPointer = (previousPoints || currentPoints || validPointIndices) ? &usedIndices : nullptr;

	if (determineFullHomography)
	{
		// we determine the 8-DOF homography
		if (!Geometry::RANSAC::homographyMatrix(previousPointCandidates.data(), currentPointCandidates.data(), previousPointCandidates.size(), randomGenerator, homography, 8u, true, 100u, squarePixelErrorThreshold, usedIndicesPointer, worker, false))
		{
			return false;
		}
	}
	else
	{
		// we determine the 2-DOF homography for translation only
		Vectors2 offsets(previousPointCandidates.size());
		for (size_t n = 0; n < offsets.size(); ++n)
		{
			offsets[n] = currentPointCandidates[n] - previousPointCandidates[n];
		}

		Vector2 translation(0, 0);
		if (!Geometry::RANSAC::translation(ConstArrayAccessor<Vector2>(offsets), randomGenerator, translation, true, 50u, squarePixelErrorThreshold, nullptr, usedIndicesPointer))
		{
			return false;
		}

		homography = SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(translation.x(), translation.y(), 1));
	}

	if (usedIndicesPointer)
	{
        if (validPointIndices)
        {
            ocean_assert(previousPoints && currentPoints);

            // the user is interested in the point correspondences but also in inliers and outliers
            // therefore, we will provide all tracked point correspondences without any filtering

            if (previousPoints)
			{
                *previousPoints = std::move(previousPointCandidates);
			}

            if (currentPoints)
			{
                *currentPoints = std::move(currentPointCandidates);
			}

            *validPointIndices = std::move(*usedIndicesPointer);
        }
        else
        {
            // the user is interested in valid point correspondences only
            // therefore, we will apply a filtering based on the indices returned by RANSAC

            if (previousPoints)
			{
                *previousPoints = Subset::subset(previousPointCandidates, *usedIndicesPointer);
			}

            if (currentPoints)
			{
                *currentPoints = Subset::subset(currentPointCandidates, *usedIndicesPointer);
			}

        }
	}

	ocean_assert(Numeric::isEqual(homography[8], 1));
	return true;
}

}

}
