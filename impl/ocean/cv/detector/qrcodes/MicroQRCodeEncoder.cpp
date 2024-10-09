/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/MicroQRCodeEncoder.h"
#include <cstdint>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

QRCodeEncoderBase::StatusCode MicroQRCodeEncoder::encodeText(const std::string& text, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, MicroQRCode& qrcode)
{
	Segments segments;
	MicroQRCode::EncodingMode encodingMode = MicroQRCode::EM_INVALID_ENCODING_MODE;

	std::vector<uint8_t> data(text.begin(), text.end());

	if (errorCorrectionCapacity == MicroQRCode::ECC_30)
	{
		ocean_assert(false && "Invalid error correction capability for Micro QR codes.");
		return SC_INVALID_ERROR_CORRECTION_CAPACITY;
	}

	if (Segment::isNumericData(text))
	{
		const StatusCode status = Segment::generateSegmentNumeric(text, segments);
		if (status != SC_SUCCESS)
		{
			return status;
		}

		encodingMode = MicroQRCode::EM_NUMERIC;
	}
	else if (Segment::isAlphanumericData(text))
	{
		const StatusCode status = Segment::generateSegmentAlphanumeric(text, segments);
		if (status != SC_SUCCESS)
		{
			return status;
		}

		encodingMode = MicroQRCode::EM_ALPHANUMERIC;
	}
	else
	{
		const StatusCode status = Segment::generateSegmentsBytes(data, segments);
		if (status != SC_SUCCESS)
		{
			return status;
		}

		encodingMode = MicroQRCode::EM_BYTE;
	}

	if (encodingMode == MicroQRCode::EM_INVALID_ENCODING_MODE)
	{
		return SC_INVALID_DATA;
	}

	std::vector<uint8_t> modules;
	unsigned int version;
	MicroQRCode::ErrorCorrectionCapacity finalErrorCorrectionCapacity;

	StatusCode status = encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity);

	// If the message is too long, try splitting it into multiple segments

	if (status == SC_CODE_CAPACITY_EXCEEDED && encodingMode != MicroQRCode::EM_NUMERIC)
	{
		// Try starting with a numeric segment
		unsigned int numericSegmentLength = 0u;
		while (numericSegmentLength < text.size() && std::isdigit((unsigned char)text[numericSegmentLength]))
		{
			++numericSegmentLength;
		}

		if (numericSegmentLength > 0u)
		{
			segments.clear();
			if (Segment::generateSegmentNumeric(text.substr(0, numericSegmentLength), segments) == SC_SUCCESS)
			{
				StatusCode tmpStatus = SC_UNKNOWN_ERROR;
				if (encodingMode == MicroQRCode::EM_ALPHANUMERIC)
				{
					tmpStatus = Segment::generateSegmentAlphanumeric(text.substr(numericSegmentLength), segments);
				}
				else if (encodingMode == MicroQRCode::EM_BYTE)
				{
					std::vector<uint8_t> byteSegmentData(text.begin() + numericSegmentLength, text.end());
					tmpStatus = Segment::generateSegmentsBytes(byteSegmentData, segments);
				}

				if (tmpStatus == SC_SUCCESS)
				{
					status = encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity);
				}
			}
		}
	}

	if (status == SC_CODE_CAPACITY_EXCEEDED && encodingMode != MicroQRCode::EM_NUMERIC)
	{
		// Try ending with a numeric segment
		unsigned int nonNumericSegmentLength = (unsigned int)(text.size());
		while (nonNumericSegmentLength > 0u && std::isdigit(text[nonNumericSegmentLength - 1u]))
		{
			--nonNumericSegmentLength;
		}

		if (nonNumericSegmentLength < text.size())
		{
			segments.clear();
			StatusCode tmpStatus = SC_UNKNOWN_ERROR;
			if (encodingMode == MicroQRCode::EM_ALPHANUMERIC)
			{
				tmpStatus = Segment::generateSegmentAlphanumeric(text.substr(0, nonNumericSegmentLength), segments);
			}
			else if (encodingMode == MicroQRCode::EM_BYTE)
			{
				std::vector<uint8_t> byteSegmentData(text.begin(), text.begin() + nonNumericSegmentLength);
				tmpStatus = Segment::generateSegmentsBytes(byteSegmentData, segments);
			}

			if (tmpStatus == SC_SUCCESS && Segment::generateSegmentNumeric(text.substr(nonNumericSegmentLength), segments) == SC_SUCCESS)
			{
				status = encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity);
			}
		}
	}

	if (status == SC_CODE_CAPACITY_EXCEEDED && encodingMode == MicroQRCode::EM_BYTE)
	{
		// Try starting with an alphanumeric segment
		unsigned int alphaSegmentLength = 0u;

		while (alphaSegmentLength < text.size() && Segment::isAlphanumericData(text.substr(alphaSegmentLength, 1u)))
		{
			++alphaSegmentLength;
		}

		if (alphaSegmentLength > 0u)
		{
			segments.clear();
			if (Segment::generateSegmentAlphanumeric(text.substr(0, alphaSegmentLength), segments) == SC_SUCCESS)
			{
				std::vector<uint8_t> byteSegmentData(text.begin() + alphaSegmentLength, text.end());
				if (Segment::generateSegmentsBytes(byteSegmentData, segments) == SC_SUCCESS)
				{
					status = encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity);
				}
			}
		}
	}

	if (status == SC_CODE_CAPACITY_EXCEEDED && encodingMode == MicroQRCode::EM_BYTE)
	{
		// Try ending with an alphanumeric segment
		unsigned int nonAlphaSegmentLength = (unsigned int)(text.size());
		while (nonAlphaSegmentLength > 0u && Segment::isAlphanumericData(text.substr(nonAlphaSegmentLength - 1u, 1u)))
		{
			--nonAlphaSegmentLength;
		}

		if (nonAlphaSegmentLength < text.size())
		{
			segments.clear();
			std::vector<uint8_t> byteSegmentData(text.begin(), text.begin() + nonAlphaSegmentLength);
			if (Segment::generateSegmentsBytes(byteSegmentData, segments) == SC_SUCCESS && Segment::generateSegmentAlphanumeric(text.substr(nonAlphaSegmentLength), segments) == SC_SUCCESS)
			{
				status = encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity);
			}
		}
	}

	if (status != SC_SUCCESS)
	{
		return status;
	}

	qrcode = MicroQRCode(std::move(data), encodingMode, finalErrorCorrectionCapacity, std::move(modules), version);
	ocean_assert(qrcode.isValid());

	if (qrcode.isValid())
	{
		return SC_SUCCESS;
	}

	return SC_UNKNOWN_ERROR;
}

QRCodeEncoderBase::StatusCode MicroQRCodeEncoder::encodeBinary(const std::vector<uint8_t>& data, MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, MicroQRCode& qrcode)
{
	Segments segments;

	StatusCode status = Segment::generateSegmentsBytes(data, segments);
	if (status != SC_SUCCESS)
	{
		return status;
	}

	std::vector<uint8_t> modules;
	unsigned int version;
	MicroQRCode::ErrorCorrectionCapacity finalErrorCorrectionCapacity;

	status = encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity);
	if (status != SC_SUCCESS)
	{
		return status;
	}

	std::vector<uint8_t> finalData(data.begin(), data.end());

	qrcode = MicroQRCode(std::move(finalData), MicroQRCode::EM_BYTE, finalErrorCorrectionCapacity, std::move(modules), version);
	ocean_assert(qrcode.isValid());

	if (qrcode.isValid())
	{
		return SC_SUCCESS;
	}

	return SC_UNKNOWN_ERROR;
}

void MicroQRCodeEncoder::addErrorCorrectionAndCreateQRCode(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const Codewords& rawCodewords, MaskingPattern mask, std::vector<uint8_t>& modules)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);
	ocean_assert(mask < 4u || mask == (unsigned int)(-1));

	std::vector<uint8_t> localModules(MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version), 0u);

	std::vector<uint8_t> functionPatternMask = MicroQRCodeEncoder::setFunctionPatterns(localModules, version, errorCorrectionCapacity);

	const Codewords eccCodewords = MicroQRCodeEncoder::addErrorCorrection(rawCodewords, version, errorCorrectionCapacity);
	MicroQRCodeEncoder::setCodewords(localModules, eccCodewords, version, errorCorrectionCapacity, functionPatternMask);

	// Determine mask index, if applicable
	if (mask == (unsigned int)(-1))
	{
		unsigned int maximumScore = 0u;

		constexpr MaskingPattern maskingPatterns[4] =
		{
			MP_PATTERN_0,
			MP_PATTERN_1,
			MP_PATTERN_2,
			MP_PATTERN_3,
		};

		for (size_t m = 0; m < 4; ++m)
		{
			MicroQRCodeEncoder::applyMaskPattern(localModules, version, functionPatternMask, maskingPatterns[m]);
			MicroQRCodeEncoder::setFormatInformation(localModules, version, errorCorrectionCapacity, maskingPatterns[m], functionPatternMask);

			const unsigned int score = MicroQRCodeEncoder::computeMaskPatternScore(localModules, version);

			if (score > maximumScore)
			{
				mask = maskingPatterns[m];
				maximumScore = score;
			}

			// Undo applying the mask (possible because of XOR)
			MicroQRCodeEncoder::applyMaskPattern(localModules, version, functionPatternMask, maskingPatterns[m]);
		}
	}

	ocean_assert(mask < 4u);

	MicroQRCodeEncoder::applyMaskPattern(localModules, version, functionPatternMask, mask);
	MicroQRCodeEncoder::setFormatInformation(localModules, version, errorCorrectionCapacity, mask, functionPatternMask);

	modules = std::move(localModules);
}

QRCodeEncoderBase::StatusCode MicroQRCodeEncoder::encodeSegments(const Segments& segments, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>& modules, unsigned int& version, MicroQRCode::ErrorCorrectionCapacity& finalErrorCorrectionCapacity, unsigned int minVersion, unsigned int maxVersion, const MaskingPattern mask, const bool maximizeErrorCorrectionCapacity)
{
	ocean_assert(segments.empty() == false);
	ocean_assert(minVersion >= 1u && minVersion <= maxVersion && maxVersion <= MicroQRCode::MAX_VERSION);
	ocean_assert(mask == (unsigned int)(-1) || (mask >= 1u && mask <= 3u));

	// Adjust version limits to avoid invalid formats

	for(auto & segment : segments)
	{
		if (segment.encodationMode() == MicroQRCode::EM_ALPHANUMERIC)
		{
			minVersion = std::max(minVersion, 2u);
		}
		else if (segment.encodationMode() == MicroQRCode::EM_BYTE)
		{
			minVersion = std::max(minVersion, 3u);
		}
	}

	if (errorCorrectionCapacity == MicroQRCode::ECC_25)
	{
		minVersion = 4u;
	}
	if (errorCorrectionCapacity != MicroQRCode::ECC_DETECTION_ONLY)
	{
		minVersion = std::max(minVersion, 2u);
	}
	if (errorCorrectionCapacity == MicroQRCode::ECC_DETECTION_ONLY && !maximizeErrorCorrectionCapacity)
	{
		maxVersion = 1u;
	}

	// Determine the lowest version that can hold the data (in range [minVersion, maxVersion], if it exists)

	version = 0u;
	unsigned int bitsUsed = 0u;
	MicroQRCode::ErrorCorrectionCapacity minimumErrorCorrectionCapacity = errorCorrectionCapacity;

	for (unsigned int tempVersion = minVersion; tempVersion <= maxVersion; ++tempVersion)
	{
		MicroQRCode::ErrorCorrectionCapacity tempErrorCorrectionCapacity = minimumErrorCorrectionCapacity;
		if (tempVersion > 1u && errorCorrectionCapacity == MicroQRCode::ECC_DETECTION_ONLY)
		{
			if (maximizeErrorCorrectionCapacity)
			{
				tempErrorCorrectionCapacity = MicroQRCode::ECC_07;
			}
			else
			{
				break;
			}
		}

		const unsigned int maxDataCapacityBits = MicroQRCodeEncoder::totalNumberDataCodewordBits(tempVersion, tempErrorCorrectionCapacity);

		if (MicroQRCodeEncoder::computeTotalBitsUsed(segments, tempVersion, bitsUsed) && bitsUsed != 0u && bitsUsed <= maxDataCapacityBits)
		{
			version = tempVersion;
			minimumErrorCorrectionCapacity = tempErrorCorrectionCapacity;
			break;
		}
	}

	if (version < MicroQRCode::MIN_VERSION || version > MicroQRCode::MAX_VERSION || bitsUsed == 0u)
	{
		return SC_CODE_CAPACITY_EXCEEDED;
	}

	// Determine the highest error correction level that still fits into the selected version

	finalErrorCorrectionCapacity = minimumErrorCorrectionCapacity;

	if (maximizeErrorCorrectionCapacity && version >= 2u)
	{
		constexpr MicroQRCode::ErrorCorrectionCapacity capacities[2] = { MicroQRCode::ECC_15, MicroQRCode::ECC_25 };

		size_t validCapacities = (version == 4u ? 2u : 1u);

		for (size_t i = 0; i < validCapacities; ++i)
		{
			if (MicroQRCode::getErrorCorrectionCapacityValue(minimumErrorCorrectionCapacity) >= MicroQRCode::getErrorCorrectionCapacityValue(capacities[i]))
			{
				continue;
			}

			if (bitsUsed > MicroQRCodeEncoder::totalNumberDataCodewordBits(version, capacities[i]))
			{
				break;
			}

			finalErrorCorrectionCapacity = capacities[i];
		}
	}

	// Concatenate all segments

	BitBuffer bitBuffer;

	for (const Segment& segment : segments)
	{
		// Mode indicator, 0 to 3 bits, depending on version
		// |   Character count indicator, N bits
		// |   |              Binary data, M bits
		// |   |              |
		// ... 01234567890... 0123...
		unsigned int modeIndicatorBits = version - 1u;
		Segment::bitBufferAppend(MicroQRCodeEncoder::encodationModeIndicatorBitSequence(segment.encodationMode()), modeIndicatorBits, bitBuffer);
		Segment::bitBufferAppend(segment.characters(), getBitsInCharacterCountIndicator(version, segment.encodationMode()), bitBuffer);
		bitBuffer.insert(bitBuffer.end(), segment.bitBuffer().begin(), segment.bitBuffer().end());
	}
	ocean_assert(bitBuffer.size() == bitsUsed);

	const unsigned int dataCapacityBits = MicroQRCodeEncoder::totalNumberDataCodewordBits(version, finalErrorCorrectionCapacity);

	// Add terminator sequence (cf. ISO/IEC 18004:2015, Section 7.4.9)

	ocean_assert(bitBuffer.size() <= dataCapacityBits);
	size_t terminatorSize = std::min(size_t(2u * version + 1u), size_t(dataCapacityBits) - bitBuffer.size());
	Segment::bitBufferAppend(0u, terminatorSize, bitBuffer);

	// Determine the end of the last full-sized byte

	const unsigned int endOfLastFullByte = (dataCapacityBits / 8u) * 8u;
	ocean_assert(endOfLastFullByte % 8 == 0);

	// Add padding to end at a full-size codeword boundary

	const unsigned int paddingBits = (bitBuffer.size() >= endOfLastFullByte) ? 0u : (8 - (bitBuffer.size() % 8));
	if (paddingBits != 0u && paddingBits < 8u)
	{
		Segment::bitBufferAppend(0u, paddingBits, bitBuffer);
	}
	ocean_assert(bitBuffer.size() % 8 == 0 || ((version & 1) && bitBuffer.size() > endOfLastFullByte));

	// If the capacity of 8-bit codewords has not been reached, fill the remainder by alternating between the two 8-bit padding constants

	for (uint8_t paddingByte = 0xEC; bitBuffer.size() < endOfLastFullByte; paddingByte ^= 0xEC ^ 0x11)
	{
		Segment::bitBufferAppend(paddingByte, 8, bitBuffer);
	}

	// If there is any capacity left, it should be the 4-bit codeword used only by versions M1 and M3, which should be filled with 0s

	if (bitBuffer.size() < dataCapacityBits)
	{
		ocean_assert((version == 1u || version == 3u) && (dataCapacityBits - bitBuffer.size() <= 4u));
		Segment::bitBufferAppend(0u, dataCapacityBits - bitBuffer.size(), bitBuffer);
	}

	ocean_assert(bitBuffer.size() == (size_t)MicroQRCodeEncoder::totalNumberDataCodewordBits(version, finalErrorCorrectionCapacity));

	// Pack bits into (big-endian) bytes

	Codewords codewords((bitBuffer.size() + 4) / 8);

	for (size_t i = 0; i < bitBuffer.size(); ++i)
	{
		codewords[i >> 3] |= (bitBuffer[i] ? 1 : 0) << (7 - (i & 7));
	}

	if (version == 1u || version == 3u)
	{
		// For versions M1 and M3, the last codeword is only 4 bits, so the last 4 bits of the last byte should be 0000
 		ocean_assert((codewords[codewords.size() - 1u] & 0b1111u) == 0u);
	}

	addErrorCorrectionAndCreateQRCode(version, finalErrorCorrectionCapacity, codewords, mask, modules);

	return SC_SUCCESS;
}

MicroQRCodeEncoder::Codewords MicroQRCodeEncoder::addErrorCorrection(const Codewords& codewords, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	ocean_assert(codewords.size() * 8 - ((version&1) ? 4 : 0) == totalNumberDataCodewordBits(version, errorCorrectionCapacity));
	const unsigned int blockEccLen = totalNumberErrorCorrectionCodewords(version, errorCorrectionCapacity);
	const unsigned int rawCodewords = (totalNumberRawDataModules(version) + 4u) / 8u;

	// Append ECC to result

	const ReedSolomon::Coefficients coefficients = ReedSolomon::generateCoefficients(blockEccLen);

	Codewords result(codewords.cbegin(), codewords.cbegin() + (rawCodewords - blockEccLen));

	const Codewords ecc = ReedSolomon::computeRemainders(result, coefficients);

	result.insert(result.end(), ecc.cbegin(), ecc.cend());

	ocean_assert(result.size() == rawCodewords);

	return result;
}

void MicroQRCodeEncoder::applyMaskPattern(std::vector<uint8_t>& modules, const unsigned int version, const std::vector<uint8_t>& functionPatternMask, const MaskingPattern mask)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);
	ocean_assert(mask < 4u);

	const unsigned int size = MicroQRCode::modulesPerSide(version);
	ocean_assert(size * size == modules.size());
	ocean_assert(modules.size() == functionPatternMask.size());

	bool (*maskFunction)(const unsigned int x, const unsigned int y) = nullptr;
	switch (mask)
	{
		case 0:
			maskFunction = [](const unsigned int /*x*/, const unsigned y) { return y % 2u == 0u; };
			break;

		case 1:
			maskFunction = [](const unsigned int x, const unsigned y) { return (x / 3u + y / 2u) % 2u == 0u; };
			break;

		case 2:
			maskFunction = [](const unsigned int x, const unsigned y) { return (x * y % 2u + x * y % 3u) % 2u == 0u; };
			break;

		case 3:
			maskFunction = [](const unsigned int x, const unsigned y) { return ((x + y) % 2u + x * y % 3u) % 2u == 0u; };
			break;

		default:
			ocean_assert(false && "Never be here!");
			break;
	}
	ocean_assert(maskFunction != nullptr);

	for (unsigned int y = 0u; y < size; ++y)
	{
		const unsigned int row = y * size;

		for (unsigned int x = 0u; x < size; ++x)
		{
			const bool invert = (*maskFunction)(x, y);
			ocean_assert(modules[row + x] == 0u || modules[row + x] == 1u);
			modules[row + x] = (invert && (functionPatternMask[row + x] == 0u)) ^ modules[row + x];
		}
	}
}

unsigned int MicroQRCodeEncoder::computeMaskPatternScore(const std::vector<uint8_t>& modules, const unsigned int version)
{
	const unsigned int size = MicroQRCode::modulesPerSide(version);
	ocean_assert(size >= 11u && size <= 17u && size % 2u == 1u);
	ocean_assert(size * size == modules.size());

	// cf. ISO/IEC 18004:2015, Section 7.8.3.2

	unsigned int sum1 = 0u;
	for (int i = 1; i < int(size); i++)
	{
		if (modules[size * i + size - 1] != 0)
		{
			sum1++;
		}
	}

	unsigned int sum2 = 0u;
	for (int j = 1; j < int(size); j++)
	{
		if (modules[size * (size - 1) + j] != 0)
		{
			sum2++;
		}
	}

	unsigned int minSum = std::min(sum1, sum2);
	unsigned int maxSum = minSum ^ sum1 ^ sum2;

	return (minSum << 4) | maxSum;
}

std::vector<uint8_t> MicroQRCodeEncoder::setFunctionPatterns(std::vector<uint8_t>& modules, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	ocean_assert(version >= 1u && version <= MicroQRCode::MAX_VERSION);

	const unsigned int size = MicroQRCode::modulesPerSide(version);
	ocean_assert(modules.size() == size * size);

#if defined(OCEAN_DEBUG)
	// All modules should have been initialized to 0 already
	for (unsigned int i = 0u; i < modules.size(); ++i)
	{
		ocean_assert(modules[i] == 0u);
	}
#endif

	std::vector<uint8_t> functionPatternMask(size * size, 0u);

	// Horizontal and vertical timing patterns

	for (unsigned int i = 8u; i < size; ++i)
	{
		const uint8_t moduleValue = i % 2u == 0u ? 1u : 0u;

		const unsigned int indexHorizontal = i;
		modules[indexHorizontal] = moduleValue;
		functionPatternMask[indexHorizontal] = 255u;

		const unsigned int indexVertical = i * size;
		modules[indexVertical] = moduleValue;
		functionPatternMask[indexVertical] = 255u;
	}

	// Finder pattern in the top-left corner

	constexpr uint8_t finderPattern[7 * 7] =
	{
		// clang-format off
		1u, 1u, 1u, 1u, 1u, 1u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 0u, 1u, 1u, 1u, 0u, 1u,
		1u, 0u, 1u, 1u, 1u, 0u, 1u,
		1u, 0u, 1u, 1u, 1u, 0u, 1u,
		1u, 0u, 0u, 0u, 0u, 0u, 1u,
		1u, 1u, 1u, 1u, 1u, 1u, 1u
		// clang-format on
	};

	for (unsigned int y = 0u; y < 7u; ++y)
	{
		for (unsigned int x = 0u; x < 7u; ++x)
		{
			modules[y * size + x] = finderPattern[y * 7u + x];
			functionPatternMask[y * size + x] = 255u;
		}
	}

	// Separator space around the finder pattern (No need to draw the actual modules; they are white by default)

	for (unsigned int y = 0u; y <= 7u; ++y)
	{
		functionPatternMask[y * size + 7u] = 255u;
	}

	for (unsigned int x = 0u; x <= 7u; ++x)
	{
		functionPatternMask[7u * size + x] = 255u;
	}

	// Draw the configuration data: format and version
	setFormatInformation(modules, version, errorCorrectionCapacity, /* dummy mask value, will be updated later */ MP_PATTERN_0, functionPatternMask);

	return functionPatternMask;
}

void MicroQRCodeEncoder::setCodewords(std::vector<uint8_t>& modules, const Codewords& codewords, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const std::vector<uint8_t>& functionPatternMask)
{
	ocean_assert(version >= 1u && version <= MicroQRCode::MAX_VERSION);

	ocean_assert(codewords.size() == (totalNumberRawDataModules(version) + 4u) / 8u);

	// All codewords are 8 bits in length, except in versions M1 and M3 where the final data codeword is 4 bits in length
	// Because the codewords are packed into bytes, the final data codeword of M1 and M3 is padded with 4 zero bits which must be skipped when writing the modules
	unsigned int startOfHalfByteToSkip = (unsigned int)(-1);
	if ((version == 1u || version == 3u))
	{
		startOfHalfByteToSkip = totalNumberDataCodewordBits(version, errorCorrectionCapacity);
		ocean_assert(startOfHalfByteToSkip % 8u == 4u);
		ocean_assert((codewords[startOfHalfByteToSkip / 8] & 0b1111u) == 0u);
	}
	else
	{
		ocean_assert(totalNumberDataCodewordBits(version, errorCorrectionCapacity) % 8u == 0u);
	}

	const unsigned int size = MicroQRCode::modulesPerSide(version);

	size_t i = 0; // Bit index into the data

	// Do the funny zigzag scan
	for (unsigned int right = size - 1u; right > 0; right -= 2u) // Index of right column in each column pair
	{
		for (unsigned int vert = 0u; vert < size; vert++) // Vertical counter
		{
			for (unsigned int j = 0u; j < 2u; j++)
			{
				unsigned int x = right - j; // Actual x coordinate
				bool upward = ((size - right) & 2u) == 0u;
				int y = upward ? size - 1u - vert : vert; // Actual y coordinate

				unsigned int index = y * size + x;
				ocean_assert(index < size * size);

				if (functionPatternMask[index] == 0u && i < codewords.size() * 8u)
				{
					if (i == startOfHalfByteToSkip)
					{
						i += 4u;
					}

					const Codeword& codeword = codewords[i >> 3];
					const unsigned int bit = 7u - (i & 7u);
					modules[index] = (codeword >> bit) & 1u;

					i++;
				}
			}
		}
	}

	ocean_assert(i == codewords.size() * 8);
}

void MicroQRCodeEncoder::setFormatInformation(std::vector<uint8_t>& modules, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern mask, std::vector<uint8_t>& functionPatternMask)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);
	ocean_assert(mask < 8u);

	const unsigned int size = MicroQRCode::modulesPerSide(version);
	const uint32_t formatInformationBits = encodeFormat(version, errorCorrectionCapacity, mask);

	// Draw format information

	for (unsigned int i = 0u, x = 8u, y = 1u; i < 15u; ++i)
	{
		modules[y * size + x] = ((formatInformationBits >> i) & 1) != 0u ? 1u : 0u;
		functionPatternMask[y * size + x] = 1u;
		if (i < 7u)
		{
			++y;
		}
		else
		{
			--x;
		}
	}
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
