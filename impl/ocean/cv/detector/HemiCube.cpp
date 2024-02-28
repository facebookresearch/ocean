// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

HemiCube::HemiCube(const unsigned int _bins, const unsigned int _imageWidth, const unsigned int _imageHeight, const Scalar _focalLength) :
	imageWidth(_imageWidth),
	imageHeight(_imageHeight),
	principalPoint(Vector2(Scalar(imageWidth) * Scalar(0.5), Scalar(imageHeight) * Scalar(0.5))),
	focalLength(_focalLength), // NOTE: This assumes the principal point coincides with the image center
	bins(_bins)
{
	ocean_assert(bins != 0u);
	ocean_assert(imageWidth != 0u && imageHeight != 0u);
	ocean_assert(focalLength > Scalar(0.0));
}

bool HemiCube::isValid() const
{
	return bins != 0u && imageWidth != 0u && imageHeight != 0u && Numeric::isNotEqualEps(focalLength) && principalPoint != Vector2(Scalar(0), Scalar(0));
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

			if (line0.isCollinear(line1, maxLineDistance, cosAngle) == false)
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

		// Add current line as is
		if (foundMatch == false)
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
			ocean_assert(similarLineIndex < (Index32)linesInMap.size());
			const FiniteLine2& similarLine = linesInMap[similarLineIndex];

			if (line.isCollinear(similarLine, maxLineDistance, cosAngle) == false)
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
			ocean_assert(bestMatchLineIndex < (Index32)linesInMap.size());
			const FiniteLine2& mergedLine = fuse(line, linesInMap[bestMatchLineIndex]);
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
				ocean_assert(linesInMap.size() != 0);
				(*mapping)[lineIndex] = (Index32)(linesInMap.size() - 1);
			}
		}
	}
}

FiniteLine2 HemiCube::fuse(const FiniteLine2& line0, const FiniteLine2& line1)
{
	ocean_assert(line0.isValid() && line1.isValid());

#if 0
	// TODO nilsplath@ In some cases the result doesn't look right, why? Disabled this section until the issue has been investigated.

	const Scalar totalLineLengths = line0.length() + line1.length();
	ocean_assert(Numeric::isNotEqualEps(totalLineLengths));

	// Line end-points in homogeneous coordinates
	const Vectors3 points = {
		Vector3(line0.point0(), Scalar(1)),
		Vector3(line0.point1(), Scalar(1)),
		Vector3(line1.point0(), Scalar(1)),
		Vector3(line1.point1(), Scalar(1))
	};

	// Scatter matrix
	const Scalar weight0 = line0.length() / totalLineLengths;
	const Scalar weight1 = line1.length() / totalLineLengths;
	ocean_assert(Numeric::isNotEqualEps(weight0) && Numeric::isNotEqualEps(weight1));
	const Scalars weights = { weight0, weight0, weight1, weight1 };

	SquareMatrix3 scatterMatrix(false);

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		scatterMatrix[0] += weights[i] * points[i].x() * points[i].x();
		scatterMatrix[1] += weights[i] * points[i].y() * points[i].x();
		scatterMatrix[2] += weights[i] * points[i].z() * points[i].x();

		scatterMatrix[3] += weights[i] * points[i].x() * points[i].y();
		scatterMatrix[4] += weights[i] * points[i].y() * points[i].y();
		scatterMatrix[5] += weights[i] * points[i].z() * points[i].y();

		scatterMatrix[6] += weights[i] * points[i].x() * points[i].z();
		scatterMatrix[7] += weights[i] * points[i].y() * points[i].z();
		scatterMatrix[8] += weights[i] * points[i].z() * points[i].z();
	}

	ocean_assert(scatterMatrix.isSymmetric(Numeric::weakEps()));

	// Normalize the scatter matrix
	const Scalar absoluteSum = scatterMatrix.absSum();
	ocean_assert(Numeric::isNotEqualEps(absoluteSum));

	scatterMatrix *= Scalar(1) / absoluteSum;

	// Compute the Eigen values and vectors of the scatter matrix
	Scalars eigenValues(3, -1);
	Vectors3 eigenVectors(3, Vector3(0, 0, 0));

	if (scatterMatrix.eigenSystem(eigenValues.data(), eigenVectors.data()) == false)
	{
		return FiniteLine2();
	}

	ocean_assert(eigenValues[0] >= eigenValues[1] && eigenValues[1] >= eigenValues[2] && "Eigen values not sorted in descending order");
	ocean_assert(eigenValues[2] > -Numeric::weakEps() && "The eigenvalues must not be negative (up to numerical error)");

	// If the eigenvalues are (s0, s1, s2) then should have s0 >= s1 >= s2 >= 0.
	// The vp corresponds to the eigenvector of s2.
	// We'll have a good fit if:
	//   s1 is sufficiently positive, and
	//   s1 is sufficiently bigger than s2
	if (!(eigenValues[1] > std::abs(eigenValues[2])))
	{
		// s1 is either negative, or smaller in magnitude than s2, so we have a bad
		// fit
		return FiniteLine2();
	}

	// Determine the infinite line, that minimizes the distances to the end-points of both line segments
	const Scalar normalLength = Vector2(eigenVectors[2].x(), eigenVectors[2].y()).length();
	ocean_assert(Numeric::isNotEqualEps(normalLength));
	const Vector3 lineParameters = eigenVectors[2] * (Scalar(1) / normalLength);
	const Line2 infiniteLine(lineParameters);

#else

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

#endif

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
	const unsigned int yEnd = std::min(mapIndex.y() + radiusCeil, bins);
	const unsigned int xStart = (mapIndex.x() >= radiusCeil ? mapIndex.x() - radiusCeil : 0u);
	const unsigned int xEnd = std::min(mapIndex.x() + radiusCeil, bins);

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
			const Map::const_iterator binIter = map.find(currentMapIndex);

			if (binIter != map.end())
			{
				similarLineIndices.insert(binIter->second.begin(), binIter->second.end());
			}
		}
	}

	return similarLineIndices;
}

size_t HemiCube::size() const
{
	return linesInMap.size();
}

size_t HemiCube::nonEmptyBins() const
{
	return map.size();
}

void HemiCube::clear()
{
	linesInMap.clear();
	map.clear();
}

const FiniteLines2& HemiCube::lines() const
{
	return linesInMap;
}

const FiniteLine2& HemiCube::operator[](const unsigned int index) const
{
	ocean_assert(index < (unsigned int)linesInMap.size());
	return linesInMap[index];
}

FiniteLine2& HemiCube::operator[](const unsigned int index)
{
	ocean_assert(index < (unsigned int)linesInMap.size());
	return linesInMap[index];
}

Ocean::CV::PixelPosition HemiCube::hemiCubeCoordinatesFrom(const MapIndex& mapIndex) const
{
	ocean_assert(isValid());
	ocean_assert(mapIndex[0] < bins && mapIndex[1] < bins && mapIndex[2] <= 2u);
	return Ocean::CV::PixelPosition(mapIndex[2] * bins + mapIndex[0], mapIndex[1]);
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
	const Scalar scale = Scalar(0.5) * Scalar(bins);
	const Scalar sx = (scale * (Scalar(1.0) + normalizedLineEquation[face1]));
	const Scalar sy = (scale * (Scalar(1.0) + normalizedLineEquation[face2]));
	const unsigned int x = std::min((bins - 1u), (unsigned int)sx); // std::floor() or cast-only work as well
	const unsigned int y = std::min((bins - 1u), (unsigned int)sy); // std::floor() or cast-only work as well
	ocean_assert(x < bins && y < bins);

	return MapIndex(x, y, face0);
}

template <bool tScale>
Vector3 HemiCube::lineEquationFrom(const FiniteLine2& line) const
{
	ocean_assert(line.isValid());
	const Vector3 ray0 = rayFrom(line.point0());
	const Vector3 ray1 = rayFrom(line.point1());
	ocean_assert(Numeric::isNotEqualEps(ray0.length()));
	ocean_assert(Numeric::isNotEqualEps(ray1.length()));

	const Vector3 lineEquation = ray0.cross(ray1);
	ocean_assert(Numeric::isNotEqualEps(lineEquation.length()));

	if constexpr (tScale)
	{
		const Scalar maxValue = std::max(std::abs(lineEquation[0]), std::max(std::abs(lineEquation[1]), std::abs(lineEquation[2])));
		ocean_assert(Numeric::isNotEqualEps(maxValue));
		return lineEquation * (Scalar(1.0) / maxValue);
	}

	return lineEquation;
}

template Vector3 HemiCube::lineEquationFrom<true>(const FiniteLine2& line) const;
template Vector3 HemiCube::lineEquationFrom<false>(const FiniteLine2& line) const;

Vector3 HemiCube::rayFrom(const Vector2& point) const
{
	const Scalar inPixels = Scalar(0.5) * Scalar(std::max(imageWidth, imageHeight));
	return Vector3(point - principalPoint, focalLength * inPixels);
}

void HemiCube::updateLine(unsigned int index, const FiniteLine2& updatedLine)
{
	ocean_assert(updatedLine.isValid());

	// Remove the selected line from the map
	ocean_assert(index < linesInMap.size());
	const MapIndex mapIndex = mapIndexFrom(linesInMap[index]);

	Map::iterator bin = map.find(mapIndex);
	ocean_assert(bin != map.end());
	IndexSet32& indicesInBin = bin->second;
	ocean_assert(indicesInBin.find(index) != indicesInBin.end());
	indicesInBin.erase(index);

	// Update the map index and add back to the map
	const MapIndex updatedMapIndex = mapIndexFrom(updatedLine);
	map[updatedMapIndex].insert(index);

	linesInMap[index] = updatedLine;
}

} // namespace Detector
} // namespace CV
} // namespace Ocean
