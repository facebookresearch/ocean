/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_MESSENGER_CODE_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_MESSENGER_CODE_H

#include "application/ocean/demo/cv/detector/ApplicationDemoCVDetector.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemocvdetector
 * @defgroup applicationdemocvdetectormessengercode Messenger code
 * @{
 * Demo to measure the performance of the detection and decoding of the Messenger code (e.g., from a live video/webcam).
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.<br>
 * The application will use the original implementation to detect the Messenger code and will compare it against a Ocean-native implementation.
 * The implementation of this class is platform independent.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent detection and decoding of Messenger code.
 * Beware: You must not have more than one Messenger code object within one application.
 * @ingroup applicationdemocvdetectionmessengercode
 */
class MessengerCodeWrapper
{
	public:

		/**
		 * Creates an invalid Messenger code object.
		 */
		MessengerCodeWrapper();

		/**
		 * Move constructor.
		 * @param messengerCodeWrapper The Messenger code object to be moved
		 */
		MessengerCodeWrapper(MessengerCodeWrapper&& messengerCodeWrapper) noexcept;

		/**
		 * Creates a new Messenger code object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of this demo
		 */
		explicit MessengerCodeWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an Messenger code object.
		 */
		~MessengerCodeWrapper();

		/**
		 * Explicitly releases this Messenger code object.
		 */
		void release();

		/**
		 * Detect and decode a Messenger code in a video sequence
		 * @param frame The camera frame from
		 * @param time The time needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @param messages The decoded message from a Messenger code (hash)
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @return True if a Messenger code has been found, otherwise false
		 */
		bool detectAndDecode(Frame& frame, double& time, std::vector<std::string>& messages, bool* lastFrameReached = nullptr);

		/**
		 * Move operator.
		 * @param messengerCodeWrapper The Messenger code object to be moved
		 */
		MessengerCodeWrapper& operator=(MessengerCodeWrapper&& messengerCodeWrapper) noexcept;

		/**
		 * Returns the frame medium providing the visual information for the wrapper.
		 * @return The wrapper's frame medium
		 */
		inline Media::FrameMediumRef frameMedium() const;

	protected:

		/**
		 * Not existing copy constructor.
		 * @param messengerCodeWrapper The Messenger code object to be copied
		 */
		MessengerCodeWrapper(MessengerCodeWrapper& messengerCodeWrapper) = delete;

		/**
		 * Not existing assign operator.
		 * @param messengerCodeWrapper The Messenger code object to be assigned
		 */
		MessengerCodeWrapper& operator=(MessengerCodeWrapper& messengerCodeWrapper) = delete;

	protected:

		/// The frame medium to provide the image sequence.
		Media::FrameMediumRef frameMedium_;

		/// The pixel format to be used for the underlying algorithms.
		FrameType::PixelFormat pixelFormat_;

		/// The timestamp of the last frame that has been handled.
		Timestamp timestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic messengerCodePerformance_;
};

inline Media::FrameMediumRef MessengerCodeWrapper::frameMedium() const
{
	return frameMedium_;
}

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_CV_DETECTOR_MESSENGER_CODE_H
