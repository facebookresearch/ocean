/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestUnidirectionalCorrespondences.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

#include "ocean/tracking/UnidirectionalCorrespondences.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

bool TestUnidirectionalCorrespondences::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   UnidirectionalCorrespondences test:   ---";
	Log::info() << " ";

	allSucceeded = testCountBijectiveCorrespondences(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRemoveNonBijectiveCorrespondences(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "UnidirectionalCorrespondences test succeeded.";
	}
	else
	{
		Log::info() << "UnidirectionalCorrespondences test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestUnidirectionalCorrespondences, CountBijectiveCorrespondences)
{
	EXPECT_TRUE(TestUnidirectionalCorrespondences::testCountBijectiveCorrespondences(GTEST_TEST_DURATION));
}

TEST(TestUnidirectionalCorrespondences, RemoveNonBijectiveCorrespondences)
{
	EXPECT_TRUE(TestUnidirectionalCorrespondences::testRemoveNonBijectiveCorrespondences(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestUnidirectionalCorrespondences::testCountBijectiveCorrespondences(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Count bijective correspondences test:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberIndices = RandomI::random(randomGenerator, 1, 1000);
		const unsigned int maximalValue = RandomI::random(randomGenerator, 999u);

		Indices32 indices;

		for (size_t n = 0; n < numberIndices; ++n)
		{
			indices.push_back(RandomI::random(randomGenerator, maximalValue));
		}

		IndexCounterMap indexCounterMap;

		for (const Index32& index : indices)
		{
			indexCounterMap[index]++;
		}

		size_t bijectiveCorrespondences = 0;

		for (const IndexCounterMap::value_type& pair : indexCounterMap)
		{
			if (pair.second == 1)
			{
				++bijectiveCorrespondences;
			}
		}

		const size_t testBijectiveCorrespondences = Tracking::UnidirectionalCorrespondences::countBijectiveCorrespondences(indices.data(), indices.size());

		OCEAN_EXPECT_EQUAL(validation, bijectiveCorrespondences, testBijectiveCorrespondences);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUnidirectionalCorrespondences::testRemoveNonBijectiveCorrespondences(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Remove non-bijective correspondences test:";

	const AnyCameraPinhole camera(PinholeCamera(1000u, 1000u, Numeric::deg2rad(60)));

	const HomogenousMatrix4 world_T_camera(true);
	const HomogenousMatrix4 flippedCamera_T_world(Camera::standard2InvertedFlipped(world_T_camera));

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool checkImagePoints : {false, true})
		{
			const size_t numberObjectPoints = RandomI::random(randomGenerator, 1, 1000);
			const size_t numberImagePoints = RandomI::random(randomGenerator, 1, 1000);

			Vectors3 objectPoints;

			for (size_t n = 0; n < numberObjectPoints; ++n)
			{
				const Vector2 randomImagePoint = Random::vector2(randomGenerator, Scalar(0), Scalar(camera.width()), Scalar(0), Scalar(camera.height()));

				objectPoints.emplace_back(camera.ray(randomImagePoint).point(Random::scalar(randomGenerator, Scalar(0.1), Scalar(10))));
			}

			Vectors2 imagePoints;

			for (size_t n = 0; n < numberImagePoints; ++n)
			{
				imagePoints.emplace_back(Random::vector2(randomGenerator, Scalar(0), Scalar(camera.width()), Scalar(0), Scalar(camera.height())));

			}

			const size_t numberCorrespondences = RandomI::random(randomGenerator, 1u, (unsigned int)(std::min(objectPoints.size(), imagePoints.size())));

			Indices32 usedObjectPointIndices;
			Indices32 usedImagePointIndices;

			if (checkImagePoints)
			{
				Indices32 availableObjectPointIndices = createIndices(objectPoints.size(), 0u);

				for (size_t n = 0; n < numberCorrespondences; ++n)
				{
					const size_t index = RandomI::random(randomGenerator, (unsigned int)(availableObjectPointIndices.size()) - 1u);

					const Index32 objectPointIndex = availableObjectPointIndices[index];

					usedObjectPointIndices.push_back(objectPointIndex);

					availableObjectPointIndices[index] = availableObjectPointIndices.back();
					availableObjectPointIndices.pop_back();
				}

				OCEAN_EXPECT_EQUAL(validation, UnorderedIndexSet32(usedObjectPointIndices.cbegin(), usedObjectPointIndices.cend()).size(), usedObjectPointIndices.size()); // each object point index is used only once

				for (size_t n = 0; n < numberCorrespondences; ++n)
				{
					const Index32 imagePointIndex = RandomI::random(randomGenerator, (unsigned int)(imagePoints.size()) - 1u);

					usedImagePointIndices.push_back(imagePointIndex);
				}

				IndexConnectionMap indexConnectionMap;

				for (size_t nCorrespondence = 0; nCorrespondence < numberCorrespondences; ++nCorrespondence)
				{
					const Index32 objectPointIndex = usedObjectPointIndices[nCorrespondence];
					const Index32 imagePointIndex = usedImagePointIndices[nCorrespondence];

					indexConnectionMap[imagePointIndex].push_back(objectPointIndex);
				}

				IndexPairSet validCorrespondenceSet;

				for (const IndexConnectionMap::value_type& connectionPair : indexConnectionMap)
				{
					const Index32 imagePointIndex = connectionPair.first;
					const Indices32& objectPointIndices = connectionPair.second;

					ocean_assert(objectPointIndices.size() >= 1);

					IndexPair32 validCorrespondence(Index32(-1), Index32(-1));

					if (objectPointIndices.size() == 1)
					{
						validCorrespondence = IndexPair32(imagePointIndex, objectPointIndices.front());
					}
					else
					{
						const Vector2& imagePoint = imagePoints[imagePointIndex];

						Index32 bestObjectPointIndex = Index32(-1);
						Scalar bestSqrDistance = Numeric::maxValue();

						for (const Index32& objectPointIndex : objectPointIndices)
						{
							const Vector3& objectPoint = objectPoints[objectPointIndex];

							const Scalar sqrDistance = imagePoint.sqrDistance(camera.projectToImageIF(flippedCamera_T_world, objectPoint));

							if (sqrDistance < bestSqrDistance)
							{
								bestSqrDistance = sqrDistance;
								bestObjectPointIndex = objectPointIndex;
							}
						}

						ocean_assert(bestObjectPointIndex != Index32(-1));

						validCorrespondence = IndexPair32(imagePointIndex, bestObjectPointIndex);
					}

					ocean_assert(!validCorrespondenceSet.contains(validCorrespondence));
					validCorrespondenceSet.insert(validCorrespondence);
				}

#ifdef OCEAN_DEBUG
				{
					UnorderedIndexSet32 debugUsedObjectPointIndices;
					UnorderedIndexSet32 debugUsedImagePointIndices;

					for (const IndexPair32& validCorrespondence : validCorrespondenceSet)
					{
						debugUsedObjectPointIndices.emplace(validCorrespondence.second);
						debugUsedImagePointIndices.emplace(validCorrespondence.first);
					}

					ocean_assert(debugUsedObjectPointIndices.size() == validCorrespondenceSet.size());
					ocean_assert(debugUsedImagePointIndices.size() == validCorrespondenceSet.size());

					ocean_assert(Tracking::UnidirectionalCorrespondences::countBijectiveCorrespondences(usedObjectPointIndices.data(), usedObjectPointIndices.size()) == usedObjectPointIndices.size());

					const size_t debugBijectiveCorrespondences = Tracking::UnidirectionalCorrespondences::countBijectiveCorrespondences(usedImagePointIndices.data(), usedImagePointIndices.size());
					const size_t debugNonBijectiveCorrespondences = Tracking::UnidirectionalCorrespondences::countNonBijectiveCorrespondences(usedImagePointIndices.data(), usedImagePointIndices.size());

					ocean_assert(debugBijectiveCorrespondences + debugNonBijectiveCorrespondences == validCorrespondenceSet.size());
				}
#endif // OCEAN_DEBUG

				Tracking::UnidirectionalCorrespondences::removeNonBijectiveCorrespondences(camera, world_T_camera, objectPoints.data(), imagePoints.data(), usedObjectPointIndices, usedImagePointIndices, checkImagePoints);

				OCEAN_EXPECT_EQUAL(validation, usedObjectPointIndices.size(), usedImagePointIndices.size());

				IndexPairSet testCorrespondenceSet;

				for (size_t n = 0; n < usedObjectPointIndices.size(); ++n)
				{
					const Index32 objectPointIndex = usedObjectPointIndices[n];
					const Index32 imagePointIndex = usedImagePointIndices[n];

					testCorrespondenceSet.emplace(imagePointIndex, objectPointIndex);
				}

				OCEAN_EXPECT_EQUAL(validation, validCorrespondenceSet.size(), testCorrespondenceSet.size());

				for (const IndexPair32& validCorrespondence : validCorrespondenceSet)
				{
					OCEAN_EXPECT_TRUE(validation, testCorrespondenceSet.contains(validCorrespondence));
				}
			}
			else
			{
				Indices32 availableImagePointIndices = createIndices(imagePoints.size(), 0u);

				for (size_t n = 0; n < numberCorrespondences; ++n)
				{
					const size_t index = RandomI::random(randomGenerator, (unsigned int)(availableImagePointIndices.size()) - 1u);

					const Index32 imagePointIndex = availableImagePointIndices[index];

					usedImagePointIndices.push_back(imagePointIndex);

					availableImagePointIndices[index] = availableImagePointIndices.back();
					availableImagePointIndices.pop_back();
				}

				OCEAN_EXPECT_EQUAL(validation, UnorderedIndexSet32(usedImagePointIndices.cbegin(), usedImagePointIndices.cend()).size(), usedImagePointIndices.size()); // each image point index is used only once

				for (size_t n = 0; n < numberCorrespondences; ++n)
				{
					const Index32 objectPointIndex = RandomI::random(randomGenerator, (unsigned int)(objectPoints.size()) - 1u);

					usedObjectPointIndices.push_back(objectPointIndex);
				}

				IndexConnectionMap indexConnectionMap;

				for (size_t nCorrespondence = 0; nCorrespondence < numberCorrespondences; ++nCorrespondence)
				{
					const Index32 objectPointIndex = usedObjectPointIndices[nCorrespondence];
					const Index32 imagePointIndex = usedImagePointIndices[nCorrespondence];

					indexConnectionMap[objectPointIndex].push_back(imagePointIndex);
				}

				IndexPairSet validCorrespondenceSet;

				for (const IndexConnectionMap::value_type& connectionPair : indexConnectionMap)
				{
					const Index32 objectPointIndex = connectionPair.first;
					const Indices32& imagePointIndices = connectionPair.second;

					ocean_assert(imagePointIndices.size() >= 1);

					IndexPair32 validCorrespondence(Index32(-1), Index32(-1));

					if (imagePointIndices.size() == 1)
					{
						validCorrespondence = IndexPair32(objectPointIndex, imagePointIndices.front());
					}
					else
					{
						const Vector3& objectPoint = objectPoints[objectPointIndex];

						Index32 bestImagePointIndex = Index32(-1);
						Scalar bestSqrDistance = Numeric::maxValue();

						for (const Index32& imagePointIndex : imagePointIndices)
						{
							const Vector2& imagePoint = imagePoints[imagePointIndex];

							const Scalar sqrDistance = imagePoint.sqrDistance(camera.projectToImageIF(flippedCamera_T_world, objectPoint));

							if (sqrDistance < bestSqrDistance)
							{
								bestSqrDistance = sqrDistance;
								bestImagePointIndex = imagePointIndex;
							}
						}

						ocean_assert(bestImagePointIndex != Index32(-1));

						validCorrespondence = IndexPair32(objectPointIndex, bestImagePointIndex);
					}

					ocean_assert(!validCorrespondenceSet.contains(validCorrespondence));
					validCorrespondenceSet.insert(validCorrespondence);
				}

#ifdef OCEAN_DEBUG
				{
					UnorderedIndexSet32 debugUsedObjectPointIndices;
					UnorderedIndexSet32 debugUsedImagePointIndices;

					for (const IndexPair32& validCorrespondence : validCorrespondenceSet)
					{
						debugUsedObjectPointIndices.emplace(validCorrespondence.first);
						debugUsedImagePointIndices.emplace(validCorrespondence.second);
					}

					ocean_assert(debugUsedObjectPointIndices.size() == validCorrespondenceSet.size());
					ocean_assert(debugUsedImagePointIndices.size() == validCorrespondenceSet.size());

					ocean_assert(Tracking::UnidirectionalCorrespondences::countBijectiveCorrespondences(usedImagePointIndices.data(), usedImagePointIndices.size()) == usedImagePointIndices.size());

					const size_t debugBijectiveCorrespondences = Tracking::UnidirectionalCorrespondences::countBijectiveCorrespondences(usedObjectPointIndices.data(), usedObjectPointIndices.size());
					const size_t debugNonBijectiveCorrespondences = Tracking::UnidirectionalCorrespondences::countNonBijectiveCorrespondences(usedObjectPointIndices.data(), usedObjectPointIndices.size());

					ocean_assert(debugBijectiveCorrespondences + debugNonBijectiveCorrespondences == validCorrespondenceSet.size());
				}
#endif // OCEAN_DEBUG

				Tracking::UnidirectionalCorrespondences::removeNonBijectiveCorrespondences(camera, world_T_camera, objectPoints.data(), imagePoints.data(), usedObjectPointIndices, usedImagePointIndices, checkImagePoints);

				OCEAN_EXPECT_EQUAL(validation, usedObjectPointIndices.size(), usedImagePointIndices.size());

				IndexPairSet testCorrespondenceSet;

				for (size_t n = 0; n < usedObjectPointIndices.size(); ++n)
				{
					const Index32 objectPointIndex = usedObjectPointIndices[n];
					const Index32 imagePointIndex = usedImagePointIndices[n];

					testCorrespondenceSet.emplace(objectPointIndex, imagePointIndex);
				}

				OCEAN_EXPECT_EQUAL(validation, validCorrespondenceSet.size(), testCorrespondenceSet.size());

				for (const IndexPair32& validCorrespondence : validCorrespondenceSet)
				{
					OCEAN_EXPECT_TRUE(validation, testCorrespondenceSet.contains(validCorrespondence));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
