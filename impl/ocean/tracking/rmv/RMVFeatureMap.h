/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_RMV_RMV_FEATURE_MAP_H
#define META_OCEAN_TRACKING_RMV_RMV_FEATURE_MAP_H

#include "ocean/tracking/rmv/RMV.h"
#include "ocean/tracking/rmv/RMVFeatureDetector.h"

#include "ocean/base/Frame.h"

#include "ocean/math/Box3.h"
#include "ocean/math/PinholeCamera.h"

#include <vector>

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

/**
 * This class implements a feature map.
 * @ingroup trackingrmv
 */
class OCEAN_TRACKING_RMV_EXPORT RMVFeatureMap
{
	public:

		/**
		 * Creates an empty feature map.
		 */
		RMVFeatureMap();

		/**
		 * Returns the object positions of the registered map features.
		 * @return Object positions
		 * @see initializationObjectPoints().
		 */
		inline const Vectors3& objectPoints() const;

		/**
		 * Returns the object points to be used during initialization iterations.
		 * If no explicit initialization object points are defined the standard feature map object points are returned.
		 * @return Initialization object points
		 * @see objectPoints().
		 */
		inline const Vectors3& initializationObjectPoints() const;

		/**
		 * Returns the indices of the most recently strong object points.
		 * @return Indices of the most recently strong object points
		 */
		inline const Indices32& recentStrongObjectPointIndices() const;

		/**
		 * Returns the indices of the most recently semi-strong object points.
		 * @return Indices of the most recently semi-strong object points
		 */
		inline const Indices32& recentSemiStrongObjectPointIndices() const;

		/**
		 * Returns the indices of the most recently used object points.
		 * @return Indices of the most recently used object points
		 */
		inline const Indices32& recentUsedObjectPointIndices() const;

		/**
		 * Returns the most recently used object points.
		 * Additional this function can return a maximal amount of object points only.
		 * @param maxNumber The maximal number of requested object points, with range [1, infinity)
		 * @return Most recently used object points
		 */
		inline Vectors3 recentUsedObjectPoints(const size_t maxNumber = size_t(-1)) const;

		/**
		 * Sets or changes the indices of the most recently object points.
		 * @param strongObjectPointIndices Indices of all strong object points, may be a subset of the used object points, with ascending order
		 * @param semiStrongObjectPointIndices Indices of all semi-strong object points, may be a subset of the used object points, the intersection with the strong features must be empty, with ascending order
		 * @param usedObjectPointIndices Indices of all (valid and therefore) use object points, with ascending order
		 */
		inline void setMostRecentObjectPointIndices(Indices32&& strongObjectPointIndices, Indices32&& semiStrongObjectPointIndices, Indices32&& usedObjectPointIndices);

		/**
		 * Removes / clears the indices of the most recently object points.
		 * The indices of all strong, semi-strong and used object points will be released.<br>
		 */
		inline void clearMostRecentObjectPointIndices();

		/**
		 * Returns the bounding box of this map.
		 * @return Bounding box
		 * @see initializationBoundingBox().
		 */
		inline const Box3& boundingBox() const;

		/**
		 * Returns the initialization camera if defined.
		 * @return Initialization camera object
		 */
		inline const PinholeCamera& initializationCamera() const;

		/**
		 * Returns the detector type for the normal feature map.
		 * @return Detector type
		 */
		inline RMVFeatureDetector::DetectorType detectorType() const;

		/**
		 * Returns the detector type for the initialization features.
		 * @return Initialization detector type
		 */
		inline RMVFeatureDetector::DetectorType initializationDetectorType() const;

		/**
		 * Returns the bounding box of the initialization feature map.
		 * The bounding box covers all strong features used for initialization.<br>
		 * If no explicit initialization features are specified the bounding box of the standard features is returned.<br>
		 * @return Initialization bounding box
		 * @see boundingBox().
		 */
		inline const Box3& initializationBoundingBox() const;

		/**
		 * Sets or replaces the features for this feature map by a given set of 3D features points.
		 * @param points The points to be used as new feature map, can be nullptr if number is zero
		 * @param number The number of features to be set, with range [0, infinity)
		 * @param pinholeCamera The pinhole camera profile which will be used during the tracking
		 * @param detectorType The type of the detector which has been used to detect the provided feature points
		 */
		void setFeatures(const Vector3* points, const size_t number, const PinholeCamera& pinholeCamera, const RMVFeatureDetector::DetectorType detectorType);

		/**
		 * Sets or replaces the features for this feature map by a given tracking pattern.
		 * @param pattern The tracking pattern from which unique and strong feature points will be extracted and stored as feature map, must be valid
		 * @param dimension The dimension of the pattern in the world coordinate system, with range (0, infinity)x[0, infinity)x[0]
		 * @param pinholeCamera The pinhole camera profile which will be used during tracking
		 * @param numberFeatures The maximal number of feature points which will be extracted from the provided pattern, with range [10, infinity)
		 * @param detectorType Detector type the feature points has been detected with
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool setFeatures(const Frame& pattern, const Vector3& dimension, const PinholeCamera& pinholeCamera, const size_t numberFeatures, const RMVFeatureDetector::DetectorType detectorType, Worker* worker = nullptr);

		/**
		 * Sets or replaces the initialization features for this feature map by a given set of 3D features points.
		 * These features are used for initialization only and thus should be a small subset of strong features.<br>
		 * If no initialization features are specified the normal feature map points are used during initialization.
		 * @param objectPoints The 3D object points to be used as new feature map during initialization iterations, can be nullptr if 'number' is zero
		 * @param number The number of initialization features to be set, with range [0, infinity)
		 * @param initializationCamera Specific initialization camera
		 * @param initializationDetectorType Specific detector type for the initialization
		 */
		void setInitializationFeatures(const Vector3* objectPoints, const size_t number, const PinholeCamera& initializationCamera, const RMVFeatureDetector::DetectorType initializationDetectorType);

		/**
		 * Sets or replaces the initialization features for this feature map by a given set of 3D features points.
		 * These features are used for initialization only and thus should be a small subset of strong features.<br>
		 * If no initialization features are specified the normal feature map points are used during initialization.
		 * @param objectPoints The 3D object points to be used as new feature map during initialization iterations, will be moved
		 * @param initializationCamera Specific initialization camera
		 * @param initializationDetectorType Specific detector type for the initialization
		 */
		void setInitializationFeatures(Vectors3&& objectPoints, const PinholeCamera& initializationCamera, const RMVFeatureDetector::DetectorType initializationDetectorType);

		/**
		 * Sets or replaces the features that will be used during the initialization only for this feature map by a given tracking pattern.
		 * @param pattern The pattern frame the tracker will detect and track, must be valid
		 * @param dimension The dimension of the pattern in the world coordinate system, with range (0, infinity)x[0, infinity)x[0]
		 * @param pinholeCamera The pinhole camera profile which will be used during the tracking
		 * @param numberInitializationObjectPoints The number of 3D object points that will be used during the initialization, with range [1, infinity)
		 * @param initializationDetectorType The detector type which will be used during the initialization, may be different from the detector type which will be used after a successful initialization
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool setInitializationFeatures(const Frame& pattern, const Vector3& dimension, const PinholeCamera& pinholeCamera, const size_t numberInitializationObjectPoints, const RMVFeatureDetector::DetectorType& initializationDetectorType, Worker* worker = nullptr);

		/**
		 * Removes all registered map feature points.
		 */
		void clear();

		/**
		 * Returns whether this feature map holds no feature points.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this feature map holds at least one feature point.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// Object points defining the feature map.
		Vectors3 mapObjectPoints;

		/// Object points defining the initialization feature map.
		Vectors3 mapInitializationObjectPoints;

		/// Indices of the strongest object points from the most recently tracking iteration.
		Indices32 mapRecentStrongObjectPointIndices;

		/// Indices of the semi-strongest object points from the most recently tracking iteration.
		Indices32 mapRecentSemiStrongObjectPointIndices;

		/// Indices of the used object points from the most recently tracking iteration.
		Indices32 mapRecentUsedObjectPointIndices;

		/// Bounding box covering all feature points.
		Box3 mapBoundingBox;

		/// Bounding box covering all initialization feature points, if existing.
		Box3 mapInitializationBoundingBox;

		/// Standard camera.
		PinholeCamera mapCamera;

		/// PinholeCamera object explicitly used for camera initialization, if defined.
		PinholeCamera mapInitializationCamera;

		/// Detector type used for the normal feature map features.
		RMVFeatureDetector::DetectorType mapDetectorType;

		/// Detector type explicitly used for initialization features.
		RMVFeatureDetector::DetectorType mapInitializationDetectorType;
};

inline const Vectors3& RMVFeatureMap::objectPoints() const
{
	return mapObjectPoints;
}

inline const Vectors3& RMVFeatureMap::initializationObjectPoints() const
{
	return mapInitializationObjectPoints.empty() ? mapObjectPoints : mapInitializationObjectPoints;
}

inline const Indices32& RMVFeatureMap::recentStrongObjectPointIndices() const
{
	return mapRecentStrongObjectPointIndices;
}

inline const Indices32& RMVFeatureMap::recentSemiStrongObjectPointIndices() const
{
	return mapRecentSemiStrongObjectPointIndices;
}

inline const Indices32& RMVFeatureMap::recentUsedObjectPointIndices() const
{
	ocean_assert(mapRecentUsedObjectPointIndices.size() < mapObjectPoints.size());
	return mapRecentUsedObjectPointIndices;
}

inline Vectors3 RMVFeatureMap::recentUsedObjectPoints(const size_t maxNumber) const
{
	ocean_assert(maxNumber != 0);

	const size_t number = min(mapRecentUsedObjectPointIndices.size(), maxNumber);

	Vectors3 result(number);

	for (size_t n = 0; n < number; ++n)
	{
		ocean_assert(mapRecentUsedObjectPointIndices[n] < mapObjectPoints.size());
		result[n] = mapObjectPoints[mapRecentUsedObjectPointIndices[n]];
	}

	return result;
}

inline void RMVFeatureMap::setMostRecentObjectPointIndices(Indices32&& strongObjectPointIndices, Indices32&& semiStrongObjectPointIndices, Indices32&& usedObjectPointIndices)
{

#ifdef OCEAN_DEBUG

	// check whether the given strong and semi-strong features have no index in common
	for (Indices32::const_iterator i = strongObjectPointIndices.begin(); i != strongObjectPointIndices.end(); ++i)
	{
		bool found = false;

		for (Indices32::const_iterator iS = semiStrongObjectPointIndices.begin(); iS != semiStrongObjectPointIndices.end(); ++iS)
			if (*i == *iS)
			{
				found = true;
				break;
			}

		ocean_assert(!found);
	}

	// check whether the order of the index is correct

	for (size_t n = 1; n < strongObjectPointIndices.size(); ++n)
		ocean_assert(strongObjectPointIndices[n - 1] < strongObjectPointIndices[n]);

	for (size_t n = 1; n < semiStrongObjectPointIndices.size(); ++n)
		ocean_assert(semiStrongObjectPointIndices[n - 1] < semiStrongObjectPointIndices[n]);

	for (size_t n = 1; n < usedObjectPointIndices.size(); ++n)
		ocean_assert(usedObjectPointIndices[n - 1] < usedObjectPointIndices[n]);

#endif // OCEAN_DEBUG

	mapRecentStrongObjectPointIndices = std::move(strongObjectPointIndices);
	mapRecentSemiStrongObjectPointIndices = std::move(semiStrongObjectPointIndices);
	mapRecentUsedObjectPointIndices = std::move(usedObjectPointIndices);
}

inline void RMVFeatureMap::clearMostRecentObjectPointIndices()
{
	mapRecentStrongObjectPointIndices.clear();
	mapRecentSemiStrongObjectPointIndices.clear();
	mapRecentUsedObjectPointIndices.clear();
}

inline const Box3& RMVFeatureMap::boundingBox() const
{
	return mapBoundingBox;
}

inline const PinholeCamera& RMVFeatureMap::initializationCamera() const
{
	ocean_assert(mapCamera || mapInitializationCamera);
	return mapInitializationCamera ? mapInitializationCamera : mapCamera;
}

inline RMVFeatureDetector::DetectorType RMVFeatureMap::initializationDetectorType() const
{
	return mapInitializationDetectorType;
}

inline RMVFeatureDetector::DetectorType RMVFeatureMap::detectorType() const
{
	return mapDetectorType;
}

inline const Box3& RMVFeatureMap::initializationBoundingBox() const
{
	return mapInitializationBoundingBox.isValid() ? mapInitializationBoundingBox : mapBoundingBox;
}

inline bool RMVFeatureMap::isEmpty() const
{
	return mapObjectPoints.empty();
}

inline RMVFeatureMap::operator bool() const
{
	return !mapObjectPoints.empty();
}

}

}

}

#endif // META_OCEAN_TRACKING_RMV_RMV_FEATURE_MAP_H
