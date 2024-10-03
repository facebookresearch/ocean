/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/MicroQRCode.h"

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
class OCEAN_CV_DETECTOR_QRCODES_EXPORT MicroQRCodeDecoder
{
	public:

		/*
		 * Definition of a bit stream
		 */
		class BitStream
		{
			public:

				/**
				 * Constructor for bit streams
				 * @param buffer The buffer holding the bit stream, must be valid
				 * @param numberOfBits The number of bits in the bit stream, range: [0, 8 * buffer.size()]
				 */
				BitStream(std::vector<uint8_t>&& buffer, const unsigned int numberOfBits);

				/**
				 * Returns the number of bits remaining in the bit stream
				 */
				inline unsigned int bitsRemaining();

				/**
				 * Consumes a number of bits from the bit stream
				 * @param numberOfBits The number of bits to consume, range: [0, max(32, bitsRemaining())]
				 * @return The consumed bits
				 */
				uint32_t consumeBits(const unsigned int numberOfBits);

				/**
				 * Peeks at a number of bits from the bit stream, checking if any are non-zero
				 * @param numberOfBits The number of bits to peek, range: [0, bitsRemaining()]
				 * @return True if any of the bits are non-zero, otherwise false
				 */
				bool peekNonzeroBits(const unsigned int numberOfBits) const;

			protected:

				/**
				 * Consumes a single bit from the bit stream
				 * @return True if the bit is non-zero, otherwise false
				 */
				bool consumeBit();

				/// The buffer holding the bit stream
				std::vector<uint8_t> buffer_;

				/// The portion of the current byte that have already been consumed
				unsigned int bitsConsumed_;

				/// The number of bytes that have already been consumed
				unsigned int bytesConsumed_;

				/// The number of bits remaining in the bit stream
				unsigned int bitsRemaining_;
		};

		/**
		 * Decodes the modules of a Micro QR code
		 * @param modules The modules of a Micro QR code, e.g., as sampled after a detection, must be valid
		 * @param code The resulting QR code instance that will hold the decoded data
		 * @return True if the modules are successfully decoded, otherwise false
		 */
		static bool decodeMicroQRCode(const std::vector<uint8_t>& modules, MicroQRCode& code);
};

unsigned int MicroQRCodeDecoder::BitStream::bitsRemaining()
{
	return bitsRemaining_;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
