/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/ContourTracker.h"
#include "ocean/cv/segmentation/ContourAnalyzer.h"
#include "ocean/cv/segmentation/ContourFinder.h"
#include "ocean/cv/segmentation/MaskCreator.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Subset.h"

#include "ocean/cv/FrameFilterMean.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/FeatureDetector.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/RANSAC.h"

#include <algorithm>

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

bool ContourTracker::detectObject(const Frame& frame, const PixelContour& roughContour, RandomGenerator& randomGenerator, const unsigned int extraContourOffset, Worker* worker, const Frame& yFrame)
{
	if (roughContour.size() <= 3)
	{
		return false;
	}

	if (!previousDenseContourSubPixel_.empty())
	{
		clear();
	}

	ocean_assert(frame.isValid());
	ocean_assert(!yFrame.isValid() || yFrame.isFrameTypeCompatible(FrameType(frame, FrameType::FORMAT_Y8), true));

	ocean_assert(roughContour.isDistinct());

	previousDenseContour_ = ContourFinder::similarityContour(frame, 21u, roughContour, extraContourOffset, randomGenerator, worker);

	previousDenseContourSubPixel_ = PixelPosition::pixelPositions2vectors(previousDenseContour_.pixels());

	if (yFrame)
	{
		previousContourStrongest_ = Detector::FeatureDetector::filterStrongHarrisPoints(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), previousDenseContourSubPixel_, 50, Scalar(5 * 5), 50, worker);
	}
	else
	{
		Frame yFrameInner;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrameInner, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		previousContourStrongest_ = Detector::FeatureDetector::filterStrongHarrisPoints(yFrameInner.constdata<uint8_t>(), yFrameInner.width(), yFrameInner.height(), yFrameInner.paddingElements(), previousDenseContourSubPixel_, 50, Scalar(5 * 5), 50, worker);
	}

	// we need a pyramid frame for the next iteration
	const unsigned int pyramidLayers = FramePyramid::idealLayers(frame.width(), frame.height(), 15u, 15u, 2u, 80u); // **TODO** unique parameter (must be class properties)

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	previousFramePyramid_.replace(frame, FramePyramid::DM_FILTER_14641, pyramidLayers, true /*copyFirstLayer*/, worker);

	return true;
}

bool ContourTracker::trackObject(const Frame& frame, RandomGenerator& randomGenerator, const unsigned int extraContourOffset, Worker* worker, const Frame& yFrame)
{
	if (previousDenseContourSubPixel_.empty())
	{
		clear();
		return false;
	}

	ocean_assert(frame.isValid());
	ocean_assert(!yFrame.isValid() || yFrame.isFrameTypeCompatible(FrameType(frame, FrameType::FORMAT_Y8), true));

	// we need a pyramid frame for the next iteration
	const unsigned int pyramidLayers = FramePyramid::idealLayers(frame.width(), frame.height(), 15u, 15u, 2u, 80u); // **TODO** unique parameter (must be class properties)

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	if (!currentFramePyramid_.replace(frame, FramePyramid::DM_FILTER_14641, pyramidLayers, true /*copyFirstLayer*/, worker))
	{
		return false;
	}

	Frame meanFrame;
	FrameFilterMean::filter(frame, meanFrame, 21u, worker);

	Indices32 usedIndices;
	Vectors2 currentContourStrongest;
	SquareMatrix3 currentHomography;

	if (!usePlanarTracking_)
	{
		// try to track strong feature points from the previous iterations with small image patches
		if (!previousContourStrongest_.empty() && Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<9u>(previousFramePyramid_, currentFramePyramid_, previousContourStrongest_, previousContourStrongest_, currentContourStrongest, 2u, 4u, worker))
		{
			const bool homographyResult = Geometry::RANSAC::homographyMatrix(previousContourStrongest_.data(), currentContourStrongest.data(), currentContourStrongest.size(), randomGenerator, currentHomography, 12u, true, 50u, Scalar(2.5 * 2.5), &usedIndices, worker);

			ocean_assert_and_suppress_unused((homographyResult && !usedIndices.empty()) || (!homographyResult && usedIndices.empty()), homographyResult);
		}

		// try to track any feature points from the previous iterations with large image patches
		if (usedIndices.empty())
		{
			IndexSet32 subsetIndexSet;
			while (subsetIndexSet.size() < 60 && subsetIndexSet.size() < previousDenseContourSubPixel_.size())
			{
				subsetIndexSet.insert(RandomI::random(randomGenerator, (unsigned int)(previousDenseContourSubPixel_.size()) - 1u));
			}

			previousContourStrongest_.clear();
			for (IndexSet32::const_iterator i = subsetIndexSet.begin(); i != subsetIndexSet.end(); ++i)
			{
				previousContourStrongest_.push_back(previousDenseContourSubPixel_[*i]);
			}

			if (Advanced::AdvancedMotion<>::trackPointsSubPixelMirroredBorder<31u>(previousFramePyramid_, currentFramePyramid_, previousContourStrongest_, previousContourStrongest_, currentContourStrongest, 2u, 4u, worker))
			{
				ocean_assert(previousContourStrongest_.size() == currentContourStrongest.size());
				const bool homographyResult = Geometry::RANSAC::homographyMatrix(previousContourStrongest_.data(), currentContourStrongest.data(), currentContourStrongest.size(), randomGenerator, currentHomography, 12u, true, 100u, Scalar(3 * 3), &usedIndices, worker);

				ocean_assert_and_suppress_unused((homographyResult && !usedIndices.empty()) || (!homographyResult && usedIndices.empty()), homographyResult);
			}
		}

		// determines the accurate homography
		if (usedIndices.size() < 20u)
		{
			usedIndices.clear();
		}
	}

	// if no accurate homography or no valid features could be tracked
	if (usedIndices.empty())
	{
		usePlanarTracking_ = true;

		const PixelBoundingBox boundingBox(40u, 40u, frame.width() - 41u, frame.height() - 41u);

		Vectors2 previousReferencePoints, currentReferencePoints;
		if (Advanced::AdvancedMotion<>::trackReliableReferencePoints<9u>(previousFramePyramid_, currentFramePyramid_, previousReferencePoints, currentReferencePoints, 20u, 20u, boundingBox, Frame(), worker) && !previousReferencePoints.empty())
		{
			const bool homographyResult = Geometry::RANSAC::homographyMatrix(previousReferencePoints.data(), currentReferencePoints.data(), previousReferencePoints.size(), randomGenerator, currentHomography, 12u, true, 200u, Scalar(2.0 * 2.0), &usedIndices, worker);

			// check whether enough consensus points have been found to determine the homography
			if (usedIndices.size() < 20u)
			{
				clear();
				return false;
			}

			ocean_assert_and_suppress_unused((homographyResult && !usedIndices.empty()) || (!homographyResult && usedIndices.empty()), homographyResult);
		}
	}

	Vectors2 currentContour;
	currentContour.reserve(previousDenseContourSubPixel_.size());

	for (Geometry::ImagePoints::const_iterator i = previousDenseContourSubPixel_.begin(); i != previousDenseContourSubPixel_.end(); ++i)
	{
		currentContour.push_back(currentHomography * *i);
	}

	const PixelContour denseContour(ContourAnalyzer::createDenseContour(currentContour));

	if (!intermediateRoughMask_.set(FrameType(frame, FrameType::FORMAT_Y8), true /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	intermediateRoughMask_.setValue(0xFF);

	MaskCreator::contour2inclusiveMaskByTriangulation(intermediateRoughMask_.data<uint8_t>(), intermediateRoughMask_.width(), intermediateRoughMask_.height(), intermediateRoughMask_.paddingElements(), denseContour.simplified(), 0x00, worker);

	Vectors2 landmarks;
	landmarks.reserve(denseContour.size());
	const bool landmarkResult = ContourFinder::findBorderLandmarks(meanFrame, intermediateRoughMask_, denseContour, extraContourOffset, landmarks);
	ocean_assert_and_suppress_unused(landmarkResult, landmarkResult);

	Vectors2 currentAdjustedContour;
	currentAdjustedContour.reserve(currentContour.size());
	const bool adjustmentResult = ContourFinder::adjustContourWithLandmarks(currentContour, landmarks, currentAdjustedContour);
	ocean_assert_and_suppress_unused(adjustmentResult, adjustmentResult);

	previousDenseContourSubPixel_ = ContourAnalyzer::equalizeContourDensity(currentAdjustedContour);

	if (yFrame)
	{
		previousContourStrongest_ = Detector::FeatureDetector::filterStrongHarrisPoints(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), previousDenseContourSubPixel_, 50, Scalar(5 * 5), 100, worker);
	}
	else
	{
		Frame yFrameInner;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrameInner, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		previousContourStrongest_ = Detector::FeatureDetector::filterStrongHarrisPoints(yFrameInner.constdata<uint8_t>(), yFrameInner.width(), yFrameInner.height(), yFrameInner.paddingElements(), previousDenseContourSubPixel_, 50, Scalar(5 * 5), 100, worker);
	}

	std::swap(previousFramePyramid_, currentFramePyramid_);

	previousHomography_ = currentHomography;

	previousDenseContour_ = ContourAnalyzer::createDenseContour(previousDenseContourSubPixel_);

	return true;
}

void ContourTracker::clear()
{
	previousDenseContourSubPixel_.clear();
	previousContourStrongest_.clear();
	previousDenseContour_ = PixelContour();

	previousHomography_.toIdentity();

	intermediateRoughMask_.release();

	usePlanarTracking_ = false;
}

}

}

}
