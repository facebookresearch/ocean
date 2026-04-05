/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/PointDetector.h"
#include "ocean/cv/calibration/CalibrationDebugElements.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/NEON.h"
#include "ocean/cv/SSE.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

bool PointDetector::PointBorderOffsets::update(const unsigned int filterSize, const BorderShape borderShape, const unsigned int width, const unsigned int paddingElements)
{
	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);
	ocean_assert(borderShape != BS_INVALID);

	if (filterSize == filterSize_ && borderShape == borderShape_ && width == width_ && paddingElements == paddingElements_)
	{
		return true;
	}

	const unsigned int strideElements = width + paddingElements;

	offsets_.clear();

	if (!determineBorderOffsets(strideElements, filterSize, borderShape, offsets_))
	{
		return false;
	}

	filterSize_ = filterSize;
	borderShape_ = borderShape;
	width_ = width;
	paddingElements_ = paddingElements;

	return true;
}

bool PointDetector::PointBorderOffsets::determineBorderOffsets(const unsigned int filterSize, const BorderShape borderShape, CV::PixelPositionsI& borderOffsets)
{
	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);

	ocean_assert(borderOffsets.empty());

	switch (borderShape)
	{
		case BS_SQUARE:
		{
			/*
			 * Detection pattern with filter size 7x7:
			 * 7 * 2 + 5 * 2 = 24 border locations, 1 center location
			 *
			 * Border location 'B',                 Border offset indices
			 * Center location 'C'
			 * B  B  B  B  B  B  B                  0  1  2  3  4  5  6
			 * B                 B                  7                 8
			 * B                 B                  9                10
			 * B        C        B                 11        C       12
			 * B                 B                 13                14
			 * B                 B                 15                16
			 * B  B  B  B  B  B  B                 17 18 19 20 21 22 23
			 */

			const unsigned int filterSize_2 = filterSize / 2u;
			const unsigned int numberBorderOffsets = filterSize * 2u + (filterSize - 2u) * 2u;

			borderOffsets.reserve(numberBorderOffsets);

			for (int x = -int(filterSize_2); x <= int(filterSize_2); ++x)
			{
				borderOffsets.emplace_back(x, -int(filterSize_2));
			}

			for (int y = -int(filterSize_2) + 1; y < int(filterSize_2); ++y)
			{
				borderOffsets.emplace_back(-int(filterSize_2), y);
				borderOffsets.emplace_back(int(filterSize_2), y);
			}

			for (int x = -int(filterSize_2); x <= int(filterSize_2); ++x)
			{
				borderOffsets.emplace_back(x, int(filterSize_2));
			}

			ocean_assert(borderOffsets.size() == numberBorderOffsets);

			return true;
		}

		case BS_CIRCLE:
		{
			const unsigned int filterSize_2 = filterSize / 2u;

			CV::Bresenham::circlePixels(0, 0, filterSize_2, borderOffsets);

			std::sort(borderOffsets.begin(), borderOffsets.end());
			ocean_assert(borderOffsets.front().y() < borderOffsets.back().y());

			return true;
		}

		case BS_INVALID:
			break;
	}

	ocean_assert(false && "Invalid border shape!");
	return false;
}

bool PointDetector::PointBorderOffsets::determineBorderOffsets(const unsigned int strideElements, const unsigned int filterSize, const BorderShape borderShape, Indices32& unsignedBorderOffsets)
{
	ocean_assert(strideElements >= filterSize);

	ocean_assert(unsignedBorderOffsets.empty());

	CV::PixelPositionsI borderOffsets;
	if (!determineBorderOffsets(filterSize, borderShape, borderOffsets))
	{
		return false;
	}

	unsignedBorderOffsets.reserve(borderOffsets.size());

	// the very first offset is a negative offset to the top-left start position of the filter

	CV::PixelPositionI previousBorderOffset = borderOffsets.front();
	ocean_assert(previousBorderOffset.y() < 0 && previousBorderOffset.x() <= 0);

	unsignedBorderOffsets.push_back(-previousBorderOffset.y() * strideElements - previousBorderOffset.x());
	ocean_assert(unsignedBorderOffsets.front() <= strideElements * filterSize);

	for (size_t n = 1; n < borderOffsets.size(); ++n)
	{
		const CV::PixelPositionI& borderOffset = borderOffsets[n];
		const CV::PixelPositionI relativeBorderOffset = borderOffset - previousBorderOffset;
		ocean_assert(relativeBorderOffset.y() >= 0);

		const unsigned int relativeOffset = relativeBorderOffset.y() * strideElements + relativeBorderOffset.x();
		ocean_assert(relativeOffset > 0u);

		unsignedBorderOffsets.push_back(relativeOffset);
		previousBorderOffset = borderOffset;
	}

	return true;
}

PointDetector::PointPeakDetector::PointPeakDetector(const Frame& yFrame) :
	yFrame_(yFrame)
{
	// nothing to do here
}

bool PointDetector::PointPeakDetector::determinePrecisePosition(const unsigned int x, const unsigned int y, const int32_t strength, Scalar& preciseX, Scalar& preciseY, int32_t& preciseStrength) const
{
	ocean_assert(x >= 1u && y >= 1u);

	ocean_assert(yFrame_.isValid() && yFrame_.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	constexpr unsigned int maxIterations = 10u;
	constexpr float stepSize = 1.0f;
	constexpr float convergenceThreshold = 0.01f;

	float internalPreciseX = NumericF::minValue();
	float internalPreciseY = NumericF::minValue();

	CV::NonMaximumSuppression::RefinementStatus refinementStatus = CV::NonMaximumSuppression::RS_INVALID;

	if (strength >= 0)
	{
		refinementStatus = NonMaximumSuppressionVote::determinePrecisePeakLocationIterativeNxN<float, 7u, false, CV::PC_TOP_LEFT>(yFrame_.constdata<uint8_t>(), yFrame_.width(), yFrame_.height(), yFrame_.paddingElements(), float(x), float(y), internalPreciseX, internalPreciseY, maxIterations, stepSize, convergenceThreshold);
	}
	else
	{
		refinementStatus = NonMaximumSuppressionVote::determinePrecisePeakLocationIterativeNxN<float, 7u, true, CV::PC_TOP_LEFT>(yFrame_.constdata<uint8_t>(), yFrame_.width(), yFrame_.height(), yFrame_.paddingElements(), float(x), float(y), internalPreciseX, internalPreciseY, maxIterations, stepSize, convergenceThreshold);
	}

	if (refinementStatus == CV::NonMaximumSuppression::RS_CONVERGED || refinementStatus == CV::NonMaximumSuppression::RS_MAX_ITERATIONS)
	{
		ocean_assert(internalPreciseX != NumericF::minValue());
		ocean_assert(internalPreciseY != NumericF::minValue());

		preciseX = Scalar(internalPreciseX);
		preciseY = Scalar(internalPreciseY);
	}
	else
	{
		preciseX = Scalar(x);
		preciseY = Scalar(y);
	}

	preciseStrength = strength;
	return true;
}

bool PointDetector::detectPoints(const Frame& yFrame, Worker* worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	if (!yFrame.isValid() || !yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(filterSize_ >= 3u && filterSize_ % 2u == 1u);
	if (filterSize_ < 3u || filterSize_ % 2u != 1u)
	{
		return false;
	}

	if (!pointBorderOffsets_.update(filterSize_, BS_CIRCLE, yFrame.width(), yFrame.paddingElements()))
	{
		return false;
	}

	ocean_assert(minimalDifference_ > 0);
	ocean_assert(minimalDifference_ <= maximalDifference_);

	points_.clear();

	if (!detectPoints(yFrame, points_, pointBorderOffsets_.offsets().data(), pointBorderOffsets_.offsets().size(), filterSize_, minimalDifference_, maximalDifference_, worker))
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

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updatePointDetectorPointsRedundantRemoved(yFrame, points_);
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

bool PointDetector::detectPoints(const Frame& yFrame, const unsigned int filterSize, Points& points, const BorderShape borderShape, const int32_t minimalDifference, const int32_t maximalDifference, Worker* worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);
	ocean_assert(maximalDifference > 0);

	Indices32 borderOffsets;
	if (!PointBorderOffsets::determineBorderOffsets(yFrame.strideElements(), filterSize, borderShape, borderOffsets))
	{
		return false;
	}

	return detectPoints(yFrame, points, borderOffsets.data(), borderOffsets.size(), filterSize, minimalDifference, maximalDifference, worker);
}

bool PointDetector::detectPoints(const Frame& yFrame, Points& points, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference, Worker* worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);
	ocean_assert(maximalDifference > 0);

	if (!yFrame.isValid() || !yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(filterSize < yFrame.width() && filterSize < yFrame.height());
	if (filterSize >= yFrame.width() || filterSize >= yFrame.height())
	{
		return false;
	}

	constexpr unsigned int simdBlockSize = 8u;

	ocean_assert(filterSize - 1u + simdBlockSize <= yFrame.width());
	if (filterSize - 1u + simdBlockSize > yFrame.width())
	{
		return false;
	}

	NonMaximumSuppressionVote nonMaximumSuppression(yFrame.width(), yFrame.height());

	const unsigned int filterSize_2 = filterSize / 2u;

	const unsigned int firstRow = filterSize_2;
	const unsigned int numberRows = nonMaximumSuppression.height() - 2u * filterSize_2;

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(&PointDetector::detectPointCandidatesSubset, &yFrame, &nonMaximumSuppression, borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference, 0u, 0u), firstRow, numberRows);
	}
	else
	{
		detectPointCandidatesSubset(&yFrame, &nonMaximumSuppression, borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference, firstRow, numberRows);
	}

	if constexpr (CalibrationDebugElements::allowDebugging_)
	{
		CalibrationDebugElements::get().updatePointDetectorPointsNonSuppressed(yFrame, nonMaximumSuppression);
	}

	const PointPeakDetector pointPeakDetector(yFrame);

	const NonMaximumSuppressionVote::PositionCallback<Scalar, int32_t> precisePositionCallback = std::bind(&PointPeakDetector::determinePrecisePosition, &pointPeakDetector, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

	NonMaximumSuppressionVote::StrengthPositions<Scalar, int32_t> strengthPositions;
	nonMaximumSuppression.suppressNonMaximum<Scalar, int32_t, false, NonMaximumSuppression::SM_MAXIMUM_POSITIVE_ONLY>(0u, yFrame.width(), 0u, yFrame.height(), strengthPositions, worker, &precisePositionCallback);

	points.clear();
	points.reserve(strengthPositions.size() * 3);

	constexpr Scalar pixelCenterOffset = Scalar(0.5);

	for (const NonMaximumSuppressionVote::StrengthPosition<Scalar, int32_t>& strengthPosition : strengthPositions)
	{
		points.emplace_back(Vector2(strengthPosition.x() + pixelCenterOffset, strengthPosition.y() + pixelCenterOffset), filterSize_2, float(strengthPosition.strength()));
	}

	strengthPositions.clear();
	nonMaximumSuppression.suppressNonMaximum<Scalar, int32_t, false, NonMaximumSuppression::SM_MINIMUM_NEGATIVE_ONLY>(0u, yFrame.width(), 0u, yFrame.height(), strengthPositions, worker, &precisePositionCallback);

	for (const NonMaximumSuppressionVote::StrengthPosition<Scalar, int32_t>& strengthPosition : strengthPositions)
	{
		points.emplace_back(Vector2(strengthPosition.x() + pixelCenterOffset, strengthPosition.y() + pixelCenterOffset), filterSize_2, float(strengthPosition.strength()));
	}

	return true;
}

bool PointDetector::createFilterResponseFrame(const Frame& yFrame, Frame& responseFrame, const unsigned int filterSize, const BorderShape borderShape, const int32_t minimalDifference, const int32_t maximalDifference, Worker* worker)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);
	ocean_assert(maximalDifference > 0);

	if (!yFrame.isValid() || !yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(filterSize < yFrame.width() && filterSize < yFrame.height());
	if (filterSize >= yFrame.width() || filterSize >= yFrame.height())
	{
		return false;
	}

	constexpr unsigned int simdBlockSize = 8u;

	ocean_assert(filterSize - 1u + simdBlockSize <= yFrame.width());
	if (filterSize - 1u + simdBlockSize > yFrame.width())
	{
		return false;
	}

	responseFrame.set(FrameType(yFrame, FrameType::genericPixelFormat<int32_t, 1u>()), false, true);

	Indices32 borderOffsets;
	if (!PointBorderOffsets::determineBorderOffsets(yFrame.strideElements(), filterSize, borderShape, borderOffsets))
	{
		return false;
	}

	if (worker != nullptr)
	{
		const uint32_t* borderOffsetsData = borderOffsets.data();

		worker->executeFunction(Worker::Function::createStatic(&PointDetector::createFilterResponseFrameSubset, &yFrame, &responseFrame, borderOffsetsData, borderOffsets.size(), filterSize, minimalDifference, maximalDifference, 0u, 0u), 0u, yFrame.height());
	}
	else
	{
		createFilterResponseFrameSubset(&yFrame, &responseFrame, borderOffsets.data(), borderOffsets.size(), filterSize, minimalDifference, maximalDifference, 0u, yFrame.height());
	}

	return true;
}

size_t PointDetector::closestPoint(const Vector2& queryPoint, const bool sign, const Geometry::SpatialDistribution::DistributionArray& pointsDistributionArray, const Points& points, const Scalar maxSqrDistance)
{
	const unsigned int xBinCenter = pointsDistributionArray.horizontalBin(queryPoint.x());
	const unsigned int yBinCenter = pointsDistributionArray.verticalBin(queryPoint.y());

	if (xBinCenter >= pointsDistributionArray.horizontalBins() || yBinCenter >= pointsDistributionArray.verticalBins())
	{
		return size_t(-1);
	}

	size_t bestIndex = size_t(-1);
	Scalar bestSqrDistance = Numeric::maxValue();

	for (unsigned int xBin = pointsDistributionArray.beginBinHorizontal<1u>(xBinCenter); xBin < pointsDistributionArray.endBinHorizontal<1u>(xBinCenter); ++xBin)
	{
		for (unsigned int yBin = pointsDistributionArray.beginBinVertical<1u>(yBinCenter); yBin < pointsDistributionArray.endBinVertical<1u>(yBinCenter); ++yBin)
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

	for (unsigned int xBin = pointsDistributionArray.beginBinHorizontal<1u>(xBinCenter); xBin < pointsDistributionArray.endBinHorizontal<1u>(xBinCenter); ++xBin)
	{
		for (unsigned int yBin = pointsDistributionArray.beginBinVertical<1u>(yBinCenter); yBin < pointsDistributionArray.endBinVertical<1u>(yBinCenter); ++yBin)
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

	for (unsigned int xBin = pointsDistributionArray.beginBinHorizontal<1u>(xBinCenter); xBin < pointsDistributionArray.endBinHorizontal<1u>(xBinCenter); ++xBin)
	{
		for (unsigned int yBin = pointsDistributionArray.beginBinVertical<1u>(yBinCenter); yBin < pointsDistributionArray.endBinVertical<1u>(yBinCenter); ++yBin)
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

	for (unsigned int xBin = pointsDistributionArray.beginBinHorizontal<1u>(xBinCenter); xBin < pointsDistributionArray.endBinHorizontal<1u>(xBinCenter); ++xBin)
	{
		for (unsigned int yBin = pointsDistributionArray.beginBinVertical<1u>(yBinCenter); yBin < pointsDistributionArray.endBinVertical<1u>(yBinCenter); ++yBin)
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

void PointDetector::detectPointCandidatesSubset(const Frame* yFrame, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr && yFrame->isValid());
	ocean_assert(nonMaximumSuppression != nullptr && nonMaximumSuppression->height() == yFrame->height());

	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);

	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);

	ocean_assert(maximalDifference > 0);

	ocean_assert(firstRow + numberRows <= yFrame->height());

	/*
	 * Example: square border detection pattern with filter size 7x7:
	 * 7 * 2 + 5 * 2 = 24 border locations, 1 center location
	 * The actual border shape (square or circle) is determined by the provided borderOffsets.
	 *
	 * Border location 'B',                 Border offset indices
	 * Center location 'C'
	 * B  B  B  B  B  B  B                  0  1  2  3  4  5  6
	 * B                 B                  7                 8
	 * B                 B                  9                10
	 * B        C        B                 11        C       12
	 * B                 B                 13                14
	 * B                 B                 15                16
	 * B  B  B  B  B  B  B                 17 18 19 20 21 22 23
	 */

	ocean_assert(filterSize < yFrame->width() && filterSize < yFrame->height());

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert_and_suppress_unused(filterSize_2 >= 1u, filterSize_2);

	ocean_assert(firstRow >= filterSize_2);
	ocean_assert(firstRow + numberRows <= yFrame->height() - filterSize_2);

	constexpr unsigned int simdBlockSize = 8u;
	ocean_assert_and_suppress_unused(yFrame->width() >= filterSize_2 + simdBlockSize + filterSize_2, simdBlockSize);

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
		determinePointCandidatesRowSSEDual(y, yFrame->constrow<uint8_t>(y), nonMaximumSuppression, borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference);
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		determinePointCandidatesRowNEONDual(y, yFrame->constrow<uint8_t>(y), nonMaximumSuppression, borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference);
#else
		determinePointCandidatesRowDual(y, yFrame->constrow<uint8_t>(y), nonMaximumSuppression, borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference);
#endif
	}
}

void PointDetector::createFilterResponseFrameSubset(const Frame* yFrame, Frame* responseFrame, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr && yFrame->isValid());
	ocean_assert(responseFrame != nullptr && responseFrame->isFrameTypeCompatible(FrameType(yFrame->frameType(), FrameType::genericPixelFormat<int32_t, 1u>()), false));

	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);

	ocean_assert(filterSize >= 3u && filterSize % 2u == 1u);

	ocean_assert(maximalDifference > 0);

	ocean_assert(firstRow + numberRows <= yFrame->height());

	/*
	 * Example: square border detection pattern with filter size 7x7:
	 * 7 * 2 + 5 * 2 = 24 border locations, 1 center location
	 * The actual border shape (square or circle) is determined by the provided borderOffsets.
	 *
	 * Border location 'B',                 Border offset indices
	 * Center location 'C'
	 * B  B  B  B  B  B  B                  0  1  2  3  4  5  6
	 * B                 B                  7                 8
	 * B                 B                  9                10
	 * B        C        B                 11        C       12
	 * B                 B                 13                14
	 * B                 B                 15                16
	 * B  B  B  B  B  B  B                 17 18 19 20 21 22 23
	 */

	ocean_assert(filterSize < yFrame->width() && filterSize < yFrame->height());

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(filterSize_2 >= 1u);

	constexpr unsigned int simdBlockSize = 8u;
	ocean_assert_and_suppress_unused(yFrame->width() >= filterSize_2 + simdBlockSize + filterSize_2, simdBlockSize);

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		if (y < filterSize_2 || y >= yFrame->height() - filterSize_2)
		{
			// we are in a top or bottom row

			memset(responseFrame->row<int32_t>(y), 0x00, responseFrame->width() * sizeof(int32_t));
			continue;
		}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
		createFilterResponseRowSSEDual(yFrame->constrow<uint8_t>(y), responseFrame->row<int32_t>(y), yFrame->width(), borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference);
#elif defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		createFilterResponseRowNEONDual(yFrame->constrow<uint8_t>(y), responseFrame->row<int32_t>(y), yFrame->width(), borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference);
#else
		createFilterResponseRowDual(yFrame->constrow<uint8_t>(y), responseFrame->row<int32_t>(y), yFrame->width(), borderOffsets, numberBorderOffsets, filterSize, minimalDifference, maximalDifference);
#endif
	}
}

void PointDetector::determinePointCandidatesRow(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(nonMaximumSuppression != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(maximalDifference > 0);

	const unsigned int filterSize_2 = filterSize / 2u;

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	yRow += filterSize_2;

	for (unsigned int x = filterSize_2; x < nonMaximumSuppression->width() - filterSize_2; ++x)
	{
		const int32_t centerValue = int32_t(*yRow);

		int32_t response = 0;

		const uint8_t* yBorderData = yRow - borderOffsets[0]; // first offset is interpreted as negative offset (to the top-left starting point)

		int32_t difference = int32_t(*yBorderData) - centerValue;

		if (difference >= minimalDifference)
		{
			response = std::min(difference * difference, maxSqrDifference);

			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset]; // all remaining offsets are positive offsets in relation to the previous position

				difference = int32_t(*yBorderData) - centerValue;

				if (difference < minimalDifference)
				{
					response = 0;
					break;
				}

				response += std::min(difference * difference, maxSqrDifference);
			}
		}

		if (response != 0)
		{
			nonMaximumSuppression->addCandidate(x, y, response);
		}

		++yRow;
	}
}

void PointDetector::determinePointCandidatesRowDual(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(nonMaximumSuppression != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(maximalDifference > 0);

	const unsigned int filterSize_2 = filterSize / 2u;

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	yRow += filterSize_2;

	for (unsigned int x = filterSize_2; x < nonMaximumSuppression->width() - filterSize_2; ++x)
	{
		const int32_t centerValue = int32_t(*yRow);

		int32_t response = 0;

		const uint8_t* yBorderData = yRow - borderOffsets[0];

		int32_t difference = int32_t(*yBorderData) - centerValue;

		if (std::abs(difference) >= minimalDifference)
		{
			if (difference > 0)
			{
				// black pixel on white background

				response = std::min(difference * difference, maxSqrDifference);

				for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
				{
					yBorderData += borderOffsets[nOffset];

					difference = int32_t(*yBorderData) - centerValue;

					if (difference < minimalDifference)
					{
						response = 0;
						break;
					}

					response += std::min(difference * difference, maxSqrDifference);
				}
			}
			else
			{
				// white pixel on black background

				response = -std::min(difference * difference, maxSqrDifference);

				for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
				{
					yBorderData += borderOffsets[nOffset];

					difference = int32_t(*yBorderData) - centerValue;

					if (difference > -minimalDifference)
					{
						response = 0;
						break;
					}

					response -= std::min(difference * difference, maxSqrDifference);
				}
			}
		}

		if (response != 0)
		{
			nonMaximumSuppression->addCandidate(x, y, response);
		}

		++yRow;
	}
}

void PointDetector::createFilterResponseRow(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(responseRow != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(maximalDifference > 0);

	const unsigned int filterSize_2 = filterSize / 2u;

	// response pixels close to the left image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));

	yRow += filterSize_2;
	responseRow += filterSize_2;

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	for (unsigned int x = filterSize_2; x < width - filterSize_2; ++x)
	{
		const int32_t centerValue = int32_t(*yRow);

		int32_t response = 0;

		const uint8_t* yBorderData = yRow - borderOffsets[0]; // first offset is interpreted as negative offset (to the top-left starting point)

		int32_t difference = int32_t(*yBorderData) - centerValue;

		if (difference >= minimalDifference)
		{
			response += std::min(difference * difference, maxSqrDifference);

			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset]; // all remaining offsets are positive offsets in relation to the previous position

				difference = int32_t(*yBorderData) - centerValue;

				if (difference < minimalDifference)
				{
					response = 0;
					break;
				}

				response += std::min(difference * difference, maxSqrDifference);
			}
		}

		*responseRow = response;

		++yRow;
		++responseRow;
	}

	// response pixels close to the right image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));
}

void PointDetector::createFilterResponseRowDual(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(responseRow != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(maximalDifference > 0);

	const unsigned int filterSize_2 = filterSize / 2u;

	// response pixels close to the left image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));

	yRow += filterSize_2;
	responseRow += filterSize_2;

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	for (unsigned int x = filterSize_2; x < width - filterSize_2; ++x)
	{
		const int32_t centerValue = int32_t(*yRow);

		int32_t response = 0;

		const uint8_t* yBorderData = yRow - borderOffsets[0];

		// first difference

		int32_t difference = int32_t(*yBorderData) - centerValue;

		if (std::abs(difference) >= minimalDifference)
		{
			if (difference > 0)
			{
				// black pixel on white background

				response += std::min(difference * difference, maxSqrDifference);

				for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
				{
					yBorderData += borderOffsets[nOffset];

					difference = int32_t(*yBorderData) - centerValue;

					if (difference < minimalDifference)
					{
						response = 0;
						break;
					}

					response += std::min(difference * difference, maxSqrDifference);
				}
			}
			else
			{
				// white pixel on black background

				response -= std::min(difference * difference, maxSqrDifference);

				for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
				{
					yBorderData += borderOffsets[nOffset];

					difference = int32_t(*yBorderData) - centerValue;

					if (difference > -minimalDifference)
					{
						response = 0;
						break;
					}

					response -= std::min(difference * difference, maxSqrDifference);
				}
			}
		}

		*responseRow = response;

		++yRow;
		++responseRow;
	}

	// response pixels close to the right image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

void PointDetector::createFilterResponseRowSSE(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(responseRow != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(width >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	// response pixels close to the left image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));

	const uint8_t* yRowCenter = yRow + filterSize_2;
	int32_t* responseRowCenter = responseRow + filterSize_2;

	const unsigned int coreWidth = width - filterSize_2 * 2u;

	const __m128i minDiffMinus1_s_16x8 = _mm_set1_epi16(int16_t(minimalDifference - 1));
	const __m128i maxSqrDiff_s_32x4 = _mm_set1_epi32(maxSqrDifference);

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			responseRowCenter -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + blockSize < coreWidth));
		}

		const __m128i center_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yRowCenter));

		__m128i valid_s_16x8 = _mm_set1_epi16(-1);
		__m128i sumSqrLow_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrHigh_s_32x4 = _mm_setzero_si128();

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0]; // first offset is interpreted as negative offset (to the top-left starting point)

		// process first border pixel
		__m128i border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
		__m128i diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

		// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
		valid_s_16x8 = _mm_and_si128(valid_s_16x8, _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8));

		if (!_mm_testz_si128(valid_s_16x8, valid_s_16x8))
		{
			// square the differences and clamp to maxSqrDifference
			__m128i sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
			__m128i sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

			sumSqrLow_s_32x4 = _mm_add_epi32(sumSqrLow_s_32x4, sqrLow_s_32x4);
			sumSqrHigh_s_32x4 = _mm_add_epi32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset]; // all remaining offsets are positive offsets in relation to the previous position

				border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
				diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

				// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
				valid_s_16x8 = _mm_and_si128(valid_s_16x8, _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8));

				// early exit if all 8 pixels have failed the threshold check
				if (_mm_testz_si128(valid_s_16x8, valid_s_16x8))
				{
					break;
				}

				// square the differences and clamp to maxSqrDifference
				sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

				sumSqrLow_s_32x4 = _mm_add_epi32(sumSqrLow_s_32x4, sqrLow_s_32x4);
				sumSqrHigh_s_32x4 = _mm_add_epi32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid mask from 16-bit to 32-bit and apply: response = valid ? sumSqr : 0
		_mm_storeu_si128((__m128i*)(responseRowCenter + 0), _mm_and_si128(sumSqrLow_s_32x4, _mm_cvtepi16_epi32(valid_s_16x8)));
		_mm_storeu_si128((__m128i*)(responseRowCenter + 4), _mm_and_si128(sumSqrHigh_s_32x4, _mm_cvtepi16_epi32(_mm_srli_si128(valid_s_16x8, 8))));

		yRowCenter += blockSize;
		responseRowCenter += blockSize;
	}

	// response pixels close to the right image border
	memset(responseRowCenter, 0x00, filterSize_2 * sizeof(int32_t));
}

void PointDetector::createFilterResponseRowSSEDual(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(responseRow != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(width >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	// response pixels close to the left image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));

	const uint8_t* yRowCenter = yRow + filterSize_2;
	int32_t* responseRowCenter = responseRow + filterSize_2;

	const unsigned int coreWidth = width - filterSize_2 * 2u;

	const __m128i minDiffMinus1_s_16x8 = _mm_set1_epi16(int16_t(minimalDifference - 1));
	const __m128i negMinDiff_s_16x8 = _mm_set1_epi16(int16_t(-minimalDifference));
	const __m128i maxSqrDiff_s_32x4 = _mm_set1_epi32(maxSqrDifference);

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			responseRowCenter -= offset;

			ocean_assert(!(x + blockSize < coreWidth));
		}

		const __m128i center_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yRowCenter));

		// track validity and response for black dots (positive diff) and white dots (negative diff)
		__m128i validDark_s_16x8 = _mm_set1_epi16(-1);
		__m128i validBright_s_16x8 = _mm_set1_epi16(-1);
		__m128i sumSqrDarkLow_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrDarkHigh_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrBrightLow_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrBrightHigh_s_32x4 = _mm_setzero_si128();

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0];

		// process first border pixel
		__m128i border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
		__m128i diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

		// dark dots: diff >= minimalDifference (i.e., diff > minimalDifference - 1)
		validDark_s_16x8 = _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8);

		// bright dots: diff <= -minimalDifference (i.e., diff < -minimalDifference + 1)
		validBright_s_16x8 = _mm_cmplt_epi16(diff_s_16x8, negMinDiff_s_16x8);

		const bool hasDarkCandidates = !_mm_testz_si128(validDark_s_16x8, validDark_s_16x8);
		const bool hasBrightCandidates = !_mm_testz_si128(validBright_s_16x8, validBright_s_16x8);

		if (hasDarkCandidates || hasBrightCandidates)
		{
			// square the differences and clamp to maxSqrDifference
			__m128i sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
			__m128i sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

			sumSqrDarkLow_s_32x4 = _mm_add_epi32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
			sumSqrDarkHigh_s_32x4 = _mm_add_epi32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
			sumSqrBrightLow_s_32x4 = _mm_add_epi32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
			sumSqrBrightHigh_s_32x4 = _mm_add_epi32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset];

				border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
				diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

				// update validity masks
				validDark_s_16x8 = _mm_and_si128(validDark_s_16x8, _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8));
				validBright_s_16x8 = _mm_and_si128(validBright_s_16x8, _mm_cmplt_epi16(diff_s_16x8, negMinDiff_s_16x8));

				// early exit if all 8 pixels have failed both threshold checks
				const __m128i anyValid_s_16x8 = _mm_or_si128(validDark_s_16x8, validBright_s_16x8);
				if (_mm_testz_si128(anyValid_s_16x8, anyValid_s_16x8))
				{
					break;
				}

				// square the differences and clamp to maxSqrDifference
				sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

				sumSqrDarkLow_s_32x4 = _mm_add_epi32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
				sumSqrDarkHigh_s_32x4 = _mm_add_epi32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
				sumSqrBrightLow_s_32x4 = _mm_add_epi32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
				sumSqrBrightHigh_s_32x4 = _mm_add_epi32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid masks from 16-bit to 32-bit
		const __m128i validDarkLow_s_32x4 = _mm_cvtepi16_epi32(validDark_s_16x8);
		const __m128i validDarkHigh_s_32x4 = _mm_cvtepi16_epi32(_mm_srli_si128(validDark_s_16x8, 8));
		const __m128i validBrightLow_s_32x4 = _mm_cvtepi16_epi32(validBright_s_16x8);
		const __m128i validBrightHigh_s_32x4 = _mm_cvtepi16_epi32(_mm_srli_si128(validBright_s_16x8, 8));

		// apply masks: darkResponse = valid ? sumSqr : 0, brightResponse = valid ? -sumSqr : 0
		const __m128i darkResponseLow_s_32x4 = _mm_and_si128(sumSqrDarkLow_s_32x4, validDarkLow_s_32x4);
		const __m128i darkResponseHigh_s_32x4 = _mm_and_si128(sumSqrDarkHigh_s_32x4, validDarkHigh_s_32x4);
		const __m128i brightResponseLow_s_32x4 = _mm_and_si128(_mm_sub_epi32(_mm_setzero_si128(), sumSqrBrightLow_s_32x4), validBrightLow_s_32x4);
		const __m128i brightResponseHigh_s_32x4 = _mm_and_si128(_mm_sub_epi32(_mm_setzero_si128(), sumSqrBrightHigh_s_32x4), validBrightHigh_s_32x4);

		// combine: each pixel can only be one or the other (or neither), so we can just add
		_mm_storeu_si128((__m128i*)(responseRowCenter + 0), _mm_add_epi32(darkResponseLow_s_32x4, brightResponseLow_s_32x4));
		_mm_storeu_si128((__m128i*)(responseRowCenter + 4), _mm_add_epi32(darkResponseHigh_s_32x4, brightResponseHigh_s_32x4));

		yRowCenter += blockSize;
		responseRowCenter += blockSize;
	}

	// response pixels close to the right image border
	memset(responseRowCenter, 0x00, filterSize_2 * sizeof(int32_t));
}

void PointDetector::determinePointCandidatesRowSSE(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(nonMaximumSuppression != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(nonMaximumSuppression->width() >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	const uint8_t* yRowCenter = yRow + filterSize_2;

	const unsigned int coreWidth = nonMaximumSuppression->width() - filterSize_2 * 2u;

	const __m128i minDiffMinus1_s_16x8 = _mm_set1_epi16(int16_t(minimalDifference - 1));
	const __m128i maxSqrDiff_s_32x4 = _mm_set1_epi32(maxSqrDifference);

	int32_t responseBlock[blockSize];

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			x = newX;

			nonMaximumSuppression->removeCandidatesRightFrom(filterSize_2 + x, y);

			// the for loop will stop after this iteration
			ocean_assert(!(x + blockSize < coreWidth));
		}

		const __m128i center_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yRowCenter));

		__m128i valid_s_16x8 = _mm_set1_epi16(-1);
		__m128i sumSqrLow_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrHigh_s_32x4 = _mm_setzero_si128();

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0]; // first offset is interpreted as negative offset (to the top-left starting point)

		// process first border pixel
		__m128i border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
		__m128i diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

		// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
		valid_s_16x8 = _mm_and_si128(valid_s_16x8, _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8));

		if (!_mm_testz_si128(valid_s_16x8, valid_s_16x8))
		{
			// square the differences and clamp to maxSqrDifference
			__m128i sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
			__m128i sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

			sumSqrLow_s_32x4 = _mm_add_epi32(sumSqrLow_s_32x4, sqrLow_s_32x4);
			sumSqrHigh_s_32x4 = _mm_add_epi32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset]; // all remaining offsets are positive offsets in relation to the previous position

				border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
				diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

				// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
				valid_s_16x8 = _mm_and_si128(valid_s_16x8, _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8));

				// early exit if all 8 pixels have failed the threshold check
				if (_mm_testz_si128(valid_s_16x8, valid_s_16x8))
				{
					break;
				}

				// square the differences and clamp to maxSqrDifference
				sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

				sumSqrLow_s_32x4 = _mm_add_epi32(sumSqrLow_s_32x4, sqrLow_s_32x4);
				sumSqrHigh_s_32x4 = _mm_add_epi32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid mask from 16-bit to 32-bit and apply: response = valid ? sumSqr : 0
		_mm_storeu_si128((__m128i*)(responseBlock + 0), _mm_and_si128(sumSqrLow_s_32x4, _mm_cvtepi16_epi32(valid_s_16x8)));
		_mm_storeu_si128((__m128i*)(responseBlock + 4), _mm_and_si128(sumSqrHigh_s_32x4, _mm_cvtepi16_epi32(_mm_srli_si128(valid_s_16x8, 8))));

		for (unsigned int n = 0u; n < blockSize; ++n)
		{
			if (responseBlock[n] != 0)
			{
				nonMaximumSuppression->addCandidate(filterSize_2 + x + n, y, responseBlock[n]);
			}
		}

		yRowCenter += blockSize;
	}
}

void PointDetector::determinePointCandidatesRowSSEDual(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(nonMaximumSuppression != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(nonMaximumSuppression->width() >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	const uint8_t* yRowCenter = yRow + filterSize_2;

	const unsigned int coreWidth = nonMaximumSuppression->width() - filterSize_2 * 2u;

	const __m128i minDiffMinus1_s_16x8 = _mm_set1_epi16(int16_t(minimalDifference - 1));
	const __m128i negMinDiff_s_16x8 = _mm_set1_epi16(int16_t(-minimalDifference));
	const __m128i maxSqrDiff_s_32x4 = _mm_set1_epi32(maxSqrDifference);

	int32_t responseBlock[blockSize];

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			x = newX;

			nonMaximumSuppression->removeCandidatesRightFrom(filterSize_2 + x, y);

			// the for loop will stop after this iteration
			ocean_assert(!(x + blockSize < coreWidth));
		}

		const __m128i center_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yRowCenter));

		// track validity and response for black dots (positive diff) and white dots (negative diff)
		__m128i validDark_s_16x8 = _mm_set1_epi16(-1);
		__m128i validBright_s_16x8 = _mm_set1_epi16(-1);
		__m128i sumSqrDarkLow_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrDarkHigh_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrBrightLow_s_32x4 = _mm_setzero_si128();
		__m128i sumSqrBrightHigh_s_32x4 = _mm_setzero_si128();

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0];

		// process first border pixel
		__m128i border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
		__m128i diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

		// dark dots: diff >= minimalDifference (i.e., diff > minimalDifference - 1)
		validDark_s_16x8 = _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8);

		// bright dots: diff <= -minimalDifference (i.e., diff < -minimalDifference + 1)
		validBright_s_16x8 = _mm_cmplt_epi16(diff_s_16x8, negMinDiff_s_16x8);

		const bool hasDarkCandidates = !_mm_testz_si128(validDark_s_16x8, validDark_s_16x8);
		const bool hasBrightCandidates = !_mm_testz_si128(validBright_s_16x8, validBright_s_16x8);

		if (hasDarkCandidates || hasBrightCandidates)
		{
			// square the differences and clamp to maxSqrDifference
			__m128i sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
			__m128i sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

			sumSqrDarkLow_s_32x4 = _mm_add_epi32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
			sumSqrDarkHigh_s_32x4 = _mm_add_epi32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
			sumSqrBrightLow_s_32x4 = _mm_add_epi32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
			sumSqrBrightHigh_s_32x4 = _mm_add_epi32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset];

				border_s_16x8 = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i*)yBorderData));
				diff_s_16x8 = _mm_sub_epi16(border_s_16x8, center_s_16x8);

				// update validity masks
				validDark_s_16x8 = _mm_and_si128(validDark_s_16x8, _mm_cmpgt_epi16(diff_s_16x8, minDiffMinus1_s_16x8));
				validBright_s_16x8 = _mm_and_si128(validBright_s_16x8, _mm_cmplt_epi16(diff_s_16x8, negMinDiff_s_16x8));

				// early exit if all 8 pixels have failed both threshold checks
				const __m128i anyValid_s_16x8 = _mm_or_si128(validDark_s_16x8, validBright_s_16x8);
				if (_mm_testz_si128(anyValid_s_16x8, anyValid_s_16x8))
				{
					break;
				}

				// square the differences and clamp to maxSqrDifference
				sqrLow_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpacklo_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = _mm_min_epi32(_mm_madd_epi16(_mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128()), _mm_unpackhi_epi16(diff_s_16x8, _mm_setzero_si128())), maxSqrDiff_s_32x4);

				sumSqrDarkLow_s_32x4 = _mm_add_epi32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
				sumSqrDarkHigh_s_32x4 = _mm_add_epi32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
				sumSqrBrightLow_s_32x4 = _mm_add_epi32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
				sumSqrBrightHigh_s_32x4 = _mm_add_epi32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid masks from 16-bit to 32-bit
		const __m128i validDarkLow_s_32x4 = _mm_cvtepi16_epi32(validDark_s_16x8);
		const __m128i validDarkHigh_s_32x4 = _mm_cvtepi16_epi32(_mm_srli_si128(validDark_s_16x8, 8));
		const __m128i validBrightLow_s_32x4 = _mm_cvtepi16_epi32(validBright_s_16x8);
		const __m128i validBrightHigh_s_32x4 = _mm_cvtepi16_epi32(_mm_srli_si128(validBright_s_16x8, 8));

		// apply masks: darkResponse = valid ? sumSqr : 0, brightResponse = valid ? -sumSqr : 0
		const __m128i darkResponseLow_s_32x4 = _mm_and_si128(sumSqrDarkLow_s_32x4, validDarkLow_s_32x4);
		const __m128i darkResponseHigh_s_32x4 = _mm_and_si128(sumSqrDarkHigh_s_32x4, validDarkHigh_s_32x4);
		const __m128i brightResponseLow_s_32x4 = _mm_and_si128(_mm_sub_epi32(_mm_setzero_si128(), sumSqrBrightLow_s_32x4), validBrightLow_s_32x4);
		const __m128i brightResponseHigh_s_32x4 = _mm_and_si128(_mm_sub_epi32(_mm_setzero_si128(), sumSqrBrightHigh_s_32x4), validBrightHigh_s_32x4);

		// combine: each pixel can only be one or the other (or neither), so we can just add
		_mm_storeu_si128((__m128i*)(responseBlock + 0), _mm_add_epi32(darkResponseLow_s_32x4, brightResponseLow_s_32x4));
		_mm_storeu_si128((__m128i*)(responseBlock + 4), _mm_add_epi32(darkResponseHigh_s_32x4, brightResponseHigh_s_32x4));

		for (unsigned int n = 0u; n < blockSize; ++n)
		{
			if (responseBlock[n] != 0)
			{
				nonMaximumSuppression->addCandidate(filterSize_2 + x + n, y, responseBlock[n]);
			}
		}

		yRowCenter += blockSize;
	}
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void PointDetector::createFilterResponseRowNEON(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(responseRow != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(width >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	// response pixels close to the left image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));

	const uint8_t* yRowCenter = yRow + filterSize_2;
	int32_t* responseRowCenter = responseRow + filterSize_2;

	const unsigned int coreWidth = width - filterSize_2 * 2u;

	const int16x8_t minDiffMinus1_s_16x8 = vdupq_n_s16(int16_t(minimalDifference - 1));
	const int32x4_t maxSqrDiff_s_32x4 = vdupq_n_s32(maxSqrDifference);

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			responseRowCenter -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(x + blockSize < coreWidth));
		}

		// load 8 center pixels and expand to 16-bit signed
		const int16x8_t center_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yRowCenter)));

		int16x8_t valid_s_16x8 = vdupq_n_s16(-1);
		int32x4_t sumSqrLow_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrHigh_s_32x4 = vdupq_n_s32(0);

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0]; // first offset is interpreted as negative offset (to the top-left starting point)

		// process first border pixel - load 8 border pixels and expand to 16-bit signed
		int16x8_t border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
		int16x8_t diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

		// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
		valid_s_16x8 = vandq_s16(valid_s_16x8, vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8)));

		// check if any pixels passed the threshold
		int16x4_t valid_or = vorr_s16(vget_low_s16(valid_s_16x8), vget_high_s16(valid_s_16x8));
		int32x2_t valid_or_32 = vreinterpret_s32_s16(valid_or);
		int64x1_t valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));

		if (vget_lane_s64(valid_or_64, 0) != 0)
		{
			// square the differences, clamp to maxSqrDifference, and accumulate
			int16x4_t diff_low = vget_low_s16(diff_s_16x8);
			int16x4_t diff_high = vget_high_s16(diff_s_16x8);

			int32x4_t sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
			int32x4_t sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

			sumSqrLow_s_32x4 = vaddq_s32(sumSqrLow_s_32x4, sqrLow_s_32x4);
			sumSqrHigh_s_32x4 = vaddq_s32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset]; // all remaining offsets are positive offsets in relation to the previous position

				// load 8 border pixels and expand to 16-bit signed
				border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
				diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

				// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
				valid_s_16x8 = vandq_s16(valid_s_16x8, vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8)));

				// early exit if all 8 pixels have failed the threshold check
				valid_or = vorr_s16(vget_low_s16(valid_s_16x8), vget_high_s16(valid_s_16x8));
				valid_or_32 = vreinterpret_s32_s16(valid_or);
				valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));
				if (vget_lane_s64(valid_or_64, 0) == 0)
				{
					break;
				}

				// square the differences, clamp to maxSqrDifference, and accumulate
				diff_low = vget_low_s16(diff_s_16x8);
				diff_high = vget_high_s16(diff_s_16x8);

				sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

				sumSqrLow_s_32x4 = vaddq_s32(sumSqrLow_s_32x4, sqrLow_s_32x4);
				sumSqrHigh_s_32x4 = vaddq_s32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid mask from 16-bit to 32-bit and apply: response = valid ? sumSqr : 0
		const int32x4_t validLow_s_32x4 = vmovl_s16(vget_low_s16(valid_s_16x8));
		const int32x4_t validHigh_s_32x4 = vmovl_s16(vget_high_s16(valid_s_16x8));

		vst1q_s32(responseRowCenter + 0, vandq_s32(sumSqrLow_s_32x4, validLow_s_32x4));
		vst1q_s32(responseRowCenter + 4, vandq_s32(sumSqrHigh_s_32x4, validHigh_s_32x4));

		yRowCenter += blockSize;
		responseRowCenter += blockSize;
	}

	// response pixels close to the right image border
	memset(responseRowCenter, 0x00, filterSize_2 * sizeof(int32_t));
}

void PointDetector::determinePointCandidatesRowNEON(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(nonMaximumSuppression != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(nonMaximumSuppression->width() >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	const uint8_t* yRowCenter = yRow + filterSize_2;

	const unsigned int coreWidth = nonMaximumSuppression->width() - filterSize_2 * 2u;

	const int16x8_t minDiffMinus1_s_16x8 = vdupq_n_s16(int16_t(minimalDifference - 1));
	const int32x4_t maxSqrDiff_s_32x4 = vdupq_n_s32(maxSqrDifference);

	int32_t responseBlock[blockSize];

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			x = newX;

			nonMaximumSuppression->removeCandidatesRightFrom(filterSize_2 + x, y);

			// the for loop will stop after this iteration
			ocean_assert(!(x + blockSize < coreWidth));
		}

		// load 8 center pixels and expand to 16-bit signed
		const int16x8_t center_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yRowCenter)));

		int16x8_t valid_s_16x8 = vdupq_n_s16(-1);
		int32x4_t sumSqrLow_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrHigh_s_32x4 = vdupq_n_s32(0);

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0]; // first offset is interpreted as negative offset (to the top-left starting point)

		// process first border pixel - load 8 border pixels and expand to 16-bit signed
		int16x8_t border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
		int16x8_t diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

		// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
		valid_s_16x8 = vandq_s16(valid_s_16x8, vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8)));

		// check if any pixels passed the threshold
		int16x4_t valid_or = vorr_s16(vget_low_s16(valid_s_16x8), vget_high_s16(valid_s_16x8));
		int32x2_t valid_or_32 = vreinterpret_s32_s16(valid_or);
		int64x1_t valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));

		if (vget_lane_s64(valid_or_64, 0) != 0)
		{
			// square the differences, clamp to maxSqrDifference, and accumulate
			int16x4_t diff_low = vget_low_s16(diff_s_16x8);
			int16x4_t diff_high = vget_high_s16(diff_s_16x8);

			int32x4_t sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
			int32x4_t sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

			sumSqrLow_s_32x4 = vaddq_s32(sumSqrLow_s_32x4, sqrLow_s_32x4);
			sumSqrHigh_s_32x4 = vaddq_s32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset]; // all remaining offsets are positive offsets in relation to the previous position

				// load 8 border pixels and expand to 16-bit signed
				border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
				diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

				// check if difference >= minimalDifference (i.e., diff > minimalDifference - 1)
				valid_s_16x8 = vandq_s16(valid_s_16x8, vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8)));

				// early exit if all 8 pixels have failed the threshold check
				valid_or = vorr_s16(vget_low_s16(valid_s_16x8), vget_high_s16(valid_s_16x8));
				valid_or_32 = vreinterpret_s32_s16(valid_or);
				valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));
				if (vget_lane_s64(valid_or_64, 0) == 0)
				{
					break;
				}

				// square the differences, clamp to maxSqrDifference, and accumulate
				diff_low = vget_low_s16(diff_s_16x8);
				diff_high = vget_high_s16(diff_s_16x8);

				sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

				sumSqrLow_s_32x4 = vaddq_s32(sumSqrLow_s_32x4, sqrLow_s_32x4);
				sumSqrHigh_s_32x4 = vaddq_s32(sumSqrHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid mask from 16-bit to 32-bit and apply: response = valid ? sumSqr : 0
		const int32x4_t validLow_s_32x4 = vmovl_s16(vget_low_s16(valid_s_16x8));
		const int32x4_t validHigh_s_32x4 = vmovl_s16(vget_high_s16(valid_s_16x8));

		vst1q_s32(responseBlock + 0, vandq_s32(sumSqrLow_s_32x4, validLow_s_32x4));
		vst1q_s32(responseBlock + 4, vandq_s32(sumSqrHigh_s_32x4, validHigh_s_32x4));

		for (unsigned int n = 0u; n < blockSize; ++n)
		{
			if (responseBlock[n] != 0)
			{
				nonMaximumSuppression->addCandidate(filterSize_2 + x + n, y, responseBlock[n]);
			}
		}

		yRowCenter += blockSize;
	}
}

void PointDetector::determinePointCandidatesRowNEONDual(const unsigned int y, const uint8_t* yRow, NonMaximumSuppressionVote* nonMaximumSuppression, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(nonMaximumSuppression != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(nonMaximumSuppression->width() >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	const uint8_t* yRowCenter = yRow + filterSize_2;

	const unsigned int coreWidth = nonMaximumSuppression->width() - filterSize_2 * 2u;

	const int16x8_t minDiffMinus1_s_16x8 = vdupq_n_s16(int16_t(minimalDifference - 1));
	const int16x8_t negMinDiff_s_16x8 = vdupq_n_s16(int16_t(-minimalDifference));
	const int32x4_t maxSqrDiff_s_32x4 = vdupq_n_s32(maxSqrDifference);

	int32_t responseBlock[blockSize];

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			x = newX;

			nonMaximumSuppression->removeCandidatesRightFrom(filterSize_2 + x, y);

			// the for loop will stop after this iteration
			ocean_assert(!(x + blockSize < coreWidth));
		}

		// load 8 center pixels and expand to 16-bit signed
		const int16x8_t center_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yRowCenter)));

		// track validity and response for black dots (positive diff) and white dots (negative diff)
		int16x8_t validDark_s_16x8 = vdupq_n_s16(-1);
		int16x8_t validBright_s_16x8 = vdupq_n_s16(-1);
		int32x4_t sumSqrDarkLow_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrDarkHigh_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrBrightLow_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrBrightHigh_s_32x4 = vdupq_n_s32(0);

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0];

		// process first border pixel - load 8 border pixels and expand to 16-bit signed
		int16x8_t border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
		int16x8_t diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

		// dark dots: diff >= minimalDifference (i.e., diff > minimalDifference - 1)
		validDark_s_16x8 = vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8));

		// bright dots: diff <= -minimalDifference (i.e., diff < -minimalDifference + 1)
		validBright_s_16x8 = vreinterpretq_s16_u16(vcltq_s16(diff_s_16x8, negMinDiff_s_16x8));

		// check if any pixels passed either threshold
		int16x8_t anyValid_s_16x8 = vorrq_s16(validDark_s_16x8, validBright_s_16x8);
		int16x4_t valid_or = vorr_s16(vget_low_s16(anyValid_s_16x8), vget_high_s16(anyValid_s_16x8));
		int32x2_t valid_or_32 = vreinterpret_s32_s16(valid_or);
		int64x1_t valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));

		if (vget_lane_s64(valid_or_64, 0) != 0)
		{
			// square the differences, clamp to maxSqrDifference, and accumulate
			int16x4_t diff_low = vget_low_s16(diff_s_16x8);
			int16x4_t diff_high = vget_high_s16(diff_s_16x8);

			int32x4_t sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
			int32x4_t sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

			sumSqrDarkLow_s_32x4 = vaddq_s32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
			sumSqrDarkHigh_s_32x4 = vaddq_s32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
			sumSqrBrightLow_s_32x4 = vaddq_s32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
			sumSqrBrightHigh_s_32x4 = vaddq_s32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset];

				// load 8 border pixels and expand to 16-bit signed
				border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
				diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

				// update validity masks
				validDark_s_16x8 = vandq_s16(validDark_s_16x8, vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8)));
				validBright_s_16x8 = vandq_s16(validBright_s_16x8, vreinterpretq_s16_u16(vcltq_s16(diff_s_16x8, negMinDiff_s_16x8)));

				// early exit if all 8 pixels have failed both threshold checks
				anyValid_s_16x8 = vorrq_s16(validDark_s_16x8, validBright_s_16x8);
				valid_or = vorr_s16(vget_low_s16(anyValid_s_16x8), vget_high_s16(anyValid_s_16x8));
				valid_or_32 = vreinterpret_s32_s16(valid_or);
				valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));
				if (vget_lane_s64(valid_or_64, 0) == 0)
				{
					break;
				}

				// square the differences, clamp to maxSqrDifference, and accumulate
				diff_low = vget_low_s16(diff_s_16x8);
				diff_high = vget_high_s16(diff_s_16x8);

				sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

				sumSqrDarkLow_s_32x4 = vaddq_s32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
				sumSqrDarkHigh_s_32x4 = vaddq_s32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
				sumSqrBrightLow_s_32x4 = vaddq_s32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
				sumSqrBrightHigh_s_32x4 = vaddq_s32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid masks from 16-bit to 32-bit
		const int32x4_t validDarkLow_s_32x4 = vmovl_s16(vget_low_s16(validDark_s_16x8));
		const int32x4_t validDarkHigh_s_32x4 = vmovl_s16(vget_high_s16(validDark_s_16x8));
		const int32x4_t validBrightLow_s_32x4 = vmovl_s16(vget_low_s16(validBright_s_16x8));
		const int32x4_t validBrightHigh_s_32x4 = vmovl_s16(vget_high_s16(validBright_s_16x8));

		// apply masks: darkResponse = valid ? sumSqr : 0, brightResponse = valid ? -sumSqr : 0
		const int32x4_t darkResponseLow_s_32x4 = vandq_s32(sumSqrDarkLow_s_32x4, validDarkLow_s_32x4);
		const int32x4_t darkResponseHigh_s_32x4 = vandq_s32(sumSqrDarkHigh_s_32x4, validDarkHigh_s_32x4);
		const int32x4_t brightResponseLow_s_32x4 = vandq_s32(vnegq_s32(sumSqrBrightLow_s_32x4), validBrightLow_s_32x4);
		const int32x4_t brightResponseHigh_s_32x4 = vandq_s32(vnegq_s32(sumSqrBrightHigh_s_32x4), validBrightHigh_s_32x4);

		// combine: each pixel can only be one or the other (or neither), so we can just add
		vst1q_s32(responseBlock + 0, vaddq_s32(darkResponseLow_s_32x4, brightResponseLow_s_32x4));
		vst1q_s32(responseBlock + 4, vaddq_s32(darkResponseHigh_s_32x4, brightResponseHigh_s_32x4));

		for (unsigned int n = 0u; n < blockSize; ++n)
		{
			if (responseBlock[n] != 0)
			{
				nonMaximumSuppression->addCandidate(filterSize_2 + x + n, y, responseBlock[n]);
			}
		}

		yRowCenter += blockSize;
	}
}

void PointDetector::createFilterResponseRowNEONDual(const uint8_t* yRow, int32_t* responseRow, const unsigned int width, const uint32_t* borderOffsets, const size_t numberBorderOffsets, const unsigned int filterSize, const int32_t minimalDifference, const int32_t maximalDifference)
{
	ocean_assert(yRow != nullptr);
	ocean_assert(responseRow != nullptr);
	ocean_assert(borderOffsets != nullptr);
	ocean_assert(numberBorderOffsets >= 1);
	ocean_assert(maximalDifference >= 0);

	constexpr unsigned int blockSize = 8u;

	const unsigned int filterSize_2 = filterSize / 2u;
	ocean_assert(width >= filterSize_2 + blockSize + filterSize_2);

	const int32_t maxSqrDifference = maximalDifference * maximalDifference;

	// response pixels close to the left image border
	memset(responseRow, 0x00, filterSize_2 * sizeof(int32_t));

	const uint8_t* yRowCenter = yRow + filterSize_2;
	int32_t* responseRowCenter = responseRow + filterSize_2;

	const unsigned int coreWidth = width - filterSize_2 * 2u;

	const int16x8_t minDiffMinus1_s_16x8 = vdupq_n_s16(int16_t(minimalDifference - 1));
	const int16x8_t negMinDiff_s_16x8 = vdupq_n_s16(int16_t(-minimalDifference));
	const int32x4_t maxSqrDiff_s_32x4 = vdupq_n_s32(maxSqrDifference);

	for (unsigned int x = 0u; x < coreWidth; x += blockSize)
	{
		if (x + blockSize > coreWidth)
		{
			ocean_assert(x >= blockSize && coreWidth > blockSize);
			const unsigned int newX = coreWidth - blockSize;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			yRowCenter -= offset;
			responseRowCenter -= offset;

			ocean_assert(!(x + blockSize < coreWidth));
		}

		// load 8 center pixels and expand to 16-bit signed
		const int16x8_t center_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yRowCenter)));

		// track validity and response for black dots (positive diff) and white dots (negative diff)
		int16x8_t validDark_s_16x8 = vdupq_n_s16(-1);
		int16x8_t validBright_s_16x8 = vdupq_n_s16(-1);
		int32x4_t sumSqrDarkLow_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrDarkHigh_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrBrightLow_s_32x4 = vdupq_n_s32(0);
		int32x4_t sumSqrBrightHigh_s_32x4 = vdupq_n_s32(0);

		const uint8_t* yBorderData = yRowCenter - borderOffsets[0];

		// process first border pixel - load 8 border pixels and expand to 16-bit signed
		int16x8_t border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
		int16x8_t diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

		// dark dots: diff >= minimalDifference (i.e., diff > minimalDifference - 1)
		validDark_s_16x8 = vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8));

		// bright dots: diff <= -minimalDifference (i.e., diff < -minimalDifference + 1)
		validBright_s_16x8 = vreinterpretq_s16_u16(vcltq_s16(diff_s_16x8, negMinDiff_s_16x8));

		// check if any pixels passed either threshold
		int16x8_t anyValid_s_16x8 = vorrq_s16(validDark_s_16x8, validBright_s_16x8);
		int16x4_t valid_or = vorr_s16(vget_low_s16(anyValid_s_16x8), vget_high_s16(anyValid_s_16x8));
		int32x2_t valid_or_32 = vreinterpret_s32_s16(valid_or);
		int64x1_t valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));

		if (vget_lane_s64(valid_or_64, 0) != 0)
		{
			// square the differences, clamp to maxSqrDifference, and accumulate
			int16x4_t diff_low = vget_low_s16(diff_s_16x8);
			int16x4_t diff_high = vget_high_s16(diff_s_16x8);

			int32x4_t sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
			int32x4_t sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

			sumSqrDarkLow_s_32x4 = vaddq_s32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
			sumSqrDarkHigh_s_32x4 = vaddq_s32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
			sumSqrBrightLow_s_32x4 = vaddq_s32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
			sumSqrBrightHigh_s_32x4 = vaddq_s32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);

			// process remaining border pixels
			for (size_t nOffset = 1; nOffset < numberBorderOffsets; ++nOffset)
			{
				yBorderData += borderOffsets[nOffset];

				// load 8 border pixels and expand to 16-bit signed
				border_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(yBorderData)));
				diff_s_16x8 = vsubq_s16(border_s_16x8, center_s_16x8);

				// update validity masks
				validDark_s_16x8 = vandq_s16(validDark_s_16x8, vreinterpretq_s16_u16(vcgtq_s16(diff_s_16x8, minDiffMinus1_s_16x8)));
				validBright_s_16x8 = vandq_s16(validBright_s_16x8, vreinterpretq_s16_u16(vcltq_s16(diff_s_16x8, negMinDiff_s_16x8)));

				// early exit if all 8 pixels have failed both threshold checks
				anyValid_s_16x8 = vorrq_s16(validDark_s_16x8, validBright_s_16x8);
				valid_or = vorr_s16(vget_low_s16(anyValid_s_16x8), vget_high_s16(anyValid_s_16x8));
				valid_or_32 = vreinterpret_s32_s16(valid_or);
				valid_or_64 = vreinterpret_s64_s32(vorr_s32(valid_or_32, vrev64_s32(valid_or_32)));
				if (vget_lane_s64(valid_or_64, 0) == 0)
				{
					break;
				}

				// square the differences, clamp to maxSqrDifference, and accumulate
				diff_low = vget_low_s16(diff_s_16x8);
				diff_high = vget_high_s16(diff_s_16x8);

				sqrLow_s_32x4 = vminq_s32(vmull_s16(diff_low, diff_low), maxSqrDiff_s_32x4);
				sqrHigh_s_32x4 = vminq_s32(vmull_s16(diff_high, diff_high), maxSqrDiff_s_32x4);

				sumSqrDarkLow_s_32x4 = vaddq_s32(sumSqrDarkLow_s_32x4, sqrLow_s_32x4);
				sumSqrDarkHigh_s_32x4 = vaddq_s32(sumSqrDarkHigh_s_32x4, sqrHigh_s_32x4);
				sumSqrBrightLow_s_32x4 = vaddq_s32(sumSqrBrightLow_s_32x4, sqrLow_s_32x4);
				sumSqrBrightHigh_s_32x4 = vaddq_s32(sumSqrBrightHigh_s_32x4, sqrHigh_s_32x4);
			}
		}

		// expand valid masks from 16-bit to 32-bit
		const int32x4_t validDarkLow_s_32x4 = vmovl_s16(vget_low_s16(validDark_s_16x8));
		const int32x4_t validDarkHigh_s_32x4 = vmovl_s16(vget_high_s16(validDark_s_16x8));
		const int32x4_t validBrightLow_s_32x4 = vmovl_s16(vget_low_s16(validBright_s_16x8));
		const int32x4_t validBrightHigh_s_32x4 = vmovl_s16(vget_high_s16(validBright_s_16x8));

		// apply masks: darkResponse = valid ? sumSqr : 0, brightResponse = valid ? -sumSqr : 0
		const int32x4_t darkResponseLow_s_32x4 = vandq_s32(sumSqrDarkLow_s_32x4, validDarkLow_s_32x4);
		const int32x4_t darkResponseHigh_s_32x4 = vandq_s32(sumSqrDarkHigh_s_32x4, validDarkHigh_s_32x4);
		const int32x4_t brightResponseLow_s_32x4 = vandq_s32(vnegq_s32(sumSqrBrightLow_s_32x4), validBrightLow_s_32x4);
		const int32x4_t brightResponseHigh_s_32x4 = vandq_s32(vnegq_s32(sumSqrBrightHigh_s_32x4), validBrightHigh_s_32x4);

		// combine: each pixel can only be one or the other (or neither), so we can just add
		vst1q_s32(responseRowCenter + 0, vaddq_s32(darkResponseLow_s_32x4, brightResponseLow_s_32x4));
		vst1q_s32(responseRowCenter + 4, vaddq_s32(darkResponseHigh_s_32x4, brightResponseHigh_s_32x4));

		yRowCenter += blockSize;
		responseRowCenter += blockSize;
	}

	// response pixels close to the right image border
	memset(responseRowCenter, 0x00, filterSize_2 * sizeof(int32_t));
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}

}
