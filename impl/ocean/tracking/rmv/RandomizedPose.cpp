/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/rmv/RandomizedPose.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

HomogenousMatrices4 RandomizedPose::hemispherePoses(const Box3& box, RandomGenerator& randomGenerator, const Scalar distance, const unsigned int longitudeSteps, const unsigned int rollSteps)
{
	ocean_assert(box.isValid() && distance >= Numeric::eps());
	ocean_assert(longitudeSteps >= 1u && rollSteps >= 1u);

	const Vector3 center = box.center();
	const Scalar diagonal = box.diagonal();

	const Scalar diagonal_20 = diagonal * Scalar(0.05);

	const HomogenousMatrix4 wTcenter(center);
	const HomogenousMatrix4 centerTdistance(Vector3(0, distance, 0));
	const HomogenousMatrix4 distanceTlookat(Rotation(1, 0, 0, -Numeric::pi_2()));
	const HomogenousMatrix4 wTlookat(wTcenter * centerTdistance * distanceTlookat);

	const Scalar invRollStepsPI2 = Numeric::pi2() / Scalar(rollSteps);
	const Scalar invLongitudeStepsPI2 = Numeric::pi2() / Scalar(longitudeSteps);

	HomogenousMatrices4 results;
	results.reserve(longitudeSteps * rollSteps + rollSteps);

	// positions at north pole
	for (unsigned int n = 0u; n < rollSteps; ++n)
	{
		const Rotation rotationZ(0, 0, 1, invRollStepsPI2 * Scalar(n));
		const HomogenousMatrix4 randomOffset(Random::vector3(randomGenerator, Scalar(-diagonal_20), Scalar(diagonal_20)), Random::euler(randomGenerator, Numeric::deg2rad(5)));

		results.push_back(wTlookat * HomogenousMatrix4(rotationZ) * randomOffset);
	}

	// one ring with 40 degree offset to the north pole (latitude 60 degree)
	for (unsigned int n = 0u; n < longitudeSteps; ++n)
	{
		const HomogenousMatrix4 centerTcenterY(Rotation(0, 1, 0, invLongitudeStepsPI2 * Scalar(n)));
		const HomogenousMatrix4 centerYcenterX(Rotation(1, 0, 0, Numeric::deg2rad(40)));

		for (unsigned int rollStep = 0u; rollStep < rollSteps; ++rollStep)
		{
			const Rotation rotationZ(0, 0, 1, invRollStepsPI2 * Scalar(rollStep));

			const HomogenousMatrix4 wTlookatF(wTcenter * centerTcenterY * centerYcenterX * centerTdistance * distanceTlookat * HomogenousMatrix4(rotationZ));
			const HomogenousMatrix4 randomOffset(Random::vector3(randomGenerator, Scalar(-diagonal_20), Scalar(diagonal_20)), Random::euler(randomGenerator, Numeric::deg2rad(5)));

			results.push_back(wTlookatF * randomOffset);
		}
	}

	return results;
}

HomogenousMatrix4 RandomizedPose::constantDistance(const Box3& box, const Scalar distance, const Scalar minYFactor)
{
	ocean_assert(box.isValid());

	Quaternion rotation;

	// find a random rotation
	while (true)
	{
		const Vector3 axis(Random::scalar(-1, 1), Random::scalar(max(Scalar(0.00001), min(minYFactor, Scalar(1))), 1), Random::scalar(-1, 1));

		if (axis.sqr() > 1)
			continue;

		ocean_assert(axis.length() > Numeric::eps());

		rotation = Quaternion(Rotation(Vector3(0, 1, 0), axis.normalized())) * Quaternion(Vector3(0, 1, 0), Random::scalar(0, Numeric::pi2()));
		break;
	}

	const HomogenousMatrix4 transformation(HomogenousMatrix4(box.center()) * HomogenousMatrix4(rotation) * HomogenousMatrix4(Vector3(0, distance, 0)) * HomogenousMatrix4(Rotation(1, 0, 0, -Numeric::pi_2())));

	return transformation;
}

void RandomizedPose::constantDistance(const Box3& box, const Scalar distance, const Scalar minYFactor, const unsigned int number, const unsigned int maximalIterations, const Scalar angleOffset, HomogenousMatrix4* poses)
{
	ocean_assert(box.isValid());
	ocean_assert(poses);

	Quaternions rotations;

	while (rotations.size() < number)
	{
		bool foundNewRotation = false;

		Quaternion rotation;
		for (unsigned int n = 0; n < max(maximalIterations, 10u); ++n)
		{
			// find a random rotation
			while (true)
			{
				const Vector3 axis(Random::scalar(-1, 1), Random::scalar(max(Scalar(0.00001), min(minYFactor, Scalar(1))), 1), Random::scalar(-1, 1));

				if (axis.sqr() > 1)
					continue;

				ocean_assert(axis.length() > Numeric::eps());

				rotation = Quaternion(Rotation(Vector3(0, 1, 0), axis.normalized())) * Quaternion(Vector3(0, 1, 0), Random::scalar(0, Numeric::pi2()));
				break;
			}

			bool similarRotation = false;
			for (Quaternions::const_iterator i = rotations.begin(); i != rotations.end(); ++i)
				if (rotation.angle(*i) < angleOffset)
				{
					similarRotation = true;
					break;
				}

			if (!similarRotation)
			{
				foundNewRotation = true;
				break;
			}
		}

		if (!foundNewRotation)
			break;

		poses[rotations.size()] = HomogenousMatrix4(box.center()) * HomogenousMatrix4(rotation) * HomogenousMatrix4(Vector3(0, distance, 0)) * HomogenousMatrix4(Rotation(1, 0, 0, -Numeric::pi_2()));
		rotations.push_back(rotation);
	}
}

HomogenousMatrix4 RandomizedPose::randomPose(const PinholeCamera& pinholeCamera, const Box3& box, RandomGenerator& randomGenerator, const Scalar minDistance, const Scalar maxDistance, const Scalar visibleRatio)
{
	ocean_assert(pinholeCamera && box.isValid());
	ocean_assert(minDistance > Numeric::eps() && minDistance < maxDistance);
	ocean_assert(visibleRatio >= 0 && visibleRatio < 1);

	Vector3 mapPoints[8];
	const unsigned int numberMapPoints = box.corners(mapPoints);

	const Box2 cameraBox(Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()));

	const Scalar minVisibleRatio = Scalar(1) - visibleRatio;
	const Scalar maxVisibleRatio = Scalar(1) + visibleRatio;

	while (true)
	{
		const Vector3 translation(box.center() + Vector3(Random::scalar(randomGenerator, -maxDistance, maxDistance), Random::scalar(randomGenerator, minDistance, maxDistance), Random::scalar(randomGenerator, -maxDistance, maxDistance)));

		const Quaternion rotation(Random::rotation(randomGenerator));

		const HomogenousMatrix4 extrinsic(translation, rotation);
		const HomogenousMatrix4 iExtrinsic(extrinsic.inverted());
		const HomogenousMatrix4 ifExtrinsic(PinholeCamera::standard2InvertedFlipped(extrinsic));

		bool valid = true;
		for (unsigned int n = 0; n < numberMapPoints; ++n)
		{
			const Vector3 mapRay(iExtrinsic * mapPoints[n]);

			if (mapRay.z() > 0)
			{
				valid = false;
				break;
			}
		}

		if (!valid)
			continue;

		const Box2 projectedBox(pinholeCamera.projectToImageIF<true>(ifExtrinsic, box, false));
		ocean_assert(projectedBox.isValid());

		const Box2 intersectedBox(cameraBox.intersection(projectedBox));
		if (!intersectedBox.isValid())
			continue;

		if (intersectedBox.area() / cameraBox.area() < minVisibleRatio)
			continue;

		if (projectedBox.area() / cameraBox.area() > maxVisibleRatio)
			continue;

		return extrinsic;
	}
}

void RandomizedPose::randomPoses(const PinholeCamera& pinholeCamera, const Box3& box, RandomGenerator& randomGenerator, const Scalar minDistance, const Scalar maxDistance, const Scalar visibleRatio, const size_t number, HomogenousMatrix4* poses, Worker* worker)
{
	ocean_assert(pinholeCamera.isValid() && box.isValid());
	ocean_assert(minDistance > Numeric::eps() && minDistance < maxDistance);
	ocean_assert(visibleRatio >= 0 && visibleRatio < 1);
	ocean_assert(number >= 1 && poses);

	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&RandomizedPose::randomPoseSubset, &pinholeCamera, &box, &randomGenerator, minDistance, maxDistance, visibleRatio, poses, 0u, 0u), 0u, (unsigned int)number, 7u, 8u, 20u);
	else
		randomPoseSubset(&pinholeCamera, &box, &randomGenerator, minDistance, maxDistance, visibleRatio, poses, 0u, (unsigned int)number);
}

void RandomizedPose::randomPoseSubset(const PinholeCamera* pinholeCamera, const Box3* box, RandomGenerator* randomGenerator, const Scalar minDistance, const Scalar maxDistance, const Scalar visibleRatio, HomogenousMatrix4* poses, const unsigned int firstPose, const unsigned int numberPoses)
{
	ocean_assert(pinholeCamera && box && poses && randomGenerator);

	// Initializes the local random generator by the global generator
	RandomGenerator localRandomGenerator(*randomGenerator);

	for (unsigned int n = firstPose; n < firstPose + numberPoses; ++n)
		poses[n] = randomPose(*pinholeCamera, *box, localRandomGenerator, minDistance, maxDistance, visibleRatio);
}

}

}

}
