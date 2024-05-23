/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/StereoscopicGeometry.h"
#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Utilities.h"

namespace Ocean
{

namespace Geometry
{

bool StereoscopicGeometry::cameraPose(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<Vector2>& accessorImagePoints0, const ConstIndexedAccessor<Vector2>& accessorImagePoints1, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera1, Vectors3* objectPoints, Indices32* validIndices, const Scalar maxRotationalSqrError, const Scalar maxArbitrarySqrError, const unsigned int iterations, const Scalar rotationalMotionMinimalValidCorrespondencesPercent)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(accessorImagePoints0.size() >= 5);
	ocean_assert(accessorImagePoints0.size() == accessorImagePoints1.size());
	ocean_assert(rotationalMotionMinimalValidCorrespondencesPercent >= Scalar(0) && rotationalMotionMinimalValidCorrespondencesPercent <= Scalar(1));

	if (!pinholeCamera.isValid() || accessorImagePoints0.size() != accessorImagePoints1.size() || accessorImagePoints0.size() < 5)
	{
		return false;
	}

	// we define that the first camera pose is located at the origin and pointing towards the negative z-space with y-axis upwards
	const HomogenousMatrix4 world_T_camera0(true);
	const HomogenousMatrix4 flippedCamera0_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera0));

	// we guess the initial locations of the object points simply by back-projecting the image points from the first frame
	Vectors3 initialBadObjectPoints(Geometry::Utilities::createObjectPoints(pinholeCamera, world_T_camera0, accessorImagePoints0, pinholeCamera.hasDistortionParameters(), 1));

	const ScopedConstMemoryAccessor<Vector2> imagePoints0(accessorImagePoints0);
	const ScopedConstMemoryAccessor<Vector2> imagePoints1(accessorImagePoints1);

	// we first expect/check whether we have a sole rotational motion between both frames
	// so we try to determine the second pose without optimizing the 3D object points as a rotational camera movement cannot determine the depth information reliably, thus it would be better to avoid a depth determination

	HomogenousMatrix4 world_T_roughCamera1 = world_T_camera0;

	SquareMatrix3 world_R_camera1(false);
	Indices32 usedIndices;
	if (Geometry::RANSAC::orientation(AnyCameraPinhole(pinholeCamera), ConstArrayAccessor<Vector3>(initialBadObjectPoints), accessorImagePoints1, randomGenerator, world_R_camera1, 5u, 100u, Scalar(4) * maxRotationalSqrError, nullptr, &usedIndices) // we take a slightly larger maxSqrError as the RANSAC does not apply any optimization
			&& Scalar(usedIndices.size()) >= Scalar(initialBadObjectPoints.size()) * rotationalMotionMinimalValidCorrespondencesPercent)
	{
		ocean_assert(!world_R_camera1.isSingular());

		// now as we seem to have a pure rotational motion we need to optimized the rotation further

		// we store the rough second pose as this rough pose will be a good start pose if we have also translational camera motion
		world_T_roughCamera1 = HomogenousMatrix4(world_R_camera1);

		Scalar sqrAverageError = Numeric::maxValue();
		SquareMatrix3 world_R_optimizedCamera1_(false);
		if (Geometry::NonLinearOptimizationOrientation::optimizeOrientation(AnyCameraPinhole(pinholeCamera), world_R_camera1, ConstArraySubsetAccessor<Vector3, Index32>(initialBadObjectPoints, usedIndices), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints1.data(), usedIndices), world_R_optimizedCamera1_, 10u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, &sqrAverageError))
		{
			ocean_assert(!world_R_optimizedCamera1_.isSingular());

			world_T_roughCamera1 = HomogenousMatrix4(world_R_optimizedCamera1_);

			if (sqrAverageError <= maxRotationalSqrError)
			{
				// we take only the determined orientation as pose for the second frame
				world_T_camera1 = HomogenousMatrix4(world_R_optimizedCamera1_);

				if (validIndices != nullptr || objectPoints != nullptr)
				{
					usedIndices.clear();

					const HomogenousMatrix4 flippedCamera1_T_world = PinholeCamera::standard2InvertedFlipped(world_T_camera1);
					const bool debugResult = determineValidCorrespondencesIF<ConstTemplateArrayAccessor<Vector3>, ConstIndexedAccessor<Vector2>, ConstIndexedAccessor<Vector2>, true>(pinholeCamera, flippedCamera0_T_world, flippedCamera1_T_world, ConstTemplateArrayAccessor<Vector3>(initialBadObjectPoints), accessorImagePoints0, accessorImagePoints1, usedIndices, pinholeCamera.hasDistortionParameters(), maxRotationalSqrError, true);
					ocean_assert_and_suppress_unused(debugResult, debugResult);

					if (objectPoints != nullptr)
					{
						if (usedIndices.size() != initialBadObjectPoints.size())
						{
							*objectPoints = Subset::subset(initialBadObjectPoints, usedIndices);
						}
						else
						{
							*objectPoints = std::move(initialBadObjectPoints);
						}
					}

					if (validIndices != nullptr)
					{
						*validIndices = std::move(usedIndices);
					}
				}

				// so we can stop here
				return true;
			}
		}
	}

	world_T_camera1 = world_T_roughCamera1;

	// we do not have a pure rotational camera motion, so we need to determine the precise depths of the 3D object points and the corresponding camera pose of the second frame concurrently
	// we do not use a fundamental matrix but a bundle-adjustment instead this is more robust especially if the translational part between the two frames is too small

	// however, due to accuracy/initial model reasons we do not apply one bundle-adjustment for the entire set of correspondences but
	// we apply a RANSAC based search for the best matching camera pose first using a small number of correspondences and check how many of the remaining correspondences are correct for the determined pose

	ocean_assert(imagePoints0.size() >= 5);
	const unsigned int subsetSize = std::max(5u, (unsigned int)(imagePoints0.size() * 10 / 100)); // 10% but at least 5

	Scalar bestError = Numeric::maxValue();

	Vectors3 bestObjectPoints;
	Indices32 bestIndices;

	Indices32 reusableValidIndices;

	Vectors3 reusableOptimizedObjectPoints;

	Vectors3 reusableTriangulatedObjectPoints;
	Indices32 reusableValidTriangulatedObjectPoints;

	Indices32 reusableIndicesSubset;
	reusableIndicesSubset.reserve(subsetSize);

	UnorderedIndexSet32 reusabledIndexSet;
	reusabledIndexSet.reserve(subsetSize);

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		// we randomly select a subset of the entire number of point correspondences and determine the corresponding object points and camera pose

		reusableIndicesSubset.clear();

		if (subsetSize == (unsigned int)(imagePoints0.size()))
		{
			createIndices(subsetSize, 0u, reusableIndicesSubset);
		}
		else
		{
			ocean_assert(subsetSize < imagePoints0.size());

			if (subsetSize * 2u >= (unsigned int)(imagePoints0.size()))
			{
				// the number of image points is quite small, so that it's faster to explicitly select random indices from all possible indices

				createIndices(imagePoints0.size(), 0u, reusableIndicesSubset);

				while (reusableIndicesSubset.size() > subsetSize)
				{
					const Index32 index = RandomI::random(randomGenerator, (unsigned int)(reusableIndicesSubset.size()) - 1u);

					reusableIndicesSubset[index] = reusableIndicesSubset.back();
					reusableIndicesSubset.pop_back();
				}
			}
			else
			{
				reusabledIndexSet.clear();

				while (reusableIndicesSubset.size() < subsetSize)
				{
					const Index32 index = RandomI::random(randomGenerator, (unsigned int)(imagePoints0.size()) - 1u);

					if (reusabledIndexSet.emplace(index).second)
					{
						reusableIndicesSubset.emplace_back(index);
					}
				}
			}

		}

		ocean_assert(reusableIndicesSubset.size() == subsetSize);
		ocean_assert(UnorderedIndexSet32(reusableIndicesSubset.cbegin(), reusableIndicesSubset.cend()).size() == reusableIndicesSubset.size());

		// we start with a SQUARE estimator expecting that we have no outliers in our set of randomly selected correspondences

		reusableOptimizedObjectPoints.resize(subsetSize);
		NonconstArrayAccessor<Vector3> subsetOptimizedObjectPointsAccessor(reusableOptimizedObjectPoints);

		HomogenousMatrix4 world_T_optimizedCamera1(false);
		if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePose(pinholeCamera, world_T_camera0, world_T_roughCamera1, ConstArraySubsetAccessor<Vector3, Index32>(initialBadObjectPoints, reusableIndicesSubset), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints0.data(), reusableIndicesSubset), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints1.data(), reusableIndicesSubset), pinholeCamera.hasDistortionParameters(), &world_T_optimizedCamera1, &subsetOptimizedObjectPointsAccessor, 30u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true))
		{
			// now we determine the 3D object point locations for each point pair by triangulation, accept any 3D object point as long as the point is in front of the camera

			reusableTriangulatedObjectPoints.clear();
			reusableValidTriangulatedObjectPoints.clear();
			Geometry::Utilities::triangulateObjectPoints(pinholeCamera, pinholeCamera, world_T_camera0, world_T_optimizedCamera1, ConstArrayAccessor<Vector2>(imagePoints0.data(), imagePoints0.size()), ConstArrayAccessor<Vector2>(imagePoints1.data(), imagePoints1.size()), reusableTriangulatedObjectPoints, reusableValidTriangulatedObjectPoints, pinholeCamera.hasDistortionParameters(), true, Scalar(-1));

			if (reusableValidTriangulatedObjectPoints.size() == imagePoints0.size())
			{
				// now we optimize the locations of all 3D object points and the camera pose concurrently

				NonconstArrayAccessor<Vector3> optimizedObjectPointsAccessor(reusableOptimizedObjectPoints, reusableTriangulatedObjectPoints.size());
				const HomogenousMatrix4 world_T_intermediateCamera1 = world_T_optimizedCamera1;

				if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePose(pinholeCamera, world_T_camera0, world_T_intermediateCamera1, ConstArrayAccessor<Vector3>(reusableTriangulatedObjectPoints), ConstArrayAccessor<Vector2>(imagePoints0.data(), imagePoints0.size()), ConstArrayAccessor<Vector2>(imagePoints1.data(), imagePoints1.size()), pinholeCamera.hasDistortionParameters(), &world_T_optimizedCamera1, &optimizedObjectPointsAccessor, 30u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5), true))
				{
					// now we check which 3D object point is valid for the given image point correspondences

					ocean_assert(world_T_optimizedCamera1.isValid());
					ocean_assert(reusableOptimizedObjectPoints.size() == imagePoints0.size());

					const HomogenousMatrix4 flippedCamera1_T_world(PinholeCamera::standard2InvertedFlipped(world_T_optimizedCamera1));

					reusableValidIndices.clear();
					Scalar iterationValidError = 0;
					if (determineValidCorrespondencesIF<ConstTemplateArrayAccessor<Vector3>, ConstIndexedAccessor<Vector2>, ConstIndexedAccessor<Vector2>, true>(pinholeCamera, flippedCamera0_T_world, flippedCamera1_T_world, ConstTemplateArrayAccessor<Vector3>(reusableOptimizedObjectPoints), accessorImagePoints0, accessorImagePoints1, reusableValidIndices, pinholeCamera.hasDistortionParameters(), maxArbitrarySqrError, true, &iterationValidError, bestIndices.size()))
					{
						if (reusableValidIndices.size() > bestIndices.size() || (reusableValidIndices.size() == bestIndices.size() && iterationValidError < bestError))
						{
							bestError = iterationValidError;
							world_T_camera1 = world_T_optimizedCamera1;

							if (reusableOptimizedObjectPoints.size() == reusableValidIndices.size())
							{
								std::swap(bestObjectPoints, reusableOptimizedObjectPoints);
							}
							else
							{
								bestObjectPoints = Subset::subset(reusableOptimizedObjectPoints, reusableValidIndices);
							}

							std::swap(bestIndices, reusableValidIndices);

							ocean_assert(bestIndices.size() == bestObjectPoints.size());

							// we check whether we can stop the remaining iterations as we have a pose covering all point pairs with high precision
							if (bestIndices.size() == imagePoints0.size() && bestError < Scalar(0.5 * 0.5) * Scalar(imagePoints0.size() * 2))
							{
								break;
							}
						}
					}
				}
			}
			else if (reusableValidTriangulatedObjectPoints.size() >= 5 && reusableValidTriangulatedObjectPoints.size() >= bestIndices.size())
			{
				// only a subset of the triangulated object points are in front of the camera (we expect object points lying behind the camera to be invalid correspondences)

				NonconstArrayAccessor<Vector3> optimizedObjectPointsAccessor(reusableOptimizedObjectPoints, reusableTriangulatedObjectPoints.size());

				const HomogenousMatrix4 world_T_intermediateCamera1 = world_T_optimizedCamera1;
				if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePose(pinholeCamera, world_T_camera0, world_T_intermediateCamera1, ConstArrayAccessor<Vector3>(reusableTriangulatedObjectPoints), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints0.data(), reusableValidTriangulatedObjectPoints), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints1.data(), reusableValidTriangulatedObjectPoints), pinholeCamera.hasDistortionParameters(), &world_T_optimizedCamera1, &optimizedObjectPointsAccessor, 30u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5), true))
				{
					// now we check which 3D object point is valid for the given image point correspondences

					ocean_assert(world_T_optimizedCamera1.isValid());
					ocean_assert(reusableOptimizedObjectPoints.size() == reusableValidTriangulatedObjectPoints.size());

					const HomogenousMatrix4 optimizedCamera1_T_world(PinholeCamera::standard2InvertedFlipped(world_T_optimizedCamera1));

					reusableValidIndices.clear();
					Scalar iterationValidError = 0;
					if (determineValidCorrespondencesIF<ConstTemplateArrayAccessor<Vector3>, ConstIndexedAccessor<Vector2>, ConstIndexedAccessor<Vector2>, true>(pinholeCamera, flippedCamera0_T_world, optimizedCamera1_T_world, ConstTemplateArrayAccessor<Vector3>(reusableOptimizedObjectPoints), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints0.data(), reusableValidTriangulatedObjectPoints), ConstArraySubsetAccessor<Vector2, Index32>(imagePoints1.data(), reusableValidTriangulatedObjectPoints), reusableValidIndices, pinholeCamera.hasDistortionParameters(), maxArbitrarySqrError, true, &iterationValidError, bestIndices.size()))
					{
						if (reusableValidIndices.size() > bestIndices.size() || (reusableValidIndices.size() == bestIndices.size() && iterationValidError < bestError))
						{
							bestError = iterationValidError;
							world_T_camera1 = world_T_optimizedCamera1;

							if (reusableOptimizedObjectPoints.size() == reusableValidIndices.size())
							{
								std::swap(bestObjectPoints, reusableOptimizedObjectPoints);
							}
							else
							{
								bestObjectPoints = Subset::subset(reusableOptimizedObjectPoints, reusableValidIndices);
							}

							if (reusableValidTriangulatedObjectPoints.size() == reusableValidIndices.size())
							{
								std::swap(bestIndices, reusableValidTriangulatedObjectPoints);
							}
							else
							{
								bestIndices = Subset::subset(reusableValidTriangulatedObjectPoints, reusableValidIndices);
							}

							ocean_assert(bestIndices.size() == bestObjectPoints.size());

							// we check whether we can stop the remaining iterations as we have a pose covering all point pairs with high precision
							if (bestIndices.size() == imagePoints0.size() && bestError < Scalar(0.5 * 0.5) * Scalar(imagePoints0.size() * 2))
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	// if we could not determine any pose with all object points lying in front of the cameras we take any camera pose

	if (bestIndices.empty())
	{
		ocean_assert(world_T_camera1 == world_T_roughCamera1);

		// we start with a SQUARE estimator followed by a HUBER estimator

		NonconstArrayAccessor<Vector3> optimizedObjectPointsAccessor(bestObjectPoints, initialBadObjectPoints.size());

		HomogenousMatrix4 world_T_optimizedCamera1;
		if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePose(pinholeCamera, world_T_camera0, world_T_roughCamera1, ConstArrayAccessor<Vector3>(initialBadObjectPoints), accessorImagePoints0, accessorImagePoints1, pinholeCamera.hasDistortionParameters(), &world_T_optimizedCamera1, &optimizedObjectPointsAccessor, 30u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true))
		{
			return false;
		}

		world_T_roughCamera1 = world_T_optimizedCamera1;
		const Vectors3 initialObjectPoints = bestObjectPoints;
		if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointsAndOnePose(pinholeCamera, world_T_camera0, world_T_roughCamera1, ConstArrayAccessor<Vector3>(initialObjectPoints), accessorImagePoints0, accessorImagePoints1, pinholeCamera.hasDistortionParameters(), &world_T_camera1, &optimizedObjectPointsAccessor, 5u, Geometry::Estimator::ET_HUBER, Scalar(0.001), Scalar(5), true))
		{
			return false;
		}

		const bool debugResult = determineValidCorrespondencesIF<ConstTemplateArrayAccessor<Vector3>, ConstIndexedAccessor<Vector2>, ConstIndexedAccessor<Vector2>, true>(pinholeCamera, flippedCamera0_T_world, PinholeCamera::standard2InvertedFlipped(world_T_camera1), ConstTemplateArrayAccessor<Vector3>(bestObjectPoints), accessorImagePoints0, accessorImagePoints1, bestIndices, pinholeCamera.hasDistortionParameters(), maxArbitrarySqrError, true);
		ocean_assert_and_suppress_unused(debugResult, debugResult);
	}

	ocean_assert(bestObjectPoints.size() == bestIndices.size());

	if (objectPoints != nullptr)
	{
		*objectPoints = std::move(bestObjectPoints);
	}

	if (validIndices != nullptr)
	{
		*validIndices = std::move(bestIndices);
	}

	return true;
}

}

}
