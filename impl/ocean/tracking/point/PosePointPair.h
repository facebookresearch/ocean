/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POINT_POSE_POINT_PAIR_H
#define META_OCEAN_TRACKING_POINT_POSE_POINT_PAIR_H

#include "ocean/tracking/point/Point.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/Database.h"

#include <vector>

namespace Ocean
{

namespace Tracking
{

namespace Point
{

// Forward declaration.
class PosePointPair;

/**
 * Definition of a vector holding pose point pairs.
 * @ingroup trackingpoint
 */
typedef std::vector<PosePointPair> PosePointPairs;

// Forward declaration.
class PosePointPairsObject;

/**
 * Definition of a vector holding pose objects.
 * @ingroup trackingpoint
 */
typedef std::vector<PosePointPairsObject> PosePointPairsObjects;

/**
 * Definition of a simple struct combining a pose and an image point id.
 * @ingroup trackingpoint
 */
class PosePointPair
{
	public:

		/**
		 * Creates an invalid pair.
		 */
		inline PosePointPair();

		/**
		 * Creates a new pair.
		 * @param poseId Id of the pose
		 * @param imagePointId Id of the image point
		 */
		inline PosePointPair(const unsigned int poseId, const unsigned int imagePointId);

		/**
		 * Returns the pose id of this pair.
		 * @return Pose id
		 */
		inline unsigned int poseId() const;

		/**
		 * Returns the image point id of this pair.
		 * @return Point id
		 */
		inline unsigned int imagePointId() const;

		/**
		 * Extracts the pose and the image point of this pair.
		 * @param database The database holding the entire tracking information
		 * @param pose Resulting pose of this pair
		 * @param imagePoint Resulting image points of this pair
		 */
		inline void extractPair(const Database& database, HomogenousMatrix4& pose, Vector2& imagePoint) const;

	private:

		/// Id of the pose.
		unsigned int pairPoseId;

		/// Id of th point.
		unsigned int pairImagePointId;
};

/**
 * Definition of an object that combines an object id with the pairs of poses and image points.
 * @ingroup trackingpoint
 */
class OCEAN_TRACKING_POINT_EXPORT PosePointPairsObject
{
	public:

		/**
		 * Creates an invalid object.
		 */
		inline PosePointPairsObject();

		/**
		 * Creates a new object.
		 * @param objectId Id of the object
		 * @param posePointPairs Pose point pairs
		 */
		inline PosePointPairsObject(const unsigned int objectId, const PosePointPairs& posePointPairs);

		/**
		 * Move constructor.
		 * @param object Object to be moved
		 */
		inline PosePointPairsObject(PosePointPairsObject&& object);

		/**
		 * Creates a new object.
		 * @param objectId Id of the object
		 * @param posePointPairs Pose point pairs
		 */
		inline PosePointPairsObject(const unsigned int objectId, PosePointPairs&& posePointPairs);

		/**
		 * Returns the id of the object point of this object.
		 * @return Object point id
		 */
		inline unsigned int objectId() const;

		/**
		 * Returns the number of poses and corresponding image points that object covers.
		 * @return Number of poses and corresponding image points
		 */
		inline unsigned int size() const;

		/**
		 * Returns the failure counter.
		 * @return Failure counter
		 */
		inline unsigned int failureCounter() const;

		/**
		 * Increments the failure counter by one.
		 */
		inline void incrementFailure();

		/**
		 * Resets the failure counter to zero.
		 */
		inline void resetFailure();

		/**
		 * Returns the pose point pairs of this objects.
		 * @return Pose point pairs
		 */
		inline const PosePointPairs& posePointPairs() const;

		/**
		 * Adds a new pose point pair to this object.
		 * @param posePointPair Pose point pair to be added
		 */
		inline void addPosePointPair(const PosePointPair& posePointPair);

		/**
		 * Adds a new pose point pair to this object.
		 * @param poseId Id of the pose to be added
		 * @param imagePointId Id of the image point to be added
		 */
		inline void addPosePointPair(const unsigned int poseId, const unsigned int imagePointId);

		/**
		 * Returns the poses of this object.
		 * @param database The database holding the entire tracking information
		 * @return Object poses
		 */
		inline HomogenousMatrices4 poses(const Database& database) const;

		/**
		 * Returns the inverted and flipped poses of this object.
		 * @param database The database holding the entire tracking information
		 * @return Inverted and flipped poses
		 */
		inline HomogenousMatrices4 posesIF(const Database& database) const;

		/**
		 * Returns the image points of this object.
		 * @param database The database holding the entire tracking information
		 * @return Image points
		 */
		inline Vectors2 imagePoints(const Database& database) const;

		/**
		 * Returns the object point of this object.
		 * @param database The database holding the entire tracking information
		 * @return Object point
		 */
		inline Vector3 objectPoint(const Database& database) const;

		/**
		 * Extracts the poses, the image points and the object point of this object.
		 * @param database The database holding the entire tracking information
		 * @param poses Resulting poses
		 * @param imagePoints Resulting image points
		 * @param objectPoint Resulting object point
		 */
		inline void extractObject(const Database& database, HomogenousMatrices4& poses, Vectors2& imagePoints, Vector3& objectPoint);

		/**
		 * Extracts the poses, the image points and the object point of this object.
		 * @param database The database holding the entire tracking information
		 * @param posesIF Resulting inverted and flipped poses
		 * @param imagePoints Resulting image points
		 * @param objectPoint Resulting object point
		 */
		inline void extractObjectIF(const Database& database, HomogenousMatrices4& posesIF, Vectors2& imagePoints, Vector3& objectPoint);

		/**
		 * Optimizes the object point according to the pose and image points of this object
		 * @param database The database holding the entire tracking information
		 * @param pinholeCamera The pinhole camera object
		 * @param useCameraDistortionParameters True, to apply the camera distortion parameters
		 * @return True, if succeeded
		 */
		bool optimizeObjectPoint(Database& database, const PinholeCamera& pinholeCamera , const bool useCameraDistortionParameters);

		/**
		 * Invalidates this object.
		 * The object id is invalidated.
		 */
		inline void invalidate();

		/**
		 * Returns whether this object holds a valid id of an object point.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Assign operator.
		 * @param object Object to be moved
		 * @return Reference to this object
		 */
		inline PosePointPairsObject& operator=(PosePointPairsObject&& object);

		/**
		 * Returns whether this object holds a valid id of an object point.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// Id of the object.
		unsigned int dataObjectId;

		/// Pose point pairs.
		PosePointPairs dataPosePointPairs;

		/// Failure counter.
		unsigned int dataFailureCounter;
};

inline PosePointPair::PosePointPair() :
	pairPoseId((unsigned int)(-1)),
	pairImagePointId((unsigned int)(-1))
{
	// nothing to do here
}

inline PosePointPair::PosePointPair(const unsigned int poseId, const unsigned int imagePointId) :
	pairPoseId(poseId),
	pairImagePointId(imagePointId)
{
	// nothing to do here
}

inline unsigned int PosePointPair::poseId() const
{
	return pairPoseId;
}

inline unsigned int PosePointPair::imagePointId() const
{
	return pairImagePointId;
}

inline void PosePointPair::extractPair(const Database& database, HomogenousMatrix4& pose, Vector2& imagePoint) const
{
	ocean_assert(database.hasPose<false>(pairPoseId));

	pose = database.pose<false>(pairPoseId);
	imagePoint = database.imagePoint<false>(pairImagePointId);
}

inline PosePointPairsObject::PosePointPairsObject() :
	dataObjectId((unsigned int)(-1)),
	dataFailureCounter(0u)
{
	// nothing to do here
}

inline PosePointPairsObject::PosePointPairsObject(const unsigned int objectId, const PosePointPairs& posePointPairs) :
	dataObjectId(objectId),
	dataPosePointPairs(posePointPairs),
	dataFailureCounter(0u)
{
	// nothing to do here
}

inline PosePointPairsObject::PosePointPairsObject(PosePointPairsObject&& object) :
	dataObjectId(object.dataObjectId),
	dataPosePointPairs(std::move(object.dataPosePointPairs)),
	dataFailureCounter(object.dataFailureCounter)
{
	object.dataObjectId = (unsigned int)(-1);
}

inline PosePointPairsObject::PosePointPairsObject(const unsigned int newObjectId, PosePointPairs&& newPosePointPairs) :
	dataObjectId(newObjectId),
	dataPosePointPairs(newPosePointPairs),
	dataFailureCounter(0u)
{
	// nothing to do here
}

inline unsigned int PosePointPairsObject::objectId() const
{
	return dataObjectId;
}

inline unsigned int PosePointPairsObject::size() const
{
	return (unsigned int)dataPosePointPairs.size();
}

inline unsigned int PosePointPairsObject::failureCounter() const
{
	return dataFailureCounter;
}

inline void PosePointPairsObject::incrementFailure()
{
	dataFailureCounter++;
}

inline void PosePointPairsObject::resetFailure()
{
	dataFailureCounter = 0u;
}

inline const PosePointPairs& PosePointPairsObject::posePointPairs() const
{
	return dataPosePointPairs;
}

void inline PosePointPairsObject::addPosePointPair(const PosePointPair& posePointPair)
{
	dataPosePointPairs.push_back(posePointPair);
}

void inline PosePointPairsObject::addPosePointPair(const unsigned int poseId, const unsigned int imagePointId)
{
	dataPosePointPairs.push_back(PosePointPair(poseId, imagePointId));
}

inline HomogenousMatrices4 PosePointPairsObject::poses(const Database& database) const
{
	HomogenousMatrices4 result;
	result.reserve(dataPosePointPairs.size());

	for (PosePointPairs::const_iterator i = dataPosePointPairs.begin(); i != dataPosePointPairs.end(); ++i)
		result.push_back(database.pose<false>(i->poseId()));

	return result;
}

inline HomogenousMatrices4 PosePointPairsObject::posesIF(const Database& database) const
{
	return PinholeCamera ::standard2InvertedFlipped(poses(database));
}

inline Vectors2 PosePointPairsObject::imagePoints(const Database& database) const
{
	Vectors2 result;
	result.reserve(dataPosePointPairs.size());

	for (PosePointPairs::const_iterator i = dataPosePointPairs.begin(); i != dataPosePointPairs.end(); ++i)
		result.push_back(database.imagePoint<false>(i->imagePointId()));

	return result;
}

inline Vector3 PosePointPairsObject::objectPoint(const Database& database) const
{
	return database.objectPoint<true>(dataObjectId);
}

inline void PosePointPairsObject::extractObject(const Database& database, HomogenousMatrices4& poses, Vectors2& imagePoints, Vector3& objectPoint)
{
	ocean_assert(poses.empty());
	ocean_assert(imagePoints.empty());

	poses.reserve(dataPosePointPairs.size());
	for (PosePointPairs::const_iterator i = dataPosePointPairs.begin(); i != dataPosePointPairs.end(); ++i)
		poses.push_back(database.pose<false>(i->poseId()));

	imagePoints.reserve(dataPosePointPairs.size());
	for (PosePointPairs::const_iterator i = dataPosePointPairs.begin(); i != dataPosePointPairs.end(); ++i)
		imagePoints.push_back(database.imagePoint<false>(i->imagePointId()));

	objectPoint = database.objectPoint<true>(dataObjectId);
}

inline void PosePointPairsObject::extractObjectIF(const Database& database, HomogenousMatrices4& posesIF, Vectors2& imagePoints, Vector3& objectPoint)
{
	ocean_assert(posesIF.empty());
	ocean_assert(imagePoints.empty());

	posesIF = PinholeCamera ::standard2InvertedFlipped(poses(database));

	imagePoints.reserve(dataPosePointPairs.size());
	for (PosePointPairs::const_iterator i = dataPosePointPairs.begin(); i != dataPosePointPairs.end(); ++i)
		imagePoints.push_back(database.imagePoint<false>(i->imagePointId()));

	objectPoint = database.objectPoint<true>(dataObjectId);
}

inline void PosePointPairsObject::invalidate()
{
	dataObjectId = (unsigned int)(-1);
}

inline bool PosePointPairsObject::isValid() const
{
	return dataObjectId != (unsigned int)(-1);
}

inline PosePointPairsObject& PosePointPairsObject::operator=(PosePointPairsObject&& object)
{
	if (this != &object)
	{
		dataObjectId = object.dataObjectId;
		dataPosePointPairs = std::move(object.dataPosePointPairs);
		dataFailureCounter = object.dataFailureCounter;

		object.dataObjectId = (unsigned int)(-1);
	}

	return *this;
}

inline PosePointPairsObject::operator bool() const
{
	return isValid();
}

}

}

}

#endif // META_OCEAN_TRACKING_POINT_POSE_POINT_PAIR_H
