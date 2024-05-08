/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_LINE_DETECTOR_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_LINE_DETECTOR_WRAPPER_H

#include "application/ocean/demo/cv/ApplicationDemoCV.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemocvdetector
 * @defgroup applicationdemocvdetectorlinedetector Line Detector Wrapper
 * @{
 * The demo application demonstrates the usage of a Line Detector able to detect lines in frames.<br>
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent wrapper for the line detector which will be used/shared by/across platform specific applications.
 * @ingroup applicationdemocvdetectorlinedetector
 */
class LineDetectorWrapper
{
	public:

		/**
		 * Creates an invalid wrapper object.
		 */
		LineDetectorWrapper();

		/**
		 * Creates a new wrapper object by a given set of command arguments.
		 * The command arguments can be used to specify the behavior/mode of the detector:<br>
		 *
		 * 1. Parameter (optional): The name or filename of the input source e.g.:
		 * "LiveVideoId:0", or "directory/trackingMovie.mp4", or "singleImage.png"
		 *
		 * 2. Parameter (optional): The preferred frame dimension of the input medium in pixel:
		 * "640x480", or "1280x720", or "1920x1080"
		 *
		 * @param commandArguments The command arguments that configure the properties of the line detector
		 */
		explicit LineDetectorWrapper(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs a wrapper object.
		 */
		~LineDetectorWrapper();

		/**
		 * Explicitly releases this wrapper object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the detection for the frame.
		 * @param frame The resulting frame showing the detected lines
		 * @param time The time the detector needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity), negative if the detection failed
		 * @return True, if succeeded
		 */
		bool detectNewFrame(Frame& frame, double& time);
	
		/**
		 * Returns the frame medium providing the visual information for the wrapper.
		 * @return The wrapper's frame medium
		 */
		inline const Media::FrameMediumRef frameMedium() const;

	protected:

		/// The frame medium providing the visual information.
		Media::FrameMediumRef frameMedium_;

		/// The timestamp of the last frame that has been handled.
		Timestamp frameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;
};

inline const Media::FrameMediumRef LineDetectorWrapper::frameMedium() const
{
	return frameMedium_;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINEDETECTOR_LINE_DETECTOR_WRAPPER_H
