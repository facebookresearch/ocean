/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/CameraCalibration.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/base/Utilities.h"

#include <algorithm>
#include <set>

namespace Ocean
{

namespace Geometry
{

CameraCalibration::Pattern::Pattern()
{
	patternCorners[0] = Vector2(0, 0);
	patternCorners[1] = Vector2(0, 0);
	patternCorners[2] = Vector2(0, 0);
	patternCorners[3] = Vector2(0, 0);
}

CameraCalibration::Pattern::Pattern(const PatternRows& rows, const Timestamp timestamp) :
	patternRows(rows),
	patternTimestamp(timestamp)
{
	ocean_assert(!rows.empty());
	const PatternRow& lastRow = patternRows[patternRows.size() - 1];

	patternCorners[0] = patternRows.front().front();
	patternCorners[1] = lastRow.front();
	patternCorners[2] = lastRow[lastRow.size() - 1];
	patternCorners[3] = patternRows.front()[patternRows.front().size() - 1];
}

Box2 CameraCalibration::Pattern::boundingBox() const
{
	ocean_assert(!isEmpty());

	Box2 box(patternCorners[0], patternCorners[1]);
	box += patternCorners[2];
	box += patternCorners[3];

	return box;
}

Scalar CameraCalibration::Pattern::distance(const Pattern& pattern) const
{
	typedef std::pair<Scalar, IndexPair32> DistancePair;
	typedef std::vector<DistancePair> DistancePairs;
	typedef std::set<unsigned int> IndexSet;

	DistancePairs distances;
	distances.reserve(16);

	distances.push_back(DistancePair(patternCorners[0].sqrDistance(pattern.patternCorners[0]), IndexPair32(0, 0)));
	distances.push_back(DistancePair(patternCorners[0].sqrDistance(pattern.patternCorners[1]), IndexPair32(0, 1)));
	distances.push_back(DistancePair(patternCorners[0].sqrDistance(pattern.patternCorners[2]), IndexPair32(0, 2)));
	distances.push_back(DistancePair(patternCorners[0].sqrDistance(pattern.patternCorners[3]), IndexPair32(0, 3)));

	distances.push_back(DistancePair(patternCorners[1].sqrDistance(pattern.patternCorners[0]), IndexPair32(1, 0)));
	distances.push_back(DistancePair(patternCorners[1].sqrDistance(pattern.patternCorners[1]), IndexPair32(1, 1)));
	distances.push_back(DistancePair(patternCorners[1].sqrDistance(pattern.patternCorners[2]), IndexPair32(1, 2)));
	distances.push_back(DistancePair(patternCorners[1].sqrDistance(pattern.patternCorners[3]), IndexPair32(1, 3)));

	distances.push_back(DistancePair(patternCorners[2].sqrDistance(pattern.patternCorners[0]), IndexPair32(2, 0)));
	distances.push_back(DistancePair(patternCorners[2].sqrDistance(pattern.patternCorners[1]), IndexPair32(2, 1)));
	distances.push_back(DistancePair(patternCorners[2].sqrDistance(pattern.patternCorners[2]), IndexPair32(2, 2)));
	distances.push_back(DistancePair(patternCorners[2].sqrDistance(pattern.patternCorners[3]), IndexPair32(2, 3)));

	distances.push_back(DistancePair(patternCorners[3].sqrDistance(pattern.patternCorners[0]), IndexPair32(3, 0)));
	distances.push_back(DistancePair(patternCorners[3].sqrDistance(pattern.patternCorners[1]), IndexPair32(3, 1)));
	distances.push_back(DistancePair(patternCorners[3].sqrDistance(pattern.patternCorners[2]), IndexPair32(3, 2)));
	distances.push_back(DistancePair(patternCorners[3].sqrDistance(pattern.patternCorners[3]), IndexPair32(3, 3)));

	std::sort(distances.begin(), distances.end(), compareFirst<Scalar, IndexPair32>);
	IndexSet indexSetThis;
	IndexSet indexSetOther;

	Scalar distance = 0;
	for (DistancePairs::const_iterator i = distances.begin(); i != distances.end() && indexSetThis.size() != 4; ++i)
		if (indexSetThis.find(i->second.first) == indexSetThis.end() && indexSetOther.find(i->second.second) == indexSetOther.end())
		{
			distance += i->first;

			indexSetThis.insert(i->second.first);
			indexSetOther.insert(i->second.second);
		}

	ocean_assert(indexSetThis.size() == 4);
	ocean_assert(indexSetOther.size() == 4);

	return distance;
}

void CameraCalibration::Pattern::changeSize(const Scalar sizeFactor)
{
	ocean_assert(sizeFactor > 0);

	for (PatternRows::iterator iR = patternRows.begin(); iR != patternRows.end(); ++iR)
		for (PatternRow::iterator iC = iR->begin(); iC != iR->end(); ++iC)
			*iC *= sizeFactor;

	const PatternRow& lastRow = patternRows[patternRows.size() - 1];

	patternCorners[0] = patternRows.front().front();
	patternCorners[1] = lastRow.front();
	patternCorners[2] = lastRow[lastRow.size() - 1];
	patternCorners[3] = patternRows.front()[patternRows.front().size() - 1];
}

bool CameraCalibration::determineCameraCalibrationPlanar(const unsigned int width, const unsigned int height, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, PinholeCamera& pinholeCamera, const unsigned int iterations, Scalar* sqrAccuracy)
{
	SquareMatrix3 intrinsic;
	SquareMatrices3 validHomographies;
	Indices32 validHomographIndices;

	if (!determineIntrinsicCameraMatrixPlanar(objectPointGroups, imagePointGroups, intrinsic, &validHomographies, &validHomographIndices))
	{
		return false;
	}

	ocean_assert(validHomographies.size() == validHomographIndices.size());

	// remove skew parameter
	intrinsic(0, 1) = 0;

	// determines the initial extrinsic camera matrices
	HomogenousMatrices4 extrinsics;
	HomogenousMatrix4 extrinsic;

	for (unsigned int n = 0; n < validHomographies.size(); ++n)
	{
		if (Geometry::Homography::extrinsicMatrix(intrinsic, validHomographies[n], extrinsic))
		{
			const unsigned int validGroupIndex = validHomographIndices[n];

			Scalar averageSqrError = 0;
			Scalar minimalSqrError = 0;
			Scalar maximalSqrError = 0;
			Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(extrinsic, PinholeCamera(intrinsic, width, height), ConstTemplateArrayAccessor<Vector3>(objectPointGroups[validGroupIndex]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[validGroupIndex]), false, averageSqrError, minimalSqrError, maximalSqrError);

			if (averageSqrError > 5 * 5 || maximalSqrError > 10 * 10)
			{
				return false;
			}

			extrinsics.push_back(extrinsic);
		}
	}

	// determines the initial distortion parameters
	Scalar distortion2 = 0;
	Scalar distortion4 = 0;
	if (!Geometry::Homography::distortionParameters(ConstArrayAccessor<HomogenousMatrix4>(extrinsics), intrinsic, objectPointGroups, imagePointGroups, distortion2, distortion4))
	{
		return false;
	}

	// define the initial camera
	pinholeCamera = PinholeCamera(intrinsic, width, height, PinholeCamera::DistortionPair(distortion2, distortion4), PinholeCamera::DistortionPair());

#ifdef OCEAN_DEBUG

	Scalar averageSqrErrorND = 0;
	Scalar minimalSqrErrorND = Numeric::maxValue();
	Scalar maximalSqrErrorND = 0;

	Scalar averageSqrErrorWD = 0;
	Scalar minimalSqrErrorWD = Numeric::maxValue();
	Scalar maximalSqrErrorWD = 0;

	for (size_t n = 0; n < objectPointGroups.size(); ++n)
	{
		HomogenousMatrix4& pose = extrinsics[n];

		Scalar averageSqrError = 0;
		Scalar minimalSqrError = Numeric::maxValue();
		Scalar maximalSqrError = 0;

		Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPointGroups[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[n]), false, averageSqrError, minimalSqrError, maximalSqrError);
		averageSqrErrorND += averageSqrError;
		minimalSqrErrorND = min(minimalSqrErrorND, minimalSqrError);
		maximalSqrErrorND = max(maximalSqrErrorND, maximalSqrError);

		Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose, pinholeCamera, ConstTemplateArrayAccessor<Vector3>(objectPointGroups[n]), ConstTemplateArrayAccessor<Vector2>(imagePointGroups[n]), true, averageSqrError, minimalSqrError, maximalSqrError);
		averageSqrErrorWD += averageSqrError;
		minimalSqrErrorWD = min(minimalSqrErrorWD, minimalSqrError);
		maximalSqrErrorWD = max(maximalSqrErrorWD, maximalSqrError);
	}

	ocean_assert(averageSqrErrorND > averageSqrErrorWD);

#endif

	PinholeCamera optimizedCamera;
	Scalar initialSqrError = 0;
	Scalar finalSqrError = 0;

	if (!NonLinearOptimizationCamera::optimizeCameraPoses(pinholeCamera, ConstArrayAccessor<HomogenousMatrix4>(extrinsics), objectPointGroups, imagePointGroups, optimizedCamera, nullptr, iterations, Estimator::ET_SQUARE, Scalar(0.001), 5, true, &initialSqrError, &finalSqrError))
	{
		return false;
	}

	pinholeCamera = optimizedCamera;

	if (sqrAccuracy != nullptr)
	{
		*sqrAccuracy = finalSqrError;
	}

	return true;
}

bool CameraCalibration::determineIntrinsicCameraMatrixPlanar(const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, SquareMatrix3& intrinsic, SquareMatrices3* homographies, Indices32* validGroupIndices)
{
	ocean_assert(objectPointGroups.size() == imagePointGroups.size());
	ocean_assert(validGroupIndices == nullptr || validGroupIndices->empty());

	if (objectPointGroups.size() < 3 || objectPointGroups.size() != imagePointGroups.size())
		return false;

	SquareMatrices3 localHomographies;
	localHomographies.reserve(objectPointGroups.size());

	for (size_t n = 0; n < objectPointGroups.size(); ++n)
	{
		const ObjectPoints& objectPoints = objectPointGroups[n];
		const ImagePoints& imagePoints = imagePointGroups[n];

		ocean_assert(objectPoints.size() == imagePoints.size());
		ocean_assert(objectPoints.size() >= 10);

		SquareMatrix3 homography;
		if (objectPoints.size() == imagePoints.size() && Homography::homographyMatrixPlaneXY(objectPoints.data(), imagePoints.data(), objectPoints.size(), homography))
		{
			localHomographies.push_back(homography);

			if (validGroupIndices)
				validGroupIndices->push_back((unsigned int)n);
		}
	}

	if (localHomographies.size() < 3)
		return false;

	if (!Homography::intrinsicMatrix(localHomographies.data(), localHomographies.size(), intrinsic))
		return false;

	if (homographies)
		*homographies = std::move(localHomographies);

	return true;
}

bool CameraCalibration::determineCameraCalibration(const PinholeCamera& roughCamera, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, PinholeCamera& pinholeCamera, Scalar* sqrAccuracy)
{
	ocean_assert(roughCamera.isValid());

	ocean_assert(objectPointGroups.size() == imagePointGroups.size());
	ocean_assert(objectPointGroups.size() >= 1);

	if (objectPointGroups.empty() || objectPointGroups.size() != imagePointGroups.size())
	{
		return false;
	}

	ImagePoints totalNormalizedObjectPoints;
	totalNormalizedObjectPoints.reserve(objectPointGroups.size() * objectPointGroups.front().size());

	ImagePoints totalImagePoints;
	totalImagePoints.reserve(totalNormalizedObjectPoints.capacity());

	RandomGenerator randomGenerator;

	for (size_t n = 0; n < objectPointGroups.size(); ++n)
	{
		const ObjectPoints& objectPoints = objectPointGroups[n];
		const ImagePoints& imagePoints = imagePointGroups[n];

		HomogenousMatrix4 roughPose;
		bool result = RANSAC::p3p(AnyCameraPinhole(roughCamera), ConstArrayAccessor<ObjectPoint>(objectPoints), ConstArrayAccessor<ImagePoint>(imagePoints), randomGenerator, roughPose);
		ocean_assert(result);
		if (!result)
		{
			continue;
		}

		HomogenousMatrix4 pose;
		if (!NonLinearOptimizationPose::optimizePose(roughCamera, roughPose, ConstArrayAccessor<ObjectPoint>(objectPoints), ConstArrayAccessor<ImagePoint>(imagePoints), true, pose))
		{
			ocean_assert(false && "Should always succeeded!");
			continue;
		}

		totalImagePoints.insert(totalImagePoints.end(), imagePoints.begin(), imagePoints.end());

		const HomogenousMatrix4 poseIF(PinholeCamera::standard2InvertedFlipped(pose));
		for (ObjectPoints::const_iterator i = objectPoints.begin(); i != objectPoints.end(); ++i)
		{
			const ObjectPoint transformedObjectPoint(poseIF * *i);
			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));

			const Scalar factor = 1 / transformedObjectPoint.z();
			totalNormalizedObjectPoints.push_back(ImagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor));
		}

#ifdef OCEAN_DEBUG

		Scalar averageSqrError = 0;
		Scalar minimalSqrError = Numeric::maxValue();
		Scalar maximalSqrError = 0;
		Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, true>(pose, roughCamera, ConstTemplateArrayAccessor<Vector3>(objectPoints), ConstTemplateArrayAccessor<Vector2>(imagePoints), true, averageSqrError, minimalSqrError, maximalSqrError);

		ocean_assert(averageSqrError < 4 * 4 && maximalSqrError < 8 * 8);
#endif
	}

	ocean_assert(totalNormalizedObjectPoints.size() == totalImagePoints.size());

	if (totalNormalizedObjectPoints.empty())
	{
		ocean_assert(false && "This should not happen very often.");
		return false;
	}

	// define the initial camera
	pinholeCamera = roughCamera;

	HomogenousMatrices4 optimizedPoses;
	PinholeCamera optimizedCamera;
	Scalar initialSqrError = 0;
	Scalar finalSqrError = 0;

	if (!NonLinearOptimizationCamera::optimizeCamera(pinholeCamera, ConstArrayAccessor<Vector2>(totalNormalizedObjectPoints), ConstArrayAccessor<Vector2>(totalImagePoints), PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, optimizedCamera, 100u, Estimator::ET_HUBER, Scalar(0.001), 10, &initialSqrError, &finalSqrError))
	{
		return false;
	}

	pinholeCamera = optimizedCamera;

	if (sqrAccuracy != nullptr)
	{
		*sqrAccuracy = finalSqrError;
	}

	return true;
}

bool CameraCalibration::createCorrespondences(const Pattern& pattern, const Vector2& boxSize, ObjectPoints& objectPoints, ImagePoints& imagePoints)
{
	if (pattern.rows().empty())
	{
		return false;
	}

	const Pattern::PatternRows& rows = pattern.rows();

	imagePoints.reserve(rows.size() * rows.front().size());
	objectPoints.reserve(rows.size() * rows.front().size());

	for (size_t r = 0; r < rows.size(); ++r)
	{
		const Scalar y = -Scalar(r) * boxSize.y();

		const Pattern::PatternRow& row = pattern.rows()[r];

		for (size_t c = 0; c < row.size(); ++c)
		{
			const Scalar x = Scalar(c) * boxSize.x();

			objectPoints.push_back(Vector3(x, y, 0));
			imagePoints.push_back(row[c]);
		}
	}

	return true;
}

bool CameraCalibration::determineBestMatchingFovX(const unsigned int width, const unsigned int height, const ConstIndexedAccessor<HomogenousMatrix4>& posesAccessor, const ConstIndexedAccessor<Vectors3>& objectPointGroupAccessor, const ConstIndexedAccessor<Vectors2>& imagePointGroupAccessor, Scalar& idealFovX, const bool twoIterations, const Scalar lowestFovX, const Scalar highestFovX, const unsigned int steps, NonconstIndexedAccessor<HomogenousMatrix4>* idealPoses)
{
	ocean_assert(width > 0u && height > 0u);

	ocean_assert(posesAccessor.size() == objectPointGroupAccessor.size());
	ocean_assert(objectPointGroupAccessor.size() == imagePointGroupAccessor.size());

	ocean_assert(idealPoses == nullptr || idealPoses->size() == posesAccessor.size());

	ocean_assert(lowestFovX > 0 && lowestFovX < highestFovX);
	ocean_assert(highestFovX < Numeric::pi());

	ocean_assert(steps > 0u);

	if (width == 0u || height == 0u || posesAccessor.size() != objectPointGroupAccessor.size() || posesAccessor.size() != imagePointGroupAccessor.size() || lowestFovX <= 0 || highestFovX >= Numeric::pi() || lowestFovX >= highestFovX)
	{
		return false;
	}

	const Scalar fovXStep = (highestFovX - lowestFovX) / Scalar(steps);
	ocean_assert(fovXStep > 0);

	Scalar bestSqrError = Numeric::maxValue();
	Scalar bestFovX = Numeric::maxValue();

	Geometry::ImagePoints entireNormalizedImagePoints;
	Geometry::ImagePoints entireImagePoints;

	HomogenousMatrices4 individualPoses;
	individualPoses.resize(posesAccessor.size());

	if (twoIterations)
	{
		// determine the total number of point correspondences
		size_t correspondences = 0;
		for (size_t n = 0; n < objectPointGroupAccessor.size(); ++n)
		{
			correspondences += objectPointGroupAccessor[n].size();
		}

		entireNormalizedImagePoints.resize(correspondences);
		entireImagePoints.resize(correspondences);
	}

	for (Scalar fovX = lowestFovX; fovX <= highestFovX; fovX += fovXStep)
	{
		PinholeCamera pinholeCamera(width, height, fovX);

		Vector2* normalizedImagePointer = entireNormalizedImagePoints.data();
		Vector2* imagePointer = entireImagePoints.data();

		Scalar sqrError = 0;

		// first, the camera poses must be optimized for the test fov
		// further, the normalized image points and the corresponding image points are gathered
		for (size_t n = 0u; n < objectPointGroupAccessor.size(); ++n)
		{
			ocean_assert(posesAccessor[n].isValid());
			ocean_assert(objectPointGroupAccessor[n].size() == imagePointGroupAccessor[n].size());

			const HomogenousMatrix4& pose = posesAccessor[n];
			const Vectors3& objectPoints = objectPointGroupAccessor[n];
			const Vectors2& imagePoints = imagePointGroupAccessor[n];

			if (objectPoints.size() != imagePoints.size())
			{
				return false;
			}

			Scalar finalSqrError = 0;
			HomogenousMatrix4 optimizedPose;
			if (!Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, pose, ConstArrayAccessor<ObjectPoint>(objectPoints), ConstArrayAccessor<ImagePoint>(imagePoints), pinholeCamera.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(10), nullptr, &finalSqrError))
			{
				ocean_assert(false && "Pose error minimization failed!");
				return false;
			}

			individualPoses[n] = optimizedPose;
			sqrError += finalSqrError;

			if (twoIterations)
			{
				PinholeCamera::objectPoints2normalizedImagePoints(optimizedPose, objectPoints.data(), objectPoints.size(), normalizedImagePointer);
				memcpy(imagePointer, imagePoints.data(), sizeof(ImagePoint) * imagePoints.size());

				ocean_assert(objectPoints.size() == imagePoints.size());
				normalizedImagePointer += objectPoints.size();
				imagePointer += imagePoints.size();
			}
		}

		if (!twoIterations)
		{
			if (sqrError < bestSqrError)
			{
				bestSqrError = sqrError;
				bestFovX = fovX;

				if (idealPoses != nullptr)
				{
					for (size_t n = 0; n < posesAccessor.size(); ++n)
					{
						(*idealPoses)[n] = individualPoses[n];
					}
				}
			}

			continue;
		}

		ocean_assert(entireNormalizedImagePoints.size() == entireImagePoints.size());
		ocean_assert(!entireNormalizedImagePoints.empty());
		ocean_assert(!entireImagePoints.empty());

		PinholeCamera optimizedCamera;

		// optimize the camera parameters for all frames concurrently
		if (!Geometry::NonLinearOptimizationCamera::optimizeCamera(pinholeCamera, ConstArrayAccessor<Vector2>(entireNormalizedImagePoints), ConstArrayAccessor<Vector2>(entireImagePoints), PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, optimizedCamera, 100u, Geometry::Estimator::ET_SQUARE, Scalar(0.1), Scalar(10)))
		{
			ocean_assert(false && "The camera parameter optimization failed!");
			return false;
		}

		pinholeCamera = optimizedCamera;

		sqrError = 0;

		// optimize the pose for all frames individually
		for (size_t n = 0; n < individualPoses.size(); ++n)
		{
			const HomogenousMatrix4& pose = posesAccessor[n];
			const Vectors3& objectPoints = objectPointGroupAccessor[n];
			const Vectors2& imagePoints = imagePointGroupAccessor[n];

			Scalar finalSqrError = 0;
			HomogenousMatrix4 optimizedPose;
			if (!Geometry::NonLinearOptimizationPose::optimizePose(pinholeCamera, pose, ConstArrayAccessor<ObjectPoint>(objectPoints), ConstArrayAccessor<ImagePoint>(imagePoints), pinholeCamera.hasDistortionParameters(), optimizedPose, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(10), nullptr, &finalSqrError))
			{
				ocean_assert(false && "Pose error minimization failed!");
				return false;
			}

			individualPoses[n] = optimizedPose;
			sqrError += finalSqrError;
		}

		if (sqrError < bestSqrError)
		{
			bestSqrError = sqrError;
			bestFovX = fovX;

			if (idealPoses != nullptr)
			{
				for (size_t n = 0; n < posesAccessor.size(); ++n)
				{
					(*idealPoses)[n] = individualPoses[n];
				}
			}
		}
	}

	ocean_assert(bestFovX >= lowestFovX && bestFovX <= highestFovX);
	idealFovX = bestFovX;

	return true;
}

bool CameraCalibration::successiveCameraPoseOptimization(const PinholeCamera& pinholeCamera, const HomogenousMatrices4& poses, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, PinholeCamera& optimizedCamera, HomogenousMatrices4& optimizedPoses, Scalar* initialSqrError, Scalar* finalSqrError)
{
	ocean_assert(pinholeCamera);
	ocean_assert(poses.size() == objectPointGroups.size());
	ocean_assert(objectPointGroups.size() == imagePointGroups.size());

	if (pinholeCamera.width() == 0u || pinholeCamera.height() == 0u || poses.size() != objectPointGroups.size() || poses.size() != imagePointGroups.size())
		return false;

	if (initialSqrError)
		*initialSqrError = 0;

	Geometry::ImagePoints entireNormalizedImagePoints;
	Geometry::ImagePoints entireImagePoints;

	// determine the total number of point correspondences
	size_t correspondences = 0u;
	for (ObjectPointGroups::const_iterator i = objectPointGroups.begin(); i != objectPointGroups.end(); ++i)
		correspondences += i->size();

	entireNormalizedImagePoints.resize(correspondences);
	entireImagePoints.resize(correspondences);

	Vector2* normalizedImagePointer = entireNormalizedImagePoints.data();
	Vector2* imagePointer = entireImagePoints.data();

	// first the camera poses must be optimized for the test fov
	// further the normalized image points and the corresponding image points are gathered
	for (size_t n = 0u; n < objectPointGroups.size(); ++n)
	{
		ocean_assert(poses[n].isValid());
		ocean_assert(objectPointGroups[n].size() == imagePointGroups[n].size());

		const HomogenousMatrix4& pose = poses[n];
		const ObjectPoints& objectPoints = objectPointGroups[n];
		const ImagePoints& imagePoints = imagePointGroups[n];

		if (objectPoints.size() != imagePoints.size())
			return false;

		PinholeCamera::objectPoints2normalizedImagePoints(pose, objectPoints.data(), objectPoints.size(), normalizedImagePointer);
		memcpy(imagePointer, imagePoints.data(), sizeof(ImagePoint) * imagePoints.size());

		if (initialSqrError)
			*initialSqrError += Error::determineCameraError(pinholeCamera, normalizedImagePointer, imagePointer, objectPoints.size(), pinholeCamera.hasDistortionParameters());

		ocean_assert(objectPoints.size() == imagePoints.size());
		normalizedImagePointer += objectPoints.size();
		imagePointer += imagePoints.size();
	}

	if (initialSqrError && !objectPointGroups.empty())
		*initialSqrError /= Scalar(objectPointGroups.size());

	// optimize the camera parameters for all frames concurrently
	if (!Geometry::NonLinearOptimizationCamera::optimizeCamera(pinholeCamera, ConstArrayAccessor<Vector2>(entireNormalizedImagePoints), ConstArrayAccessor<Vector2>(entireImagePoints), PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, optimizedCamera, 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.1), Scalar(10)))
	{
		ocean_assert(false && "Failed to optimize the camera");
		return false;
	}

	optimizedPoses.resize(poses.size());

	Scalar sqrError = 0;

	// optimize the pose for all frames individually
	for (size_t n = 0; n < objectPointGroups.size(); ++n)
	{
		const HomogenousMatrix4& pose = poses[n];
		const ObjectPoints& objectPoints = objectPointGroups[n];
		const ImagePoints& imagePoints = imagePointGroups[n];

		Scalar setFinalSqrError = 0;
		if (!Geometry::NonLinearOptimizationPose::optimizePose(optimizedCamera, pose, ConstArrayAccessor<ObjectPoint>(objectPoints), ConstArrayAccessor<ImagePoint>(imagePoints), true, optimizedPoses[n], 20u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(10), nullptr, &setFinalSqrError))
		{
			ocean_assert(false && "Pose error minimization failed!");
			return false;
		}

		sqrError += setFinalSqrError;
	}

	if (finalSqrError && !objectPointGroups.empty())
		*finalSqrError = sqrError / Scalar(objectPointGroups.size());

	return true;
}

}

}
