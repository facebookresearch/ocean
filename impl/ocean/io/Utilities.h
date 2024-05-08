/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_UTILITIES_H
#define META_OCEAN_IO_UTILITIES_H

#include "ocean/io/IO.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include <vector>

namespace Ocean
{

namespace IO
{

/**
 * This class implements utility functions for input and output operations.
 * @ingroup io
 */
class OCEAN_IO_EXPORT Utilities
{
	public:

		/**
		 * Definition of a vector holding 8 bit elements.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Reads a file and copies the data into a buffer.
		 * In case the file exists but is empty, the resulting buffer will be empty but the function succeeds.
		 * @param filename The name of the file from which the data will be read, must be valid
		 * @param buffer The buffer receiving the data
		 * @return True, if succeeded
		 */
		static bool readFile(const std::string& filename, Buffer& buffer);

		/**
		 * Encodes a 4x4 homogeneous matrix.
		 * The matrix will be stored with 64 bit precision.
		 * @param matrix The homogeneous matrix which will be encoded
		 * @param buffer The resulting buffer which will be extended so that already existing data stays untouched
		 */
		static void encodeHomogenousMatrix4(const HomogenousMatrix4& matrix, Buffer& buffer);

		/**
		 * Encodes a 4x4 homogeneous matrix.
		 * The matrix will be stored with 64 bit precision.
		 * @param matrix The homogeneous matrix which will be encoded
		 * @param reservedHeaderSize The number of bytes which will be reserved for an optional header, so that the resulting buffer has a header followed by the payload data
		 * @return The resulting buffer storing the given matrix
		 */
		static inline Buffer encodeHomogenousMatrix4(const HomogenousMatrix4& matrix, const size_t reservedHeaderSize = 0);

		/**
		 * Decodes a 4x4 homogeneous matrix.
		 * The buffer must provide the matrix with 64 bit precision.
		 * @param data The pointer to the buffer providing the matrix data, will be shifted by the number of consumed bytes afterwards
		 * @param size The size of the entire buffer, will be reduced by the number of consumed bytes afterwards
		 * @param matrix The resulting matrix
		 * @return True, if succeeded
		 */
		static bool decodeHomogenousMatrix4(const uint8_t*& data, size_t& size, HomogenousMatrix4& matrix);

		/**
		 * Decodes a 4x4 homogeneous matrix.
		 * The buffer must provide the matrix with 64 bit precision.
		 * @param buffer The buffer providing the matrix data
		 * @param matrix The resulting matrix
		 * @return True, if succeeded
		 */
		static inline bool decodeHomogenousMatrix4(const Buffer& buffer, HomogenousMatrix4& matrix);

		/**
		 * Encodes a set of given 2D vectors.
		 * The vectors will be stored with 64 bit precision.
		 * @param vectors The vectors to encode
		 * @param size The number of given vectors
		 * @param buffer The resulting buffer which will be extended so that already existing data stays untouched
		 */
		static void encodeVectors2(const Vector2* vectors, const size_t size, Buffer& buffer);

		/**
		 * Encodes a set of given 2D vectors.
		 * The vectors will be stored with 64 bit precision.
		 * @param vectors The vectors to encode
		 * @param size The number of given vectors
		 * @param reservedHeaderSize The number of bytes which will be reserved for an optional header, so that the resulting buffer has a header followed by the payload data
		 * @return The resulting buffer storing the given vectors
		 */
		static inline Buffer encodeVectors2(const Vector2* vectors, const size_t size, const size_t reservedHeaderSize = 0);

		/**
		 * Decodes a set of 2D vectors.
		 * The buffer must provide the vectors with 64 bit precision.
		 * @param data The pointer to the buffer providing the 2D vectors, will be shifted by the number of consumed bytes afterwards
		 * @param size The size of the entire buffer, will be reduced by the number of consumed bytes afterwards
		 * @param vectors The resulting vectors
		 * @return True, if succeeded
		 */
		static bool decodeVectors2(const uint8_t*& data, size_t& size, Vectors2& vectors);

		/**
		 * Decodes a set of 2D vectors.
		 * The buffer must provide the vectors with 64 bit precision.
		 * @param buffer The buffer providing the 2D vectors
		 * @param vectors The resulting vectors
		 * @return True, if succeeded
		 */
		static inline bool decodeVectors2(const Buffer& buffer, Vectors2& vectors);

		/**
		 * Encodes a set of given 3D vectors.
		 * The vectors will be stored with 64 bit precision.
		 * @param vectors The vectors to encode
		 * @param size The number of given vectors
		 * @param buffer The resulting buffer which will be extended so that already existing data stays untouched
		 */
		static void encodeVectors3(const Vector3* vectors, const size_t size, Buffer& buffer);

		/**
		 * Encodes a set of given 3D vectors.
		 * The vectors will be stored with 64 bit precision.
		 * @param vectors The vectors to encode
		 * @param size The number of given vectors
		 * @param reservedHeaderSize The number of bytes which will be reserved for an optional header, so that the resulting buffer has a header followed by the payload data
		 * @return The resulting buffer storing the given vectors
		 */
		static inline Buffer encodeVectors3(const Vector3* vectors, const size_t size, const size_t reservedHeaderSize = 0);

		/**
		 * Decodes a set of 3D vectors.
		 * The buffer must provide the vectors with 64 bit precision.
		 * @param data The pointer to the buffer providing the 3D vectors, will be shifted by the number of consumed bytes afterwards
		 * @param size The size of the entire buffer, will be reduced by the number of consumed bytes afterwards
		 * @param vectors The resulting vectors
		 * @return True, if succeeded
		 */
		static bool decodeVectors3(const uint8_t*& data, size_t& size, Vectors3& vectors);

		/**
		 * Decodes a set of 3D vectors.
		 * The buffer must provide the vectors with 64 bit precision.
		 * @param buffer The buffer providing the 3D vectors
		 * @param vectors The resulting vectors
		 * @return True, if succeeded
		 */
		static inline bool decodeVectors3(const Buffer& buffer, Vectors3& vectors);

		/**
		 * Encodes a set of given 4D vectors.
		 * The vectors will be stored with 64 bit precision.
		 * @param vectors The vectors to encode
		 * @param size The number of given vectors
		 * @param buffer The resulting buffer which will be extended so that already existing data stays untouched
		 */
		static void encodeVectors4(const Vector4* vectors, const size_t size, Buffer& buffer);

		/**
		 * Encodes a set of given 4D vectors.
		 * The vectors will be stored with 64 bit precision.
		 * @param vectors The vectors to encode
		 * @param size The number of given vectors
		 * @param reservedHeaderSize The number of bytes which will be reserved for an optional header, so that the resulting buffer has a header followed by the payload data
		 * @return The resulting buffer storing the given vectors
		 */
		static inline Buffer encodeVectors4(const Vector4* vectors, const size_t size, const size_t reservedHeaderSize = 0);

		/**
		 * Decodes a set of 4D vectors.
		 * The buffer must provide the vectors with 64 bit precision.
		 * @param data The pointer to the buffer providing the 4D vectors, will be shifted by the number of consumed bytes afterwards
		 * @param size The size of the entire buffer, will be reduced by the number of consumed bytes afterwards
		 * @param vectors The resulting vectors
		 * @return True, if succeeded
		 */
		static bool decodeVectors4(const uint8_t*& data, size_t& size, Vectors4& vectors);

		/**
		 * Decodes a set of 4D vectors.
		 * The buffer must provide the vectors with 64 bit precision.
		 * @param buffer The buffer providing the 4D vectors
		 * @param vectors The resulting vectors
		 * @return True, if succeeded
		 */
		static inline bool decodeVectors4(const Buffer& buffer, Vectors4& vectors);
};

inline Utilities::Buffer Utilities::encodeHomogenousMatrix4(const HomogenousMatrix4& matrix, const size_t reservedHeaderSize)
{
	Buffer result(reservedHeaderSize);

	encodeHomogenousMatrix4(matrix, result);

	return result;
}

inline bool Utilities::decodeHomogenousMatrix4(const Buffer& buffer, HomogenousMatrix4& matrix)
{
	const uint8_t* data = buffer.data();
	size_t size = buffer.size();

	return decodeHomogenousMatrix4(data, size, matrix);
}

inline Utilities::Buffer Utilities::encodeVectors2(const Vector2* vectors, const size_t size, const size_t reservedHeaderSize)
{
	Buffer result(reservedHeaderSize);

	encodeVectors2(vectors, size, result);

	return result;
}

inline bool Utilities::decodeVectors2(const Buffer& buffer, Vectors2& vectors)
{
	const uint8_t* data = buffer.data();
	size_t size = buffer.size();

	return decodeVectors2(data, size, vectors);
}

inline Utilities::Buffer Utilities::encodeVectors3(const Vector3* vectors, const size_t size, const size_t reservedHeaderSize)
{
	Buffer result(reservedHeaderSize);

	encodeVectors3(vectors, size, result);

	return result;
}

inline bool Utilities::decodeVectors3(const Buffer& buffer, Vectors3& vectors)
{
	const uint8_t* data = buffer.data();
	size_t size = buffer.size();

	return decodeVectors3(data, size, vectors);
}

inline Utilities::Buffer Utilities::encodeVectors4(const Vector4* vectors, const size_t size, const size_t reservedHeaderSize)
{
	Buffer result(reservedHeaderSize);

	encodeVectors4(vectors, size, result);

	return result;
}

inline bool Utilities::decodeVectors4(const Buffer& buffer, Vectors4& vectors)
{
	const uint8_t* data = buffer.data();
	size_t size = buffer.size();

	return decodeVectors4(data, size, vectors);
}

}

}

#endif // META_OCEAN_IO_UTILITIES_H
