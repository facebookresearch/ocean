/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/EpipolarGeometry.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/MultipleViewGeometry.h"
#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/NonLinearOptimizationPlane.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/NonLinearOptimizationTransformation.h"
#include "ocean/geometry/P3P.h"
#include "ocean/geometry/P4P.h"
#include "ocean/geometry/PnP.h"
#include "ocean/geometry/SpatialDistribution.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Line2.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Geometry
{

unsigned int RANSAC::iterations(const unsigned int model, const Scalar successProbability, const Scalar faultyRate, const unsigned int maximalIterations)
{
	ocean_assert(model > 0u);
	ocean_assert(successProbability > Scalar(0) && successProbability < Scalar(1));
	ocean_assert(faultyRate >= Scalar(0) && faultyRate < Scalar(1));
	ocean_assert(maximalIterations >= 1u);

	if (Numeric::isEqualEps(faultyRate))
	{
		// we do not have any faulty elements in our dataset, therefore one iteration is sufficient
		return 1u;
	}

	/*
	 * successProbablity      =  1 - faultyCandidateProbability ^ iterations
	 *                        =  1 - (1 - inlierCandidateProbability) ^ iterations
	 *                        =  1 - (1 - inlierRate ^ model) ^ iterations
	 *                        =  1 - (1 - (1 - faultyRate) ^ model) ^ iterations
	 *
	 * 1 - successProbablity  =  (1 - (1 - faultyRate) ^ model) ^ iterations
	 *
	 * iterations = log(1 - successProbablity)  / log(1 - (1 - faultyRate) ^ model)
	 *            = log(1 - successProbability) / log(faultyCandidateProbability)
	 *            = log(failureProbability)     / log(faultyCandidateProbability)
	 */

	const Scalar inlierRate = Scalar(1) - faultyRate;
	const Scalar inlierCandidateProbability = Numeric::pow(inlierRate, Scalar(model));
	const Scalar faultyCandidateProbability = Scalar(1) - inlierCandidateProbability;

	ocean_assert(faultyCandidateProbability > Numeric::eps());

	if (Numeric::isEqualEps(faultyCandidateProbability))
	{
		// we mainly have no outliers
		return 1u;
	}

	const Scalar failureProbability = Scalar(1 - successProbability);

	ocean_assert(failureProbability > Numeric::eps());

	if (Numeric::isEqualEps(failureProbability))
	{
		// we mainly want to guarantee that we find a valid solution
		return maximalIterations;
	}

	const Scalar nominator = Numeric::log(failureProbability);
	const Scalar denominator = Numeric::log(faultyCandidateProbability);

	if (Numeric::isEqualEps(denominator))
	{
		return maximalIterations;
	}

	const Scalar expectedIterations = nominator / denominator;
	ocean_assert(expectedIterations >= 0);

	if (expectedIterations > Scalar(maximalIterations))
	{
		return maximalIterations;
	}

	return std::max(1u, (unsigned int)(Numeric::ceil(expectedIterations)));
}

bool RANSAC::p3p(const AnyCamera& anyCamera, const ConstIndexedAccessor<Vector3>& objectPointAccessor, const ConstIndexedAccessor<Vector2>& imagePointAccessor, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(minimalValidCorrespondences >= 4u);
	ocean_assert(objectPointAccessor.size() >= 4);
	ocean_assert(objectPointAccessor.size() == imagePointAccessor.size());
	ocean_assert(objectPointAccessor.size() >= minimalValidCorrespondences);
	ocean_assert(iterations >= 1u);

	if (objectPointAccessor.size() < 4 || objectPointAccessor.size() != imagePointAccessor.size() || objectPointAccessor.size() < minimalValidCorrespondences)
	{
		return false;
	}

	const ScopedConstMemoryAccessor<Vector3> objectPoints(objectPointAccessor);
	const ScopedConstMemoryAccessor<Vector2> imagePoints(imagePointAccessor);

	const unsigned int correspondences = (unsigned int)(objectPoints.size());

	Indices32 indices;
	indices.reserve(correspondences);

	Indices32 bestIndices;
	bestIndices.reserve(correspondences);

	Vector3 permutationObjectPoints[3];
	Vector3 permutationImageRays[3];

	HomogenousMatrix4 world_T_candidateCameras[4];
	HomogenousMatrix4 world_T_bestCamera(false);

	Scalar bestSqrErrors = Numeric::maxValue();

	// due to numerical stability, we ensure that we always apply at least 4 iterations
	const unsigned int minimalAdaptiveIterations = std::min(4u, iterations);

	unsigned int adpativeIterations = iterations;

	for (unsigned int i = 0u; i < adpativeIterations; ++i)
	{
		unsigned int index0, index1, index2;
		Random::random(randomGenerator, correspondences - 1u, index0, index1, index2);

		ocean_assert(index0 < correspondences);
		ocean_assert(index1 < correspondences);
		ocean_assert(index2 < correspondences);

		ocean_assert(index0 != index1 && index1 != index2);

		permutationObjectPoints[0] = objectPoints[index0];
		permutationObjectPoints[1] = objectPoints[index1];
		permutationObjectPoints[2] = objectPoints[index2];

		permutationImageRays[0] = anyCamera.vector(imagePoints[index0]);
		permutationImageRays[1] = anyCamera.vector(imagePoints[index1]);
		permutationImageRays[2] = anyCamera.vector(imagePoints[index2]);

		const unsigned int numberPoses = P3P::poses(permutationObjectPoints, permutationImageRays, world_T_candidateCameras);
		ocean_assert(numberPoses <= 4u);

		// test which of the (at most four) poses is valid for most remaining point correspondences
		for (unsigned int n = 0u; n < numberPoses; ++n)
		{
			indices.clear();

			const HomogenousMatrix4& world_T_candidateCamera = world_T_candidateCameras[n];

			Scalar sqrErrors = 0;

			const HomogenousMatrix4 flippedCandidateCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_candidateCamera));

			ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[index0]));
			ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[index1]));
			ocean_assert(AnyCamera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[index2]));

			// now we test each 2D/3D point correspondences and check whether the accuracy of the pose is good enough, we can stop if we cannot reach a better configuration than we have already
			for (unsigned int c = 0u; indices.size() + (correspondences - c) >= bestIndices.size() && c < correspondences; ++c)
			{
				// we accept only object points lying in front of the camera
				if (AnyCamera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[c]))
				{
					const Vector2 projectedImagePoint(anyCamera.projectToImageIF(flippedCandidateCamera_T_world, objectPoints[c]));
					const ImagePoint& imagePoint = imagePoints[c];

					const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);

					if (sqrError <= sqrPixelErrorThreshold)
					{
						indices.push_back(c);
						sqrErrors += sqrError;
					}
				}
			}

			if (indices.size() >= minimalValidCorrespondences)
			{
				if (indices.size() > bestIndices.size() || (indices.size() == bestIndices.size() && sqrErrors < bestSqrErrors))
				{
					bestSqrErrors = sqrErrors;

					world_T_bestCamera = world_T_candidateCamera;
					std::swap(bestIndices, indices);

					constexpr Scalar successProbability = Scalar(0.99);
					const Scalar faultyRate =  Scalar(1) - Scalar(bestIndices.size()) / Scalar(correspondences);

					const unsigned int expectedIterationsForFoundCorrespondences = RANSAC::iterations(3u, successProbability, faultyRate);

					adpativeIterations = minmax(minimalAdaptiveIterations, expectedIterationsForFoundCorrespondences, adpativeIterations);
				}
			}
		}
	}

	if (bestIndices.size() < minimalValidCorrespondences)
	{
		return false;
	}

	world_T_camera = world_T_bestCamera;

	if (sqrAccuracy != nullptr)
	{
		*sqrAccuracy = bestSqrErrors /= Scalar(bestIndices.size());
	}

	// non linear least square refinement step
	if (refine)
	{
		const size_t bestIndicesUsedForOptimization = bestIndices.size();

		if (!NonLinearOptimizationPose::optimizePose(anyCamera, world_T_bestCamera, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), world_T_camera, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy))
		{
			return false;
		}

		// check whether we need to determine the indices for the optimized pose followed by another final optimization step
		if (usedIndices != nullptr && bestIndices.size() != correspondences)
		{
			const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

			bestIndices.clear();
			for (unsigned int c = 0; c < correspondences; ++c)
			{
				// we accept only object points lying in front of the camera
				if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoints[c]))
				{
					if (imagePoints[c].sqrDistance(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoints[c])) <= sqrPixelErrorThreshold)
				 	{
						bestIndices.push_back(c);
					}
				}
			}

			if (bestIndices.size() < minimalValidCorrespondences)
			{
				return false;
			}

			if (bestIndices.size() != bestIndicesUsedForOptimization)
			{
				ocean_assert(bestIndices.size() <= correspondences);

				world_T_bestCamera = world_T_camera;

				if (!NonLinearOptimizationPose::optimizePose(anyCamera, world_T_bestCamera, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), world_T_camera, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy))
				{
					return false;
				}
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(bestIndices);
	}

	return true;
}

bool RANSAC::objectPoint(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, ObjectPoint& objectPoint, const unsigned int iterations, const Scalar maximalSqrError, const unsigned int minValidCorrespondences, const bool onlyFrontObjectPoint, const Estimator::EstimatorType refinementEstimator, Scalar* finalRobustError, Indices32* usedIndices)
{
	ocean_assert(cameras.size() == world_T_cameras.size() && world_T_cameras.size() == imagePoints.size() && world_T_cameras.size() >= 2 && maximalSqrError >= 0);
	ocean_assert(iterations >= 1u);

	if (world_T_cameras.size() <= 1)
	{
		return false;
	}

	Lines3 rays(world_T_cameras.size());
	HomogenousMatrices4 flippedCamera_T_world(world_T_cameras.size());

	for (size_t n = 0; n < world_T_cameras.size(); ++n)
	{
		rays[n] = cameras[n]->ray(imagePoints[n], world_T_cameras[n]);
		flippedCamera_T_world[n] = AnyCamera::standard2InvertedFlipped(world_T_cameras[n]);
	}

	Scalar bestSqrError = Numeric::maxValue();
	size_t bestNumber = min(size_t(minValidCorrespondences), world_T_cameras.size());

	Indices32 bestIndices;

	Indices32 indices;
	indices.reserve(flippedCamera_T_world.size());

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		unsigned int index0;
		unsigned int index1;
		RandomI::random(randomGenerator, (unsigned int)(flippedCamera_T_world.size()) - 1u, index0, index1);

		ObjectPoint candidate;
		if (rays[index0].nearestPoint(rays[index1], candidate))
		{
			Scalar sqrError = 0;
			indices.clear();

			for (size_t n = 0; n < flippedCamera_T_world.size(); ++n)
			{
				if (!onlyFrontObjectPoint || AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world[n], candidate))
				{
					const Scalar localSqrError = imagePoints[n].sqrDistance(cameras[n]->projectToImageIF(flippedCamera_T_world[n], candidate));

					if (localSqrError <= maximalSqrError)
					{
						sqrError += localSqrError;
						indices.emplace_back(Index32(n));
					}
				}
			}

			if (indices.size() > bestNumber || (indices.size() == bestNumber && sqrError < bestSqrError))
			{
				objectPoint = candidate;
				bestNumber = indices.size();
				bestSqrError = sqrError;
				bestIndices = std::move(indices);
			}
		}
	}

	if (bestSqrError == Numeric::maxValue())
	{
		return false;
	}

	if (finalRobustError)
	{
		*finalRobustError = bestSqrError;
	}

	if (refinementEstimator != Estimator::ET_INVALID)
	{
		ObjectPoint optimizedObjectPoint;

		if (bestIndices.size() == flippedCamera_T_world.size())
		{
			if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(cameras, ConstArrayAccessor<HomogenousMatrix4>(flippedCamera_T_world), objectPoint, imagePoints, optimizedObjectPoint, 10u, refinementEstimator, Scalar(0.001), Scalar(5), onlyFrontObjectPoint, nullptr, finalRobustError))
			{
				objectPoint = optimizedObjectPoint;
			}
		}
		else
		{
			const ScopedConstMemoryAccessor<const AnyCamera*> scopedMemoryCameras(cameras);

			if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(ConstArraySubsetAccessor<const AnyCamera*, unsigned int>(scopedMemoryCameras.data(), bestIndices.data(), bestIndices.size()), ConstArraySubsetAccessor<HomogenousMatrix4, unsigned int>(flippedCamera_T_world, bestIndices), objectPoint, ConstIndexedAccessorSubsetAccessor<ImagePoint, unsigned int>(imagePoints, bestIndices), optimizedObjectPoint, 10u, refinementEstimator, Scalar(0.001), Scalar(5), onlyFrontObjectPoint, nullptr, finalRobustError))
			{
				objectPoint = optimizedObjectPoint;
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(bestIndices);
	}

	return true;
}

bool RANSAC::objectPoint(const AnyCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& world_R_cameras, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, ObjectPoint& objectPoint, const Scalar objectPointDistance, const unsigned int iterations, const Scalar maximalError, const unsigned int minValidCorrespondences, const bool onlyFrontObjectPoint, const Estimator::EstimatorType refinementEstimator, Scalar* finalError, Indices32* usedIndices)
{
	ocean_assert(camera.isValid());
	ocean_assert(world_R_cameras.size() == imagePoints.size() && world_R_cameras.size() >= 2 && maximalError > 0);
	ocean_assert(objectPointDistance > Numeric::eps());
	ocean_assert(iterations >= 1u);

	if (world_R_cameras.size() <= 1)
	{
		return false;
	}

	SquareMatrices3 flippedCameras_R_world;
	flippedCameras_R_world.reserve(world_R_cameras.size());

	for (size_t n = 0; n < world_R_cameras.size(); ++n)
	{
		flippedCameras_R_world.emplace_back(PinholeCamera::standard2InvertedFlipped(world_R_cameras[n]));
	}

	Scalar bestError = Numeric::maxValue();
	size_t bestNumber = std::min(size_t(minValidCorrespondences), world_R_cameras.size());

	Indices32 bestIndices;

	Indices32 indices;
	indices.reserve(flippedCameras_R_world.size());

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(flippedCameras_R_world.size()) - 1u);

		const ObjectPoint candidateObjectPoint(camera.ray(imagePoints[index], HomogenousMatrix4(world_R_cameras[index])).direction() * objectPointDistance);

		Scalar error = 0;
		indices.clear();

		for (size_t n = 0; n < flippedCameras_R_world.size(); ++n)
		{
			if (!onlyFrontObjectPoint || AnyCamera::isObjectPointInFrontIF(flippedCameras_R_world[n], candidateObjectPoint))
			{
				const Scalar localError = imagePoints[n].sqrDistance(camera.projectToImageIF(HomogenousMatrix4(flippedCameras_R_world[n]), candidateObjectPoint));

				if (localError <= maximalError)
				{
					error += localError;
					indices.push_back(Index32(n));
				}
			}
		}

		if (indices.size() > bestNumber || (indices.size() == bestNumber && error < bestError))
		{
			objectPoint = candidateObjectPoint;
			bestNumber = indices.size();
			bestError = error;
			bestIndices = std::move(indices);
		}
	}

	if (bestError == Numeric::maxValue())
	{
		return false;
	}

	if (finalError != nullptr)
	{
		*finalError = bestError;
	}

	if (refinementEstimator != Estimator::ET_INVALID)
	{
		ObjectPoint optimizedObjectPoint;

		if (bestIndices.size() == flippedCameras_R_world.size())
		{
			if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientationsIF(camera, ConstArrayAccessor<SquareMatrix3>(flippedCameras_R_world), imagePoints, objectPoint, objectPointDistance, optimizedObjectPoint, 10u, refinementEstimator, Scalar(0.001), Scalar(5), onlyFrontObjectPoint, nullptr, finalError))
			{
				objectPoint = optimizedObjectPoint;
			}
		}
		else
		{
			if (Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedOrientationsIF(camera, ConstArraySubsetAccessor<SquareMatrix3, unsigned int>(flippedCameras_R_world, bestIndices), ConstIndexedAccessorSubsetAccessor<ImagePoint, unsigned int>(imagePoints, bestIndices), objectPoint, objectPointDistance, optimizedObjectPoint, 10u, refinementEstimator, Scalar(0.001), Scalar(5), onlyFrontObjectPoint, nullptr, finalError))
			{
				objectPoint = optimizedObjectPoint;
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(bestIndices);
	}

	return true;
}

bool RANSAC::plane(const ConstIndexedAccessor<ObjectPoint>& objectPoints, RandomGenerator& randomGenerator, Plane3& plane, const unsigned int iterations, const Scalar medianDistanceFactor, const unsigned int minValidCorrespondences, const Estimator::EstimatorType refinementEstimator, Scalar* finalError, Indices32* usedIndices)
{
	ocean_assert(objectPoints.size() >= 3);
	ocean_assert(iterations >= 1u);

	if (objectPoints.size() < 3)
		return false;

	Scalar maximalDistance = Numeric::maxValue();

	if (medianDistanceFactor >= 0)
	{
		const Vector3 medianObjectPoint = Utilities::medianObjectPoint(objectPoints);

		Scalars sqrDistancesToMedian(objectPoints.size());
		for (size_t n = 0; n < objectPoints.size(); ++n)
			sqrDistancesToMedian[n] = medianObjectPoint.sqrDistance(objectPoints[n]);

		const Scalar medianDistance = Numeric::sqrt(Median::median(sqrDistancesToMedian.data(), sqrDistancesToMedian.size()));

		maximalDistance = medianDistance * medianDistanceFactor;
	}

	Scalar bestError = Numeric::maxValue();
	size_t bestNumber = min(size_t(minValidCorrespondences), objectPoints.size());

	Indices32 bestIndices;

	Indices32 indices;
	indices.reserve(objectPoints.size());

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		unsigned int index0, index1, index2;
		RandomI::random(randomGenerator, (unsigned int)objectPoints.size() - 1u, index0, index1, index2);
		ocean_assert(index0 != index1 && index0 != index2 && index1 != index2);

		// we do not check whether the provided 3D object points are individual or non-collinear as this is done in the constructor of the plane
		const Plane3 candidate(objectPoints[index0], objectPoints[index1], objectPoints[index2]);

		// check whether the plane is valid (e.g., no collinear object points, etc.)
		if (!candidate.isValid())
		{
			continue;
		}

		Scalar error = 0;
		indices.clear();

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Scalar distance = Numeric::abs(candidate.signedDistance(objectPoints[n]));

			if (distance <= maximalDistance)
			{
				error += distance;
				indices.push_back((unsigned int)n);
			}
		}

		if (indices.size() > bestNumber || (indices.size() == bestNumber && error < bestError))
		{
			plane = candidate;
			bestNumber = indices.size();
			bestError = error;
			bestIndices = std::move(indices);
		}
	}

	if (bestError == Numeric::maxValue())
	{
		return false;
	}

	if (finalError != nullptr)
	{
		*finalError = bestError;
	}

	if (refinementEstimator != Estimator::ET_INVALID)
	{
		Plane3 optimizedPlane;

		if (bestIndices.size() == objectPoints.size())
		{
			if (Geometry::NonLinearOptimizationPlane::optimizePlane(plane, objectPoints, optimizedPlane, 20u, refinementEstimator, Scalar(0.001), Scalar(5), nullptr, finalError))
			{
				plane = optimizedPlane;
			}
		}
		else
		{
			if (Geometry::NonLinearOptimizationPlane::optimizePlane(plane, ConstIndexedAccessorSubsetAccessor<ObjectPoint, unsigned int>(objectPoints, bestIndices), optimizedPlane, 20u, refinementEstimator, Scalar(0.001), Scalar(5), nullptr, finalError))
			{
				plane = optimizedPlane;
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(bestIndices);
	}

	return true;
}

bool RANSAC::plane(const Plane3& initialPlane, const ConstIndexedAccessor<ObjectPoint>& objectPoints, RandomGenerator& randomGenerator, Plane3& plane, const unsigned int iterations, const Scalar maximalNormalOrientationOffset, const Scalar medianDistanceFactor, const unsigned int minValidCorrespondences, const Estimator::EstimatorType refinementEstimator, Scalar* finalError, Indices32* usedIndices)
{
	ocean_assert(initialPlane.isValid());
	ocean_assert(objectPoints.size() >= 3);
	ocean_assert(iterations >= 1u);

	if (objectPoints.size() < 3 || !initialPlane.isValid())
	{
		return false;
	}

	ocean_assert(maximalNormalOrientationOffset >= 0 && maximalNormalOrientationOffset <= Numeric::pi_2());
	const Scalar maximalNormalOrientationOffsetCos = Numeric::cos(maximalNormalOrientationOffset);

	Scalar maximalDistance = Numeric::maxValue();

	if (medianDistanceFactor >= 0)
	{
		Scalars distances(objectPoints.size());

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			distances[n] = Numeric::abs(plane.signedDistance(objectPoints[n]));
		}

		const Scalar medianDistance = Median::median(distances.data(), distances.size());
		maximalDistance = medianDistance * medianDistanceFactor;
	}

	ocean_assert(maximalDistance >= 0);

	Scalar bestError = Numeric::maxValue();
	size_t bestNumber = min(size_t(minValidCorrespondences), objectPoints.size());

	Indices32 bestIndices;

	Indices32 indices;
	indices.reserve(objectPoints.size());

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		unsigned int index0, index1, index2;
		RandomI::random(randomGenerator, (unsigned int)objectPoints.size() - 1u, index0, index1, index2);
		ocean_assert(index0 != index1 && index0 != index2 && index1 != index2);

		// we do not check whether the provided 3D object points are individual or non-collinear as this is done in the constructor of the plane
		Plane3 candidatePlane(objectPoints[index0], objectPoints[index1], objectPoints[index2]);

		// check whether the plane is valid (e.g., no collinear object points, etc.)
		if (!candidatePlane.isValid())
		{
			continue;
		}

		// we ensure that the normal of the candidate plane and the normal of the initial plane look in the same direction
		if (initialPlane.normal() * candidatePlane.normal() < 0)
		{
			candidatePlane = -candidatePlane;
		}

		// now we check whether the angle between the now normal and the initial normal is large so that we can directly reject the candidate
		const Scalar cosAngle = initialPlane.normal() * candidatePlane.normal();

		if (cosAngle < maximalNormalOrientationOffsetCos)
		{
			continue;
		}

		Scalar error = 0;
		indices.clear();

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Scalar distance = Numeric::abs(candidatePlane.signedDistance(objectPoints[n]));

			if (distance <= maximalDistance)
			{
				error += distance;
				indices.push_back((unsigned int)n);
			}
		}

		if (indices.size() > bestNumber || (indices.size() == bestNumber && error < bestError))
		{
			plane = candidatePlane;
			bestNumber = indices.size();
			bestError = error;
			bestIndices = std::move(indices);
		}
	}

	if (bestError == Numeric::maxValue())
	{
		return false;
	}

	if (finalError != nullptr)
	{
		*finalError = bestError;
	}

	// we check whether we need to refine the plane further by reducing the overall error to all valid object points
	if (refinementEstimator != Estimator::ET_INVALID)
	{
		Plane3 optimizedPlane;

		if (bestIndices.size() == objectPoints.size())
		{
			if (Geometry::NonLinearOptimizationPlane::optimizePlane(plane, objectPoints, optimizedPlane, 20u, refinementEstimator, Scalar(0.001), Scalar(5), nullptr, finalError))
			{
				plane = optimizedPlane;
			}
		}
		else
		{
			if (Geometry::NonLinearOptimizationPlane::optimizePlane(plane, ConstIndexedAccessorSubsetAccessor<ObjectPoint, unsigned int>(objectPoints, bestIndices), optimizedPlane, 20u, refinementEstimator, Scalar(0.001), Scalar(5), nullptr, finalError))
			{
				plane = optimizedPlane;
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(bestIndices);
	}

	return true;
}

bool RANSAC::line(const ConstIndexedAccessor<Vector2>& positions, RandomGenerator& randomGenerator, Line2& line, const bool refine, const unsigned int iterations, const Scalar maxSqrError, Scalar* finalError, Indices32* usedIndices)
{
	ocean_assert(positions.size() >= 2);
	ocean_assert(iterations > 0u && maxSqrError >= 0);
	ocean_assert(usedIndices == nullptr || usedIndices->empty());

	if (positions.size() < 2)
	{
		return false;
	}

	Indices32 bestIndices;
	Indices32 localIndices;

	Scalar bestError = Numeric::maxValue();
	Line2 bestLine(Numeric::maxValue(), Numeric::minValue());

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		unsigned int index0, index1;
		RandomI::random(randomGenerator, (unsigned int)positions.size() - 1u, index0, index1);

		const Vector2& position0 = positions[index0];
		const Vector2& position1 = positions[index1];

		Vector2 direction = position1 - position0;

		if (!direction.normalize())
		{
			ocean_assert(false && "Two identical points!");
			continue;
		}

		const Line2 lineCandidate(position0, direction);

		localIndices.clear();
		Scalar localError = Scalar(0);

		for (size_t n = 0; n < positions.size(); ++n)
		{
			const Scalar sqrError = lineCandidate.sqrDistance(positions[n]);

			if (sqrError <= maxSqrError)
			{
				localIndices.push_back(Index32(n));
				localError += sqrError;
			}
		}

		if (localIndices.size() > bestIndices.size() || (localIndices.size() == bestIndices.size() && localError < bestError))
		{
			std::swap(bestIndices, localIndices);
			bestError = localError;
			bestLine = lineCandidate;
		}
	}

	ocean_assert(!bestIndices.empty());
	if (bestIndices.empty())
	{
		return false;
	}

	line = bestLine;

	if (finalError != nullptr)
	{
		*finalError = bestError / Scalar(bestIndices.size());
	}

	ocean_assert(line.isValid());

	if (refine)
	{
		Vectors2 validPositions(bestIndices.size());

		for (size_t n = 0; n < bestIndices.size(); ++n)
		{
			validPositions[n] = positions[bestIndices[n]];
		}

		if (!Line2::fitLineLeastSquare(validPositions.data(), validPositions.size(), line))
		{
			return false;
		}
	}

	if (usedIndices != nullptr)
	{
		// we determine the indices of the positions fitting to the (possibly refined) line

		usedIndices->clear();
		usedIndices->reserve(bestIndices.size());

		Scalar error = Scalar(0);

		for (size_t n = 0; n < positions.size(); ++n)
		{
			const Scalar sqrError = line.sqrDistance(positions[n]);

			if (sqrError <= maxSqrError)
			{
				error += sqrError;
				usedIndices->push_back(Index32(n));
			}
		}

		ocean_assert(usedIndices->size() >= 2);
		if (usedIndices->size() < 2)
		{
			return false;
		}

		if (finalError != nullptr)
		{
			*finalError = error / Scalar(usedIndices->size());
		}
	}

	return true;
}

bool RANSAC::direction(const ConstIndexedAccessor<Vector2>& directions, RandomGenerator& randomGenerator, Vector2& direction, const bool acceptOppositeDirections, const bool refine, const unsigned int iterations, const Scalar maxAngle, Scalar* finalError, Indices32* usedIndices)
{
	ocean_assert(!directions.isEmpty());
	ocean_assert(iterations > 0u);
	ocean_assert(maxAngle >= 0 && maxAngle < Numeric::pi());
	ocean_assert(usedIndices == nullptr || usedIndices->empty());

	if (directions.isEmpty())
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	// just ensuring that we have unit vectors
	for (size_t n = 0; n < directions.size(); ++n)
	{
		ocean_assert(directions[n].isUnit());
	}
#endif

	const Scalar maxAngleCos = Numeric::cos(maxAngle);

	size_t bestMatches = 0;
	Scalar bestCosValues = 0;
	Vector2 bestDirection(0, 0);

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		const unsigned int index = RandomI::random(randomGenerator, (unsigned int)directions.size() - 1u);
		const Vector2& candidate = directions[index];

		size_t localMatches = 0;
		Scalar localCosValues = Scalar(0);

		if (acceptOppositeDirections)
		{
			for (size_t n = 0; n < directions.size(); ++n)
			{
				const Scalar absCosValue = Numeric::abs(candidate * directions[n]);

				if (absCosValue >= maxAngleCos)
				{
					localMatches++;
					localCosValues += absCosValue; // we sum up the cosValue - this is fine for small angle threshold
				}
			}
		}
		else
		{
			for (size_t n = 0; n < directions.size(); ++n)
			{
				const Scalar cosValue = candidate * directions[n];

				if (cosValue >= maxAngleCos)
				{
					localMatches++;
					localCosValues += cosValue; // we sum up the cosValue - this is fine for small angle threshold
				}
			}
		}

		if (localMatches > bestMatches || (localMatches == bestMatches && localCosValues > bestCosValues))
		{
			bestMatches = localMatches;
			bestCosValues = localCosValues;
			bestDirection = candidate;
		}
	}

	ocean_assert(bestMatches != 0);
	if (bestMatches == 0)
	{
		return false;
	}

	direction = bestDirection;
	ocean_assert(direction.isUnit());

	if (finalError != nullptr)
	{
		*finalError = Numeric::acos(bestCosValues / Scalar(bestMatches));
	}

	ocean_assert(!bestDirection.isNull());

	if (refine)
	{
		// let's refine the direction by averaging all directions

		Vector2 sumDirection(0, 0);
		size_t number = 0;

		for (size_t n = 0; n < directions.size(); ++n)
		{
			const Vector2& candidate = directions[n];

			const Scalar cosValue = candidate * direction;

			if (cosValue >= maxAngleCos)
			{
				sumDirection += candidate;
				number++;
			}
			else if (acceptOppositeDirections && -cosValue >= maxAngleCos)
			{
				sumDirection -= candidate; // we have an opposite direction, therefore subtracting the direction
				number++;
			}
		}

		ocean_assert_and_suppress_unused(number == bestMatches, number);

		ocean_assert(Numeric::isNotEqualEps(sumDirection.length()));
		direction = sumDirection.normalized();
	}

	if (usedIndices != nullptr)
	{
		// we determine the indices of the directions fitting to the (possibly refined) direction
		// however that may not be the directions that really have been used to find the final direction

		usedIndices->clear();
		usedIndices->reserve(bestMatches);

		Scalar cosValues = Scalar(0);

		for (size_t n = 0; n < directions.size(); ++n)
		{
			const Vector2& candidate = directions[n];

			const Scalar cosValue = candidate * direction;

			if (cosValue >= maxAngleCos)
			{
				cosValues += cosValue;
				usedIndices->push_back((unsigned int)(n));
			}
			else if (acceptOppositeDirections && -cosValue >= maxAngleCos)
			{
				cosValues -= cosValue;
				usedIndices->push_back((unsigned int)(n));
			}
		}

		ocean_assert(!usedIndices->empty());
		if (usedIndices->empty())
		{
			return false;
		}

		if (finalError != nullptr)
		{
			*finalError = Numeric::acos(cosValues / Scalar(usedIndices->size()));
		}
	}

	return true;
}

bool RANSAC::translation(const ConstIndexedAccessor<Vector2>& translations, RandomGenerator& randomGenerator, Vector2& translation, const bool refine, const unsigned int iterations, const Scalar maxSqrError, Scalar* finalError, Indices32* usedIndices)
{
	ocean_assert(!translations.isEmpty());
	ocean_assert(iterations > 0u && maxSqrError >= 0);
	ocean_assert(usedIndices == nullptr || usedIndices->empty());

	if (translations.isEmpty())
	{
		return false;
	}

	size_t bestMatches = 0;
	Scalar bestError = Numeric::maxValue();
	Vector2 bestTranslation(Numeric::maxValue(), Numeric::minValue());

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		const unsigned int index = RandomI::random(randomGenerator, (unsigned int)translations.size() - 1u);
		const Vector2& candidate = translations[index];

		size_t localMatches = 0;
		Scalar localError = Scalar(0);

		for (size_t n = 0; n < translations.size(); ++n)
		{
			const Scalar sqrError = candidate.sqrDistance(translations[n]);

			if (sqrError <= maxSqrError)
			{
				localMatches++;
				localError += sqrError;
			}
		}

		if (localMatches > bestMatches || (localMatches == bestMatches && localError < bestError))
		{
			bestMatches = localMatches;
			bestError = localError;
			bestTranslation = candidate;
		}
	}

	ocean_assert(bestMatches != 0);
	if (bestMatches == 0)
	{
		return false;
	}

	translation = bestTranslation;

	if (finalError != nullptr)
	{
		*finalError = bestError / Scalar(bestMatches);
	}

	ocean_assert(bestTranslation != Vector2(Numeric::maxValue(), Numeric::minValue()));

	if (refine)
	{
		Vector2 sumTranslation(0, 0);
		size_t number = 0;

		for (size_t n = 0; n < translations.size(); ++n)
		{
			const Vector2& candidate = translations[n];

			if (bestTranslation.sqrDistance(candidate) <= maxSqrError)
			{
				sumTranslation += candidate;
				number++;
			}
		}

		ocean_assert_and_suppress_unused(number == bestMatches, number);
		translation = sumTranslation / Scalar(bestMatches);
	}

	if (usedIndices != nullptr)
	{
		// we determine the indices of the translations fitting to the (possibly refined) translation
		// note: that may not be the translations that really have been used to find the final translation

		usedIndices->clear();
		usedIndices->reserve(bestMatches);

		Scalar error = Scalar(0);

		for (size_t n = 0; n < translations.size(); ++n)
		{
			const Scalar sqrError = translation.sqrDistance(translations[n]);

			if (sqrError <= maxSqrError)
			{
				error += sqrError;
				usedIndices->push_back((unsigned int)(n));
			}
		}

		ocean_assert(!usedIndices->empty());
		if (usedIndices->empty())
		{
			return false;
		}

		if (finalError != nullptr)
		{
			*finalError = error / Scalar(usedIndices->size());
		}
	}

	return true;
}

bool RANSAC::orientation(const AnyCamera& camera, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, SquareMatrix3& orientation, const unsigned int minValidCorrespondences, const unsigned int iterations, const Scalar maxSqrError, Scalar* finalError, Indices32* usedIndices)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints.size() && imagePoints.size());
	ocean_assert(objectPoints.size() >= 2);

	unsigned int bestValidCorrespondences = max(2u, minValidCorrespondences);
	Scalar bestError = Numeric::maxValue();

	unsigned int index0, index1;

	Indices32 indices;

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		indices.clear();

		RandomI::random(randomGenerator, (unsigned int)(objectPoints.size()) - 1u, index0, index1);

		// the determination of the orientation is based on two point correspondences
		// we take two rays (targetVectors) between two individual 3D object points and the camera's point of projection
		// further we take two viewing rays (referenceVectors) of the corresponding image points (starting at the camera's point of projection and intersecting the image plane at the positions of the image points) for a default extrinsic camera matrix
		// we determine the orientation of the camera by determination of the rotation between the two pairs of rays

		Vector3 targetVector0(objectPoints[index0]);
		Vector3 targetVector1(objectPoints[index1]);

		if (!targetVector0.normalize() || !targetVector1.normalize())
		{
			ocean_assert(false && "Invalid object point!");
			continue;
		}

		const Vector2 imagePoint0(imagePoints[index0]);
		const Vector2 imagePoint1(imagePoints[index1]);

		const Vector3 imageRay0(camera.vector(imagePoint0, true));
		const Vector3 imageRay1(camera.vector(imagePoint1, true));

		ocean_assert(targetVector0.isUnit() && targetVector1.isUnit());
		ocean_assert(imageRay0.isUnit() && imageRay1.isUnit());

		// we calculate the first rotation which rotates the first reference vector to the first target vector
		const Rotation rotation0(imageRay0, targetVector0);
		ocean_assert((rotation0 * imageRay0).angle(targetVector0) < Numeric::rad2deg(Scalar(0.001)));

		// now we need to find the second rotation around the first rotated reference vector (= target vector) so that also the second reference vector corresponds with the second target vector
		const Vector3 rotatedImageRay1(rotation0 * imageRay1);

		const Vector3 directionA = targetVector1 - targetVector0 * (targetVector0 * targetVector1);
		const Vector3 directionB = rotatedImageRay1 - targetVector0 * (rotatedImageRay1 * targetVector0);

		Rotation rotation(rotation0);

		if (!directionA.isNull() && !directionB.isNull())
		{
			ocean_assert(Numeric::isEqualEps(directionA * targetVector0));
			ocean_assert(Numeric::isEqualEps(directionB * targetVector0));

			Scalar angle = directionA.angle(directionB);

			if (directionA.cross(directionB) * targetVector0 > 0)
			{
				angle = -angle;
			}

			const Rotation rotation1(targetVector0, angle);
			rotation = Rotation(rotation1 * rotation0);
		}

		const HomogenousMatrix4 world_T_camera(rotation);
		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

		Scalar error = 0;
		unsigned int validCorrespondences = 0u;

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector2 projectedObjectPoint(camera.projectToImageIF(flippedCamera_T_world, objectPoints[n]));

			const Scalar sqrDistance = imagePoints[n].sqrDistance(projectedObjectPoint);

			if (sqrDistance <= maxSqrError)
			{
				error += sqrDistance;
				validCorrespondences++;
				indices.push_back((unsigned int)n);
			}
		}

		if (validCorrespondences > bestValidCorrespondences || (validCorrespondences == bestValidCorrespondences && error < bestError))
		{
			bestValidCorrespondences = validCorrespondences;
			bestError = error;

			orientation = SquareMatrix3(rotation);

			ocean_assert(validCorrespondences >= 1u);

			if (finalError != nullptr)
			{
				*finalError = bestError / Scalar(validCorrespondences);
			}

			if (usedIndices != nullptr)
			{
				std::swap(*usedIndices, indices);
			}
		}
	}

	return bestError != Numeric::maxValue();
}

bool RANSAC::fundamentalMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const unsigned int width, const unsigned int height, SquareMatrix3& fundamental, const unsigned int testCandidates, const unsigned int iterations, const Scalar errorThreshold, Indices32* usedIndices)
{
	ocean_assert(errorThreshold > 0);
	ocean_assert(leftImagePoints && rightImagePoints);

	if (testCandidates < 8u || correspondences < testCandidates)
	{
		return false;
	}

	ImagePoints permutationLeftImagePoints(testCandidates);
	ImagePoints permutationRightImagePoints(testCandidates);

	unsigned int maxValidCorrespondences = testCandidates - 1u;
	Scalar minErrors = Numeric::maxValue();

	unsigned int horizontalBins = 0u;
	unsigned int verticalBins = 0u;
	const SpatialDistribution::DistributionArray indexArray(SpatialDistribution::distributeToArray(leftImagePoints, correspondences, Scalar(0), Scalar(0), Scalar(width), Scalar(height), 4u, 10u, 10u, horizontalBins, verticalBins));

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		IndexSet32 indexSet;

		while (indexSet.size() < testCandidates)
		{
			const unsigned int horizontalBin = Random::random(horizontalBins - 1u);
			const unsigned int verticalBin = Random::random(verticalBins - 1u);

			if (indexArray(horizontalBin, verticalBin).empty())
			{
				continue;
			}

			const unsigned int element = Random::random((unsigned int)(indexArray(horizontalBin, verticalBin).size()) - 1u);

			indexSet.insert(indexArray(horizontalBin, verticalBin)[element]);
		}

		unsigned int n = 0u;
		for (IndexSet32::const_iterator iS = indexSet.begin(); iS != indexSet.end(); ++iS)
		{
			ocean_assert(*iS < correspondences);

			permutationLeftImagePoints[n] = leftImagePoints[*iS];
			permutationRightImagePoints[n] = rightImagePoints[*iS];
			++n;
		}
		ocean_assert(n == testCandidates);

		SquareMatrix3 candidateFundamental;
		if (EpipolarGeometry::fundamentalMatrix(permutationLeftImagePoints.data(), permutationRightImagePoints.data(), testCandidates, candidateFundamental))
		{
			unsigned int validCorrespondences = 0u;
			Scalar errors = 0;
			Indices32 indices;

			for (unsigned int c = 0; c < correspondences; ++c)
			{
				const Vector3 left(leftImagePoints[c], 1);
				const Vector3 right(rightImagePoints[c], 1);

				const Scalar scalarProduct = Numeric::abs((candidateFundamental * left) * right);

				if (scalarProduct < errorThreshold)
				{
					++validCorrespondences;
					errors += scalarProduct;

					if (usedIndices)
					{
						indices.push_back(c);
					}
				}
			}

			if (validCorrespondences > maxValidCorrespondences || (validCorrespondences == maxValidCorrespondences && errors < minErrors))
			{
				fundamental = candidateFundamental;
				minErrors = errors;
				maxValidCorrespondences = validCorrespondences;

				if (usedIndices)
				{
					*usedIndices = indices;
				}
			}
		}
	}

	return maxValidCorrespondences >= testCandidates;
}

bool RANSAC::extrinsicMatrix(const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, HomogenousMatrix4& transformation, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, const Box3& maxTranslation, const Scalar maxRotation, Indices32* usedIndices)
{
	ocean_assert(squarePixelErrorThreshold > 0);
	ocean_assert(leftImagePoints && rightImagePoints);

	if (testCandidates < 8u || correspondences < testCandidates)
	{
		return false;
	}

	Indices32 intermediateUsedIndices;
	if (usedIndices == nullptr)
	{
		usedIndices = &intermediateUsedIndices;
	}

	unsigned int maxValidCorrespondences = testCandidates - 1u;
	Scalar minSquareErrors = Numeric::maxValue();

	HomogenousMatrix4 bestTransformation;

	const Box2 boundingBoxLeft(leftImagePoints, correspondences);
	ocean_assert(boundingBoxLeft);

	unsigned int horizontalBins = 0u;
	unsigned int verticalBins = 0u;

	const SpatialDistribution::DistributionArray indexArray(SpatialDistribution::distributeToArray(leftImagePoints, correspondences, boundingBoxLeft.left(), boundingBoxLeft.top(), boundingBoxLeft.width(), boundingBoxLeft.height(), 4u, 10u, 10u, horizontalBins, verticalBins));

	const unsigned int arrayBins = horizontalBins * verticalBins;

	for (unsigned int i = 0; i < iterations; ++i)
	{
		IndexSet32 indexSet;

		// find subset of given point correspondences for one RANSAC iteration
		while (indexSet.size() < testCandidates)
		{
			const unsigned int bin = Random::random(arrayBins - 1u);
			ocean_assert(bin < arrayBins);

			if (indexArray[bin].empty())
			{
				continue;
			}

			const unsigned int element = Random::random((unsigned int)(indexArray[bin].size()) - 1u);
			ocean_assert(element < indexArray[bin].size());

			indexSet.insert(indexArray[bin][element]);
		}

		const ImagePoints permutationLeftImagePoints(Subset::subset(leftImagePoints, correspondences, indexSet));
		const ImagePoints permutationRightImagePoints(Subset::subset(rightImagePoints, correspondences, indexSet));

		SquareMatrix3 candidateFundamental;
		if (EpipolarGeometry::fundamentalMatrix(permutationLeftImagePoints.data(), permutationRightImagePoints.data(), testCandidates, candidateFundamental))
		{
			const SquareMatrix3 candidateEssential(EpipolarGeometry::fundamental2essential(candidateFundamental, leftCamera, rightCamera));

			HomogenousMatrix4 candidateTransformation;
			if (EpipolarGeometry::factorizeEssential(candidateEssential, leftCamera, rightCamera, permutationLeftImagePoints, permutationRightImagePoints, candidateTransformation) == (unsigned int)permutationLeftImagePoints.size())
			{
				const Scalar candidateRotationAngle = candidateTransformation.rotation().angle();
				const Vector3 candidateTranslation = candidateTransformation.translation();

				if ((!maxTranslation.isValid() || maxTranslation.isInside(candidateTranslation)) && candidateRotationAngle < maxRotation)
				{
					unsigned int validCorrespondences = 0u;
					Scalar squareErrors = 0;
					Indices32 indices;
					indices.reserve(correspondences);

					const HomogenousMatrix4 leftTransformation(true);
					const HomogenousMatrix4& rightTransformation = candidateTransformation;

					const HomogenousMatrix4 invertedFlippedLeft(PinholeCamera::standard2InvertedFlipped(leftTransformation));
					const HomogenousMatrix4 invertedFlippedRight(PinholeCamera::standard2InvertedFlipped(rightTransformation));

					for (unsigned int n = 0u; n < correspondences; ++n)
					{
						const Vector2& leftImagePoint = leftImagePoints[n];
						const Vector2& rightImagePoint = rightImagePoints[n];

						const Line3 leftRay(leftCamera.ray(leftImagePoint, leftTransformation));
						const Line3 rightRay(rightCamera.ray(rightImagePoint, rightTransformation));

						Vector3 center;
						if (leftRay.nearestPoint(rightRay, center) && center.z() < 0)
						{
							// project the object point to both image planes
							const Vector2 leftProjectedImagePoint = leftCamera.projectToImageIF<true>(invertedFlippedLeft, center, false);
							const Vector2 rightProjectedImagePoint = rightCamera.projectToImageIF<true>(invertedFlippedRight, center, false);

							const Scalar leftSquareError = leftImagePoint.sqrDistance(leftProjectedImagePoint);
							const Scalar rightSquareError = rightImagePoint.sqrDistance(rightProjectedImagePoint);

							if (leftSquareError <= squarePixelErrorThreshold && rightSquareError <= squarePixelErrorThreshold)
							{
								++validCorrespondences;
								squareErrors += leftSquareError + rightSquareError;

								indices.push_back(n);
							}
						}
					}

					if (validCorrespondences > maxValidCorrespondences || (validCorrespondences == maxValidCorrespondences && squareErrors < minSquareErrors))
					{
						minSquareErrors = squareErrors;
						maxValidCorrespondences = validCorrespondences;
						bestTransformation = rightTransformation;

						*usedIndices = std::move(indices);
					}
				}
			}
		}
	}

	if (maxValidCorrespondences < testCandidates)
	{
		return false;
	}

	transformation = bestTransformation;
	return true;
}

bool RANSAC::homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const Vector2& leftQuadrantCenter, RandomGenerator& randomGenerator, SquareMatrix3 homographies[4], const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker)
{
	ocean_assert(squarePixelErrorThreshold > 0);
	ocean_assert(leftImagePoints && rightImagePoints);

	if (testCandidates < 4u || correspondences < testCandidates)
	{
		return false;
	}

	Indices32 tmpIndices;
	Indices32* indices = usedIndices ? usedIndices : &tmpIndices;

	unsigned int maxValidCorrespondences = testCandidates - 1u;
	Scalar minSquareErrors = Numeric::maxValue();

	SquareMatrix3 commonHomography;

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&geometricTransformSubset, Homography::homographyMatrixSVD, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &commonHomography, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, (Lock*)&lock, 0u, 0u), 0u, iterations, 12u, 13u, 5u);
	}
	else
	{
		geometricTransformSubset(Homography::homographyMatrixSVD, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &commonHomography, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, nullptr, 0u, iterations);
	}

	if (maxValidCorrespondences < testCandidates)
	{
		return false;
	}

	const Vectors2 validLeftImagePoints(Subset::subset(leftImagePoints, correspondences, *indices));
	const Vectors2 validRightImagePoints(Subset::subset(rightImagePoints, correspondences, *indices));

	SquareMatrix3 optimizedHomography;
	if (Geometry::NonLinearOptimizationHomography::optimizeHomography(commonHomography, validLeftImagePoints.data(), validRightImagePoints.data(), validLeftImagePoints.size(), 9u, optimizedHomography, 20u, Geometry::Estimator::ET_SQUARE))
	{
		commonHomography = optimizedHomography;
	}

	// we create four individual homographies individually for each frame quadrant
	Vectors2 quadrantLeftImagePoints, quadrantRightImagePoints;
	quadrantLeftImagePoints.reserve(validLeftImagePoints.size());
	quadrantRightImagePoints.reserve(validRightImagePoints.size());

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		homographies[n] = commonHomography;

		quadrantLeftImagePoints.clear();
		quadrantRightImagePoints.clear();

		const Scalar left = (n == 0u || n == 2u) ? Numeric::minValue() : leftQuadrantCenter.x();
		const Scalar right = (n == 0u || n == 2u) ? leftQuadrantCenter.x() : Numeric::maxValue();

		const Scalar top = (n == 0u || n == 1u) ? Numeric::minValue() : leftQuadrantCenter.y();
		const Scalar bottom = (n == 0u || n == 1u) ? leftQuadrantCenter.y() : Numeric::maxValue();

		for (size_t i = 0; i < validLeftImagePoints.size(); ++i)
		{
			const Vector2& leftImagePoint = validLeftImagePoints[i];

			if (leftImagePoint.x() >= left && leftImagePoint.x() <= right && leftImagePoint.y() >= top && leftImagePoint.y() <= bottom)
			{
				quadrantLeftImagePoints.push_back(leftImagePoint);
				quadrantRightImagePoints.push_back(validRightImagePoints[i]);
			}
		}

		// check whether the quadrant has enough point correspondences
		if (quadrantLeftImagePoints.size() >= testCandidates)
		{
			if (Geometry::NonLinearOptimizationHomography::optimizeHomography(commonHomography, quadrantLeftImagePoints.data(), quadrantRightImagePoints.data(), quadrantLeftImagePoints.size(), 9u, optimizedHomography, 20u, Geometry::Estimator::ET_SQUARE))
			{
				homographies[n] = optimizedHomography;
			}
		}
	}

	return true;
}

bool RANSAC::projectiveReconstructionFrom6PointsIF(const ConstIndexedAccessor<ImagePoints>& imagePointsPerPose, NonconstIndexedAccessor<HomogenousMatrix4>* posesIF, const unsigned int iterations, const Scalar squarePixelErrorThreshold, NonconstArrayAccessor<ObjectPoint>* objectPointsIF, Indices32* usedIndices, Worker* worker)
{
	ocean_assert(squarePixelErrorThreshold > 0);
	ocean_assert(imagePointsPerPose.size() > 2);

	if (imagePointsPerPose.size() < 1 || imagePointsPerPose[0].size() < 6)
	{
		return false;
	}

	Indices32 indices;
	RandomGenerator randomGenerator;

	Scalar minSquareErrors = Numeric::maxValue();

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&projectiveReconstructionFrom6PointsIFSubset, &imagePointsPerPose, imagePointsPerPose.size(), &randomGenerator, posesIF, squarePixelErrorThreshold, objectPointsIF, &indices, &minSquareErrors, (Lock*)&lock, 0u, 0u), 0u, iterations, 9u, 10u, 5u);
	}
	else
		projectiveReconstructionFrom6PointsIFSubset(&imagePointsPerPose, imagePointsPerPose.size(), &randomGenerator, posesIF, squarePixelErrorThreshold, objectPointsIF, &indices, &minSquareErrors, nullptr, 0u, iterations);

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(indices);
	}

	return true;
}

bool RANSAC::determineCameraCalibrationPlanar(const unsigned int width, const unsigned int height, const CameraCalibration::Patterns& calibrationPatterns, const Vector2& calibrationPatternBoxSize, const unsigned int testCandidates, PinholeCamera& pinholeCamera, const unsigned int iterations, Scalar* sqrAccuracy, Worker* worker, Indices32* usedIndices)
{
	ocean_assert(width > 0u && height > 0u && testCandidates >= 4u);

	if (calibrationPatterns.size() < 4 || testCandidates > calibrationPatterns.size())
	{
		return false;
	}

	typedef std::vector<Indices32> IndexVectorSet;

	const unsigned int maxInterations = iterations * 10u;

	// find valid subsets of the calibration patterns
	IndexVectorSet indexVectorSet;
	for (unsigned int i = 0u; i < maxInterations; ++i)
	{
		IndexSet32 indexSet;
		while (indexSet.size() < testCandidates)
		{
			indexSet.insert(Random::random((unsigned int)(calibrationPatterns.size()) - 1u));
		}

		// test whether the calibration pattern are sufficient different
		Indices32 indexVector;
		for (IndexSet32::const_iterator iS = indexSet.begin(); iS != indexSet.end(); ++iS)
		{
			indexVector.push_back(*iS);
		}

		bool differentPattern = true;
		for (unsigned int outer = 0u; differentPattern && outer < indexVector.size() - 1; ++outer)
		{
			for (unsigned int inner = outer + 1u; inner < indexVector.size(); ++inner)
			{
				const Scalar distance = calibrationPatterns[indexVector[outer]].distance(calibrationPatterns[indexVector[inner]]);

				if (distance < Scalar(5 * 5 * 4))
				{
					differentPattern = false;
					break;
				}
			}
		}

		if (!differentPattern)
		{
			continue;
		}

		indexVectorSet.push_back(indexVector);

		if (indexVectorSet.size() >= iterations)
		{
			break;
		}
	}

	ObjectPointGroups objectPointGroups(calibrationPatterns.size());
	ImagePointGroups imagePointGroups(calibrationPatterns.size());

	for (unsigned int n = 0u; n < calibrationPatterns.size(); ++n)
	{
		CameraCalibration::createCorrespondences(calibrationPatterns[n], calibrationPatternBoxSize, objectPointGroups[n], imagePointGroups[n]);
	}

	Scalars sqrAccuracies(indexVectorSet.size(), Numeric::maxValue());
	PinholeCameras cameras(indexVectorSet.size());

	const ConstArrayAccessor<Vectors3> objectPointGroupsAccessor(objectPointGroups);
	const ConstArrayAccessor<Vectors2> imagePointGroupsAccessor(imagePointGroups);

	if (worker)
	{
		Worker::Functions functions;
		functions.reserve(indexVectorSet.size());

		for (unsigned int n = 0u; n < indexVectorSet.size(); ++n)
		{
			functions.emplace_back(Worker::Function::createStatic(&RANSAC::determineCameraCalibrationPlanarIteration, width, height, (const ConstIndexedAccessor<Vectors3>*)(&objectPointGroupsAccessor), (const ConstIndexedAccessor<Vectors2>*)(&imagePointGroupsAccessor), &indexVectorSet[n], &cameras[n], &sqrAccuracies[n]));
		}

		worker->executeFunctions(functions);
	}
	else
	{
		for (unsigned int n = 0u; n < indexVectorSet.size(); ++n)
		{
			determineCameraCalibrationPlanarIteration(width, height, &objectPointGroupsAccessor, &imagePointGroupsAccessor, &indexVectorSet[n], &cameras[n], &sqrAccuracies[n]);
		}
	}

	Scalar minSqrAccuracy = Numeric::maxValue();
	unsigned int minSqrAccuracyIndex = (unsigned int)(-1);

	for (unsigned int n = 0u; n < sqrAccuracies.size(); ++n)
	{
		if (sqrAccuracies[n] < minSqrAccuracy)
		{
			minSqrAccuracy = sqrAccuracies[n];
			minSqrAccuracyIndex = n;
		}
	}

	if (minSqrAccuracyIndex == (unsigned int)(-1) || minSqrAccuracy > 10)
	{
		return false;
	}

	// find all subsets valid for the camera calibration found during the RANSAC iterations

	const PinholeCamera finalCamera = cameras[minSqrAccuracyIndex];

	ObjectPointGroups finalObjectPointGroups;
	ImagePointGroups finalImagePointGroups;
	HomogenousMatrices4 finalExtrinsics;

	ocean_assert(!usedIndices || usedIndices->empty());

	RandomGenerator randomGenerator;

	for (unsigned int n = 0u; n < objectPointGroups.size(); ++n)
	{
		ObjectPoints& objectPoints = objectPointGroups[n];
		ImagePoints& imagePoints = imagePointGroups[n];

		HomogenousMatrix4 roughPose;
		const bool result = RANSAC::p3p(AnyCameraPinhole(finalCamera), ConstArrayAccessor<ObjectPoint>(objectPoints), ConstArrayAccessor<ImagePoint>(imagePoints), randomGenerator, roughPose);

		ocean_assert(result);
		if (!result)
		{
			continue;
		}

		HomogenousMatrix4 pose;
		if (!NonLinearOptimizationPose::optimizePose(finalCamera, roughPose, ConstArrayAccessor<ObjectPoint>(objectPoints), ConstArrayAccessor<ImagePoint>(imagePoints), true, pose))
		{
			ocean_assert(false && "Should always succeed!");
			continue;
		}

		Scalar averageSqrError = 0;
		Scalar minimalSqrError = Numeric::maxValue();
		Scalar maximalSqrError = 0;
		Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose, finalCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), true, averageSqrError, minimalSqrError, maximalSqrError);

		if (averageSqrError < 4 * 4 && maximalSqrError < 8 * 8)
		{
			finalObjectPointGroups.push_back(std::move(objectPoints));
			finalImagePointGroups.push_back(std::move(imagePoints));
			finalExtrinsics.push_back(pose);

			if (usedIndices != nullptr)
			{
				usedIndices->push_back(n);
			}
		}
	}

	if (finalExtrinsics.empty())
	{
		return false;
	}

	PinholeCamera finalOptimizedCamera;
	Scalar initialSqrError = 0;
	Scalar finalSqrError = 0;

	if (!NonLinearOptimizationCamera::optimizeCameraPoses(finalCamera, ConstArrayAccessor<HomogenousMatrix4>(finalExtrinsics), ConstArrayAccessor<Vectors3>(finalObjectPointGroups), ConstArrayAccessor<Vectors2>(finalImagePointGroups), finalOptimizedCamera, nullptr, 50u, Estimator::ET_SQUARE, Scalar(0.001), 10, true, &initialSqrError, &finalSqrError))
	{
		return false;
	}

	pinholeCamera = finalOptimizedCamera;

	if (sqrAccuracy != nullptr)
	{
		*sqrAccuracy = finalSqrError;
	}

	return true;
}

void RANSAC::determineCameraCalibrationPlanarIteration(const unsigned int width, const unsigned int height, const ConstIndexedAccessor<Vectors3>* objectPointGroups, const ConstIndexedAccessor<Vectors2>* imagePointGroups, Indices32* indices, PinholeCamera* pinholeCamera, Scalar* sqrAccuracy)
{
	ocean_assert(width > 0u && height > 0u);
	ocean_assert(objectPointGroups && imagePointGroups);
	ocean_assert(indices && pinholeCamera && sqrAccuracy);

	ocean_assert(indices->size() >= 4);

	CameraCalibration::determineCameraCalibrationPlanar(width, height, ConstIndexedAccessorSubsetAccessor<Vectors3, unsigned int>(*objectPointGroups, *indices), ConstIndexedAccessorSubsetAccessor<Vectors2, unsigned int>(*imagePointGroups, *indices), *pinholeCamera, 20u, sqrAccuracy);
}

bool RANSAC::affineMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& right_A_left, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker)
{
	if (testCandidates < 3u || correspondences < testCandidates)
	{
		return false;
	}

	return geometricTransform(Geometry::Homography::affineMatrix, leftImagePoints, rightImagePoints, correspondences, randomGenerator, right_A_left, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
}

bool RANSAC::similarityMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& similarity, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker)
{
	if (testCandidates < 2u || correspondences < testCandidates)
	{
		return false;
	}

	return geometricTransform(Geometry::Homography::similarityMatrix, leftImagePoints, rightImagePoints, correspondences, randomGenerator, similarity, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
}

bool RANSAC::objectTransformationStereo(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const ConstIndexedAccessor<Vector3>& objectPointsA, const ConstIndexedAccessor<Vector3>& objectPointsB, const ConstIndexedAccessor<Vector2>& imagePointsA, const ConstIndexedAccessor<Vector2>& imagePointsB, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_object, const unsigned int minimalValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndicesA, Indices32* usedIndicesB, Scalar* sqrAccuracy, const bool allowMonoObservation)
{
	ocean_assert(anyCameraA.isValid() && anyCameraB.isValid());
	ocean_assert(world_T_cameraA.isValid() && world_T_cameraB.isValid());

	ocean_assert(objectPointsA.size() == imagePointsA.size());
	ocean_assert(objectPointsB.size() == imagePointsB.size());

	ocean_assert(minimalValidCorrespondences >= 4u);
	ocean_assert(iterations >= 1u);

	if ((objectPointsA.size() < 4 && objectPointsB.size() < 4) || (objectPointsA.size() < size_t(minimalValidCorrespondences) && objectPointsB.size() < size_t(minimalValidCorrespondences)) || objectPointsA.size() != imagePointsA.size() || objectPointsB.size() != imagePointsB.size())
	{
		return false;
	}

	const ScopedConstMemoryAccessor<Vector3> memoryObjectPointsA(objectPointsA);
	const ScopedConstMemoryAccessor<Vector2> memoryImagePointsA(imagePointsA);

	const ScopedConstMemoryAccessor<Vector3> memoryObjectPointsB(objectPointsB);
	const ScopedConstMemoryAccessor<Vector2> memoryImagePointsB(imagePointsB);

	const HomogenousMatrix4 cameraA_T_cameraB(world_T_cameraA.inverted() * world_T_cameraB);
	const HomogenousMatrix4 cameraB_T_cameraA(world_T_cameraB.inverted() * world_T_cameraA);

	const unsigned int correspondencesA = (unsigned int)(memoryObjectPointsA.size());
	const unsigned int correspondencesB = (unsigned int)(memoryObjectPointsB.size());

	Indices32 indicesA, bestIndicesA;
	indicesA.reserve(correspondencesA);
	bestIndicesA.reserve(correspondencesA);

	Indices32 indicesB, bestIndicesB;
	indicesB.reserve(correspondencesB);
	bestIndicesB.reserve(correspondencesB);

	Vector3 permutationObjectPoints[3];
	Vector2 permutationImagePoints[3];

	unsigned int numberPoses = 0u;
	HomogenousMatrix4 cameraPoses_object_T_camera[4];
	HomogenousMatrix4 flippedCamerasA_T_object[4];
	HomogenousMatrix4 flippedCamerasB_T_object[4];

	HomogenousMatrix4 internal_world_T_object;

	Scalar bestSqrErrors = Numeric::maxValue();

	unsigned int adpativeIterations = iterations;

	for (unsigned int nIteration = 0u; nIteration < adpativeIterations; ++nIteration)
	{
		const bool useCameraA = correspondencesB < minimalValidCorrespondences || (correspondencesA >= minimalValidCorrespondences && (nIteration % 2u == 0u));

		if (useCameraA)
		{
			ocean_assert(correspondencesA >= minimalValidCorrespondences);

			unsigned int index0, index1, index2;
			Random::random(randomGenerator, correspondencesA - 1u, index0, index1, index2);

			ocean_assert(index0 < correspondencesA && index1 < correspondencesA && index2 < correspondencesA);
			ocean_assert(index0 != index1 && index1 != index2 && index0 != index2);

			permutationObjectPoints[0] = memoryObjectPointsA[index0];
			permutationObjectPoints[1] = memoryObjectPointsA[index1];
			permutationObjectPoints[2] = memoryObjectPointsA[index2];

			permutationImagePoints[0] = memoryImagePointsA[index0];
			permutationImagePoints[1] = memoryImagePointsA[index1];
			permutationImagePoints[2] = memoryImagePointsA[index2];

			numberPoses = P3P::poses(anyCameraA, permutationObjectPoints, permutationImagePoints, cameraPoses_object_T_camera);

			for (unsigned int n = 0u; n < numberPoses; ++n)
			{
				flippedCamerasA_T_object[n] = PinholeCamera::standard2InvertedFlipped(cameraPoses_object_T_camera[n]);
				flippedCamerasB_T_object[n] = PinholeCamera::standard2InvertedFlipped(cameraPoses_object_T_camera[n] * cameraA_T_cameraB);
			}
		}
		else
		{
			ocean_assert(correspondencesB >= minimalValidCorrespondences);

			unsigned int index0, index1, index2;
			Random::random(randomGenerator, correspondencesB - 1u, index0, index1, index2);

			ocean_assert(index0 < correspondencesB && index1 < correspondencesB && index2 < correspondencesB);
			ocean_assert(index0 != index1 && index1 != index2 && index0 != index2);

			permutationObjectPoints[0] = memoryObjectPointsB[index0];
			permutationObjectPoints[1] = memoryObjectPointsB[index1];
			permutationObjectPoints[2] = memoryObjectPointsB[index2];

			permutationImagePoints[0] = memoryImagePointsB[index0];
			permutationImagePoints[1] = memoryImagePointsB[index1];
			permutationImagePoints[2] = memoryImagePointsB[index2];

			numberPoses = P3P::poses(anyCameraB, permutationObjectPoints, permutationImagePoints, cameraPoses_object_T_camera);

			for (unsigned int n = 0u; n < numberPoses; ++n)
			{
				flippedCamerasB_T_object[n] = PinholeCamera::standard2InvertedFlipped(cameraPoses_object_T_camera[n]);
				flippedCamerasA_T_object[n] = PinholeCamera::standard2InvertedFlipped(cameraPoses_object_T_camera[n] * cameraB_T_cameraA);
			}
		}

		ocean_assert(numberPoses <= 4u);

		// test which of the (at most four) poses is valid for most remaining point correspondences
		for (unsigned int nPose = 0u; nPose < numberPoses; ++nPose)
		{
			const size_t bestCorrespondences = bestIndicesA.size() + bestIndicesB.size();

			indicesA.clear();
			indicesB.clear();

			Scalar sqrErrors = 0;

			const HomogenousMatrix4& flippedCameraA_T_object = flippedCamerasA_T_object[nPose];

			// now we test each 2D/3D point correspondences and check whether the accuracy of the pose is good enough, we can stop if we cannot reach a better configuration than we have already

			for (unsigned int nCandidateA = 0u; indicesA.size() + (correspondencesA - nCandidateA) + correspondencesB >= bestCorrespondences && nCandidateA < correspondencesA; ++nCandidateA)
			{
				// we accept only object points lying in front of the camera
				if (PinholeCamera::isObjectPointInFrontIF(flippedCameraA_T_object, memoryObjectPointsA[nCandidateA]))
				{
					const Vector2 projectedImagePoint(anyCameraA.projectToImageIF(flippedCameraA_T_object, memoryObjectPointsA[nCandidateA]));
					const Vector2& imagePoint = memoryImagePointsA[nCandidateA];

					const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);

					if (sqrError <= sqrPixelErrorThreshold)
					{
						indicesA.push_back(nCandidateA);
						sqrErrors += sqrError;
					}
				}
			}

			const HomogenousMatrix4& flippedCameraB_T_object = flippedCamerasB_T_object[nPose];

			for (unsigned int nCandidateB = 0u; indicesA.size() + indicesB.size() + (correspondencesB - nCandidateB) >= bestCorrespondences && nCandidateB < correspondencesB; ++nCandidateB)
			{
				// we accept only object points lying in front of the camera
				if (PinholeCamera::isObjectPointInFrontIF(flippedCameraB_T_object, memoryObjectPointsB[nCandidateB]))
				{
					const Vector2 projectedImagePoint(anyCameraB.projectToImageIF(flippedCameraB_T_object, memoryObjectPointsB[nCandidateB]));
					const Vector2& imagePoint = memoryImagePointsB[nCandidateB];

					const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);

					if (sqrError <= sqrPixelErrorThreshold)
					{
						indicesB.push_back(nCandidateB);
						sqrErrors += sqrError;
					}
				}
			}

			if (indicesA.size() + indicesB.size() >= minimalValidCorrespondences)
			{
				if (indicesA.size() + indicesB.size() > bestCorrespondences || (indicesA.size() + indicesB.size() == bestCorrespondences && sqrErrors < bestSqrErrors))
				{
					bestSqrErrors = sqrErrors;

					internal_world_T_object = world_T_cameraA * PinholeCamera::flippedTransformationLeftSide(flippedCameraA_T_object);

					std::swap(bestIndicesA, indicesA);
					std::swap(bestIndicesB, indicesB);

					const unsigned int correspondences = correspondencesA + correspondencesB;

					constexpr Scalar successProbability = Scalar(0.99);
					const Scalar faultyRate = Scalar(1) - Scalar(bestIndicesA.size() + bestIndicesB.size()) / Scalar(correspondences);

					adpativeIterations = minmax<unsigned int>(2u, RANSAC::iterations(3u, successProbability, faultyRate), adpativeIterations);
				}
			}
		}
	}

	if (bestIndicesA.size() + bestIndicesB.size() < minimalValidCorrespondences)
	{
		return false;
	}

	if (!allowMonoObservation && (bestIndicesA.empty() || bestIndicesB.empty()))
	{
		return false;
	}

	world_T_object = internal_world_T_object;

	if (sqrAccuracy)
	{
		ocean_assert(bestIndicesA.size() + bestIndicesB.size() != 0);
		*sqrAccuracy = bestSqrErrors / Scalar(bestIndicesA.size() + bestIndicesB.size());
	}

	// non linear least square refinement step
	if (refine)
	{
		if (bestIndicesA.empty() || bestIndicesB.empty())
		{
			// the object is only visible in one of both camera frames

			ocean_assert(allowMonoObservation);
			ocean_assert(!bestIndicesA.empty() || !bestIndicesB.empty());

			HomogenousMatrices4 flippedCameras_T_world;
			std::vector<Vectors3> objectPointGroups;
			std::vector<Vectors2> imagePointGroups;

			const AnyCamera& anyCamera = bestIndicesA.empty() ? anyCameraB : anyCameraA;

			if (bestIndicesA.empty())
			{
				flippedCameras_T_world = HomogenousMatrices4(1, PinholeCamera::standard2InvertedFlipped(world_T_cameraB));
				objectPointGroups = std::vector<Vectors3>(1, Subset::subset(memoryObjectPointsB.data(), memoryObjectPointsB.size(), bestIndicesB));
				imagePointGroups = std::vector<Vectors2>(1, Subset::subset(memoryImagePointsB.data(), memoryImagePointsB.size(), bestIndicesB));
			}
			else
			{
				flippedCameras_T_world = HomogenousMatrices4(1, PinholeCamera::standard2InvertedFlipped(world_T_cameraA));
				objectPointGroups = std::vector<Vectors3>(1, Subset::subset(memoryObjectPointsA.data(), memoryObjectPointsA.size(), bestIndicesA));
				imagePointGroups = std::vector<Vectors2>(1, Subset::subset(memoryImagePointsA.data(), memoryImagePointsA.size(), bestIndicesA));
			}

			if (!NonLinearOptimizationTransformation::optimizeObjectTransformationIF(anyCamera, flippedCameras_T_world, internal_world_T_object, objectPointGroups, imagePointGroups, world_T_object, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy))
			{
				return false;
			}
		}
		else
		{
			const HomogenousMatrices4 flippedCamerasA_T_world(1, PinholeCamera::standard2InvertedFlipped(world_T_cameraA));
			const HomogenousMatrices4 flippedCamerasB_T_world(1, PinholeCamera::standard2InvertedFlipped(world_T_cameraB));

			const std::vector<Vectors3> objectPointGroupsA(1, Subset::subset(memoryObjectPointsA.data(), memoryObjectPointsA.size(), bestIndicesA));
			const std::vector<Vectors3> objectPointGroupsB(1, Subset::subset(memoryObjectPointsB.data(), memoryObjectPointsB.size(), bestIndicesB));

			const std::vector<Vectors2> imagePointGroupsA(1, Subset::subset(memoryImagePointsA.data(), memoryImagePointsA.size(), bestIndicesA));
			const std::vector<Vectors2> imagePointGroupsB(1, Subset::subset(memoryImagePointsB.data(), memoryImagePointsB.size(), bestIndicesB));

			if (!NonLinearOptimizationTransformation::optimizeObjectTransformationStereoIF(anyCameraA, anyCameraB, flippedCamerasA_T_world, flippedCamerasB_T_world, internal_world_T_object, objectPointGroupsA, objectPointGroupsB, imagePointGroupsA, imagePointGroupsB, world_T_object, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy))
			{
				return false;
			}
		}

		// check whether we need to determine the indices for the optimized pose again

		if (usedIndicesA)
		{
			const HomogenousMatrix4 flippedCameraA_T_object(PinholeCamera::flippedTransformationLeftSide(world_T_cameraA.inverted() * world_T_object));

			bestIndicesA.clear();

			for (unsigned int nCandidateA = 0u; nCandidateA < correspondencesA; ++nCandidateA)
			{
				const Vector3& objectPoint = memoryObjectPointsA[nCandidateA];
				const Vector2& imagePoint = memoryImagePointsA[nCandidateA];

				// we accept only object points lying in front of the camera
				if (PinholeCamera::isObjectPointInFrontIF(flippedCameraA_T_object, objectPoint) && imagePoint.sqrDistance(anyCameraA.projectToImageIF(flippedCameraA_T_object, objectPoint)) <= sqrPixelErrorThreshold)
				{
					bestIndicesA.push_back(nCandidateA);
				}
			}
		}

		if (usedIndicesB)
		{
			const HomogenousMatrix4 flippedCameraB_T_object(PinholeCamera::flippedTransformationLeftSide(world_T_cameraB.inverted() * world_T_object));

			bestIndicesB.clear();

			for (unsigned int nCandidateB = 0u; nCandidateB < correspondencesB; ++nCandidateB)
			{
				const Vector3& objectPoint = memoryObjectPointsB[nCandidateB];
				const Vector2& imagePoint = memoryImagePointsB[nCandidateB];

				// we accept only object points lying in front of the camera
				if (PinholeCamera::isObjectPointInFrontIF(flippedCameraB_T_object, objectPoint) && imagePoint.sqrDistance(anyCameraB.projectToImageIF(flippedCameraB_T_object, objectPoint)) <= sqrPixelErrorThreshold)
				{
					bestIndicesB.push_back(nCandidateB);
				}
			}
		}

		if (bestIndicesA.size() + bestIndicesB.size() < minimalValidCorrespondences)
		{
			return false;
		}
	}

	if (usedIndicesA != nullptr)
	{
		*usedIndicesA = std::move(bestIndicesA);
	}

	if (usedIndicesB != nullptr)
	{
		*usedIndicesB = std::move(bestIndicesB);
	}

	return true;
}

bool RANSAC::p3p(const HomogenousMatrix4* world_T_roughCamera, const AnyCamera& camera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const Vector3* maxPositionOffset, const Scalar* maxOrientationOffset, const unsigned int minValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights)
{
	const ScopedConstMemoryAccessor<Vector3> objectPoints(objectPointAccessor);
	const ScopedConstMemoryAccessor<Vector2> imagePoints(imagePointAccessor);

	ocean_assert(objectPoints.size() >= 4);
	ocean_assert(objectPoints.size() == imagePoints.size());

	if (objectPoints.size() < 4 || objectPoints.size() != imagePoints.size())
	{
		return false;
	}

	ocean_assert(NumericT<unsigned int>::isInsideValueRange(objectPoints.size()));
	const unsigned int correspondences = (unsigned int)(objectPoints.size());

	Indices32 indices, bestIndices;
	indices.reserve(correspondences);
	bestIndices.reserve(correspondences);

	Vector3 permutationObjectPoints[3];
	Vector3 permutationImageRays[3];

	HomogenousMatrix4 world_T_candidateCameras[4];
	HomogenousMatrix4 world_T_bestCamera(false);

	Scalar bestSqrErrors = Numeric::maxValue();

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		unsigned int index0;
		unsigned int index1;
		unsigned int index2;
		Random::random(randomGenerator, (unsigned int)(correspondences - 1u), index0, index1, index2);

		ocean_assert(index0 < correspondences);
		ocean_assert(index1 < correspondences);
		ocean_assert(index2 < correspondences);
		ocean_assert(index0 != index1 && index1 != index2 && index0 != index2);

		permutationObjectPoints[0] = objectPoints[index0];
		permutationObjectPoints[1] = objectPoints[index1];
		permutationObjectPoints[2] = objectPoints[index2];

		permutationImageRays[0] = camera.vector(imagePoints[index0]);
		permutationImageRays[1] = camera.vector(imagePoints[index1]);
		permutationImageRays[2] = camera.vector(imagePoints[index2]);

		const unsigned int numberPoses = P3P::poses(permutationObjectPoints, permutationImageRays, world_T_candidateCameras);
		ocean_assert(numberPoses <= 4u);

		// test which of the (at most four) poses is valid for most remaining point correspondences
		for (unsigned int n = 0u; n < numberPoses; ++n)
		{
			indices.clear();

			if (world_T_roughCamera != nullptr && !Error::posesAlmostEqual(*world_T_roughCamera, world_T_candidateCameras[n], *maxPositionOffset, *maxOrientationOffset))
			{
				continue;
			}

			Scalar sqrErrors = 0;

			const HomogenousMatrix4 candidateFlippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_candidateCameras[n]));

			// now we test each 2D/3D point correspondences and check whether the accuracy of the pose is good enough
			for (unsigned int c = 0u; c < correspondences; ++c)
			{
				// we accept only object points lying in front of the camera
				if (AnyCamera::isObjectPointInFrontIF(candidateFlippedCamera_T_world, objectPoints[c]))
				{
					const Vector2 projectedImagePoint(camera.projectToImageIF(candidateFlippedCamera_T_world, objectPoints[c]));
					const Vector2& imagePoint = imagePoints[c];

					const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);

					if (sqrError <= sqrPixelErrorThreshold)
					{
						indices.push_back(c);
						sqrErrors += sqrError;
					}
				}
			}

			if (indices.size() >= minValidCorrespondences)
			{
				if (indices.size() > bestIndices.size() || (indices.size() == bestIndices.size() && sqrErrors < bestSqrErrors))
				{
					bestSqrErrors = sqrErrors;

					world_T_bestCamera = world_T_candidateCameras[n];
					std::swap(bestIndices, indices);
				}
			}
		}
	}

	if (bestIndices.size() < minValidCorrespondences)
	{
		return false;
	}

	world_T_camera = world_T_bestCamera;

	if (sqrAccuracy != nullptr)
	{
		*sqrAccuracy = bestSqrErrors /= Scalar(bestIndices.size());
	}

	// non linear least square refinement step
	if (refine)
	{
		Matrix invertedCovariances;

		if (weights != nullptr)
		{
			invertedCovariances.resize(bestIndices.size() * 2, 2);
			SquareMatrix2* invertedCovarianceMatrices = reinterpret_cast<SquareMatrix2*>(invertedCovariances.data());

			for (size_t n = 0; n < bestIndices.size(); ++n)
			{
				invertedCovarianceMatrices[n] = SquareMatrix2(weights[bestIndices[n]], 0, 0, weights[bestIndices[n]]);
			}
		}

		world_T_bestCamera = world_T_camera;

		if (!NonLinearOptimizationPose::optimizePose(camera, world_T_bestCamera, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), world_T_camera, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy, invertedCovariances ? &invertedCovariances : nullptr))
		{
			return false;
		}

		// check whether we need to determine the indices for the optimized pose followed by another final optimization step
		if (usedIndices != nullptr && bestIndices.size() != correspondences)
		{
			const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

			bestIndices.clear();

			for (unsigned int c = 0u; c < correspondences; ++c)
			{
				// we accept only object points lying in front of the camera
				if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoints[c]))
				{
					const Vector2 projectedImagePoint(camera.projectToImageIF(flippedCamera_T_world, objectPoints[c]));
					const Vector2& imagePoint = imagePoints[c];

					const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);

					if (sqrError <= sqrPixelErrorThreshold)
					{
						bestIndices.push_back(c);
					}
				}
			}

			ocean_assert(invertedCovariances.elements() == 0 || weights != nullptr);

			if (weights != nullptr)
			{
				invertedCovariances.resize(bestIndices.size() * 2, 2);
				SquareMatrix2* invertedCovarianceMatrices = reinterpret_cast<SquareMatrix2*>(invertedCovariances.data());

				for (size_t n = 0; n < bestIndices.size(); ++n)
				{
					invertedCovarianceMatrices[n] = SquareMatrix2(weights[bestIndices[n]], 0, 0, weights[bestIndices[n]]);
				}
			}

			world_T_bestCamera = world_T_camera;

			if (!NonLinearOptimizationPose::optimizePose(camera, world_T_bestCamera, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), world_T_camera, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy, invertedCovariances ? &invertedCovariances : nullptr))
			{
				return false;
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(bestIndices);
	}

	return true;
}

bool RANSAC::p3pZoom(const HomogenousMatrix4* initialPose, const Scalar* initialZoom, const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, Scalar& zoom, const Vector3* maxPositionOffset, const Scalar* maxOrientationOffset, const unsigned int minValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights)
{
	ocean_assert((initialPose && initialZoom) || (!initialPose && !initialZoom));

	const ScopedConstMemoryAccessor<Vector3> objectPoints(objectPointAccessor);
	const ScopedConstMemoryAccessor<Vector2> imagePoints(imagePointAccessor);

	ocean_assert(objectPoints.size() >= 4);
	ocean_assert(objectPoints.size() == imagePoints.size());

	if (objectPoints.size() < 4 || objectPoints.size() != imagePoints.size())
		return false;

	const unsigned int correspondences = (unsigned int)objectPoints.size();

	Indices32 indices, bestIndices;
	indices.reserve(correspondences);
	bestIndices.reserve(correspondences);

	ObjectPoint permutationObjectPoints[4];
	ImagePoint permutationImagePoints[4];
	HomogenousMatrix4 poses[4];

	HomogenousMatrix4 internalPose;
	Scalar internalZoom = Numeric::minValue();

	PinholeCamera initialZoomedCamera(pinholeCamera);

	if (initialZoom && *initialZoom > Numeric::eps())
		initialZoomedCamera.applyZoomFactor(*initialZoom);

	Scalar bestSqrErrors = Numeric::maxValue();

	for (unsigned int i = 0u; i < iterations; ++i)
	{
		unsigned int index0, index1, index2;
		Random::random(randomGenerator, (unsigned int)correspondences - 1u, index0, index1, index2);

		ocean_assert(index0 < correspondences);
		ocean_assert(index1 < correspondences);
		ocean_assert(index2 < correspondences);

		ocean_assert(index0 != index1 && index1 != index2);

		permutationObjectPoints[0] = objectPoints[index0];
		permutationObjectPoints[1] = objectPoints[index1];
		permutationObjectPoints[2] = objectPoints[index2];

		permutationImagePoints[0] = useDistortionParameters ? initialZoomedCamera.undistort<true>(imagePoints[index0]) : imagePoints[index0];
		permutationImagePoints[1] = useDistortionParameters ? initialZoomedCamera.undistort<true>(imagePoints[index1]) : imagePoints[index1];
		permutationImagePoints[2] = useDistortionParameters ? initialZoomedCamera.undistort<true>(imagePoints[index2]) : imagePoints[index2];

		const unsigned int numberPoses = P3P::poses(initialZoomedCamera, permutationObjectPoints, permutationImagePoints, poses);
		ocean_assert(numberPoses <= 4u);

		// test which of the (at most four) poses is valid for most remaining point correspondences
		for (unsigned int n = 0u; n < numberPoses; ++n)
		{
			indices.clear();

			// as we used the 'initialZoomedCamera' we can directly compare the pose with the 'initialPose'
			if (initialPose && !Error::posesAlmostEqual(*initialPose, poses[n], *maxPositionOffset, *maxOrientationOffset))
			{
				continue;
			}

			// now we first have to determine/approximate the rough zoom factor so that we can measure the pixel errors correctly (if we use zoom=1 the pixel errors may be very large even for a 'good' pose)
			// we find the rough pose by randomly selecting a fourth point correspondence so that we can apply a non linear optimization
			// for each pose we test up to three zoom determination steps (if the number of matching indices does not exceed 3)

			unsigned int zoomIteration = 0u;
			Scalar sqrErrors = 0;

			HomogenousMatrix4 optimizedPoseIF;
			Scalar optimizedZoom = 0;

			while (zoomIteration++ < 3u && indices.size() <= 3)
			{
				indices.clear();
				sqrErrors = 0;

				unsigned int index3 = Random::random(randomGenerator, (unsigned int)correspondences - 1u);
				while (index3 == index0 || index3 == index1 || index3 == index2)
				{
					index3 = Random::random(randomGenerator, (unsigned int)correspondences - 1u);
				}

				const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(poses[n]));

				permutationObjectPoints[3] = objectPoints[index3];

				permutationImagePoints[0] = imagePoints[index0]; // we need to overwrite the possibly undistorted image points
				permutationImagePoints[1] = imagePoints[index1];
				permutationImagePoints[2] = imagePoints[index2];
				permutationImagePoints[3] = imagePoints[index3];

				const Scalar startZoom = initialZoom ? *initialZoom : Scalar(1);
				Scalar finalSqrError = Numeric::maxValue();

				if (!PinholeCamera::isObjectPointInFrontIF(poseIF, permutationObjectPoints[0]) || !PinholeCamera::isObjectPointInFrontIF(poseIF, permutationObjectPoints[1])
						|| !PinholeCamera::isObjectPointInFrontIF(poseIF, permutationObjectPoints[2]) || !PinholeCamera::isObjectPointInFrontIF(poseIF, permutationObjectPoints[3]))
				{
					continue;
				}

				if (!NonLinearOptimizationPose::optimizePoseZoomIF(pinholeCamera, poseIF, startZoom, ConstArrayAccessor<Vector3>(permutationObjectPoints, 4), ConstArrayAccessor<Vector2>(permutationImagePoints, 4), true, optimizedPoseIF, optimizedZoom, 10u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, &finalSqrError) || finalSqrError > Scalar(10 * 10))
				{
					continue;
				}

				// now we test each 2D/3D point correspondences and check whether the accuracy of the pose is good enough
				for (unsigned int c = 0; c < correspondences; ++c)
				{
					// we accept only object points lying in front of the camera
					if ((optimizedPoseIF * objectPoints[c]).z() > Numeric::eps())
					{
						const Vector2 projectedImagePoint(pinholeCamera.projectToImageIF<true>(optimizedPoseIF, objectPoints[c], useDistortionParameters, optimizedZoom));
						const ImagePoint& imagePoint = imagePoints[c];

						const Scalar sqrError = imagePoint.sqrDistance(projectedImagePoint);

						if (sqrError <= sqrPixelErrorThreshold)
						{
							indices.push_back(c);
							sqrErrors += sqrError;
						}
					}
				}
			}

			if (optimizedZoom != Scalar(0) && (indices.size() > bestIndices.size() || (indices.size() == bestIndices.size() && sqrErrors < bestSqrErrors)))
			{
				bestSqrErrors = sqrErrors;

				internalPose = PinholeCamera::invertedFlipped2Standard(optimizedPoseIF);
				internalZoom = optimizedZoom;
				std::swap(bestIndices, indices);
			}
		}
	}

	if (bestIndices.size() < minValidCorrespondences || bestIndices.size() < 5)
	{
		return false;
	}

	pose = internalPose;
	zoom = internalZoom;

	if (sqrAccuracy != nullptr)
	{
		*sqrAccuracy = bestSqrErrors /= Scalar(bestIndices.size());
	}

	// non linear least square refinement step
	if (refine)
	{
		Matrix invertedCovariances;
		if (weights)
		{
			invertedCovariances.resize(bestIndices.size() * 2, 2);
			SquareMatrix2* invertedCovarianceMatrices = reinterpret_cast<SquareMatrix2*>(invertedCovariances.data());

			for (size_t n = 0; n < bestIndices.size(); ++n)
			{
				invertedCovarianceMatrices[n] = SquareMatrix2(weights[bestIndices[n]], 0, 0, weights[bestIndices[n]]);
			}
		}

		internalPose = pose;
		internalZoom = zoom;

		if (!NonLinearOptimizationPose::optimizePoseZoom(pinholeCamera, internalPose, internalZoom, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), useDistortionParameters, pose, zoom, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy, invertedCovariances ? &invertedCovariances : nullptr))
		{
			return false;
		}

		// check whether we need to determine the indices for the optimized pose followed by another final optimization step
		if (usedIndices && bestIndices.size() != correspondences)
		{
			const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));

			bestIndices.clear();
			for (unsigned int c = 0; c < correspondences; ++c)
			{
				// we accept only object points lying in front of the camera
				if ((poseIF * objectPoints[c]).z() > Numeric::eps() && imagePoints[c].sqrDistance(pinholeCamera.projectToImageIF<true>(poseIF, objectPoints[c], useDistortionParameters, zoom)) <= sqrPixelErrorThreshold)
					bestIndices.push_back(c);
			}

			ocean_assert(invertedCovariances.elements() == 0 || weights);
			if (weights)
			{
				invertedCovariances.resize(bestIndices.size() * 2, 2);
				SquareMatrix2* invertedCovarianceMatrices = reinterpret_cast<SquareMatrix2*>(invertedCovariances.data());

				for (size_t n = 0; n < bestIndices.size(); ++n)
				{
					invertedCovarianceMatrices[n] = SquareMatrix2(weights[bestIndices[n]], 0, 0, weights[bestIndices[n]]);
				}
			}

			internalPose = pose;
			internalZoom = zoom;

			if (!NonLinearOptimizationPose::optimizePoseZoom(pinholeCamera, internalPose, internalZoom, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), useDistortionParameters, pose, zoom, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy, invertedCovariances ? &invertedCovariances : nullptr))
			{
				return false;
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(bestIndices);
	}

	return true;
}

bool RANSAC::geometricTransform(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& transformMatrix, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker)
{
	ocean_assert(geometricTransformFunction != nullptr);
	ocean_assert(squarePixelErrorThreshold > 0);
	ocean_assert(leftImagePoints && rightImagePoints);

	if (correspondences < testCandidates)
	{
		return false;
	}

	Indices32 tmpIndices;
	Indices32* indices = &tmpIndices;

	ocean_assert(testCandidates != 0u);
	unsigned int maxValidCorrespondences = testCandidates - 1u;
	Scalar minSquareErrors = Numeric::maxValue();

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&geometricTransformSubset, geometricTransformFunction, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &transformMatrix, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, (Lock*)&lock, 0u, 0u), 0u, iterations, 12u, 13u, 5u);
	}
	else
	{
		geometricTransformSubset(geometricTransformFunction, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &transformMatrix, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, nullptr, 0u, iterations);
	}

	if (maxValidCorrespondences < testCandidates)
	{
		return false;
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(tmpIndices);
	}

	return true;
}

bool RANSAC::geometricTransformForNonBijectiveCorrespondences(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator& randomGenerator, SquareMatrix3& transformMatrix, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker)
{
	ocean_assert(geometricTransformFunction != nullptr);
	ocean_assert(leftImagePoints != nullptr && rightImagePoints != nullptr);
	ocean_assert(numberLeftImagePoints >= 1 && numberRightImagePoints >= 1);
	ocean_assert(correspondences != nullptr);
	ocean_assert(numberCorrespondences >= 1);
	ocean_assert(squarePixelErrorThreshold > 0);

	if (numberCorrespondences < size_t(testCandidates))
	{
		return false;
	}

	Indices32 tmpIndices;
	Indices32* indices = &tmpIndices;

	ocean_assert(testCandidates != 0u);
	unsigned int maxValidCorrespondences = testCandidates - 1u;
	Scalar minSquareErrors = Numeric::maxValue();

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&geometricTransformForNonBijectiveCorrespondencesSubset, geometricTransformFunction, leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, &randomGenerator, &transformMatrix, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, (Lock*)&lock, 0u, 0u), 0u, iterations, 15u, 16u, 5u);
	}
	else
	{
		geometricTransformForNonBijectiveCorrespondencesSubset(geometricTransformFunction, leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, &randomGenerator, &transformMatrix, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, nullptr, 0u, iterations);
	}

	if (maxValidCorrespondences < testCandidates)
	{
		return false;
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(tmpIndices);
	}

	return true;
}

void RANSAC::geometricTransformSubset(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator* randomGenerator, SquareMatrix3* transformMatrix, const unsigned int testCandidates, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, unsigned int* maxValidCandidates, Scalar* minSquareErrors, Lock* lock, const unsigned int /*firstIteration*/, const unsigned int numberIterations)
{
	ocean_assert(geometricTransformFunction != nullptr);
	ocean_assert(squarePixelErrorThreshold > 0);
	ocean_assert(leftImagePoints && rightImagePoints);

	ocean_assert(randomGenerator);
	ocean_assert(transformMatrix);
	ocean_assert(maxValidCandidates);
	ocean_assert(minSquareErrors);

	ocean_assert(testCandidates <= correspondences);

	RandomGenerator generator(*randomGenerator);

	Indices32 localBestIndices;
	localBestIndices.reserve(correspondences);

	Indices32 internalIntermediateIndices;
	internalIntermediateIndices.reserve(correspondences);

	ocean_assert(testCandidates >= 1u);
	unsigned int localMaxValidCorrespondences = testCandidates - 1u;
	Scalar localMinSquareErrors = Numeric::maxValue();
	SquareMatrix3 localBestModel;

	for (unsigned int i = 0u; i < numberIterations; ++i)
	{
		internalIntermediateIndices.clear();

		IndexSet32 indexSet;

		// find subset of given point correspondences for one RANSAC iteration
		while (indexSet.size() < testCandidates)
		{
			const Index32 index = RandomI::random(generator, (unsigned int)correspondences - 1u);
			indexSet.insert(index);
		}

		const ImagePoints permutationLeftImagePoints(Subset::subset(leftImagePoints, correspondences, indexSet));
		const ImagePoints permutationRightImagePoints(Subset::subset(rightImagePoints, correspondences, indexSet));

		SquareMatrix3 candidateModel;
		if (geometricTransformFunction(permutationLeftImagePoints.data(), permutationRightImagePoints.data(), testCandidates, candidateModel))
		{
			unsigned int validCorrespondences = 0u;
			Scalar squareErrors = 0;

			for (size_t n = 0; n < correspondences; ++n)
			{
				const Vector2& leftImagePoint = leftImagePoints[n];
				const Vector2& rightImagePoint = rightImagePoints[n];

				Vector2 testRightPoint;
				if (candidateModel.multiply(leftImagePoint, testRightPoint))
				{
					const Scalar squareError = testRightPoint.sqrDistance(rightImagePoint);

					if (squareError < squarePixelErrorThreshold)
					{
						++validCorrespondences;
						squareErrors += squareError;

						internalIntermediateIndices.push_back((unsigned int)n);
					}
				}
			}

			if (validCorrespondences > localMaxValidCorrespondences || (validCorrespondences == localMaxValidCorrespondences && squareErrors < localMinSquareErrors))
			{
				localMinSquareErrors = squareErrors;
				localMaxValidCorrespondences = validCorrespondences;
				localBestModel = candidateModel;

				std::swap(localBestIndices, internalIntermediateIndices);
			}
		}
	}

	if (localMaxValidCorrespondences >= testCandidates)
	{
		const OptionalScopedLock scopedLock(lock);

		if (localMaxValidCorrespondences > *maxValidCandidates || (localMaxValidCorrespondences == *maxValidCandidates && localMinSquareErrors < *minSquareErrors))
		{
			*minSquareErrors = localMinSquareErrors / Scalar(localMaxValidCorrespondences);
			*maxValidCandidates = localMaxValidCorrespondences;
			*transformMatrix = localBestModel;

			if (usedIndices != nullptr)
			{
				*usedIndices = std::move(localBestIndices);
			}
		}
	}
}

void RANSAC::geometricTransformForNonBijectiveCorrespondencesSubset(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator* randomGenerator, SquareMatrix3* transformMatrix, const unsigned int testCandidates, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, unsigned int* maxValidCandidates, Scalar* minSquareErrors, Lock* lock, const unsigned int /*firstIteration*/, const unsigned int numberIterations)
{
	ocean_assert(geometricTransformFunction != nullptr);
	ocean_assert(leftImagePoints != nullptr && rightImagePoints != nullptr);
	ocean_assert(numberLeftImagePoints >= 1 && numberRightImagePoints >= 1);
	ocean_assert(correspondences != nullptr && numberCorrespondences > 0);
	ocean_assert(squarePixelErrorThreshold > 0);

	ocean_assert(randomGenerator);
	ocean_assert(transformMatrix);
	ocean_assert(maxValidCandidates);
	ocean_assert(minSquareErrors);

	ocean_assert(testCandidates <= numberCorrespondences);

	RandomGenerator generator(*randomGenerator);

	Indices32 localBestIndices;
	localBestIndices.reserve(numberCorrespondences);

	Indices32 internalIntermediateIndices;
	internalIntermediateIndices.reserve(numberCorrespondences);

	ocean_assert(testCandidates >= 1u);
	Scalar localMinSquareErrors = Numeric::maxValue();
	SquareMatrix3 localBestModel;

	const unsigned int maximalSearchIterations = testCandidates * 20u;

	ImagePoints permutationLeftImagePoints;
	ImagePoints permutationRightImagePoints;

	std::vector<unsigned char> leftIndicesUsed(numberLeftImagePoints);
	std::vector<unsigned char> rightIndicesUsed(numberRightImagePoints);

	for (unsigned int i = 0u; i < numberIterations; ++i)
	{
		internalIntermediateIndices.clear();

		permutationLeftImagePoints.clear();
		permutationRightImagePoints.clear();

		memset(leftIndicesUsed.data(), 0x00, leftIndicesUsed.size() * sizeof(unsigned char));
		memset(rightIndicesUsed.data(), 0x00, rightIndicesUsed.size() * sizeof(unsigned char));

		unsigned int searchIterations = 0u;

		// find subset of given point correspondences for one RANSAC iteration
		while (permutationLeftImagePoints.size() < testCandidates && searchIterations++ < maximalSearchIterations)
		{
			const Index32 correspondenceIndex = RandomI::random(generator, (unsigned int)numberCorrespondences - 1u);

			const IndexPair32& correspondence = correspondences[correspondenceIndex];
			const unsigned int leftIndex = correspondence.first;
			const unsigned int rightIndex = correspondence.second;

			ocean_assert(leftIndex < (unsigned int)numberLeftImagePoints);
			ocean_assert(rightIndex < (unsigned int)numberRightImagePoints);

			if (!leftIndicesUsed[leftIndex] && !rightIndicesUsed[rightIndex])
			{
				leftIndicesUsed[leftIndex] = 1u;
				rightIndicesUsed[rightIndex] = 1u;

				permutationLeftImagePoints.push_back(leftImagePoints[leftIndex]);
				permutationRightImagePoints.push_back(rightImagePoints[rightIndex]);
			}
		}

		ocean_assert(permutationLeftImagePoints.size() == permutationRightImagePoints.size());

		if (permutationLeftImagePoints.size() < testCandidates)
		{
			// we could not find enough valid correspondences in this iterations
			continue;
		}

		SquareMatrix3 candidateModel;
		if (geometricTransformFunction(permutationLeftImagePoints.data(), permutationRightImagePoints.data(), testCandidates, candidateModel))
		{
			Scalar squareErrors = 0;

			for (size_t n = 0; n < numberCorrespondences; ++n)
			{
				const IndexPair32& correspondence = correspondences[n];
				const unsigned int leftIndex = correspondence.first;
				const unsigned int rightIndex = correspondence.second;

				ocean_assert(leftIndex < (unsigned int)numberLeftImagePoints);
				ocean_assert(rightIndex < (unsigned int)numberRightImagePoints);

				const Vector2& leftImagePoint = leftImagePoints[leftIndex];
				const Vector2& rightImagePoint = rightImagePoints[rightIndex];

				Vector2 testRightPoint;
				if (candidateModel.multiply(leftImagePoint, testRightPoint))
				{
					const Scalar squareError = testRightPoint.sqrDistance(rightImagePoint);

					if (squareError < squarePixelErrorThreshold)
					{
						if (!leftIndicesUsed[leftIndex] && !rightIndicesUsed[rightIndex])
						{
							leftIndicesUsed[leftIndex] = 1u;
							rightIndicesUsed[rightIndex] = 1u;

							squareErrors += squareError;

							internalIntermediateIndices.push_back((unsigned int)n);
						}
					}
				}
			}

			if (internalIntermediateIndices.size() > localBestIndices.size() || (internalIntermediateIndices.size() == localBestIndices.size() && squareErrors < localMinSquareErrors))
			{
				localMinSquareErrors = squareErrors;
				localBestModel = candidateModel;

				std::swap(localBestIndices, internalIntermediateIndices);
			}
		}
	}

	if (localBestIndices.size() >= size_t(testCandidates))
	{
		const OptionalScopedLock scopedLock(lock);

		if ((unsigned int)localBestIndices.size() > *maxValidCandidates || ((unsigned int)localBestIndices.size() == *maxValidCandidates && localMinSquareErrors < *minSquareErrors))
		{
			*minSquareErrors = localMinSquareErrors / Scalar(localBestIndices.size());
			*maxValidCandidates = (unsigned int)localBestIndices.size();
			*transformMatrix = localBestModel;

			if (usedIndices != nullptr)
			{
				*usedIndices = std::move(localBestIndices);
			}
		}
	}
}

void RANSAC::projectiveReconstructionFrom6PointsIFSubset(const ConstIndexedAccessor<ImagePoints>* imagePointsPerPose, const size_t views, RandomGenerator* randomGenerator, NonconstIndexedAccessor<HomogenousMatrix4>* posesIF, const Scalar squarePixelErrorThreshold, NonconstArrayAccessor<ObjectPoint>* objectPointsIF, Indices32* usedIndices, Scalar* minSquareErrors, Lock* lock, const unsigned int /*firstIteration*/, const unsigned int numberIterations)
{
	ocean_assert(squarePixelErrorThreshold > 0);
	ocean_assert(views >= 2);
	ocean_assert(imagePointsPerPose);
	ocean_assert(randomGenerator);
	ocean_assert(posesIF);
	ocean_assert(minSquareErrors);

	size_t correspondences = (*imagePointsPerPose)[0].size();

	RandomGenerator generator(*randomGenerator);

	*minSquareErrors = Numeric::maxValue();
	size_t maxCountInliers = 0;

	for (unsigned int i = 0u; i < numberIterations; ++i)
	{
		IndexSet32 indexSet;

		// find subset of given point correspondences for one RANSAC iteration
		while (indexSet.size() < 6)
		{
			const Index32 index = RandomI::random(generator, (unsigned int)correspondences - 1u);
			indexSet.insert(index);
		}

		std::vector<ImagePoints> permutationImagePointsSet;
		permutationImagePointsSet.reserve(views);
		for (size_t n = 0; n < views; n++)
		{
			const ImagePoints permutationImagePoints(Subset::subset((*imagePointsPerPose)[n], indexSet));
			permutationImagePointsSet.push_back(permutationImagePoints);
		}

		HomogenousMatrices4 candidateModels;
		NonconstArrayAccessor<HomogenousMatrix4> candidateModelsAccessor(candidateModels, views);
		Scalar projectionSqrError;
		if (MultipleViewGeometry::projectiveReconstructionFrom6PointsIF(ConstArrayAccessor<ImagePoints>(permutationImagePointsSet), candidateModelsAccessor.pointer(), squarePixelErrorThreshold, &projectionSqrError))
		{
			Scalar squareErrors = 0;
			Indices32 indices;
			indices.reserve(correspondences);

			const PinholeCamera identify(SquareMatrix3(true), 1, 1);

			Geometry::ObjectPoints points3d = Geometry::EpipolarGeometry::triangulateImagePointsIF(ConstArrayAccessor<HomogenousMatrix4>(candidateModels), *imagePointsPerPose);

			for (unsigned int n = 0u; n < correspondences; ++n)
			{
				Scalar sqrDistance(0);
				for (size_t iView = 0; iView < views; iView++)
				{
					const ImagePoint imagePoint = identify.projectToImageIF<true>(candidateModels[iView], points3d[n], false);
					sqrDistance += imagePoint.sqrDistance((*imagePointsPerPose)[iView][n]);
				}


				if (sqrDistance < Scalar(views) * squarePixelErrorThreshold)
				{
					squareErrors += sqrDistance;
					indices.push_back(n);
				}
			}

			const OptionalScopedLock scopedLock(lock);

			if (indices.size() > maxCountInliers || (indices.size() == maxCountInliers && squareErrors < *minSquareErrors))
			{
				maxCountInliers = indices.size();
				*minSquareErrors = squareErrors;
				for (size_t n = 0; n < posesIF->size(); ++n)
					(*posesIF)[n] = std::move(candidateModels[n]);

				if (usedIndices != nullptr)
				{
					*usedIndices = indices;
				}

				if (objectPointsIF != nullptr)
				{
					*objectPointsIF = NonconstArrayAccessor<ObjectPoint>(points3d, points3d.size());
				}
			}
		}
	}
}

}

}
