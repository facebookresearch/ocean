/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/MarkerCandidate.h"
#include "ocean/cv/calibration/Marker.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

MarkerCandidate::MarkerCandidate(const Indices32& borderIndices, const Points& points)
{
	ocean_assert(borderIndices.size() == 16);

	if (borderIndices.size() == 16)
	{
#ifdef OCEAN_DEBUG
		for (const Index32& borderIndex : borderIndices)
		{
			ocean_assert(borderIndex < points.size());
		}
#endif

		/**
		 * Border indices:
		 *  0  1  2  3  4
		 * 15           5
		 * 14           6
		 * 13           7
		 * 12  11 10 9  8
		 */

		setSign(points[borderIndices[0]].sign());

		for (const Index32 borderIndex : borderIndices)
		{
			ocean_assert_and_suppress_unused(sign() == points[borderIndex].sign(), borderIndex);
		}

		const Vector2& point0 = points[borderIndices[0]].observation();
		const Vector2& point4 = points[borderIndices[4]].observation();
		const Vector2& point12 = points[borderIndices[12]].observation();

		const Vector2 directionA = point4 - point0;
		const Vector2 directionB = point12 - point0;

		const bool counterClockWise = directionA.cross(directionB) < 0;

		pointIndices_ = borderIndicesToMarkerIndices(borderIndices, !counterClockWise);
	}
}

Index32 MarkerCandidate::pointIndex(const size_t indexInMarker) const
{
	/**
	 * Marker indices:
	 *  0  1  2  3  4
	 *  5  6  7  8  9
	 * 10 11 12 13 14
	 * 15 16 17 18 19
	 * 20 21 22 23 24
	 */

	ocean_assert(pointIndices_.size() == 25);
	ocean_assert(indexInMarker < 25);

	if (indexInMarker >= pointIndices_.size())
	{
		return Index32(-1);
	}

	return pointIndices_[indexInMarker];
}

Index32 MarkerCandidate::borderIndex(const size_t indexInBorder) const
{
	/**
	 * Border indices:
	 *  0  1  2  3  4
	 * 15           5
	 * 14           6
	 * 13           7
	 * 12  11 10 9  8
	 *
	 * Marker indices:
	 *  0  1  2  3  4
	 *  5  6  7  8  9
	 * 10 11 12 13 14
	 * 15 16 17 18 19
	 * 20 21 22 23 24
	 */

	ocean_assert(indexInBorder < 16);

	constexpr uint8_t borderIndices[16] = {0, 1, 2, 3, 4, 9, 14, 19, 24, 23, 22, 21, 20, 15, 10, 5};

	const uint8_t indexInMarker = borderIndices[indexInBorder];

	return pointIndex(size_t(indexInMarker));
}

void MarkerCandidate::setPointIndex(const size_t indexInMarker, const Index32 pointIndex)
{
	/**
	 * Marker indices:
	 *  0  1  2  3  4
	 *  5  6  7  8  9
	 * 10 11 12 13 14
	 * 15 16 17 18 19
	 * 20 21 22 23 24
	 */

	ocean_assert(indexInMarker < 25);

	pointIndices_[indexInMarker] = pointIndex;
}

MarkerCandidate MarkerCandidate::rotatedClockWise(const int degree) const
{
	MarkerCandidate markerCandidate(*this);

	markerCandidate.rotateClockWise(degree);

	return markerCandidate;
}

void MarkerCandidate::rotateClockWise(const int degree)
{
	ocean_assert(degree % 90 == 0);

	const int steps = degree / 90;
	ocean_assert(steps >= -4 && steps <= 4);

	switch (steps)
	{
		case 0:
		case 4:
		case -4:
			// nothing to do here
			break;

		case 1:
		case -3:
			pointIndices_ = rotateIndicesClockWise90(pointIndices_);
			break;

		case 2:
		case -2:
			pointIndices_ = rotateIndices180(pointIndices_);
			break;

		case 3:
		case -1:
			pointIndices_ = rotateIndicesCounterClockWise90(pointIndices_);
			break;

		default:
			ocean_assert(false && "This should never happen!");
			break;
	}

	if (!neighborMap_.empty() && steps >= -3 && steps <= 3)
	{
		NeighborMap rotatedDirectionToNeighborMap;
		rotatedDirectionToNeighborMap.reserve(neighborMap_.size());

		for (NeighborMap::value_type& neighborPair : neighborMap_)
		{
			const int edgeAngle = int(neighborPair.first);

			/**
			 * X---------------------       X---------------------
			 * |                     |      |                     |
			 * |    NW    N    NE    |      |    45    0    315   |
			 * |                     |      |                     |
			 * |     W    P    E     |      |    90    P    270   |
			 * |                     |      |                     |
			 * |    SW    S    SE    |      |   135   180   225   |
			 * |                     |      |                     |
			 *  ---------------------        ---------------------
			 */

			const int rotatedAngle = modulo<int>(edgeAngle - degree, 360); // pixel direction is counter clockwise, so we subtract the clockwise rotation angle
			ocean_assert(rotatedAngle >= 0 && rotatedAngle < 360);

			const CV::PixelDirection rotatedEdge = CV::PixelDirection(rotatedAngle);

			ocean_assert(rotatedDirectionToNeighborMap.find(rotatedEdge) == rotatedDirectionToNeighborMap.cend());
			rotatedDirectionToNeighborMap[rotatedEdge] = neighborPair.second;
		}

		neighborMap_ = std::move(rotatedDirectionToNeighborMap);
	}
}

bool MarkerCandidate::isNeighbor(const MarkerCandidate& markerCandidate, const Points& points, CV::PixelDirection& localEdge, CV::PixelDirection& neighborEdge, const Scalar maxDistancePercentage) const
{
	ocean_assert(hasValidBorderIndices());
	ocean_assert(markerCandidate.hasValidBorderIndices());
	ocean_assert(maxDistancePercentage > Numeric::eps());

	if (sign() == markerCandidate.sign())
	{
		// neighbors have different signs
		return false;
	}

	/**
	 * Marker indices:
	 *  0  1  2  3  4
	 *  5           9
	 * 10          14
	 * 15          19
	 * 20 21 22 23 24
	 */

	const Vector2& point0 = points[pointIndices_[0]].observation();
	const Vector2& point5 = points[pointIndices_[5]].observation();

	const Vector2& point4 = points[pointIndices_[4]].observation();
	const Vector2& point9 = points[pointIndices_[9]].observation();

	const Vector2 offset0_5 = point0 - point5;
	const Vector2 offset4_9 = point4 - point9;

	const Vector2 predicted0_5 = point0 + offset0_5;
	const Vector2 predicted4_9 = point4 + offset4_9;

	CV::PixelDirection neighborDirection = markerCandidate.hasEdge(predicted0_5, predicted4_9, points, offset0_5.sqr() * Numeric::sqr(maxDistancePercentage), offset4_9.sqr() * Numeric::sqr(maxDistancePercentage));

	if (neighborDirection != CV::PD_INVALID)
	{
		localEdge = CV::PD_NORTH;
		neighborEdge = neighborDirection;

		return true;
	}

	const Vector2& point3 = points[pointIndices_[3]].observation();

	const Vector2& point24 = points[pointIndices_[24]].observation();
	const Vector2& point23 = points[pointIndices_[23]].observation();

	const Vector2 offset4_3 = point4 - point3;
	const Vector2 offset24_23 = point24 - point23;

	const Vector2 predicted4_3 = point4 + offset4_3;
	const Vector2 predicted24_23 = point24 + offset24_23;

	neighborDirection = markerCandidate.hasEdge(predicted4_3, predicted24_23, points, offset4_3.sqr() * Numeric::sqr(maxDistancePercentage), offset24_23.sqr() * Numeric::sqr(maxDistancePercentage));

	if (neighborDirection != CV::PD_INVALID)
	{
		localEdge = CV::PD_EAST;
		neighborEdge = neighborDirection;

		return true;
	}

	const Vector2& point19 = points[pointIndices_[19]].observation();

	const Vector2& point20 = points[pointIndices_[20]].observation();
	const Vector2& point15 = points[pointIndices_[15]].observation();

	const Vector2 offset24_19 = point24 - point19;
	const Vector2 offset20_15 = point20 - point15;

	const Vector2 predicted24_19 = point24 + offset24_19;
	const Vector2 predicted20_15 = point20 + offset20_15;

	neighborDirection = markerCandidate.hasEdge(predicted24_19, predicted20_15, points, offset24_19.sqr() * Numeric::sqr(maxDistancePercentage), offset20_15.sqr() * Numeric::sqr(maxDistancePercentage));

	if (neighborDirection != CV::PD_INVALID)
	{
		localEdge = CV::PD_SOUTH;
		neighborEdge = neighborDirection;

		return true;
	}

	const Vector2& point21 = points[pointIndices_[21]].observation();

	const Vector2& point1 = points[pointIndices_[1]].observation();

	const Vector2 offset20_21 = point20 - point21;
	const Vector2 offset0_1 = point0 - point1;

	const Vector2 predicted20_21 = point20 + offset20_21;
	const Vector2 predicted0_1 = point0 + offset0_1;

	neighborDirection = markerCandidate.hasEdge(predicted20_21, predicted0_1, points, offset20_21.sqr() * Numeric::sqr(maxDistancePercentage), offset0_1.sqr() * Numeric::sqr(maxDistancePercentage));

	if (neighborDirection != CV::PD_INVALID)
	{
		localEdge = CV::PD_WEST;
		neighborEdge = neighborDirection;

		return true;
	}

	return false;
}

CV::PixelDirection MarkerCandidate::hasEdge(const Vector2& predictedCornerA, const Vector2& predictedCornerB, const Points& points, const Scalar maxSqrDistanceA, const Scalar maxSqrDistanceB) const
{
	ocean_assert(hasValidBorderIndices());
	ocean_assert(maxSqrDistanceA >= 0);
	ocean_assert(maxSqrDistanceB >= 0);

	/**
	 * Marker indices:
	 *  0  1  2  3  4
	 *  5           9
	 * 10          14
	 * 15          19
	 * 20 21 22 23 24
	 */

	const Vector2& point20 = points[pointIndices_[20]].observation();
	const Vector2& point24 = points[pointIndices_[24]].observation();

	if (predictedCornerA.sqrDistance(point20) <= maxSqrDistanceA && predictedCornerB.sqrDistance(point24) <= maxSqrDistanceB)
	{
		// this marker candidate is north of the provided points, so the provided points are in the south
		return CV::PD_SOUTH;
	}

	ocean_assert(predictedCornerA.sqrDistance(point24) > maxSqrDistanceA || predictedCornerB.sqrDistance(point20) > maxSqrDistanceB); // ensure correct marker rotation

	const Vector2& point0 = points[pointIndices_[0]].observation();

	if (predictedCornerA.sqrDistance(point0) <= maxSqrDistanceA && predictedCornerB.sqrDistance(point20) <= maxSqrDistanceB)
	{
		// this marker candidate is east of the provided points, so the provided points are in the west
		return CV::PD_WEST;
	}

	ocean_assert(predictedCornerA.sqrDistance(point20) > maxSqrDistanceA || predictedCornerB.sqrDistance(point0) > maxSqrDistanceB);

	const Vector2& point4 = points[pointIndices_[4]].observation();

	if (predictedCornerA.sqrDistance(point4) <= maxSqrDistanceA && predictedCornerB.sqrDistance(point0) <= maxSqrDistanceB)
	{
		// this marker candidate is south of the provided points, so the provided points are in the north
		return CV::PD_NORTH;
	}

	ocean_assert(predictedCornerA.sqrDistance(point0) > maxSqrDistanceA || predictedCornerB.sqrDistance(point4) > maxSqrDistanceB);

	if (predictedCornerA.sqrDistance(point24) <= maxSqrDistanceA && predictedCornerB.sqrDistance(point4) <= maxSqrDistanceB)
	{
		// this marker candidate is west of the provided points, so the provided points are in the east
		return CV::PD_EAST;
	}

	ocean_assert(predictedCornerA.sqrDistance(point4) > maxSqrDistanceA || predictedCornerB.sqrDistance(point24) > maxSqrDistanceB);

	return CV::PD_INVALID;
}

Vector2 MarkerCandidate::center(const Points& points) const
{
	ocean_assert(hasValidBorderIndices());

	Vector2 point(0, 0);

	size_t validPoints = 0;

	for (const Index32 index : pointIndices_)
	{
		if (index != Index32(-1))
		{
			ocean_assert(index < points.size());

			point += points[index].observation();

			++validPoints;
		}
	}

	ocean_assert(validPoints != 0);

	return point / Scalar(validPoints);
}

bool MarkerCandidate::hasValidBorderIndices() const
{
	if (pointIndices_.size() != 25)
	{
		return false;
	}

	for (size_t n = 0; n < 16; ++n)
	{
		if (borderIndex(n) == Index32(-1))
		{
			return false;
		}
	}

	return true;
}

bool MarkerCandidate::hasValidIndices() const
{
	if (pointIndices_.size() != 25)
	{
		return false;
	}

	for (size_t n = 0; n < 25; ++n)
	{
		if (pointIndices_[n] == Index32(-1))
		{
			return false;
		}
	}

	return true;
}

bool MarkerCandidate::hasNeighborWithMarkerId(const MarkerCandidates& markerCandidates) const
{
	for (NeighborMap::const_iterator iNeighbor = neighborMap_.cbegin(); iNeighbor != neighborMap_.cend(); ++iNeighbor)
	{
		const size_t neighborMarkerCandidateIndex = iNeighbor->second;

		ocean_assert(neighborMarkerCandidateIndex < markerCandidates.size());
		const MarkerCandidate& neighborMarkerCandidate = markerCandidates[neighborMarkerCandidateIndex];

		if (neighborMarkerCandidate.hasMarkerId())
		{
			return true;
		}
	}

	return false;
}

CV::PixelDirection MarkerCandidate::neighborDirection(const size_t markerCandidateIndex) const
{
	for (const NeighborMap::value_type& neighborPair : neighborMap_)
	{
		if (neighborPair.second == markerCandidateIndex)
		{
			return neighborPair.first;
		}
	}

	return CV::PD_INVALID;
}

bool MarkerCandidate::determineMarkerId(const Points& points)
{
	ocean_assert(isValid());
	ocean_assert(hasValidIndices());
	ocean_assert(!hasMarkerId());

	size_t negativeCounter = 25;

	for (size_t indexInMarker = 0; indexInMarker < 25; ++indexInMarker)
	{
		const Index32 index = pointIndex(indexInMarker);

		ocean_assert(index < points.size());
		if (size_t(index) >= points.size())
		{
			return false;
		}

		const Point& point = points[index];

		if (point.sign())
		{
			--negativeCounter;
		}
	}

	const size_t positiveCounter = 25 - negativeCounter;

	if (negativeCounter == 0 || positiveCounter == 0)
	{
		// a marker has at least one toggled point
		return false;
	}

	if (negativeCounter > 4 && positiveCounter > 4) // TODO, currently we use 3
	{
		return false;
	}

	const bool sign = negativeCounter < positiveCounter;

	const Marker::LayoutManager::Layouts& layouts = Marker::LayoutManager::layouts();

	Marker::LayoutManager::Layout layout;
	memset(layout.data(), 1, sizeof(Marker::LayoutManager::Layout));

	for (size_t indexInMarker = 0; indexInMarker < 25; ++indexInMarker)
	{
		const Index32 index = pointIndex(indexInMarker);

		const Point& point = points[index];

		if (point.sign() != sign)
		{
			layout[indexInMarker] = 0u;
		}
	}

	for (size_t markerId = 0; markerId < layouts.size(); ++markerId)
	{
		const CV::PixelDirection orientation = Marker::LayoutManager::isRotated(layouts[markerId], layout);

		if (orientation != CV::PD_INVALID)
		{
			rotateClockWise(-int(orientation));

			setMarkerId(markerId);

			return true;
		}
	}

	return false;
}

Indices32 MarkerCandidate::borderIndicesToMarkerIndices(const Indices32& borderIndices, const bool clockWise)
{
	ocean_assert(borderIndices.size() == 16);

	if (clockWise)
	{
		/**
		 * Border indices in cw:
		 *  0  1  2  3  4
		 * 15           5
		 * 14           6
		 * 13           7
		 * 12  11 10 9  8
		 *
		 * Marker indices:
		 *  0  1  2  3  4
		 *  5  6  7  8  9
		 * 10 11 12 13 14
		 * 15 16 17 18 19
		 * 20 21 22 23 24
		 */

		return
		{
			 borderIndices[0],  borderIndices[1],  borderIndices[2], borderIndices[3], borderIndices[4],
			borderIndices[15],       Index32(-1),       Index32(-1),      Index32(-1), borderIndices[5],
			borderIndices[14],       Index32(-1),       Index32(-1),      Index32(-1), borderIndices[6],
			borderIndices[13],       Index32(-1),       Index32(-1),      Index32(-1), borderIndices[7],
			borderIndices[12], borderIndices[11], borderIndices[10], borderIndices[9], borderIndices[8],
		};
	}
	else
	{
		/**
		 * Border indices in ccw:
		 *  0 15 14 13 12
		 *  1          11
		 *  2          10
		 *  3           9
		 *  4  5  6  7  8
		 *
		 * Marker indices:
		 *  0  1  2  3  4
		 *  5  6  7  8  9
		 * 10 11 12 13 14
		 * 15 16 17 18 19
		 * 20 21 22 23 24
		 */

		return
		{
			borderIndices[0], borderIndices[15], borderIndices[14], borderIndices[13], borderIndices[12],
			borderIndices[1],       Index32(-1),       Index32(-1),       Index32(-1), borderIndices[11],
			borderIndices[2],       Index32(-1),       Index32(-1),       Index32(-1), borderIndices[10],
			borderIndices[3],       Index32(-1),       Index32(-1),       Index32(-1), borderIndices[9],
			borderIndices[4],  borderIndices[5],  borderIndices[6],  borderIndices[7], borderIndices[8],
		};
	}
}

Indices32 MarkerCandidate::rotateIndicesClockWise90(const Indices32& indices)
{
	/**
	 * Normal:
	 *  0  1  2  3  4
	 *  5  6  7  8  9
	 * 10 11 12 13 14
	 * 15 16 17 18 19
	 * 20 21 22 23 24
	 *
	 * Rotated right by 90 degree:
	 * 20 15 10 5 0
	 * 21 16 11 6 1
	 * 22 17 12 7 2
	 * 23 18 13 8 3
	 * 24 19 14 9 4
	 */

	ocean_assert(indices.size() == 25);

	return
	{
		indices[20], indices[15], indices[10], indices[5], indices[0],
		indices[21], indices[16], indices[11], indices[6], indices[1],
		indices[22], indices[17], indices[12], indices[7], indices[2],
		indices[23], indices[18], indices[13], indices[8], indices[3],
		indices[24], indices[19], indices[14], indices[9], indices[4]
	};
}

Indices32 MarkerCandidate::rotateIndices180(const Indices32& indices)
{
	/**
	 * Normal:
	 *  0  1  2  3  4
	 *  5  6  7  8  9
	 * 10 11 12 13 14
	 * 15 16 17 18 19
	 * 20 21 22 23 24
	 *
	 * Rotated right by 180 degree:
	 * 24 23 22 21 20
	 * 19 18 17 16 15
	 * 14 13 12 11 10
	 *  9  8  7  6  5
	 *  4  3  2  1  0
	 */

	ocean_assert(indices.size() == 25);

	return
	{
		indices[24], indices[23], indices[22], indices[21], indices[20],
		indices[19], indices[18], indices[17], indices[16], indices[15],
		indices[14], indices[13], indices[12], indices[11], indices[10],
		indices[9], indices[8], indices[7], indices[6], indices[5],
		indices[4], indices[3], indices[2], indices[1], indices[0]
	};
}

Indices32 MarkerCandidate::rotateIndicesCounterClockWise90(const Indices32& indices)
{
	/**
	 * Normal:
	 *  0  1  2  3  4
	 *  5  6  7  8  9
	 * 10 11 12 13 14
	 * 15 16 17 18 19
	 * 20 21 22 23 24
	 *
	 * Rotated left by 90 degree:
	 * 4 9 14 19 24
	 * 3 8 13 18 23
	 * 2 7 12 17 22
	 * 1 6 11 16 21
	 * 0 5 10 15 20
	 */

	ocean_assert(indices.size() == 25);

	return
	{
		indices[4], indices[9], indices[14], indices[19], indices[24],
		indices[3], indices[8], indices[13], indices[18], indices[23],
		indices[2], indices[7], indices[12], indices[17], indices[22],
		indices[1], indices[6], indices[11], indices[16], indices[21],
		indices[0], indices[5], indices[10], indices[15], indices[20]
	};
}

void MarkerCandidate::removeMarkerCandidate(MarkerCandidates& markerCandidates, const size_t index)
{
	ocean_assert(index < markerCandidates.size());

	if (markerCandidates.size() == 1)
	{
		markerCandidates.clear();
		return;
	}

#ifdef OCEAN_DEBUG
	for (const MarkerCandidate& markerCandidate : markerCandidates)
	{
		UnorderedIndexSet32 links;
		for (const MarkerCandidate::NeighborMap::value_type& neighborPair : markerCandidate.neighborMap_)
		{
			const size_t neighborMarkerCandidateIndex = neighborPair.second;

			ocean_assert(neighborMarkerCandidateIndex < markerCandidates.size());
			ocean_assert(links.emplace(Index32(neighborMarkerCandidateIndex)).second);
		}

		ocean_assert(links.size() == markerCandidate.neighborMap_.size());
	}
#endif

	const size_t backIndex = markerCandidates.size() - 1;

	for (MarkerCandidate& markerCandidate : markerCandidates)
	{
		for (MarkerCandidate::NeighborMap::iterator iNeighbor = markerCandidate.neighborMap_.begin(); iNeighbor != markerCandidate.neighborMap_.end(); /*noop*/)
		{
			size_t& neighborMarkerCandidateIndex = iNeighbor->second;

			if (neighborMarkerCandidateIndex == index)
			{
				// let's remove the neighbor link to the marker to be removed

				iNeighbor = markerCandidate.neighborMap_.erase(iNeighbor);
			}
			else
			{
				// we will move the last element to the free spot, so we have to adjust the links to the last element

				if (neighborMarkerCandidateIndex == backIndex)
				{
					ocean_assert(index != backIndex); // will never be the last element which will not exist anymore
					neighborMarkerCandidateIndex = index;
				}

				++iNeighbor;
			}
		}
	}

	markerCandidates[index] = markerCandidates.back();
	markerCandidates.pop_back();

#ifdef OCEAN_DEBUG
	for (const MarkerCandidate& markerCandidate : markerCandidates)
	{
		UnorderedIndexSet32 links;
		for (const MarkerCandidate::NeighborMap::value_type& neighborPair : markerCandidate.neighborMap_)
		{
			const size_t neighborMarkerCandidateIndex = neighborPair.second;

			ocean_assert(neighborMarkerCandidateIndex < markerCandidates.size());
			ocean_assert(links.emplace(Index32(neighborMarkerCandidateIndex)).second);
		}

		ocean_assert(links.size() == markerCandidate.neighborMap_.size());
	}
#endif
}

}

}

}
