/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Utilities.h"

#include "ocean/math/Numeric.h"

#include <cstring>

namespace Ocean
{

namespace IO
{

bool Utilities::readFile(const std::string& filename, Buffer& buffer)
{
	ocean_assert(!filename.empty());

	if (filename.empty())
	{
		return false;
	}

	std::ifstream stream(filename, std::ios::binary);

	if (!stream.is_open())
	{
		return false;
	}

	stream.seekg(0, std::ios_base::end);
		const std::istream::pos_type position = stream.tellg();
	stream.seekg(0, std::ios_base::beg);

	const std::streamoff fileSize = position;

	buffer.clear();

	if (fileSize != 0)
	{
		if (!NumericT<size_t>::isInsideValueRange(fileSize))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const size_t bufferSize = size_t(fileSize);

		buffer.resize(bufferSize);

		if (buffer.size() != bufferSize)
		{
			return false;
		}

		stream.read((char*)(buffer.data()), buffer.size());

		return stream.good();
	}

	return true;
}

void Utilities::encodeHomogenousMatrix4(const HomogenousMatrix4& matrix, Buffer& buffer)
{
	static_assert(sizeof(HomogenousMatrixD4) == 8 * 16, "Invalid data type!");

	const size_t offset = buffer.size();
	buffer.resize(buffer.size() + 8 * 16);

	if constexpr (std::is_same<HomogenousMatrix4, HomogenousMatrixD4>::value)
	{
		*((HomogenousMatrixD4*)(buffer.data() + offset)) = HomogenousMatrixD4(matrix);
	}
	else
	{
		*((HomogenousMatrixD4*)(buffer.data() + offset)) = (HomogenousMatrixD4&)matrix;
	}
}

bool Utilities::decodeHomogenousMatrix4(const uint8_t*& data, size_t& size, HomogenousMatrix4& matrix)
{
	static_assert(sizeof(HomogenousMatrixD4) == 8 * 16, "Invalid data type!");

	ocean_assert(data && size != 0);

	if (size < 8 * 16)
	{
		return false;
	}

	if constexpr (std::is_same<HomogenousMatrix4, HomogenousMatrixD4>::value)
	{
		matrix = *((HomogenousMatrix4*)data);
	}
	else
	{
		matrix = HomogenousMatrix4(*((HomogenousMatrixD4*)data));
	}

	data += 8 * 16;
	size -= 8 * 16;

	return true;
}

void Utilities::encodeVectors2(const Vector2* vectors, const size_t size, Buffer& buffer)
{
	static_assert(sizeof(VectorD2) == 8 * 2, "Invalid data type!");

	ocean_assert(vectors != nullptr || size == 0);

	const size_t offset = buffer.size();
	buffer.resize(buffer.size() + 8 + 8 * 2 * size);

	// set the number of vectors
	const uint64_t numberVectors = uint64_t(size);
	memcpy(buffer.data() + offset, &numberVectors, sizeof(numberVectors));

	if constexpr (std::is_same<Vector2, VectorD2>::value)
	{
		memcpy(buffer.data() + offset + 8, vectors, 8 * 2 * size);
	}
	else
	{
		for (size_t n = 0; n < size; ++n)
		{
			const VectorD2 value(vectors[n]);
			memcpy(buffer.data() + offset + 8 + n * 8 * 2, &value, sizeof(value));
		}
	}
}

bool Utilities::decodeVectors2(const uint8_t*& data, size_t& size, Vectors2& vectors)
{
	static_assert(sizeof(VectorD2) == 8 * 2, "Invalid data type!");

	ocean_assert(data && size != 0);

	if (size < 8)
	{
		return false;
	}

	uint64_t numberVectors = 0ull;
	memcpy(&numberVectors, data, sizeof(numberVectors));

	if (numberVectors > uint64_t((size - 8) / (8 * 2)))
	{
		return false;
	}

	const size_t number = size_t(numberVectors);

	ocean_assert(vectors.empty());
	vectors.clear();
	vectors.resize(number);

	if constexpr (std::is_same<Vector2, VectorD2>::value)
	{
		memcpy(vectors.data(), data + 8, number * 8 * 2);
	}
	else
	{
		for (size_t n = 0; n < number; ++n)
		{
			VectorD2 value;
			memcpy(&value, data + 8 + n * 8 * 2, sizeof(value));

			vectors[n] = Vector2(value);
		}
	}

	data += 8 + number * 8 * 2;
	size -= 8 + number * 8 * 2;

	return true;
}

void Utilities::encodeVectors3(const Vector3* vectors, const size_t size, Buffer& buffer)
{
	static_assert(sizeof(VectorD3) == 8 * 3, "Invalid data type!");

	ocean_assert(vectors != nullptr || size == 0);

	const size_t offset = buffer.size();
	buffer.resize(buffer.size() + 8 + 8 * 3 * size);

	// set the number of vectors
	const uint64_t numberVectors = uint64_t(size);
	memcpy(buffer.data() + offset, &numberVectors, sizeof(numberVectors));

	if constexpr (std::is_same<Vector3, VectorD3>::value)
	{
		memcpy(buffer.data() + offset + 8, vectors, 8 * 3 * size);
	}
	else
	{
		for (size_t n = 0; n < size; ++n)
		{
			const VectorD3 value(vectors[n]);
			memcpy(buffer.data() + offset + 8 + n * 8 * 3, &value, sizeof(value));
		}
	}
}

bool Utilities::decodeVectors3(const uint8_t*& data, size_t& size, Vectors3& vectors)
{
	static_assert(sizeof(VectorD3) == 8 * 3, "Invalid data type!");

	ocean_assert(data && size != 0);

	if (size < 8)
	{
		return false;
	}

	uint64_t numberVectors = 0ull;
	memcpy(&numberVectors, data, sizeof(numberVectors));

	if (numberVectors > uint64_t((size - 8) / (8 * 3)))
	{
		return false;
	}

	const size_t number = size_t(numberVectors);

	ocean_assert(vectors.empty());
	vectors.clear();
	vectors.resize(number);

	if constexpr (std::is_same<Vector3, VectorD3>::value)
	{
		memcpy(vectors.data(), data + 8, number * 8 * 3);
	}
	else
	{
		for (size_t n = 0; n < number; ++n)
		{
			VectorD3 value;
			memcpy(&value, data + 8 + n * 8 * 3, sizeof(value));

			vectors[n] = Vector3(value);
		}
	}

	data += 8 + number * 8 * 3;
	size -= 8 + number * 8 * 3;

	return true;
}

void Utilities::encodeVectors4(const Vector4* vectors, const size_t size, Buffer& buffer)
{
	ocean_assert(vectors != nullptr || size == 0);

	static_assert(sizeof(VectorD4) == 8 * 4, "Invalid data type!");

	ocean_assert(vectors != nullptr || size == 0);

	const size_t offset = buffer.size();
	buffer.resize(buffer.size() + 8 + 8 * 4 * size);

	// set the number of vectors
	const uint64_t numberVectors = uint64_t(size);
	memcpy(buffer.data() + offset, &numberVectors, sizeof(numberVectors));

	if constexpr (std::is_same<Vector4, VectorD4>::value)
	{
		memcpy(buffer.data() + offset + 8, vectors, 8 * 4 * size);
	}
	else
	{
		for (size_t n = 0; n < size; ++n)
		{
			const VectorD4 value(vectors[n]);
			memcpy(buffer.data() + offset + 8 + n * 8 * 4, &value, sizeof(value));
		}
	}
}

bool Utilities::decodeVectors4(const uint8_t*& data, size_t& size, Vectors4& vectors)
{
	static_assert(sizeof(VectorD4) == 8 * 4, "Invalid data type!");

	ocean_assert(data && size != 0);

	if (size < 8)
	{
		return false;
	}

	uint64_t numberVectors = 0ull;
	memcpy(&numberVectors, data, sizeof(numberVectors));

	if (numberVectors > uint64_t((size - 8) / (8 * 4)))
	{
		return false;
	}

	const size_t number = size_t(numberVectors);

	ocean_assert(vectors.empty());
	vectors.clear();
	vectors.resize(number);

	if constexpr (std::is_same<Vector4, VectorD4>::value)
	{
		memcpy(vectors.data(), data + 8, number * 8 * 4);
	}
	else
	{
		for (size_t n = 0; n < number; ++n)
		{
			VectorD4 value;
			memcpy(&value, data + 8 + n * 8 * 4, sizeof(value));

			vectors[n] = Vector4(value);
		}
	}

	data += 8 + number * 8 * 4;
	size -= 8 + number * 8 * 4;

	return true;
}

}

}
