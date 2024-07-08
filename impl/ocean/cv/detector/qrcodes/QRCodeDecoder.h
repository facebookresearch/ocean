/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * Definition of a QR code decoder
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT QRCodeDecoder
{
	public:

		/**
		 * Decodes the modules of a QR code
		 * @param modules The modules of a QR code, e.g., as sampled after a detection, must be valid
		 * @param code The resulting QR code instance that will hold the decoded data
		 * @return True if the modules are successfully decoded, otherwise false
		 */
		static bool decodeQRCode(const std::vector<uint8_t>& modules, QRCode& code);
};

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
