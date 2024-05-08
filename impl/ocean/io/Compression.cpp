/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Compression.h"

#include <zlib.h>

namespace Ocean
{

namespace IO
{

bool Compression::gzipCompress(const void* buffer, const size_t bufferSize, Buffer& compressedBuffer)
{
	ocean_assert(compressedBuffer.empty());
	compressedBuffer.clear();

	if (bufferSize == 0)
	{
		return true;
	}

	if (bufferSize > size_t(0xFFFFFFF0ull))
	{
		return false;
	}

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.total_out = 0;
	stream.next_in = (Bytef*)(buffer);
	stream.avail_in = (unsigned int)(bufferSize);

	if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, (16 + MAX_WBITS), 8, Z_DEFAULT_STRATEGY) != Z_OK)
	{
		return false;
	}

	bool succeeded = true;
	compressedBuffer.resize(16384);

	while (true)
	{
		// check whether the output buffer is too small
		if (stream.total_out >= (unsigned int)(compressedBuffer.size()))
		{
			compressedBuffer.resize(compressedBuffer.size() + 16384);
		}

		stream.next_out = (Bytef*)(compressedBuffer.data() + stream.total_out);
		stream.avail_out = (unsigned int)(compressedBuffer.size()) - (unsigned int)(stream.total_out);

		// deflate the next chunk

		const int error = deflate(&stream, Z_FINISH);

		if (error == Z_STREAM_END)
		{
			break;
		}
		else if (error != Z_OK)
		{
			succeeded = false;
			break;
		}
	}

	if (deflateEnd(&stream) != Z_OK || !succeeded)
	{
		compressedBuffer.clear();
		return false;
	}

	compressedBuffer.resize(stream.total_out);
	return true;
}

bool Compression::gzipDecompress(const void* compressedBuffer, const size_t compressedBufferSize, Buffer& uncompressedBuffer)
{
	ocean_assert(uncompressedBuffer.empty());
	uncompressedBuffer.clear();

	if (compressedBufferSize == 0)
	{
		return true;
	}

	if (compressedBufferSize > 0xFFFFFFF0ull)
	{
		return false;
	}

	const size_t fullLength = compressedBufferSize;
	const size_t halfLength = compressedBufferSize / 2;

	const size_t uncompressedLength = fullLength;

	z_stream stream;
	stream.next_in = (Bytef*)(compressedBuffer);
	stream.avail_in = (unsigned int)(compressedBufferSize);
	stream.total_out = 0;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;

	if (inflateInit2(&stream, (16 + MAX_WBITS)) != Z_OK)
	{
		return false;
	}

	bool succeeded = true;
	uncompressedBuffer.resize(uncompressedLength);

	while (true)
	{
		// check whether the output buffer is too small
		if (stream.total_out >= (unsigned int)uncompressedBuffer.size())
		{
			uncompressedBuffer.resize(uncompressedBuffer.size() + halfLength);
		}

		stream.next_out = (Bytef*)(uncompressedBuffer.data() + stream.total_out);
		stream.avail_out = (unsigned int)uncompressedBuffer.size() - (unsigned int)stream.total_out;

		// inflate the next chunk

		const int error = inflate(&stream, Z_SYNC_FLUSH);

		if (error == Z_STREAM_END)
		{
			break;
		}
		else if (error != Z_OK)
		{
			succeeded = false;
			break;
		}
	}

	if (inflateEnd(&stream) != Z_OK || !succeeded)
	{
		uncompressedBuffer.clear();
		return false;
	}

	uncompressedBuffer.resize(stream.total_out);
	return true;
}

}

}
