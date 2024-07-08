/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_RANSAC_H
#define META_OCEAN_GEOMETRY_RANSAC_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/CameraCalibration.h"
#include "ocean/geometry/Error.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimizationHomography.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/Lock.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Box3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements several RANSAC functions for pose determination.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT RANSAC
{
	protected:

		/**
		 * Definition of a function pointer determining a geometric transformation e.g., a affine transformation.
		 * Parameter0: Pointer to the left image point, must be valid
		 * Parameter1: Pointer to the right image points, must be valid
		 * Parameter2: Number of point correspondences, with range [1, infinity)
		 * Parameter3: Resulting transformation
		 * Return parameter: True, if succeeded
		 */
		typedef bool (*GeometricTransformFunction)(const ImagePoint*, const ImagePoint*, const size_t, SquareMatrix3&);

	public:

		/**
		 * Calculates the number of iterations necessary to find an outlier-free model data set.
		 * @param model Number of data elements sufficient to define a test model
		 * @param successProbability Probability that at least one outlier-free model will be selected, with range (0, 1)
		 * @param faultyRate Rate of faulty data elements inside the given data set, with range [0, 1)
		 * @param maximalIterations The number of maximal iterations which can be returned, with range [1, infinity)
		 * @return Minimal number of iterations to guarantee the specified probability, with range [1, maximalIterations]
		 */
		static unsigned int iterations(const unsigned int model, const Scalar successProbability = Scalar(0.99), const Scalar faultyRate = Scalar(0.2), const unsigned int maximalIterations = 1000000u);

		/**
		 * Calculates a pose using the perspective pose problem with three point correspondences using any camera.
		 * The specified point correspondences should be sorted by strength or stability to enhance the pose determination.<br>
		 * @param anyCamera The camera object specifying the projection, must be valid
		 * @param objectPointAccessor The accessor providing the 3D object points, at least 4
		 * @param imagePointAccessor The accessor providing the 2D image points, one image point for each object point
		 * @param randomGenerator A random generator to be used
		 * @param world_T_camera The resulting pose transforming camera points to world points
		 * @param minimalValidCorrespondences Minimal number of valid correspondences, with range [4, objectPointAccessor.size()]
		 * @param refine Determines whether a not linear least square algorithm is used to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates, with range (0, infinity)
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error
		 * @return True, if succeeded
		 */
		static bool p3p(const AnyCamera& anyCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const unsigned int minimalValidCorrespondences = 5u, const bool refine = true, const unsigned int iterations = 20u, const Scalar sqrPixelErrorThreshold = Scalar(5 * 5), Indices32* usedIndices = nullptr, Scalar* sqrAccuracy = nullptr);

		/**
		 * Deprecated.
		 *
		 * Calculates a pose using the perspective pose problem with three point correspondences.
		 * The specified point correspondences should be sorted by strength or stability to enhance the pose determination.<br>
		 * This function does not used any apriori information (like e.g. a previous pose).<br>
		 * Beware: There is another p3p() function with almost identical functionality/parameter layout.<br>
		 * However, this function here supports 'weights' parameters and thus creates a bigger binary footprint.
		 * @param pinholeCamera The pinhole camera object specifying the intrinsic camera parameters
		 * @param objectPointAccessor The accessor providing the 3D object points, at least 4
		 * @param imagePointAccessor The accessor providing the 2D image points, one image point for each object point
		 * @param randomGenerator A random generator object
		 * @param useDistortionParameters True, if the provided image points are distorted and if the camera's distortion parameters should be applied during the pose determination
		 * @param pose Resulting pose
		 * @param minimalValidCorrespondences Minimal number of valid correspondences
		 * @param refine Determines whether a not linear least square algorithm is used to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates, with range (0, infinity)
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error
		 * @param weights Optional explicit weights to weight the point correspondences individually
		 * @return True, if succeeded
		 */
		static inline bool p3p(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, const unsigned int minimalValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights);

		/**
		 * Calculates a camera pose using the perspective pose problem with three point correspondences.
		 * The calculation uses a rough camera pose (e.g., from a previous camera frame) to increase the robustness.
		 * @param world_T_roughCamera The already known rough camera pose, transforming rough camera points to world points, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param camera The camera profile defining the project, must be valid
		 * @param objectPointAccessor The accessor providing the 3D object points, at least 4
		 * @param imagePointAccessor The accessor providing the 2D image points, one image point for each 3D object point
		 * @param randomGenerator The random generator to be used
		 * @param world_T_camera The resulting camera pose, transforming camera points to world points, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param maxPositionOffset Maximal position offset between initial and final pose for three axis
		 * @param maxOrientationOffset Maximal orientation offset between initial and final pose in radian angle
		 * @param minValidCorrespondences Minimal number of valid correspondences
		 * @param refine True, if optimization step will be applied to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates, with range (0, infinity)
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error, with range [0, infinity)
		 * @param weights Optional weights to weight the individual point correspondences individually, nullptr to use the same weight for all point correspondences
		 * @return True, if succeeded
		 */
		static inline bool p3p(const HomogenousMatrix4& world_T_roughCamera, const AnyCamera& camera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const Vector3& maxPositionOffset = Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1)), const Scalar maxOrientationOffset = Numeric::deg2rad(10), const unsigned int minValidCorrespondences = 5u, const bool refine = true, const unsigned int iterations = 20u, const Scalar sqrPixelErrorThreshold = Scalar(5 * 5), Indices32* usedIndices = nullptr, Scalar* sqrAccuracy = nullptr, const Scalar* weights = nullptr);

		/**
		 * Deprecated.
		 *
		 * Calculates a pose using the perspective pose problem with three point correspondences.
		 * The calculation uses a pose from a previous calculation to increase the robustness.<br>
		 * The specified point correspondences should be sorted by strength or stability to enhance the pose determination.<br>
		 * This function uses a rough pose as apriori information to improve the robustness of the result.
		 * @param initialPose Rough initial pose to increase determination robustness
		 * @param pinholeCamera The pinhole camera object specifying the intrinsic camera parameters
		 * @param objectPointAccessor The accessor providing the 3D object points, at least 4
		 * @param imagePointAccessor The accessor providing the 2D image points, one image point for each object point
		 * @param randomGenerator A random generator object
		 * @param useDistortionParameters True, if the provided image points are distorted and if the camera's distortion parameters should be applied during the pose determination
		 * @param pose Resulting pose
		 * @param maxPositionOffset Maximal position offset between initial and final pose for three axis
		 * @param maxOrientationOffset Maximal orientation offset between initial and final pose in radian angle
		 * @param minValidCorrespondences Minimal number of valid correspondences
		 * @param refine True, if optimization step will be applied to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates, with range (0, infinity)
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error
		 * @param weights Optional explicit weights to weight the point correspondences individually
		 * @return True, if succeeded
		 */
		static inline bool p3p(const HomogenousMatrix4& initialPose, const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, const Vector3& maxPositionOffset = Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1)), const Scalar maxOrientationOffset = Numeric::deg2rad(10), const unsigned int minValidCorrespondences = 5u, const bool refine = true, const unsigned int iterations = 20u, const Scalar sqrPixelErrorThreshold = Scalar(5 * 5), Indices32* usedIndices = nullptr, Scalar* sqrAccuracy = nullptr, const Scalar* weights = nullptr);

		/**
		 * Calculates a pose including zoom factor using the perspective pose problem with three point correspondences.
		 * The specified point correspondences should be sorted by strength or stability to enhance the pose determination.<br>
		 * This function does not used any apriori information (like e.g. a previous pose).
		 * @param pinholeCamera The pinhole camera object specifying the intrinsic camera parameters
		 * @param objectPointAccessor The accessor providing the 3D object points, at least 4
		 * @param imagePointAccessor The accessor providing the 2D image points, one image point for each object point
		 * @param randomGenerator A random generator object
		 * @param useDistortionParameters True, if the provided image points are distorted and if the camera's distortion parameters should be applied during the pose determination
		 * @param pose Resulting pose
		 * @param zoom Resulting zoom factor matching to the resulting pose, with range (0, infinity)
		 * @param minimalValidCorrespondences Minimal number of valid correspondences
		 * @param refine Determines whether a not linear least square algorithm is used to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates, with range (0, infinity)
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error
		 * @param weights Optional explicit weights to weight the point correspondences individually
		 * @return True, if succeeded
		 */
		static inline bool p3pZoom(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, Scalar& zoom, const unsigned int minimalValidCorrespondences = 5u, const bool refine = true, const unsigned int iterations = 20u, const Scalar sqrPixelErrorThreshold = Scalar(5 * 5), Indices32* usedIndices = nullptr, Scalar* sqrAccuracy = nullptr, const Scalar* weights = nullptr);

		/**
		 * Calculates a pose including zoom factor, using the perspective pose problem with three point correspondences.
		 * The calculation uses a pose from a previous calculation to increase the robustness.<br>
		 * The specified point correspondences should be sorted by strength or stability to enhance the pose determination.<br>
		 * This function uses a rough pose as apriori information to improve the robustness of the result.
		 * @param initialPose Rough initial pose to increase determination robustness
		 * @param initialZoom Rough initial zoom factor to increase the determination robustness, with range (0, infinity)
		 * @param pinholeCamera The pinhole camera object specifying the intrinsic camera parameters
		 * @param objectPointAccessor The accessor providing the 3D object points, at least 4
		 * @param imagePointAccessor The accessor providing the 2D image points, one image point for each object point
		 * @param randomGenerator A random generator object
		 * @param useDistortionParameters True, if the provided image points are distorted and if the camera's distortion parameters should be applied during the pose determination
		 * @param pose Resulting pose
		 * @param zoom Resulting zoom factor matching to the resulting pose, with range (0, infinity)
		 * @param maxPositionOffset Maximal position offset between initial and final pose for three axis
		 * @param maxOrientationOffset Maximal orientation offset between initial and final pose in radian angle
		 * @param minValidCorrespondences Minimal number of valid correspondences
		 * @param refine True, to apply an optimization step to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates, with range (0, infinity)
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error
		 * @param weights Optional explicit weights to weight the point correspondences individually
		 * @return True, if succeeded
		 */
		static inline bool p3pZoom(const HomogenousMatrix4& initialPose, const Scalar initialZoom, const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, Scalar& zoom, const Vector3& maxPositionOffset = Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1)), const Scalar maxOrientationOffset = Numeric::deg2rad(10), const unsigned int minValidCorrespondences = 5u, const bool refine = true, const unsigned int iterations = 20u, const Scalar sqrPixelErrorThreshold = Scalar(5 * 5), Indices32* usedIndices = nullptr, Scalar* sqrAccuracy = nullptr, const Scalar* weights = nullptr);

		/**
		 * Determines the 2D line best fitting to a set of given 2D positions.
		 * @param positions The 2D positions for which the line will be determined, at least 2
		 * @param randomGenerator Random generator object
		 * @param line The resulting line best fitting with the given points
		 * @param refine True, to apply an optimization step to increase the accuracy of the line
		 * @param iterations The number of RANSAC iterations, with range [1, infinity)
		 * @param maxSqrError The maximal square error between a direction candidate and a direction from the provided set to count as valid, with range (0, infinity)
		 * @param finalError Optional resulting final average square error between all valid directions and the mean direction
		 * @param usedIndices Optional resulting indices of all used point correspondences
		 * @return True, if succeeded
		 */
		static bool line(const ConstIndexedAccessor<Vector2>& positions, RandomGenerator& randomGenerator, Line2& line, const bool refine = true, const unsigned int iterations = 20u, const Scalar maxSqrError = Scalar(5 * 5), Scalar* finalError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Determines the 2D direction from a set of given 2D directions provided as unit vectors.
		 * @param directions The sets of 2D unit vectors from which the major direction is determined, at least 1
		 * @param randomGenerator A random generator object
		 * @param direction The resulting major direction of the provided set of vectors, will be a unit vector
		 * @param acceptOppositeDirections True, to accept opposite direction (unit vector almost parallel but pointing towards the opposite direction); False, to treat opposite directions as different directions
		 * @param refine True, to apply an optimization step to increase the accuracy of the direction
		 * @param iterations The number of RANSAC iterations, with range [1, infinity)
		 * @param maxAngle The maximal angle between a direction candidate and a direction from the provided set to count as valid, in radian, with range [0, PI)
		 * @param finalError Optional resulting final average angle between all valid directions and the mean direction, this is an approximation only, with range [0, PI)
		 * @param usedIndices Optional resulting indices of all used point correspondences
		 * @return True, if succeeded
		 */
		static bool direction(const ConstIndexedAccessor<Vector2>& directions, RandomGenerator& randomGenerator, Vector2& direction, const bool acceptOppositeDirections, const bool refine = true, const unsigned int iterations = 20u, const Scalar maxAngle = Numeric::deg2rad(5), Scalar* finalError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Determines the 2D translation (offset/translation vector) from a set of given 2D vectors.
		 * @param translations The sets of 2D vectors from which the major translation/offset is determined, at least 1
		 * @param randomGenerator A random generator object
		 * @param translation The resulting major translation of the provided set of vectors
		 * @param refine True, to apply an optimization step to increase the accuracy of the translation
		 * @param iterations The number of RANSAC iterations, with range [1, infinity)
		 * @param maxSqrError The maximal square error between a translation candidate and a translation from the provided set to count as valid, with range (0, infinity)
		 * @param finalError Optional resulting final average square error between all valid translations and the mean translation
		 * @param usedIndices Optional resulting indices of all used point correspondences
		 * @return True, if succeeded
		 */
		static bool translation(const ConstIndexedAccessor<Vector2>& translations, RandomGenerator& randomGenerator, Vector2& translation, const bool refine = true, const unsigned int iterations = 20u, const Scalar maxSqrError = Scalar(5 * 5), Scalar* finalError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Determines the 3DOF rotation of a camera pose for a set of given 2D/3D point correspondences by minimizing the projection error between 3D object points and 2D image points.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPoints The accessor providing the given 3D object points, at least 2
		 * @param imagePoints The accessor providing the given 2D image points, one image point for each 3D object point
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param world_R_camera The resulting camera orientation, transforming camera to world, with default camera pointing towards the negative z-space with y-axis upwards
		 * @param minValidCorrespondences The minimal number of valid correspondences which are necessary for a valid orientation
		 * @param iterations The number of RANSAC iterations, with range [1, infinity)
		 * @param maxSqrError The maximal square pixel error between a projected 3D object point and a 2D image point to count as valid correspondence, with range (0, infinity)
		 * @param finalError Optional resulting final average square pixel error between all valid point correspondences for the resulting orientation
		 * @param usedIndices Optional resulting indices of all used point correspondences
		 * @return True, if succeeded
		 */
		static bool orientation(const AnyCamera& camera, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, SquareMatrix3& world_R_camera, const unsigned int minValidCorrespondences = 5u, const unsigned int iterations = 20u, const Scalar maxSqrError = Scalar(5 * 5), Scalar* finalError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Determines the 3D object point for a set of image points observing the same object point under individual camera poses (with rotational and translational camera motion).
		 * @param camera The camera profile for all camera frames and poses, must be valid
		 * @param world_T_cameras The camera poses of the camera frames in which the image points are located, one for each image point, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param imagePoints The image points observing the 3D object point, one image point for each pose
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param objectPoint Resulting 3D object point defined in world
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param maximalSqrError The maximal square pixel error between a projected object point and an image point, with range [0, infinity)
		 * @param minValidCorrespondences The minimal number of image points that have a projected pixel error smaller than 'maximalError', with range [2, correspondences]
		 * @param onlyFrontObjectPoint True, if the resulting object point must lie in front of the camera
		 * @param refinementEstimator An robust estimator to invoke an optimization step to increase the accuracy of the resulting position, ET_INVALID to avoid the refinement
		 * @param finalRobustError Optional the resulting final robust error
		 * @param usedIndices Optional resulting indices of valid image points
		 * @return True, if succeeded
		 */
		static inline bool objectPoint(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, ObjectPoint& objectPoint, const unsigned int iterations = 20u, const Scalar maximalSqrError = Scalar(3 * 3), const unsigned int minValidCorrespondences = 2u, const bool onlyFrontObjectPoint = true, const Estimator::EstimatorType refinementEstimator = Estimator::ET_SQUARE, Scalar* finalRobustError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Determines the 3D object point for a set of image points observing the same object point under individual camera poses (with rotational and translational camera motion), while each camera pose has an own camera profile.
		 * @param cameras The camera profiles for all camera frames and poses, must be valid
		 * @param world_T_cameras The camera poses of the camera frames in which the image points are located, one for each image point, with default camera pointing towards the negative z-space with y-axis upwards, at least two
		 * @param imagePoints The image points observing the 3D object point, one image point for each pose
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param objectPoint Resulting 3D object point, defined in world
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param maximalSqrError The maximal square pixel error between a projected object point and an image point, with range [0, infinity)
		 * @param minValidCorrespondences The minimal number of image points that have a projected pixel error smaller than 'maximalError', with range [2, correspondences]
		 * @param onlyFrontObjectPoint True, if the resulting object point must lie in front of the camera
		 * @param refinementEstimator An robust estimator to invoke an optimization step to increase the accuracy of the resulting position, ET_INVALID to avoid the refinement
		 * @param finalRobustError Optional the resulting final robust error
		 * @param usedIndices Optional resulting indices of valid image points
		 * @return True, if succeeded
		 */
		static bool objectPoint(const ConstIndexedAccessor<const AnyCamera*>& cameras, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, ObjectPoint& objectPoint, const unsigned int iterations = 20u, const Scalar maximalSqrError = Scalar(3 * 3), const unsigned int minValidCorrespondences = 2u, const bool onlyFrontObjectPoint = true, const Estimator::EstimatorType refinementEstimator = Estimator::ET_SQUARE, Scalar* finalRobustError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Determines the 3D object point for a set of image points observing the same object point under individual camera poses (with rotational camera motion only).
		 * The center position of each camera is located at the origin of the coordinate system.
		 * @param camera The camera profile defining the projection for all camera frames and poses, must be valid
		 * @param world_R_cameras The camera orientations of the camera frames in which the image points are located, transforming cameras to world, with default camera pointing towards the negeative z-space with y-axis upwards, at least two
		 * @param imagePoints The image points observing the 3D object point, one image point for each pose
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param objectPoint The resulting 3D object point
		 * @param objectPointDistance The distance between the origin and a resulting object point, with range (0, infinity)
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param maximalError The maximal square pixel error between a projected object point and an image point, with range (0, infinity)
		 * @param minValidCorrespondences The minimal number of image points that have a projected pixel error smaller than 'maximalError', with range [2, correspondences]
		 * @param onlyFrontObjectPoint True, if the resulting object point must lie in front of the camera
		 * @param refinementEstimator An robust estimator to invoke an optimization step to increase the accuracy of the resulting position, ET_INVALID to avoid the refinement
		 * @param finalError Optional the resulting final error
		 * @param usedIndices Optional resulting indices of valid image points
		 * @return True, if succeeded
		 */
		static bool objectPoint(const AnyCamera& camera, const ConstIndexedAccessor<SquareMatrix3>& world_R_cameras, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, ObjectPoint& objectPoint, const Scalar objectPointDistance, const unsigned int iterations = 20u, const Scalar maximalError = Scalar(3 * 3), const unsigned int minValidCorrespondences = 2u, const bool onlyFrontObjectPoint = true, const Estimator::EstimatorType refinementEstimator = Estimator::ET_SQUARE, Scalar* finalError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Determines a 3D plane best matching to a set of given 3D object points.
		 * @param objectPoints The 3D object points for which the best fitting plane will be determined, at least 3
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param plane Resulting 3D plane
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param medianDistanceFactor The factor which is used to defined the maximal distance between an object point on the plane to count as valid, the factor is multiplied with the median distance between all object points and the median object point, with range [0, infinity), -1 to accept all object points as valid
		 * @param minValidCorrespondences The minimal number of image points that have an error smaller than the determine median-based threshold, with range [2, correspondences]
		 * @param refinementEstimator An robust estimator to invoke an optimization step to increase the accuracy of the resulting position, ET_INVALID to avoid the refinement
		 * @param finalError Optional the resulting final error
		 * @param usedIndices Optional resulting indices of valid object points
		 * @return True, if succeeded
		 */
		static bool plane(const ConstIndexedAccessor<ObjectPoint>& objectPoints, RandomGenerator& randomGenerator, Plane3& plane, const unsigned int iterations = 20u, const Scalar medianDistanceFactor = Scalar(0.1), const unsigned int minValidCorrespondences = 3u, const Estimator::EstimatorType refinementEstimator = Estimator::ET_SQUARE, Scalar* finalError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Optimizes an already known 3D plane by minimizing the error between the plane and a set of given 3D object points.
		 * @param initialPlane The already known initial plane which will be optimized, must be valid
		 * @param objectPoints The 3D object points for which the best fitting plane will be determined, at least 3
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param plane Resulting 3D plane
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param maximalNormalOrientationOffset The maximal angle between the normal of the initial plane and the final optimized plane in radian, with range [0, PI/2]
		 * @param medianDistanceFactor The factor which is used to defined the maximal distance between an object point on the plane to count as valid, the factor is multiplied with the median distance between all object points and the initial plane, with range [0, infinity), -1 to accept all object points as valid
		 * @param minValidCorrespondences The minimal number of image points that have an error smaller than the determine median-based threshold, with range [2, correspondences]
		 * @param refinementEstimator A robust estimator to invoke an optimization step to increase the accuracy of the resulting position, ET_INVALID to avoid the refinement
		 * @param finalError Optional resulting final error
		 * @param usedIndices Optional resulting indices of valid object points
		 * @return True, if succeeded
		 */
		static bool plane(const Plane3& initialPlane, const ConstIndexedAccessor<ObjectPoint>& objectPoints, RandomGenerator& randomGenerator, Plane3& plane, const unsigned int iterations = 20u, const Scalar maximalNormalOrientationOffset = Numeric::deg2rad(30), const Scalar medianDistanceFactor = Scalar(0.1), const unsigned int minValidCorrespondences = 3u, const Estimator::EstimatorType refinementEstimator = Estimator::ET_SQUARE, Scalar* finalError = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Calculates the fundamental matrix by given point correspondences for two stereo images.
		 * In each RANSAC iterations randomly 8 correspondences are selected and the resulting fundamental matrix is tested against all given correspondences.
		 * @param leftImagePoints Left image points (each point corresponds to a right image point)
		 * @param rightImagePoints Right image points (each point corresponds to a left image point)
		 * @param correspondences Number of point correspondences, with range [testCandidates, infinity)
		 * @param width The width of the left image in pixel
		 * @param height The height of the left image in pixel
		 * @param fundamental Resulting fundamental matrix
		 * @param testCandidates Number of candidates used in each RANSAC iteration, with range [8, correspondences]
		 * @param iterations Number of RANSAC iterations, with range (0, infinity)
		 * @param errorThreshold Maximal error threshold for valid RANSAC candidates, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @return True, if succeeded
		 * @see extrinsicMatrix(), Geometry::EpipolarGeometry::fundamental2essential().
		 */
		static bool fundamentalMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const unsigned int width, const unsigned int height, SquareMatrix3& fundamental, const unsigned int testCandidates = 8u, const unsigned int iterations = 20u, const Scalar errorThreshold = Numeric::weakEps(), Indices32* usedIndices = nullptr);

		/**
		 * Calculates the extrinsic camera matrix by given point correspondences for two stereo images.
		 * The extrinsic matrix can be determined up to a scale factor for the translation vector.<br>
		 * The left camera is expected to be located in the origin while looking towards the negative z-axis with y-axis as up vector.<br>
		 * The resulting transformation defines the extrinsic camera matrix for the right camera.
		 * @param leftCamera Left camera corresponding to the left image points
		 * @param rightCamera Right camera corresponding to the right image points
		 * @param leftImagePoints Left image points (each point corresponds to a right image point)
		 * @param rightImagePoints Right image points (each point corresponds to a left image point)
		 * @param correspondences Number of point correspondences, with range [testCandidates, infinity)
		 * @param transformation Resulting extrinsic camera transformation matrix which transforms points lying in the right camera coordinate system into points lying in the left camera coordinate system
		 * @param testCandidates Number of candidates used in each RANSAC iteration, with range [8, correspondences]
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold Maximal square pixel error of a valid projection of a 3D point onto the 2D image plane, with range (0, infinity)
		 * @param maxTranslation Optional maximal resulting translation bounding box vector, if defined should have a maximal expansion of +/- 1 in each axis
		 * @param maxRotation Optional maximal resulting rotation in radian
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @return True, if succeeded
		 * @see fundamentalMatrix().
		 */
		static bool extrinsicMatrix(const PinholeCamera& leftCamera, const PinholeCamera& rightCamera, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, HomogenousMatrix4& transformation, const unsigned int testCandidates = 8u, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), const Box3& maxTranslation = Box3(), const Scalar maxRotation = Numeric::pi(), Indices32* usedIndices = nullptr);

		/**
		 * Calculates the homography between two images transforming the given image points between two images.
		 * This function needs bijective correspondences (each left image points matches exactly with one unique right image points).<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * Beware: This function is intended for prototyping purposes only. When binary size matters, please use the corresponding function using template parameters instead of 'refine' and 'useSVD'.
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image, must be valid
		 * @param rightImagePoints Image points in the right camera, one for each point in the left frame, must be valid
		 * @param correspondences Number of points correspondences, with range [4, infinity)
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param homography Resulting homography for the given image points
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [4, correspondences]
		 * @param refine True, to apply a non-linear least square optimization to increase the transformation accuracy after the RANSAC step
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold Maximal square pixel error between a right point and a transformed left point so that a point correspondence counts as valid, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @param useSVD True, to use the slower SVD approach (i.e., Homography::homographyMatrixSVD); False, to use the two-step approach (i.e., Homography::homographyMatrixLinearWithOptimizations)
		 * @return True, if succeeded
		 * @see homographyMatrix<tRefine, tUseSVD>(), Geometry::Homography::homographyMatrix(), homographyMatrixForNonBijectiveCorrespondences().
		 */
		static inline bool homographyMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& homography, const unsigned int testCandidates = 8u, const bool refine = true, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), Indices32* usedIndices = nullptr, Worker* worker = nullptr, const bool useSVD = true);

		/**
		 * Calculates the homography between two images transforming the given image points between two images.
		 * This function can be applied if the given correspondences are not bijective.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * Beware: This function is intended for prototyping purposes only. When binary size matters, please use the corresponding function using template parameters instead of 'refine' and 'useSVD'.
		 * @param leftImagePoints Image points in the left camera, must be valid
		 * @param numberLeftImagePoints the number of given left image points, with range [4, infinity)
		 * @param rightImagePoints Image points in the right camera, must be valid
		 * @param numberRightImagePoints The number of given right image points, with range [4, infinity)
		 * @param correspondences The pairs defining the non-bijective correspondences, with range [0, numberLeftImagePoints - 1]x[0, numberRightImagePoints - 1], must be valid
		 * @param numberCorrespondences The number of given correspondence pairs, with range [4, infinity)
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param right_H_left Resulting homography for the given image points, (rightPoint = right_H_left * leftPoint)
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [4, correspondences]
		 * @param refine True, to apply a non-linear least square optimization to increase the transformation accuracy after the RANSAC step
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold Maximal square pixel error between a right point and a transformed left point so that a point correspondence counts as valid, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @param useSVD True, to use the slower SVD approach (i.e., Homography::homographyMatrixSVD); False, to use the two-step approach (i.e., Homography::homographyMatrixLinearWithOptimizations)
		 * @return True, if succeeded
		 * @see homographyMatrixForNonBijectiveCorrespondences<tRefine, tUseSVD>(), homographyMatrix().
		 */
		static inline bool homographyMatrixForNonBijectiveCorrespondences(const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator& randomGenerator, SquareMatrix3& right_H_left, const unsigned int testCandidates = 8u, const bool refine = true, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), Indices32* usedIndices = nullptr, Worker* worker = nullptr, const bool useSVD = true);

		/**
		 * Calculates the homography between two images transforming the given image points between two images.
		 * This function needs bijective correspondences (each left image points matches exactly with one unique right image points).<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * This function applies template parameters for refine and useSVD. Thus, this function will create smaller binary sizes.
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera, one for each point in the left frame
		 * @param correspondences Number of points correspondences, with range [4, infinity)
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param homography Resulting homography for the given image points
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [4, correspondences]
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold Maximal square pixel error between a right point and a transformed left point so that a point correspondence counts as valid, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tRefine True, to apply a non-linear least square optimization to increase the transformation accuracy after the RANSAC step
		 * @tparam tUseSVD True, to use the slower SVD approach (i.e., Homography::homographyMatrixSVD); False, to use the two-step approach (i.e., Homography::homographyMatrixLinearWithOptimizations)
		 * @see homographyMatrix(), Geometry::Homography::homographyMatrix().
		 */
		template <bool tRefine, bool tUseSVD>
		static bool homographyMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& homography, const unsigned int testCandidates = 8u, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), Indices32* usedIndices = nullptr, Worker* worker = nullptr);

		/**
		 * Calculates the homography between two images transforming the given image points between two images.
		 * This function can be applied if the given correspondences are not bijective.<br>
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = H * leftPoint).<br>
		 * This function applies template parameters for refine and useSVD. Thus, this function will create smaller binary sizes.
		 * @param leftImagePoints Image points in the left camera, must be valid
		 * @param numberLeftImagePoints the number of given left image points, with range [4, infinity)
		 * @param rightImagePoints Image points in the right camera, must be valid
		 * @param numberRightImagePoints The number of given right image points, with range [4, infinity)
		 * @param correspondences The pairs defining the non-bijective correspondences, with range [0, numberLeftImagePoints - 1]x[0, numberRightImagePoints - 1], must be valid
		 * @param numberCorrespondences The number of given correspondence pairs, with range [4, infinity)
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param right_H_left Resulting homography for the given image points, (rightPoint = right_H_left * leftPoint)
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [4, correspondences]
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold Maximal square pixel error between a right point and a transformed left point so that a point correspondence counts as valid, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tRefine True, to apply a non-linear least square optimization to increase the transformation accuracy after the RANSAC step
		 * @tparam tUseSVD True, to use the slower SVD approach (i.e., Homography::homographyMatrixSVD); False, to use the two-step approach (i.e., Homography::homographyMatrixLinearWithOptimizations)
		 * @see homographyMatrixForNonBijectiveCorrespondences().
		 */
		template <bool tRefine, bool tUseSVD>
		static bool homographyMatrixForNonBijectiveCorrespondences(const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator& randomGenerator, SquareMatrix3& right_H_left, const unsigned int testCandidates = 8u, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), Indices32* usedIndices = nullptr, Worker* worker = nullptr);

		/**
		 * Calculates four homographies between two images transforming the given image points between two images.
		 * The resulting homographies transforms image points defined in the left image to image points defined in the right image (rightPoint = H[i] * leftPoint).<br>
		 * Four homographies are determined, one homography for each quadrant in the frame allowing to address minor distortions in the frames.<br>
		 * The quadrant order of the homographies is as follows: top left, top right, bottom left, bottom right.
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param leftQuadrantCenter The center of the four quadrants in the left frame (this may be the center of the left frame), with range [0, leftImageWidth)x[0, leftImageHeight)
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param homographies Resulting four homographies for the given image points
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [4, correspondences]
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold Maximal square pixel error between a right point and a transformed left point so that a point correspondence counts as valid, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see Geometry::Homography::homographyMatrix().
		 */
		static bool homographyMatrices(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, const Vector2& leftQuadrantCenter, RandomGenerator& randomGenerator, SquareMatrix3 homographies[4], const unsigned int testCandidates = 8u, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), Indices32* usedIndices = nullptr, Worker* worker = nullptr);

		/**
		 * Calculates inverted flipped camera poses (up to a common 3d projection transformation) for image point corresponds between images from multiple views.
		 * @param imagePointsPerPose Image points per view (at least 3 views and at least 6 point correspondences)
		 * @param posesIF Resulting camera projection matrices per view (inverted flipped)
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold Maximal square pixel error of a valid projection of a 3D point onto the 2D image plane, with range (0, infinity)
		 * @param objectPointsIF Optional vector which will receive the object points of the image point correspondences, if defined
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see Geometry::MultipleViewGeometry::projectiveReconstructionFrom6PointsIF().
		 */
		static bool projectiveReconstructionFrom6PointsIF(const ConstIndexedAccessor<ImagePoints>& imagePointsPerPose, NonconstIndexedAccessor<HomogenousMatrix4>* posesIF, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), NonconstArrayAccessor<ObjectPoint>* objectPointsIF = nullptr, Indices32* usedIndices = nullptr, Worker* worker = nullptr);

		/**
		 * Determines the camera calibration for several given camera calibration patterns, each pattern object holds the 2D/3D point correspondences for a planar calibration pattern observed from an individual viewing position.
		 * The camera profile must be constant (must be identical) for all given calibration patterns.
		 *
		 * This calibration function uses RANSAC iterations to find the best calibration for the given point correspondence sets.<br>
		 * Thus, only a subset of the given correspondence sets are used for camera determination and then tested against the entire set.<br>
		 * The best matching subset is then used to create a calibration for all valid calibration patterns.
		 * @param width The width of the camera frame in pixel, with range [1, infinity)
		 * @param height The height of the camera frame in pixel, with range [1, infinity)
		 * @param calibrationPatterns The detected calibration patterns used for calibration, at least 4
		 * @param calibrationPatternBoxSize Size of one calibration pattern box in a specific unit (e.g., meter), with ranges (0, infinity)x(0, infinity)
		 * @param testCandidates Number of candidates used in each RANSAC iteration, with range [4, correspondences]
		 * @param pinholeCamera Resulting camera holding all extracted calibration information like e.g. intrinsic camera and distortion parameters
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param sqrAccuracy Optional accuracy value receiving the average square pixel error, if defined
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @return True, if succeeded
		 */
		static bool determineCameraCalibrationPlanar(const unsigned int width, const unsigned int height, const CameraCalibration::Patterns& calibrationPatterns, const Vector2& calibrationPatternBoxSize, const unsigned int testCandidates, PinholeCamera& pinholeCamera, const unsigned int iterations = 20u, Scalar* sqrAccuracy = nullptr, Worker* worker = nullptr, Indices32* usedIndices = nullptr);

		/**
		 * Performs execution of a camera calibration for a given subset of the entire data provided.
		 * The calibration is done for several individual groups of 3D object points all lying on the same 3D plane and corresponding 2D image points.
		 * The individual groups of image points can e.g., be the result of individual images of a calibration pattern observed from different viewing positions.
		 * @param width The width of the camera image in pixel, with range [1, infinity)
		 * @param height The height of the camera image in pixel, with range [1, infinity)
		 * @param objectPointGroups The groups of object points (all points lie in the Z == 0 plane), each group has a corresponding group of image points
		 * @param imagePointGroups The groups of image points, one group for each corresponding group of object points
		 * @param indices Indices defining the subset of given image and object points to be used for camera calibration calculation
		 * @param pinholeCamera Resulting camera profile holding all extracted calibration information like e.g. intrinsic camera and distortion parameters
		 * @param sqrAccuracy Optional resulting average square projection error in pixel accuracy, if the iteration fails the accuracy value is untouched
		 */
		static void determineCameraCalibrationPlanarIteration(const unsigned int width, const unsigned int height, const ConstIndexedAccessor<Vectors3>* objectPointGroups, const ConstIndexedAccessor<Vectors2>* imagePointGroups, Indices32* indices, PinholeCamera* pinholeCamera, Scalar* sqrAccuracy);

		/**
		 * Calculates the affine transformation (6DOF - translation, rotation, scale, aspect ratio, shear) between two sets of 2D image points.
		 * @param leftImagePoints The image points in the left image, must be valid
		 * @param rightImagePoints The image points in the right image, one for each point in the left image, must be valid
		 * @param correspondences The number of points correspondences, with range [3, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param right_A_left The resulting affine transformation matrix transforming left to right image points
		 * @param testCandidates The number of candidates used in each RANSAC iterations, with range [3, correspondences]
		 * @param iterations The number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold The maximal square pixel error between two corresponding image points to count as valid, with range (0, infinity)
		 * @param usedIndices Optional resulting indices of the used point correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see Homography::affineMatrix().
		 */
		static bool affineMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& right_A_left, const unsigned int testCandidates = 6u, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(3 * 3), Indices32* usedIndices = nullptr, Worker* worker = nullptr);

		/**
		 * Calculates the similarity transformation (4DOF - translation, rotation, scale) between two images transforming the given image points between two images.
		 * The resulting similarity transforms image points defined in the left image to image points defined in the right image (rightPoint = S * leftPoint).
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param similarity Resulting similarity for the given image points so that (rightPoint = S * leftPoint) holds
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [2, correspondences]
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold The maximal square pixel error, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @see Geometry::Homography::similarityMatrix().
		 */
		static bool similarityMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& similarity, const unsigned int testCandidates = 4u, const unsigned int iterations = 20u, const Scalar squarePixelErrorThreshold = Scalar(9), Indices32* usedIndices = nullptr, Worker* worker = nullptr);

		/**
		 * Determines the 6-DOF object transformation between world and object so that the transformation fits with two observations in two images (left and right stereo image).
		 * @param anyCameraA The camera profile of the first camera, must be valid
		 * @param anyCameraB the camera profile of the second camera, must be valid
		 * @param world_T_cameraA The transformation between first camera and world, must be valid
		 * @param world_T_cameraB The transformation between second camera and world, must be valid
		 * @param objectPointsA The 3D object points of the observation of the object in the first camera, defined in the coordinate system of the object
		 * @param objectPointsB The 3D object points of the observation of the object in the second camera, defined in the coordinate system of the object
		 * @param imagePointsA The 2D image points of the observation of the object in the first camera, one for each 3D object point
		 * @param imagePointsB The 2D image points of the observation of the object in the second camera, one for each 3D object point
		 * @param randomGenerator Random generator object to be used
		 * @param world_T_object The resulting transformation between object and world
		 * @param minimalValidCorrespondences Minimal number of correspondences so that the resulting transformation counts as valid, with range [4, objectPointsA.size() + objectPointsB.size()]
		 * @param refine True, to apply a non-linear least square optimization to increase the transformation accuracy after the RANSAC step
		 * @param iterations The number of RANSAC iteration to be applied, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square projection error for valid 2D/3D correspondences, with range [0, infinity)
		 * @param usedIndicesA Optional resulting indices of valid correspondences from the first camera
		 * @param usedIndicesB Optional resulting indices of valid correspondences from the second camera
		 * @param sqrAccuracy Optional resulting average square pixel error
		 * @param allowMonoObservation True, to allow that the object is visible in one of both camera frames only; False, to fail in case the object is visible in only one camera frame
		 * @return True, if succeeded
		 */
		static bool objectTransformationStereo(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const ConstIndexedAccessor<Vector3>& objectPointsA, const ConstIndexedAccessor<Vector3>& objectPointsB, const ConstIndexedAccessor<Vector2>& imagePointsA, const ConstIndexedAccessor<Vector2>& imagePointsB, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_object, const unsigned int minimalValidCorrespondences = 5u, const bool refine = true, const unsigned int iterations = 20u, const Scalar sqrPixelErrorThreshold = Scalar(5 * 5), Indices32* usedIndicesA = nullptr, Indices32* usedIndicesB = nullptr, Scalar* sqrAccuracy = nullptr, const bool allowMonoObservation = true);

	private:

		/**
		 * Calculates a camera pose using the perspective pose problem with three point correspondences.
		 * The calculation can use a rough camera pose (e.g., from a previous camera frame) to increase the robustness.
		 * @param world_T_roughCamera Optional already known rough camera pose, transforming rough camera points to world points, with default camera pointing towards the negative z-space with y-axis upwards, nullptr if unknown
		 * @param camera The camera profile defining the project, must be valid
		 * @param objectPointAccessor The accessor providing the 3D object points, each corresponding to one image point, at least four
		 * @param imagePointAccessor The accessor providing the 2D image points, each corresponding to one object point, at least four
		 * @param randomGenerator The random generator to be used
		 * @param world_T_camera The resulting camera pose, transforming camera points to world points, with default camera pointing towards the negative z-space with y-axis upwards, must be valid
		 * @param maxPositionOffset Optional maximal position offset between initial and final pose for three axis
		 * @param maxOrientationOffset Optional maximal orientation offset between initial and final pose in radian angle
		 * @param minValidCorrespondences Minimal number of valid correspondences
		 * @param refine Determines whether a not linear least square algorithm is used to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error, with range [0, infinity)
		 * @param weights Optional weights to weight the individual point correspondences individually, nullptr to use the same weight for all point correspondences
		 * @return True, if succeeded
		 */
		static bool p3p(const HomogenousMatrix4* world_T_roughCamera, const AnyCamera& camera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const Vector3* maxPositionOffset, const Scalar* maxOrientationOffset, const unsigned int minValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights);

		/**
		 * Calculates a pose including zoom factor using the perspective pose problem with three point correspondences.
		 * The calculation can use a pose from a previous calculation to increase the robustness.<br>
		 * The specified point correspondences should be sorted by strength or stability to enhance the pose determination.<br>
		 * This function may use apriori information (like e.g. a previous pose).
		 * @param initialPose Optional rough initial pose to increase determination robustness
		 * @param initialZoom Optional rough initial zoom to increase determination robustness
		 * @param pinholeCamera The pinhole camera object specifying the intrinsic camera parameters
		 * @param objectPointAccessor The accessor providing the 3D object points, each corresponding to one image point, at least four
		 * @param imagePointAccessor The accessor providing the 2D image points, each corresponding to one object point, at least four
		 * @param randomGenerator A random generator object
		 * @param useDistortionParameters True, to use the distortion parameters of the camera profile; False, otherwise
		 * @param pose Resulting pose
		 * @param zoom Resulting zoom factor
		 * @param maxPositionOffset Optional maximal position offset between initial and final pose for three axis
		 * @param maxOrientationOffset Optional maximal orientation offset between initial and final pose in radian angle
		 * @param minValidCorrespondences Minimal number of valid correspondences
		 * @param refine Determines whether a not linear least square algorithm is used to increase the pose accuracy after the RANSAC step
		 * @param iterations Number of maximal RANSAC iterations, with range [1, infinity)
		 * @param sqrPixelErrorThreshold Square pixel error threshold for valid RANSAC candidates
		 * @param usedIndices Optional vector receiving the indices of all valid correspondences
		 * @param sqrAccuracy Optional resulting average square pixel error
		 * @param weights Optional weights for each point correspondence
		 * @return True, if succeeded
		 */
		static bool p3pZoom(const HomogenousMatrix4* initialPose, const Scalar* initialZoom, const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, Scalar& zoom, const Vector3* maxPositionOffset, const Scalar* maxOrientationOffset, const unsigned int minValidCorrespondences = 5u, const bool refine = true, const unsigned int iterations = 20u, const Scalar sqrPixelErrorThreshold = Scalar(5 * 5), Indices32* usedIndices = nullptr, Scalar* sqrAccuracy = nullptr, const Scalar* weights = nullptr);

		/**
		 * Calculates the geometry transformation between two images transforming the given image points between two images.
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = M * leftPoint).
		 * @param geometricTransformFunction The pointer to the geometric transformation function to be applied, must be valid
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param transformMatrix Resulting homography for the given image points
		 * @param testCandidates Number of candidates used in each RANSAC iterations
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold The maximal square pixel error, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool geometricTransform(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& transformMatrix, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker);

		/**
		 * Calculates the geometry transformation between two images transforming the given image points between two images.
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = M * leftPoint).
		 * @param geometricTransformFunction The pointer to the geometric transformation function to be applied, must be valid
		 * @param leftImagePoints Image points in the left camera, must be valid
		 * @param numberLeftImagePoints The number of given left image points, with range [1, infinity)
		 * @param rightImagePoints Image points in the right camera, must be valid
		 * @param numberRightImagePoints The number of given right image points, with range [1, infinity)
		 * @param correspondences The pairs defining the non-bijective correspondences, with range [0, numberLeftImagePoints - 1]x[0, numberRightImagePoints - 1], must be valid
		 * @param numberCorrespondences The number of given correspondence pairs, with range [1, infinity)
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param transformMatrix Resulting homography for the given image points
		 * @param testCandidates Number of candidates used in each RANSAC iterations
		 * @param iterations Number of RANSAC iterations, with range [1, infinity)
		 * @param squarePixelErrorThreshold The maximal square pixel error, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool geometricTransformForNonBijectiveCorrespondences(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator& randomGenerator, SquareMatrix3& transformMatrix, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker);

		/**
		 * Internal function to calculate the geometry transformation between two images transforming the projected planar object points between the two images.
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = M * leftPoint).
		 * @param geometricTransformFunction The pointer to the geometric transformation function to be applied, must be valid
		 * @param leftImagePoints Image points in the left camera, each point corresponds to one point in the right image
		 * @param rightImagePoints Image points in the right camera
		 * @param correspondences Number of points correspondences
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param transformMatrix Resulting homography for the given image points
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [1, correspondences]
		 * @param squarePixelErrorThreshold The maximal square pixel error, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param maxValidCandidates Maximal number of candidates found already, this parameter will be used by all threads concurrently
		 * @param minSquareErrors Minimal square errors of candidates found already, this parameter will be used by all threads concurrently
		 * @param lock The lock object to avoid race conditions for the global parameter, should be nullptr in a single threaded call, must be defined in a multi threaded call
		 * @param firstIteration First iteration to be applied
		 * @param numberIterations Number of iterations to be applied
		 */
		static void geometricTransformSubset(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator* randomGenerator, SquareMatrix3* transformMatrix, const unsigned int testCandidates, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, unsigned int* maxValidCandidates, Scalar* minSquareErrors, Lock* lock, const unsigned int firstIteration, const unsigned int numberIterations);

		/**
		 * Internal function to calculate the geometry transformation between two images transforming the projected planar object points between the two images.
		 * The resulting homography transforms image points defined in the left image to image points defined in the right image (rightPoint = M * leftPoint).
		 * @param geometricTransformFunction The pointer to the geometric transformation function to be applied, must be valid
		 * @param leftImagePoints Image points in the left camera, must be valid
		 * @param numberLeftImagePoints The number of given left image points, with range [1, infinity)
		 * @param rightImagePoints Image points in the right camera, must be valid
		 * @param numberRightImagePoints The number of given right image points, with range [1, infinity)
		 * @param correspondences The pairs defining the non-bijective correspondences, with range [0, numberLeftImagePoints - 1]x[0, numberRightImagePoints - 1], must be valid
		 * @param numberCorrespondences The number of given correspondence pairs, with range [1, infinity)
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param transformMatrix Resulting homography for the given image points
		 * @param testCandidates Number of candidates used in each RANSAC iterations, with range [1, correspondences]
		 * @param squarePixelErrorThreshold The maximal square pixel error, with range (0, infinity)
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param maxValidCandidates Maximal number of candidates found already, this parameter will be used by all threads concurrently
		 * @param minSquareErrors Minimal square errors of candidates found already, this parameter will be used by all threads concurrently
		 * @param lock The lock object to avoid race conditions for the global parameter, should be nullptr in a single threaded call, must be defined in a multi threaded call
		 * @param firstIteration First iteration to be applied
		 * @param numberIterations Number of iterations to be applied
		 */
		static void geometricTransformForNonBijectiveCorrespondencesSubset(const GeometricTransformFunction geometricTransformFunction, const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator* randomGenerator, SquareMatrix3* transformMatrix, const unsigned int testCandidates, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, unsigned int* maxValidCandidates, Scalar* minSquareErrors, Lock* lock, const unsigned int firstIteration, const unsigned int numberIterations);

		/**
		 * Internal function to calculate inverted flipped camera poses (up to a common 3d projection transformation) for image point corresponds between images from multiple views.
		 * @param imagePointsPerPose Image points per view (at least 3 views and at least 6 point correspondences)
		 * @param views Count of views
		 * @param randomGenerator Random generator object to be used for creating random numbers
		 * @param posesIF Resulting camera projection matrices per view (inverted flipped)
		 * @param squarePixelErrorThreshold Maximal square pixel error of a valid projection of a 3D point onto the 2D image plane, with range (0, infinity)
		 * @param objectPointsIF Optional vector which will receive the object points of the image point correspondences, if defined
		 * @param usedIndices Optional vector which will receive the indices of the used image correspondences, if defined
		 * @param minSquareErrors Minimal square errors of candidates found already, this parameter will be used by all threads concurrently
		 * @param lock The lock object to avoid race conditions for the global parameter, should be nullptr in a single threaded call, must be defined in a multi threaded call
		 * @param firstIteration First iteration to be applied
		 * @param numberIterations Number of iterations to be applied
		 * @see Geometry::MultipleViewGeometry::projectiveReconstructionFrom6PointsIF().
		 */
		static void projectiveReconstructionFrom6PointsIFSubset(const ConstIndexedAccessor<ImagePoints>* imagePointsPerPose, const size_t views, RandomGenerator* randomGenerator, NonconstIndexedAccessor<HomogenousMatrix4>* posesIF, const Scalar squarePixelErrorThreshold, NonconstArrayAccessor<ObjectPoint>* objectPointsIF, Indices32* usedIndices, Scalar* minSquareErrors, Lock* lock, const unsigned int firstIteration, const unsigned int numberIterations);
};

inline bool RANSAC::homographyMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& homography, const unsigned int testCandidates, const bool refine, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker, const bool useSVD)
{
	ocean_assert(leftImagePoints != nullptr && rightImagePoints != nullptr);
	ocean_assert(correspondences >= 4);
	ocean_assert(testCandidates >= 4u);
	ocean_assert(squarePixelErrorThreshold > 0);

	if (refine)
	{
		if (useSVD)
		{
			return homographyMatrix<true, true>(leftImagePoints, rightImagePoints, correspondences, randomGenerator, homography, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
		else
		{
			return homographyMatrix<true, false>(leftImagePoints, rightImagePoints, correspondences, randomGenerator, homography, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
	}
	else
	{
		if (useSVD)
		{
			return homographyMatrix<false, true>(leftImagePoints, rightImagePoints, correspondences, randomGenerator, homography, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
		else
		{
			return homographyMatrix<false, false>(leftImagePoints, rightImagePoints, correspondences, randomGenerator, homography, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
	}
}

inline bool RANSAC::homographyMatrixForNonBijectiveCorrespondences(const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator& randomGenerator, SquareMatrix3& right_H_left, const unsigned int testCandidates, const bool refine, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker, const bool useSVD)
{
	ocean_assert(leftImagePoints != nullptr && rightImagePoints != nullptr);
	ocean_assert(numberLeftImagePoints >= 4 && numberRightImagePoints >= 4);
	ocean_assert(correspondences != nullptr);
	ocean_assert(numberCorrespondences >= 4);
	ocean_assert(testCandidates >= 4u);
	ocean_assert(squarePixelErrorThreshold > 0);

	if (refine)
	{
		if (useSVD)
		{
			return homographyMatrixForNonBijectiveCorrespondences<true, true>(leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, randomGenerator, right_H_left, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
		else
		{
			return homographyMatrixForNonBijectiveCorrespondences<true, false>(leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, randomGenerator, right_H_left, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
	}
	else
	{
		if (useSVD)
		{
			return homographyMatrixForNonBijectiveCorrespondences<false, true>(leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, randomGenerator, right_H_left, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
		else
		{
			return homographyMatrixForNonBijectiveCorrespondences<false, false>(leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, randomGenerator, right_H_left, testCandidates, iterations, squarePixelErrorThreshold, usedIndices, worker);
		}
	}
}

template <bool tRefine, bool tUseSVD>
bool RANSAC::homographyMatrix(const ImagePoint* leftImagePoints, const ImagePoint* rightImagePoints, const size_t correspondences, RandomGenerator& randomGenerator, SquareMatrix3& homography, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker)
{
	ocean_assert(leftImagePoints != nullptr && rightImagePoints != nullptr);
	ocean_assert(correspondences >= 4);
	ocean_assert(testCandidates >= 4u);
	ocean_assert(squarePixelErrorThreshold > 0);

	if (testCandidates < 4u || correspondences < testCandidates)
	{
		return false;
	}

	Indices32 tmpIndices;
	Indices32* indices = (tRefine || usedIndices) ? &tmpIndices : nullptr;

	unsigned int maxValidCorrespondences = testCandidates - 1u;
	Scalar minSquareErrors = Numeric::maxValue();

	if (worker != nullptr)
	{
		Lock lock;

		if constexpr (tUseSVD)
		{
			worker->executeFunction(Worker::Function::createStatic(&geometricTransformSubset, Homography::homographyMatrixSVD, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &homography, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, (Lock*)(&lock), 0u, 0u), 0u, iterations, 12u, 13u, 5u);
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&geometricTransformSubset, Homography::homographyMatrixLinearWithoutOptimations, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &homography, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, (Lock*)(&lock), 0u, 0u), 0u, iterations, 12u, 13u, 5u);
		}
	}
	else
	{
		if constexpr (tUseSVD)
		{
			geometricTransformSubset(Homography::homographyMatrixSVD, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &homography, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, nullptr, 0u, iterations);
		}
		else
		{
			geometricTransformSubset(Homography::homographyMatrixLinearWithoutOptimations, leftImagePoints, rightImagePoints, correspondences, &randomGenerator, &homography, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, nullptr, 0u, iterations);
		}
	}

	if (maxValidCorrespondences < testCandidates || homography.isSingular())
	{
		return false;
	}

	Homography::normalizeHomography(homography);

	if constexpr (tRefine)
	{
		ocean_assert(indices);

		const Vectors2 validLeftImagePoints(Subset::subset(leftImagePoints, correspondences, *indices));
		const Vectors2 validRightImagePoints(Subset::subset(rightImagePoints, correspondences, *indices));

		SquareMatrix3 optimizedHomography;
		if (Geometry::NonLinearOptimizationHomography::optimizeHomography<Geometry::Estimator::ET_SQUARE>(homography, validLeftImagePoints.data(), validRightImagePoints.data(), validLeftImagePoints.size(), 9u, optimizedHomography, 20u))
		{
			homography = optimizedHomography;

			if (usedIndices)
			{
				indices->clear();

				for (size_t n = 0; n < correspondences; ++n)
				{
					if (rightImagePoints[n].sqrDistance(homography * leftImagePoints[n]) <= squarePixelErrorThreshold)
					{
						indices->emplace_back(Index32(n));
					}
				}
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(tmpIndices);
	}

	return true;
}

template <bool tRefine, bool tUseSVD>
bool RANSAC::homographyMatrixForNonBijectiveCorrespondences(const ImagePoint* leftImagePoints, const size_t numberLeftImagePoints, const ImagePoint* rightImagePoints, const size_t numberRightImagePoints, const IndexPair32* correspondences, const size_t numberCorrespondences, RandomGenerator& randomGenerator, SquareMatrix3& right_H_left, const unsigned int testCandidates, const unsigned int iterations, const Scalar squarePixelErrorThreshold, Indices32* usedIndices, Worker* worker)
{
	ocean_assert(squarePixelErrorThreshold > 0);
	ocean_assert(leftImagePoints && rightImagePoints);
	ocean_assert(correspondences != nullptr);
	ocean_assert(numberCorrespondences >= 4);

	if (testCandidates < 4u || numberCorrespondences < testCandidates)
	{
		return false;
	}

	Indices32 tmpIndices;
	Indices32* indices = (tRefine || usedIndices) ? &tmpIndices : nullptr;

	unsigned int maxValidCorrespondences = testCandidates - 1u;
	Scalar minSquareErrors = Numeric::maxValue();

	if (worker != nullptr)
	{
		Lock lock;

		if constexpr (tUseSVD)
		{
			worker->executeFunction(Worker::Function::createStatic(&geometricTransformForNonBijectiveCorrespondencesSubset, Homography::homographyMatrixSVD, leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, &randomGenerator, &right_H_left, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, (Lock*)&lock, 0u, 0u), 0u, iterations, 15u, 16u, 5u);
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&geometricTransformForNonBijectiveCorrespondencesSubset, Homography::homographyMatrixLinearWithoutOptimations, leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, &randomGenerator, &right_H_left, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, (Lock*)&lock, 0u, 0u), 0u, iterations, 15u, 16u, 5u);
		}
	}
	else
	{
		if constexpr (tUseSVD)
		{
			geometricTransformForNonBijectiveCorrespondencesSubset(Homography::homographyMatrixSVD, leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, &randomGenerator, &right_H_left, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, nullptr, 0u, iterations);
		}
		else
		{
			geometricTransformForNonBijectiveCorrespondencesSubset(Homography::homographyMatrixLinearWithoutOptimations, leftImagePoints, numberLeftImagePoints, rightImagePoints, numberRightImagePoints, correspondences, numberCorrespondences, &randomGenerator, &right_H_left, testCandidates, squarePixelErrorThreshold, indices, &maxValidCorrespondences, &minSquareErrors, nullptr, 0u, iterations);
		}
	}

	if (maxValidCorrespondences < testCandidates || right_H_left.isSingular())
	{
		return false;
	}

	Homography::normalizeHomography(right_H_left);

	if constexpr (tRefine)
	{
		ocean_assert(indices);

		Vectors2 validLeftImagePoints;
		Vectors2 validRightImagePoints;

		validLeftImagePoints.reserve(indices->size());
		validRightImagePoints.reserve(indices->size());

		for (const Index32& index : *indices)
		{
			const IndexPair32& correspondencePair = correspondences[index];

			const unsigned int leftIndex = correspondencePair.first;
			const unsigned int rightIndex = correspondencePair.second;

			validLeftImagePoints.push_back(leftImagePoints[leftIndex]);
			validRightImagePoints.push_back(rightImagePoints[rightIndex]);
		}

		SquareMatrix3 optimizedHomography;
		if (Geometry::NonLinearOptimizationHomography::optimizeHomography<Geometry::Estimator::ET_SQUARE>(right_H_left, validLeftImagePoints.data(), validRightImagePoints.data(), validLeftImagePoints.size(), 9u, optimizedHomography, 20u))
		{
			right_H_left = optimizedHomography;

			if (usedIndices)
			{
				indices->clear();

				for (size_t n = 0; n < numberCorrespondences; ++n)
				{
					const IndexPair32& correspondencePair = correspondences[n];

					const unsigned int leftIndex = correspondencePair.first;
					const unsigned int rightIndex = correspondencePair.second;

					if (rightImagePoints[rightIndex].sqrDistance(right_H_left * leftImagePoints[leftIndex]) <= squarePixelErrorThreshold)
					{
						indices->push_back((unsigned int)n);
					}
				}
			}
		}
	}

	if (usedIndices != nullptr)
	{
		*usedIndices = std::move(tmpIndices);
	}

	return true;
}

inline bool RANSAC::p3p(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, const unsigned int minimalValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights)
{
	const AnyCameraPinhole anyCameraPinhole(PinholeCamera(pinholeCamera, useDistortionParameters));

	return p3p(nullptr, anyCameraPinhole, objectPointAccessor, imagePointAccessor, randomGenerator, pose, nullptr, nullptr, minimalValidCorrespondences, refine, iterations, sqrPixelErrorThreshold, usedIndices, sqrAccuracy, weights);
}

inline bool RANSAC::p3p(const HomogenousMatrix4& world_T_roughCamera, const AnyCamera& camera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, HomogenousMatrix4& world_T_camera, const Vector3& maxPositionOffset, const Scalar maxOrientationOffset, const unsigned int minValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights)
{
	return p3p(&world_T_roughCamera, camera, objectPointAccessor, imagePointAccessor, randomGenerator, world_T_camera, &maxPositionOffset, &maxOrientationOffset, minValidCorrespondences, refine, iterations, sqrPixelErrorThreshold, usedIndices, sqrAccuracy, weights);
}

inline bool RANSAC::p3p(const HomogenousMatrix4& initialPose, const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, const Vector3& maxPositionOffset, const Scalar maxOrientationOffset, const unsigned int minValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights)
{
	const AnyCameraPinhole anyCameraPinhole(PinholeCamera(pinholeCamera, useDistortionParameters));

	return p3p(&initialPose, anyCameraPinhole, objectPointAccessor, imagePointAccessor, randomGenerator, pose, &maxPositionOffset, &maxOrientationOffset, minValidCorrespondences, refine, iterations, sqrPixelErrorThreshold, usedIndices, sqrAccuracy, weights);
}

inline bool RANSAC::p3pZoom(const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, Scalar& zoom, const unsigned int minimalValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights)
{
	return p3pZoom(nullptr, nullptr, pinholeCamera, objectPointAccessor, imagePointAccessor, randomGenerator, useDistortionParameters, pose, zoom, nullptr, nullptr, minimalValidCorrespondences, refine, iterations, sqrPixelErrorThreshold, usedIndices, sqrAccuracy, weights);
}

inline bool RANSAC::p3pZoom(const HomogenousMatrix4& initialPose, const Scalar initialZoom, const PinholeCamera& pinholeCamera, const ConstIndexedAccessor<ObjectPoint>& objectPointAccessor, const ConstIndexedAccessor<ImagePoint>& imagePointAccessor, RandomGenerator& randomGenerator, const bool useDistortionParameters, HomogenousMatrix4& pose, Scalar& zoom, const Vector3& maxPositionOffset, const Scalar maxOrientationOffset, const unsigned int minValidCorrespondences, const bool refine, const unsigned int iterations, const Scalar sqrPixelErrorThreshold, Indices32* usedIndices, Scalar* sqrAccuracy, const Scalar* weights)
{
	ocean_assert(initialZoom > Numeric::eps());
	return p3pZoom(&initialPose, &initialZoom, pinholeCamera, objectPointAccessor, imagePointAccessor, randomGenerator, useDistortionParameters, pose, zoom, &maxPositionOffset, &maxOrientationOffset, minValidCorrespondences, refine, iterations, sqrPixelErrorThreshold, usedIndices, sqrAccuracy, weights);
}

inline bool RANSAC::objectPoint(const AnyCamera& camera, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<ImagePoint>& imagePoints, RandomGenerator& randomGenerator, ObjectPoint& objectPoint, const unsigned int iterations, const Scalar maximalSqrError, const unsigned int minValidCorrespondences, const bool onlyFrontObjectPoint, const Estimator::EstimatorType refinementEstimator, Scalar* finalRobustError, Indices32* usedIndices)
{
#if 1
	// creating local pointer to avoid Clang compiler bug
	const AnyCamera* cameraPointer = &camera;
	const ConstElementAccessor<const AnyCamera*> cameraAccessor(world_T_cameras.size(), cameraPointer);
#else
	const ConstElementAccessor<const AnyCamera*> cameraAccessor(world_T_cameras.size(), &camera);
#endif

	return RANSAC::objectPoint(cameraAccessor, world_T_cameras, imagePoints, randomGenerator, objectPoint, iterations, maximalSqrError, minValidCorrespondences, onlyFrontObjectPoint, refinementEstimator, finalRobustError, usedIndices);
}

}

}

#endif // META_OCEAN_GEOMETRY_RANSAC_H
