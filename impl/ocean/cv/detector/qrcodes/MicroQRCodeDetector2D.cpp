/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/MicroQRCodeDetector2D.h"

#include "ocean/cv/detector/qrcodes/MicroQRCodeDecoder.h"
#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"
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

MicroQRCodes MicroQRCodeDetector2D::detectMicroQRCodes(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, Observations* observations, Worker* worker)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(yFrame != nullptr);

	if (width < 15u || height < 15u) // == 11 modules (v M1) + 2 * 2 modules for the quiet zone
	{
		return MicroQRCodes();
	}

	const FinderPatterns finderPatterns = FinderPatternDetector::detectFinderPatterns(yFrame, width, height, /* minimumDistance */ 10u, paddingElements, worker);

	constexpr size_t maximumNumberOfDetectableCodes = 10;

	if (finderPatterns.size() > maximumNumberOfDetectableCodes)
	{
		// Too many finder patterns; abort here to avoid potential spike in the runtime performance
		return MicroQRCodes();
	}

	MicroQRCodes codes;
	Observations localObservations;

	for (size_t index = 0; index < finderPatterns.size(); ++index)
	{
		const FinderPattern& finderPattern = finderPatterns[index];

		const bool isNormalReflectance = finderPattern.isNormalReflectance();
		const unsigned int grayThreshold = finderPattern.grayThreshold();

		std::vector<unsigned int> provisionalVersions;
		HomogenousMatrices4 code_T_cameras;

		if (computePosesAndProvisionalVersions(anyCamera, yFrame, width, height, paddingElements, finderPattern, code_T_cameras, provisionalVersions))
		{
			bool foundCode = false;

			for (size_t i = 0; !foundCode && i < code_T_cameras.size() && i < provisionalVersions.size(); ++i)
			{
				const HomogenousMatrix4& code_T_camera = code_T_cameras[i];
				const unsigned int provisionalVersion = provisionalVersions[i];

				ocean_assert(code_T_camera.isValid());

				std::vector<uint8_t> modules;
				if (!extractModulesFromImage(anyCamera, yFrame, width, height, paddingElements, provisionalVersion, code_T_camera, isNormalReflectance, grayThreshold, modules))
				{
					continue;
				}

				// Verify encoded version matches the provisional version
				const unsigned int version = extractVersionFromModules(provisionalVersion, modules);
				if (provisionalVersion != version)
				{
					continue;
				}

				// Decode Micro QR code
				MicroQRCode code;

				if (MicroQRCodeDecoder::decodeMicroQRCode(modules, code))
				{
					ocean_assert(code.isValid());
					codes.emplace_back(std::move(code));

					localObservations.emplace_back(code_T_camera, finderPattern);

					foundCode = true;

					break;
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
