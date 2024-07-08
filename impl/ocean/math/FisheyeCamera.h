/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_FISHEYE_CAMERA_H
#define META_OCEAN_MATH_FISHEYE_CAMERA_H

#include "ocean/math/Math.h"
#include "ocean/math/Camera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix2.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class FisheyeCameraT;

/**
 * Definition of a FisheyeCamera object using Scalar as data type.
 * @see FisheyeCameraT
 * @ingroup math
 */
typedef FisheyeCameraT<Scalar> FisheyeCamera;

/**
 * Definition of a FisheyeCamera object using 'float'' as data type.
 * @see FisheyeCameraT
 * @ingroup math
 */
typedef FisheyeCameraT<float> FisheyeCameraF;

/**
 * Definition of a FisheyeCamera object using 'double'' as data type.
 * @see FisheyeCameraT
 * @ingroup math
 */
typedef FisheyeCameraT<double> FisheyeCameraD;

/**
 * Definition of a typename alias for vectors with FisheyeCameraT objects.
 * @see FisheyeCameraT
 * @ingroup math
 */
template <typename T>
using FisheyeCamerasT = std::vector<FisheyeCameraT<T>>;

/**
 * Definition of a vector holding camera objects.
 * @ingroup math
 */
typedef std::vector<FisheyeCamera> FisheyeCameras;

/**
 * Class representing a fisheye camera.<br>
 * The class holds the intrinsic and distortion parameters of a camera.<br>
 * <pre>
 * The camera holds:
 *
 * 1) Width and height of the camera image.
 *
 * 2) Intrinsic camera matrix:
 * | Fx  0 mx |
 * |  0 Fy my |
 * |  0  0  1 |
 * with mx and my as principal point,
 * and with Fx = f / sx, Fy = f / sy, with focus f and pixel sizes sx and sy.
 *
 * 3) Six radial distortion parameters k3, k5, k7, k9, k11, k13
 *
 * 4) Two tangential distortion parameters p1 and p2.
 *
 * An undistorted image point (x, y), is transformed to the corresponding distorted image point (x', y') as follows:
 * x' = x_r + x_t
 * y' = y_r + y_t

 * radial distortion:
 * x_r = x * (theta + k3 * theta^3 + k5 * theta^5 + k7 * theta^7 + k9 * theta^9 + k11 * theta^11 + k13 * theta^13) / r
 * y_r = y * (theta + k3 * theta^3 + k5 * theta^5 + k7 * theta^7 + k9 * theta^9 + k11 * theta^11 + k13 * theta^13) / r
 *
 * tangential distortion:
 * x_t = p1 * (2 * x_r^2 + radial^2) + p2 * 2 * x_r * y_r,
 * y_t = p2 * (2 * y_r^2 + radial^2) + p1 * 2 * x_r * y_r.
 *
 * with
 * r = sqrt(x^2 + y^2)
 * theta = atan(r)
 * radial^2 = x_r^2 + y_r^2
 *
 * With x, y undistorted normalized coordinates
 * With x', y' distorted normalized coordinates
 *
 * </pre>
 * @ingroup math
 * @tparam T The data type of a scalar, 'float' or 'double'
 */
template <typename T>
class FisheyeCameraT : public CameraT<T>
{
	template <typename U> friend class FisheyeCameraT;

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

		/**
		 * Definition of individual parameter configurations.
		 */
		enum ParameterConfiguration
		{
			/**
			 * An unknown parameter configuration.
			 */
			PC_UNKNOWN,

			/**
			 * 3 parameters with order:
			 * focal length (one identical value for horizontal and vertical direction),
			 * horizontal principal point,
			 * vertical principal point
			 */
			PC_3_PARAMETERS_ONE_FOCAL_LENGTH,

			/**
			 * 4 parameters with order:
			 * horizontal focal length,
			 * vertical focal length,
			 * horizontal principal point,
			 * vertical principal point
			 */
			PC_4_PARAMETERS,

			/**
			 * 11 parameters with order:
			 * focal length (one identical value for horizontal and vertical direction),
			 * horizontal principal point,
			 * vertical principal point,
			 * six radial distortion parameters k3, k5, k7, k9, k11, k13
			 * two tangential distortion parameters p1, p2
			 */
			PC_11_PARAMETERS_ONE_FOCAL_LENGTH,

			/**
			 * 12 parameters with order:
			 * horizontal focal length,
			 * vertical focal length,
			 * horizontal principal point,
			 * vertical principal point,
			 * six radial distortion parameters k3, k5, k7, k9, k11, k13
			 * two tangential distortion parameters p1, p2
			 */
			PC_12_PARAMETERS
		};

	public:

		/**
		 * Default constructor creating an invalid camera object.
		 */
		FisheyeCameraT() = default;

		/**
		 * Copy constructor.
		 * @param fisheyeCamera The fisheye camera profile to be copied
		 */
		FisheyeCameraT(const FisheyeCameraT<T>& fisheyeCamera) = default;

		/**
		 * Copy constructor for a fisheye camera with difference element data type than T.
		 * @param fisheyeCamera The fisheye camera profile to be copied
		 * @tparam U The element data type of the given fisheye camera
		 */
		template <typename U>
		explicit inline FisheyeCameraT(const FisheyeCameraT<U>& fisheyeCamera);

		/**
		 * Creates a new camera object with known field of view.
		 * @param width The width of the camera image (in pixel), with range [1, infinity)
		 * @param height The height of the camera image (in pixel), with range [1, infinity)
		 * @param fovX Field of view in x-direction (in radian), with range (0, PI]
		 */
		inline FisheyeCameraT(const unsigned int width, const unsigned int height, const T fovX);

		/**
		 * Creates a new camera object without distortion parameters.
		 * @param width The width of the camera image (in pixel), with range [1, infinity)
		 * @param height The height of the camera image (in pixel), with range [1, infinity)
		 * @param focalX Focal parameter of the horizontal axis, with range (0, infinity)
		 * @param focalY Focal parameter of the vertical axis, with range (0, infinity)
		 * @param principalX Principal point of the horizontal axis (in pixel)
		 * @param principalY Principal point of the vertical axis (in pixel)
		 */
		inline FisheyeCameraT(const unsigned int width, const unsigned int height, const T focalX, const T focalY, const T principalX, const T principalY);

		/**
		 * Creates a new camera object with distortion parameters.
		 * @param width The width of the camera image (in pixel), with range [1, infinity)
		 * @param height The height of the camera image (in pixel), with range [1, infinity)
		 * @param focalX Focal parameter of the horizontal axis, with range (0, infinity)
		 * @param focalY Focal parameter of the vertical axis, with range (0, infinity)
		 * @param principalX Principal point of the horizontal axis (in pixel), with range (0, width)
		 * @param principalY Principal point of the vertical axis (in pixel), with range (0, height)
		 * @param radialDistortion Six radial distortion values, with order k3, k5, k7, k9, k11, 13, must be valid
		 * @param tangentialDistortion Two tangential distortion values, with order p1, p2, must be valid
		 * @tparam TParameter The scalar data type in which the intrinsic camera parameters are provided, will be converted to 'T' internally, 'float' or 'double'
		 */
		template <typename TParameter>
		inline FisheyeCameraT(const unsigned int width, const unsigned int height, const TParameter focalX, const TParameter focalY, const TParameter principalX, const TParameter principalY, const TParameter* radialDistortion, const TParameter* tangentialDistortion);

		/**
		 * Creates a new camera object with parameters with specific configuration.
		 * @param width The width of the camera image (in pixel), with range [1, infinity)
		 * @param height The height of the camera image (in pixel), with range [1, infinity)
		 * @param parameterConfiguration The configuration of the given parameter, must be valid
		 * @param parameters The parameters matching with the specific configuration, must be valid
		 * @tparam TParameter The scalar data type in which the intrinsic camera parameters are provided, will be converted to 'T' internally, 'float' or 'double'
		 */
		template <typename TParameter>
		inline FisheyeCameraT(const unsigned int width, const unsigned int height, const ParameterConfiguration parameterConfiguration, const TParameter* parameters);

		/**
		 * Returns whether this camera object has specified distortion parameters.
		 * @return True, if so
		 */
		inline bool hasDistortionParameters() const;

		/**
		 * Returns the width of the camera image.
		 * @return Width of the camera image, in pixel, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the camera image.
		 * @return Height of the camera image, in pixel, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the coordinate of the principal point of the camera image in the pixel domain.
		 * @return The 2D location of the principal point, with range [0, width)x[0, height)
		 */
		inline VectorT2<T> principalPoint() const;

		/**
		 * Returns the x-value of the principal point of the camera image in the pixel domain.
		 * @return x-value of the principal point, with range [0, width)
		 */
		inline T principalPointX() const;

		/**
		 * Returns the y-value of the principal point of the camera image in the pixel domain.
		 * @return y-value of the principal point, with range [0, height)
		 */
		inline T principalPointY() const;

		/**
		 * Returns the horizontal focal length parameter.
		 * @return Horizontal focal length parameter
		 */
		inline T focalLengthX() const;

		/**
		 * Returns the vertical focal length parameter.
		 * @return Vertical focal length parameter
		 */
		inline T focalLengthY() const;

		/**
		 * Returns the inverse horizontal focal length parameter.
		 * @return Inverse horizontal focal length parameter
		 */
		inline T inverseFocalLengthX() const;

		/**
		 * Returns the inverse vertical focal length parameter.
		 * @return Inverse vertical focal length parameter
		 */
		inline T inverseFocalLengthY() const;

		/**
		 * Returns the six radial distortion parameters of the camera model.
		 * @return The six radial distortion parameters, with order k3, k5, k7, k9, k11, k13
		 */
		inline const T* radialDistortion() const;

		/**
		 * Returns the two tangential distortion parameters of the camera model.
		 * @return The two tangential distortion parameters, with order p1, p2
		 */
		inline const T* tangentialDistortion() const;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the left and right part of the camera.
		 * @return Field of view (in radian), with range (0, PI]
		 */
		T fovX() const;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the top and bottom part of the camera.
		 * @return Field of view (in radian), with range (0, PI)
		 */
		T fovY() const;

		/**
		 * Returns the diagonal field of view of the camera
		 * @return Diagonal field of view (in radian), with range (0, PI]
		 */
		T fovDiagonal() const;

		/**
		 * Copies the parameters of this camera.
		 * @param width The resulting width of the camera, in pixel, with range [0, infinity)
		 * @param height The resulting height of the camera, in pixel, with range [0, infinity)
		 * @param parameters The resulting parameters of the camera
		 * @param parameterConfiguration The resulting configuration of the resulting parameters
		 */
		template <typename TParameter>
		void copyParameters(unsigned int& width, unsigned int& height, std::vector<TParameter>& parameters, ParameterConfiguration& parameterConfiguration) const;

		/**
		 * Returns whether a given 2D image point lies inside the camera frame.
		 * Optional an explicit border can be defined to allow points slightly outside the camera image, or further inside the image.<br>
		 * Defined a negative border size to allow image points outside the camera frame, or a positive border size to prevent points within the camera frame but close to the boundary.
		 * @param imagePoint Image point to be checked, must be valid
		 * @param signedBorder The optional border increasing or decreasing the rectangle in which the image point must be located, in pixels, with range (-infinity, std::min(width() / 2, height() / 2)
		 * @return True, if the image point lies in the ranges [0, width())x[0, height())
		 */
		inline bool isInside(const VectorT2<T>& imagePoint, const T signedBorder = T(0)) const;

		/**
		 * Returns the normalized distorted position of a given undistorted normalized position.
		 * @param undistortedNormalized Undistorted normalized position to be distorted
		 * @return Resulting distorted normalized position
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 */
		template <bool tUseDistortionParameters = true>
		VectorT2<T> distortNormalized(const VectorT2<T>& undistortedNormalized) const;

		/**
		 * Returns the normalized undistorted position of a given distorted normalized position.
		 * @param distortedNormalized Distorted normalized position to be undistorted
		 * @return Resulting undistorted normalized position
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 */
		template <bool tUseDistortionParameters = true>
		VectorT2<T> undistortNormalized(const VectorT2<T>& distortedNormalized) const;

		/**
		 * Projects a 3D object point into the camera's image of the fisheye camera.
		 * The 3D object point must be defined in relation to the (standard) camera coordinate system.<br>
		 * The default viewing direction of the standard camera is into the negative z-space with x-axis to the right, and y-axis upwards.
		 * @param worldObjectPoint 3D object point which is located in the world
		 * @return Resulting 2D image point within the camera frame
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see projectToImageIF().
		 */
		template <bool tUseDistortionParameters = true>
		inline VectorT2<T> projectToImage(const VectorT3<T>& worldObjectPoint) const;

		/**
		 * Projects a 3D object point into the camera's image of the fisheye camera.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates (world from camera).<br>
		 * The default viewing direction of the camera is into the negative z-space with x-axis to the right, and y-axis upwards.
		 * @param world_T_camera The extrinsic camera matrix, must be valid
		 * @param worldObjectPoint 3D object point which is located in the world
		 * @return Resulting 2D image point within the camera frame
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see projectToImageIF().
		 */
		template <bool tUseDistortionParameters = true>
		inline VectorT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& worldObjectPoint) const;

		/**
		 * Projects a 3D object point to the 2D image plane of the fisheye camera by a given inverted (and flipped) extrinsic camera matrix.
		 * The inverted (and flipped) extrinsic matrix transforms a 3D point given in 3D world coordinates into 3D (flipped) camera coordinates (flipped camera from world).<br>
		 * The default viewing direction of the flipped camera is into the positive z-space with x-axis to the right, and y-axis downwards.
		 * @param flippedCamera_T_world Inverted and flipped extrinsic camera matrix, must be valid
		 * @param worldObjectPoint 3D object point which is located in the world
		 * @return Resulting 2D image point within the camera frame
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see projectToImage().
		 */
		template <bool tUseDistortionParameters = true>
		VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint) const;

		/**
		 * Projects a 3D object point to the 2D image plane of the fisheye camera.
		 * The 3D object point must be defined in relation to the (flipped) camera coordinate system.<br>
		 * The default viewing direction of the flipped camera is into the positive z-space with x-axis to the right, and y-axis downwards.
		 * @param cameraFlippedObjectPoint 3D object point which is located in the flipped camera coordinate system
		 * @return Resulting 2D image point within the camera frame
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see projectToImage().
		 */
		template <bool tUseDistortionParameters = true>
		VectorT2<T> projectToImageIF(const VectorT3<T>& cameraFlippedObjectPoint) const;

		/**
		 * Returns a unit vector (with length 1) starting at the camera's center and intersecting a given 2D point in the image.
		 * The vector is determined for the default camera looking into the negative z-space with y-axis up.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return Unit vector pointing into the negative z-space
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see vectorIF(), ray().
		 */
		template <bool tUseDistortionParameters = true>
		inline VectorT3<T> vector(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector = true) const;

		/**
		 * Returns a normalized vector (with length 1) starting at the camera's center and intersecting a given 2D point on the image plane.
		 * The vector is determined for the default camera looking into the positive z-space with y-axis down.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return Normalized vector into the negative z-space
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see vector().
		 */
		template <bool tUseDistortionParameters = true>
		inline VectorT3<T> vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector = true) const;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param world_T_camera The pose of the camera, the extrinsic camera matrix, must be valid
		 * @return The specified ray with direction pointing into the camera's negative z-space
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see vector().
		 */
		template <bool tUseDistortionParameters = true>
		inline LineT3<T> ray(const VectorT2<T>& distortedImagePoint, const HomogenousMatrixT4<T>& world_T_camera) const;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @return The specified ray with direction pointing into the camera's negative z-space
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 * @see vector().
		 */
		template <bool tUseDistortionParameters = true>
		inline LineT3<T> ray(const VectorT2<T>& distortedImagePoint) const;

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
		 * @tparam tUseDistortionParameters True, to use the camera's distortion parameter; False, to just scale the normalized image due to theta
		 */
		template <bool tUseDistortionParameters = true>
		inline void pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const;

		/**
		 * Returns whether two camera profiles are identical up to a given epsilon.
		 * The image resolution must always be identical.
		 * @param fisheyeCamera The second camera profile to be used for comparison, can be invalid
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isEqual(const FisheyeCameraT<T>& fisheyeCamera, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether this camera is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two fisheye cameras are identical.
		 * @param fisheyeCamera The second fisheye camera to be check
		 * @return True, if so
		 */
		bool operator==(const FisheyeCameraT<T>& fisheyeCamera) const;

		/**
		 * Returns whether two fisheye cameras are not identical.
		 * @param fisheyeCamera The second fisheye camera to be check
		 * @return True, if so
		 */
		inline bool operator!=(const FisheyeCameraT<T>& fisheyeCamera) const;

		/**
		 * Copy assignment operator.
		 * @param fisheyeCamera The fisheye camera profile to be copied
		 * @return A reference to this object
		 */
		FisheyeCameraT<T>& operator=(const FisheyeCameraT<T>& fisheyeCamera) = default;

		/**
		 * Returns whether the camera holds valid parameters.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Calculates the tangential-free distortion of a normalized (distorted) image point.
		 * @param distortedNormalized The distorted normalized image point from which the tangential distortion will be removed
		 * @return The normalized image point containing radial distortion only
		 */
		VectorT2<T> tangentialFreeDistortion(const VectorT2<T>& distortedNormalized) const;

		/**
		 * Determines the 2x2 Jacobian of distorting a normalized image point in a fisheye camera with radial and tangential distortion.
		 * The resulting jacobian has the following form:
		 * <pre>
		 * | dfx / dx, dfx / dy |
		 * | dfy / dx, dfy / dy |
		 * </pre>
		 * @param x The horizontal coordinate of the normalized image point to be distorted
		 * @param y The vertical coordinate of the normalized image point to be distorted
		 * @param radialDistortion The six radial distortion parameters, must be valid
		 * @param tangentialDistortion The two radial distortion parameters, must be valid
		 * @param jx First row of the jacobian, with 2 column entries, must be valid
		 * @param jy Second row of the jacobian, with 2 column entries, must be valid
		 */
		static OCEAN_FORCE_INLINE void jacobianDistortNormalized2x2(const T x, const T y, const T* radialDistortion, const T* tangentialDistortion, T* jx, T* jy);

	protected:

		/// Width of the camera image, in pixel.
		unsigned int cameraWidth_ = 0u;

		/// Height of the camera image, in pixel.
		unsigned int cameraHeight_ = 0u;

		/// The horizontal focal length of the camera, with range (0, infinity)
		T focalLengthX_ = T(0);

		/// The vertical focal length of the camera, with range (0, infinity)
		T focalLengthY_ = T(0);

		/// The horizontal inverse focal length of the camera, with range (0, infinity)
		T invFocalLengthX_ = T(0);

		/// The vertical inverse focal length of the camera, with range (0, infinity)
		T invFocalLengthY_ = T(0);

		/// The horizontal principal point of the camera, in pixels, with range [0, width())
		T principalPointX_ = T(0);

		/// The vertical principal point of the camera, in pixels, with range [0, width())
		T principalPointY_ = T(0);

		/// True, if the distortion parameters are defined.
		bool hasDistortionParameters_ = false;

		/// The six radial distortion parameters.
		T radialDistortion_[6] = {T(0), T(0), T(0), T(0), T(0), T(0)};

		/// The two tangential distortion parameters.
		T tangentialDistortion_[2] = {T(0), T(0)};
};

template <typename T>
template <typename U>
inline FisheyeCameraT<T>::FisheyeCameraT(const FisheyeCameraT<U>& fisheyeCamera) :
	cameraWidth_(fisheyeCamera.cameraWidth_),
	cameraHeight_(fisheyeCamera.cameraHeight_),
	focalLengthX_(T(fisheyeCamera.focalLengthX_)),
	focalLengthY_(T(fisheyeCamera.focalLengthY_)),
	principalPointX_(T(fisheyeCamera.principalPointX_)),
	principalPointY_(T(fisheyeCamera.principalPointY_)),
	hasDistortionParameters_(fisheyeCamera.hasDistortionParameters_)
{
	static_assert(sizeof(radialDistortion_) / 6u == sizeof(T), "Invalid parameter");
	static_assert(sizeof(tangentialDistortion_) / 2u == sizeof(T), "Invalid parameter");

	invFocalLengthX_ = NumericT<T>::isEqualEps(focalLengthX_) ? T(0) : T(1) / focalLengthX_;
	invFocalLengthY_ = NumericT<T>::isEqualEps(focalLengthY_) ? T(0) : T(1) / focalLengthY_;

	for (unsigned int n = 0u; n < 6u; ++n)
	{
		radialDistortion_[n] = T(fisheyeCamera.radialDistortion_[n]);
	}

	tangentialDistortion_[0] = T(fisheyeCamera.tangentialDistortion_[0]);
	tangentialDistortion_[1] = T(fisheyeCamera.tangentialDistortion_[1]);
}

template <typename T>
inline FisheyeCameraT<T>::FisheyeCameraT(const unsigned int width, const unsigned int height, const T fovX) :
	cameraWidth_(width),
	cameraHeight_(height),
	focalLengthX_(0),
	focalLengthY_(0),
	invFocalLengthX_(0),
	invFocalLengthY_(0),
	principalPointX_(0),
	principalPointY_(0),
	hasDistortionParameters_(false)
{
	ocean_assert(cameraWidth_ != 0u && cameraHeight_ != 0u);
	ocean_assert(fovX > NumericT<T>::eps() && fovX <= NumericT<T>::pi());

	const T principalX = T(cameraWidth_) * T(0.5);
	const T principalY = T(cameraHeight_) * T(0.5);

	const T focalLength = principalX / NumericT<T>::tan(fovX * T(0.5));

	focalLengthX_ = focalLength;
	focalLengthY_ = focalLength;

	ocean_assert(NumericT<T>::isNotEqualEps(focalLength));
	const T invFocalLength = T(1) / focalLength;
	invFocalLengthX_ = invFocalLength;
	invFocalLengthY_ = invFocalLength;

	principalPointX_ = principalX;
	principalPointY_ = principalY;

	for (unsigned int n = 0u; n < 6u; ++n)
	{
		radialDistortion_[n] = T(0);
	}

	tangentialDistortion_[0] = T(0);
	tangentialDistortion_[1] = T(0);
}

template <typename T>
inline FisheyeCameraT<T>::FisheyeCameraT(const unsigned int width, const unsigned int height, const T focalX, const T focalY, const T principalX, const T principalY) :
	cameraWidth_(width),
	cameraHeight_(height),
	focalLengthX_(focalX),
	focalLengthY_(focalY),
	invFocalLengthX_(0),
	invFocalLengthY_(0),
	principalPointX_(principalX),
	principalPointY_(principalY),
	hasDistortionParameters_(false)
{
	ocean_assert(cameraWidth_ != 0u && cameraHeight_ != 0u);

	ocean_assert(NumericT<T>::isNotEqualEps(focalLengthX_) && NumericT<T>::isNotEqualEps(focalLengthY_));
	invFocalLengthX_ = T(1) / focalLengthX_;
	invFocalLengthY_ = T(1) / focalLengthY_;
	ocean_assert(fovX() > NumericT<T>::eps() && fovX() <= NumericT<T>::pi());

	for (unsigned int n = 0u; n < 6u; ++n)
	{
		radialDistortion_[n] = T(0);
	}

	tangentialDistortion_[0] = T(0);
	tangentialDistortion_[1] = T(0);
}

template <typename T>
template <typename TParameter>
inline FisheyeCameraT<T>::FisheyeCameraT(const unsigned int width, const unsigned int height, const TParameter focalX, const TParameter focalY, const TParameter principalX, const TParameter principalY, const TParameter* radialDistortion, const TParameter* tangentialDistortion) :
	cameraWidth_(width),
	cameraHeight_(height),
	focalLengthX_(T(focalX)),
	focalLengthY_(T(focalY)),
	invFocalLengthX_(0),
	invFocalLengthY_(0),
	principalPointX_(T(principalX)),
	principalPointY_(T(principalY)),
	hasDistortionParameters_(true)
{
	static_assert((std::is_same<TParameter, float>::value) || (std::is_same<TParameter, double>::value), "Invalid TParameter, must be 'float' or 'double'!");

	ocean_assert(cameraWidth_ != 0u && cameraHeight_ != 0u);
	ocean_assert(NumericT<T>::isNotEqualEps(focalLengthX_) && NumericT<T>::isNotEqualEps(focalLengthY_));
	invFocalLengthX_ = T(1) / focalLengthX_;
	invFocalLengthY_ = T(1) / focalLengthY_;

	for (unsigned int n = 0u; n < 6u; ++n)
	{
		radialDistortion_[n] = T(radialDistortion[n]);
	}

	tangentialDistortion_[0] = T(tangentialDistortion[0]);
	tangentialDistortion_[1] = T(tangentialDistortion[1]);
}

template <typename T>
template <typename TParameter>
inline FisheyeCameraT<T>::FisheyeCameraT(const unsigned int width, const unsigned int height, const ParameterConfiguration parameterConfiguration, const TParameter* parameters) :
	cameraWidth_(width),
	cameraHeight_(height),
	focalLengthX_(0),
	focalLengthY_(0),
	invFocalLengthX_(0),
	invFocalLengthY_(0),
	principalPointX_(0),
	principalPointY_(0),
	hasDistortionParameters_(false)
{
	static_assert((std::is_same<TParameter, float>::value) || (std::is_same<TParameter, double>::value), "Invalid TParameter, must be 'float' or 'double'!");

	ocean_assert(cameraWidth_ != 0u && cameraHeight_ != 0u);
	ocean_assert(parameters != nullptr);

	switch (parameterConfiguration)
	{
		case PC_3_PARAMETERS_ONE_FOCAL_LENGTH:
		{
			focalLengthX_ = T(parameters[0]);
			focalLengthY_ = T(parameters[0]);

			principalPointX_ = T(parameters[1]);
			principalPointY_ = T(parameters[2]);

			hasDistortionParameters_ = false;

			break;
		}

		case PC_4_PARAMETERS:
		{
			focalLengthX_ = T(parameters[0]);
			focalLengthY_ = T(parameters[1]);

			principalPointX_ = T(parameters[2]);
			principalPointY_ = T(parameters[3]);

			hasDistortionParameters_ = false;

			break;
		}

		case PC_11_PARAMETERS_ONE_FOCAL_LENGTH:
		{
			focalLengthX_ = T(parameters[0]);
			focalLengthY_ = T(parameters[0]);

			principalPointX_ = T(parameters[1]);
			principalPointY_ = T(parameters[2]);

			radialDistortion_[0] = T(parameters[3]);
			radialDistortion_[1] = T(parameters[4]);
			radialDistortion_[2] = T(parameters[5]);
			radialDistortion_[3] = T(parameters[6]);
			radialDistortion_[4] = T(parameters[7]);
			radialDistortion_[5] = T(parameters[8]);

			tangentialDistortion_[0] = T(parameters[9]);
			tangentialDistortion_[1] = T(parameters[10]);

			hasDistortionParameters_ = true;

			break;
		}

		case PC_12_PARAMETERS:
		{
			focalLengthX_ = T(parameters[0]);
			focalLengthY_ = T(parameters[1]);

			principalPointX_ = T(parameters[2]);
			principalPointY_ = T(parameters[3]);

			radialDistortion_[0] = T(parameters[4]);
			radialDistortion_[1] = T(parameters[5]);
			radialDistortion_[2] = T(parameters[6]);
			radialDistortion_[3] = T(parameters[7]);
			radialDistortion_[4] = T(parameters[8]);
			radialDistortion_[5] = T(parameters[9]);

			tangentialDistortion_[0] = T(parameters[10]);
			tangentialDistortion_[1] = T(parameters[11]);

			hasDistortionParameters_ = true;

			break;
		}

		default:
			ocean_assert(false && "Invalid parameter configuration!");
			return;
	}

	ocean_assert(NumericT<T>::isNotEqualEps(focalLengthX_) && NumericT<T>::isNotEqualEps(focalLengthY_));
	invFocalLengthX_ = T(1) / focalLengthX_;
	invFocalLengthY_ = T(1) / focalLengthY_;
}

template <typename T>
inline bool FisheyeCameraT<T>::hasDistortionParameters() const
{
	return hasDistortionParameters_;
}

template <typename T>
inline unsigned int FisheyeCameraT<T>::width() const
{
	return cameraWidth_;
}

template <typename T>
inline unsigned int FisheyeCameraT<T>::height() const
{
	return cameraHeight_;
}

template <typename T>
inline VectorT2<T> FisheyeCameraT<T>::principalPoint() const
{
	return VectorT2<T>(principalPointX(), principalPointY());
}

template <typename T>
inline T FisheyeCameraT<T>::principalPointX() const
{
	return principalPointX_;
}

template <typename T>
inline T FisheyeCameraT<T>::principalPointY() const
{
	return principalPointY_;
}

template <typename T>
inline T FisheyeCameraT<T>::focalLengthX() const
{
	return focalLengthX_;
}

template <typename T>
inline T FisheyeCameraT<T>::focalLengthY() const
{
	return focalLengthY_;
}

template <typename T>
inline T FisheyeCameraT<T>::inverseFocalLengthX() const
{
	return invFocalLengthX_;
}

template <typename T>
inline T FisheyeCameraT<T>::inverseFocalLengthY() const
{
	return invFocalLengthY_;
}

template <typename T>
inline const T* FisheyeCameraT<T>::radialDistortion() const
{
	return radialDistortion_;
}

template <typename T>
inline const T* FisheyeCameraT<T>::tangentialDistortion() const
{
	return tangentialDistortion_;
}

template <typename T>
T FisheyeCameraT<T>::fovX() const
{
	ocean_assert(isValid());

	/**
	 * x = Fx * X / Z + mx
	 *
	 * (x - mx) / Fx = X / Z
	 */

	if (NumericT<T>::isEqualEps(focalLengthX()))
	{
		return T(0);
	}

	const T leftAngle = NumericT<T>::abs(NumericT<T>::atan(-principalPointX() * invFocalLengthX_));

	if (T(cameraWidth_) <= principalPointX())
	{
		ocean_assert(false && "Invalid principal point");
		return T(2) * leftAngle;
	}

	const T rightAngle = NumericT<T>::atan((T(cameraWidth_) - principalPointX()) * invFocalLengthX_);

	return leftAngle + rightAngle;
}

template <typename T>
T FisheyeCameraT<T>::fovY() const
{
	ocean_assert(isValid());

	/**
	 * y = Fy * Y / Z + my
	 *
	 * (y - my) / Fy = Y / Z
	 */

	if (NumericT<T>::isEqualEps(focalLengthY()))
	{
		return T(0);
	}

	const T topAngle = NumericT<T>::abs(NumericT<T>::atan(-principalPointY() * invFocalLengthY_));

	if (T(cameraHeight_) <= principalPointY())
	{
		ocean_assert(false && "Invalid principal point");
		return T(2) * topAngle;
	}

	const T bottomAngle = NumericT<T>::atan((T(cameraHeight_) - principalPointY()) * invFocalLengthY_);

	return topAngle + bottomAngle;
}

template <typename T>
T FisheyeCameraT<T>::fovDiagonal() const
{
	const VectorT2<T> topLeft(-principalPointX(), -principalPointY());
	const VectorT2<T> bottomRight(principalPointX(), principalPointY());

	const T diagonal = (topLeft - bottomRight).length();
	const T halfDiagonal = diagonal * T(0.5);

	const T invFocalLength = (invFocalLengthX_ + invFocalLengthY_) * T(0.5);

	return T(2) * NumericT<T>::abs(NumericT<T>::atan(halfDiagonal * invFocalLength));
}

template <typename T>
template <typename TParameter>
void FisheyeCameraT<T>::copyParameters(unsigned int& width, unsigned int& height, std::vector<TParameter>& parameters, ParameterConfiguration& parameterConfiguration) const
{
	if (isValid())
	{
		width = cameraWidth_;
		height = cameraHeight_;

		parameters =
		{
			TParameter(focalLengthX_),
			TParameter(focalLengthY_),

			TParameter(principalPointX_),
			TParameter(principalPointY_),

			TParameter(radialDistortion_[0]),
			TParameter(radialDistortion_[1]),
			TParameter(radialDistortion_[2]),
			TParameter(radialDistortion_[3]),
			TParameter(radialDistortion_[4]),
			TParameter(radialDistortion_[5]),

			TParameter(tangentialDistortion_[0]),
			TParameter(tangentialDistortion_[1]),
		};

		ocean_assert(parameters.size() == 12);

		parameterConfiguration = PC_12_PARAMETERS;
	}
	else
	{
		width = 0u;
		height = 0u;

		parameters.clear();

		parameterConfiguration = PC_UNKNOWN;
	}
}

template <typename T>
inline bool FisheyeCameraT<T>::isInside(const VectorT2<T>& imagePoint, const T signedBorder) const
{
	ocean_assert(isValid());
	ocean_assert(signedBorder < T(std::min(cameraWidth_ / 2u, cameraHeight_ / 2u)));

	return imagePoint.x() >= signedBorder && imagePoint.y() >= signedBorder
			&& imagePoint.x() < T(cameraWidth_) - signedBorder && imagePoint.y() < T(cameraHeight_) - signedBorder;
}

template <typename T>
template <bool tUseDistortionParameters>
VectorT2<T> FisheyeCameraT<T>::distortNormalized(const VectorT2<T>& undistortedNormalized) const
{
	ocean_assert(isValid());

	/*
	 * 3) Two radial distortion parameters k3, k5, k7, k9, k11, k13
	 *
	 * 4) Two tangential distortion parameters p1 and p2.
	 *
	 * An undistorted image point (x, y), is transformed to the corresponding distorted image point (x', y') as follows:
	 * x' = x_r + x_t
	 * y' = y_r + y_t

	 * radial distortion:
	 * x_r = x * (theta + k3 * theta^3 + k5 * theta^5 + k7 * theta^7 + k9 * theta^9 + k11 * theta^11 + k13 * theta^13) / r
	 * y_r = y * (theta + k3 * theta^3 + k5 * theta^5 + k7 * theta^7 + k9 * theta^9 + k11 * theta^11 + k13 * theta^13) / r
	 *
	 * tangential distortion:
	 * x_t = p1 * (2 * x_r^2 + radial^2) + p2 * 2 * x_r * y_r,
	 * y_t = p2 * (2 * y_r^2 + radial^2) + p1 * 2 * x_r * y_r.
	 *
	 * with
	 * r = sqrt(x^2 + y^2)
	 * theta = atan(r)
	 * radial^2 = x_r^2 + y_r^2
	 *
	 * With x, y undistorted normalized coordinates
	 * With x', y' distorted normalized coordinates
	 */

	const T r2 = undistortedNormalized.sqr();
	const T r = NumericT<T>::sqrt(r2);

	if (NumericT<T>::isEqualEps(r))
	{
		return VectorT2<T>(0, 0);
	}

	const T theta = NumericT<T>::atan(r);

	if (tUseDistortionParameters && hasDistortionParameters_)
	{
		const T theta2 = theta * theta;
		const T theta3 = theta2 * theta;
		const T theta5 = theta2 * theta3;
		const T theta7 = theta2 * theta5;
		const T theta9 = theta2 * theta7;
		const T theta11 = theta2 * theta9;
		const T theta13 = theta2 * theta11;

		const T& k3 = radialDistortion_[0];
		const T& k5 = radialDistortion_[1];
		const T& k7 = radialDistortion_[2];
		const T& k9 = radialDistortion_[3];
		const T& k11 = radialDistortion_[4];
		const T& k13 = radialDistortion_[5];

		const T radialDistortionFactor = (theta + k3 * theta3 + k5 * theta5 + k7 * theta7 + k9 * theta9 + k11 * theta11 + k13 * theta13) / r;

		const T x_r = undistortedNormalized.x() * radialDistortionFactor;
		const T y_r = undistortedNormalized.y() * radialDistortionFactor;

		const T radius_r2 = x_r * x_r + y_r * y_r;

		const T& p1 = tangentialDistortion_[0];
		const T& p2 = tangentialDistortion_[1];

		const T x_t = p1 * (T(2) * x_r * x_r + radius_r2) + p2 * T(2) * x_r * y_r;
		const T y_t = p2 * (T(2) * y_r * y_r + radius_r2) + p1 * T(2) * x_r * y_r;

		return VectorT2<T>(x_r + x_t, y_r + y_t);
	}
	else
	{
		const T scale = theta / r;

		return VectorT2<T>(undistortedNormalized.x() * scale, undistortedNormalized.y() * scale);
	}
}

template <typename T>
template <bool tUseDistortionParameters>
VectorT2<T> FisheyeCameraT<T>::undistortNormalized(const VectorT2<T>& distortedNormalized) const
{
	ocean_assert(isValid());

	if constexpr (tUseDistortionParameters)
	{
		const VectorT2<T> distortedTangentialFree = tangentialFreeDistortion(distortedNormalized);

		const T& k3 = radialDistortion_[0];
		const T& k5 = radialDistortion_[1];
		const T& k7 = radialDistortion_[2];
		const T& k9 = radialDistortion_[3];
		const T& k11 = radialDistortion_[4];
		const T& k13 = radialDistortion_[5];

		const T r = distortedTangentialFree.length();

		if (NumericT<T>::isEqualEps(r))
		{
			return VectorT2<T>(0, 0);
		}

		T theta = NumericT<T>::pow(r, T(0.3333333333333));

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const T theta2 = theta * theta;
			const T theta4 = theta2 * theta2;
			const T theta6 = theta4 * theta2;
			const T theta8 = theta6 * theta2;
			const T theta10 = theta8 * theta2;
			const T theta12 = theta10 * theta2;

			const T error = theta * (T(1) + k3 * theta2 + k5 * theta4 + k7 * theta6 + k9 * theta8 + k11 * theta10 + k13 * theta12) - r;

			const T df = T(1) + T(3) * k3 * theta2 + T(5) * k5 * theta4 + T(7) * k7 * theta6 + T(9) * k9 * theta8 + T(11) * k11 * theta10 + T(13) * k13 * theta12;

			if (NumericT<T>::isEqualEps(df))
			{
				break;
			}

			const T delta = error / df;

			theta -= delta;

			if (NumericT<T>::isEqualEps(delta))
			{
				break;
			}
		}

		const T scale = NumericT<T>::tan(theta) / r;

		return distortedTangentialFree * scale;
	}
	else
	{
		const T r = distortedNormalized.length();

		if (NumericT<T>::isEqualEps(r))
		{
			return VectorT2<T>(0, 0);
		}

		const T scale = NumericT<T>::tan(r) / r;

		return distortedNormalized * scale;
	}
}

template <typename T>
template <bool tUseDistortionParameters>
inline VectorT2<T> FisheyeCameraT<T>::projectToImage(const VectorT3<T>& worldObjectPoint) const
{
	ocean_assert(isValid());

	return projectToImageIF<tUseDistortionParameters>(VectorT3<T>(worldObjectPoint.x(), -worldObjectPoint.y(), -worldObjectPoint.z()));
}

template <typename T>
template <bool tUseDistortionParameters>
inline VectorT2<T> FisheyeCameraT<T>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& worldObjectPoint) const
{
	ocean_assert(isValid());

	ocean_assert(world_T_camera.isValid());
	return projectToImageIF<tUseDistortionParameters>(CameraT<T>::standard2InvertedFlipped(world_T_camera), worldObjectPoint);
}

template <typename T>
template <bool tUseDistortionParameters>
VectorT2<T> FisheyeCameraT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& worldObjectPoint) const
{
	ocean_assert(isValid());

	ocean_assert(flippedCamera_T_world.isValid());
	return projectToImageIF<tUseDistortionParameters>(flippedCamera_T_world * worldObjectPoint);
}

template <typename T>
template <bool tUseDistortionParameters>
VectorT2<T> FisheyeCameraT<T>::projectToImageIF(const VectorT3<T>& cameraFlippedObjectPoint) const
{
	ocean_assert(isValid());
	ocean_assert(NumericT<T>::isNotEqualEps(cameraFlippedObjectPoint.z()));

	const T invZ = T(1) / cameraFlippedObjectPoint.z();

	const VectorT2<T> undistortedNormalized(cameraFlippedObjectPoint.x() * invZ, cameraFlippedObjectPoint.y() * invZ);
	const VectorT2<T> distortedNormalizedImagePoint = distortNormalized<tUseDistortionParameters>(undistortedNormalized);

	return VectorT2<T>(distortedNormalizedImagePoint.x() * focalLengthX() + principalPointX(), distortedNormalizedImagePoint.y() * focalLengthY() + principalPointY());
}

template <typename T>
template <bool tUseDistortionParameters>
inline VectorT3<T> FisheyeCameraT<T>::vector(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	ocean_assert(isValid());

	const VectorT2<T> distortedNormalized((distortedImagePoint.x() - principalPointX_) * invFocalLengthX_, (distortedImagePoint.y() - principalPointY_) * invFocalLengthY_);
	const VectorT2<T> undistortedNormalized = undistortNormalized<tUseDistortionParameters>(distortedNormalized);

	if (makeUnitVector)
	{
		return VectorT3<T>(undistortedNormalized.x(), -undistortedNormalized.y(), T(-1)).normalized();
	}
	else
	{
		return VectorT3<T>(undistortedNormalized.x(), -undistortedNormalized.y(), T(-1));
	}
}

template <typename T>
template <bool tUseDistortionParameters>
inline VectorT3<T> FisheyeCameraT<T>::vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	ocean_assert(isValid());

	const VectorT2<T> distortedNormalized((distortedImagePoint.x() - principalPointX_) * invFocalLengthX_, (distortedImagePoint.y() - principalPointY_) * invFocalLengthY_);
	const VectorT2<T> undistortedNormalized = undistortNormalized<tUseDistortionParameters>(distortedNormalized);

	if (makeUnitVector)
	{
		return VectorT3<T>(undistortedNormalized.x(), undistortedNormalized.y(), T(1)).normalized();
	}
	else
	{
		return VectorT3<T>(undistortedNormalized.x(), undistortedNormalized.y(), T(1));
	}
}

template <typename T>
template <bool tUseDistortionParameters>
inline LineT3<T> FisheyeCameraT<T>::ray(const VectorT2<T>& distortedImagePoint, const HomogenousMatrixT4<T>& world_T_camera) const
{
	ocean_assert(isValid() && world_T_camera.isValid());

	return LineT3<T>(world_T_camera.translation(), world_T_camera.rotationMatrix(vector<tUseDistortionParameters>(distortedImagePoint)));
}

template <typename T>
template <bool tUseDistortionParameters>
inline LineT3<T> FisheyeCameraT<T>::ray(const VectorT2<T>& distortedImagePoint) const
{
	ocean_assert(isValid());

	return LineT3<T>(Vector3(0, 0, 0), vector<tUseDistortionParameters>(distortedImagePoint));
}

template <typename T>
template <bool tUseDistortionParameters>
inline void FisheyeCameraT<T>::pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const
{
	ocean_assert(isValid());
	ocean_assert(jx != nullptr && jy != nullptr);

	const T fx = focalLengthX();
	const T fy = focalLengthY();

	const T u = flippedCameraObjectPoint.x();
	const T v = flippedCameraObjectPoint.y();
	const T w = flippedCameraObjectPoint.z();

	ocean_assert(NumericT<T>::isNotEqualEps(w));
	const T invW = T(1) / w;

	const T u_invW = u * invW;
	const T v_invW = v * invW;

	if constexpr (tUseDistortionParameters)
	{
		T jDistX[2];
		T jDistY[2];

		jacobianDistortNormalized2x2(u_invW, v_invW, radialDistortion_, tangentialDistortion_, jDistX, jDistY);

		const T fx_jDistXx_invW = fx * jDistX[0] * invW;
		const T fy_jDistYx_invW = fy * jDistY[0] * invW;

		const T fx_jDistXy_invW = fx * jDistX[1] * invW;
		const T fy_jDistYy_invW = fy * jDistY[1] * invW;

		const T u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
		const T u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

		jx[0] = fx_jDistXx_invW;
		jx[1] = fx_jDistXy_invW;
		jx[2] = -u_fx_jDistXx__;

		jy[0] = fy_jDistYx_invW;
		jy[1] = fy_jDistYy_invW;
		jy[2] = -u_fy_jDistYx__;
	}
	else
	{
		const T fx_jDistXx_invW = fx * invW;
		const T fy_jDistYx_invW = fy * invW;

		const T fx_jDistXy_invW = fx * invW;
		const T fy_jDistYy_invW = fy * invW;

		const T u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
		const T u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

		jx[0] = fx_jDistXx_invW;
		jx[1] = fx_jDistXy_invW;
		jx[2] = -u_fx_jDistXx__;

		jy[0] = fy_jDistYx_invW;
		jy[1] = fy_jDistYy_invW;
		jy[2] = -u_fy_jDistYx__;
	}
}

template <typename T>
bool FisheyeCameraT<T>::isEqual(const FisheyeCameraT<T>& fisheyeCamera, const T eps) const
{
	return cameraWidth_ == fisheyeCamera.cameraWidth_ && cameraHeight_ == fisheyeCamera.cameraHeight_ && hasDistortionParameters_ == fisheyeCamera.hasDistortionParameters_
				&& NumericT<T>::isEqual(focalLengthX_, fisheyeCamera.focalLengthX_, eps) && NumericT<T>::isEqual(focalLengthY_, fisheyeCamera.focalLengthY_, eps)
				&& NumericT<T>::isEqual(principalPointX_, fisheyeCamera.principalPointX_, eps) && NumericT<T>::isEqual(principalPointY_, fisheyeCamera.principalPointY_, eps)
				&& NumericT<T>::isEqual(radialDistortion_[0], fisheyeCamera.radialDistortion_[0], eps) && NumericT<T>::isEqual(radialDistortion_[1], fisheyeCamera.radialDistortion_[1], eps)
				&& NumericT<T>::isEqual(radialDistortion_[2], fisheyeCamera.radialDistortion_[2], eps) && NumericT<T>::isEqual(radialDistortion_[3], fisheyeCamera.radialDistortion_[3], eps)
				&& NumericT<T>::isEqual(radialDistortion_[4], fisheyeCamera.radialDistortion_[4], eps) && NumericT<T>::isEqual(radialDistortion_[5], fisheyeCamera.radialDistortion_[5], eps)
				&& NumericT<T>::isEqual(tangentialDistortion_[0], fisheyeCamera.tangentialDistortion_[0], eps) && NumericT<T>::isEqual(tangentialDistortion_[1], fisheyeCamera.tangentialDistortion_[1], eps);
}

template <typename T>
inline bool FisheyeCameraT<T>::isValid() const
{
	ocean_assert(NumericT<T>::isEqualEps(focalLengthX_) || NumericT<T>::isEqual(T(1) / focalLengthX_, invFocalLengthX_));
	ocean_assert(NumericT<T>::isEqualEps(focalLengthY_) || NumericT<T>::isEqual(T(1) / focalLengthY_, invFocalLengthY_));

	return cameraWidth_ != 0u && cameraHeight_ != 0u;
}

template <typename T>
bool FisheyeCameraT<T>::operator==(const FisheyeCameraT<T>& fisheyeCamera) const
{
	return cameraWidth_ == fisheyeCamera.cameraWidth_ && cameraHeight_ == fisheyeCamera.cameraHeight_
				&& focalLengthX_ == fisheyeCamera.focalLengthX_ && focalLengthY_ == fisheyeCamera.focalLengthY_
				&& invFocalLengthX_ == fisheyeCamera.invFocalLengthX_ && invFocalLengthY_ == fisheyeCamera.invFocalLengthY_
				&& principalPointX_ == fisheyeCamera.principalPointX_ && principalPointY_ == fisheyeCamera.principalPointY_
				&& hasDistortionParameters_ == fisheyeCamera.hasDistortionParameters_
				&& memcmp(radialDistortion_, fisheyeCamera.radialDistortion_, sizeof(T) * 6) == 0
				&& memcmp(tangentialDistortion_, fisheyeCamera.tangentialDistortion_, sizeof(T) * 2) == 0;
}

template <typename T>
inline bool FisheyeCameraT<T>::operator!=(const FisheyeCameraT<T>& fisheyeCamera) const
{
	return !(*this == fisheyeCamera);
}

template <typename T>
inline FisheyeCameraT<T>::operator bool() const
{
	return isValid();
}

template <typename T>
VectorT2<T> FisheyeCameraT<T>::tangentialFreeDistortion(const VectorT2<T>& distortedNormalized) const
{
	// x' = x_r + x_t
	// y' = y_r + y_t

	// x_t = p1 * (2 * x_r^2 + radial^2) + p2 * 2 * x_r * y_r
	// y_t = p2 * (2 * y_r^2 + radial^2) + p1 * 2 * x_r * y_r

	// newton-based solving for x_r, y_r:
	// x' = p1 * (2 * x_r^2 + radial^2) + p2 * 2 * x_r * y_r + x_r
	// y' = p2 * (2 * y_r^2 + radial^2) + p1 * 2 * x_r * y_r + y_r

	const T& p1 = tangentialDistortion_[0];
	const T& p2 = tangentialDistortion_[1];

	if (NumericT<T>::isEqualEps(p1) && NumericT<T>::isEqualEps(p2))
	{
		return distortedNormalized;
	}

	VectorT2<T> distortedTangentialFree(distortedNormalized);

	for (unsigned int n = 0u; n < 2u; ++n)
	{
		const T& x_r = distortedTangentialFree.x();
		const T& y_r = distortedTangentialFree.y();

		const T resultX = p1 * T(3) * x_r * x_r + p1 * y_r * y_r + T(2) * p2 * x_r * y_r + x_r - distortedNormalized.x();
		const T resultY = p2 * T(3) * y_r * y_r + p2 * x_r * x_r + T(2) * p1 * x_r * y_r + y_r - distortedNormalized.y();

		const T dxx = p1 * T(6) * x_r + T(2) * p2 * y_r + T(1);
		const T dxy = p1 * T(2) * y_r + T(2) * p2 * x_r;

		const T& dyx = dxy; // dxy == p2 * T(2) * y_r + T(2) * p1 * y_r;
		const T dyy = p2 * T(6) * y_r + T(2) * p1 * x_r + T(1);

		VectorT2<T> delta(0, 0);
		SquareMatrixT2<T>(dxx, dyx, dxy, dyy).solve(VectorT2<T>(resultX, resultY), delta);

		distortedTangentialFree -= delta;

		if (delta.sqr() < Numeric::eps())
		{
			break;
		}
	}

	return distortedTangentialFree;
}

template <typename T>
OCEAN_FORCE_INLINE void FisheyeCameraT<T>::jacobianDistortNormalized2x2(const T x, const T y, const T* radialDistortion, const T* tangentialDistortion, T* jx, T* jy)
{
	ocean_assert(jx != nullptr && jy != nullptr);
	ocean_assert(radialDistortion != nullptr && tangentialDistortion != nullptr);

	const T& k3 = radialDistortion[0];
	const T& k5 = radialDistortion[1];
	const T& k7 = radialDistortion[2];
	const T& k9 = radialDistortion[3];
	const T& k11 = radialDistortion[4];
	const T& k13 = radialDistortion[5];

	const T& p1 = tangentialDistortion[0];
	const T& p2 = tangentialDistortion[1];

	const T x2 = x * x;
	const T y2 = y * y;

	const T xy2 = x2 + y2;

	const T r = NumericT<T>::sqrt(xy2);
	const T r3 = r * r * r;

	const T t = NumericT<T>::atan(r);
	const T t2 = t * t;
	const T t3 = t2 * t;
	const T t4 = t3 * t;
	const T t5 = t4 * t;
	const T t6 = t5 * t;
	const T t7 = t6 * t;
	const T t8 = t7 * t;
	const T t9 = t8 * t;
	const T t10 = t9 * t;
	const T t11 = t10 * t;
	const T t12 = t11 * t;
	const T t13 = t12 * t;

	const T term0 = k13 * t13 + k11 * t11 + k9 * t9 + k7 * t7 + k5 * t5 + k3 * t3 + t;
	const T term1 = 13 * k13 * t12 + 11 * k11 * t10 + 9 * k9 * t8 + 7 * k7 * t6 + 5 * k5 * t4 + 3 * k3 * t2 + 1;

	const T term2 = (xy2 + 1) * term0;
	const T term3 = r3 * (xy2 + 1);
	const T invTerm3 = T(1) / term3;

	const T xDistortion_dx = (xy2 * term2 - x2 * term2 + x2 * r * term1) * invTerm3;
	const T xDistortion_dy = (x * term1 * y) / (xy2 * (xy2 + 1)) - (x * y * term0) / r3;

	//const T yDistortion_dx = (y * term1 * x) / (xy2 * (xy2 + 1)) - (y * x * term0) / r3; == xDistortion_dy
	const T& yDistortion_dx = xDistortion_dy;
	const T yDistortion_dy = (xy2 * term2 - y2 * term2 + y2 * r * term1) * invTerm3;

	const T radialDistortionFactor = term0 / r;

	const T rx = x * radialDistortionFactor;
	const T ry = y * radialDistortionFactor;

	const T xTangential_dx = 6 * p1 * rx + 2 * p2 * ry + 1;
	const T xTangential_dy = 2 * p1 * ry + 2 * p2 * rx;

	// const T yTangential_dx = 2 * p2 * rx + 2 * p1 * ry; // == yTangential_dx
	const T& yTangential_dx = xTangential_dy;
	const T yTangential_dy = 6 * p2 * ry + 2 * p1 * rx + 1;

	// chain rule
	// | xTangential_dx  xTangential_dy |   | xDistortion_dx  xDistortion_dy |
	// | yTangential_dx  yTangential_dy | * | yDistortion_dx  yDistortion_dy |

	jx[0] = xTangential_dx * xDistortion_dx + xTangential_dy * yDistortion_dx;
	jx[1] = xTangential_dx * xDistortion_dy + xTangential_dy * yDistortion_dy;

	jy[0] = yTangential_dx * xDistortion_dx + yTangential_dy * yDistortion_dx;
	jy[1] = yTangential_dx * xDistortion_dy + yTangential_dy * yDistortion_dy;
}

}

#endif // META_OCEAN_MATH_FISHEYE_CAMERA_H
