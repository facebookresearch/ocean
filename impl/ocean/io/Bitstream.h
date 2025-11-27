/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_BITSTREAM_H
#define META_OCEAN_IO_BITSTREAM_H

#include "ocean/io/IO.h"

#include <istream>
#include <ostream>

namespace Ocean
{

namespace IO
{

/**
 * This class implements an input bitstream.
 * The implementation of the input bitstream is not thread-safe, thus ensure that the internal input stream object is not used within several threads concurrently.<br>
 * The following data types are supported:
 * <pre>
 * Data type:          Number of bytes:      Description:
 * bool                1
 * char                1
 * wchar_t             4                     Depending on the platform wchar_t has 1, 2 or 4 bytes thus this stream object stores 4 bytes per wchar_t object (see also std::wstring)
 * unsigned char       1
 * short               2
 * unsigned short      2
 * int                 4
 * unsigned int        4
 * float               4
 * double              8
 * long long           8
 * unsigned long long  8
 * std::string         4 + 1 * size          Four bytes for the size of the string and 1 byte for each character
 * std::wstring        4 + 4 * size          Four bytes for the size of the string and 4 bytes for each character
 * </pre>
 * The following data types are not supported as they have individual size on individual platforms and thus must not be used:
 * <pre>
 * Data type:          Description:
 * size_t              size_t has 4 bytes on 32 bit platforms and 8 bytes on 64 bit platforms
 * </pre>
 * @ingroup io
 */
class OCEAN_IO_EXPORT InputBitstream
{
	public:

		/**
		 * Creates a new bitstream object.
		 * @param stream The input stream object that is encapsulated by this object
		 */
		InputBitstream(std::istream& stream);

		/**
		 * Reads a value from the bitstream and moves the internal position inside the bitstream accordingly.
		 * If the read process fails, the new position of the bitstream may be arbitrary.<br>
		 * Beware: It's recommended to provide the template argument explicitly to avoid type ambiguities.<br>
		 * @param value The resulting value that is read
		 * @return True, if the value could be read successfully
		 * @tparam T The data type that is read, ensure that only supported data types are applied
		 * @see readDefault().
		 */
		template <typename T>
		bool read(T& value);

		/**
		 * Reads a value from the bitstream and moves the internal position inside the bitstream accordingly.
		 * If the read process fails, the new position of the bitstream may be arbitrary.<br>
		 * This function does not provide a success value but returns a given default value if the read process fails.<br>
		 * Beware: It's recommended to provide the template argument explicitly to avoid type ambiguities.<br>
		 * @param defaultValue The default value that is returned if the read process fails
		 * @return The resulting value that has been read
		 * @tparam T The data type that is read, ensure that only supported data types are applied
		 * @see read().
		 */
		template <typename T>
		T readDefault(const T& defaultValue);

		/**
		 * Reads a defined memory block from the stream into a given buffer and moves the internal position inside the bitstream accordingly.
		 * If the read process fails, the new position of the bitstream may be arbitrary.
		 * @param data The buffer that will receive the memory block, ensure that this buffer is large enough, may be nullptr if size is 0
		 * @param size The number of bytes that will be read into the memory block, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool read(void* data, const size_t size);

		/**
		 * Reads a value from the bitstream but does not move the internal position inside the bitstream.
		 * Beware: It's recommended to provide the template argument explicitly to avoid type ambiguities.<br>
		 * @param value  The resulting value that is looked-up
		 * @return True, if the value could be looked-up successfully
		 * @tparam T The data type that is read, ensure that only supported data types are applied
		 */
		template <typename T>
		bool look(T& value);

		/**
		 * Returns the current position inside the bitstream, in bytes counting from the beginning of the stream.
		 * @return The current stream position in bytes, -1 if the current position cannot be determined
		 */
		uint64_t position() const;

		/**
		 * Returns the current size of the bitstream, in bytes.
		 * @return The current stream size in bytes, -1 if the current size cannot be determined
		 */
		uint64_t size() const;

		/**
		 * Sets the current position inside the bitstream explicitly.
		 * @param position The new position inside the bitstream, in bytes counting from the beginning of the stream; with range [0, size()]
		 * @return True, if succeeded
		 */
		bool setPosition(const uint64_t position);

		/**
		 * Skips a specified number of bytes in the bitstream by moving the position forward.
		 * @param bytes The number of bytes to skip, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool skip(const uint64_t bytes);

		/**
		 * Returns whether the current position is at the end of the bitstream.
		 * @return True, if the end of the stream has been reached
		 */
		bool isEndOfFile() const;

		/**
		 * Returns whether this bitstream object is valid and can be used.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The internal input stream object that this object encapsulates.
		std::istream& inputStream_;
};

/**
 * This class implements a scoped input bitstream that allows to read content from a bitstream while the stream position is restored if this object is disposed.
 * The implementation of this scoped input bitstream object is not thread-safe.
 * @see InputBitstream
 * @ingroup io
 */
class ScopedInputBitstream : public InputBitstream
{
	public:

		/**
		 * Creates a new scoped input bitstream object from a given input bitstream object and stores the current stream position of the given stream object.
		 * @param stream The input bitstream object that is encapsulated by this object
		 */
		inline ScopedInputBitstream(InputBitstream& stream);

		/**
		 * Destructs this scoped object and restores the stream position of the original input bitstream object.
		 */
		inline ~ScopedInputBitstream();

	protected:

		/**
		 * Deleted copy constructor.
		 */
		ScopedInputBitstream(const ScopedInputBitstream&) = delete;

		/**
		 * Deleted move constructor.
		 */
		inline ScopedInputBitstream(ScopedInputBitstream&&) = delete;

		/**
		 * Deleted copy operator.
		 * @return Reference to this object
		 */
		ScopedInputBitstream& operator=(const ScopedInputBitstream&) = delete;

		/**
		 * Deleted move operator.
		 * @return Reference to this object
		 */
		ScopedInputBitstream& operator=(ScopedInputBitstream&&) = delete;

	protected:

		/// The original stream position of the original bitstream object.
		uint64_t streamStartPosition_ = uint64_t(-1);
};

/**
 * This class implements an output bitstream.
 * The implementation of the output bitstream is not thread-safe, thus ensure that the internal output stream object is not used within several threads concurrently.<br>
 * The bitstream supports specific data types, see InputBitstream for detailed information.
 * @ingroup io
 */
class OCEAN_IO_EXPORT OutputBitstream
{
	public:

		/**
		 * Creates a new output bitstream object.
		 * @param stream The output stream object that is encapsulated by this object
		 */
		OutputBitstream(std::ostream& stream);

		/**
		 * Writes a data object to the stream and moves the internal position inside the bitstream accordingly.
		 * If the write process fails, the new position of the bitstream may be arbitrary.<br>
		 * Beware: It's recommended to provide the template argument explicitly to avoid type ambiguities.<br>
		 * @param value The value that will be written
		 * @return True, if the value could be written successfully
		 * @tparam T the data type that is written, ensure that only supported data types are applied
		 */
		template <typename T>
		bool write(const T& value);

		/**
		 * Writes a defined memory block from a given buffer into the stream and moves the internal position inside the bitstream accordingly.
		 * If the write process fails, the new position of the bitstream may be arbitrary.
		 * @param data The buffer that will be written, may be nullptr if size is 0
		 * @param size The number of bytes that will be written, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool write(const void* data, const size_t size);

		/**
		 * Returns the current size of the bitstream, in bytes.
		 * @return The current stream size in bytes, -1 if the current size cannot be determined
		 */
		uint64_t size() const;

		/**
		 * Returns whether this bitstream object is valid and can be used.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The internal output stream object that this object encapsulates.
		std::ostream& outputStream_;
};

/**
 * This class implements a tag that allows to identify specific objects in an input/output stream.
 * A tag is a 64-bit identifier that identifies one unique object type.
 * @ingroup io
 */
class OCEAN_IO_EXPORT Tag
{
	protected:

#ifdef OCEAN_DEBUG

		/**
		 * This class implements a tag manager that allows to ensure that tags are not defined twice.
		 */
		class TagManager
		{
			protected:

				/**
				 * Definition of a set holding tag values.
				 */
				using TagSet = std::unordered_set<unsigned long long>;

			public:

				/**
				 * Registers a new tag value.
				 * @param tag The tag value that will be registered
				 * @return True, if the tag value hasn't been registered before
				 */
				static inline bool registerTag(const unsigned long long tag);

			protected:

				/// The set of registered tag values.
				TagSet tagsSet_;
		};

#endif // OCEAN_DEBUG

	public:

		/**
		 * Creates a new invalid tag object.
		 */
		inline Tag();

		/**
		 * Creates a new tag object by a given eight character string.
		 * Use this constructor only for the definition of a tag.
		 * @param tagString A string with exactly eight characters which are used to define the unique tag value
		 */
		inline explicit Tag(const char tagString[8]);

		/**
		 * Returns whether two tag objects are identical.
		 * @param tag The second tag to be compared
		 * @return True, if so
		 */
		inline bool operator==(const Tag& tag) const;

		/**
		 * Returns whether two tag objects are not identical.
		 * @param tag The second tag to be compared
		 * @return True, if so
		 */
		inline bool operator!=(const Tag& tag) const;

		/**
		 * Returns the unique 64 bit tag value of this tag object.
		 * @return The tag value
		 */
		inline unsigned long long value() const;

		/**
		 * Returns the unique 64 bit tag value of this tag object.
		 * @return The tag value
		 */
		inline unsigned long long& value();

		/**
		 * Writes a tag to a bitstream.
		 * @param bitstream The bitstream to which the tag is written
		 * @param tag The tag to be written
		 * @return True, if succeeded
		 */
		static bool writeTag(OutputBitstream& bitstream, const Tag& tag);

		/**
		 * Reads a tag from a bitstream.
		 * The position in the bitstream is moved forward after the tag has been read.<br>
		 * @param bitstream The bitstream from which the tag is read
		 * @param tag The resulting bitstream
		 * @return True, if succeeded
		 */
		static bool readTag(InputBitstream& bitstream, Tag& tag);

		/**
		 * Reads (only a look) a tag from a bitstream.
		 * The position in the bitstream is not modified.<br>
		 * @param bitstream The bitstream from which the tag is read
		 * @param tag The resulting bitstream
		 * @return True, if succeeded
		 */
		static bool lookTag(InputBitstream& bitstream, Tag& tag);

		/**
		 * Reads a tag from a bitstream and checks whether the tag is identical with an expectedTag tag.<br>
		 * The position in the bitstream is moved forward after the tag has been read.<br>
		 * @param bitstream The bitstream from which the tag is read
		 * @param expectedTag The expected tag that should match the tag that has been read
		 * @return True, if received tag and the expected tag are identical
		 */
		static bool readAndCheckTag(InputBitstream& bitstream, const Tag& expectedTag);

		/**
		 * Reads (only a look) a tag from a bitstream and checks whether the tag is identical with an expectedTag tag.<br>
		 * The position in the bitstream is not modified.<br>
		 * @param bitstream The bitstream from which the tag is read
		 * @param expectedTag The expected tag that should match the tag that has been read
		 * @return True, if received tag and the expected tag are identical
		 */
		static bool lookAndCheckTag(InputBitstream& bitstream, const Tag& expectedTag);

		/**
		 * Converts a string with exactly eight characters to a unique tag value.
		 * @param tagString The string with eight characters to be converted
		 * @return The resulting tag value
		 */
		static constexpr unsigned long long string2tag(const char tagString[8]);

	protected:

		/// The tag value of this tag object.
		unsigned long long tagValue_;
};

inline ScopedInputBitstream::ScopedInputBitstream(InputBitstream& stream) :
	InputBitstream(stream)
{
	streamStartPosition_ = position();
}

inline ScopedInputBitstream::~ScopedInputBitstream()
{
	if (streamStartPosition_ != uint64_t(-1))
	{
		setPosition(streamStartPosition_);
	}
}

inline InputBitstream::operator bool() const
{
	return inputStream_.good();
}

inline OutputBitstream::operator bool() const
{
	return outputStream_.good();
}

inline Tag::Tag() :
	tagValue_(0ull)
{
	// nothing to do here
}

inline Tag::Tag(const char tagString[8]) :
	tagValue_(string2tag(tagString))
{
	ocean_assert(TagManager::registerTag(tagValue_));
}

inline bool Tag::operator==(const Tag& tag) const
{
	return tagValue_ == tag.tagValue_;
}

inline bool Tag::operator!=(const Tag& tag) const
{
	return !(*this == tag);
}

inline unsigned long long Tag::value() const
{
	return tagValue_;
}

inline unsigned long long& Tag::value()
{
	return tagValue_;
}

constexpr unsigned long long Tag::string2tag(const char tagString[8])
{
	ocean_assert(tagString[0] != 0 && tagString[1] != 0 && tagString[2] != 0 && tagString[3] != 0
				&& tagString[4] != 0 && tagString[5] != 0 && tagString[6] != 0 && tagString[7] != 0);

	return ((unsigned long long)tagString[0] << 0ull)
				| ((unsigned long long)tagString[1] << 8ull)
				| ((unsigned long long)tagString[2] << 16ull)
				| ((unsigned long long)tagString[3] << 24ull)
				| ((unsigned long long)tagString[4] << 32ull)
				| ((unsigned long long)tagString[5] << 40ull)
				| ((unsigned long long)tagString[6] << 48ull)
				| ((unsigned long long)tagString[7] << 56ull);
}

#ifdef OCEAN_DEBUG

inline bool Tag::TagManager::registerTag(const unsigned long long tag)
{
	static TagManager manager;

	const bool newTag = manager.tagsSet_.find(tag) == manager.tagsSet_.end();
	manager.tagsSet_.insert(tag);

	return newTag;
}

#endif // OCEAN_DEBUG

}

}

#endif // META_OCEAN_IO_BITSTREAM_H
