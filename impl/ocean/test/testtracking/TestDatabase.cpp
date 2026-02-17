/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestDatabase.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/RandomI.h"

#include "ocean/tracking/Utilities.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

bool TestDatabase::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Database test");
	Log::info() << " ";

	if (selector.shouldRun("addobjectpointfromdatabase"))
	{
		testResult = testAddObjectPointFromDatabase(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("serialization"))
	{
		testResult = testSerialization(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestDatabase, AddObjectPointFromDatabase)
{
	EXPECT_TRUE(TestDatabase::testAddObjectPointFromDatabase(GTEST_TEST_DURATION));
}

TEST(TestDatabase, Serialization)
{
	EXPECT_TRUE(TestDatabase::testSerialization(GTEST_TEST_DURATION));
}


#endif // OCEAN_USE_GTEST

bool TestDatabase::testAddObjectPointFromDatabase(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test addObjectPointFromDatabase:";

	const static unsigned int maxNumberPoses = 100u;
	const static unsigned int maxNumberObjectPoints = 100u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// our first random database
		const unsigned int firstDatabaseNumberPoses = RandomI::random(randomGenerator, 1u, maxNumberPoses);
		const unsigned int firstDatabaseNumberObjectPoints = RandomI::random(randomGenerator, 1u, maxNumberObjectPoints);
		const Tracking::Database firstDatabaseCopy(createDatabaseWithRandomTopology(randomGenerator, 0u, maxNumberPoses - 1u, firstDatabaseNumberPoses, firstDatabaseNumberObjectPoints, 0u, firstDatabaseNumberObjectPoints));

		const Indices32 firstDatabaseCopyPoseIds = firstDatabaseCopy.poseIds<false>();
		const HomogenousMatrices4 firstDatabaseCopyPoses = firstDatabaseCopy.poses<false>(firstDatabaseCopyPoseIds.data(), firstDatabaseCopyPoseIds.size());

		const Indices32 firstDatabaseCopyObjectPointIds = firstDatabaseCopy.objectPointIds<false>();
		const Vectors3 firstDatabaseCopyObjectPoints = firstDatabaseCopy.objectPoints<false>(firstDatabaseCopyObjectPointIds);

		const Indices32 firstDatabaseCopyImagePointIds = firstDatabaseCopy.imagePointIds<false>();
		const Vectors2 firstDatabaseCopyImagePoints = firstDatabaseCopy.imagePoints<false>(firstDatabaseCopyImagePointIds);

		// our second random database
		const unsigned int secondDatabaseNumberPoses = RandomI::random(randomGenerator, 1u, maxNumberPoses);
		const unsigned int secondDatabaseNumberObjectPoints = RandomI::random(randomGenerator, 1u, maxNumberObjectPoints);
		const Tracking::Database secondDatabase(createDatabaseWithRandomTopology(randomGenerator, 0u, maxNumberPoses - 1u, secondDatabaseNumberPoses, secondDatabaseNumberObjectPoints, 0u, secondDatabaseNumberObjectPoints));


		// now we copy entire tracks (object points with connected information)

		Tracking::Database firstDatabase(firstDatabaseCopy);

		const Indices32 secondDatabaseObjectPointIds(secondDatabase.objectPointIds<false>());

		const unsigned int numberObjectPointsToAdd = RandomI::random(randomGenerator, 1u, secondDatabaseNumberObjectPoints);

		// simple translation by (100, 100)
		const SquareMatrix3 transformation(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(100, 100, 1));

		Index32 lowerPoseId = Tracking::Database::invalidId;
		Index32 upperPoseId = Tracking::Database::invalidId;

		if (RandomI::boolean(randomGenerator))
		{
			lowerPoseId = RandomI::random(randomGenerator, (unsigned int)secondDatabase.poseNumber<false>() + 10u);
		}

		if (RandomI::boolean(randomGenerator))
		{
			if (lowerPoseId == Tracking::Database::invalidId)
			{
				upperPoseId = RandomI::random(randomGenerator, (unsigned int)secondDatabase.poseNumber<false>() + 10u);
			}
			else
			{
				upperPoseId = RandomI::random(randomGenerator, lowerPoseId, (unsigned int)secondDatabase.poseNumber<false>() + 20u);
			}
		}

		ocean_assert(lowerPoseId == Tracking::Database::invalidId || upperPoseId == Tracking::Database::invalidId || lowerPoseId <= upperPoseId);

		const bool forExistingPosesOnly = RandomI::boolean(randomGenerator);

		IndexSet32 addedObjectPointIds;

		for (unsigned int n = 0u; n < numberObjectPointsToAdd; ++n)
		{
			Index32 secondDatabaseObjectPointId = secondDatabaseObjectPointIds[RandomI::random(randomGenerator, 0u, (unsigned int)(secondDatabaseObjectPointIds.size() - 1))];
			while (addedObjectPointIds.find(secondDatabaseObjectPointId) != addedObjectPointIds.end())
			{
				secondDatabaseObjectPointId = secondDatabaseObjectPointIds[RandomI::random(randomGenerator, 0u, (unsigned int)(secondDatabaseObjectPointIds.size() - 1))];
			}

			const Index32 newFirstDatabaseObjectPointId = firstDatabase.addObjectPointFromDatabase(secondDatabase, secondDatabaseObjectPointId, transformation, Tracking::Database::invalidId, lowerPoseId, upperPoseId, forExistingPosesOnly);

			// first we need to check that original data hasn't been modified

			for (unsigned int i = 0u; validation.succeededSoFar() && i < firstDatabaseCopyPoseIds.size(); ++i)
			{
				const Index32 poseId = firstDatabaseCopyPoseIds[i];

				// the pose must still exist
				if (!firstDatabase.hasPose<false>(poseId))
				{
					OCEAN_SET_FAILED(validation);
					break;
				}

				// the pose value must be identical
				const HomogenousMatrix4& pose = firstDatabase.pose<false>(poseId);
				if (pose != firstDatabaseCopyPoses[i])
				{
					OCEAN_SET_FAILED(validation);
					break;
				}

				// all attached image points must still be attached
				const IndexSet32 copyAttachedImagePointIds(firstDatabaseCopy.imagePointsFromPose<false>(poseId));
				IndexSet32 newAttachedImagePointIds(firstDatabase.imagePointsFromPose<false>(poseId));

				newAttachedImagePointIds.insert(copyAttachedImagePointIds.cbegin(), copyAttachedImagePointIds.cend());

				if (newAttachedImagePointIds != firstDatabase.imagePointsFromPose<false>(poseId))
				{
					OCEAN_SET_FAILED(validation);
					break;
				}

				// all attached object points must still be attached
				const Indices32 copyAttachedObjectPointIds(firstDatabaseCopy.objectPointIds<false>(poseId));
				const Indices32 newAttachedObjectPointIds(firstDatabase.objectPointIds<false>(poseId));

				IndexSet32 newAttachedObjectPointIdSet(newAttachedObjectPointIds.cbegin(), newAttachedObjectPointIds.cend());
				newAttachedObjectPointIdSet.insert(copyAttachedObjectPointIds.cbegin(), copyAttachedObjectPointIds.cend());

				if (newAttachedObjectPointIdSet != IndexSet32(newAttachedObjectPointIds.cbegin(), newAttachedObjectPointIds.cend()))
				{
					OCEAN_SET_FAILED(validation);
					break;
				}
			}

			for (unsigned int i = 0u; validation.succeededSoFar() && i < firstDatabaseCopyImagePointIds.size(); ++i)
			{
				const Index32 imagePointId = firstDatabaseCopyImagePointIds[i];

				// the image point must still exist
				if (!firstDatabase.hasImagePoint<false>(imagePointId))
				{
					OCEAN_SET_FAILED(validation);
					break;
				}

				// the image point value must be identical
				const Vector2& imagePoint = firstDatabase.imagePoint<false>(imagePointId);
				if (imagePoint != firstDatabaseCopyImagePoints[i])
				{
					OCEAN_SET_FAILED(validation);
					break;
				}
			}

			for (unsigned int i = 0u; validation.succeededSoFar() && i < firstDatabaseCopyObjectPointIds.size(); ++i)
			{
				const Index32 objectPointId = firstDatabaseCopyObjectPointIds[i];

				// the object point must still exist
				if (!firstDatabase.hasObjectPoint<false>(objectPointId))
				{
					OCEAN_SET_FAILED(validation);
					break;
				}

				// the object point value must be identical
				const Vector3& objectPoint = firstDatabase.objectPoint<false>(objectPointId);
				if (objectPoint != firstDatabaseCopyObjectPoints[i])
				{
					OCEAN_SET_FAILED(validation);
					break;
				}
			}

			// now we want to ensure that the new object points comes with all necessary information
			// we want correct topology and correct values

			Vector3 newFirstDatabaseObjectPoint, secondDatabaseObjectPoint;
			if (!firstDatabase.hasObjectPoint<false>(newFirstDatabaseObjectPointId, &newFirstDatabaseObjectPoint) || !secondDatabase.hasObjectPoint<false>(secondDatabaseObjectPointId, &secondDatabaseObjectPoint))
			{
				OCEAN_SET_FAILED(validation);
				break;
			}

			if (newFirstDatabaseObjectPoint != secondDatabaseObjectPoint)
			{
				OCEAN_SET_FAILED(validation);
				break;
			}

			Indices32 oldVisibleInPoses, oldImagePointIds;
			Vectors2 oldImagePoints;
			secondDatabase.observationsFromObjectPoint<false>(secondDatabaseObjectPointId, oldVisibleInPoses, oldImagePointIds, &oldImagePoints);

			if (oldVisibleInPoses.size() != oldImagePointIds.size() || oldImagePointIds.size() != oldImagePoints.size())
			{
				OCEAN_SET_FAILED(validation);
				break;
			}

			Indices32 newVisibleInPoses, newImagePointIds;
			Vectors2 newImagePoints;
			firstDatabase.observationsFromObjectPoint<false>(newFirstDatabaseObjectPointId, newVisibleInPoses, newImagePointIds, &newImagePoints);

			if (newVisibleInPoses.size() != newImagePointIds.size() || newImagePointIds.size() != newImagePoints.size())
			{
				OCEAN_SET_FAILED(validation);
				break;
			}

			// we want to ensure that we do not get more observations

			if (newVisibleInPoses.size() > oldVisibleInPoses.size() || newImagePointIds.size() > oldImagePointIds.size())
			{
				OCEAN_SET_FAILED(validation);
				break;
			}

			const Index32 lowerPoseRange = lowerPoseId == Tracking::Database::invalidId ? 0u : lowerPoseId;
			const Index32 upperPoseRange = upperPoseId == Tracking::Database::invalidId ? 0xFFFFFFFFu : upperPoseId;

			for (size_t i = 0; validation.succeededSoFar() && i < oldImagePointIds.size(); ++i)
			{
				const Index32 poseId = oldVisibleInPoses[i]; // oldPoseId == newPoseId

				if (forExistingPosesOnly && !firstDatabaseCopy.hasPose<false>(poseId))
				{
					// the original database did not have this pose, so we still must not have this pose
					if (firstDatabase.hasPose<false>(poseId))
					{
						OCEAN_SET_FAILED(validation);
						break;
					}
				}
				else
				{
					if (lowerPoseRange <= poseId && poseId <= upperPoseRange)
					{
						const Vector2& oldImagePoint = oldImagePoints[i];

						Vector2 newImagePoint;
						if (!firstDatabase.hasObservation<false>(poseId, newFirstDatabaseObjectPointId, &newImagePoint))
						{
							OCEAN_SET_FAILED(validation);
							break;
						}

						if (newImagePoint != oldImagePoint + Vector2(100, 100))
						{
							OCEAN_SET_FAILED(validation);
							break;
						}
					}
					else
					{
						if (firstDatabase.hasObservation<false>(poseId, newFirstDatabaseObjectPointId))
						{
							OCEAN_SET_FAILED(validation);
							break;
						}
					}
				}
			}

			if (!validation.succeededSoFar())
			{
				break;
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDatabase::testSerialization(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test serialization:";

	const static unsigned int maxNumberPoses = 100u;
	const static unsigned int maxNumberObjectPoints = 100u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int firstDatabaseNumberPoses = RandomI::random(randomGenerator, 1u, maxNumberPoses);
		const unsigned int firstDatabaseNumberObjectPoints = RandomI::random(randomGenerator, 1u, maxNumberObjectPoints);

		const Tracking::Database originalDatabase(createDatabaseWithRandomTopology(randomGenerator, 0u, maxNumberPoses - 1u, firstDatabaseNumberPoses, firstDatabaseNumberObjectPoints, 0u, firstDatabaseNumberObjectPoints));

		std::ostringstream stringOutputStream(std::ios::binary);
		IO::OutputBitstream outputStream(stringOutputStream);
		if (!Tracking::Utilities::writeDatabase(originalDatabase, outputStream))
		{
			OCEAN_SET_FAILED(validation);
		}

		std::istringstream stringInputStream(stringOutputStream.str(), std::ios::binary);
		IO::InputBitstream inputStream(stringInputStream);

		Tracking::Database newDatabase;
		if (Tracking::Utilities::readDatabase(inputStream, newDatabase))
		{
			{
				// checking the poses

				const Indices32 poseIds = originalDatabase.poseIds<false>();

				for (const Index32& poseId : poseIds)
				{
					if (!newDatabase.hasPose<false>(poseId) || originalDatabase.pose<false>(poseId) != newDatabase.pose<false>(poseId))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}

			{
				// checking the image points

				const Indices32 imagePointIds = originalDatabase.imagePointIds<false>();

				for (const Index32& imagePointId : imagePointIds)
				{
					if (!newDatabase.hasImagePoint<false>(imagePointId) || originalDatabase.imagePoint<false>(imagePointId) != newDatabase.imagePoint<false>(imagePointId))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}

			{
				// checking the object points

				const Indices32 objectPointIds = originalDatabase.objectPointIds<false>();

				for (const Index32& objectPointId : objectPointIds)
				{
					if (!newDatabase.hasObjectPoint<false>(objectPointId) || originalDatabase.objectPoint<false>(objectPointId) != newDatabase.objectPoint<false>(objectPointId))
					{
						OCEAN_SET_FAILED(validation);
					}

					if (originalDatabase.imagePointsFromObjectPoint<false>(objectPointId) != newDatabase.imagePointsFromObjectPoint<false>(objectPointId))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Tracking::Database TestDatabase::createDatabaseWithRandomTopology(RandomGenerator& randomGenerator, const unsigned int lowerPoseId, const unsigned int upperPoseId, const unsigned int numberPoses, const unsigned int numberObjectPoints, const unsigned int minimalNumberObservations, const unsigned int maximalNumberObservations)
{
	ocean_assert(lowerPoseId <= upperPoseId);
	ocean_assert(numberPoses <= upperPoseId - lowerPoseId + 1u);

	ocean_assert(numberObjectPoints >= 1u);
	ocean_assert(minimalNumberObservations <= maximalNumberObservations && maximalNumberObservations >= 1u && maximalNumberObservations <= numberObjectPoints);

	Tracking::Database database;

	// first, we create our poses

	IndexSet32 databasePoseIds;

	while (databasePoseIds.size() < numberPoses)
	{
		databasePoseIds.insert(RandomI::random(lowerPoseId, upperPoseId));
	}

	for (IndexSet32::const_iterator i = databasePoseIds.cbegin(); i != databasePoseIds.cend(); ++i)
	{
		ocean_assert(*i >= lowerPoseId && *i <= upperPoseId);

		// we encode the id of the pose as x-translation of the pose
		database.addPose<false>(*i, HomogenousMatrix4(Vector3(Scalar(*i), 0, 0)));
	}

	// now, we create our object points

	Indices32 datbaseObjectPointIds;
	datbaseObjectPointIds.reserve(numberObjectPoints);

	for (size_t n = 0u; n < numberObjectPoints; ++n)
	{
		const Index32 objectPointId = database.addObjectPoint<false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		// we encode the id of the object point as x-location of the point
		database.setObjectPoint<false>(objectPointId, Vector3(Scalar(objectPointId), 0, 0));

		datbaseObjectPointIds.push_back(objectPointId);
	}

	// now, we create our observations (image points in each pose), and we create the topology

	for (IndexSet32::const_iterator iP = databasePoseIds.cbegin(); iP != databasePoseIds.cend(); ++ iP)
	{
		const Index32& poseId = *iP;

		const unsigned int numberImagePoints = RandomI::random(randomGenerator, minimalNumberObservations, maximalNumberObservations);

		IndexSet32 usedObjectPoints;

		for (unsigned int i = 0u; i < numberImagePoints; ++i)
		{
			ocean_assert(usedObjectPoints.size() < datbaseObjectPointIds.size());

			// we seek for an unused object point (unsued for this pose)

			Index32 objectPointId = datbaseObjectPointIds[RandomI::random((unsigned int)datbaseObjectPointIds.size() - 1)];
			while (usedObjectPoints.find(objectPointId) != usedObjectPoints.cend())
			{
				objectPointId = datbaseObjectPointIds[RandomI::random((unsigned int)datbaseObjectPointIds.size() - 1)];
			}

			usedObjectPoints.insert(objectPointId);

			// we  encode the poseId and objectPointId as image coordinates
			const Index32 imagePointId = database.addImagePoint<false>(Vector2(Scalar(poseId), Scalar(objectPointId)));

			database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
			database.attachImagePointToPose<false>(imagePointId, poseId);
		}
	}

	return database;
}

}

}

}
