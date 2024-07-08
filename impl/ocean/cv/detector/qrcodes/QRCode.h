/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include <algorithm>
#include <cstdint>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/// Forward declaration
class QRCode;

/// Definition of a vector of QR codes
typedef std::vector<QRCode> QRCodes;

/**
 * Definition of a QR code
 * @ingroup cvdetectorqrcodes
 */
class QRCode
{
	friend class QRCodeEncoder;
	friend class QRCodeDecoder;

	public:

		/**
		 * Enumeration of the levels of error correction
		 * The value of the enums correspond to the standard-defined bit sequences (2 bits long)
		 */
		enum ErrorCorrectionCapacity : uint32_t
		{
			/// Indicates that 7% of the modules reserved error correction
			ECC_07 = 0b01u,
			/// Indicates that 15% of the modules reserved error correction
			ECC_15 = 0b00u,
			/// Indicates that 25% of the modules reserved error correction
			ECC_25 = 0b11u,
			/// Indicates that 30% of the modules reserved error correction
			ECC_30 = 0b10u,
			/// Indicator for an invalid error correction capacity
			ECC_INVALID = uint32_t(-1),
		};

		/**
		 * Definition of encoding modes
		 */
		enum EncodingMode : uint32_t
		{
			/// Mode that supports digits 0-9
			EM_NUMERIC = 0u,
			/// Mode that supports A-Z, 0-9 and a few others, cf. `ALPHANUMERIC_CHARSET`
			EM_ALPHANUMERIC,
			/// Mode that represents data as a sequence of bytes
			EM_BYTE,

			// Modes below this line are currently not supported

			/// Mode that handles Kanji characters in accordance with the Shift JIS system based on JIS X 0208
			EM_KANJI,
			/// Mode that allows the output data stream to have interpretations different from that of the default character set.
			EM_ECI,
			/// Structured Append mode is used to split the encodation of the data from a message over a number of QR Code symbols.
			EM_STRUCTURED_APPEND,
			/// Mode that is used for messages containing data formatted either in accordance with the UCC/EAN Application Identifiers standard
			EM_FNC1,

			/// Indicator for an invalid encoding mode
			EM_INVALID_ENCODING_MODE = uint32_t(-1),
		};

	public:

		/**
		 * Creates an invalid QR code instance
		 */
		QRCode() = default;

		/**
		 * Returns whether this is a valid QR code instance
		 * @return True if this is a valid QR code instance, otherwise false
		 */
		inline bool isValid() const;

		/**
		 * Returns a constant reference to the plain data of the QR code
		 * @return The constant reference
		 * @sa dataAlphanumeric()
		 */
		inline const std::vector<uint8_t>& data() const;

		/**
		 * Return the plain data of the QR code as a string, if possible
		 * @return The data as string if the encoding mode is `EM_NUMERIC` or `EM_ALPHANUMERIC`, otherwise an empty string is returned
		 * @sa data()
		 */
		inline std::string dataString() const;

		/**
		 * Returns the modules of this QR code
		 * The modules are stored in a vector and will have `modulesPerSide() * modulesPerSide()` elements (row-wise order)
		 * @return The vector modules
		 */
		inline const std::vector<uint8_t>& modules() const;

		/**
		 * Returns the encoding mode of the QR code
		 * @return The encoding mode
		 */
		inline EncodingMode encodingMode() const;

		/**
		 * Returns the error correction capacity of the QR code
		 * @return The error correction capacity
		 */
		inline ErrorCorrectionCapacity errorCorrectionCapacity() const;

		/**
		 * Returns the version of the QR code
		 * @return The version number
		 */
		inline unsigned int version() const;

		/**
		 * Returns the number of modules per side of the QR code
		 * @return The number of modules per side
		 */
		inline unsigned int modulesPerSide() const;

		/**
		 * Computes the number of modules per side of a QR code given its version
		 * @param version The version number for which the number of modules per side will be computed, range: [1, 40]
		 * @return The number of modules per side
		 */
		static inline unsigned int modulesPerSide(const unsigned int version);

		/**
		 * Checks if another code is identical to this one.
		 * @param otherCode The code that will be compared against for identity, must be valid.
		 * @param ignoreModules True, identical modules in both codes will not be required, otherwise they will.
		 * @return True if both codes are identical, otherwise false.
		 */
		inline bool isSame(const QRCode& otherCode, const bool ignoreModules) const;

		/**
		 * Checks for equality with another QR code
		 * Two QR codes are equal iff they store identical data, use the same configuration, and have identical modules
		 * @returns True if both QR codes are equal, otherwise false
		 */
		inline bool operator==(const QRCode& otherCode) const;

		/**
		 * Checks for inequality with another QR code
		 * @returns True if both QR codes are not equal, otherwise false
		 */
		inline bool operator!=(const QRCode& otherCode) const;

		/**
		 * Translates an encoding mode enum into a human-readable string
		 * @param encodingMode The encoding mode that will be converted into a string
		 * @return The translated encoding mode
		 */
		static inline std::string translateEncodingMode(const EncodingMode encodingMode);

		/**
		 * Translates an error correction capacity enum into a human-readable string
		 * @param errorCorrectionCapacity The error correction capacity that will be converted into a string
		 * @return The translated error correction capacity
		 */
		static inline std::string translateErrorCorrectionCapacity(const ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Returns the integer value of an error correction capacity
		 * @param errorCorrectionCapacity The error correction capacity that will be converted into a string
		 * @return The value of the error correction capacity
		 */
		static inline unsigned int getErrorCorrectionCapacityValue(const ErrorCorrectionCapacity errorCorrectionCapacity);

	protected:

		/**
		 * Creates an QR code instance
		 * @param data The plain data of this QR code, must be valid
		 * @param encodingMode The encoding mode that was used to encode the data, must not be `EM_INVALID_ENCODING_MODE`
		 * @param errorCorrectionCapacity The error correction capacity that was used to generate this QR code, must not be `ECC_INVALID`
		 * @param modules The modules of the QR code that store the data, must be valid
		 * @param version The version of the QR code, range: [1, 40]
		 * @sa QRCodeEncoder, QRCodeDecoder
		 */
		inline explicit QRCode(std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version);

	protected:

		/// The plain data
		std::vector<uint8_t> data_;

		/// The encoding mode that was used to generate this QR code
		EncodingMode encodingMode_ = EM_INVALID_ENCODING_MODE;

		/// The error correction capacity that was used to generated this QR code
		ErrorCorrectionCapacity errorCorrectionCapacity_ = ECC_INVALID;

		/// The modules of the QR code that store the data
		std::vector<uint8_t> modules_;

		/// The version of the QR code
		unsigned int version_ = 0u;
};

inline QRCode::QRCode(std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version) :
	data_(std::move(data)),
	encodingMode_(encodingMode),
	errorCorrectionCapacity_(errorCorrectionCapacity),
	modules_(std::move(modules)),
	version_(version)
{
	ocean_assert(isValid());
}

inline bool QRCode::isValid() const
{
	if (encodingMode_ == EM_INVALID_ENCODING_MODE || errorCorrectionCapacity_ == ECC_INVALID)
	{
		return false;
	}

	if (data_.empty() || modules_.empty())
	{
		return false;
	}

	if (version_ < 1u || version_ > 40u)
	{
		return false;
	}

	if (modules_.size() != modulesPerSide() * modulesPerSide())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

inline const std::vector<uint8_t>& QRCode::data() const
{
	return data_;
}

inline std::string QRCode::dataString() const
{
	return std::string(data_.begin(), data_.end());
}

inline const std::vector<uint8_t>& QRCode::modules() const
{
	return modules_;
}

inline QRCode::EncodingMode QRCode::encodingMode() const
{
	return encodingMode_;
}

inline QRCode::ErrorCorrectionCapacity QRCode::errorCorrectionCapacity() const
{
	return errorCorrectionCapacity_;
}

inline unsigned int QRCode::version() const
{
	return version_;
}

inline unsigned int QRCode::modulesPerSide() const
{
	return modulesPerSide(version());
}

inline unsigned int QRCode::modulesPerSide(const unsigned int version)
{
	if (version >= 1u && version <= 40u)
	{
		return 4u * version + 17u;
	}

	return 0u;
}

inline bool QRCode::isSame(const QRCode& otherCode, const bool ignoreModules) const
{
	if (isValid() != otherCode.isValid())
	{
		return false;
	}

	if (version() != otherCode.version())
	{
		return false;
	}

	if (encodingMode() != otherCode.encodingMode())
	{
		return false;
	}

	if (errorCorrectionCapacity() != otherCode.errorCorrectionCapacity())
	{
		return false;
	}

	if (modulesPerSide() != otherCode.modulesPerSide())
	{
		return false;
	}

	if (data_.size() != otherCode.data().size())
	{
		return false;
	}
	else if (!std::equal(data_.begin(), data_.end(), otherCode.data().begin()))
	{
		return false;
	}

	if (!ignoreModules)
	{
		if (modules_.size() != otherCode.modules().size())
		{
			return false;
		}
		else if (!std::equal(modules_.begin(), modules_.end(), otherCode.modules().begin()))
		{
			return false;
		}
	}

	return true;
}

inline bool QRCode::operator==(const QRCode& otherCode) const
{
		return isSame(otherCode, /* ignoreModules */ false);
}

inline bool QRCode::operator!=(const QRCode& otherCode) const
{
		return !(*this == otherCode);
}

inline std::string QRCode::translateEncodingMode(const QRCode::EncodingMode encodingMode)
{
	switch (encodingMode)
	{
		case EM_NUMERIC:
			return "NUMERIC";

		case EM_ALPHANUMERIC:
			return "ALPHANUMERIC";

		case EM_BYTE:
			return "BYTE";

		case EM_KANJI:
			return "KANJI";

		case EM_ECI:
			return "ECI";

		case EM_STRUCTURED_APPEND:
			return "STRUCTURED_APPEND";

		case EM_FNC1:
			return "FNC1";

		case EM_INVALID_ENCODING_MODE:
			return "INVALID";
	}

	ocean_assert(false && "Never be here!");
	return "Unknown encoding mode";
}

inline std::string QRCode::translateErrorCorrectionCapacity(const QRCode::ErrorCorrectionCapacity errorCorrectionCapacity)
{
	switch (errorCorrectionCapacity)
	{
		case ECC_07:
			return "07";

		case ECC_15:
			return "15";

		case ECC_25:
			return "25";

		case ECC_30:
			return "30";

		case ECC_INVALID:
			return "INVALID";
	}

	ocean_assert(false && "Never be here!");
	return "Unknown error correction capacity";
}

inline unsigned int QRCode::getErrorCorrectionCapacityValue(const ErrorCorrectionCapacity errorCorrectionCapacity)
{
	switch (errorCorrectionCapacity)
	{
		case ECC_07:
			return 7u;

		case ECC_15:
			return 15u;

		case ECC_25:
			return 25u;

		case ECC_30:
			return 30u;

		case ECC_INVALID:
			// Handled below
			break;
	}

	ocean_assert(false && "Never be here!");
	return 0u;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
