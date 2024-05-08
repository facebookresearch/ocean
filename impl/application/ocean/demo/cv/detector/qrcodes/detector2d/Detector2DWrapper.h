/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "application/ocean/demo/cv/detector/qrcodes/ApplicationDemoCVDetectorQRCodes.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/devices/DevicePlayer.h"

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemocvdetectorqrcodes
 * @defgroup applicationdemocvdetectorqrcodesdetector2d Demo for the QR Code Detector (2D).
 * @{
 * Demo for the detection and decoding of QR codes (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.<br>
 * The implementation of this class is platform independent.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent detection and decoding of QR codes.
 * Beware: You must not have more than one Detector2DWrapper object within one application.
 * @ingroup applicationdemocvdetectorqrcodesdetector2d
 */
class Detector2DWrapper
{
	public:

		/**
		 * Creates an invalid Detector2DWrapper object.
		 */
		Detector2DWrapper() = default;

		/**
		 * Move constructor.
		 * @param detector2dWrapper The wrapper object to be moved
		 */
		Detector2DWrapper(Detector2DWrapper&& detector2dWrapper);

		/**
		 * Creates a new wrapper object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of this demo
		 */
		explicit Detector2DWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an QRCOde object.
		 */
		~Detector2DWrapper();

		/**
		 * Explicitly releases this QRCOde object.
		 */
		void release();

		/**
		 * Returns the input frame medium which is used.
		 * @return The tracker's input frame medium
		 */
		inline Media::FrameMediumRef frameMedium() const;

		/**
		 * Detect and decode a QR code in a video sequence
		 * @param frame The camera frame from
		 * @param time The time needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @param messages The decoded message from a QR code
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @return True if a QR code has been found, otherwise false
		 */
		bool detectAndDecode(Frame& frame, double& time, std::vector<std::string>& messages, bool* lastFrameReached = nullptr);

		/**
		 * Move operator.
		 * @param detector2dWrapper The detector wrapper object to be moved
		 */
		Detector2DWrapper& operator=(Detector2DWrapper&& detector2dWrapper);

	protected:

		/**
		 * Not existing copy constructor.
		 * @param detector2dWrapper The wrapper object to be copied
		 */
		Detector2DWrapper(const Detector2DWrapper& detector2dWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param detector2dWrapper The wrapper object to be assigned
		 */
		Detector2DWrapper& operator=(const Detector2DWrapper& detector2dWrapper) = delete;

	protected:

		/// Indicates whether the old detector should be used instead of the new one.
		bool useOldDetector_ = false;

		/// Device player which may be used for replay.
		Devices::SharedDevicePlayer devicePlayer_;

		/// The frame medium to provide the image sequence.
		Media::FrameMediumRef frameMedium_;

		/// The time stamp of the last frame that has been handled.
		Timestamp timestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;
};

inline Media::FrameMediumRef Detector2DWrapper::frameMedium() const
{
	return frameMedium_;
}

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER

/**
 * Creates a device player.
 * @param commandArguments The command arguments to use
 * @return The resulting device player, nullptr if the player could not be created
 */
Devices::SharedDevicePlayer Detector2DWrapper_createExternalDevicePlayer(const CommandArguments& commandArguments);

#endif // OCEAN_USE_EXTERNAL_DEVICE_PLAYER
