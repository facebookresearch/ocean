/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/LegacyQRCodeDetector2D.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"
#include "ocean/cv/detector/qrcodes/QRCodeDecoder.h"
#include "ocean/cv/detector/qrcodes/TransitionDetector.h"

#include "ocean/base/StaticVector.h"

#include "ocean/cv/FrameEnlarger.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInverter.h"

#include "ocean/geometry/Estimator.h"
#include "ocean/geometry/NonLinearOptimizationHomography.h"

#include "ocean/math/Line2.h"
#include "ocean/math/SquareMatrix3.h"

#include <cstdint>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

QRCodes LegacyQRCodeDetector2D::detectQRCodes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, Worker* worker, const DetectionMode detectionMode, Observations* observations)
{
	ocean_assert(yFrame != nullptr);

	// Abort early if the image is too small (21 modules (V1) + 2 * 4 modules for the quiet zone = 29)
	if (width < 29u || height < 29u)
	{
		return QRCodes();
	}

	// Detect finder patterns; a minimum of 3 finder patterns is required for the detection of a QR code

	constexpr uint32_t minimumDistanceBetweenFinderPatterns = 10u;

	FinderPatterns finderPatterns;
	Frame yFrameUsedForDetection;

	uint32_t extraBorder = 0u;

	for (const bool invertFrame : {false, true})
	{
		extraBorder = 0u;

		if (invertFrame)
		{
			yFrameUsedForDetection = Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
			CV::FrameInverter::invert8BitPerChannel(yFrame, yFrameUsedForDetection.data<uint8_t>(), width, height, /* channels */ 1u, paddingElements, yFrameUsedForDetection.paddingElements());
		}
		else
		{
			yFrameUsedForDetection = Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_USE_KEEP_LAYOUT, paddingElements);
		}

		ocean_assert(yFrameUsedForDetection.isValid());
		ocean_assert(yFrameUsedForDetection.width() == (width + 2u * extraBorder) && yFrameUsedForDetection.height() == (height + 2u * extraBorder));

		finderPatterns = FinderPatternDetector::detectFinderPatterns(yFrameUsedForDetection.constdata<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), minimumDistanceBetweenFinderPatterns, yFrameUsedForDetection.paddingElements(), worker);

		if (finderPatterns.size() < 3 && detectionMode != DM_STANDARD)
		{
			// Additional checks to look for finder patterns

			if ((detectionMode & DM_EXTRA_BORDER) == DM_EXTRA_BORDER)
			{
				// Add ~5% of the longest side but stay within a reasonable range of values
				extraBorder = minmax<uint32_t>(10u, (std::max(width, height) + 10u) / 20u, 200u);
				Frame frameWithExtraBorder(FrameType(yFrameUsedForDetection, width + 2u * extraBorder, height + 2u * extraBorder));

				FrameEnlarger::addBorderNearestPixel<uint8_t, /* channels */ 1u>(yFrameUsedForDetection.constdata<uint8_t>(), frameWithExtraBorder.data<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), extraBorder, extraBorder, extraBorder, extraBorder, yFrameUsedForDetection.paddingElements(), frameWithExtraBorder.paddingElements());

				yFrameUsedForDetection = std::move(frameWithExtraBorder);
			}

			if ((detectionMode & DM_BLUR) == DM_BLUR)
			{
				// Incrementally increase the amount of blurring applied to the input image.
				Frame incrementallyBlurredFrame(yFrameUsedForDetection.frameType());

				for (const uint32_t filterSize : { 11u, 21u })
				{
					if (!FrameFilterGaussian::filter<uint8_t, uint32_t>(yFrameUsedForDetection.constdata<uint8_t>(), incrementallyBlurredFrame.data<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), yFrameUsedForDetection.channels(), yFrameUsedForDetection.paddingElements(), incrementallyBlurredFrame.paddingElements(), filterSize, filterSize, /* sigma */ -1.0f, worker))
					{
						ocean_assert(false && "This should never happen");
						return QRCodes();
					}

					finderPatterns = FinderPatternDetector::detectFinderPatterns(incrementallyBlurredFrame.constdata<uint8_t>(), incrementallyBlurredFrame.width(), incrementallyBlurredFrame.height(), minimumDistanceBetweenFinderPatterns, incrementallyBlurredFrame.paddingElements(), worker);

					// The current blurred frame will be the input frame for the next iteration of blurring
					std::swap(yFrameUsedForDetection, incrementallyBlurredFrame);

					if (finderPatterns.size() >= 3)
					{
						// Stop the search if at least one potential QR code has been found (i.e. one triplet of finder patterns). Continue to use the blurred frame to the remainder of this detection process
						break;
					}
				}
			}
			else
			{
				finderPatterns = FinderPatternDetector::detectFinderPatterns(yFrameUsedForDetection.constdata<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), minimumDistanceBetweenFinderPatterns, yFrameUsedForDetection.paddingElements(), worker);
			}
		}

		if (finderPatterns.size() >= 3)
		{
			// Stop the search if at least one potential QR code has been found (i.e. one triplet of finder patterns)
			break;
		}
	}

	if (finderPatterns.size() < 3)
	{
		return QRCodes();
	}

	constexpr size_t maximumNumberOfDetectableCodes = 5;
	constexpr size_t maximumNumberOfFinderPatterns = 3 * maximumNumberOfDetectableCodes;

	if (finderPatterns.size() > maximumNumberOfFinderPatterns)
	{
		// Too many finder patterns; abort here to avoid potential spike in the runtime performance
		return QRCodes();
	}

	// Extract all finder patterns, i.e., triplets of finder patterns that potentially belong to the same QR code

	const IndexTriplets indexTriplets = FinderPatternDetector::extractIndexTriplets(finderPatterns); // TOOD Add the finder patterns to the DebugElements

	// Find QR codes by analyzing the triplets of finder patterns

	QRCodes detectedQRCodes;

	for (const IndexTriplet& indexTriplet : indexTriplets)
	{
		FinderPatternTriplet finderPatterTriplet =
		{
			finderPatterns[indexTriplet[0]], // top-left
			finderPatterns[indexTriplet[1]], // bottom-left
			finderPatterns[indexTriplet[2]] // top-right
		};

		unsigned int versionX = (unsigned int)(-1);
		unsigned int versionY = (unsigned int)(-1);
		if (computeProvisionalVersionRange(finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], versionX, versionY))
		{
			unsigned int cornerIndexTopLeft = (unsigned int)(-1);
			unsigned int cornerIndexBottomLeft = (unsigned int)(-1);
			unsigned int cornerIndexTopRight = (unsigned int)(-1);
			determineOuterMostCorners(finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], cornerIndexTopLeft, cornerIndexBottomLeft, cornerIndexTopRight);

			unsigned int version = (unsigned int)(-1);
			SquareMatrix3 initialHomography;

			if (versionX == versionY)
			{
				version = versionX;

				if (computeInitialHomography(finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], version, cornerIndexTopLeft, cornerIndexBottomLeft, cornerIndexTopRight, initialHomography) == false)
				{
					continue;
				}
			}
			else
			{
				ocean_assert(std::abs(int(versionX) - int(versionY)) <= 2);

				SquareMatrix3 initialHomographyX;
				const bool computedInitialHomographyX = computeInitialHomography(finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], versionX, cornerIndexTopLeft, cornerIndexBottomLeft, cornerIndexTopRight, initialHomographyX);

				SquareMatrix3 initialHomographyY;
				const bool computedInitialHomographyY = computeInitialHomography(finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], versionY, cornerIndexTopLeft, cornerIndexBottomLeft, cornerIndexTopRight, initialHomographyY);

				if (computedInitialHomographyX == false && computedInitialHomographyY == false)
				{
					continue;
				}
				else if (initialHomographyX.isHomography() == false)
				{
					ocean_assert(initialHomographyY.isHomography());
					initialHomography = initialHomographyY;
					version = versionY;
				}
				else if (initialHomographyY.isHomography() == false)
				{
					ocean_assert(initialHomographyX.isHomography());
					initialHomography = initialHomographyX;
					version = versionX;
				}
				else
				{
					// Choose the homography that minimizes the sum of absolute squared distances between the centers of the three finder patterns projected into the image and the observed locations of the centers of these finder patterns.

					const unsigned int modulesX = QRCode::modulesPerSide(versionX);
					const Scalar sumAbsoluteDifferencesX = finderPatterTriplet[0].position().sqrDistance(initialHomographyX * Vector2(Scalar(3.5), Scalar(3.5)))
						+ finderPatterTriplet[1].position().sqrDistance(initialHomographyX * Vector2(Scalar(3.5), Scalar(modulesX) - Scalar(3.5)))
						+ finderPatterTriplet[2].position().sqrDistance(initialHomographyX * Vector2(Scalar(modulesX) - Scalar(3.5), Scalar(3.5)));

					const unsigned int modulesY = QRCode::modulesPerSide(versionY);
					const Scalar sumAbsoluteDifferencesY = finderPatterTriplet[0].position().sqrDistance(initialHomographyY * Vector2(Scalar(3.5), Scalar(3.5)))
						+ finderPatterTriplet[1].position().sqrDistance(initialHomographyY * Vector2(Scalar(3.5), Scalar(modulesY) - Scalar(3.5)))
						+ finderPatterTriplet[2].position().sqrDistance(initialHomographyY * Vector2(Scalar(modulesY) - Scalar(3.5), Scalar(3.5)));

					if (sumAbsoluteDifferencesX < sumAbsoluteDifferencesY)
					{
						initialHomography = initialHomographyX;
						version = versionX;
					}
					else
					{
						initialHomography = initialHomographyY;
						version = versionY;
					}
				}
			}

			ocean_assert(initialHomography.isHomography() && version >= 1u && version <= 40u);


			// QR code versions 1-6 do not have dedicated bit fields that store the version information. For versions 7-40 continue to extract the information from the bit field.

			if (version >= 7u)
			{
				unsigned int versionFromImage = (unsigned int)(-1);
				if (determineSymbolVersionFromImage(yFrameUsedForDetection.constdata<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), yFrameUsedForDetection.paddingElements(), finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], version, initialHomography, versionFromImage) == false)
				{
					continue;
				}

				ocean_assert(versionFromImage != 0u && versionFromImage <= 40u);

				if (version != versionFromImage)
				{
					// Update the initial homography according to the version information extraction from the image
					if (computeInitialHomography(finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], versionFromImage, cornerIndexTopLeft, cornerIndexBottomLeft, cornerIndexTopRight, initialHomography) == false)
					{
						continue;
					}
					version = versionFromImage;
				}
			}

			ocean_assert(version >= 1u && version <= 40u && initialHomography.isHomography());

			if (detectTimerPatterns(yFrameUsedForDetection.constdata<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), yFrameUsedForDetection.paddingElements(), version, initialHomography, finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2]))
			{
				SquareMatrix3 homography;
				if (computeRefinedHomography(yFrameUsedForDetection.constdata<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), yFrameUsedForDetection.paddingElements(), finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], cornerIndexTopLeft, cornerIndexBottomLeft, cornerIndexTopRight, version, initialHomography, homography))
				{
					QRCode::ErrorCorrectionCapacity errorCorrectionCapacity;
					QRCodeEncoder::MaskingPattern maskingPattern;

					if (determineSymbolFormat(yFrameUsedForDetection.constdata<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), yFrameUsedForDetection.paddingElements(), finderPatterTriplet[0], finderPatterTriplet[1], finderPatterTriplet[2], version, homography, errorCorrectionCapacity, maskingPattern))
					{
						std::vector<uint8_t> modules;
						const unsigned int grayThreshold = ((finderPatterTriplet[0].grayThreshold() + finderPatterTriplet[1].grayThreshold() + finderPatterTriplet[2].grayThreshold()) * 1024u + 512u) / 3072u;

						if (extractModules(yFrameUsedForDetection.constdata<uint8_t>(), yFrameUsedForDetection.width(), yFrameUsedForDetection.height(), yFrameUsedForDetection.paddingElements(), grayThreshold, version, homography, modules))
						{
							QRCode code;

							if (QRCodeDecoder::decodeQRCode(modules, code))
							{
								ocean_assert(code.isValid());
								detectedQRCodes.emplace_back(std::move(code));

								if (observations != nullptr)
								{
									observations->emplace_back(homography, std::move(finderPatterTriplet));
								}
							}
						}
					}
				}
			}
		}
	}

	// If an extra border was added around the input image, correct the observation so that it matches the original input image
	if (observations != nullptr && extraBorder != 0u)
	{
		ocean_assert((detectionMode & DM_EXTRA_BORDER) == DM_EXTRA_BORDER);

		// Remove the offset caused by the extra border
		const Vector2 offset = Vector2(Scalar(extraBorder), Scalar(extraBorder));

		const SquareMatrix3 offsetTransformation = SquareMatrix3(Vector3(Scalar(1), 0, 0), Vector3(0, Scalar(1), 0), Vector3(-offset, Scalar(1)));

		for (Observation& observation : *observations)
		{
			const SquareMatrix3 frame_H_code = offsetTransformation * observation.frame_H_code();

			FinderPatternTriplet currentFinderPatterns = observation.finderPatterns(); // Intentional copy!

			for (size_t i = 0; i < 3; ++i)
			{
				FinderPattern& finderPattern = currentFinderPatterns[i];

				const Vector2* cornersWithOffset = finderPattern.corners();
				ocean_assert(cornersWithOffset != nullptr);
				Vector2 corners[4];

				for (size_t c = 0; c < 4; ++c)
				{
					corners[c] = cornersWithOffset[c] - offset;
					ocean_assert(corners[c].x() >= 0 && corners[c].x() < Scalar(width) && corners[c].y() >= 0 && corners[c].y() < Scalar(height));
				}

				finderPattern = FinderPattern(finderPattern.position() - offset, finderPattern.length(), finderPattern.centerIntensity(), finderPattern.grayThreshold(), finderPattern.symmetryScore(), corners, finderPattern.orientation(), finderPattern.moduleSize());
			}

			observation = Observation(frame_H_code, std::move(currentFinderPatterns));
		}
	}

	return detectedQRCodes;
}

bool LegacyQRCodeDetector2D::detectTimerPatterns(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int version, const SquareMatrix3& homography, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight)
{
	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);
	ocean_assert(version != 0u && version <= 40u);
	ocean_assert(topLeft.position() != bottomLeft.position() && topLeft.position() != topRight.position());
	ocean_assert(bottomLeft.position() != topRight.position());
	ocean_assert(topLeft.cornersKnown() && bottomLeft.cornersKnown() && topRight.cornersKnown());

	const unsigned int yFrameStrideElements = width + yFramePaddingElements;

	// Check the horizontal timer pattern (between the top-left and top-right finder pattern) and, if necessary, the vertical one as well
	// (between the top-left and the bottom-left finder pattern)

	ocean_assert(QRCode::modulesPerSide(version) >= 21u);
	const unsigned int timingPatternSize = QRCode::modulesPerSide(version) - 14u;
	ocean_assert(timingPatternSize != 0u && timingPatternSize % 2u == 1u);

	unsigned int correctIntensities[2] = { 0u, 0u };

	// i == 0u : horizontal timer pattern
	// i == 1u : vertical timer pattern
	for (unsigned int i = 0u; i < 2u; ++i)
	{
		Vector2 qrcodeLocation = (i == 0u ? Vector2(Scalar(7.5), Scalar(6.5)) : Vector2(Scalar(6.5), Scalar(7.5)));
		const Vector2 step = (i == 0u ? Vector2(Scalar(1), Scalar(0)) : Vector2(Scalar(0), Scalar(1)));
		const unsigned int threshold = (i == 0u ? ((topLeft.grayThreshold() + topRight.grayThreshold() + 1u) / 2u) : ((topLeft.grayThreshold() + bottomLeft.grayThreshold() + 1u) / 2u));

		Vector2 imageLocation;
		unsigned int imageX;
		unsigned int imageY;

		for (unsigned int t = 0u; t < (timingPatternSize - 1u); t += 2u)
		{
			// Check alternating pairs of modules: light + dark

			// Light module

			if (homography.multiply(qrcodeLocation, imageLocation) == false)
			{
				continue;
			}

			imageX = (unsigned int)Numeric::round32(imageLocation.x());
			imageY = (unsigned int)Numeric::round32(imageLocation.y());

			if (imageY < height && imageX < width && yFrame[imageY * yFrameStrideElements + imageX] >= threshold)
			{
				correctIntensities[i]++;
			}

			qrcodeLocation += step;

			// Dark module

			if (homography.multiply(qrcodeLocation, imageLocation) == false)
			{
				continue;
			}

			imageX = (unsigned int)Numeric::round32(imageLocation.x());
			imageY = (unsigned int)Numeric::round32(imageLocation.y());

			if (imageY < height && imageX < width && yFrame[imageY * yFrameStrideElements + imageX] < threshold)
			{
				correctIntensities[i]++;
			}

			qrcodeLocation += step;
		}

		// Check for the last light module

		if (homography.multiply(qrcodeLocation, imageLocation) == false)
		{
			continue;
		}

		imageX = (unsigned int)Numeric::round32(imageLocation.x());
		imageY = (unsigned int)Numeric::round32(imageLocation.y());

		if (imageY < height && imageX < width && yFrame[imageY * yFrameStrideElements + imageX] >= threshold)
		{
			correctIntensities[i]++;
		}

		if (correctIntensities[i] == timingPatternSize)
		{
			break;
		}
	}

	return correctIntensities[0] == timingPatternSize || correctIntensities[1] == timingPatternSize || (correctIntensities[0] >= timingPatternSize * 8u / 10u && correctIntensities[1] >= timingPatternSize * 8u / 10u);
}

bool LegacyQRCodeDetector2D::determineOuterMostCorners(const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, unsigned int& cornerIndexTopLeft, unsigned int& cornerIndexBottomLeft, unsigned int& cornerIndexTopRight)
{
	ocean_assert(topLeft.position() != bottomLeft.position() && topLeft.position() != topRight.position());
	ocean_assert(bottomLeft.position() != topRight.position());

	cornerIndexTopLeft = (unsigned int)(-1);
	cornerIndexBottomLeft = (unsigned int)(-1);
	cornerIndexTopRight = (unsigned int)(-1);

	if (topLeft.cornersKnown() == false || bottomLeft.cornersKnown() == false || topRight.cornersKnown() == false)
	{
		return false;
	}

	// Find the outer-most corners of the top-left, bottom-left, and top-right finder pattern (marked by *)
	//
	//   TL               TR
	//      *--       --*
	//      |  |     |  |
	//       --       --
	//
	//       --
	//	    |  |
	//      *--
	//   BL

	// Outer-most corner of the top-left finder pattern

	const Vector2 diagonalOutward = -((bottomLeft.position() - topLeft.position()) + (topRight.position() - topLeft.position()));
	ocean_assert(diagonalOutward.isNull() == false);

	Scalar bestValueTopLeft = (topLeft.corners()[0] - topLeft.position()) * diagonalOutward;
	cornerIndexTopLeft = 0u;

	const Vector2 diagonalBottomLeftToTopRight = topRight.position() - bottomLeft.position();
	ocean_assert(diagonalBottomLeftToTopRight.isNull() == false);

	Scalar bestValueBottomLeft = (bottomLeft.corners()[0] - bottomLeft.position()) * -diagonalBottomLeftToTopRight;
	cornerIndexBottomLeft = 0u;

	Scalar bestValueTopRight = (topRight.corners()[0] - topRight.position()) * diagonalBottomLeftToTopRight;
	cornerIndexTopRight = 0u;

	for (unsigned int i = 1u; i < 4u; ++i)
	{
		const Scalar valueTopLeft = (topLeft.corners()[i] - topLeft.position()) * diagonalOutward;
		if (valueTopLeft > bestValueTopLeft)
		{
			bestValueTopLeft = valueTopLeft;
			cornerIndexTopLeft = i;
		}

		const Scalar valueBottomLeft = (bottomLeft.corners()[i] - bottomLeft.position()) * -diagonalBottomLeftToTopRight;
		if (valueBottomLeft > bestValueBottomLeft)
		{
			bestValueBottomLeft = valueBottomLeft;
			cornerIndexBottomLeft = i;
		}

		Scalar valueTopRight = (topRight.corners()[i] - topRight.position()) * diagonalBottomLeftToTopRight;
		if (valueTopRight > bestValueTopRight)
		{
			bestValueTopRight = valueTopRight;
			cornerIndexTopRight = i;
		}
	}

	return cornerIndexTopLeft < 4u && cornerIndexBottomLeft < 4u && cornerIndexTopRight < 4u;
}

bool LegacyQRCodeDetector2D::determineSymbolVersionFromImage(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int estimatedVersion, const SquareMatrix3& homography, unsigned int& version)
{
	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);
	ocean_assert(topLeft.position() != bottomLeft.position() && topLeft.position() != topRight.position());
	ocean_assert(bottomLeft.position() != topRight.position());
	ocean_assert(estimatedVersion >= 7u && estimatedVersion <= 40u);
	ocean_assert(homography.isHomography());

	version = (unsigned int)(-1);

	if (estimatedVersion < 7u
		|| topLeft.position().x() < 0 || topLeft.position().x() >= Scalar(width) || topLeft.position().y() < 0 || topLeft.position().y() >= Scalar(height)
		|| bottomLeft.position().x() < 0 || bottomLeft.position().x() >= Scalar(width) || bottomLeft.position().y() < 0 || bottomLeft.position().y() >= Scalar(height)
		|| topRight.position().x() < 0 || topRight.position().x() >= Scalar(width) || topRight.position().y() < 0 || topRight.position().y() >= Scalar(height))
	{
		return false;
	}

	const unsigned int grayThreshold = (topLeft.grayThreshold() + topRight.grayThreshold() + bottomLeft.grayThreshold() + 1u) / 3u;
	ocean_assert(grayThreshold <= 255u);

	// Try to decode the version bit field in the upper-right corner (version information 1). If that fails, try the one in the lower-left corner (version information 2)
	//
	//                                   version information 1 (6 x 3 modules)
	//                                   |
	//                                   v
	//      ##############             0 1 2  ##############
	//      ##          ##             3 4 5  ##          ##
	//      ##  ######  ##             6 7 8  ##  ######  ##
	//      ##  ######  ##             91011  ##  ######  ##
	//      ##  ######  ##            121314  ##  ######  ##
	//      ##          ##            151617  ##          ##
	//      ##############                    ##############
	//
	//
	//
	//
	//       0 3 6 91215
	//       1 4 7101316  <- version information 2 (3 x 6 modules)
	//       2 5 8111417
	//
	//      ##############
	//      ##          ##
	//      ##  ######  ##
	//      ##  ######  ##
	//      ##  ######  ##
	//      ##          ##
	//      ##############
	//

	const unsigned int yFrameStrideElements = width + yFramePaddingElements;
	const unsigned int modules = QRCode::modulesPerSide(std::min(40u, estimatedVersion));

	const Vector2 modulesVersionInformation1[18] =
	{
		Vector2(Scalar(modules - 11u) + Scalar(0.5), Scalar(0.5)), // Bit 0
		Vector2(Scalar(modules - 10u) + Scalar(0.5), Scalar(0.5)), // Bit 1
		Vector2(Scalar(modules - 9u) + Scalar(0.5), Scalar(0.5)),  // Bit 2
		Vector2(Scalar(modules - 11u) + Scalar(0.5), Scalar(1.5)), // Bit 3
		Vector2(Scalar(modules - 10u) + Scalar(0.5), Scalar(1.5)), // Bit 4
		Vector2(Scalar(modules - 9u) + Scalar(0.5), Scalar(1.5)),  // Bit 5
		Vector2(Scalar(modules - 11u) + Scalar(0.5), Scalar(2.5)), // Bit 6
		Vector2(Scalar(modules - 10u) + Scalar(0.5), Scalar(2.5)), // Bit 7
		Vector2(Scalar(modules - 9u) + Scalar(0.5), Scalar(2.5)),  // Bit 8
		Vector2(Scalar(modules - 11u) + Scalar(0.5), Scalar(3.5)), // Bit 9
		Vector2(Scalar(modules - 10u) + Scalar(0.5), Scalar(3.5)), // Bit 10
		Vector2(Scalar(modules - 9u) + Scalar(0.5), Scalar(3.5)),  // Bit 11
		Vector2(Scalar(modules - 11u) + Scalar(0.5), Scalar(4.5)), // Bit 12
		Vector2(Scalar(modules - 10u) + Scalar(0.5), Scalar(4.5)), // Bit 13
		Vector2(Scalar(modules - 9u) + Scalar(0.5), Scalar(4.5)),  // Bit 14
		Vector2(Scalar(modules - 11u) + Scalar(0.5), Scalar(5.5)), // Bit 15
		Vector2(Scalar(modules - 10u) + Scalar(0.5), Scalar(5.5)), // Bit 16
		Vector2(Scalar(modules - 9u) + Scalar(0.5), Scalar(5.5)),  // Bit 17
	};

	uint32_t versionInformation1Bits = 0u;
	for (unsigned int i = 0u; i < 18u; ++i)
	{
		Vector2 moduleImageLocation;
		if (homography.multiply(modulesVersionInformation1[i], moduleImageLocation) == false
			|| moduleImageLocation.x() < Scalar(0) || moduleImageLocation.x() > Scalar(width - 1u)
			|| moduleImageLocation.y() < Scalar(0) || moduleImageLocation.y() > Scalar(height - 1u))
		{
			continue;
		}

		const unsigned int moduleValue = yFrame[(unsigned int)(moduleImageLocation.y() + Scalar(0.5)) * yFrameStrideElements + (unsigned int)(moduleImageLocation.x() + Scalar(0.5))];

		versionInformation1Bits |= (moduleValue < grayThreshold ? 1u : 0u) << i;
		ocean_assert(versionInformation1Bits >> 18u == 0u);
	}

	if (QRCodeEncoder::decodeVersionBits(versionInformation1Bits, version) == false)
	{
		// Decoding the top-right version information field has failed, now try the one the in the bottom-left

		const Vector2 modulesVersionInformation2[18] =
		{
			Vector2(Scalar(0.5), Scalar(modules - 11u) + Scalar(0.5)), // Bit 0
			Vector2(Scalar(0.5), Scalar(modules - 10u) + Scalar(0.5)), // Bit 1
			Vector2(Scalar(0.5), Scalar(modules - 9u) + Scalar(0.5)),  // Bit 2
			Vector2(Scalar(1.5), Scalar(modules - 11u) + Scalar(0.5)), // Bit 3
			Vector2(Scalar(1.5), Scalar(modules - 10u) + Scalar(0.5)), // Bit 4
			Vector2(Scalar(1.5), Scalar(modules - 9u) + Scalar(0.5)),  // Bit 5
			Vector2(Scalar(2.5), Scalar(modules - 11u) + Scalar(0.5)), // Bit 6
			Vector2(Scalar(2.5), Scalar(modules - 10u) + Scalar(0.5)), // Bit 7
			Vector2(Scalar(2.5), Scalar(modules - 9u) + Scalar(0.5)),  // Bit 8
			Vector2(Scalar(3.5), Scalar(modules - 11u) + Scalar(0.5)), // Bit 9
			Vector2(Scalar(3.5), Scalar(modules - 10u) + Scalar(0.5)), // Bit 10
			Vector2(Scalar(3.5), Scalar(modules - 9u) + Scalar(0.5)),  // Bit 11
			Vector2(Scalar(4.5), Scalar(modules - 11u) + Scalar(0.5)), // Bit 12
			Vector2(Scalar(4.5), Scalar(modules - 10u) + Scalar(0.5)), // Bit 13
			Vector2(Scalar(4.5), Scalar(modules - 9u) + Scalar(0.5)),  // Bit 14
			Vector2(Scalar(5.5), Scalar(modules - 11u) + Scalar(0.5)), // Bit 15
			Vector2(Scalar(5.5), Scalar(modules - 10u) + Scalar(0.5)), // Bit 16
			Vector2(Scalar(5.5), Scalar(modules - 9u) + Scalar(0.5)),  // Bit 17
		};

		uint32_t versionInformation2Bits = 0u;
		for (unsigned int i = 0u; i < 18u; ++i)
		{
			Vector2 moduleImageLocation;
			if (homography.multiply(modulesVersionInformation2[i], moduleImageLocation) == false
				|| moduleImageLocation.x() < Scalar(0) || moduleImageLocation.x() > Scalar(width - 1u)
				|| moduleImageLocation.y() < Scalar(0) || moduleImageLocation.y() > Scalar(height - 1u))
			{
				continue;
			}

			const unsigned int moduleValue = yFrame[(unsigned int)(moduleImageLocation.y() + Scalar(0.5)) * yFrameStrideElements + (unsigned int)(moduleImageLocation.x() + Scalar(0.5))];
			versionInformation2Bits |= (moduleValue < grayThreshold ? 1u : 0u) << i;
			ocean_assert(versionInformation2Bits >> 18u == 0u);
		}

		if (QRCodeEncoder::decodeVersionBits(versionInformation2Bits, version) == false)
		{
			return false;
		}
	}

	return version >= 1u && version <= 40u;
}

bool LegacyQRCodeDetector2D::determineSymbolFormat(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int version, const SquareMatrix3& homography, QRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, QRCodeEncoder::MaskingPattern& maskingPattern)
{
	ocean_assert(topLeft.position() != bottomLeft.position() && topLeft.position() != topRight.position());
	ocean_assert(bottomLeft.position() != topRight.position());
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert(homography.isHomography());

	if (version == 0u || version > 40u)
	{
		return false;
	}

	// Try to decode the format bit field in the upper-left corner (format information 1). If that fails, try to read format information 2 that is near the top-right finder pattern (2a) and the bottom-left finder pattern (2b)
	//
	//                  format information 1 (15 modules)
	//                  |
	//                  v
	// ##############   0                ##############
	// ##          ##   1                ##          ##
	// ##  ######  ##   2                ##  ######  ##
	// ##  ######  ##   3                ##  ######  ##
	// ##  ######  ##   4                ##  ######  ##
	// ##          ##   5                ##          ##
	// ##############                    ##############
	//                  6
	// 1413121110 9   8 7               7 6 5 4 3 2 1 0 <- format information 2a
	//
	//
	//
	//
	//
	//
	// ##############   8
	// ##          ##   9
	// ##  ######  ##  10
	// ##  ######  ##  11 <- format information 2b
	// ##  ######  ##  12
	// ##          ##  13
	// ##############  14
	//

	const unsigned int modules = QRCode::modulesPerSide(std::min(40u, version));
	const unsigned int grayThreshold = (topLeft.grayThreshold() + topRight.grayThreshold() + bottomLeft.grayThreshold() + 1u) / 3u;
	ocean_assert(grayThreshold <= 255u);

	const Vector2 modulesFormatInformation1[15] =
	{
		// Right of the top-left finder pattern (top to bottom)
		Vector2(Scalar(8.5), Scalar(0.5)), // Bit 0
		Vector2(Scalar(8.5), Scalar(1.5)), // Bit 1
		Vector2(Scalar(8.5), Scalar(2.5)), // Bit 2
		Vector2(Scalar(8.5), Scalar(3.5)), // Bit 3
		Vector2(Scalar(8.5), Scalar(4.5)), // Bit 4
		Vector2(Scalar(8.5), Scalar(5.5)), // Bit 5
		Vector2(Scalar(8.5), Scalar(7.5)), // Bit 6

		// Below the top-left finder pattern (right to left)
		Vector2(Scalar(8.5), Scalar(8.5)), // Bit 7
		Vector2(Scalar(7.5), Scalar(8.5)), // Bit 8
		Vector2(Scalar(5.5), Scalar(8.5)), // Bit 9
		Vector2(Scalar(4.5), Scalar(8.5)), // Bit 10
		Vector2(Scalar(3.5), Scalar(8.5)), // Bit 11
		Vector2(Scalar(2.5), Scalar(8.5)), // Bit 12
		Vector2(Scalar(1.5), Scalar(8.5)), // Bit 13
		Vector2(Scalar(0.5), Scalar(8.5)), // Bit 14
	};

	uint32_t formatInformation1Bits = 0u;
	for (unsigned int i = 0u; i < 15u; ++i)
	{
		Vector2 moduleImageLocation;

		if (homography.multiply(modulesFormatInformation1[i], moduleImageLocation) == false)
		{
			continue;
		}

		const int moduleY = Numeric::round32(moduleImageLocation.y());
		const int moduleX = Numeric::round32(moduleImageLocation.x());

		if (moduleX < 0 || moduleX >= int(width) || moduleY < 0 || moduleY >= int(height))
		{
			continue;
		}

		const unsigned int moduleValue = yFrame[(unsigned int)(moduleY) * (width + yFramePaddingElements) + (unsigned int)(moduleX)];

		formatInformation1Bits |= (moduleValue < grayThreshold ? 1u : 0u) << i;
		ocean_assert(formatInformation1Bits >> 15u == 0u);
	}

	if (QRCodeEncoder::decodeFormatBits(formatInformation1Bits, errorCorrectionCapacity, maskingPattern))
	{
		return true;
	}

	// Reading the format information around the upper-left finder pattern failed, so now try to read the information near
	// the top-right and bottom-left finder pattern

	const Vector2 modulesFormatInformation2[15] =
	{
		// Below the top-right finder pattern (right to left)
		Vector2(Scalar(modules) - Scalar(0.5), Scalar(8.5)), // Bit 0
		Vector2(Scalar(modules) - Scalar(1.5), Scalar(8.5)), // Bit 1
		Vector2(Scalar(modules) - Scalar(2.5), Scalar(8.5)), // Bit 2
		Vector2(Scalar(modules) - Scalar(3.5), Scalar(8.5)), // Bit 3
		Vector2(Scalar(modules) - Scalar(4.5), Scalar(8.5)), // Bit 4
		Vector2(Scalar(modules) - Scalar(5.5), Scalar(8.5)), // Bit 5
		Vector2(Scalar(modules) - Scalar(6.5), Scalar(8.5)), // Bit 6
		Vector2(Scalar(modules) - Scalar(7.5), Scalar(8.5)), // Bit 7

		// Right of the bottom-left finder pattern (top to bottom)
		Vector2(Scalar(8.5), Scalar(modules) - Scalar(6.5)), // Bit 8
		Vector2(Scalar(8.5), Scalar(modules) - Scalar(5.5)), // Bit 9
		Vector2(Scalar(8.5), Scalar(modules) - Scalar(4.5)), // Bit 10
		Vector2(Scalar(8.5), Scalar(modules) - Scalar(3.5)), // Bit 11
		Vector2(Scalar(8.5), Scalar(modules) - Scalar(2.5)), // Bit 12
		Vector2(Scalar(8.5), Scalar(modules) - Scalar(1.5)), // Bit 13
		Vector2(Scalar(8.5), Scalar(modules) - Scalar(0.5)), // Bit 14
	};

	uint32_t formatInformation2Bits = 0u;
	for (unsigned int i = 0u; i < 15u; ++i)
	{
		Vector2 moduleImageLocation;

		if (homography.multiply(modulesFormatInformation2[i], moduleImageLocation) == false)
		{
			continue;
		}

		const int moduleY = Numeric::round32(moduleImageLocation.y());
		const int moduleX = Numeric::round32(moduleImageLocation.x());

		if (moduleX < 0 || moduleX >= int(width) || moduleY < 0 || moduleY >= int(height))
		{
			continue;
		}

		const unsigned int moduleValue = yFrame[(unsigned int)(moduleY) * (width + yFramePaddingElements) + (unsigned int)(moduleX)];
		formatInformation2Bits |= (moduleValue < grayThreshold ? 1u : 0u) << i;
		ocean_assert(formatInformation2Bits >> 15u == 0u);
	}

	return QRCodeEncoder::decodeFormatBits(formatInformation2Bits, errorCorrectionCapacity, maskingPattern);
}

bool LegacyQRCodeDetector2D::computeRefinedHomography(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int cornerIndexTopLeft, const unsigned int cornerIndexBottomLeft, const unsigned int cornerIndexTopRight, unsigned int& version, const SquareMatrix3& initialHomography, SquareMatrix3& homography)
{
	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);
	ocean_assert(version >= 1u && version <= 40u);

	if (version == 1u)
	{
		homography = initialHomography;
		return true;
	}

	// QR code versions 2 to 40 have alignment patterns of which the locations are known. Use these known locations in order to refine the accuracy of the initial homography.

	if (topLeft.cornersKnown() == false || bottomLeft.cornersKnown() == false || topRight.cornersKnown() == false || cornerIndexTopLeft >= 4u || cornerIndexBottomLeft >= 4u || cornerIndexTopRight >= 4u || initialHomography.isHomography() == false)
	{
		return false;
	}

	const unsigned int yFrameStrideElements = width + yFramePaddingElements;
	const unsigned int grayThreshold = (topLeft.grayThreshold() + bottomLeft.grayThreshold() + topRight.grayThreshold()) / 3u;
	const unsigned int maximumSearchDistance = (unsigned int)((topLeft.moduleSize() + bottomLeft.moduleSize() + topRight.moduleSize()) * Scalar(2) / Scalar(9) + Scalar(0.5));

	const VectorsI2 alignmentPatterns = QRCodeEncoder::computeAlignmentPatternPositions(version);
	ocean_assert(std::is_sorted(alignmentPatterns.begin(), alignmentPatterns.end(), [](const VectorI2& a, const VectorI2& b) { return a.y() < b.y() || (a.y() == b.y() && a.x() < b.x()); }));

	// Define the initial set of point correspondences. Here, use the four corners of the three finder patterns
	// (top-left, bottom-left, and top-right):
	//
	//   TL               TR
	//     *0--3     9--8*
	//      |  |     |  |
	//      1--2    10--11
	//
	//      7--6
	//	    |  |
	//     *4--5
	//   BL
	//
	// The numbers indicate the index in the list of points below. The asterik (*) denotes the outer-most corners
	// that were determined above. Because the corners of the finder patterns are in counter-clockwise order, they
	// can easily be enumerated using `i & 0b0011u` as cheaper equivalent to `i % 4`.

	Vectors2 imagePoints =
	{
		// Corners of the top-left finder pattern
		topLeft.corners()[(cornerIndexTopLeft + 0u) & 0b0011u], // (cornerIndexTopLeft + 0u) & 0b0011u == (cornerIndexTopLeft + x) % 4
		topLeft.corners()[(cornerIndexTopLeft + 1u) & 0b0011u],
		topLeft.corners()[(cornerIndexTopLeft + 2u) & 0b0011u],
		topLeft.corners()[(cornerIndexTopLeft + 3u) & 0b0011u],

		// Corners of the bottom-left finder pattern
		bottomLeft.corners()[(cornerIndexBottomLeft + 0u) & 0b0011u],
		bottomLeft.corners()[(cornerIndexBottomLeft + 1u) & 0b0011u],
		bottomLeft.corners()[(cornerIndexBottomLeft + 2u) & 0b0011u],
		bottomLeft.corners()[(cornerIndexBottomLeft + 3u) & 0b0011u],

		// Corners of the top-right finder pattern
		topRight.corners()[(cornerIndexTopRight + 0u) & 0b0011u],
		topRight.corners()[(cornerIndexTopRight + 1u) & 0b0011u],
		topRight.corners()[(cornerIndexTopRight + 2u) & 0b0011u],
		topRight.corners()[(cornerIndexTopRight + 3u) & 0b0011u],
	};

	const unsigned int modules = QRCode::modulesPerSide(version);

	Vectors2 qrcodePoints =
	{
		// Corners of the top-left finder pattern
		Vector2(Scalar(0), Scalar(0)),
		Vector2(Scalar(0), Scalar(7)),
		Vector2(Scalar(7), Scalar(7)),
		Vector2(Scalar(7), Scalar(0)),

		// Corners of the bottom-left finder pattern
		Vector2(Scalar(0), Scalar(modules)),
		Vector2(Scalar(7), Scalar(modules)),
		Vector2(Scalar(7), Scalar(modules - 7u)),
		Vector2(Scalar(0), Scalar(modules - 7u)),

		// Corners of the top-right finder pattern
		Vector2(Scalar(modules), Scalar(0)),
		Vector2(Scalar(modules - 7u), Scalar(0)),
		Vector2(Scalar(modules - 7u), Scalar(7)),
		Vector2(Scalar(modules), Scalar(7)),
	};

	ocean_assert(imagePoints.size() >= 4 && imagePoints.size() == qrcodePoints.size());

	homography = initialHomography;
	SquareMatrix3 refinedHomography = initialHomography;

	for (const VectorI2& alignmentPattern : alignmentPatterns)
	{
		const Vector2 alignmentCenter(Scalar(alignmentPattern.x()) + Scalar(0.5), Scalar(alignmentPattern.y()) + Scalar(0.5));
		const Vector2 alignmentCenterImage = refinedHomography * alignmentCenter;

#if 1 // TODO Improve this, it needs to be more robust
		const unsigned int x0 = (unsigned int)(alignmentCenterImage.x() + Scalar(0.5));
		const unsigned int y0 = (unsigned int)(alignmentCenterImage.y() + Scalar(0.5));
		const unsigned int radius = std::max(3u, maximumSearchDistance / 2u);

		if (x0 >= width || y0 >= height)
		{
			continue;
		}

		unsigned int minValue = yFrame[y0 * yFrameStrideElements + x0];
		unsigned int minX = x0;
		unsigned int minY = y0;

		for (unsigned int iy = (y0 > radius ? y0 - radius : 0u); iy < (y0 + radius < height ? y0 + radius + 1: y0); ++iy)
		{
			for (unsigned int ix = (x0 > radius ? x0 - radius : 0u); ix < (x0 + radius < width ? x0 + radius + 1u: x0); ++ix)
			{
				const unsigned int value = yFrame[iy * yFrameStrideElements + ix];

				if (value < minValue)
				{
					minValue = value;
					minX = ix;
					minY = iy;
				}
			}
		}

		const unsigned int x = minX;
		const unsigned int y = minY;
#else
		const unsigned int x = (unsigned int)(alignmentCenterImage.x() + Scalar(0.5));
		const unsigned int y = (unsigned int)(alignmentCenterImage.y() + Scalar(0.5));
#endif

		if (x >= width || y >= height || yFrame[y * yFrameStrideElements + x] >= grayThreshold)
		{
			continue;
		}

		ocean_assert(x < width && y < height);

		// Find intensity transitions (dark to light) to left (0), right (1), top (2), and bottom (3) of the center (x, y).
		// The search direction is defined by the vectors between the top-left and the other two finder patterns.
		const Vector2 verticalDown = bottomLeft.position() - topLeft.position();
		const Vector2 horizontalRight = topRight.position() - topLeft.position();

		CV::Bresenham bresenham[4] =
		{
			Bresenham(int(x), int(y), int(x) - Numeric::round32(horizontalRight.x()), int(y) - Numeric::round32(horizontalRight.y())),
			Bresenham(int(x), int(y), int(x) + Numeric::round32(horizontalRight.x()), int(y) + Numeric::round32(horizontalRight.y())),
			Bresenham(int(x), int(y), int(x) - Numeric::round32(verticalDown.x()), int(y) - Numeric::round32(verticalDown.y())),
			Bresenham(int(x), int(y), int(x) + Numeric::round32(verticalDown.x()), int(y) + Numeric::round32(verticalDown.y()))
		};

		for (size_t i = 0; i < 4; ++i)
		{
			if (bresenham[i].isValid() == false)
			{
				continue;
			}
		}

		unsigned int columns[4];
		unsigned int rows[4];
		VectorT2<unsigned int> lastPointInside[4];
		VectorT2<unsigned int> firstPointOutside[4];

		if (TransitionDetector::findNextPixel<false>(yFrame, int(x), int(y), width, height, yFramePaddingElements, bresenham[0], maximumSearchDistance, grayThreshold, columns[0], rows[0], lastPointInside[0], firstPointOutside[0]) == false
			|| TransitionDetector::findNextPixel<false>(yFrame, int(x), int(y), width, height, yFramePaddingElements, bresenham[1], maximumSearchDistance, grayThreshold, columns[1], rows[1], lastPointInside[1], firstPointOutside[1]) == false
			|| TransitionDetector::findNextPixel<false>(yFrame, int(x), int(y), width, height, yFramePaddingElements, bresenham[2], maximumSearchDistance, grayThreshold, columns[2], rows[2], lastPointInside[2], firstPointOutside[2]) == false
			|| TransitionDetector::findNextPixel<false>(yFrame, int(x), int(y), width, height, yFramePaddingElements, bresenham[3], maximumSearchDistance, grayThreshold, columns[3], rows[3], lastPointInside[3], firstPointOutside[3]) == false)
		{
			continue;
		}

		// Determine transition points with sub-pixel accuracy, compute the refined center of the current alignment pattern, and add a new correspondence to point lists

		const Vector2 transitionPoints[4] =
		{
			TransitionDetector::computeTransitionPointSubpixelAccuracy(yFrame, width, height, yFramePaddingElements, lastPointInside[0], firstPointOutside[0], grayThreshold),
			TransitionDetector::computeTransitionPointSubpixelAccuracy(yFrame, width, height, yFramePaddingElements, lastPointInside[1], firstPointOutside[1], grayThreshold),
			TransitionDetector::computeTransitionPointSubpixelAccuracy(yFrame, width, height, yFramePaddingElements, lastPointInside[2], firstPointOutside[2], grayThreshold),
			TransitionDetector::computeTransitionPointSubpixelAccuracy(yFrame, width, height, yFramePaddingElements, lastPointInside[3], firstPointOutside[3], grayThreshold)
		};

		const Vector2 refinedAlignmentPatternCenterImage(Scalar(0.5) * (transitionPoints[0].x() + transitionPoints[1].x()), Scalar(0.5) * (transitionPoints[2].y() + transitionPoints[3].y()));
		ocean_assert(refinedAlignmentPatternCenterImage.x() >= 0 && refinedAlignmentPatternCenterImage.x() < Scalar(width) && refinedAlignmentPatternCenterImage.y() >= 0 && refinedAlignmentPatternCenterImage.y() < Scalar(height));

		imagePoints.emplace_back(refinedAlignmentPatternCenterImage);
		qrcodePoints.emplace_back(alignmentCenter);

		// Optimize the homography again
		if (Geometry::NonLinearOptimizationHomography::optimizeHomography<Geometry::Estimator::ET_SQUARE>(homography, qrcodePoints.data(), imagePoints.data(), imagePoints.size(), /* modelParameters */ 9u, refinedHomography) == false)
		{
			return false;
		}

		homography = refinedHomography;
	}

	return homography.isHomography();
}

bool LegacyQRCodeDetector2D::extractModules(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int grayThreshold, const unsigned int version, const SquareMatrix3& homography, std::vector<uint8_t>& modules)
{
	ocean_assert(yFrame != nullptr && width != 0u && height != 0u);
	ocean_assert(grayThreshold <= 255u);
	ocean_assert(version != 0u && version <= 40u);
	ocean_assert(homography.isHomography());

	const unsigned int size = QRCode::modulesPerSide(version);
	modules.resize(size * size);

	unsigned int moduleIndex = 0u;
	for (unsigned int y = 0u; y < size; ++y)
	{
		for (unsigned int x = 0u; x < size; ++x)
		{
			const Vector2 moduleLocation = Vector2(Scalar(x) + Scalar(0.5), Scalar(y) + Scalar(0.5));
			Vector2 moduleImageLocation;

			if (homography.multiply(moduleLocation, moduleImageLocation))
			{
				const unsigned int ix = (unsigned int)(moduleImageLocation.x() + Scalar(0.5));
				const unsigned int iy = (unsigned int)(moduleImageLocation.y() + Scalar(0.5));
				if (ix < width && iy < height)
				{
					modules[moduleIndex] = yFrame[iy * (width + yFramePaddingElements) + ix] < grayThreshold ? 1u : 0u;
				}
			}

			moduleIndex++;
		}
	}

	return true;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
