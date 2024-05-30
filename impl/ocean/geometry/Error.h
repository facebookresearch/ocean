/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_ERROR_H
#define META_OCEAN_GEOMETRY_ERROR_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/Estimator.h"

#include "ocean/base/Accessor.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"

#include <algorithm>

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements to functions to determine the error or accuracy of geometric functions and their parameter.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Error
{
	public:

		/**
		 * Definition of different error determination stages.
		 */
		enum ErrorDetermination
		{
			/// Invalid stage.
			ED_INVALID,
			/// Unique error determination.
			ED_UNIQUE,
			/// Approximated error determination.
			ED_APPROXIMATED,
			/// Ambiguous error determination.
			ED_AMBIGUOUS
		};

	private:

		/**
		 * This class implements an element storing the error between to image points.
		 * Further, the indices of the points are stored.
		 */
		class ErrorElement
		{
			public:

				/**
				 * Creates a new error element.
				 * @param imageIndex Index of the image point
				 * @param candidateIndex Index of the candidate point
				 * @param error The error between the two points
				 */
				inline ErrorElement(const unsigned int imageIndex, const unsigned int candidateIndex, const Scalar error);

				/**
				 * Returns the index of the image point.
				 * @return Image point index
				 */
				inline unsigned int imageIndex() const;

				/**
				 * Return the index of the candidate point.
				 * @return Candidate point index
				 */
				inline unsigned int candidateIndex() const;

				/**
				 * Returns the error between the two points.
				 * @return Stored error value
				 */
				inline Scalar error() const;

				/**
				 * Returns whether the left element has a smaller error than the right one.
				 * @param element Right element to compare
				 * @return True, if so
				 */
				inline bool operator<(const ErrorElement& element) const;

			private:

				/// Index of the image point.
				unsigned int imageIndex_ = (unsigned int)(-1);

				/// Index of the candidate point.
				unsigned int candidateIndex_ = (unsigned int)(-1);

				/// Error between the two points.
				Scalar error_ = Numeric::maxValue();
		};

		/**
		 * Definition of a vector holding error elements.
		 */
		typedef std::vector<ErrorElement> ErrorElements;

	public:

		/**
		 * Determines the indices of a set of given parameter values that are below ore equal to a provided threshold.
		 * @param parameters Parameter values that have to be investigated
		 * @param number The number of provided values, with range [1, infinity)
		 * @param threshold The threshold that is used to filter the parameter values
		 * @param validIndices Resulting indices of the provided parameter values that are below the specified threshold
		 */
		static void determineValidParameters(const Scalar* parameters, const size_t number, const Scalar threshold, Indices32& validIndices);

		/**
		 * Determines the indices of a set of given parameter values that are above a provided threshold.
		 * @param parameters Parameter values that have to be investigated
		 * @param number The number of provided values, with range [1, infinity)
		 * @param threshold The threshold that is used to filter the parameter values
		 * @param validIndices Resulting indices of the provided parameter values that are above the specified threshold
		 */
		static void determineInvalidParameters(const Scalar* parameters, const size_t number, const Scalar threshold, Indices32& validIndices);

		/**
		 * Returns the average square error between two sets of 2D positions.
		 * Each point in the first point set correspond to a point in the second point set with same index.
		 * @param firstPoints First set of 2D points, each point has a corresponding point in the second set
		 * @param secondPoints Second set of 2D points
		 * @param errors Optional resulting errors individual for each provided point pair, make sure that the provided buffer is large enough
		 * @param sqrErrors Optional resulting squared errors individual for each provided point pair, make sure that the provided buffer is large enough
		 * @return Average square error between all point correspondences
		 */
		static Scalar determineAverageError(const Vectors2& firstPoints, const Vectors2& secondPoints, Vector2* errors = nullptr, Scalar* sqrErrors = nullptr);

		/**
		 * Returns the average square error between two sets of 2D positions.
		 * Each point in the first point set correspond to a point in the second point set with same index.
		 * @param firstTransformation Transformation that will be applied for all image points from the first set
		 * @param firstPoints First set of 2D points, each point has a corresponding point in the second set
		 * @param secondTransformation Transformation that will be applied for all image points from the second set
		 * @param secondPoints Second set of 2D points
		 * @return Average square error between all point correspondences
		 */
		static Scalar determineAverageError(const SquareMatrix3& firstTransformation, const Vectors2& firstPoints, const SquareMatrix3& secondTransformation, const Vectors2& secondPoints);

		/**
		 * Returns the average square error between two sets of 3D positions.
		 * Each point in the first point set correspond to a point in the second point set with same index.
		 * @param firstPoints First set of 3D points, each point has a corresponding point in the second set
		 * @param secondPoints Second set of 3D points
		 * @return Average square error between all point correspondences
		 */
		static Scalar determineAverageError(const Vectors3& firstPoints, const Vectors3& secondPoints);

		/**
		 * Determining the average, minimal and maximal square error between two sets of 2D positions.
		 * Each point in the first point set correspond to a point in the second point set with same index.
		 * @param firstPoints First set of 2D points, each point has a corresponding point in the second set
		 * @param secondPoints Second set of 2D points
		 * @param sqrAverageError Average square error of all point correspondences
		 * @param sqrMinimalError Minimal square error of all point correspondences
		 * @param sqrMaximalError Maximal square error of all point correspondences
		 * @return Sum of square errors of all point correspondences
		 */
		static Scalar determineError(const Vectors2& firstPoints, const Vectors2& secondPoints, Scalar& sqrAverageError, Scalar& sqrMinimalError, Scalar& sqrMaximalError);

		/**
		 * Returns the average, minimal and maximal square error between two sets of 3D positions.
		 * Each point in the first point set correspond to a point in the second point set with same index.
		 * @param firstPoints First set of 3D points, each point has a corresponding point in the second set
		 * @param secondPoints Second set of 3D points
		 * @param sqrAverageError Average square error of all point correspondences
		 * @param sqrMinimalError Minimal square error of all point correspondences
		 * @param sqrMaximalError Maximal square error of all point correspondences
		 * @return Sum of square errors of all point correspondences
		 */
		static Scalar determineError(const Vectors3& firstPoints, const Vectors3& secondPoints, Scalar& sqrAverageError, Scalar& sqrMinimalError, Scalar& sqrMaximalError);

		/**
		 * Returns whether the offsets between two given 6DOF poses are below specified thresholds.
		 * @param poseFirst First pose to be checked
		 * @param poseSecond Second poses to be checked
		 * @param maxTranslationOffset Maximal allowed translation offset, with positive values for the individual translation axes
		 * @param maxOrientationOffset Maximal allowed orientation offset, defined in radian
		 * @return True, if so
		 */
		static bool posesAlmostEqual(const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const Vector3& maxTranslationOffset = Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1)), const Scalar maxOrientationOffset = Numeric::deg2rad(15));

		/**
		 * Returns whether the offsets between two given 6DOF poses are below specified thresholds.
		 * @param poseFirst First pose to be checked
		 * @param poseSecond Second poses to be checked
		 * @param maxTranslationOffset Maximal allowed translation offset, with positive values for the individual translation axes, might be Vector3(Scalar(0.1), Scalar(0.1), Scalar(0.1))
		 * @return True, if so
		 */
		static inline bool posesAlmostEqual(const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const Vector3& maxTranslationOffset);

		/**
		 * Returns whether the offsets between two given 6DOF poses are below specified thresholds.
		 * @param poseFirst First pose to be checked
		 * @param poseSecond Second poses to be checked
		 * @param maxOrientationOffset Maximal allowed orientation offset, defined in radian, might be Numeric::deg2rad(15)
		 * @return True, if so
		 */
		static inline bool posesAlmostEqual(const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const Scalar maxOrientationOffset);

		/**
		 * Determines the accuracy of the intrinsic camera matrix (and camera distortion parameters if requested).
		 * The accuracy is determined by transforming the normalized 3D image points (3D object points transformed by the flipped and inverted extrinsic matrix) to the image plane.<br>
		 * @param pinholeCamera The pinhole camera object to be tested
		 * @param normalizedObjectPoints Normalized object points
		 * @param imagePoints Image points, each point corresponds to one normalized object point
		 * @param correspondences Number of correspondences to be checked
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @param errors Optional resulting error values individually for each given point correspondence
		 * @param sqrErrors Optional resulting squared error values individually for each given point correspondence
		 * @return Returns the average square projection pixel error
		 */
		static Scalar determineCameraError(const PinholeCamera& pinholeCamera, const Vector2* normalizedObjectPoints, const Vector2* imagePoints, const size_t correspondences, const bool useDistortionParameters, Vector2* errors = nullptr, Scalar* sqrErrors = nullptr);

		/**
		 * Determines the accuracy of the intrinsic camera matrix (and camera distortion parameters if requested).
		 * The accuracy is determined by transforming the normalized 3D image points (3D object points transformed by the flipped and inverted extrinsic matrix) to the image plane.<br>
		 * @param pinholeCamera The pinhole camera object to be tested
		 * @param normalizedObjectPoints Normalized object points
		 * @param imagePoints Image points, each point corresponds to one normalized object point
		 * @param correspondences Number of correspondences to be checked
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @param sqrAveragePixelError Average square projection pixel error
		 * @param sqrMinimalPixelError Minimal square projection pixel error
		 * @param sqrMaximalPixelError Maximal square projection pixel error
		 */
		static void determineCameraError(const PinholeCamera& pinholeCamera, const Vector2* normalizedObjectPoints, const Vector2* imagePoints, const size_t correspondences, const bool useDistortionParameters, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError);

		/**
		 * Determines the accuracy of a given homography for a set of corresponding image points.
		 * The given homography H transforms a point p0 from the first set of image points to the corresponding point p1 from the second set of image points: p1 = H * p0
		 * In case, a transformed point cannot be normalized (de-homogenized) by the z-component, the individual errors will be set to Numeric::maxValue() and the resulting average error will be accordingly.
		 * @param points1_H_points0 The homography transforming points0 to points1, must be valid
		 * @param imagePointAccessor0 The first set of image points, may be empty
		 * @param imagePointAccessor1 The second set of image points, each point has a corresponding point in the first set
		 * @param errors Optional resulting error values individually for each given point correspondence
		 * @param sqrErrors Optional resulting squared error values individually for each given point correspondence
		 * @return Returns the average square pixel error
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tResultingErrors True, if errors is defined
		 * @tparam tResultingSqrErrors True, if sqrErrors is defined
		 */
		template <typename TAccessorImagePoints, bool tResultingErrors, bool tResultingSqrErrors>
		static Scalar determineHomographyError(const SquareMatrix3& points1_H_points0, const TAccessorImagePoints& imagePointAccessor0, const TAccessorImagePoints& imagePointAccessor1, Vector2* errors = nullptr, Scalar* sqrErrors = nullptr);

		/**
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param world_T_camera The camera pose transforming camera to world, with default camera pointing towards the negative z-space, with y-axis upwards, must be valid
		 * @param camera The camera model defining the projection, must be valid
		 * @param objectPoint The 3D Object point, defined in world
		 * @param imagePoint The 2D image point corresponding to the object point, defined in the camera pixel domain
		 * @return The resulting error value
		 */
		static inline Vector2 determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const Vector3& objectPoint, const Vector2& imagePoint);

		/**
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param flippedCamera_T_world The inverted and flipped camera pose, transforming world to flipped camera, with default flipped camera pointing towards the positive z-space, with y-axis downwards, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPoint The 3D Object point, defined in world
		 * @param imagePoint The 2D image point corresponding to the object point, defined in the camera pixel domain
		 * @return The resulting error value
		 */
		static inline Vector2 determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const Vector3& objectPoint, const Vector2& imagePoint);

		/**
		 * Deprecated.
		 *
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param world_T_camera The camera pose transforming camera to world, must be valid
		 * @param pinholeCamera The pinhole camera model defining the projection, must be valid
		 * @param objectPoint 3D Object point defined in world
		 * @param imagePoint 2D Image point corresponding to the object point
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @return Resulting error value
		 */
		static inline Vector2 determinePoseError(const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const Vector3& objectPoint, const Vector2& imagePoint, const bool useDistortionParameters);

		/**
		 * Deprecated.
		 *
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param flippedCamera_T_world Inverted and flipped camera pose, transforming world to flipped camera, must be valid
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param objectPoint 3D Object point defined in world
		 * @param imagePoint 2D Image point corresponding to the object point
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @return Resulting error value
		 */
		static inline Vector2 determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vector3& objectPoint, const Vector2& imagePoint, const bool useDistortionParameters);

		/**
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param world_T_camera The camera pose transforming camera to world, must be valid
		 * @param pinholeCamera The pinhole camera model defining the projection, must be valid
		 * @param objectPointAccessor The accessor providing the 3D object points defined in world
		 * @param imagePointAccessor The accessor providing the 2D image points corresponding to the image points, one image point for each object point
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @param errors Optional resulting error values individually for each given point correspondence
		 * @param sqrErrors Optional resulting squared error values individually for each given point correspondence
		 * @return Returns the average squared projection pixel error
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tUseBorderDistortionIfOutside True, to apply the camera distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False, to apply the distortion from the given position
		 * @tparam tResultingErrors True, if errors is defined
		 * @tparam tResultingSqrErrors True, if sqrErrors is defined
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside, bool tResultingErrors, bool tResultingSqrErrors>
		static inline Scalar determinePoseError(const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, const Scalar zoom = Scalar(1), Vector2* errors = nullptr, Scalar* sqrErrors = nullptr);

		/**
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param world_T_camera The camera pose, transforming camera to world, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param objectPointAccessor The accessor providing the 3D object points corresponding to the given pose
		 * @param imagePointAccessor The accessor providing the 2D image points corresponding to the image points, one image point for each object point
		 * @param errors Optional resulting error values individually for each given point correspondence
		 * @param sqrErrors Optional resulting squared error values individually for each given point correspondence
		 * @return Returns the average squared projection pixel error
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tResultingErrors True, if errors is defined
		 * @tparam tResultingSqrErrors True, if sqrErrors is defined
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tResultingErrors, bool tResultingSqrErrors>
		static Scalar determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& anyCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Vector2* errors = nullptr, Scalar* sqrErrors = nullptr);

		/**
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param flippedCamera_T_world Inverted and flipped extrinsic camera pose, transforming world to flipped camera, must be valid
		 * @param pinholeCamera The pinhole camera model defining the projection, must be valid
		 * @param objectPointAccessor The accessor providing the 3D object points defined in world
		 * @param imagePointAccessor The accessor providing the 2D image points corresponding to the image points, one image point for each object point
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @param errors Optional resulting error values individually for each given point correspondence
		 * @param sqrErrors Optional resulting squared error values individually for each given point correspondence
		 * @return Returns the average squared projection pixel error
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tUseBorderDistortionIfOutside True, to apply the camera distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False, to apply the distortion from the given position
		 * @tparam tResultingErrors True, if errors is defined
		 * @tparam tResultingSqrErrors True, if sqrErrors is defined
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside, bool tResultingErrors, bool tResultingSqrErrors>
		static Scalar determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, const Scalar zoom = Scalar(1), Vector2* errors = nullptr, Scalar* sqrErrors = nullptr);

		/**
		 * Determines the accuracy of the camera pose based on 2D/3D correspondences.
		 * The accuracy is determined based on the projection errors between projected 3D points and their corresponding 2D image points.
		 * @param flippedCamera_T_world The inverted and flipped camera pose, transforming world to flipped camera, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param objectPointAccessor The accessor providing the 3D object points corresponding to the given pose
		 * @param imagePointAccessor The accessor providing the 2D image points corresponding to the image points, one image point for each object point
		 * @param errors Optional resulting error values individually for each given point correspondence
		 * @param sqrErrors Optional resulting squared error values individually for each given point correspondence
		 * @return Returns the average squared projection pixel error
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tResultingErrors True, if errors is defined
		 * @tparam tResultingSqrErrors True, if sqrErrors is defined
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tResultingErrors, bool tResultingSqrErrors>
		static Scalar determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Vector2* errors = nullptr, Scalar* sqrErrors = nullptr);

		/**
		 * Determines the accuracy of a camera pose in combination with the corresponding camera profile.
		 * The accuracy is determined by transforming the given 3D object points using the extrinsic camera parameter and projecting these points onto the image plane.
		 * @param world_T_camera The camera pose, transforming camera to world, with default camera pointing towards the negative z-space, with y-axis upwards, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPointAccessor Accessor providing the 3D object points corresponding to the given pose
		 * @param imagePointAccessor Accessor providing the 2D image points corresponding to the image points
		 * @param sqrAveragePixelError Average square projection pixel error
		 * @param sqrMinimalPixelError Minimal square projection pixel error
		 * @param sqrMaximalPixelError Maximal square projection pixel error
		 * @return True, if the error could be determined for all point correspondences; False, if the input was invalid or e.g., a 3D object point is located behind the camera and 'tOnlyFrontObjectPoints == true'
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tOnlyFrontObjectPoints True, to fail in case a 3D object point is not in front of the camera; False, to ignore whether 3D object points are in front of behind the camera
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tOnlyFrontObjectPoints>
		[[nodiscard]] static inline bool determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError);

		/**
		 * Deprecated.
		 *
		 * Determines the accuracy of a camera pose in combination with the corresponding camera profile.
		 * The accuracy is determined by transforming the given 3D object points using the extrinsic camera parameter and projecting these points onto the image plane.
		 * @param world_T_camera The camera pose, transforming camera to world, with default camera pointing towards the negative z-space, with y-axis upwards, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPointAccessor Accessor providing the 3D object points corresponding to the given pose
		 * @param imagePointAccessor Accessor providing the 2D image points corresponding to the image points
		 * @param sqrAveragePixelError Average square projection pixel error
		 * @param sqrMinimalPixelError Minimal square projection pixel error
		 * @param sqrMaximalPixelError Maximal square projection pixel error
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints>
		static inline void determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError);

		/**
		 * Determines the accuracy of the extrinsic and intrinsic camera matrix (and camera distortion if requested).
		 * The accuracy is determined by transforming the given 3D object points using the extrinsic camera parameter and projecting these points onto the image plane.
		 * @param world_T_camera The camera pose, transforming camera to world, must be valid
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param objectPointAccessor Accessor providing the 3D object points corresponding to the given pose
		 * @param imagePointAccessor Accessor providing the 2D image points corresponding to the image points
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @param sqrAveragePixelError Average square projection pixel error
		 * @param sqrMinimalPixelError Minimal square projection pixel error
		 * @param sqrMaximalPixelError Maximal square projection pixel error
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tUseBorderDistortionIfOutside True, to apply the camera distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False, to apply the distortion from the given position
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside>
		static inline void determinePoseError(const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError, const Scalar zoom = Scalar(1));

		/**
		 * Determines the accuracy of a camera pose in combination with the corresponding camera profile.
		 * The accuracy is determined by transforming the given 3D object points using the extrinsic camera parameter and projecting these points onto the image plane.<br>
		 * @param flippedCamera_T_world The inverted and flipped camera pose, transforming world to flipped camera, with default flipped camera pointing towards the positive z-space, with y-axis downwards, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPointAccessor Accessor providing the 3D object points defined in world
		 * @param imagePointAccessor Accessor providing the 2D image points corresponding to the image points
		 * @param sqrAveragePixelError Average square projection pixel error
		 * @param sqrMinimalPixelError Minimal square projection pixel error
		 * @param sqrMaximalPixelError Maximal square projection pixel error
		 * @return True, if the error could be determined for all point correspondences; False, if the input was invalid or e.g., a 3D object point is located behind the camera and 'tOnlyFrontObjectPoints == true'
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tOnlyFrontObjectPoints True, to fail in case a 3D object point is not in front of the camera; False, to ignore whether 3D object points are in front of behind the camera
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tOnlyFrontObjectPoints>
		[[nodiscard]] static bool determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError);

		/**
		 * Deprecated.
		 *
		 * Determines the accuracy of a camera pose in combination with the corresponding camera profile.
		 * The accuracy is determined by transforming the given 3D object points using the extrinsic camera parameter and projecting these points onto the image plane.<br>
		 * @param flippedCamera_T_world The inverted and flipped camera pose, transforming world to flipped camera, with default flipped camera pointing towards the positive z-space, with y-axis downwards, must be valid
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPointAccessor Accessor providing the 3D object points defined in world
		 * @param imagePointAccessor Accessor providing the 2D image points corresponding to the image points
		 * @param sqrAveragePixelError Average square projection pixel error
		 * @param sqrMinimalPixelError Minimal square projection pixel error
		 * @param sqrMaximalPixelError Maximal square projection pixel error
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints>
		static void determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError);

		/**
		 * Determines the accuracy of the extrinsic and intrinsic camera matrix (and camera distortion if requested).
		 * The accuracy is determined by transforming the given 3D object points using the extrinsic camera parameter and projecting these points onto the image plane.<br>
		 * Beware: The given camera matrix is not equal to a extrinsic matrix.<br>
		 * Instead, camera matrix is the extrinsic camera matrix flipped around the x-axis and inverted afterwards.
		 * @param flippedCamera_T_world Inverted and flipped camera pose, transforming world to flipped camera, must be valid
		 * @param pinholeCamera The pinhole camera specifying the internal camera parameters and optionally distortion
		 * @param objectPointAccessor Accessor providing the 3D object points defined in world
		 * @param imagePointAccessor Accessor providing the 2D image points corresponding to the image points
		 * @param useDistortionParameters True, to respect the distortion parameters of the given camera during object point projection
		 * @param sqrAveragePixelError Average square projection pixel error
		 * @param sqrMinimalPixelError Minimal square projection pixel error
		 * @param sqrMaximalPixelError Maximal square projection pixel error
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @tparam TAccessorObjectPoints The template type of the accessor for the object points
		 * @tparam TAccessorImagePoints The template type of the accessor for the image points
		 * @tparam tUseBorderDistortionIfOutside True, to apply the camera distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False, to apply the distortion from the given position
		 */
		template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside>
		static void determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError, const Scalar zoom = Scalar(1));

		/**
		 * Determines the unique robust minimal average square error between two 2D points clouds.
		 * This function calls uniqueAveragedRobustErrorInPointCloud() or ambiguousAveragedRobustErrorInPointCloud() depending on the uniqueCorrespondences parameter.<br>
		 * @param imagePoints Image points to determine the minimal errors for, must be valid
		 * @param numberImagePoints Number of given image points, with range [1, numberCandidatePoints]
		 * @param validImagePoints The number of image points which can be expected to have a unique corresponding point inside the candidate set, with range [1, numberImagePoints]
		 * @param candidatePoints Possible candidate image points to be used for finding the minimal error, must be valid
		 * @param numberCandidatePoints Number of given candidate points, with range [1, infinity)
		 * @param errorDetermination Depending on this flag uniqueAveragedRobustErrorInPointCloud, approximatedAveragedRobustErrorInPointCloud() or ambiguousAveragedRobustErrorInPointCloud() will be used
		 * @param correspondences Optional resulting point correspondences, for each index of an image point one corresponding candidate point index will be returned
		 * @return Robust averaged square error
		 * @tparam tEstimator Estimator type to be applied
		 * @see uniqueAveragedRobustErrorInPointCloud(), approximatedAveragedRobustErrorInPointCloud(), ambiguousAveragedRobustErrorInPointCloud().
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar averagedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, const ErrorDetermination errorDetermination, IndexPairs32* correspondences = nullptr);

		/**
		 * Determines the unique robust minimal average square error between two 2D points clouds.
		 * The given point clouds may not have the same size, and therefore the point order does not define any correspondences.<br>
		 * However, the number of given image points must be smaller or equal to the number of given candidate points.<br>
		 * The minimal error is found by determining the absolute minimal error between image points and candidate points.<br>
		 *
		 * Candidate points already used will not be used for any further correspondences creating unique correspondences.<br>
		 * The uniqueness is guaranteed due to a brute force calculation of all possible distances.
		 * @param imagePoints Image points to determine the minimal errors for, must be valid
		 * @param numberImagePoints Number of given image points, with range [1, numberCandidatePoints]
		 * @param validImagePoints The number of image points which can be expected to have a unique corresponding point inside the candidate set, with range [1, numberImagePoints]
		 * @param candidatePoints Possible candidate image points to be used for finding the minimal error, must be valid
		 * @param numberCandidatePoints Number of given candidate points, with range [1, infinity)
		 * @param correspondences Optional resulting point correspondences, for each index of an image point one corresponding candidate point index will be returned
		 * @return Robust averaged minimal square error
		 * @tparam tEstimator Estimator type to be applied
		 * @see approximatedAveragedRobustErrorInPointCloud(), ambiguousAveragedRobustErrorInPointCloud().
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar uniqueAveragedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, IndexPairs32* correspondences = nullptr);

		/**
		 * Determines the approximated robust minimal average square error between two 2D points clouds.
		 * The given point clouds may not have the same size, and therefore the point order does not define any correspondences.<br>
		 * However, the number of given image points must be smaller or equal to the number of given candidate points.<br>
		 * The minimal error is found by determining the approximated absolute minimal error between image points and candidate points.
		 *
		 * Candidate points already used will not be used for any further correspondences creating unique correspondences.<br>
		 * Instead of using a brute force method this function flags already used candidate points to avoid the second usage, however the result may not be the global optimum.
		 * @param imagePoints Image points to determine the minimal errors for, must be valid
		 * @param numberImagePoints Number of given image points, with range [1, numberCandidatePoints]
		 * @param validImagePoints The number of image points which can be expected to have a unique corresponding point inside the candidate set, with range [1, numberImagePoints]
		 * @param candidatePoints Possible candidate image points to be used for finding the minimal error, must be valid
		 * @param numberCandidatePoints Number of given candidate points, with range [1, infinity)
		 * @param correspondences Optional resulting point correspondences, for each index of an image point one corresponding candidate point index will be returned
		 * @return Robust averaged minimal square error
		 * @tparam tEstimator Estimator type to be applied
		 * @see uniqueAveragedRobustErrorInPointCloud(), ambiguousAveragedRobustErrorInPointCloud().
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar approximatedAveragedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, IndexPairs32* correspondences = nullptr);

		/**
		 * Determines the ambiguous robust minimal average square error between two 2D points clouds.
		 * The given point clouds may not have the same size, and therefore the point order does not define any correspondences.<br>
		 * However, the number of given image points must be smaller or equal to the number of given candidate points.<br>
		 * The minimal error is found by determining the absolute minimal error between image points and candidate points.<br>
		 *
		 * Beware: Candidate points already used may be used for any further correspondences also making ambiguous correspondences.<br>
		 * Thus, in a worst case all points may be assigned to one unique candidate point.
		 * @param imagePoints Image points to determine the minimal errors for, must be valid
		 * @param numberImagePoints Number of given image points, with range [1, numberCandidatePoints]
		 * @param validImagePoints The number of image points which can be expected to have a unique corresponding point inside the candidate set, with range [1, numberImagePoints]
		 * @param candidatePoints Possible candidate image points to be used for finding the minimal error, must be valid
		 * @param numberCandidatePoints Number of given candidate points, with range [1, infinity)
		 * @param correspondences Optional resulting point correspondences, for each index of an image point one corresponding candidate point index will be returned
		 * @return Robust averaged minimal square error
		 * @see uniqueAveragedRobustErrorInPointCloud(), approximatedAveragedRobustErrorInPointCloud().
		 * @tparam tEstimator Estimator type to be applied
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar ambiguousAveragedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, IndexPairs32* correspondences = nullptr);

		/**
		 * Returns the averaged robust error for a given set of error values using a defined estimator.
		 * @param sqrErrors Specified squared error values to return the averaged error for
		 * @param number The number of given error values
		 * @param explicitWeights Optional additional weight values individual for each error to be applied to the resulting average robust error only
		 * @return Averaged robust error
		 * @tparam tEstimator Robust estimator to be used for error calculation
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar averagedRobustError(const Scalar* sqrErrors, const size_t number, const Scalar* explicitWeights = nullptr);

		/**
		 * Returns the averaged robust error for a given set of error values using a defined estimator.
		 * @param sqrErrors Specified squared error values to return the averaged error for
		 * @param number The number of given error values
		 * @param estimator Robust estimator to be used for error calculation
		 * @param explicitWeights Optional additional weight values individual for each error to be applied to the resulting average robust error only
		 * @return Averaged robust error
		 */
		static inline Scalar averagedRobustError(const Scalar* sqrErrors, const size_t number, const Estimator::EstimatorType estimator, const Scalar* explicitWeights = nullptr);

		/**
		 * Returns the averaged robust error for a given set of error values using a defined estimator.
		 * Not all error values are used but those defined as indices.
		 * @param sqrErrors Specified squared error values to return the summed error for
		 * @param indices Indices of the given error values to be used for error calculation
		 * @param numberIndices Number of given indices
		 * @param explicitWeights Optional additional weight values individual for each error to be applied to the resulting average robust error only
		 * @return Averaged robust error
		 * @tparam tEstimator Robust estimator to be used for error calculation
		 */
		template <Estimator::EstimatorType tEstimator>
		static Scalar averagedRobustError(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const Scalar* explicitWeights = nullptr);

		/**
		 * Returns the averaged robust error for a given set of error values using a defined estimator.
		 * Not all error values are used but those defined as indices.
		 * @param sqrErrors Specified squared error values to return the summed error for
		 * @param indices Indices of the given error values to be used for error calculation
		 * @param numberIndices Number of given indices
		 * @param estimator Robust estimator to be used for error calculation
		 * @param explicitWeights Optional additional weight values individual for each error to be applied to the resulting average robust error only
		 * @return Averaged robust error
		 */
		static inline Scalar averagedRobustError(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const Estimator::EstimatorType estimator, const Scalar* explicitWeights = nullptr);
};

inline Error::ErrorElement::ErrorElement(const unsigned int imageIndex, const unsigned int candidateIndex, const Scalar error) :
	imageIndex_(imageIndex),
	candidateIndex_(candidateIndex),
	error_(error)
{
	// nothing to do here
}

inline unsigned int Error::ErrorElement::imageIndex() const
{
	return imageIndex_;
}

inline unsigned int Error::ErrorElement::candidateIndex() const
{
	return candidateIndex_;
}

inline Scalar Error::ErrorElement::error() const
{
	return error_;
}

inline bool Error::ErrorElement::operator<(const ErrorElement& element) const
{
	return error_ < element.error_;
}

inline bool Error::posesAlmostEqual(const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const Vector3& maxTranslationOffset)
{
	const Vector3 poseFirstPosition(poseFirst.translation());
	const Vector3 poseSecondPosition(poseSecond.translation());

	return Numeric::abs(poseFirstPosition.x() - poseSecondPosition.x()) <= maxTranslationOffset.x()
			&& Numeric::abs(poseFirstPosition.y() - poseSecondPosition.y()) <= maxTranslationOffset.y()
			&& Numeric::abs(poseFirstPosition.z() - poseSecondPosition.z()) <= maxTranslationOffset.z();
}

inline bool Error::posesAlmostEqual(const HomogenousMatrix4& poseFirst, const HomogenousMatrix4& poseSecond, const Scalar maxOrientationOffset)
{
	const Quaternion poseFirstOrientation(poseFirst.rotation());
	const Quaternion poseSecondOrientation(poseSecond.rotation());

	const Scalar maxOrientationOffsetCos2(Numeric::cos(maxOrientationOffset * Scalar(0.5)));

	return poseFirstOrientation.cos2(poseSecondOrientation) >= maxOrientationOffsetCos2;
}

template <typename TAccessorImagePoints, bool tResultingErrors, bool tResultingSqrErrors>
Scalar Error::determineHomographyError(const SquareMatrix3& points1_H_points0, const TAccessorImagePoints& imagePointAccessor0, const TAccessorImagePoints& imagePointAccessor1, Vector2* errors, Scalar* sqrErrors)
{
	ocean_assert(imagePointAccessor0.size() == imagePointAccessor1.size());

	if (imagePointAccessor0.isEmpty())
	{
		return Scalar(0);
	}

	ocean_assert((tResultingErrors && errors != nullptr) || (!tResultingErrors && errors == nullptr));
	ocean_assert((tResultingSqrErrors && sqrErrors != nullptr) || (!tResultingSqrErrors && sqrErrors == nullptr));

	ocean_assert(!points1_H_points0.isSingular());

	Scalar sqrAveragePixelError = 0;

	for (size_t n = 0; n < imagePointAccessor0.size(); ++n)
	{
		Vector2 transformedPoint;

		if (points1_H_points0.multiply(imagePointAccessor0[n], transformedPoint))
		{
			const Vector2& measuredPoint = imagePointAccessor1[n];

			const Vector2 difference(transformedPoint - measuredPoint);
			const Scalar sqrPixelError = difference.sqr();

			sqrAveragePixelError += sqrPixelError;

			if constexpr (tResultingErrors)
			{
				errors[n] = difference;
			}

			if constexpr (tResultingSqrErrors)
			{
				sqrErrors[n] = sqrPixelError;
			}
		}
		else
		{
			if constexpr (tResultingErrors)
			{
				errors[n] = Vector2(Numeric::maxValue(), Numeric::maxValue());
			}

			if constexpr (tResultingSqrErrors)
			{
				sqrErrors[n] = Numeric::maxValue();
			}

			sqrAveragePixelError = Numeric::maxValue();
		}
	}

	ocean_assert(imagePointAccessor0.size() != 0);
	ocean_assert(sqrAveragePixelError >= 0 && sqrAveragePixelError <= Numeric::maxValue());

	return sqrAveragePixelError / Scalar(imagePointAccessor0.size());
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside, bool tResultingErrors, bool tResultingSqrErrors>
inline Scalar Error::determinePoseError(const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, const Scalar zoom, Vector2* errors, Scalar* sqrErrors)
{
	return determinePoseErrorIF<TAccessorObjectPoints, TAccessorImagePoints, tUseBorderDistortionIfOutside, tResultingErrors, tResultingSqrErrors>(PinholeCamera::standard2InvertedFlipped(world_T_camera), pinholeCamera, objectPointAccessor, imagePointAccessor, useDistortionParameters, zoom, errors, sqrErrors);
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tResultingErrors, bool tResultingSqrErrors>
inline Scalar Error::determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& anyCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Vector2* errors, Scalar* sqrErrors)
{
	return determinePoseErrorIF<TAccessorObjectPoints, TAccessorImagePoints, tResultingErrors, tResultingSqrErrors>(PinholeCamera::standard2InvertedFlipped(world_T_camera), anyCamera, objectPointAccessor, imagePointAccessor, errors, sqrErrors);
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tOnlyFrontObjectPoints>
inline bool Error::determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError)
{
	return determinePoseErrorIF<TAccessorObjectPoints, TAccessorImagePoints, tOnlyFrontObjectPoints>(AnyCamera::standard2InvertedFlipped(world_T_camera), camera, objectPointAccessor, imagePointAccessor, sqrAveragePixelError, sqrMinimalPixelError,sqrMaximalPixelError);
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints>
inline void Error::determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError)
{
	determinePoseErrorIF<TAccessorObjectPoints, TAccessorImagePoints>(AnyCamera::standard2InvertedFlipped(world_T_camera), camera, objectPointAccessor, imagePointAccessor, sqrAveragePixelError, sqrMinimalPixelError,sqrMaximalPixelError);
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside>
inline void Error::determinePoseError(const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError, const Scalar zoom)
{
	determinePoseErrorIF<TAccessorObjectPoints, TAccessorImagePoints, tUseBorderDistortionIfOutside>(PinholeCamera::standard2InvertedFlipped(world_T_camera), pinholeCamera, objectPointAccessor, imagePointAccessor, useDistortionParameters, sqrAveragePixelError, sqrMinimalPixelError,sqrMaximalPixelError, zoom);
}

inline Vector2 Error::determinePoseError(const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const Vector3& objectPoint, const Vector2& imagePoint)
{
	return determinePoseErrorIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), camera, objectPoint, imagePoint);
}

inline Vector2 Error::determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const Vector3& objectPoint, const Vector2& imagePoint)
{
	return camera.projectToImageIF(flippedCamera_T_world, objectPoint) - imagePoint;
}

inline Vector2 Error::determinePoseError(const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const Vector3& objectPoint, const Vector2& imagePoint, const bool useDistortionParameters)
{
	return determinePoseErrorIF(PinholeCamera::standard2InvertedFlipped(world_T_camera), pinholeCamera, objectPoint, imagePoint, useDistortionParameters);
}

inline Vector2 Error::determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vector3& objectPoint, const Vector2& imagePoint, const bool useDistortionParameters)
{
	return pinholeCamera.projectToImageIF<true>(flippedCamera_T_world, objectPoint, useDistortionParameters) - imagePoint;
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside, bool tResultingErrors, bool tResultingSqrErrors>
Scalar Error::determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, const Scalar zoom, Vector2* errors, Scalar* sqrErrors)
{
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(objectPointAccessor.size() == imagePointAccessor.size());
	ocean_assert((tResultingErrors && errors != nullptr) || (!tResultingErrors && errors == nullptr));
	ocean_assert((tResultingSqrErrors && sqrErrors != nullptr) || (!tResultingSqrErrors && sqrErrors == nullptr));
	ocean_assert(zoom > Numeric::eps());

	if (objectPointAccessor.isEmpty())
	{
		return 0;
	}

	Scalar sqrAveragePixelError = 0;

	if (useDistortionParameters && pinholeCamera.hasDistortionParameters())
	{
		for (size_t n = 0; n < objectPointAccessor.size(); ++n)
		{
			const Vector2 imagePoint(pinholeCamera.projectToImageIF<true, tUseBorderDistortionIfOutside>(flippedCamera_T_world, objectPointAccessor[n], zoom));
			const Vector2& measuredImagePoint = imagePointAccessor[n];

			const Vector2 difference(imagePoint - measuredImagePoint);
			const Scalar sqrPixelError = difference.sqr();

			sqrAveragePixelError += sqrPixelError;

			if constexpr (tResultingErrors)
			{
				errors[n] = difference;
			}

			if constexpr (tResultingSqrErrors)
			{
				sqrErrors[n] = sqrPixelError;
			}
		}
	}
	else
	{
		// create one transformation matrix covering the entire pipeline (transformation and then projection)
		const HomogenousMatrix4 transformationMatrixIF(pinholeCamera.transformationMatrixIF(flippedCamera_T_world, zoom));

#ifdef OCEAN_DEBUG
		SquareMatrix4 debugCameraMatrix(pinholeCamera.intrinsic());
		debugCameraMatrix(0, 0) *= zoom;
		debugCameraMatrix(1, 1) *= zoom;
		debugCameraMatrix[15] = 1;

		const SquareMatrix4 debugEntireMatrix(debugCameraMatrix * (SquareMatrix4&)flippedCamera_T_world);
		const HomogenousMatrix4& debugTransformationMatrixIF = (const HomogenousMatrix4&)debugEntireMatrix;

		ocean_assert(transformationMatrixIF == debugTransformationMatrixIF);
#endif

		for (size_t n = 0; n < objectPointAccessor.size(); ++n)
		{
			const Vector3 transformedObjectPoint(transformationMatrixIF * objectPointAccessor[n]);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar factor = Scalar(1) / transformedObjectPoint.z();

			const Vector2 imagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);

			const Vector2 difference(imagePoint - imagePointAccessor[n]);
			const Scalar sqrPixelError = difference.sqr();

			sqrAveragePixelError += sqrPixelError;

			if constexpr (tResultingErrors)
			{
				errors[n] = difference;
			}

			if constexpr (tResultingSqrErrors)
			{
				sqrErrors[n] = sqrPixelError;
			}
		}
	}

	ocean_assert(objectPointAccessor.size() != 0);
	return sqrAveragePixelError / Scalar(objectPointAccessor.size());
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tResultingErrors, bool tResultingSqrErrors>
Scalar Error::determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Vector2* errors, Scalar* sqrErrors)
{
	ocean_assert(flippedCamera_T_world.isValid() && anyCamera.isValid());
	ocean_assert(objectPointAccessor.size() == imagePointAccessor.size());

	ocean_assert((tResultingErrors && errors != nullptr) || (!tResultingErrors && errors == nullptr));
	ocean_assert((tResultingSqrErrors && sqrErrors != nullptr) || (!tResultingSqrErrors && sqrErrors == nullptr));

	if (objectPointAccessor.isEmpty())
	{
		return 0;
	}

	Scalar sqrAveragePixelError = 0;

	for (size_t n = 0; n < objectPointAccessor.size(); ++n)
	{
		const Vector2 imagePoint = anyCamera.projectToImageIF(flippedCamera_T_world, objectPointAccessor[n]);

		const Vector2& measuredImagePoint = imagePointAccessor[n];

		const Vector2 difference(imagePoint - measuredImagePoint);
		const Scalar sqrPixelError = difference.sqr();

		sqrAveragePixelError += sqrPixelError;

		if constexpr (tResultingErrors)
		{
			errors[n] = difference;
		}

		if constexpr (tResultingSqrErrors)
		{
			sqrErrors[n] = sqrPixelError;
		}
	}

	ocean_assert(objectPointAccessor.size() != 0);
	return sqrAveragePixelError / Scalar(objectPointAccessor.size());
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tOnlyFrontObjectPoints>
bool Error::determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError)
{
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(objectPointAccessor.size() == imagePointAccessor.size());

	sqrAveragePixelError = 0;
	sqrMinimalPixelError = Numeric::maxValue();
	sqrMaximalPixelError = 0;

	if (objectPointAccessor.isEmpty())
	{
		return true;
	}

	for (size_t n = 0; n < objectPointAccessor.size(); ++n)
	{
		const Vector3& objectPoint = objectPointAccessor[n];

		if constexpr (tOnlyFrontObjectPoints)
		{
			if (!AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint))
			{
				return false;
			}
		}

		const Vector2 projectedImagePoint(camera.projectToImageIF(flippedCamera_T_world, objectPoint));
		const Vector2& imagePoint = imagePointAccessor[n];

		const Scalar pixelError = projectedImagePoint.sqrDistance(imagePoint);

		sqrAveragePixelError += pixelError;

		if (pixelError < sqrMinimalPixelError)
		{
			sqrMinimalPixelError = pixelError;
		}

		if (pixelError > sqrMaximalPixelError)
		{
			sqrMaximalPixelError = pixelError;
		}
	}

	ocean_assert(objectPointAccessor.size() != 0);
	sqrAveragePixelError /= Scalar(objectPointAccessor.size());

	return true;
}


template <typename TAccessorObjectPoints, typename TAccessorImagePoints>
void Error::determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError)
{
	constexpr bool tOnlyFrontObjectPoints = false;

	const bool result = determinePoseErrorIF<TAccessorObjectPoints, TAccessorImagePoints, tOnlyFrontObjectPoints>(flippedCamera_T_world, camera, objectPointAccessor, imagePointAccessor, sqrAveragePixelError, sqrMinimalPixelError, sqrMaximalPixelError);
	ocean_assert_and_suppress_unused(result, result);
}

template <typename TAccessorObjectPoints, typename TAccessorImagePoints, bool tUseBorderDistortionIfOutside>
void Error::determinePoseErrorIF(const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const TAccessorObjectPoints& objectPointAccessor, const TAccessorImagePoints& imagePointAccessor, const bool useDistortionParameters, Scalar& sqrAveragePixelError, Scalar& sqrMinimalPixelError, Scalar& sqrMaximalPixelError, const Scalar zoom)
{
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(objectPointAccessor.size() == imagePointAccessor.size());
	ocean_assert(zoom > Numeric::eps());

	sqrAveragePixelError = 0;
	sqrMinimalPixelError = Numeric::maxValue();
	sqrMaximalPixelError = 0;

	if (objectPointAccessor.isEmpty())
	{
		return;
	}

	if (useDistortionParameters && pinholeCamera.hasDistortionParameters())
	{
		for (size_t n = 0; n < objectPointAccessor.size(); ++n)
		{
			const Vector2 imagePoint(pinholeCamera.projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, objectPointAccessor[n], useDistortionParameters, zoom));
			const Vector2& realImagePoint = imagePointAccessor[n];

			const Scalar pixelError = imagePoint.sqrDistance(realImagePoint);

			sqrAveragePixelError += pixelError;

			if (pixelError < sqrMinimalPixelError)
			{
				sqrMinimalPixelError = pixelError;
			}

			if (pixelError > sqrMaximalPixelError)
			{
				sqrMaximalPixelError = pixelError;
			}
		}
	}
	else
	{
		// create one transformation matrix covering the entire pipeline (transformation and then projection)
		const HomogenousMatrix4 transformationMatrixIF(pinholeCamera.transformationMatrixIF(flippedCamera_T_world, zoom));

#ifdef OCEAN_DEBUG
		SquareMatrix4 debugCameraMatrix(pinholeCamera.intrinsic());
		debugCameraMatrix(0, 0) *= zoom;
		debugCameraMatrix(1, 1) *= zoom;
		debugCameraMatrix[15] = 1;

		const SquareMatrix4 debugEntireMatrix(debugCameraMatrix * (SquareMatrix4&)flippedCamera_T_world);
		const HomogenousMatrix4& debugTransformationMatrixIF = (const HomogenousMatrix4&)debugEntireMatrix;

		ocean_assert(transformationMatrixIF == debugTransformationMatrixIF);
#endif

		for (size_t n = 0; n < objectPointAccessor.size(); ++n)
		{
			const Vector3 transformedObjectPoint(transformationMatrixIF * objectPointAccessor[n]);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar factor = Scalar(1) / transformedObjectPoint.z();

			const Vector2 imagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);

			const Scalar pixelError = imagePoint.sqrDistance(imagePointAccessor[n]);

			sqrAveragePixelError += pixelError;

			if (pixelError < sqrMinimalPixelError)
			{
				sqrMinimalPixelError = pixelError;
			}

			if (pixelError > sqrMaximalPixelError)
			{
				sqrMaximalPixelError = pixelError;
			}
		}
	}

	ocean_assert(objectPointAccessor.size() != 0);
	sqrAveragePixelError /= Scalar(objectPointAccessor.size());
}

template <Estimator::EstimatorType tEstimator>
Scalar Error::averagedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, const ErrorDetermination errorDetermination, IndexPairs32* correspondences)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(numberImagePoints <= numberCandidatePoints);
	ocean_assert(validImagePoints <= numberImagePoints);

	switch (errorDetermination)
	{
		case ED_UNIQUE:
			return uniqueAveragedRobustErrorInPointCloud<tEstimator>(imagePoints, numberImagePoints, validImagePoints, candidatePoints, numberCandidatePoints, correspondences);

		case ED_APPROXIMATED:
			return approximatedAveragedRobustErrorInPointCloud<tEstimator>(imagePoints, numberImagePoints, validImagePoints, candidatePoints, numberCandidatePoints, correspondences);

		case ED_AMBIGUOUS:
			return ambiguousAveragedRobustErrorInPointCloud<tEstimator>(imagePoints, numberImagePoints, validImagePoints, candidatePoints, numberCandidatePoints, correspondences);

		default:
			break;
	}

	ocean_assert(false && "Invalid errorDetermination parameter!");
	return Numeric::maxValue();
}

template <Estimator::EstimatorType tEstimator>
Scalar Error::uniqueAveragedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, IndexPairs32* correspondences)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(numberImagePoints <= numberCandidatePoints);
	ocean_assert(validImagePoints <= numberImagePoints);

	if (numberImagePoints > numberCandidatePoints)
		return Numeric::maxValue();

	ErrorElements errorElements;
	errorElements.reserve(numberImagePoints * numberCandidatePoints);

	for (unsigned int i = 0; i < numberImagePoints; ++i)
	{
		const Vector2& imagePoint(imagePoints[i]);

		for (unsigned int c = 0; c < numberCandidatePoints; ++c)
		{
			errorElements.push_back(ErrorElement(i, c, imagePoint.sqrDistance(candidatePoints[c])));
		}
	}

	std::sort(errorElements.begin(), errorElements.end());

	std::vector<unsigned char> usedImagePoints(numberImagePoints, 0u);
	std::vector<unsigned char> usedCandidatePoints(numberCandidatePoints, 0u);

	if constexpr (Estimator::isStandardEstimator<tEstimator>())
	{
		size_t numberUsedErrors = 0;
		Scalar sqrErrors = 0;

		ErrorElements::const_iterator i = errorElements.begin();
		while (i != errorElements.end() && numberUsedErrors < validImagePoints)
		{
			ocean_assert(i != errorElements.end());

			if (usedImagePoints[i->imageIndex()] == 0u && usedCandidatePoints[i->candidateIndex()] == 0u)
			{
				sqrErrors += i->error();
				numberUsedErrors++;

				usedImagePoints[i->imageIndex()] = 1u;
				usedCandidatePoints[i->candidateIndex()] = 1u;

				if (correspondences)
				{
					correspondences->push_back(IndexPair32(i->imageIndex(), i->candidateIndex()));
				}
			}

			++i;
		}

		if (numberUsedErrors == 0)
		{
			return 0;
		}

		return sqrErrors / Scalar(numberUsedErrors);
	}
	else
	{
		Scalars sqrErrors;
		sqrErrors.reserve(validImagePoints);

		ErrorElements::const_iterator i = errorElements.begin();
		while (i != errorElements.end() && sqrErrors.size() < validImagePoints)
		{
			ocean_assert(i != errorElements.end());

			if (usedImagePoints[i->imageIndex()] == 0u && usedCandidatePoints[i->candidateIndex()] == 0u)
			{
				sqrErrors.push_back(i->error());

				usedImagePoints[i->imageIndex()] = 1u;
				usedCandidatePoints[i->candidateIndex()] = 1u;

				if (correspondences)
				{
					correspondences->push_back(IndexPair32(i->imageIndex(), i->candidateIndex()));
				}
			}

			++i;
		}

		if (sqrErrors.empty())
		{
			return 0;
		}

		const size_t numberUsedErrors = min(validImagePoints, sqrErrors.size());

		return averagedRobustError<tEstimator>(sqrErrors.data(), numberUsedErrors);
	}
}

template <Estimator::EstimatorType tEstimator>
Scalar Error::approximatedAveragedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, IndexPairs32* correspondences)
{
	if (validImagePoints == 0)
		return 0;

	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(numberImagePoints <= numberCandidatePoints);
	ocean_assert(validImagePoints <= numberImagePoints);

	if (numberImagePoints > numberCandidatePoints)
	{
		return Numeric::maxValue();
	}

	ErrorElements errorElements;
	errorElements.reserve(numberImagePoints);

	std::vector<unsigned char> usedCandidates(numberCandidatePoints, 0u);

	for (unsigned int i = 0u; i < numberImagePoints; ++i)
	{
		const Vector2& imagePoint(imagePoints[i]);

		Scalar minValue = Numeric::maxValue();
		unsigned int minIndex = 0xFFFFFFFF;

		for (unsigned int c = 0u; c < numberCandidatePoints; ++c)
		{
			if (usedCandidates[c] == 0u)
			{
				const Scalar value = imagePoint.sqrDistance(candidatePoints[c]);

				if (value < minValue)
				{
					minValue = value;
					minIndex = c;
				}
			}
		}

		ocean_assert(minIndex != 0xFFFFFFFF);

		errorElements.push_back(ErrorElement(i, minIndex, minValue));

		ocean_assert(usedCandidates[minIndex] == 0u);
		usedCandidates[minIndex] = 1u;
	}

	std::sort(errorElements.begin(), errorElements.end());

	ocean_assert(!errorElements.empty());

	if (errorElements.empty())
	{
		return 0;
	}

	const size_t numberUsedErrors = min(validImagePoints, errorElements.size());

	if constexpr (Estimator::isStandardEstimator<tEstimator>())
	{
		Scalar sqrErrors = 0;

		if (correspondences)
		{
			for (size_t n = 0; n < numberUsedErrors; ++n)
			{
				sqrErrors += errorElements[n].error();
				correspondences->push_back(IndexPair32(errorElements[n].imageIndex(), errorElements[n].candidateIndex()));
			}
		}
		else
		{
			for (size_t n = 0; n < numberUsedErrors; ++n)
			{
				sqrErrors += errorElements[n].error();
			}
		}

		return sqrErrors / Scalar(numberUsedErrors);
	}
	else
	{
		Scalars sqrErrors;
		sqrErrors.reserve(numberUsedErrors);

		for (unsigned int n = 0; n < numberUsedErrors; ++n)
		{
			sqrErrors.push_back(errorElements[n].error());

			if (correspondences)
			{
				correspondences->push_back(IndexPair32(errorElements[n].imageIndex(), errorElements[n].candidateIndex()));
			}
		}

		return averagedRobustError<tEstimator>(sqrErrors.data(), numberUsedErrors);
	}
}

template <Estimator::EstimatorType tEstimator>
Scalar Error::ambiguousAveragedRobustErrorInPointCloud(const Vector2* imagePoints, const size_t numberImagePoints, const size_t validImagePoints, const Vector2* candidatePoints, const size_t numberCandidatePoints, IndexPairs32* correspondences)
{
	ocean_assert(imagePoints && candidatePoints);
	ocean_assert(numberImagePoints <= numberCandidatePoints);
	ocean_assert(validImagePoints <= numberImagePoints);

	if (numberImagePoints > numberCandidatePoints)
	{
		return Numeric::maxValue();
	}

	if (numberImagePoints == 0)
	{
		return 0;
	}

	ErrorElements errorElements;
	errorElements.reserve(numberImagePoints);

	for (unsigned int i = 0; i < numberImagePoints; ++i)
	{
		const Vector2& imagePoint(imagePoints[i]);

		Scalar minValue = Numeric::maxValue();
		unsigned int minIndex = 0xFFFFFFFF;

		for (unsigned int c = 0; c < numberCandidatePoints; ++c)
		{
			const Scalar value = imagePoint.sqrDistance(candidatePoints[c]);

			if (value < minValue)
			{
				minValue = value;
				minIndex = c;
			}
		}

		ocean_assert(minIndex != 0xFFFFFFFF);

		errorElements.push_back(ErrorElement(i, minIndex, minValue));
	}

	std::sort(errorElements.begin(), errorElements.end());

	ocean_assert(!errorElements.empty());

	if (errorElements.empty())
	{
		return 0;
	}

	const size_t numberUsedErrors = min(validImagePoints, errorElements.size());

	if constexpr (!Estimator::isStandardEstimator<tEstimator>())
	{
		Scalar sqrErrors = 0;

		if (correspondences)
		{
			for (unsigned int n = 0; n < numberUsedErrors; ++n)
			{
				sqrErrors += errorElements[n].error();
				correspondences->push_back(IndexPair32(errorElements[n].imageIndex(), errorElements[n].candidateIndex()));
			}
		}
		else
		{
			for (unsigned int n = 0; n < numberUsedErrors; ++n)
			{
				sqrErrors += errorElements[n].error();
			}
		}

		return sqrErrors / Scalar(numberUsedErrors);
	}
	else
	{
		Scalars sqrErrors;
		sqrErrors.reserve(numberUsedErrors);

		if (correspondences)
		{
			for (unsigned int n = 0; n < numberUsedErrors; ++n)
			{
				sqrErrors.push_back(errorElements[n].error());
				correspondences->push_back(IndexPair32(errorElements[n].imageIndex(), errorElements[n].candidateIndex()));
			}
		}
		else
		{
			for (unsigned int n = 0; n < numberUsedErrors; ++n)
			{
				sqrErrors.push_back(errorElements[n].error());
			}
		}

		return Geometry::Error::averagedRobustError<tEstimator>(sqrErrors.data(), numberUsedErrors);
	}
}

template <Estimator::EstimatorType tEstimator>
Scalar Error::averagedRobustError(const Scalar* sqrErrors, const size_t number, const Scalar* explicitWeights)
{
	ocean_assert(sqrErrors);
	ocean_assert(number > 0);

	if (number == 0)
	{
		return 0;
	}

	// the number of model parameters is guessed to be 6
	const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(sqrErrors, number, 6)) : 0;

	Scalar summedError = 0;

	if (explicitWeights)
	{
		const Scalar* const sqrErrorsEnd = sqrErrors + number;
		while (sqrErrors != sqrErrorsEnd)
		{
			summedError += Estimator::robustErrorSquare<tEstimator>(*sqrErrors++, sqrSigma) * *explicitWeights++;
		}
	}
	else
	{
		const Scalar* const sqrErrorsEnd = sqrErrors + number;
		while (sqrErrors != sqrErrorsEnd)
		{
			summedError += Estimator::robustErrorSquare<tEstimator>(*sqrErrors++, sqrSigma);
		}
	}

	return summedError / Scalar(number);
}

inline Scalar Error::averagedRobustError(const Scalar* sqrErrors, const size_t number, const Estimator::EstimatorType estimator, const Scalar* explicitWeights)
{
	switch (estimator)
	{
		case Estimator::ET_SQUARE:
			return averagedRobustError<Estimator::ET_SQUARE>(sqrErrors, number, explicitWeights);

		case Estimator::ET_LINEAR:
			return averagedRobustError<Estimator::ET_LINEAR>(sqrErrors, number, explicitWeights);

		case Estimator::ET_HUBER:
			return averagedRobustError<Estimator::ET_HUBER>(sqrErrors, number, explicitWeights);

		case Estimator::ET_CAUCHY:
			return averagedRobustError<Estimator::ET_CAUCHY>(sqrErrors, number, explicitWeights);

		case Estimator::ET_TUKEY:
			return averagedRobustError<Estimator::ET_TUKEY>(sqrErrors, number, explicitWeights);

		default:
			break;
	}

	ocean_assert(false && "Invalid estimator type!");
	return averagedRobustError<Estimator::ET_SQUARE>(sqrErrors, number, explicitWeights);
}

template <Estimator::EstimatorType tEstimator>
Scalar Error::averagedRobustError(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const Scalar* explicitWeights)
{
	ocean_assert(sqrErrors);
	ocean_assert(numberIndices > 0);

	if (numberIndices == 0)
	{
		return 0;
	}

	// the number of model parameters is guessed to be 6
	const Scalar sqrSigma = Estimator::needSigma<tEstimator>() ? Numeric::sqr(Estimator::determineSigmaSquare<tEstimator>(sqrErrors, indices, numberIndices, 6)) : 0;

	Scalar summedError = 0;

	if (explicitWeights)
	{
		const Scalar* const sqrErrorsEnd = sqrErrors + numberIndices;
		while (sqrErrors != sqrErrorsEnd)
		{
			const unsigned int index = *indices++;
			summedError += Estimator::robustErrorSquare<tEstimator>(sqrErrors[index], sqrSigma) * explicitWeights[index];
		}
	}
	else
	{
		const Scalar* const sqrErrorsEnd = sqrErrors + numberIndices;
		while (sqrErrors != sqrErrorsEnd)
		{
			summedError += Estimator::robustErrorSquare<tEstimator>(sqrErrors[*indices++], sqrSigma);
		}
	}

	return summedError / Scalar(numberIndices);
}

inline Scalar Error::averagedRobustError(const Scalar* sqrErrors, const unsigned int* indices, const size_t numberIndices, const Estimator::EstimatorType estimator, const Scalar* explicitWeights)
{
	switch (estimator)
	{
		case Estimator::ET_SQUARE:
			return averagedRobustError<Estimator::ET_SQUARE>(sqrErrors, indices, numberIndices, explicitWeights);

		case Estimator::ET_LINEAR:
			return averagedRobustError<Estimator::ET_LINEAR>(sqrErrors, indices, numberIndices, explicitWeights);

		case Estimator::ET_HUBER:
			return averagedRobustError<Estimator::ET_HUBER>(sqrErrors, indices, numberIndices, explicitWeights);

		case Estimator::ET_CAUCHY:
			return averagedRobustError<Estimator::ET_CAUCHY>(sqrErrors, indices, numberIndices, explicitWeights);

		case Estimator::ET_TUKEY:
			return averagedRobustError<Estimator::ET_TUKEY>(sqrErrors, indices, numberIndices, explicitWeights);

		default:
			break;
	}

	ocean_assert(false && "Invalid estimator type!");
	return averagedRobustError<Estimator::ET_SQUARE>(sqrErrors, indices, numberIndices, explicitWeights);
}

}

}

#endif // META_OCEAN_GEOMETRY_ERROR_H
