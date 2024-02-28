// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MATH_ANY_CAMERA_PERCEPTION_H
#define META_OCEAN_MATH_ANY_CAMERA_PERCEPTION_H

#include "ocean/math/Math.h"
#include "ocean/math/AnyCamera.h"

#include <perception/camera/CameraModelInterface.h>

#include <Eigen/Core>

namespace Ocean
{

/**
 * This class implements the base wrapper around Perception's abstract base class for all Perception camera models.
 * The class can be used as 'TCameraWrapperBase' in 'CameraWrapperT' to create a full wrapper class e.g., 'CameraWrapperT<T, CameraWrapperBasePerceptionT<T>>'.
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class CameraWrapperBasePerceptionT
{
	template <typename U> friend class CameraWrapperBasePerceptionT;

	public:

		/**
		 * Definition of the actual camera object wrapped by this class.
		 */
		typedef std::shared_ptr<perception::CameraModelInterface<T>> ActualCamera;

		/**
		 * Definition of the parent wrapper class which is using this base class to create a full wrapped camera object.
		 */
		using WrappedCamera = CameraWrapperT<T, CameraWrapperBasePerceptionT<T>>;

	public:

		/**
		 * Creates a new CameraWrapperBasePerceptionT object wrapping the actual perception camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBasePerceptionT(ActualCamera&& actualCamera);

		/**
		 * Creates a new CameraWrapperBasePerceptionT object wrapping the actual perception camera model.
		 * @param actualCamera The actual camera object to be wrapped
		 */
		explicit CameraWrapperBasePerceptionT(const ActualCamera& actualCamera);

		/**
		 * Returns the type of this camera.
		 * @see AnyCameraT::anyCameraType().
		 */
		inline AnyCameraType anyCameraType() const;

		/**
		 * Returns the actual camera object wrappred by this class.
		 * @return The actual camera object
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
		 * Returns a unit vector (with length 1) starting at the camera's center and intersecting a given 2D point in the image.
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
		inline bool isEqual(const CameraWrapperBasePerceptionT<T>& basePerception, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether this camera is valid.
		 * @see AnyCameraT::isValid();
		 */
		inline bool isValid() const;

		/**
		 * Returns a copy of the actual camera object.
		 * Beware: Perception does not support to clone a float camera model to a double camera model.
		 * The image resolution of the cloned camera must have the same aspect ratio as the current image resolution.
		 * @param width The width of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @param height the height of the cloned camera in pixel, with range [1, infinity), 0 to use the current image resolution
		 * @return New instance of the actual camera object used in this wrapper.
		 * @tparam U The scalar data type of the resulting cloned object, either 'float' or 'double'
		 */
		template <typename U>
		inline std::unique_ptr<AnyCameraT<U>> clone(const unsigned int width, const unsigned int height) const;

		/**
		 * Returns the name of this camera.
		 * @see AnyCameraT::name().
		 */
		static inline std::string name();

	protected:

		/// The actual camera.
		ActualCamera actualCamera_;
};

/**
 * Definition of an AnyCamera object based on Perception's abstract camera class with template parameter to define the element precision.
 * @see AnyCameraT, CameraWrapperBasePerceptionT, AnyCameraPerception, AnyCameraPerceptionD, AnyCameraPerceptionF.
 * @ingroup math
 */
template <typename T>
using AnyCameraPerceptionT = AnyCameraWrappingT<T, CameraWrapperT<T, CameraWrapperBasePerceptionT<T>>>;

/**
 * Definition of an AnyCamera object based on Perception's abstract camera class with element precision 'Scalar'.
 * @see AnyCameraT, CameraWrapperBasePerceptionT, AnyCameraPerceptionT.
 * @ingroup math
 */
typedef AnyCameraPerceptionT<Scalar> AnyCameraPerception;

/**
 * Definition of an AnyCamera object based on Perception's abstract camera class with element precision 'double'.
 * @see AnyCameraT, CameraWrapperBasePerceptionT, AnyCameraPerceptionT.
 * @ingroup math
 */
typedef AnyCameraPerceptionT<double> AnyCameraPerceptionD;

/**
 * Definition of an AnyCamera object based on Perception's abstract camera class with element precision 'float'.
 * @see AnyCameraT, CameraWrapperBasePerceptionT, AnyCameraPerceptionT.
 * @ingroup math
 */
typedef AnyCameraPerceptionT<float> AnyCameraPerceptionF;

template <typename T>
CameraWrapperBasePerceptionT<T>::CameraWrapperBasePerceptionT(ActualCamera&& actualCamera) :
	actualCamera_(std::move(actualCamera))
{
	ocean_assert(actualCamera_);
}

template <typename T>
CameraWrapperBasePerceptionT<T>::CameraWrapperBasePerceptionT(const ActualCamera& actualCamera) :
	actualCamera_(actualCamera)
{
	ocean_assert(actualCamera_);
}

template <typename T>
inline AnyCameraType CameraWrapperBasePerceptionT<T>::anyCameraType() const
{
	ocean_assert(actualCamera_);

	switch (actualCamera_->cameraModelType())
	{
		case perception::CameraModelType::FISHEYE:
		case perception::CameraModelType::FISHEYE44:
		case perception::CameraModelType::FISHEYE62:
		case perception::CameraModelType::FISHEYE_RAD_TAN_THINPRISM:
		case perception::CameraModelType::FISHEYE62_FAST_UNPROJECT:
		case perception::CameraModelType::FISHEYE62_UNDISTORTION_LUT:
		case perception::CameraModelType::FISHEYE_CUBIC_PLUS_RD4:
		case perception::CameraModelType::FISHEYE62_WITH_EXTRA_LUT_FOR_CW:
			return AnyCameraType::FISHEYE;

		case perception::CameraModelType::RADTAN:
		case perception::CameraModelType::PINHOLE:
		case perception::CameraModelType::RADTAN3:
			return AnyCameraType::PINHOLE;

		default:
			break;
	}

	ocean_assert(false && "Missing implementation!");
	return AnyCameraType::INVALID;
}

template <typename T>
inline const typename CameraWrapperBasePerceptionT<T>::ActualCamera& CameraWrapperBasePerceptionT<T>::actualCamera() const
{
	ocean_assert(actualCamera_);
	return actualCamera_;
}

template <typename T>
inline unsigned int CameraWrapperBasePerceptionT<T>::width() const
{
	ocean_assert(actualCamera_);
	return (unsigned int)(actualCamera_->imageSize().width);
}

template <typename T>
inline unsigned int CameraWrapperBasePerceptionT<T>::height() const
{
	ocean_assert(actualCamera_);
	return (unsigned int)(actualCamera_->imageSize().height);
}

template <typename T>
inline T CameraWrapperBasePerceptionT<T>::principalPointX() const
{
	ocean_assert(actualCamera_);
	return actualCamera_->principalPoint().x();
}

template <typename T>
inline T CameraWrapperBasePerceptionT<T>::principalPointY() const
{
	ocean_assert(actualCamera_);
	return actualCamera_->principalPoint().y();
}

template <typename T>
inline T CameraWrapperBasePerceptionT<T>::focalLengthX() const
{
	ocean_assert(actualCamera_);
	return actualCamera_->focalLengths().x();
}

template <typename T>
inline T CameraWrapperBasePerceptionT<T>::focalLengthY() const
{
	ocean_assert(actualCamera_);
	return actualCamera_->focalLengths().y();
}

template <typename T>
inline T CameraWrapperBasePerceptionT<T>::inverseFocalLengthX() const
{
	ocean_assert(actualCamera_);
	return T(1) / actualCamera_->focalLengths().x(); // could be stored as member variable
}

template <typename T>
inline T CameraWrapperBasePerceptionT<T>::inverseFocalLengthY() const
{
	ocean_assert(actualCamera_);
	return T(1) / actualCamera_->focalLengths().y(); // could be stored as member variable
}

template <typename T>
inline VectorT2<T> CameraWrapperBasePerceptionT<T>::projectToImageIF(const VectorT3<T>& objectPoint) const
{
	ocean_assert(actualCamera_);

	const Eigen::Vector2<T> imagePoint = actualCamera_->projectNoChecks(Eigen::Vector3<T>(objectPoint.x(), objectPoint.y(), objectPoint.z()));

	return VectorT2<T>(imagePoint.x(), imagePoint.y());
}

template <typename T>
inline VectorT2<T> CameraWrapperBasePerceptionT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint) const
{
	return projectToImageIF(flippedCamera_T_world * objectPoint);
}

template <typename T>
inline void CameraWrapperBasePerceptionT<T>::projectToImageIF(const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	ocean_assert(actualCamera_);

	ocean_assert(size == 0 || objectPoints != nullptr);
	ocean_assert(size == 0 || imagePoints != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		imagePoints[n] = projectToImageIF(objectPoints[n]); // **TODO** use vectorized implementation
	}
}

template <typename T>
inline void CameraWrapperBasePerceptionT<T>::projectToImageIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>* objectPoints, const size_t size, VectorT2<T>* imagePoints) const
{
	ocean_assert(actualCamera_);

	ocean_assert(size == 0 || objectPoints != nullptr);
	ocean_assert(size == 0 || imagePoints != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		imagePoints[n] = projectToImageIF(flippedCamera_T_world, objectPoints[n]); // **TODO** use vectorized implementation
	}
}

template <typename T>
inline VectorT3<T> CameraWrapperBasePerceptionT<T>::vectorIF(const VectorT2<T>& distortedImagePoint, const bool makeUnitVector) const
{
	ocean_assert(actualCamera_);

	const Eigen::Vector3<T> objectPoint = actualCamera_->unprojectNoChecks(Eigen::Vector2<T>(distortedImagePoint.x(), distortedImagePoint.y()));

	if (makeUnitVector)
	{
		return VectorT3<T>(objectPoint.x(), objectPoint.y(), objectPoint.z()).normalized();
	}
	else
	{
		return VectorT3<T>(objectPoint.x(), objectPoint.y(), objectPoint.z());
	}
}

template <typename T>
inline void CameraWrapperBasePerceptionT<T>::vectorIF(const VectorT2<T>* distortedImagePoint, const size_t size, VectorT3<T>* vectors, const bool makeUnitVector) const
{
	ocean_assert(distortedImagePoint != nullptr && size > 0);
	ocean_assert(vectors != nullptr);

	for (size_t n = 0; n < size; ++n)
	{
		vectors[n] = vectorIF(distortedImagePoint[n], makeUnitVector);
	}
}

template <typename T>
inline void CameraWrapperBasePerceptionT<T>::pointJacobian2x3IF(const VectorT3<T>& flippedCameraObjectPoint, T* jx, T* jy) const
{
	ocean_assert(actualCamera_);
	ocean_assert(jx != nullptr && jy != nullptr);

	Eigen::Matrix<T, 2, 3> pointJacobian;
	actualCamera_->projectNoChecks(Eigen::Vector3<T>(flippedCameraObjectPoint.x(), flippedCameraObjectPoint.y(), flippedCameraObjectPoint.z()), pointJacobian);

	jx[0] = pointJacobian(0, 0);
	jx[1] = pointJacobian(0, 1);
	jx[2] = pointJacobian(0, 2);

	jy[0] = pointJacobian(1, 0);
	jy[1] = pointJacobian(1, 1);
	jy[2] = pointJacobian(1, 2);
}

template <typename T>
inline bool CameraWrapperBasePerceptionT<T>::isEqual(const CameraWrapperBasePerceptionT<T>& basePerception, const T eps) const
{
	if (bool(actualCamera_) != bool(basePerception.actualCamera_))
	{
		// one camera is invalid
		return false;
	}

	if (!bool(actualCamera_))
	{
		// both cameras are invalid
		return true;
	}

	if (actualCamera_->cameraModelType() != basePerception.actualCamera_->cameraModelType()
			|| actualCamera_->imageSize() != basePerception.actualCamera_->imageSize()
			|| NumericT<T>::isNotEqual(actualCamera_->focalLengths().x(), basePerception.actualCamera_->focalLengths().x(), eps)
			|| NumericT<T>::isNotEqual(actualCamera_->focalLengths().y(), basePerception.actualCamera_->focalLengths().y(), eps)
			|| NumericT<T>::isNotEqual(actualCamera_->principalPoint().x(), basePerception.actualCamera_->principalPoint().x(), eps)
			|| NumericT<T>::isNotEqual(actualCamera_->principalPoint().y(), basePerception.actualCamera_->principalPoint().y(), eps))
	{
		return false;
	}

	ocean_assert(actualCamera_->getIntrinsicParameters().cols() == 1);
	ocean_assert(basePerception.actualCamera_->getIntrinsicParameters().cols() == 1);

	if (actualCamera_->getIntrinsicParameters().rows() != basePerception.actualCamera_->getIntrinsicParameters().rows())
	{
		return false;
	}

	for (int row = 0; row < actualCamera_->getIntrinsicParameters().rows(); ++row)
	{
		if (NumericT<T>::isNotEqual(actualCamera_->getIntrinsicParameters()(row, 0), basePerception.actualCamera_->getIntrinsicParameters()(row, 0), eps))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
inline bool CameraWrapperBasePerceptionT<T>::isValid() const
{
	ocean_assert(actualCamera_);
	return bool(actualCamera_);
}

template <typename T>
template <typename U>
inline std::unique_ptr<AnyCameraT<U>> CameraWrapperBasePerceptionT<T>::clone(const unsigned int width, const unsigned int height) const
{
	std::shared_ptr<perception::CameraModelInterface<U>> clonedActualCamera;

	if constexpr (std::is_same<U, float>::value)
	{
		clonedActualCamera = actualCamera_->cloneToFloat();
	}
	else
	{
		static_assert(std::is_same<U, double>::value, "Invalid data type!");

		if constexpr (std::is_same<T, double>::value)
		{
			clonedActualCamera = actualCamera_->clone();
		}
		else
		{
			ocean_assert(false && "Perception does not support to clone a float camera model to a double camera model!");
			return nullptr;
		}
	}

	ocean_assert(clonedActualCamera);

	if ((width != 0u || height != 0u) && (width != (unsigned int)(clonedActualCamera->imageSize().width) || height != (unsigned int)(clonedActualCamera->imageSize().height)))
	{
		U scale = 0;

		if (width > height)
		{
			scale = U(width) / U(clonedActualCamera->imageSize().width);
		}
		else
		{
			scale = U(height) / U(clonedActualCamera->imageSize().height);
		}

		clonedActualCamera->scaleIntrinsics(scale);

		if (!NumericT<unsigned int>::isEqual(width, (unsigned int)(clonedActualCamera->imageSize().width), 1u)
			|| !NumericT<unsigned int>::isEqual(height, (unsigned int)(clonedActualCamera->imageSize().height), 1u))
		{
			ocean_assert(false && "Wrong aspect ratio!");

			return nullptr;
		}
	}

	return std::make_unique<AnyCameraWrappingT<U, CameraWrapperT<U, CameraWrapperBasePerceptionT<U>>>>(std::move(clonedActualCamera));
}

template <typename T>
inline std::string CameraWrapperBasePerceptionT<T>::name()
{
	return std::string("Perception CameraModelInterface");
}

}

#endif // META_OCEAN_MATH_ANY_CAMERA_H
