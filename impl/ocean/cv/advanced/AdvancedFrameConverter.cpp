/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/AdvancedFrameConverter.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameFilterLaplace.h"
#include "ocean/cv/FrameFilterMean.h"
#include "ocean/cv/FrameFilterScharr.h"
#include "ocean/cv/FrameFilterScharrMagnitude.h"
#include "ocean/cv/IntegralImage.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

bool AdvancedFrameConverter::convertToYUT24ScharrMagnitude(const Frame& source, Frame& targetFrame, const unsigned int window, Worker* worker)
{
	ocean_assert(window >= 1u && window % 2u == 1u);

	Frame yuvFrame;
	if (!FrameConverter::Comfort::convert(source, FrameType::FORMAT_YUV24, yuvFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame integralFrame;
	if (!createScharrMagnitudeIntegral(yuvFrame, window, integralFrame, worker))
	{
		return false;
	}

	if (!targetFrame.set(FrameType(yuvFrame.width(), yuvFrame.height(), FrameType::FORMAT_YUV24, yuvFrame.pixelOrigin()), true /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	const unsigned int integralBorder = window / 2u;

	const unsigned int integralOffset0 = 2u * integralBorder + 1u;
	const unsigned int integralOffset1 = integralOffset0 * integralFrame.strideElements();
	const unsigned int integralArea = integralOffset0 * integralOffset0;
	ocean_assert(integralArea >= 1u);

	for (unsigned int y = 0u; y < yuvFrame.height(); ++y)
	{
		const uint8_t* yuvRow = yuvFrame.constrow<uint8_t>(y);
		const uint32_t* integralRow = integralFrame.constrow<uint32_t>(y);

		uint8_t* targetRow = targetFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < yuvFrame.width(); ++x)
		{
			const unsigned int magnitude = *integralRow - *(integralRow + integralOffset0) - *(integralRow + integralOffset1) + *(integralRow + integralOffset0 + integralOffset1);

			*targetRow++ = *yuvRow++;
			*targetRow++ = *yuvRow++;
			*targetRow++ = uint8_t(magnitude / integralArea);

			++yuvRow;
			++integralRow;
		}
	}

	return true;
}

bool AdvancedFrameConverter::convertToYUVT32ScharrMagnitude(const Frame& source, Frame& targetFrame, const unsigned int window, Worker* worker)
{
	ocean_assert(window >= 1u && window % 2u == 1u);

	Frame yuvFrame;
	if (!FrameConverter::Comfort::convert(source, FrameType::FORMAT_YUV24, yuvFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame integralFrame;
	if (!createScharrMagnitudeIntegral(yuvFrame, window, integralFrame, worker))
	{
		return false;
	}

	if (!targetFrame.set(FrameType(yuvFrame.width(), yuvFrame.height(), FrameType::FORMAT_YUVT32, yuvFrame.pixelOrigin()), true /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	const unsigned int integralBorder = window / 2u;

	const unsigned int integralOffset0 = 2u * integralBorder + 1u;
	const unsigned int integralOffset1 = integralOffset0 * integralFrame.strideElements();
	const unsigned int integralArea = integralOffset0 * integralOffset0;
	ocean_assert(integralArea >= 1u);

	for (unsigned int y = 0u; y < yuvFrame.height(); ++y)
	{
		const uint8_t* yuvRow = yuvFrame.constrow<uint8_t>(y);
		const uint32_t* integralRow = integralFrame.constrow<uint32_t>(y);

		uint8_t* targetRow = targetFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < yuvFrame.width(); ++x)
		{
			const unsigned int magnitude = *integralRow - *(integralRow + integralOffset0) - *(integralRow + integralOffset1) + *(integralRow + integralOffset0 + integralOffset1);

			*targetRow++ = *yuvRow++;
			*targetRow++ = *yuvRow++;
			*targetRow++ = *yuvRow++;
			*targetRow++ = uint8_t(magnitude / integralArea);

			++integralRow;
		}
	}

	return true;
}

bool AdvancedFrameConverter::convertToRGBT32ScharrMagnitude(const Frame& source, Frame& targetFrame, const unsigned int window, Worker* worker)
{
	ocean_assert(window >= 1u && window % 2u == 1u);

	Frame rgbFrame;
	if (!FrameConverter::Comfort::convert(source, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame yFrame;
	if (!FrameConverter::Comfort::convert(source, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame integralFrame;
	if (!createScharrMagnitudeIntegral(yFrame, window, integralFrame, worker))
	{
		return false;
	}

	if (!targetFrame.set(FrameType(source, FrameType::FORMAT_RGBT32), true, true))
	{
		return false;
	}

	const unsigned int integralBorder = window / 2u;

	const unsigned int integralOffset0 = 2u * integralBorder + 1u;
	const unsigned int integralOffset1 = integralOffset0 * integralFrame.strideElements();
	const unsigned int integralArea = integralOffset0 * integralOffset0;
	ocean_assert(integralArea >= 1u);

	for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
	{
		const uint8_t* rgbRow = rgbFrame.constrow<uint8_t>(y);
		const uint32_t* integralRow = integralFrame.constrow<uint32_t>(y);

		uint8_t* targetRow = targetFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < rgbFrame.width(); ++x)
		{
			const unsigned int magnitude = *integralRow - *(integralRow + integralOffset0) - *(integralRow + integralOffset1) + *(integralRow + integralOffset0 + integralOffset1);

			*targetRow++ = *rgbRow++;
			*targetRow++ = *rgbRow++;
			*targetRow++ = *rgbRow++;
			*targetRow++ = uint8_t(magnitude / integralArea);

			++integralRow;
		}
	}

	return true;
}

bool AdvancedFrameConverter::convertToRGBT32ScharrMagnitude(const Frame& source, Frame& target, Worker* worker)
{
	Frame rgbFrame;
	if (!FrameConverter::Comfort::convert(source, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame yFrame;
	if (!FrameConverter::Comfort::convert(source, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame tFrame(yFrame.frameType());
	FrameFilterScharr::filterMaximumAbsolute8BitPerChannel<uint8_t, 1u>(yFrame.constdata<uint8_t>(), tFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), tFrame.paddingElements(), worker);

	if (!target.set(FrameType(source, FrameType::FORMAT_RGBT32), true, true))
	{
		return false;
	}

	CV::FrameChannels::addLastChannel<unsigned char, 3u>(rgbFrame.constdata<uint8_t>(), tFrame.constdata<uint8_t>(), target.data<uint8_t>(), target.width(), target.height(), CV::FrameChannels::CONVERT_NORMAL, rgbFrame.paddingElements(), tFrame.paddingElements(), target.paddingElements(), worker);

	return true;
}

bool AdvancedFrameConverter::convertToYUVLLL48LaplaceMagnitude(const Frame& source, Frame& target, const unsigned int window, Worker* worker)
{
	ocean_assert(window >= 1u && window % 2u == 1u);

	Frame yuvFrame;
	if (!CV::FrameConverter::Comfort::convert(source, FrameType::FORMAT_YUV24, yuvFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame frame0(FrameType(source, FrameType::FORMAT_Y8));
	Frame frame1(frame0.frameType());
	Frame frame2(frame0.frameType());

	constexpr unsigned int channels = 3u;

	uint8_t* targetFrames[channels] = {frame0.data<uint8_t>(), frame1.data<uint8_t>(), frame2.data<uint8_t>()};
	const unsigned int targetFramesPaddingElements[channels] = {frame0.paddingElements(), frame1.paddingElements(), frame2.paddingElements()};

	CV::FrameChannels::separateTo1Channel(yuvFrame.constdata<uint8_t>(), targetFrames, yuvFrame.width(), yuvFrame.height(), channels, yuvFrame.paddingElements(), targetFramesPaddingElements);

	Frame laplace0(frame0.frameType());
	Frame laplace1(frame0.frameType());
	Frame laplace2(frame0.frameType());
	CV::FrameFilterLaplace::filterNormalizedMagnitude1Channel8Bit(frame0.constdata<uint8_t>(), laplace0.data<uint8_t>(), frame0.width(), frame0.height(), frame0.paddingElements(), laplace0.paddingElements(), worker);
	CV::FrameFilterLaplace::filterNormalizedMagnitude1Channel8Bit(frame1.constdata<uint8_t>(), laplace1.data<uint8_t>(), frame0.width(), frame0.height(), frame1.paddingElements(), laplace1.paddingElements(), worker);
	CV::FrameFilterLaplace::filterNormalizedMagnitude1Channel8Bit(frame2.constdata<uint8_t>(), laplace2.data<uint8_t>(), frame0.width(), frame0.height(), frame2.paddingElements(), laplace2.paddingElements(), worker);

	CV::FrameFilterMean::filter(frame0, window, worker);
	CV::FrameFilterMean::filter(frame1, window, worker);
	CV::FrameFilterMean::filter(frame2, window, worker);

	CV::FrameFilterMean::filter(laplace0, window, worker);
	CV::FrameFilterMean::filter(laplace1, window, worker);
	CV::FrameFilterMean::filter(laplace2, window, worker);

	constexpr unsigned int targetChannels = 6u;

	if (!target.set(FrameType(source, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, targetChannels>()), true, true))
	{
		return false;
	}

	const uint8_t* sourceFrames[targetChannels] = {frame0.constdata<uint8_t>(), frame1.constdata<uint8_t>(), frame2.constdata<uint8_t>(), laplace0.constdata<uint8_t>(), laplace1.constdata<uint8_t>(), laplace2.constdata<uint8_t>()};
	const unsigned int sourceFramesPaddingElements[targetChannels] = {frame0.paddingElements(), frame1.paddingElements(), frame2.paddingElements(), laplace0.paddingElements(), laplace1.paddingElements(), laplace2.paddingElements()};

	CV::FrameChannels::zipChannels(sourceFrames, target.data<uint8_t>(), target.width(), target.height(), targetChannels, sourceFramesPaddingElements, target.paddingElements());

	return true;
}

bool AdvancedFrameConverter::changeToYUT24ScharrMagnitude(Frame& frame, const unsigned int window, Worker* worker)
{
	Frame tmpFrame;
	if (!convertToYUT24ScharrMagnitude(frame, tmpFrame, window, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

bool AdvancedFrameConverter::changeToYUVT32ScharrMagnitude(Frame& frame, const unsigned int window, Worker* worker)
{
	Frame tmpFrame;
	if (!convertToYUVT32ScharrMagnitude(frame, tmpFrame, window, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

bool AdvancedFrameConverter::changeToRGBT32ScharrMagnitude(Frame& frame, const unsigned int window, Worker* worker)
{
	Frame tmpFrame;
	if (!convertToRGBT32ScharrMagnitude(frame, tmpFrame, window, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

bool AdvancedFrameConverter::createScharrMagnitudeIntegral(const Frame& frame, const unsigned int window, Frame& integralFrame, Worker* worker)
{
	ocean_assert(frame.isValid());

	Frame yFrame;
	if (!FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	Frame magnitudeFrame(yFrame.frameType());
	FrameFilterScharr::filterMaximumAbsolute8BitPerChannel<uint8_t, 1u>(yFrame.constdata<uint8_t>(), magnitudeFrame.data<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), magnitudeFrame.paddingElements(), worker);

	const unsigned int integralBorder = window / 2u;
	const unsigned int integralWidth = yFrame.width() + 2u * integralBorder + 1u;
	const unsigned int integralHeight = yFrame.height() + 2u * integralBorder + 1u;

	if (!integralFrame.set(FrameType(integralWidth, integralHeight, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), true, true))
	{
		return false;
	}

	IntegralImage::createBorderedImageMirror<uint8_t, uint32_t, 1u>(magnitudeFrame.constdata<uint8_t>(), integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), integralBorder, magnitudeFrame.paddingElements(), integralFrame.paddingElements());

	return true;
}

}

}

}
