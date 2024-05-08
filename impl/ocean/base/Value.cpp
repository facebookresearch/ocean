/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Value.h"
#include "ocean/base/String.h"

namespace Ocean
{

Value::Value(const Value& value)
{
	*this = value;
}

Value::Value(Value&& value)
{
	*this = std::move(value);
}

Value::Value(const bool value) :
	valueType_(VT_BOOL)
{
	valueUnion_.valueBool_ = value;
}

Value::Value(const int32_t value) :
	valueType_(VT_INT_32)
{
	valueUnion_.valueInt_ = value;
}

Value::Value(const int64_t value) :
	valueType_(VT_INT_64)
{
	valueUnion_.valueInt64_ = value;
}

Value::Value(const float value) :
	valueType_(VT_FLOAT_32)
{
	valueUnion_.valueFloat_ = value;
}

Value::Value(const double value) :
	valueType_(VT_FLOAT_64)
{
	valueUnion_.valueFloat64_ = value;
}

Value::Value(const char* value)
{
	if (value != nullptr)
	{
		*this = Value(std::string(value));
	}
}

Value::Value(const std::string& value) :
	valueType_(VT_STRING)
{
	valueUnion_.bufferStruct_.buffer_ = nullptr;
	valueUnion_.bufferStruct_.size_ = 0;

	if (value.length() != 0)
	{
		valueUnion_.bufferStruct_.buffer_ = malloc(value.length() + 1);

		if (valueUnion_.bufferStruct_.buffer_ == nullptr)
		{
			valueType_ = VT_INVALID;
		}
		else
		{
			memcpy(valueUnion_.bufferStruct_.buffer_, value.c_str(), value.length());

			((uint8_t*)(valueUnion_.bufferStruct_.buffer_))[value.length()] = '\0';
			valueUnion_.bufferStruct_.size_ = value.length() + 1;
		}
	}
}

Value::Value(const void* value, const size_t size) :
	valueType_(VT_BUFFER)
{
	valueUnion_.bufferStruct_.buffer_ = nullptr;
	valueUnion_.bufferStruct_.size_ = 0;

	if (value != nullptr && size != 0)
	{
		valueUnion_.bufferStruct_.buffer_ = malloc(size);

		if (valueUnion_.bufferStruct_.buffer_ == nullptr)
		{
			valueType_ = VT_INVALID;
		}
		else
		{
			memcpy(valueUnion_.bufferStruct_.buffer_, value, size);
			valueUnion_.bufferStruct_.size_ = size;
		}
	}
}

Value::~Value()
{
	release();
}

bool Value::boolValue() const
{
	ocean_assert(valueType_ == VT_BOOL);
	if (valueType_ != VT_BOOL)
	{
		return false;
	}

	return valueUnion_.valueBool_;
}

int32_t Value::intValue() const
{
	ocean_assert(valueType_ == VT_INT_32);
	if (valueType_ != VT_INT_32)
	{
		return 0;
	}

	return valueUnion_.valueInt_;
}

int64_t Value::int64Value() const
{
	ocean_assert(valueType_ == VT_INT_64);
	if (valueType_ != VT_INT_64)
	{
		return 0ll;
	}

	return valueUnion_.valueInt64_;
}

float Value::floatValue() const
{
	ocean_assert(valueType_ == VT_FLOAT_32);
	if (valueType_ != VT_FLOAT_32)
	{
		return 0.0f;
	}

	return valueUnion_.valueFloat_;
}

double Value::float64Value(const bool allowIntAndFloat) const
{
	if (allowIntAndFloat)
	{
		switch (valueType_)
		{
			case VT_FLOAT_64:
				return valueUnion_.valueFloat64_;

			case VT_FLOAT_32:
				return double(valueUnion_.valueFloat_);

			case VT_INT_64:
				return double(valueUnion_.valueInt64_);

			case VT_INT_32:
				return double(valueUnion_.valueInt_);

			default:
				break;
		}

		ocean_assert(false && "Invalid type!");
		return 0.0;
	}
	else
	{
		ocean_assert(valueType_ == VT_FLOAT_64);
		if (valueType_ != VT_FLOAT_64)
		{
			return 0.0;
		}

		return valueUnion_.valueFloat64_;
	}
}

std::string Value::stringValue() const
{
	ocean_assert(valueType_ == VT_STRING);
	if (valueType_ != VT_STRING)
	{
		return std::string();
	}

	if (valueUnion_.bufferStruct_.size_ == 0)
	{
		return std::string();
	}

	ocean_assert(valueUnion_.bufferStruct_.size_ > 1);
	ocean_assert(valueUnion_.bufferStruct_.buffer_ != nullptr);

	std::string result(valueUnion_.bufferStruct_.size_ - 1, ' ');
	memcpy(&result[0], valueUnion_.bufferStruct_.buffer_, valueUnion_.bufferStruct_.size_ - 1);

	return result;
}

const void* Value::bufferValue(size_t& size) const
{
	ocean_assert(valueType_ == VT_BUFFER);
	if (valueType_ != VT_BUFFER)
	{
		size = 0;
		return nullptr;
	}

	size = valueUnion_.bufferStruct_.size_;
	return valueUnion_.bufferStruct_.buffer_;
}

std::string Value::readableString(const unsigned int floatingPointPrecision) const
{
	switch (valueType_)
	{
		case VT_BOOL:
			return valueUnion_.valueBool_ ? "true" : "false";

		case VT_INT_32:
			return String::toAString(valueUnion_.valueInt_);

		case VT_INT_64:
			return String::toAString(valueUnion_.valueInt64_);

		case VT_FLOAT_32:
			return String::toAString(valueUnion_.valueFloat_, floatingPointPrecision);

		case VT_FLOAT_64:
			return String::toAString(valueUnion_.valueFloat64_, floatingPointPrecision);

		case VT_STRING:
			return stringValue();

		default:
			break;
	}

	ocean_assert(false && "Invalid value!");
	return std::string();
}

Value& Value::operator=(Value&& value)
{
	if (this != &value)
	{
		release();

		valueType_ = value.valueType_;
		value.valueType_ = VT_INVALID;

		if (valueType_ == VT_STRING || valueType_ == VT_BUFFER)
		{
			valueUnion_.bufferStruct_.buffer_ = value.valueUnion_.bufferStruct_.buffer_;
			valueUnion_.bufferStruct_.size_ = value.valueUnion_.bufferStruct_.size_;

			value.valueUnion_.bufferStruct_.buffer_ = nullptr;
			value.valueUnion_.bufferStruct_.size_ = 0;
		}
		else
		{
			memcpy(&valueUnion_, &value.valueUnion_, sizeof(ValueUnion));
		}
	}

	return *this;
}

Value& Value::operator=(const Value& value)
{
	if (this != &value)
	{
		if (valueType_ == VT_STRING || valueType_ == VT_BUFFER)
		{
			if ((value.valueType_ == VT_STRING || value.valueType_ == VT_BUFFER) && valueUnion_.bufferStruct_.size_ == value.valueUnion_.bufferStruct_.size_)
			{
				// we hold an identical buffer, so we can simply copy the memory

				valueType_ = value.valueType_;

				if (value.valueUnion_.bufferStruct_.size_ != 0)
				{
					memcpy(valueUnion_.bufferStruct_.buffer_, value.valueUnion_.bufferStruct_.buffer_, value.valueUnion_.bufferStruct_.size_);
				}
				else
				{
					ocean_assert(valueUnion_.bufferStruct_.buffer_ == nullptr);
					ocean_assert(valueUnion_.bufferStruct_.size_ == 0);
				}

				return *this;
			}
		}

		release();

		valueType_ = value.valueType_;

		if (valueType_ == VT_STRING || valueType_ == VT_BUFFER)
		{
			if (value.valueUnion_.bufferStruct_.size_ != 0)
			{
				valueUnion_.bufferStruct_.buffer_ = malloc(value.valueUnion_.bufferStruct_.size_);

				if (valueUnion_.bufferStruct_.buffer_ != nullptr)
				{
					memcpy(valueUnion_.bufferStruct_.buffer_, value.valueUnion_.bufferStruct_.buffer_, value.valueUnion_.bufferStruct_.size_);
					valueUnion_.bufferStruct_.size_ = value.valueUnion_.bufferStruct_.size_;
				}
			}
			else
			{
				valueUnion_.bufferStruct_.buffer_ = nullptr;
				valueUnion_.bufferStruct_.size_ = 0;
			}
		}
		else
		{
			memcpy(&valueUnion_, &value.valueUnion_, sizeof(ValueUnion));
		}
	}

	return *this;
}

bool Value::operator==(const Value& value) const
{
	if (type() != value.type())
	{
		return false;
	}

	switch (valueType_)
	{
		case VT_INVALID:
		{
			// both values are invalid, so both objects are identical
			return true;
		}

		case VT_BOOL:
		{
			return valueUnion_.valueBool_ == value.valueUnion_.valueBool_;
		}

		case VT_INT_32:
		{
			return valueUnion_.valueInt_ == value.valueUnion_.valueInt_;
		}

		case VT_INT_64:
		{
			return valueUnion_.valueInt64_ == value.valueUnion_.valueInt64_;
		}

		case VT_FLOAT_32:
		{
			return valueUnion_.valueFloat_ == value.valueUnion_.valueFloat_;
		}

		case VT_FLOAT_64:
		{
			return valueUnion_.valueFloat64_ == value.valueUnion_.valueFloat64_;
		}

		case VT_STRING:
		case VT_BUFFER:
		{
			if (valueUnion_.bufferStruct_.size_ != value.valueUnion_.bufferStruct_.size_)
			{
				return false;
			}

			if (valueUnion_.bufferStruct_.size_ == 0)
			{
				return true;
			}

			return memcmp(valueUnion_.bufferStruct_.buffer_, value.valueUnion_.bufferStruct_.buffer_, valueUnion_.bufferStruct_.size_) == 0;
		}
	}

	ocean_assert(false && "This should never happen!");
	return false;
}

bool Value::writeToBuffer(const Value& value, std::vector<uint8_t>& buffer, const size_t offsetInBuffer)
{
#ifndef OCEAN_LITTLE_ENDIAN
	static_assert(false, "We need little endian");
	return false;
#endif

	switch (value.type())
	{
		case VT_INVALID:
		{
			return false;
		}

		case VT_BOOL:
		{
			buffer.resize(offsetInBuffer + 2);

			buffer[offsetInBuffer] = uint8_t('B');
			buffer[offsetInBuffer + 1] = value.boolValue() ? 1u : 0u;

			return true;
		}

		case VT_INT_32:
		{
			static_assert(sizeof(int32_t) == 4, "Invalid data type!");

			buffer.resize(offsetInBuffer + 5);

			const int32_t intValue = value.intValue();

			buffer[offsetInBuffer] = uint8_t('i');
			memcpy(buffer.data() + offsetInBuffer + 1, &intValue, sizeof(intValue));

			return true;
		}

		case VT_INT_64:
		{
			static_assert(sizeof(int64_t) == 8, "Invalid data type!");

			buffer.resize(offsetInBuffer + 9);

			const int64_t intValue = value.int64Value();

			buffer[offsetInBuffer] = uint8_t('I');
			memcpy(buffer.data() + offsetInBuffer + 1, &intValue, sizeof(intValue));

			return true;
		}

		case VT_FLOAT_32:
		{
			static_assert(sizeof(float) == 4, "Invalid data type!");

			buffer.resize(offsetInBuffer + 5);

			const float floatValue = value.floatValue();

			buffer[offsetInBuffer] = uint8_t('f');
			memcpy(buffer.data() + offsetInBuffer + 1, &floatValue, sizeof(floatValue));

			return true;
		}

		case VT_FLOAT_64:
		{
			static_assert(sizeof(double) == 8, "Invalid data type!");

			buffer.resize(offsetInBuffer + 9);

			const double doubleValue = value.float64Value();

			buffer[offsetInBuffer] = uint8_t('F');
			memcpy(buffer.data() + offsetInBuffer + 1, &doubleValue, sizeof(doubleValue));

			return true;
		}

		case VT_STRING:
		{
			const std::string stringValue = value.stringValue();

			buffer.resize(offsetInBuffer + 1 + stringValue.size() + 1); // including zero character

			buffer[offsetInBuffer] = uint8_t('S');

			if (stringValue.empty())
			{
				buffer[offsetInBuffer + 1] = uint8_t('\0');
			}
			else
			{
				ocean_assert(stringValue.c_str()[stringValue.size()] == '\0');

				memcpy(buffer.data() + offsetInBuffer + 1, stringValue.c_str(), stringValue.size() + 1);
			}

			return true;
		}

		case VT_BUFFER:
		{
			size_t bufferSize = 0;
			const void* bufferValue = value.bufferValue(bufferSize);

			buffer.resize(offsetInBuffer + 1 + bufferSize);

			buffer[offsetInBuffer] = uint8_t('R');

			if (bufferSize != 0)
			{
				memcpy(buffer.data() + offsetInBuffer + 1, bufferValue, bufferSize);
			}

			return true;
		}
	}

	ocean_assert(false && "Invalid value type!");
	return false;
}

Value Value::readFromBuffer(const void* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr && size >= 1);

#ifndef OCEAN_LITTLE_ENDIAN
	static_assert(false, "We need little endian");
	return Value();
#endif

	const uint8_t* buffer8 = (const uint8_t*)(buffer);

	switch(buffer8[0])
	{
		// VT_BOOL
		case uint8_t('B'):
		{
			if (size == 2)
			{
				const bool boolValue = buffer8[1] != 0u;

				return Value(boolValue);
			}

			break;
		}

		// VT_INT_32
		case uint8_t('i'):
		{
			if (size == 5)
			{
				static_assert(sizeof(int32_t) == 4, "Invalid data type!");

				int32_t intValue;
				memcpy(&intValue, buffer8 + 1, sizeof(uint32_t));

				return Value(intValue);
			}

			break;
		}

		// VT_INT_64
		case uint8_t('I'):
		{
			if (size == 9)
			{
				static_assert(sizeof(int64_t) == 8, "Invalid data type!");

				int64_t intValue;
				memcpy(&intValue, buffer8 + 1, sizeof(uint64_t));

				return Value(intValue);
			}

			break;
		}

		// VT_FLOAT_32
		case uint8_t('f'):
		{
			if (size == 5)
			{
				static_assert(sizeof(float) == 4, "Invalid data type!");

				float floatValue;
				memcpy(&floatValue, buffer8 + 1, sizeof(float));

				return Value(floatValue);
			}

			break;
		}

		// VT_FLOAT_64
		case uint8_t('F'):
		{
			if (size == 9)
			{
				static_assert(sizeof(double) == 8, "Invalid data type!");

				double doubleValue;
				memcpy(&doubleValue, buffer8 + 1, sizeof(double));

				return Value(doubleValue);
			}

			break;
		}

		// VT_STRING
		case uint8_t('S'):
		{
			if (size >= 2 && buffer8[size - 1] == uint8_t('\0'))
			{
				return Value((const char*)(buffer8 + 1));
			}

			break;
		}

		// VT_BUFFER
		case uint8_t('R'):
		{
			if (size > 1)
			{
				return Value(buffer8 + 1, size - 1);
			}

			return Value(nullptr, 0);
		}
	}

	ocean_assert(false && "Invalid value");
	return Value();
}

void Value::release()
{
	if (valueType_ == VT_STRING || valueType_ == VT_BUFFER)
	{
		free(valueUnion_.bufferStruct_.buffer_);

		valueUnion_.bufferStruct_.buffer_ = nullptr;
		valueUnion_.bufferStruct_.size_ = 0;
	}

	valueType_ = VT_INVALID;
}

}
