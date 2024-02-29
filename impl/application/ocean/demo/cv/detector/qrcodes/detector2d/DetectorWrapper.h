// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/demo/cv/detector/qrcodes/ApplicationDemoCVDetectorQRCodes.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#ifdef OCEAN_USE_DEVICES_VRS
	#include "ocean/devices/vrs/DevicePlayer.h"
#endif // OCEAN_USE_DEVICES_VRS

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemocvdetectorqrcodes
 * @defgroup applicationdemocvdetectorqrcodesdetector QR Code
 * @{
 * Demo for the detection and decoding of QR codes (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.<br>
 * The implementation of this class is platform independent.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent detection and decoding of QR codes.
 * Beware: You must not have more than one DetectorWrapper object within one application.
 * @ingroup applicationdemocvdetectorqrcodesdetector
 */
class DetectorWrapper
{
	public:

		/**
		 * Creates an invalid DetectorWrapper object.
		 */
		DetectorWrapper() = default;

		/**
		 * Move constructor.
		 * @param detectorWrapper The wrapper object to be moved
		 */
		DetectorWrapper(DetectorWrapper&& detectorWrapper);

		/**
		 * Creates a new wrapper object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of this demo
		 */
		explicit DetectorWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an QRCOde object.
		 */
		~DetectorWrapper();

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
		 * @param detectorWrapper The detector wrapper object to be moved
		 */
		DetectorWrapper& operator=(DetectorWrapper&& detectorWrapper);

	protected:

		/**
		 * Not existing copy constructor.
		 * @param detectorWrapper The wrapper object to be copied
		 */
		DetectorWrapper(const DetectorWrapper& detectorWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param detectorWrapper The wrapper object to be assigned
		 */
		DetectorWrapper& operator=(const DetectorWrapper& detectorWrapper) = delete;

	protected:

		/// Indicates whether the old detector should be used instead of the new one.
		bool useOldDetector_ = false;

#ifdef OCEAN_USE_DEVICES_VRS
		/// Device player that is used for VRS replay
		std::shared_ptr<Devices::VRS::DevicePlayer> devicePlayer_;
#endif // OCEAN_USE_DEVICES_VRS

		/// The frame medium to provide the image sequence.
		Media::FrameMediumRef frameMedium_;

		/// The time stamp of the last frame that has been handled.
		Timestamp timestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;
};

inline Media::FrameMediumRef DetectorWrapper::frameMedium() const
{
	return frameMedium_;
}
