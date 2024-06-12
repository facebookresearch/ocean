/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/ImageQuality.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace CV
{

bool ImageQuality::structuralSimilarity8BitPerChannel(const uint8_t* const imageX, const uint8_t* const imageY, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int imageXPaddingElements, const unsigned int imageYPaddingElements, double& meanSSIM, double& meanContrast, Worker* worker)
{
	ocean_assert(imageX != nullptr && imageY != nullptr);
	ocean_assert(width >= 11u && height >= 11u && channels >= 1u);

	meanSSIM = 1.0;
	meanContrast = 1.0;

	constexpr unsigned int filterSize = 11u;
	constexpr float filterSigma = 1.5f;

	if (imageX == nullptr || imageY == nullptr || width < filterSize || height < filterSize || channels == 0u)
	{
		return false;
	}

	// we need 32 bit floating point values to avoid out-of-range problems

	const FrameType floatFrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, channels), FrameType::ORIGIN_UPPER_LEFT);

	Frame xFrame(floatFrameType);
	CV::FrameConverter::cast<uint8_t, float>(imageX, xFrame.data<float>(), width, height, channels, imageXPaddingElements, 0u);
	float* x = xFrame.data<float>();

	Frame yFrame(floatFrameType);
	CV::FrameConverter::cast(imageY, yFrame.data<float>(), width, height, channels, imageYPaddingElements, 0u);
	float* y = yFrame.data<float>();

	// calculate:
	//                   (2 * ux * uy + c1) * (2 * sxy + c2)
	// ssim(x, y)  =  ------------------------------------------
	//                 (ux^2 + uy^2 + c1) * (sx^2 + sy^2 + c2)
	//
	// with:
	// ux = mean(x)
	// uy = mean(y)
	// sx = variance(x)
	// sy = variance(y)
	// sxy = covariance(x, y)

	// let's first calculate pixel- and elements-wise x^2, y^2, and xy

	Frame x2Frame(floatFrameType); // x^2
	Frame y2Frame(floatFrameType); // y^2
	Frame xyFrame(floatFrameType); // x * y

	float* x2 = x2Frame.data<float>();
	float* y2 = y2Frame.data<float>();
	float* xy = xyFrame.data<float>();

	const unsigned int elements = floatFrameType.pixels() * channels;

	for (unsigned int n = 0u; n < elements; ++n)
	{
		x2[n] = x[n] * x[n];
		y2[n] = y[n] * y[n];
		xy[n] = x[n] * y[n];
	}

	// let's calculate mean(x) = ux and mean(y) = uy
	// we approximate the mean by applying a Gaussian blur - an actual window approach may be better

	// we do not need x and y anymore - so that we can reuse the memory for the blurred result (~ the mean of x and y)
	// (CV::FrameFilterGaussian can apply blur filter in place)

	constexpr unsigned int xPaddingElements = 0u;
	constexpr unsigned int yPaddingElements = 0u;

	CV::FrameFilterGaussian::filter<float, float>(x, x, width, height, channels, xPaddingElements, yPaddingElements, filterSize, filterSize, filterSigma, worker);
	CV::FrameFilterGaussian::filter<float, float>(y, y, width, height, channels, xPaddingElements, yPaddingElements, filterSize, filterSize, filterSigma, worker);

	// beware: from now x and y hold mean(x) = ux and mean(y) = uy
	float* ux = x;
	float* uy = y;
	x = nullptr;
	y = nullptr;

	// let's calculate mean(x)^2 = ux^2, uy^2 and ux * uy
	// we do not need ux and uy anymore (once we have the means) - so we can reuse the memory once again
	Frame uxuyFrame(floatFrameType);
	float* const uxuy = uxuyFrame.data<float>();

	for (unsigned int n = 0u; n < elements; ++n)
	{
		uxuy[n] = ux[n] * uy[n]; // first uxuy as ux and uy will change in the next lines
		ux[n] = ux[n] * ux[n];
		uy[n] = uy[n] * uy[n];
	}

	// beware: from now ux and uy hold ux^2 and uy^2
	float* const uxux = ux;
	float* const uyuy = uy;
	ux = nullptr;
	uy = nullptr;

	// let's calculate mean(x^2), mean(y^2) and mean(x*y)
	// we do not need x^2, y^2 and xy anymore (once we have the means) - so we can resuse the memory

	constexpr unsigned int x2PaddingElements = 0u;
	constexpr unsigned int y2PaddingElements = 0u;
	constexpr unsigned int xyPaddingElements = 0u;

	CV::FrameFilterGaussian::filter<float, float>(x2, x2, width, height, channels, x2PaddingElements, x2PaddingElements, filterSize, filterSize, filterSigma, worker);
	CV::FrameFilterGaussian::filter<float, float>(y2, y2, width, height, channels, y2PaddingElements, y2PaddingElements, filterSize, filterSize, filterSigma, worker);
	CV::FrameFilterGaussian::filter<float, float>(xy, xy, width, height, channels, xyPaddingElements, xyPaddingElements, filterSize, filterSize, filterSigma, worker);

	// beware: from now x^2, y^2 and xy hold mean(x^2), mean(y^2) and mean(xy)
	float* ux2 = x2;
	float* uy2 = y2;
	float* uxy = xy;
	x2 = nullptr;
	y2 = nullptr;
	xy = nullptr;

	// let's calculate the variance of x, y, and xy

	for (unsigned int n = 0u; n < elements; ++n)
	{
		ux2[n] -= uxux[n];
		uy2[n] -= uyuy[n];
		uxy[n] -= uxuy[n];
	}

	// beware: from now ux2, uy2 and uxy hold variance(x), variance(y) and variance(xy)
	float* const sx2 = ux2;
	float* const sy2 = uy2;
	float* const sxy = uxy;
	ux2 = nullptr;
	uy2 = nullptr;
	uxy = nullptr;


	// so finally we have all elements we need for our calculation

	const float c1 = 6.5025f;  // = (k * L)^2, k = 0.01, L = 255
	const float c2 = 58.5225f; // = (k * L)^2, k = 0.03, L = 255

	// we do not count pixels at the frame's border
	// so we use the inner core pixels only

	const unsigned int filterSize_2 = filterSize / 2u;

	const unsigned int innerCoreWidth = width - filterSize_2 * 2u;
	const unsigned int innerCoreHeight = height - filterSize_2 * 2u;

	const unsigned int innerCoreStride = innerCoreWidth * channels;

	const unsigned int startOffset = (width * filterSize_2 + filterSize_2) * channels;

	const float* core_sxy = sxy + startOffset;
	const float* core_sx2 = sx2 + startOffset;
	const float* core_sy2 = sy2 + startOffset;
	const float* core_uxux = uxux + startOffset;
	const float* core_uyuy = uyuy + startOffset;
	const float* core_uxuy = uxuy + startOffset;

	double sumContrast = 0.0;
	double sumSSIM = 0.0;

#ifdef OCEAN_DEBUG
	unsigned int debugElementCount = 0u;
#endif

	for (unsigned int row = 0u; row < innerCoreHeight; ++row)
	{
		for (unsigned int n = 0u; n < innerCoreStride; ++n)
		{
			ocean_assert(core_sxy + n < sxy + width * height * channels);

			ocean_assert(NumericF::isNotEqualEps(core_sx2[n] + core_sy2[n] + c2));

			const float contrast = (2.0f * core_sxy[n] + c2) / (core_sx2[n] + core_sy2[n] + c2);
			const float ssim = contrast * (2.0f * core_uxuy[n] + c1) / (core_uxux[n] + core_uyuy[n] + c1);

			sumContrast += contrast;
			sumSSIM += ssim;

#ifdef OCEAN_DEBUG
			debugElementCount++;
#endif
		}

		core_sxy += width * channels;
		core_sx2 += width * channels;
		core_sy2 += width * channels;

		core_uxux += width * channels;
		core_uyuy += width * channels;
		core_uxuy += width * channels;
	}

	const unsigned int innerCoreElements = innerCoreWidth * innerCoreHeight * channels;
	ocean_assert(innerCoreElements != 0u);

#ifdef OCEAN_DEBUG
	ocean_assert(innerCoreElements == debugElementCount);
#endif

	meanSSIM = minmax<double>(0.0, sumSSIM / double(innerCoreElements), 1.0);
	meanContrast = minmax<double>(0.0, sumContrast / double(innerCoreElements), 1.0);

	return true;
}

bool ImageQuality::multiScaleStructuralSimilarity8BitPerChannel(const uint8_t* const imageX, const unsigned char* const imageY, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int imageXPaddingElements, const unsigned int imageYPaddingElements, double& msssim, Worker* worker)
{
	ocean_assert(imageX != nullptr && imageY != nullptr);
	ocean_assert(width >= 11u && height >= 11u);
	ocean_assert(channels >= 1u && channels <= 4u);

	const unsigned int filterSize = 11u;

	if (imageX == nullptr || imageY == nullptr || width < filterSize || height < filterSize || channels == 0u || channels > 4u)
	{
		return false;
	}

	const unsigned int maximalLayers = 5u;
	const double msssiWeights[maximalLayers] = {0.0448, 0.2856, 0.3001, 0.2363, 0.1333};

	const unsigned int layers = min(CV::FramePyramid::idealLayers(width, height, 10u, 10u), maximalLayers);
	ocean_assert(layers >= 1u && layers <= maximalLayers);

	if (layers == 0u)
	{
		return false;
	}

	const CV::FramePyramid pyramidX(imageX, width, height, channels, FrameType::ORIGIN_UPPER_LEFT, layers, imageXPaddingElements, false /*copyFirstLayer*/, worker);
	const CV::FramePyramid pyramidY(imageY, width, height, channels, FrameType::ORIGIN_UPPER_LEFT, layers, imageYPaddingElements, false /*copyFirstLayer*/, worker);

	// Beware: For backwards-compatibility the implementation of this function does not fully match that in the paper "Multi-Scale Structural Similarity for Image Quality Assessment"

	msssim = 1.0;

	for (unsigned int n = 0u; n < layers; ++n)
	{
		double meanSSIM = -1.0;
		double meanContrast = -1.0;

		const Frame& layerX = pyramidX[n];
		const Frame& layerY = pyramidY[n];

		if (!structuralSimilarity8BitPerChannel(layerX.constdata<uint8_t>(), pyramidY[n].constdata<uint8_t>(), layerX.width(), layerX.height(), channels, layerX.paddingElements(), layerY.paddingElements(), meanSSIM, meanContrast, worker))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (n == layers - 1u)
		{
			// we use the ssim index from the coarsest pyramid layer
			msssim *= meanSSIM;
		}
		else
		{
			// we use the contrast value only

			msssim *= NumericD::pow(meanContrast, msssiWeights[n]);
		}
	}

	return true;
}

}

}
