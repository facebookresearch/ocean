/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/serialization/DataTimestamp.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

bool DataTimestamp::operator==(const DataTimestamp& dataTimestamp) const
{
	if (valueType_ != dataTimestamp.valueType_)
	{
		return false;
	}

	if (valueType_ == VT_DOUBLE)
	{
		return value_.doubleValue_ == dataTimestamp.value_.doubleValue_;
	}
	else if (valueType_ == VT_INT64)
	{
		return value_.intValue_ == dataTimestamp.value_.intValue_;
	}

	return true;
}

bool DataTimestamp::operator<(const DataTimestamp& dataTimestamp) const
{
	ocean_assert(isValid() && dataTimestamp.isValid());

	if (valueType_ != dataTimestamp.valueType_)
	{
		ocean_assert(false && "Not matching value types!");
		return false;
	}

	if (valueType_ == VT_DOUBLE)
	{
		return value_.doubleValue_ < dataTimestamp.value_.doubleValue_;
	}
	else if (valueType_ == VT_INT64)
	{
		return value_.intValue_ < dataTimestamp.value_.intValue_;
	}

	return false;
}

bool DataTimestamp::operator<=(const DataTimestamp& dataTimestamp) const
{
	ocean_assert(isValid() && dataTimestamp.isValid());

	if (valueType_ != dataTimestamp.valueType_)
	{
		return false;
	}

	if (valueType_ == VT_DOUBLE)
	{
		return value_.doubleValue_ <= dataTimestamp.value_.doubleValue_;
	}
	else if (valueType_ == VT_INT64)
	{
		return value_.intValue_ <= dataTimestamp.value_.intValue_;
	}

	return false;
}

bool DataTimestamp::operator>(const DataTimestamp& dataTimestamp) const
{
	ocean_assert(isValid() && dataTimestamp.isValid());

	if (valueType_ != dataTimestamp.valueType_)
	{
		ocean_assert(false && "Not matching value types!");
		return false;
	}

	if (valueType_ == VT_DOUBLE)
	{
		return value_.doubleValue_ > dataTimestamp.value_.doubleValue_;
	}
	else if (valueType_ == VT_INT64)
	{
		return value_.intValue_ > dataTimestamp.value_.intValue_;
	}

	return false;
}

bool DataTimestamp::operator>=(const DataTimestamp& dataTimestamp) const
{
	if (valueType_ != dataTimestamp.valueType_)
	{
		return false;
	}

	if (valueType_ == VT_DOUBLE)
	{
		return value_.doubleValue_ >= dataTimestamp.value_.doubleValue_;
	}
	else if (valueType_ == VT_INT64)
	{
		return value_.intValue_ >= dataTimestamp.value_.intValue_;
	}

	return false;
}

bool DataTimestamp::read(InputBitstream& inputBitstream, DataTimestamp& dataTimestamp)
{
	uint8_t valueType = 0u;

	if (inputBitstream.read<uint8_t>(valueType))
	{
		if (valueType == DataTimestamp::VT_INVALID)
		{
			dataTimestamp = DataTimestamp();

			return true;
		}
		else if (valueType == DataTimestamp::VT_DOUBLE)
		{
			double value;
			if (inputBitstream.read<double>(value))
			{
				dataTimestamp = DataTimestamp(value);

				return true;
			}
		}
		else if (valueType == DataTimestamp::VT_INT64)
		{
			int64_t value;
			if (inputBitstream.read<int64_t>(value))
			{
				dataTimestamp = DataTimestamp(value);

				return true;
			}
		}
	}

	return false;
}

bool DataTimestamp::write(OutputBitstream& outputBitstream, const DataTimestamp& dataTimestamp)
{
	if (!outputBitstream.write<uint8_t>(dataTimestamp.valueType_))
	{
		return false;
	}

	if (dataTimestamp.valueType_ == DataTimestamp::VT_INVALID)
	{
		return true;
	}
	else if (dataTimestamp.valueType_ == DataTimestamp::VT_DOUBLE)
	{
		return outputBitstream.write<double>(dataTimestamp.value_.doubleValue_);
	}
	else if (dataTimestamp.valueType_ == DataTimestamp::VT_INT64)
	{
		return outputBitstream.write<int64_t>(dataTimestamp.value_.intValue_);
	}

	return false;
}

}

}

}
