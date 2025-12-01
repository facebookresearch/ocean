/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/serialization/VectorOutputStream.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

VectorOutputStream::VectorStreamBuffer::int_type VectorOutputStream::VectorStreamBuffer::overflow(int_type character)
{
	if (character != traits_type::eof())
	{
		if (currentPosition_ >= buffer_.size())
		{
			buffer_.push_back(char(character));
		}
		else
		{
			buffer_[currentPosition_] = char(character);
		}

		++currentPosition_;

		return character;
	}

	return traits_type::eof();
}

std::streamsize VectorOutputStream::VectorStreamBuffer::xsputn(const char* data, std::streamsize size)
{
	if (size <= 0)
	{
		return 0;
	}

	const size_t endPosition = currentPosition_ + size_t(size);

	if (endPosition > buffer_.size())
	{
		buffer_.resize(endPosition);
	}

	std::copy(data, data + size, buffer_.begin() + currentPosition_);

	currentPosition_ = endPosition;

	return size;
}

VectorOutputStream::VectorStreamBuffer::pos_type VectorOutputStream::VectorStreamBuffer::seekoff(off_type offset, std::ios_base::seekdir direction, std::ios_base::openmode mode)
{
	if (mode != std::ios_base::out && mode != (std::ios_base::in | std::ios_base::out))
	{
		return pos_type(off_type(-1));
	}

	off_type newPosition = 0;

	switch (direction)
	{
		case std::ios_base::beg:
			newPosition = offset;
			break;

		case std::ios_base::cur:
			newPosition = off_type(currentPosition_) + offset;
			break;

		case std::ios_base::end:
			newPosition = off_type(buffer_.size()) + offset;
			break;

		default:
			return pos_type(off_type(-1));
	}

	if (newPosition < 0)
	{
		return pos_type(off_type(-1));
	}

	currentPosition_ = size_t(newPosition);

	return pos_type(newPosition);
}

VectorOutputStream::VectorStreamBuffer::pos_type VectorOutputStream::VectorStreamBuffer::seekpos(pos_type position, std::ios_base::openmode mode)
{
	if (mode != std::ios_base::out && mode != (std::ios_base::in | std::ios_base::out))
	{
		return pos_type(off_type(-1));
	}

	const off_type offset = off_type(position);

	if (offset < 0)
	{
		return pos_type(off_type(-1));
	}

	currentPosition_ = size_t(offset);

	return position;
}

}

}

}
