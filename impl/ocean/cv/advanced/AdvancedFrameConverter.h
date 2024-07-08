/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_CONVERTER_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_CONVERTER_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements a frame converter creating advanced and artificial pixel formats.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT AdvancedFrameConverter
{
	public:

		/**
		 * Converts a 3 channel 24 bit color frame to a 2 channel 16 bit color (with YU component) and 1 channel 8 bit texture frame.
		 * The 8 bit texture component is created by averaging the Scharr 3x3 filter response over a window area.
		 * @param source The source frame to convert
		 * @param target The target frame receiving the converted frame
		 * @param window Size of the window in pixel used for filter response averaging, with range [1, infinity), must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool convertToYUT24ScharrMagnitude(const Frame& source, Frame& target, const unsigned int window = 11u, Worker* worker = nullptr);

		/**
		 * Converts a 4 channel 32 bit color frame to a 3 channel 24 bit color (with YUV component) and 1 channel 8 bit texture frame.
		 * The 8 bit texture component is created by averaging the Scharr 3x3 filter response over a window area.
		 * @param source The source frame to convert
		 * @param target The target frame receiving the converted frame
		 * @param window Size of the window in pixel used for filter response averaging, with range [1, infinity), must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool convertToYUVT32ScharrMagnitude(const Frame& source, Frame& target, const unsigned int window = 11u, Worker* worker = nullptr);

		/**
		 * Converts a 4 channel 32 bit color frame to a 3 channel 24 bit color (with RGB component) and 1 channel 8 bit texture frame.
		 * The 8 bit texture component is created by averaging the Scharr 3x3 filter response over a window area.
		 * @param source The source frame to convert
		 * @param target The target frame receiving the converted frame
		 * @param window Size of the window in pixel used for filter response averaging, with range [1, infinity), must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool convertToRGBT32ScharrMagnitude(const Frame& source, Frame& target, const unsigned int window = 11u, Worker* worker = nullptr);

		/**
		 * Converts a given frame into a RGB frame with additional (fourth) texture channel.
		 * The texture is determined by the absolute magnitude of the horizontal and vertical Scharr filter response.
		 * @param source The source frame to be converted
		 * @param target The target frame receiving the converted frame
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool convertToRGBT32ScharrMagnitude(const Frame& source, Frame& target, Worker* worker = nullptr);

		/**
		 * Converts a given frame into a mean filtered YUV frame with extended Laplace (magnitude) channels.
		 * First, the frame is converted in a YUV frame. Second, for each channel the magnitude Laplace filter response is determined and added as additional channel.<br>
		 * Finally, each channel is filtered by a mean filter with specified filter size.
		 * @param source The source frame which will be converted
		 * @param target The resulting target frame with 6 data channels
		 * @param window Size of the window in pixel used for filter response averaging, with range [1, infinity), must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool convertToYUVLLL48LaplaceMagnitude(const Frame& source, Frame& target, const unsigned int window = 11u, Worker* worker = nullptr);

		/**
		 * Changes a 3 channel 24 bit color frame to a 2 channel 16 bit color (with YU component) and 1 channel 8 bit texture frame.
		 * The 8 bit texture component is created by averaging the Scharr 3x3 filter response over a window area.
		 * @param frame The frame to convert
		 * @param window Size of the window in pixel used for filter response averaging, with range [1, infinity), must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool changeToYUT24ScharrMagnitude(Frame& frame, const unsigned int window = 11u, Worker* worker = nullptr);

		/**
		 * Changes a 4 channel 32 bit color frame to a 3 channel 24 bit color (with YUV component) and 1 channel 8 bit texture frame.
		 * The 8 bit texture component is created by averaging the Scharr 3x3 filter response over a window area.
		 * @param frame The frame to convert
		 * @param window Size of the window in pixel used for filter response averaging, with range [1, infinity), must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool changeToYUVT32ScharrMagnitude(Frame& frame, const unsigned int window = 11u, Worker* worker = nullptr);

		/**
		 * Changes a 4 channel 32 bit color frame to a 3 channel 24 bit color (with RGB component) and 1 channel 8 bit texture frame.
		 * The 8 bit texture component is created by averaging the Scharr 3x3 filter response over a window area.
		 * @param frame The frame to convert
		 * @param window Size of the window in pixel used for filter response averaging, with range [1, infinity), must be odd
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool changeToRGBT32ScharrMagnitude(Frame& frame, const unsigned int window = 11u, Worker* worker = nullptr);

	private:

		/**
		 * Creates a bordered integral frame holding the maximal absolute Scharr filter response values of 4 directories.
		 * @param frame The frame to create the integral frame from, must be valid
		 * @param window Size of the integral border in pixel, with range [1, infinity), must be odd
		 * @param integralFrame Resulting integral frame with size (frame.width() + 2 * window + 1)x(frame.height() + 2 * window + 1)
		 * @param worker Optional worker object to create the integral frame from
		 * @return True, if succeeded
		 */
		static bool createScharrMagnitudeIntegral(const Frame& frame, const unsigned int window, Frame& integralFrame, Worker* worker = nullptr);
};

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_CONVERTER_H
