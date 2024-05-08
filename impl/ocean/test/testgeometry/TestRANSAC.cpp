/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestRANSAC.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/Timestamp.h"

#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestRANSAC::test(const double testDuration, Worker* /*worker*/)
{
	Log::info() << "---   RANSAC test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testObjectTransformationStereoAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testObjectTransformationStereo(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "RANSAC test succeeded.";
	}
	else
	{
		Log::info() << "RANSAC test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestRANSAC, ObjectTransformationStereoAnyCamera)
{
	EXPECT_TRUE(TestRANSAC::testObjectTransformationStereoAnyCamera(GTEST_TEST_DURATION));
}

TEST(TestRANSAC, ObjectTransformationStereo)
{
	EXPECT_TRUE(TestRANSAC::testObjectTransformationStereo(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestRANSAC::testObjectTransformationStereoAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Determination of 6-DOF object transformation for any stereo camera:";
	Log::info() << " ";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCameraA = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		const std::shared_ptr<AnyCamera> anyCameraB = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCameraA && anyCameraB);

		Log::info() << " ";
		Log::info() << "Camera name: " << anyCameraA->name();

		ocean_assert(testDuration > 0.0);

		const double faultyRate = 0.15; // 15%

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			const HomogenousMatrix4 world_T_object(Random::vector3(randomGenerator, -5, 5), Random::quaternion(randomGenerator));
			const HomogenousMatrix4 object_T_world = world_T_object.inverted();

			std::vector<Vectors3> objectPointGroups(2);
			std::vector<Vectors2> imagePointGroups(2);
			HomogenousMatrices4 world_T_cameras(2);

			std::vector<IndexSet32> faultyCorrespondenceGroups(2);

			for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
			{
				Vectors3& objectPoints = objectPointGroups[nCamera];
				Vectors2& imagePoints = imagePointGroups[nCamera];
				HomogenousMatrix4& world_T_camera = world_T_cameras[nCamera];
				IndexSet32& faultyCorrespondences = faultyCorrespondenceGroups[nCamera];

				const AnyCamera& anyCamera = nCamera == 0u ? *anyCameraA : *anyCameraB;

				world_T_camera = HomogenousMatrix4(Random::vector3(randomGenerator, -5, 5), Random::quaternion(randomGenerator));

				const unsigned int correspondences = RandomI::random(randomGenerator, 20u, 200u);

				objectPoints.reserve(correspondences);
				imagePoints.reserve(correspondences);

				for (unsigned int nCorrespondence = 0u; nCorrespondence < correspondences; ++nCorrespondence)
				{
					const Vector2 imagePoint = Random::vector2(randomGenerator, Scalar(10), Scalar(anyCamera.width() - 10u), Scalar(10), Scalar(anyCamera.height() - 10u));

					const Vector3 objectPoint = object_T_world * anyCamera.ray(imagePoint, world_T_camera).point(Random::scalar(randomGenerator, 1, 5));
					ocean_assert(anyCamera.projectToImage(world_T_camera, world_T_object * objectPoint).isEqual(imagePoint, 1));

					objectPoints.push_back(objectPoint);
					imagePoints.push_back(imagePoint);
				}

				const unsigned int numberFaultyCorrespondences = (unsigned int)(double(correspondences) * faultyRate + 0.5); // result can be zero

				while (faultyCorrespondences.size() < numberFaultyCorrespondences)
				{
					const unsigned int faultyIndex = RandomI::random(randomGenerator, correspondences - 1u);

					imagePoints[faultyIndex] = Random::vector2(randomGenerator, Scalar(10), Scalar(anyCamera.width() - 10u), Scalar(10), Scalar(anyCamera.height() - 10u));

					faultyCorrespondences.insert(faultyIndex);
				}
			}

			const unsigned int ransacIterations = Geometry::RANSAC::iterations(6u, Scalar(0.995), Scalar(faultyRate));

			HomogenousMatrix4 ransac_world_T_object;

			Indices32 usedIndicesA;
			Indices32 usedIndicesB;
			Scalar sqrAccuracy = Numeric::maxValue();

			if (Geometry::RANSAC::objectTransformationStereo(*anyCameraA, *anyCameraB, world_T_cameras[0], world_T_cameras[1], ConstArrayAccessor<Vector3>(objectPointGroups[0]), ConstArrayAccessor<Vector3>(objectPointGroups[1]), ConstArrayAccessor<Vector2>(imagePointGroups[0]), ConstArrayAccessor<Vector2>(imagePointGroups[1]), randomGenerator, ransac_world_T_object, 5u, true, ransacIterations, Scalar(5 * 5), &usedIndicesA, &usedIndicesB, &sqrAccuracy))
			{
				bool iterationIsValid = sqrAccuracy <= Scalar(5 * 5);

				for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
				{
					const Vectors3& objectPoints = objectPointGroups[nCamera];
					const IndexSet32& faultyCorrespondences = faultyCorrespondenceGroups[nCamera];

					const Indices32& usedIndices = nCamera == 0u ? usedIndicesA : usedIndicesB;

					const std::vector<uint8_t> faultyStatements = Subset::indices2statements<Index32, 1u>(faultyCorrespondences, objectPoints.size());
					const IndexSet32 usedIndexSet = IndexSet32(usedIndices.cbegin(), usedIndices.cend());

					ocean_assert(faultyCorrespondences.size() < objectPoints.size());
					if (usedIndices.size() < objectPoints.size() - faultyCorrespondences.size())
					{
						iterationIsValid = false;
					}
					else
					{
						for (size_t n = 0; n < faultyStatements.size(); ++n)
						{
							if (faultyStatements[n] == 0u)
							{
								// the correspondences is not faulty

								if (usedIndexSet.find(Index32(n)) == usedIndexSet.cend())
								{
									// although the correspondence is not faulty, it hasn't been used
									iterationIsValid = false;
								}
							}
						}
					}
				}

				if (iterationIsValid)
				{
					++validIterations;
				}
			}

			++iterations;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		const double threshold = std::is_same<Scalar, double>::value ? 0.99 : 0.95;

		if (percent < threshold)
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestRANSAC::testObjectTransformationStereo(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Determination of 6-DOF object transformation for stereo fisheye camera:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	const FisheyeCamera fisheyeCameraA = Utilities::realisticFisheyeCamera(0u);
	const FisheyeCamera fisheyeCameraB = Utilities::realisticFisheyeCamera(1u);

	ocean_assert(fisheyeCameraA.isValid() && fisheyeCameraB.isValid());
	ocean_assert(testDuration > 0.0);

	const double faultyRate = 0.15; // 15%

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		const HomogenousMatrix4 world_T_object(Random::vector3(randomGenerator, -5, 5), Random::quaternion(randomGenerator));
		const HomogenousMatrix4 object_T_world = world_T_object.inverted();

		std::vector<Vectors3> objectPointGroups(2);
		std::vector<Vectors2> imagePointGroups(2);
		HomogenousMatrices4 world_T_cameras(2);

		std::vector<IndexSet32> faultyCorrespondenceGroups(2);

		for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
		{
			Vectors3& objectPoints = objectPointGroups[nCamera];
			Vectors2& imagePoints = imagePointGroups[nCamera];
			HomogenousMatrix4& world_T_camera = world_T_cameras[nCamera];
			IndexSet32& faultyCorrespondences = faultyCorrespondenceGroups[nCamera];

			const FisheyeCamera& fisheyeCamera = nCamera == 0u ? fisheyeCameraA : fisheyeCameraB;

			world_T_camera = HomogenousMatrix4(Random::vector3(randomGenerator, -5, 5), Random::quaternion(randomGenerator));

			const unsigned int correspondences = RandomI::random(randomGenerator, 20u, 200u);

			objectPoints.reserve(correspondences);
			imagePoints.reserve(correspondences);

			for (unsigned int nCorrespondence = 0u; nCorrespondence < correspondences; ++nCorrespondence)
			{
				const Vector2 imagePoint = Random::vector2(randomGenerator, Scalar(10), Scalar(fisheyeCamera.width() - 10u), Scalar(10), Scalar(fisheyeCamera.height() - 10u));

				const Vector3 objectPoint = object_T_world * fisheyeCamera.ray(imagePoint, world_T_camera).point(Random::scalar(randomGenerator, 1, 5));
				ocean_assert(fisheyeCamera.projectToImage(world_T_camera, world_T_object * objectPoint).isEqual(imagePoint, 1));

				objectPoints.push_back(objectPoint);
				imagePoints.push_back(imagePoint);
			}

			const unsigned int numberFaultyCorrespondences = (unsigned int)(double(correspondences) * faultyRate + 0.5); // result can be zero

			while (faultyCorrespondences.size() < numberFaultyCorrespondences)
			{
				const unsigned int faultyIndex = RandomI::random(randomGenerator, correspondences - 1u);

				imagePoints[faultyIndex] = Random::vector2(randomGenerator, Scalar(10), Scalar(fisheyeCamera.width() - 10u), Scalar(10), Scalar(fisheyeCamera.height() - 10u));

				faultyCorrespondences.insert(faultyIndex);
			}
		}

		const unsigned int ransacIterations = Geometry::RANSAC::iterations(6u, Scalar(0.995), Scalar(faultyRate));

		HomogenousMatrix4 ransac_world_T_object;

		Indices32 usedIndicesA;
		Indices32 usedIndicesB;
		Scalar sqrAccuracy = Numeric::maxValue();

		if (Geometry::RANSAC::objectTransformationStereo(fisheyeCameraA, fisheyeCameraB, world_T_cameras[0], world_T_cameras[1], ConstArrayAccessor<Vector3>(objectPointGroups[0]), ConstArrayAccessor<Vector3>(objectPointGroups[1]), ConstArrayAccessor<Vector2>(imagePointGroups[0]), ConstArrayAccessor<Vector2>(imagePointGroups[1]), randomGenerator, ransac_world_T_object, 5u, true, ransacIterations, Scalar(5 * 5), &usedIndicesA, &usedIndicesB, &sqrAccuracy))
		{
			bool iterationIsValid = sqrAccuracy <= Scalar(5 * 5);

			for (unsigned int nCamera = 0u; nCamera < 2u; ++nCamera)
			{
				const Vectors3& objectPoints = objectPointGroups[nCamera];
				const IndexSet32& faultyCorrespondences = faultyCorrespondenceGroups[nCamera];

				const Indices32& usedIndices = nCamera == 0u ? usedIndicesA : usedIndicesB;

				const std::vector<uint8_t> faultyStatements = Subset::indices2statements<Index32, 1u>(faultyCorrespondences, objectPoints.size());
				const IndexSet32 usedIndexSet = IndexSet32(usedIndices.cbegin(), usedIndices.cend());

				ocean_assert(faultyCorrespondences.size() < objectPoints.size());
				if (usedIndices.size() < objectPoints.size() - faultyCorrespondences.size())
				{
					iterationIsValid = false;
				}
				else
				{
					for (size_t n = 0; n < faultyStatements.size(); ++n)
					{
						if (faultyStatements[n] == 0u)
						{
							// the correspondences is not faulty

							if (usedIndexSet.find(Index32(n)) == usedIndexSet.cend())
							{
								// although the correspondence is not faulty, it hasn't been used
								iterationIsValid = false;
							}
						}
					}
				}
			}

			if (iterationIsValid)
			{
				++validIterations;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const double threshold = std::is_same<Scalar, double>::value ? 0.99 : 0.95;

	return percent >= threshold;
}

}

}

}
