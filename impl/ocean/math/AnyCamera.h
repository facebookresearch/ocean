/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_ANY_CAMERA_H
#define META_OCEAN_MATH_ANY_CAMERA_H

#include "ocean/math/Math.h"
#include "ocean/math/Camera.h"
#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class AnyCameraT;

/**
 * Definition of an AnyCamera object with Scalar precision.
 * @see AnyCameraT
 * @ingroup math
 */
using AnyCamera = AnyCameraT<Scalar>;

/**
 * Definition of an AnyCamera object with double precision.
 * @see AnyCameraT
 * @ingroup math
 */
using AnyCameraD = AnyCameraT<double>;

/**
 * Definition of an AnyCamera object with float precision.
 * @see AnyCameraT
 * @ingroup math
 */
using AnyCameraF = AnyCameraT<float>;

/**
 * Definition of a shared pointer holding an AnyCamera object with Scalar precision.
 * @tparam T the data type of the scalar to be used either 'float' or 'double'
 * @see AnyCameraT
 * @ingroup math
 */
template <typename T>
using SharedAnyCameraT = std::shared_ptr<AnyCameraT<T>>;

/**
 * Definition of a shared pointer holding an AnyCamera object with Scalar precision.
 * @see AnyCameraT
 * @ingroup math
 */
using SharedAnyCamera = std::shared_ptr<AnyCamera>;

/**
 * Definition of a shared pointer holding an AnyCamera object with double precision.
 * @see AnyCameraT
 * @ingroup math
 */
using SharedAnyCameraD = std::shared_ptr<AnyCameraD>;

/**
 * Definition of a shared pointer holding an AnyCamera object with float precision.
 * @see AnyCameraT
 * @ingroup math
 */
using SharedAnyCameraF = std::shared_ptr<AnyCameraF>;

/**
 * Definition of a typename alias for vectors with shared AnyCameraT objects.
 * @tparam T the data type of the scalar to be used either 'float' or 'double'
 * @see VectorT2
 * @ingroup math
 */
template <typename T>
using SharedAnyCamerasT = std::vector<std::shared_ptr<AnyCameraT<T>>>;

/**
 * Definition of a vector holding AnyCamera objects.
 * @see AnyCamera
 * @ingroup math
 */
using SharedAnyCameras = SharedAnyCamerasT<Scalar>;

/**
 * Definition of a vector holding AnyCameraD objects.
 * @see AnyCameraD
 * @ingroup math
 */
using SharedAnyCamerasD = SharedAnyCamerasT<double>;

/**
 * Definition of a vector holding AnyCameraF objects.
 * @see AnyCameraF
 * @ingroup math
 */
using SharedAnyCamerasF = SharedAnyCamerasT<float>;

/**
 * Definition of individual camera types.
 * @ingroup math
 */
enum class AnyCameraType : uint32_t
{
	/// An invalid camera type.
	INVALID = 0u,
	/// A pinhole camera.
	PINHOLE,
	/// A fisheye camera.
	FISHEYE
};

/**
 * This class implements the abstract base class for all AnyCamera objects.
 * A custom camera object can be implemented by
 * - simply deriving a new class from this base class
 * - using the helper class AnyCameraWrappingT which helps reducing the implementation effort
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class AnyCameraT : public CameraT<T>
{
	public:

		/// The scalar data type of this object.
		typedef T TScalar;

	public:

		/**
		 * Destructs the AnyCamera object.
		 */
		virtual ~AnyCameraT() = default;

		/**
		 * Returns the type of this camera.
		 * @return The camera's type
		 */
		virtual AnyCameraType anyCameraType() const = 0;

		/**
		 * Returns the name of this camera.
		 * @return The camera's name
		 */
		virtual std::string name() const = 0;

		/**
		 * Returns a copy of this camera object.
		 * The image resolution of the cloned camera must have the same aspect ratio as the current image resolution.
		 * @param width The width of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @param height the height of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @return New instance of this camera object
		 */
		virtual std::unique_ptr<AnyCameraT<T>> clone(const unsigned int width = 0u, const unsigned int height = 0u) const = 0;

		/**
		 * Returns a copy of this camera object with float precision.
		 * The image resolution of the cloned camera must have the same aspect ratio as the current image resolution.
		 * @param width The width of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @param height the height of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @return New instance of this camera object
		 */
		virtual std::unique_ptr<AnyCameraT<float>> cloneToFloat(const unsigned int width = 0u, const unsigned int height = 0u) const = 0;

		/**
		 * Returns a copy of this camera object with double precision.
		 * The image resolution of the cloned camera must have the same aspect ratio as the current image resolution.
		 * @param width The width of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @param height the height of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @return New instance of this camera object
		 */
		virtual std::unique_ptr<AnyCameraT<double>> cloneToDouble(const unsigned int width = 0u, const unsigned int height = 0u) const = 0;

		/**
		 * Returns the width of the camera image.
		 * @return Width of the camera image, in pixel, with range [0, infinity)
		 */
		virtual unsigned int width() const = 0;

		/**
		 * Returns the height of the camera image.
		 * @return Height of the camera image, in pixel, with range [0, infinity)
		 */
		virtual unsigned int height() const = 0;

		/**
		 * Returns the coordinate of the principal point of the camera image in the pixel domain.
		 * @return The 2D location of the principal point, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		virtual VectorT2<T> principalPoint() const = 0;

		/**
		 * Returns the x-value of the principal point of the camera image in the pixel domain.
		 * @return x-value of the principal point, with range (-infinity, infinity)
		 */
		virtual T principalPointX() const = 0;

		/**
		 * Returns the y-value of the principal point of the camera image in the pixel domain.
		 * @return y-value of the principal point, with range (-infinity, infinity)
		 */
		virtual T principalPointY() const = 0;

		/**
		 * Returns the horizontal focal length parameter.
		 * @return Horizontal focal length parameter in pixel domain, with range (0, infinity)
		 */
		virtual T focalLengthX() const = 0;

		/**
		 * Returns the vertical focal length parameter.
		 * @return Vertical focal length parameter in pixel domain, with range (0, infinity)
		 */
		virtual T focalLengthY() const = 0;

		/**
		 * Returns the inverse horizontal focal length parameter.
		 * @return Inverse horizontal focal length parameter in pixel domain, with range (0, infinity)
		 */
		virtual T inverseFocalLengthX() const = 0;

		/**
		 * Returns the inverse vertical focal length parameter.
		 * @return Inverse vertical focal length parameter in pixel domain, with range (0, infinity)
		 */
		virtual T inverseFocalLengthY() const = 0;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the left and right part of the camera.
		 * @return Field of view (in radian), with range (0, 2 * PI]
		 */
		virtual T fovX() const = 0;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the top and bottom part of the camera.
		 * @return Field of view (in radian), with range (0, 2 * PI]
		 */
		virtual T fovY() const = 0;

		/**
		 * Returns whether a given 2D image point lies inside the camera frame.
		 * Optional an explicit border can be defined to allow points slightly outside the camera image, or further inside the image.<br>
		 * Defined a negative border size to allow image points outside the camera frame, or a positive border size to prevent points within the camera frame but close to the boundary.
		 * @param imagePoint Image point to be checked, must be valid
		 * @param signedBorder The optional border increasing or decreasing the rectangle in which the image point must be located, in pixels, with range (-infinity, std::min(width() / 2, height() / 2)
		 * @return True, if the image point lies in the ranges [0, width())x[0, height())
		 */
		virtual bool isInside(const VectorT2<T>& imagePoint, const T signedBorder = T(0)) const = 0;

		/**
		 * Projects a 3D object point into the camera frame.
		 * The projection is applied with a default camera pose, the camera is looking into the negative z-space with y-axis up.
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		virtual VectorT2<T> projectToImage(const VectorT3<T>& objectPoint) const = 0;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @param world_T_camera The camera pose, the default camera is looking into the negative z-space with y-axis up, transforming camera to world, must be valid
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		virtual VectorT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& objectPoint) const = 0;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * The projection is applied with a default camera pose, the camera is looking into the negative z-space with y-axis up.
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		virtual void projectToImage(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const = 0;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @param world_T_camera The camera pose, the default camera is looking into the negative z-space with y-axis up, transforming camera to world, must be valid
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		virtual void projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const = 0;

		/**
		 * Projects a 3D object point into the camera frame.
		 * The projection is applied with a default (inverted) and flipped camera pose, the camera is looking into the positive z-space with y-axis down.
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		virtual VectorT2<T> projectToImageIF(const VectorT3<T>& objectPoint) const = 0;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @param flippedCamera_T_world The inverted and flipped camera pose, the default camera is looking into the positive z-space with y-axis down, transforming world to flipped camera, must be valid
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		virtual VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const = 0;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * The projection is applied with a default (inverted) and flipped camera pose, the camera is looking into the positive z-space with y-axis down.
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		virtual void projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const = 0;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @param flippedCamera_T_world The inverted and flipped camera pose, the default camera is looking into the positive z-space with y-axis down, transforming world to flipped camera, must be valid
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		virtual void projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const = 0;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * The vector is determined for a default camera looking into the negative z-space with y-axis up.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return Vector pointing into the negative z-space
		 * @see vectorIF(), ray().
		 */
		virtual VectorT3<T> vector(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector = true) const = 0;

		/**
		 * Determines vectors starting at the camera's center and intersecting given 2D points in the image.
		 * The vectors are determined for a default camera looking into the negative z-space with y-axis up.
		 * @param distortedImagePoints 2D (distorted) positions within the image, with range [0, width())x[0, height()), must be valid
		 * @param size The number of provided points, with range [1, infinity)
		 * @param vectors The resulting vectors pointing into the negative z-space, one for each image point, must be valid
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @see vectorIF(), ray().
		 */
		virtual void vector(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector = true) const = 0;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * The vector is determined for a default camera looking into the positive z-space with y-axis down.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return Vector pointing into the positive z-space
		 */
		virtual VectorT3<T> vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector = true) const = 0;

		/**
		 * Returns vectors starting at the camera's center and intersecting a given 2D points in the image.
		 * The vectors are determined for a default camera looking into the positive z-space with y-axis down.
		 * @param distortedImagePoints 2D (distorted) positions within the image, with range [0, width())x[0, height()), must be valid
		 * @param size The number of provided points, with range [1, infinity)
		 * @param vectors The resulting vectors pointing into the positive z-space, one for each image point, must be valid
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 */
		virtual void vectorIF(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector = true) const = 0;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * The ray is determined for a default camera looking into the negative z-space with y-axis up.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param world_T_camera The pose of the camera, the extrinsic camera matrix, must be valid
		 * @return The specified ray with direction pointing into the camera's negative z-space
		 * @see vector().
		 */
		virtual LineT3<T> ray(const VectorT2<T>& distortedImagePoint, const HomogenousMatrixT4<T>& world_T_camera) const = 0;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * The ray is determined for a default camera looking into the negative z-space with y-axis up.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @return The specified ray with direction pointing into the camera's negative z-space
		 * @see vector().
		 */
		virtual LineT3<T> ray(const VectorT2<T>& distortedImagePoint) const = 0;

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
		 * @see pointJacobian2nx3IF().
		 */
		virtual void pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const = 0;

		/**
		 * Calculates the 2n x 3 jacobian matrix for the 3D object point projection into the camera frame.
		 * The resulting jacobian matrix has the following layout:
		 * <pre>
		 * | dfu / dx, dfu / dy, dfu / dz | <- for object point 0
		 * | dfv / dx, dfv / dy, dfv / dz |
		 * |           ...                |
		 * | dfu / dx, dfu / dy, dfu / dz | <- for object point n - 1
		 * | dfv / dx, dfv / dy, dfv / dz |
		 * with projection function
		 * q = f(p)
		 * q_u = fu(p), q_y = fv(p)
		 * with 2D image point q = (q_u, q_v) and 3D object point p = (x, y, z)
		 * </pre>
		 * @param flippedCameraObjectPoints The 3D object points defined in relation to the inverted and flipped camera pose (camera looking into the positive z-space with y-axis pointing down).
		 * @param numberObjectPoints The number of given 3D object points, with range [1, infinity)
		 * @param jacobians The resulting 2n x 3 Jacobian matrix, with 2 * numberObjectPoints * 3 elements, must be valid
		 * @see pointJacobian2x3IF().
		 */
		virtual void pointJacobian2nx3IF(const VectorT3<T>* flippedCameraObjectPoints, const size_t numberObjectPoints, T* jacobians) const = 0;

		/**
		 * Returns whether two camera objects are identical up to a given epsilon.
		 * The image resolution must always be identical.
		 * @param anyCamera The second camera to be used for comparison, can be invalid
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		virtual bool isEqual(const AnyCameraT<T>& anyCamera, const T eps = NumericT<T>::eps()) const = 0;

		/**
		 * Returns whether this camera is valid.
		 * @return True, if so
		 */
		virtual bool isValid() const = 0;

		/**
		 * Converts an AnyCamera object with arbitrary scalar type to another AnyCamera object with arbitrary scalar type.
		 * In case both scalar types are identical, the object is simply returned.
		 * In case both scalar types are different, a clone is returned.
		 * @param anyCamera The AnyCamera object to be converted, can be nullptr
		 * @return The resulting AnyCamera object
		 * @tparam U The scalar data type of the given AnyCamera object
		 */
		template <typename U>
		static std::shared_ptr<AnyCameraT<T>> convert(const std::shared_ptr<AnyCameraT<U>>& anyCamera);

	protected:

		/**
		 * Protected default constructor.
		 */
		AnyCameraT() = default;

		/**
		 * Protected copy constructor.
		 * @param anyCamera The object to copy
		 */
		AnyCameraT(const AnyCameraT<T>& anyCamera) = default;

		/**
		 * Disabled move constructor.
		 */
		AnyCameraT(AnyCameraT<T>&&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		AnyCameraT& operator=(const AnyCameraT&) = delete;

		/**
		 * Disabled move operator.
		 * @return Reference to this object
		 */
		AnyCameraT& operator=(AnyCameraT&&) = delete;
};

/**
 * This class implements a specialized AnyCamera object wrapping the actual camera model.
 * The class is a helper class to simplify the implementation of specialized AnyCamera objects.
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @tparam TCameraWrapper The data type of the class actually wrapping the camera object
 * @ingroup math
 */
template <typename T, typename TCameraWrapper>
class AnyCameraWrappingT final :
	public AnyCameraT<T>,
	public TCameraWrapper
{
	public:

		/// The scalar data type of this object.
		typedef T TScalar;

		/// The class which is actually wrapping the camera object.
		typedef TCameraWrapper CameraWrapper;

		/// The actual camera object wrapped by this class.
		typedef typename TCameraWrapper::ActualCamera ActualCamera;

	public:

		/**
		 * Creates a new AnyCamera object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit AnyCameraWrappingT(ActualCamera&& actualCamera);

		/**
		 * Creates a new AnyCamera object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit AnyCameraWrappingT(const ActualCamera& actualCamera);

		/**
		 * Returns the type of this camera.
		 * @return The camera's type
		 */
		AnyCameraType anyCameraType() const override;

		/**
		 * Returns the name of this camera.
		 * @return The camera's name
		 */
		std::string name() const override;

		/**
		 * Returns a copy of this camera object.
		 * The image resolution of the cloned camera must have the same aspect ratio as the current image resolution.
		 * @param width The width of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @param height the height of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @return New instance of this camera object
		 */
		std::unique_ptr<AnyCameraT<T>> clone(const unsigned int width = 0u, const unsigned int height = 0u) const override;

		/**
		 * Returns a copy of this camera object with float precision.
		 * The image resolution of the cloned camera must have the same aspect ratio as the current image resolution.
		 * @param width The width of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @param height the height of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @return New instance of this camera object
		 */
		std::unique_ptr<AnyCameraT<float>> cloneToFloat(const unsigned int width = 0u, const unsigned int height = 0u) const override;

		/**
		 * Returns a copy of this camera object with double precision.
		 * The image resolution of the cloned camera must have the same aspect ratio as the current image resolution.
		 * @param width The width of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @param height the height of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @return New instance of this camera object
		 */
		std::unique_ptr<AnyCameraT<double>> cloneToDouble(const unsigned int width = 0u, const unsigned int height = 0u) const override;

		/**
		 * Returns the width of the camera image.
		 * @return Width of the camera image, in pixel, with range [0, infinity)
		 */
		unsigned int width() const override;

		/**
		 * Returns the height of the camera image.
		 * @return Height of the camera image, in pixel, with range [0, infinity)
		 */
		unsigned int height() const override;

		/**
		 * Returns the coordinate of the principal point of the camera image in the pixel domain.
		 * @return The 2D location of the principal point, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		VectorT2<T> principalPoint() const override;

		/**
		 * Returns the x-value of the principal point of the camera image in the pixel domain.
		 * @return x-value of the principal point, with range (-infinity, infinity)
		 */
		T principalPointX() const override;

		/**
		 * Returns the y-value of the principal point of the camera image in the pixel domain.
		 * @return y-value of the principal point, with range (-infinity, infinity)
		 */
		T principalPointY() const override;

		/**
		 * Returns the horizontal focal length parameter.
		 * @return Horizontal focal length parameter in pixel domain, with range (0, infinity)
		 */
		T focalLengthX() const override;

		/**
		 * Returns the vertical focal length parameter.
		 * @return Vertical focal length parameter in pixel domain, with range (0, infinity)
		 */
		T focalLengthY() const override;

		/**
		 * Returns the inverse horizontal focal length parameter.
		 * @return Inverse horizontal focal length parameter in pixel domain, with range (0, infinity)
		 */
		T inverseFocalLengthX() const override;

		/**
		 * Returns the inverse vertical focal length parameter.
		 * @return Inverse vertical focal length parameter in pixel domain, with range (0, infinity)
		 */
		T inverseFocalLengthY() const override;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the left and right part of the camera.
		 * @return Field of view (in radian), with range (0, 2 * PI]
		 */
		T fovX() const override;

		/**
		 * Returns the field of view in x direction of the camera.
		 * The fov is the sum of the top and bottom part of the camera.
		 * @return Field of view (in radian), with range (0, 2 * PI]
		 */
		T fovY() const override;

		/**
		 * Returns whether a given 2D image point lies inside the camera frame.
		 * Optional an explicit border can be defined to allow points slightly outside the camera image, or further inside the image.<br>
		 * Defined a negative border size to allow image points outside the camera frame, or a positive border size to prevent points within the camera frame but close to the boundary.
		 * @param imagePoint Image point to be checked, must be valid
		 * @param signedBorder The optional border increasing or decreasing the rectangle in which the image point must be located, in pixels, with range (-infinity, std::min(width() / 2, height() / 2)
		 * @return True, if the image point lies in the ranges [0, width())x[0, height())
		 */
		bool isInside(const VectorT2<T>& imagePoint, const T signedBorder = T(0)) const override;

		/**
		 * Projects a 3D object point into the camera frame.
		 * The projection is applied with a default camera pose, the camera is looking into the negative z-space with y-axis up.
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		VectorT2<T> projectToImage(const VectorT3<T>& objectPoint) const override;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @param world_T_camera The camera pose, the default camera is looking into the negative z-space with y-axis up, transforming camera to world, must be valid
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		VectorT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& objectPoint) const override;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * The projection is applied with a default camera pose, the camera is looking into the negative z-space with y-axis up.
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		void projectToImage(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const override;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @param world_T_camera The camera pose, the default camera is looking into the negative z-space with y-axis up, transforming camera to world, must be valid
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		void projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const override;

		/**
		 * Projects a 3D object point into the camera frame.
		 * The projection is applied with a default (inverted) and flipped camera pose, the camera is looking into the positive z-space with y-axis down.
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		VectorT2<T> projectToImageIF(const VectorT3<T>& objectPoint) const override;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @param flippedCamera_T_world The inverted and flipped camera pose, the default camera is looking into the positive z-space with y-axis down, transforming world to flipped camera, must be valid
		 * @param objectPoint The 3D object point to project, defined in world
		 * @return The projected 2D image point
		 */
		VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const override;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * The projection is applied with a default (inverted) and flipped camera pose, the camera is looking into the positive z-space with y-axis down.
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		void projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const override;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @param flippedCamera_T_world The inverted and flipped camera pose, the default camera is looking into the positive z-space with y-axis down, transforming world to flipped camera, must be valid
		 * @param objectPoints The 3D object points to project, defined in world, must be valid
		 * @param size The number of object points, with range [1, infinity)
		 * @param imagePoints The resulting 2D image points, must be valid
		 */
		void projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const override;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * The vector is determined for the default camera looking into the negative z-space with y-axis up.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return Vector pointing into the negative z-space
		 * @see vectorIF(), ray().
		 */
		VectorT3<T> vector(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector = true) const override;

		/**
		 * Determines vectors starting at the camera's center and intersecting given 2D points in the image.
		 * The vectors are determined for a default camera looking into the negative z-space with y-axis up.
		 * @param distortedImagePoints 2D (distorted) positions within the image, with range [0, width())x[0, height()), must be valid
		 * @param size The number of provided points, with range [1, infinity)
		 * @param vectors The resulting vectors pointing into the negative z-space, one for each image point, must be valid
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @see vectorIF(), ray().
		 */
		void vector(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector = true) const override;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * The vector is determined for the default camera looking into the positive z-space with y-axis down.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 * @return Vector pointing into the positive z-space
		 */
		VectorT3<T> vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector = true) const override;

		/**
		 * Returns vectors starting at the camera's center and intersecting a given 2D points in the image.
		 * The vectors are determined for a default camera looking into the positive z-space with y-axis down.
		 * @param distortedImagePoints 2D (distorted) positions within the image, with range [0, width())x[0, height()), must be valid
		 * @param size The number of provided points, with range [1, infinity)
		 * @param vectors The resulting vectors pointing into the positive z-space, one for each image point, must be valid
		 * @param makeUnitVector True, to return a vector with length 1; False, to return a vector with any length
		 */
		void vectorIF(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector = true) const override;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @param world_T_camera The pose of the camera, the extrinsic camera matrix, must be valid
		 * @return The specified ray with direction pointing into the camera's negative z-space
		 * @see vector().
		 */
		LineT3<T> ray(const VectorT2<T>& distortedImagePoint, const HomogenousMatrixT4<T>& world_T_camera) const override;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * @param distortedImagePoint 2D (distorted) position within the image, with range [0, width())x[0, height())
		 * @return The specified ray with direction pointing into the camera's negative z-space
		 * @see vector().
		 */
		LineT3<T> ray(const VectorT2<T>& distortedImagePoint) const override;

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
		 */
		void pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const override;

		/**
		 * Calculates the 2n x 3 jacobian matrix for the 3D object point projection into the camera frame.
		 * The resulting jacobian matrix has the following layout:
		 * <pre>
		 * | dfu / dx, dfu / dy, dfu / dz | <- for object point 0
		 * | dfv / dx, dfv / dy, dfv / dz |
		 * |           ...                |
		 * | dfu / dx, dfu / dy, dfu / dz | <- for object point n - 1
		 * | dfv / dx, dfv / dy, dfv / dz |
		 * with projection function
		 * q = f(p)
		 * q_u = fu(p), q_y = fv(p)
		 * with 2D image point q = (q_u, q_v) and 3D object point p = (x, y, z)
		 * </pre>
		 * @param flippedCameraObjectPoints The 3D object points defined in relation to the inverted and flipped camera pose (camera looking into the positive z-space with y-axis pointing down).
		 * @param numberObjectPoints The number of given 3D object points, with range [1, infinity)
		 * @param jacobians The resulting 2n x 3 Jacobian matrix, with 2 * numberObjectPoints * 3 elements, must be valid
		 */
		void pointJacobian2nx3IF(const VectorT3<T>* flippedCameraObjectPoints, const size_t numberObjectPoints, T* jacobians) const override;

		/**
		 * Returns whether two camera objects are identical up to a given epsilon.
		 * The image resolution must always be identical.
		 * @param anyCamera The second camera to be used for comparison, can be invalid
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isEqual(const AnyCameraT<T>& anyCamera, const T eps = NumericT<T>::eps()) const override;

		/**
		 * Returns whether this camera is valid.
		 * @return True, if so
		 */
		bool isValid() const override;
};

/**
 * This class implements a wrapper for an actual camera object.
 * - TCameraWrapperBase implements the wrapper functions necessary for the individual camera models.
 * - CameraWrapperT implements some additional functions necessary to fully implement all necessary functions for AnyCameraT.
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @tparam TCameraWrapperBase The base class implementing all functions necessary for the wrapped camera object.
 * @ingroup math
 */
template <typename T, typename TCameraWrapperBase>
class CameraWrapperT : public TCameraWrapperBase
{
	public:

		/**
		 * Definition of the actual camera object which is wrapped in this class.
		 */
		using typename TCameraWrapperBase::ActualCamera;

	public:

		/**
		 * Creates a new CameraWrapperT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperT(ActualCamera&& actualCamera);

		/**
		 * Creates a new CameraWrapperT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperT(const ActualCamera& actualCamera);

		/**
		 * Returns the coordinate of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPoint().
		 */
		inline VectorT2<T> principalPoint() const;

		/**
		 * Returns the field of view in x direction of the camera.
		 * @see AnyCameraT::fovX().
		 */
		inline T fovX() const;

		/**
		 * Returns the field of view in x direction of the camera.
		 * @see AnyCameraT::fovY().
		 */
		inline T fovY() const;

		/**
		 * Returns whether a given 2D image point lies inside the camera frame.
		 * @see AnyCameraT::isInside().
		 */
		inline bool isInside(const VectorT2<T>& imagePoint, const T signedBorder = T(0)) const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see projectToImage().
		 */
		inline VectorT2<T> projectToImage(const VectorT3<T>& objectPoint) const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see projectToImage().
		 */
		inline VectorT2<T> projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& objectPoint) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see projectToImage().
		 */
		inline void projectToImage(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see projectToImage().
		 */
		inline void projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * @see AnyCameraT::vector().
		 */
		inline VectorT3<T> vector(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const;

		/**
		 * Determines vectors starting at the camera's center and intersecting given 2D points in the image.
		 * @see AnyCameraT::vector().
		 */
		inline void vector(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * @see ray().
		 */
		inline LineT3<T> ray(const VectorT2<T>& distortedImagePoint, const HomogenousMatrixT4<T>& world_T_camera) const;

		/**
		 * Returns a ray starting at the camera's center and intersecting a given 2D point in the image.
		 * @see ray().
		 */
		inline LineT3<T> ray(const VectorT2<T>& distortedImagePoint) const;

		/**
		 * Calculates the 2x3 jacobian matrix for the 3D object point projection into the camera frame.
		 * @see AnyCameraT::pointJacobian2nx3IF().
		 */
		inline void pointJacobian2nx3IF(const VectorT3<T>* flippedCameraObjectPoints, const size_t numberObjectPoints, T* jacobians) const;
};

/**
 * This class implements the base wrapper around Ocean's pinhole camera profile.
 * The class can be used as 'TCameraWrapperBase' in 'CameraWrapperT' to create a full wrapper class e.g., 'CameraWrapperT<T, CameraWrapperBasePinholeT<T>>'.
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class CameraWrapperBasePinholeT
{
	public:

		/**
		 * Definition of the actual camera object wrapped by this class.
		 */
		typedef PinholeCameraT<T> ActualCamera;

		/**
		 * Definition of the parent WrappedCamera class using this base class.
		 */
		using WrappedCamera = CameraWrapperT<T, CameraWrapperBasePinholeT<T>>;

	public:

		/**
		 * Creates a new CameraWrapperBasePinholeT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBasePinholeT(ActualCamera&& actualCamera);

		/**
		 * Creates a new CameraWrapperBasePinholeT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBasePinholeT(const ActualCamera& actualCamera);

		/**
		 * Returns the actual camera object wrapped in this class.
		 * @return The wrapped camera object
		 */
		inline const ActualCamera& actualCamera() const;

		/**
		 * Returns the width of the camera image.
		 * @see AnyCameraT::width().
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the camera image.
		 * @see AnyCameraT::height().
		 */
		inline unsigned int height() const;

		/**
		 * Returns the x-value of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPointX().
		 */
		inline T principalPointX() const;

		/**
		 * Returns the y-value of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPointY().
		 */
		inline T principalPointY() const;

		/**
		 * Returns the horizontal focal length parameter.
		 * @see AnyCameraT::focalLengthX().
		 */
		inline T focalLengthX() const;

		/**
		 * Returns the vertical focal length parameter.
		 * @see AnyCameraT::focalLengthY().
		 */
		inline T focalLengthY() const;

		/**
		 * Returns the inverse horizontal focal length parameter.
		 * @see AnyCameraT::inverseFocalLengthX().
		 */
		inline T inverseFocalLengthX() const;

		/**
		 * Returns the inverse vertical focal length parameter.
		 * @see AnyCameraT::inverseFocalLengthY().
		 */
		inline T inverseFocalLengthY() const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline VectorT2<T> projectToImageIF(const VectorT3<T>& objectPoint) const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline void projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline void projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * @see AnyCameraT::vectorIF().
		 */
		inline VectorT3<T> vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const;

		/**
		 * Returns vectors starting at the camera's center and intersecting a given 2D points in the image.
		 * @see AnyCameraT::vectorIF().
		 */
		inline void vectorIF(const VectorT2<T>* distortedImagePoint, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const;

		/**
		 * Calculates the 2x3 jacobian matrix for the 3D object point projection into the camera frame.
		 * @see AnyCameraT::pointJacobian2x3IF().
		 */
		inline void pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const;

		/**
		 * Returns whether two camera objects are identical up to a given epsilon.
		 * @see AnyCameraT::isEqual().
		 */
		inline bool isEqual(const CameraWrapperBasePinholeT<T>& basePinhole, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether this camera is valid.
		 * @see AnyCameraT::isValid().
		 */
		inline bool isValid() const;

		/**
		 * Returns a copy of the actual camera object.
		 * @return New instance of the actual camera object used in this wrapper.
		 * @tparam U The scalar data type of the resulting cloned object, either 'float' or 'double'
		 */
		template <typename U>
		inline std::unique_ptr<AnyCameraT<U>> clone(const unsigned int width = 0u, const unsigned int height = 0u) const;

		/**
		 * Returns the type of this camera.
		 * @see AnyCameraT::anyCameraType().
		 */
		static inline AnyCameraType anyCameraType();

		/**
		 * Returns the name of this camera.
		 * @see AnyCameraT::name().
		 */
		static inline std::string name();

	protected:

		/// The actual pinhole camera.
		ActualCamera actualCamera_;
};

/**
 * This class implements the base wrapper around Ocean's fisheye camera profile.
 * The class can be used as 'TCameraWrapperBase' in 'CameraWrapperT' to create a full wrapper class e.g., 'CameraWrapperT<T, CameraWrapperBaseFisheyeT<T>>'.
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class CameraWrapperBaseFisheyeT
{
	public:

		/**
		 * Definition of the actual camera object wrapped by this class.
		 */
		typedef FisheyeCameraT<T> ActualCamera;

		/**
		 * Definition of the parent WrappedCamera class using this base class.
		 */
		using WrappedCamera = CameraWrapperT<T, CameraWrapperBaseFisheyeT<T>>;

	public:

		/**
		 * Creates a new CameraWrapperBaseFisheyeT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBaseFisheyeT(ActualCamera&& actualCamera);

		/**
		 * Creates a new CameraWrapperBaseFisheyeT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBaseFisheyeT(const ActualCamera& actualCamera);

		/**
		 * Returns the actual camera object wrapped in this class.
		 * @return The wrapped camera object
		 */
		inline const ActualCamera& actualCamera() const;

		/**
		 * Returns the width of the camera image.
		 * @see AnyCameraT::width().
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the camera image.
		 * @see AnyCameraT::height().
		 */
		inline unsigned int height() const;

		/**
		 * Returns the coordinate of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPoint().
		 */
		inline VectorT2<T> principalPoint() const;

		/**
		 * Returns the x-value of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPointX().
		 */
		inline T principalPointX() const;

		/**
		 * Returns the y-value of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPointY().
		 */
		inline T principalPointY() const;

		/**
		 * Returns the horizontal focal length parameter.
		 * @see AnyCameraT::focalLengthX().
		 */
		inline T focalLengthX() const;

		/**
		 * Returns the vertical focal length parameter.
		 * @see AnyCameraT::focalLengthY().
		 */
		inline T focalLengthY() const;

		/**
		 * Returns the inverse horizontal focal length parameter.
		 * @see AnyCameraT::inverseFocalLengthX().
		 */
		inline T inverseFocalLengthX() const;

		/**
		 * Returns the inverse vertical focal length parameter.
		 * @see AnyCameraT::inverseFocalLengthY().
		 */
		inline T inverseFocalLengthY() const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline VectorT2<T> projectToImageIF(const VectorT3<T>& objectPoint) const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline void projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline void projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * @see AnyCameraT::vectorIF().
		 */
		inline VectorT3<T> vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const;

		/**
		 * Returns vectors starting at the camera's center and intersecting a given 2D points in the image.
		 * @see AnyCameraT::vectorIF().
		 */
		inline void vectorIF(const VectorT2<T>* distortedImagePoint, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const;

		/**
		 * Calculates the 2x3 jacobian matrix for the 3D object point projection into the camera frame.
		 * @see AnyCameraT::pointJacobian2x3IF().
		 */
		inline void pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const;

		/**
		 * Returns whether two camera objects are identical up to a given epsilon.
		 * @see AnyCameraT::isEqual().
		 */
		inline bool isEqual(const CameraWrapperBaseFisheyeT<T>& baseFisheye, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether this camera is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns a copy of the actual camera object.
		 * @return New instance of the actual camera object used in this wrapper.
		 */
		template <typename U>
		inline std::unique_ptr<AnyCameraT<U>> clone(const unsigned int width = 0u, const unsigned int height = 0u) const;

		/**
		 * Returns the type of this camera.
		 * @see AnyCamera::anyCameraType().
		 */
		static inline AnyCameraType anyCameraType();

		/**
		 * Returns the name of this camera.
		 * @see AnyCamera::name().
		 */
		static inline std::string name();

	protected:

		/// The actual fisheye camera object.
		ActualCamera actualCamera_;
};

/**
 * This class implements invalid camera profiles, e.g. when no intrinsic information is available.
 * @tparam T The data type of a 'Scalar', 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class InvalidCameraT
{
	public:

		/**
		 * Creates an invalid camera
		 * @param reason The reason why no valid camera is available, must be valid
		 */
		explicit InvalidCameraT(const std::string& reason);

		/**
		 * Returns the reason of this invalid camera
		 * @return The reason
		 */
		const std::string& reason() const;

	protected:

		/// The reason why no valid camera is available.
		std::string reason_;
};

/**
 * Definition of an invalid camera object based with element precision 'Scalar'.
 * @see AnyCameraT, AnyCameraInvalidT.
 * @ingroup math
 */
typedef InvalidCameraT<Scalar> InvalidCamera;

/**
 * Definition of an invalid camera object based with element precision 'double'.
 * @see AnyCameraT, AnyCameraInvalidT.
 * @ingroup math
 */
typedef InvalidCameraT<double> InvalidCameraD;

/**
 * Definition of an invalid camera object based with element precision 'float'.
 * @see AnyCameraT, AnyCameraInvalidT.
 * @ingroup math
 */
typedef InvalidCameraT<float> InvalidCameraF;

/**
 * This class implements the base wrapper around an invalid camera profile.
 * The class can be used as 'TCameraWrapperBase' in 'CameraWrapperT' to create a full wrapper class e.g., 'CameraWrapperT<T, CameraWrapperBaseInvalidT<T>>'.
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class CameraWrapperBaseInvalidT
{
	public:

		/**
		 * Definition of the actual camera object wrapped by this class.
		 */
		typedef InvalidCameraT<T> ActualCamera;

		/**
		 * Definition of the parent WrappedCamera class using this base class.
		 */
		using WrappedCamera = CameraWrapperT<T, CameraWrapperBaseInvalidT<T>>;

	public:

		/**
		 * Creates a new CameraWrapperBaseInvalidT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBaseInvalidT(ActualCamera&& actualCamera);

		/**
		 * Creates a new CameraWrapperBaseInvalidT object wrapping the actual camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBaseInvalidT(const ActualCamera& actualCamera);

		/**
		 * Returns the actual camera object wrapped in this class.
		 * @return The wrapped camera object
		 */
		inline const ActualCamera& actualCamera() const;

		/**
		 * Returns the width of the camera image.
		 * @see AnyCameraT::width().
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the camera image.
		 * @see AnyCameraT::height().
		 */
		inline unsigned int height() const;

		/**
		 * Returns the coordinate of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPoint().
		 */
		inline VectorT2<T> principalPoint() const;

		/**
		 * Returns the x-value of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPointX().
		 */
		inline T principalPointX() const;

		/**
		 * Returns the y-value of the principal point of the camera image in the pixel domain.
		 * @see AnyCameraT::principalPointY().
		 */
		inline T principalPointY() const;

		/**
		 * Returns the horizontal focal length parameter.
		 * @see AnyCameraT::focalLengthX().
		 */
		inline T focalLengthX() const;

		/**
		 * Returns the vertical focal length parameter.
		 * @see AnyCameraT::focalLengthY().
		 */
		inline T focalLengthY() const;

		/**
		 * Returns the inverse horizontal focal length parameter.
		 * @see AnyCameraT::inverseFocalLengthX().
		 */
		inline T inverseFocalLengthX() const;

		/**
		 * Returns the inverse vertical focal length parameter.
		 * @see AnyCameraT::inverseFocalLengthY().
		 */
		inline T inverseFocalLengthY() const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline VectorT2<T> projectToImageIF(const VectorT3<T>& objectPoint) const;

		/**
		 * Projects a 3D object point into the camera frame.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline VectorT2<T> projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline void projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Projects several 3D object points into the camera frame at once.
		 * @see AnyCameraT::projectToImageIF().
		 */
		inline void projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const;

		/**
		 * Returns a vector starting at the camera's center and intersecting a given 2D point in the image.
		 * @see AnyCameraT::vectorIF().
		 */
		inline VectorT3<T> vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const;

		/**
		 * Returns vectors starting at the camera's center and intersecting a given 2D points in the image.
		 * @see AnyCameraT::vectorIF().
		 */
		inline void vectorIF(const VectorT2<T>* distortedImagePoint, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const;

		/**
		 * Calculates the 2x3 jacobian matrix for the 3D object point projection into the camera frame.
		 * @see AnyCameraT::pointJacobian2x3IF().
		 */
		inline void pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const;

		/**
		 * Returns whether two camera objects are identical up to a given epsilon.
		 * @see AnyCameraT::isEqual().
		 */
		inline bool isEqual(const CameraWrapperBaseInvalidT<T>& baseInvalid, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether this camera is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns a copy of the actual camera object.
		 * @return New instance of the actual camera object used in this wrapper.
		 */
		template <typename U>
		inline std::unique_ptr<AnyCameraT<U>> clone(const unsigned int width = 0u, const unsigned int height = 0u) const;

		/**
		 * Returns the type of this camera.
		 * @see AnyCamera::anyCameraType().
		 */
		static inline AnyCameraType anyCameraType();

		/**
		 * Returns the name of this camera.
		 * @see AnyCamera::name().
		 */
		static inline std::string name();

	protected:

		/// The actual invalid camera.
		ActualCamera actualCamera_;
};

/**
 * Definition of an AnyCamera object based on Ocean's pinhole camera class with template parameter to define the element precision.
 * @tparam T The scalar data type
 * @see AnyCameraT, CameraWrapperBasePinholeT, AnyCameraPinhole, AnyCameraPinholeD, AnyCameraPinholeF.
 * @ingroup math
 */
template <typename T>
using AnyCameraPinholeT = AnyCameraWrappingT<T, CameraWrapperT<T, CameraWrapperBasePinholeT<T>>>;

/**
 * Definition of an AnyCamera object based on Ocean's pinhole camera class with element precision 'Scalar'.
 * @see AnyCameraT, AnyCameraPinholeT.
 * @ingroup math
 */
typedef AnyCameraPinholeT<Scalar> AnyCameraPinhole;

/**
 * Definition of an AnyCamera object based on Ocean's pinhole camera class with element precision 'double'.
 * @see AnyCameraT, AnyCameraPinholeT.
 * @ingroup math
 */
typedef AnyCameraPinholeT<double> AnyCameraPinholeD;

/**
 * Definition of an AnyCamera object based on Ocean's pinhole camera class with element precision 'float'.
 * @see AnyCameraT, AnyCameraPinholeT.
 * @ingroup math
 */
typedef AnyCameraPinholeT<float> AnyCameraPinholeF;

/**
 * Definition of an AnyCamera object based on Ocean's fisheye camera class with template parameter to define the element precision.
 * @tparam T The scalar data type
 * @see AnyCameraT, CameraWrapperBaseFisheyeT, AnyCameraFisheye, AnyCameraFisheyeD, AnyCameraFisheyeF.
 * @ingroup math
 */
template <typename T>
using AnyCameraFisheyeT = AnyCameraWrappingT<T, CameraWrapperT<T, CameraWrapperBaseFisheyeT<T>>>;

/**
 * Definition of an AnyCamera object based on Ocean's fisheye camera class with element precision 'Scalar'.
 * @see AnyCameraT, AnyCameraFisheyeT.
 * @ingroup math
 */
typedef AnyCameraFisheyeT<Scalar> AnyCameraFisheye;

/**
 * Definition of an AnyCamera object based on Ocean's fisheye camera class with element precision 'double'.
 * @see AnyCameraT, AnyCameraFisheyeT.
 * @ingroup math
 */
typedef AnyCameraFisheyeT<double> AnyCameraFisheyeD;

/**
 * Definition of an AnyCamera object based on Ocean's fisheye camera class with element precision 'float'.
 * @see AnyCameraT, AnyCameraFisheyeT.
 * @ingroup math
 */
typedef AnyCameraFisheyeT<float> AnyCameraFisheyeF;

/**
 * Definition of an AnyCamera object based on an invalid (by design) camera with template parameter to define the element precision.
 * @tparam T The scalar data type
 * @see AnyCameraT, CameraWrapperBaseFisheyeT, AnyCameraInvalid, AnyCameraInvalidD, AnyCameraInvalidF.
 * @ingroup math
 */
template <typename T>
using AnyCameraInvalidT = AnyCameraWrappingT<T, CameraWrapperT<T, CameraWrapperBaseInvalidT<T>>>;

/**
 * Definition of an AnyCamera object based on an invalid (by design) camera with element precision 'Scalar'.
 * @see AnyCameraT, AnyCameraInvalidT.
 * @ingroup math
 */
typedef AnyCameraInvalidT<Scalar> AnyCameraInvalid;

/**
 * Definition of an AnyCamera object based on an invalid (by design) camera with element precision 'double'.
 * @see AnyCameraT, AnyCameraInvalidT.
 * @ingroup math
 */
typedef AnyCameraInvalidT<double> AnyCameraInvalidD;

/**
 * Definition of an AnyCamera object based on an invalid (by design) camera with element precision 'float'.
 * @see AnyCameraT, AnyCameraInvalidT.
 * @ingroup math
 */
typedef AnyCameraInvalidT<float> AnyCameraInvalidF;

template <>
template <>
inline std::shared_ptr<AnyCameraT<float>> AnyCameraT<float>::convert(const std::shared_ptr<AnyCameraT<double>>& anyCamera)
{
	if (anyCamera)
	{
		return anyCamera->cloneToFloat();
	}

	return nullptr;
}

template <>
template <>
inline std::shared_ptr<AnyCameraT<double>> AnyCameraT<double>::convert(const std::shared_ptr<AnyCameraT<float>>& anyCamera)
{
	if (anyCamera)
	{
		return anyCamera->cloneToDouble();
	}

	return nullptr;
}

template <typename T>
template <typename U>
std::shared_ptr<AnyCameraT<T>> AnyCameraT<T>::convert(const std::shared_ptr<AnyCameraT<U>>& anyCamera)
{
	static_assert(std::is_same<T, U>::value, "Invalid data types!");

	return anyCamera;
}

template <typename T, typename TCameraWrapper>
AnyCameraWrappingT<T, TCameraWrapper>::AnyCameraWrappingT(ActualCamera&& actualCamera) :
	TCameraWrapper(std::move(actualCamera))
{
	// nothing to do here
}

template <typename T, typename TCameraWrapper>
AnyCameraWrappingT<T, TCameraWrapper>::AnyCameraWrappingT(const ActualCamera& actualCamera) :
	TCameraWrapper(actualCamera)
{
	// nothing to do here
}

template <typename T, typename TCameraWrapper>
AnyCameraType AnyCameraWrappingT<T, TCameraWrapper>::anyCameraType() const
{
	return TCameraWrapper::anyCameraType();
}

template <typename T, typename TCameraWrapper>
std::string AnyCameraWrappingT<T, TCameraWrapper>::name() const
{
	return TCameraWrapper::name();
}

template <typename T, typename TCameraWrapper>
std::unique_ptr<AnyCameraT<T>> AnyCameraWrappingT<T, TCameraWrapper>::clone(const unsigned int width, const unsigned int height) const
{
	return TCameraWrapper::template clone<T>(width, height);
}

template <typename T, typename TCameraWrapper>
std::unique_ptr<AnyCameraT<float>> AnyCameraWrappingT<T, TCameraWrapper>::cloneToFloat(const unsigned int width, const unsigned int height) const
{
	return TCameraWrapper::template clone<float>(width, height);
}

template <typename T, typename TCameraWrapper>
std::unique_ptr<AnyCameraT<double>> AnyCameraWrappingT<T, TCameraWrapper>::cloneToDouble(const unsigned int width, const unsigned int height) const
{
	return TCameraWrapper::template clone<double>(width, height);
}

template <typename T, typename TCameraWrapper>
unsigned int AnyCameraWrappingT<T, TCameraWrapper>::width() const
{
	return TCameraWrapper::width();
}

template <typename T, typename TCameraWrapper>
unsigned int AnyCameraWrappingT<T, TCameraWrapper>::height() const
{
	return TCameraWrapper::height();
}

template <typename T, typename TCameraWrapper>
VectorT2<T> AnyCameraWrappingT<T, TCameraWrapper>::principalPoint() const
{
	return TCameraWrapper::principalPoint();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::principalPointX() const
{
	return TCameraWrapper::principalPointX();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::principalPointY() const
{
	return TCameraWrapper::principalPointY();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::focalLengthX() const
{
	return TCameraWrapper::focalLengthX();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::focalLengthY() const
{
	return TCameraWrapper::focalLengthY();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::inverseFocalLengthX() const
{
	return TCameraWrapper::inverseFocalLengthX();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::inverseFocalLengthY() const
{
	return TCameraWrapper::inverseFocalLengthY();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::fovX() const
{
	return TCameraWrapper::fovX();
}

template <typename T, typename TCameraWrapper>
T AnyCameraWrappingT<T, TCameraWrapper>::fovY() const
{
	return TCameraWrapper::fovY();
}

template <typename T, typename TCameraWrapper>
bool AnyCameraWrappingT<T, TCameraWrapper>::isInside(const VectorT2<T>& imagePoint, const T signedBorder) const
{
	return TCameraWrapper::isInside(imagePoint, signedBorder);
}

template <typename T, typename TCameraWrapper>
VectorT2<T> AnyCameraWrappingT<T, TCameraWrapper>::projectToImage(const VectorT3<T>& objectPoint) const
{
	return TCameraWrapper::projectToImage(objectPoint);
}

template <typename T, typename TCameraWrapper>
VectorT2<T> AnyCameraWrappingT<T, TCameraWrapper>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& objectPoint) const
{
	return TCameraWrapper::projectToImage(world_T_camera, objectPoint);
}

template <typename T, typename TCameraWrapper>
VectorT2<T> AnyCameraWrappingT<T, TCameraWrapper>::projectToImageIF(const VectorT3<T>& objectPoint) const
{
	return TCameraWrapper::projectToImageIF(objectPoint);
}

template <typename T, typename TCameraWrapper>
VectorT2<T> AnyCameraWrappingT<T, TCameraWrapper>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const
{
	return TCameraWrapper::projectToImageIF(flippedCamera_T_world, objectPoint);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::projectToImage(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	return TCameraWrapper::projectToImage(objectPoints, size, imagePoints);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	return TCameraWrapper::projectToImage(world_T_camera, objectPoints, size, imagePoints);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	return TCameraWrapper::projectToImageIF(objectPoints, size, imagePoints);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	return TCameraWrapper::projectToImageIF(flippedCamera_T_world, objectPoints, size, imagePoints);
}

template <typename T, typename TCameraWrapper>
VectorT3<T> AnyCameraWrappingT<T, TCameraWrapper>::vector(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	return TCameraWrapper::vector(distortedImagePoint, makeUnitVector);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::vector(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const
{
	return TCameraWrapper::vector(distortedImagePoints, size, vectors, makeUnitVector);
}

template <typename T, typename TCameraWrapper>
VectorT3<T> AnyCameraWrappingT<T, TCameraWrapper>::vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	return TCameraWrapper::vectorIF(distortedImagePoint, makeUnitVector);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::vectorIF(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const
{
	return TCameraWrapper::vectorIF(distortedImagePoints, size, vectors, makeUnitVector);
}

template <typename T, typename TCameraWrapper>
LineT3<T> AnyCameraWrappingT<T, TCameraWrapper>::ray(const VectorT2<T>& distortedImagePoint, const HomogenousMatrixT4<T>& world_T_camera) const
{
	return TCameraWrapper::ray(distortedImagePoint, world_T_camera);
}

template <typename T, typename TCameraWrapper>
LineT3<T> AnyCameraWrappingT<T, TCameraWrapper>::ray(const VectorT2<T>& distortedImagePoint) const
{
	return TCameraWrapper::ray(distortedImagePoint);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const
{
	return TCameraWrapper::pointJacobian2x3IF(flippedCameraObjectPoint, jx, jy);
}

template <typename T, typename TCameraWrapper>
void AnyCameraWrappingT<T, TCameraWrapper>::pointJacobian2nx3IF(const VectorT3<T>* flippedCameraObjectPoints, const size_t numberObjectPoints, T* jacobians) const
{
	return TCameraWrapper::pointJacobian2nx3IF(flippedCameraObjectPoints, numberObjectPoints, jacobians);
}

template <typename T, typename TCameraWrapper>
bool AnyCameraWrappingT<T, TCameraWrapper>::isEqual(const AnyCameraT<T>& anyCamera, const T eps) const
{
	ocean_assert(eps >= T(0));

	if (isValid() != anyCamera.isValid())
	{
		// one camera is value, one is not valid
		return false;
	}

	if (!isValid())
	{
		// both cameras are invalid
		return true;
	}

	if (name() != anyCamera.name())
	{
		return false;
	}

	return TCameraWrapper::isEqual((const AnyCameraWrappingT<T, TCameraWrapper>&)(anyCamera), eps);
}

template <typename T, typename TCameraWrapper>
bool AnyCameraWrappingT<T, TCameraWrapper>::isValid() const
{
	return TCameraWrapper::isValid();
}

template <typename T, typename TCameraWrapperBase>
CameraWrapperT<T, TCameraWrapperBase>::CameraWrapperT(ActualCamera&& actualCamera) :
	TCameraWrapperBase(std::move(actualCamera))
{
	// nothing to do here
}

template <typename T, typename TCameraWrapperBase>
CameraWrapperT<T, TCameraWrapperBase>::CameraWrapperT(const ActualCamera& actualCamera) :
	TCameraWrapperBase(actualCamera)
{
	// nothing to do here
}

template <typename T, typename TCameraWrapperBase>
inline VectorT2<T> CameraWrapperT<T, TCameraWrapperBase>::principalPoint() const
{
	return VectorT2<T>(TCameraWrapperBase::principalPointX(), TCameraWrapperBase::principalPointY());
}

template <typename T, typename TCameraWrapperBase>
inline T CameraWrapperT<T, TCameraWrapperBase>::fovX() const
{
	ocean_assert(TCameraWrapperBase::isValid());

	/**
	 * x = Fx * X / Z + mx
	 *
	 * (x - mx) / Fx = X / Z
	 */

	if (NumericT<T>::isEqualEps(TCameraWrapperBase::focalLengthX()))
	{
		return T(0);
	}

	const T leftAngle = NumericT<T>::abs(NumericT<T>::atan(-TCameraWrapperBase::principalPointX() * TCameraWrapperBase::inverseFocalLengthX()));

	if (T(TCameraWrapperBase::width()) <= TCameraWrapperBase::principalPointX())
	{
		ocean_assert(false && "Invalid principal point");
		return T(2) * leftAngle;
	}

	const T rightAngle = NumericT<T>::atan((T(TCameraWrapperBase::width()) - TCameraWrapperBase::principalPointX()) * TCameraWrapperBase::inverseFocalLengthX());

	return leftAngle + rightAngle;
}

template <typename T, typename TCameraWrapperBase>
inline T CameraWrapperT<T, TCameraWrapperBase>::fovY() const
{
	ocean_assert(TCameraWrapperBase::isValid());

	/**
	 * y = Fy * Y / Z + my
	 *
	 * (y - my) / Fy = Y / Z
	 */

	if (NumericT<T>::isEqualEps(TCameraWrapperBase::focalLengthY()))
	{
		return T(0);
	}

	const T topAngle = NumericT<T>::abs(NumericT<T>::atan(-TCameraWrapperBase::principalPointY() * TCameraWrapperBase::inverseFocalLengthY()));

	if (T(TCameraWrapperBase::height()) <= TCameraWrapperBase::principalPointY())
	{
		ocean_assert(false && "Invalid principal point");
		return T(2) * topAngle;
	}

	const T bottomAngle = NumericT<T>::atan((T(TCameraWrapperBase::height()) - TCameraWrapperBase::principalPointY()) * TCameraWrapperBase::inverseFocalLengthY());

	return topAngle + bottomAngle;
}

template <typename T, typename TCameraWrapperBase>
inline bool CameraWrapperT<T, TCameraWrapperBase>::isInside(const VectorT2<T>& imagePoint, const T signedBorder) const
{
	ocean_assert(TCameraWrapperBase::isValid());

	const unsigned int cameraWidth = TCameraWrapperBase::width();
	const unsigned int cameraHeight = TCameraWrapperBase::height();

	ocean_assert(signedBorder < T(std::min(cameraWidth / 2u, cameraHeight / 2u)));

	return imagePoint.x() >= signedBorder && imagePoint.y() >= signedBorder
			&& imagePoint.x() < T(cameraWidth) - signedBorder && imagePoint.y() < T(cameraHeight) - signedBorder;
}

template <typename T, typename TCameraWrapperBase>
inline VectorT2<T> CameraWrapperT<T, TCameraWrapperBase>::projectToImage(const VectorT3<T>& objectPoint) const
{
	return TCameraWrapperBase::projectToImageIF(VectorT3<T>(objectPoint.x(), -objectPoint.y(), -objectPoint.z()));
}

template <typename T, typename TCameraWrapperBase>
inline VectorT2<T> CameraWrapperT<T, TCameraWrapperBase>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>& objectPoint) const
{
	return TCameraWrapperBase::projectToImageIF(CameraT<T>::standard2InvertedFlipped(world_T_camera), objectPoint);
}

template <typename T, typename TCameraWrapperBase>
inline void CameraWrapperT<T, TCameraWrapperBase>::projectToImage(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	ocean_assert(size == 0 || objectPoints != nullptr);
	ocean_assert(size == 0 || imagePoints != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		const VectorT3<T>& objectPoint = objectPoints[n];
		imagePoints[n] = TCameraWrapperBase::projectToImageIF(VectorT3<T>(objectPoint.x(), -objectPoint.y(), -objectPoint.z()));
	}
}

template <typename T, typename TCameraWrapperBase>
inline void CameraWrapperT<T, TCameraWrapperBase>::projectToImage(const HomogenousMatrixT4<T>& world_T_camera, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	return TCameraWrapperBase::projectToImageIF(CameraT<T>::standard2InvertedFlipped(world_T_camera), objectPoints, size, imagePoints);
}

template <typename T, typename TCameraWrapperBase>
inline VectorT3<T> CameraWrapperT<T, TCameraWrapperBase>::vector(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	const VectorT3<T> localVectorIF(TCameraWrapperBase::vectorIF(distortedImagePoint, makeUnitVector));

	return VectorT3<T>(localVectorIF.x(), -localVectorIF.y(), -localVectorIF.z());
}

template <typename T, typename TCameraWrapperBase>
inline void CameraWrapperT<T, TCameraWrapperBase>::vector(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const
{
	TCameraWrapperBase::vectorIF(distortedImagePoints, size, vectors, makeUnitVector);

	for (size_t n = 0; n < size; ++n)
	{
		const VectorT3<T>& localVectorIF = vectors[n];

		vectors[n] = VectorT3<T>(localVectorIF.x(), -localVectorIF.y(), -localVectorIF.z());
	}
}

template <typename T, typename TCameraWrapperBase>
inline LineT3<T> CameraWrapperT<T, TCameraWrapperBase>::ray(const VectorT2<T>& distortedImagePoint, const HomogenousMatrixT4<T>& world_T_camera) const
{
	ocean_assert(TCameraWrapperBase::isValid() && world_T_camera.isValid());

	return LineT3<T>(world_T_camera.translation(), world_T_camera.rotationMatrix(vector(distortedImagePoint, true /*makeUnitVector*/)));
}

template <typename T, typename TCameraWrapperBase>
inline LineT3<T> CameraWrapperT<T, TCameraWrapperBase>::ray(const VectorT2<T>& distortedImagePoint) const
{
	ocean_assert(TCameraWrapperBase::isValid());

	return LineT3<T>(VectorT3<T>(0, 0, 0), vector(distortedImagePoint, true /*makeUnitVector*/));
}

template <typename T, typename TCameraWrapperBase>
inline void CameraWrapperT<T, TCameraWrapperBase>::pointJacobian2nx3IF(const VectorT3<T>* flippedCameraObjectPoints, const size_t numberObjectPoints, T* jacobians) const
{
	ocean_assert(flippedCameraObjectPoints != nullptr);
	ocean_assert(numberObjectPoints >= 1);
	ocean_assert(jacobians != nullptr);

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		TCameraWrapperBase::pointJacobian2x3IF(flippedCameraObjectPoints[n], jacobians + 0, jacobians + 3);
		jacobians += 6;
	}
}

template <typename T>
CameraWrapperBasePinholeT<T>::CameraWrapperBasePinholeT(ActualCamera&& actualCamera) :
	actualCamera_(std::move(actualCamera))
{
	// nothing to do here
}

template <typename T>
CameraWrapperBasePinholeT<T>::CameraWrapperBasePinholeT(const ActualCamera& actualCamera) :
	actualCamera_(actualCamera)
{
	// nothing to do here
}

template <typename T>
inline const typename CameraWrapperBasePinholeT<T>::ActualCamera& CameraWrapperBasePinholeT<T>::actualCamera() const
{
	return actualCamera_;
}

template <typename T>
inline unsigned int CameraWrapperBasePinholeT<T>::width() const
{
	return actualCamera_.width();
}

template <typename T>
inline unsigned int CameraWrapperBasePinholeT<T>::height() const
{
	return actualCamera_.height();
}

template <typename T>
inline T CameraWrapperBasePinholeT<T>::principalPointX() const
{
	return T(actualCamera_.principalPointX());
}

template <typename T>
inline T CameraWrapperBasePinholeT<T>::principalPointY() const
{
	return T(actualCamera_.principalPointY());
}

template <typename T>
inline T CameraWrapperBasePinholeT<T>::focalLengthX() const
{
	return T(actualCamera_.focalLengthX());
}

template <typename T>
inline T CameraWrapperBasePinholeT<T>::focalLengthY() const
{
	return T(actualCamera_.focalLengthY());
}

template <typename T>
inline T CameraWrapperBasePinholeT<T>::inverseFocalLengthX() const
{
	return T(actualCamera_.inverseFocalLengthX());
}

template <typename T>
inline T CameraWrapperBasePinholeT<T>::inverseFocalLengthY() const
{
	return T(actualCamera_.inverseFocalLengthY());
}

template <typename T>
inline VectorT2<T> CameraWrapperBasePinholeT<T>::projectToImageIF(const VectorT3<T>& objectPoint) const
{
	return VectorT2<T>(actualCamera_.template projectToImageIF<true>(HomogenousMatrixT4<T>(true), objectPoint, true));
}

template <typename T>
inline VectorT2<T> CameraWrapperBasePinholeT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const
{
	return VectorT2<T>(actualCamera_.template projectToImageIF<true>(flippedCamera_T_world, objectPoint, true));
}

template <typename T>
inline void CameraWrapperBasePinholeT<T>::projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	ocean_assert(size == 0 || objectPoints != nullptr);
	ocean_assert(size == 0 || imagePoints != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		imagePoints[n] = projectToImageIF(objectPoints[n]);
	}
}

template <typename T>
inline void CameraWrapperBasePinholeT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	ocean_assert(size == 0 || objectPoints != nullptr);
	ocean_assert(size == 0 || imagePoints != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		imagePoints[n] = projectToImageIF(flippedCamera_T_world, objectPoints[n]);
	}
}

template <typename T>
inline VectorT3<T> CameraWrapperBasePinholeT<T>::vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	const VectorT2<T> undistortedImagePoint(actualCamera_.template undistort<true>(distortedImagePoint));

	return VectorT3<T>(actualCamera_.vectorIF(undistortedImagePoint, makeUnitVector));
}

template <typename T>
inline void CameraWrapperBasePinholeT<T>::vectorIF(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const
{
	ocean_assert(distortedImagePoints != nullptr && size > 0);
	ocean_assert(vectors != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		vectors[n] = vectorIF(distortedImagePoints[n], makeUnitVector);
	}
}

template <typename T>
inline void CameraWrapperBasePinholeT<T>::pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const
{
	ocean_assert(jx != nullptr && jy != nullptr);
	actualCamera_.template pointJacobian2x3IF<T, true>(flippedCameraObjectPoint, jx, jy);
}

template <typename T>
inline bool CameraWrapperBasePinholeT<T>::isEqual(const CameraWrapperBasePinholeT<T>& basePinhole, const T eps) const
{
	ocean_assert(eps >= T(0));

	return actualCamera_.isEqual(basePinhole.actualCamera_, eps);
}

template <typename T>
inline bool CameraWrapperBasePinholeT<T>::isValid() const
{
	return actualCamera_.isValid();
}

template <typename T>
template <typename U>
inline std::unique_ptr<AnyCameraT<U>> CameraWrapperBasePinholeT<T>::clone(const unsigned int width, const unsigned int height) const
{
	ocean_assert(actualCamera_.isValid());

	if constexpr (std::is_same<T, U>::value)
	{
		if ((width == 0u && height == 0u) || (width == actualCamera_.width() && height == actualCamera_.height()))
		{
			return std::make_unique<AnyCameraWrappingT<U, CameraWrapperT<U, CameraWrapperBasePinholeT<U>>>>(actualCamera_);
		}

		const unsigned int validWidth = (height * actualCamera_.width() + actualCamera_.height() / 2u) / actualCamera_.height();
		const unsigned int validHeight = (width * actualCamera_.height() + actualCamera_.width() / 2u) / actualCamera_.width();

		if (!NumericT<unsigned int>::isEqual(width, validWidth, 1u) && !NumericT<unsigned int>::isEqual(height, validHeight, 1u)) // either of the valid width/height needs to be close by 1 pixel
		{
			ocean_assert(false && "Wrong aspect ratio!");
			return nullptr;
		}

		return std::make_unique<AnyCameraWrappingT<U, CameraWrapperT<U, CameraWrapperBasePinholeT<U>>>>(PinholeCameraT<U>(width, height, actualCamera_));
	}
	else
	{
		const PinholeCameraT<U> convertedPinholeCamera(actualCamera_);

		if ((width == 0u && height == 0u) || (width == actualCamera_.width() && height == actualCamera_.height()))
		{
			return std::make_unique<AnyCameraWrappingT<U, CameraWrapperT<U, CameraWrapperBasePinholeT<U>>>>(convertedPinholeCamera);
		}

		const unsigned int validWidth = (height * actualCamera_.width() + actualCamera_.height() / 2u) / actualCamera_.height();
		const unsigned int validHeight = (width * actualCamera_.height() + actualCamera_.width() / 2u) / actualCamera_.width();

		if (!NumericT<unsigned int>::isEqual(width, validWidth, 1u) && !NumericT<unsigned int>::isEqual(height, validHeight, 1u)) // either of the valid width/height needs to be close by 1 pixel
		{
			ocean_assert(false && "Wrong aspect ratio!");
			return nullptr;
		}

		return std::make_unique<AnyCameraWrappingT<U, CameraWrapperT<U, CameraWrapperBasePinholeT<U>>>>(PinholeCameraT<U>(width, height, convertedPinholeCamera));
	}
}

template <typename T>
inline AnyCameraType CameraWrapperBasePinholeT<T>::anyCameraType()
{
	return AnyCameraType::PINHOLE;
}

template <typename T>
inline std::string CameraWrapperBasePinholeT<T>::name()
{
	return std::string("Ocean Pinhole");
}

template <typename T>
CameraWrapperBaseFisheyeT<T>::CameraWrapperBaseFisheyeT(ActualCamera&& camera) :
	actualCamera_(std::move(camera))
{
	// nothing to do here
}

template <typename T>
CameraWrapperBaseFisheyeT<T>::CameraWrapperBaseFisheyeT(const ActualCamera& camera) :
	actualCamera_(camera)
{
	// nothing to do here
}

template <typename T>
inline const typename CameraWrapperBaseFisheyeT<T>::ActualCamera& CameraWrapperBaseFisheyeT<T>::actualCamera() const
{
	return actualCamera_;
}

template <typename T>
inline unsigned int CameraWrapperBaseFisheyeT<T>::width() const
{
	return actualCamera_.width();
}

template <typename T>
inline unsigned int CameraWrapperBaseFisheyeT<T>::height() const
{
	return actualCamera_.height();
}

template <typename T>
inline VectorT2<T> CameraWrapperBaseFisheyeT<T>::principalPoint() const
{
	return actualCamera_.principalPoint();
}

template <typename T>
inline T CameraWrapperBaseFisheyeT<T>::principalPointX() const
{
	return actualCamera_.principalPointX();
}

template <typename T>
inline T CameraWrapperBaseFisheyeT<T>::principalPointY() const
{
	return actualCamera_.principalPointY();
}

template <typename T>
inline T CameraWrapperBaseFisheyeT<T>::focalLengthX() const
{
	return actualCamera_.focalLengthX();
}

template <typename T>
inline T CameraWrapperBaseFisheyeT<T>::focalLengthY() const
{
	return actualCamera_.focalLengthY();
}

template <typename T>
inline T CameraWrapperBaseFisheyeT<T>::inverseFocalLengthX() const
{
	return actualCamera_.inverseFocalLengthX();
}

template <typename T>
inline T CameraWrapperBaseFisheyeT<T>::inverseFocalLengthY() const
{
	return actualCamera_.inverseFocalLengthY();
}

template <typename T>
inline VectorT2<T> CameraWrapperBaseFisheyeT<T>::projectToImageIF(const VectorT3<T>& objectPoint) const
{
	return actualCamera_.projectToImageIF(objectPoint);
}

template <typename T>
inline VectorT2<T> CameraWrapperBaseFisheyeT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const
{
	return actualCamera_.projectToImageIF(flippedCamera_T_world, objectPoint);
}

template <typename T>
inline void CameraWrapperBaseFisheyeT<T>::projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	ocean_assert(size == 0 || objectPoints != nullptr);
	ocean_assert(size == 0 || imagePoints != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		imagePoints[n] = projectToImageIF(objectPoints[n]);
	}
}

template <typename T>
inline void CameraWrapperBaseFisheyeT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	ocean_assert(size == 0 || objectPoints != nullptr);
	ocean_assert(size == 0 || imagePoints != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		imagePoints[n] = projectToImageIF(flippedCamera_T_world, objectPoints[n]);
	}
}

template <typename T>
inline VectorT3<T> CameraWrapperBaseFisheyeT<T>::vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	return actualCamera_.vectorIF(distortedImagePoint, makeUnitVector);
}

template <typename T>
inline void CameraWrapperBaseFisheyeT<T>::vectorIF(const VectorT2<T>* distortedImagePoints, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const
{
	ocean_assert(distortedImagePoints != nullptr && size > 0);
	ocean_assert(vectors != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		vectors[n] = vectorIF(distortedImagePoints[n], makeUnitVector);
	}
}

template <typename T>
inline void CameraWrapperBaseFisheyeT<T>::pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const
{
	actualCamera_.pointJacobian2x3IF(flippedCameraObjectPoint, jx, jy);
}

template <typename T>
inline bool CameraWrapperBaseFisheyeT<T>::isEqual(const CameraWrapperBaseFisheyeT& baseFisheye, const T eps) const
{
	ocean_assert(eps >= T(0));

	return actualCamera_.isEqual(baseFisheye.actualCamera_, eps);
}

template <typename T>
inline bool CameraWrapperBaseFisheyeT<T>::isValid() const
{
	return actualCamera_.isValid();
}

template <typename T>
template <typename U>
inline std::unique_ptr<AnyCameraT<U>> CameraWrapperBaseFisheyeT<T>::clone(const unsigned int width, const unsigned int height) const
{
	ocean_assert(actualCamera_.isValid());

	if ((width == 0u && height == 0u) || (width == actualCamera_.width() && height == actualCamera_.height()))
	{
		return std::make_unique<AnyCameraWrappingT<U, CameraWrapperT<U, CameraWrapperBaseFisheyeT<U>>>>(FisheyeCameraT<U>(actualCamera_));
	}

	const unsigned int validWidth = (height * actualCamera_.width() + actualCamera_.height() / 2u) / actualCamera_.height();

	if (!NumericT<unsigned int>::isEqual(width, validWidth, 1u))
	{
		ocean_assert(false && "Wrong aspect ratio!");
		return nullptr;
	}

	const T xFactor = T(width) / T(actualCamera_.width());
	const T yFactor = T(height) / T(actualCamera_.height());

	const U newPrincipalX = U(actualCamera_.principalPointX() * xFactor);
	const U newPrincipalY = U(actualCamera_.principalPointY() * yFactor);

	const U newFocalLengthX = U(actualCamera_.focalLengthX() * xFactor);
	const U newFocalLengthY = U(actualCamera_.focalLengthY() * yFactor);

	U radialDistortion[6];
	for (unsigned int n = 0u; n < 6u; ++n)
	{
		radialDistortion[n] = U(actualCamera_.radialDistortion()[n]);
	}

	const U tangentialDistortion[2] =
	{
		U(actualCamera_.tangentialDistortion()[0]),
		U(actualCamera_.tangentialDistortion()[1])
	};

	return std::make_unique<AnyCameraWrappingT<U, CameraWrapperT<U, CameraWrapperBaseFisheyeT<U>>>>(FisheyeCameraT<U>(width, height, newFocalLengthX, newFocalLengthY, newPrincipalX, newPrincipalY, radialDistortion, tangentialDistortion));
}

template <typename T>
inline AnyCameraType CameraWrapperBaseFisheyeT<T>::anyCameraType()
{
	return AnyCameraType::FISHEYE;
}

template <typename T>
inline std::string CameraWrapperBaseFisheyeT<T>::name()
{
	return std::string("Ocean Fisheye");
}

template <typename T>
CameraWrapperBaseInvalidT<T>::CameraWrapperBaseInvalidT(ActualCamera&& camera) :
	actualCamera_(std::move(camera))
{
	// nothing to do here
}

template <typename T>
CameraWrapperBaseInvalidT<T>::CameraWrapperBaseInvalidT(const ActualCamera& camera) :
	actualCamera_(camera)
{
	// nothing to do here
}

template <typename T>
inline const typename CameraWrapperBaseInvalidT<T>::ActualCamera& CameraWrapperBaseInvalidT<T>::actualCamera() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return actualCamera_;
}

template <typename T>
inline unsigned int CameraWrapperBaseInvalidT<T>::width() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return (unsigned int)(-1);
}

template <typename T>
inline unsigned int CameraWrapperBaseInvalidT<T>::height() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return (unsigned int)(-1);
}

template <typename T>
inline VectorT2<T> CameraWrapperBaseInvalidT<T>::principalPoint() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return VectorT2<T>(NumericT<T>::minValue(), NumericT<T>::minValue());
}

template <typename T>
inline T CameraWrapperBaseInvalidT<T>::principalPointX() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return NumericT<T>::minValue();
}

template <typename T>
inline T CameraWrapperBaseInvalidT<T>::principalPointY() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return NumericT<T>::minValue();
}

template <typename T>
inline T CameraWrapperBaseInvalidT<T>::focalLengthX() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return NumericT<T>::minValue();
}

template <typename T>
inline T CameraWrapperBaseInvalidT<T>::focalLengthY() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return NumericT<T>::minValue();
}

template <typename T>
inline T CameraWrapperBaseInvalidT<T>::inverseFocalLengthX() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return NumericT<T>::minValue();
}

template <typename T>
inline T CameraWrapperBaseInvalidT<T>::inverseFocalLengthY() const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return NumericT<T>::minValue();
}

template <typename T>
inline VectorT2<T> CameraWrapperBaseInvalidT<T>::projectToImageIF(const VectorT3<T>& /*objectPoint*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return VectorT2<T>(NumericT<T>::minValue(), NumericT<T>::minValue());
}

template <typename T>
inline VectorT2<T> CameraWrapperBaseInvalidT<T>::projectToImageIF(const HomogenousMatrixT4<T>& /*flippedCamera_T_world*/, const VectorT3<T>& /*objectPoint*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return VectorT2<T>(NumericT<T>::minValue(), NumericT<T>::minValue());
}

template <typename T>
inline void CameraWrapperBaseInvalidT<T>::projectToImageIF(const VectorT3<T>* /*objectPoints*/, const size_t /*size*/, VectorT2<T>* /*imagePoints*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

}

template <typename T>
inline void CameraWrapperBaseInvalidT<T>::projectToImageIF(const HomogenousMatrixT4<T>& /*flippedCamera_T_world*/, const VectorT3<T>* /*objectPoints*/, const size_t /*size*/, VectorT2<T>* /*imagePoints*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");
}

template <typename T>
inline VectorT3<T> CameraWrapperBaseInvalidT<T>::vectorIF(const VectorT2<T>& /*distortedImagePoint*/, const bool /*makeUnitVector*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return VectorT3<T>(NumericT<T>::minValue(), NumericT<T>::minValue(), NumericT<T>::minValue());
}

template <typename T>
inline void CameraWrapperBaseInvalidT<T>::vectorIF(const VectorT2<T>* /*distortedImagePoints*/, const size_t /*size*/, VectorT3<T>* /*vectors*/, const bool /*makeUnitVector*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");
}

template <typename T>
inline void CameraWrapperBaseInvalidT<T>::pointJacobian2x3IF(const VectorT3<T>& /*flippedCameraObjectPoint*/, T* /*jx*/, T* /*jy*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");
}

template <typename T>
inline bool CameraWrapperBaseInvalidT<T>::isEqual(const CameraWrapperBaseInvalidT& /*baseFisheye*/, const T /*eps*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return false;
}

template <typename T>
inline bool CameraWrapperBaseInvalidT<T>::isValid() const
{
	return false;
}

template <typename T>
template <typename U>
inline std::unique_ptr<AnyCameraT<U>> CameraWrapperBaseInvalidT<T>::clone(const unsigned int /*width*/, const unsigned int /*height*/) const
{
	Log::error() << "Invalid camera: " << actualCamera_.reason();

	ocean_assert(false && "This function must never be called.");

	return nullptr;
}

template <typename T>
inline AnyCameraType CameraWrapperBaseInvalidT<T>::anyCameraType()
{
	return AnyCameraType::INVALID;
}

template <typename T>
inline std::string CameraWrapperBaseInvalidT<T>::name()
{
	return std::string("Invalid camera");
}

template <typename T>
InvalidCameraT<T>::InvalidCameraT(const std::string& reason) :
	reason_(reason)
{
	// nothing to do here
}

template <typename T>
const std::string& InvalidCameraT<T>::reason() const
{
	return reason_;
}

}

#endif // META_OCEAN_MATH_ANY_CAMERA_H
