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
		typedef Callback<void, CVPixelBufferRef, SharedAnyCamera, double, double> OnNewSampleCallback;

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
		 * Returns the current exposure duration of this device.
		 * @see exposureDuration().
		 */
		double exposureDuration(double* minDuration = nullptr, double* maxDuration = nullptr) const override;

		/**
		 * Returns the current ISO of this device.
		 * @see iso().
		 */
		float iso(float* minISO = nullptr, float* maxISO = nullptr) const override;

		/**
		 * Returns the current focus of this device.
		 * @see focus().
		 */
		float focus() const override;

		/**
		 * Sets the exposure duriation of this device.
		 * @see setExposureDuration().
		 */
		bool setExposureDuration(const double duration) override;

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
		 * Explicitly feeds a new external pixel buffer into this live video.
		 * This function is intended for situations in which this live video does not receive the pixel buffers anymore from the system (e.g., when ARKit is accessing the video stream).<br>
		 * Do not call this function in case the live video is still receiving pixel buffers from the AVFoundation system.
		 * @param pixelBuffer A pointer to a buffer that contains the pixel data of the sample
		 * @param anyCamera The profile of the camera, invalid if unknown
		 * @param unixTimestamp Used to set timestamp of the Frame associated to the sample (measured in seconds since 1970-01-01 00:00 UTC)
		 * @param sampleTime The presentation time of the sample in seconds, used to set the relative timestamp of the frame associated to the sample
		 */
		void feedNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera anyCamera, const double unixTimestamp, const double sampleTime);

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
		void onNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera anyCamera, const double unixTimestamp, const double sampleTime) override;

		/**
		 * Determines the exact session preset for a specified frame dimension.
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param presetWidth The width of the frame matching to the resulting preset, with range (0, infinity), 0 if no valid preset exists
		 * @param presetHeight The height of the frame matching to the resulting preset, with range (0, infinity), 0 if no valid preset exists
		 * @return The preset matching to the specified frame dimension, nullptr if no exact preset exists
		 */
		static NSString* determineExactPreset(const unsigned int width, const unsigned int height, unsigned int& presetWidth, unsigned int& presetHeight);

		/**
		 * Determines the best matching or next larger preset for a specified frame dimension.
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param presetWidth The width of the frame matching to the resulting preset, with range (0, infinity)
		 * @param presetHeight The height of the frame matching to the resulting preset, with range (0, infinity)
		 * @return The best preset better or equal to the specified frame dimension
		 */
		static NSString* determineNextLargerPreset(const unsigned int width, const unsigned int height, unsigned int& presetWidth, unsigned int& presetHeight);

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

		/// The number of measurements of ISO and exposure values.
		unsigned int isoExposureMeasurements_ = 0u;

		/// The sum of ISO values.
		float isoSum_ = 0.0f;

		/// The sum of exposure duration values, in seconds.
		float exposureDurationSum_ = 0.0f;

		/// The approximated camera profile of this live video.
		SharedAnyCamera approximatedAnyCamera_;

		/// The transformation between camera and device.
		HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(true);
};

}

}

}

#endif // META_OCEAN_MEDIA_AVF_LIVE_VIDEO_H
