// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"

#include "ocean/math/Approximation.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Blob
{

const BlobFeatureDescriptor::LookupTable& BlobFeatureDescriptor::lookupTable()
{
	static LookupTable lookupTable;
	return lookupTable;
}

void BlobFeatureDescriptor::calculateOrientation(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeature& feature, const bool forceCalculation)
{
	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert_and_suppress_unused(pixelOrigin == FrameType::ORIGIN_UPPER_LEFT, pixelOrigin);
	ocean_assert(feature.observation().x() >= Scalar(0) && feature.observation().x() < Scalar(width));
	ocean_assert(feature.observation().y() >= Scalar(0) && feature.observation().y() < Scalar(height));

	if (type == feature.orientationType() && !forceCalculation)
		return;

	/**
	 * We do not use a standard haar wavelet, because the wavelet does not have a unique center pixel.
	 * We use the following horizontal filter instead:
	 *  -----------
	 * |    | |    |
	 * |    | |    |
	 * | -1 |X| +1 |
	 * |    | |    |
	 * |    | |    |
	 *  -----------
	 * The filter is quadratic and has size of 2 * 1.5s + 1 pixel resulting in an odd filter size.
	 * X defines the center position.
	 */

	/**
	 * Our vertical filter::
	 *  ---------
	 * |         |
	 * |    -1   |
	 * |---------|
	 * |    X    |
	 * |---------|
	 * |    +1   |
	 * |         |
	 *  ---------
	 * The filter is quadratic and has size of 2 * 1.5s + 1 pixel resulting in an odd filter size.
	 * X defines the center position.
	 */

	const int integralWidth = width + 1;

	const int filterSizeHalf = (unsigned int)(feature.scale() * 1.5 + 0.5);
	const int filterSize = filterSizeHalf * 2 + 1;

	switch (type)
	{
		case BlobFeature::ORIENTATION_SLIDING_WINDOW:
		{
			Vector2 slides[24];
			std::fill_n(slides, 24, Vector2(0, 0));

			for (int sampleX = -3; sampleX <= 3; ++sampleX)
			{
				for (int sampleY = -3; sampleY <= 3; ++sampleY)
				{
					const Scalar filterX = feature.observation().x() + Scalar(sampleX) * feature.scale();
					const Scalar filterY = feature.observation().y() + Scalar(sampleY) * feature.scale();

					const int x = int(filterX + Scalar(0.5));
					const int y = int(filterY + Scalar(0.5));

					const int left = x - filterSizeHalf;
					const int top = y - filterSizeHalf;

					if (left < 0 || (x + filterSizeHalf) >= int(width) || top < 0 || (y + filterSizeHalf) >= int(height))
					{
						continue;
					}

					const uint32_t* topLeftIntegral = linedIntegralImage + top * integralWidth + left;
					const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

					// horizontal filter
					const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
														+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

					// vertical filter
					unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
					topLeftIntegral += filterSizeHalf * integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					topLeftIntegral += integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

					const Scalar normalization = lookupTable().gaussianWeights_03_2[abs(sampleX)][abs(sampleY)];

					if (horizontalResponse != 0 || verticalResponse != 0)
					{
#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
						const Scalar angle = max(-Numeric::pi(), min(Numeric::pi(), Approximation::atan2(-Scalar(int(verticalResponse)), Scalar(int(horizontalResponse)))));
#else
						const Scalar angle = Numeric::atan2(-Scalar(int(verticalResponse)), Scalar(int(horizontalResponse)));
#endif

						ocean_assert_accuracy(angle >= -Numeric::pi() && angle <= Numeric::pi());

						// (angleRad * 180 / PI) / 15 = 3.8197186342054880584532103209403
						const int slideIndex = angle > 0 ? int(angle * Scalar(3.819718634205488)) : 23 + int(angle * Scalar(3.819718634205488));
						ocean_assert(slideIndex >= 0 && slideIndex < 24);

						slides[slideIndex] += Vector2(Scalar(int(horizontalResponse)) * normalization, Scalar(int(verticalResponse)) * normalization);
					}
				}
			}

			Vector2 maxTotalResponseVector(0, 0);

			for (unsigned int n = 0; n < 21; ++n)
			{
				const Vector2 totalResponseVector(slides[n] + slides[n + 1] + slides[n + 2] + slides[n + 3]);

				if (totalResponseVector.sqr() > maxTotalResponseVector.sqr())
				{
					maxTotalResponseVector = totalResponseVector;
				}
			}

			const Vector2 totalResponseVector21_22_23_0(slides[21] + slides[22] + slides[23] + slides[0]);
			if (totalResponseVector21_22_23_0.sqr() > maxTotalResponseVector.sqr())
			{
				maxTotalResponseVector = totalResponseVector21_22_23_0;
			}

			const Vector2 totalResponseVector22_23_0_1(slides[22] + slides[23] + slides[0] + slides[1]);
			if (totalResponseVector22_23_0_1.sqr() > maxTotalResponseVector.sqr())
			{
				maxTotalResponseVector = totalResponseVector22_23_0_1;
			}

			const Vector2 totalResponseVector23_0_1_2(slides[23] + slides[0] + slides[1] + slides[2]);
			if (totalResponseVector23_0_1_2.sqr() > maxTotalResponseVector.sqr())
			{
				maxTotalResponseVector = totalResponseVector23_0_1_2;
			}

			Scalar orientation = Scalar(0);

			if (!maxTotalResponseVector.isNull())
			{
#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
				orientation = Approximation::atan2(-maxTotalResponseVector.y(), maxTotalResponseVector.x());
#else
				orientation = Numeric::atan2(-maxTotalResponseVector.y(), maxTotalResponseVector.x());
#endif
			}

			ocean_assert_accuracy(Numeric::isInsideRange(-Numeric::pi(), orientation, Numeric::pi()));

			// adjust the orientation to the range [0, 2PI)
			if (orientation >= 0)
			{
				feature.setOrientation(orientation);
			}
			else
			{
				feature.setOrientation(Numeric::pi2() + orientation);
			}

			feature.setOrientationType(BlobFeature::ORIENTATION_SLIDING_WINDOW);
			break;
		}

		case BlobFeature::ORIENTATION_SUMMED_ORIENTATION:
		{

			Vector2 sumTotalResponseVector(0, 0);

			for (int sampleX = -3; sampleX <= 3; ++sampleX)
			{
				for (int sampleY = -3; sampleY <= 3; ++sampleY)
				{
					const Scalar filterX = feature.observation().x() + Scalar(sampleX) * feature.scale();
					const Scalar filterY = feature.observation().y() + Scalar(sampleY) * feature.scale();

					const int x = int(filterX + Scalar(0.5));
					const int y = int(filterY + Scalar(0.5));

					const int left = x - filterSizeHalf;
					const int right = x + filterSizeHalf;
					const int top = y - filterSizeHalf;
					const int bottom = y + filterSizeHalf;

					if (left < 0 || right >= int(width) || top < 0 || bottom >= int(height))
					{
						continue;
					}

					const uint32_t* topLeftIntegral = linedIntegralImage + top * integralWidth + left;
					const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

					// horizontal filter
					const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
														+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

					// vertical filter
					unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
					topLeftIntegral += filterSizeHalf * integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					topLeftIntegral += integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

					const Scalar normalization = lookupTable().gaussianWeights_03_2[abs(sampleX)][abs(sampleY)];

					if (horizontalResponse != 0 || verticalResponse != 0)
					{
						sumTotalResponseVector += Vector2(Scalar(int(horizontalResponse)) * normalization, Scalar(int(verticalResponse)) * normalization);
					}
				}
			}

			Scalar orientation = Scalar(0);

			if (!sumTotalResponseVector.isNull())
			{
#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE
				orientation = Approximation::atan2(-sumTotalResponseVector.y(), sumTotalResponseVector.x());
#else
				orientation = Numeric::atan2(-sumTotalResponseVector.y(), sumTotalResponseVector.x());
#endif
			}

			ocean_assert_accuracy(orientation >= -Numeric::pi() && orientation <= Numeric::pi());

			// adjust the orientation to the range [0, 2PI)
			if (orientation >= 0)
			{
				feature.setOrientation(orientation);
			}
			else
			{
				ocean_assert_accuracy(Numeric::pi2() + orientation >= 0 && Numeric::pi2() + orientation < Numeric::pi2());
				feature.setOrientation(Numeric::pi2() + orientation);
			}

			feature.setOrientationType(BlobFeature::ORIENTATION_SUMMED_ORIENTATION);
			break;
		}

		default:
			ocean_assert(false && "Invalid orientation type!");
			break;
	}
}

void BlobFeatureDescriptor::calculateDescriptor(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeature& feature, const bool forceCalculation)
{
	if (feature.descriptorType() == BlobFeature::DESCRIPTOR_ORIENTED && !forceCalculation)
	{
		return;
	}

	ocean_assert_and_suppress_unused(pixelOrigin == FrameType::ORIGIN_UPPER_LEFT, pixelOrigin);

	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(feature.observation().x() >= Scalar(0) && feature.observation().x() < Scalar(width));
	ocean_assert(feature.observation().y() >= Scalar(0) && feature.observation().y() < Scalar(height));

	/**
	 * Counter-clockwise rotation in a righthanded coordinate system for a point p = (x, y):
	 * An image with bottom up data storage (the first pixel starts at the lower left corner) has a righthanded coordinate system.
	 * x' = cos * x - sin * y
	 * y' = sin * x + cos * y
	 *
	 * Inverse rotation:
	 * x = cos * x' + sin * y'
	 * y = -sin * x' + cos * y
	 *
	 *
	 *
	 *
	 * Counter-clockwise rotation in a lefthanded coordinate system for a point p = (x, y):
	 * An image with top-down data storage (the first pixel starts at the upper left corner) has a lefthanded coordinate system.
	 * x' = cos * x + sin * y
	 * y' = -sin * x + cos * y
	 *
	 * Inverse rotation:
	 * x = cos * x' - sin * y'
	 * y = sin * x' + cos * y'
	 */

	const int integralWidth = int(width) + 1;

	//const Scalar cosValue = Numeric::cos(feature.orientation());
	//const Scalar sinValue = Numeric::sin(feature.orientation());

	const unsigned int angleDeg = (unsigned int)(Numeric::rad2deg(Numeric::angleAdjustPositive(feature.orientation())) + Scalar(0.5));
	ocean_assert_accuracy(angleDeg >= 0u && angleDeg <= 360u);
	const Scalar cosValue = lookupTable().cosValues[angleDeg];
	const Scalar sinValue = lookupTable().sinValues[angleDeg];

	// filterSize defined by the feature's scale, however it must be even
	const int filterSizeHalf = (unsigned int)((feature.scale() * Scalar(1.3333333333333333333333) + 0.5));
	ocean_assert(filterSizeHalf >= 1);
	const int filterSize = filterSizeHalf * 2 + 1;

	Scalar norm = 0;

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	ocean_assert(feature.descriptor().elements() == 36);

	Scalar descriptorElements[36];
	Scalar* descriptorElement = descriptorElements;

	// forward rotation of the x and y axis
	const Vector2 sHorizontal(cosValue * Scalar(1.3333333333333333333333) * feature.scale(), -sinValue * Scalar(1.3333333333333333333333) * feature.scale());
	const Vector2 sVertical(-sHorizontal.y(), sHorizontal.x());
	ocean_assert_accuracy(Numeric::isEqual(sHorizontal.angle(sVertical), Numeric::pi_2()));

	for (int yBig = -1; yBig <= 1; ++yBig)
	{
		for (int xBig = -1; xBig <= 1; ++xBig)
		{
			Scalar dx = 0;
			Scalar dy = 0;
			Scalar adx = 0;
			Scalar ady = 0;

			for (int x = 0; x < 5; ++x)
			{
				for (int y = 0; y < 5; ++y)
				{
					const int xOffset = (xBig * 4) + x - 2;
					const int yOffset = (yBig * 4) + y - 2;

					const Vector2 exactPosition = feature.observation() + sHorizontal * Scalar(xOffset) + sVertical * Scalar(yOffset);

					const int filterX = int(exactPosition.x() + Scalar(0.5));
					const int filterY = int(exactPosition.y() + Scalar(0.5));

					const int left = filterX - filterSizeHalf;
					const int top = filterY - filterSizeHalf;

					if (left < 0 || (filterX + filterSizeHalf) >= int(width) || top < 0 || (filterY + filterSizeHalf) >= int(height))
					{
						continue;
					}

					const uint32_t* topLeftIntegral = linedIntegralImage + top * integralWidth + left;
					const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

					// horizontal filter
					const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
														+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

					// vertical filter
					unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
					topLeftIntegral += filterSizeHalf * integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					topLeftIntegral += integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

					ocean_assert(abs(xOffset) <= 6 && abs(yOffset) <= 6);
					const Scalar normalization = lookupTable().gaussianWeights_06_3[abs(xOffset)][abs(yOffset)];
					const Scalar normalizedHorizontalResponse = normalization * Scalar(int(horizontalResponse));
					const Scalar normalizedVerticalResponse = normalization * Scalar(int(verticalResponse));

					// inverse rotation:
					const Scalar rotatedHorizontalResponse = cosValue * normalizedHorizontalResponse - sinValue * normalizedVerticalResponse;
					const Scalar rotatedVerticalResponse = sinValue * normalizedHorizontalResponse + cosValue * normalizedVerticalResponse;

					dx += rotatedHorizontalResponse;
					adx += Numeric::abs(rotatedHorizontalResponse);

					dy += rotatedVerticalResponse;
					ady += Numeric::abs(rotatedVerticalResponse);
				}
			}

			*descriptorElement++ = dx;
			norm += dx * dx;

			*descriptorElement++ = dy;
			norm += dy * dy;

			*descriptorElement++ = adx;
			norm += adx * adx;

			*descriptorElement++ = ady;
			norm += ady * ady;
		}
	}

	const Scalar length = Numeric::sqrt(norm);
	ocean_assert(Numeric::isNotEqualEps(length));

	const Scalar factor = BlobDescriptor::descriptorNormalization() / length;

	for (unsigned int n = 0; n < 36; ++n)
	{
		feature.descriptor()[n] = BlobDescriptor::DescriptorSSD(descriptorElements[n] * factor);
	}

#else // OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	ocean_assert(feature.descriptor().elements() == 64);

	Scalar descriptorElements[64];
	Scalar* descriptorElement = descriptorElements;

	// forward rotation of the x and y axis
	const Vector2 sHorizontal(cosValue * Scalar(1.2) * feature.scale(), -sinValue * Scalar(1.2) * feature.scale());
	const Vector2 sVertical(-sHorizontal.y(), sHorizontal.x());
	ocean_assert_accuracy(Numeric::isEqual(sHorizontal.angle(sVertical), Numeric::pi_2()));

	for (int yBig = -2; yBig < 2; ++yBig)
	{
		for (int xBig = -2; xBig < 2; ++xBig)
		{
			Scalar dx = 0;
			Scalar dy = 0;
			Scalar adx = 0;
			Scalar ady = 0;

			for (int x = 0; x < 5; ++x)
			{
				for (int y = 0; y < 5; ++y)
				{
					const int xOffset = (xBig * 4) + x;
					const int yOffset = (yBig * 4) + y;

					const Vector2 exactPosition = feature.observation() + sHorizontal * Scalar(xOffset) + sVertical * Scalar(yOffset);

					const int filterX = int(exactPosition.x() + Scalar(0.5));
					const int filterY = int(exactPosition.y() + Scalar(0.5));

					const int left = filterX - filterSizeHalf;
					const int top = filterY - filterSizeHalf;

					if (left < 0 || (filterX + filterSizeHalf) >= int(width) || top < 0 || (filterY + filterSizeHalf) >= int(height))
					{
						continue;
					}

					const uint32_t* topLeftIntegral = linedIntegralImage + top * integralWidth + left;
					const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

					// horizontal filter
					const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
														+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

					// vertical filter
					unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
					topLeftIntegral += filterSizeHalf * integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					topLeftIntegral += integralWidth;
					verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
					verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

					const Scalar normalization = lookupTable().gaussianWeights_08_3[abs(xOffset)][abs(yOffset)];
					const Scalar normalizedHorizontalResponse = normalization * int(horizontalResponse);
					const Scalar normalizedVerticalResponse = normalization * int(verticalResponse);

					// inverse rotation:
					const Scalar rotatedHorizontalResponse = cosValue * normalizedHorizontalResponse - sinValue * normalizedVerticalResponse;
					const Scalar rotatedVerticalResponse = sinValue * normalizedHorizontalResponse + cosValue * normalizedVerticalResponse;

					dx += rotatedHorizontalResponse;
					adx += Numeric::abs(rotatedHorizontalResponse);

					dy += rotatedVerticalResponse;
					ady += Numeric::abs(rotatedVerticalResponse);
				}
			}

			*descriptorElement++ = dx;
			norm += dx * dx;

			*descriptorElement++ = dy;
			norm += dy * dy;

			*descriptorElement++ = adx;
			norm += adx * adx;

			*descriptorElement++ = ady;
			norm += ady * ady;
		}
	}

	const Scalar length = Numeric::sqrt(norm);
	ocean_assert(Numeric::isNotEqualEps(length));

	const Scalar factor = BlobDescriptor::descriptorNormalization() / length;

	for (unsigned int n = 0; n < 64; ++n)
	{
		feature.descriptor()[n] = BlobDescriptor::DescriptorSSD(descriptorElements[n] * factor);
	}

#endif // OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	feature.setDescriptorType(BlobFeature::DESCRIPTOR_ORIENTED);
}

void BlobFeatureDescriptor::calculateNotOrientedDescriptor(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeature& feature, const bool forceCalculation)
{
	if (feature.descriptorType() == BlobFeature::DESCRIPTOR_NOT_ORIENTED && !forceCalculation)
	{
		return;
	}

	ocean_assert_and_suppress_unused(pixelOrigin == FrameType::ORIGIN_UPPER_LEFT, pixelOrigin);

	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(feature.observation().x() >= Scalar(0) && feature.observation().x() < Scalar(width));
	ocean_assert(feature.observation().y() >= Scalar(0) && feature.observation().y() < Scalar(height));

	const int integralWidth = int(width) + 1;

	Scalar norm = 0;

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	ocean_assert(feature.descriptor().elements() == 36);

	Scalar descriptorElements[36];
	Scalar* descriptorElement = descriptorElements;

	// filterSize defined by the feature's scale, however it must be even
	const int filterSizeHalf = (unsigned int)((feature.scale() * Scalar(1.3333333333333333333333) + 0.5));
	ocean_assert(filterSizeHalf >= 1);
	const int filterSize = filterSizeHalf * 2 + 1;

	const int filterAreaHalf = 7 * filterSizeHalf;
	const int filterLeft = int(feature.observation().x() + 0.5) - filterAreaHalf;
	const int filterRight = int(feature.observation().x() + 0.5) + filterAreaHalf;
	const int filterTop = int(feature.observation().y() + 0.5) - filterAreaHalf;
	const int filterBottom = int(feature.observation().y() + 0.5) + filterAreaHalf;

	if (filterLeft < 0 || filterRight >= int(width) || filterTop < 0 || filterBottom >= int(height))
	{
		for (int yBig = -1; yBig <= 1; ++yBig)
		{
			for (int xBig = -1; xBig <= 1; ++xBig)
			{
				Scalar dx = 0;
				Scalar dy = 0;
				Scalar adx = 0;
				Scalar ady = 0;

				for (int x = 0; x < 5; ++x)
				{
					for (int y = 0; y < 5; ++y)
					{
						const int xOffset = (xBig * 4) + x - 2;
						const int yOffset = (yBig * 4) + y - 2;

						const Vector2 exactPosition(feature.observation().x() + Scalar(xOffset) * feature.scale(), feature.observation().y() + Scalar(yOffset) * feature.scale());

						const int filterX = int(exactPosition.x() + 0.5);
						const int filterY = int(exactPosition.y() + 0.5);

						const int left = filterX - filterSizeHalf;
						const int top = filterY - filterSizeHalf;

						if (left < 0 || (filterX + filterSizeHalf) >= int(width) || top < 0 || (filterY + filterSizeHalf) >= int(height))
							continue;

						const uint32_t* topLeftIntegral = linedIntegralImage + top * integralWidth + left;
						const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

						// horizontal filter
						const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
															+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

						// vertical filter
						unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
						topLeftIntegral += filterSizeHalf * integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						topLeftIntegral += integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

						ocean_assert(abs(xOffset) <= 6 && abs(yOffset) <= 6);
						const Scalar normalization = lookupTable().gaussianWeights_06_3[abs(xOffset)][abs(yOffset)];
						const Scalar normalizedHorizontalResponse = normalization * Scalar(int(horizontalResponse));
						const Scalar normalizedVerticalResponse = normalization * Scalar(int(verticalResponse));

						dx += normalizedHorizontalResponse;
						adx += Numeric::abs(normalizedHorizontalResponse);

						dy += normalizedVerticalResponse;
						ady += Numeric::abs(normalizedVerticalResponse);
					}
				}

				*descriptorElement++ = dx;
				norm += dx * dx;

				*descriptorElement++ = dy;
				norm += dy * dy;

				*descriptorElement++ = adx;
				norm += adx * adx;

				*descriptorElement++ = ady;
				norm += ady * ady;
			}
		}
	}
	else // the entire filter area is inside the image and therefore we do not have to consider bounding problems
	{
		const uint32_t* integralTop = linedIntegralImage + filterTop * integralWidth + filterLeft;
		const Scalar* weight = lookupTable().gaussianWeights_225_3;

		const uint32_t* const integralBoxVerticalEnd = integralTop + integralWidth * 12 * filterSizeHalf;

		const unsigned int filterJump = (integralWidth - 5) * filterSizeHalf;
		const unsigned int boxJup = (integralWidth * 4 - 12) * filterSizeHalf;

		unsigned int outerY = 0;

		while (integralTop != integralBoxVerticalEnd)
		{
			ocean_assert(++outerY <= 3);
			ocean_assert(integralTop < integralBoxVerticalEnd);

			const uint32_t* integralBoxHorizontalEnd = integralTop + 12 * filterSizeHalf;

			unsigned int outerX = 0;

			while (integralTop != integralBoxHorizontalEnd)
			{
				ocean_assert(++outerX <= 3);
				ocean_assert(integralTop < integralBoxHorizontalEnd);

				Scalar dx = 0;
				Scalar adx = 0;
				Scalar dy = 0;
				Scalar ady = 0;

				const uint32_t* const integralFilterVerticalEnd = integralTop + integralWidth * 5 * filterSizeHalf;

				unsigned int innerY = 0;

				while (integralTop != integralFilterVerticalEnd)
				{
					ocean_assert(++innerY <= 5);
					ocean_assert(integralTop < integralFilterVerticalEnd);

					const uint32_t* const integralFilterHorizontalEnd = integralTop + 5 * filterSizeHalf;

					unsigned int innerX = 0;

					while (integralTop != integralFilterHorizontalEnd)
					{
						ocean_assert(++innerX <= 5);
						ocean_assert(integralTop < integralFilterHorizontalEnd);

						const uint32_t* topLeftIntegral = integralTop;
						const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

						// horizontal filter
						const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
															+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

						// vertical filter
						unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
						topLeftIntegral += filterSizeHalf * integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						topLeftIntegral += integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

						const Scalar normalizedHorizontalResponse = Scalar(int(horizontalResponse)) * *weight;
						const Scalar normalizedVerticalResponse = Scalar(int(verticalResponse)) * *weight;

						dx += normalizedHorizontalResponse;
						adx += Numeric::abs(normalizedHorizontalResponse);

						dy += normalizedVerticalResponse;
						ady += Numeric::abs(normalizedVerticalResponse);

						integralTop += filterSizeHalf;

						weight++;
					}

					ocean_assert_and_suppress_unused(innerX == 5, innerX);

					integralTop += filterJump;
				}

				ocean_assert_and_suppress_unused(innerY == 5, innerY);

				*descriptorElement++ = dx;
				norm += dx * dx;

				*descriptorElement++ = dy;
				norm += dy * dy;

				*descriptorElement++ = adx;
				norm += adx * adx;

				*descriptorElement++ = ady;
				norm += ady * ady;

				integralTop -= (5 * filterSizeHalf) * integralWidth - 4 * filterSizeHalf;
			}

			ocean_assert_and_suppress_unused(outerX == 3, outerX);

			integralTop += boxJup;
		}

		ocean_assert_and_suppress_unused(outerY == 3, outerY);
	}

	const Scalar length = Numeric::sqrt(norm);
	ocean_assert(Numeric::isNotEqualEps(length));

	const Scalar factor = BlobDescriptor::descriptorNormalization() / length;

	for (unsigned int n = 0; n < 36; ++n)
	{
		feature.descriptor()[n] = BlobDescriptor::DescriptorSSD(descriptorElements[n] * factor);
	}

#else // OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	ocean_assert(feature.descriptor().elements() == 64);

	Scalar descriptorElements[64];
	Scalar* descriptorElement = descriptorElements;

	// filterSize defined by the feature's scale, however it must be even
	const int filterSizeHalf = (unsigned int)((feature.scale() * Scalar(1.2) + 0.5));
	ocean_assert(filterSizeHalf >= 2);
	const int filterSize = filterSizeHalf * 2 + 1;

	const int filterAreaHalf = 9 * filterSizeHalf;
	const int filterLeft = int(feature.observation().x() + 0.5) - filterAreaHalf;
	const int filterRight = int(feature.observation().x() + 0.5) + filterAreaHalf;
	const int filterTop = int(feature.observation().y() + 0.5) - filterAreaHalf;
	const int filterBottom = int(feature.observation().y() + 0.5) + filterAreaHalf;

	if (filterLeft < 0 || filterRight >= int(width) || filterTop < 0 || filterBottom >= int(height))
	{
		for (int yBig = -2; yBig < 2; ++yBig)
		{
			for (int xBig = -2; xBig < 2; ++xBig)
			{
				Scalar dx = 0;
				Scalar dy = 0;
				Scalar adx = 0;
				Scalar ady = 0;

				for (int x = 0; x < 5; ++x)
				{
					for (int y = 0; y < 5; ++y)
					{
						const int xOffset = (xBig * 4) + x;
						const int yOffset = (yBig * 4) + y;

						const Vector2 exactPosition(feature.observation().x() + xOffset * feature.scale(), feature.observation().y() + Scalar(yOffset) * feature.scale());

						const int filterX = int(exactPosition.x() + 0.5);
						const int filterY = int(exactPosition.y() + 0.5);

						const int left = filterX - filterSizeHalf;
						const int top = filterY - filterSizeHalf;

						if (left < 0 || (filterX + filterSizeHalf) >= int(width) || top < 0 || (filterY + filterSizeHalf) >= int(height))
							continue;

						const uint32_t* topLeftIntegral = linedIntegralImage + top * integralWidth + left;
						const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

						// horizontal filter
						const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
															+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

						// vertical filter
						unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
						topLeftIntegral += filterSizeHalf * integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						topLeftIntegral += integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

						ocean_assert(abs(xOffset) <= 8 && abs(yOffset) <= 8);
						const Scalar normalization = lookupTable().gaussianWeights_08_3[abs(xOffset)][abs(yOffset)];
						const Scalar normalizedHorizontalResponse = normalization * int(horizontalResponse);
						const Scalar normalizedVerticalResponse = normalization * int(verticalResponse);

						dx += normalizedHorizontalResponse;
						adx += Numeric::abs(normalizedHorizontalResponse);

						dy += normalizedVerticalResponse;
						ady += Numeric::abs(normalizedVerticalResponse);
					}
				}

				*descriptorElement++ = dx;
				norm += dx * dx;

				*descriptorElement++ = dy;
				norm += dy * dy;

				*descriptorElement++ = adx;
				norm += adx * adx;

				*descriptorElement++ = ady;
				norm += ady * ady;
			}
		}
	}
	else // the entire filter area is inside the image and therefore we do not have to consider bounding problems
	{
		const uint32_t* integralTop = linedIntegralImage + filterTop * integralWidth + filterLeft;
		const Scalar* weight = lookupTable().gaussianWeights_400_3;

		const uint32_t* const integralBoxVerticalEnd = integralTop + integralWidth * 16 * filterSizeHalf;

		const unsigned int filterJump = (integralWidth - 5) * filterSizeHalf;
		const unsigned int boxJup = (integralWidth * 4 - 16) * filterSizeHalf;

		unsigned int outerY = 0;

		while (integralTop != integralBoxVerticalEnd)
		{
			ocean_assert(++outerY <= 4);
			ocean_assert(integralTop < integralBoxVerticalEnd);

			const uint32_t* integralBoxHorizontalEnd = integralTop + 16 * filterSizeHalf;

			unsigned int outerX = 0;

			while (integralTop != integralBoxHorizontalEnd)
			{
				ocean_assert(++outerX <= 4);
				ocean_assert(integralTop < integralBoxHorizontalEnd);

				Scalar dx = 0;
				Scalar adx = 0;
				Scalar dy = 0;
				Scalar ady = 0;

				const uint32_t* const integralFilterVerticalEnd = integralTop + integralWidth * 5 * filterSizeHalf;

				unsigned int innerY = 0;

				while (integralTop != integralFilterVerticalEnd)
				{
					ocean_assert(++innerY <= 5);
					ocean_assert(integralTop < integralFilterVerticalEnd);

					const uint32_t* const integralFilterHorizontalEnd = integralTop + 5 * filterSizeHalf;

					unsigned int innerX = 0;

					while (integralTop != integralFilterHorizontalEnd)
					{
						ocean_assert(++innerX <= 5);
						ocean_assert(integralTop < integralFilterHorizontalEnd);

						const uint32_t* topLeftIntegral = integralTop;
						const uint32_t* const bottomLeftIntegral = topLeftIntegral + filterSize * integralWidth;

						// horizontal filter
						const unsigned int horizontalResponse = *(topLeftIntegral + filterSizeHalf) - *topLeftIntegral + *(topLeftIntegral + filterSizeHalf + 1) - *(topLeftIntegral + filterSize)
															+ *bottomLeftIntegral - *(bottomLeftIntegral + filterSizeHalf) - *(bottomLeftIntegral + filterSizeHalf + 1) + *(bottomLeftIntegral + filterSize);

						// vertical filter
						unsigned int verticalResponse = *(topLeftIntegral + filterSize) - *topLeftIntegral;
						topLeftIntegral += filterSizeHalf * integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						topLeftIntegral += integralWidth;
						verticalResponse += *topLeftIntegral - *(topLeftIntegral + filterSize);
						verticalResponse += *(bottomLeftIntegral + filterSize) - *bottomLeftIntegral;

						const Scalar normalizedHorizontalResponse = int(horizontalResponse) * *weight;
						const Scalar normalizedVerticalResponse = int(verticalResponse) * *weight;

						dx += normalizedHorizontalResponse;
						adx += Numeric::abs(normalizedHorizontalResponse);

						dy += normalizedVerticalResponse;
						ady += Numeric::abs(normalizedVerticalResponse);

						integralTop += filterSizeHalf;

						weight++;
					}

					ocean_assert(innerX == 5);

					integralTop += filterJump;
				}

				ocean_assert(innerY == 5);

				*descriptorElement++ = dx;
				norm += dx * dx;

				*descriptorElement++ = dy;
				norm += dy * dy;

				*descriptorElement++ = adx;
				norm += adx * adx;

				*descriptorElement++ = ady;
				norm += ady * ady;

				integralTop -= (5 * filterSizeHalf) * integralWidth - 4 * filterSizeHalf;
			}

			ocean_assert(outerX == 4);

			integralTop += boxJup;
		}

		ocean_assert(outerY == 4);
	}

	const Scalar length = Numeric::sqrt(norm);
	ocean_assert(Numeric::isNotEqualEps(length));

	const Scalar factor = BlobDescriptor::descriptorNormalization() / length;

	for (unsigned int n = 0; n < 64; ++n)
		feature.descriptor()[n] = BlobDescriptor::DescriptorSSD(descriptorElements[n] * factor);

#endif // OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	feature.setDescriptorType(BlobFeature::DESCRIPTOR_NOT_ORIENTED);
	feature.setOrientationType(BlobFeature::ORIENTATION_NOT_ORIENTED);
}

void BlobFeatureDescriptor::calculateOrientationsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures)
{
	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(features != nullptr);

	ocean_assert(firstFeature + numberFeatures <= features->size());

	for (unsigned int n = firstFeature; n < firstFeature + numberFeatures; ++n)
	{
		calculateOrientation(linedIntegralImage, width, height, pixelOrigin, type, (*features)[n], forceCalculation);
	}
}

void BlobFeatureDescriptor::calculateOrientationsAndDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures)
{
	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(features != nullptr);

	ocean_assert(firstFeature + numberFeatures <= features->size());

	for (unsigned int n = firstFeature; n < firstFeature + numberFeatures; ++n)
	{
		calculateOrientation(linedIntegralImage, width, height, pixelOrigin, type, (*features)[n], forceCalculation);
		calculateDescriptor(linedIntegralImage, width, height, pixelOrigin, (*features)[n], forceCalculation);
	}
}

void BlobFeatureDescriptor::calculateOrientationsAndDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures* features, const FeatureIndices* featureIndices, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures)
{
	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(features != nullptr);
	ocean_assert(featureIndices != nullptr);

	ocean_assert(firstFeature + numberFeatures <= featureIndices->size());

	for (unsigned int n = firstFeature; n < firstFeature + numberFeatures; ++n)
	{
		const unsigned int realIndex = (*featureIndices)[n];
		ocean_assert(realIndex < features->size());

		calculateOrientation(linedIntegralImage, width, height, pixelOrigin, type, (*features)[realIndex], forceCalculation);
		calculateDescriptor(linedIntegralImage, width, height, pixelOrigin, (*features)[realIndex], forceCalculation);
	}
}

void BlobFeatureDescriptor::calculateDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures)
{
	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(features != nullptr);

	ocean_assert(firstFeature + numberFeatures <= features->size());

	for (unsigned int n = firstFeature; n < firstFeature + numberFeatures; ++n)
		calculateDescriptor(linedIntegralImage, width, height, pixelOrigin, (*features)[n], forceCalculation);
}

void BlobFeatureDescriptor::calculateNotOrientedDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures)
{
	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(features != nullptr);

	ocean_assert(firstFeature + numberFeatures <= features->size());

	for (unsigned int n = firstFeature; n < firstFeature + numberFeatures; ++n)
	{
		calculateNotOrientedDescriptor(linedIntegralImage, width, height, pixelOrigin, (*features)[n], forceCalculation);
	}
}

void BlobFeatureDescriptor::calculateNotOrientedDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures* features, const FeatureIndices* featureIndices, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures)
{
	ocean_assert(linedIntegralImage != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(features != nullptr);
	ocean_assert(featureIndices != nullptr);

	ocean_assert(firstFeature + numberFeatures <= featureIndices->size());

	for (unsigned int n = firstFeature; n < firstFeature + numberFeatures; ++n)
	{
		const unsigned int realIndex = (*featureIndices)[n];
		ocean_assert(realIndex < features->size());

		calculateNotOrientedDescriptor(linedIntegralImage, width, height, pixelOrigin, (*features)[realIndex], forceCalculation);
	}
}

BlobFeatureDescriptor::LookupTable::LookupTable()
{
	initializeGaussianDistribution();
}

void BlobFeatureDescriptor::LookupTable::initializeGaussianDistribution()
{
	for (int x = 0; x <= 3; ++x)
	{
		for (int y = 0; y <= 3; ++y)
		{
			gaussianWeights_03_2[x][y] = Numeric::gaussianDistribution2(Scalar(x), Scalar(y), Scalar(2), Scalar(2));
		}
	}

	for (int x = 0; x <= 5; ++x)
	{
		for (int y = 0; y <= 5; ++y)
		{
			gaussianWeights_05_2[x][y] = Numeric::gaussianDistribution2(Scalar(x), Scalar(y), Scalar(2), Scalar(2));
		}
	}

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	for (int x = 0; x <= 6; ++x)
	{
		for (int y = 0; y <= 6; ++y)
		{
			gaussianWeights_06_3[x][y] = Numeric::gaussianDistribution2(Scalar(x) * Scalar(1.333333333333333), Scalar(y) * Scalar(1.333333333333333), Scalar(3), Scalar(3));
		}
	}

	unsigned int n = 0;
	for (int yBig = -1; yBig <= 1; ++yBig)
	{
		for (int xBig = -1; xBig <= 1; ++xBig)
		{
			for (int x = 0; x < 5; ++x)
			{
				for (int y = 0; y < 5; ++y)
				{
					const int xOffset = xBig * 4 + x - 2;
					const int yOffset = yBig * 4 + y - 2;

					ocean_assert_and_suppress_unused(abs(xOffset) <= 6, xOffset);
					ocean_assert_and_suppress_unused(abs(yOffset) <= 6, yOffset);

					gaussianWeights_225_3[n] = Numeric::gaussianDistribution2(Scalar(x) * Scalar(1.333333333333333), Scalar(y) * Scalar(1.333333333333333), Scalar(3), Scalar(3));
					++n;
				}
			}
		}
	}

	ocean_assert(n == 225);

#else

	for (int x = 0; x <= 8; ++x)
	{
		for (int y = 0; y <= 8; ++y)
		{
			gaussianWeights_08_3[x][y] = Numeric::gaussianDistribution2(Scalar(x), Scalar(y), Scalar(3), Scalar(3));
		}
	}

	unsigned int n = 0;
	for (int yBig = -2; yBig < 2; ++yBig)
	{
		for (int xBig = -2; xBig < 2; ++xBig)
		{
			for (int x = 0; x < 5; ++x)
			{
				for (int y = 0; y < 5; ++y)
				{
					gaussianWeights_400_3[n] = Numeric::gaussianDistribution2(Scalar(x), Scalar(y), Scalar(3), Scalar(3));
					++n;
				}
			}
		}
	}
	ocean_assert(n == 400u);

#endif // OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

	for (unsigned int i = 0u; i <= 360u; ++i)
	{
		sinValues[i] = Numeric::sin(Numeric::deg2rad(Scalar(i)));
	}

	for (unsigned int i = 0; i <= 360u; ++i)
	{
		cosValues[i] = Numeric::cos(Numeric::deg2rad(Scalar(i)));
	}
}

}

}

}

}
