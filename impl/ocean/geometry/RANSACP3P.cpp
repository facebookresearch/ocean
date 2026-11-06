/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/P3P.h"

#include "ocean/math/Random.h"

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

bool RANSAC::p3p(const AnyCamera& anyCamera, const ConstIndexedAccessor<Vector3>& objectPointAccessor, const ConstIndexedAccessor<Vector2>& imagePointAccessor, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const GravityConstraints* gravityConstraints)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(minimalValidCorrespondences >= 4u);
	ocean_assert(objectPointAccessor.size() >= 4);
	ocean_assert(objectPointAccessor.size() == imagePointAccessor.size());
	ocean_assert(objectPointAccessor.size() >= minimalValidCorrespondences);
	ocean_assert(iterations >= 1u);
	ocean_assert(gravityConstraints == nullptr || (gravityConstraints->isValid() && gravityConstraints->numberCameras() == 1));

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

	unsigned int adaptiveIterations = iterations;

	for (unsigned int i = 0u; i < adaptiveIterations; ++i)
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
			const HomogenousMatrix4& world_T_candidateCamera = world_T_candidateCameras[n];

			if (gravityConstraints != nullptr)
			{
				if (!gravityConstraints->isCameraAlignedWithGravity(world_T_candidateCamera))
				{
					continue;
				}
			}

			indices.clear();

			Scalar sqrErrors = 0;

			const HomogenousMatrix4 flippedCandidateCamera_T_world(Camera::standard2InvertedFlipped(world_T_candidateCamera));

			ocean_assert(Camera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[index0]));
			ocean_assert(Camera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[index1]));
			ocean_assert(Camera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[index2]));

			// now we test each 2D/3D point correspondences and check whether the accuracy of the pose is good enough, we can stop if we cannot reach a better configuration than we have already
			for (unsigned int c = 0u; indices.size() + (correspondences - c) >= bestIndices.size() && c < correspondences; ++c)
			{
				// we accept only object points lying in front of the camera
				if (Camera::isObjectPointInFrontIF(flippedCandidateCamera_T_world, objectPoints[c]))
				{
					const Vector2 projectedImagePoint(anyCamera.projectToImageIF(flippedCandidateCamera_T_world, objectPoints[c]));
					const Vector2& imagePoint = imagePoints[c];

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

					adaptiveIterations = minmax(minimalAdaptiveIterations, expectedIterationsForFoundCorrespondences, adaptiveIterations);
				}
			}
		}
	}

	if (bestIndices.empty() || bestIndices.size() < minimalValidCorrespondences)
	{
		return false;
	}

	world_T_camera = world_T_bestCamera;

	if (sqrAccuracy != nullptr)
	{
		*sqrAccuracy = bestSqrErrors / Scalar(bestIndices.size());
	}

	// non linear least square refinement step
	if (refine)
	{
		const size_t bestIndicesUsedForOptimization = bestIndices.size();

		if (!NonLinearOptimizationPose::optimizePose(anyCamera, world_T_bestCamera, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), world_T_camera, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy, nullptr, gravityConstraints))
		{
			return false;
		}

		// check whether we need to determine the indices for the optimized pose followed by another final optimization step
		if (usedIndices != nullptr && bestIndices.size() != correspondences)
		{
			const HomogenousMatrix4 flippedCamera_T_world(Camera::standard2InvertedFlipped(world_T_camera));

			bestIndices.clear();
			for (unsigned int c = 0; c < correspondences; ++c)
			{
				// we accept only object points lying in front of the camera
				if (Camera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoints[c]))
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

				if (!NonLinearOptimizationPose::optimizePose(anyCamera, world_T_bestCamera, ConstArraySubsetAccessor<Vector3, unsigned int>(objectPoints.data(), bestIndices), ConstArraySubsetAccessor<Vector2, unsigned int>(imagePoints.data(), bestIndices), world_T_camera, 20u, Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), nullptr, sqrAccuracy, nullptr, gravityConstraints))
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

}

}
