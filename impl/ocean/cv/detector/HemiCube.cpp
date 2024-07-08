/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/HemiCube.h"

#include "ocean/cv/Canvas.h"

#include <algorithm>
#include <functional>

namespace Ocean
{
namespace CV
{
namespace Detector
{
std::size_t HemiCube::MapIndexHash::operator()(const MapIndex& mapIndex) const noexcept
{
	//                                      v--- prime numbers
	const std::size_t hashX = mapIndex[0] * 48187u;
	const std::size_t hashY = mapIndex[1] * 15487469u;
	const std::size_t hashZ = mapIndex[2] * 98474539u;
	const std::size_t hash = ((hashX ^ hashY) ^ hashZ);

	return hash;
}

HemiCube::HemiCube(const unsigned int bins, const unsigned int imageWidth, const unsigned int imageHeight, const Scalar focalLength) :
	imageWidth_(imageWidth),
	imageHeight_(imageHeight),
	principalPoint_(Vector2(Scalar(imageWidth_) * Scalar(0.5), Scalar(imageHeight_) * Scalar(0.5))),
	focalLength_(focalLength), // NOTE: This assumes the principal point coincides with the image center
	numberBins_(bins)
{
	ocean_assert(numberBins_ != 0u);
	ocean_assert(imageWidth_ != 0u && imageHeight_ != 0u);
	ocean_assert(focalLength_ > Scalar(0.0));
}

FiniteLines2 HemiCube::mergeGreedyBruteForce(const FiniteLines2& lines, const Scalar maxLineDistance, const Scalar maxLineGap, Indices32* mapping, const Scalar cosAngle)
{
	ocean_assert(maxLineDistance >= Scalar(0));
	ocean_assert(maxLineGap >= Scalar(0));

	FiniteLines2 mergedLines;
	mergedLines.reserve(lines.size());

	if (mapping)
	{
		mapping->resize(lines.size());
		std::fill(mapping->begin(), mapping->end(), (Index32)-1);
	}

	const Scalar squareMaxLineGap = maxLineGap * maxLineGap;

	for (unsigned int lineIndex = 0u; lineIndex < lines.size(); ++lineIndex)
	{
		const FiniteLine2& line0 = lines[lineIndex];
		ocean_assert(line0.isValid());

		bool foundMatch = false;

		for (unsigned int mergedLineIndex = 0u; mergedLineIndex < mergedLines.size(); ++mergedLineIndex)
		{
			FiniteLine2& line1 = mergedLines[mergedLineIndex];
			ocean_assert(line1.isValid());

			if (!line0.isCollinear(line1, maxLineDistance, cosAngle))
			{
				continue;
			}

			const Scalar distance0 = line0.nearestPoint(line1.point0()).sqrDistance(line1.point0());
			const Scalar distance1 = line0.nearestPoint(line1.point1()).sqrDistance(line1.point1());

			if (std::min(distance0, distance1) > squareMaxLineGap)
			{
				continue;
			}

			const FiniteLine2 mergedLine = fuse(line0, line1);
			ocean_assert(mergedLine.isValid());
			line1 = mergedLine;
			foundMatch = true;

			if (mapping)
			{
				ocean_assert(lineIndex < mapping->size());
				(*mapping)[lineIndex] = mergedLineIndex;
			}

			break;
		}

		// Add the current line as-is
		if (!foundMatch)
		{
			mergedLines.emplace_back(line0);

			if (mapping)
			{
				(*mapping)[lineIndex] = (unsigned int)(mergedLines.size() - 1);
			}
		}
	}

	ocean_assert(mergedLines.size() <= lines.size());
	return mergedLines;
}

void HemiCube::merge(const FiniteLines2& lines, const Scalar maxLineDistance, const Scalar maxLineGap, Indices32* mapping, const Scalar cosAngle)
{
	ocean_assert(isValid());
	ocean_assert(maxLineDistance >= Scalar(0));
	ocean_assert(maxLineGap >= Scalar(0));

	if (mapping)
	{
		mapping->resize(lines.size());
		std::fill(mapping->begin(), mapping->end(), (Index32)-1);
	}

	const Scalar searchRadius = Scalar(1.5); // Covers the 8-neighborhood since > sqrt(2)
	const Scalar maxSquareLineGap = maxLineGap * maxLineGap;

	for (unsigned int lineIndex = 0u; lineIndex < lines.size(); ++lineIndex)
	{
		const FiniteLine2& line = lines[lineIndex];
		ocean_assert(line.isValid());

		Scalar bestMatchValue = Numeric::maxValue();
		Index32 bestMatchLineIndex = (Index32)-1;
		bool foundMatch = false;

		const IndexSet32 similarLineIndices = find(line, searchRadius);

		for (const Index32& similarLineIndex : similarLineIndices)
		{
			ocean_assert(similarLineIndex < (Index32)linesInMap_.size());
			const FiniteLine2& similarLine = linesInMap_[similarLineIndex];

			if (!line.isCollinear(similarLine, maxLineDistance, cosAngle))
			{
				continue;
			}

			const Scalar squareLineGap0 = line.nearestPoint(similarLine.point0()).sqrDistance(similarLine.point0());
			const Scalar squareLineGap1 = line.nearestPoint(similarLine.point1()).sqrDistance(similarLine.point1());

			if (std::min(squareLineGap0, squareLineGap1) >= maxSquareLineGap)
			{
				continue;
			}

			// Check if the current line is the best match so far.
			// A good match is supposed to have a small distance of the endpoints from the line segments (maxDistance*, the smaller, the better) and
			// the segments should be as parallel as possible, i.e. scalar product of the line normals should be as close to 1 as possible (worst case: orthogonal lines, scalar product 0)
			// The match value if ratio of the max. distance and the inverse of the scalar product, best possible match value: 0, worst match: infinity
			const Scalar maxDistanceLineToSimilarLine = std::max(line.normal() * (similarLine.point0() - line.point0()), line.normal() * (similarLine.point1() - line.point0()));
			const Scalar maxDistanceSimilarLineToLine = std::max(similarLine.normal() * (line.point0() - similarLine.point0()), similarLine.normal() * (line.point1() - similarLine.point0()));
			const Scalar normalAlignment = std::max(Numeric::abs(line.normal() * similarLine.normal()), Numeric::weakEps());
			ocean_assert(Numeric::isInsideRange(Numeric::weakEps(), normalAlignment, Scalar(1)));
			const Scalar matchValue = std::max(maxDistanceLineToSimilarLine, maxDistanceSimilarLineToLine) / normalAlignment;

			if (matchValue < bestMatchValue)
			{
				bestMatchValue = matchValue;
				bestMatchLineIndex = similarLineIndex;
				foundMatch = true;
			}
		}

		if (foundMatch == true)
		{
			ocean_assert(bestMatchLineIndex < (Index32)linesInMap_.size());
			const FiniteLine2& mergedLine = fuse(line, linesInMap_[bestMatchLineIndex]);
			updateLine(bestMatchLineIndex, mergedLine);

			if (mapping)
			{
				ocean_assert(lineIndex < mapping->size());
				(*mapping)[lineIndex] = bestMatchLineIndex;
			}
		}
		else
		{
			insert(line);

			if (mapping)
			{
				ocean_assert(lineIndex < mapping->size());
				ocean_assert(linesInMap_.size() != 0);
				(*mapping)[lineIndex] = (Index32)(linesInMap_.size() - 1);
			}
		}
	}
}

FiniteLine2 HemiCube::fuse(const FiniteLine2& line0, const FiniteLine2& line1)
{
	ocean_assert(line0.isValid() && line1.isValid());

	const Scalar sumLineLengths = line0.length() + line1.length();

	ocean_assert(sumLineLengths > Scalar(0));
	const Scalar weightedAverageNormalizer = Scalar(1) / sumLineLengths;
	const Scalar weight0 = line0.length() * weightedAverageNormalizer;
	const Scalar weight1 = line1.length() * weightedAverageNormalizer;

	const Vector2 weightedCentroid = (line0.point0() + line0.point1()) * Scalar(0.5) * weight0 + (line1.point0() + line1.point1()) * Scalar(0.5) * weight1;

	// Make sure directions of the lines point into the same half-space in order to avoid mutual cancellation (extreme case: l0.dir() == -l1.dir())
	const Vector2 line1Direction = line0.direction() * line1.direction() >= Scalar(0) ? line1.direction() : -line1.direction();
	const Vector2 weightedDirection = line0.direction() * weight0 + line1Direction * weight1;

	const Line2 infiniteLine(weightedCentroid, weightedDirection.normalized());

	ocean_assert(infiniteLine.isValid());

	// Project the end-points of the input line segments on the infinite line
	const Vectors2 projectedPoints = {
		infiniteLine.nearestPoint(line0.point0()),
		infiniteLine.nearestPoint(line0.point1()),
		infiniteLine.nearestPoint(line1.point0()),
		infiniteLine.nearestPoint(line1.point1()),
	};

	// Find the combination of projected end-points that maximizes the length of the resulting line segment

	// Let q be any point on the infinite line, and d its direction.
	// Then the two endpoints we choose are the points p that maximize and minimize:
	//   lineCoordinate(p) := d.dot(p - q).
	const auto lineCoordinate = [&infiniteLine](const Vector2& p)
	{
		return infiniteLine.direction() * (p - infiniteLine.point());
	};

	Vector2 point0 = projectedPoints[0]; // will have min coord
	Vector2 point1 = point0; // will have max coord

	Scalar minCoord = lineCoordinate(point0);
	Scalar maxCoord = lineCoordinate(point1);

	for (size_t i = 1; i < projectedPoints.size(); ++i)
	{
		const Vector2& currentPoint = projectedPoints[i];
		const Scalar currentCoord = lineCoordinate(currentPoint);

		if (currentCoord < minCoord)
		{
			minCoord = currentCoord;
			point0 = currentPoint;
		}
		else if (currentCoord > maxCoord)
		{
			maxCoord = currentCoord;
			point1 = currentPoint;
		}
	}

	ocean_assert(lineCoordinate(point0) < lineCoordinate(point1));

	const FiniteLine2 fusedLine(point0, point1);
	ocean_assert(fusedLine.isValid());

	return fusedLine;
}

IndexSet32 HemiCube::find(const FiniteLine2& line, const Scalar radius) const
{
	ocean_assert(isValid());
	ocean_assert(line.isValid());
	ocean_assert(radius >= Scalar(0));

	IndexSet32 similarLineIndices;
	const MapIndex mapIndex = mapIndexFrom(line);

	const unsigned int radiusCeil = (unsigned int)Numeric::ceil(radius);
	const unsigned int yStart = (mapIndex.y() >= radiusCeil ? mapIndex.y() - radiusCeil : 0u);
	const unsigned int yEnd = std::min(mapIndex.y() + radiusCeil, numberBins_);
	const unsigned int xStart = (mapIndex.x() >= radiusCeil ? mapIndex.x() - radiusCeil : 0u);
	const unsigned int xEnd = std::min(mapIndex.x() + radiusCeil, numberBins_);

	for (unsigned int y = yStart; y < yEnd; ++y)
	{
		for (unsigned int x = xStart; x < xEnd; ++x)
		{
			const Scalar distance = (Vector2(Scalar(x), Scalar(y)) - Vector2(Scalar(mapIndex.x()), Scalar(mapIndex.y()))).length();

			if (distance > radius)
			{
				continue;
			}

			const MapIndex currentMapIndex(x, y, mapIndex.z());
			const Map::const_iterator binIter = map_.find(currentMapIndex);

			if (binIter != map_.end())
			{
				similarLineIndices.insert(binIter->second.begin(), binIter->second.end());
			}
		}
	}

	return similarLineIndices;
}

HemiCube::MapIndex HemiCube::mapIndexFrom(const FiniteLine2& line) const
{
	ocean_assert(isValid());
	const Vector3 lineEquation = lineEquationFrom<false>(line);

	// Scale largest dimension of the line equation to one (and the other dimensions accordingly),
	// so that the line equation becomes l = (1, a, b) or l = (a, 1, b) or l = (a, b, 1), where
	// |a| <= 1 and |b| <= 1
	const Vector3 absLineEquation(std::abs(lineEquation[0]), std::abs(lineEquation[1]), std::abs(lineEquation[2]));
	const unsigned int face0 = (absLineEquation[0] >= absLineEquation[1] && absLineEquation[0] >= absLineEquation[2]) ? 0u : (absLineEquation[1] >= absLineEquation[2] ? 1u : 2u);
	const unsigned int face1 = (face0 + 1u) % 3u;
	const unsigned int face2 = (face0 + 2u) % 3u;

	ocean_assert(Numeric::isNotEqualEps(lineEquation[face0]));
	const Vector3 normalizedLineEquation = lineEquation * (Scalar(1.0) / lineEquation[face0]);
	ocean_assert(Numeric::isEqual(normalizedLineEquation[face0], Scalar(1.0)) && std::abs(normalizedLineEquation[face1]) <= Scalar(1.0) && std::abs(normalizedLineEquation[face2]) <= Scalar(1.0));

	// Compute the discrete coordinates of the bin that this line falls into.
	const Scalar scale = Scalar(0.5) * Scalar(numberBins_);
	const Scalar sx = (scale * (Scalar(1.0) + normalizedLineEquation[face1]));
	const Scalar sy = (scale * (Scalar(1.0) + normalizedLineEquation[face2]));
	const unsigned int x = std::min((numberBins_ - 1u), (unsigned int)sx); // std::floor() or cast-only work as well
	const unsigned int y = std::min((numberBins_ - 1u), (unsigned int)sy); // std::floor() or cast-only work as well
	ocean_assert(x < numberBins_ && y < numberBins_);

	return MapIndex(x, y, face0);
}

void HemiCube::updateLine(unsigned int index, const FiniteLine2& updatedLine)
{
	ocean_assert(updatedLine.isValid());

	// Remove the selected line from the map
	ocean_assert(index < linesInMap_.size());
	const MapIndex mapIndex = mapIndexFrom(linesInMap_[index]);

	Map::iterator bin = map_.find(mapIndex);
	ocean_assert(bin != map_.end());

	IndexSet32& indicesInBin = bin->second;

	ocean_assert(indicesInBin.find(index) != indicesInBin.end());
	indicesInBin.erase(index);

	// Update the map index and add back to the map
	const MapIndex updatedMapIndex = mapIndexFrom(updatedLine);
	map_[updatedMapIndex].insert(index);

	linesInMap_[index] = updatedLine;
}

}

}

}
