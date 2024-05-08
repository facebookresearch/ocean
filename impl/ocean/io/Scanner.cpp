/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Scanner.h"

#include "ocean/base/Exception.h"
#include "ocean/base/String.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Ocean
{

namespace IO
{

Scanner::Token::Token(std::string&& data, const Type type) :
	type_(type),
	id_(invalidId),
	data_(std::move(data))
{
	// nothing to do here
}

Scanner::Token::Token(const std::string& data, const Type type) :
	type_(type),
	id_(invalidId),
	data_(data)
{
	// nothing to do here
}

Scanner::Token::Token(std::string&& data, const uint32_t id, const Type type) :
	type_(type),
	id_(id),
	data_(std::move(data))
{
	ocean_assert(type_ == TOKEN_KEYWORD || type_ == TOKEN_SYMBOL);
}

Scanner::Token::Token(const std::string& data, const uint32_t id, const Type type) :
	type_(type),
	id_(id),
	data_(data)
{
	ocean_assert(type_ == TOKEN_KEYWORD || type_ == TOKEN_SYMBOL);
}

bool Scanner::Token::isKeyword(const uint32_t keyword) const
{
	return type_ == TOKEN_KEYWORD && keyword == id_;
}

bool Scanner::Token::isSymbol(const uint32_t symbol) const
{
	return type_ == TOKEN_SYMBOL && symbol == id_;
}

uint8_t Scanner::Token::character() const
{
	if (type_ != TOKEN_CHARACTER)
	{
		ocean_assert(false && "The token holds no character.");
		throw OceanException("The token holds no character.");
	}

	ocean_assert(data_.size() == 1);
	return uint8_t(data_[0]);
}

const std::string& Scanner::Token::identifier() const
{
	if (type_ != TOKEN_IDENTIFIER)
	{
		ocean_assert(false && "The token holds no identifier.");
		throw OceanException("The token holds no identifier.");
	}

	ocean_assert(data_.empty() == false);
	return data_;
}

int Scanner::Token::integer() const
{
	if (type_ != TOKEN_INTEGER)
	{
		ocean_assert(false && "The token holds no integer.");
		throw OceanException("The token holds no integer.");
	}

	ocean_assert(data_.empty() == false);
	return atoi(data_.c_str());
}

Scalar Scanner::Token::number() const
{
	if (type_ != TOKEN_NUMBER)
	{
		ocean_assert(false && "The token holds no number.");
		throw OceanException("The token holds no number.");
	}

	ocean_assert(data_.empty() == false);
	return Scalar(atof(data_.c_str()));
}

Scalar Scanner::Token::integerOrNumber() const
{
	if (type_ == TOKEN_INTEGER)
	{
		return Scalar(integer());
	}

	return number();
}

const std::string& Scanner::Token::line() const
{
	if (type_ != TOKEN_LINE)
	{
		ocean_assert(false && "The token holds no remaining line.");
		throw OceanException("The token holds no remaining line.");
	}

	return data_;
}

uint32_t Scanner::Token::keyword() const
{
	if (type_ != TOKEN_KEYWORD)
	{
		ocean_assert(false && "The token holds no keyword.");
		throw OceanException("The token holds no keyword.");
	}

	ocean_assert(id_ != invalidId);
	return id_;
}

const std::string& Scanner::Token::string() const
{
	if (type_ != TOKEN_STRING)
	{
		ocean_assert(false && "The token holds no string.");
		throw OceanException("The token holds no string.");
	}

	return data_;
}

std::string Scanner::Token::moveString()
{
	if (type_ != TOKEN_STRING)
	{
		ocean_assert(false && "The token holds no string.");
		throw OceanException("The token holds no string.");
	}

	type_ = TOKEN_INVALID;

	return std::move(data_);
}

uint32_t Scanner::Token::symbol() const
{
	if (type_ != TOKEN_SYMBOL)
	{
		ocean_assert(false && "The token holds no symbol.");
		throw OceanException("The token holds no symbol.");
	}

	ocean_assert(id_ != invalidId);
	return id_;
}

Scanner::Scanner(float* progress, bool* cancel) :
	progress_(progress),
	cancel_(cancel)
{
	/**
	 * CHAR_INVALID = 0,
	 * CHAR_CHARACTER = 1,
	 * CHAR_IDENTIFIER = 2,
	 * CHAR_NUMBER = 4,
	 * CHAR_INTEGER = 8,
	 * (CHAR_KEYWORD = 16),
	 * CHAR_STRING = 32,
	 * (CHAR_SYMBOL = 64),
	 * CHAR_REMARK = 128,
	 * CHAR_SPACE = 256
	 */

	firstCharTable_ =
	{{
		//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
		// NUL SOH STX ETX EOT ENQ ACK BEL  BS  HT  LF  VT  FF  CR  SO  SI
			 0,  0,  0,  0,  0,  0,  0,  0,  0,256,256,  0,  0,256,  0,  0, // 0
		// DLE DC1 DC2 DC3 DC$ NAK SYN ETB CAN  EM SUB ESC  FS  GS  RS  US
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 1
		// ' '   !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
		   256,  0, 32,  2,  2,  0,  0,  1,  0,  0,  0,  0,  0, 12,  4,  0, // 2
		//   0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
			12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  0,  0,  0,  0,  0,  0, // 3
		//   @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // 4
		//   P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0,  2, // 5
		//   `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
			 0,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // 6
		//   p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0,  0, // 7
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 8
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 9
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // A
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // B
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // C
			 2,  2,  2,  2,  2,  2,  2,  0,  2,  2,  2,  2,  2,  2,  2,  2, // D
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // E
			 2,  2,  2,  2,  2,  2,  2,  0,  2,  2,  2,  2,  2,  2,  2,  2  // F
	}};

	followingCharTable_ =
	{{
		//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
		// NUL SOH STX ETX EOT ENQ ACK BEL  BS  HT  LF  VT  FF  CR  SO  SI
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,256,  0,  0,  0,  0,  0, // 0
		// DLE DC1 DC2 DC3 DC$ NAK SYN ETB CAN  EM SUB ESC  FS  GS  RS  US
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 1
		// ' '   !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
		   256,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0, // 2
		//   0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
			14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  0,  0,  0,  0,  0,  0, // 3
		//   @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // 4
		//   P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0,  2, // 5
		//   `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
			 0,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // 6
		//   p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0,  0, // 7
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 8
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 9
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // A
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // B
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // C
			 2,  2,  2,  2,  2,  2,  2,  0,  2,  2,  2,  2,  2,  2,  2,  2, // D
			 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, // E
			 2,  2,  2,  2,  2,  2,  2,  0,  2,  2,  2,  2,  2,  2,  2,  2  // F
	}};

	invalidCharTable_ =
	{{
		//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
		// NUL SOH STX ETX EOT ENQ ACK BEL  BS  HT  LF  VT  FF  CR  SO  SI
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0
		// DLE DC1 DC2 DC3 DC$ NAK SYN ETB CAN  EM SUB ESC  FS  GS  RS  US
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 1
		// ' '   !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
			 0,  0, 15, 12, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  0, // 2
		//   0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 3
		//   @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
			 0, 12, 12, 12, 12,  8, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, // 4
		//   P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
			12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  0,  0,  0,  0,  0, // 5
		//   `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
			 0, 12, 12, 12, 12,  8, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, // 6
		//   p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
			12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  0,  0,  0,  0,  0, // 7
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 8
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 9
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // A
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // B
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // C
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // D
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // E
			 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  // F
	}};

	intermediateBuffer_ = Memory(maxBufferSize_);
	intermediateBufferPointer_ = intermediateBuffer_.data<uint8_t>();
}

Scanner::Scanner(const std::shared_ptr<std::istream>& stream, float* progress, bool* cancel) :
	Scanner(progress, cancel)
{
	if (!stream || !stream->fail())
	{
		// skip possible utf8 bit order mask if exists
		if (stream->peek() == 0xEF)
		{
			const uint8_t value0 = uint8_t(stream->get());
			ocean_assert_and_suppress_unused(value0 == 0xEF, value0);

			if (stream->peek() != 0xBB)
			{
				stream->putback(char(uint8_t(0xEF)));
			}
			else
			{
				const uint8_t value1 = uint8_t(stream->get());
				ocean_assert_and_suppress_unused(value1 == 0xBB, value1);

				if (stream->peek() != 0xBF)
				{
					stream->putback(char(uint8_t(0xBB)));
					stream->putback(char(uint8_t(0xEF)));
				}
				else
				{
					const uint8_t value2 = uint8_t(stream->get());
					ocean_assert_and_suppress_unused(value2 == 0xBF, value2);
				}
			}
		}

		stream_ = stream;
	}
}

Scanner::~Scanner()
{
	// nothing to do here
}

const Scanner::Token& Scanner::token()
{
	if (recentToken_.isType(Token::TOKEN_INVALID))
	{
		recentToken_ = readToken();
	}

	return recentToken_;
}

const Scanner::Token& Scanner::lineToken()
{
	readLine(recentToken_, true);
	return recentToken_;
}

Scanner::Token Scanner::tokenPop()
{
	const Token result(token());
	pop();

	return result;
}

const Scanner::Token& Scanner::look()
{
	if (nextToken_.isType(Token::TOKEN_INVALID))
	{
		nextToken_ = readToken(false);
	}

	return nextToken_;
}

void Scanner::pop()
{
	if (nextToken_.isType(Token::TOKEN_INVALID) == false)
	{
		recentToken_ = nextToken_;

		ocean_assert(line_ != 0 && column_ != 0);
		nextToken_ = Token();
	}
	else
	{
		recentToken_ = Token();
	}
}

size_t Scanner::position() const
{
	return position_;
}

size_t Scanner::size() const
{
	if (!stream_)
	{
		return 0;
	}

	std::istream::pos_type pos = stream_->tellg();

	stream_->seekg(0, std::ios_base::end);
	const std::istream::pos_type endPosition = stream_->tellg();
	stream_->seekg(std::istream::off_type(pos), std::ios_base::beg);

	return size_t(endPosition);
}

bool Scanner::findNextToken(const char* pointer, const size_t size, const size_t start, size_t& tokenStart, size_t& tokenLength)
{
	ocean_assert(pointer != nullptr && size > 0);

	if (start >= size)
	{
		return false;
	}

	size_t posStart = start;

	while (isWhitespace(pointer[posStart]))
	{
		++posStart;

		if (posStart >= size)
		{
			return false;
		}
	}

	size_t posEnd = posStart + 1;

	while (posEnd < size && !isWhitespace(pointer[posEnd]))
	{
		++posEnd;
	}

	tokenStart = posStart;
	tokenLength = posEnd - posStart;

	return true;
}

bool Scanner::findNextToken(const char* pointer, const size_t start, size_t& tokenStart, size_t& tokenLength)
{
	if (pointer == nullptr || pointer[start] == '\0')
	{
		return false;
	}

	size_t posStart = start;

	while (isWhitespace(pointer[posStart]))
	{
		++posStart;

		if (pointer[posStart] == '\0')
		{
			return false;
		}
	}

	size_t posEnd = posStart + 1;

	while (pointer[posEnd] != '\0' && !isWhitespace(pointer[posEnd]))
	{
		++posEnd;
	}

	tokenStart = posStart;
	tokenLength = posEnd - posStart;

	return pointer[posEnd] != '\0';
}

uint8_t Scanner::get(const size_t offset)
{
	if (intermediateBufferSize_ > offset)
	{
		return intermediateBufferPointer_[offset];
	}

	if (intermediateBufferSize_ < minBufferSize_)
	{
		if (refillIntermediateBuffer())
		{
			if (intermediateBufferSize_ > offset)
			{
				return intermediateBufferPointer_[offset];
			}
		}
		else
		{
			return uint8_t(0);
		}
	}

	return getExtra(offset - intermediateBufferSize_);
}

std::string Scanner::data(const size_t size) const
{
	if (size <= intermediateBufferSize_)
	{
		return std::string((const char*)(intermediateBufferPointer_), size);
	}

	if (size <= intermediateBufferSize_ + extraBufferSize_)
	{
		ocean_assert(extraBufferPointer_ != nullptr || size - intermediateBufferSize_ == 0);

		return std::string((const char*)(intermediateBufferPointer_), intermediateBufferSize_) + std::string((const char*)(extraBufferPointer_), size - intermediateBufferSize_);
	}

	ocean_assert(false && "Out of range.");
	throw OceanException("Out of range.");
}

std::string Scanner::data(const size_t offset, const size_t size) const
{
	if (offset + size <= intermediateBufferSize_)
	{
		return std::string((const char*)(intermediateBufferPointer_) + offset, size);
	}

	if (offset + size <= intermediateBufferSize_ + extraBufferSize_)
	{
		ocean_assert(extraBufferPointer_ != nullptr || size - intermediateBufferSize_ + offset == 0);

		return std::string((const char*)(intermediateBufferPointer_) + offset, intermediateBufferSize_ - offset) + std::string((const char*)(extraBufferPointer_), size - intermediateBufferSize_ + offset);
	}

	ocean_assert(false && "Out of range.");
	throw OceanException("Out of range.");
}

void Scanner::consume(const size_t chars)
{
	ocean_assert(chars != 0);

	if (chars <= intermediateBufferSize_)
	{
		intermediateBufferPointer_ += chars;
		column_ += chars;
		position_ += chars;
		intermediateBufferSize_ -= chars;
	}
	else
	{
		intermediateBufferPointer_ += intermediateBufferSize_;
		column_ += chars;
		position_ += chars;

		size_t consumeExtraChars = chars - intermediateBufferSize_;
		intermediateBufferSize_ = 0;

		extraBufferPointer_ += consumeExtraChars;
		extraBufferSize_ -= consumeExtraChars;
	}
}

bool Scanner::refillIntermediateBuffer()
{
	ocean_assert(stream_);
	ocean_assert(intermediateBufferSize_ < minBufferSize_);

	bool result = true;

	if (progress_)
	{
		const size_t total = size();
		if (total != 0)
		{
			*progress_ = float(position()) / float(total);
		}
	}

	// moving pending buffer to the begin
	ocean_assert(intermediateBuffer_.isInside(intermediateBuffer_.data(), intermediateBufferSize_));
	ocean_assert(intermediateBuffer_.isInside(intermediateBufferPointer_, intermediateBufferSize_));

	memmove(intermediateBuffer_.data(), intermediateBufferPointer_, intermediateBufferSize_);

	if (extraBuffer_)
	{
		ocean_assert(extraBufferPointer_ != nullptr);
		ocean_assert(extraBufferSize_ != 0);

		const size_t copyFromExtraBuffer = min(extraBufferSize_ - intermediateBufferSize_, extraBufferSize_);
		ocean_assert(extraBufferSize_ >= copyFromExtraBuffer);

		ocean_assert(intermediateBuffer_.isInside(intermediateBuffer_.data<uint8_t>() + intermediateBufferSize_, copyFromExtraBuffer));
		ocean_assert(extraBuffer_.isInside(extraBufferPointer_, copyFromExtraBuffer));

		memcpy(intermediateBuffer_.data<uint8_t>() + intermediateBufferSize_, extraBufferPointer_, copyFromExtraBuffer);

		extraBufferPointer_ += copyFromExtraBuffer;
		extraBufferSize_ -= copyFromExtraBuffer;
		intermediateBufferSize_ += copyFromExtraBuffer;

		if (extraBufferSize_ == 0)
		{
			extraBuffer_.free();
			extraBufferPointer_ = nullptr;

			if (intermediateBufferSize_ < maxBufferSize_)
			{
				stream_->read(intermediateBuffer_.data<char>() + intermediateBufferSize_, std::streamsize(maxBufferSize_ - intermediateBufferSize_));
				intermediateBufferSize_ += size_t(stream_->gcount());
			}
		}
	}
	else
	{
		ocean_assert(extraBufferPointer_ == nullptr);
		ocean_assert(extraBufferSize_ == 0);

		stream_->read(intermediateBuffer_.data<char>() + intermediateBufferSize_, std::streamsize(maxBufferSize_ - intermediateBufferSize_));
		intermediateBufferSize_ += size_t(stream_->gcount());
		result = stream_->gcount() != 0;
	}

	intermediateBufferPointer_ = intermediateBuffer_.data<uint8_t>();
	return result;
}

uint8_t Scanner::getExtra(const size_t offset)
{
	if (offset < extraBufferSize_ || refillExtraBuffer(offset))
	{
		return extraBufferPointer_[offset];
	}

	return uint8_t(0);
}

bool Scanner::refillExtraBuffer(const size_t minIndex)
{
	ocean_assert(stream_);

	size_t newExtraBufferSize = maxBufferSize_ * (1 + (minIndex + 1) / maxBufferSize_);
	Memory newExtraBuffer(newExtraBufferSize);

	if (!newExtraBuffer)
	{
		throw OutOfMemoryException("Not enough memory to allocate a new extra buffer.");
	}

	ocean_assert(newExtraBuffer.isInside(newExtraBuffer.data(), extraBufferSize_));
	ocean_assert(extraBuffer_.isInside(extraBufferPointer_, extraBufferSize_));

	// copy pending buffer to the begin
	memcpy(newExtraBuffer.data(), extraBufferPointer_, extraBufferSize_);

	stream_->read(newExtraBuffer.data<char>() + extraBufferSize_, std::streamsize(newExtraBufferSize - extraBufferSize_));

	extraBuffer_ = std::move(newExtraBuffer);
	extraBufferPointer_ = extraBuffer_.data<uint8_t>();
	extraBufferSize_ += size_t(stream_->gcount());

	return extraBufferSize_ > minIndex;
}

uint32_t Scanner::keywordId(const std::string& data) const
{
	const IdMap::const_iterator i = keywordMap_.find(data);

	if (i == keywordMap_.cend())
	{
		return invalidId;
	}

	return i->second;
}

uint32_t Scanner::symbolId(const std::string& data) const
{
	const IdMap::const_iterator i = symbolMap_.find(data);

	if (i == symbolMap_.cend())
	{
		return invalidId;
	}

	return i->second;
}

void Scanner::setKeywordProperty(const bool caseSensitive)
{
	ocean_assert(keywordMap_.empty());

	keywordsAreCaseSensitive_ = caseSensitive;
}

void Scanner::registerKeyword(const std::string& keyword, const uint32_t id)
{
	std::string value = keyword;

	if (keywordsAreCaseSensitive_ == false)
	{
		value = String::toLower(keyword);
	}

	ocean_assert(keywordMap_.find(value) == keywordMap_.cend());
	ocean_assert(symbolMap_.find(value) == symbolMap_.cend());

	ocean_assert(id != invalidId);
	ocean_assert(keyword.empty() == false);

	keywordMap_[value] = id;

	firstCharTable_[value[0]] |= CHAR_KEYWORD;

	for (size_t n = 1u; n < keyword.size(); ++n)
	{
		followingCharTable_[value[n]] |= CHAR_KEYWORD;
	}
}

void Scanner::registerSymbol(const std::string& symbol, const uint32_t id)
{
	ocean_assert(symbolMap_.find(symbol) == symbolMap_.end());
	ocean_assert(keywordMap_.find(symbol) == keywordMap_.end());

	ocean_assert(id != invalidId);
	ocean_assert(symbol.empty() == false);

	symbolMap_[symbol] = id;

	firstCharTable_[symbol[0]] |= CHAR_SYMBOL;

	for (size_t n = 1u; n < symbol.size(); ++n)
	{
		followingCharTable_[symbol[n]] |= CHAR_SYMBOL;
	}
}

void Scanner::registerLineRemark(const std::string& lineRemark)
{
	ocean_assert(lineRemarks_.find(lineRemark) == lineRemarks_.cend());
	ocean_assert(lineRemark.empty() == false);

	if (lineRemark.empty())
	{
		return;
	}

	lineRemarks_.insert(lineRemark);
	maximalLengthLineRemarks_ = max(maximalLengthLineRemarks_, lineRemark.size());

	firstCharTable_[lineRemark[0]] |= CHAR_REMARK;

	for (size_t n = 1; n < lineRemark.size(); ++n)
	{
		followingCharTable_[lineRemark[n]] |= CHAR_REMARK;
	}
}

void Scanner::registerScopeRemark(const std::string& begin, const std::string& end)
{
	ocean_assert(scopeRemarks_.find(begin) == scopeRemarks_.cend());
	ocean_assert(begin.empty() == false);
	ocean_assert(end.empty() == false);

	if (begin.empty() || end.empty())
	{
		return;
	}

	scopeRemarks_[begin] = end;
	maximalLengthScopeRemarks_ = max(maximalLengthScopeRemarks_, begin.size());

	firstCharTable_[begin[0]] |= CHAR_REMARK;

	for (size_t n = 1; n < begin.size(); ++n)
	{
		followingCharTable_[begin[n]] |= CHAR_REMARK;
	}
}

bool Scanner::registerWhiteSpaceCharacter(const uint8_t character)
{
	if (firstCharTable_[character] != CHAR_INVALID)
	{
		return false;
	}

	firstCharTable_[character] = CHAR_SPACE;
	return true;
}

Scanner::Token Scanner::readToken(const bool consumeBytes)
{
	// if a cancel flag is provided an the scanner has to be terminated explicitly
	if (cancel_ && *cancel_)
	{
		return Token("end of file", Token::TOKEN_END_OF_FILE);
	}

	uint8_t c = get();
	Token token;

	if (firstCharTable_[c] & CHAR_SPACE || firstCharTable_[c] & CHAR_REMARK)
	{
		c = readWhiteSpace();
	}

	if (c == CHAR_INVALID)
	{
		return Token("end of file", Token::TOKEN_END_OF_FILE);
	}

	if (firstCharTable_[c] & CHAR_CHARACTER)
	{
		if (readCharacter(token, consumeBytes))
		{
			return token;
		}

		return Token();
	}

	if (firstCharTable_[c] & CHAR_STRING)
	{
		if (readString(token, consumeBytes))
		{
			return token;
		}

		return Token();
	}

	if (firstCharTable_[c] & CHAR_INTEGER)
	{
		if (readInteger(token, consumeBytes))
		{
			return token;
		}
	}

	if (firstCharTable_[c] & CHAR_NUMBER)
	{
		if (readNumber(token, consumeBytes))
		{
			return token;
		}
	}

	if (firstCharTable_[c] & CHAR_KEYWORD || (!keywordsAreCaseSensitive_ && firstCharTable_[tolower(c)] & CHAR_KEYWORD))
	{
		if (readKeyword(token, consumeBytes))
		{
			return token;
		}
	}

	if (firstCharTable_[c] & CHAR_SYMBOL)
	{
		if (readSymbol(token, consumeBytes))
		{
			return token;
		}
	}

	if (firstCharTable_[c] & CHAR_IDENTIFIER)
	{
		if (readIdentifier(token, consumeBytes))
		{
			return token;
		}
	}

	ocean_assert(token.isType(Token::TOKEN_INVALID));
	return Token(discardNonWhiteSpace(), Token::TOKEN_INVALID);
}

uint8_t Scanner::readWhiteSpace(bool crossLines)
{
	uint8_t c = get();
	ocean_assert(firstCharTable_[c] & CHAR_SPACE || firstCharTable_[c] & CHAR_REMARK);

	while (true)
	{
		if (firstCharTable_[c] & CHAR_SPACE)
		{
			if (c == 10)
			{
				if (crossLines == false)
				{
					break;
				}

				++line_;
				column_ = 1;

				consume(1);
			}
			else if (c == 13 && get(1) == 10)
			{
				if (crossLines == false)
				{
					break;
				}

				++line_;
				column_ = 1;

				consume(2);
			}
			else
			{
				consume();
			}
		}
		else if (firstCharTable_[c] & CHAR_REMARK)
		{
			if (readRemark() == false)
			{
				break;
			}
		}
		else
		{
			break;
		}

		c = get();
	}

	return c;
}

std::string Scanner::discardNonWhiteSpace()
{
	size_t position = 0;

	while (true)
	{
		const uint8_t c = get(position);

		if (c == 0 || firstCharTable_[c] & CHAR_SPACE)
		{
			std::string result(data(position));
			consume(position);
			return result;
		}

		++position;
	}

	return std::string();
}

bool Scanner::readRemark()
{
	ocean_assert(firstCharTable_[get()] & CHAR_REMARK);

	return readScopeRemark() || readLineRemark();
}

bool Scanner::readLineRemark()
{
	ocean_assert(firstCharTable_[get()] & CHAR_REMARK);

	if (lineRemarks_.empty())
	{
		return false;
	}

	size_t pos = 1;
	bool found = false;

	if (lineRemarks_.find(std::string(1, get())) != lineRemarks_.cend())
	{
		found = true;
	}
	else
	{
		while ((followingCharTable_[get(pos)] & CHAR_REMARK) && pos + 1 <= maximalLengthLineRemarks_)
		{
			if (lineRemarks_.find(data(pos + 1)) != lineRemarks_.cend())
			{
				++pos;
				found = true;
				break;
			}

			++pos;
		}
	}

	if (found)
	{
		while (true)
		{
			if (get(pos) == 10)
			{
				++line_;
				column_ = 1;

				consume(pos + 1);

				break;
			}
			else if (get(pos) == 13 && get(pos + 1) == 10)
			{
				++line_;
				column_ = 1;

				consume(pos + 2);

				break;
			}
			else if (get(pos) == 0)
			{
				consume(pos);
				break;
			}

			++pos;
		}
	}

	return found;
}

bool Scanner::readScopeRemark()
{
	ocean_assert(firstCharTable_[get()] & CHAR_REMARK);

	if (scopeRemarks_.empty())
	{
		return false;
	}

	size_t pos = 1;

	ScopeRemarks::const_iterator i = scopeRemarks_.find(std::string(1, get()));

	if (i == scopeRemarks_.cend())
	{
		while ((followingCharTable_[get(pos)] & CHAR_REMARK) && pos + 1 <= maximalLengthScopeRemarks_)
		{
			i = scopeRemarks_.find(data(pos + 1));

			if (i != scopeRemarks_.cend())
			{
				++pos;
				break;
			}

			++pos;
		}
	}

	if (i == scopeRemarks_.cend())
	{
		return false;
	}


	bool foundEnd = false;
	size_t startLine = line_;
	const std::string& endRemark = i->second;

	ocean_assert(endRemark.length() > 0);
	const uint8_t cEnd = uint8_t(endRemark[0]);

	while (true)
	{
		if (get(pos) == cEnd)
		{
			foundEnd = true;
			size_t n = 1;

			for (n = 1; n < endRemark.size(); ++n)
			{
				if (get(pos + n) != endRemark[n])
				{
					foundEnd = false;
					break;
				}
			}

			if (foundEnd)
			{
				consume(pos + n);
				break;
			}
		}

		if (get(pos) == 10)
		{
			++line_;
			column_ = 1;
		}
		else if (get(pos) == 13 && get(pos + 1) == 10)
		{
			++line_;
			++pos;

			column_ = 1;
		}
		else if (get(pos) == 0)
		{
			throw OceanException(std::string("Invalid scoped remark starting at line: ") + String::toAString(startLine) + std::string(" and has no end."));
		}

		++pos;
	}

	return true;
}

bool Scanner::readCharacter(Token& token, const bool consumeBytes)
{
	ocean_assert(firstCharTable_[get()] & CHAR_CHARACTER);

	const uint8_t character = get(1);
	const uint8_t end = get(2);

	if (end != get())
	{
		return false;
	}

	token = Token(std::string(1, character), Token::TOKEN_CHARACTER);

	if (consumeBytes)
	{
		consume(3);
	}

	return true;
}

bool Scanner::readIdentifier(Token& token, const bool consumeBytes)
{
	ocean_assert(firstCharTable_[get()] & CHAR_IDENTIFIER);

	size_t pos = 1;
	while (true)
	{
		const uint8_t c = get(pos++);

		if (followingCharTable_[c] & CHAR_IDENTIFIER)
		{
			continue;
		}

		if (invalidCharTable_[c] & CHAR_IDENTIFIER)
		{
			return false;
		}

		if ((followingCharTable_[c] & CHAR_IDENTIFIER) == CHAR_INVALID)
		{
			token = Token(data(pos - 1), Token::TOKEN_IDENTIFIER);

			if (consumeBytes)
			{
				consume(pos - 1);
			}

			return true;
		}
	}
}

bool Scanner::readInteger(Token& token, const bool consumeBytes)
{
	ocean_assert(firstCharTable_[get()] & CHAR_INTEGER);

	size_t pos = 1;

	while (true)
	{
		const uint8_t c = get(pos++);

		if (followingCharTable_[c] & CHAR_INTEGER)
		{
			continue;
		}

		if (invalidCharTable_[c] & CHAR_INTEGER)
		{
			return false;
		}

		if ((followingCharTable_[c] & CHAR_INTEGER) == CHAR_INVALID)
		{
			token = Token(data(pos - 1), Token::TOKEN_INTEGER);

			if (consumeBytes)
			{
				consume(pos - 1);
			}

			return true;
		}
	}
}

bool Scanner::readKeyword(Token& token, const bool consumeBytes)
{
	ocean_assert((firstCharTable_[get()] & CHAR_KEYWORD) || (!keywordsAreCaseSensitive_ && (firstCharTable_[tolower(get())] & CHAR_KEYWORD)));

	size_t pos = 1;

	while (true)
	{
		uint8_t c = get(pos++);

		if (keywordsAreCaseSensitive_ == false)
		{
			c = uint8_t(tolower(c));
		}

		if (followingCharTable_[c] & CHAR_KEYWORD)
		{
			continue;
		}

		if (invalidCharTable_[c] & CHAR_KEYWORD)
		{
			return false;
		}

		if (c == 0 || (((followingCharTable_[c] & CHAR_KEYWORD) == CHAR_INVALID) && ((followingCharTable_[c] & CHAR_IDENTIFIER) == CHAR_INVALID)))
		{
			std::string value = data(pos - 1);

			if (keywordsAreCaseSensitive_ == false)
			{
				value = String::toLower(value);
			}

			const IdMap::const_iterator i = keywordMap_.find(value);
			if (i == keywordMap_.cend())
			{
				return false;
			}

			token = Token(value, i->second, Token::TOKEN_KEYWORD);

			if (consumeBytes)
			{
				consume(pos - 1);
			}

			return true;
		}
	}
}

bool Scanner::readLine(Token& token, const bool consumeBytes)
{
	size_t pos = 0;
	uint8_t c = get(pos);

	if ((firstCharTable_[c] & CHAR_SPACE) && c != 10 && c != 13)
	{
		readWhiteSpace(false);
	}

	while (true)
	{
		c = get(pos);

		if (firstCharTable_[c] & CHAR_SPACE)
		{
			if (c == 10)
			{
				++line_;
				column_ = 1;

				token = Token(data(pos), Token::TOKEN_LINE);

				if (consumeBytes)
				{
					consume(pos + 1);
				}

				return true;
			}
			else if (c == 13 && get(pos + 1) == 10)
			{
				++line_;
				column_ = 1;

				token = Token(data(pos), Token::TOKEN_LINE);

				if (consumeBytes)
				{
					consume(pos + 2);
				}

				return true;
			}
		}
		else if (c == 0)
		{
			return false;
		}

		++pos;
	}
}

bool Scanner::readNumber(Token& token, const bool consumeBytes)
{
	ocean_assert(firstCharTable_[get()] & CHAR_NUMBER);

	size_t pos = 1;
	bool exponent = false;

	while (true)
	{
		uint8_t c = get(pos++);

		if (followingCharTable_[c] & CHAR_NUMBER)
		{
			continue;
		}

		if (invalidCharTable_[c] & CHAR_NUMBER)
		{
			return false;
		}

		if (exponent == false && (c == 'e' || c == 'E'))
		{
			exponent = true;

			c = get(pos++);
			if (c != '+' && c != '-')
			{
				return false;
			}

			continue;
		}

		if ((followingCharTable_[c] & CHAR_NUMBER) == CHAR_INVALID)
		{
			token = Token(data(pos - 1), Token::TOKEN_NUMBER);

			if (consumeBytes)
			{
				consume(pos - 1);
			}

			return true;
		}
	}
}

bool Scanner::readString(Token& token, const bool consumeBytes)
{
	ocean_assert(firstCharTable_[get()] & CHAR_STRING);

	size_t pos = 1;

	while (true)
	{
		uint8_t c = get(pos++);

		if (c == '\"' && get(pos - 2) != '\\')
		{
			std::string stringValue = data(pos);

			token = Token(stringValue.substr(1, stringValue.length() - 2), Token::TOKEN_STRING);

			if (consumeBytes)
			{
				consume(pos);
			}

			return true;
		}

		if (c == 0)
		{
			return false;
		}
	}
}

bool Scanner::readSymbol(Token& token, const bool consumeBytes)
{
	ocean_assert(firstCharTable_[get()] & CHAR_SYMBOL);

	size_t pos = 1;

	while (true)
	{
		uint8_t c = get(pos++);

		if (followingCharTable_[c] & CHAR_SYMBOL)
		{
			continue;
		}

		if (invalidCharTable_[c] & CHAR_SYMBOL)
		{
			return false;
		}

		if ((followingCharTable_[c] & CHAR_SYMBOL) == CHAR_INVALID)
		{
			std::string value = data(pos - 1);

			const IdMap::const_iterator i = symbolMap_.find(value);
			if (i == symbolMap_.cend())
			{
				return false;
			}

			token = Token(value, i->second, Token::TOKEN_SYMBOL);

			if (consumeBytes)
			{
				consume(pos - 1);
			}

			return true;
		}
	}
}

}

}
