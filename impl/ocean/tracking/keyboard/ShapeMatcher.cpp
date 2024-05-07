// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/keyboard/ShapeMatcher.h"

#include "ocean/base/Base.h"
#include "ocean/cv/Bresenham.h"
#include "ocean/cv/NonMaximumSuppression.h"

#include "ocean/math/Line2.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix2.h"

namespace Ocean
{

namespace Tracking
{

namespace Keyboard
{

std::vector<Indices32> ShapeMatcher::determineHorizontalGroups(const unsigned int height, const Vector2* positions, const size_t size, const unsigned int minimalGroupSize, const Scalar maximalGroupRadius, Scalars* groupPositions)
{
	ocean_assert(height >= 1u && positions != nullptr);
	ocean_assert(minimalGroupSize >= 1u);
	ocean_assert(maximalGroupRadius > 0);

	const unsigned int verticalBins = std::min((unsigned int)(Scalar(height) / maximalGroupRadius + Scalar(0.5)), height * 100u);
	ocean_assert(verticalBins >= 1u);

	const Scalar position2bin = Scalar(verticalBins) / Scalar(height);
	const Scalar bin2position = Scalar(height) / Scalar(verticalBins);

	std::vector<Indices32> verticalHistogram(verticalBins + 1); // +1 to avoid extreme rounding issues, we will not use the last element

	for (size_t n = 0; n < size; ++n)
	{
		const Vector2& position = positions[n];
		ocean_assert(position.y() < Scalar(height));

		const unsigned int bin = (unsigned int)(position.y() * position2bin); // no rounding
		ocean_assert(bin < (unsigned int)verticalHistogram.size());

		verticalHistogram[bin].push_back(Index32(n));
	}

	const Scalar maximalGroupRadiusSqr = Numeric::sqr(maximalGroupRadius);

	std::vector<Indices32> positionGroups;

	Indices32 intermediateGroup;
	intermediateGroup.reserve(128);

	for (unsigned int n = 1u; n < verticalBins - 1u; ++n)
	{
		// non-maximum suppression
		if (verticalHistogram[n - 1u].size() < verticalHistogram[n].size() && verticalHistogram[n].size() >= verticalHistogram[n + 1u].size())
		{
			intermediateGroup.clear();

			Scalar sumPositions = Scalar(0);

			for (unsigned int i = n - 1u; i <= n + 1u; ++i)
			{
				const Indices32& indicesInBin = verticalHistogram[i];

				const Scalar binCenterPosition = (Scalar(i) + Scalar(0.5)) * bin2position;

				for (const Index32& index : indicesInBin)
				{
					if (Numeric::sqr(positions[index].y() - binCenterPosition) <= maximalGroupRadiusSqr)
					{
						intermediateGroup.push_back(index);

						sumPositions += positions[index].y();
					}
				}
			}

			if ((unsigned int)intermediateGroup.size() >= minimalGroupSize)
			{
				if (groupPositions)
				{
					groupPositions->push_back(sumPositions / Scalar(intermediateGroup.size()));

				}

				positionGroups.emplace_back(std::move(intermediateGroup));

				// the next bin cannot be a non-maximum suppression bin, so we skip one bin
				n++;
			}
		}
	}

	return positionGroups;
}

ShapeMatcher::LShape ShapeMatcher::mergeShapes(const LShape* lShapes, const Indices32& indices)
{
	ocean_assert(lShapes != nullptr);
	ocean_assert(!indices.empty());

	Vector2 sumPosition(0, 0);
	Vector2 sumDirection(0, 0);
	Scalar sumScore = 0;

	for (const Index32& index : indices)
	{
		ocean_assert(sumDirection.isNull() || (sumDirection.normalizedOrZero() * lShapes[index].direction()) >= Numeric::cos(Numeric::deg2rad(45)));

		sumPosition += lShapes[index].position();
		sumDirection += lShapes[index].direction();
		sumScore += lShapes[index].score();
	}

	if (!sumDirection.normalize())
	{
		ocean_assert(false && "This should never happen for similar shapes!");
	}

	constexpr unsigned int invalidLineIndex = (unsigned int)(-1);

	const Scalar rotation45 = Scalar(0.70710678118654752440084436210485); // cos(45), sin(45)

	// |  cos(45) sin(45) |
	// | -sin(45) cos(45) |

	const Vector2 leftEdge((sumDirection.x() + sumDirection.y()) * rotation45, (-sumDirection.x() + sumDirection.y()) * rotation45);
	const Vector2 rightEdge((sumDirection.x() - sumDirection.y()) * rotation45, (sumDirection.x() + sumDirection.y()) * rotation45);

	const Scalar invSize = Scalar(1) / Scalar(indices.size());

	return LShape(invalidLineIndex, invalidLineIndex, sumPosition * invSize, sumDirection, leftEdge, rightEdge, sumScore * invSize);
}

ShapeMatcher::TShape ShapeMatcher::mergeShapes(const TShape* tShapes, const Indices32& indices)
{
	ocean_assert(tShapes != nullptr);
	ocean_assert(!indices.empty());

	Vector2 sumPosition(0, 0);
	Vector2 sumDirection(0, 0);

	for (const Index32& index : indices)
	{
		ocean_assert(sumDirection.isNull() || sumDirection.normalizedOrZero() * tShapes[index].direction() >= Numeric::cos(Numeric::deg2rad(45)));

		sumPosition += tShapes[index].position();
		sumDirection += tShapes[index].direction();
	}

	if (!sumDirection.normalize())
	{
		ocean_assert(false && "This should never happen for similar shapes!");
	}

	constexpr unsigned int invalidLineIndex = (unsigned int)(-1);

	return TShape(invalidLineIndex, invalidLineIndex, sumPosition / Scalar(indices.size()), sumDirection);
}

ShapeMatcher::XShape ShapeMatcher::mergeShapes(const XShape* xShapes, const Indices32& indices)
{
	ocean_assert(xShapes != nullptr);
	ocean_assert(!indices.empty());

	const Scalar angleCos35 = Scalar(0.819152044288992); // = Numeric::cos(Numeric::deg2rad(35))

	const Vector2& direction0 = xShapes[indices.front()].direction0();
	const Vector2& direction1 = xShapes[indices.front()].direction1();

	Vector2 sumPosition = xShapes[indices.front()].position();
	Vector2 sumDirection0 = direction0;
	Vector2 sumDirection1 = direction1;

	for (size_t n = 1; n < indices.size(); ++n)
	{
		const Index32& index = indices[n];

		ocean_assert(Numeric::abs(direction0 * xShapes[index].direction0()) >= angleCos35 || Numeric::abs(direction0 * xShapes[index].direction1()) >= angleCos35);
		ocean_assert(Numeric::abs(direction1 * xShapes[index].direction0()) >= angleCos35 || Numeric::abs(direction1 * xShapes[index].direction1()) >= angleCos35);

		sumPosition += xShapes[index].position();

		const Scalar cosValue00 = direction0 * xShapes[index].direction0();
		const Scalar cosValue01 = direction0 * xShapes[index].direction1();
		const Scalar cosValue10 = direction1 * xShapes[index].direction0();
		const Scalar cosValue11 = direction1 * xShapes[index].direction1();

		if (Numeric::abs(cosValue00) >= angleCos35)
		{
			ocean_assert(Numeric::abs(cosValue01) < angleCos35);
			ocean_assert(Numeric::abs(cosValue10) < angleCos35);

			if (cosValue00 >= angleCos35)
			{
				sumDirection0 += xShapes[index].direction0();
			}
			else
			{
				ocean_assert(cosValue00 <= -angleCos35);
				sumDirection0 -= xShapes[index].direction0();
			}

			ocean_assert(Numeric::abs(cosValue11) >= angleCos35);

			if (cosValue11 >= angleCos35)
			{
				sumDirection1 += xShapes[index].direction1();
			}
			else
			{
				ocean_assert(cosValue11 <= -angleCos35);
				sumDirection1 -= xShapes[index].direction1();
			}
		}
		else
		{
			ocean_assert(Numeric::abs(cosValue01) >= angleCos35);
			ocean_assert(Numeric::abs(cosValue11) < angleCos35);

			if (cosValue01 >= angleCos35)
			{
				sumDirection0 += xShapes[index].direction1();
			}
			else
			{
				ocean_assert(cosValue01 <= -angleCos35);
				sumDirection0 -= xShapes[index].direction1();
			}

			ocean_assert(Numeric::abs(cosValue10) >= angleCos35);

			if (cosValue10 >= angleCos35)
			{
				sumDirection1 += xShapes[index].direction0();
			}
			else
			{
				ocean_assert(cosValue10 <= -angleCos35);
				sumDirection1 -= xShapes[index].direction0();
			}
		}
	}

	if (!sumDirection0.normalize() || !sumDirection1.normalize())
	{
		ocean_assert(false && "This should never happen for similar shapes!");
	}

	constexpr unsigned int invalidLineIndex = (unsigned int)(-1);

	return XShape(invalidLineIndex, invalidLineIndex, sumPosition / Scalar(indices.size()), sumDirection0, sumDirection1);
}

ShapeMatcher::TShape ShapeMatcher::mergeTAndLShapes(const TShape* tShapes, const Indices32& tIndices, const LShape* lShapes, const Indices32& lIndices)
{
	ocean_assert(tShapes != nullptr && lShapes != nullptr);
	ocean_assert(!tIndices.empty() && !lIndices.empty());

	Vector2 sumPosition(0, 0);
	Vector2 sumDirection(0, 0);

	for (const Index32& tIndex : tIndices)
	{
		ocean_assert(sumDirection.isNull() || sumDirection.normalizedOrZero() * tShapes[tIndex].direction() >= Numeric::cos(Numeric::deg2rad(45)));
		ocean_assert(tShapes[tIndex].position().sqrDistance(tShapes[tIndices.front()].position()) < Numeric::sqr(20));

		sumPosition += tShapes[tIndex].position();
		sumDirection += tShapes[tIndex].direction();
	}

	ocean_assert(!sumDirection.isNull());
	const Vector2 mergedTDirection = sumDirection.normalized();

	for (const Index32& lIndex : lIndices)
	{
		ocean_assert(mergedTDirection * lShapes[lIndex].edgeLeft() >= Numeric::cos(Numeric::deg2rad(45)) || mergedTDirection * lShapes[lIndex].edgeRight() >= Numeric::cos(Numeric::deg2rad(45)));
		ocean_assert(lShapes[lIndex].position().sqrDistance(tShapes[tIndices.front()].position()) < Numeric::sqr(20));

		sumPosition += lShapes[lIndex].position();

		if (mergedTDirection * lShapes[lIndex].edgeLeft() > mergedTDirection * lShapes[lIndex].edgeRight())
		{
			sumDirection += lShapes[lIndex].edgeLeft();
		}
		else
		{
			sumDirection += lShapes[lIndex].edgeRight();
		}
	}

	if (!sumDirection.normalize())
	{
		ocean_assert(false && "This should never happen for similar shapes!");
	}

	constexpr unsigned int invalidLineIndex = (unsigned int)(-1);

	return TShape(invalidLineIndex, invalidLineIndex, sumPosition / Scalar(tIndices.size() + lIndices.size()), sumDirection);
}

ShapeMatcher::XShape ShapeMatcher::mergeXAndTShapes(const XShape* xShapes, const Indices32& xIndices, const TShape* tShapes, const Indices32& tIndices)
{
	ocean_assert(xShapes != nullptr && tShapes != nullptr);
	ocean_assert(!xIndices.empty() && !tIndices.empty());

	const Scalar angleCos35 = Scalar(0.819152044288992); // = Numeric::cos(Numeric::deg2rad(35))

	const Vector2& direction0 = xShapes[xIndices.front()].direction0();
	const Vector2& direction1 = xShapes[xIndices.front()].direction1();

	Vector2 sumPosition = xShapes[xIndices.front()].position();
	Vector2 sumDirection0 = direction0;
	Vector2 sumDirection1 = direction1;

	for (size_t n = 1; n < xIndices.size(); ++n)
	{
		const Index32& xIndex = xIndices[n];

		ocean_assert(Numeric::abs(direction0 * xShapes[xIndex].direction0()) >= angleCos35 || Numeric::abs(direction0 * xShapes[xIndex].direction1()) >= angleCos35);
		ocean_assert(Numeric::abs(direction1 * xShapes[xIndex].direction0()) >= angleCos35 || Numeric::abs(direction1 * xShapes[xIndex].direction1()) >= angleCos35);

		sumPosition += xShapes[xIndex].position();

		const Scalar cosValue00 = direction0 * xShapes[xIndex].direction0();
		const Scalar cosValue01 = direction0 * xShapes[xIndex].direction1();
		const Scalar cosValue10 = direction1 * xShapes[xIndex].direction0();
		const Scalar cosValue11 = direction1 * xShapes[xIndex].direction1();

		if (Numeric::abs(cosValue00) >= angleCos35)
		{
			ocean_assert(Numeric::abs(cosValue01) < angleCos35);
			ocean_assert(Numeric::abs(cosValue10) < angleCos35);

			if (cosValue00 >= angleCos35)
			{
				sumDirection0 += xShapes[xIndex].direction0();
			}
			else
			{
				ocean_assert(cosValue00 <= -angleCos35);
				sumDirection0 -= xShapes[xIndex].direction0();
			}

			ocean_assert(Numeric::abs(cosValue11) >= angleCos35);

			if (cosValue11 >= angleCos35)
			{
				sumDirection1 += xShapes[xIndex].direction1();
			}
			else
			{
				ocean_assert(cosValue11 <= -angleCos35);
				sumDirection1 -= xShapes[xIndex].direction1();
			}
		}
		else
		{
			ocean_assert(Numeric::abs(cosValue01) >= angleCos35);
			ocean_assert(Numeric::abs(cosValue11) < angleCos35);

			if (cosValue01 >= angleCos35)
			{
				sumDirection0 += xShapes[xIndex].direction1();
			}
			else
			{
				ocean_assert(cosValue01 <= -angleCos35);
				sumDirection0 -= xShapes[xIndex].direction1();
			}

			ocean_assert(Numeric::abs(cosValue10) >= angleCos35);

			if (cosValue10 >= angleCos35)
			{
				sumDirection1 += xShapes[xIndex].direction0();
			}
			else
			{
				ocean_assert(cosValue10 <= -angleCos35);
				sumDirection1 -= xShapes[xIndex].direction0();
			}
		}
	}

	for (const Index32& tIndex : tIndices)
	{
		ocean_assert(Numeric::abs(direction0 * tShapes[tIndex].direction()) >= angleCos35 || Numeric::abs(direction1 * tShapes[tIndex].direction()) >= angleCos35);

		sumPosition += tShapes[tIndex].position();

		const Scalar cosValue0 = direction0 * tShapes[tIndex].direction();
		const Scalar cosValue1 = direction1 * tShapes[tIndex].direction();

		if (Numeric::abs(cosValue0) >= angleCos35)
		{
			ocean_assert(Numeric::abs(cosValue1) < angleCos35);

			if (cosValue0 >= angleCos35)
			{
				sumDirection0 += tShapes[tIndex].direction();
			}
			else
			{
				ocean_assert(cosValue0 <= -angleCos35);
				sumDirection0 -= tShapes[tIndex].direction();
			}
		}
		else
		{
			ocean_assert(Numeric::abs(cosValue1) >= angleCos35);

			if (cosValue1 >= angleCos35)
			{
				sumDirection1 += tShapes[tIndex].direction();
			}
			else
			{
				ocean_assert(cosValue1 <= -angleCos35);
				sumDirection1 -= tShapes[tIndex].direction();
			}
		}
	}

	if (!sumDirection0.normalize() || !sumDirection1.normalize())
	{
		ocean_assert(false && "This should never happen for similar shapes!");
	}

	constexpr unsigned int invalidLineIndex = (unsigned int)(-1);

	return XShape(invalidLineIndex, invalidLineIndex, sumPosition / Scalar(xIndices.size() + tIndices.size()), sumDirection0, sumDirection1);
}

void ShapeMatcher::determineAlignedMatching(const std::vector<const TwoLineShape*>& shapes, const Geometry::SpatialDistribution::DistributionArray& shapeDistributionArrayWithCopiedNeighborhood8, const std::vector<const TwoLineShape*>& shapeCandidates, IndexPairs32& matches, const Scalar maximalDistance, const Scalar angleThreshold, const Vector2& shiftedCandidates_O_candidates, const SquareMatrix3& shapes_H_shiftedCandidates, Indices32* unmatchedReferenceShapeIndices, Indices32* unmatchedCandidateShapeIndices, Scalar* averageSqrDistance)
{
	ocean_assert(shapeDistributionArrayWithCopiedNeighborhood8.isValid());
	ocean_assert(shapeDistributionArrayWithCopiedNeighborhood8.hasCopiedNeighborhood8());

	ocean_assert(angleThreshold >= 0 && angleThreshold < Numeric::pi_2());
	ocean_assert(!shapes_H_shiftedCandidates.isSingular());

	ocean_assert(unmatchedCandidateShapeIndices == nullptr || unmatchedCandidateShapeIndices->empty());

	ocean_assert(matches.empty());
	matches.clear();

	if (shapeCandidates.empty() || shapes.empty())
	{
		return;
	}

	const bool useCandidateShapeTransformation = !shapes_H_shiftedCandidates.isIdentity();

	ocean_assert(maximalDistance > 0);
	const ShapeMatcher shapeMatcher(maximalDistance, Numeric::deg2rad(20) /*, candidateShapePositionOffset we check the position of our own and this parameter is not used*/);

	const Scalar maximalSqrDistance = Numeric::sqr(shapeMatcher.maximalDistance());

	constexpr unsigned int invalidMatchIndex = (unsigned int)(-1);

	Indices32 usedReferenceShapesCounter(shapes.size(), 0u);
	Scalars sqrDistances;
	sqrDistances.reserve(shapes.size());

	matches.reserve(shapeCandidates.size());

	for (size_t nCandidate = 0; nCandidate < shapeCandidates.size(); ++nCandidate)
	{
		const CV::Detector::ShapeDetector::TwoLineShape& candidateShape = *shapeCandidates[nCandidate];

		const Vector2 transformedCandidatePosition = useCandidateShapeTransformation ? (shapes_H_shiftedCandidates * (candidateShape.position() + shiftedCandidates_O_candidates)) : (candidateShape.position() + shiftedCandidates_O_candidates);

		int xBin = shapeDistributionArrayWithCopiedNeighborhood8.horizontalBin(transformedCandidatePosition.x());
		int yBin = shapeDistributionArrayWithCopiedNeighborhood8.verticalBin(transformedCandidatePosition.y());

		// we allow that positions fall to the bins outside of the actual array with distance 1

		if (xBin < -1 || yBin < -1 || xBin >= int(shapeDistributionArrayWithCopiedNeighborhood8.horizontalBins()) + 1 || yBin >= int(shapeDistributionArrayWithCopiedNeighborhood8.verticalBins()) + 1)
		{
			if (unmatchedCandidateShapeIndices)
			{
				unmatchedCandidateShapeIndices->push_back(Index32(nCandidate));
			}

			continue;
		}

		xBin = minmax(0, xBin, int(shapeDistributionArrayWithCopiedNeighborhood8.horizontalBins()) - 1);
		yBin = minmax(0, yBin, int(shapeDistributionArrayWithCopiedNeighborhood8.verticalBins()) - 1);

		const Indices32& neighborhoodIndices9 = shapeDistributionArrayWithCopiedNeighborhood8(xBin, yBin);

		unsigned int matchIndex = invalidMatchIndex;
		ShapeMatcher::MatchType matchType = ShapeMatcher::MT_UNKNOWN;
		Scalar bestSqrDistance = Numeric::maxValue();
		Scalar secondBestSqrDistance = Numeric::maxValue();

		for (const Index32& index : neighborhoodIndices9)
		{
			const CV::Detector::ShapeDetector::TwoLineShape& referenceShape = *(shapes[index]);

			const Scalar sqrDistance = referenceShape.position().sqrDistance(transformedCandidatePosition);

			if (sqrDistance <= maximalSqrDistance)
			{
				const ShapeMatcher::MatchType localMatchType = useCandidateShapeTransformation ? shapeMatcher.matchShapes<false /* no position check */, true>(referenceShape, candidateShape, &shapes_H_shiftedCandidates) : shapeMatcher.matchShapes<false /* no position check */, false>(referenceShape, candidateShape, nullptr /*reference_H_shiftedCandidate*/);

				if (localMatchType >= ShapeMatcher::MT_PARTIAL_MATCH)
				{
					if (sqrDistance < bestSqrDistance)
					{
						matchIndex = index;
						matchType = localMatchType;

						secondBestSqrDistance = bestSqrDistance;
						bestSqrDistance = sqrDistance;
					}
					else if (sqrDistance < secondBestSqrDistance)
					{
						secondBestSqrDistance = sqrDistance;
					}
				}
			}
		}

		ocean_assert(matchIndex == invalidMatchIndex || bestSqrDistance < secondBestSqrDistance);

		if (secondBestSqrDistance != Numeric::maxValue())
		{
			// we have actually two possible matching candidates
			// we check whether one match is significantly better (distance-wise) than the other

			const Scalar bestDistance = Numeric::sqrt(bestSqrDistance);
			const Scalar secondBestDistance = Numeric::sqrt(secondBestSqrDistance);

			ocean_assert(bestDistance < secondBestDistance);

			if (secondBestDistance - bestDistance < maximalDistance * Scalar(0.40)) // 40% of the maximal allowed distance
			{
				matchIndex = invalidMatchIndex;
			}
		}

		if (matchIndex != invalidMatchIndex)
		{
			matches.push_back(IndexPair32(matchIndex, Index32(nCandidate)));

			ocean_assert(bestSqrDistance < Numeric::maxValue());
			sqrDistances.push_back(bestSqrDistance);

			ocean_assert_and_suppress_unused(matchType >= ShapeMatcher::MT_PARTIAL_MATCH, matchType);

			usedReferenceShapesCounter[matchIndex]++;
		}
		else
		{
			if (unmatchedCandidateShapeIndices)
			{
				unmatchedCandidateShapeIndices->push_back(Index32(nCandidate));
			}
		}
	}

	// let's check whether we cannot trust a match as the match is not bijective

	ocean_assert(matches.size() == sqrDistances.size());

	for (size_t nMatch = 0; nMatch < matches.size(); /* noop */)
	{
		const Index32& referenceIndex = matches[nMatch].first;
		const Index32& candidateIndex = matches[nMatch].second;

		ocean_assert(usedReferenceShapesCounter[referenceIndex] >= 1u);

		if (usedReferenceShapesCounter[referenceIndex] != 1u)
		{
			if (unmatchedCandidateShapeIndices)
			{
				unmatchedCandidateShapeIndices->push_back(candidateIndex);
			}

			// the match is used several times, we reject the match
			matches[nMatch] = matches.back();
			matches.pop_back();

			sqrDistances[nMatch] = sqrDistances.back();
			sqrDistances.pop_back();
		}
		else
		{
			++nMatch;
		}
	}

	ocean_assert(matches.size() <= shapes.size());
	ocean_assert(matches.size() <= shapeCandidates.size());

	if (unmatchedReferenceShapeIndices)
	{
		ocean_assert(unmatchedReferenceShapeIndices->empty());
		unmatchedReferenceShapeIndices->reserve(shapes.size() - matches.size());

		for (size_t n = 0; n < usedReferenceShapesCounter.size(); ++n)
		{
			if (usedReferenceShapesCounter[n] != 1u)
			{
				unmatchedReferenceShapeIndices->push_back(Index32(n));
			}
		}

		ocean_assert(unmatchedReferenceShapeIndices->size() + matches.size() == shapes.size());
	}

	ocean_assert(matches.size() == sqrDistances.size());

	if (averageSqrDistance)
	{
		Scalar sqrDistanceSum = 0;
		for (const Scalar& sqrDistance : sqrDistances)
		{
			sqrDistanceSum += sqrDistance;
		}

		*averageSqrDistance = sqrDistances.empty() ? Numeric::maxValue() : (sqrDistanceSum / Scalar(sqrDistances.size()));
	}

	ocean_assert(unmatchedCandidateShapeIndices == nullptr || unmatchedCandidateShapeIndices->size() + matches.size() == shapeCandidates.size());
}

IndexGroups32 ShapeMatcher::determineShapesAlongLines(const unsigned int width, const unsigned int height, const std::vector<const TwoLineShape*>& shapes, Lines2* lines, const unsigned int minimalShapes)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(minimalShapes >= 2u);

	// we apply a Hough transformation to determine the lines

	constexpr unsigned int offsetRotationRange = 15u;

	SquareMatrices2 offsetRotations(offsetRotationRange * 2u + 1u);
	for (int angleDeg = -int(offsetRotationRange); angleDeg <= int(offsetRotationRange); ++angleDeg)
	{
		const Rotation rotation(0, 0, 1, Numeric::deg2rad(Scalar(angleDeg)));
		const SquareMatrix3 rotationMatrix(rotation);

		offsetRotations[angleDeg + int(offsetRotationRange)] = SquareMatrix2(rotationMatrix(0, 0), rotationMatrix(1, 0), rotationMatrix(0, 1), rotationMatrix(1, 1));
	}

	const int angleDegStart = 90 - 45;
	const int angleDegStop = 90 + 45;
	const unsigned int angleDegRange = (unsigned int)(angleDegStop - angleDegStart + 1);
	ocean_assert(angleDegRange <= 360u);

	const unsigned int angleBins = angleDegRange;
	const unsigned int distanceBins = std::max(width, height);

	Frame binFrame(FrameType(distanceBins, angleBins, FrameType::genericPixelFormat<uint32_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	binFrame.setValue(0x00u);

	const unsigned int binFrameStrideElements = binFrame.strideElements();
	ocean_assert(binFrameStrideElements == distanceBins);

	uint32_t* binFrameData = binFrame.data<uint32_t>();

	for (const TwoLineShape* shape : shapes)
	{
		const Vector2& position = shape->position();

		Vector2 normal;

		if (shape->type() == TShape::ST_SHAPE_T)
		{
			normal = ((const TShape*)shape)->direction();
		}
		else if (shape->type() == XShape::ST_SHAPE_X)
		{
			const XShape& xShape = *((const XShape*)shape);

			if (Numeric::abs(xShape.direction0() * Vector2(0, 1)) > Numeric::abs(xShape.direction1() * Vector2(0, 1)))
			{
				normal = xShape.direction0();
			}
			else
			{
				normal = xShape.direction1();
			}
		}
		else
		{
			// we do not handle other shapes for now
			continue;
		}

		for (const SquareMatrix2& offsetRotation : offsetRotations)
		{
			const Vector2 offsetNormal(offsetRotation * normal);

			Scalar angle = Numeric::atan2(offsetNormal.y(), offsetNormal.x());
			Scalar distance = offsetNormal * position;

			if (distance < 0)
			{
				distance = -distance;
				angle = angle += Numeric::pi();
			}

			angle = Numeric::rad2deg(Numeric::angleAdjustPositive(angle)) - Scalar(angleDegStart);

			const unsigned int distanceBin = (unsigned int)(int(distance + Scalar(0.5)));
			const unsigned int angleBin = (unsigned int)(int(angle + Scalar(0.5)));

			if (distanceBin < distanceBins && angleBin < angleBins)
			{
				++binFrameData[angleBin * binFrameStrideElements + distanceBin];
			}
		}
	}

	CV::NonMaximumSuppression<uint32_t> nonMaximumSuppression(binFrame.width(), binFrame.height());

	for (unsigned int y = 0u; y < binFrame.height(); ++y)
	{
		const uint32_t* binFrameRow = binFrame.constrow<uint32_t>(y);

		for (unsigned int x = 0u; x < binFrame.width(); ++x)
		{
			if (*binFrameRow >= std::max(4u, minimalShapes / 4u))
			{
				nonMaximumSuppression.addCandidate(x, y, *binFrameRow);
			}

			++binFrameRow;
		}
	}

	constexpr unsigned int suppressionRadius = 6u;

	CV::NonMaximumSuppression<uint32_t>::StrengthPositions<unsigned int, uint32_t> peaks(nonMaximumSuppression.suppressNonMaximum<unsigned int, uint32_t>(0u, binFrame.width(), 0u, binFrame.height(), nullptr, nullptr, false));
	peaks = CV::NonMaximumSuppression<uint32_t>::suppressNonMaximum<unsigned int, uint32_t>(binFrame.width(), binFrame.height(), peaks, suppressionRadius, false);
	std::sort(peaks.begin(), peaks.end(), CV::NonMaximumSuppression<uint32_t>::StrengthPosition<unsigned int, uint32_t>::compareStrength<true>);

	Lines2 internalLines;
	internalLines.reserve(peaks.size());

	for (const CV::NonMaximumSuppression<uint32_t>::StrengthPosition<unsigned int, uint32_t>& peak : peaks)
	{
		const unsigned int distance = peak.x();
		const int angleDeg = angleDegStart + int(peak.y());

		internalLines.emplace_back(Numeric::deg2rad(Scalar(angleDeg)), -Scalar(distance));
	}

	const unsigned int horizontalBins = minmax(1u, width / suppressionRadius, 20u);
	const unsigned int verticalBins = minmax(1u, height / suppressionRadius, 20u);

	Geometry::SpatialDistribution::DistributionArray distributionArray(Scalar(0), Scalar(0), Scalar(width), Scalar(height), horizontalBins, verticalBins);

	for (size_t n = 0; n < shapes.size(); ++n)
	{
		const TwoLineShape* shape = shapes[n];

		const unsigned int horizontalBin = distributionArray.horizontalBin(shape->position().x());
		const unsigned int verticalBin = distributionArray.verticalBin(shape->position().y());

		if (horizontalBin < distributionArray.horizontalBins() && verticalBin < distributionArray.verticalBins())
		{
			distributionArray(horizontalBin, verticalBin).push_back(Index32(n));
		}
	}

	// we check which point belongs to which line, and marked points which are assigned to a line

	constexpr Scalar maximalDistance = 3;
	constexpr Scalar maximalSqrDistance = maximalDistance * maximalDistance;

	std::vector<uint8_t> usedShapes(shapes.size(), 0u);

	std::vector<Indices32> internalIndexGroups;
	internalIndexGroups.reserve(internalLines.size());

	const Scalar minimalAngleParallelCos = Numeric::cos(Numeric::deg2rad(30));

	for (size_t nLine = 0; nLine < internalLines.size(); ++nLine)
	{
		const Line2& line = internalLines[nLine];

		const Vector2 lineNormal = line.normal();

		Indices32 internalIndices;

		Scalar x0, y0, x1, y1;
		if (CV::Bresenham::borderIntersection(line, Scalar(0), Scalar(0), Scalar(width - 1u), Scalar(height - 1u), x0, y0, x1, y1))
		{
			int xBin0 = distributionArray.clampedHorizontalBin(x0);
			int yBin0 = distributionArray.clampedVerticalBin(y0);

			const int xBin1 = distributionArray.clampedHorizontalBin(x1);
			const int yBin1 = distributionArray.clampedVerticalBin(y1);

			if (xBin0 != xBin1 || yBin0 != yBin1)
			{
				CV::Bresenham bresenham(xBin0, yBin0, xBin1, yBin1);

				while (xBin0 != xBin1 || yBin0 != yBin1)
				{
					for (int yBin = std::max(0, yBin0 - 1); yBin < std::min(yBin0 + 2, int(distributionArray.verticalBins())); ++yBin)
					{
						for (int xBin = std::max(0, xBin0 - 1); xBin < std::min(xBin0 + 2, int(distributionArray.horizontalBins())); ++xBin)
						{
							const Indices32& indices = distributionArray(xBin, yBin);

							for (const Index32& index : indices)
							{
								const TwoLineShape& twoLineShape = *shapes[index];

								if (usedShapes[index] == 0u && line.sqrDistance(twoLineShape.position()) <= maximalSqrDistance)
								{
									if (areDirectionsParallel(lineNormal, twoLineShape, minimalAngleParallelCos))
									{
										usedShapes[index] = 1u;
										internalIndices.emplace_back(index);
									}
								}
							}
						}
					}

					bresenham.findNext(xBin0, yBin0);
				}
			}
		}

		if (internalIndices.size() >= minimalShapes)
		{
			internalIndexGroups.emplace_back(std::move(internalIndices));
		}
	}

	std::sort(internalIndexGroups.rbegin(), internalIndexGroups.rend(), compareVectorSize);

	if (lines)
	{
		lines->clear();
		lines->reserve(internalIndexGroups.size());

		// we optimize the line by minimizing the distances to all contributing shapes

		Vectors2 points;
		points.reserve(internalIndexGroups.empty() ? 0 : internalIndexGroups.front().size());

		for (const Indices32& internalIndexGroup : internalIndexGroups)
		{
			points.clear();

			for (size_t n = 0; n < internalIndexGroup.size(); ++n)
			{
				points.push_back(shapes[internalIndexGroup[n]]->position());
			}

			Line2 line;
			const bool result = Line2::fitLineLeastSquare(points.data(), points.size(), line);
			ocean_assert_and_suppress_unused(result, result);

			lines->push_back(line);
		}
	}

	return internalIndexGroups;
}

IndexGroups32 ShapeMatcher::determineParallelLineGroups(const Lines2& lines, const Scalar maximalAngle, const unsigned int minimalParallelLines)
{
	ocean_assert(maximalAngle >= 0 && maximalAngle < Numeric::pi_2());
	ocean_assert(minimalParallelLines >= 1u);

	if (lines.empty())
	{
		return IndexGroups32();
	}

	// the lines are expected to be sorted so that the most important lines come first

	Vectors3 implicitLines;
	implicitLines.reserve(lines.size());

	for (const Line2& line : lines)
	{
		implicitLines.emplace_back(line.decomposeNormalDistance(true));
	}

	const Scalar minimalAngleParallelCos = Numeric::cos(maximalAngle);

	std::vector<uint8_t> usedLines(lines.size(), 0u);

	IndexGroups32 indexGroups;
	indexGroups.reserve(10);

	Indices32 indices;

	// we apply a naive approach with complexity O(n^2), however we do not expect more than ~10 lines so that a more efficient approach does not make sense here

	for (size_t nInterestLine = 0; nInterestLine < implicitLines.size() - size_t(1); ++nInterestLine)
	{
		indices.clear();

		if (usedLines[nInterestLine] != 0u)
		{
			continue;
		}

		const Vector3& interestLineImplicit = implicitLines[nInterestLine];

		// no need to set usedLines[nInterestLine] = 1, as we will never come back to this line

		indices.push_back(Index32(nInterestLine));

		for (size_t nTestLine = nInterestLine + 1; nTestLine < implicitLines.size(); ++nTestLine)
		{
			if (usedLines[nTestLine] != 0u)
			{
				continue;
			}

			const Vector3& testLineImplicit = implicitLines[nTestLine];

			if (interestLineImplicit.xy() * testLineImplicit.xy() >= minimalAngleParallelCos)
			{
				usedLines[nTestLine] = 1u;
				indices.push_back(Index32(nTestLine));
			}
		}

		if (indices.size() >= minimalParallelLines)
		{
			indexGroups.push_back(std::move(indices));
		}
	}

	std::sort(indexGroups.rbegin(), indexGroups.rend(), compareVectorSize);

	return indexGroups;
}

std::vector<const ShapeMatcher::TwoLineShape*> ShapeMatcher::serializeShapes(const XShapes& xShapes, const TShapes& tShapes, const LShapes& lShapes, size_t* firstXShapeIndex, size_t* firstTShapeIndex, size_t* firstLShapeIndex)
{
	std::vector<const TwoLineShape*> serializedShapes;
	serializedShapes.reserve(xShapes.size() + tShapes.size() + lShapes.size());

	for (size_t n = 0; n < xShapes.size(); ++n)
	{
		serializedShapes.push_back(xShapes.data() + n);
	}

	for (size_t n = 0; n < tShapes.size(); ++n)
	{
		serializedShapes.push_back(tShapes.data() + n);
	}

	for (size_t n = 0; n < lShapes.size(); ++n)
	{
		serializedShapes.push_back(lShapes.data() + n);
	}

	ocean_assert(serializedShapes.size() == xShapes.size() + tShapes.size() + lShapes.size());

	if (firstXShapeIndex)
	{
		*firstXShapeIndex = xShapes.empty() ? size_t(-1) : size_t(0);

		ocean_assert(xShapes.empty() || serializedShapes[*firstXShapeIndex] == &xShapes.front());
	}

	if (firstTShapeIndex)
	{
		*firstTShapeIndex = tShapes.empty() ? size_t(-1) : size_t(xShapes.size());

		ocean_assert(tShapes.empty() || serializedShapes[*firstTShapeIndex] == &tShapes.front());
	}

	if (firstLShapeIndex)
	{
		*firstLShapeIndex = lShapes.empty() ? size_t(-1) : size_t(xShapes.size() + tShapes.size());

		ocean_assert(lShapes.empty() || serializedShapes[*firstLShapeIndex] == &lShapes.front());
	}

	return serializedShapes;
}

bool ShapeMatcher::areDirectionsSimilar(const Vector2& direction, const TwoLineShape& twoLineShape, const Scalar minimalAngleParallelCos)
{
	ocean_assert(direction.isUnit());
	ocean_assert(minimalAngleParallelCos >= 0 && minimalAngleParallelCos <= 1);

	switch (twoLineShape.type())
	{
		case TwoLineShape::ST_SHAPE_L:
		{
			const LShape& lShape = static_cast<const LShape&>(twoLineShape);
			return lShape.direction() * direction >= minimalAngleParallelCos;
		}

		case TwoLineShape::ST_SHAPE_T:
		{
			const TShape& tShape = static_cast<const TShape&>(twoLineShape);
			return tShape.direction() * direction >= minimalAngleParallelCos;
		}


		case TwoLineShape::ST_SHAPE_X:
		{
			const XShape& xShape = static_cast<const XShape&>(twoLineShape);
			return xShape.direction0() * direction >= minimalAngleParallelCos || xShape.direction1() * direction >= minimalAngleParallelCos;
		}

		case TwoLineShape::ST_INVALID:
			break;
	}

	ocean_assert(false && "Invalid shape type!");
	return false;
}

bool ShapeMatcher::areDirectionsParallel(const Vector2& direction, const TwoLineShape& twoLineShape, const Scalar minimalAngleParallelCos)
{
	ocean_assert(direction.isUnit());
	ocean_assert(minimalAngleParallelCos >= 0 && minimalAngleParallelCos <= 1);

	switch (twoLineShape.type())
	{
		case TwoLineShape::ST_SHAPE_L:
		{
			const LShape& lShape = static_cast<const LShape&>(twoLineShape);
			return Numeric::abs(lShape.direction() * direction) >= minimalAngleParallelCos;
		}

		case TwoLineShape::ST_SHAPE_T:
		{
			const TShape& tShape = static_cast<const TShape&>(twoLineShape);
			return Numeric::abs(tShape.direction() * direction) >= minimalAngleParallelCos;
		}


		case TwoLineShape::ST_SHAPE_X:
		{
			const XShape& xShape = static_cast<const XShape&>(twoLineShape);
			return Numeric::abs(xShape.direction0() * direction) >= minimalAngleParallelCos || Numeric::abs(xShape.direction1() * direction) >= minimalAngleParallelCos;
		}

		case TwoLineShape::ST_INVALID:
			break;
	}

	ocean_assert(false && "Invalid shape type!");
	return false;
}

}

}

}
