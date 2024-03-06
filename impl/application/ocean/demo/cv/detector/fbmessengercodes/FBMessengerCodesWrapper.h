// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FB_MESSENGER_CODES_H
#define META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FB_MESSENGER_CODES_H

#include "application/ocean/demo/cv/detector/ApplicationDemoCVDetector.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemocvdetector
 * @defgroup applicationdemocvdetectorfbmessengercodes FB Messenger codes
 * @{
 * Demo to measure the performance of the detection and decoding of the FB Messenger codes (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.<br>
 * The application will use the original implementation to detect the FB Messenger codes and will compare it against a Ocean-native implementation.
 * The implementation of this class is platform independent.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent detection and decoding of FB Messenger codes.
 * Beware: You must not have more than one FB Messenger codes object within one application.
 * @ingroup applicationdemocvdetectionfbmessengercodes
 */
class FBMessengerCodesWrapper
{
	public:

		/**
		 * Creates an invalid FB Messenger codes object.
		 */
		FBMessengerCodesWrapper();

		/**
		 * Move constructor.
		 * @param fbMessengerCodesWrapper The FB Messenger codes object to be moved
		 */
		FBMessengerCodesWrapper(FBMessengerCodesWrapper&& fbMessengerCodesWrapper) noexcept;

		/**
		 * Creates a new FB Messenger codes object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of this demo
		 */
		explicit FBMessengerCodesWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an FB Messenger codes object.
		 */
		~FBMessengerCodesWrapper();

		/**
		 * Explicitly releases this FB Messenger codes object.
		 */
		void release();

		/**
		 * Detect and decode a FB Messenger code in a video sequence
		 * @param frame The camera frame from
		 * @param time The time needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @param messages The decoded message from a Messenger code (hash)
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @return True if a Messenger code has been found, otherwise false
		 */
		bool detectAndDecode(Frame& frame, double& time, std::vector<std::string>& messages, bool* lastFrameReached = nullptr);

		/**
		 * Move operator.
		 * @param fbMessengerCodesWrapper The FB Messenger codes object to be moved
		 */
		FBMessengerCodesWrapper& operator=(FBMessengerCodesWrapper&& fbMessengerCodesWrapper) noexcept;

		/**
		 * Returns the frame medium providing the visual information for the wrapper.
		 * @return The wrapper's frame medium
		 */
		inline const Media::FrameMediumRef frameMedium() const;

	protected:

		/**
		 * Not existing copy constructor.
		 * @param fbMessengerCodesWrapper The FB Messenger codes object to be copied
		 */
		FBMessengerCodesWrapper(FBMessengerCodesWrapper& fbMessengerCodesWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param fbMessengerCodesWrapper The FB Messenger codes object to be assigned
		 */
		FBMessengerCodesWrapper& operator=(FBMessengerCodesWrapper& fbMessengerCodesWrapper) = delete;

	protected:

		/// The frame medium to provide the image sequence.
		Media::FrameMediumRef frameMedium_;

		/// The pixel format to be used for the underlying algorithms.
		FrameType::PixelFormat pixelFormat_;

		/// The timestamp of the last frame that has been handled.
		Timestamp timestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic fbMessengerCodesPerformance_;
};

inline const Media::FrameMediumRef FBMessengerCodesWrapper::frameMedium() const
{
	return frameMedium_;
}

#endif // META_APPLICATION_OCEAN_DEMO_CV_DETECTOR_FB_MESSENGER_CODES_H
