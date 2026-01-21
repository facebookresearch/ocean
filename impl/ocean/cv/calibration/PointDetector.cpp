/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/PointDetector.h"
#include "ocean/cv/calibration/CalibrationDebugElements.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

PointDetector::PointPattern::PointPattern(const PointPattern& pointPattern, const unsigned int frameStrideElements) :
	PointPattern(pointPattern.radius_, pointPattern.innerRadius_, frameStrideElements, pointPattern.isCircle_)
{
	// nothing to do here
}

PointDetector::PointPattern::PointPattern(const unsigned int radius, const unsigned int innerRadius, const unsigned int frameStrideElements, const bool useCircle)
{
	if (determineOffsets(radius, innerRadius, frameStrideElements, useCircle, negativeOffset_, positiveOffsets_, &offsets_))
	{
		ocean_assert(positiveOffsets_.size() == offsets_.size());

		radius_ = radius;
		innerRadius_ = innerRadius;
		frameStrideElements_ = frameStrideElements;

		isCircle_ = useCircle;

		ocean_assert(!positiveOffsets_.empty());
		strengthNormalization_ = 1.0f / float(positiveOffsets_.size());

		ocean_assert(isValid());
	}
	else
	{
		ocean_assert(!isValid());
	}
}

bool PointDetector::PointPattern::determinePointStrength(const Frame& yFrame, const Vector2& observation, int32_t& strength, bool& strict) const
{
	ocean_assert(isValid());

	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));

	ocean_assert(frameStrideElements_ == yFrame.strideElements());
	if (frameStrideElements_ != yFrame.strideElements())
	{
		return false;
	}

	uint8_t centerPixel;
	CV::FrameInterpolatorBilinear::interpolatePixel<uint8_t, uint8_t, 1u, CV::PC_CENTER>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), observation, &centerPixel);

	int32_t sumSqrDifference = 0;
	strict = true;

	for (const CV::PixelPositionI& offset : offsets_)
	{
		uint8_t surroundingPixel;
		CV::FrameInterpolatorBilinear::interpolatePixel<uint8_t, uint8_t, 1u, CV::PC_CENTER>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), observation + Vector2(offset.x(), offset.y()), &surroundingPixel);

		const int32_t difference = int32_t(surroundingPixel) - int32_t(centerPixel);

		if (difference >= 0)
		{
			sumSqrDifference += difference * difference;
		}
		else
		{
			sumSqrDifference -= difference * difference;
		}

		if (strict)
		{
			// let's check whether the sign of the difference has changed

			if (sumSqrDifference != 0 && difference != 0)
			{
				if ((difference > 0 && sumSqrDifference < 0) || (difference < 0 && sumSqrDifference > 0))
				{
					strict = false;
				}
			}
		}
	}

	strength = sumSqrDifference;

	return true;
}

bool PointDetector::PointPattern::determineOffsets(const unsigned int radius, const unsigned int innerRadius, const unsigned int frameStrideElements, const bool useCircle, Index32& negativeOffset, Indices32& positiveOffsets, CV::PixelPositionsI* offsets)
{
	ocean_assert(radius >= 1u);
	ocean_assert(innerRadius < radius);
	ocean_assert(frameStrideElements >= radius * 2u + 1u);

	if (radius == 0u || innerRadius >= radius || frameStrideElements < radius * 2u + 1u)
	{
		return false;
	}

	const unsigned int diameter = radius * 2u + 1u;
	ocean_assert(diameter <= frameStrideElements);

	if (diameter > frameStrideElements)
	{
		return false;
	}

	CV::PixelPositionsI pixelOffsets;
	pixelOffsets.reserve((radius * 2u + 1u) * (radius * 2u + 1u));

	for (int32_t y = -int32_t(radius); y <= int32_t(radius); ++y)
	{
		for (int32_t x = -int32_t(radius); x <= int32_t(radius); ++x)
		{
			if (useCircle)
			{
				// we apply a real (pixilated circle) to determine the points

				const unsigned int sqrDistance = x * x + y * y;

				if (sqrDistance > radius * radius || sqrDistance <= innerRadius * innerRadius)
				{
					continue;
				}
			}
			else
			{
				// we apply a square to determine the points, with edge length 2 * radius + 1

				if (abs(y) <= int32_t(innerRadius) && abs(x) <= int32_t(innerRadius))
				{
					continue;
				}
			}

			pixelOffsets.emplace_back(x, y);
		}
	}

	if (pixelOffsets.empty())
	{
		return false;
	}

	CV::PixelPositionI previousPixelOffset = pixelOffsets.front();
	ocean_assert(previousPixelOffset.x() <= 0 && previousPixelOffset.y() < 0);

	negativeOffset = Index32(-previousPixelOffset.y() * int32_t(frameStrideElements) - previousPixelOffset.x());
	ocean_assert(negativeOffset <= (radius + 1u) * frameStrideElements);

	ocean_assert(positiveOffsets.empty());
	positiveOffsets.clear();
	positiveOffsets.reserve(pixelOffsets.size());

	for (const CV::PixelPositionI& pixelOffset : pixelOffsets)
	{
		const CV::PixelPositionI difference = pixelOffset - previousPixelOffset;
		ocean_assert(difference.y() >= 0);

		const Index32 positiveIndex = Index32(difference.y() * int32_t(frameStrideElements) + difference.x());

		positiveOffsets.push_back(positiveIndex);

		previousPixelOffset = pixelOffset;
	}

#ifdef OCEAN_DEBUG
	{
		int32_t debugOffsetTest = -int32_t(negativeOffset);

		ocean_assert(pixelOffsets.size() == positiveOffsets.size());
		ocean_assert(positiveOffsets.front() == 0u);
		for (size_t n = 0; n < pixelOffsets.size(); ++n)
		{
			const Index32 positiveOffset = positiveOffsets[n];
			const CV::PixelPositionI& pixelOffset = pixelOffsets[n];

			debugOffsetTest += int32_t(positiveOffset);

			const int32_t expectedOffset = pixelOffset.y() * int32_t(frameStrideElements) + pixelOffset.x();

			ocean_assert(expectedOffset == debugOffsetTest);
		}
	}
#endif

	if (offsets != nullptr)
	{
		*offsets = std::move(pixelOffsets);
	}

	return true;
}

bool PointDetector::detectPoints(const Frame& yFrame, Worker* worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));

	if (!yFrame.isValid() || !yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	if (pointPatterns_.empty())
	{
		constexpr unsigned int pointPatternRadius = 5u;
		constexpr unsigned int pointPatternInnerRadius = 4u;

		pointPatterns_ = createPointPatterns(pointPatternRadius, pointPatternInnerRadius, true /*useCircle*/, yFrame.strideElements());
	}
	else
	{
		updatePointPatterns(pointPatterns_, yFrame.strideElements());
	}

	ocean_assert(!pointPatterns_.empty());
	if (pointPatterns_.empty())
	{
		return false;
	}

	const unsigned int maxVariance = maxDeviation_ * maxDeviation_;

	constexpr unsigned int detectionScaleSteps = 2u;

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		if (CalibrationDebugElements::get().isElementActive(CalibrationDebugElements::EI_POINT_DETECTOR_POINTS_NON_SUPPRESSED))
		{
			Points debugPoints;
			if (detectPoints(yFrame, pointPatterns_, minDifference_, maxVariance, debugPoints, false /*suppressNonMaximum*/, detectionScaleSteps, worker))
			{
				CalibrationDebugElements::get().updatePointDetectorPointsNonSuppressed(yFrame, debugPoints);
			}
		}
	}

	roughPoints_.clear();
	if (!detectPoints(yFrame, pointPatterns_, minDifference_, maxVariance, roughPoints_, true /*suppressNonMaximum*/, detectionScaleSteps, worker))
	{
		return false;
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updatePointDetectorPointsSuppressed(yFrame, roughPoints_);
	}

	if (!yPointPatternImages_.isValid())
	{
		const unsigned int width = (unsigned int)(pointPatterns_.size()) * pointPatternImageSize_;
		const unsigned int height = (unsigned int)(pointPatternImageSize_) * 2u;

		yPointPatternImages_.set(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);

		for (size_t nPattern = 0; nPattern < pointPatterns_.size(); ++nPattern)
		{
			const PointPattern& pointPattern = pointPatterns_[nPattern];

			const unsigned int left = (unsigned int)(nPattern * pointPatternImageSize_);

			Frame yDarkPointPatternImage = yPointPatternImages_.subFrame(left, 0u, pointPatternImageSize_, pointPatternImageSize_);
			Frame yBrightPointPatternImage = yPointPatternImages_.subFrame(left, pointPatternImageSize_, pointPatternImageSize_, pointPatternImageSize_);

			const unsigned int pointRadius = pointPattern.radius();

			if (!paintPointPattern(yDarkPointPatternImage, pointRadius, 0x00u) || !paintPointPattern(yBrightPointPatternImage, pointRadius, 0xFFu))
			{
				ocean_assert(false && "This should never happen!");
			}
		}
	}

	points_.clear();
	if (!optimizePoints(yFrame, roughPoints_, pointPatterns_, points_, worker))
	{
		return false;
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updatePointDetectorPointsOptimized(yFrame, points_);
	}

	if (points_.size() >= 2)
	{
		removeDuplicatedPoints(yFrame.width(), yFrame.height(), points_, maxDistanceBetweenDuplicatePoints_);
	}

	if (pointsDistributionArray_.isValid() && pointsDistributionArray_.width() == Scalar(yFrame.width()) && pointsDistributionArray_.height() == Scalar(yFrame.height()))
	{
		pointsDistributionArray_.clear();
	}
	else
	{
		constexpr Scalar distance = Scalar(50);

		unsigned int horizontalBins = 0u;
		unsigned int verticalBins = 0u;
		Geometry::SpatialDistribution::idealBinsNeighborhood9(yFrame.width(), yFrame.height(), distance, horizontalBins, verticalBins, 2u, 2u, 32u, 32u);

		pointsDistributionArray_ = Geometry::SpatialDistribution::DistributionArray(Scalar(0), Scalar(0), Scalar(yFrame.width()), Scalar(yFrame.height()), horizontalBins, verticalBins);
	}

	for (size_t n = 0; n < points_.size(); ++n)
	{
		const Vector2& point = points_[n].observation();

		const unsigned int xBin = pointsDistributionArray_.horizontalBin(point.x());
		const unsigned int yBin = pointsDistributionArray_.verticalBin(point.y());

		ocean_assert(xBin < pointsDistributionArray_.horizontalBins());
		ocean_assert(yBin < pointsDistributionArray_.verticalBins());

		pointsDistributionArray_(xBin, yBin).push_back(Index32(n));
	}

	return true;
}

size_t PointDetector::closestPoint(const Vector2& queryPoint, const bool sign, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, const Scalar maxSqrDistance)
{
	size_t bestIndex = size_t(-1);
	Scalar bestSqrDistance = Numeric::maxValue();

	const unsigned int xBinCenter = pointsDistributionArray.horizontalBin(queryPoint.x());
	const unsigned int yBinCenter = pointsDistributionArray.verticalBin(queryPoint.y());

	for (unsigned int xBin = (unsigned int)(std::max(0, int(xBinCenter) - 1)); xBin < std::min(xBinCenter + 2u, pointsDistributionArray.horizontalBins()); ++xBin)
	{
		for (unsigned int yBin = (unsigned int)(std::max(0, int(yBinCenter) - 1)); yBin < std::min(yBinCenter + 2u, pointsDistributionArray.verticalBins()); ++yBin)
		{
			const Indices32& indices = pointsDistributionArray(xBin, yBin);

			for (const Index32& index : indices)
			{
				if (sign != points[index].sign())
				{
					continue;
				}

				const Scalar sqrDistance = queryPoint.sqrDistance(points[index].observation());

				if (sqrDistance > maxSqrDistance)
				{
					continue;
				}

				if (sqrDistance < bestSqrDistance)
				{
					bestIndex = size_t(index);
					bestSqrDistance = sqrDistance;
				}
			}
		}
	}

	return bestIndex;
}

bool PointDetector::closestPoints(const Vector2& queryPoint, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, Index32& closestPointIndex, Index32& secondClosestPointIndex, Scalar& closestSqrDistance, Scalar& secondClosestSqrDistance)
{
	const unsigned int xBinCenter = pointsDistributionArray.horizontalBin(queryPoint.x());
	const unsigned int yBinCenter = pointsDistributionArray.verticalBin(queryPoint.y());

	if (xBinCenter >= pointsDistributionArray.horizontalBins() || yBinCenter >= pointsDistributionArray.verticalBins())
	{
		return false;
	}

	closestPointIndex = Index32(-1);
	secondClosestPointIndex = Index32(-1);

	closestSqrDistance = Numeric::maxValue();
	secondClosestSqrDistance = Numeric::maxValue();

	for (unsigned int xBin = (unsigned int)(std::max(0, int(xBinCenter) - 1)); xBin < std::min(xBinCenter + 2u, pointsDistributionArray.horizontalBins()); ++xBin)
	{
		for (unsigned int yBin = (unsigned int)(std::max(0, int(yBinCenter) - 1)); yBin < std::min(yBinCenter + 2u, pointsDistributionArray.verticalBins()); ++yBin)
		{
			const Indices32& indices = pointsDistributionArray(xBin, yBin);

			for (const Index32& index : indices)
			{
				const Scalar sqrDistance = queryPoint.sqrDistance(points[index].observation());

				if (sqrDistance < closestSqrDistance)
				{
					secondClosestPointIndex = closestPointIndex;
					secondClosestSqrDistance = closestSqrDistance;

					closestPointIndex = index;
					closestSqrDistance = sqrDistance;
				}
				else if (sqrDistance < secondClosestSqrDistance)
				{
					secondClosestPointIndex = index;
					secondClosestSqrDistance = sqrDistance;
				}
			}
		}
	}

	ocean_assert(closestSqrDistance <= secondClosestSqrDistance);

	return closestPointIndex != Index32(-1);
}

bool PointDetector::optimizePoints(const Frame& yFrame, const Points& points, const PointPatterns& pointPatterns, Points& optimizedPoints, Worker* worker) const
{
	ocean_assert(optimizedPoints.empty());
	optimizedPoints.clear();

	Vectors2 patternPoints;
	Vectors2 framePoints;
	Vectors2 trackedFramePoints;

	patternPoints.reserve(points.size());
	framePoints.reserve(points.size());
	trackedFramePoints.reserve(points.size());

	ocean_assert(!pointPatterns.empty());
	ocean_assert(pointPatterns.size() == 1 || pointPatterns.front().radius() < pointPatterns.back().radius());

	const Vector2 pixelOffset = Vector2(Scalar(0.5), Scalar(0.5)); // AdvancedMotion is using pixel center in the top-left corner of a pixel

	ocean_assert(yPointPatternImages_.isValid());

	ocean_assert(yPointPatternImages_.width() == pointPatternImageSize_ * pointPatterns.size());
	ocean_assert(yPointPatternImages_.height() == pointPatternImageSize_ * 2u);

	const Vector2 darkPatternPosition = Vector2(Scalar(pointPatternImageSize_) * Scalar(0.5), Scalar(pointPatternImageSize_) * Scalar(0.5)) - pixelOffset;
	const Vector2 brightPatternPosition = Vector2(Scalar(pointPatternImageSize_) * Scalar(0.5), Scalar(pointPatternImageSize_) * Scalar(1.5)) - pixelOffset;

	Vectors2 debugMoved;
	Vectors2 debugMovedTo;
	Vectors2 debugFlipped;
	Vectors2 debugFailed;

	for (size_t nPattern = 0; nPattern < pointPatterns.size(); ++nPattern)
	{
		const PointPattern& pointPattern = pointPatterns[nPattern];

		patternPoints.clear();
		framePoints.clear();

		const unsigned int pointRadius = pointPattern.radius();

		for (const Point& point : points)
		{
			if (point.radius() != pointRadius)
			{
				continue;
			}

			framePoints.push_back(point.observation() - pixelOffset);

			if (point.strength() > 0.0f)
			{
				patternPoints.push_back(darkPatternPosition);
			}
			else
			{
				patternPoints.push_back(brightPatternPosition);
			}
		}

		if (framePoints.empty())
		{
			continue;
		}

		const Frame yPointPatternImages = yPointPatternImages_.subFrame((unsigned int)(nPattern * pointPatternImageSize_), 0u, pointPatternImageSize_, pointPatternImageSize_ * 2u);

		trackedFramePoints.clear();

		const unsigned int searchRadius = 2u + pointRadius;

		constexpr unsigned int coarsestLayerRadius = 8u;
		ocean_assert(searchRadius <= coarsestLayerRadius);

		if (pointRadius <= 3u)
		{
			if (yFrame.pixels() <= 640u * 480u)
			{
				CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<5u>(yPointPatternImages, yFrame, patternPoints, framePoints, trackedFramePoints, searchRadius, searchRadius, CV::FramePyramid::DM_FILTER_11, coarsestLayerRadius, worker);
			}
			else
			{
				CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<7u>(yPointPatternImages, yFrame, patternPoints, framePoints, trackedFramePoints, searchRadius, searchRadius, CV::FramePyramid::DM_FILTER_11, coarsestLayerRadius, worker);
			}
		}
		else
		{
			ocean_assert(pointRadius <= 5u);
			CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<9u>(yPointPatternImages, yFrame, patternPoints, framePoints, trackedFramePoints, searchRadius, searchRadius, CV::FramePyramid::DM_FILTER_11, coarsestLayerRadius, worker);
		}

		const Box2 boundingBox(Scalar(pointRadius + 1u), Scalar(pointRadius + 1u), Scalar(yFrame.width() - pointRadius - 2u), Scalar(yFrame.height() - pointRadius - 2u));

		for (size_t n = 0; n < framePoints.size(); ++n)
		{
			const Vector2 patternPoint = patternPoints[n] + pixelOffset;
			const Vector2 framePoint = framePoints[n] + pixelOffset;
			const Vector2 trackedFramePoint = trackedFramePoints[n] + pixelOffset;

			if (!boundingBox.isInside(trackedFramePoint))
			{
				// some points may have been moved too close to the frame border
				continue;
			}

			const Scalar sqrDistance = framePoint.sqrDistance(trackedFramePoint);

			const Scalar maxSqrOffset = Numeric::sqr(Scalar(searchRadius - 1u));

			if (sqrDistance <= maxSqrOffset)
			{
				int32_t strength = 0;
				bool strict = false;

				if (pointPattern.determinePointStrength(yFrame, trackedFramePoint, strength, strict))
				{
					const bool oldSign = patternPoint.y() < Scalar(pointPatternImageSize_);

					const bool newSign = strength >= 0;

					// we skip all object points with a flipped sign

					if (oldSign == newSign)
					{
						const float normalizedStrength = float(strength) / float(pointPattern.positiveOffsets().size()); // average of square differences

						optimizedPoints.emplace_back(trackedFramePoint, pointPattern.radius(), normalizedStrength);
					}
					else
					{
						if constexpr (CalibrationDebugElements::allowDebugging_)
						{
							debugFlipped.push_back(framePoint);
						}
					}
				}
				else
				{
					if constexpr (CalibrationDebugElements::allowDebugging_)
					{
						debugFailed.push_back(framePoint);
					}
				}
			}
			else
			{
				if constexpr (CalibrationDebugElements::allowDebugging_)
				{
					debugMoved.push_back(framePoint);
					debugMovedTo.push_back(trackedFramePoint);
				}
			}
		}
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updatePointDetectorPointsOptimizationPointPatterns(pointPatterns, pointPatternImageSize_);

		CalibrationDebugElements::get().updatePointDetectorPointsOptimization(yFrame, debugMoved, debugMovedTo, debugFlipped, debugFailed);
	}

	return true;
}

bool PointDetector::detectPoints(const Frame& yFrame, const PointPatterns& pointPatterns, const unsigned int minDifference, const unsigned int maxVariance, Points& points, const bool suppressNonMaximum, const unsigned int detectionScaleSteps, Worker* worker)
{
	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));

	if (!yFrame.isValid() || !yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(!pointPatterns.empty());
	if (pointPatterns.empty())
	{
		return false;
	}

	ocean_assert(detectionScaleSteps >= 1u);
	if (detectionScaleSteps < 1u)
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	for (size_t n = 1; n < pointPatterns.size(); ++n)
	{
		ocean_assert(pointPatterns[n - 1].radius() < pointPatterns[n].radius());
	}
#endif

	Frame debugFramePointsCandidates;
	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		if (CalibrationDebugElements::get().isElementActive(CalibrationDebugElements::EI_POINT_DETECTOR_POINTS_CANDIDATES))
		{
			CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, debugFramePointsCandidates, CV::FrameConverter::CP_ALWAYS_COPY);
		}
	}

	constexpr Scalar distance = Scalar(50);

	unsigned int horizontalBins = 0u;
	unsigned int verticalBins = 0u;
	Geometry::SpatialDistribution::idealBinsNeighborhood9(yFrame.width(), yFrame.height(), distance, horizontalBins, verticalBins, 2u, 2u, yFrame.width(), yFrame.height());

	Geometry::SpatialDistribution::DistributionArray darkPointsDistributionArray(Scalar(0), Scalar(0), Scalar(yFrame.width()), Scalar(yFrame.height()), horizontalBins, verticalBins);

	ocean_assert(points.empty());
	points.clear();

	Frame detectionMask;

	size_t pointPatternDetectionIndex = pointPatterns.size() - 1;

	while (true)
	{
		ocean_assert(pointPatternDetectionIndex < pointPatterns.size());
		const PointPattern& detectionPointPattern = pointPatterns[pointPatternDetectionIndex];

		ocean_assert(detectionPointPattern.isValid());
		if (!detectionPointPattern.isValid())
		{
			return false;
		}

		ocean_assert(detectionPointPattern.diameter() <= yFrame.width() && detectionPointPattern.diameter() <= yFrame.height());
		if (detectionPointPattern.diameter() > yFrame.width() || detectionPointPattern.diameter() > yFrame.height())
		{
			return false;
		}

		ocean_assert(minDifference <= 32u);
		if (minDifference > 32u)
		{
			return false;
		}

		darkPointsDistributionArray.clear();

		CV::NonMaximumSuppression<uint32_t> nonMaximumSuppression(yFrame.width(), yFrame.height());

		const uint8_t* mask = nullptr;

		if (detectionMask.isValid())
		{
			mask = detectionMask.constdata<uint8_t>();
			ocean_assert(detectionMask.isContinuous());
		}

		const size_t iterationFirstPointIndex = points.size();

		for (const bool detectDarkPoints : {true, false})
		{
			if (detectDarkPoints)
			{
				detectPointCandidates<true>(yFrame.constdata<uint8_t>(), yFrame.paddingElements(), mask, detectionPointPattern, uint8_t(minDifference), maxVariance, nonMaximumSuppression, worker);
			}
			else
			{
				nonMaximumSuppression.reset();
				detectPointCandidates<false>(yFrame.constdata<uint8_t>(), yFrame.paddingElements(), mask, detectionPointPattern, uint8_t(minDifference), maxVariance, nonMaximumSuppression, worker);
			}

			using StrengthPositions = NonMaximumSuppression<uint32_t>::StrengthPositions<unsigned int, uint32_t>;
			using StrengthPosition = StrengthPositions::value_type;

			StrengthPositions strengthPositions;

			if (suppressNonMaximum)
			{
				nonMaximumSuppression.suppressNonMaximum<unsigned int, uint32_t, false>(0u, yFrame.width(), 0u, yFrame.height(), strengthPositions, worker);
			}
			else
			{
				nonMaximumSuppression.candidates(0u, yFrame.width(), 0u, yFrame.height(), strengthPositions);
			}

			if constexpr (CalibrationDebugElements::allowDebugging_)
			{
				if (debugFramePointsCandidates)
				{
					StrengthPositions debugStrengthPositions;
					nonMaximumSuppression.candidates(0u, yFrame.width(), 0u, yFrame.height(), debugStrengthPositions);

					const uint8_t* color = detectDarkPoints ? CV::Canvas::green() : CV::Canvas::blue();

					for (const StrengthPosition& strengthPosition : debugStrengthPositions)
					{
						CV::Canvas::ellipse(debugFramePointsCandidates, CV::PixelPosition(strengthPosition.x(), strengthPosition.y()), 3u, 3u, color);
					}
				}
			}

			points.reserve(points.size() + strengthPositions.size());

			for (const StrengthPosition& strengthPosition : strengthPositions)
			{
				const Vector2 observation(Scalar(strengthPosition.x()), Scalar(strengthPosition.y()));

				if (detectDarkPoints)
				{
					const unsigned int xBin = darkPointsDistributionArray.horizontalBin(observation.x());
					const unsigned int yBin = darkPointsDistributionArray.verticalBin(observation.y());

					ocean_assert(xBin < horizontalBins);
					ocean_assert(yBin < verticalBins);

					darkPointsDistributionArray(xBin, yBin).push_back(Index32(points.size()));
				}
				else
				{
					if (hasClosePoint(observation, darkPointsDistributionArray, points, Numeric::sqr(4)))
					{
						// a dark point has already been detected in the direct neighborhood, so we ignore this detection

						continue;
					}
				}

				unsigned int radius = detectionPointPattern.radius();
				unsigned int strength = strengthPosition.strength();

				if (pointPatterns.size() >= 2)
				{
					if (detectDarkPoints)
					{
						determinePointRadius<true>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), CV::PixelPosition(strengthPosition.x(), strengthPosition.y()), detectionPointPattern.radius(), pointPatterns.data(), pointPatterns.size(), uint8_t(minDifference), maxVariance, radius, strength);
					}
					else
					{
						determinePointRadius<false>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), CV::PixelPosition(strengthPosition.x(), strengthPosition.y()), detectionPointPattern.radius(), pointPatterns.data(), pointPatterns.size(), uint8_t(minDifference), maxVariance, radius, strength);
					}
				}

				const float sign = detectDarkPoints ? 1.0f : -1.0f;

				const float normalizedStrength = detectionPointPattern.normalizedStrength(strength); // average of square differences

				points.emplace_back(Vector2(Scalar(strengthPosition.x()), Scalar(strengthPosition.y())), radius, normalizedStrength * sign);
			}
		}

		if (pointPatternDetectionIndex < detectionScaleSteps)
		{
			break;
		}

		pointPatternDetectionIndex -= detectionScaleSteps;

		if (!detectionMask.isValid())
		{
			detectionMask.set(yFrame.frameType(), true /*forceOwner*/, true /*forceWritable*/);
			detectionMask.setValue(0xFFu);
		}

		for (size_t nPoint = iterationFirstPointIndex; nPoint < points.size(); ++nPoint)
		{
			const Point& point = points[nPoint];

			ocean_assert(Scalar(int(point.observation().x())) == point.observation().x());
			ocean_assert(Scalar(int(point.observation().y())) == point.observation().y());

			const unsigned int radius = point.radius() * 3u / 2u; // radius * 1.5

			const int pointLeft = int(point.observation().x()) - int(radius);
			const int pointTop = int(point.observation().y()) - int(radius);

			const unsigned int pointWidth = radius * 2u + 1u;
			const unsigned int pointHeight = radius * 2u + 1u;

			constexpr uint8_t black = 0x00u;
			CV::Canvas::rectangle8BitPerChannel<1u>(detectionMask.data<uint8_t>(), yFrame.width(), yFrame.height(), pointLeft, pointTop, pointWidth, pointHeight, &black);
		}
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		if (debugFramePointsCandidates)
		{
			CalibrationDebugElements::get().updateElement(CalibrationDebugElements::EI_POINT_DETECTOR_POINTS_CANDIDATES, std::move(debugFramePointsCandidates));
		}
	}

	return true;
}


template <bool tDarkPoint>
void PointDetector::detectPointCandidates(const uint8_t* yFrame, const unsigned int yFramePaddingElements, const uint8_t* mask, const PointPattern& pointPattern, const uint8_t minDifference, const unsigned int maxVariance, CV::NonMaximumSuppression<uint32_t>& nonMaximumSuppression, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(pointPattern.isValid());

	ocean_assert(pointPattern.diameter() <= nonMaximumSuppression.width());
	ocean_assert(pointPattern.diameter() <= nonMaximumSuppression.height());

	const unsigned int firstColumn = pointPattern.radius();
	const unsigned int numberColumns = nonMaximumSuppression.width() - 2u * pointPattern.radius();

	const unsigned int firstRow = pointPattern.radius();
	const unsigned int numberRows = nonMaximumSuppression.height() - 2u * pointPattern.radius();

	if (worker != nullptr)
	{
		if (mask != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(&PointDetector::detectPointCandidatesSubset<tDarkPoint, true>, yFrame, yFramePaddingElements, mask, &pointPattern, minDifference, maxVariance, &nonMaximumSuppression, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows);
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&PointDetector::detectPointCandidatesSubset<tDarkPoint, false>, yFrame, yFramePaddingElements, mask, &pointPattern, minDifference, maxVariance, &nonMaximumSuppression, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows);
		}
	}
	else
	{
		if (mask != nullptr)
		{
			detectPointCandidatesSubset<tDarkPoint, true>(yFrame, yFramePaddingElements, mask, &pointPattern, minDifference, maxVariance, &nonMaximumSuppression, firstColumn, numberColumns, firstRow, numberRows);
		}
		else
		{
			detectPointCandidatesSubset<tDarkPoint, false>(yFrame, yFramePaddingElements, mask, &pointPattern, minDifference, maxVariance, &nonMaximumSuppression, firstColumn, numberColumns, firstRow, numberRows);
		}
	}
}

template <bool tDarkPoint, bool tUseMask>
void PointDetector::detectPointCandidatesSubset(const uint8_t* yFrame, const unsigned int yFramePaddingElements, const uint8_t* mask, const PointPattern* pointPattern, const uint8_t minDifference, const unsigned int maxVariance, CV::NonMaximumSuppression<uint32_t>* nonMaximumSuppression, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(pointPattern != nullptr && pointPattern->isValid());
	ocean_assert(nonMaximumSuppression != nullptr && nonMaximumSuppression->width() != 0u && nonMaximumSuppression->height() != 0u);

	if constexpr (tUseMask)
	{
		ocean_assert(mask != nullptr);
	}
	else
	{
		ocean_assert(mask == nullptr);
	}

	const unsigned int yFrameStrideElements = nonMaximumSuppression->width() + yFramePaddingElements;

	ocean_assert(pointPattern->frameStrideElements() == yFrameStrideElements);

	const Index32 negativeOffset = pointPattern->negativeOffset();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const uint8_t* centerPixel = yFrame + y * yFrameStrideElements + firstColumn;
		const uint8_t* firstSurroundingPixel = centerPixel - negativeOffset;
		ocean_assert(yFrame <= firstSurroundingPixel);

		const uint8_t* maskPixel = tUseMask ? mask + y * nonMaximumSuppression->width() + firstColumn : nullptr;

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if constexpr (tUseMask)
			{
				if (*maskPixel++ == 0u)
				{
					// the pixel is not of interest, let's skip it

					++centerPixel;
					++firstSurroundingPixel;

					continue;
				}
			}

			const uint8_t centerPixelValue = *centerPixel;

			if constexpr (tDarkPoint)
			{
				constexpr uint8_t maxCenterColorFixed = 0xFFu * 75u / 100u; // 75% of white
				constexpr uint8_t minSurroundingColorFixed = 0xFFu * 25u / 100u; // 25% of white

				const uint32_t strength = pointPattern->determineDarkPointStrength<maxCenterColorFixed, minSurroundingColorFixed>(centerPixelValue, firstSurroundingPixel, minDifference, maxVariance);

				if (strength != 0u)
				{
					nonMaximumSuppression->addCandidate(x, y, strength);
				}
			}
			else
			{
				constexpr uint8_t minCenterColorFixed = 0xFFu * 25u / 100u; // 25% of white
				constexpr uint8_t maxSurroundingColorFixed = 0xFFu * 75u / 100u; // 75% of white

				const uint32_t strength = pointPattern->determineBrightPointStrength<minCenterColorFixed, maxSurroundingColorFixed>(centerPixelValue, firstSurroundingPixel, minDifference, maxVariance);

				if (strength != 0u)
				{
					nonMaximumSuppression->addCandidate(x, y, strength);
				}
			}

			++centerPixel;
			++firstSurroundingPixel;
		}

		ocean_assert(centerPixel == yFrame + y * yFrameStrideElements + firstColumn + numberColumns);
	}
}

template <bool tDarkPoint>
bool PointDetector::determinePointRadius(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const CV::PixelPosition& pixelPosition, const unsigned int currentRadius, const PointPattern* pointPatterns, const size_t numberPointPatterns, const uint8_t minDifference, const unsigned int maxVariance, unsigned int& radius, unsigned int& strength)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(pointPatterns != nullptr);
	ocean_assert(numberPointPatterns >= 2);
	ocean_assert(pointPatterns[0].radius() < pointPatterns[numberPointPatterns - 1].radius());

	ocean_assert(pixelPosition.x() < width && pixelPosition.y() < height);

	const unsigned int yFrameStrideElements = width + yFramePaddingElements;

	const uint8_t* yPoint = yFrame + pixelPosition.y() * yFrameStrideElements + pixelPosition.x();

	for (size_t nPointPattern = 0; nPointPattern < numberPointPatterns; ++nPointPattern)
	{
		const PointPattern& pointPattern = pointPatterns[nPointPattern];

		ocean_assert(pointPattern.isValid());
		ocean_assert(pointPattern.frameStrideElements() == yFrameStrideElements);

		if (currentRadius <= pointPattern.radius())
		{
			// the radius is not smaller
			break;
		}

		if (pixelPosition.x() < pointPattern.radius() || pixelPosition.y() < pointPattern.radius() || pixelPosition.x() + pointPattern.radius() >= width || pixelPosition.y() + pointPattern.radius() >= height)
		{
			break;
		}

		if constexpr (tDarkPoint)
		{
			constexpr uint8_t maxCenterColorFixed = 0xFFu * 75u / 100u; // 75% of white
			constexpr uint8_t minSurroundingColorFixed = 0xFFu * 25u / 100u; // 25% of white

			const uint32_t pointStrength = pointPattern.determineDarkPointStrength<maxCenterColorFixed, minSurroundingColorFixed>(yPoint, minDifference, maxVariance);

			if (pointStrength != 0u)
			{
				radius = pointPattern.radius();
				strength = pointStrength;

				return true;
			}
		}
		else
		{
			constexpr uint8_t minCenterColorFixed = 0xFFu * 25u / 100u; // 25% of white
			constexpr uint8_t maxSurroundingColorFixed = 0xFFu * 75u / 100u; // 75% of white

			const uint32_t pointStrength = pointPattern.determineBrightPointStrength<minCenterColorFixed, maxSurroundingColorFixed>(yPoint, minDifference, maxVariance);

			if (pointStrength != 0u)
			{
				radius = pointPattern.radius();
				strength = pointStrength;

				return true;
			}
		}
	}

	return false;
}

void PointDetector::removeDuplicatedPoints(const unsigned int width, const unsigned int height, Points& points, const Scalar maxDistance)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(points.size() >= 2);
	ocean_assert(maxDistance >= 0);

	constexpr Scalar distance = Scalar(10);

	unsigned int horizontalBins = 0u;
	unsigned int verticalBins = 0u;
	Geometry::SpatialDistribution::idealBinsNeighborhood9(width, height, distance, horizontalBins, verticalBins, 2u, 2u, 50u, 50u);

	Geometry::SpatialDistribution::DistributionArray pointsDistributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);

	std::vector<uint8_t> duplicatedPoints(points.size(), 0u);

	Indices32 neighborPointIndices;
	neighborPointIndices.reserve(32);

	for (size_t pointIndex = 0; pointIndex < points.size(); ++pointIndex)
	{
		const Vector2& point = points[pointIndex].observation();

		const unsigned int xBin = pointsDistributionArray.horizontalBin(point.x());
		const unsigned int yBin = pointsDistributionArray.verticalBin(point.y());

		ocean_assert(xBin < horizontalBins);
		ocean_assert(yBin < verticalBins);

		pointsDistributionArray(xBin, yBin).push_back(Index32(pointIndex));
	}

	for (size_t pointIndex = 0; pointIndex < points.size(); ++pointIndex)
	{
		if (duplicatedPoints[pointIndex] != 0u)
		{
			// the point is already marked as duplicated
			continue;
		}

		const Point& point = points[pointIndex];

		neighborPointIndices.clear();
		if (closestPoints(point.observation(), pointsDistributionArray, points, Numeric::sqr(maxDistance), neighborPointIndices))
		{
			if (neighborPointIndices.size() == 1)
			{
				ocean_assert(neighborPointIndices[0] == pointIndex);
				continue;
			}

			for (const Index32& neighborIndex : neighborPointIndices)
			{
				if (neighborIndex == pointIndex)
				{
					continue;
				}

				if (duplicatedPoints[neighborIndex] != 0u)
				{
					// the point is already marked as duplicated
					continue;
				}

				if (NumericF::abs(point.strength()) < NumericF::abs(points[neighborIndex].strength()))
				{
					duplicatedPoints[pointIndex] = 1u;
					break;
				}
				else
				{
					duplicatedPoints[neighborIndex] = 1u;
				}
			}
		}
		else
		{
			ocean_assert(false && "This should never happen!");
		}
	}

	const size_t debugInitialPoints = points.size();

	for (size_t pointIndex = points.size() - 1; pointIndex < points.size(); --pointIndex)
	{
		if (duplicatedPoints[pointIndex] != 0u)
		{
			points[pointIndex] = std::move(points.back());
			points.pop_back();
		}
	}

	const size_t debugFilteredPoints = debugInitialPoints - points.size();

	if (debugFilteredPoints != 0)
	{
		Log::debug() << "Removed points: " << debugFilteredPoints << " duplicates from " << debugInitialPoints;
	}
}

bool PointDetector::hasClosePoint(const Vector2& queryPoint, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, const Scalar maxSqrDistance)
{
	const unsigned int xBinCenter = pointsDistributionArray.horizontalBin(queryPoint.x());
	const unsigned int yBinCenter = pointsDistributionArray.verticalBin(queryPoint.y());

	for (unsigned int xBin = (unsigned int)(std::max(0, int(xBinCenter) - 1)); xBin < std::min(xBinCenter + 2u, pointsDistributionArray.horizontalBins()); ++xBin)
	{
		for (unsigned int yBin = (unsigned int)(std::max(0, int(yBinCenter) - 1)); yBin < std::min(yBinCenter + 2u, pointsDistributionArray.verticalBins()); ++yBin)
		{
			const Indices32& indices = pointsDistributionArray(xBin, yBin);

			for (const Index32& index : indices)
			{
				const Scalar sqrDistance = queryPoint.sqrDistance(points[index].observation());

				if (sqrDistance <= maxSqrDistance)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool PointDetector::closestPoints(const Vector2& queryPoint, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, const Scalar maxSqrDistance, Indices32& pointIndices)
{
	ocean_assert(pointIndices.empty());

	const unsigned int xBinCenter = pointsDistributionArray.horizontalBin(queryPoint.x());
	const unsigned int yBinCenter = pointsDistributionArray.verticalBin(queryPoint.y());

	if (xBinCenter >= pointsDistributionArray.horizontalBins() || yBinCenter >= pointsDistributionArray.verticalBins())
	{
		return false;
	}

	for (unsigned int xBin = (unsigned int)(std::max(0, int(xBinCenter) - 1)); xBin < std::min(xBinCenter + 2u, pointsDistributionArray.horizontalBins()); ++xBin)
	{
		for (unsigned int yBin = (unsigned int)(std::max(0, int(yBinCenter) - 1)); yBin < std::min(yBinCenter + 2u, pointsDistributionArray.verticalBins()); ++yBin)
		{
			const Indices32& indices = pointsDistributionArray(xBin, yBin);

			for (const Index32& index : indices)
			{
				const Scalar sqrDistance = queryPoint.sqrDistance(points[index].observation());

				if (sqrDistance > maxSqrDistance)
				{
					continue;
				}

				pointIndices.push_back(index);
			}
		}
	}

	return !pointIndices.empty();
}

PointDetector::PointPatterns PointDetector::createPointPatterns(const unsigned int radius, const unsigned int innerRadius,  const bool useCircle, const unsigned int frameStrideElements)
{
	ocean_assert(radius >= 1u);
	ocean_assert(innerRadius < radius);

	PointPatterns pointPatterns;
	pointPatterns.reserve(8);

	for (unsigned int r = 1u; r < radius; ++r)
	{
		const unsigned int ir = std::min(r - 1u, innerRadius);

		PointDetector::PointPattern pointPattern(r, ir, frameStrideElements, useCircle);

		// let's ensure that we use enough pixels in the pattern
		if (pointPattern.offsets().size() < 8)
		{
			continue;
		}

		pointPatterns.emplace_back(std::move(pointPattern));
	}

	pointPatterns.emplace_back(radius, innerRadius, frameStrideElements, useCircle);

	return pointPatterns;
}

void PointDetector::updatePointPatterns(PointPatterns& pointPatterns, const unsigned int frameStrideElements)
{
	ocean_assert(!pointPatterns.empty());

	for (PointPattern& pointPattern : pointPatterns)
	{
		if (pointPattern.frameStrideElements() != frameStrideElements)
		{
			pointPattern = PointPattern(pointPattern.radius(), pointPattern.innerRadius(), frameStrideElements, pointPattern.isCircle());
		}
	}
}

bool PointDetector::paintPointPattern(Frame& yFrame, const unsigned int radius, const uint8_t pointColor)
{
	ocean_assert(yFrame.isValid() && yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8));

	if (!yFrame.isValid() || !yFrame.isPixelFormatDataLayoutCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(yFrame.width() % 2u == 1u && yFrame.height() % 2u == 1u);
	if (yFrame.width() % 2u == 0u || yFrame.height() % 2u == 0u)
	{
		return false;
	}

	const unsigned int diameter = radius * 2u + 1u;

	ocean_assert(yFrame.width() >= diameter && yFrame.height() >= diameter);
	if (yFrame.width() < diameter || yFrame.height() < diameter)
	{
		return false;
	}

	ocean_assert(pointColor == 0xFFu || pointColor == 0x00u);

	const uint8_t backgroundColor = 0xFFu - pointColor;

	yFrame.setValue(backgroundColor);

	const unsigned int xCenter = yFrame.width() / 2u;
	const unsigned int yCenter = yFrame.height() / 2u;

	Indices32 coefficients(diameter);

	for (unsigned int n = 0u; n < diameter; ++n)
	{
		coefficients[n] = NumericT<unsigned int>::binomialCoefficient(diameter - 1u, n);
	}

	const unsigned int maxCoefficient = coefficients[diameter / 2u];
	const float maxCoefficientSquared = float(maxCoefficient * maxCoefficient);

	constexpr float factor = 1.0f;

	const float normalization = 1.0f / NumericF::pow(maxCoefficientSquared, factor);

	for (unsigned int y = 0u; y < diameter; ++y)
	{
		uint8_t* row = yFrame.pixel<uint8_t>(xCenter - radius, yCenter - radius + y);

		for (unsigned int x = 0u; x < diameter; ++x)
		{
			const float coefficient = float(coefficients[x] * coefficients[y]);

			const float adjustedCoefficient = NumericF::pow(coefficient, factor);
			const float colorF = adjustedCoefficient * normalization * 255.0f;
			ocean_assert(colorF >= 0.0 && colorF <= 255.5);

			const unsigned int color = (unsigned int)(colorF);

			if (pointColor == 0xFFu)
			{
				row[x] = uint8_t(color);
			}
			else
			{
				row[x] = 0xFFu - uint8_t(color);
			}
		}
	}

	return true;
}

}

}

}
