/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_RMV_RANDOM_MODEL_VARIATION_H
#define META_OCEAN_TRACKING_RMV_RANDOM_MODEL_VARIATION_H

#include "ocean/tracking/rmv/RMV.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/Error.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace RMV
{

/**
 * This class implements random model variation algorithms.
 * @ingroup trackingrmv
 */
class OCEAN_TRACKING_RMV_EXPORT RandomModelVariation
{
	public:

		/**
		 * Returns the optimized camera pose for a given rough pose, a cloud of object points and a cloud of image points with a sufficient number of valid correspondences.<br>
		 * However, no explicit correspondences between the 3D object points and 2D image points are defined.<br>
		 * Therefore, the point order of the given points will be ignored.
		 *
		 * This function supports two individual modes:<br>
		 * The first mode (if tLessImagePoints is True) seeks a corresponding object point for each provided image point - thus, the number of image points must be smaller than the number of object points.<br>
		 * The second mode (if tLessImagePoints is False) seeks a corresponding image point for each provided object point - thus, the number of object points must be smaller than the number of image points.<br>
		 * Further, the amount of valid correspondences between image and object points must be defined to improve the accuracy of the pose determination.
		 * @param initialPoseIF The initial and rough inverted and flipped pose that will be improved by application of the random model variation approach
		 * @param pinholeCamera The pinhole camera profile defining the projection between 3D object points and 2D image points
		 * @param objectPoints Objects points to be used for pose determination, must be valid
		 * @param numberObjectPoints Number of object points to be used for pose determination, with range [numberImagePoints, infinity) if tLessImagePoints is True; with range [3, numberImagePoints) if tLessImagePoints is False
		 * @param imagePoints Image points to be used for pose determination, must be valid
		 * @param numberImagePoints Number of image points to be used for pose determination, with range [3, numberObjectPoints] if tLessImagePoints is True; with range [numberImagePoints, infinity) is tLessImagePoints is False
		 * @param numberValidCorrespondences Approximately number of valid correspondences between image points and object points, with range [3, min(numberImagePoints, numberObjectPoints)]
		 * @param randomGenerator Random generator object used as initialization for the local random generators
		 * @param poseIF The resulting inverted and flipped optimized pose
		 * @param errorDetermination Defines the applied error determination method to allowing different error quality results
		 * @param targetAverageSqrError The expected target average square pixel error for valid point correspondences to be reached before the calculation will stop, with range [0, infinity)
		 * @param maximalTranslationOffset Maximal translation offset between the initial pose and the final resulting pose for all three axes (should be approx. 3 times higher than the real expected value)
		 * @param maximalOrientationOffset Maximal orientation offset between the initial pose and the final resulting pose, in radian (should be approx. 3 times higher than the real expected value)
		 * @param timeout Maximal time for the algorithm to determine the pose, in seconds
		 * @param resultingSqrError Optional resulting average square pixel error, with range [0, infinity), must be -1 if specified
		 * @param correspondences Optional resulting point correspondences, for each index of an image point (first index) one corresponding object point index (second index) will be returned
		 * @param explicitStop Optional possibility to stop the matrix determination use a stop flag, True to stop the calculation explicitly
		 * @param worker Optional worker object to execute the function on several CPU cores concurrently
		 * @return True, if succeeded
		 * @tparam tLessImagePoints True, to find a corresponding object point for each given image point (as the number of image points is smaller than the number of object points); False, to find a corresponding image points for each object point
		 */
		template <bool tLessImagePoints>
		static bool optimizedPoseFromPointCloudsWithOneInitialPoseIF(const HomogenousMatrix4& initialPoseIF, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator& randomGenerator, HomogenousMatrix4& poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError = Scalar(4 * 4), const Vector3& maximalTranslationOffset = Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.3)), const Scalar maximalOrientationOffset = Numeric::deg2rad(30), const double timeout = 1, Scalar* resultingSqrError = nullptr, IndexPairs32* correspondences = nullptr, bool* explicitStop = nullptr, Worker* worker = nullptr);

		/**
		 * Returns the optimized camera pose for several given rough pose candidates, a cloud of object points and a cloud of image points with a sufficient number of valid correspondences.<br>
		 * However, no explicit correspondences between the 3D object points and 2D image points are defined.<br>
		 * Therefore, the point order of the given points will be ignored.
		 *
		 * This function supports two individual modes:<br>
		 * The first mode (if tLessImagePoints is True) seeks a corresponding object point for each provided image point - thus, the number of image points must be smaller than the number of object points.<br>
		 * The second mode (if tLessImagePoints is False) seeks a corresponding image point for each provided object point - thus, the number of object points must be smaller than the number of image points.<br>
		 * Further, the amount of valid correspondences between image and object points must be defined to improve the accuracy of the pose determination.
		 *
		 * In the case a worker object provided, this function will execute the pose determination using several CPU cores concurrently.<br>
		 * Each CPU core will receive a different initial pose. The entire calculation stops if the first execute receives a valid result.<br>
		 * Thus, the number of given initial poses should be a multiple of the existing CPU cores.
		 * @param initialPosesIF Initial and rough inverted and flipped poses to be used for precise pose determination
		 * @param numberInitialPoses The number of given initial poses, with range [2, infinity)
		 * @param pinholeCamera The pinhole camera profile defining the projection between 3D object points and 2D image points
		 * @param objectPoints Objects points to be used for pose determination, must be valid
		 * @param numberObjectPoints Number of object points to be used for pose determination, with range [numberImagePoints, infinity) if tLessImagePoints is True; with range [3, numberImagePoints) if tLessImagePoints is False
		 * @param imagePoints Image points to be used for pose determination, must be valid
		 * @param numberImagePoints Number of image points to be used for pose determination, with range [3, numberObjectPoints] if tLessImagePoints is True; with range [numberImagePoints, infinity) is tLessImagePoints is False
		 * @param numberValidCorrespondences Approximately number of valid correspondences between image points and object points, with range [3, min(numberImagePoints, numberObjectPoints)]
		 * @param randomGenerator Random generator object used as initialization for the local random generators
		 * @param poseIF The one unique best matching (and optimized) inverted and flipped pose from the set of given poses
		 * @param errorDetermination Defines the applied error determination method to allowing different error quality results
		 * @param targetAverageSqrError The expected target average square pixel error for valid point correspondences to be reached before the calculation will stop, with range [0, infinity)
		 * @param maximalTranslationOffset Maximal translation offset between the initial pose and the final resulting pose for all three axes (should be approx. 3 times higher than the real expected value)
		 * @param maximalOrientationOffset Maximal orientation offset between the initial pose and the final resulting pose, in radian (should be approx. 3 times higher than the real expected value)
		 * @param timeout Maximal time for the algorithm to determine the pose, in seconds
		 * @param resultingSqrError Optional resulting average square pixel error, must be -1 if specified
		 * @param explicitStop Optional possibility to stop the matrix determination use a stop flag, True to stop the calculation explicitly
		 * @param worker Optional worker object to execute the function on several CPU cores concurrently
		 * @return True, if succeeded
		 * @tparam tLessImagePoints True, to find a corresponding object point for each given image point (as the number of image points is smaller than the number of object points); False, to find a corresponding image points for each object point
		 * @see Worker::threads().
		 */
		template <bool tLessImagePoints>
		static bool optimizedPoseFromPointCloudsWithSeveralInitialPosesIF(const HomogenousMatrix4* initialPosesIF, const size_t numberInitialPoses, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator& randomGenerator, HomogenousMatrix4& poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError = Scalar(4 * 4), const Vector3& maximalTranslationOffset = Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.3)), const Scalar maximalOrientationOffset = Numeric::deg2rad(30), const double timeout = 1, Scalar* resultingSqrError = nullptr, bool* explicitStop = nullptr, Worker* worker = nullptr);

	private:

		/**
		 * Returns the extrinsic camera matrix by a rough pose, valid object points and image points with a sufficient number of valid correspondences.<br>
		 * However, no explicit correspondences between the 3D object points and 2D image points are defined.<br>
		 * Therefore, the point order of the given points will be ignored.

		 * This function supports two individual modes:<br>
		 * The first mode (if tLessImagePoints is True) seeks a corresponding object point for each provided image point - thus, the number of image points must be smaller than the number of object points.<br>
		 * The second mode (if tLessImagePoints is False) seeks a corresponding image point for each provided object point - thus, the number of object points must be smaller than the number of image points.<br>
		 * Further, the amount of valid correspondences between image and object points must be defined to improve the accuracy of the pose determination.
		 * @param initialPoseIF The initial and rough inverted and flipped pose that will be improved by application of the random model variation approach
		 * @param pinholeCamera The pinhole camera profile defining the projection between 3D object points and 2D image points
		 * @param objectPoints Objects points to be used for pose determination, must be valid
		 * @param numberObjectPoints Number of object points to be used for pose determination, with range [numberImagePoints, infinity) if tLessImagePoints is True; with range [3, numberImagePoints) if tLessImagePoints is False
		 * @param imagePoints Image points to be used for pose determination, must be valid
		 * @param numberImagePoints Number of image points to be used for pose determination, with range [3, numberObjectPoints] if tLessImagePoints is True; with range [numberImagePoints, infinity) is tLessImagePoints is False
		 * @param numberValidCorrespondences Approximately number of valid correspondences between image points and object points, with range [3, min(numberImagePoints, numberObjectPoints)]
		 * @param randomGenerator Random generator object used as initialization for the local random generators
		 * @param poseIF The resulting optimized inverted and flipped pose
		 * @param errorDetermination Defines the applied error determination method to allowing different error quality results
		 * @param targetAverageSqrError The expected target average square pixel error for valid point correspondences to be reached before the calculation will stop, with range [0, infinity)
		 * @param maximalTranslationOffset Maximal translation offset between the initial pose and the final resulting pose for all three axes (should be approx. 3 times higher than the real expected value)
		 * @param maximalOrientationOffset Maximal orientation offset between the initial pose and the final resulting pose, in radian (should be approx. 3 times higher than the real expected value)
		 * @param timeout Maximal time for the algorithm to determine the pose, in seconds
		 * @param resultingSqrError Resulting average square pixel error
		 * @param correspondences Optional resulting point correspondences, for each index of an image point one corresponding object point index will be returned
		 * @param explicitStop Optional possibility to stop the matrix determination use a stop flag, True to stop the calculation explicitly
		 * @param lock Optional lock object to lock the resulting values during assignment for execution on several cpu cores
		 * @return True, if succeeded
		 * @tparam tLessImagePoints True, to find a corresponding object point for each given image point (as the number of image points is smaller than the number of object points); False, to find a corresponding image points for each object point
		 */
		template <bool tLessImagePoints>
		static bool optimizedPoseFromPointCloudsAbortableIF(const HomogenousMatrix4* initialPoseIF, const PinholeCamera* pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator* randomGenerator, HomogenousMatrix4* poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError = Scalar(4 * 4), Vector3 maximalTranslationOffset = Vector3(Scalar(0.3), Scalar(0.3), Scalar(0.3)), Scalar maximalOrientationOffset = Numeric::deg2rad(30), const double timeout = 1, Scalar* resultingSqrError = nullptr, IndexPairs32* correspondences = nullptr, bool* explicitStop = nullptr, Lock* lock = nullptr);

		/**
		 * Returns the optimized camera pose for a subset of several given rough pose candidates, a cloud of object points and a cloud of image points with a sufficient number of valid correspondences.<br>
		 * However, no explicit correspondences between the 3D object points and 2D image points are defined.<br>
		 * Therefore, the point order of the given points will be ignored.
		 *
		 * This function supports two individual modes:<br>
		 * The first mode (if tLessImagePoints is True) seeks a corresponding object point for each provided image point - thus, the number of image points must be smaller than the number of object points.<br>
		 * The second mode (if tLessImagePoints is False) seeks a corresponding image point for each provided object point - thus, the number of object points must be smaller than the number of image points.<br>
		 * Further, the amount of valid correspondences between image and object points must be defined to improve the accuracy of the pose determination.
		 * @param initialPosesIF The initial and rough inverted and flipped poses which will be improved by application of the random model variation approach
		 * @param firstInitialPose First initial pose to be handled, must be valid
		 * @param numberInitialPoses Number of initial poses to be handled, with range [1, infinity)
		 * @param pinholeCamera The pinhole camera profile defining the projection between 3D object points and 2D image points
		 * @param objectPoints Objects points to be used for pose determination, must be valid
		 * @param numberObjectPoints Number of object points to be used for pose determination, with range [numberImagePoints, infinity) if tLessImagePoints is True; with range [3, numberImagePoints) if tLessImagePoints is False
		 * @param imagePoints Image points to be used for pose determination, must be valid
		 * @param numberImagePoints Number of image points to be used for pose determination, with range [3, numberObjectPoints] if tLessImagePoints is True; with range [numberImagePoints, infinity) is tLessImagePoints is False
		 * @param numberValidCorrespondences Approximately number of valid correspondences between image points and object points, with range [3, min(numberImagePoints, numberObjectPoints)]
		 * @param randomGenerator Random generator object used as initialization for the local random generators
		 * @param poseIF The one unique best matching (and optimized) inverted and flipped pose from the set of given poses
		 * @param errorDetermination Defines the applied error determination method to allowing different error quality results
		 * @param targetAverageSqrError The expected target average square pixel error for valid point correspondences to be reached before the calculation will stop, with range [0, infinity)
		 * @param maximalTranslationOffset Maximal translation offset between the initial pose and the final resulting pose for all three axes (should be approx. 3 times higher than the real expected value)
		 * @param maximalOrientationOffset Maximal orientation offset between the initial pose and the final resulting pose, in radian (should be approx. 3 times higher than the real expected value)
		 * @param timeout Maximal time for the algorithm to determine the pose, in seconds
		 * @param resultingSqrError Resulting average square pixel error
		 * @param explicitStop Optional possibility to stop the matrix determination use a stop flag, True to stop the calculation explicitly
		 * @param lock Optional lock object assuring that only one valid result is returned
		 * @return True, if succeeded
		 * @tparam tLessImagePoints True, to find a corresponding object point for each given image point (as the number of image points is smaller than the number of object points); False, to find a corresponding image points for each object point
		 */
		template <bool tLessImagePoints>
		static bool optimizedPoseFromPointCloudsPoseIFSubset(const HomogenousMatrix4* initialPosesIF, const unsigned int firstInitialPose, const unsigned int numberInitialPoses, const PinholeCamera* pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const size_t numberValidCorrespondences, RandomGenerator* randomGenerator, HomogenousMatrix4* poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError, Vector3 maximalTranslationOffset, Scalar maximalOrientationOffset, const double timeout, Scalar* resultingSqrError, bool* explicitStop, Lock* lock);

		/**
		 * Assigns a pose candidate to a target pose if the pose quality is better than the currently known pose quality.
		 * The pose itself is not used for error determination but the point clouds only.<br>
		 * This function can be thread-safe if a lock object is provided.
		 * @param smallPointGroup The smaller group of points, must be valid
		 * @param smallPointGroupNumber The number of points in the smaller group, with range [1, infinity)
		 * @param largePointGroup The larger group of points, must be valid
		 * @param largePointGroupNumber The number of points in the larger group, with range [smallPointGroupNumber, infinity)
		 * @param numberValidCorrespondences The expected number of points that have a corresponding point in the other point set, with range [1, smallPointGroupNumber]
		 * @param candidateSqrError The square error of the current candidate pose, with range [0, infinity)
		 * @param poseCandidateIF The inverted and flipped candidate pose
		 * @param poseIF The target pose (receiving the candidate pose if this pose is the best)
		 * @param errorDetermination The error determination to be used
		 * @param targetAverageSqrError The maximal square distance between two corresponding points so that they count as valid pair, with range [0, infinity)
		 * @param resultingSqrError Optional resulting square distance that can be used to decide the best result of parallel threads
		 * @param correspondences Optional resulting index pairs of correspondences between the two point groups, only for pairs with square distance better than 'targetAverageSqrError'
		 * @param explicitStop Optional stop flag that will be set if provided
		 * @param lock Optional lock object to make this function thread-safe
		 * @return True, if the pose candidate has been assigned as better pose
		 */
		static bool assignBestPoseIF(const Vector2* smallPointGroup, const size_t smallPointGroupNumber, const Vector2* largePointGroup, const size_t largePointGroupNumber, const size_t numberValidCorrespondences, const Scalar candidateSqrError, const HomogenousMatrix4& poseCandidateIF, HomogenousMatrix4& poseIF, const Geometry::Error::ErrorDetermination errorDetermination, const Scalar targetAverageSqrError, Scalar* resultingSqrError, IndexPairs32* correspondences, bool* explicitStop, Lock* lock);
};

}

}

}

#endif // META_OCEAN_TRACKING_RMV_RANDOM_MODEL_VARIATION_H
