/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Bitstream.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace IO
{

InputBitstream::InputBitstream(std::istream& stream) :
	inputStream_(stream)
{
	static_assert(sizeof(bool) == 1, "Invalid data type!");

	static_assert(sizeof(char) == 1, "Invalid data type!");
	static_assert(sizeof(unsigned char) == 1, "Invalid data type!");

	static_assert(sizeof(wchar_t) <= 4, "Invalid data type!");

	static_assert(sizeof(signed short) == 2, "Invalid data type!");
	static_assert(sizeof(unsigned short) == 2, "Invalid data type!");

	static_assert(sizeof(int) == 4, "Invalid data type!");
	static_assert(sizeof(unsigned int) == 4, "Invalid data type!");

	static_assert(sizeof(float) == 4, "Invalid data type!");
	static_assert(sizeof(double) == 8, "Invalid data type!");

	static_assert(sizeof(long long) == 8, "Invalid data type!");
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");
}

template <typename T>
bool InputBitstream::read(T& value)
{
	if (inputStream_.good())
	{
		inputStream_.read((char*)&value, sizeof(T));
		return inputStream_.good();
	}

	return false;
}

template <>
bool OCEAN_IO_EXPORT InputBitstream::read(wchar_t& value)
{
	static_assert(sizeof(wchar_t) <= 4, "Invalid data type!");
	static_assert(sizeof(int) == 4, "Invalid data type!");

	// the wchar_t type needs a special handling as this data type may have size 1, 2 or 4 on individual platforms, thus we store 4 byte per object

	if (inputStream_.good())
	{
		int value32;

		inputStream_.read((char*)(&value32), sizeof(int));

		if (inputStream_.good())
		{
			value = wchar_t(value32);
			return true;
		}
	}

	return false;
}

template <>
bool OCEAN_IO_EXPORT InputBitstream::read(std::string& value)
{
	if (inputStream_.good())
	{
		unsigned int stringLength = 0u;

		if (!read<unsigned int>(stringLength))
		{
			return false;
		}

		value.resize(stringLength);

		return read((void*)(value.data()), sizeof(char) * size_t(stringLength));
	}

	return false;
}

template <>
bool OCEAN_IO_EXPORT InputBitstream::read(std::wstring& value)
{
	static_assert(sizeof(wchar_t) <= 4, "Invalid data type!");

	// the wchar_t type needs a special handling as this data type may have size 1, 2 or 4 on individual platforms, thus we store 4 byte per object

	if (inputStream_.good())
	{
		unsigned int stringLength = 0u;

		if (!read<unsigned int>(stringLength))
		{
			return false;
		}

		value.resize(stringLength);

		for (size_t n = 0; n < value.size(); ++n)
		{
			if (!read<wchar_t>(value[n]))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

template <typename T>
T InputBitstream::readDefault(const T& defaultValue)
{
	if (inputStream_.good())
	{
		T value;

		if (read<T>(value))
		{
			return value;
		}
	}

	return defaultValue;
}

bool InputBitstream::read(void* data, const size_t size)
{
	ocean_assert(data || size == 0);

	if (size == 0)
	{
		return true;
	}

	if (inputStream_.good())
	{
		inputStream_.read((char*)(data), size);
		return inputStream_.good();
	}

	return false;
}

template <typename T>
bool InputBitstream::look(T& value)
{
	ScopedInputBitstream scope(*this);

	return scope.read<T>(value);
}

uint64_t InputBitstream::position() const
{
	if (inputStream_.good())
	{
		const std::streampos currentPosition = inputStream_.tellg();

		if (inputStream_.good())
		{
			ocean_assert(NumericT<uint64_t>::isInsideValueRange(currentPosition));

			return uint64_t(currentPosition);
		}
	}

	return uint64_t(-1);
}

uint64_t InputBitstream::size() const
{
	if (inputStream_.good())
	{
		const std::streampos currentPosition = inputStream_.tellg();

		inputStream_.seekg(0, inputStream_.end);
		const uint64_t value = uint64_t(inputStream_.tellg());

		inputStream_.seekg(currentPosition, inputStream_.beg);

		if (inputStream_.good())
		{
			return value;
		}
	}

	return uint64_t(-1);
}

bool InputBitstream::setPosition(const uint64_t position)
{
	if (inputStream_.good())
	{
		const uint64_t currentSize = size();

		if (currentSize != uint64_t(-1) && position <= currentSize)
		{
			inputStream_.seekg(position, inputStream_.beg);
			return inputStream_.good();
		}
	}

	return false;
}

bool InputBitstream::skip(const uint64_t bytes)
{
	if (bytes == 0)
	{
		return true;
	}

	if (inputStream_.good())
	{
		const uint64_t currentPosition = position();

		if (currentPosition != uint64_t(-1))
		{
			const uint64_t newPosition = currentPosition + bytes;
			return setPosition(newPosition);
		}
	}

	return false;
}

bool InputBitstream::isEndOfFile() const
{
	if (inputStream_.good())
	{
		const uint64_t currentPosition = position();
		const uint64_t streamSize = size();

		if (currentPosition != uint64_t(-1) && streamSize != uint64_t(-1))
		{
			return currentPosition >= streamSize;
		}
	}

	return true;
}

OutputBitstream::OutputBitstream(std::ostream& stream) :
	outputStream_(stream)
{
	// nothing to do here
}

template <typename T>
bool OutputBitstream::write(const T& value)
{
	if (outputStream_.good())
	{
		outputStream_.write((const char*)(&value), sizeof(T));
		return outputStream_.good();
	}

	return false;
}

template <>
bool OCEAN_IO_EXPORT OutputBitstream::write(const wchar_t& value)
{
	static_assert(sizeof(wchar_t) <= 4, "Invalid data type!");
	static_assert(sizeof(int) == 4, "Invalid data type!");

	// the wchar_t type needs a special handling as this data type may have size 1, 2 or 4 on individual platforms, thus we store 4 byte per object

	if (outputStream_.good())
	{
		const int value32 = int(value);

		outputStream_.write((const char*)(&value32), sizeof(int));
		return outputStream_.good();
	}

	return false;
}

template <>
bool OCEAN_IO_EXPORT OutputBitstream::write(const std::string& value)
{
	if (outputStream_.good())
	{
		ocean_assert(value.size() <= NumericT<unsigned int>::maxValue());

		if (!write<unsigned int>((unsigned int)(value.size())))
		{
			return false;
		}

		return write((const void*)(value.data()), sizeof(char) * value.size());
	}

	return false;
}

template <>
bool OCEAN_IO_EXPORT OutputBitstream::write(const std::wstring& value)
{
	if (outputStream_.good())
	{
		ocean_assert(value.size() <= NumericT<unsigned int>::maxValue());

		if (!write<unsigned int>((unsigned int)(value.size())))
		{
			return false;
		}

		for (size_t n = 0; n < value.size(); ++n)
		{
			if (!write<wchar_t>(value[n]))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool OutputBitstream::write(const void* data, const size_t size)
{
	ocean_assert(data || size == 0);

	if (size == 0)
	{
		return true;
	}

	if (outputStream_.good())
	{
		outputStream_.write((const char*)(data), size);
		return outputStream_.good();
	}

	return false;
}

uint64_t OutputBitstream::size() const
{
	if (outputStream_.good())
	{
		const uint64_t value = uint64_t(outputStream_.tellp());
		return value;
	}

	return uint64_t(-1);
}

template bool OCEAN_IO_EXPORT InputBitstream::read<bool>(bool&);
template bool OCEAN_IO_EXPORT InputBitstream::read<char>(char&);
template bool OCEAN_IO_EXPORT InputBitstream::read<unsigned char>(unsigned char&);
template bool OCEAN_IO_EXPORT InputBitstream::read<signed short>(signed short&);
template bool OCEAN_IO_EXPORT InputBitstream::read<unsigned short>(unsigned short&);
template bool OCEAN_IO_EXPORT InputBitstream::read<int>(int&);
template bool OCEAN_IO_EXPORT InputBitstream::read<unsigned int>(unsigned int&);
template bool OCEAN_IO_EXPORT InputBitstream::read<float>(float&);
template bool OCEAN_IO_EXPORT InputBitstream::read<double>(double&);
template bool OCEAN_IO_EXPORT InputBitstream::read<long long>(long long&);
template bool OCEAN_IO_EXPORT InputBitstream::read<unsigned long long>(unsigned long long&);

template bool OCEAN_IO_EXPORT InputBitstream::readDefault<bool>(const bool&);
template char OCEAN_IO_EXPORT InputBitstream::readDefault<char>(const char&);
template unsigned char OCEAN_IO_EXPORT InputBitstream::readDefault<unsigned char>(const unsigned char&);
template wchar_t OCEAN_IO_EXPORT InputBitstream::readDefault<wchar_t>(const wchar_t&);
template signed short OCEAN_IO_EXPORT InputBitstream::readDefault<signed short>(const signed short&);
template unsigned short OCEAN_IO_EXPORT InputBitstream::readDefault<unsigned short>(const unsigned short&);
template int OCEAN_IO_EXPORT InputBitstream::readDefault<int>(const int&);
template unsigned int OCEAN_IO_EXPORT InputBitstream::readDefault<unsigned int>(const unsigned int&);
template float OCEAN_IO_EXPORT InputBitstream::readDefault<float>(const float&);
template double OCEAN_IO_EXPORT InputBitstream::readDefault<double>(const double&);
template long long OCEAN_IO_EXPORT InputBitstream::readDefault<long long>(const long long&);
template unsigned long long OCEAN_IO_EXPORT InputBitstream::readDefault<unsigned long long>(const unsigned long long&);

template bool OCEAN_IO_EXPORT InputBitstream::look<bool>(bool&);
template bool OCEAN_IO_EXPORT InputBitstream::look<char>(char&);
template bool OCEAN_IO_EXPORT InputBitstream::look<unsigned char>(unsigned char&);
template bool OCEAN_IO_EXPORT InputBitstream::look<wchar_t>(wchar_t&);
template bool OCEAN_IO_EXPORT InputBitstream::look<signed short>(signed short&);
template bool OCEAN_IO_EXPORT InputBitstream::look<unsigned short>(unsigned short&);
template bool OCEAN_IO_EXPORT InputBitstream::look<int>(int&);
template bool OCEAN_IO_EXPORT InputBitstream::look<unsigned int>(unsigned int&);
template bool OCEAN_IO_EXPORT InputBitstream::look<float>(float&);
template bool OCEAN_IO_EXPORT InputBitstream::look<double>(double&);
template bool OCEAN_IO_EXPORT InputBitstream::look<long long>(long long&);
template bool OCEAN_IO_EXPORT InputBitstream::look<unsigned long long>(unsigned long long&);

template bool OCEAN_IO_EXPORT OutputBitstream::write<bool>(const bool&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<char>(const char&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<unsigned char>(const unsigned char&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<signed short>(const signed short&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<unsigned short>(const unsigned short&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<int>(const int&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<unsigned int>(const unsigned int&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<float>(const float&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<double>(const double&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<long long>(const long long&);
template bool OCEAN_IO_EXPORT OutputBitstream::write<unsigned long long>(const unsigned long long&);

bool Tag::writeTag(IO::OutputBitstream& bitstream, const Tag& tag)
{
	ocean_assert(bitstream);

	static_assert(sizeof(Tag) == sizeof(unsigned long long), "Invalid tag size!");

	return bitstream.write<unsigned long long>(tag.value());
}

bool Tag::readTag(IO::InputBitstream& bitstream, Tag& tag)
{
	ocean_assert(bitstream);

	static_assert(sizeof(Tag) == sizeof(unsigned long long), "Invalid tag size!");

	if (!bitstream.read<unsigned long long>(tag.value()))
	{
		return false;
	}

	return true;
}

bool Tag::lookTag(IO::InputBitstream& bitstream, Tag& tag)
{
	ocean_assert(bitstream);

	static_assert(sizeof(Tag) == sizeof(unsigned long long), "Invalid tag size!");

	if (!bitstream.look<unsigned long long>(tag.value()))
	{
		return false;
	}

	return true;
}

bool Tag::readAndCheckTag(IO::InputBitstream& bitstream, const Tag& expectedTag)
{
	ocean_assert(bitstream);

	Tag tag;
	if (!readTag(bitstream, tag) || tag != expectedTag)
	{
		return false;
	}

	return true;
}

bool Tag::lookAndCheckTag(IO::InputBitstream& bitstream, const Tag& expectedTag)
{
	ocean_assert(bitstream);

	Tag tag;
	if (!lookTag(bitstream, tag) || tag != expectedTag)
	{
		return false;
	}

	return true;
}

}

}
