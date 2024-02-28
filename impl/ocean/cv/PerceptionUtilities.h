// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_PERCEPTION_UTILITIES_H
#define META_OCEAN_CV_PERCEPTION_UTILITIES_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"

#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SophusUtilities.h"
#include "ocean/math/ToonUtilities.h"

#include <perception/camera/CameraModelInterface.h>
#include <perception/camera/Fisheye62CameraModelLut1D.h>
#include <perception/image/ImageSlice.h>

#include <viper/camera/CameraGeometry.h>

namespace Ocean
{

namespace CV
{

/**
 * This class implements utility functions for the migration from Perception to Ocean.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT PerceptionUtilities
{
	public:
	/**
		 * Convert a Perception writable ImageSlice with 8bit grayscale image content to an Ocean Frame image object.
		 * @param imageSlice The image slice to be converted
		 * @param copy If true, the data will be copied to (and owned by) to Ocean frame
		 * @return The resulting Ocean Frame object
		 */
	static inline Frame toOceanFrame(const perception::ImageSlice8u& imageSlice, const bool copy = false);

	/**
		 * Convert a Perception read-only ImageSlice with 8bit grayscale image content to an Ocean Frame image object.
		 * @param imageSlice The image slice to be converted
		 * @param copy If true, the data will be copied to (and owned by) to Ocean frame
		 * @return The resulting Ocean Frame object
		 */
	static inline Frame toOceanFrame(const perception::ConstImageSlice8u& imageSlice, const bool copy = false);

	/**
		 * Convert a Perception writable ImageSlice with 24bit color image content to an Ocean Frame image object.
		 * @param imageSlice The image slice to be converted
		 * @param copy If true, the data will be copied to (and owned by) to Ocean frame
		 * @return The resulting Ocean Frame object
		 */
	static inline Frame toOceanFrame(const perception::ImageSlice8uC3& imageSlice, const bool copy = false);

	/**
		 * Convert a Perception read-only ImageSlice with 24bit color image content to an Ocean Frame image object.
		 * @param imageSlice The image slice to be converted
		 * @param copy If true, the data will be copied to (and owned by) to Ocean frame
		 * @return The resulting Ocean Frame object
		 */
	static inline Frame toOceanFrame(const perception::ConstImageSlice8uC3& imageSlice, const bool copy = false);

	/**
		 * Converts a Perception camera model to an Ocean FisheyeCamera object.
		 * @param cameraModel The Perception camera model to convert
		 * @param fisheyeCamera The resulting Ocean FisheyeCamera object
		 * @return True, if succeeded
		 * @tparam TSource The data type of the Perception camera model, e.g., `float` or `double`
		 * @tparam TTarget The data type of the Ocean fisheye camera model, e.g., `float` or `double`
		 */
	template <typename TSource, typename TTarget>
	static inline bool toFisheyeCamera(const perception::CameraModelInterface<TSource>& cameraModel, FisheyeCameraT<TTarget>& fisheyeCamera);

	/**
		 * Converts a Viper CameraGeometry to an Ocean FisheyeCamera object and camera transformation.
		 * @param cameraGeometry The CameraGeometry object to convert
		 * @param fisheyeCamera The resulting Ocean FisheyeCamera object
		 * @param device_T_camera The resulting Ocean transformation between camera and device
		 * @return True, if succeeded
		 * @tparam TSource The data type of the Viper camera geometry, e.g., `float` or `double`
		 * @tparam TTarget The data type of the Ocean fisheye camera model, e.g., `float` or `double`
		 */
	template <typename TSource, typename TTarget>
	static inline bool toFisheyeCameraAndPose(const viper::CameraGeometry& cameraGeometry, FisheyeCameraT<TTarget>& fisheyeCamera, HomogenousMatrixT4<TTarget>& device_T_camera);

	/**
		 * Converts an Ocean fisheye camera profile to a Perception FISHEYE62 camera model.
		 * @param fisheyeCamera The Ocean fisheye camera to be converted, must be valid
		 * @return The resulting perception camera model
		 * @tparam TSource The data type of the Ocean fisheye camera model, e.g., `float` or `double`
		 * @tparam TTarget The data type of the Perception camera geometry, e.g., `float` or `double`
		 */
	template <typename TSource, typename TTarget>
	static inline std::unique_ptr<perception::CameraModelInterface<TTarget>> fromFisheyeCamera(const FisheyeCameraT<TSource>& fisheyeCamera);

	protected:
	/**
		 * Creates a read-only Frame based on some input data.
		 * @param width The width of the resulting frame, with range [1, infinity)
		 * @param height The height of the resulting frame, with range [1, infinity)
		 * @param strideInBytes The frame stride, in bytes, with range [width, infinity]
		 * @param data The read-only input memory, must be valid
		 * @param pixelFormat The pixel format of the resulting frame
		 * @param copy True, to make of copy of the given input memory; False, to use the memory only
		 * @return The resulting Ocean Frame object
		 * @tparam T The data type of the frame's elements
		 */
	template <typename T>
	static inline Frame toOceanFrame(const int16_t width, const int16_t height, const int strideInBytes, const T* data, const FrameType::PixelFormat pixelFormat, const bool copy);

	/**
		 * Creates a writable Frame based on some input data.
		 * @param width The width of the resulting frame, with range [1, infinity)
		 * @param height The height of the resulting frame, with range [1, infinity)
		 * @param strideInBytes The frame stride, in bytes, with range [width, infinity]
		 * @param data The writable input memory, must be valid
		 * @param pixelFormat The pixel format of the resulting frame
		 * @param copy True, to make of copy of the given input memory; False, to use the memory only
		 * @return The resulting Ocean Frame object
		 * @tparam T The data type of the frame's elements
		 */
	template <typename T>
	static inline Frame toOceanFrame(const int16_t width, const int16_t height, const int strideInBytes, T* data, const FrameType::PixelFormat pixelFormat, const bool copy);
};

inline Frame PerceptionUtilities::toOceanFrame(const perception::ImageSlice8u& imageSlice, const bool copy)
{
	return toOceanFrame<uint8_t>(imageSlice.width(), imageSlice.height(), imageSlice.strideInBytes(), (uint8_t*)(imageSlice.data()), FrameType::FORMAT_Y8, copy);
}

inline Frame PerceptionUtilities::toOceanFrame(const perception::ConstImageSlice8u& imageSlice, const bool copy)
{
	return toOceanFrame<uint8_t>(imageSlice.width(), imageSlice.height(), imageSlice.strideInBytes(), (const uint8_t*)(imageSlice.data()), FrameType::FORMAT_Y8, copy);
}

inline Frame PerceptionUtilities::toOceanFrame(const perception::ImageSlice8uC3& imageSlice, const bool copy)
{
	return toOceanFrame<uint8_t>(imageSlice.width(), imageSlice.height(), imageSlice.strideInBytes(), (uint8_t*)(imageSlice.data()), FrameType::FORMAT_RGB24, copy);
}

inline Frame PerceptionUtilities::toOceanFrame(const perception::ConstImageSlice8uC3& imageSlice, const bool copy)
{
	return toOceanFrame<uint8_t>(imageSlice.width(), imageSlice.height(), imageSlice.strideInBytes(), (const uint8_t*)(imageSlice.data()), FrameType::FORMAT_RGB24, copy);
}

template <typename TSource, typename TTarget>
inline bool PerceptionUtilities::toFisheyeCamera(const perception::CameraModelInterface<TSource>& cameraModel, FisheyeCameraT<TTarget>& fisheyeCamera)
{
	if (cameraModel.getIntrinsicParameters().rows() != 11)
	{
		return false;
	}

	fisheyeCamera = FisheyeCameraT<TTarget>((unsigned int)(cameraModel.imageSize().x()), (unsigned int)(cameraModel.imageSize().y()), FisheyeCameraT<TTarget>::PC_11_PARAMETERS_ONE_FOCAL_LENGTH, cameraModel.getIntrinsicParameters().data());

	return true;
}

template <typename TSource, typename TTarget>
inline bool PerceptionUtilities::toFisheyeCameraAndPose(const viper::CameraGeometry& cameraGeometry, FisheyeCameraT<TTarget>& fisheyeCamera, HomogenousMatrixT4<TTarget>& device_T_camera)
{
	if (!toFisheyeCamera<TSource, TTarget>(*cameraGeometry.cameraModel, fisheyeCamera))
	{
		return false;
	}

	const HomogenousMatrixT4<TTarget> flippedCamera_T_device(SophusUtilities::toHomogenousMatrix4<TSource, TTarget>(cameraGeometry.T_cameraFromDevice));

	// in Ocean, a camera pose is given wrt a default camera pointing towards negative z-space (with y-axis up),
	// in Perception, a camera pose is expected with a default camera pointing towards positive z-space (with y-axis down)

	device_T_camera = PinholeCamera::invertedFlipped2Standard(flippedCamera_T_device);

	return true;
}

template <typename TSource, typename TTarget>
inline std::unique_ptr<perception::CameraModelInterface<TTarget>> PerceptionUtilities::fromFisheyeCamera(const FisheyeCameraT<TSource>& fisheyeCamera)
{
	ocean_assert(fisheyeCamera.isValid());

	ocean_assert(fisheyeCamera.width() <= 32767u && fisheyeCamera.height() <= 32767u);
	const perception::ImageSize imageSize(int16_t(fisheyeCamera.width()), int16_t(fisheyeCamera.height()));

	std::vector<TTarget> intrinsics(11);
	intrinsics[0] = TTarget((fisheyeCamera.focalLengthX() + fisheyeCamera.focalLengthY()) * TSource(0.5));
	intrinsics[1] = TTarget(fisheyeCamera.principalPointX());
	intrinsics[2] = TTarget(fisheyeCamera.principalPointY());

	for (unsigned int n = 0u; n < 6u; ++n)
	{
		intrinsics[3u + n] = TTarget(fisheyeCamera.radialDistortion()[n]);
	}

	for (unsigned int n = 0u; n < 2u; ++n)
	{
		intrinsics[3u + 6u + n] = TTarget(fisheyeCamera.tangentialDistortion()[n]);
	}

	return std::make_unique<perception::Fisheye62CameraModelLut1D<TTarget>>(imageSize, intrinsics);
}

template <typename T>
inline Frame PerceptionUtilities::toOceanFrame(const int16_t width, const int16_t height, const int strideInBytes, const T* data, const FrameType::PixelFormat pixelFormat, const bool copy)
{
	if (width <= 0 || height <= 0 || strideInBytes <= 0)
	{
		ocean_assert(false && "Invalid image slice!");
		return Frame();
	}

	const unsigned int uWidth = (unsigned int)(width);
	const unsigned int uHeight = (unsigned int)(height);
	const unsigned int uStrideBytes = (unsigned int)(strideInBytes);

	unsigned int paddingElements;
	if (!Frame::strideBytes2paddingElements(pixelFormat, uWidth, uStrideBytes, paddingElements))
	{
		ocean_assert(false && "Invalid image slice!");
		return Frame();
	}

	const FrameType frameType(uWidth, uHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	const Frame::CopyMode copyMode = copy ? Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA : Frame::CM_USE_KEEP_LAYOUT;

	return Frame(frameType, data, copyMode, paddingElements); // will be a read-only frame if copy == true
}

template <typename T>
inline Frame PerceptionUtilities::toOceanFrame(const int16_t width, const int16_t height, const int strideInBytes, T* data, const FrameType::PixelFormat pixelFormat, const bool copy)
{
	if (width <= 0 || height <= 0 || strideInBytes <= 0)
	{
		ocean_assert(false && "Invalid image slice!");
		return Frame();
	}

	const unsigned int uWidth = (unsigned int)(width);
	const unsigned int uHeight = (unsigned int)(height);
	const unsigned int uStrideBytes = (unsigned int)(strideInBytes);

	unsigned int paddingElements;
	if (!Frame::strideBytes2paddingElements(pixelFormat, uWidth, uStrideBytes, paddingElements))
	{
		ocean_assert(false && "Invalid image slice!");
		return Frame();
	}

	const FrameType frameType(uWidth, uHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

	const Frame::CopyMode copyMode = copy ? Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA : Frame::CM_USE_KEEP_LAYOUT;

	return Frame(frameType, data, copyMode, paddingElements); // will be a writable frame
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_PERCEPTION_UTILITIES_H
