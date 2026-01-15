/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_VECTOR_OUTPUT_STREAM_H
#define META_OCEAN_IO_SERIALIZATION_VECTOR_OUTPUT_STREAM_H

#include "ocean/io/serialization/Serialization.h"

#include <streambuf>

namespace Ocean
{

namespace IO
{

namespace Serialization
{

/**
 * This class implements an output stream that writes to an internal vector buffer.
 * The class provides a memory-based alternative to file or network streams, allowing efficient in-memory buffering of stream data.<br>
 * This is particularly useful for scenarios where data needs to be serialized to memory before being processed or transmitted, rather than written directly to a file.<br>
 * The implementation uses a custom stream buffer (VectorStreamBuffer) that stores data in a std::vector, providing automatic memory management and resizing.
 * @ingroup ioserialization
 */
class OCEAN_IO_SERIALIZATION_EXPORT VectorOutputStream : public std::ostream
{
	public:

		/**
		 * This class implements a custom stream buffer that stores data in a vector.
		 * The class extends std::streambuf to provide a memory-backed buffer that grows dynamically as data is written.<br>
		 * It supports standard streambuf operations including overflow handling, batch writes via xsputn, and seeking within the buffer.
		 */
		class OCEAN_IO_SERIALIZATION_EXPORT VectorStreamBuffer : public std::streambuf
		{
			public:

				/// Definition of the buffer type storing the stream data.
				using Buffer = std::vector<char>;

			public:

				/**
				 * Creates a new vector stream buffer with an empty buffer.
				 */
				VectorStreamBuffer() = default;

				/**
				 * Creates a new vector stream buffer by moving an existing buffer.
				 * @param buffer The buffer to be moved into this stream buffer
				 */
				explicit VectorStreamBuffer(Buffer&& buffer);

				/**
				 * Creates a new vector stream buffer with a specified initial capacity.
				 * @param capacity The initial reserved capacity of the buffer, in bytes, with range [0, infinity)
				 */
				explicit VectorStreamBuffer(const size_t capacity);

				/**
				 * Writes a single character to the buffer when the put area is full.
				 * @param character The character to write
				 * @return The character written, or EOF on failure
				 */
				int_type overflow(int_type character) override;

				/**
				 * Writes a sequence of characters to the buffer.
				 * @param data The pointer to the data to write, must be valid
				 * @param size The number of bytes to write, with range [0, infinity)
				 * @return The number of bytes actually written
				 */
				std::streamsize xsputn(const char* data, std::streamsize size) override;

				/**
				 * Repositions the stream position indicator using relative offsets.
				 * @param offset The offset to apply, can be positive or negative
				 * @param direction The reference position (beginning, current, or end)
				 * @param mode The open mode (input, output, or both)
				 * @return The new position, or pos_type(off_type(-1)) on failure
				 */
				pos_type seekoff(off_type offset, std::ios_base::seekdir direction, std::ios_base::openmode mode = std::ios_base::out) override;

				/**
				 * Repositions the stream position indicator to an absolute position.
				 * @param position The absolute position to seek to
				 * @param mode The open mode (input, output, or both)
				 * @return The new position, or pos_type(off_type(-1)) on failure
				 */
				pos_type seekpos(pos_type position, std::ios_base::openmode mode = std::ios_base::out) override;

				/**
				 * Returns the current size of the buffer.
				 * @return The number of bytes currently stored in the buffer, with range [0, infinity)
				 */
				size_t size() const;

				/**
				 * Reserves memory for the buffer to avoid reallocations during writing.
				 * @param capacity The desired capacity in bytes, with range [0, infinity)
				 */
				void reserve(const size_t capacity);

				/**
				 * Returns a const reference to the internal buffer.
				 * @return The const reference to the internal vector buffer
				 */
				const Buffer& buffer() const;

				/**
				 * Returns a reference to the internal buffer.
				 * @return The reference to the internal vector buffer
				 */
				Buffer& buffer();

				/**
				 * Clears the buffer content.
				 */
				void clear();

			protected:

				/// The internal vector storing the stream data.
				Buffer buffer_;

				/// The current position in the buffer for writing, with range [0, infinity)
				size_t currentPosition_ = 0;
		};

	public:

		/**
		 * Creates a new vector output stream with an empty buffer.
		 */
		VectorOutputStream();

		/**
		 * Creates a new vector output stream with a specified initial buffer capacity.
		 * @param capacity The initial capacity of the buffer, in bytes, with range [0, infinity)
		 */
		explicit VectorOutputStream(const size_t capacity);

		/**
		 * Returns a pointer to the stream data.
		 * @return The pointer to the internal buffer data, nullptr if the buffer is empty
		 */
		const void* data() const;

		/**
		 * Returns the current size of the stream.
		 * @return The number of bytes currently stored in the stream, with range [0, infinity)
		 */
		size_t size() const;

		/**
		 * Clears the stream content.
		 */
		void clear();

	protected:

		/// The internal stream buffer storing the data.
		VectorStreamBuffer streamBuffer_;
};

inline VectorOutputStream::VectorStreamBuffer::VectorStreamBuffer(Buffer&& buffer) :
	buffer_(std::move(buffer))
{
	// nothing to do here
}

inline VectorOutputStream::VectorStreamBuffer::VectorStreamBuffer(const size_t capacity)
{
	buffer_.reserve(capacity);
}

inline size_t VectorOutputStream::VectorStreamBuffer::size() const
{
	return buffer_.size();
}

inline void VectorOutputStream::VectorStreamBuffer::reserve(const size_t capacity)
{
	buffer_.reserve(capacity);
}

inline const VectorOutputStream::VectorStreamBuffer::Buffer& VectorOutputStream::VectorStreamBuffer::buffer() const
{
	return buffer_;
}

inline VectorOutputStream::VectorStreamBuffer::Buffer& VectorOutputStream::VectorStreamBuffer::buffer()
{
	return buffer_;
}

inline void VectorOutputStream::VectorStreamBuffer::clear()
{
	buffer_.clear();
	currentPosition_ = 0;
}

inline VectorOutputStream::VectorOutputStream() :
	std::ostream(&streamBuffer_)
{
	// nothing to do here
}

inline VectorOutputStream::VectorOutputStream(const size_t capacity) :
	std::ostream(&streamBuffer_)
{
	streamBuffer_.reserve(capacity);
}

inline const void* VectorOutputStream::data() const
{
	return streamBuffer_.buffer().data();
}

inline size_t VectorOutputStream::size() const
{
	return streamBuffer_.size();
}

inline void VectorOutputStream::clear()
{
	streamBuffer_.clear();
}

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_VECTOR_OUTPUT_STREAM_H
