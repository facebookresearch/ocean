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

#include "ocean/math/SampleMap.h"

#include "ocean/media/MovieRecorder.h"

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemocvdetectorqrcodes
 * @defgroup applicationdemocvdetectorqrcodesdetector3d QR Code
 * @{
 * Demo for the 6-DOF detection QR codes (e.g., as replay from a recording file).
 * The implementation of this class is platform independent.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent 6-DOF detection of QR codes.
 * Beware: You must not have more than one Detector3DWrapper object within one application.
 * @ingroup applicationdemocvdetectorqrcodesdetector3d
 */
class Detector3DWrapper
{
	public:

		/**
		 * Creates an invalid Detector3DWrapper object.
		 */
		Detector3DWrapper() = default;

		/**
		 * Move constructor.
		 * @param detector3DWrapper The wrapper object to be moved
		 */
		Detector3DWrapper(Detector3DWrapper&& detector3DWrapper);

		/**
		 * Creates a new wrapper object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of this demo
		 */
		explicit Detector3DWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an QRCOde object.
		 */
		~Detector3DWrapper();

		/**
		 * Explicitly releases this QRCOde object.
		 */
		void release();

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
		 * @param detector3DWrapper The detector wrapper object to be moved
		 */
		Detector3DWrapper& operator=(Detector3DWrapper&& detector3DWrapper);

	protected:

		/**
		 * Not existing copy constructor.
		 * @param detector3DWrapper The wrapper object to be copied
		 */
		Detector3DWrapper(const Detector3DWrapper& detector3DWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param detector3DWrapper The wrapper object to be assigned
		 */
		Detector3DWrapper& operator=(const Detector3DWrapper& detector3DWrapper) = delete;

	protected:

		// The index of the first camera stream from the recording file that will be used.
		std::atomic<unsigned int> recordingCameraIndex0_ = 0u;

		// The index of the second camera stream from the recording that will be used.
		std::atomic<unsigned int> recordingCameraIndex1_ = 1u;

		/// Device player that is used for replay.
		Devices::SharedDevicePlayer devicePlayer_;

		/// The frame mediums to provide the image sequence.
		Media::FrameMediumRefs frameMediums_;

		/// The time stamp of the last frame that has been handled.
		Timestamp timestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;

		/// A movie recorder to visualize the processed data.
		Media::MovieRecorderRef movieRecorder_;
};

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER

/**
 * Creates a device player.
 * @param commandArguments The command arguments to use
 * @return The resulting device player, nullptr if the player could not be created
 */
Devices::SharedDevicePlayer Detector3DWrapper_createExternalDevicePlayer(const CommandArguments& commandArguments);

#endif // OCEAN_USE_EXTERNAL_DEVICE_PLAYER
