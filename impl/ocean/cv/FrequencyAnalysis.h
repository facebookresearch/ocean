/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FREQUENCY_ANALYSIS_H
#define META_OCEAN_CV_FREQUENCY_ANALYSIS_H

#include "ocean/cv/CV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Complex.h"

namespace Ocean
{

namespace CV
{

/**
 * This class provides frame frequency analysis functions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrequencyAnalysis
{
	public:

		/**
		 * Applies a Fourier transformation for a given frame.
		 * @param frame The frame for which the analysis is requested, must be valid
		 * @param frequencies Resulting frequencies, the number of resulting frequency components will be adjusted automatically
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static inline bool image2frequencies(const Frame& frame, Complexes& frequencies, Worker* worker = nullptr);

		/**
		 * Applies a Fourier transformation for a given frame.
		 * @param frame The frame for which the analysis is requested, must be valid
		 * @param frequencies Resulting frequencies
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool image2frequencies(const Frame& frame, Complex* frequencies, Worker* worker = nullptr);

		/**
		 * Applies an inverse Fourier analysis for given frequencies.
		 * The frame type of the resulting frame must match with the given set of frequencies.
		 * @param frequencies The frequencies for which an inverse Fourier transformation will be applied, each channel must be provided as joined block of frequencies with `frame.channels() * (frame.width() * frame.height())` elements, must be valid
		 * @param frame Resulting frame that defines the resulting frame type, must be valid
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool frequencies2image(const Complex* frequencies, Frame& frame, Worker* worker = nullptr);

		/**
		 * Applies a Fourier transformation for a given 1 channel 8 bit frame.
		 * @param frame The frame for which the analysis is requested, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels The number of interleaved frame channels, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param frequencies Resulting frequencies, with `channels * (width * height)` elements, each channel will be provided as joined block of frequencies, must be valid
		 * @param worker Optional worker object to distribute the computation
		 */
		static void image2frequencies8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, Complex* frequencies, Worker* worker = nullptr);

		/**
		 * Applies an inverse Fourier transformation for a given 1 channel 8 bit frame.
		 * @param frequencies The complex frequencies for which the inverse transformation will be applied
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param channels Number of interleaved frame channels, each channel is provided as joined block of frequencies, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param frame Resulting frame data with interleaved data channels
		 * @param worker Optional worker object to distribute the computation
		 */
		static void frequencies2image8BitPerChannel(const Complex* frequencies, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, uint8_t* frame, Worker* worker = nullptr);

		/**
		 * Creates a magnitude frame for a given set of frequencies.
		 * @param frequencies The complex frequencies for which the magnitude frame will be created, each channel needs to be provided as joined block of frequencies, must be valid
		 * @param width The width of the resulting frame (and also the original frame) in pixel
		 * @param height The height of the resulting frame (and also the original frame) in pixel
		 * @param channels The number of channels the frequencies have, with range [1, infinity)
		 * @param octaves Number of octaves that will be displayed
		 * @param shift True, to create a shifted magnitude frame
		 * @return Resulting magnitude frame
		 * @see FourierTransformation::shiftHalfDimension2(), FrameNormalizer::normalizeLogarithmToUint8().
		 */
		static Frame magnitudeFrame(const Complex* frequencies, const unsigned int width, const unsigned int height, const unsigned int channels, const Scalar octaves = Scalar(5), const bool shift = true);

		/**
		 * Performs a Fourier transformation on a given frame (that is transformed into a grayscale frame before) and returns a magnitude frame for the resulting set of frequencies.
		 * @param frame The frame for which the magnitude frame will be determined
		 * @param octaves Number of octaves that will be displayed
		 * @param shift True, to create a shifted magnitude frame
		 * @param worker Optional worker object to distribute the computation
		 * @return Resulting magnitude frame
		 * @see FourierTransformation::shiftHalfDimension2(), FrameNormalizer::normalizeLogarithmToUint8().
		 */
		static Frame magnitudeFrame(const Frame& frame, const Scalar octaves, const bool shift, Worker* worker = nullptr);
};

inline bool FrequencyAnalysis::image2frequencies(const Frame& frame, Complexes& frequencies, Worker* worker)
{
	ocean_assert(frame.isValid() && frame.numberPlanes() == 1u);

	frequencies.resize(frame.pixels() * FrameType::channels(frame.pixelFormat()));
	return image2frequencies(frame, frequencies.data(), worker);
}

}

}

#endif // META_OCEAN_CV_FREQUENCY_ANALYSIS_H
