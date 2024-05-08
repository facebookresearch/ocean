/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/HomographyPlaneFinder.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/FrameBlender.h"
#include "ocean/cv/advanced/AdvancedMotion.h"

namespace Ocean
{

namespace Tracking
{

bool HomographyPlaneFinder::addImagePoint(const Vectors2& imagePoints)
{
	ocean_assert(planeImagePointsSets == 0 || planeFinderNormalPairs.size() + 1 == planeImagePointsSets);
	if (planeImagePointsSets != 0 && planeFinderNormalPairs.size() + 1 != planeImagePointsSets)
	{
		return false;
	}

	if (imagePointCorrespondences.size() < 2)
	{
		if (!imagePointCorrespondences.addElements(imagePoints))
		{
			return false;
		}
	}
	else
	{
		if (!imagePointCorrespondences.setLastElements(imagePoints))
		{
			return false;
		}
	}

	planeImagePointsSets++;

	ocean_assert(imagePointCorrespondences.size() <= 2);
	return true;
}

bool HomographyPlaneFinder::addImagePoint(Vectors2&& imagePoints)
{
	ocean_assert(planeImagePointsSets == 0 || planeFinderNormalPairs.size() + 1 == planeImagePointsSets);
	if (planeImagePointsSets != 0 && planeFinderNormalPairs.size() + 1 != planeImagePointsSets)
	{
		return false;
	}

	if (imagePointCorrespondences.size() < 2)
	{
		if (!imagePointCorrespondences.addElements(imagePoints))
		{
			return false;
		}
	}
	else
	{
		if (!imagePointCorrespondences.setLastElements(imagePoints))
		{
			return false;
		}
	}

	planeImagePointsSets++;

	ocean_assert(imagePointCorrespondences.size() <= 2);
	return true;
}

bool HomographyPlaneFinder::addImagePoint(const Vectors2& imagePoints, const Indices32& validIndices)
{
	ocean_assert(planeImagePointsSets == 0 || planeFinderNormalPairs.size() + 1 == planeImagePointsSets);
	if (planeImagePointsSets != 0 && planeFinderNormalPairs.size() + 1 != planeImagePointsSets)
	{
		return false;
	}

	if (imagePointCorrespondences.size() < 2)
	{
		if (!imagePointCorrespondences.addElements(imagePoints, validIndices))
		{
			return false;
		}
	}
	else
	{
		if (!imagePointCorrespondences.setLastElements(imagePoints, validIndices))
		{
			return false;
		}
	}

	planeImagePointsSets++;

	ocean_assert(imagePointCorrespondences.size() <= 2);
	return true;
}

bool HomographyPlaneFinder::addPlaneCandidates(const PinholeCamera& pinholeCamera)
{
	if (imagePointCorrespondences.size() < 2)
	{
		return true;
	}

	ocean_assert(imagePointCorrespondences.size() == 2);

	const Vectors2& imagePoints0 = imagePointCorrespondences.correspondences().front();
	const Vectors2& imagePoints1 = imagePointCorrespondences.correspondences().back();

	// if two sets of point pairs are available, the homography determination can start
	ocean_assert(!imagePoints0.empty() && !imagePoints1.empty());
	ocean_assert(imagePoints0.size() == imagePoints1.size());

	SquareMatrix3 homography, optimizedHomography;
	Indices32 validIndices;

	// first use RANSAC to find the subset of point pairs defining a valid homography
	if (!Geometry::RANSAC::homographyMatrix(imagePoints0.data(), imagePoints1.data(), imagePoints0.size(), randomGenerator, homography, 8u, true, 200u, Scalar(1.5 * 1.5), &validIndices, WorkerPool::get().scopedWorker()()))
		return false;

	const Vectors2 validImagePoints0(Subset::subset(imagePoints0, validIndices));
	const Vectors2 validImagePoints1(Subset::subset(imagePoints1, validIndices));

	HomogenousMatrix4 transformations[2];
	Vector3 normals[2] =
	{
		Vector3(0, 0, 0),
		Vector3(0, 0, 0)
	};

	// Factorize the optimized homography into pose translation, rotation and plane normal
	if (!Geometry::Homography::factorizeHomographyMatrix(homography, pinholeCamera, pinholeCamera, validImagePoints0.data(), validImagePoints1.data(), validImagePoints0.size(), transformations, normals))
	{
		return false;
	}

	// store both possible plane normals so that we can compare the found normals with previous and subsequent frames
	planeFinderNormalPairs.push_back(NormalPair(normals[0], normals[1]));

	ocean_assert(planeFinderNormalPairs.size() + 1 == planeImagePointsSets);

	return true;
}

bool HomographyPlaneFinder::hasAccuratePlane(const Scalar maxAngle)
{
	ocean_assert(maxAngle >= 0 && maxAngle < Numeric::pi_2());

	// check whether in three successive frames three normals are almost identical
	if (planeFinderNormalPairs.size() < 3)
	{
		return false;
	}

	ocean_assert(planeFinderNormalPairs.size() + 1 == planeImagePointsSets);

	const size_t center = planeFinderNormalPairs.size() - 2;
	const Scalar cosValue = maximalCosBetweenNormalPairs(planeFinderNormalPairs[center - 1], planeFinderNormalPairs[center], planeFinderNormalPairs[center + 1]);

	return cosValue >= Numeric::cos(maxAngle);
}

bool HomographyPlaneFinder::determineMostAccuratePlanes(Plane3 planes[2])
{
	ocean_assert(planeFinderNormalPairs.size() + 1 == planeImagePointsSets);

	// take the best two normals that have been found in all frames
	size_t bestNormalPairIndex = 0;
	Scalar bestNormalAngle;
	if (!determineBestNormalPair(planeFinderNormalPairs, bestNormalPairIndex, bestNormalAngle))
	{
		return false;
	}

	// check whether the quality of the plane is high enough
	if (bestNormalAngle > Numeric::deg2rad(2))
		return false; // **TODO** provide feedback what the user should do now

	ocean_assert(bestNormalPairIndex > 0 && bestNormalPairIndex + 1 < planeFinderNormalPairs.size());

	// define the two plane candidates that are both most ideal for all frames
	planes[0] = Plane3(planeFinderNormalPairs[bestNormalPairIndex].first, -1);
	planes[1] = Plane3(planeFinderNormalPairs[bestNormalPairIndex].second, -1);

	return true;
}

Scalar HomographyPlaneFinder::maximalCosBetweenNormalPairs(const NormalPair& minus, const NormalPair& center, const NormalPair& plus)
{
	const Scalar cosValue = max(min(max(center.first * minus.first, center.first * minus.second), max(center.first * plus.first, center.first * plus.second)),
			min(max(center.second * minus.first, center.second * minus.second), max(center.second * plus.first, center.second * plus.second)));

#ifdef OCEAN_DEBUG
	const Scalar angleFirst = max(min(center.first.angle(minus.first), center.first.angle(minus.second)), min(center.first.angle(plus.first), center.first.angle(plus.second)));
	const Scalar angleSecond = max(min(center.second.angle(minus.first), center.second.angle(minus.second)), min(center.second.angle(plus.first), center.second.angle(plus.second)));
	const Scalar angle = min(angleFirst, angleSecond);
	ocean_assert(Numeric::isWeakEqual(Numeric::cos(angle), cosValue));
#endif // OCEAN_DEBUG

	return cosValue;
}

bool HomographyPlaneFinder::determineBestNormalPair(const NormalPairs& normalPairs, size_t& index, Scalar& angle)
{
	if (normalPairs.size() < 3)
	{
		return false;
	}

	Scalar maxCosValue = Numeric::minValue();

	index = 0;
	for (size_t n = 1; n < normalPairs.size() - 1; ++n)
	{
		const NormalPair& pairMinus = normalPairs[n - 1];
		const NormalPair& pair = normalPairs[n];
		const NormalPair& pairPlus = normalPairs[n + 1];

		const Scalar cosValue = maximalCosBetweenNormalPairs(pairMinus, pair, pairPlus);

		if (cosValue > maxCosValue)
		{
			maxCosValue = cosValue;
			index = n;
		}
	}

	angle = Numeric::acos(maxCosValue);
	return true;
}

}

}
