/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_LIVE_VIDEO_H
#define META_OCEAN_MEDIA_AVF_LIVE_VIDEO_H

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/AVFFrameMedium.h"

#include "ocean/media/LiveVideo.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements a AVFoundation live video object.
 * @ingroup mediaavf
 */
class AVFLiveVideo :
	virtual public AVFFrameMedium,
	virtual public LiveVideo
{
	friend class AVFLibrary;

	public:

		/**
		 * Definition of a callback function for new samples.
		 */
		using OnNewSampleCallback = Callback<void, CVPixelBufferRef, SharedAnyCamera, double, double>;

	public:

		/**
		 * Clones this live video medium and returns a new independent instance of this medium.
		 * @see Medium::clone()
		 */
		MediumRef clone() const override;

		/**
		 * Returns the transformation between the camera and device.
		 * @see FrameMedium::device_T_camera().
		 */
		HomogenousMatrixD4 device_T_camera() const override;

		/**
		 * Returns the supported stream types.
		 * @see LiveVideo::supportedStreamTypes().
		 */
		StreamTypes supportedStreamTypes() const override;

		/**
		 * Returns the supported stream configurations for a given stream type.
		 * @see LiveVideo::supportedStreamConfigurations().
		 */
		StreamConfigurations supportedStreamConfigurations(const StreamType streamType = ST_INVALID) const override;

		/**
		 * Returns the current exposure duration of this device.
		 * @see exposureDuration().
		 */
		double exposureDuration(double* minDuration = nullptr, double* maxDuration = nullptr, ControlMode* exposureMode = nullptr) const override;

		/**
		 * Returns the current ISO of this device.
		 * @see iso().
		 */
		float iso(float* minISO = nullptr, float* maxISO = nullptr, ControlMode* isoMode = nullptr) const override;

		/**
		 * Returns the current focus of this device.
		 * @see focus().
		 */
		float focus(ControlMode* focusMode = nullptr) const override;

		/**
		 * Sets the preferred stream type.
		 * @see LiveVideo::setPreferredStreamConfiguration().
		 */
		bool setPreferredStreamType(const StreamType streamType) override;

		/**
		 * Sets the preferred stream configuration.
		 * @see LiveVideo::setPreferredStreamConfiguration().
		 */
		bool setPreferredStreamConfiguration(const StreamConfiguration& streamConfiguration) override;

		/**
		 * Sets the exposure duriation of this device.
		 * @see setExposureDuration().
		 */
		bool setExposureDuration(const double duration, const bool allowShorterExposure = false) override;

		/**
		 * Sets the ISO of this device.
		 * @see setISO().
		 */
		bool setISO(const float iso) override;

		/**
		 * Sets the focus of this device.
		 * @see setFocus().
		 */
		bool setFocus(const float position) override;

		/**
		 * Sets the known camera profile of this frame medium.
		 * @see FrameMedium::setCamera()
		 */
		bool setCamera(SharedAnyCamera&& camera) override;

		/**
		 * Returns whether video stabilization is currently enabled.
		 * @see videoStabilization().
		 */
		bool videoStabilization() const override;

		/**
		 * Sets whether video stabilization should be enabled.
		 * @see setVideoStabilization().
		 */
		bool setVideoStabilization(const bool enable) override;

		/**
		 * Explicitly feeds a new external pixel buffer into this live video.
		 * This function is intended for situations in which this live video does not receive the pixel buffers anymore from the system (e.g., when ARKit is accessing the video stream).<br>
		 * Do not call this function in case the live video is still receiving pixel buffers from the AVFoundation system.
		 * @param pixelBuffer A pointer to a buffer that contains the pixel data of the sample
		 * @param camera The profile of the camera, invalid if unknown
		 * @param unixTimestamp Used to set timestamp of the Frame associated to the sample (measured in seconds since 1970-01-01 00:00 UTC)
		 * @param sampleTime The presentation time of the sample in seconds, used to set the relative timestamp of the frame associated to the sample
		 */
		void feedNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera camera, const double unixTimestamp, const double sampleTime);

	protected:

		/**
		 * Creates a new live video by a given url.
		 * @param url Url of the live video
		 */
		explicit AVFLiveVideo(const std::string& url);

		/**
		 * Destructs a AVFLiveVideo object.
		 */
		~AVFLiveVideo() override;

		/**
		 * Creates the capture device necessary for the live video.
		 * @return True, if succeeded
		 */
		bool createCaptureDevice();

		/**
		 * Creates the capture session neccessary for the live video.
		 * This function needs a valid capture device object.
		 * @return True, if succeeded
		 */
		bool createCaptureSession();

		/**
		 * Releases the capture session.
		 */
		void releaseCaptureSession();

		/**
		 * Internally starts the medium.
		 * @see AVFMedium::internalStart()
		 */
		bool internalStart() override;

		/**
		 * Internally pauses the medium.
		 * @see AVFMedium::internalPause()
		 */
		bool internalPause() override;

		/**
		 * Internally stops the medium.
		 * @see AVFMedium::internalStop()
		 */
		bool internalStop() override;

		/**
		 * Called, if a new sample arrived.
		 * @see AVFFrameMedium::onNewSample(pixelBuffer, unixTimestamp, sampleTime)
		 */
		void onNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera camera, const double unixTimestamp, const double sampleTime) override;

		/**
		 * Returns the best matching capture device format for a specified frame dimension, pixel format and frame frequency.
		 * @param captureDevice The capture device for which the best matching format will be determined, must be valid
		 * @param preferredWidth The preferred width of the frame in pixel, with range [0, infinity), 0 to use any width
		 * @param preferredHeight The preferred height of the frame in pixel, with range [0, infinity), 0 to use any height
		 * @param preferredFrameFrequency The preferred frame frequency of the frame, with range (0, infinity), 0 to use any frame frequency
		 * @param preferredPixelFormat The preferred pixel format of the frame, FrameType::FORMAT_UNDEFINED to use any pixel format
		 * @param explicitFrameRate The resulting explicit frame rate of the best matching format, with range (0, infinity)
		 * @return The best matching capture device format, nullptr if no matching format could be found
		 */
		static AVCaptureDeviceFormat* bestMatchingCaptureDeviceFormat(AVCaptureDevice* captureDevice, const unsigned int preferredWidth, const unsigned int preferredHeight, const double preferredFrameFrequency, const FrameType::PixelFormat preferredPixelFormat, double& explicitFrameRate);

		/**
		 * Returns the best matching (horizontal) field of view for a specified frame dimension.
		 * The field of view value is determine from the device's parameters/description, if available.<br>
		 * This function is not supported on non-IOS platforms.
		 * @param device The device for which the field of view will be determined, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @return The best matching horizontal field of view in radian, -1 if no field of view could be determined
		 */
		static double bestMatchingFieldOfView(AVCaptureDevice* device, const unsigned int width, const unsigned int height);

		/**
		 * Determines the available stream configurations for the capture device.
		 * @return The resulting stream configurations
		 */
		StreamConfigurations determineAvailableStreamConfigurations() const;

	protected:

		/// The capture device object.
		AVCaptureDevice* captureDevice_ = nullptr;

		/// The capture session.
		AVCaptureSession* captureSession_ = nullptr;

		/// The device input for the capture session.
		AVCaptureDeviceInput* captureDeviceInput_ = nullptr;

		/// The video data output for the capture session.
		AVCaptureVideoDataOutput* captureVideoDataOutput_ = nullptr;

		/// The delegate object for new sample callbacks.
		NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>* sampleBufferDelegate_ = nullptr;

		/// The manual exposure duration of live video device, in seconds -1 or 0 if no manual exposure is selected.
		double exposureDuration_ = -1.0;

		/// The manual ISO of the live video device, -1 if no manual exposure is selected.
		float iso_ = -1.0f;

		/// Whether video stabilization is enabled (true) or disabled (false).
		bool videoStabilizationEnabled_ = false;

		/// The number of measurements of ISO and exposure values.
		unsigned int isoExposureMeasurements_ = 0u;

		/// The sum of ISO values.
		float isoSum_ = 0.0f;

		/// The sum of exposure duration values, in seconds.
		float exposureDurationSum_ = 0.0f;

		/// The camera profile for all images.
		SharedAnyCamera camera_;

		/// The approximated camera profile of this live video.
		SharedAnyCamera approximatedCamera_;
	
		/// True, if the medium waits for the first frame; False, if several frames have been received already.
		bool waitingForFirstFrame_ = true;

		/// The transformation between camera and device.
		HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(true);

		/// The stream configurations available for this camera.
		StreamConfigurations availableStreamConfigurations_;
};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_LIVE_VIDEO_H
