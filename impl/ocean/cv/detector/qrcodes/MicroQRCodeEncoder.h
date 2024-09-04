/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCodeEncoderBase.h"

#include "ocean/cv/detector/qrcodes/MicroQRCode.h"

#include "ocean/math/Vector2.h"

#include <cctype>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * This class implements an encoder and decoder for QR codes.
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT MicroQRCodeEncoder : public QRCodeEncoderBase
{
	public:

		/**
		 * Enum for the mask patterns used to shuffle modules of a Micro QR code.
		 * The values of the enum items correspond to the standard-defined bit sequences for the masks (2 bits long)
		 */
		enum MaskingPattern : uint32_t
		{
			/// Masking condition: i mod 2 = 0
			MP_PATTERN_0 = 0b00u,

			/// Masking condition: ((i div 2) + (j div 3)) mod 2 = 0
			MP_PATTERN_1 = 0b01u,

			/// Masking condition: ((i j) mod 2 + (i j) mod 3) mod 2 = 0
			MP_PATTERN_2 = 0b10u,

			/// Masking condition: ((i j) mod 3 + (i+j) mod 2) mod 2 = 0
			MP_PATTERN_3 = 0b11u,

			/// Denotes unknown masking patterns (not part of the standard)
			MP_PATTERN_UNKNOWN = uint32_t(-1)
		};

		static constexpr uint32_t INVALID_VALUE = uint32_t(-1);

	public:

		/**
		 * Encode text and store it in a QR code, will automatically choose the most efficient encodation mode
		 * @param text The text/data to be encoded as a QR code
		 * @param errorCorrectionCapacity Specifies the level of possible error correction
		 * @param qrcode The Micro QR code that will store the encoded data
		 * @return True if the Micro QR code has been successfully generated
		 */
		static bool encodeText(const std::string& text, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, MicroQRCode& qrcode);

		/**
		 * Encode binary data and store it in a QR code, will always use the byte encodation mode
		 * @param data The data to be encoded as a QR code
		 * @param errorCorrectionCapacity Specifies the level of possible error correction
		 * @param qrcode The Micro QR code that will store the encoded data
		 * @return True if the Micro QR code has been successfully generated
		 */
		static bool encodeBinary(const std::vector<uint8_t>& data, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, MicroQRCode& qrcode);

		/**
		 * Encodes the error correction level and the index of the masking pattern as a sequence of 15 bits with error correction ((15, 5) BCH code).
		 * @param version The version of the Micro QR code that will be encoded
		 * @param errorCorrectionCapacity An error correction capacity that will be decoded
		 * @param maskingPattern The masking pattern that will be encoded
		 * @return The error correction level and masking pattern encoded as a sequence of 15 bits
		 */
		static inline uint32_t encodeFormat(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern maskingPattern);

		/**
		 * Decodes a sequence of 15 bits and extracts the encoded error correction level and index of the masking pattern
		 * Note: the format bit sequence can contain up to 3 bit error. More bit errors will cause this function to fail because the result would be ambiguous.
		 * @param formatBits A sequence of 15 bits containing the format information, range: [0, 2^15)
		 * @param version The version extracted from the bit sequence
		 * @param errorCorrectionCapacity The error correction capacity extracted from the bit sequence
		 * @param maskingPattern The masking pattern extracted from the bit sequence
		 * @return True if the sequence was decoded successfully, otherwise false
		 */
		static inline bool decodeFormatBits(const uint32_t formatBits, unsigned int& version, MicroQRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, MaskingPattern& maskingPattern);

		/**
		 * Encodes the format information as a sequence of 15 bits with error correction ((15, 5) BCH code).
		 * @param format The format bit sequence consisting of the concatenated error correction capacity (2 bits) and masking pattern (3 bits), range: [0, 32)
		 * @return The error correction level and masking pattern encoded as a sequence of 15 bits
		 * @sa encodeFormat(const ErrorCorrectionCapacity, const MaskingPattern)
		 */
		static inline uint32_t encodeFormatBits(const uint32_t format);

	protected:

		/**
		 * Helper function to initialize a Micro QR code instance
		 * @param version The version of this Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param errorCorrectionCapacity The error correction level that will be used to generate the error-corrected codewords stored in this Micro QR code
		 * @param rawCodewords The encoded codewords. The size must fit exactly into the selected version of this Micro QR code
		 * @param mask The index of the bit shuffle masked that was used to generate the modules of this Micro QR code
		 * @param modules The resulting modules of the Micro QR code
		 * @return True if the initialization was successful, otherwise false
		 */
		static bool addErrorCorrectionAndCreateQRCode(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const Codewords& rawCodewords, MaskingPattern mask, std::vector<uint8_t>& modules);

		/**
		 * Encodes segments and writes them into a Micro QR code
		 * @param segments The raw segements that will be encoded and written into the Micro QR code instance, `qrcode`
		 * @param errorCorrectionCapacity The desired level of error recovery capacity. Note: if `maximizeErrorCorrectionCapacity` is true, this function will increase this level as long as it doesn't increase the size of the smallest Micro QR code that can fit the data
		 * @param modules The resulting modules of the Micro QR code
		 * @param version The resulting version of the Micro QR code
		 * @param finalErrorCorrectionCapacity The resulting error correction capacity that the Micro QR code will finally have
		 * @param minVersion The minimum version that the final Micro QR code is supposed to have, range: [1, maxVersion]
		 * @param maxVersion The maximum version that the final Micro QR code is supposed to have, range: [minVersion, 4]. Note: if this value is chosen too small, the initialization may fail
		 * @param mask The index of the bit shuffle mask that is to be used, range: [0, 3] or (unsigned int)(-1). The latter value will cause this function to automatically select the optimal mask (cf. ISO/IEC 18004:2015, Section 7.8.3)
		 * @param maximizeErrorCorrectionCapacity If true, this function will try to maximize the error correction level as long as it doesn't increase the size of the smallest Micro QR code that can fit the data, cf. `errorCorrectionCapacity`
		 * @return True on success, otherwise false
		 */
		static bool encodeSegments(const Segments& segments, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>& modules, unsigned int& version, MicroQRCode::ErrorCorrectionCapacity& finalErrorCorrectionCapacity, unsigned int minVersion = 1u, unsigned int maxVersion = MicroQRCode::MAX_VERSION, const MaskingPattern mask = MP_PATTERN_UNKNOWN, const bool maximizeErrorCorrectionCapacity = true);

		/**
		 * Returns the number of modules that can be used to store data for a given Micro QR code version
		 * This is the number of all modules less the number of function modules (finder pattern, timing pattern, version and format information, and separators)
		 * @param version The version of a Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @return The number of modules that can be used to store data
		 */
		static inline unsigned int totalNumberRawDataModules(const unsigned int version);

		/**
		 * Return the number of codeword bits for a specified version and error correction level
		 * @param version The version of a Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param errorCorrectionCapacity The error correction capacity, must be valid given for the version, cf. ISO/IEC 18004:2015, Table 2
		 * @return The total number of codeword bits that fit into such a Micro QR code
		 */
		static inline unsigned int totalNumberDataCodewordBits(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		* Gets the symbol number for the version and error correction capacity
		* @param version The version of the Micro QR code, range [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		* @param errorCorrectionCapacity The error correction capacity, must be valid given for the version, cf. ISO/IEC 18004:2015, Table 2
		* @return The symbol number corresponding to the version and error correction capacity
		*/
		static inline uint32_t getSymbolNumber(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Return the number of codeword bits for a specified version and error correction level
		 * @param version The version of the Micro QR code, range [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param errorCorrectionCapacity The error correction capacity, must be valid given for the version, cf. ISO/IEC 18004:2015, Table 2
		 * @return The total number of codewords used for error correction
		 */
		static inline unsigned int totalNumberErrorCorrectionCodewords(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Computes the number of bits used given some data (segments) for a specified version of a QR code (this number varies depending on the version, i.e. bits per character)
		 * @param segments The segments for which the number of bits will be computed
		 * @param version The version of a QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param bitsUsed The total number of bits the `segments` will need in a QR code of version `version`
		 * @return True on success, otherwise false (e.g. because of an overflow)
		 */
		static inline bool computeTotalBitsUsed(const Segments& segments, const unsigned int version, unsigned int& bitsUsed);

		/**
		 * Generates the error correction codewords appends to the raw codewords
		 * @param codewords The raw code words for which the error code will be generated, must be valid
		 * @param version The version of the designated Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param errorCorrectionCapacity The level of error correction to be used
		 * @return The error-corrected codewords
		 */
		static Codewords addErrorCorrection(const Codewords& codewords, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Applies a data shuffle mask to the specified modules
		 * Note: Calling this function on the same data and with the same parameters a second time will undo the changes from the first time (because of the XOR used internally)
		 * @param modules The modules that will be shuffled, must be valid
		 * @param version The version of the designated Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param functionPatternMask The binary mask that indicates the location of function patterns (finder patern, etc.), which should not be shuffled, cf. `setFunctionPatterns()`
		 * @param mask The index of the shuffle mask, range: [0, 3]
		 * @sa setFunctionPatterns()
		 */
		static void applyMaskPattern(std::vector<uint8_t>& modules, const unsigned int version, const std::vector<uint8_t>& functionPatternMask, const MaskingPattern mask);

		/**
		 * Computes a score (fitness value) for a module configuration, cf. ISO/IEC 18004:2015, Section 7.8.3.2
		 * The result of this function is used to determine the optimal shuffle mask that is used to generate the Micro QR code
		 * @param modules The modules of the designated Micro QR code, must be valid
		 * @param version The version of the designated Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @return The score for this configuration of modules
		 */
		static unsigned int computeMaskPatternScore(const std::vector<uint8_t>& modules, const unsigned int version);

		/**
		 * Sets (draws) the modules (bits) of all function patterns
		 * Function patterns include: finder pattern, timing patterns, version and format information, and separators
		 *
		 * This function:
		 * 1. sets the function pattern in the modules (final bit matrix).
		 * 2. returns a binary mask denoting those bits (pixels) which are function patterns, i.e. which cannot store data and must not be overwritten.
		 *
		 * The other set*-functions use this mask in order to avoid overwriting function patterns.
		 *
		 * @param modules The modules where the codewords will be written to, must be valid and of size `MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version)`
		 * @param version The version of the designated Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param errorCorrectionCapacity The level of error correction for the designated Micro QR code
		 * @return A binary mask that will denote all locations of the modules with function patterns (pixel value = 255) and data modules (pixel value = 0), the size will be same as for `modules`
		 */
		static std::vector<uint8_t> setFunctionPatterns(std::vector<uint8_t>& modules, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Sets (draws) the codewords (zig-zag scan)
		 * Note: the size of the codewords must match exactly the version and level of error correction
		 * @param modules The modules where the codewords will be written to, must be valid and of size `MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version)`
		 * @param codewords The modules where the codewords will be written to, must be valid
		 * @param version The version of the designated Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION], must match the size of `modules`
		 * @param errorCorrectionCapacity The level of error correction used to generate this Micro QR code
		 * @param functionPatternMask The mask that is used to identify all function patterns in the Micro QR code (and to not overwrite them), cf. `setFunctionPatterns()`
		 * @sa setFunctionPatterns()
		 */
		static void setCodewords(std::vector<uint8_t>& modules, const Codewords& codewords, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const std::vector<uint8_t>& functionPatternMask);

		/**
		 * Sets (draws) the format information (15 bits) into the modules of a Micro QR code
		 * Note: format information = `s2 s1 s0 | m1 m0`, where `si` are the bits for the symbol number (cf. ISO/IEC 18004:2015, Table 13) and `mj` are the bits for the bit shuffle mask
		 * @param[in,out] modules The modules where the format information will be written to, must be valid and of size `MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version)`
		 * @param version The version of the designated Micro QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION], must match the size of `modules`
		 * @param errorCorrectionCapacity The level of error correction used to generate this Micro QR code
		 * @param mask The index of the bit shuffle mask used to generate this Micro QR code, range: [0, 3]
		 * @param functionPatternMask The mask that is used to identify all function patterns in the Micro QR code (and to not overwrite them), cf. `setFunctionPatterns()`
		 * @sa setFunctionPatterns()
		 */
		static void setFormatInformation(std::vector<uint8_t>& modules, const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern mask, std::vector<uint8_t>& functionPatternMask);

		/**
		 * Sets (draws) the version information (2x15 bits) into the modules of a QR code
		 * @param [in,out] modules The modules where the version information will be written to, must be valid and of size `MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version)`
		 * @param version The version of the designated QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION], must match the size of `module`
		 * @param functionPatternMask The mask that is used to identify all function pattern in the QR code (and to not overwrite them), cf. `setFunctionPatterns()`
		 * @sa setFunctionPatterns()
		 */
		static void setVersionInformation(std::vector<uint8_t>& modules, const unsigned int version, std::vector<uint8_t>& functionPatternMask);

		/**
		 * Returns the bit sequence identifying the encodation mode set (up to 4 bits long, cf. ISO/IEC 18004:2015, Table 2)
		 * @return The bit sequence of the encodation mode used
		 */
		static inline unsigned int encodationModeIndicatorBitSequence(const MicroQRCode::EncodingMode mode);

		/**
		 * Returns the number of bits per character for a specific version and encodation mode, cf. ISO/IEC 18004:2015, Table 3
		 * @param version Version number of a QR code, range: [MicroQRCode::MIN_VERSION, MicroQRCode::MAX_VERSION]
		 * @param mode The encodation mode, must be valid given the version (minimum M2 for alphanumeric, minimum M3 for byte and kanji)
		 * @return The number of bits per character (zero for invalid combinations of version and mode)
		*/
		static inline unsigned int getBitsInCharacterCountIndicator(const unsigned int version, const MicroQRCode::EncodingMode mode);
};

inline unsigned int MicroQRCodeEncoder::encodationModeIndicatorBitSequence(const MicroQRCode::EncodingMode mode)
{
	static_assert(int(MicroQRCode::EM_NUMERIC) == 0 && int(MicroQRCode::EM_ALPHANUMERIC) == 1 && int(MicroQRCode::EM_BYTE) == 2 && int(MicroQRCode::EM_KANJI) == 3, "Unexpected order of enums");
	ocean_assert((unsigned int)mode < 4u);

	constexpr unsigned int encodationModeBitSequences[4] =
	{
		0b000u, // MicroQRCode::EM_NUMERIC
		0b001u, // MicroQRCode::EM_ALPHANUMERIC
		0b010u, // MicroQRCode::EM_BYTE
		0b011u, // MicroQRCode::EM_KANJI
	};

	ocean_assert(encodationModeBitSequences[(unsigned int)mode] >> 3u == 0u);
	return encodationModeBitSequences[(unsigned int)mode];
}

inline unsigned int MicroQRCodeEncoder::getBitsInCharacterCountIndicator(const unsigned int version, const MicroQRCode::EncodingMode mode)
{
	static_assert(int(MicroQRCode::EM_NUMERIC) == 0 && int(MicroQRCode::EM_ALPHANUMERIC) == 1 && int(MicroQRCode::EM_BYTE) == 2 && int(MicroQRCode::EM_KANJI) == 3, "Unexpected order of enums");
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);
	ocean_assert((unsigned int)mode < 4u);

	constexpr unsigned int characterCountIndicators[16] =
	{
	// Cf. ISO/IEC 18004:2015, Table 3
	// 	M1
	// 	|   M2
	// 	|   |   M3
	// 	|   |   |   M4
	// 	|   |   |   |
		 3u, 4u, 5u, 6u, // Numeric
		 0u, 3u, 4u, 5u, // Alphanumeric
		 0u, 0u, 4u, 5u, // Byte
		 0u, 0u, 3u, 4u  // Kanji
	};

	const unsigned int column = version - 1u;
	ocean_assert(column < 4u);

	return characterCountIndicators[(unsigned int)mode * 4u + column];
}

inline uint32_t MicroQRCodeEncoder::encodeFormat(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern maskingPattern)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);
	ocean_assert(errorCorrectionCapacity != MicroQRCode::ECC_INVALID && errorCorrectionCapacity != MicroQRCode::ECC_30);
	ocean_assert(maskingPattern >> 2u == 0u);

	return encodeFormatBits(getSymbolNumber(version, errorCorrectionCapacity) << 2u | maskingPattern);
}

inline bool MicroQRCodeEncoder::decodeFormatBits(const uint32_t formatBits, unsigned int& version, MicroQRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, MaskingPattern& maskingPattern)
{
	ocean_assert(formatBits >> 15u == 0u);

	uint32_t minDistanceFormat = uint32_t(-1);
	uint32_t minDistance = uint32_t(-1);
	uint32_t minDistanceCounter = 0u;

	for (uint32_t referenceFormat = 0u; referenceFormat < 32u; ++referenceFormat)
	{
		const uint32_t referenceFormatBits = encodeFormatBits(referenceFormat);
		const uint32_t distance = computeHammingWeight(formatBits ^ referenceFormatBits);

		if (distance < minDistance)
		{
			minDistance = distance;
			minDistanceFormat = referenceFormat;
			minDistanceCounter = 1u;
		}
		else if (distance == minDistance)
		{
			minDistanceCounter++;
		}
	}

	// Check if the result is unambiguous, i.e. if at least two reference formats have the same Hamming distance the input format cannot be decoded unambiguously (>= 4 bits wrong).

	if (minDistanceCounter != 1u || minDistance >= 4u)
	{
		return false;
	}

	ocean_assert(minDistance != uint32_t(-1) && minDistanceFormat != uint32_t(-1));
	ocean_assert(minDistanceFormat >> 5u == 0u);
	
	switch (minDistanceFormat >> 2u)
	{
		case 0u:
			version = 1u;
			errorCorrectionCapacity = MicroQRCode::ECC_DETECTION_ONLY;
			break;

		case 1u:
			version = 2u;
			errorCorrectionCapacity = MicroQRCode::ECC_07;
			break;

		case 2u:
			version = 2u;
			errorCorrectionCapacity = MicroQRCode::ECC_15;
			break;

		case 3u:
			version = 3u;
			errorCorrectionCapacity = MicroQRCode::ECC_07;
			break;

		case 4u:
			version = 3u;
			errorCorrectionCapacity = MicroQRCode::ECC_15;
			break;

		case 5u:
			version = 4u;
			errorCorrectionCapacity = MicroQRCode::ECC_07;
			break;

		case 6u:
			version = 4u;
			errorCorrectionCapacity = MicroQRCode::ECC_15;
			break;

		case 7u:
			version = 4u;
			errorCorrectionCapacity = MicroQRCode::ECC_25;
			break;

		default:
			ocean_assert(false && "Never be here!");
			return false;
			break;
	}

	maskingPattern = MaskingPattern(minDistanceFormat & 0b00011u);

	return true;
}

inline uint32_t MicroQRCodeEncoder::encodeFormatBits(const uint32_t format)
{
	ocean_assert(format >> 5u == 0u);

	// Details in ISO/IEC 18004:2015, Annex C
	//
	// Compute the remainder of polynomial long division with a (15, 5) BCH code using the generator
	// polynomial G(x) = x^10 + x^8 + x^5 + x^4 + x^2 + x + 1 ~ 10100110111.

	const uint32_t remainder = computePolynomialDivisonRemainderBCH<15u, 5u, 0b10100110111u>(format << 10u);

	// Append the remainder to the format and XOR it with 100010001000101
	const uint32_t formatBitsUnmasked = (format << 10u) ^ remainder;
	const uint32_t formatBitsMasked = formatBitsUnmasked ^ 0b100010001000101u;
	ocean_assert(formatBitsMasked >> 15u == 0u);

	return formatBitsMasked;
}

inline unsigned int MicroQRCodeEncoder::totalNumberRawDataModules(const unsigned int version)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);

	// cf. ISO/IEC 18004:2015, Table 1
	static constexpr unsigned int rawDataModuleTable[4] = {36u, 80, 132u, 192u};

	const unsigned int rawDataModules = rawDataModuleTable[version - 1];

	ocean_assert(rawDataModules < MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version));
	return rawDataModules;
}

inline unsigned int MicroQRCodeEncoder::totalNumberErrorCorrectionCodewords(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	/// Number of error correction codewords, indexed by symbol number, cf. ISO/IEC 18004:2015, Table 2 and Table 9, column 4
	static constexpr int8_t ECC_CODEWORDS[8] = {2, 5, 6, 6, 8, 8, 10, 14};

	return ECC_CODEWORDS[getSymbolNumber(version, errorCorrectionCapacity)];
}

inline unsigned int MicroQRCodeEncoder::totalNumberDataCodewordBits(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	return (totalNumberRawDataModules(version) - totalNumberErrorCorrectionCodewords(version, errorCorrectionCapacity) * 8u);
}

inline bool MicroQRCodeEncoder::computeTotalBitsUsed(const Segments& segments, const unsigned int version, unsigned int& bitsUsed)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);

	bitsUsed = 0u;

	for (const Segment& segment : segments)
	{
		// Length of mode indicator varies by version, cf. ISO/IEC 18004:2015, Table 2
		const unsigned int modeIndicatorBits = version - 1u;
		const unsigned int characterCountBits = getBitsInCharacterCountIndicator(version, segment.encodationMode());

		// Make sure the segment fits into the field's bit width and the sum will not overflow.
		if (segment.characters() >= (1u << characterCountBits)
			|| (modeIndicatorBits + characterCountBits) > (NumericT<unsigned int>::maxValue() - bitsUsed)
			|| segment.bitBuffer().size() > (NumericT<unsigned int>::maxValue() - bitsUsed))
		{
			return false;
		}

		bitsUsed += modeIndicatorBits + characterCountBits;
		bitsUsed += (unsigned int)segment.bitBuffer().size();
	}

	return true;
}

inline uint32_t MicroQRCodeEncoder::getSymbolNumber(const unsigned int version, const MicroQRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	ocean_assert(version >= MicroQRCode::MIN_VERSION && version <= MicroQRCode::MAX_VERSION);

	switch(errorCorrectionCapacity)
	{
		case MicroQRCode::ECC_DETECTION_ONLY:
			ocean_assert(version == 1u);
			if (version == 1u)
			{
				return 0u;
			}
			break;
		case MicroQRCode::ECC_07:
			ocean_assert(version >= 2u && version <= MicroQRCode::MAX_VERSION);
			if (version >= 2u && version <= MicroQRCode::MAX_VERSION)
			{
				return 2u * version - 3u;
			}
			break;
		case MicroQRCode::ECC_15:
			ocean_assert(version >= 2u && version <= MicroQRCode::MAX_VERSION);
			if (version >= 2u && version <= MicroQRCode::MAX_VERSION)
			{
				return 2u * version - 2u;
			}
			break;
		case MicroQRCode::ECC_25:
			ocean_assert(version == MicroQRCode::MAX_VERSION);
			if (version == MicroQRCode::MAX_VERSION)
			{
				return 7u;
			}
			break;
		default:
			break;
	}

	ocean_assert(false && "Never be here!");
	return INVALID_VALUE;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
