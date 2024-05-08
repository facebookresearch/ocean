/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"
#include <cstdint>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

const int8_t QRCodeEncoder::ECC_CODEWORDS_PER_BLOCK[4][41] =
{
	// clang-format off
	// Note: these values are ordered according to the bit sequence values of each error correction level
	// Version:
	//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
	{-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},  // Medium, 0b00
	{-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Low, 0b01
	{-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // High, 0b10
	{-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Quartile, 0b11
	// clang-format on
};

const int8_t QRCodeEncoder::NUM_ERROR_CORRECTION_BLOCKS[4][41] =
{
	// clang-format off
	// Note 1: index 0 is for padding, and is set to an illegal value
	// Note 2: these values are ordered according to the bit sequence values of each error correction level
	// Versions:
	//0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
	{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium, 0b00
	{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low, 0b01
	{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High, 0b10
	{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile, 0b11
	// clang-format on
};

bool QRCodeEncoder::Segment::generateSegmentNumeric(const std::string& data, Segments& segments)
{
	if (isNumericData(data) == false)
	{
		return false;
	}

	// Cf. ISO/IEC 18004:2015, Section 7.4.3

	BitBuffer bitBuffer;
	bitBuffer.reserve((data.size() / 3) * 10 + (data.size() % 3 != 0 ? (data.size() % 3) * 3 + 1 : 0));

	for (size_t i = 0; i < data.size(); i += 3)
	{
		const size_t length = (i + 3 <= data.size() ? 3 : data.size() - i);
		ocean_assert(length != 0 && length <= 3);

		int value;
		if (String::isInteger32(data.substr(i, length), &value) == false)
		{
			return false;
		}
		ocean_assert(value >= 0);

		bitBufferAppend((unsigned int)value, length * 3 + 1, bitBuffer);
	}

	segments.emplace_back(QRCode::EM_NUMERIC, (unsigned int)data.size(), bitBuffer);

	return true;
}

bool QRCodeEncoder::Segment::generateSegmentAlphanumeric(const std::string& data, Segments& segments)
{
	if (!isAlphanumericData(data))
	{
		return false;
	}

	// Cf. ISO/IEC 18004:2015, Section 7.4.4

	BitBuffer bitBuffer;
	bitBuffer.reserve(data.size() % 2 == 0 ? 11 * data.size() : 11 * (data.size() - 1) + 6);

	const std::string& alphanumericCharset = getAlphanumericCharset();

	unsigned int buffer = 0u;
	unsigned int bufferSize = 0u;
	for (size_t i = 0; i < data.size(); ++i)
	{
		const size_t charsetIndex = alphanumericCharset.find(data[i]);
		ocean_assert(charsetIndex < alphanumericCharset.size());

		const unsigned int encodedCharacter = (unsigned int)charsetIndex;

		buffer = buffer * 45u + encodedCharacter;
		bufferSize += 1u;

		if (bufferSize == 2u)
		{
			bitBufferAppend(buffer, 11, bitBuffer);

			buffer = 0u;
			bufferSize = 0u;
		}
	}

	ocean_assert(bufferSize <= 1u);

	if (bufferSize != 0u)
	{
		bitBufferAppend(buffer, 6, bitBuffer);
	}

	segments.emplace_back(QRCode::EM_ALPHANUMERIC, (unsigned int)data.size(), bitBuffer);

	return true;
}

bool QRCodeEncoder::Segment::generateSegmentsBytes(const std::vector<uint8_t>& data, Segments& segments)
{
	ocean_assert(data.empty() == false);

	// Cf. ISO/IEC 18004:2015, Section 7.4.5

	BitBuffer bitBuffer;
	for (size_t i = 0; i < data.size(); ++i)
	{
		bitBufferAppend((unsigned int)data[i], 8, bitBuffer);
	}
	segments.emplace_back(QRCode::EM_BYTE, (unsigned int)data.size(), bitBuffer);

	return true;
}

QRCodeEncoder::ReedSolomon::Coefficients QRCodeEncoder::ReedSolomon::generateCoefficients(const unsigned int degree)
{
	// Cf. ISO/IEC 18004:2015, Annex B
	ocean_assert(degree != 0u && degree < 256u);

	Coefficients coefficients(degree);

	// Monomial x^0
	coefficients[degree - 1u] = 1u;

	// Compute the product polynomial: (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}).
	// The highest coefficient is dropped. The remaining coefficients are stored in descending order.
	// Note: r = 0x02 is a generator element of GF(2^8/0x11D).

	uint8_t root = 1u;
	for (unsigned int i = 0u; i < coefficients.size(); ++i)
	{
		for (unsigned int j = 0; j < coefficients.size(); ++j)
		{
			coefficients[j] = multiply(coefficients[j], root);

			if (j + 1 < coefficients.size())
			{
				coefficients[j] ^= coefficients[j + 1u];
			}
		}

		root = multiply(root, 0x02u);
	}

	return coefficients;
}

QRCodeEncoder::Codewords QRCodeEncoder::ReedSolomon::computeRemainders(const Codewords& codewords, const Coefficients& coefficients)
{
	std::deque<Codeword> remaindersDeque(coefficients.size(), 0u);

	for (const Codeword codeword : codewords)
	{
		const uint8_t factor = codeword ^ remaindersDeque[0];
		remaindersDeque.pop_front();
		remaindersDeque.push_back(0u);

		for (size_t i = 0; i < remaindersDeque.size(); ++i)
		{
			remaindersDeque[i] ^= multiply(coefficients[i], factor);
		}
	}

	Codewords remainders(remaindersDeque.begin(), remaindersDeque.end());

	return remainders;
}

uint8_t QRCodeEncoder::ReedSolomon::multiply(const uint8_t x, const uint8_t y)
{
	// Russian peasant multiplication
	unsigned int z = 0u;
	for (unsigned int i = 7u; i < 8u; --i)
	{
		z = (z << 1) ^ ((z >> 7) * 0x11D);
		z ^= ((y >> i) & 1) * x;
	}

	ocean_assert(z >> 8u == 0u);

	return uint8_t(z);
}

bool QRCodeEncoder::encodeText(const std::string& text, QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, QRCode& qrcode)
{
	Segments segments;
	QRCode::EncodingMode encodingMode = QRCode::EM_INVALID_ENCODING_MODE;

	std::vector<uint8_t> data(text.begin(), text.end());

	if (Segment::isNumericData(text))
	{
		if (!Segment::generateSegmentNumeric(text, segments))
		{
			return false;
		}

		encodingMode = QRCode::EM_NUMERIC;
	}
	else if (Segment::isAlphanumericData(text))
	{
		if (!Segment::generateSegmentAlphanumeric(text, segments))
		{
			return false;
		}

		encodingMode = QRCode::EM_ALPHANUMERIC;
	}
	else
	{
		if (!Segment::generateSegmentsBytes(data, segments))
		{
			return false;
		}

		encodingMode = QRCode::EM_BYTE;
	}

	if (encodingMode == QRCode::EM_INVALID_ENCODING_MODE)
	{
		return false;
	}

	std::vector<uint8_t> modules;
	unsigned int version;
	QRCode::ErrorCorrectionCapacity finalErrorCorrectionCapacity;

	if (QRCodeEncoder::encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity))
	{
		qrcode = QRCode(std::move(data), encodingMode, finalErrorCorrectionCapacity, std::move(modules), version);
		ocean_assert(qrcode.isValid());

		return qrcode.isValid();
	}

	return false;
}

bool QRCodeEncoder::encodeBinary(const std::vector<uint8_t>& data, QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, QRCode& qrcode)
{
	Segments segments;

	if (Segment::generateSegmentsBytes(data, segments))
	{
		std::vector<uint8_t> modules;
		unsigned int version;
		QRCode::ErrorCorrectionCapacity finalErrorCorrectionCapacity;

		if (QRCodeEncoder::encodeSegments(segments, errorCorrectionCapacity, modules, version, finalErrorCorrectionCapacity))
		{
			std::vector<uint8_t> finalData(data.begin(), data.end());

			qrcode = QRCode(std::move(finalData), QRCode::EM_BYTE, finalErrorCorrectionCapacity, std::move(modules), version);
			ocean_assert(qrcode.isValid());

			return qrcode.isValid();
		}
	}

	return false;
}

bool QRCodeEncoder::addErrorCorrectionAndCreateQRCode(const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const Codewords& rawCodewords, MaskingPattern mask, std::vector<uint8_t>& modules)
{
	// TODO Refactor this 1. pull out the error correction, 2. pull out the computation of the optimal mask, 3. only leave the initialization of the modules

	ocean_assert(version != 0 && version <= 40u);
	ocean_assert(mask < 8u || mask == (unsigned int)(-1));

	std::vector<uint8_t> localModules(QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version), 0u);

	std::vector<uint8_t> functionPatternMask = QRCodeEncoder::setFunctionPatterns(localModules, version, errorCorrectionCapacity);

	const Codewords eccCodewords = QRCodeEncoder::addErrorCorrectionAndInterleave(rawCodewords, version, errorCorrectionCapacity);
	QRCodeEncoder::setCodewords(localModules, eccCodewords, version, functionPatternMask);

	// Determine mask index, if applicable
	if (mask == (unsigned int)(-1))
	{
		unsigned int minimumPenalty = NumericT<unsigned int>::maxValue();

		constexpr MaskingPattern maskingPatterns[8] =
		{
			MP_PATTERN_0,
			MP_PATTERN_1,
			MP_PATTERN_2,
			MP_PATTERN_3,
			MP_PATTERN_4,
			MP_PATTERN_5,
			MP_PATTERN_6,
			MP_PATTERN_7
		};

		for (size_t m = 0; m < 8; ++m)
		{
			QRCodeEncoder::applyMaskPattern(localModules, version, functionPatternMask, maskingPatterns[m]);
			QRCodeEncoder::setFormatInformation(localModules, version, errorCorrectionCapacity, maskingPatterns[m], functionPatternMask);

			const unsigned int penalty = QRCodeEncoder::computeMaskPatternPenalty(localModules, version);

			if (penalty < minimumPenalty)
			{
				mask = maskingPatterns[m];
				minimumPenalty = penalty;
			}

			// Undo applying the mask (possible because of XOR)
			QRCodeEncoder::applyMaskPattern(localModules, version, functionPatternMask, maskingPatterns[m]);
		}
	}

	ocean_assert(mask < 8u);

	QRCodeEncoder::applyMaskPattern(localModules, version, functionPatternMask, mask);
	QRCodeEncoder::setFormatInformation(localModules, version, errorCorrectionCapacity, mask, functionPatternMask);

	modules = std::move(localModules);

	return true;
}

bool QRCodeEncoder::encodeSegments(const Segments& segments, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>& modules, unsigned int& version, QRCode::ErrorCorrectionCapacity& finalErrorCorrectionCapacity, const unsigned int minVersion, const unsigned int maxVersion, const MaskingPattern mask, const bool maximizeErrorCorrectionCapacity)
{
	ocean_assert(segments.empty() == false);
	ocean_assert(minVersion >= 1u && minVersion <= maxVersion && maxVersion <= 40u);
	ocean_assert(mask == (unsigned int)(-1) || (mask >= 1u && mask <= 7u));

	// Determine the lowest version that can hold the data (in range [minVersion, maxVersion], if it exists)

	version = 0u;
	unsigned int bitsUsed = 0u;

	for (unsigned int v = minVersion; v <= maxVersion; ++v)
	{
		const unsigned int maxDataCapacityBits = QRCodeEncoder::totalNumberDataCodewords(v, errorCorrectionCapacity) * 8u;

		if (QRCodeEncoder::computeTotalBitsUsed(segments, v, bitsUsed) && bitsUsed != 0u && bitsUsed <= maxDataCapacityBits)
		{
			version = v;
			break;
		}
	}

	if (version == 0u || version > 40u || bitsUsed == 0u)
	{
		return false;
	}

	// Determine the highest error correction level that still fits into the selected version

	finalErrorCorrectionCapacity = errorCorrectionCapacity;

	if (maximizeErrorCorrectionCapacity)
	{
		// The values of error correction capacities are their corresponding bit sequence. Here, the "order of appearance" is needed,
		// so the array below maps the values back to the order in which the enums are defined, i.e., 0. low, 1. medium, 2. quartile, 3. high
		constexpr unsigned int matchCapacityBackToIndex[4] = { 1u, 0u, 3u, 2u, };
		constexpr QRCode::ErrorCorrectionCapacity capacities[3] = { QRCode::ECC_15, QRCode::ECC_25, QRCode::ECC_30 };

		for (size_t i = 0; i < 3; ++i)
		{
			if (matchCapacityBackToIndex[errorCorrectionCapacity] >= matchCapacityBackToIndex[capacities[i]])
			{
				continue;
			}

			if (bitsUsed > QRCodeEncoder::totalNumberDataCodewords(version, capacities[i]) * 8u)
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
		// Mode indicator, 4 bits
		// |    Character count indicator, N bits
		// |    |              Binary data, M bits
		// |    |              |
		// 0123 01234567890... 0123...
		Segment::bitBufferAppend(QRCodeEncoder::Segment::encodationModeIndicatorBitSequence(segment.encodationMode()), 4, bitBuffer);
		Segment::bitBufferAppend(segment.characters(), Segment::getBitsInCharacterCountIndicator(version, segment.encodationMode()), bitBuffer);
		bitBuffer.insert(bitBuffer.end(), segment.bitBuffer().begin(), segment.bitBuffer().end());
	}
	ocean_assert(bitBuffer.size() == bitsUsed);

	const unsigned int dataCapacityBits = QRCodeEncoder::totalNumberDataCodewords(version, finalErrorCorrectionCapacity) * 8u;

	// Add terminator sequence (cf. ISO/IEC 18004:2015, Section 7.4.9)

	ocean_assert(bitBuffer.size() <= dataCapacityBits);
	Segment::bitBufferAppend(0u, std::min(size_t(4), size_t(dataCapacityBits) - bitBuffer.size()), bitBuffer);

	// Add padding so that the size of the bit buffer is a multiple of 8

	const unsigned int paddingBits = (unsigned int)(8 - (bitBuffer.size() % 8));
	if (paddingBits != 0u && paddingBits < 8u)
	{
		Segment::bitBufferAppend(0u, paddingBits, bitBuffer);
	}
	ocean_assert(bitBuffer.size() % 8 == 0);

	// If the capacity has not been reached, will the remaining bits with alternating bytes

	ocean_assert(dataCapacityBits % 8 == 0);

	for (uint8_t paddingByte = 0xEC; bitBuffer.size() < dataCapacityBits; paddingByte ^= 0xEC ^ 0x11)
	{
		Segment::bitBufferAppend(paddingByte, 8, bitBuffer);
	}

	ocean_assert(bitBuffer.size() % 8 == 0 && bitBuffer.size() / 8 == (size_t)QRCodeEncoder::totalNumberDataCodewords(version, finalErrorCorrectionCapacity));

	// Pack bits into (big-endian) bytes

	Codewords codewords(bitBuffer.size() / 8);

	for (size_t i = 0; i < bitBuffer.size(); ++i)
	{
		codewords[i >> 3] |= (bitBuffer[i] ? 1 : 0) << (7 - (i & 7));
	}

	return addErrorCorrectionAndCreateQRCode(version, finalErrorCorrectionCapacity, codewords, mask, modules);
}

QRCodeEncoder::Codewords QRCodeEncoder::addErrorCorrectionAndInterleave(const Codewords& codewords, const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	ocean_assert(version != 0u && version <= 40u);
	ocean_assert(codewords.size() == totalNumberDataCodewords(version, errorCorrectionCapacity));
	ocean_assert((unsigned int)errorCorrectionCapacity < 4u);

	const unsigned int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[(unsigned int)(errorCorrectionCapacity)][version];
	const unsigned int blockEccLen = ECC_CODEWORDS_PER_BLOCK[(unsigned int)(errorCorrectionCapacity)][version];
	const unsigned int rawCodewords = totalNumberRawDataModules(version) / 8u;
	const unsigned int numShortBlocks = numBlocks - rawCodewords % numBlocks;
	const unsigned int shortBlockLen = rawCodewords / numBlocks;

	// Split data into blocks and append ECC to each block

	std::vector<Codewords> blocks;
	blocks.reserve(numBlocks);

	const ReedSolomon::Coefficients coefficients = ReedSolomon::generateCoefficients(blockEccLen);

	for (unsigned int i = 0u, k = 0u; i < numBlocks; ++i)
	{
		blocks.emplace_back(codewords.cbegin() + k, codewords.cbegin() + (k + shortBlockLen - blockEccLen + (i < numShortBlocks ? 0u : 1u)));
		k += (unsigned int)blocks.back().size();

		const Codewords ecc = ReedSolomon::computeRemainders(blocks[i], coefficients);

		if (i < numShortBlocks)
		{
			blocks.back().push_back(0);
		}

		blocks.back().insert(blocks.back().end(), ecc.cbegin(), ecc.cend());
	}

	// Interleave (not concatenate) the bytes from every block into a single sequence

	Codewords result;
	for (size_t i = 0; i < blocks.at(0).size(); i++)
	{
		for (size_t j = 0; j < blocks.size(); j++)
		{
			// Skip the padding byte in short blocks
			if (i != static_cast<unsigned int>(shortBlockLen - blockEccLen) || j >= static_cast<unsigned int>(numShortBlocks))
			{
				result.push_back(blocks.at(j).at(i));
			}
		}
	}
	ocean_assert(result.size() == rawCodewords);

	return result;
}

void QRCodeEncoder::applyMaskPattern(std::vector<uint8_t>& modules, const unsigned int version, const std::vector<uint8_t>& functionPatternMask, const MaskingPattern mask)
{
	ocean_assert(version != 0u && version <= 40u);
	ocean_assert(mask < 8u);

	const unsigned int size = QRCode::modulesPerSide(version);
	ocean_assert(size * size == modules.size());
	ocean_assert(modules.size() == functionPatternMask.size());

	bool (*maskFunction)(const unsigned int x, const unsigned int y) = nullptr;
	switch (mask)
	{
		case 0:
			maskFunction = [](const unsigned int x, const unsigned y) { return (x + y) % 2u == 0u; };
			break;

		case 1:
			maskFunction = [](const unsigned int /*x*/, const unsigned y) { return y % 2u == 0u; };
			break;

		case 2:
			maskFunction = [](const unsigned int x, const unsigned /*y*/) { return x % 3u == 0u; };
			break;

		case 3:
			maskFunction = [](const unsigned int x, const unsigned y) { return (x + y) % 3u == 0u; };
			break;

		case 4:
			maskFunction = [](const unsigned int x, const unsigned y) { return (x / 3u + y / 2u) % 2u == 0u; };
			break;

		case 5:
			maskFunction = [](const unsigned int x, const unsigned y) { return x * y % 2u + x * y % 3u == 0u; };
			break;

		case 6:
			maskFunction = [](const unsigned int x, const unsigned y) { return (x * y % 2u + x * y % 3u) % 2u == 0u; };
			break;

		case 7:
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

unsigned int QRCodeEncoder::computeMaskPatternPenalty(const std::vector<uint8_t>& modules, const unsigned int version)
{
	const unsigned int size = QRCode::modulesPerSide(version);
	ocean_assert(size >= 21u && size <= 177u && size % 2u == 1u);
	ocean_assert(size * size == modules.size());

	// Penalties, cf. ISO/IEC 18004:2015, Section 7.8.3.1
	const unsigned int penalties[4] = { 3u, 3u, 40u, 10u };
	unsigned int penaltyTotal = 0ul;

	// Run checks on the modules, cf ISO/IEC 18004:2015, Table 11

	// Horizontal versions of:
	//  * Check N1: adjacent modules in row/column in same color
	//  * Check N3: 1:1:3:1:1 ratio pattern (finder pattern) in row/column, preceded or followed by light area 4 modules wide

	for (unsigned int y = 0u; y < size; ++y)
	{
		std::deque<unsigned int> history(7, 0u);
		uint8_t moduleValue = 0u;
		unsigned int count = 0u;

		for (unsigned int x = 0u; x < size; ++x)
		{
			const unsigned int index = y * size + x;

			if (modules[index] == moduleValue)
			{
				// Check N1
				count++;

				if (count == 5u)
				{
					penaltyTotal += penalties[0];
				}
				else if (count > 5u)
				{
					penaltyTotal++;
				}
			}
			else
			{
				// Check N3
				history.pop_back();
				history.push_front(count);

				// History, index:     0  1  2  3      4  5  6
				//         values: ...ww bb ww bbbbbb ww bb ww...
				if (moduleValue == false && history[1] != 0u && history[1] == history[2] && history[1] * 3u == history[3] && history[1] == history[4] && history[1] == history[5] && std::max(history[0], history[6]) >= history[1] * 4u)
				{
					penaltyTotal += penalties[2];
				}

				moduleValue = modules[index];
				count = 1u;
			}
		}

		// Check N3 for the remaining elements of the current row

		history.pop_back();
		history.push_front(count);

		if (moduleValue != 0u)
		{
			history.pop_back();
			history.push_front(0u); // White placeholder (dummy)
		}

		if (history[1] != 0u && history[1] == history[2] && history[1] * 3u == history[3] && history[1] == history[4] && history[1] == history[5] && std::max(history[0], history[6]) >= history[1] * 4u)
		{
			penaltyTotal += penalties[2];
		}
	}

	// Vertical versions of:
	//  * Check N1
	//  * Check N3

	for (unsigned int x = 0u; x < size; ++x)
	{
		std::deque<unsigned int> history(7, 0u);
		uint8_t moduleValue = 0u;
		unsigned int count = 0u;

		for (unsigned int y = 0u; y < size; ++y)
		{
			const unsigned int index = y * size + x;

			if (modules[index] == moduleValue)
			{
				// Check N1
				count++;

				if (count == 5u)
				{
					penaltyTotal += penalties[0];
				}
				else if (count > 5u)
				{
					penaltyTotal++;
				}
			}
			else
			{
				// Check N3
				history.pop_back();
				history.push_front(count);

				// History, index:     0  1  2  3      4  5  6
				//         values: ...ww bb ww bbbbbb ww bb ww...
				if (moduleValue == false && history[1] != 0u && history[1] == history[2] && history[1] * 3u == history[3] && history[1] == history[4] && history[1] == history[5] && std::max(history[0], history[6]) >= history[1] * 4u)
				{
					penaltyTotal += penalties[2];
				}

				moduleValue = modules[index];
				count = 1u;
			}
		}

		// Check N3 for the remaining elements of the current row

		history.pop_back();
		history.push_front(count);

		if (moduleValue != 0u)
		{
			history.pop_back();
			history.push_front(0u); // White placeholder (dummy)
		}

		if (history[1] != 0u && history[1] == history[2] && history[1] * 3u == history[3] && history[1] == history[4] && history[1] == history[5] && std::max(history[0], history[6]) >= history[1] * 4u)
		{
			penaltyTotal += penalties[2];
		}
	}

	// Check N2: block of modules (2x2) in the same color

	for (unsigned int y = 0u; y < size - 1u; ++y)
	{
		for (unsigned int x = 0u; x < size - 1u; ++x)
		{
			const unsigned int index = y * size + x;
			const uint8_t moduleValue = modules[index];
			if (moduleValue == modules[y * size + x + 1u] && moduleValue == modules[(y + 1u) * size + x] && moduleValue == modules[(y + 1u) * size + x + 1u])
			{
				penaltyTotal += penalties[1];
			}
		}
	}

	// Check N4: proportion of black modules in the entire symbol

	const unsigned int totalCount = size * size;
	unsigned int blackCount = 0u;

	for (const bool module : modules)
	{
		blackCount += (unsigned int)(module == true);
	}

	// Compute the smallest integer k >= 0 such that (45-5k)% <= black/total <= (55+5k)%

	const unsigned int k = (unsigned int)std::max(0, ((std::abs(int(blackCount) * 20 - int(totalCount) * 10) + int(totalCount) - 1) / int(totalCount)) - 1);
	penaltyTotal += k * penalties[3];

	return penaltyTotal;
}

std::vector<uint8_t> QRCodeEncoder::setFunctionPatterns(std::vector<uint8_t>& modules, const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	ocean_assert(version >= 1u && version <= 40u);

	const unsigned int size = QRCode::modulesPerSide(version);
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

	const unsigned int timingPatternStart = 8u;
	const unsigned int timingPatternEnd = size - 8u;

	for (unsigned int i = timingPatternStart; i < timingPatternEnd; ++i)
	{
		const uint8_t moduleValue = i % 2u == 0u ? 1u : 0u;

		const unsigned int indexHorizontal = 6u * size + i;
		modules[indexHorizontal] = moduleValue;
		functionPatternMask[indexHorizontal] = 255u;

		const unsigned int indexVertical = i * size + 6u;
		modules[indexVertical] = moduleValue;
		functionPatternMask[indexVertical] = 255u;
	}

	// Finder patterns in the top-left, bottom-left, and top-right corner

	const unsigned int finderPatternStarts[3] = { 0u, (size - 7u) * size, size - 7u };
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

	for (unsigned i = 0u; i < 3u; ++i)
	{
		for (unsigned int y = 0u; y < 7u; ++y)
		{
			for (unsigned int x = 0u; x < 7u; ++x)
			{
				modules[finderPatternStarts[i] + y * size + x] = finderPattern[y * 7u + x];
				functionPatternMask[finderPatternStarts[i] + y * size + x] = 255u;
			}
		}
	}

	// Separator space around the finder patterns (No need to draw the actual modules; they are white by default)

	for (unsigned int y = 0u; y <= 7u; ++y)
	{
		functionPatternMask[y * size + 7u] = 255u;
		functionPatternMask[y * size + size - 8u] = 255u;
		functionPatternMask[(size - 8u + y) * size + 7u] = 255u;
	}

	for (unsigned int x = 0u; x <= 7u; ++x)
	{
		functionPatternMask[7u * size + x] = 255u;
		functionPatternMask[7u * size + size - 8u + x] = 255u;
		functionPatternMask[(size - 8u) * size + x] = 255u;
	}

	// Draw the alignment patterns, if applicable

	const uint8_t alignmentPattern[5 * 5] =
	{
		// clang-format off
		1u, 1u, 1u, 1u, 1u,
		1u, 0u, 0u, 0u, 1u,
		1u, 0u, 1u, 0u, 1u,
		1u, 0u, 0u, 0u, 1u,
		1u, 1u, 1u, 1u, 1u,
		// clang-format on
	};

	const VectorsI2 alignmentPatternPositions = computeAlignmentPatternPositions(version);

	for (const VectorI2& alignmentPatternPosition : alignmentPatternPositions)
	{
		const unsigned int xStart = alignmentPatternPosition.x() - 2u;
		const unsigned int yStart = alignmentPatternPosition.y() - 2u;

		for (unsigned int y = 0u; y < 5u; ++y)
		{
			for (unsigned int x = 0u; x < 5u; ++x)
			{
				modules[(yStart + y) * size + (xStart + x)] = alignmentPattern[y * 5u + x];
				functionPatternMask[(yStart + y) * size + (xStart + x)] = 255u;
			}
		}
	}

	// Draw the configuration data: format and version
	setFormatInformation(modules, version, errorCorrectionCapacity, /* dummy mask value, will be updated later */ MP_PATTERN_0, functionPatternMask);
	setVersionInformation(modules, version, functionPatternMask);

	return functionPatternMask;
}

void QRCodeEncoder::setCodewords(std::vector<uint8_t>& modules, const Codewords& codewords, const unsigned int version, const std::vector<uint8_t>& functionPatternMask)
{
	ocean_assert(codewords.size() == totalNumberRawDataModules(version) / 8);
	ocean_assert(version >= 1u && version <= 40u);

	const unsigned int size = QRCode::modulesPerSide(version);

	size_t i = 0; // Bit index into the data

	// Do the funny zigzag scan
	for (unsigned int right = size - 1u; right < size; right -= 2u) // Index of right column in each column pair
	{
		if (right == 6u)
		{
			right = 5u;
		}

		for (unsigned int vert = 0u; vert < size; vert++) // Vertical counter
		{
			for (unsigned int j = 0u; j < 2u; j++)
			{
				unsigned int x = right - j; // Actual x coordinate
				bool upward = ((right + 1u) & 2u) == 0u;
				int y = upward ? size - 1u - vert : vert; // Actual y coordinate

				unsigned int index = y * size + x;
				ocean_assert(index < size * size);

				if (functionPatternMask[index] == 0u && i < codewords.size() * 8u)
				{
					const Codeword& codeword = codewords[i >> 3];
					const unsigned int bit = 7u - (i & 7u);
					modules[index] = (codeword >> bit) & 1u;

					i++;
				}

				// If this QR Code has any remainder bits (0 to 7), they were assigned as
				// 0/false/white and are left unchanged by this method
			}
		}
	}

	ocean_assert(i == codewords.size() * 8);
}

void QRCodeEncoder::setFormatInformation(std::vector<uint8_t>& modules, const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern mask, std::vector<uint8_t>& functionPatternMask)
{
	ocean_assert(version != 0 && version <= 40u);
	ocean_assert(mask < 8u);

	const unsigned int size = QRCode::modulesPerSide(version);
	const uint32_t formatInformationBits = encodeFormat(errorCorrectionCapacity, mask);

	// Draw format information (top-left corner)

	for (unsigned int y = 0u; y <= 5u; ++y)
	{
		modules[y * size + 8u] = ((formatInformationBits >> y) & 1) != 0u ? 1u : 0u;
		functionPatternMask[y * size + 8u] = 1u;
	}

	modules[7u * size + 8u] = ((formatInformationBits >> 6u) & 1) != 0u ? 1u : 0u;
	functionPatternMask[7u * size + 8u] = 255u;

	modules[8u * size + 8u] = ((formatInformationBits >> 7u) & 1) != 0u ? 1u : 0u;
	functionPatternMask[8u * size + 8u] = 255u;

	modules[8u * size + 7u] = ((formatInformationBits >> 8u) & 1) != 0u ? 1u : 0u;
	functionPatternMask[8u * size + 7u] = 255u;

	for (unsigned int x = 0u; x < 6u; ++x)
	{
		modules[8u * size + (5u - x)] = (formatInformationBits >> (9u + x) & 1) != 0u ? 1u : 0u;
		functionPatternMask[8u * size + (5u - x)] = 1u;
	}

	// Draw a duplicate of the format information:
	//   * top-right finder pattern: bits 0-7
	//   * bottom-left finder pattern: bits 8-14

	for (unsigned int i = 0u; i < 8u; ++i)
	{
		modules[8u * size + (size - 1u) - i] = ((formatInformationBits >> i) & 1) != 0u ? 1u : 0u;
		functionPatternMask[8u * size + (size - 1u) - i] = 255u;
	}

	for (unsigned int i = 8u; i < 15u; ++i)
	{
		modules[(size - 15 + i) * size + 8u] = ((formatInformationBits >> i) & 1) != 0u ? 1u : 0u;
		functionPatternMask[(size - 15 + i) * size + 8u] = 255u;
	}

	//  Set the "Always black" module
	modules[(size - 8) * size + 8u] = 1u;
	functionPatternMask[(size - 8) * size + 8u] = 255u;
}

void QRCodeEncoder::setVersionInformation(std::vector<uint8_t>& modules, const unsigned int version, std::vector<uint8_t>& functionPatternMask)
{
	ocean_assert(version != 0u && version <= 40u);

	if (version < 7u)
	{
		return;
	}

	// Encode the version number and  draw two copies of the version information (near the top-right and bottom finder patterns)

	const uint32_t versionInformationBits = encodeVersion(uint32_t(version));
	const unsigned int size = QRCode::modulesPerSide(version);
	ocean_assert(size >= 11u);

	for (unsigned int i = 0u; i < 18u; i++)
	{
		const uint8_t moduleValue = (versionInformationBits >> i) & 1u;

		const unsigned int a = size - 11u + i % 3u;
		const unsigned int b = i / 3u;

		const unsigned int indexTopRight = b * size + a;
		modules[indexTopRight] = moduleValue;
		functionPatternMask[indexTopRight] = 255u;

		const unsigned int indexBottomLeft = a * size + b;
		modules[indexBottomLeft] = moduleValue;
		functionPatternMask[indexBottomLeft] = 255u;
	}
}

VectorsI2 QRCodeEncoder::computeAlignmentPatternPositions(const unsigned int version)
{
	ocean_assert(version != 0u && version <= 40u);

	if (version == 1u)
	{
		return VectorsI2();
	}

	const unsigned int totalAxes = version / 7u + 2u;
	const unsigned int step = (version == 32u ? 26u : ((version * 4u + totalAxes * 2u + 1u) / (totalAxes * 2u - 2u) * 2u));
	const unsigned int size = QRCode::modulesPerSide(version);

	Indices32 axes(totalAxes);
	Index32 position = size - 7u;

	for (unsigned int i = (totalAxes - 1u); i != 0u; --i)
	{
		ocean_assert(position < size);
		axes[i] = position;
		position -= step;
	}
	axes[0] = 6u;

	VectorsI2 alignmentPatternPositions;
	alignmentPatternPositions.reserve((totalAxes * totalAxes) - 3u);

	for (unsigned int i = 0u; i < totalAxes; ++i)
	{
		for (unsigned int j = 0u; j < totalAxes; ++j)
		{
			// Skip any alignment pattern in the location of a finder pattern (top-left, bottom-left, and top-right corners)
			if ((i == 0u && j == 0u) || (i == totalAxes - 1u && j == 0) || (i == 0 && j == totalAxes - 1))
			{
				continue;
			}

			alignmentPatternPositions.emplace_back(axes[j], axes[i]);
		}
	}
	ocean_assert(alignmentPatternPositions.size() == (totalAxes * totalAxes) - 3u);

	ocean_assert(std::is_sorted(alignmentPatternPositions.begin(), alignmentPatternPositions.end(), [](const VectorI2& a, const VectorI2& b) { return a.y() < b.y() || (a.y() == b.y() && a.x() < b.x()); }));
	return alignmentPatternPositions;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
