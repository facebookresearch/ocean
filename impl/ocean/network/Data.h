/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_DATA_H
#define FACEBOOK_NETWORK_DATA_H

#include "ocean/network/Network.h"

namespace Ocean
{

namespace Network
{

/**
 * This class provides function handling different network data.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT Data
{
	public:

		/**
		 * Definition of different order types.
		 */
		enum OrderType : uint32_t
		{
			/// Big endian type.
			TYPE_BIG_ENDIAN,
			/// Little endian type.
			TYPE_LITTLE_ENDIAN,
			/// Readable endian type which is platform specific.
			TYPE_READABLE
		};

	public:

		/**
		 * Returns a value in big-endian order given in local system bit order.<br>
		 * E.g. local system order on Windows platforms is little-endian.
		 * @param value The value to translate
		 * @return Translated value in bit-endian order
		 */
		static inline int toBigEndian(const int32_t value);

		/**
		 * Returns a value in big-endian order given in local system bit order.<br>
		 * E.g. local system order on Windows platforms is little-endian.
		 * @param value The value to translate
		 * @return Translated value in bit-endian order
		 */
		static inline uint32_t toBigEndian(const uint32_t value);

		/**
		 * Returns a value in big-endian order given in local system bit order.<br>
		 * E.g. local system order on Windows platforms is little-endian.
		 * @param value The value to translate
		 * @return Translated value in bit-endian order
		 */
		static inline int16_t toBigEndian(const int16_t value);

		/**
		 * Returns a value in big-endian order given in local system bit order.<br>
		 * E.g. local system order on Windows platforms is little-endian.
		 * @param value The value to translate
		 * @return Translated value in bit-endian order
		 */
		static inline uint16_t toBigEndian(const uint16_t value);

		/**
		 * Returns a value in local system bit order given as big-endian bit order.<br>
		 * E.g. local system bit order on windows platforms is little-endian.
		 * @param value The value to translate
		 * @return Translated value
		 */
		static inline int32_t fromBigEndian(const int32_t value);

		/**
		 * Returns a value in local system bit order given as big-endian bit order.<br>
		 * E.g. local system bit order on windows platforms is little-endian.
		 * @param value The value to translate
		 * @return Translated value
		 */
		static inline uint32_t fromBigEndian(const uint32_t value);

		/**
		 * Returns a value in local system bit order given as big-endian bit order.<br>
		 * E.g. local system bit order on windows platforms is little-endian.<br>
		 * @param value The value to translate
		 * @return Translated value
		 */
		static inline int16_t fromBigEndian(const int16_t value);

		/**
		 * Returns a value in local system bit order given as big-endian bit order.<br>
		 * E.g. local system bit order on windows platforms is little-endian.<br>
		 * @param value The value to translate
		 * @return Translated value
		 */
		static inline uint16_t fromBigEndian(const uint16_t value);
};

inline int Data::toBigEndian(const int32_t value)
{
	return toBigEndian(uint32_t(value));
}

inline uint32_t Data::toBigEndian(const uint32_t value)
{
#ifdef OCEAN_LITTLE_ENDIAN
	return (value >> 24u) | ((value >> 8u) & 0xFF00u) | ((value << 8u) & 0xFF0000u) | (value << 24u);
#else
	return value;
#endif
}

inline int16_t Data::toBigEndian(const int16_t value)
{
	return toBigEndian(uint16_t(value));
}

inline uint16_t Data::toBigEndian(const uint16_t value)
{
#ifdef OCEAN_LITTLE_ENDIAN
	return uint16_t((value >> 8u) | (value << 8u));
#else
	return value;
#endif
}

inline int32_t Data::fromBigEndian(const int32_t value)
{
	return fromBigEndian(uint32_t(value));
}

inline uint32_t Data::fromBigEndian(const uint32_t value)
{
#ifdef OCEAN_LITTLE_ENDIAN
	return (value >> 24u) | ((value >> 8u) & 0xFF00u) | ((value << 8u) & 0xFF0000u) | (value << 24u);
#else
	return value;
#endif
}

inline int16_t Data::fromBigEndian(const int16_t value)
{
	return fromBigEndian(uint16_t(value));
}

inline uint16_t Data::fromBigEndian(const uint16_t value)
{
#ifdef OCEAN_LITTLE_ENDIAN
	return uint16_t((value >> 8u) | (value << 8u));
#else
	return value;
#endif
}

}

}

#endif // FACEBOOK_NETWORK_DATA_H
