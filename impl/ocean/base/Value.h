/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_VALUE_H
#define META_OCEAN_BASE_VALUE_H

#include "ocean/base/Base.h"

#include <string>

namespace Ocean
{

/**
 * This class implements a type independent value.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Value
{
	public:

		/**
		 * Definition of different internal value type.
		 */
		enum ValueType
		{
			/// Invalid value type.
			VT_INVALID,
			/// Boolean value type.
			VT_BOOL,
			/// 32 bit integer value.
			VT_INT_32,
			/// 64 bit integer value.
			VT_INT_64,
			/// Single precision floating point value.
			VT_FLOAT_32,
			/// Double precision floating point value.
			VT_FLOAT_64,
			/// String value.
			VT_STRING,
			/// Buffer value.
			VT_BUFFER
		};

	protected:

		/**
		 * Definition of a struct combining memory pointer and size.
		 */
		struct BufferStruct
		{
			/// Pointer of the buffer.
			void* buffer_;

			/// The size of the buffer, in bytes.
			size_t size_;
		};

	public:

		/**
		 * Creates an empty value.
		 */
		inline Value() noexcept;

		/**
		 * Move constructor.
		 * @param value the value object to move
		 */
		Value(Value&& value);

		/**
		 * Copies a value object.
		 * @param value The value object to copy
		 */
		Value(const Value& value);

		/**
		 * Creates a new value object with an boolean value as internal data type.
		 * @param value Internal value
		 */
		explicit Value(const bool value);

		/**
		 * Creates a new value object with an integer value as internal data type.
		 * @param value Internal value
		 */
		explicit Value(const int32_t value);

		/**
		 * Creates a new value object with an 64 bit integer value as internal data type.
		 * @param value Internal value
		 */
		explicit Value(const int64_t value);

		/**
		 * Creates a new value object with a floating point value as internal data type.
		 * @param value Internal value
		 */
		explicit Value(const float value);

		/**
		 * Creates a new value object with a double precision floating point value as internal data type.
		 * @param value Internal value
		 */
		explicit Value(const double value);

		/**
		 * Creates a new value object with a string value as internal data type.
		 * @param value Internal value
		 */
		explicit Value(const char* value);

		/**
		 * Creates a new value object with a string value as internal data type.
		 * @param value Internal value
		 */
		explicit Value(const std::string& value);

		/**
		 * Creates a new value object with an arbitrary buffer as internal data type.
		 * @param value Internal value, can be nullptr if size == 0
		 * @param size The size of the buffer in bytes, with range [0, infinity)
		 */
		explicit Value(const void* value, const size_t size);

		/**
		 * Destructs a value object.
		 */
		~Value();

		/**
		 * Returns the internal data type of this value object.
		 * @return Internal data type.
		 */
		inline ValueType type() const;

		/**
		 * Returns whether the internal data type is equivalent to a given one.
		 * @param valueType Internal data type to check
		 * @return True, if so
		 */
		inline bool isType(const ValueType valueType) const;

		/**
		 * Returns whether this object holds an boolean value as internal data.
		 * @return True, if so
		 */
		inline bool isBool() const;

		/**
		 * Returns whether this object holds an integer value as internal data.
		 * @return True, if so
		 */
		inline bool isInt() const;

		/**
		 * Returns whether this object holds a 64 bit integer value as internal data.
		 * @return True, if so
		 */
		inline bool isInt64() const;

		/**
		 * Returns whether this object holds a float value as internal data.
		 * @return True, if so
		 */
		inline bool isFloat() const;

		/**
		 * Returns whether this object holds a 64 bit float value as internal data.
		 * @param allowIntAndFloat True, to allow int, int64, float and float64 values; False, to force float64 value only
		 * @return True, if so
		 */
		inline bool isFloat64(const bool allowIntAndFloat = false) const;

		/**
		 * Returns whether this object holds a string value as internal data.
		 * @return True, if so
		 */
		inline bool isString() const;

		/**
		 * Returns whether this object holds a buffer value as internal data.
		 * @return True, if so
		 */
		inline bool isBuffer() const;

		/**
		 * Returns whether this object does not holds any internal data.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns the internal value as a boolean.
		 * Beware: Check the internal type of this value object before!
		 * @return Internal data as boolean
		 */
		bool boolValue() const;

		/**
		 * Returns the internal value as integer.
		 * Beware: Check the internal type of this value object before!
		 * @return Internal data as integer
		 */
		int32_t intValue() const;

		/**
		 * Returns the internal value as 64 bit integer.
		 * Beware: Check the internal type of this value object before!
		 * @return Internal data as 64 bit integer
		 */
		int64_t int64Value() const;

		/**
		 * Returns the internal value as float.
		 * Beware: Check the internal type of this value object before!
		 * @return Internal data as float
		 */
		float floatValue() const;

		/**
		 * Returns the internal value as 64 bit float.
		 * Beware: Check the internal type of this value object before!
		 * @param allowIntAndFloat True, to allow int, int64, float and float64 values; False, to force float64 value only
		 * @return Internal data as 64 bit float
		 */
		double float64Value(const bool allowIntAndFloat = false) const;

		/**
		 * Returns the internal value as string.
		 * Beware: Check the internal type of this value object before!
		 * @return Internal data as string
		 */
		std::string stringValue() const;

		/**
		 * Returns the internal value as buffer.
		 * Beware: Check the internal type of this value object before!
		 * @param size The size of the buffer in bytes, with range [0, infinity)
		 * @return The pointer to the buffer value
		 */
		const void* bufferValue(size_t& size) const;

		/**
		 * Returns the internal value as a readable string not considering the type of the value.
		 * Thus, this function can be called for any value type.
		 * @param floatingPointPrecision The number of decimal places for floating point numbers, with range [1, infinity)
		 * @return The readable string of this value
		 */
		std::string readableString(const unsigned int floatingPointPrecision = 2u) const;

		/**
		 * Move operator.
		 * @param value The value object to be moved
		 * @return Reference to this object
		 */
		Value& operator=(Value&& value);

		/**
		 * Assigns a value object to this one.
		 * @param value The value object to assign
		 * @return Reference to this object
		 */
		Value& operator=(const Value& value);

		/**
		 * Returns whether two value objects are identical (having same type and holding the same value).
		 * @param value The second value to compare
		 * @return True, if so
		 */
		bool operator==(const Value& value) const;

		/**
		 * Returns whether two value objects are not identical (not having same type or not holding the same value).
		 * @param value The second value to compare
		 * @return True, if so
		 */
		inline bool operator!=(const Value& value) const;

		/**
		 * Returns whether this object holds any internal data.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Writes a Value object to a buffer.
		 * @param value The value to write to the buffer, must be valid
		 * @param buffer The buffer to which the value will be written, will be resized as needed
		 * @param offsetInBuffer The offset within the buffer at which position the value information will be written, with range [0, infinity)
		 * @return True, if succeeded
		 * @see readFromBuffer().
		 */
		static bool writeToBuffer(const Value& value, std::vector<uint8_t>& buffer, const size_t offsetInBuffer = 0);

		/**
		 * Reads a Value object from a buffer.
		 * @param buffer The pointer to the input buffer, must be valid
		 * @param size The size of the input buffer in bytes, with range [1, infinity)
		 * @return The resulting Value object, invalid if the input was invalid
		 * @see writeToBuffer().
		 */
		static Value readFromBuffer(const void* buffer, const size_t size);

	protected:

		/**
		 * Releases the value data.
		 */
		void release();

	protected:

		/// Internal value type.
		ValueType valueType_ = VT_INVALID;

		/**
		 * Definition of a union holding all possible values.
		 */
		union ValueUnion
		{
			/// Possible boolean value.
			bool valueBool_;

			/// Possible int value.
			int32_t valueInt_;

			/// Possible int 64 value.
			int64_t valueInt64_;

			/// Possible floating point value.
			float valueFloat_;

			/// Possible double precision floating point value.
			double valueFloat64_;

			/// Possible buffer value.
			BufferStruct bufferStruct_;
		} valueUnion_;
};

inline Value::Value() noexcept :
	valueType_(VT_INVALID)
{
	// nothing to do here
}

inline Value::ValueType Value::type() const
{
	return valueType_;
}

inline bool Value::isType(const ValueType valueType) const
{
	return valueType_ == valueType;
}

inline bool Value::isBool() const
{
	return valueType_ == VT_BOOL;
}

inline bool Value::isInt() const
{
	return valueType_ == VT_INT_32;
}

inline bool Value::isInt64() const
{
	return valueType_ == VT_INT_64;
}

inline bool Value::isFloat() const
{
	return valueType_ == VT_FLOAT_32;
}

inline bool Value::isFloat64(const bool allowIntAndFloat) const
{
	if (allowIntAndFloat)
	{
		return valueType_ == VT_FLOAT_64 || valueType_ == VT_FLOAT_32 || valueType_ == VT_INT_64 || valueType_ == VT_INT_32;
	}
	else
	{
		return valueType_ == VT_FLOAT_64;
	}
}

inline bool Value::isString() const
{
	return valueType_ == VT_STRING;
}

inline bool Value::isBuffer() const
{
	return valueType_ == VT_BUFFER;
}

inline bool Value::isNull() const
{
	return valueType_ == VT_INVALID;
}

inline bool Value::operator!=(const Value& value) const
{
	return !(*this == value);
}

inline Value::operator bool() const
{
	return valueType_ != VT_INVALID;
}

}

#endif // META_OCEAN_BASE_VALUE_H
