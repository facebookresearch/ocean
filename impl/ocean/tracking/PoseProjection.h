/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_POSE_PROJECTION_H
#define META_OCEAN_TRACKING_POSE_PROJECTION_H

#include "ocean/tracking/Tracker.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/detector/PointFeature.h"

#include "ocean/geometry/Error.h"

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/Estimator.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include <algorithm>

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a pose projection.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT PoseProjection
{
	public:

		/**
		 * Creates an empty pose projection object.
		 */
		PoseProjection();

		/**
		 * Creates a new pose projection object by a given pose and object points.
		 * @param pose Pose used to project the object points to the image plane
		 * @param pinholeCamera The pinhole camera object defining the projection
		 * @param objectPoints Object points to be projected
		 * @param number Number Of object points to be projected
		 * @param distortImagePoints Distorts the image points after projection if True
		 */
		PoseProjection(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Geometry::ObjectPoint* objectPoints, const size_t number, const bool distortImagePoints);

		/**
		 * Returns the pose of this projection.
		 * @return Projection pose
		 */
		inline const HomogenousMatrix4& pose() const;

		/**
		 * Returns the image points (the projected object points) of this pose projection.
		 * @return Pose image points
		 */
		inline const Geometry::ImagePoints& imagePoints() const;

		/**
		 * Returns the distortion state of this projection.
		 * @return Distortion state
		 */
		inline CV::Detector::PointFeature::DistortionState distortionState() const;

		/**
		 * Returns the number of stored pose points.
		 * @return Pose points
		 */
		inline size_t size() const;

		/**
		 * Returns the robust minimal average square error between this pose projection and a given 2D point cloud.
		 * The number of given image points must be equal or small to the number of internal pose points of this object.
		 * @param imagePoints Image points to find corresponding pose points for
		 * @param numberImagePoints Number of given image points
		 * @param validImagePoints Number of valid image points expecting to have corresponding points in the pose points, with range [1, numberImagePoints]
		 * @param errorDetermination Depending on this flag uniqueAveragedRobustErrorInPointCloud, approximatedAveragedRobustErrorInPointCloud() or ambiguousAveragedRobustErrorInPointCloud() will be used
		 * @return Resulting minimal average square error
		 * @tparam tEstimator Estimator to be applied
		 */
		template <Geometry::Estimator::EstimatorType tEstimator>
		Scalar minimalAverageSquareError(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination);

		/**
		 * Returns whether this pose projection holds no points.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this pose projection holds at least one point.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/**
		 * Converts an element to an object point.
		 * @param element Element to be converted
		 * @return Converted object point
		 */
		static inline const Vector3& objectPoint2objectPoint(const Geometry::ObjectPoint& element);

	private:

		/// Pose of this projection.
		HomogenousMatrix4 poseTransformation;

		/// Projection object points for this pose.
		Geometry::ImagePoints poseImagePoints;

		/// Distortion state of the image points.
		CV::Detector::PointFeature::DistortionState poseDistortionState;
};

/**
 * This class implements a set of pose projections.
 * @ingroup tracking
 */
 class OCEAN_TRACKING_EXPORT PoseProjectionSet
 {
	public:

		/**
		 * Definition of a vector holding pose projections.
		 */
		typedef std::vector<PoseProjection> PoseProjections;

		/**
		 * This class implements a error object.
		 */
		class ErrorObject
		{
			public:

				/**
				 * Creates a new error object.
				 * @param index Index of the element
				 * @param error Error of the element
				 */
				inline ErrorObject(const unsigned int index, const Scalar error);

				/**
				 * Returns the index of this object.
				 * @return Object index
				 */
				inline unsigned int index() const;

				/**
				 * Returns the error of this object.
				 * @return Object error
				 */
				inline Scalar error() const;

				/**
				 * Returns whether the left object has a lesser error value than the right one.
				 * @param object Right error object to compare
				 * @return True, if so
				 */
				inline bool operator<(const ErrorObject& object) const;

			private:

				/// Object index.
				unsigned int objectIndex;

				/// Object error.
				Scalar objectError;
		};

		/**
		 * Definition of a vector holding error objects.
		 */
		typedef std::vector<ErrorObject> ErrorObjects;

	public:

		/**
		 * Creates an empty set of pose projections.
		 */
		PoseProjectionSet();

		/**
		 * Destructs a set of pose projections.
		 */
		~PoseProjectionSet();

		/**
		 * Returns the width of the camera in pixel used for all projections.
		 * @return The camera width in pixels
		 * @see height().
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the camera in pixel used for all projections.
		 * @return The camera height in pixels
		 * @see width().
		 */
		inline unsigned int height() const;

		/**
		 * Adds a new pose projection.
		 * @param poseProjection Pose projection to be added
		 */
		inline void addPoseProjection(const PoseProjection& poseProjection);

		/**
		 * Returns the registered pose projections.
		 * @return Pose projections
		 */
		inline const PoseProjections& poseProjections() const;

		/**
		 * Returns the number of registered pose projections.
		 * @return Pose projection number
		 */
		inline size_t size() const;

		/**
		 * Sets the dimension of the camera used for all pose projections.
		 * @param width The camera width in pixel
		 * @param height The camera height in pixel
		 * @see width(), height().
		 */
		inline void setDimension(const unsigned int width, const unsigned int height);

		/**
		 * Clears the projection set.
		 */
		void clear();

		/**
		 * Returns the pose with the minimal distance error.
		 * The number of given image points must be equal or small to the number of internal pose points of this object.
		 * @param imagePoints Image points to find corresponding pose points for
		 * @param numberImagePoints Number of given image points
		 * @param validImagePoints Number of valid image points expecting to have corresponding points in the pose points, with range [1, numberImagePoints]
		 * @param errorDetermination Depending on this flag uniqueAveragedRobustErrorInPointCloud, approximatedAveragedRobustErrorInPointCloud() or ambiguousAveragedRobustErrorInPointCloud() will be used
		 * @param resultingError Optional resulting minimal average square error
		 * @param worker Optional worker object to distribute the computation
		 * @return Pose with minimal error
		 * @tparam tEstimator Estimator to be applied
		 */
		template <Geometry::Estimator::EstimatorType tEstimator>
		inline HomogenousMatrix4 findPoseWithMinimalError(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination, Scalar* resultingError = nullptr, Worker* worker = nullptr);

		/**
		 * Returns the poses with the minimal distance error.
		 * The number of given image points must be equal or small to the number of internal pose points of this object.
		 * @param imagePoints Image points to find corresponding pose points for
		 * @param numberImagePoints Number of given image points
		 * @param validImagePoints Number of valid image points expecting to have corresponding points in the pose points, with range [1, numberImagePoints]
		 * @param errorDetermination Depending on this flag uniqueAveragedRobustErrorInPointCloud, approximatedAveragedRobustErrorInPointCloud() or ambiguousAveragedRobustErrorInPointCloud() will be used
		 * @param numberPoses Number of poses to be found
		 * @param poses Resulting poses, make sure that the memory block provides enough space
		 * @param resultingErrors Optional resulting minimal average square errors individual for each returned pose, make sure that the memory block provides enough space
		 * @param worker Optional worker object to distribute the computation
		 * @return Number of found best poses
		 * @tparam tEstimator Estimator to be applied
		 */
		template <Geometry::Estimator::EstimatorType tEstimator>
		unsigned int findPosesWithMinimalError(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination, const size_t numberPoses, HomogenousMatrix4* poses, Scalar* resultingErrors = nullptr, Worker* worker = nullptr);

		/**
		 * Returns whether this set holds no pose projections.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this set holds at least one pose projection.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/**
		 * Returns the pose with the minimal distance error for a subset of the pose projections.
		 * @param imagePoints Image points to find corresponding pose points for
		 * @param numberImagePoints Number of given image points
		 * @param validImagePoints Number of valid image points expecting to have corresponding points in the pose points, with range [1, numberImagePoints]
		 * @param errorDetermination Depending on this flag uniqueAveragedRobustErrorInPointCloud, approximatedAveragedRobustErrorInPointCloud() or ambiguousAveragedRobustErrorInPointCloud() will be used
		 * @param errorObjects Error objects receiving the estimated error values
		 * @param firstProjection First projection to be handled
		 * @param numberProjections Number of projections to be handled
		 * @tparam tEstimator Estimator to be applied
		 */
		template <Geometry::Estimator::EstimatorType tEstimator>
		void findPoseWithMinimalErrorSubset(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination, ErrorObject* errorObjects, const unsigned int firstProjection, const unsigned int numberProjections);

	private:

		/// All registered pose projections.
		PoseProjections projectionSetPoseProjections;

		/// Width of the camera in pixel used for all pose projections
		unsigned int projectionSetCameraWidth;

		/// Height of the camera in pixel used for all pose projections
		unsigned int projectionSetCameraHeight;
};

inline const Vector3& PoseProjection::objectPoint2objectPoint(const Geometry::ObjectPoint& objectPoint)
{
	return objectPoint;
}

inline const HomogenousMatrix4& PoseProjection::pose() const
{
	return poseTransformation;
}

inline const Geometry::ImagePoints& PoseProjection::imagePoints() const
{
	return poseImagePoints;
}

inline CV::Detector::PointFeature::DistortionState PoseProjection::distortionState() const
{
	return poseDistortionState;
}

inline size_t PoseProjection::size() const
{
	return poseImagePoints.size();
}

inline bool PoseProjection::isEmpty() const
{
	return poseImagePoints.empty();
}

inline PoseProjection::operator bool() const
{
	return !poseImagePoints.empty();
}

inline unsigned int PoseProjectionSet::width() const
{
	return projectionSetCameraWidth;
}

inline unsigned int PoseProjectionSet::height() const
{
	return projectionSetCameraHeight;
}

inline void PoseProjectionSet::setDimension(const unsigned int width, const unsigned int height)
{
	projectionSetCameraWidth = width;
	projectionSetCameraHeight = height;
}

inline void PoseProjectionSet::addPoseProjection(const PoseProjection& poseProjection)
{
	projectionSetPoseProjections.push_back(poseProjection);
}

inline const PoseProjectionSet::PoseProjections& PoseProjectionSet::poseProjections() const
{
	return projectionSetPoseProjections;
}

inline size_t PoseProjectionSet::size() const
{
	return projectionSetPoseProjections.size();
}

inline bool PoseProjectionSet::isEmpty() const
{
	return projectionSetPoseProjections.empty();
}

inline PoseProjectionSet::operator bool() const
{
	return !projectionSetPoseProjections.empty();
}

inline PoseProjectionSet::ErrorObject::ErrorObject(const unsigned int index, const Scalar error) :
	objectIndex(index),
	objectError(error)
{
	// nothing to do here
}

inline unsigned int PoseProjectionSet::ErrorObject::index() const
{
	return objectIndex;
}

inline Scalar PoseProjectionSet::ErrorObject::error() const
{
	return objectError;
}

inline bool PoseProjectionSet::ErrorObject::operator<(const ErrorObject& element) const
{
	return objectError < element.objectError;
}

template <Geometry::Estimator::EstimatorType tEstimator>
Scalar PoseProjection::minimalAverageSquareError(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination)
{
	if (isEmpty())
		return Numeric::maxValue();

	const size_t points = min(size(), numberImagePoints);
	const size_t validPoints = min(validImagePoints, points);

	return Geometry::Error::averagedRobustErrorInPointCloud<tEstimator>(imagePoints, points, validPoints, poseImagePoints.data(), size(), errorDetermination);
}

template <Geometry::Estimator::EstimatorType tEstimator>
HomogenousMatrix4 PoseProjectionSet::findPoseWithMinimalError(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination, Scalar* resultingError, Worker* worker)
{
	ocean_assert(!isEmpty());

	if (isEmpty())
		return HomogenousMatrix4();

	ErrorObjects errorObjects(projectionSetPoseProjections.size(), ErrorObject(0xFFFFFFFF, Numeric::maxValue()));

	if (worker)
		worker->executeFunction(Worker::Function::create(*this, &PoseProjectionSet::findPoseWithMinimalErrorSubset<tEstimator>, imagePoints, numberImagePoints, validImagePoints, errorDetermination, errorObjects.data(), 0u, 0u), 0u, size(), 5u, 6u);
	else
		findPoseWithMinimalErrorSubset<tEstimator>(imagePoints, numberImagePoints, validImagePoints, errorDetermination, errorObjects.data(), 0u, (unsigned int)size());

	std::sort(errorObjects.begin(), errorObjects.end());

	if (resultingError)
		*resultingError = errorObjects.front().error();

	ocean_assert(errorObjects.front().index() < projectionSetPoseProjections.size());
	return projectionSetPoseProjections[errorObjects.front().index()].pose();
}

template <Geometry::Estimator::EstimatorType tEstimator>
unsigned int PoseProjectionSet::findPosesWithMinimalError(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination, const size_t numberPoses, HomogenousMatrix4* poses, Scalar* resultingErrors, Worker* worker)
{
	ocean_assert(poses);

	if (isEmpty() || numberPoses == 0)
		return 0;

	ErrorObjects errorObjects(projectionSetPoseProjections.size(), ErrorObject(0xFFFFFFFF, Numeric::maxValue()));

	if (worker)
		worker->executeFunction(Worker::Function::create(*this, &PoseProjectionSet::findPoseWithMinimalErrorSubset<tEstimator>, imagePoints, numberImagePoints, validImagePoints, errorDetermination, errorObjects.data(), 0u, 0u), 0u, (unsigned int)size(), 5u, 6u);
	else
		findPoseWithMinimalErrorSubset<tEstimator>(imagePoints, numberImagePoints, validImagePoints, errorDetermination, errorObjects.data(), 0u, (unsigned int)size());

	// **NOTE** We should seek a slightly larger set of best matching poses as we should try to find good poses but also different poses
	// **NOTE** We should think about the application of more suitable data structures like KD-Trees to improve the performance

	std::sort(errorObjects.begin(), errorObjects.end());

	const size_t results = min(numberPoses, errorObjects.size());

	for (size_t n = 0; n < results; ++n)
	{
		poses[n] = projectionSetPoseProjections[errorObjects[n].index()].pose();
		if (resultingErrors)
			resultingErrors[n] = errorObjects[n].error();
	}

	return (unsigned int)results;
}

template <Geometry::Estimator::EstimatorType tEstimator>
void PoseProjectionSet::findPoseWithMinimalErrorSubset(const Geometry::ImagePoint* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Geometry::Error::ErrorDetermination errorDetermination, ErrorObject* errorObjects, const unsigned int firstProjection, const unsigned int numberProjections)
{
	ocean_assert(imagePoints);
	ocean_assert(errorObjects);

	ocean_assert(firstProjection + numberProjections <= projectionSetPoseProjections.size());

	for (unsigned int n = firstProjection; n < firstProjection + numberProjections; ++n)
	{
		const Scalar errorValue = projectionSetPoseProjections[n].minimalAverageSquareError<tEstimator>(imagePoints, numberImagePoints, validImagePoints, errorDetermination);
		errorObjects[n] = ErrorObject(n, errorValue);
	}
}

}

}

#endif // META_OCEAN_TRACKING_POSE_PROJECTION_H
