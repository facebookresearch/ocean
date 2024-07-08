/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"
#include "ocean/cv/detector/qrcodes/QRCodeDecoder.h"
#include "ocean/cv/detector/qrcodes/TransitionDetector.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

QRCodes QRCodeDetector2D::detectQRCodes(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, Observations* observations, Worker* worker)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame != nullptr);

	if (width < 29u || height < 29u) // == 21 modules (V1) + 2 * 4 modules for the quiet zone
	{
		return QRCodes();
	}

	const FinderPatterns finderPatterns = FinderPatternDetector::detectFinderPatterns(yFrame, width, height, /* minimumDistance */ 10u, paddingElements, worker);

	if (finderPatterns.size() < 3)
	{
		// Not enough finder patterns.
		return QRCodes();
	}

	constexpr size_t maximumNumberOfDetectableCodes = 5;
	constexpr size_t maximumNumberOfFinderPatterns = 3 * maximumNumberOfDetectableCodes;

	if (finderPatterns.size() > maximumNumberOfFinderPatterns)
	{
		// Too many finder patterns; abort here to avoid potential spike in the runtime performance
		return QRCodes();
	}

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	{
		if (!QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24))
		{
			QRCodeDebugElements::get().activateElement(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24);
		}

		Frame ySourceFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_COPY_REMOVE_PADDING_LAYOUT, paddingElements);
		Frame rgbSourceFrame;

		CV::FrameConverter::Comfort::convert(ySourceFrame, FrameType::FORMAT_RGB24, rgbSourceFrame, /* forceCopy */ true, worker);
		ocean_assert(rgbSourceFrame.isValid());

		QRCodeDebugElements::get().updateElement(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24, std::move(rgbSourceFrame));
	}

	const uint8_t* colors[4] =
	{
		CV::Canvas::red(FrameType::FORMAT_RGB24),
		CV::Canvas::green(FrameType::FORMAT_RGB24),
		CV::Canvas::blue(FrameType::FORMAT_RGB24),
		CV::Canvas::yellow(FrameType::FORMAT_RGB24),
	};

	if (QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_FINDER_PATTERNS))
	{
		Frame eiFinderPatternsFrame = QRCodeDebugElements::get().element(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24);
		ocean_assert(eiFinderPatternsFrame.isValid());

		for (const FinderPattern& finderPattern : finderPatterns)
		{
			Utilities::drawFinderPattern(eiFinderPatternsFrame, finderPattern, colors[1]);
		}

		QRCodeDebugElements::get().updateElement(QRCodeDebugElements::EI_FINDER_PATTERNS, std::move(eiFinderPatternsFrame));
	}
#endif // OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

	IndexTriplets indexTriplets = FinderPatternDetector::extractIndexTriplets(finderPatterns);

	if (indexTriplets.empty())
	{
		// If no triplets have been found with the default parameters, try again with relaxed parameters
		const Scalar distanceScaleTolerance = Scalar(0.35);
		const Scalar moduleSizeScaleTolerance = Scalar(0.7);
		const Scalar angleTolerance = Numeric::deg2rad(Scalar(18));
		indexTriplets = FinderPatternDetector::extractIndexTriplets(finderPatterns, distanceScaleTolerance, moduleSizeScaleTolerance, angleTolerance);

		if (indexTriplets.empty())
		{
			return QRCodes();
		}
	}

	QRCodes codes;
	Observations localObservations;

	for (size_t index = 0; index < indexTriplets.size(); ++index)
	{
		const IndexTriplet& indexTriplet = indexTriplets[index];

		FinderPatternTriplet finderPatternTriplet =
		{
			finderPatterns[indexTriplet[0]], // top-left
			finderPatterns[indexTriplet[1]], // bottom-left
			finderPatterns[indexTriplet[2]], // top-right
		};

		if (finderPatternTriplet[0].isNormalReflectance() != finderPatternTriplet[1].isNormalReflectance()
				|| finderPatternTriplet[0].isNormalReflectance() != finderPatternTriplet[2].isNormalReflectance())
		{
			continue;
		}

		const bool isNormalReflectance = finderPatternTriplet[0].isNormalReflectance();
		const unsigned int grayThreshold = ((finderPatternTriplet[0].grayThreshold() + finderPatternTriplet[1].grayThreshold() + finderPatternTriplet[2].grayThreshold()) * 1024u + 512u) / 3072u;

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	if (QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_FINDER_PATTERN_TRIPLETS))
	{
		const QRCodeDebugElements::ScopedHierarchy scopedHierarchyTagID(String::toAString(index));

		ocean_assert(QRCodeDebugElements::get().isElementActive(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24));
		Frame rgbFrame = QRCodeDebugElements::get().element(QRCodeDebugElements::EI_SOURCE_IMAGE_RGB24);
		ocean_assert(rgbFrame.isValid());

		for (const FinderPattern& finderPattern : finderPatternTriplet)
		{
			Utilities::drawFinderPattern(rgbFrame, finderPattern, colors[1]);
		}

		for (size_t i = 0; i < 3u; ++i)
		{
			Utilities::drawLine<1u>(anyCamera, rgbFrame, finderPatternTriplet[i].position(), finderPatternTriplet[(i + 1) % 3].position(), colors[1]);
		}

		QRCodeDebugElements::get().updateElement(QRCodeDebugElements::EI_FINDER_PATTERN_TRIPLETS, std::move(rgbFrame));
	}
#endif // OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

		constexpr unsigned int maxAllowedVersionDifference = 5u;

		unsigned int versionLow = (unsigned int)(-1);
		unsigned int versionHigh = 0u;

		bool computedVersionRange = false;

		if (anyCamera.anyCameraType() == AnyCameraType::PINHOLE)
		{
			if (computeProvisionalVersionRange(finderPatternTriplet[0], finderPatternTriplet[1], finderPatternTriplet[2], versionHigh, versionLow))
			{
				if (versionLow > versionHigh)
				{
					std::swap(versionLow, versionHigh);
				}

				computedVersionRange = true;
			}
		}
		else
		{
			computedVersionRange = computeProvisionalVersionRange(anyCamera, finderPatternTriplet.data(), maxAllowedVersionDifference, versionLow, versionHigh);
		}

		if (!computedVersionRange)
		{
			continue;
		}

		ocean_assert(versionLow >= 1u && versionLow <= versionHigh && versionHigh <= 40u && (versionHigh - versionLow <= maxAllowedVersionDifference));

		// To account for potential rounding errors and perspective, expand the version range a little.
		versionLow = versionLow >= 3u ? versionLow - 2u : 1u;
		versionHigh = versionHigh <= 38u ? versionHigh + 2u : 40u;

		int versionI = int((versionLow + versionHigh) / 2u);
		const int numberOffsets = int(versionHigh - versionLow + 1);

		HomogenousMatrices4 code_T_cameras;

		bool foundCode = false;
		for (int offset = 0; !foundCode && offset < numberOffsets; ++offset)
		{
			versionI = offset % 2 == 0 ? (versionI - offset) : (versionI + offset);

			if (versionI < int(versionLow) || versionI > int(versionHigh))
			{
				continue;
			}

			ocean_assert(versionI != 0 && versionI < 41);

			const unsigned int version = (unsigned int)versionI;

			code_T_cameras.clear();

			if (computePoses(anyCamera, yFrame, width, height, paddingElements, finderPatternTriplet.data(), version, code_T_cameras))
			{
				for (const HomogenousMatrix4& code_T_camera : code_T_cameras)
				{
					ocean_assert(code_T_camera.isValid());

					std::vector<uint8_t> modules;
					if (!extractModulesFromImage(anyCamera, yFrame, width, height, paddingElements, version, code_T_camera, isNormalReflectance, grayThreshold, modules))
					{
						continue;
					}

					QRCode code;
					if (QRCodeDecoder::decodeQRCode(modules, code))
					{
						ocean_assert(code.isValid());
						codes.emplace_back(std::move(code));

						localObservations.emplace_back(code_T_camera, std::move(finderPatternTriplet));

						foundCode = true;

						break;
					}
				}
			}
		}
	}

	if (observations != nullptr)
	{
		*observations = std::move(localObservations);
	}

	return codes;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
