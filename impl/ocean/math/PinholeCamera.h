/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_PINHOLE_CAMERA_H
#define META_OCEAN_MATH_PINHOLE_CAMERA_H

#include "ocean/math/Math.h"
#include "ocean/math/Box2.h"
#include "ocean/math/Box3.h"
#include "ocean/math/Camera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line2.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Lookup2.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Triangle3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class PinholeCameraT;

/**
 * Definition of an pinhole camera object with Scalar precision.
 * @see PinholeCameraT
 * @ingroup math
 */
typedef PinholeCameraT<Scalar> PinholeCamera;

/**
 * Definition of an pinhole camera object with double precision.
 * @see PinholeCameraT
 * @ingroup math
 */
typedef PinholeCameraT<double> PinholeCameraD;

/**
 * Definition of an pinhole camera object with float precision.
 * @see PinholeCameraT
 * @ingroup math
 */
typedef PinholeCameraT<float> PinholeCameraF;

/**
 * Definition of a typename alias for vectors with PinholeCameraT objects.
 * @see PinholeCameraT
 * @ingroup math
 */
template <typename T>
using PinholeCamerasT = std::vector<PinholeCameraT<T>>;

/**
 * Definition of a vector holding pinhole camera objects.
 * @see PinholeCamera
 * @ingroup math
 */
typedef PinholeCamerasT<Scalar> PinholeCameras;

/**
 * Definition of a vector holding PinholeCameraD objects.
 * @see PinholeCameraD
 * @ingroup math
 */
typedef PinholeCamerasT<double> PinholeCamerasD;

/**
 * Definition of a vector holding PinholeCameraF objects.
 * @see PinholeCameraF
 * @ingroup math
 */
typedef PinholeCamerasT<float> PinholeCamerasF;

/**
 * Definition of a pinhole camera model.<br>
 * The class holds the intrinsic and distortion parameters of a camera.<br>
 * <pre>
 * The camera holds:
 * 1) Width and height of the camera image.
 * 2) Intrinsic camera matrix:
 * | Fx  0 mx |
 * |  0 Fy my |
 * |  0  0  1 |
 * with mx and my as principal point,
 * and with Fx = f / sx, Fy = f / sy, with focus f and pixel sizes sx and sy.
 * 3) Two radial distortion parameters k1, and k2 for r^2 and r^4:
 * 4) Two tangential distortion parameters p1 and p2.
 * An distortion free (undistorted) image point is transformed to the corresponding distorted image position as follows:
 * x' = x + x * (k1 * r^2 + k2 * r^4) + p1 * 2 * x * y + p2 * (r^2 + 2 * x^2),
 * y' = y + y * (k1 * r^2 + k2 * r^4) + p2 * 2 * x * y + p1 * (r^2 + 2 * y^2).
 * With: Undistorted and normalized image coordinates (x, y) and distorted and normalized image coordinates (x', y'),
 * r^2 is defined by: r^2 = (x^2 + y^2), while r^4 = (x^2 + y^2)^2.
 * The normalized image coordinate is obtained from a 3D object point by application of the perspective division.
 * Normalized image coordinates may be transformed into the pixel coordinate system by applying the focal length and the principal point.
 * </pre>
 * This class provides several distort, undistort or projection functions of image points or object points.<br>
 * Points lying outside the camera frame, may be distorted or undistorted due to the given position or due to the nearest position lying on the camera frame's border.<br>
 * The larger the distance to the principal point of the camera, the more irregular the un-/distortion of the distortion model of this camera.<br>
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class OCEAN_MATH_EXPORT PinholeCameraT : public CameraT<T>
{
	template <typename U> friend class PinholeCameraT;

	public:

		/// The scalar data type of this object.
		typedef T TScalar;

		/// Definition of a pair of distortion values.
		typedef std::pair<T, T> DistortionPair;

		/**
		 * Definition of individual optimization strategies for camera parameters.
		 */
		enum OptimizationStrategy
		{
			/// No optimization.
			OS_NONE = 0,
			/// Optimization of one focal length parameter (the same/identical parameter for horizontal and vertical focal length).
			OS_FOCAL_LENGTH = 1,
			/// Optimization of two focal length parameters: Horizontal focal length and vertical focal length.
			OS_FOCAL_LENGTHS = 2,
			/// Optimization of the four (basic) intrinsic camera parameters: Horizontal focal length, vertical focal length, horizontal principal point, vertical principal point.
			OS_INTRINSIC_PARAMETERS = 4,
			/// Optimization of six parameters: Horizontal focal length, vertical focal length, two radial distortion parameters, two tangential distortion parameters.
			OS_FOCAL_LENGTHS_DISTORTION = 6,
			/// Optimization of all camera parameters while the horizontal and vertical focal length parameter is identical.
			OS_SYMMETRIC_INTRINSIC_PARAMETERS_DISTORTIONS = 7,
			/// Optimization of all 8 intrinsic camera parameters including the distortion parameters: (2x focal length, 2x principal point, 2x radial distortion, 2x tangential distortion).
			OS_INTRINSIC_PARAMETERS_DISTORTIONS = 8,
			/// Optimization of four distortion parameters: Two radial distortion parameters, two tangential distortion parameters.
			OS_DISTORTION = 0x1000 | 4,
			/// Optimization of four (basic) intrinsic camera parameters: Horizontal focal length, vertical focal length, horizontal principal point, vertical principal point and two radial distortion parameters.
			OS_INTRINSIC_PARAMETERS_RADIAL_DISTORTION = 0x1000 | 6
		};

		/**
		 * This class encapsulates a lookup table for camera distortion offsets allowing for faster un-distortion of image points (which is than an approximated position only).
		 */
		class OCEAN_MATH_EXPORT DistortionLookup
		{
			protected:

				/**
				 * Definition of a lookup table for 2D vectors.
				 */
				typedef LookupCenter2<VectorT2<T>, T> LookupTable;

			public:

				/**
				 * Creates an invalid lookup object.
				 */
				inline DistortionLookup();

				/**
				 * Creates an lookup object for a given camera.
				 * @param camera The camera profile for that the lookup table is determined
				 * @param binSize The size of the lookup bins for the camera distortion, horizontal and vertical in pixel, with range [1u, min(camera.width(), camera.height())]
				 */
				explicit DistortionLookup(const PinholeCameraT<T>& camera, const unsigned int binSize);

				/**
				 * Returns the undistorted image point for a given (distorted) image point (by application of a bilinear interpolation).
				 * @param distortedImagePoint The distorted image point for that the undistorted image point is returned
				 * @return The approximation of the undistorted image point
				 */
				inline VectorT2<T> undistortedImagePoint(const VectorT2<T>& distortedImagePoint) const;

				/**
				 * Returns the offset that needs to be added to an distorted image point so that it would be undistorted (by application of a bilinear interpolation).
				 * @param distortedImagePoint The distorted image point for that the offset is returned
				 * @return The approximation of the offset
				 */
				inline VectorT2<T> undistortionOffset(const VectorT2<T>& distortedImagePoint) const;

				/**
				 * Returns the undistorted image point for a given (distorted) image point (by application of a bicubic interpolation).
				 * @param distortedImagePoint The distorted image point for that the undistorted image point is returned
				 * @return The approximation of the undistorted image point
				 */
				inline VectorT2<T> undistortedImagePointBicubic(const VectorT2<T>& distortedImagePoint) const;

				/**
				 * Returns the offset that needs to be added to an distorted image point so that it would be undistorted (by application of a bicubic interpolation).
				 * @param distortedImagePoint The distorted image point for that the offset is returned
				 * @return The approximation of the offset
				 */
				inline VectorT2<T> undistortionOffsetBicubic(const VectorT2<T>& distortedImagePoint) const;

			protected:

				/// The distortion lookup table.
				LookupTable distortionLookupTable;
		};

	public:

		/**
		 * Standard constructor.
		 * Creates a new PinholeCameraT<T> object with all internal parameters as zero.
		 * The resulting camera object is invalid.<br>
		 */
		PinholeCameraT() = default;

		/**
		 * Creates a new camera object with specified frame dimension and intrinsic camera parameters best matching to a given reference camera profile with different frame dimension.
		 * Beware: The dimension aspect ratio between the new camera and the given reference camera profile should be almost similar.
		 * @param width The width of the camera dimension in pixel, with range [1, infinity)
		 * @param height The height of the camera dimension in pixel, with range [1, infinity)
		 * @param camera The camera profile that is used to adopt the intrinsic (and distortion) parameters
		 */
		PinholeCameraT(const unsigned int width, const unsigned int height, const PinholeCameraT<T>& camera);

		/**
		 * Creates a new sub-frame camera profile based on a camera profile of the entire camera frame.
		 * @param subFrameLeft The horizontal start position of the sub-frame within the original camera frame, in pixel, with range (-infinity, infinity)
		 * @param subFrameTop The vertical start position of the sub-frame within the original camera frame, in pixel, with range (-infinity, infinity)
		 * @param subFrameWidth The width of the sub-frame in pixel, with range [1, infinity)
		 * @param subFrameHeight The height of the sub-frame in pixel, with range [1, infinity)
		 * @param camera The original camera profile for which a sub-frame camera profile will be created
		 */
		PinholeCameraT(const T subFrameLeft, const T subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const PinholeCameraT<T>& camera);

		/**
		 * Creates a new PinholeCameraT<T> object by it's given intrinsic parameters.
		 * @param width The width of the camera image, in pixel, with range [1, infinity)
		 * @param height The height of the camera image, in pixel, with range [1, infinity)
		 * @param focalX The focal parameter of the horizontal axis
		 * @param focalY The focal parameter of the vertical axis
		 * @param principalX The principal point of the horizontal axis (in pixel)
		 * @param principalY The principal point of the vertical axis (in pixel)
		 */
		PinholeCameraT(const unsigned int width, const unsigned int height, const T focalX, const T focalY, const T principalX, const T principalY);

		/**
		 * Creates a new PinholeCameraT<T> object by it's given intrinsic parameters.
		 * @param width The width of the camera image, in pixel, with range [1, infinity)
		 * @param height The height of the camera image, in pixel, with range [1, infinity)
		 * @param focalX The focal parameter of the horizontal axis
		 * @param focalY The focal parameter of the vertical axis
		 * @param principalX The principal point of the horizontal axis (in pixel)
		 * @param principalY The principal point of the vertical axis (in pixel)
		 * @param radial The pair of radial distortion parameter for r^2 and r^4
		 * @param tangential The tangential distortion parameters
		 */
		PinholeCameraT(const unsigned int width, const unsigned int height, const T focalX, const T focalY, const T principalX, const T principalY, const DistortionPair& radial, const DistortionPair& tangential);

		/**
		 * Creates a new PinholeCameraT<T> object by it's given intrinsic parameters.
		 * @param width The width of the camera image, in pixel, with range [1, infinity)
		 * @param height The height of the camera image, in pixel, with range [1, infinity)
		 * @param parameters The four to eight intrinsic and distortion parameters of the camera focalLegnthX(), focalLengthY(), principalPointX(), principalPointY(), radialDistortion().first, radialDistortion().second, tangentialDistortion().first, tangentialDistortion().second
		 * @param radialDistortion True, if parameter[4] and parameter[5] exist and store radial distortion parameters, must be 'True' if tangentialDistortion is 'True'
		 * @param tangentialDistortion True, if parameter[6] and parameter[7] exist and store tangential distortion parameters
		 */
		PinholeCameraT(const unsigned int width, const unsigned int height, const T* parameters, const bool radialDistortion = true, const bool tangentialDistortion = true);

		/**
		 * Creates a new PinholeCameraT<T> object by a given projection matrix with the intrinsic camera parameters.
		 * @param intrinsic The matrix with intrinsic camera parameter.
		 */
		explicit PinholeCameraT(const SquareMatrixT3<T>& intrinsic);

		/**
		 * Creates a new PinholeCameraT<T> object by the given intrinsic camera matrix and width and height of the camera.
		 * @param intrinsic The intrinsic camera matrix
		 * @param width The width of the camera image, in pixel, with range [1, infinity)
		 * @param height The height of the camera image, in pixel, with range [1, infinity)
		 */
		PinholeCameraT(const SquareMatrixT3<T>& intrinsic, const unsigned int width, const unsigned int height);

		/**
		 * Creates a new PinholeCameraT<T> object by the given intrinsic camera matrix, the width and height and the radial distortion parameters.
		 * @param intrinsic The intrinsic camera matrix
		 * @param width The width of the camera image, in pixel, with range [1, infinity)
		 * @param height The height of the camera image, in pixel, with range [1, infinity)
		 * @param radial The radial distortion parameters (r^2 and r^4)
		 * @param tangential The tangential distortion parameters
		 */
		PinholeCameraT(const SquareMatrixT3<T>& intrinsic, const unsigned int width, const unsigned int height, const DistortionPair& radial, const DistortionPair& tangential);

		/**
		 * Creates a new PinholeCameraT<T> object by the given width, height and field of view of a camera.
		 * This camera has no radial distortion.
		 * @param width The width of the camera image, in pixel, with range [1, infinity)
		 * @param height The height of the camera image, in pixel, with range [1, infinity)
		 * @param fovX The field of view in x-direction, in radian, with range (0, PI)
		 */
		PinholeCameraT(const unsigned int width, const unsigned int height, const T fovX);

		/**
		 * Creates a new PinholeCameraT<T> object by the given frame dimensions, the ideal field of view, and the camera's principal point.
		 * The provided field of view is expected to be the ideal field of view (if the camera would have a principal point in the perfect center of the image).<br>
		 * This camera has no radial distortion.
		 * @param width The width of the camera image, in pixel, with range [1, infinity)
		 * @param height The height of the camera image, in pixel, with range [1, infinity)
		 * @param fovX The camera's ideal field of view in x-direction, in radian, with range (0, PI)
		 * @param principalX The horizontal principal point within the camera frame, in pixel, with range (-infinity, infinity)
		 * @param principalY The vertical principal point within the camera frame, in pixel, with range (-infinity, infinity)
		 */
		PinholeCameraT(const unsigned int width, const unsigned int height, const T fovX, const T principalX, const T principalY);

		/**
		 * Copy constructor for a pinhole camera with difference element data type than T.
		 * @param pinholeCamera The pinhole camera profile to be copied
		 * @param copyDistortionParameters True, to copy the distortion parameters; False, create a new pinhole camera without distortion parameters
		 * @tparam U The element data type of the given pinhole camera
		 */
		template <typename U>
		explicit inline PinholeCameraT(const PinholeCameraT<U>& pinholeCamera, const bool copyDistortionParameters = true);

		/**
		 * Returns the intrinsic camera matrix.
		 * @return The intrinsic camera matrix
		 */
		inline const SquareMatrixT3<T>& intrinsic() const;

		/**
		 * Returns the inverted intrinsic camera matrix.
		 * @return The inverted intrinsic camera matrix
		 */
		inline const SquareMatrixT3<T>& invertedIntrinsic() const;

		/**
		 * Returns the pair of radial distortion parameters.
		 * @return The radial distortion parameters for r^2 and r^4
		 */
		inline const DistortionPair& radialDistortion() const;

		/**
		 * Returns the pair of tangential distortion parameters.
		 * @return The tangential distortion parameters
		 */
		inline const DistortionPair& tangentialDistortion() const;

		/**
		 * Returns whether this camera object has specified distortion parameters.
		 * @return True, if so
		 */
		inline bool hasDistortionParameters() const;

		/**
		 * Returns the width of the camera image.
		 * @return The width of the camera image
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the camera image.
		 * @return The height of the camera image
		 */
		inline unsigned int height() const;

		/**
		 * Returns the coordinate of the principal point of the camera image in the pixel domain.
		 * @return The 2D location of the principal point, with range [0, width)x[0, height)
		 */
		inline VectorT2<T> principalPoint() const;

		/**
		 * Returns the x-value of the principal point of the camera image in the pixel domain.
		 * @return The x-value of the principal point, with range [0, width)
		 */
		inline T principalPointX() const;

		/**
		 * Returns the y-value of the principal point of the camera image in the pixel domain.
		 * @return The y-value of the principal point, with range [0, height)
		 */
		inline T principalPointY() const;

		/**
		 * Returns the horizontal focal length parameter.
		 * @return The horizontal focal length parameter
		 */
		inline T focalLengthX() const;

		/**
		 * Returns the vertical focal length parameter.
		 * @return The vertical focal length parameter
		 */
		inline T focalLengthY() const;

		/**
		 * Returns the inverse horizontal focal length parameter.
		 * @return The inverse horizontal focal length parameter
		 */
		inline T inverseFocalLengthX() const;

		/**
		 * Returns the inverse vertical focal length parameter.
		 * @return The inverse vertical focal length parameter
		 */
		inline T inverseFocalLengthY() const;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the left and right part of the camera.
		 * @return The field of view (in radian), with range (0, PI)
		 */
		T fovX() const;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the top and bottom part of the camera.
		 * @return The field of view (in radian), with range (0, PI)
		 */
		T fovY() const;

		/**
		 * Returns the left field of view in x direction.
		 * @return The left field of view (in radian), with range (-PI, PI), negative if principal point is outside image region
		 */
		T fovXLeft() const;

		/**
		 * Returns the right field of view in x direction.
		 * @return The right field of view (in radian), with range (-PI, PI), negative if principal point is outside image region
		 */
		T fovXRight() const;

		/**
		 * Returns the top field of view in y direction.
		 * @return The top field of view (in radian), with range (-PI, PI), negative if principal point is outside image region
		 */
		T fovYTop() const;

		/**
		 * Returns the bottom field of view in y direction.
		 * @return The bottom field of view (in radian), with range (-PI, PI), negative if principal point is outside image region
		 */
		T fovYBottom() const;

		/**
		 * Returns the diagonal field of view of the camera
		 * @return The diagonal field of view (in radian), with range (0, PI)
		 */
		T fovDiagonal() const;

		/**
		 * Gets two rotation parameters of the viewing ray for a given undistorted 2D position in the camera image.
		 * @param undistortedPosition The undistorted 2D position [in pixel]
		 * @param angleX The horizontal angle for the viewing ray [in radian]
		 * @param angleY The vertical angle for the viewing ray [in radian]
		 * @return True, if succeeded
		 */
		bool rotation(const VectorT2<T>& undistortedPosition, T& angleX, T& angleY) const;

		/**
		 * Copies the elements of this camera to an array with 4 to 8 floating point values.
		 * The resulting values are stored in the following order: focalLegnthX(), focalLengthY(), principalPointX(), principalPointY(), radialDistortion().first, radialDistortion().second, tangentialDistortion().first, tangentialDistortion().second.
		 * @param arrayValues The array with 4 to 8 floating point values receiving the camera data
		 * @param copyRadialDistortion True, if the array holds at least 6 elements so that the radial parameters will be copied too, must be 'True' if tangentialDistortion is 'True'
		 * @param copyTangentialDistortion True, if the array holds at least 8 elements so that the tangential parameters will be copied too
		 */
		void copyElements(T* arrayValues, const bool copyRadialDistortion = true, const bool copyTangentialDistortion = true) const;

		/**
		 * Sets the intrinsic camera matrix.
		 * @param intrinsic The intrinsic camera matrix
		 * @return True, if the camera matrix is valid
		 */
		bool setIntrinsic(const SquareMatrixT3<T>& intrinsic);

		/**
		 * Sets the radial distortion parameters.
		 * @param radial The pair of radial distortion parameters for r^2 and r^4
		 */
		inline void setRadialDistortion(const DistortionPair& radial);

		/**
		 * Sets the tangential distortion parameters.
		 * @param tangential The pair of tangential distortion parameters
		 */
		inline void setTangentialDistortion(const DistortionPair& tangential);

		/**
		 * Applies a given (relative) zoom factor which mainly multiplies the focal length parameters by the given factor.
		 * @param relativeZoom The (realtive) zoom factor to apply, with range (0, infinity)
		 */
		void applyZoomFactor(const T relativeZoom);

		/**
		 * Checks whether the distortion of this camera is plausible.
		 * The check is based on two individual criteria.
		 * First, we check whether we can distort and un-distort the corner points of the camera frame to almost the same position in the camera frame.<br>
		 * Second we check whether the distortion is almost symmetric by checking that the distances between the distorted corner points of the camera frame and the (virtual) center of the camera frame are almost identical.
		 * @param symmetricFocalLengthRatio The symmetric tolerance ratio between the horizontal and vertical focal length values of the camera, with range (1, infinity)
		 * @param modelAccuracy The accuracy of the distortion model function, measuring the maximal offset between the distorted position of an undistorted position, in percent of the frame size, with range (0, 1)
		 * @param symmetricDistortionRatio The symmetric tolerance ratio between the camera frame's center and the distorted corner positions in percent, with range (1, infinity)
		 * @return True, if so
		 */
		bool isDistortionPlausible(const T symmetricFocalLengthRatio = T(1.05), const T modelAccuracy = T(0.001), const T symmetricDistortionRatio = T(1.08)) const;

		/**
		 * Returns the undistorted position of a given distorted position defined in pixel coordinates.<br>
		 * Beware: As the camera distortion model can not be inverted numerically, there is no guarantee that a given point can be undistorted for any given camera profile.
		 * @param distorted The distorted pixel position to be undistorted
		 * @param iterations The number of iterative calculations, with range [1, 100]
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The resulting undistorted pixel position
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see distort().
		 */
		template <bool tUseBorderDistortionIfOutside>
		VectorT2<T> undistort(const VectorT2<T>& distorted, const unsigned int iterations = 10u, const T zoom = T(1)) const;

		/**
		 * Returns the undistorted position of a given distorted position defined in pixel coordinates.<br>
		 * Beware: As the camera distortion model can not be inverted numerically, there is no guarantee that a given point can be undistorted for any given camera profile.
		 * This function applies a damping for the distortion outside the camera frame so that the quadratic and quartic radii do not have such a significant impact.
		 * @param distorted The distorted pixel position to be undistorted
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param iterations The number of iterative calculations, with range [1, 100]
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The resulting undistorted pixel position
		 * @see distortDamped().
		 */
		VectorT2<T> undistortDamped(const VectorT2<T>& distorted, const T dampingFactor = T(1), const unsigned int iterations = 10u, const T zoom = T(1)) const;

		/**
		 * Returns the distorted position of a given undistorted position defined in pixel coordinates.<br>
		 * The distorted position is calculated by the usage of the internal distortion parameters of this camera object:
		 * <pre>
		 * dx = x + x * (k1 * r^2 + k2 * r^4) + p1 * 2 * x * y + p2 * (r^2 + 2 * x^2),
		 * dy = y + y * (k1 * r^2 + k2 * r^4) + p2 * 2 * x * y + p1 * (r^2 + 2 * y^2).
		 * </pre>
		 * With (dx, dy) the distorted normalized point coordinates, (x, y) the undistorted normalized point coordinates,<br>
		 * and dcx = Fx * dx + mx, dcy = Fy * dy + my the distorted pixel coordinates.
		 * @param undistorted The undistorted position to be distorted in pixel coordinates
		 * @return The resulting distorted position in pixel coordinates
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see undistort().
		 */
		template <bool tUseBorderDistortionIfOutside>
		VectorT2<T> distort(const VectorT2<T>& undistorted) const;

		/**
		 * Returns the distorted position of a given undistorted position defined in pixel coordinates.<br>
		 * This function applies a damping for the distortion outside the camera frame so that the quadratic and quartic radii do not have such a significant impact.<br>
		 * The damping has an asymptotic pattern and so that distortion is based on normalized coordinates never reaching a specified boundary.<br>
		 * The distorted position is calculated by the usage of the internal distortion parameters of this camera object:
		 * <pre>
		 * dx = x + x * (k1 * r^2 + k2 * r^4) + p1 * 2 * x * y + p2 * (r^2 + 2 * x^2),
		 * dy = y + y * (k1 * r^2 + k2 * r^4) + p2 * 2 * x * y + p1 * (r^2 + 2 * y^2).
		 * </pre>
		 * With (dx, dy) the distorted normalized point coordinates, (x, y) the undistorted normalized point coordinates,<br>
		 * and dcx = Fx * dx + mx, dcy = Fy * dy + my the distorted pixel coordinates.
		 * @param undistorted The undistorted position to be distorted in pixel coordinates
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The resulting distorted position in pixel coordinates
		 * @see undistortDamped().
		 */
		VectorT2<T> distortDamped(const VectorT2<T>& undistorted, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Returns whether a given 2D image point lies inside the camera frame.
		 * Optional an explicit border can be defined to allow points slightly outside the camera image, or further inside the image.<br>
		 * Defined a negative border size to allow image points outside the camera frame, or a positive border size to prevent points within the camera frame but close to the boundary.
		 * @param imagePoint The image point to be checked, must be valid
		 * @param signedBorder The optional border increasing or decreasing the rectangle in which the image point must be located, in pixels, with range (-infinity, std::min(width() / 2, height() / 2)
		 * @return True, if the image point lies in the ranges [0, width())x[0, height())
		 */
		inline bool isInside(const VectorT2<T>& imagePoint, const T signedBorder = T(0)) const;

		/**
		 * Returns the viewing angle between two undistorted points on the camera's image plane.
		 * @param first The first undistorted point
		 * @param second The second undistorted point
		 * @return The viewing angle between the defined points (in radian)
		 * @see calculateCosBetween().
		 */
		T calculateAngleBetween(const VectorT2<T>& first, const VectorT2<T>& second) const;

		/**
		 * Returns the cosine of the viewing angle between two undistorted points on the camera's image plane.
		 * @param first The first undistorted point
		 * @param second The second undistorted point
		 * @return The cosine of viewing angle between the defined points
		 * @see calculateAngleBetween().
		 */
		T calculateCosBetween(const VectorT2<T>& first, const VectorT2<T>& second) const;

		/**
		 * Calculates the normalized image point corresponding to a given (distorted) image point.
		 * @param imagePoint The image point (that might be distorted)
		 * @param undistortImagePoint True, to force the un-distortion of the image point using the distortion parameters of this camera object
		 * @return The resulting normalized and (optional undistorted) image point
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 */
		template <bool tUseBorderDistortionIfOutside>
		VectorT2<T> imagePoint2normalizedImagePoint(const VectorT2<T>& imagePoint, const bool undistortImagePoint) const;

		/**
		 * Calculates the normalized image point corresponding to a given (distorted) image point.
		 * This function applies a damping for the distortion outside the camera frame so that the quadratic and quartic radii do not have such a significant impact.
		 * @param imagePoint The image point (that might be distorted)
		 * @param undistortImagePoint True, to force the un-distortion of the image point using the distortion parameters of this camera object
		 * @return The resulting normalized and (optional undistorted) image point
		 */
		inline VectorT2<T> imagePoint2normalizedImagePointDamped(const VectorT2<T>& imagePoint, const bool undistortImagePoint) const;

		/**
		 * Calculates the image point corresponding to a given normalized image point.
		 * @param normalizedImagePoint The normalized image point (projected 3D object point without adjustment of focal length and principal point)
		 * @param distortImagePoint True, to force the distortion of the normalized image point using the distortion parameters of this camera object
		 * @return The resulting image point
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 */
		template <bool tUseBorderDistortionIfOutside>
		VectorT2<T> normalizedImagePoint2imagePoint(const VectorT2<T>& normalizedImagePoint, const bool distortImagePoint) const;

		/**
		 * Calculates the image points corresponding to a set of given normalized image points.
		 * @param normalizedImagePoints The set of normalized image points (projected 3D object points without adjustment of focal length and principal point)
		 * @param numberNormalizedImagePoints The number of normalized image points
		 * @param distortImagePoints True, to force the distortion of the normalized image point using the distortion parameters of this camera object
		 * @param imagePoints The resulting image points, make sure that enough memory is provided
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 */
		template <bool tUseBorderDistortionIfOutside>
		void normalizedImagePoints2imagePoints(const VectorT2<T>* normalizedImagePoints, const size_t numberNormalizedImagePoints, const bool distortImagePoints, VectorT2<T>* imagePoints) const;

		/**
		 * Projects a 3D object point to the 2D image plane of the camera by a given camera pose.
		 * This function may not apply the distortion parameters if the distortion-free projected image point lies outside the camera image.<br>
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param world_T_camera The pose of the camera, must be valid
		 * @param worldObjectPoint The 3D object point to project, defined in world
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The imagePoint Resulting 2D image plane point defined inside the camera pixel coordinate system
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImageIF().
		 */
		template <bool tUseBorderDistortionIfOutside>
		inline VectorT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& worldObjectPoint, const bool distortImagePoint, const T zoom = T(1)) const;

		/**
		 * Projects a 3D box to the 2D image plane of the camera by a given camera pose.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param world_T_camera The pose of the camera, must be valid
		 * @param worldObjectBox The 3D box to project, defined in world, must be valid
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D image box defined inside the camera pixel coordinate system
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImageIF().
		 */
		template <bool tUseBorderDistortionIfOutside>
		inline BoxT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const BoxT3<T>& worldObjectBox, const bool distortImagePoint, const T zoom = T(1)) const;

		/**
		 * Projects a 3D triangle to the 2D image plane of the camera by a given camera pose.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param world_T_camera The pose of the camera, must be valid
		 * @param worldObjectTriangle The 3D triangle to project, defined in world, must be valid
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D image triangle defined inside the camera pixel coordinate system
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImageIF().
		 */
		template <bool tUseBorderDistortionIfOutside>
		inline TriangleT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const TriangleT3<T>& worldObjectTriangle, const bool distortImagePoint, const T zoom = T(1)) const;

		/**
		 * Projects a set of 3D object points onto an image plane of the camera by a given camera pose.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param world_T_camera The pose of the camera, must be valid
		 * @param worldObjectPoints The 3D object points to project, defined in world
		 * @param numberObjectPoints The number of object points to project, with range [0, infinity)
		 * @param distortImagePoints True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param imagePoints The resulting image points, make sure that enough memory is provided
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImageIF().
		 */
		template <bool tUseBorderDistortionIfOutside>
		inline void projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>* worldObjectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T zoom = T(1)) const;

		/**
		 * Projects a 3D line onto an image plane of the camera by a given camera pose.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param world_T_camera The pose of the camera, must be valid
		 * @param worldLine The 3D line to be projected, defined in world, with unit length direction
		 * @param distortProjectedLine True, to distort the projected 3D line (a very rough approximation only)
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The projectedLine Resulting projected 2D line, an invalid line is returned if the projection fails
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 */
		template <bool tUseBorderDistortionIfOutside>
		inline LineT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const LineT3<T>& worldLine, const bool distortProjectedLine, const T zoom = T(1)) const;

		/**
		 * Projects a 3D object point to the 2D image plane of the camera by a given inverse camera pose.
		 * This function may not apply the distortion parameters if the (default) projected image point lies outside the camera image.<br>
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param flippedCamera_T_world The inverted and flipped extrinsic camera matrix, must be valid
		 * @param objectPoint The 3D object point to project
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The imagePoint Resulting 2D image plane point defined inside the camera pixel coordinate system
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImage().
		 */
		template <bool tUseBorderDistortionIfOutside>
		VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint, const bool distortImagePoint, const T zoom = T(1)) const;

		/**
		 * Projects a 3D object point to the 2D image plane of the camera by a given inverse camera pose.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param flippedCamera_T_world The inverted and flipped extrinsic camera matrix, must be valid
		 * @param worldObjectPoint The 3D object point to project, defined in world
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D image plane point defined inside the camera pixel coordinate system
		 * @tparam tDistortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImage().
		 */
		template <bool tDistortImagePoint, bool tUseBorderDistortionIfOutside>
		inline VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint, const T zoom = T(1)) const;

		/**
		 * Projects a 3D box to the 2D image plane of the camera by a given inverse camera pose.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param flippedCamera_T_world The inverted and flipped extrinsic camera matrix, must be valid
		 * @param worldObjectBox The 3D box to project, defined in world, must be valid
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D box defined inside the camera pixel coordinate system
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImage().
		 */
		template <bool tUseBorderDistortionIfOutside>
		BoxT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const BoxT3<T>& worldObjectBox, const bool distortImagePoint, const T zoom = T(1)) const;

		/**
		 * Projects a 3D triangle to the 2D image plane of the camera by a given extrinsic camera pose.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param flippedCamera_T_world The inverted and flipped extrinsic camera matrix, must be valid
		 * @param worldObjectTriangle The 3D triangle to project, defined in world, must be valid
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D triangle defined inside the camera pixel coordinate system
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImage().
		 */
		template <bool tUseBorderDistortionIfOutside>
		TriangleT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const TriangleT3<T>& worldObjectTriangle, const bool distortImagePoint, const T zoom = T(1)) const;

		/**
		 * Transforms a normalized object point (a 3D object point transformed by the inverted and flipped extrinsic camera matrix) into the camera pixel coordinate system.
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param normalizedObjectPoint The 2D normalized object point of a 3D object point
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D image plane point defined inside the camera pixel coordinate system
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImage().
		 */
		template <bool tUseBorderDistortionIfOutside>
		VectorT2<T> projectToImageIF(const VectorT2<T>& normalizedObjectPoint, const bool distortImagePoint, const T zoom = T(1)) const;

		/**
		 * Projects a set of 3D object points onto an image plane of the camera by a given inverse camera pose.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param flippedCamera_T_world The inverted and flipped extrinsic camera matrix, must be valid
		 * @param worldObjectPoints The 3D object points to project, defined in world
		 * @param numberObjectPoints The number of object points to project, with range [0, infinity)
		 * @param distortImagePoints True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param imagePoints The resulting image points, make sure that enough memory is provided
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see projectToImage().
		 */
		template <bool tUseBorderDistortionIfOutside>
		void projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* worldObjectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T zoom = T(1)) const;

		/**
		 * Projects a 3D line onto an image plane of the camera by a given inverse camera pose.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param flippedCamera_T_world The inverted and flipped extrinsic camera matrix, must be valid
		 * @param worldLine The 3D line to be projected, with unit length direction, defined in world, must be valid
		 * @param distortProjectedLine True, to distort the projected 3D line (a very rough approximation only)
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The projectedLine Resulting projected 2D line, an invalid line is returned if the projection fails
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 */
		template <bool tUseBorderDistortionIfOutside>
		LineT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const LineT3<T>& worldLine, const bool distortProjectedLine, const T zoom = T(1)) const;

		/**
		 * Projects a 3D object point to the 2D image plane of the camera by a given extrinsic camera matrix.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param extrinsic The extrinsic camera matrix
		 * @param objectPoint The 3D object point to project
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The imagePoint Resulting 2D image plane point defined inside the camera pixel coordinate system
		 * @see projectToImageDampedIF().
		 */
		inline VectorT2<T> projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>& objectPoint, const bool distortImagePoint, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Projects a 3D box to the 2D image plane of the camera by a given inverse extrinsic camera matrix.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param extrinsic The extrinsic camera matrix
		 * @param objectBox The 3D box to project
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D box defined inside the camera pixel coordinate system
		 * @see projectToImageDampedIF().
		 */
		inline BoxT2<T> projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const BoxT3<T>& objectBox, const bool distortImagePoint, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Projects a 3D triangle to the 2D image plane of the camera by a given extrinsic camera matrix.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param extrinsic The extrinsic camera matrix
		 * @param objectTriangle The 3D triangle to project
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D image triangle defined inside the camera pixel coordinate system
		 * @see projectToImageDampedIF().
		 */
		inline TriangleT2<T> projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const TriangleT3<T>& objectTriangle, const bool distortImagePoint, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Projects a set of 3D object points onto an image plane of the camera by a given extrinsic camera matrix.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param extrinsic The extrinsic camera matrix
		 * @param objectPoints The set of 3D object points to project
		 * @param numberObjectPoints The number of object points to project
		 * @param distortImagePoints True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param imagePoints The resulting image points, make sure that enough memory is provided
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @see projectToImageIF().
		 */
		inline void projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Projects a 3D object point to the 2D image plane of the camera by a given inverse extrinsic camera matrix.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param iFlippedExtrinsic The inverted and flipped extrinsic camera matrix
		 * @param objectPoint The 3D object point to project
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The resulting 2D image plane point defined inside the camera pixel coordinate system
		 * @see projectToImageDamped().
		 */
		VectorT2<T> projectToImageDampedIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const VectorT3<T>& objectPoint, const bool distortImagePoint, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Projects a 3D box to the 2D image plane of the camera by a given inverse extrinsic camera matrix.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param iFlippedExtrinsic The inverted and flipped extrinsic camera matrix
		 * @param objectBox The 3D box to project
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The optional zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D box defined inside the camera pixel coordinate system
		 * @see projectToImageDamped().
		 */
		BoxT2<T> projectToImageDampedIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const BoxT3<T>& objectBox, const bool distortImagePoint, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Projects a 3D triangle to the 2D image plane of the camera by a given inverse extrinsic camera matrix.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param iFlippedExtrinsic The inverted and flipped extrinsic camera matrix
		 * @param objectTriangle The 3D triangle to project
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The 2D triangle defined inside the camera pixel coordinate system
		 * @see projectToImageDamped().
		 */
		TriangleT2<T> projectToImageDampedIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const TriangleT3<T>& objectTriangle, const bool distortImagePoint, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Projects a set of 3D object points onto an image plane of the camera by a given inverse extrinsic camera matrix.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.<br>
		 * Object points projecting outside the camera frame will be distorted (if desired) by application of a damping factor.<br>
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param invertedFlippedExtrinsic The inverted and flipped extrinsic camera matrix
		 * @param objectPoints The set of 3D object points to project
		 * @param numberObjectPoints The number of object points to project
		 * @param distortImagePoints True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @param imagePoints The resulting image points, make sure that enough memory is provided
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @see projectToImageDamped().
		 */
		void projectToImageDampedIF(const HomogenousMatrixT4<T>& invertedFlippedExtrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T dampingFactor = T(1), const T zoom = T(1)) const;

		/**
		 * Returns a normalized vector (with length 1) starting at the camera's center and intersecting a given 2D point on the image plane.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return The normalized vector with -Z direction
		 * @see vectorIF(), ray().
		 */
		inline VectorT3<T> vector(const VectorT2<T>& position, const bool makeUnitVector = true) const;

		/**
		 * Returns a normalized vector (with length 1) starting at the camera's center and intersecting a given 2D point on the image plane.
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return The normalized vector with -Z direction
		 * @see vectorIF(), ray().
		 */
		inline VectorT3<T> vector(const VectorT2<T>& position, const T zoom, const bool makeUnitVector = true) const;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point on the image plane.
		 * The length of the vector is determined so that the vector (exactly) reaches a plane parallel to the image plane.<br>
		 * The plane's normal is parallel to the z-axis while a positive distance locates the plane in the space of the negative z-axis.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param distance The distance between the camera's center and the plane parallel to the image plane, with range (0, infinity)
		 * @return The normalized vector with -Z direction
		 */
		inline VectorT3<T> vectorToPlane(const VectorT2<T>& position, const T distance) const;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point on the image plane.
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.<br>
		 * The length of the vector is determined so that the vector (exactly) reaches a plane parallel to the image plane.<br>
		 * The plane's normal is parallel to the z-axis while a positive distance locates the plane in the space of the negative z-axis.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param distance The distance between the camera's center and the plane parallel to the image plane, with range (0, infinity)
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The normalized vector with -Z direction
		 */
		inline VectorT3<T> vectorToPlane(const VectorT2<T>& position, const T distance, const T zoom) const;

		/**
		 * Returns a normalized vector (with length 1) starting at the camera's center and intersecting a given 2D point on the image plane.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return The normalized vector with +Z direction
		 */
		inline VectorT3<T> vectorIF(const VectorT2<T>& position, const bool makeUnitVector = true) const;

		/**
		 * Returns a normalized vector (with length 1) starting at the camera's center and intersecting a given 2D point on the image plane.
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return The normalized vector with +Z direction
		 */
		inline VectorT3<T> vectorIF(const VectorT2<T>& position, const T zoom, const bool makeUnitVector) const;

		/**
		 * Returns a ray starting at the camera's center and intersection a given 2D point on the image plane.
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param world_T_camera The pose of the camera, the extrinsic camera matrix, must be valid
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The specified ray for a camera pointing towards the negative z-space
		 * @see vector().
		 */
		inline LineT3<T> ray(const VectorT2<T>& position, const HomogenousMatrixT4<T>& world_T_camera, const T zoom = T(1)) const;

		/**
		 * Returns a ray starting at the camera's center and intersection a given 2D point on the image plane.
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param position The 2D position on the image plane, specified in the pixel domain
		 * @param world_t_camera The translation (position) of the camera pose
		 * @param world_Q_camera The standard rotation quaternion of the camera pose, must be valid
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The specified ray for a camera pointing towards the negative z-space
		 * @see vector().
		 */
		inline LineT3<T> ray(const VectorT2<T>& position, const VectorT3<T>& world_t_camera, const QuaternionT<T>& world_Q_camera, const T zoom = T(1)) const;

		/**
		 * Returns the 4x4 frustum projection matrix corresponding to this camera.
		 * The frustum matrix is defined to point into negative z axis and does not provide any distortion parameters.<br>
		 * @param nearDistance The positive distance to the near clipping plane
		 * @param farDistance The positive distance to the far clipping plane
		 * @return The resulting frustum projection matrix
		 */
		SquareMatrixT4<T> frustumMatrix(const T nearDistance, const T farDistance) const;

		/**
		 * Returns a 4x4 homogenous transformation matrix (corresponding to a 3x4 matrix) that covers an extrinsic (inverted and flipped) camera matrix and the intrinsic projection matrix of this camera object.
		 * Further this function can apply a specific zoom to the intrinsic camera matrix.
		 * @param iFlippedExtrinsic The inverted and flipped extrinsic camera matrix
		 * @param zoom The zoom factor of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The resulting transformation matrix
		 */
		inline HomogenousMatrixT4<T> transformationMatrixIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const T zoom = T(1)) const;

		/**
		 * Returns whether two camera profiles are identical up to a given epsilon.
		 * The image resolution must always be identical.
		 * @param camera The second camera profile to be used for comparison, can be invalid
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isEqual(const PinholeCameraT<T>& camera, const T eps = NumericT<T>::eps()) const;

		/**
		 * Calculates the 2x3 jacobian matrix for the 3D object point projection into the camera frame.
		 * The resulting jacobian matrix has the following layout:
		 * <pre>
		 * | dfu / dx, dfu / dy, dfu / dz |
		 * | dfv / dx, dfv / dy, dfv / dz |
		 * with projection function
		 * q = f(p)
		 * q_u = fu(p), q_y = fv(p)
		 * with 2D image point q = (q_u, q_v) and 3D object point p = (x, y, z)
		 * </pre>
		 * @param flippedCameraObjectPoint The 3D object point defined in relation to the inverted and flipped camera pose (camera looking into the positive z-space with y-axis pointing down).
		 * @param jx The resulting first row of the Jacobian matrix, must contain three elements, must be valid
		 * @param jy The resulting second row of the Jacobian matrix, must contain three elements, must be valid
		 * @tparam U The data type of the scalar element either 'float' or 'double'
		 * @tparam tUseDistortionParameters True, to use the camera profile's distortion parameters when calculating the Jacobian; False, to skip the distortion parameters
		 */
		template <typename U, bool tUseDistortionParameters>
		inline void pointJacobian2x3IF(const VectorT3<U>& flippedCameraObjectPoint, U* jx, U* jy) const;

		/**
		 * Returns whether this camera is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two camera objects are identical up to a small epsilon.
		 * @param camera The second camera object
		 * @return True, if so
		 */
		bool operator==(const PinholeCameraT<T>& camera) const;

		/**
		 * Returns whether two camera objects are not identical up to a small epsilon.
		 * @param camera The second camera object
		 * @return True, if so
		 */
		inline bool operator!=(const PinholeCameraT<T>& camera) const;

		/**
		 * Returns whether the camera holds valid parameters.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/**
		 * Determines the inverse of the intrinsic camera matrix.
		 * This function must be invoked immediately after the intrinsic matrix has changed.
		 */
		inline void calculateInverseIntrinsic();

		/**
		 * Returns the distorted position of a given undistorted normalized position.<br>
		 * The distorted position is calculated by the usage of the internal distortion parameters of this camera object:
		 * <pre>
		 * x' = x + x * (k1 * r^2 + k2 * r^4) + p1 * 2 * x * y + p2 * (r^2 + 2 * x^2),
		 * y' = y + y * (k1 * r^2 + k2 * r^4) + p2 * 2 * x * y + p1 * (r^2 + 2 * y^2).
		 * </pre>
		 * @param undistortedNormalized The undistorted normalized position to be distorted
		 * @param invZoom The optional the inverse zoom factor (1/zoom) of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The resulting distorted normalized position
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 * @see distort().
		 */
		template <bool tUseBorderDistortionIfOutside>
		VectorT2<T> distortNormalized(const VectorT2<T>& undistortedNormalized, const T invZoom) const;

		/**
		 * Returns the distorted position of a given undistorted normalized position.<br>
		 * This function applies a damping for the distortion outside the camera frame so that the quadratic and quartic radii do not have such a significant impact.<br>
		 * The damping has an asymptotic pattern and so that distortion is based on normalized coordinates never reaching a specified boundary.<br>
		 * The distorted position is calculated by the usage of the internal distortion parameters of this camera object:
		 * <pre>
		 * dx = x + x * (k1 * r^2 + k2 * r^4) + p1 * 2 * x * y + p2 * (r^2 + 2 * x^2),
		 * dy = y + y * (k1 * r^2 + k2 * r^4) + p2 * 2 * x * y + p1 * (r^2 + 2 * y^2).
		 * </pre>
		 * With (dx, dy) the distorted normalized point coordinates and (x, y) the undistorted normalized point coordinates.
		 * @param undistortedNormalized The undistorted normalized position to be distorted
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param invZoom The optional the inverse zoom factor (1/zoom) of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The resulting distorted normalized position
		 * @see distortDamped().
		 */
		VectorT2<T> distortNormalizedDamped(const VectorT2<T>& undistortedNormalized, const T dampingFactor, const T invZoom) const;

		/**
		 * Determines the damped normalized coordinate for a given normalized coordinate.
		 * The damping is applied to coordinates outside the camera frame.<br>
		 * The damping has an asymptotic pattern so that a damped coordinate never reaches a boundary (specified by the dampingFactor).
		 * @param normalized The normalized coordinate for which the damped coordinate will be calculated
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param invZoom The optional the inverse zoom factor (1/zoom) of the camera, with range (0, infinity), with 1 the default zoom factor
		 * @return The damped normalized coordinate
		 */
		VectorT2<T> dampedNormalized(const VectorT2<T>& normalized, const T dampingFactor, const T invZoom) const;

		/**
		 * Determines the damped normalized coordinate for a given normalized coordinate.
		 * The damping is applied to coordinates outside the camera frame.<br>
		 * The damping has an asymptotic pattern so that a damped coordinate never reaches a boundary (specified by the dampingFactor).
		 * @param normalized The normalized coordinate for which the damped coordinate will be calculated
		 * @param dampingFactor The factor defining the boundary of the asymptotic damping behavior for normalized coordinates, with range [0, infinity)
		 * @param leftNormalizedBorder The left border of camera frame in normalized coordinates, respecting a possible zoom factor already
		 * @param rightNormalizedBorder The right border of the camera frame in normalized coordinates, respecting a possible zoom factor already
		 * @param topNormalizedBorder The top border of the camera frame in normalized coordinates, respecting a possible zoom factor already
		 * @param bottomNormalizedBorder The bottom border of the camera frame in normalized coordinates, respecting a possible zoom factor already
		 * @return The damped normalized coordinate
		 */
		VectorT2<T> dampedNormalized(const VectorT2<T>& normalized, const T dampingFactor, const T leftNormalizedBorder, const T rightNormalizedBorder, const T topNormalizedBorder, const T bottomNormalizedBorder) const;

	private:

		/// Intrinsic camera matrix.
		SquareMatrixT3<T> intrinsics_ = SquareMatrixT3<T>(false);

		/// Inverted intrinsic camera matrix.
		SquareMatrixT3<T> invertedIntrinsics_ = SquareMatrixT3<T>(false);

		/// Width of the camera image (in pixels).
		unsigned int width_ = 0u;

		/// Height of the camera image (in pixels).
		unsigned int height_ = 0u;

		/// Pair of radial distortion parameters for r^2 and r^4.
		DistortionPair radialDistortion_ = DistortionPair(T(0), T(0));

		/// Pair of tangential distortion parameters.
		DistortionPair tangentialDistortion_ = DistortionPair(T(0), T(0));
};

template <typename T>
inline PinholeCameraT<T>::DistortionLookup::DistortionLookup()
{
	// nothing to do here
}

template <typename T>
inline VectorT2<T> PinholeCameraT<T>::DistortionLookup::undistortedImagePoint(const VectorT2<T>& distortedImagePoint) const
{
	ocean_assert(distortionLookupTable);
	return distortionLookupTable.bilinearValue(distortedImagePoint.x(), distortedImagePoint.y()) + distortedImagePoint;
}

template <typename T>
inline VectorT2<T> PinholeCameraT<T>::DistortionLookup::undistortionOffset(const VectorT2<T>& distortedImagePoint) const
{
	ocean_assert(distortionLookupTable);
	return distortionLookupTable.bilinearValue(distortedImagePoint.x(), distortedImagePoint.y());
}

template <typename T>
inline VectorT2<T> PinholeCameraT<T>::DistortionLookup::undistortedImagePointBicubic(const VectorT2<T>& distortedImagePoint) const
{
	ocean_assert(distortionLookupTable);
	return distortionLookupTable.bicubicValue(distortedImagePoint.x(), distortedImagePoint.y()) + distortedImagePoint;
}

template <typename T>
inline VectorT2<T> PinholeCameraT<T>::DistortionLookup::undistortionOffsetBicubic(const VectorT2<T>& distortedImagePoint) const
{
	ocean_assert(distortionLookupTable);
	return distortionLookupTable.bicubicValue(distortedImagePoint.x(), distortedImagePoint.y());
}

template <typename T>
inline const SquareMatrixT3<T>& PinholeCameraT<T>::intrinsic() const
{
	return intrinsics_;
}

template <typename T>
inline const SquareMatrixT3<T>& PinholeCameraT<T>::invertedIntrinsic() const
{
	return invertedIntrinsics_;
}

template <typename T>
inline const typename PinholeCameraT<T>::DistortionPair& PinholeCameraT<T>::radialDistortion() const
{
	return radialDistortion_;
}

template <typename T>
inline const typename PinholeCameraT<T>::DistortionPair& PinholeCameraT<T>::tangentialDistortion() const
{
	return tangentialDistortion_;
}

template <typename T>
template <typename U>
inline PinholeCameraT<T>::PinholeCameraT(const PinholeCameraT<U>& pinholeCamera, const bool copyDistortionParameters) :
	PinholeCameraT<T>(SquareMatrixT3<T>(pinholeCamera.intrinsics_), pinholeCamera.width(), pinholeCamera.height())
{
	if (copyDistortionParameters)
	{
		radialDistortion_ = DistortionPair(T(pinholeCamera.radialDistortion_.first), T(pinholeCamera.radialDistortion_.second));
		tangentialDistortion_ = DistortionPair(T(pinholeCamera.tangentialDistortion_.first), T(pinholeCamera.tangentialDistortion_.second));
	}
}

template <typename T>
inline bool PinholeCameraT<T>::hasDistortionParameters() const
{
	return radialDistortion_.first != 0 || radialDistortion_.second != 0
			|| tangentialDistortion_.first != 0 || tangentialDistortion_.second != 0;
}

template <typename T>
inline unsigned int PinholeCameraT<T>::width() const
{
	return width_;
}

template <typename T>
inline unsigned int PinholeCameraT<T>::height() const
{
	return height_;
}

template <typename T>
inline VectorT2<T> PinholeCameraT<T>::principalPoint() const
{
	return VectorT2<T>(principalPointX(), principalPointY());
}

template <typename T>
inline T PinholeCameraT<T>::principalPointX() const
{
	return intrinsics_(6);
}

template <typename T>
inline T PinholeCameraT<T>::principalPointY() const
{
	return intrinsics_(7);
}

template <typename T>
inline T PinholeCameraT<T>::focalLengthX() const
{
	return intrinsics_(0);
}

template <typename T>
inline T PinholeCameraT<T>::focalLengthY() const
{
	return intrinsics_(4);
}

template <typename T>
inline T PinholeCameraT<T>::inverseFocalLengthX() const
{
	ocean_assert((std::is_same<T, float>::value) || intrinsics_.inverted() == invertedIntrinsics_);
	ocean_assert(NumericT<T>::isEqual(invertedIntrinsics_(0) * intrinsics_(0), 1));

	return invertedIntrinsics_(0);
}

template <typename T>
inline T PinholeCameraT<T>::inverseFocalLengthY() const
{
	ocean_assert((std::is_same<T, float>::value) || intrinsics_.inverted() == invertedIntrinsics_);
	ocean_assert(NumericT<T>::isEqual(invertedIntrinsics_(4) * intrinsics_(4), 1));

	return invertedIntrinsics_(4);
}

template <typename T>
inline void PinholeCameraT<T>::setRadialDistortion(const DistortionPair& radial)
{
	radialDistortion_ = radial;
}

template <typename T>
inline void PinholeCameraT<T>::setTangentialDistortion(const DistortionPair& tangential)
{
	tangentialDistortion_ = tangential;
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
VectorT2<T> PinholeCameraT<T>::undistort(const VectorT2<T>& distorted, const unsigned int iterations, const T zoom) const
{
	ocean_assert(iterations >= 1u && iterations <= 1000u && zoom > NumericT<T>::eps());

	// check whether the camera is distortion free
	if (!hasDistortionParameters())
	{
		return distorted;
	}

	const T invZoom = T(1) / zoom;

	const VectorT2<T> nDistorted((distorted.x() - principalPointX()) * inverseFocalLengthX() * invZoom, (distorted.y() - principalPointY()) * inverseFocalLengthY() * invZoom);

	const VectorT2<T> nMainOffset(distortNormalized<tUseBorderDistortionIfOutside>(nDistorted, invZoom) - nDistorted);
	VectorT2<T> nIntermediateUndistorted(nDistorted - nMainOffset);

	unsigned int i = 0u;

	while (i++ < iterations)
	{
		const VectorT2<T> nIntermediateDistorted(distortNormalized<tUseBorderDistortionIfOutside>(nIntermediateUndistorted, invZoom));
		const VectorT2<T> nIntermediateOffset(nDistorted - nIntermediateDistorted);

		nIntermediateUndistorted = nIntermediateUndistorted + nIntermediateOffset * T(0.75);

		const T offsetPixelX = NumericT<T>::abs(nIntermediateOffset.x() * focalLengthX());
		const T offsetPixelY = NumericT<T>::abs(nIntermediateOffset.y() * focalLengthY());

		if (offsetPixelX < 0.05 && offsetPixelY < 0.05)
		{
			break;
		}

		if (offsetPixelX > T(width_ * 10u) || offsetPixelY > T(height_ * 10u))
		{
			return distorted;
		}
	}

	return VectorT2<T>(nIntermediateUndistorted.x() * focalLengthX() * zoom + principalPointX(), nIntermediateUndistorted.y() * focalLengthY() * zoom + principalPointY());
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
VectorT2<T> PinholeCameraT<T>::distort(const VectorT2<T>& undistorted) const
{
	if (hasDistortionParameters())
	{
		if constexpr (tUseBorderDistortionIfOutside)
		{
			const VectorT2<T> nUndistorted((undistorted.x() - principalPointX()) * inverseFocalLengthX(),
											(undistorted.y() - principalPointY()) * inverseFocalLengthY());

			const VectorT2<T> clampedNormalizedImagePoint(minmax(-principalPointX() * inverseFocalLengthX(), nUndistorted.x(), (T(width_) - principalPointX()) * inverseFocalLengthX()),
															minmax(-principalPointY() * inverseFocalLengthY(), nUndistorted.y(), (T(height_) - principalPointY()) * inverseFocalLengthY()));

			const T sqr = clampedNormalizedImagePoint.sqr();

			const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

			const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
																+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

			const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
																+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

			return VectorT2<T>((nUndistorted.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() + principalPointX(),
								(nUndistorted.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() + principalPointY());
		}
		else
		{
			const VectorT2<T> nUndistorted((undistorted.x() - principalPointX()) * inverseFocalLengthX(),
											(undistorted.y() - principalPointY()) * inverseFocalLengthY());

			const T sqr = nUndistorted.sqr();

			const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

			const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * nUndistorted.x() * nUndistorted.y()
																+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(nUndistorted.x()));

			const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(nUndistorted.y()))
																+ tangentialDistortion_.second * 2 * nUndistorted.x() * nUndistorted.y();

			return VectorT2<T>((nUndistorted.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() + principalPointX(),
								(nUndistorted.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() + principalPointY());
		}
	}
	else
	{
		return undistorted;
	}
}

template <typename T>
inline bool PinholeCameraT<T>::isInside(const VectorT2<T>& imagePoint, const T signedBorder) const
{
	ocean_assert(isValid());
	ocean_assert(signedBorder < T(std::min(width_ / 2u, height_ / 2u)));

	return imagePoint.x() >= signedBorder && imagePoint.y() >= signedBorder
			&& imagePoint.x() < T(width_) - signedBorder && imagePoint.y() < T(height_) - signedBorder;
}

template <typename T>
inline HomogenousMatrixT4<T> PinholeCameraT<T>::transformationMatrixIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const T zoom) const
{
	ocean_assert(zoom > NumericT<T>::eps());

#ifdef OCEAN_DEBUG
	SquareMatrixT4<T> debugCameraMatrix(intrinsic());
	debugCameraMatrix(0, 0) *= zoom;
	debugCameraMatrix(1, 1) *= zoom;
	debugCameraMatrix[15] = 1;

	const SquareMatrixT4<T> debugEntireMatrix(debugCameraMatrix * (SquareMatrixT4<T>&)iFlippedExtrinsic);
	const HomogenousMatrixT4<T>& debugTransformationMatrix = (const HomogenousMatrixT4<T>&)debugEntireMatrix;

	const HomogenousMatrixT4<T> transformationMatrix(HomogenousMatrixT4<T>(VectorT3<T>(intrinsics_[0] * zoom, intrinsics_[1], intrinsics_[2]), VectorT3<T>(intrinsics_[3], intrinsics_[4] * zoom, intrinsics_[5]), VectorT3<T>(intrinsics_[6], intrinsics_[7], intrinsics_[8])) * iFlippedExtrinsic);
	ocean_assert(transformationMatrix.isValid());

	ocean_assert(transformationMatrix == debugTransformationMatrix);
#endif

	return  HomogenousMatrixT4<T>(HomogenousMatrixT4<T>(VectorT3<T>(intrinsics_[0] * zoom, intrinsics_[1], intrinsics_[2]), VectorT3<T>(intrinsics_[3], intrinsics_[4] * zoom, intrinsics_[5]), VectorT3<T>(intrinsics_[6], intrinsics_[7], intrinsics_[8])) * iFlippedExtrinsic);
}

template <typename T>
template <typename U, bool tUseDistortionParameters>
inline void PinholeCameraT<T>::pointJacobian2x3IF(const VectorT3<U>& flippedCameraObjectPoint, U* jx, U* jy) const
{
	ocean_assert(isValid());
	ocean_assert(jx != nullptr && jy != nullptr);

	if (tUseDistortionParameters && hasDistortionParameters())
	{
		const U x = U(flippedCameraObjectPoint.x());
		const U y = U(flippedCameraObjectPoint.y());
		const U z = U(flippedCameraObjectPoint.z());

		const U fx = U(focalLengthX());
		const U fy = U(focalLengthY());

		const U k1 = U(radialDistortion().first);
		const U k2 = U(radialDistortion().second);

		const U p1 = U(tangentialDistortion().first);
		const U p2 = U(tangentialDistortion().second);

		ocean_assert(NumericT<U>::isNotEqualEps(z));
		const U invZ = U(1) / z;

		const U u = x * invZ;
		const U v = y * invZ;

		const U dist1_u = U(1) + U(6) * p2 * u + U(2) * p1 * v + k1 * (U(3) * u * u + v * v) + k2 * (u * u + v * v) * (U(5) * u * u + v * v);
		const U dist2_u_1_v = U(2) * (p1 * u + v * (p2 + u * (k1 + U(2) * k2 * (u * u + v * v))));
		const U dist2_v = U(1) + U(2) * p2 * u + U(6) * p1 * v + k1 * (u * u + U(3) * v * v) + k2 * (u * u + v * v) * (u * u + U(5) * v * v);

		const U Fx_w_dist1_u = fx * invZ * dist1_u;
		const U Fy_w_dist2_u = fy * invZ * dist2_u_1_v;

		const U Fx_w_dist1_v = fx * invZ * dist2_u_1_v;
		const U Fy_w_dist2_v = fy * invZ * dist2_v;

		const U Fx_w2__ = -fx * invZ * invZ * (x * dist1_u + y * dist2_u_1_v);
		const U Fy_w2__ = -fy * invZ * invZ * (x * dist2_u_1_v + y * dist2_v);

		jx[0] = Fx_w_dist1_u;
		jx[1] = Fx_w_dist1_v;
		jx[2] = Fx_w2__;

		jy[0] = Fy_w_dist2_u;
		jy[1] = Fy_w_dist2_v;
		jy[2] = Fy_w2__;
	}
	else
	{
		ocean_assert(NumericT<U>::isNotEqualEps(U(flippedCameraObjectPoint.z())));
		const U invZ = U(1) / U(flippedCameraObjectPoint.z());

		const U fx_z = U(focalLengthX()) * invZ;
		const U fy_z = U(focalLengthY()) * invZ;

		const U fx_x_z2 = -fx_z * U(flippedCameraObjectPoint.x()) * invZ;
		const U fy_y_z2 = -fy_z * U(flippedCameraObjectPoint.y()) * invZ;

		jx[0] = fx_z;
		jx[1] = 0;
		jx[2] = fx_x_z2;

		jy[0] = 0;
		jy[1] = fy_z;
		jy[2] = fy_y_z2;
	}
}

template <typename T>
inline bool PinholeCameraT<T>::isValid() const
{
	return width_ != 0u && height_ != 0u;
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
VectorT2<T> PinholeCameraT<T>::imagePoint2normalizedImagePoint(const VectorT2<T>& imagePoint, const bool undistortImagePoint) const
{
	if (undistortImagePoint)
	{
		return invertedIntrinsics_ * undistort<tUseBorderDistortionIfOutside>(imagePoint);
	}

	return invertedIntrinsics_ * imagePoint;
}

template <typename T>
inline VectorT2<T> PinholeCameraT<T>::imagePoint2normalizedImagePointDamped(const VectorT2<T>& imagePoint, const bool undistortImagePoint) const
{
	if (undistortImagePoint)
	{
		return invertedIntrinsics_ * undistortDamped(imagePoint);
	}

	return invertedIntrinsics_ * imagePoint;
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
VectorT2<T> PinholeCameraT<T>::normalizedImagePoint2imagePoint(const VectorT2<T>& normalizedImagePoint, const bool distortImagePoints) const
{
	ocean_assert(isValid());

	if (distortImagePoints)
	{
		if constexpr (tUseBorderDistortionIfOutside)
		{
			const VectorT2<T> clampedNormalizedImagePoint(minmax(-principalPointX() * inverseFocalLengthX(), normalizedImagePoint.x(), (T(width_) - principalPointX()) * inverseFocalLengthX()),
															minmax(-principalPointY() * inverseFocalLengthY(), normalizedImagePoint.y(), (T(height_) - principalPointY()) * inverseFocalLengthY()));

			// if the camera does not provide a tangential distortion
			if (tangentialDistortion_.first == 0 && tangentialDistortion_.second == 0)
			{
				const T sqr = clampedNormalizedImagePoint.sqr();
				const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

				return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor) * focalLengthX() + principalPointX(),
									(normalizedImagePoint.y() * radialDistortionFactor) * focalLengthY() + principalPointY());
			}
			else
			{
				ocean_assert(tangentialDistortion_.first != 0 || tangentialDistortion_.second != 0);

				const T sqr = clampedNormalizedImagePoint.sqr();
				const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

				const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
																	+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

				const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
																	+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

				return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() + principalPointX(),
									(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() + principalPointY());
			}
		}
		else
		{
			// if the camera does not provide a tangential distortion
			if (tangentialDistortion_.first == 0 && tangentialDistortion_.second == 0)
			{
				const T sqr = normalizedImagePoint.sqr();
				const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

				return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor) * focalLengthX() + principalPointX(),
									(normalizedImagePoint.y() * radialDistortionFactor) * focalLengthY() + principalPointY());
			}
			else
			{
				ocean_assert(tangentialDistortion_.first != 0 || tangentialDistortion_.second != 0);

				const T sqr = normalizedImagePoint.sqr();
				const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

				const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * normalizedImagePoint.x() * normalizedImagePoint.y()
																	+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.x()));

				const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.y()))
																	+ tangentialDistortion_.second * 2 * normalizedImagePoint.x() * normalizedImagePoint.y();

				return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() + principalPointX(),
									(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() + principalPointY());
			}
		}
	}
	else
	{
		return VectorT2<T>(normalizedImagePoint.x() * focalLengthX() + principalPointX(), normalizedImagePoint.y() * focalLengthY() + principalPointY());
	}
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
void PinholeCameraT<T>::normalizedImagePoints2imagePoints(const VectorT2<T>* normalizedImagePoints, const size_t numberNormalizedImagePoints, const bool distortImagePoints, VectorT2<T>* imagePoints) const
{
	ocean_assert(numberNormalizedImagePoints == 0u || (normalizedImagePoints && imagePoints));

	const T leftClamping = -principalPointX() * inverseFocalLengthX();
	const T rightClamping = (T(width_) - principalPointX()) * inverseFocalLengthX();
	const T topClamping = -principalPointY() * inverseFocalLengthY();
	const T bottomClamping = (T(height_) - principalPointY()) * inverseFocalLengthY();

	if (distortImagePoints && hasDistortionParameters())
	{
		// if the camera does not provide a tangential distortion
		if (tangentialDistortion_.first == 0 && tangentialDistortion_.second == 0)
		{
			T sqr, radialDistortionFactor;

			for (unsigned int n = 0u; n < numberNormalizedImagePoints; ++n)
			{
				const VectorT2<T>& normalizedImagePoint(normalizedImagePoints[n]);

				if constexpr (tUseBorderDistortionIfOutside)
				{
					const VectorT2<T> clampedNormalizedImagePoint(minmax(leftClamping, normalizedImagePoint.x(), rightClamping), minmax(topClamping, normalizedImagePoint.y(), bottomClamping));

					sqr = clampedNormalizedImagePoint.sqr();
					radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					imagePoints[n].x() = (normalizedImagePoint.x() * radialDistortionFactor) * focalLengthX() + principalPointX();
					imagePoints[n].y() = (normalizedImagePoint.y() * radialDistortionFactor) * focalLengthY() + principalPointY();
				}
				else
				{
					sqr = normalizedImagePoint.sqr();
					radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					imagePoints[n].x() = (normalizedImagePoint.x() * radialDistortionFactor) * focalLengthX() + principalPointX();
					imagePoints[n].y() = (normalizedImagePoint.y() * radialDistortionFactor) * focalLengthY() + principalPointY();
				}
			}
		}
		else
		{
			ocean_assert(tangentialDistortion_.first != 0 || tangentialDistortion_.second != 0);
			T sqr, radialDistortionFactor, tangentialDistortionCorrectionX, tangentialDistortionCorrectionY;

			for (unsigned int n = 0u; n < numberNormalizedImagePoints; ++n)
			{
				const VectorT2<T>& normalizedImagePoint(normalizedImagePoints[n]);

				if constexpr (tUseBorderDistortionIfOutside)
				{
					const VectorT2<T> clampedNormalizedImagePoint(minmax(leftClamping, normalizedImagePoint.x(), rightClamping), minmax(topClamping, normalizedImagePoint.y(), bottomClamping));

					sqr = clampedNormalizedImagePoint.sqr();
					radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
																		+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

					tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
																		+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

					imagePoints[n].x() = (normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() + principalPointX();
					imagePoints[n].y() = (normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() + principalPointY();
				}
				else
				{
					sqr = normalizedImagePoint.sqr();
					radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * normalizedImagePoint.x() * normalizedImagePoint.y()
																		+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.x()));

					tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.y()))
																		+ tangentialDistortion_.second * 2 * normalizedImagePoint.x() * normalizedImagePoint.y();

					imagePoints[n].x() = (normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() + principalPointX();
					imagePoints[n].y() = (normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() + principalPointY();
				}
			}
		}
	}
	else
	{
		for (unsigned int n = 0u; n < numberNormalizedImagePoints; ++n)
		{
			const VectorT2<T>& normalizedImagePoint(normalizedImagePoints[n]);

			imagePoints[n].x() = normalizedImagePoint.x() * focalLengthX() + principalPointX();
			imagePoints[n].y() = normalizedImagePoint.y() * focalLengthY() + principalPointY();
		}
	}
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
inline VectorT2<T> PinholeCameraT<T>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& worldObjectPoint, const bool distortImagePoint, const T zoom) const
{
	ocean_assert(world_T_camera.isValid() && zoom > NumericT<T>::eps());
	return projectToImageIF<tUseBorderDistortionIfOutside>(CameraT<T>::standard2InvertedFlipped(world_T_camera), worldObjectPoint, distortImagePoint, zoom);
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
inline BoxT2<T> PinholeCameraT<T>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const BoxT3<T>& worldObjectBox, const bool distortImagePoint, const T zoom) const
{
	ocean_assert(world_T_camera.isValid() && worldObjectBox.isValid() && zoom > NumericT<T>::eps());
	return projectToImageIF<tUseBorderDistortionIfOutside>(CameraT<T>::standard2InvertedFlipped(world_T_camera), worldObjectBox, distortImagePoint, zoom);
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
inline TriangleT2<T> PinholeCameraT<T>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const TriangleT3<T>& worldObjectTriangle, const bool distortImagePoint, const T zoom) const
{
	ocean_assert(world_T_camera.isValid() && worldObjectTriangle.isValid() && zoom > NumericT<T>::eps());
	return projectToImageIF<tUseBorderDistortionIfOutside>(CameraT<T>::standard2InvertedFlipped(world_T_camera), worldObjectTriangle, distortImagePoint, zoom);
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
inline void PinholeCameraT<T>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>* worldObjectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T zoom) const
{
	ocean_assert(world_T_camera.isValid() && zoom > NumericT<T>::eps());
	ocean_assert(numberObjectPoints == 0u || (worldObjectPoints != nullptr && imagePoints != nullptr));

	return projectToImageIF<tUseBorderDistortionIfOutside>(CameraT<T>::standard2InvertedFlipped(world_T_camera), worldObjectPoints, numberObjectPoints, distortImagePoints, imagePoints, zoom);
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
inline LineT2<T> PinholeCameraT<T>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const LineT3<T>& worldLine, const bool distortProjectedLine, const T zoom) const
{
	ocean_assert(world_T_camera.isValid() && zoom > NumericT<T>::eps());
	ocean_assert(worldLine.isValid());

	return projectToImageIF<tUseBorderDistortionIfOutside>(CameraT<T>::standard2InvertedFlipped(world_T_camera), worldLine, distortProjectedLine, zoom);
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
VectorT2<T> PinholeCameraT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint, const bool distortImagePoint, const T zoom) const
{
	ocean_assert(flippedCamera_T_world.isValid() && zoom > NumericT<T>::eps());

	const VectorT3<T> transformedObjectPoint(flippedCamera_T_world * worldObjectPoint);

	ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
	const T factor = T(1) / transformedObjectPoint.z();

	const VectorT2<T> normalizedImagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);

	if (!distortImagePoint)
	{
		return VectorT2<T>(normalizedImagePoint.x() * focalLengthX() * zoom + principalPointX(), normalizedImagePoint.y() * focalLengthY() * zoom + principalPointY());
	}

	if constexpr (tUseBorderDistortionIfOutside)
	{
		const T invZoom = T(1) / zoom;

		const VectorT2<T> clampedNormalizedImagePoint(minmax(-principalPointX() * inverseFocalLengthX() * invZoom, normalizedImagePoint.x(), (T(width_) - principalPointX()) * inverseFocalLengthX() * invZoom),
															minmax(-principalPointY() * inverseFocalLengthY() * invZoom, normalizedImagePoint.y(), (T(height_) - principalPointY()) * inverseFocalLengthY() * invZoom));

		const T sqr = clampedNormalizedImagePoint.sqr();

		const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

		const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
															+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

		const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
															+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

		return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
						(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
	}
	else
	{
		const T sqr = normalizedImagePoint.sqr();

		const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

		const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * normalizedImagePoint.x() * normalizedImagePoint.y()
															+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.x()));

		const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.y()))
															+ tangentialDistortion_.second * 2 * normalizedImagePoint.x() * normalizedImagePoint.y();

		return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
						(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
	}
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
BoxT2<T> PinholeCameraT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const BoxT3<T>& worldObjectBox, const bool distortImagePoint, const T zoom) const
{
	ocean_assert(flippedCamera_T_world.isValid() && zoom > NumericT<T>::eps());
	ocean_assert(worldObjectBox.isValid());

	VectorT3<T> boxObjectCorners[8];
	const unsigned int numberBoxImagePoints = worldObjectBox.corners(boxObjectCorners);

	BoxT2<T> result;
	for (unsigned int n = 0; n < numberBoxImagePoints; ++n)
	{
		result += projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, boxObjectCorners[n], distortImagePoint, zoom);
	}

	return result;
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
TriangleT2<T> PinholeCameraT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const TriangleT3<T>& worldObjectTriangle, const bool distortImagePoint, const T zoom) const
{
	ocean_assert(flippedCamera_T_world.isValid() && zoom > NumericT<T>::eps());
	ocean_assert(worldObjectTriangle.isValid());

	return TriangleT2<T>(projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, worldObjectTriangle.point0(), distortImagePoint, zoom),
						projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, worldObjectTriangle.point1(), distortImagePoint, zoom),
						projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, worldObjectTriangle.point2(), distortImagePoint, zoom));
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
VectorT2<T> PinholeCameraT<T>::projectToImageIF(const VectorT2<T>& normalizedObjectPoint, const bool distortImagePoint, const T zoom) const
{
	ocean_assert(zoom > NumericT<T>::eps());

	if (distortImagePoint)
	{
		if constexpr (tUseBorderDistortionIfOutside)
		{
			const T invZoom = T(1) / zoom;

			const VectorT2<T> clampedNormalizedImagePoint(minmax(-principalPointX() * inverseFocalLengthX() * invZoom, normalizedObjectPoint.x(), (T(width_) - principalPointX()) * inverseFocalLengthX() * invZoom),
																minmax(-principalPointY() * inverseFocalLengthY() * invZoom, normalizedObjectPoint.y(), (T(height_) - principalPointY()) * inverseFocalLengthY() * invZoom));

			const T sqr = clampedNormalizedImagePoint.sqr();

			const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

			const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
																+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

			const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
																+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

			return VectorT2<T>((normalizedObjectPoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
							(normalizedObjectPoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
		}
		else
		{
			const T sqr = normalizedObjectPoint.sqr();

			const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

			const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * normalizedObjectPoint.x() * normalizedObjectPoint.y()
																+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(normalizedObjectPoint.x()));

			const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(normalizedObjectPoint.y()))
																+ tangentialDistortion_.second * 2 * normalizedObjectPoint.x() * normalizedObjectPoint.y();

			return VectorT2<T>((normalizedObjectPoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
							(normalizedObjectPoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
		}
	}
	else
	{
		return VectorT2<T>(normalizedObjectPoint.x() * focalLengthX() * zoom + principalPointX(), normalizedObjectPoint.y() * focalLengthY() * zoom + principalPointY());
	}
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
void PinholeCameraT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* worldObjectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T zoom) const
{
	ocean_assert(flippedCamera_T_world.isValid() && zoom > NumericT<T>::eps());
	ocean_assert((worldObjectPoints != nullptr && imagePoints != nullptr) || numberObjectPoints == 0u);

	if (distortImagePoints && hasDistortionParameters())
	{
		const T invZoom = T(1) / zoom;

		const T leftClamping = -principalPointX() * inverseFocalLengthX() * invZoom;
		const T rightClamping = (T(width_) - principalPointX()) * inverseFocalLengthX() * invZoom;
		const T topClamping = -principalPointY() * inverseFocalLengthY() * invZoom;
		const T bottomClamping = (T(height_) - principalPointY()) * inverseFocalLengthY() * invZoom;

		// if the camera does not provide tangential distortion
		if (tangentialDistortion_.first == 0 && tangentialDistortion_.second == 0)
		{
			for (size_t n = 0; n < numberObjectPoints; ++n)
			{
				const VectorT3<T> objectPoint(flippedCamera_T_world * worldObjectPoints[n]);

				ocean_assert(NumericT<T>::isNotEqualEps(objectPoint.z()));
				const T factor = 1 / objectPoint.z();

				const VectorT2<T> normalizedImagePoint(objectPoint.x() * factor, objectPoint.y() * factor);

				if constexpr (tUseBorderDistortionIfOutside)
				{
					const VectorT2<T> clampedNormalizedImagePoint(minmax(leftClamping, normalizedImagePoint.x(), rightClamping), minmax(topClamping, normalizedImagePoint.y(), bottomClamping));

					const T sqr = clampedNormalizedImagePoint.sqr();
					const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					*imagePoints = VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor) * focalLengthX() * zoom + principalPointX(),
												(normalizedImagePoint.y() * radialDistortionFactor) * focalLengthY() * zoom + principalPointY());
				}
				else
				{
					const T sqr = normalizedImagePoint.sqr();
					const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					*imagePoints = VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor) * focalLengthX() * zoom + principalPointX(),
												(normalizedImagePoint.y() * radialDistortionFactor) * focalLengthY() * zoom + principalPointY());
				}

				++imagePoints;
			}
		}
		else
		{
			ocean_assert(tangentialDistortion_.first != 0 || tangentialDistortion_.second != 0);

			for (size_t n = 0; n < numberObjectPoints; ++n)
			{
				const VectorT3<T> objectPoint(flippedCamera_T_world * worldObjectPoints[n]);

				ocean_assert(NumericT<T>::isNotEqualEps(objectPoint.z()));
				const T factor = 1 / objectPoint.z();

				const VectorT2<T> normalizedImagePoint(objectPoint.x() * factor, objectPoint.y() * factor);

				if constexpr (tUseBorderDistortionIfOutside)
				{
					const VectorT2<T> clampedNormalizedImagePoint(minmax(leftClamping, normalizedImagePoint.x(), rightClamping), minmax(topClamping, normalizedImagePoint.y(), bottomClamping));

					const T sqr = clampedNormalizedImagePoint.sqr();
					const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
																		+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

					const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
																		+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

					*imagePoints = VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
												(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
				}
				else
				{
					const T sqr = normalizedImagePoint.sqr();
					const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

					const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * normalizedImagePoint.x() * normalizedImagePoint.y()
																		+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.x()));

					const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.y()))
																		+ tangentialDistortion_.second * 2 * normalizedImagePoint.x() * normalizedImagePoint.y();

					*imagePoints = VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
												(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
				}

				++imagePoints;
			}
		}
	}
	else
	{
		// create one transformation matrix covering the entire pipeline (transformation and then projection)
		const HomogenousMatrixT4<T> transformationIF(transformationMatrixIF(flippedCamera_T_world, zoom));
		ocean_assert(transformationIF.isValid());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const VectorT3<T> transformedObjectPoint(transformationIF * worldObjectPoints[n]);

			ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
			const T factor = 1 / transformedObjectPoint.z();

			*imagePoints++ = VectorT2<T>(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);
		}
	}
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
LineT2<T> PinholeCameraT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const LineT3<T>& worldLine, const bool distortProjectedLine, const T zoom) const
{
	ocean_assert(flippedCamera_T_world.isValid() && zoom > NumericT<T>::eps());
	ocean_assert(worldLine.isValid());

	const VectorT2<T> firstImagePoint(projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, worldLine.point(), distortProjectedLine, zoom));
	const VectorT2<T> secondImagePoint(projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, worldLine.point(10), distortProjectedLine, zoom));

	if (firstImagePoint == secondImagePoint)
	{
		return LineT2<T>();
	}

	return LineT2<T>(firstImagePoint, (secondImagePoint - firstImagePoint).normalized());
}

template <typename T>
inline VectorT2<T> PinholeCameraT<T>::projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>& objectPoint, const bool distortImagePoint, const T dampingFactor, const T zoom) const
{
	ocean_assert(extrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());
	return projectToImageDampedIF(CameraT<T>::standard2InvertedFlipped(extrinsic), objectPoint, distortImagePoint, dampingFactor, zoom);
}

template <typename T>
inline BoxT2<T> PinholeCameraT<T>::projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const BoxT3<T>& objectBox, const bool distortImagePoint, const T dampingFactor, const T zoom) const
{
	ocean_assert(extrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());
	return projectToImageDampedIF(CameraT<T>::standard2InvertedFlipped(extrinsic), objectBox, distortImagePoint, dampingFactor, zoom);
}

template <typename T>
inline TriangleT2<T> PinholeCameraT<T>::projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const TriangleT3<T>& objectTriangle, const bool distortImagePoint, const T dampingFactor, const T zoom) const
{
	ocean_assert(extrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());
	return projectToImageDampedIF(CameraT<T>::standard2InvertedFlipped(extrinsic), objectTriangle, distortImagePoint, dampingFactor, zoom);
}

template <typename T>
inline void PinholeCameraT<T>::projectToImageDamped(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T dampingFactor, const T zoom) const
{
	ocean_assert(extrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());
	ocean_assert(numberObjectPoints == 0u || (objectPoints && imagePoints));

	return projectToImageDampedIF(CameraT<T>::standard2InvertedFlipped(extrinsic), objectPoints, numberObjectPoints, distortImagePoints, imagePoints, dampingFactor, zoom);
}

template <typename T>
template <bool tDistortImagePoint, bool tUseBorderDistortionIfOutside>
inline VectorT2<T> PinholeCameraT<T>::projectToImageIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const VectorT3<T>& objectPoint, const T zoom) const
{
	ocean_assert(iFlippedExtrinsic.isValid() && zoom > NumericT<T>::eps());

	const VectorT3<T> transformedObjectPoint(iFlippedExtrinsic * objectPoint);

	ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
	const T factor = T(1) / transformedObjectPoint.z();
	ocean_assert(NumericT<T>::isNotEqualEps(factor));

	const VectorT2<T> normalizedImagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);

	if (!tDistortImagePoint)
	{
		return VectorT2<T>(normalizedImagePoint.x() * focalLengthX() * zoom + principalPointX(), normalizedImagePoint.y() * focalLengthY() * zoom + principalPointY());
	}

	if constexpr (tUseBorderDistortionIfOutside)
	{
		const T invZoom = T(1) / zoom;

		const VectorT2<T> clampedNormalizedImagePoint(minmax(-principalPointX() * inverseFocalLengthX() * invZoom, normalizedImagePoint.x(), (T(width_) - principalPointX()) * inverseFocalLengthX() * invZoom),
																minmax(-principalPointY() * inverseFocalLengthY() * invZoom, normalizedImagePoint.y(), (T(height_) - principalPointY()) * inverseFocalLengthY() * invZoom));

		const T sqr = clampedNormalizedImagePoint.sqr();

		const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

		const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
															+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

		const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
															+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

		return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
							(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
	}
	else
	{
		const T sqr = normalizedImagePoint.sqr();

		const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

		const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * normalizedImagePoint.x() * normalizedImagePoint.y()
															+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.x()));

		const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(normalizedImagePoint.y()))
															+ tangentialDistortion_.second * 2 * normalizedImagePoint.x() * normalizedImagePoint.y();

		return VectorT2<T>((normalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
							(normalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
	}
}

template <typename T>
inline VectorT3<T> PinholeCameraT<T>::vector(const VectorT2<T>& position, const bool makeUnitVector) const
{
	/**
	 * Ray determination: with object point (X, Y, Z) and image point (x, y, z) -> (x', y')
	 *
	 * x = Fx * X + mx * Z
	 * y = Fy * Y + my * Z
	 * z = Z
	 *
	 * x' = x / z = (Fx * X) / Z + mx
	 * y' = y / z = (Fy * Y) / Z + my
	 *
	 * Inverse calculation:
	 * (x' - mx) / Fx = X / Z
	 * (y' - my) / Fy = Y / Z
	 *
	 * Using a distance of Z:= 1 results in:
	 * X = (x' - mx) / Fx
	 * Y = (y' - my) / Fy
	 */

	/**
	 * The calculation is identical to the multiplication between the inverted camera matrix and the position vector.
	 *
	 * Inverse camera matrix:
	 * | 1/Fx     0    -mx/Fx |<br>
	 * |   0    1/Fy   -my/Fy |<br>
	 * |   0      0        1  |<br>
	 */

#ifdef OCEAN_DEBUG

	const VectorT3<T> testVector(VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX(), (position(1) - principalPointY()) * -inverseFocalLengthY(), -1).normalized());
	ocean_assert(position.isEqual(projectToImage<false>(HomogenousMatrixT4<T>(true), testVector, false), T(0.01)));
	ocean_assert(NumericT<T>::isWeakEqual(testVector.length(), 1));

#endif

	if (makeUnitVector)
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX(), (position(1) - principalPointY()) * -inverseFocalLengthY(), -1).normalized();
	}
	else
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX(), (position(1) - principalPointY()) * -inverseFocalLengthY(), -1);
	}
}

template <typename T>
inline VectorT3<T> PinholeCameraT<T>::vector(const VectorT2<T>& position, const T zoom, const bool makeUnitVector) const
{
	ocean_assert(zoom > NumericT<T>::eps());

	/**
	 * Ray determination: with object point (X, Y, Z), zoom s, and image point (x, y, z) -> (x', y')
	 *
	 * x = s * Fx * X + mx * Z
	 * y = s * Fy * Y + my * Z
	 * z = Z
	 *
	 * x' = x / z = (s * Fx * X) / Z + mx
	 * y' = y / z = (s * Fy * Y) / Z + my
	 *
	 * Inverse calculation:
	 * (x' - mx) / (s * Fx) = X / Z
	 * (y' - my) / (s * Fy) = Y / Z
	 *
	 * Using a distance of Z:= 1 results in:
	 * X = (x' - mx) / (s * Fx)
	 * Y = (y' - my) / (s * Fy)
	 */

	/**
	 * The calculation is identical to the multiplication between the inverted camera matrix and the position vector.
	 *
	 * Inverse camera matrix:
	 * | 1/(s Fx)     0     -mx/(s Fx) |<br>
	 * |   0      1/(s Fy)  -my/(s Fy) |<br>
	 * |   0          0          1     |<br>
	 */

	const T invZoom = T(1) / zoom;

#ifdef OCEAN_DEBUG

	const VectorT3<T> testVector(VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, (position(1) - principalPointY()) * -inverseFocalLengthY() * invZoom, -1).normalized());
	ocean_assert(position.isEqual(projectToImage<false>(HomogenousMatrixT4<T>(true), testVector, false, zoom), T(0.01)));
	ocean_assert(NumericT<T>::isWeakEqual(testVector.length(), 1));

#endif

	if (makeUnitVector)
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, (position(1) - principalPointY()) * -inverseFocalLengthY() * invZoom, -1).normalized();
	}
	else
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, (position(1) - principalPointY()) * -inverseFocalLengthY() * invZoom, -1);
	}
}

template <typename T>
inline VectorT3<T> PinholeCameraT<T>::vectorToPlane(const VectorT2<T>& position, const T distance) const
{
	/**
	 * Ray determination: with object point (X, Y, Z) and image point (x, y, z) -> (x / z, y / z, 1) -> (x', y')
	 *
	 * x = Fx * X + mx * Z
	 * y = Fy * Y + my * Z
	 * z = Z
	 *
	 * x' = x / z = (Fx * X) / Z + mx
	 * y' = y / z = (Fy * Y) / Z + my
	 *
	 * Inverse calculation:
	 * (x' - mx) / Fx = X / Z
	 * (y' - my) / Fy = Y / Z
	 */

#ifdef OCEAN_DEBUG

	const VectorT3<T> testVector(distance * (position(0) - principalPointX()) * inverseFocalLengthX(), distance * (position(1) - principalPointY()) * -inverseFocalLengthY(), -distance);
	ocean_assert(position.isEqual(projectToImage<false>(HomogenousMatrixT4<T>(true), testVector, false), T(0.01)));
	ocean_assert(NumericT<T>::isWeakEqual(testVector * VectorT3<T>(0, 0, -1), distance));

#endif

	return VectorT3<T>(distance * (position(0) - principalPointX()) * inverseFocalLengthX(), distance * (position(1) - principalPointY()) * -inverseFocalLengthY(), -distance);
}

template <typename T>
inline VectorT3<T> PinholeCameraT<T>::vectorToPlane(const VectorT2<T>& position, const T distance, const T zoom) const
{
	ocean_assert(zoom > NumericT<T>::eps());

	/**
	 * Ray determination: with object point (X, Y, Z) and image point (x, y, z) -> (x / z, y / z, 1) -> (x', y')
	 *
	 * x = Fx * X + mx * Z
	 * y = Fy * Y + my * Z
	 * z = Z
	 *
	 * x' = x / z = (Fx * X) / Z + mx
	 * y' = y / z = (Fy * Y) / Z + my
	 *
	 * Inverse calculation:
	 * (x' - mx) / Fx = X / Z
	 * (y' - my) / Fy = Y / Z
	 */

	const T invZoom = T(1) / zoom;

#ifdef OCEAN_DEBUG

	const VectorT3<T> testVector(distance * (position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, distance * (position(1) - principalPointY()) * -inverseFocalLengthY() * invZoom, -distance);
	ocean_assert(position.isEqual(projectToImage<false>(HomogenousMatrixT4<T>(true), testVector, false, zoom), T(0.01)));
	ocean_assert(NumericT<T>::isWeakEqual(testVector * VectorT3<T>(0, 0, -1), distance));

#endif

	return VectorT3<T>(distance * (position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, distance * (position(1) - principalPointY()) * -inverseFocalLengthY() * invZoom, -distance);
}

template <typename T>
inline VectorT3<T> PinholeCameraT<T>::vectorIF(const VectorT2<T>& position, const bool makeUnitVector) const
{
#ifdef OCEAN_DEBUG

	const VectorT3<T> testVector(VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX(), (position(1) - principalPointY()) * inverseFocalLengthY(), 1).normalized());
	ocean_assert((std::is_same<T, float>::value) || position.isEqual(projectToImageIF<false>(HomogenousMatrixT4<T>(true), testVector, false), T(0.01)));
#endif

	if (makeUnitVector)
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX(), (position(1) - principalPointY()) * inverseFocalLengthY(), 1).normalized();
	}
	else
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX(), (position(1) - principalPointY()) * inverseFocalLengthY(), 1);
	}
}

template <typename T>
inline VectorT3<T> PinholeCameraT<T>::vectorIF(const VectorT2<T>& position, const T zoom, const bool makeUnitVector) const
{
	ocean_assert(zoom > NumericT<T>::eps());

	const T invZoom = T(1) / zoom;

#ifdef OCEAN_DEBUG

	const VectorT3<T> testVector(VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, (position(1) - principalPointY()) * inverseFocalLengthY() * invZoom, 1).normalized());
	ocean_assert(position.isEqual(projectToImageIF<false>(HomogenousMatrixT4<T>(true), testVector, false, zoom), T(0.01)));

#endif

	if (makeUnitVector)
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, (position(1) - principalPointY()) * inverseFocalLengthY() * invZoom, 1).normalized();
	}
	else
	{
		return VectorT3<T>((position(0) - principalPointX()) * inverseFocalLengthX() * invZoom, (position(1) - principalPointY()) * inverseFocalLengthY() * invZoom, 1);
	}
}

template <typename T>
inline LineT3<T> PinholeCameraT<T>::ray(const VectorT2<T>& position, const HomogenousMatrixT4<T>& world_T_camera, const T zoom) const
{
	ocean_assert(world_T_camera.isValid() && zoom > NumericT<T>::eps());
	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::rad2deg((world_T_camera.rotation() * vector(position, zoom)).angle(world_T_camera.rotationMatrix(vector(position, zoom)))) <= T(0.01));
	ocean_assert(NumericT<T>::isEqual((world_T_camera.rotation() * vector(position, zoom)).length(), 1));

	return LineT3<T>(world_T_camera.translation(), world_T_camera.rotationMatrix(vector(position, zoom)));
}

template <typename T>
inline LineT3<T> PinholeCameraT<T>::ray(const VectorT2<T>& position, const VectorT3<T>& world_t_camera, const QuaternionT<T>& world_Q_camera, const T zoom) const
{
	ocean_assert(world_Q_camera.isValid() && zoom >= NumericT<T>::eps());

	return LineT3<T>(world_t_camera, world_Q_camera * vector(position, zoom));
}

template <typename T>
inline void PinholeCameraT<T>::calculateInverseIntrinsic()
{
	/**
	 * The calculation is identical to the multiplication between the inverted camera matrix and the position vector.
	 *
	 * Inverse camera matrix:
	 * | 1/Fx     0    -mx/Fx |<br>
	 * |   0    1/Fy   -my/Fy |<br>
	 * |   0      0        1  |<br>
	 */

	ocean_assert(NumericT<T>::isNotEqualEps(focalLengthX()));
	ocean_assert(NumericT<T>::isNotEqualEps(focalLengthY()));

	const T inverseFocalLengthX = T(1) / focalLengthX();
	const T inverseFocalLengthY = T(1) / focalLengthY();

	invertedIntrinsics_(0, 0) = inverseFocalLengthX;
	invertedIntrinsics_(1, 1) = inverseFocalLengthY;
	invertedIntrinsics_(0, 2) = - principalPointX() * inverseFocalLengthX;
	invertedIntrinsics_(1, 2) = - principalPointY() * inverseFocalLengthY;
	invertedIntrinsics_(2, 2) = 1;

	ocean_assert(invertedIntrinsics_(1, 0) == 0);
	ocean_assert(invertedIntrinsics_(2, 0) == 0);
	ocean_assert(invertedIntrinsics_(0, 1) == 0);
	ocean_assert(invertedIntrinsics_(2, 1) == 0);
}

template <typename T>
inline bool PinholeCameraT<T>::operator!=(const PinholeCameraT<T>& camera) const
{
	return !(*this == camera);
}

template <typename T>
inline PinholeCameraT<T>::operator bool() const
{
	return isValid();
}

template <typename T>
template <bool tUseBorderDistortionIfOutside>
VectorT2<T> PinholeCameraT<T>::distortNormalized(const VectorT2<T>& undistortedNormalized, const T invZoom) const
{
	ocean_assert(invZoom > NumericT<T>::eps());

	if (hasDistortionParameters())
	{
		if constexpr (tUseBorderDistortionIfOutside)
		{
			const VectorT2<T> clampedNormalizedImagePoint(minmax(-principalPointX() * inverseFocalLengthX() * invZoom, undistortedNormalized.x(), (T(width_) - principalPointX()) * inverseFocalLengthX() * invZoom),
																minmax(-principalPointY() * inverseFocalLengthY() * invZoom, undistortedNormalized.y(), (T(height_) - principalPointY()) * inverseFocalLengthY() * invZoom));

			const T sqr = clampedNormalizedImagePoint.sqr();

			const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

			const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y()
																+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.x()));

			const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(clampedNormalizedImagePoint.y()))
																+ tangentialDistortion_.second * 2 * clampedNormalizedImagePoint.x() * clampedNormalizedImagePoint.y();

			return VectorT2<T>(undistortedNormalized.x() * radialDistortionFactor + tangentialDistortionCorrectionX,
								undistortedNormalized.y() * radialDistortionFactor + tangentialDistortionCorrectionY);
		}
		else
		{
			const T sqr = undistortedNormalized.sqr();

			const T radialDistortionFactor = T(1) + radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

			const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * undistortedNormalized.x() * undistortedNormalized.y()
																+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(undistortedNormalized.x()));

			const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(undistortedNormalized.y()))
																+ tangentialDistortion_.second * 2 * undistortedNormalized.x() * undistortedNormalized.y();

			return VectorT2<T>(undistortedNormalized.x() * radialDistortionFactor + tangentialDistortionCorrectionX,
								undistortedNormalized.y() * radialDistortionFactor + tangentialDistortionCorrectionY);
		}
	}
	else
	{
		return undistortedNormalized;
	}
}

}

#endif // META_OCEAN_MATH_PINHOLE_CAMERA_H
