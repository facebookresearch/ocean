/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_ADVANCED_POINT_TRACKER_H
#define META_OCEAN_TRACKING_POINT_ADVANCED_POINT_TRACKER_H

#include "ocean/tracking/point/Point.h"
#include "ocean/tracking/point/PosePointPair.h"
#include "ocean/tracking/point/Utilities.h"

#include "ocean/base/StaticVector.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"

#include "ocean/tracking/DataContainer.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

/**
 * This class implements an advanced point tracker.
 * @ingroup trackingpoint
 */
template <unsigned int tSuccessiveFrames>
class AdvancedPointTracker
{
	protected:

		class Candidate
		{
			protected:

				typedef StaticVector<unsigned int, tSuccessiveFrames> StaticPoseIndices;

				typedef StaticVector<Vector2, 10u> StaticVectors2;
				typedef StaticVector<StaticVectors2, tSuccessiveFrames> StaticVectors2Set;

				typedef StaticVector<Line2, 10u> StaticLines2;
				typedef StaticVector<StaticLines2, tSuccessiveFrames> StaticLines2Set;

				typedef StaticVector<Line3, 10u> StaticLines3;
				typedef StaticVector<StaticLines3, tSuccessiveFrames> StaticLines3Set;

			public:

				inline Candidate(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const Vector2& firstImagePoint, const bool useCameraDistortionParameters, const unsigned int firstPoseId);

				inline void replace(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const Vector2& firstImagePoint, const bool useCameraDistortionParameters, const unsigned int firstPoseId);

				inline bool isValid() const;

				PosePointPairsObject newPose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const HomogenousMatrix4& poseIF, const unsigned int poseId, const Vectors2& imagePoints, const bool useCameraDistortionParameters, const Geometry::SpatialDistribution::DistributionArray& imagePointsDistribution, const Scalar searchRadius, const Scalar sqrProjectionDistanceconst);

				inline void add2OccupancyArray(Geometry::SpatialDistribution::OccupancyArray& occupancyArray) const;

			//protected:

				StaticVectors2Set candidateSuccessiveImagePoints;

				StaticLines3Set candidateSuccessiveRays;

				StaticPoseIndices candidatePoseIds;

				bool candidateIsValid;
		};

		typedef std::vector<Candidate> Candidates;

	public:

		AdvancedPointTracker();

		PosePointPairsObjects newPose(const PinholeCamera& pinholeCamera, const unsigned int poseId, const Vectors2& imagePoints, const bool useCameraDistortionParameters, Geometry::SpatialDistribution::OccupancyArray* externalOccupancyArray = nullptr, const bool updateOccupancyArray = true);

	protected:

		Geometry::SpatialDistribution::OccupancyArray determineOccupancyArray(const PinholeCamera& pinholeCamera, const unsigned int horizontalBins, const unsigned int verticalBins);

	protected:

		Scalar trackerSearchRadius;

		Scalar trackerSqrProjectionDistance;

		Candidates trackerCandidates;

		Indices32 trackerInvalidCandidates;
};

template <unsigned int tSuccessiveFrames>
inline AdvancedPointTracker<tSuccessiveFrames>::Candidate::Candidate(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const Vector2& firstImagePoint, const bool useCameraDistortionParameters, const unsigned int firstPoseId) :
	candidatePoseIds(firstPoseId),
	candidateIsValid(true)
{
	ocean_assert(pinholeCamera.isValid() && firstPose.isValid());

	candidateSuccessiveImagePoints.weakResize(1);
	candidateSuccessiveImagePoints.back().pushBack(firstImagePoint);

	candidateSuccessiveRays.weakResize(1);

	if (useCameraDistortionParameters)
	{
		const Vector2 firstUndistortedImagePoint(pinholeCamera.undistort<true>(firstImagePoint));
		ocean_assert(firstImagePoint.sqrDistance(pinholeCamera.distort<true>(firstUndistortedImagePoint)) < 1);

		candidateSuccessiveRays.back().pushBack(pinholeCamera.ray(firstUndistortedImagePoint, firstPose));
	}
	else
		candidateSuccessiveRays.back().pushBack(pinholeCamera.ray(firstImagePoint, firstPose));
}

template <unsigned int tSuccessiveFrames>
inline void AdvancedPointTracker<tSuccessiveFrames>::Candidate::replace(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& firstPose, const Vector2& firstImagePoint, const bool useCameraDistortionParameters, const unsigned int firstPoseId)
{
	ocean_assert(!isValid());
	ocean_assert(pinholeCamera.isValid() && firstPose.isValid());

	ocean_assert(candidateSuccessiveImagePoints.size() == candidateSuccessiveRays.size());
	for (size_t n = 0; n < candidateSuccessiveImagePoints.size(); ++n)
	{
		candidateSuccessiveImagePoints[n].weakClear();
		candidateSuccessiveRays[n].weakClear();
	}

	candidateSuccessiveRays.weakResize(1);

	if (useCameraDistortionParameters)
	{
		const Vector2 firstUndistortedImagePoint(pinholeCamera.undistort<true>(firstImagePoint));
		ocean_assert(firstImagePoint.sqrDistance(pinholeCamera.distort<true>(firstUndistortedImagePoint)) < 1);

		candidateSuccessiveRays.front().pushBack(pinholeCamera.ray(firstUndistortedImagePoint, firstPose));
	}
	else
		candidateSuccessiveRays.front().pushBack(pinholeCamera.ray(firstImagePoint, firstPose));

	candidatePoseIds.weakResize(1);
	candidatePoseIds.front() = firstPoseId;

	candidateIsValid = true;

	candidateSuccessiveImagePoints.weakResize(1);
	candidateSuccessiveImagePoints.front().pushBack(firstImagePoint);
}

template <unsigned int tSuccessiveFrames>
inline bool AdvancedPointTracker<tSuccessiveFrames>::Candidate::isValid() const
{
	return candidateIsValid;
}

template <unsigned int tSuccessiveFrames>
PosePointPairsObject AdvancedPointTracker<tSuccessiveFrames>::Candidate::newPose(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const HomogenousMatrix4& poseIF, const unsigned int poseId, const Vectors2& imagePoints, const bool useCameraDistortionParameters, const Geometry::SpatialDistribution::DistributionArray& imagePointsDistribution, const Scalar searchRadius, const Scalar sqrProjectionDistance)
{
	ocean_assert(candidateIsValid);
	ocean_assert(pinholeCamera.isValid() && poseIF.isValid());
	ocean_assert(!candidateSuccessiveImagePoints.empty());
	ocean_assert(!candidateSuccessiveImagePoints.occupied());

	// find new point candidates according to the position of the previous point candidates
	const StaticVectors2& previousImagePoints = candidateSuccessiveImagePoints.back();

	StaticVector<unsigned int, 10u> currentCandidateIndices;

	for (size_t n = 0; n < previousImagePoints.size(); ++n)
	{
		const Indices32 indices(Geometry::SpatialDistribution::determineNeighbors(previousImagePoints[n], imagePoints.data(), (unsigned int)imagePoints.size(), searchRadius, imagePointsDistribution));

		for (Indices32::const_iterator i = indices.begin(); !currentCandidateIndices.occupied() && i != indices.end(); ++i)
		{
			bool found = false;

			for (size_t t = 0; t < currentCandidateIndices.size(); ++t)
				if (*i == currentCandidateIndices[t])
				{
					found = true;
					break;
				}

			if (!found)
				currentCandidateIndices.pushBack(*i);
		}
	}

	StaticVectors2 currentCandidates;

	for (size_t n = 0; n < currentCandidateIndices.size(); ++n)
		currentCandidates.pushBack(imagePoints[currentCandidateIndices[n]]);

	// projected the 3D rays from the previous frames into the camera by application of the current pose
	StaticLines2Set previousProjectedRays;
	previousProjectedRays.weakResize(candidateSuccessiveRays.size());

	for (size_t a = 0; a < candidateSuccessiveRays.size(); ++a)
	{
		previousProjectedRays[a].weakResize(candidateSuccessiveRays[a].size());

		for (size_t b = 0; b < candidateSuccessiveRays[a].size(); ++b)
			previousProjectedRays[a][b] = pinholeCamera.projectToImageIF<true>(poseIF, candidateSuccessiveRays[a][b], useCameraDistortionParameters);
	}

	// filter the point candidates according to previous projected rays
	for (size_t c = 0; c < currentCandidates.size(); /* noop */)
	{
		const Vector2& candidate = currentCandidates[c];

		size_t nearToLineCounter = 0;

		for (size_t a = 0; a < previousProjectedRays.size(); ++a)
			for (size_t b = 0; b < previousProjectedRays[a].size(); ++b)
			{
				const Line2& projectedRay = previousProjectedRays[a][b];

				if (projectedRay && projectedRay.sqrDistance(candidate) <= sqrProjectionDistance)
				{
					nearToLineCounter++;
					break;
				}
			}

		if (nearToLineCounter == previousProjectedRays.size())
			c++;
		else
			currentCandidates.unstableErase(c);
	}

	candidateSuccessiveImagePoints.pushBack(currentCandidates);

	// create 3D rays for the current candidates and pose

	candidateSuccessiveRays.weakResize(candidateSuccessiveRays.size() + 1);
	for (size_t n = 0; n < currentCandidates.size(); ++n)
	{
		const Vector2 undistortedCandidate(pinholeCamera.undistort<true>(currentCandidates[n]));
		ocean_assert(currentCandidates[n].sqrDistance(pinholeCamera.distort<true>(undistortedCandidate)) < 1);

		candidateSuccessiveRays.back().securePushBack(pinholeCamera.ray(undistortedCandidate, pose));
	}

	// filter previous projected rays (and their corresponding points) according to the just filtered point candidates

	ocean_assert(previousProjectedRays.size() + 1 == candidateSuccessiveImagePoints.size());
	ocean_assert(previousProjectedRays.size() >= 1);

	for (size_t a = 0; a < previousProjectedRays.size() - 1; ++a)
	{
		StaticLines2& projectedRays = previousProjectedRays[a];

		ocean_assert(projectedRays.size() == candidateSuccessiveImagePoints[a].size());

		for (size_t b = 0; b < projectedRays.size(); /* noop */)
		{
			const Line2& projectedRay = projectedRays[b];

			bool hasFound = false;

			for (size_t c = 0; c < currentCandidates.size(); ++c)
				if (projectedRay.sqrDistance(currentCandidates[c]) <= sqrProjectionDistance)
				{
					hasFound = true;
					break;
				}

			if (hasFound)
				b++;
			else
			{
				projectedRays.unstableErase(b);
				candidateSuccessiveRays[a].unstableErase(b);
				candidateSuccessiveImagePoints[a].unstableErase(b);
			}
		}

		// if the current level is empty, the entire candidate is invalidated
		if (projectedRays.empty())
		{
			candidateIsValid = false;
			return PosePointPairsObject();
		}
	}

	candidatePoseIds.pushBack(poseId);

#ifdef OCEAN_DEBUG

	const StaticVectors2& _imagePoints = candidateSuccessiveImagePoints.back();

	for (size_t a = 0; a + 1 < _imagePoints.size(); ++a)
		for (size_t b = a + 1; b < _imagePoints.size(); ++b)
			ocean_assert(_imagePoints[a] != _imagePoints[b]);
#endif

	// check whether enough poses have been investigated
	if (candidateSuccessiveImagePoints.size() >= tSuccessiveFrames)
	{
		for (unsigned int n = 0u; n < candidateSuccessiveImagePoints.size(); ++n)
			if (candidateSuccessiveImagePoints[n].size() != 1)
			{
				candidateIsValid = false;
				return PosePointPairsObject();
			}

		// we have enough unique successive image points to create an initial 3D position

		StaticVector<Line3, tSuccessiveFrames> rays;
		for (size_t n = 0; n < candidateSuccessiveRays.size(); ++n)
			rays.pushBack(candidateSuccessiveRays[n].front());

		Vector3 initialObjectPoint;

		if (!Utilities::determineInitialObjectPoint(pinholeCamera, rays.data(), (unsigned int)rays.size(), initialObjectPoint, true))
		{
			candidateIsValid = false;
			return PosePointPairsObject();
		}

		ocean_assert(candidatePoseIds.size() == candidateSuccessiveRays.size());

		DataContainer& dataContainer = DataContainer::get();

		StaticVector<HomogenousMatrix4, tSuccessiveFrames> posesIF;
		StaticVector<Vector2, tSuccessiveFrames> imagePoints;

		for (size_t n = 0; n < candidatePoseIds.size(); ++n)
		{
			ocean_assert(dataContainer.hasPose<true>(candidatePoseIds[n]));
			posesIF.pushBack(dataContainer.poseIF<true>(candidatePoseIds[n]));

			imagePoints.pushBack(candidateSuccessiveImagePoints[n].front());
		}

		Vector3 optimizedObjectPoint;
		Scalar finalError = 0;
		if (!Utilities::optimizeObjectPointIF(pinholeCamera, posesIF.data(), imagePoints.data(), (unsigned int)imagePoints.size(), initialObjectPoint, optimizedObjectPoint, &finalError) || finalError > Scalar(0.7 * 0.7))
		{
			candidateIsValid = false;
			return PosePointPairsObject();
		}

		PosePointPairs posePointPairs;
		posePointPairs.reserve(imagePoints.size());

		for (size_t n = 0; n < imagePoints.size(); ++n)
		{
			const unsigned int imageId = dataContainer.registerImagePoint<true>(imagePoints[n]);
			posePointPairs.push_back(PosePointPair(candidatePoseIds[n], imageId));
		}

		const unsigned int objectId = dataContainer.registerObjectPoint<true>(optimizedObjectPoint);

		candidateIsValid = false;
		return PosePointPairsObject(objectId, std::move(posePointPairs));
	}

#ifdef OCEAN_DEBUG
	ocean_assert(candidateSuccessiveImagePoints.size() == candidateSuccessiveRays.size());
	for (size_t n = 0; n < candidateSuccessiveImagePoints.size(); ++n)
		ocean_assert(candidateSuccessiveImagePoints[n].size() == candidateSuccessiveRays[n].size());
#endif

	return PosePointPairsObject();
}

template <unsigned int tSuccessiveFrames>
inline void AdvancedPointTracker<tSuccessiveFrames>::Candidate::add2OccupancyArray(Geometry::SpatialDistribution::OccupancyArray& occupancyArray) const
{
	ocean_assert(!candidateSuccessiveImagePoints.empty());

	for (size_t n = 0; n < candidateSuccessiveImagePoints.back().size(); ++n)
		occupancyArray += candidateSuccessiveImagePoints.back()[n];
}

template <unsigned int tSuccessiveFrames>
AdvancedPointTracker<tSuccessiveFrames>::AdvancedPointTracker() :
	trackerSearchRadius(Scalar(10)),
	trackerSqrProjectionDistance(Scalar(1.5 * 1.5))
{
	// nothing to do here
}

template <unsigned int tSuccessiveFrames>
PosePointPairsObjects AdvancedPointTracker<tSuccessiveFrames>::newPose(const PinholeCamera& pinholeCamera, const unsigned int poseId, const Vectors2& imagePoints, const bool useCameraDistortionParameters, Geometry::SpatialDistribution::OccupancyArray* externalOccupancyArray, const bool updateExternalOccupancyArray)
{
	ocean_assert(DataContainer::get().hasPose<true>(poseId));

	const HomogenousMatrix4 pose(DataContainer::get().pose<true>(poseId));
	const HomogenousMatrix4 poseIF(DataContainer::get().poseIF<true>(poseId));

	PosePointPairsObjects posePointPairsObjects;

	// try to retrack the current candidates
	if (!trackerCandidates.empty())
	{
		const Geometry::SpatialDistribution::DistributionArray imagePointsDistibution(Geometry::SpatialDistribution::distributeToArray<20u>(imagePoints.data(), (unsigned int)imagePoints.size(), Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), trackerSearchRadius));

		for (size_t n = 0; n < trackerCandidates.size(); ++n)
		{
			Candidate& candidate = trackerCandidates[n];

			if (candidate.isValid())
			{
				PosePointPairsObject posePointPairsObject(candidate.newPose(pinholeCamera, pose, poseIF, poseId, imagePoints, useCameraDistortionParameters, imagePointsDistibution, trackerSearchRadius, trackerSqrProjectionDistance));

				if (posePointPairsObject)
					posePointPairsObjects.push_back(std::move(posePointPairsObject));

				// check whether the candidate point has become invalid
				if (!candidate.isValid())
					trackerInvalidCandidates.push_back((unsigned int)n);
			}
		}
	}

	// internal occupancy array
	Geometry::SpatialDistribution::OccupancyArray internalOccupancyArray(determineOccupancyArray(pinholeCamera, 50u, 50u)); // **TODO**

	// try to add new candidates in free image areas

	if (trackerCandidates.empty())
		trackerCandidates.reserve(imagePoints.size());

	//Frame frame(FrameType(pinholeCamera.width(), pinholeCamera.height(), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	//memset(frame.data(), 0xFF, frame.size());

	if (externalOccupancyArray)
	{
		for (Vectors2::const_iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
			//if (!internalOccupancyArray(*i))
			if (!internalOccupancyArray.isOccupiedNeighborhood9(*i))
			//if (!internalOccupancyArray.isOccupiedNeighborhood9(*i))
			{
				/*const unsigned int externalHorizontalBin = externalOccupancyArray->horizontalBin(i->x());
				const unsigned int externalVerticalBin = externalOccupancyArray->verticalBin(i->y());

				unsigned char& isExternalOccupied = (*externalOccupancyArray)(externalHorizontalBin, externalVerticalBin);

				if (!isExternalOccupied)*/

				if (!externalOccupancyArray->isOccupiedNeighborhood9(*i))
				//if (!(*externalOccupancyArray)(*i))
				{
					//const unsigned char color[3] = {0x00, 0x00, 0x00};
					//CV::Canvas::ellipticRegion(frame, CV::PixelPosition(int(i->x()), int(i->y())), 7u, 7u, color);

					if (trackerInvalidCandidates.empty())
						trackerCandidates.push_back(Candidate(pinholeCamera, pose, *i, useCameraDistortionParameters, poseId));
					else
					{
						trackerCandidates[trackerInvalidCandidates.back()].replace(pinholeCamera, pose, *i, useCameraDistortionParameters, poseId);
						trackerInvalidCandidates.pop_back();
					}

					internalOccupancyArray += *i;

					//if (updateExternalOccupancyArray)
					//	isExternalOccupied = 1u;
				}
			}
	}
	else
	{
		for (Vectors2::const_iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
			if (!internalOccupancyArray.isOccupiedNeighborhood9(*i))
			{
				if (trackerInvalidCandidates.empty())
					trackerCandidates.push_back(Candidate(pinholeCamera, pose, *i, useCameraDistortionParameters, poseId));
				else
				{
					trackerCandidates[trackerInvalidCandidates.back()].replace(pinholeCamera, pose, *i, useCameraDistortionParameters, poseId);
					trackerInvalidCandidates.pop_back();
				}
			}
	}

	/*const unsigned char color[3] = {0x00, 0x00, 0x00};

	for (unsigned int y = 0u; y < externalOccupancyArray->verticalBins(); ++y)
		for (unsigned int x = 0u; x < externalOccupancyArray->horizontalBins(); ++x)
		{
			if ((*externalOccupancyArray)(x, y))
			{
				const int x0 = int(x) * int(pinholeCamera.width()) / int(externalOccupancyArray->horizontalBins());
				const int y0 = int(y) * int(pinholeCamera.height()) / int(externalOccupancyArray->verticalBins());

				const int x1 = int(x + 1) * int(pinholeCamera.width()) / int(externalOccupancyArray->horizontalBins());
				const int y1 = int(y + 1) * int(pinholeCamera.height()) / int(externalOccupancyArray->verticalBins());

				CV::Canvas::line(frame, x0, y0, x1, y1, color);
				CV::Canvas::line(frame, x1, y0, x0, y1, color);
			}
		}


	for (size_t n = 0; n < trackerCandidates.size(); ++n)
	{
		Candidate& candidate = trackerCandidates[n];

		if (candidate.isValid())
		{
			for (size_t n = 0; n < candidate.candidateSuccessiveImagePoints.back().size(); ++n)
				CV::Canvas::ellipticRegion(frame, CV::PixelPosition(int(candidate.candidateSuccessiveImagePoints.back()[n].x()), int(candidate.candidateSuccessiveImagePoints.back()[n].y())), 7u, 7u, color);
		}
	}*/

	//Platform::Win::Utilities::desktopFrameOutput(2000, 0, frame);
	//Sleep(1000);

	return posePointPairsObjects; // **TODO**
}

template <unsigned int tSuccessiveFrames>
Geometry::SpatialDistribution::OccupancyArray AdvancedPointTracker<tSuccessiveFrames>::determineOccupancyArray(const PinholeCamera& pinholeCamera, const unsigned int horizontalBins, const unsigned int verticalBins)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(horizontalBins >= 0u && verticalBins >= 0u);

	Geometry::SpatialDistribution::OccupancyArray occupancyArray(Scalar(0), Scalar(0), Scalar(pinholeCamera.width()), Scalar(pinholeCamera.height()), horizontalBins, verticalBins);

	for (Candidates::const_iterator i = trackerCandidates.begin(); i != trackerCandidates.end(); ++i)
		if (i->isValid())
			i->add2OccupancyArray(occupancyArray);

	return occupancyArray;
}

}

}

}

#endif // META_OCEAN_TRACKING_POINT_ADVANCED_POINT_TRACKER_H
