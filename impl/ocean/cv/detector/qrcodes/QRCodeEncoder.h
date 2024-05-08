/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"

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
class OCEAN_CV_DETECTOR_QRCODES_EXPORT QRCodeEncoder
{
	public:

		/// Codeword: sequence of 8 bits
		typedef uint8_t Codeword;

		/// Vector of codewords
		typedef std::vector<Codeword> Codewords;

		/// Typedef for a bit buffer
		typedef std::vector<uint8_t> BitBuffer;

		/// Forward declaration of the segment class
		class Segment;

		/// Typedef for a vector of segments
		typedef std::vector<Segment> Segments;

		/**
		 * Definition of the segment class
		 * A sequence is a sequence of data encoded according to the rules of one ECI or encodation mode
		 */
		class OCEAN_CV_DETECTOR_QRCODES_EXPORT Segment
		{
			public:
				/**
				 * Constructor for segments
				 * @param mode The data encodation mode of this segment
				 * @param characters The number of characters stored in this segment, range: [0, infinity)
				 * @param bitBuffer The bit sequence representing the data stored in this segment, must be valid
				 */
				inline Segment(const QRCode::EncodingMode mode, const unsigned int characters, BitBuffer& bitBuffer);

				/**
				 * Returns the encodation mode set for this segment
				 * @return The encodation mode
				 */
				inline QRCode::EncodingMode encodationMode() const;

				/**
				 * Returns the number of characters stored in this segment
				 * @return The number of characters stored in this segment
				 */
				inline unsigned int characters() const;

				/**
				 * Returns the encoded bits stored in this segment
				 * @return The encoded bits
				 */
				inline const BitBuffer& bitBuffer() const;

				/**
				 * Encode a sequence of digits (0-9) and store it in a segment
				 * @param data The data that will be encoded, must be a sequence of digits, i.e., `isNumeric(data) == true`
				 * @param segments The segment storing the encoded data will be appended to this vector, memory will be initialized internally
				 * @return True if the data was successfully encoded, otherwise false
				 */
				static bool generateSegmentNumeric(const std::string& data, Segments& segments);

				/**
				 * Encode a sequence of alphanumeric characters (cf. ISO/IEC 18004:2015, Table 5) and store it in a segment
				 * @param data The data that will be encoded, must be a sequence of alphanumeric characters, i.e., `isAlphanumeric(data) == true`
				 * @param segments The segment storing the encoded data will be appended to this vector, memory will be initialized internally
				 * @return True if the data was successfully encoded, otherwise false
				 */
				static bool generateSegmentAlphanumeric(const std::string& data, Segments& segments);

				/**
				 * Encode a sequence of bytes and store it in a segment
				 * @param data The data that will be encoded
				 * @param segments The segment storing the encoded data will be appended to this vector, memory will be initialized internally
				 * @return True if the data was successfully encoded, otherwise false
				 */
				static bool generateSegmentsBytes(const std::vector<uint8_t>& data, Segments& segments);

				/**
				 * Returns the bit sequence identifying the encodation mode set for this segment (4 bits long)
				 * @return The bit sequence of the encodation mode used (only use the first 4 bits)
				 */
				static inline unsigned int encodationModeIndicatorBitSequence(QRCode::EncodingMode mode);

				/**
				 * Returns the number of bits per charactor for a specific version and encodation mode, cf. ISO/IEC 18004:2015, Table 3
				 * @param version Version number of a QR code, range: [0, 40]
				 * @param mode The encodation mode
				 * @return The number of bits per character or `(unsigned int)(-1)` on failure
				 */
				static inline unsigned int getBitsInCharacterCountIndicator(unsigned int version, QRCode::EncodingMode mode);

				/**
				 * Helper function to append a certain number of bits of a number to a bit buffer
				 * @param value The value of which the first `N` bits will be appended to the bit buffer
				 * @param bits The number of bits to append, range: [0, sizeof(unsigned int) * 8]
				 * @param bitBuffer The bit buffer to which the `N` bits will be appended
				 */
				static inline void bitBufferAppend(const unsigned int value, size_t bits, BitBuffer& bitBuffer);

				/**
				 * Test to check if data is numeric (consisting of only digits)
				 * @param data The data to be tested
				 * @return True if the data consists of only digits, otherwise false
				 */
				static inline bool isNumericData(const std::string& data);

				/**
				 * Test to check if data contains only alphanumeric characters
				 * @param data The data to be tested
				 * @return True if the data consists of alphanumeric characters, otherwise false
				 * @sa ALPHANUMERIC_CHARSET
				 */
				static inline bool isAlphanumericData(const std::string& data);

				/**
				 * Returns the character set for the alphanumeric data mode
				 * The character set for the alphanumeric data mode, cf. ISO/IEC 18004:2015, Table 5. The index
				 * of each character in the string corresponds to the value assigned to them in the alphanumeric
				 * encoding/decoding table.
				 * @return The character set for the alphanumeric data mode.
				 */
				static inline const std::string& getAlphanumericCharset();

			protected:

				/// The mode used to encode the data of this segment
				QRCode::EncodingMode encodationMode_;

				/// The number of characters stored in this segment
				unsigned int characters_;

				/// The actual encoded data (sequence of bits)
				BitBuffer bitBuffer_;
		};

		/**
		 * Definition of the ReedSolomon class
		 */
		class ReedSolomon
		{
			public:

				/// Coefficients of the divisor polynomial, stored from highest to lower power (excluding the leading term which is always 1). Example x^3 + 255x^2 + 8x + 93 is stored as {255, 8, 93}.
				typedef std::vector<uint8_t> Coefficients;

			public:

				/**
				 * Generates the Reed-Solomon coefficients for a divisor polynomial of degree `N`.
				 * @param degree The degree of the divisor polynomial, range: [1, infinity)
				 * @return The coeffiencts of the divisor polynomial (will have `N = degree` elements)
				 */
				static Coefficients generateCoefficients(const unsigned int degree);

				/**
				 * Computes the Reed-Solomon error correction codewords for a sequence of data codewords
				 * @param codewords The input codewords for which the error correction will be computed
				 * @param coefficients The coefficients of the Reed-Solomon divisor polynomial of degree `N`
				 * @return The error correction codewords
				 */
				static Codewords computeRemainders(const Codewords& codewords, const Coefficients& coefficients);

			protected:

				/**
				 * Return the product of two fields modulo GF(2^8/0x11D)
				 */
				static uint8_t multiply(const uint8_t a, const uint8_t b);
		};

		/**
		 * Enum for the mask patterns used to shuffle modules of a QR code.
		 * The values of the enum items correspond to the standard-defined bit sequences for the masks (3 bits long)
		 */
		enum MaskingPattern : uint32_t
		{
			/// Masking condition: (i + j) mod 2 = 0
			MP_PATTERN_0 = 0b000u,

			/// Masking condition: i mod 2 = 0
			MP_PATTERN_1 = 0b001u,

			/// Masking condition: j mod 3 = 0
			MP_PATTERN_2 = 0b010u,

			/// Masking condition: (i + j) mod 3 = 0
			MP_PATTERN_3 = 0b011u,

			/// Masking condition: ((i div 2) + (j div 3)) mod 2 = 0
			MP_PATTERN_4 = 0b100u,

			/// Masking condition: (i j) mod 2 + (i j) mod 3 = 0
			MP_PATTERN_5 = 0b101u,

			/// Masking condition: ((i j) mod 2 + (i j) mod 3) mod 2 = 0
			MP_PATTERN_6 = 0b110u,

			/// Masking condition: ((i j) mod 3 + (i+j) mod 2) mod 2 = 0
			MP_PATTERN_7 = 0b111u,

			/// Denotes unknown masking patterns (not part of the standard)
			MP_PATTERN_UNKNOWN = uint32_t(-1)
		};

		/// Number of error correction codewords (rows: 0 - low, 1 - medium, 2 - quartile, 3 - high, column 0 is ignored since no version 0 exists), cf. ISO/IEC 18004:2015, Table 9, column 4
		static const int8_t ECC_CODEWORDS_PER_BLOCK[4][41];

		/// Number of error correction blocks (rows: 0 - low, 1 - medium, 2 - quartile, 3 - high, column 0 is ignored since no version 0 exists), cf. ISO/IEC 18004:2015, Table 9, column 6
		static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41];

	public:

		/**
		 * Encode text and store it in a QR code, will automatically choose the most efficient encodation mode
		 * @param text The text/data to be encoded as a QR code
		 * @param errorCorrectionCapacity Specifies the level of possible error correction
		 * @param qrcode The QR code that will store the encoded data
		 * @return True if the QR code has been successfully generated
		 */
		static bool encodeText(const std::string& text, QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, QRCode& qrcode);

		/**
		 * Encode binary data and store it in a QR code, will always use the byte encodation mode
		 * @param data The data to be encoded as a QR code
		 * @param errorCorrectionCapacity Specifies the level of possible error correction
		 * @param qrcode The QR code that will store the encoded data
		 * @return True if the QR code has been successfully generated
		 */
		static bool encodeBinary(const std::vector<uint8_t>& data, QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, QRCode& qrcode);

		/**
		 * Encodes the error correction level and the index of the masking pattern as a sequence of 15 bits with error correction ((15, 5) BCH code).
		 * @param errorCorrectionCapacity An error correction capacity that will be decoded
		 * @param maskingPattern The masking pattern that will be encoded
		 * @return The error correction level and masking pattern encoded as a sequence of 15 bits
		 */
		static inline uint32_t encodeFormat(const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern maskingPattern);

		/**
		 * Decodes a sequence of 15 bits and extracts the encoded error correction level and index of the masking pattern
		 * Note: the format bit sequence can contain up to 3 bit error. More bit errors will cause this function to fail because the result would be ambiguous.
		 * @param formatBits A sequence of 15 bits containing the format information, range: [0, 2^15)
		 * @param errorCorrectionCapacity The error correction capacity extracted from the bit sequence
		 * @param maskingPattern The masking pattern extracted from the bit sequence
		 * @return True if the sequence was decoded successfully, otherwise false
		 */
		static inline bool decodeFormatBits(const uint32_t formatBits, QRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, MaskingPattern& maskingPattern);

		/**
		 * Encodes the version numbers as a sequences of 18 bits with error correction ((18, 6) BCH code).
		 * @param version The version number to be encoded as a 18 bit long sequence, range: [7, 40] (QR code version 1-6 have no version information bit field)
		 * @return The version number encoded as a 18-bit long sequence
		 */
		static inline uint32_t encodeVersion(const uint32_t version);

		/**
		 * Decodes a sequence of 18 bits and extracts the encoded version number
		 * Note: the version bit sequence can contain up to 4 bit error. More bit errors will cause this function to fail because the result would be ambiguous.
		 * @param versionBits The sequence of 18 bits containing the encoded version information, range: [0, 2^18)
		 * @param version The version number that was extracted from the input bit sequence
		 * @return True if the sequence was decoded successfully, otherwise false
		 */
		static inline bool decodeVersionBits(const uint32_t versionBits, uint32_t& version);

		/**
		 * Computes the 2D locations of the alignment patterns for a specified version of a QR code
		 * @param version The version of a QR code, range: [1, 40]
		 * @return The list of the locations of alignment patterns, the locations are guaranteed to be in row-wise order (left-to-right and top-to-bottom), will be empty for version 1
		 */
		static VectorsI2 computeAlignmentPatternPositions(const unsigned int version);

		/**
		 * Encodes the format information as a sequence of 15 bits with error correction ((15, 5) BCH code).
		 * @param format The format bit sequence consisting of the concatenated error correction capacity (2 bits) and masking pattern (3 bits), range: [0, 32)
		 * @return The error correction level and masking pattern encoded as a sequence of 15 bits
		 * @sa encodeFormat(const ErrorCorrectionCapacity, const MaskingPattern)
		 */
		static inline uint32_t encodeFormat(const uint32_t format);

		/**
		 * Computes the remainder of a polynomial long division for (n, k) BCH codes
		 * Notation:
		 * n : block length in bits
		 * k : number of information/data bits
		 * @param data The data used as the nominator in this division
		 * @return The remainder of the polynomial division
		 * @tparam tBlockLength The length, n, of the BCH code in bits, range: (tDataLength, infinity)
		 * @tparam tDataLength The number the information/data bits, k, range: [1, tBlockLength)
		 * @tparam tGeneratorPolynomial The generator polynomial used by this code for the polynomial division. This is specificied as an integer (or binary number). Range: (0, 2^n - 1) (Important: make sure this value is correct according to the Galois field theory behind it, there are no additional sanity checks)
		 */
		template <uint32_t tBlockLength, uint32_t tDataLength, uint32_t tGeneratorPolynomial>
		static inline uint32_t computePolynomialDivisonRemainderBCH(const uint32_t data);

		/**
		 * Determines the number of 1-bits in an integer value (Hamming weight)
		 * @param value The value of which the 1-bits will be counted
		 * @return The number of 1-bits in the input value
		 */
		static inline uint32_t computeHammingWeight(uint32_t value);

	protected:

		/**
		 * Helper function to initialize a QR code instance
		 * @param version The version of this QR code, range: [1, 40]
		 * @param errorCorrectionCapacity The error correction level that will be used to generate the error-corrected codewords stored in this QR code
		 * @param rawCodewords The encoded codewords. The size must fit exactly into the selected version of this QR code
		 * @param mask The index of the bit shuffle masked that was used to generate the modules of this QR code
		 * @param modules The resulting modules of the QR code
		 * @return True if the initialization was successful, otherwise false
		 */
		static bool addErrorCorrectionAndCreateQRCode(const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const Codewords& rawCodewords, MaskingPattern mask, std::vector<uint8_t>& modules);

		/**
		 * Encodes segments and writes them into a QR code
		 * @param segments The raw segements that will be encoded and written into the QR code instance, `qrcode`
		 * @param errorCorrectionCapacity The desired level of error recovery capacity. Note: if `maximizeErrorCorrectionCapacity` is true, this function will increase this level as long as it doesn't increase the size of the smallest QR code that can fit the data
		 * @param modules The resulting modules of the QR code
		 * @param version The resulting version of the QR code
		 * @param finalErrorCorrectionCapacity The resulting error correction capacity that the QR code will finally have
		 * @param minVersion The minimum version that the final QR code is supposed to have, range: [1, maxVersion]
		 * @param maxVersion The maximum version that the final QR code is supposed to have, range: [minVersion, 40]. Note: if this value is chosen too small, the initialization may fail
		 * @param mask The index of the bit shuffle mask that is to be used, range: [0, 7] or (unsigned int)(-1). The latter value will cause this function to automatically select the optimal mask (cf. ISO/IEC 18004:2015, Section 7.8.3)
		 * @param maximizeErrorCorrectionCapacity If true, this function will try to maximize the error correction level as long as it doesn't increase the size of the smallest QR code that can fit the data, cf. `errorCorrectionCapacity`
		 * @return True on success, otherwise false
		 */
		static bool encodeSegments(const Segments& segments, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>& modules, unsigned int& version, QRCode::ErrorCorrectionCapacity& finalErrorCorrectionCapacity, const unsigned int minVersion = 1u, const unsigned int maxVersion = 40u, const MaskingPattern mask = MP_PATTERN_UNKNOWN, const bool maximizeErrorCorrectionCapacity = true);

		/**
		 * Returns the number of modules that can be used to store data for a given QR code version
		 * This is the number of all modules less the number of function modules (finder pattern, timing pattern, alignment pattern, version and format information, black pixel, and separators)
		 * @param version The version of a QR code, range: [1, 40]
		 * @return The number of modules that can be used to store data
		 */
		static inline unsigned int totalNumberRawDataModules(const unsigned int version);

		/**
		 * Return the number of codewords for a specified version and error correction level
		 * @param version The version of a QR code, range: [1, 40]
		 * @param errorCorrectionCapacity The error correction level of a QR code
		 * @return The total number of codewords that fit into such a QR code
		 */
		static inline unsigned int totalNumberDataCodewords(const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Computes the number of bits used given some data (segments) for a specified version of a QR code (this number varies depending on the version, i.e. bits per character)
		 * @param segments The segments for which the number of bits will be computed
		 * @param version The version of a QR code, range: [1, 40]
		 * @param bitsUsed The total number of bits the `segments` will need in a QR code of version `version`
		 * @return True on success, otherwise false (e.g. because of an overflow)
		 */
		static inline bool computeTotalBitsUsed(const Segments& segments, const unsigned int version, unsigned int& bitsUsed);

		/**
		 * Generates the error correction codewords and interleaves the with the raw codewords
		 * @param codewords The raw code words for which the error code will be generated, must be valid
		 * @param version The version of the designated QR code, range: [1, 40]
		 * @param errorCorrectionCapacity The level of error correction to be used
		 * @return The error-corrected + interleaved codewords
		 */
		static Codewords addErrorCorrectionAndInterleave(const Codewords& codewords, const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Applies a data shuffle mask to the specified modules
		 * Note: Calling this function on the same data and with the same parameters a second time will undo the changes from the first time (because of the XOR used internally)
		 * @param modules The modules that will be shuffled, must be valid
		 * @param version The version of the designated QR code, range: [1, 40]
		 * @param functionPatternMask The binary mask that indicates the location of function patterns (finder patern, etc.), which should not be shuffled, cf. `setFunctionPatterns()`
		 * @param mask The index of the shuffle mask, range: [0, 7]
		 * @sa setFunctionPatterns()
		 */
		static void applyMaskPattern(std::vector<uint8_t>& modules, const unsigned int version, const std::vector<uint8_t>& functionPatternMask, const MaskingPattern mask);

		/**
		 * Computes a penalty value (fitness value) for a module configuration, cf. ISO/IEC 18004:2015, Section 7.8.3.1
		 * The result of this function is used to determine the optimal shuffle mask that is used to generate the QR code
		 * @param modules The modules of the designated QR code, must be valid
		 * @param version The version of the designated QR code, range: [1, 40]
		 * @return The penalty score for this configuration of modules
		 */
		static unsigned int computeMaskPatternPenalty(const std::vector<uint8_t>& modules, const unsigned int version);

		/**
		 * Sets (draws) the modules (bits) of all function patterns
		 * Function patterns include: finder patterns, alignment patterns, timing patterns, version and format information, separators, and the black pixel
		 *
		 * This function:
		 * 1. sets the function patterns in the modules (final bit matrix).
		 * 2. returns a binary mask denoting those bits (pixels) which are function patterns, i.e. which cannot store data and must not be overwritten.
		 *
		 * The other set*-functions use this mask in order to avoid overwriting function patterns.
		 *
		 * @param modules The modules where the codewords will be written to, must be valid and of size `QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version)`
		 * @param version The version of the designated QR code, range: [1, 40]
		 * @param errorCorrectionCapacity The level of error correction for the designated QR code
		 * @return A binary mask that will denote all locations of the modules with function patterns (pixel value = 255) and data modules (pixel value = 0), the size will be same as for `modules`
		 */
		static std::vector<uint8_t> setFunctionPatterns(std::vector<uint8_t>& modules, const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Sets (draws) the codewords (zig-zag scan)
		 * Note: the size of the codewords must match exactly the version and level of error correction
		 * @param modules The modules where the codewords will be written to, must be valid and of size `QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version)`
		 * @param codewords The modules where the codewords will be written to, must be valid
		 * @param version The version of the designated QR code, range: [1, 40], must match the size of `modules`
		 * @param functionPatternMask The mask that is used to identify all function patterns in the QR code (and to not overwrite them), cf. `setFunctionPatterns()`
		 * @sa setFunctionPatterns()
		 */
		static void setCodewords(std::vector<uint8_t>& modules, const Codewords& codewords, const unsigned int version, const std::vector<uint8_t>& functionPatternMask);

		/**
		 * Sets (draws) the format information (2x5 bits) into the modules of a QR code
		 * Note: format information = `e1 e0 | m2 m1 m0`, where `ei` and `mj` are the bits for the error correction level and bit shuffle mask, respectively
		 * @param[in,out] modules The modules where the format information will be written to, must be valid and of size `QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version)`
		 * @param version The version of the designated QR code, range: [1, 40], must match the size of `modules`
		 * @param errorCorrectionCapacity The level of error correction used to generate this QR code
		 * @param mask The index of the bit shuffle mask used to generate this QR code, range: [0, 7]
		 * @param functionPatternMask The mask that is used to identify all function patterns in the QR code (and to not overwrite them), cf. `setFunctionPatterns()`
		 * @sa setFunctionPatterns()
		 */
		static void setFormatInformation(std::vector<uint8_t>& modules, const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern mask, std::vector<uint8_t>& functionPatternMask);

		/**
		 * Sets (draws) the version information (2x15 bits) into the modules of a QR code
		 * @param [in,out] modules The modules where the version information will be written to, must be valid and of size `QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version)`
		 * @param version The version of the designated QR code, range: [1, 40], must match the size of `module`
		 * @param functionPatternMask The mask that is used to identify all function pattern in the QR code (and to not overwrite them), cf. `setFunctionPatterns()`
		 * @sa setFunctionPatterns()
		 */
		static void setVersionInformation(std::vector<uint8_t>& modules, const unsigned int version, std::vector<uint8_t>& functionPatternMask);
};

inline QRCodeEncoder::Segment::Segment(const QRCode::EncodingMode mode, const unsigned int characters, BitBuffer& bitBuffer) :
	encodationMode_(mode),
	characters_(characters),
	bitBuffer_(bitBuffer)
{
	// Nothing else to do.
}

inline QRCode::EncodingMode QRCodeEncoder::Segment::encodationMode() const
{
	return encodationMode_;
}

inline unsigned int QRCodeEncoder::Segment::characters() const
{
	return characters_;
}

inline const QRCodeEncoder::BitBuffer& QRCodeEncoder::Segment::bitBuffer() const
{
	return bitBuffer_;
}

inline unsigned int QRCodeEncoder::Segment::encodationModeIndicatorBitSequence(QRCode::EncodingMode mode)
{
	static_assert(int(QRCode::EM_NUMERIC) == 0 && int(QRCode::EM_ALPHANUMERIC) == 1 && int(QRCode::EM_BYTE) == 2 && int(QRCode::EM_KANJI) == 3 && int(QRCode::EM_ECI) == 4 && int(QRCode::EM_STRUCTURED_APPEND) == 5 && int(QRCode::EM_FNC1), "Unexpected order of enums");
	ocean_assert((unsigned int)mode < 7u);

	const unsigned int encodationModeBitSequences[7] =
	{
		0b0001u, // QRCode::EM_NUMERIC
		0b0010u, // QRCode::EM_ALPHANUMERIC
		0b0100u, // QRCode::EM_BYTE
		0b1000u, // QRCode::EM_KANJI
		0b0111u, // QRCode::EM_ECI
		0b0011u, // QRCode::EM_STRUCTURED_APPEND

		// Note: This mode has two different return values and requires a different solution; since this mode isn't currently supported we'll ignore this
		0b0000u, // QRCode::EM_FNC1
	};

	ocean_assert(encodationModeBitSequences[(unsigned int)mode] >> 4u == 0u);
	return encodationModeBitSequences[(unsigned int)mode];
}

inline unsigned int QRCodeEncoder::Segment::getBitsInCharacterCountIndicator(unsigned int version, QRCode::EncodingMode mode)
{
	static_assert(int(QRCode::EM_NUMERIC) == 0 && int(QRCode::EM_ALPHANUMERIC) == 1 && int(QRCode::EM_BYTE) == 2 && int(QRCode::EM_KANJI) == 3 && int(QRCode::EM_ECI) == 4, "Unexpected order of enums");
	ocean_assert(version >= 1u && version <= 40u);
	ocean_assert((unsigned int)mode < 5u);

	const unsigned int characterCountIndicators[15] =
	{
	// Cf. ISO/IEC 18004:2015, Table 3
	// 	Versions 1-9
	// 	|    Versions 10-26
	// 	|    |   Versions 27-40
	// 	|    |   |
		10, 12, 14, // Numeric
		 9, 11, 13, // Alphanumeric
		 8, 16, 16, // Byte
		 8, 10, 12, // Kanji
		 0,  0,  0  // ECI
	};

	const unsigned int column = (version + 7u) / 17u;
	ocean_assert(column < 3u);

	return characterCountIndicators[(unsigned int)mode * 3u + column];
}

inline bool QRCodeEncoder::Segment::isNumericData(const std::string& data)
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (std::isdigit((unsigned char)data[i]) == false)
		{
			return false;
		}
	}

	return data.size() != 0;
}

inline bool QRCodeEncoder::Segment::isAlphanumericData(const std::string& data)
{
	if (data.empty())
	{
		return false;
	}

	const std::string& alphanumericCharset = getAlphanumericCharset();

	for (const char character : data)
	{
		if (alphanumericCharset.find(character) == std::string::npos)
		{
			return false;
		}
	}

	return true;
}

inline const std::string& QRCodeEncoder::Segment::getAlphanumericCharset()
{
	const static std::string alphanumericCharset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
	ocean_assert(alphanumericCharset.size() == 45);

	return alphanumericCharset;
}

inline void QRCodeEncoder::Segment::bitBufferAppend(const unsigned int value, size_t bits, BitBuffer& bitBuffer)
{
	ocean_assert(bits <= sizeof(unsigned int) * 8);

	if (bits == 0u)
	{
		return;
	}

	for (size_t i = bits - 1; i < bits; --i)
	{
		bitBuffer.push_back((value >> i) & 1);
	}
}

inline uint32_t QRCodeEncoder::encodeFormat(const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity, const MaskingPattern maskingPattern)
{
	ocean_assert(errorCorrectionCapacity >> 2u == 0u && maskingPattern >> 3u == 0u);
	return encodeFormat(errorCorrectionCapacity << 3u | maskingPattern);
}

inline bool QRCodeEncoder::decodeFormatBits(const uint32_t formatBits, QRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, MaskingPattern& maskingPattern)
{
	ocean_assert(formatBits >> 15u == 0u);

	const uint32_t formatBitsUnmasked = formatBits ^ 0b101010000010010u;

	uint32_t minDistanceFormat = uint32_t(-1);
	uint32_t minDistance = uint32_t(-1);
	uint32_t minDistanceCounter = 0u;

	for (uint32_t referenceFormat = 0u; referenceFormat < 32u; ++referenceFormat)
	{
		const uint32_t referenceFormatBitsMasked = encodeFormat(referenceFormat);
		const uint32_t referenceFormatBitsUnmasked = referenceFormatBitsMasked ^ 0b101010000010010u; // TODO Is it possible to leave out the unmasking (XOR)?
		const uint32_t distance = computeHammingWeight(formatBitsUnmasked ^ referenceFormatBitsUnmasked);

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

	switch (minDistanceFormat >> 3u)
	{
		case 0b01u:
			errorCorrectionCapacity = QRCode::ECC_07;
			break;

		case 0b00u:
			errorCorrectionCapacity = QRCode::ECC_15;
			break;

		case 0b11u:
			errorCorrectionCapacity = QRCode::ECC_25;
			break;

		case 0b10u:
			errorCorrectionCapacity = QRCode::ECC_30;
			break;

		default:
			ocean_assert(false && "Never be here!");
			return false;
			break;
	}

	maskingPattern = MaskingPattern(minDistanceFormat & 0b00111);

	return true;
}

inline uint32_t QRCodeEncoder::encodeVersion(const uint32_t version)
{
	ocean_assert(version != 0u && version <= 40u);
	ocean_assert(version >> 6u == 0u);

	// Details in ISO/IEC 18004:2015, Annex D
	//
	// Compute the remainder of polynomial long division with a (18, 6) BCH code (or Golay code) using the generator
	// polynomial G(x) = x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1 ~ 1111100100101.

	const uint32_t remainder = computePolynomialDivisonRemainderBCH<18u, 6u, 0b1111100100101u>(version << 12u);
	const uint32_t versionBits = (version << 12u) ^ remainder;
	ocean_assert(versionBits >> 18u == 0u);

	return versionBits;
}

inline bool QRCodeEncoder::decodeVersionBits(const uint32_t versionBits, uint32_t& version)
{
	ocean_assert(versionBits >> 18u == 0u);

	uint32_t minDistanceVersion = uint32_t(-1);
	uint32_t minDistance = uint32_t(-1);
	uint32_t minDistanceCounter = 0u;

	// Note: QR codes version 1-6 do not have a bit field for their version information but mathematically it's valid to decode those sequences as well
	for (uint32_t referenceVersion = 1u; referenceVersion <= 40u; ++referenceVersion)
	{
		const uint32_t referenceVersionBits = encodeVersion(referenceVersion);
		const uint32_t distance = computeHammingWeight(versionBits ^ referenceVersionBits);

		if (distance < minDistance)
		{
			minDistance = distance;
			minDistanceVersion = referenceVersion;
			minDistanceCounter = 1u;
		}
		else if (distance == minDistance)
		{
			minDistanceCounter++;
		}
	}

	// Check if the result is unambiguous, i.e. if at least two reference versions have the same Hamming distance the input version cannot be decoded unambiguously (>= 5 bits wrong).

	if (minDistanceCounter != 1u || minDistance >= 5u)
	{
		return false;
	}

	ocean_assert(minDistance != uint32_t(-1) && minDistanceVersion != uint32_t(-1));
	ocean_assert(minDistanceVersion >> 6u == 0u);

	version = minDistanceVersion;

	return true;
}

inline uint32_t QRCodeEncoder::encodeFormat(const uint32_t format)
{
	ocean_assert(format >> 5u == 0u);

	// Details in ISO/IEC 18004:2015, Annex C
	//
	// Compute the remainder of polynomial long division with a (15, 5) BCH code using the generator
	// polynomial G(x) = x^10 + x^8 + x^5 + x^4 + x^2 + x + 1 ~ 10100110111.

	const uint32_t remainder = computePolynomialDivisonRemainderBCH<15u, 5u, 0b10100110111u>(format << 10u);

	// Append the remainder to the format and XOR it with 101010000010010
	const uint32_t formatBitsUnmasked = (format << 10u) ^ remainder;
	const uint32_t formatBitsMasked = formatBitsUnmasked ^ 0b101010000010010u;
	ocean_assert(formatBitsMasked >> 15u == 0u);

	return formatBitsMasked;
}

template <uint32_t tBlockLength, uint32_t tDataLength, uint32_t tGeneratorPolynomial>
inline uint32_t QRCodeEncoder::computePolynomialDivisonRemainderBCH(const uint32_t data)
{
	static_assert(tBlockLength != 0u && tDataLength != 0u && tBlockLength > tDataLength, "The block length must be larger than the number of data bits and both must be nonzero.");
	static_assert(tGeneratorPolynomial != 0u && tGeneratorPolynomial >> tBlockLength == 0u, "The generator polynomial cannot be zero and must fit into the block length");
	ocean_assert(data >> tBlockLength == 0u);

	// Example for a (15, 5) BCH code:
	//
	//  * block length: 15
	//  * data length: 5
	//  * generator polynomial = 10100110111 ~ G(x) = x^10 + x^8 + x^5 + x^4 + x^2 + x + 1
	//  * data = 000111101011001, i.e. 00011|1101011001
	//                                 data | error correction
	//
	// remainder0 = 000111101011001
	//            ^ 10100110111     skip step - left-most bit of the remainder is zero
	//            -----------------
	// remainder1 = 000111101011001
	//            ^  10100110111    skip step - left-most bit of the remainder is zero
	//            -----------------
	// remainder2 = 000111101011001
	//            ^   10100110111   skip step - left-most bit of the remainder is zero
	//            -----------------
	// remainder3 = 000111101011001
	//            ^    10100110111
	//            -----------------
	// remainder4 = 000010100110111
	//            ^     10100110111
	//            -----------------
	// remainder  = 000000000000000

	constexpr uint32_t errorCorrectionBits = tBlockLength - tDataLength;

	uint32_t remainder = data;

	for (uint32_t i = 0u; i < tDataLength; ++i)
	{
		ocean_assert(tDataLength >= i + 1u);
		const uint32_t shift = tDataLength - i - 1u;

		if (remainder & (1u << (shift + errorCorrectionBits)))
		{
			remainder ^= tGeneratorPolynomial << shift;
		}
	}

	ocean_assert(tGeneratorPolynomial > remainder);
	return remainder;
}

uint32_t QRCodeEncoder::computeHammingWeight(uint32_t value)
{
	uint32_t weight = 0u;

	while (value > 0u)
	{
		value = value & (value - 1u);
		weight += 1u;
	}

	return weight;
}

inline unsigned int QRCodeEncoder::totalNumberRawDataModules(const unsigned int version)
{
	ocean_assert(version != 0u && version <= 40u);

	// TODO Improve documentation of this calculation

	// Number of total modules (4 * version + 17)^2 minus the modules for the
	// * finder patterns
	// * separators
	// * timing patterns
	// * version information
	unsigned int rawDataModules = (16u * version + 128u) * version + 64u;

	// Subtract the modules for the alignment patterns, if applicable
	if (version >= 2u)
	{
		const unsigned int alignmentPatterns = (version / 7u) + 2u;

		ocean_assert(rawDataModules >= (25u * alignmentPatterns - 10u) * alignmentPatterns - 55u);
		rawDataModules -= (25u * alignmentPatterns - 10u) * alignmentPatterns - 55u;

		if (version >= 7u)
		{
			ocean_assert(rawDataModules >= 36u);
			rawDataModules -= 36u;
		}
	}

	ocean_assert(rawDataModules < QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version));
	return rawDataModules;
}

inline unsigned int QRCodeEncoder::totalNumberDataCodewords(const unsigned int version, const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	ocean_assert(version != 0u && version <= 40u);
	ocean_assert(uint32_t(errorCorrectionCapacity) < 4u);

	return (totalNumberRawDataModules(version) / 8u) - (ECC_CODEWORDS_PER_BLOCK[errorCorrectionCapacity][version] * NUM_ERROR_CORRECTION_BLOCKS[errorCorrectionCapacity][version]);
}

inline bool QRCodeEncoder::computeTotalBitsUsed(const Segments& segments, const unsigned int version, unsigned int& bitsUsed)
{
	ocean_assert(version != 0u && version <= 40u);

	bitsUsed = 0u;

	for (const Segment& segment : segments)
	{
		const unsigned int bitsPerCharacter = Segment::getBitsInCharacterCountIndicator(version, segment.encodationMode());

		// Make sure the segment fits into the field's bit width and the sum will not overflow.
		if (segment.characters() >= (1u << bitsPerCharacter)
			|| (4u + bitsPerCharacter) > (NumericT<unsigned int>::maxValue() - bitsUsed)
			|| segment.bitBuffer().size() > (NumericT<unsigned int>::maxValue() - bitsUsed))
		{
			return false;
		}

		bitsUsed += 4u + bitsPerCharacter;
		bitsUsed += (unsigned int)segment.bitBuffer().size();
	}

	return true;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
