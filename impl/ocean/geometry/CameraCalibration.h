/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_CAMERA_CALIBRATION_H
#define META_OCEAN_GEOMETRY_CAMERA_CALIBRATION_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"

#include <vector>

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements functions to calibrate a camera, to determine the profile of a camera.
 * Use CameraCalibration::determineCameraCalibrationPlanar() for groups of 2D/3D point correspondences with all 3D object points located on one plane.<br>
 * Use CameraCalibration::determineCameraCalibration() for groups of 2D/3D point correspondences with any kind of 3D object points.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT CameraCalibration
{
	public:

		/**
		 * Definition of a class holding the information about one calibration pattern.
		 */
		class OCEAN_GEOMETRY_EXPORT Pattern
		{
			public:

				/**
				 * Definition of a (row) vector holding 2D positions.
				 */
				typedef Vectors2 PatternRow;

				/**
				 * Definition of a vector holding rows.
				 */
				typedef std::vector<PatternRow> PatternRows;

			public:

				/**
				 * Creates an empty pattern object.
				 */
				Pattern();

				/**
				 * Creates a new pattern object.
				 * @param rows Pattern rows defining the pattern
				 * @param timestamp Timestamp of the pattern
				 */
				Pattern(const PatternRows& rows, const Timestamp timestamp);

				/**
				 * Returns the rows of this pattern.
				 * @return Pattern rows
				 */
				inline const PatternRows& rows() const;

				/**
				 * Returns the four corners of this pattern.
				 * @return Four corners
				 */
				inline const Vector2* corners() const;

				/**
				 * Returns the bounding box of this pattern
				 * @return 2D bounding box
				 */
				Box2 boundingBox() const;

				/**
				 * Returns the timestmap of this pattern.
				 * @return Pattern timestamp
				 */
				inline Timestamp timestamp() const;

				/**
				 * Returns whether the pattern object holds not valid corners.
				 * @return True, if so
				 */
				inline bool isEmpty() const;

				/**
				 * Returns the minimal sum of square distances between the corners of two calibration patterns.
				 * @return Sum of square distances
				 */
				Scalar distance(const Pattern& pattern) const;

				/**
				 * Applies a size factor to resize the entire pattern.
				 * All corner positions will be changed by the given factor.
				 * @param sizeFactor Size factor to be applied, with range (0, infinity)
				 */
				void changeSize(const Scalar sizeFactor);

			private:

				/**
				 * Compares the first elements of two pairs.
				 * @param first First pair to compare
				 * @param second Second pair to compare
				 * @return True, if the first element of the first pair is lesser than the first element of the second pair
				 */
				template <typename TF, typename TS> static inline bool compareFirst(const std::pair<TF, TS>& first, const std::pair<TF, TS>& second);

			private:

				/// Pattern rows.
				PatternRows patternRows;

				/// Pattern corners.
				Vector2 patternCorners[4];

				/// Pattern timestamp.
				Timestamp patternTimestamp;
		};

		/**
		 * Definition of a vector holding calibration patterns.
		 */
		typedef std::vector<Pattern> Patterns;

	public:

		/**
		 * Determines the camera calibration for several individual groups of 3D object points all lying on the same 3D plane and corresponding 2D image points.
		 * The camera calibration is determines by usage of all given correspondences.<br>
		 * The individual groups of image points can e.g., be the result of individual images of a calibration pattern observed from different viewing positions.<br>
		 * The camera profile must not change between individual groups (images of the calibration pattern).<br>
		 * Each group must provide at least 10 points.
		 * @param width The width of the camera frame in pixel, with range [1, infinity)
		 * @param height The height of the camera frame in pixel, with range [1, infinity)
		 * @param objectPointGroups The groups of object points (all points lie in the Z == 0 plane), each group has a corresponding group of image points, at least 3
		 * @param imagePointGroups The groups of image points, each group has a corresponding group of object points
		 * @param pinholeCamera Resulting pinhole camera profile holding all extracted calibration information like e.g. intrinsic camera and distortion parameters
		 * @param iterations Number of iterations the camera parameters will be improved after the initial model has been determined, using a non linear optimization approach, with range [0, infinity)
		 * @param sqrAccuracy Optional resulting average square pixel error for all point correspondences
		 * @return True, if succeeded
		 * @see determineCameraCalibration().
		 */
		static bool determineCameraCalibrationPlanar(const unsigned int width, const unsigned int height, const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, PinholeCamera& pinholeCamera, const unsigned int iterations = 20u, Scalar* sqrAccuracy = nullptr);

		/**
		 * Determines the intrinsic camera matrix for several groups of corresponding 2D/3D points.
		 * All 3D object points must be located on a plane.
		 * Each group must provide at least 10 points.
		 * @param objectPointGroups Groups of object points, each group corresponds to a group of image points, at least three
		 * @param imagePointGroups Groups of image points, each group corresponds to a group of object points, at least three
		 * @param intrinsic Resulting intrinsic camera matrix
		 * @param homographies Optional resulting homographies, one for each given group for which a valid homography could be determined, may be less than the provided number of groups
		 * @param validGroupIndices Optional resulting indices of the valid groups for which a valid homography could be determined
		 * @return True, if succeeded
		 */
		static bool determineIntrinsicCameraMatrixPlanar(const ConstIndexedAccessor<Vectors3>& objectPointGroups, const ConstIndexedAccessor<Vectors2>& imagePointGroups, SquareMatrix3& intrinsic, SquareMatrices3* homographies = nullptr, Indices32* validGroupIndices = nullptr);

		/**
		 * Determines the camera calibration while a rough camera calibration is already known.
		 * The camera calibration is improved by usage of all given groups of point correspondences.<br>
		 * The individual groups of image points can be arbitrary and do not need to be based on a planar calibration pattern.<br>
		 * The camera profile must not change between individual groups (images of the calibration pattern).
		 * @param roughCamera Rough camera object already determined, must be valid
		 * @param objectPointGroups Groups of object points, each group corresponds to a group of image points, at least one
		 * @param imagePointGroups Groups of image points, each group corresponds to a group of object points, at least one
		 * @param pinholeCamera Resulting pinhole camera holding all extracted calibration information like e.g. intrinsic camera and distortion parameters
		 * @param sqrAccuracy Optional resulting average square pixel error, if defined
		 * @return True, if succeeded
		 * @see determineCameraCalibrationPlanar().
		 */
		static bool determineCameraCalibration(const PinholeCamera& roughCamera, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, PinholeCamera& pinholeCamera, Scalar* sqrAccuracy = nullptr);

		/**
		 * Creates point correspondences from a given calibration pattern.
		 * @param pattern Calibration patter to create the correspondences from
		 * @param boxSize Size of one calibration box in m
		 * @param objectPoints Resulting object points
		 * @param imagePoints Resulting image points
		 * @return True, if succeeded
		 */
		static bool createCorrespondences(const Pattern& pattern, const Vector2& boxSize, ObjectPoints& objectPoints, ImagePoints& imagePoints);

		/**
		 * Determines the horizontal field of view that matches best to a set of poses, object point and image points.
		 * @param width The width of the camera in pixel, with range [1, infinity)
		 * @param height The height of the camera in pixel, with range [1, infinity)
		 * @param posesAccessor The accessor for the poses that match to the number of given object point groups (and image point groups)
		 * @param objectPointGroupAccessor The accessor for the individual groups of object points, one group for each group of image points
		 * @param imagePointGroupAccessor The accessor for the individual groups of image points, one group for each group of object points with same nubmer of elements
		 * @param twoIterations True, to apply a two-iteration approach, otherwise a faster one-iteration approach will be applied
		 * @param idealFovX Resulting best matching horizontal field of view, in radian
		 * @param lowestFovX Lowest field of view that will be tested in radian, with range (0, highestFovX)
		 * @param highestFovX Highest field of view that will be tested in radian, with range (lowestFovX, PI)
		 * @param steps Number of angles that will be tested, with range [1, infinity)
		 * @param idealPoses Optional resulting poses corresponding to the given sets of object and image points and the found fovX
		 * @return True, if succeeded
		 */
		static bool determineBestMatchingFovX(const unsigned int width, const unsigned int height, const ConstIndexedAccessor<HomogenousMatrix4>& posesAccessor, const ConstIndexedAccessor<Vectors3>& objectPointGroupAccessor, const ConstIndexedAccessor<Vectors2>& imagePointGroupAccessor, Scalar& idealFovX, const bool twoIterations = true, const Scalar lowestFovX = Numeric::deg2rad(25), const Scalar highestFovX = Numeric::deg2rad(75), const unsigned int steps = 20u, NonconstIndexedAccessor<HomogenousMatrix4>* idealPoses = nullptr);

		/**
		 * Applies one camera and one pose optimization successively for a given set of poses and corresponding image and object points.
		 * @param pinholeCamera The initial pinhole camera object that has to be optimized according to the projection error of the given point sets
		 * @param poses A set of camera poses, each pose corresponds to a group of image and object points
		 * @param objectPointGroups Groups of object points, each group corresponds to one camera pose and the group of image points
		 * @param imagePointGroups Groups of image points, each group corresponds to one camera pose and the group of object points
		 * @param optimizedCamera Resulting optimized camera
		 * @param optimizedPoses Resulting optimized poses
		 * @param initialSqrError Optional initial average sqr projection error
		 * @param finalSqrError Optional resulting final average sqr projection error
		 * @return True, if succeeded
		 */
		static bool successiveCameraPoseOptimization(const PinholeCamera& pinholeCamera, const HomogenousMatrices4& poses, const ObjectPointGroups& objectPointGroups, const ImagePointGroups& imagePointGroups, PinholeCamera& optimizedCamera, HomogenousMatrices4& optimizedPoses, Scalar* initialSqrError = nullptr, Scalar* finalSqrError = nullptr);
};

inline const CameraCalibration::Pattern::PatternRows& CameraCalibration::Pattern::rows() const
{
	return patternRows;
}

inline const Vector2* CameraCalibration::Pattern::corners() const
{
	return patternCorners;
}

inline Timestamp CameraCalibration::Pattern::timestamp() const
{
	return patternTimestamp;
}

inline bool CameraCalibration::Pattern::isEmpty() const
{
	return patternRows.empty();
}

template <typename TF, typename TS>
inline bool CameraCalibration::Pattern::compareFirst(const std::pair<TF, TS>& first, const std::pair<TF, TS>& second)
{
	return first.first < second.first;
}

}

}

#endif // META_OCEAN_GEOMETRY_CAMERA_CALIBRATION_H
