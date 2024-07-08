/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "application/ocean/demo/cv/detector/barcodes/ApplicationDemoCVDetectorBarcodes.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/devices/DevicePlayer.h"

#include "ocean/math/SampleMap.h"

#include "ocean/media/MovieRecorder.h"

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemocvdetectorbarcodes
 * @defgroup applicationdemocvdetectorbarcodesbarcodedetector2d Demo for the barcode detector (2D)
 * @{
 * Demo for the detection of barcodes using a webcam or as replay from a recording file.
 * The implementation of this class is platform independent.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent detection of barcodes (2D).
 * Beware: You must not have more than one Wrapper object within one application.
 * @ingroup applicationdemocvdetectorbarcodesbarcodedetector2d
 */
class Wrapper
{
	public:

		/**
		 * Creates an invalid Wrapper object.
		 */
		Wrapper() = default;

		/**
		 * Move constructor.
		 * @param barcodeDetector2DWrapper The wrapper object to be moved
		 */
		Wrapper(Wrapper&& barcodeDetector2DWrapper);

		/**
		 * Creates a new wrapper object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of this demo
		 */
		explicit Wrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destroys this wrapper object.
		 */
		~Wrapper();

		/**
		 * Explicitly releases this wrapper object.
		 */
		void release();

		/**
		 * Detects and decodes a barcodes in a video sequence
		 * @param frame The resulting camera frame that contains all visualizations of the detected barcodes
		 * @param time The result time needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @param messages The payload of the detected barcodes as well as additional information
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @return True if the processing was successful, otherwise false
		 */
		bool detectAndDecode(Frame& frame, double& time, std::vector<std::string>& messages, bool* lastFrameReached = nullptr);

		/**
		 * Move operator.
		 * @param barcodeDetector2DWrapper The detector wrapper object to be moved
		 */
		Wrapper& operator=(Wrapper&& barcodeDetector2DWrapper);

	protected:

		/**
		 * Not existing copy constructor.
		 * @param barcodeDetector2DWrapper The wrapper object to be copied
		 */
		Wrapper(const Wrapper& barcodeDetector2DWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param barcodeDetector2DWrapper The wrapper object to be assigned
		 */
		Wrapper& operator=(const Wrapper& barcodeDetector2DWrapper) = delete;

	protected:

		/// Device player which may be used for replay.
		Devices::SharedDevicePlayer devicePlayer_;

		/// The frame medium to provide the image sequence.
		Media::FrameMediumRef frameMedium_;

		/// The time stamp of the last frame that has been handled.
		Timestamp timestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;

		/// A movie recorder to visualize the processed data.
		Media::MovieRecorderRef movieRecorder_;
};

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER

/**
 * Optional registers additional command arguments.
 * @param commandArguments The command arguments
 */
void Wrapper_registerExternalCommandArguments(CommandArguments& commandArguments);

/**
 * Creates a device player.
 * @param commandArguments The command arguments to use
 * @return The resulting device player, nullptr if the player could not be created
 */
Devices::SharedDevicePlayer Wrapper_createExternalDevicePlayer(const CommandArguments& commandArguments);

#endif // OCEAN_USE_EXTERNAL_DEVICE_PLAYER
