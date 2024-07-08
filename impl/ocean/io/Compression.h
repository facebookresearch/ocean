/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_COMPRESSION_H
#define META_OCEAN_IO_COMPRESSION_H

#include "ocean/io/IO.h"

#include <vector>

namespace Ocean
{

namespace IO
{

/**
 * This class implements individual compression algorithms.
 * @ingroup io
 */
class OCEAN_IO_EXPORT Compression
{
	public:

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Compresses a buffer with gzip
		 * @param buffer The buffer to compress
		 * @param bufferSize The size of the buffer, in bytes
		 * @param compressedBuffer The compressed buffer
		 * @return True, if succeeded
		 */
		static bool gzipCompress(const void* buffer, const size_t bufferSize, Buffer& compressedBuffer);

		/**
		 * Decompresses a buffer which has been compressed with gzip
		 * @param compressedBuffer The compressed buffer
		 * @param compressedBufferSize The size of the uncompressed buffer, in bytes
		 * @param uncompressedBuffer The uncompressed buffer
		 * @return True, if succeeded
		 */
		static bool gzipDecompress(const void* compressedBuffer, const size_t compressedBufferSize, Buffer& uncompressedBuffer);
};

}

}

#endif // META_OCEAN_IO_COMPRESSION_H
