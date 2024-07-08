/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/barcodes/ParserEan13Upca.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Barcodes
{

template <bool tReportUpcASeparately>
bool ParserEan13Upca::parse(const uint32_t* segmentData, const size_t size, Barcode& barcode, IndexPair32& xCoordinates)
{
	ocean_assert(segmentData != nullptr);

	// EAN-13/UPC-A
	//
	// S - Left guard
	// L - Left block of 6 digits
	// M - Middle guard
	// R - Right block of 6 digits
	// E - Right guard
	//
	// Structure:                     S   LLLLLL    M   RRRRRR    E
	// Number of segments:       59 = 3 + (6 * 4) + 5 + (6 * 4) + 3
	// Number of modules:        95 = 3 + (6 * 7) + 5 + (6 * 7) + 3
	// Offsets (segmentData + X):     0   3        27   32       56

	constexpr size_t barcodeSizeInSegments = 59;

	if (size >= barcodeSizeInSegments)
	{
		uint32_t moduleSize = 0u;
		uint32_t minModuleSize = 0u;
		uint32_t maxModuleSize = 0u;

		if (isLeftGuard(segmentData, size, moduleSize, minModuleSize, maxModuleSize))
		{
			ocean_assert(moduleSize != 0u && minModuleSize <= moduleSize && moduleSize <= maxModuleSize);

			if (isMiddleGuard(segmentData + 27, size - 27, minModuleSize, maxModuleSize))
			{
				if (isRightGuard(segmentData + 56, size - 56, minModuleSize, maxModuleSize))
				{
					std::vector<uint8_t> digits;
					if (decodeDigits(segmentData + 3, segmentData + 32, moduleSize, minModuleSize, maxModuleSize, digits))
					{
						ocean_assert(digits.size() == 12 || digits.size() == 13);

						BarcodeType barcodeType = BarcodeType::INVALID;
						size_t startDigitIndex = 0;

						if (tReportUpcASeparately && digits[0] == 0u)
						{
							// For compatibility, the GS-1 standard reserves EAN-13 codes with prefix 0 for UPC-A.
							barcodeType = BarcodeType::UPC_A;
							startDigitIndex = 1;
						}
						else
						{
							barcodeType = BarcodeType::EAN_13;
							startDigitIndex = 0;
						}

						std::string barcodeData(digits.size() - startDigitIndex, '0');

						for (size_t digitIndex = startDigitIndex; digitIndex < digits.size(); ++digitIndex)
						{
							ocean_assert(digits[digitIndex] < 10u);
							barcodeData[digitIndex] = '0' + digits[digitIndex];
						}

						ocean_assert(barcodeType != BarcodeType::INVALID && !barcodeData.empty());

						// Determine the width of the barcode in pixels by summing the width the segments it occupies.
						constexpr Index32 xStart = 0u;
						uint32_t widthInPixels = 0u;

						for (size_t segmentIndex = 0; segmentIndex < barcodeSizeInSegments; ++segmentIndex)
						{
							widthInPixels += segmentData[segmentIndex];
						}

						ocean_assert(widthInPixels != 0u);

						barcode = Barcode(barcodeType, std::move(barcodeData));
						xCoordinates = IndexPair32(xStart, xStart + Index32(widthInPixels) - Index32(1)); // Inclusive!

						return true;
					}
				}
			}
		}
	}

	barcode = Barcode();
	xCoordinates = IndexPair32(Index32(-1), Index32(-1));

	return false;
}

// Explicit instantiations
template bool ParserEan13Upca::parse<true>(const uint32_t*, const size_t, Barcode&, IndexPair32&);
template bool ParserEan13Upca::parse<false>(const uint32_t*, const size_t, Barcode&, IndexPair32&);

bool ParserEan13Upca::isLeftGuard(const uint32_t* segmentData, const size_t size, uint32_t& moduleSize, uint32_t& minModuleSize, uint32_t& maxModuleSize)
{
	ocean_assert(segmentData != nullptr);
	ocean_assert_and_suppress_unused(size >= 3, size);

	// Rounded average module size
	moduleSize = (2u * (segmentData[0] + segmentData[1] + segmentData[2]) + 3u) / 6u;

	if (moduleSize == 0u)
	{
		return false;
	}

	// Accepted deviation of the module widths (rounded)
	minModuleSize = std::max(1u, (40u * moduleSize + 50u) / 100u); // ~ 40%
	maxModuleSize = (160u * moduleSize + 50u) / 100u; // ~ 160%

	ocean_assert(minModuleSize != 0u && minModuleSize <= moduleSize && moduleSize <= maxModuleSize);

	for (size_t i = 0; i < 3; ++i)
	{
		if (segmentData[i] < minModuleSize || segmentData[i] > maxModuleSize)
		{
			// The deviation is too large, it's likely not a guard pattern
			return false;
		}
	}

	return true;
}

bool ParserEan13Upca::isMiddleGuard(const uint32_t* segmentData, const size_t size, const uint32_t& minModuleSize, const uint32_t& maxModuleSize)
{
	ocean_assert(segmentData != nullptr);
	ocean_assert_and_suppress_unused(size >= 5, size);
	ocean_assert(minModuleSize <= maxModuleSize);

	for (size_t i = 0; i < 5; ++i)
	{
		if (segmentData[i] < minModuleSize || segmentData[i] > maxModuleSize)
		{
			// The deviation is too large, it's likely not a guard pattern
			return false;
		}
	}

	return true;
}

bool ParserEan13Upca::isRightGuard(const uint32_t* segmentData, const size_t size, const uint32_t& minModuleSize, const uint32_t& maxModuleSize)
{
	ocean_assert(segmentData != nullptr);
	ocean_assert_and_suppress_unused(size >= 3, size);
	ocean_assert(minModuleSize <= maxModuleSize);

	for (size_t i = 0; i < 3; ++i)
	{
		if (segmentData[i] < minModuleSize || segmentData[i] > maxModuleSize)
		{
			// The deviation is too large, it's likely not a guard pattern
			return false;
		}
	}

	return true;
}

bool ParserEan13Upca::decodeDigits(const uint32_t* leftDigitsSegmentData, const uint32_t* rightDigitsSegmentData, const uint32_t moduleSize, const uint32_t& minModuleSize, const uint32_t& maxModuleSize, std::vector<uint8_t>& decodedDigits)
{
	ocean_assert(leftDigitsSegmentData != nullptr && rightDigitsSegmentData != nullptr);
	ocean_assert(leftDigitsSegmentData != rightDigitsSegmentData);
	ocean_assert(moduleSize != 0u && moduleSize >= minModuleSize && moduleSize <= maxModuleSize);

	// Decode the digits 1-12 individually from the run-length encoded segments. The parities of the digits 1-6 encode digit 0, which is handled later.

	std::vector<uint8_t> digits(13, 0u);

	uint32_t leftParityBits = 0u;

	for (size_t digitIndex = 0; digitIndex < 6; ++digitIndex)
	{
		const size_t digitSegmentOffset = 4 * digitIndex;

		// Left block - uses the L- and G-codes (even and odd parity)
		uint8_t leftDigit = 0xFFu;
		unsigned int leftParityBit = 0xFFFFFFFFu;

		if (!decodeDigit<true>(leftDigitsSegmentData + digitSegmentOffset, moduleSize, minModuleSize, maxModuleSize, leftDigit, leftParityBit))
		{
			return false;
		}

		// Right block - only uses the R-codes (always odd parity)
		uint8_t rightDigit = 0xFFu;
		unsigned int rightParityBit = 0xFFFFFFFFu; // Ignored, except for the assert below.

		if (!decodeDigit<false>(rightDigitsSegmentData + digitSegmentOffset, moduleSize, minModuleSize, maxModuleSize, rightDigit, rightParityBit))
		{
			return false;
		}

		ocean_assert(leftParityBit <= 1u && rightParityBit == 1u);
		leftParityBits = (leftParityBits << 1u) | leftParityBit;

		ocean_assert(leftDigit < 10u && rightDigit < 10u);
		digits[1 + digitIndex] = leftDigit;
		digits[1 + digitIndex + 6] = rightDigit;
	}

	// Determine digit 0 of the barcode from the parity bits of the L-codes (odd or 1) and G-codes (even or 0)
	// The index is of the parity patterns below will be the corresponding value of digit 0.
	constexpr uint32_t parityPatterns[10] =
	{
		0b111111u, // 0
		0b110100u, // 1
		0b110010u, // 2
		0b110001u, // 3
		0b101100u, // 4
		0b100110u, // 5
		0b100011u, // 6
		0b101010u, // 7
		0b101001u, // 8
		0b100101u, // 9
	};

	for (uint8_t patternIndex = 0u; patternIndex < 10u; ++patternIndex)
	{
		if (leftParityBits == parityPatterns[patternIndex])
		{
			ocean_assert(patternIndex < 10u);
			digits[0] = patternIndex;

			break;
		}
	}

	if (verifyCheckDigit(digits.data(), digits.size()))
	{
		decodedDigits = std::move(digits);

		return true;
	}

	return false;
}

template <bool tUseCodesLAndG>
bool ParserEan13Upca::decodeDigit(const uint32_t* digitsSegmentData, const uint32_t moduleSize, const uint32_t& minModuleSize, const uint32_t& maxModuleSize, uint8_t& decodedDigit, unsigned int& parityBit)
{
	ocean_assert(digitsSegmentData != nullptr);
	ocean_assert(moduleSize != 0u && moduleSize >= minModuleSize && moduleSize <= maxModuleSize);

	// The two blocks of digits on a barcode are encoded differently. The left block (digits 1-6) uses both, L- and G-codes.
	// The right block (digits 7-12) only uses the R-codes.
	//
	// The first digit (digit 0) is encoded by the parity of the digits in the left block (L-codes: odd parity, G-codes: even parity).
	// This function does not decode digit 0, it only extracts the parity and the caller will have to decode the last digit.
	constexpr std::array<DigitPattern, 20> digitPatternsLG =
	{
		// L-Codes                       Digit | Modules
		DigitPattern{3u, 2u, 1u, 1u}, //   0   | 0001101
		DigitPattern{2u, 2u, 2u, 1u}, //   1   | 0011001
		DigitPattern{2u, 1u, 2u, 2u}, //   2   | 0010011
		DigitPattern{1u, 4u, 1u, 1u}, //   3   | 0111101
		DigitPattern{1u, 1u, 3u, 2u}, //   4   | 0100011
		DigitPattern{1u, 2u, 3u, 1u}, //   5   | 0110001
		DigitPattern{1u, 1u, 1u, 4u}, //   6   | 0101111
		DigitPattern{1u, 3u, 1u, 2u}, //   7   | 0111011
		DigitPattern{1u, 2u, 1u, 3u}, //   8   | 0110111
		DigitPattern{3u, 1u, 1u, 2u}, //   9   | 0001011

		// G-Codes                       Digit | Modules
		DigitPattern{1u, 1u, 2u, 3u}, //   0   | 0100111
		DigitPattern{1u, 2u, 2u, 2u}, //   1   | 0110011
		DigitPattern{2u, 2u, 1u, 2u}, //   2   | 0011011
		DigitPattern{1u, 1u, 4u, 1u}, //   3   | 0100001
		DigitPattern{2u, 3u, 1u, 1u}, //   4   | 0011101
		DigitPattern{1u, 3u, 2u, 1u}, //   5   | 0111001
		DigitPattern{4u, 1u, 1u, 1u}, //   6   | 0000101
		DigitPattern{2u, 1u, 3u, 1u}, //   7   | 0010001
		DigitPattern{3u, 1u, 2u, 1u}, //   8   | 0001001
		DigitPattern{2u, 1u, 1u, 3u}, //   9   | 0010111
	};

	constexpr std::array<DigitPattern, 10> digitPatternsR =
	{
		// R-Codes                       Digit | Modules
		DigitPattern{3u, 2u, 1u, 1u}, //   0   | 1110010
		DigitPattern{2u, 2u, 2u, 1u}, //   1   | 1100110
		DigitPattern{2u, 1u, 2u, 2u}, //   2   | 1101100
		DigitPattern{1u, 4u, 1u, 1u}, //   3   | 1000010
		DigitPattern{1u, 1u, 3u, 2u}, //   4   | 1011100
		DigitPattern{1u, 2u, 3u, 1u}, //   5   | 1001110
		DigitPattern{1u, 1u, 1u, 4u}, //   6   | 1010000
		DigitPattern{1u, 3u, 1u, 2u}, //   7   | 1000100
		DigitPattern{1u, 2u, 1u, 3u}, //   8   | 1001000
		DigitPattern{3u, 1u, 1u, 2u}, //   9   | 1110100
	};

	const DigitPattern* digitPatterns = tUseCodesLAndG ? digitPatternsLG.data() : digitPatternsR.data();
	const size_t digitPatternsSize = tUseCodesLAndG ? digitPatternsLG.size() : digitPatternsR.size();

	// Find the digit pattern that has the smallest difference to the input pattern (within a certain tolerance range)
	constexpr size_t invalidIndex = size_t(-1);

	size_t bestMatchIndex = invalidIndex;
	uint32_t bestMatchScore = NumericT<uint32_t>::maxValue();

	for (size_t patternIndex = 0; patternIndex < digitPatternsSize; ++patternIndex)
	{
		const uint32_t score = computePatternScore(digitsSegmentData, digitPatterns[patternIndex], moduleSize, minModuleSize, maxModuleSize);

		if (score < bestMatchScore)
		{
			bestMatchScore = score;
			bestMatchIndex = patternIndex;
		}
	}

	ocean_assert(bestMatchIndex == invalidIndex || bestMatchIndex < digitPatternsSize);

	if (bestMatchIndex == invalidIndex)
	{
		return false;
	}

	ocean_assert(bestMatchIndex < digitPatternsSize);

	if constexpr (tUseCodesLAndG)
	{
		if (bestMatchIndex < 10)
		{
			// This is an L-code, which has an odd parity
			decodedDigit = (uint8_t)bestMatchIndex;
			parityBit = 1u;
		}
		else
		{
			// This is an G-code, which has an even parity.
			decodedDigit = (uint8_t)(bestMatchIndex - 10);
			parityBit = 0u;
		}
	}
	else
	{
		// This is an R-code, which has an odd parity
		decodedDigit = (uint8_t)bestMatchIndex;
		parityBit = 1u;
	}

	ocean_assert(decodedDigit < 10u);

	return true;
}

uint32_t ParserEan13Upca::computePatternScore(const uint32_t* digitsSegmentData, const DigitPattern& digitPattern, const uint32_t moduleSize, const uint32_t& minModuleSize, const uint32_t& maxModuleSize)
{
	ocean_assert(digitsSegmentData != nullptr);
	ocean_assert(moduleSize != 0u && moduleSize >= minModuleSize && moduleSize <= maxModuleSize);

	uint32_t score = 0u;

	for (size_t patternIndex = 0; patternIndex < 4; ++patternIndex)
	{
		ocean_assert(digitPattern[patternIndex] != 0u);

		const uint32_t segmentSize = digitPattern[patternIndex] * moduleSize;
		const uint32_t minSegmentSize = digitPattern[patternIndex] * minModuleSize;
		const uint32_t maxSegmentSize = digitPattern[patternIndex] * maxModuleSize;

		if (digitsSegmentData[patternIndex] < minSegmentSize || digitsSegmentData[patternIndex] > maxSegmentSize)
		{
			// The difference is outside the accepted tolerance range.
			return NumericT<uint32_t>::maxValue();
		}

		const int32_t difference = int32_t(digitsSegmentData[patternIndex]) - int32_t(segmentSize);

		score += uint32_t(difference * difference);
	}

	return score;
}

bool ParserEan13Upca::verifyCheckDigit(const uint8_t* barcodeDigits, const size_t numberDigits)
{
	ocean_assert(barcodeDigits != nullptr);

	if (numberDigits != 13)
	{
		ocean_assert(false && "Invalid EAN-13/UPC-A data.");

		return false;
	}

	unsigned int checksum = 0u;

	for (size_t i = 0; i < numberDigits - 1; i += 2)
	{
		ocean_assert(barcodeDigits[i + 0] < 10u && barcodeDigits[i + 1] < 10u);

		checksum += 1u * barcodeDigits[i + 0];
		checksum += 3u * barcodeDigits[i + 1];
	}

	const unsigned int expectedCheckDigit = (10u - (checksum % 10u)) % 10u;
	ocean_assert(expectedCheckDigit < 10u);

	ocean_assert(barcodeDigits[numberDigits - 1u] < 10u);
	const unsigned int actualCheckDigit = barcodeDigits[numberDigits - 1u];

	if (actualCheckDigit == expectedCheckDigit)
	{
		return true;
	}

	return false;
}

} // namespace Barcodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
