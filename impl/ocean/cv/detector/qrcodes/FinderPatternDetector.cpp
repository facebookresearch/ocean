/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"

#include "ocean/cv/detector/qrcodes/TransitionDetector.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

FinderPatterns FinderPatternDetector::detectFinderPatterns(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int minimumDistance, const unsigned int paddingElements, Worker* worker)
{
	ocean_assert(yFrame != nullptr);

	// Abort early if the image is too small
	if (width < 21u || height < 21u)
	{
		return FinderPatterns();
	}

	FinderPatterns finderPatterns;
	finderPatterns.reserve(15);

	if (worker && height >= 600u)
	{
		Lock multiThreadLock;
		worker->executeFunction(Worker::Function::createStatic(&detectFinderPatternsSubset, yFrame, width, height, &finderPatterns, &multiThreadLock, paddingElements, 0u, 0u), 10u, height - 20u, 6u, 7u);
	}
	else
	{
		detectFinderPatternsSubset(yFrame, width, height, &finderPatterns, nullptr, paddingElements, 10u, height - 20u);
	}

	// Filter the finder patterns

	ocean_assert(std::is_sorted(finderPatterns.begin(), finderPatterns.end(), FinderPattern::comesBefore));

	FinderPatterns filteredFinderPatterns;
	for (size_t i = 0; i < finderPatterns.size(); ++i)
	{
		bool foundClosePosition = false;

		for (size_t n = 0; !foundClosePosition && n < filteredFinderPatterns.size(); ++n)
		{
			if (finderPatterns[i].position().sqrDistance(filteredFinderPatterns[n].position()) < Numeric::sqr(Scalar(minimumDistance)))
			{
				if (finderPatterns[i].symmetryScore() < filteredFinderPatterns[n].symmetryScore())
				{
					filteredFinderPatterns[n] = finderPatterns[i];
				}

				foundClosePosition = true;
			}
		}

		if (!foundClosePosition)
		{
			if (!refineFinderPatternLocation(yFrame, width, height, finderPatterns[i], paddingElements))
			{
				Log::info() << "Failed to refine the location of a finder pattern";
			}

			filteredFinderPatterns.push_back(finderPatterns[i]);
		}
	}

	finderPatterns = std::move(filteredFinderPatterns);

	return finderPatterns;
}

void FinderPatternDetector::detectFinderPatternsSubset(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, FinderPatterns* finderPatterns, Lock* multiThreadLock, const unsigned int paddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 21u && height >= 21u);
	ocean_assert(finderPatterns != nullptr);
	ocean_assert(firstRow >= 10u && numberRows <= height - 10u);

	FinderPatterns localFinderPatterns;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		detectFinderPatternInRow(yFrame, width, height, y, localFinderPatterns, paddingElements);
	}

	if (localFinderPatterns.empty() == false)
	{
		const OptionalScopedLock scopedLock(multiThreadLock);

		ocean_assert(std::is_sorted(localFinderPatterns.begin(), localFinderPatterns.end(), FinderPattern::comesBefore));
		ocean_assert(std::is_sorted(finderPatterns->begin(), finderPatterns->end(), FinderPattern::comesBefore));

		FinderPatterns mergedFinderPatterns;
		std::merge(finderPatterns->begin(), finderPatterns->end(), localFinderPatterns.begin(), localFinderPatterns.end(), std::back_inserter(mergedFinderPatterns), FinderPattern::comesBefore);
		*finderPatterns = std::move(mergedFinderPatterns);
	}
}

void FinderPatternDetector::detectFinderPatternInRow(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int y, FinderPatterns& finderPatterns, const unsigned int paddingElements)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 29u && height >= 29u);
	ocean_assert(y >= 10u && y < height - 10u);

	const unsigned int yFrameStrideElements = width + paddingElements;

	const uint8_t* const yRow = yFrame + yFrameStrideElements * y;

	// Scanning for the following 1D pattern: white, black, white, black, white, black, white
	// Ratios:                                 >=1 :   1  :   1  :   3  :   1  :  1   :  >=1
	// Segments:                                       1      2      3      4     5      6

	const unsigned int invalidSegmentStart = (unsigned int)(-1);

	unsigned int segment_1_start_black = invalidSegmentStart;
	unsigned int segment_2_start_white = invalidSegmentStart;

	unsigned int x = 1u;

	// Start segment 1: find the first pixel of the first black segment

	TransitionHistory history;
	while (x < width && isTransitionToBlack(yRow + x, history) == false)
	{
		++x;
	}

	if (x >= width)
	{
		return;
	}

	ocean_assert(segment_1_start_black == invalidSegmentStart);
	segment_1_start_black = x;

	while (x < width)
	{
		// Start segment 2: find the first pixel of the first white segment
		if (segment_2_start_white == invalidSegmentStart)
		{
			history.reset();
			while (x < width && isTransitionToWhite(yRow + x, history) == false)
			{
				++x;
			}

			if (x >= width)
			{
				break;
			}

			ocean_assert(segment_2_start_white == invalidSegmentStart);
			segment_2_start_white = x;
		}

		ocean_assert(segment_1_start_black < segment_2_start_white);
		const unsigned int segment_1_size = segment_2_start_white - segment_1_start_black;

		const unsigned int blackSquareSegmentMin = max(1u, segment_1_size * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int blackSquareSegmentMax = max(segment_1_size + 3u, segment_1_size * 640u / 512u); // 1.25 ~ 640/512

		// Start segment 3: find the first pixel of the second black segment (the big black square in the middle)

		history.reset();
		while (x < width && isTransitionToBlack(yRow + x, history) == false)
		{
			++x;
		}

		if (x >= width)
		{
			break;
		}

		ocean_assert(segment_2_start_white < x);
		const unsigned int segment_2_size = x - segment_2_start_white;

		// Check if the size of segments 1 and 2 is approximately identical

		ocean_assert(segment_1_start_black < segment_2_start_white);
		ocean_assert(segment_2_start_white < x);

		const unsigned int whiteSquareSegmentMin = max(1u, segment_1_size * 384u / 512u); // 0.75 ~ 384/512
		const unsigned int whiteSquareSegmentMax = max(segment_1_size + 6u, segment_1_size * 640u / 512u); // 1.25 ~ 640/512

		if (segment_2_size < whiteSquareSegmentMin || segment_2_size > whiteSquareSegmentMax)
		{
			// The first two segments are too different. Discard them and use the current location, x,
			// as the start of segment 1. Then continue the search.

			segment_1_start_black = x;
			segment_2_start_white = invalidSegmentStart;

			// x stays untouched

			continue;
		}

		const unsigned int segment_3_start_black = x;

		// Start segment 4: find the first pixel of the second white segment

		history.reset();
		while (x < width && isTransitionToWhite(yRow + x, history) == false)
		{
			++x;
		}

		if (x >= width)
		{
			break;
		}

		ocean_assert(segment_3_start_black < x);
		const unsigned int segment_3_size = x - segment_3_start_black;

		const unsigned int segment_4_start_white = x;

		// Check if the size of segment 3 (center square) is approximately three times that of the first two segments

		ocean_assert(segment_1_start_black < segment_2_start_white);
		ocean_assert(segment_2_start_white < segment_3_start_black);
		ocean_assert(segment_3_start_black < segment_4_start_white);

		const unsigned int centerSegmentMin = std::max((unsigned int)std::max(1, int(3 * blackSquareSegmentMin) - 6), blackSquareSegmentMin * 1280u / 512u); // 2.5 ~ 1280/512;
		const unsigned int centerSegmentMax = blackSquareSegmentMax * 1664u / 512u; // 3.25 ~ 1664/512;

		if (segment_3_size < centerSegmentMin || segment_3_size > centerSegmentMax)
		{
			// The size of the center square does not match the expected ratio to the first two segments.
			// Discard the first two segments and start again with the start of center square (segment 3)
			// as the new segment 1. Segment 4 will be the new segment 2.

			segment_1_start_black = segment_3_start_black;
			segment_2_start_white = segment_4_start_white;

			// Reset x as well
			x = segment_2_start_white;

			continue;
		}

		// Start segment 5: find the first pixel of the third black segment

		history.reset();
		while (x < width && isTransitionToBlack(yRow + x, history) == false)
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_4_start_white < x);
		const unsigned int segment_4_size = x - segment_4_start_white;

		// Check if segment 4 has approximately the same size as the first two segments

		if (segment_4_size < whiteSquareSegmentMin || segment_4_size > whiteSquareSegmentMax)
		{
			// Two options to proceed:
			// a. if segment 4 has the same size as segment 3, reassign segments 3 -> 1, 4 -> 2
			// b. otherwise reassign the start of segment 5 to segment 1
			// then continue the search.

			if (segment_4_size < segment_3_size * 384u / 512u || segment_4_size > segment_3_size * 640u / 512u) // 0.75 ~ 384/512,    1.25 ~ 640/512
			{
				// Option b. - continue with segment 5 as the new segment 1

				segment_1_start_black = x;
				segment_2_start_white = invalidSegmentStart;

				// x remains unchanged
			}
			else
			{
				// Option a. - continue with segments 3 -> 1 and 4 -> 2

				segment_1_start_black = segment_3_start_black;
				segment_2_start_white = segment_4_start_white;

				// Reset x as well
				x = segment_2_start_white;
			}

			continue;
		}

		const unsigned int segment_5_start_black = x;

		// Start "segment 6": find the beginning of next white segment

		history.reset();
		while (x < width && isTransitionToWhite(yRow + x, history) == false)
		{
			++x;
		}

		if (x == width)
		{
			break;
		}

		ocean_assert(segment_5_start_black < x);
		const unsigned int segment_5_size = x - segment_5_start_black;

		// Check if segment 5 has approximately the same size as the first two segments

		if (segment_5_size >= blackSquareSegmentMin && segment_5_size <= blackSquareSegmentMax)
		{
			// Found the correct 1D signal of a finder pattern

			const unsigned int xCenter = (segment_3_start_black + segment_4_start_white + 1u) / 2u;

			const unsigned int centerIntensity = yFrame[y * yFrameStrideElements + xCenter];
			const unsigned int grayThreshold = determineThreshold(yRow + segment_1_start_black, segment_1_size, segment_2_size, segment_3_size, segment_4_size, segment_5_size);

			if (grayThreshold <= 255u && centerIntensity < grayThreshold)
			{
				const unsigned int diameter = x - segment_1_start_black;
				ocean_assert(diameter >= 6u);

				const unsigned int diameter3_4 = (diameter * 3u + 2u) / 4u;

				Scalar symmetryScore;
				Vector2 edgePoints[16];

				if (xCenter >= diameter3_4 && y >= diameter3_4 && xCenter < width - diameter3_4 && y < height - diameter3_4
					&& checkFinderPatternInNeighborhood(yFrame, width, height, paddingElements, xCenter, y, grayThreshold, blackSquareSegmentMin * 307u / 512u, blackSquareSegmentMax * 1280u / 512u, whiteSquareSegmentMin * 307u / 512u, whiteSquareSegmentMax * 1280u / 512u, centerSegmentMin * 307u / 512u, centerSegmentMax * 1280u / 512u, symmetryScore, edgePoints))
				{
					Vector2 location;
					Vector2 corners[4];
					Vector2 orientation;
					Scalar moduleSize;

					if (estimateFinderPatternCorners(xCenter, y, edgePoints, 16u, location, corners, orientation, moduleSize))
					{
						ocean_assert(x >= Scalar(0) && x <= Scalar(width) && y >= Scalar(0) && y <= Scalar(height));
						ocean_assert(Numeric::isNotEqualEps(orientation.length()));
						ocean_assert(Numeric::isNotEqualEps(moduleSize));

						finderPatterns.emplace_back(location, Scalar(diameter), centerIntensity, grayThreshold, symmetryScore, corners, orientation, moduleSize);
					}
				}
			}
		}

		// Regardless of whether a marker has been found, continue the search in the current after
		// reassigning segments 3 -> 1 and 4 -> 2

		segment_1_start_black = segment_3_start_black;
		segment_2_start_white = segment_4_start_white;

		// Reset x as well
		x = segment_2_start_white;
	}

	// TODO Enough to assert std::is_sorted(...)? Should be.
	std::sort(finderPatterns.begin(), finderPatterns.end(), FinderPattern::comesBefore);
}

IndexTriplets FinderPatternDetector::extractIndexTriplets(const FinderPatterns& finderPatterns, const Scalar distanceScaleTolerance, const Scalar moduleSizeScaleTolerance, const Scalar angleTolerance)
{
	ocean_assert(finderPatterns.size() >= 3);
	ocean_assert(Numeric::isInsideRange(Scalar(0), distanceScaleTolerance, Scalar(1)));
	ocean_assert_and_suppress_unused(Numeric::isInsideRange(Scalar(0), moduleSizeScaleTolerance, Scalar(1)), moduleSizeScaleTolerance);
	ocean_assert(angleTolerance >= Numeric::deg2rad(Scalar(0)) && angleTolerance < Numeric::deg2rad(Scalar(90)));

	IndexTriplets finderPatternTriplets;

	const Scalar angleThreshold = Numeric::abs(Numeric::cos(angleTolerance));
	const Scalar parallelOrientationTreshold = Numeric::cos(Scalar(0)) - angleThreshold;
	const Scalar perpendicularOrientationThreshold = Numeric::cos(Scalar(90)) + angleThreshold;

	for (size_t a = 0; a < finderPatterns.size() - 2; ++a)
	{
		const FinderPattern& finderPatternA = finderPatterns[a];

		if (finderPatternA.cornersKnown() == false)
		{
			continue;
		}

		for (size_t b = a + 1; b < finderPatterns.size() - 1; ++b)
		{
			const FinderPattern& finderPatternB = finderPatterns[b];

			if (finderPatternB.cornersKnown() == false)
			{
				continue;
			}

			const Scalar absCosOrientationAB = std::abs(finderPatternA.orientation() * finderPatternB.orientation());
			if (absCosOrientationAB <= parallelOrientationTreshold && absCosOrientationAB >= perpendicularOrientationThreshold)
			{
				ocean_assert(Numeric::rad2deg(Numeric::acos(absCosOrientationAB)) >= Numeric::rad2deg(angleTolerance) && Numeric::rad2deg(Numeric::acos(absCosOrientationAB)) <= (Scalar(90) - Numeric::rad2deg(angleTolerance)));
				continue;
			}

			const bool isParallelAB = isParallel(finderPatternA, finderPatternB, distanceScaleTolerance);
			const bool isDiagonalAB = isDiagonal(finderPatternA, finderPatternB, angleTolerance);

			if ((isParallelAB ^ isDiagonalAB) == false)
			{
				continue;
			}

			for (size_t c = b + 1; c < finderPatterns.size(); ++c)
			{
				const FinderPattern& finderPatternC = finderPatterns[c];

				if (finderPatternC.cornersKnown() == false)
				{
					continue;
				}

				ocean_assert(isParallelAB ^ isDiagonalAB);

				const Scalar absCosOrientationAC = std::abs(finderPatternA.orientation() * finderPatternC.orientation());
				const Scalar absCosOrientationBC = std::abs(finderPatternB.orientation() * finderPatternC.orientation());
				if ((absCosOrientationAC <= parallelOrientationTreshold && absCosOrientationAC >= perpendicularOrientationThreshold)
					|| (absCosOrientationBC <= parallelOrientationTreshold && absCosOrientationBC >= perpendicularOrientationThreshold))
				{
					ocean_assert((Numeric::rad2deg(Numeric::acos(absCosOrientationAB)) >= Numeric::rad2deg(angleTolerance) && Numeric::rad2deg(Numeric::acos(absCosOrientationAB)) <= (Scalar(90) - Numeric::rad2deg(angleTolerance)))
						|| (Numeric::rad2deg(Numeric::acos(absCosOrientationAB)) >= Numeric::rad2deg(angleTolerance) && Numeric::rad2deg(Numeric::acos(absCosOrientationAB)) <= (Scalar(90) - Numeric::rad2deg(angleTolerance))));
					continue;
				}

				const bool isParallelAC = isParallel(finderPatternA, finderPatternC, distanceScaleTolerance);
				const bool isDiagonalAC = isDiagonal(finderPatternA, finderPatternC, angleTolerance);

				if ((isParallelAC ^ isDiagonalAC) == false)
				{
					continue;
				}

				const bool isParallelBC = isParallel(finderPatternB, finderPatternC, distanceScaleTolerance);
				const bool isDiagonalBC = isDiagonal(finderPatternB, finderPatternC, angleTolerance);

				if ((isParallelBC ^ isDiagonalBC) == false)
				{
					continue;
				}

				unsigned int topLeftIndex = (unsigned int)(-1);
				unsigned int otherIndices[2] = { (unsigned int)(-1), (unsigned int)(-1) };

				if (isParallelAB && isParallelAC && isDiagonalBC)
				{
					topLeftIndex = (unsigned int)a;
					otherIndices[0] = (unsigned int)b;
					otherIndices[1] = (unsigned int)c;
				}
				else if (isParallelAB && isDiagonalAC && isParallelBC)
				{
					topLeftIndex = (unsigned int)b;
					otherIndices[0] = (unsigned int)c;
					otherIndices[1] = (unsigned int)a;
				}
				else if (isDiagonalAB && isParallelAC && isParallelBC)
				{
					topLeftIndex = (unsigned int)c;
					otherIndices[0] = (unsigned int)a;
					otherIndices[1] = (unsigned int)b;
				}
				else
				{
					continue;
				}

				// Sort the finder patterns in counter-clockwise order (starting with the one in - what will be - the top-left corner)

				if ((finderPatterns[otherIndices[0]].position() - finderPatterns[topLeftIndex].position()).cross(finderPatterns[otherIndices[1]].position() - finderPatterns[topLeftIndex].position()) >= Scalar(0))
				{
					std::swap(otherIndices[0], otherIndices[1]);
				}

				const IndexTriplet finderPattern = {{topLeftIndex, /* bottom left */ otherIndices[0], /* bottom right */ otherIndices[1]}};
				finderPatternTriplets.emplace_back(finderPattern);
			}
		}
	}

	return finderPatternTriplets;
}

bool FinderPatternDetector::estimateFinderPatternCorners(const unsigned int xCenter, const unsigned int yCenter, const Vector2* edgePoints, const unsigned int edgePointsSize, Vector2& location, Vector2* corners, Vector2& orientation, Scalar& moduleSize, const Scalar edgePointDistanceTolerance, const Scalar /*maxEdgeLineDistance*/)
{
	ocean_assert(edgePoints != nullptr && edgePointsSize != 0u && edgePointsSize % 2u == 0u);
	ocean_assert(corners != nullptr);
	ocean_assert(edgePointDistanceTolerance >= 0);

	// Find the edge point that is closest to the center

	unsigned int minDistanceIndex = (unsigned int)(-1);
	Scalar minDistance = Numeric::maxValue();

	for (unsigned int x = 0u; x < edgePointsSize; ++x)
	{
		const Scalar squareDistance = Vector2(Scalar(xCenter), Scalar(yCenter)).sqrDistance(edgePoints[x]);

		if (squareDistance < minDistance)
		{
			minDistance = squareDistance;
			minDistanceIndex = x;
		}
	}

	// Select the edge points at 90, 180, 270 degree which must have a similar distance to the center (because the finder pattern is a square and because of how the edge points are arranged)

	const unsigned int closestPointsIndices[4] =
	{
		minDistanceIndex,
		(minDistanceIndex + (1u * edgePointsSize / 4u)) % edgePointsSize, // Orthogonal to first (90 deg)
		(minDistanceIndex + (2u * edgePointsSize / 4u)) % edgePointsSize, // Opposite to first (180 deg)
		(minDistanceIndex + (3u * edgePointsSize / 4u)) % edgePointsSize, // Orthogonal to first (270 deg) (opposite to second)
	};

	const Scalar maxEdgePointDistanceThreshold = minDistance * (Scalar(1) + edgePointDistanceTolerance);

	ocean_assert(Vector2(Scalar(xCenter), Scalar(yCenter)).sqrDistance(edgePoints[closestPointsIndices[1]]) >= minDistance
		|| Vector2(Scalar(xCenter), Scalar(yCenter)).sqrDistance(edgePoints[closestPointsIndices[2]]) >= minDistance
		|| Vector2(Scalar(xCenter), Scalar(yCenter)).sqrDistance(edgePoints[closestPointsIndices[3]]) >= minDistance);

	if (Vector2(Scalar(xCenter), Scalar(yCenter)).sqrDistance(edgePoints[closestPointsIndices[1]]) >= maxEdgePointDistanceThreshold
		|| Vector2(Scalar(xCenter), Scalar(yCenter)).sqrDistance(edgePoints[closestPointsIndices[2]]) >= maxEdgePointDistanceThreshold
		|| Vector2(Scalar(xCenter), Scalar(yCenter)).sqrDistance(edgePoints[closestPointsIndices[3]]) >= maxEdgePointDistanceThreshold)
	{
		return false;
	}

	// Enforce counter-clockwise order of the edge points

	ocean_assert((edgePoints[closestPointsIndices[1]] - edgePoints[closestPointsIndices[0]]).cross(edgePoints[closestPointsIndices[3]] - edgePoints[closestPointsIndices[0]]) <= 0);
	ocean_assert((edgePoints[closestPointsIndices[2]] - edgePoints[closestPointsIndices[1]]).cross(edgePoints[closestPointsIndices[0]] - edgePoints[closestPointsIndices[1]]) <= 0);
	ocean_assert((edgePoints[closestPointsIndices[3]] - edgePoints[closestPointsIndices[2]]).cross(edgePoints[closestPointsIndices[1]] - edgePoints[closestPointsIndices[2]]) <= 0);
	ocean_assert((edgePoints[closestPointsIndices[0]] - edgePoints[closestPointsIndices[3]]).cross(edgePoints[closestPointsIndices[2]] - edgePoints[closestPointsIndices[3]]) <= 0);

	// Determine the four lines that are adjacent to the four sides of this finder pattern: from a start point, iteratively add edge points on the left (clockwise) and right (counter-clockwise) side of a starting point

	const Scalar estimatedModuleSize = (edgePoints[closestPointsIndices[0]].distance(edgePoints[closestPointsIndices[2]]) + edgePoints[closestPointsIndices[1]].distance(edgePoints[closestPointsIndices[3]])) * Scalar(1.0 / 14.0);
	const Scalar maxSqrDistanceFromLine = (Scalar(0.5) * estimatedModuleSize) * (Scalar(0.5) * estimatedModuleSize);

	Line2 edgeLines[4];

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		// Initial estimate using the adjacent edge points on the left and right side of the start point

		const unsigned int indexClockwise = (closestPointsIndices[i] < edgePointsSize - 1u ? closestPointsIndices[i] + 1u : 0u);
		const unsigned int indexCounterClockwise = (closestPointsIndices[i] > 0u ? closestPointsIndices[i] - 1u : edgePointsSize - 1u);
		ocean_assert(indexClockwise < edgePointsSize && indexCounterClockwise < edgePointsSize);

		edgeLines[i] = Line2(edgePoints[indexClockwise], (edgePoints[indexCounterClockwise] - edgePoints[indexClockwise]).normalized());

		if (edgeLines[i].sqrDistance(edgePoints[closestPointsIndices[i]]) > maxSqrDistanceFromLine)
		{
			return false;
		}
	}

	// Determine the intersection points of the four edge lines (= corners of this finder pattern)

	if ((edgeLines[0].intersection(edgeLines[1], corners[0])
		&& edgeLines[1].intersection(edgeLines[2], corners[1])
		&& edgeLines[2].intersection(edgeLines[3], corners[2])
		&& edgeLines[3].intersection(edgeLines[0], corners[3])) == false)
	{
		return false;
	}

	// Enforce counter-clockwise order of the edge points

	ocean_assert((corners[1] - corners[0]).cross(corners[3] - corners[0]) <= 0);
	ocean_assert((corners[2] - corners[1]).cross(corners[0] - corners[1]) <= 0);
	ocean_assert((corners[3] - corners[2]).cross(corners[1] - corners[2]) <= 0);
	ocean_assert((corners[0] - corners[3]).cross(corners[2] - corners[3]) <= 0);

	// Compute the dominant orientation of this finder pattern from its edge lines

	const Vector2 center = (corners[0] + corners[1] + corners[2] + corners[3]) * Scalar(0.25);

	const Vector2 centerToEdgeMidpoints[4] =
	{
		(corners[0] + (corners[1] - corners[0]) * Scalar(0.5)) - center,
		(corners[1] + (corners[2] - corners[1]) * Scalar(0.5)) - center,
		(corners[2] + (corners[3] - corners[2]) * Scalar(0.5)) - center,
		(corners[3] + (corners[0] - corners[3]) * Scalar(0.5)) - center
	};

	const Scalar xAxisAlignment[4] =
	{
		Vector2(1, 0) * centerToEdgeMidpoints[0],
		Vector2(1, 0) * centerToEdgeMidpoints[1],
		Vector2(1, 0) * centerToEdgeMidpoints[2],
		Vector2(1, 0) * centerToEdgeMidpoints[3]
	};

	size_t maxIndex = xAxisAlignment[0] > xAxisAlignment[1] ? 0 : 1;
	maxIndex = xAxisAlignment[2] > xAxisAlignment[maxIndex] ? 2 : maxIndex;
	maxIndex = xAxisAlignment[3] > xAxisAlignment[maxIndex] ? 3 : maxIndex;

	const Vector2 sumCenterToMidPoints =
		centerToEdgeMidpoints[(maxIndex + 0) & 0b0011] // (maxIndex + x) & 0b0011 == (maxIndex + x) % 4
		+ centerToEdgeMidpoints[(maxIndex + 1) & 0b0011].perpendicular()
		- centerToEdgeMidpoints[(maxIndex + 2) & 0b0011]
		- centerToEdgeMidpoints[(maxIndex + 3) & 0b0011].perpendicular();

	orientation = sumCenterToMidPoints.normalizedOrZero();

	// Location of the center of this finder pattern with sub-pixel accuracy (= geometric center of the four corners)

	location = center;

	// Compute the module size based on the known geometry of the finder pattern and the measured pixel distances

	ocean_assert(sumCenterToMidPoints.length() > Scalar(0));
	moduleSize = sumCenterToMidPoints.length() / Scalar(14); // * 1/4 * 2/7, average, no. modules

	return true;
}

bool FinderPatternDetector::refineFinderPatternLocation(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, FinderPattern& finderPattern, const unsigned int yFramePaddingElements)
{
	constexpr unsigned int tMaxSupportPoints = 11u;
	static_assert(tMaxSupportPoints >= 2u, "Number of support points must be in range [2, infinity)");

	ocean_assert(yFrame != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(finderPattern.position().x() >= 0 && finderPattern.position().x() <= Scalar(width - 1u) && finderPattern.position().y() >= 0 && finderPattern.position().y() <= Scalar(height - 1u));

	if (finderPattern.cornersKnown() == false)
	{
		return false;
	}

	Vector2 corners[4] =
	{
		finderPattern.corners()[0],
		finderPattern.corners()[1],
		finderPattern.corners()[2],
		finderPattern.corners()[3]
	};

	// Require the corners to be in counter-clockwise order
	ocean_assert((corners[1] - corners[0]).cross(corners[3] - corners[0]) <= 0);
	ocean_assert((corners[2] - corners[1]).cross(corners[0] - corners[1]) <= 0);
	ocean_assert((corners[3] - corners[2]).cross(corners[1] - corners[2]) <= 0);
	ocean_assert((corners[0] - corners[3]).cross(corners[2] - corners[3]) <= 0);

	Line2 edgeLines[4] =
	{
		Line2(corners[1], (corners[0] - corners[1]).normalized()),
		Line2(corners[2], (corners[1] - corners[2]).normalized()),
		Line2(corners[3], (corners[2] - corners[3]).normalized()),
		Line2(corners[0], (corners[3] - corners[0]).normalized())
	};

	const unsigned int frameStrideElements = width + yFramePaddingElements;

	const unsigned int maxPerpendicularSearchDistance = std::max(1u, (unsigned int)Numeric::round32(finderPattern.moduleSize() * Scalar(0.25)));

	for (size_t i = 0; i < 4; ++i)
	{
		const size_t j = (i + 1) & 0b0011; // ~ j = (i + 1) % 4
		const Vector2 edge_ji = corners[j] - corners[i];
		const Vector2 perpendicularOut = edge_ji.perpendicular();

		// Find up to number of `tMaxSupportPoints` line points, that will be used to refine the outer edges of a finder pattern. However, it could be less points than that

		Vector2 stepWidth = edge_ji / Scalar(tMaxSupportPoints + 1u);

		unsigned int linePointsCount = 0u;
		Vector2 linePoints[tMaxSupportPoints];
		unsigned int stepIndices[tMaxSupportPoints];
		VectorT2<unsigned int> pixelLocationsIn[tMaxSupportPoints];
		VectorT2<unsigned int> pixelLocationsOut[tMaxSupportPoints];
		Scalar pixelValuesIn[tMaxSupportPoints];
		Scalar pixelValuesOut[tMaxSupportPoints];
		Scalar interpolationWeights[tMaxSupportPoints];

		for (unsigned int k = 0u; k < tMaxSupportPoints; ++k)
		{
			const Vector2 point = corners[i] + stepWidth * Scalar(k + 1u);

			const unsigned int x = (unsigned int)Numeric::round32(point.x());
			const unsigned int y = (unsigned int)Numeric::round32(point.y());

			if (x >= width || y >= height)
			{
				continue;
			}

			unsigned int columns = 0u;
			unsigned int rows = 0u;
			bool foundTransition = false;

			// If the value of the current pixel (x, y) is below the threshold, search in the outward direction (perpendicular to edge and away from the center of the finder pattern), otherwise search in the inward direction

			if (yFrame[y * frameStrideElements + x] < finderPattern.grayThreshold())
			{
				Bresenham bresenham(int(x), int(y), Numeric::round32(point.x() + perpendicularOut.x()), Numeric::round32(point.y() + perpendicularOut.y()));

				if (bresenham.isValid())
				{
					foundTransition = TransitionDetector::findNextPixel<false>(yFrame, x, y, width, height, yFramePaddingElements, bresenham, maxPerpendicularSearchDistance, finderPattern.grayThreshold(), columns, rows, pixelLocationsIn[linePointsCount], pixelLocationsOut[linePointsCount]);
				}
			}
			else
			{
				ocean_assert(yFrame[y * frameStrideElements + x] >= finderPattern.grayThreshold());
				Bresenham bresenham(int(x), int(y), Numeric::round32(point.x() - perpendicularOut.x()), Numeric::round32(point.y() - perpendicularOut.y()));

				if (bresenham.isValid())
				{
					foundTransition = TransitionDetector::findNextPixel<true>(yFrame, x, y, width, height, yFramePaddingElements, bresenham, maxPerpendicularSearchDistance, finderPattern.grayThreshold(), columns, rows, pixelLocationsOut[linePointsCount], pixelLocationsIn[linePointsCount]);
				}
			}

			if (foundTransition == false)
			{
				continue;
			}

			ocean_assert(linePointsCount < tMaxSupportPoints);
			ocean_assert(pixelLocationsIn[linePointsCount].x() < width && pixelLocationsIn[linePointsCount].y() < height);
			ocean_assert(pixelLocationsOut[linePointsCount].x() < width && pixelLocationsOut[linePointsCount].y() < height);

			pixelValuesIn[linePointsCount] = Scalar(yFrame[pixelLocationsIn[linePointsCount].y() * frameStrideElements + pixelLocationsIn[linePointsCount].x()]);
			pixelValuesOut[linePointsCount] = Scalar(yFrame[pixelLocationsOut[linePointsCount].y() * frameStrideElements + pixelLocationsOut[linePointsCount].x()]);
			ocean_assert(pixelValuesIn[linePointsCount] < Scalar(finderPattern.grayThreshold()) && pixelValuesOut[linePointsCount] >= Scalar(finderPattern.grayThreshold()));

			interpolationWeights[linePointsCount] = (pixelValuesOut[linePointsCount] - Scalar(finderPattern.grayThreshold())) / (pixelValuesOut[linePointsCount] - pixelValuesIn[linePointsCount]);
			ocean_assert(interpolationWeights[linePointsCount] >= 0 && interpolationWeights[linePointsCount] <= 1);

			linePoints[linePointsCount] = Vector2(pixelLocationsOut[linePointsCount]) + (Vector2(pixelLocationsIn[linePointsCount]) - Vector2(pixelLocationsOut[linePointsCount])) * interpolationWeights[linePointsCount];
			ocean_assert(linePoints[linePointsCount].x() >= 0 && linePoints[linePointsCount].x() < Scalar(width) && linePoints[linePointsCount].y() >= 0 && linePoints[linePointsCount].y() < Scalar(height));

			stepIndices[linePointsCount] = k;

			linePointsCount++;
		}

		if (linePointsCount >= 2u)
		{
			Line2 line;
			if (Line2::fitLineLeastSquare(linePoints, linePointsCount, line) == false)
			{
				continue;
			}

			// Iteratively refine the line estimate using the measured line points from above until the change between iteration drops below epsilon

			Vector2 projectedLinePoints[tMaxSupportPoints];

			for (unsigned int refinementIteration = 0u; refinementIteration < 5u; ++refinementIteration)
			{
				// Compute corners of the current line estimate
				//
				// c0     e0     c3
				//    *--------*
				//    |        |
				// e1 |        | e3
				//    |        |
				//    *--------*
				// c1     e2    c2
				//
				// c_i - i-th corner
				// e_j - j-th edge line
				//
				// c_A = intersection(e_A+1, e_A)
				// c_B = intersection(e_A+2, e_A), B = A + 1

				Vector2 cornerA;
				Vector2 cornerB;

				if (line.intersection(edgeLines[i], cornerA) == false || line.intersection(edgeLines[(i + 2) & 0b0011], cornerB) == false) // (i + 2) & 0b0011 == (i + 2) % 4
				{
					break;
				}

				ocean_assert(Numeric::isNotWeakEqualEps(cornerA.sqrDistance(cornerB)));

				// Project initially measured line points on the corresponding line point of the current line estimate.

				stepWidth = (cornerB - cornerA) / Scalar(tMaxSupportPoints + 1u);
				for (unsigned int pointIndex = 0u; pointIndex < linePointsCount; ++pointIndex)
				{
					const Vector2 point = cornerA + stepWidth * Scalar(stepIndices[pointIndex] + 1u);
					projectedLinePoints[pointIndex] = point + line.normal() * (line.normal() * (linePoints[pointIndex] - point));
				}

				Line2 newLine;
				if (Line2::fitLineLeastSquare(projectedLinePoints, linePointsCount, newLine) == false || line.normal().angle(newLine.normal()) < Numeric::deg2rad(Numeric::eps()))
				{
					break;
				}

				ocean_assert(newLine.isValid());
				line = newLine;
			}

			ocean_assert(line.isValid());
			edgeLines[(i + 1) & 0b0011] = line; // (i + 1) & 0b0011 == (i + 1) % 4
		}
	}

	Vector2 refinedCorners[4];

	if ((edgeLines[0].intersection(edgeLines[1], refinedCorners[0])
		&& edgeLines[1].intersection(edgeLines[2], refinedCorners[1])
		&& edgeLines[2].intersection(edgeLines[3], refinedCorners[2])
		&& edgeLines[3].intersection(edgeLines[0], refinedCorners[3])) == false)
	{
		return false;
	}

	// Enforce counter-clockwise order of the corners
	const bool isCounterClockwise = (refinedCorners[1] - refinedCorners[0]).cross(refinedCorners[3] - refinedCorners[0]) <= 0 &&
		(refinedCorners[2] - refinedCorners[1]).cross(refinedCorners[0] - refinedCorners[1]) <= 0 &&
		(refinedCorners[3] - refinedCorners[2]).cross(refinedCorners[1] - refinedCorners[2]) <= 0 &&
		(refinedCorners[0] - refinedCorners[3]).cross(refinedCorners[2] - refinedCorners[3]) <= 0;

	if (!isCounterClockwise)
	{
		// The optimization result is invalid because it inverted the direction of the corners.
		return false;
	}

	// Compute the dominant orientation of this finder pattern from its edge lines

	const Vector2 center = (refinedCorners[0] + refinedCorners[1] + refinedCorners[2] + refinedCorners[3]) * Scalar(0.25);

	const Vector2 centerToEdgeMidpoints[4] =
	{
		(refinedCorners[0] + (refinedCorners[1] - refinedCorners[0]) * Scalar(0.5)) - center,
		(refinedCorners[1] + (refinedCorners[2] - refinedCorners[1]) * Scalar(0.5)) - center,
		(refinedCorners[2] + (refinedCorners[3] - refinedCorners[2]) * Scalar(0.5)) - center,
		(refinedCorners[3] + (refinedCorners[0] - refinedCorners[3]) * Scalar(0.5)) - center
	};

	const Scalar xAxisAlignment[4] =
	{
		Vector2(1, 0) * centerToEdgeMidpoints[0],
		Vector2(1, 0) * centerToEdgeMidpoints[1],
		Vector2(1, 0) * centerToEdgeMidpoints[2],
		Vector2(1, 0) * centerToEdgeMidpoints[3]
	};

	size_t maxIndex = xAxisAlignment[0] > xAxisAlignment[1] ? 0 : 1;
	maxIndex = xAxisAlignment[2] > xAxisAlignment[maxIndex] ? 2 : maxIndex;
	maxIndex = xAxisAlignment[3] > xAxisAlignment[maxIndex] ? 3 : maxIndex;

	const Vector2 sumCenterToMidPoints = centerToEdgeMidpoints[(maxIndex + 0) & 0b0011] // (maxIndex + x) & 0b0011 == (maxIndex + x) % 4
		+ centerToEdgeMidpoints[(maxIndex + 1) & 0b0011].perpendicular()
		- centerToEdgeMidpoints[(maxIndex + 2) & 0b0011]
		- centerToEdgeMidpoints[(maxIndex + 3) & 0b0011].perpendicular();

	const Vector2 orientation = sumCenterToMidPoints.normalizedOrZero();

	// Compute the module size based on the known geometry of the finder pattern and the measured pixel distances

	ocean_assert(sumCenterToMidPoints.length() > Scalar(0));
	const Scalar moduleSize = sumCenterToMidPoints.length() / Scalar(14); // 1/14 = 1/4 * 2/7, i.e. average and no. modules from center to the mid-points

	finderPattern = FinderPattern(center, finderPattern.length(), finderPattern.centerIntensity(), finderPattern.grayThreshold(), finderPattern.symmetryScore(), refinedCorners, orientation, moduleSize);

	return true;
}

bool FinderPatternDetector::checkFinderPatternInNeighborhood(const uint8_t* const yFrame, const unsigned width, const unsigned height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const unsigned int threshold, const unsigned int blackSquareSegmentMin, const unsigned int blackSquareSegmentMax, const unsigned int whiteSquareSegmentMin, const unsigned int whiteSquareSegmentMax, const unsigned int centerSegmentMin, const unsigned int centerSegmentMax, Scalar& symmetryScore, Vector2* edgePoints)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 29u && height >= 29u);
	ocean_assert(xCenter < width && yCenter < height);
	ocean_assert(edgePoints != nullptr);

	// Apply directional scanline checks: 8 directions yielding 16 edge points pairs: (i, i + 8)
	//
	//                      4 (90.0 deg)
	// 5 (... deg)  #################  3 (67.5 deg)
	//           ########### ###########
	//        #######               #######
	//    6  #####                     #####  2 (45.0 deg)
	//     #####                         #####
	//    #####                           #####
	// 7 #####                             ##### 1 (22.5 deg)
	//   ####                               ####
	//   ####                               ####
	// 8 ###----------------+----------------### 0 (0.0 deg)
	//   ####                               ####
	//   ####                               ####
	// 9 #####                             ##### 15
	//    #####                           #####
	//     #####                         #####
	//   10  #####                     #####  14
	//        #######               #######
	//           ########### ###########
	//          11  #################  13
	//                     12
	const Scalar angles[8] =
	{
		Numeric::deg2rad(Scalar(0.0)),
		Numeric::deg2rad(Scalar(22.5)),
		Numeric::deg2rad(Scalar(45.0)),
		Numeric::deg2rad(Scalar(67.5)),
		Numeric::deg2rad(Scalar(90.0)),
		Numeric::deg2rad(Scalar(112.5)),
		Numeric::deg2rad(Scalar(135.0)),
		Numeric::deg2rad(Scalar(157.5)),
	};

	for (unsigned int i = 0u; i < 8; ++i)
	{
		if (checkFinderPatternDirectional(yFrame, width, height, paddingElements, xCenter, yCenter, angles[i], threshold, blackSquareSegmentMin, blackSquareSegmentMax, whiteSquareSegmentMin, whiteSquareSegmentMax, centerSegmentMin, centerSegmentMax, edgePoints[i], edgePoints[i + 8]) == false)
		{
			return false;
		}
	}

#if defined(OCEAN_DEBUG)
	for (size_t i = 0; i < 16; ++i)
	{
		ocean_assert(edgePoints[i].x() >= Scalar(0) && edgePoints[i].x() < Scalar(width) && edgePoints[i].y() >= 0 && edgePoints[i].y() < Scalar(height) && "Edge point not inside image boundaries");
		ocean_assert(Numeric::isNotEqualEps(edgePoints[i].distance(edgePoints[(i + 1) % 16])) && "Adjacent edge points must have different locations");
	}
#endif

	const Vector2 center = Vector2(Scalar(xCenter), Scalar(yCenter));

	Scalar sumSymmetricAbsoluteSquareDistanceDifferences = Scalar(0);
	Scalar minSquareDistance = Numeric::maxValue();
	Scalar maxSquareDistance = Scalar(0);

	for (size_t i = 0; i < 8; ++i)
	{
		const Scalar squareDistance0 = center.sqrDistance(edgePoints[i]);
		const Scalar squareDistance1 = center.sqrDistance(edgePoints[i + 8]);

		sumSymmetricAbsoluteSquareDistanceDifferences += std::abs(squareDistance0 - squareDistance1);

		minSquareDistance = std::min(minSquareDistance, std::min(squareDistance0, squareDistance1));
		maxSquareDistance = std::max(maxSquareDistance, std::max(squareDistance0, squareDistance1));
	}
	ocean_assert(sumSymmetricAbsoluteSquareDistanceDifferences >= Scalar(0));

	// Make sure the ratio of min length to max length is reasonable and check if the center block contains enough black pixels

	if (minSquareDistance > Scalar(0) && minSquareDistance <= maxSquareDistance && minSquareDistance / maxSquareDistance >= Scalar(0.15))
	{
		const int radius = std::max(2, Numeric::round32(Numeric::sqrt(minSquareDistance) / Scalar(7)));

		if (int(xCenter) >= radius && int(yCenter) >= radius && int(xCenter) + radius < int(width) && int(yCenter) + radius < int(height))
		{
			for (int y = int(yCenter) - radius; y <= int(yCenter) + radius; ++y)
			{
				ocean_assert(y >= 0 && y < int(height));

				const uint8_t* const yRow = yFrame + y * (width + paddingElements);

				for (int x = int(xCenter) - radius; x <= int(xCenter) + radius; ++x)
				{
					ocean_assert(x >= 0 && x < int(width));

					if (yRow[x] >= threshold)
					{
						return false;
					}
				}
			}

			symmetryScore = sumSymmetricAbsoluteSquareDistanceDifferences;

			return true;
		}
	}

	return false;
}

bool FinderPatternDetector::checkFinderPatternDirectional(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int xCenter, const unsigned int yCenter, const Scalar angle, const unsigned int threshold, const unsigned int blackSquareSegmentMin, const unsigned int blackSquareSegmentMax, const unsigned int whiteSquareSegmentMin, const unsigned int whiteSquareSegmentMax, const unsigned int centerSegmentMin, const unsigned int centerSegmentMax, Vector2& topBorder, Vector2& bottomBorder)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(xCenter < width && yCenter < height);
	ocean_assert(Numeric::isInsideRange(Scalar(0), angle, Numeric::pi()));

#if 1
	const Vector3 scanlineDirectionHomogeneous = Quaternion(Vector3(0, 0, 1), -angle) * Vector3(1, 0, 1);
	ocean_assert(Numeric::isNotEqualEps(scanlineDirectionHomogeneous.z()));
	const Vector2 scanlineDirection = Vector2(scanlineDirectionHomogeneous.x() / scanlineDirectionHomogeneous.z(), scanlineDirectionHomogeneous.y() / scanlineDirectionHomogeneous.z()) * Scalar(std::max(10000u, std::max(width, height)));
#else
	const Vector2 scanlineDirection = (SquareMatrix3(Rotation(Vector3(0, 0, 1), -angle)) * Vector2(1, 0)) * Scalar(std::max(10000u, std::max(width, height)));
#endif

	Bresenham bresenhamTop(Numeric::round32(Scalar(xCenter)), Numeric::round32(Scalar(yCenter)), Numeric::round32(Scalar(xCenter) + scanlineDirection.x()), Numeric::round32(Scalar(yCenter) + scanlineDirection.y()));
	Bresenham bresenhamBottom(Numeric::round32(Scalar(xCenter)), Numeric::round32(Scalar(yCenter)), Numeric::round32(Scalar(xCenter) - scanlineDirection.x()), Numeric::round32(Scalar(yCenter) - scanlineDirection.y()));

	if (bresenhamTop.isValid() == false || bresenhamBottom.isValid() == false)
	{
		return false;
	}

	// The black center square

	const unsigned int centerSegmentMaxHalf = (centerSegmentMax + 1u) / 2u;
	const unsigned int centerSegmentSymmetricTolerance = max(1u, centerSegmentMax * 179u / 512u); // ~ 35%

	unsigned int topColumns;
	unsigned int topRows;
	unsigned int bottomColumns;
	unsigned int bottomRows;

	VectorT2<unsigned int> topIn;
	VectorT2<unsigned int> topOut;

	VectorT2<unsigned int> bottomIn;
	VectorT2<unsigned int> bottomOut;

	if (TransitionDetector::findNextPixel<false>(yFrame, xCenter, yCenter, width, height, paddingElements, bresenhamTop, centerSegmentMaxHalf + 1u, threshold, topColumns, topRows, topIn, topOut) == false
		|| TransitionDetector::findNextPixel<false>(yFrame, xCenter, yCenter, width, height, paddingElements, bresenhamBottom, centerSegmentMaxHalf + 1u, threshold, bottomColumns, bottomRows, bottomIn, bottomOut) == false
		|| NumericT<unsigned int>::isNotEqual(topRows * topRows + topColumns * topColumns, bottomRows * bottomRows + bottomColumns * bottomColumns, centerSegmentSymmetricTolerance * centerSegmentSymmetricTolerance)
		|| (topColumns + bottomColumns - 1u) * (topColumns + bottomColumns - 1u) + (topRows + bottomRows - 1u) * (topRows + bottomRows - 1u) < centerSegmentMin * centerSegmentMin // (topColumns + bottomColums - 1): as top and bottom include the center pixel
		|| (topColumns + bottomColumns - 1u) * (topColumns + bottomColumns - 1u) + (topRows + bottomRows - 1u) * (topRows + bottomRows - 1u) > centerSegmentMax * centerSegmentMax
		)
	{
		return false;
	}

	ocean_assert(topIn.x() < width && topIn.y() < height && topOut.x() < width && topOut.y() < height);
	ocean_assert(bottomIn.x() < width && bottomIn.y() < height && bottomOut.x() < width && bottomOut.y() < height);

	// The inner white square

	if (TransitionDetector::findNextPixel<true>(yFrame, topOut.x(), topOut.y(), width, height, paddingElements, bresenhamTop, whiteSquareSegmentMax + 1u, threshold, topColumns, topRows, topIn, topOut) == false
		|| TransitionDetector::findNextPixel<true>(yFrame, bottomOut.x(), bottomOut.y(), width, height, paddingElements, bresenhamBottom, whiteSquareSegmentMax + 1u, threshold, bottomColumns, bottomRows, bottomIn, bottomOut) == false
		|| (topColumns + topRows) * (topColumns + topRows) < whiteSquareSegmentMin * whiteSquareSegmentMin
		|| (bottomColumns + bottomRows) * (bottomColumns + bottomRows) < whiteSquareSegmentMin * whiteSquareSegmentMin
		|| (topColumns + topRows) * (topColumns + topRows) > whiteSquareSegmentMax * whiteSquareSegmentMax
		|| (bottomColumns + bottomRows) * (bottomColumns + bottomRows) > whiteSquareSegmentMax * whiteSquareSegmentMax
		)
	{
		return false;
	}

	ocean_assert(topIn.x() < width && topIn.y() < height && topOut.x() < width && topOut.y() < height);
	ocean_assert(bottomIn.x() < width && bottomIn.y() < height && bottomOut.x() < width && bottomOut.y() < height);

	// The outer black square

	if (TransitionDetector::findNextPixel<false>(yFrame, topOut.x(), topOut.y(), width, height, paddingElements, bresenhamTop, blackSquareSegmentMax + 1u, threshold, topColumns, topRows, topIn, topOut) == false
		|| TransitionDetector::findNextPixel<false>(yFrame, bottomOut.x(), bottomOut.y(), width, height, paddingElements, bresenhamBottom, blackSquareSegmentMax + 1u, threshold, bottomColumns, bottomRows, bottomIn, bottomOut) == false
		|| (topColumns + topRows) * (topColumns + topRows) < blackSquareSegmentMin * blackSquareSegmentMin
		|| (bottomColumns + bottomRows) * (bottomColumns + bottomRows) < blackSquareSegmentMin * blackSquareSegmentMin
		|| (topColumns + topRows) * (topColumns + topRows) > blackSquareSegmentMax * blackSquareSegmentMax
		|| (bottomColumns + bottomRows) * (bottomColumns + bottomRows) > blackSquareSegmentMax * blackSquareSegmentMax
		)
	{
		return false;
	}

	// Determine the location of the top border with sub-pixel accuracy

	topBorder = TransitionDetector::computeTransitionPointSubpixelAccuracy(yFrame, width, height, paddingElements, topIn, topOut, threshold);
	ocean_assert(topBorder.x() >= 0 && topBorder.x() < Scalar(width) && topBorder.y() >= 0 && topBorder.y() < Scalar(height));

	bottomBorder = TransitionDetector::computeTransitionPointSubpixelAccuracy(yFrame, width, height, paddingElements, bottomIn, bottomOut, threshold);
	ocean_assert(bottomBorder.x() >= 0 && bottomBorder.x() < Scalar(width) && bottomBorder.y() >= 0 && bottomBorder.y() < Scalar(height));

	return true;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
