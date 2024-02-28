// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanfreak/OceanFreak.h"
#include "ocean/unifiedfeatures/oceanfreak/OFHarrisFreakFeatureDetector.h"

#include <ar/camera/CameraModelAny.h>

#include <offline_mapping/libraries/math/CoordinateScaleUtils.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

/**
 * Definition of a functor for camera derivatives that can work with Surreal cameras
 * @sa Ocean::CV::Detector::FREAKDescriptorT<T>::CameraDerivativeFunctor
 * @ingroup unifiedfeaturesoceanfreak
 */
template <size_t tDescriptorSizeBytes>
class SurrealCameraDerivativeFunctor : public OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::FreakDescriptor::CameraDerivativeFunctor
{
	public:

		/**
		 * Constructor
		 * @param cameraModel The Surreal camera model that will be used to initialize this instance, must be valid
		 */
		SurrealCameraDerivativeFunctor(const surreal::CameraModelAny<float>& cameraModel);

		/**
		 * Computes the camera derivative for a given point
		 * @sa Ocean::CV::Detector::FREAKDescriptorT<T>::CameraDerivativeFunctor::computeCameraDerivativeData()
		 */
		typename OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::FreakDescriptor::CameraDerivativeData computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel = 1u) const override;

		/**
		 * Returns the maximum number of pyramid levels for which camera derivative data can be computed
		 * @sa Ocean::CV::Detector::FREAKDescriptorT<T>::CameraDerivativeFunctor::supportedPyramidLevels()
		 */
		unsigned int supportedPyramidLevels() const override;

	private:

		/// The managed Surreal camera model
		surreal::CameraModelAny<float> cameraModelLevel0_;
};

template <size_t tDescriptorSizeBytes>
SurrealCameraDerivativeFunctor<tDescriptorSizeBytes>::SurrealCameraDerivativeFunctor(const surreal::CameraModelAny<float>& cameraModel) :
	cameraModelLevel0_(cameraModel)
{
	// Nothing else to do.
}

template <size_t tDescriptorSizeBytes>
typename OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::FreakDescriptor::CameraDerivativeData SurrealCameraDerivativeFunctor<tDescriptorSizeBytes>::computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel) const
{
	const float scale = surreal::mapping::scaleFromOctavePyramidLevel(pointPyramidLevel);
	ocean_assert(scale > 0.0f);

	const Eigen::Vector2f pointLevel0 = surreal::mapping::computeLevel0Coordinate(point, scale);

	typename Ocean::UnifiedFeatures::OceanFreak::OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::FreakDescriptor::CameraDerivativeData data;
	data.unprojectRayIF = cameraModelLevel0_.Unproject(pointLevel0).normalized();
	data.pointJacobianMatrixIF = cameraModelLevel0_.dProject_dP(data.unprojectRayIF) / scale;

	return data;
}

template <size_t tDescriptorSizeBytes>
unsigned int SurrealCameraDerivativeFunctor<tDescriptorSizeBytes>::supportedPyramidLevels() const
{
	return std::numeric_limits<unsigned int>::max();
}

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
