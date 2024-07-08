/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/ContourAnalyzer.h"

#include "ocean/base/HashSet.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

Scalars ContourAnalyzer::curvature(const PixelPositions& contour, const unsigned int offset)
{
	ocean_assert(offset >= 1 && offset < contour.size());

	Scalars cosineValues;
	cosineValues.reserve(contour.size());

	for (unsigned int n = 0; n < contour.size(); ++n)
	{
		const unsigned int nPlus = (unsigned int)modulo(int(n + offset), int(contour.size()));
		const unsigned int nMinus = (unsigned int)modulo(int(n - offset), int(contour.size()));

		const PixelPosition& point = contour[n];
		const PixelPosition& plus = contour[nPlus];
		const PixelPosition& minus = contour[nMinus];

		const int xPlus = int(plus.x()) - int(point.x());
		const int yPlus = int(plus.y()) - int(point.y());

		const int xMinus = int(minus.x()) - int(point.x());
		const int yMinus = int(minus.y()) - int(point.y());

		const int dot = xPlus * xMinus + yPlus * yMinus;

		const unsigned int sqrPlus = point.sqrDistance(plus);
		const unsigned int sqrMinus = point.sqrDistance(minus);

		if (sqrPlus == 0 || sqrMinus == 0)
		{
			cosineValues.push_back(Scalar(1));
			continue;
		}

		ocean_assert(sqrPlus > 0);
		ocean_assert(sqrMinus > 0);

		const Scalar cosine = Scalar(dot) / (Numeric::sqrt(Scalar(sqrPlus)) * Numeric::sqrt(Scalar(sqrMinus)));
		ocean_assert(Numeric::isInsideRange(Scalar(-1), cosine, Scalar(1)));

		cosineValues.push_back(minmax(Scalar(-1), cosine, Scalar(1)));
	}

	return cosineValues;
}

PixelContour ContourAnalyzer::createDenseContour(const PixelPositions& initialPixels)
{
	if (initialPixels.empty())
		return PixelContour();

	// remove identical (and successive) pixels
	PixelPositions uniquePixels;
	uniquePixels.reserve(initialPixels.size());

	ocean_assert(!initialPixels.empty());
	uniquePixels.push_back(initialPixels.front());

	for (PixelPositions::const_iterator i = initialPixels.begin(); i != initialPixels.end(); ++i)
		if (*i != uniquePixels.back())
			uniquePixels.push_back(*i);

	while (!uniquePixels.empty() && uniquePixels.front() == uniquePixels.back())
		uniquePixels.pop_back();

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < uniquePixels.size(); ++n)
		ocean_assert(uniquePixels[n].sqrDistance(uniquePixels[modulo(int(n) + 1, int(uniquePixels.size()))]) > 0u);
#endif

	ocean_assert(uniquePixels.size() >= 1);

	// fill gabs with pixels
	PixelPositions fullUniquePixels;
	fullUniquePixels.reserve(uniquePixels.size() * 10);

	for (unsigned int n = 0u; n < uniquePixels.size(); ++n)
	{
		const unsigned int nPlus = modulo(int(n) + 1, int(uniquePixels.size()));

		ocean_assert(n != nPlus);
		ocean_assert(uniquePixels[n] != uniquePixels[nPlus]);

		// check whether pixels have to be added
		if (!uniquePixels[n].inArea9(uniquePixels[nPlus]))
		{
			ocean_assert(uniquePixels[n].sqrDistance(uniquePixels[nPlus]) > 2u);

			int x = int(uniquePixels[n].x());
			int y = int(uniquePixels[n].y());

			const int xEnd = int(uniquePixels[nPlus].x());
			const int yEnd = int(uniquePixels[nPlus].y());

			CV::Bresenham bresenham(x, y, xEnd, yEnd);

			while (x != xEnd || y != yEnd)
			{
				fullUniquePixels.push_back(PixelPosition(x, y));
				bresenham.findNext(x, y);
			}
		}
		else
			fullUniquePixels.push_back(uniquePixels[n]);
	}

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < fullUniquePixels.size(); ++n)
		ocean_assert(fullUniquePixels[n].isNeighbor8(fullUniquePixels[modulo(int(n) + 1, int(fullUniquePixels.size()))]));
#endif

	// now we remove possible exact loops

	while (true)
	{
		HashSet<PixelPosition> exactPositionsHash(fullUniquePixels.size() * 5, pixelPositionHashValueFunction);
		Indices exactCrossings;

		for (unsigned int n = 0u; n < fullUniquePixels.size(); ++n)
			if (!exactPositionsHash.insert(fullUniquePixels[n]))
				exactCrossings.push_back(n);

		// stop if no further crossings exists
		if (exactCrossings.empty())
			break;

		std::vector<unsigned char> fullUniquePixelsUsage(fullUniquePixels.size(), 1);

		for (Indices::const_iterator i = exactCrossings.begin(); i != exactCrossings.end(); ++i)
		{
			ocean_assert(*i < fullUniquePixels.size());

			const unsigned int index = *i;
			const PixelPosition& position = fullUniquePixels[index];

			unsigned int minusSize = 0u;
			unsigned int plusSize = 0u;

			for (unsigned int n = 1u; n < fullUniquePixels.size(); ++n)
			{
				const unsigned int minusIndex = modulo(int(index - n), int(fullUniquePixels.size()));
				const unsigned int plusIndex = modulo(int(index + n), int(fullUniquePixels.size()));

				if (fullUniquePixels[minusIndex] == position)
				{
					++plusSize;
					break;
				}

				++minusSize;

				if (fullUniquePixels[plusIndex] == position)
					break;

				++plusSize;
			}

			ocean_assert(minusSize != plusSize);

			ocean_assert(minusSize >= 1u);
			ocean_assert(plusSize >= 1u);

			ocean_assert(minusSize + 2u < fullUniquePixels.size());
			ocean_assert(plusSize + 2u < fullUniquePixels.size());

			unsigned int startIndex, size;

			if (minusSize < plusSize)
			{
				startIndex = modulo(int(index) - int(minusSize), int(fullUniquePixels.size()));
				size = minusSize;
			}
			else
			{
				startIndex = modulo(int(index) + 1, int(fullUniquePixels.size()));
				size = plusSize;
			}
			ocean_assert(fullUniquePixels[modulo(int(startIndex) - 1, int(fullUniquePixels.size()))] == fullUniquePixels[modulo(int(startIndex) + int(size), int(fullUniquePixels.size()))]);

			for (unsigned int n = 0u; n <= size; ++n)
			{
				const unsigned int moduloIndex = modulo(int(startIndex) + int(n), int(fullUniquePixels.size()));
				fullUniquePixelsUsage[moduloIndex] = 0;
			}
		}

		PixelPositions exactCrossingFree;
		exactCrossingFree.reserve(fullUniquePixels.size());

		for (unsigned int n = 0u; n < fullUniquePixels.size(); ++n)
			if (fullUniquePixelsUsage[n])
			{
				if (!exactCrossingFree.empty() && !exactCrossingFree.back().isNeighbor8(fullUniquePixels[n]))
					CV::Bresenham::intermediatePixels(exactCrossingFree.back(), fullUniquePixels[n], exactCrossingFree);

				exactCrossingFree.push_back(fullUniquePixels[n]);
			}

		ocean_assert(!exactCrossingFree.empty());

		if (!exactCrossingFree.front().isNeighbor8(exactCrossingFree.back()))
			CV::Bresenham::intermediatePixels(exactCrossingFree.back(), exactCrossingFree.front(), exactCrossingFree);

		fullUniquePixels = exactCrossingFree;

#ifdef OCEAN_DEBUG
		for (unsigned int n = 0u; n < fullUniquePixels.size(); ++n)
			ocean_assert(fullUniquePixels[n].isNeighbor8(fullUniquePixels[modulo(int(n) + 1, int(fullUniquePixels.size()))]));
#endif

	}

	while (true)
	{
		PixelPositions centerPixels;
		centerPixels.reserve(fullUniquePixels.size());

		for (unsigned int n = 1u; n < fullUniquePixels.size(); ++n)
		{
			const PixelPosition& position0 = fullUniquePixels[n - 1u];
			const PixelPosition& position1 = fullUniquePixels[n];

			centerPixels.push_back((position0.twice() + position1.twice()).half());
		}

		if (fullUniquePixels.size() >= 2)
			centerPixels.push_back((fullUniquePixels.back().twice() + fullUniquePixels.front().twice()).half());


		// remove intermediate loops
		HashSet<PixelPosition> centerPositionsHash(centerPixels.size() * 5, pixelPositionHashValueFunction);
		Indices centerCrossings;

		for (unsigned int n = 0u; n < centerPixels.size(); ++n)
			if (!centerPositionsHash.insert(centerPixels[n]))
				centerCrossings.push_back(n);

		// stop if no further crossings exists
		if (centerCrossings.empty())
			break;

		std::vector<unsigned char> fullUniquePixelsUsage(fullUniquePixels.size(), 1);

		for (Indices::const_iterator i = centerCrossings.begin(); i != centerCrossings.end(); ++i)
		{
			ocean_assert(*i < fullUniquePixels.size());

			const unsigned int index = *i;
			const PixelPosition& position = centerPixels[index];

			unsigned int minusSize = 0u;
			unsigned int plusSize = 0u;

			for (unsigned int n = 1u; n < centerPixels.size(); ++n)
			{
				const unsigned int minusIndex = modulo(int(index - n), int(centerPixels.size()));
				const unsigned int plusIndex = modulo(int(index + n), int(centerPixels.size()));

				if (centerPixels[minusIndex] == position)
				{
					++plusSize;
					break;
				}

				++minusSize;

				if (centerPixels[plusIndex] == position)
					break;

				++plusSize;
			}

			ocean_assert(minusSize != plusSize);

			ocean_assert(minusSize >= 1u);
			ocean_assert(plusSize >= 1u);

			ocean_assert(minusSize + 2u < centerPixels.size());
			ocean_assert(plusSize + 2u < centerPixels.size());

			unsigned int startIndex, size;

			if (minusSize < plusSize)
			{
				startIndex = modulo(int(index) - int(minusSize), int(centerPixels.size()));
				size = minusSize;
			}
			else
			{
				startIndex = modulo(int(index) + 1, int(centerPixels.size()));
				size = plusSize;
			}
			ocean_assert(centerPixels[modulo(int(startIndex) - 1, int(centerPixels.size()))] == centerPixels[modulo(int(startIndex) + int(size), int(centerPixels.size()))]);

			for (unsigned int n = 0u; n <= size; ++n)
			{
				const unsigned int moduloIndex = modulo(int(startIndex) + int(n), int(centerPixels.size()));
				fullUniquePixelsUsage[moduloIndex] = 0;
			}
		}

		PixelPositions exactCrossingFree;
		exactCrossingFree.reserve(centerPixels.size());

		for (unsigned int n = 0u; n < fullUniquePixels.size(); ++n)
			if (fullUniquePixelsUsage[n])
			{
				if (!exactCrossingFree.empty() && !exactCrossingFree.back().isNeighbor8(fullUniquePixels[n]))
					CV::Bresenham::intermediatePixels(exactCrossingFree.back(), fullUniquePixels[n], exactCrossingFree);

				exactCrossingFree.push_back(fullUniquePixels[n]);
			}

		ocean_assert(!exactCrossingFree.empty());

		if (!exactCrossingFree.front().isNeighbor8(exactCrossingFree.back()))
			CV::Bresenham::intermediatePixels(exactCrossingFree.back(), exactCrossingFree.front(), exactCrossingFree);

		fullUniquePixels = exactCrossingFree;

#ifdef OCEAN_DEBUG
		for (unsigned int n = 0u; n < fullUniquePixels.size(); ++n)
			ocean_assert(fullUniquePixels[n].isNeighbor8(fullUniquePixels[modulo(int(n) + 1, int(fullUniquePixels.size()))]));
#endif

	}

#ifdef OCEAN_DEBUG
	for (unsigned int n = 0u; n < fullUniquePixels.size(); ++n)
		ocean_assert(fullUniquePixels[n].isNeighbor8(fullUniquePixels[modulo(int(n) + 1, int(fullUniquePixels.size()))]));
#endif

	ocean_assert(PixelContour(fullUniquePixels).isDense());
	return PixelContour(fullUniquePixels);
}

Vectors2 ContourAnalyzer::equalizeContourDensity(const Vectors2& contour)
{
	ocean_assert(!contour.empty());
	if (contour.empty())
		Vectors2();

	// determine the dense of each contour point
	DenseObjects denses;
	denses.reserve(contour.size());

	for (unsigned int n = 0; n < contour.size(); ++n)
	{
		const unsigned int nMinus = modulo(int(n) - 1, int(contour.size()));
		const unsigned int nPlus = modulo(int(n) + 1, int(contour.size()));

		const Scalar denseMinus = contour[n].sqrDistance(contour[nMinus]);
		const Scalar densePlus = contour[n].sqrDistance(contour[nPlus]);

		denses.push_back(DenseObject(max(denseMinus, densePlus), n));
	}

	std::sort(denses.rbegin(), denses.rend());

	// determine the total contour length
	Scalar totalLength = contour.back().distance(contour.front());
	for (unsigned int n = 1; n < contour.size(); ++n)
		totalLength += contour[n - 1].distance(contour[n]);
	totalLength /= Scalar(contour.size());

	Vectors2 equalizedContour = contour;

	// a point is moved only once, thus we need something to flag
	std::vector<unsigned char> moved(equalizedContour.size(), 0);

	for (unsigned int n = 0; n < denses.size(); ++n)
	{
		const DenseObject& object = denses[n];

		const Vector2& point = equalizedContour[object.index()];

		const unsigned int indexMinus = modulo(int(object.index()) - 1, int(equalizedContour.size()));
		const unsigned int indexPlus = modulo(int(object.index()) + 1, int(equalizedContour.size()));

		// if the minus point hasn't been moved yet
		if (moved[indexMinus] == 0)
		{
			Vector2 direction(equalizedContour[indexMinus] - point);

			if (direction.normalize())
			{
				equalizedContour[indexMinus] = point + direction * totalLength;
				moved[indexMinus] = 1;
			}
		}

		// if the plus point hasn't been moved yet
		if (moved[indexPlus] == 0)
		{
			Vector2 direction(equalizedContour[indexPlus] - point);

			if (direction.normalize())
			{
				equalizedContour[indexPlus] = point + direction * totalLength;
				moved[indexPlus] = 1;
			}
		}
	}

	return equalizedContour;
}

}

}

}
