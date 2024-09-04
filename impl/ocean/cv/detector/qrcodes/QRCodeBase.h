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

/**
 * Base class for QR code implementations
 * @ingroup cvdetectorqrcodes
 */
class QRCodeBase
{
	public:

		/**
		 * Definition of available types of codes
		 */
		enum CodeType : unsigned int
		{
			/// Default value of undefined or invalid code types
			CT_UNDEFINED = 0u,
			/// Indicates a standard QR codes
			CT_STANDARD,
			/// Indicates a Micro QR codes
			CT_MICRO,
		};

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
			/// Indicates that the capacity is limited to error detection only (used only by Micro QR Code version M1)
			ECC_DETECTION_ONLY = 5u,
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
		 * Destructs a QR code instance
		 */
		virtual ~QRCodeBase() = default;

		/**
		 * Returns whether this is a valid QR code instance
		 * @return True if this is a valid QR code instance, otherwise false
		 */
		virtual inline bool isValid() const = 0;

		/**
		 * Returns the type or variant of this code.
		 * @returns The code type
		 */
		inline CodeType codeType() const;

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
		 * Returns the version of the QR code as a string
		 * @return The version string
		 */
		virtual inline std::string versionString() const;

		/**
		 * Returns the number of modules per side of the QR code
		 * @return The number of modules per side
		 */
		virtual inline unsigned int modulesPerSide() const = 0;

		/**
		 * Checks if another code is identical to this one.
		 * @param otherCode The code that will be compared against for identity, must be valid.
		 * @param ignoreModules True, identical modules in both codes will not be required, otherwise they will.
		 * @return True if both codes are identical, otherwise false.
		 */
		inline bool isSame(const QRCodeBase& otherCode, const bool ignoreModules) const;

		/**
		 * Checks for equality with another QR code
		 * Two QR codes are equal iff they store identical data, use the same configuration, and have identical modules
		 * @returns True if both QR codes are equal, otherwise false
		 */
		inline bool operator==(const QRCodeBase& otherCode) const;

		/**
		 * Checks for inequality with another QR code
		 * @returns True if both QR codes are not equal, otherwise false
		 */
		inline bool operator!=(const QRCodeBase& otherCode) const;

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
		 * @param errorCorrectionCapacity The error correction capacity that will be converted into an integer
		 * @return The value of the error correction capacity
		 */
		static inline unsigned int getErrorCorrectionCapacityValue(const ErrorCorrectionCapacity errorCorrectionCapacity);

		/**
		 * Translate the code type into a human-readable string
		 * @param codeType The code type that will be translated into a human-readable string
		 * @return The human-readable string
		 */
		static inline std::string translateCodeType(const CodeType codeType);

	protected:

		/**
		 * Creates an invalid QR code instance
		 */
		QRCodeBase() = default;

		/**
		 * Creates an QR code instance
		 * @param codeType The type or variant of this codes, must not be `CT_UNDEFINED`.
		 * @param data The plain data of this QR code, must be valid
		 * @param encodingMode The encoding mode that was used to encode the data, must not be `EM_INVALID_ENCODING_MODE`
		 * @param errorCorrectionCapacity The error correction capacity that was used to generate this QR code, must not be `ECC_INVALID`
		 * @param modules The modules of the QR code that store the data, must be valid
		 * @param version The version of the QR code, range: [1, 40]
		 * @sa QRCodeEncoder, QRCodeDecoder
		 */
		inline explicit QRCodeBase(const CodeType codeType, std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version);

		/**
		 * Copy constructor
		 */
		inline QRCodeBase(const QRCodeBase& otherCode) = default;

		/**
		 * Copy assignment operator
		 */
		inline QRCodeBase& operator=(const QRCodeBase& otherCode) = default;

	protected:

		/// The variant of this QR code
		CodeType codeType_ = CT_UNDEFINED;

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

inline QRCodeBase::QRCodeBase(const CodeType codeType, std::vector<uint8_t>&& data, const EncodingMode encodingMode, const ErrorCorrectionCapacity errorCorrectionCapacity, std::vector<uint8_t>&& modules, const unsigned int version) :
	codeType_(codeType),
	data_(std::move(data)),
	encodingMode_(encodingMode),
	errorCorrectionCapacity_(errorCorrectionCapacity),
	modules_(std::move(modules)),
	version_(version)
{
	ocean_assert(codeType_ != CT_UNDEFINED);
}

inline QRCodeBase::CodeType QRCodeBase::codeType() const
{
	return codeType_;
}

inline const std::vector<uint8_t>& QRCodeBase::data() const
{
	return data_;
}

inline std::string QRCodeBase::dataString() const
{
	return std::string(data_.begin(), data_.end());
}

inline const std::vector<uint8_t>& QRCodeBase::modules() const
{
	return modules_;
}

inline QRCodeBase::EncodingMode QRCodeBase::encodingMode() const
{
	return encodingMode_;
}

inline QRCodeBase::ErrorCorrectionCapacity QRCodeBase::errorCorrectionCapacity() const
{
	return errorCorrectionCapacity_;
}

inline unsigned int QRCodeBase::version() const
{
	return version_;
}

inline std::string QRCodeBase::versionString() const
{
	return std::to_string(version_);
}

inline bool QRCodeBase::isSame(const QRCodeBase& otherCode, const bool ignoreModules) const
{
	if (isValid() != otherCode.isValid())
	{
		return false;
	}

	if (codeType() != otherCode.codeType())
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

inline bool QRCodeBase::operator==(const QRCodeBase& otherCode) const
{
		return isSame(otherCode, /* ignoreModules */ false);
}

inline bool QRCodeBase::operator!=(const QRCodeBase& otherCode) const
{
		return !(*this == otherCode);
}

inline std::string QRCodeBase::translateEncodingMode(const QRCodeBase::EncodingMode encodingMode)
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

inline std::string QRCodeBase::translateErrorCorrectionCapacity(const QRCodeBase::ErrorCorrectionCapacity errorCorrectionCapacity)
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
		
		case ECC_DETECTION_ONLY:
			return "DETECTION_ONLY";
		
		case ECC_INVALID:
			return "INVALID";
	}

	ocean_assert(false && "Never be here!");
	return "Unknown error correction capacity";
}

inline unsigned int QRCodeBase::getErrorCorrectionCapacityValue(const ErrorCorrectionCapacity errorCorrectionCapacity)
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

		case ECC_DETECTION_ONLY:
			return 0u;

		case ECC_INVALID:
			// Handled below
			break;
	}

	ocean_assert(false && "Never be here!");
	return 0u;
}

inline std::string QRCodeBase::translateCodeType(const CodeType codeType)
{
	switch (codeType)
	{
		case CT_UNDEFINED:
			return "UNDEFINED";

		case CT_STANDARD:
			return "STANDARD";

		case CT_MICRO:
			return "MICRO";
	}

	ocean_assert(false && "Never be here!");
	return "";
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
