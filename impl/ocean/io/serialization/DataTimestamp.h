/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_DATA_TIMESTAMP_H
#define META_OCEAN_IO_SERIALIZATION_DATA_TIMESTAMP_H

#include "ocean/io/serialization/Serialization.h"

#include "ocean/io/Bitstream.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

/**
 * This class implements a data timestamp which can hold either a double or int64_t value.
 * @ingroup ioserialization
 */
class OCEAN_IO_SERIALIZATION_EXPORT DataTimestamp
{
	public:

		/**
		 * Definition of the value types.
		 */
		enum ValueType : uint8_t
		{
			/// Invalid value type.
			VT_INVALID = 0u,
			/// Double value type.
			VT_DOUBLE,
			/// Int64 value type.
			VT_INT64
		};

	public:

		/**
		 * Creates a new invalid data timestamp.
		 */
		DataTimestamp() = default;

		/**
		 * Creates a new data timestamp with a double value.
		 * @param value The double value
		 */
		explicit inline DataTimestamp(const double value);

		/**
		 * Creates a new data timestamp with an int64 value.
		 * @param value The int64 value
		 */
		explicit inline DataTimestamp(const int64_t value);

		/**
		 * Returns whether this timestamp holds a double value.
		 * @return True, if so
		 */
		inline bool isDouble() const;

		/**
		 * Returns whether this timestamp holds an int64 value.
		 * @return True, if so
		 */
		inline bool isInt() const;

		/**
		 * Returns the double value of this timestamp.
		 * @return The double value
		 */
		inline double asDouble() const;

		/**
		 * Returns the int64 value of this timestamp.
		 * @return The int64 value
		 */
		inline int64_t asInt() const;

		/**
		 * Returns the timestamp as a double value regardless of the actual value type.
		 * In case the value type is int64, the value will be converted to a double value.
		 * @return The timestamp as a double value
		 */
		inline double forceDouble() const;

		/**
		 * Returns whether this timestamp is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two timestamps are equal.
		 * @param dataTimestamp The second timestamp to compare
		 * @return True, if so
		 */
		bool operator==(const DataTimestamp& dataTimestamp) const;

		/**
		 * Returns whether this timestamp is less than another timestamp.
		 * @param dataTimestamp The second timestamp to compare
		 * @return True, if so
		 */
		bool operator<(const DataTimestamp& dataTimestamp) const;

		/**
		 * Returns whether this timestamp is less than or equal to another timestamp.
		 * @param dataTimestamp The second timestamp to compare
		 * @return True, if so
		 */
		bool operator<=(const DataTimestamp& dataTimestamp) const;

		/**
		 * Returns whether this timestamp is greater than another timestamp.
		 * @param dataTimestamp The second timestamp to compare
		 * @return True, if so
		 */
		bool operator>(const DataTimestamp& dataTimestamp) const;

		/**
		 * Returns whether this timestamp is greater than or equal to another timestamp.
		 * @param dataTimestamp The second timestamp to compare
		 * @return True, if so
		 */
		bool operator>=(const DataTimestamp& dataTimestamp) const;

		/**
		 * Reads a data timestamp from an input bitstream.
		 * @param inputBitstream The input bitstream from which the timestamp will be read
		 * @param dataTimestamp The resulting data timestamp
		 * @return True, if succeeded
		 */
		static bool read(InputBitstream& inputBitstream, DataTimestamp& dataTimestamp);

		/**
		 * Writes a data timestamp to an output bitstream.
		 * @param outputBitstream The output bitstream to which the timestamp will be written
		 * @param dataTimestamp The data timestamp to write
		 * @return True, if succeeded
		 */
		static bool write(OutputBitstream& outputBitstream, const DataTimestamp& dataTimestamp);

	protected:

		/**
		 * The union holding the value.
		 */
		union
		{
			/// The double value.
			double doubleValue_;
			/// The int64 value.
			int64_t intValue_;
		} value_;

		/// The value type.
		ValueType valueType_ = VT_INVALID;
};

inline DataTimestamp::DataTimestamp(const double value) :
	valueType_(VT_DOUBLE)
{
	value_.doubleValue_ = value;
}

inline DataTimestamp::DataTimestamp(const int64_t value) :
	valueType_(VT_INT64)
{
	value_.intValue_ = value;
}

inline bool DataTimestamp::isDouble() const
{
	return valueType_ == VT_DOUBLE;
}

inline bool DataTimestamp::isInt() const
{
	return valueType_ == VT_INT64;
}

inline double DataTimestamp::asDouble() const
{
	ocean_assert(isValid());
	ocean_assert(valueType_ == VT_DOUBLE);

	return value_.doubleValue_;
}

inline int64_t DataTimestamp::asInt() const
{
	ocean_assert(isValid());
	ocean_assert(valueType_ == VT_INT64);

	return value_.intValue_;
}

inline double DataTimestamp::forceDouble() const
{
	ocean_assert(isValid());

	if (valueType_ == VT_DOUBLE)
	{
		return asDouble();
	}
	else
	{
		ocean_assert(valueType_ == VT_INT64);

		return double(asInt());
	}
}

inline bool DataTimestamp::isValid() const
{
	return valueType_ != VT_INVALID;
}

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_DATA_TIMESTAMP_H
