/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/rmv/RandomModelVariation.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

template <bool tLessImagePoints>
bool RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF(const HomogenousMatrix4& initialPoseIF, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator& randomGenerator, HomogenousMatrix4& poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError, const Vector3& maximalTranslationOffset, const Scalar maximalOrientationOffset, const double timeout, Scalar* resultingSqrError, IndexPairs32* correspondences, bool* explicitStop, Worker* worker)
{
	ocean_assert(initialPoseIF.isValid() && pinholeCamera.isValid());
	ocean_assert(objectPoints && imagePoints && timeout > 0.0);

	ocean_assert(!tLessImagePoints || numberImagePoints <= numberObjectPoints);
	ocean_assert(tLessImagePoints || numberObjectPoints <= numberImagePoints);

	if (numberValidCorrespondences < 3 || numberValidCorrespondences > min(numberObjectPoints, numberImagePoints))
		return false;

	Vectors2 projectedObjectPoints(numberObjectPoints);
	pinholeCamera.projectToImageIF<true>(initialPoseIF, objectPoints, numberObjectPoints, false, projectedObjectPoints.data());
	ocean_assert(projectedObjectPoints.size() == numberObjectPoints);

	const Vector2* smallPointGroup = tLessImagePoints ? imagePoints : projectedObjectPoints.data();
	const size_t smallPointGroupNumber = tLessImagePoints ? numberImagePoints : numberObjectPoints;

	const Vector2* largePointGroup = tLessImagePoints ? projectedObjectPoints.data() : imagePoints;
	const size_t largePointGroupNumber = tLessImagePoints ? numberObjectPoints : numberImagePoints;

	ocean_assert(smallPointGroup != largePointGroup);

	const Scalar initialError = Geometry::Error::averagedRobustErrorInPointCloud<Geometry::Estimator::ET_HUBER>(smallPointGroup, smallPointGroupNumber, numberValidCorrespondences, largePointGroup, largePointGroupNumber, errorDetermination);

	// check whether the initial pose is accurate enough
	if (initialError <= targetAverageSqrError)
		return assignBestPoseIF(smallPointGroup, smallPointGroupNumber, largePointGroup, largePointGroupNumber, numberValidCorrespondences, initialError, initialPoseIF, poseIF, errorDetermination, targetAverageSqrError, resultingSqrError, correspondences, nullptr, nullptr);

	Scalar dummySqrError = -1;
	if (resultingSqrError)
		*resultingSqrError = -1;
	else
		resultingSqrError = &dummySqrError;

	if (worker)
	{
		Lock lock;

		if (explicitStop)
			return worker->executeAbortableFunction(Worker::AbortableFunction::createStatic(&RandomModelVariation::optimizedPoseFromPointCloudsAbortableIF<tLessImagePoints>, &initialPoseIF, &pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, numberValidCorrespondences, &randomGenerator, &poseIF, errorDetermination, targetAverageSqrError, maximalTranslationOffset, maximalOrientationOffset, timeout, resultingSqrError, correspondences, explicitStop, &lock), 16);
		else
		{
			bool abort = false;
			return worker->executeAbortableFunction(Worker::AbortableFunction::createStatic(&RandomModelVariation::optimizedPoseFromPointCloudsAbortableIF<tLessImagePoints>, &initialPoseIF, &pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, numberValidCorrespondences, &randomGenerator, &poseIF, errorDetermination, targetAverageSqrError, maximalTranslationOffset, maximalOrientationOffset, timeout, resultingSqrError, correspondences, &abort, &lock), 16);
		}
	}
	else
		return optimizedPoseFromPointCloudsAbortableIF<tLessImagePoints>(&initialPoseIF, &pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, numberValidCorrespondences, &randomGenerator, &poseIF, errorDetermination, targetAverageSqrError, maximalTranslationOffset, maximalOrientationOffset, timeout, resultingSqrError, correspondences, explicitStop);
}

template <bool tLessImagePoints>
bool RandomModelVariation::optimizedPoseFromPointCloudsWithSeveralInitialPosesIF(const HomogenousMatrix4* initialPosesIF, const size_t numberInitialPoses, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator& randomGenerator, HomogenousMatrix4& poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError, const Vector3& maximalTranslationOffset, const Scalar maximalOrientationOffset, const double timeout, Scalar* resultingSqrError, bool* explicitStop, Worker* worker)
{
	ocean_assert(initialPosesIF && numberInitialPoses != 0);

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < numberInitialPoses; ++n)
		ocean_assert(initialPosesIF[n].isValid());
#endif

	ocean_assert(pinholeCamera.isValid());

	ocean_assert(!tLessImagePoints || numberImagePoints <= numberObjectPoints);
	ocean_assert(tLessImagePoints || numberObjectPoints <= numberImagePoints);

	if (numberValidCorrespondences < 3 || numberValidCorrespondences > min(numberObjectPoints, numberImagePoints))
		return false;

	Scalar dummySqrError = -1;
	if (resultingSqrError)
		*resultingSqrError = -1;
	else
		resultingSqrError = &dummySqrError;

	if (worker)
	{
		Lock lock;

		if (explicitStop)
			return worker->executeSeparableAndAbortableFunction(Worker::AbortableFunction::createStatic(&RandomModelVariation::optimizedPoseFromPointCloudsPoseIFSubset<tLessImagePoints>, initialPosesIF, 0u, 0u, &pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, numberValidCorrespondences, &randomGenerator, &poseIF, errorDetermination, targetAverageSqrError, maximalTranslationOffset, maximalOrientationOffset, timeout, resultingSqrError, explicitStop, &lock), 0u, (unsigned int)numberInitialPoses, 1u, 2u, 17u);
		else
		{
			bool abort = false;
			return worker->executeSeparableAndAbortableFunction(Worker::AbortableFunction::createStatic(&RandomModelVariation::optimizedPoseFromPointCloudsPoseIFSubset<tLessImagePoints>, initialPosesIF, 0u, 0u, &pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, numberValidCorrespondences, &randomGenerator, &poseIF, errorDetermination, targetAverageSqrError, maximalTranslationOffset, maximalOrientationOffset, timeout, resultingSqrError, &abort, &lock), 0u, (unsigned int)numberInitialPoses, 1u, 2u, 17u);
		}
	}
	else
		return optimizedPoseFromPointCloudsPoseIFSubset<tLessImagePoints>(initialPosesIF, 0u, (unsigned int)numberInitialPoses, &pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, numberValidCorrespondences, &randomGenerator, &poseIF, errorDetermination, targetAverageSqrError, maximalTranslationOffset, maximalOrientationOffset, timeout, resultingSqrError, explicitStop, nullptr);
}

template <bool tLessImagePoints>
bool RandomModelVariation::optimizedPoseFromPointCloudsAbortableIF(const HomogenousMatrix4* initialPoseIF, const PinholeCamera* pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator* randomGenerator, HomogenousMatrix4* poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError, Vector3 maximalTranslationOffset, Scalar maximalOrientationOffset, const double timeout, Scalar* resultingSqrError, IndexPairs32* correspondences, bool* explicitStop, Lock* lock)
{
	ocean_assert(objectPoints && imagePoints && timeout > 0.0);

	ocean_assert(initialPoseIF && pinholeCamera && randomGenerator && poseIF);

	ocean_assert(!tLessImagePoints || numberImagePoints <= numberObjectPoints);
	ocean_assert(tLessImagePoints || numberObjectPoints <= numberImagePoints);

	ocean_assert(numberValidCorrespondences >= 3);
	ocean_assert(numberValidCorrespondences < min(numberObjectPoints, numberImagePoints));

	if (numberValidCorrespondences < 3 || numberValidCorrespondences > min(numberObjectPoints, numberImagePoints))
		return false;

	// create a local random generator with individual seed value
	RandomGenerator generator(*randomGenerator);

	const Vector3 initialTranslationIF(initialPoseIF->translation());
	const Quaternion initialOrientationIF(initialPoseIF->rotation());

	Vectors2 projectedObjectPoints(numberObjectPoints);
	pinholeCamera->projectToImageIF<true>(*initialPoseIF, objectPoints, numberObjectPoints, false, projectedObjectPoints.data());

	const Vector2* smallPointGroup = tLessImagePoints ? imagePoints : projectedObjectPoints.data();
	const size_t smallPointGroupNumber = tLessImagePoints ? numberImagePoints : numberObjectPoints;

	const size_t largePointGroupNumber = tLessImagePoints ? numberObjectPoints : numberImagePoints;
	const Vector2* largePointGroup = tLessImagePoints ? projectedObjectPoints.data() : imagePoints;

	ocean_assert(smallPointGroup != largePointGroup);

	const Scalar initialError = Geometry::Error::averagedRobustErrorInPointCloud<Geometry::Estimator::ET_HUBER>(smallPointGroup, smallPointGroupNumber, numberValidCorrespondences, largePointGroup, largePointGroupNumber, errorDetermination);

	// check whether the initial pose is accurate enough
	if (initialError <= targetAverageSqrError)
		return assignBestPoseIF(smallPointGroup, smallPointGroupNumber, largePointGroup, largePointGroupNumber, numberValidCorrespondences, initialError, *initialPoseIF, *poseIF, errorDetermination, targetAverageSqrError, resultingSqrError, correspondences, explicitStop, lock);

	Scalar optimizedError = initialError;

	HomogenousMatrix4 optimizedPoseIF(*initialPoseIF);
	const Scalar maxOrientationOffsetCos2(Numeric::cos(maximalOrientationOffset * Scalar(0.5)));

	Vector3 currentTranslationOffset = maximalTranslationOffset * Scalar(0.5);
	Scalar currentOrientationOffset = maximalOrientationOffset * Scalar(0.5);

	const Timestamp startTimestamp(true);
	while (startTimestamp + timeout > Timestamp(true) && (!explicitStop || !*explicitStop))
	{
		// we add an explicit loop to reduce the number of timestamp lookups
		for (unsigned int i = 0u; i < 100u; ++i)
		{
			const HomogenousMatrix4 testOptimizedPoseIF = optimizedPoseIF * HomogenousMatrix4(Random::vector3(generator, currentTranslationOffset), Random::euler(generator, currentOrientationOffset));
			ocean_assert(testOptimizedPoseIF.isValid());

			const Vector3 testTranslationIF(testOptimizedPoseIF.translation());
			const Quaternion testOrientationIF(testOptimizedPoseIF.rotation());

			if (!(Numeric::abs(initialTranslationIF.x() - testTranslationIF.x()) <= maximalTranslationOffset.x()
						&& Numeric::abs(initialTranslationIF.y() - testTranslationIF.y()) <= maximalTranslationOffset.y()
						&& Numeric::abs(initialTranslationIF.z() - testTranslationIF.z()) <= maximalTranslationOffset.z()
						&& initialOrientationIF.cos2(testOrientationIF) >= maxOrientationOffsetCos2))
					continue;

			if (explicitStop && *explicitStop)
				return false;

			pinholeCamera->projectToImageIF<true>(testOptimizedPoseIF, objectPoints, numberObjectPoints, false, projectedObjectPoints.data());

			if (explicitStop && *explicitStop)
				return false;

			const Scalar testError = Geometry::Error::averagedRobustErrorInPointCloud<Geometry::Estimator::ET_HUBER>(smallPointGroup, smallPointGroupNumber, numberValidCorrespondences, largePointGroup, largePointGroupNumber, errorDetermination);

			if (testError > optimizedError)
			{
				ocean_assert(testError > targetAverageSqrError);
				continue;
			}

			// check whether the current error is good enough
			if (testError <= targetAverageSqrError)
				return assignBestPoseIF(smallPointGroup, smallPointGroupNumber, largePointGroup, largePointGroupNumber, numberValidCorrespondences, testError, testOptimizedPoseIF, *poseIF, errorDetermination, targetAverageSqrError, resultingSqrError, correspondences, explicitStop, lock);

			currentTranslationOffset *= Scalar(0.9);
			currentOrientationOffset *= Scalar(0.9);

			optimizedPoseIF = testOptimizedPoseIF;
			optimizedError = testError;
		}
	}

	return false;
}

template <bool tLessImagePoints>
bool RandomModelVariation::optimizedPoseFromPointCloudsPoseIFSubset(const HomogenousMatrix4* initialPosesIF, const unsigned int firstInitialPose, const unsigned int numberInitialPoses, const PinholeCamera* pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator* randomGenerator, HomogenousMatrix4* poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError, Vector3 maximalTranslationOffset, Scalar maximalOrientationOffset, const double timeout, Scalar* resultingSqrError, bool* explicitStop, Lock* lock)
{
	ocean_assert(numberInitialPoses != 0);
	const double sharedTimeout = timeout / double(numberInitialPoses);

	for (unsigned int n = firstInitialPose; n < firstInitialPose + numberInitialPoses && (!explicitStop || !*explicitStop); ++n)
		if (optimizedPoseFromPointCloudsAbortableIF<tLessImagePoints>(initialPosesIF + n, pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, numberValidCorrespondences, randomGenerator, poseIF, errorDetermination, targetAverageSqrError, maximalTranslationOffset, maximalOrientationOffset, sharedTimeout, resultingSqrError, nullptr, explicitStop, lock))
			return true;

	return false;
}

bool RandomModelVariation::assignBestPoseIF(const Vector2* smallPointGroup, const size_t smallPointGroupNumber, const Vector2* largePointGroup, const size_t largePointGroupNumber, const size_t numberValidCorrespondences, const Scalar candidateSqrError, const HomogenousMatrix4& poseCandidateIF, HomogenousMatrix4& poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError, Scalar* resultingSqrError, IndexPairs32* correspondences, bool* explicitStop, Lock* lock)
{
	ocean_assert(smallPointGroup && largePointGroup);
	ocean_assert(smallPointGroupNumber <= largePointGroupNumber);
	ocean_assert(numberValidCorrespondences <= min(smallPointGroupNumber, largePointGroupNumber));

	// force the explicit stop of all other (parallel) threads here (however, will be done int he worker object later also)
	if (explicitStop)
		*explicitStop = true;

	IndexPairs32 resultCorrespondences;

	if (correspondences)
	{
		IndexPairs32 allCorrespondences;
		allCorrespondences.reserve(smallPointGroupNumber);

		Geometry::Error::averagedRobustErrorInPointCloud<Geometry::Estimator::ET_HUBER>(smallPointGroup, smallPointGroupNumber, numberValidCorrespondences, largePointGroup, largePointGroupNumber, errorDetermination, &allCorrespondences);

		for (IndexPairs32::const_iterator i = allCorrespondences.begin(); i != allCorrespondences.end(); ++i)
		{
			ocean_assert(i->first < smallPointGroupNumber);
			ocean_assert(i->second < largePointGroupNumber);

			if (smallPointGroup[i->first].sqrDistance(largePointGroup[i->second]) < targetAverageSqrError)
				resultCorrespondences.push_back(*i);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	// checks whether a parallel thread concurrently found an even better solution
	if (resultingSqrError && *resultingSqrError >= 0 && candidateSqrError > *resultingSqrError)
		return false;

	poseIF = poseCandidateIF;

	if (correspondences)
		*correspondences = resultCorrespondences;

	if (resultingSqrError)
		*resultingSqrError = candidateSqrError;

	return true;
}

template bool RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<false>(const HomogenousMatrix4&, const PinholeCamera&, const Vector3*, const size_t, const Vector2*, const size_t, const size_t, RandomGenerator&, HomogenousMatrix4&, const Geometry::Error::ErrorDetermination, const Scalar, const Vector3&, const Scalar, const double, Scalar*, IndexPairs32*, bool*, Worker*);
template bool RandomModelVariation::optimizedPoseFromPointCloudsWithOneInitialPoseIF<true>(const HomogenousMatrix4&, const PinholeCamera&, const Vector3*, const size_t, const Vector2*, const size_t, const size_t, RandomGenerator&, HomogenousMatrix4&, const Geometry::Error::ErrorDetermination, const Scalar, const Vector3&, const Scalar, const double, Scalar*, IndexPairs32*, bool*, Worker*);

template bool RandomModelVariation::optimizedPoseFromPointCloudsWithSeveralInitialPosesIF<false>(const HomogenousMatrix4*, const size_t, const PinholeCamera&, const Vector3*, const size_t, const Vector2*, const size_t, const size_t, RandomGenerator&, HomogenousMatrix4&, const Geometry::Error::ErrorDetermination, const Scalar, const Vector3&, const Scalar, const double, Scalar*, bool*, Worker*);
template bool RandomModelVariation::optimizedPoseFromPointCloudsWithSeveralInitialPosesIF<true>(const HomogenousMatrix4*, const size_t, const PinholeCamera&, const Vector3*, const size_t, const Vector2*, const size_t, const size_t, RandomGenerator&, HomogenousMatrix4&, const Geometry::Error::ErrorDetermination, const Scalar, const Vector3&, const Scalar, const double, Scalar*, bool*, Worker*);

}

}

}
