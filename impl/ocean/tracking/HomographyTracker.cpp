/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/HomographyTracker.h"

#include "ocean/base/Subset.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/RANSAC.h"

namespace Ocean
{

namespace Tracking
{

bool HomographyTracker::trackPoints(const Frame& currentFrame, const Frame& yPreviousFrame, RandomGenerator& randomGenerator, const Vectors2& previousPositions, SquareMatrix3& homography, Worker* worker, const Scalar frameBorder)
{
	ocean_assert(currentFrame.isValid());
	ocean_assert(!previousFramePyramid_.isValid() || yPreviousFrame.isValid());
	ocean_assert(!yPreviousFrame.isValid() || yPreviousFrame.isFrameTypeCompatible(FrameType(currentFrame, FrameType::FORMAT_Y8), true));

	ocean_assert(frameBorder >= Scalar(0) && frameBorder < Scalar(min(currentFrame.width(), currentFrame.height()) / 4u));

	if (yPreviousFrame.isValid() && !yPreviousFrame.isFrameTypeCompatible(FrameType(currentFrame, FrameType::FORMAT_Y8), true))
	{
		return false;
	}

	// we need a pyramid frame for the next iteration
	const unsigned int pyramidLayers = CV::FramePyramid::idealLayers(currentFrame.width(), currentFrame.height(), 15u, 15u, 2u, 80u); // **TODO** unique parameter (must be class properties)

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	if (!currentFramePyramid_.replace(currentFrame, CV::FramePyramid::DM_FILTER_14641, pyramidLayers, true /*copyFirstLayer*/, worker))
	{
		return false;
	}

	if (!previousFramePyramid_)
	{
		// this is the first function call so that we simply store the frame pyramid of the current frame and we return the identity homography
		std::swap(previousFramePyramid_, currentFramePyramid_);

		homography.toIdentity();
		return true;
	}

	bool result = false;

	if (frameBorder <= 0)
	{
		result = trackPoints(yPreviousFrame, previousFramePyramid_, currentFramePyramid_, randomGenerator, previousPositions, homography, worker, patchSize_);
	}
	else
	{
		Vectors2 clippedPreviousPositions;
		clippedPreviousPositions.reserve(previousPositions.size());

		for (const Vector2& previousPoint : previousPositions)
		{
			if (previousPoint.x() >= frameBorder && previousPoint.x() < Scalar(currentFrame.width()) - frameBorder && previousPoint.y() >= frameBorder && previousPoint.y() < Scalar(currentFrame.height()) - frameBorder)
			{
				clippedPreviousPositions.emplace_back(previousPoint);
			}
		}

		if (clippedPreviousPositions.empty())
		{
			std::swap(previousFramePyramid_, currentFramePyramid_);
			return false;
		}

		result = trackPoints(yPreviousFrame, previousFramePyramid_, currentFramePyramid_, randomGenerator, clippedPreviousPositions, homography, worker, patchSize_);
	}

	std::swap(previousFramePyramid_, currentFramePyramid_);

	return result;
}

bool HomographyTracker::trackPoints(const Frame& yPreviousFrame, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, RandomGenerator& randomGenerator, const Vectors2& previousPositions, SquareMatrix3& homography, Worker* worker, const unsigned int patchSize)
{
	ocean_assert(yPreviousFrame.isValid() && yPreviousFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	if (previousPositions.empty())
	{
		return false;
	}

	ocean_assert(previousFramePyramid && currentFramePyramid);

	Indices32 usedIndices;
	Vectors2 currentContourStrongest;

	Vectors2 previousContourStrongest = CV::Detector::FeatureDetector::filterStrongHarrisPoints(yPreviousFrame.constdata<uint8_t>(), yPreviousFrame.width(), yPreviousFrame.height(), yPreviousFrame.paddingElements(), previousPositions, 50, Scalar(5 * 5), 100, worker);

	// try to track strong feature points from the previous iterations with small image patches
	if (!previousContourStrongest.empty() && CV::Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<9u>(previousFramePyramid, currentFramePyramid, previousContourStrongest, previousContourStrongest, currentContourStrongest, 2u, 4u, worker))
	{
		const bool homographyResult = Geometry::RANSAC::homographyMatrix(previousContourStrongest.data(), currentContourStrongest.data(), currentContourStrongest.size(), randomGenerator, homography, 12u, true, 50u, Scalar(2.5 * 2.5), &usedIndices, worker);

		ocean_assert_and_suppress_unused((homographyResult && !usedIndices.empty()) || (!homographyResult && usedIndices.empty()), homographyResult);
	}

	// try to track any feature points from the previous iterations with large image patches
	if (usedIndices.empty())
	{
		IndexSet32 subsetIndexSet;
		while (subsetIndexSet.size() < 60 && subsetIndexSet.size() < previousPositions.size())
		{
			subsetIndexSet.emplace(RandomI::random(randomGenerator, (unsigned int)(previousPositions.size()) - 1u));
		}

		previousContourStrongest.clear();
		for (IndexSet32::const_iterator i = subsetIndexSet.begin(); i != subsetIndexSet.end(); ++i)
		{
			previousContourStrongest.emplace_back(previousPositions[*i]);
		}

		bool pointTrackingSucceeded = false;

		switch (patchSize)
		{
			case 5u:
				pointTrackingSucceeded = CV::Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<5u>(previousFramePyramid, currentFramePyramid, previousContourStrongest, previousContourStrongest, currentContourStrongest, 2u, 2u, worker);
				break;

			case 7u:
				pointTrackingSucceeded = CV::Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<7u>(previousFramePyramid, currentFramePyramid, previousContourStrongest, previousContourStrongest, currentContourStrongest, 2u, 2u, worker);
				break;

			case 15u:
				pointTrackingSucceeded = CV::Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, previousContourStrongest, previousContourStrongest, currentContourStrongest, 2u, 2u, worker);
				break;

			default:
				ocean_assert(patchSize == 31u);
				pointTrackingSucceeded = CV::Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<31u>(previousFramePyramid, currentFramePyramid, previousContourStrongest, previousContourStrongest, currentContourStrongest, 2u, 2u, worker);
				break;
		}

		if (pointTrackingSucceeded)
		{
			ocean_assert(previousContourStrongest.size() == currentContourStrongest.size());
			const bool homographyResult = Geometry::RANSAC::homographyMatrix(previousContourStrongest.data(), currentContourStrongest.data(), currentContourStrongest.size(), randomGenerator, homography, 12u, true, 100u, Scalar(3 * 3), &usedIndices, worker);

			ocean_assert_and_suppress_unused((homographyResult && !usedIndices.empty()) || (!homographyResult && usedIndices.empty()), homographyResult);
		}
	}

	// if no accurate homography or no valid features could be tracked
	if (usedIndices.empty())
	{
		const CV::PixelBoundingBox boundingBox(40u, 40u, currentFramePyramid.finestWidth() - 41u, currentFramePyramid.finestHeight() - 41u);

		Vectors2 previousReferencePoints, currentReferencePoints;
		if (CV::Advanced::AdvancedMotion<>::trackReliableReferencePoints<9u>(previousFramePyramid, currentFramePyramid, previousReferencePoints, currentReferencePoints, 20u, 20u, boundingBox, Frame(), worker) && !previousReferencePoints.empty())
		{
			const bool homographyResult = Geometry::RANSAC::homographyMatrix(previousReferencePoints.data(), currentReferencePoints.data(), previousReferencePoints.size(), randomGenerator, homography, 12u, true, 200u, Scalar(2.0 * 2.0), &usedIndices, worker);

			// check whether enough consensus points have been found to determine the homography
			if (usedIndices.size() < 20u)
			{
				return false;
			}

			ocean_assert_and_suppress_unused((homographyResult && !usedIndices.empty()) || (!homographyResult && usedIndices.empty()), homographyResult);
		}
	}

	return true;
}

}

}
