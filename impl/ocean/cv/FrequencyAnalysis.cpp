/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrequencyAnalysis.h"

#include "ocean/base/Utilities.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameNormalizer.h"

#include "ocean/math/FourierTransformation.h"

namespace Ocean
{

namespace CV
{

bool FrequencyAnalysis::image2frequencies(const Frame& frame, Complex* frequencies, Worker* worker)
{
	ocean_assert(frame.isValid() && frequencies != nullptr);

	if (!frame.isValid() || frequencies == nullptr)
	{
		return false;
	}

	if (frame.numberPlanes() != 1u || frame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		ocean_assert(false && "Invalid pixel format!");
		return false;
	}

	image2frequencies8BitPerChannel(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.paddingElements(), frequencies, worker);
	return true;
}

bool FrequencyAnalysis::frequencies2image(const Complex* frequencies, Frame& frame, Worker* worker)
{
	ocean_assert(frequencies != nullptr && frame.isValid());

	if (!frame.isValid() || frequencies == nullptr)
	{
		return false;
	}

	if (frame.numberPlanes() != 1u || frame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		ocean_assert(false && "Invalid pixel format!");
		return false;
	}

	frequencies2image8BitPerChannel(frequencies, frame.width(), frame.height(), frame.channels(), frame.paddingElements(), frame.data<uint8_t>(), worker);
	return true;
}

void FrequencyAnalysis::image2frequencies8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, Complex* frequencies, Worker* worker)
{
	ocean_assert(frame != nullptr && frequencies != nullptr);
	ocean_assert(width >= 1u && height >= 1u && channels >= 1u);

	// first, we need to separate the individual channels into joined memory blocks

	const unsigned int frameStrideElements = width * channels + framePaddingElements;

	const unsigned int pixels = width * height;

	Scalars spatialBuffer(channels * pixels);

	for (unsigned int y = 0u; y < height; ++y)
	{
		Scalar* spatialRowChannel0 = spatialBuffer.data() + y * width;
		const uint8_t* frameRow = frame + y * frameStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				spatialRowChannel0[x + c * pixels] = Scalar(frameRow[x * channels + c]);
			}
		}
	}

	// now, we apply the FFT

	constexpr unsigned int spatialBufferPaddingElements = 0u;
	constexpr unsigned int frequenciesPaddingElements = 0u;

	if (worker == nullptr || channels == 1u)
	{
		for (unsigned int c = 0u; c < channels; ++c)
		{
			FourierTransformation::spatialToFrequency2<Scalar>(spatialBuffer.data() + c * pixels, width, height, (Scalar*)(frequencies + c * pixels), spatialBufferPaddingElements, frequenciesPaddingElements);
		}
	}
	else
	{
		ocean_assert(worker != nullptr);

		Worker::Functions functions(channels);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			functions[c] = Worker::Function::createStatic(FourierTransformation::spatialToFrequency2<Scalar>, (const Scalar*)(spatialBuffer.data() + c * pixels), width, height, (Scalar*)(frequencies + c * pixels), spatialBufferPaddingElements, frequenciesPaddingElements);
		}

		worker->executeFunctions(functions);
	}
}

void FrequencyAnalysis::frequencies2image8BitPerChannel(const Complex* frequencies, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, uint8_t* frame, Worker* worker)
{
	ocean_assert(frequencies != nullptr && frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u && channels >= 1u);

	const unsigned int pixels = width * height;

	Scalars spatial(channels * pixels);

	constexpr unsigned int frequenciesPaddingElements = 0u;
	constexpr unsigned int spatialPaddingElements = 0u;

	if (worker == nullptr || channels == 1u)
	{
		for (unsigned int c = 0u; c < channels; ++c)
		{
			FourierTransformation::frequencyToSpatial2((const Scalar*)(frequencies + c * pixels), width, height, (Scalar*)(spatial.data() + c * pixels), frequenciesPaddingElements, spatialPaddingElements);
		}
	}
	else
	{
		ocean_assert(worker != nullptr);

		Worker::Functions functions(channels);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			functions[c] = Worker::Function::createStatic(FourierTransformation::frequencyToSpatial2, (const Scalar*)(frequencies + c * pixels), width, height, (Scalar*)(spatial.data() + c * pixels), frequenciesPaddingElements, spatialPaddingElements);
		}

		worker->executeFunctions(functions);

	}

	const unsigned int frameStrideElements = width * channels + framePaddingElements;

	for (unsigned int c = 0u; c < channels; ++c)
	{
		Scalar* spatialChannel = spatial.data() + c * pixels;

		for (unsigned int y = 0u; y < height; ++y)
		{
			uint8_t* frameRowChannel = frame + y * frameStrideElements + c;

			for (unsigned int x = 0u; x < width; ++x)
			{
				*frameRowChannel = uint8_t(minmax<Scalar>(0, Scalar(*spatialChannel++), 255) + Scalar(0.5));
				frameRowChannel += channels;
			}
		}
	}
}

Frame FrequencyAnalysis::magnitudeFrame(const Complex* frequencies, const unsigned int width, const unsigned int height, const unsigned int channels, const Scalar octaves, const bool shift)
{
	ocean_assert(frequencies != nullptr);
	ocean_assert(channels >= 1u);

	const unsigned int pixels = width * height;

	Scalars magnitudes(pixels, 0);

	for (unsigned int c = 0u; c < channels; ++c)
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			const Complex* frequenciesRow = frequencies + c * pixels + y * width;

			Scalar* magnitudeRow = magnitudes.data() + y * width;

			for (unsigned int x = 0u; x < width; ++x)
			{
				const Scalar magnitude = std::abs(frequenciesRow[x]);

				if (c == 0u || magnitudeRow[x] < magnitude)
				{
					magnitudeRow[x] = magnitude;
				}
			}
		}
	}

	if (shift)
	{
		FourierTransformation::shiftHalfDimension2(magnitudes.data(), width, height);
	}

	const Frame magnitudeFrame(FrameType(width, height, FrameType::genericPixelFormat<Scalar, 1u>(), FrameType::ORIGIN_UPPER_LEFT), (const void*)(magnitudes.data()), Frame::CM_USE_KEEP_LAYOUT, 0u /*paddingElements*/);

	Frame normalizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	CV::FrameNormalizer::normalizeLogarithmToUint8(magnitudeFrame, normalizedFrame, octaves);

	return normalizedFrame;
}

Frame FrequencyAnalysis::magnitudeFrame(const Frame& frame, const Scalar octaves, const bool shift, Worker* worker)
{
	ocean_assert(frame);

	Frame yFrame;
	if (!FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, false, worker))
	{
		return Frame();
	}

	Complexes frequencies;
	if (!image2frequencies(yFrame, frequencies, worker))
	{
		return Frame();
	}

	return magnitudeFrame(frequencies.data(), yFrame.width(), yFrame.height(), 1u, octaves, shift);
}

}

}
