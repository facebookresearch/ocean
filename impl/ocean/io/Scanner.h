/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SCANNER_H
#define META_OCEAN_IO_SCANNER_H

#include "ocean/io/IO.h"

#include "ocean/base/Memory.h"

#include "ocean/math/Math.h"

#include <array>
#include <istream>

namespace Ocean
{

namespace IO
{

/**
 * This class implements a simple scanner.
 * @ingroup io
 */
class OCEAN_IO_EXPORT Scanner
{
	public:

		/**
		 * Definition of an invalid keyword or symbol id.
		 */
		static constexpr uint32_t invalidId = uint32_t(-1);

		/**
		 * This class implements a token for the scanner.
		 */
		class OCEAN_IO_EXPORT Token final
		{
			public:

				/**
				 * Definition of different token types.
				 */
				enum Type : uint32_t
				{
					/// Invalid token.
					TOKEN_INVALID = 0u,
					/// Character token.
					TOKEN_CHARACTER,
					/// End of file token.
					TOKEN_END_OF_FILE,
					/// Identifier token.
					TOKEN_IDENTIFIER,
					/// Line token.
					TOKEN_LINE,
					/// Number token.
					TOKEN_NUMBER,
					/// Integer token.
					TOKEN_INTEGER,
					/// Keyword token.
					TOKEN_KEYWORD,
					/// String token.
					TOKEN_STRING,
					/// Symbol token.
					TOKEN_SYMBOL
				};

			public:

				/**
				 * Creates an invalid token.
				 */
				Token() = default;

				/**
				 * Creates a new token with given data and type.
				 * @param data Token data
				 * @param type Token type
				 */
				Token(std::string&& data, const Type type);

				/**
				 * Creates a new token with given data and type.
				 * @param data Token data
				 * @param type Token type
				 */
				Token(const std::string& data, const Type type);

				/**
				 * Creates a new token with given keyword or symbol id.
				 * @param data Raw token data
				 * @param id Keyword or symbol id of the token
				 * @param type Token type
				 */
				Token(std::string&& data, const uint32_t id, const Type type);

				/**
				 * Creates a new token with given keyword or symbol id.
				 * @param data Raw token data
				 * @param id Keyword or symbol id of the token
				 * @param type Token type
				 */
				Token(const std::string& data, const uint32_t id, const Type type);

				/**
				 * Returns the type of the token.
				 * @return Token type
				 */
				inline Type type() const;

				/**
				 * Returns whether this token is of a specific type.
				 * @param type The type to check
				 * @return True, if so
				 */
				inline bool isType(const Type type) const;

				/**
				 * Returns whether this token holds a character.
				 * @return True, if so
				 */
				inline bool isCharacter() const;

				/**
				 * Returns whether this token holds an identifier.
				 * @return True, if so
				 */
				inline bool isIdentifier() const;

				/**
				 * Returns whether this token holds an integer.
				 * @return True, if so
				 */
				inline bool isInteger() const;

				/**
				 * Returns whether this token holds a remaining line.
				 * @return True, if so
				 */
				inline bool isLine() const;

				/**
				 * Returns whether this token holds a number.
				 * @return True, if so
				 */
				inline bool isNumber() const;

				/**
				 * Returns whether this token holds an integer or a number.
				 * @return True, if so
				 */
				inline bool isIntegerOrNumber() const;

				/**
				 * Returns whether this token holds a keyword.
				 * @return True, if so
				 */
				inline bool isKeyword() const;

				/**
				 * Returns whether this token holds a special keyword.
				 * @param keyword The keyword id to check
				 * @return True, if so
				 */
				bool isKeyword(const uint32_t keyword) const;

				/**
				 * Returns whether this token holds a string.
				 * @return True, if so
				 */
				inline bool isString() const;

				/**
				 * Returns whether this token holds a symbol.
				 * @return True, if so
				 */
				inline bool isSymbol() const;

				/**
				 * Returns whether this token holds a special symbol.
				 * @param symbol The symbol id to check
				 * @return True, if so
				 */
				bool isSymbol(const uint32_t symbol) const;

				/**
				 * Returns whether this token holds a end of file.
				 * @return True, if so
				 */
				inline bool isEndOfFile() const;

				/**
				 * Returns the character value of this token.
				 * @return Character value
				 */
				uint8_t character() const;

				/**
				 * Returns the identifier value of this token.
				 * @return Identifier value
				 */
				const std::string& identifier() const;

				/**
				 * Returns the integer value of this token.
				 * @return Integer value
				 */
				int integer() const;

				/**
				 * Returns the float value of this token.
				 * @return Float value
				 */
				Scalar number() const;

				/**
				 * Returns the integer or float value of this token.
				 * @return Integer or float value as float value
				 */
				Scalar integerOrNumber() const;

				/**
				 * Returns the remaining line of this token.
				 * @return Line value
				 */
				const std::string& line() const;

				/**
				 * Returns the id of the keyword of this token.
				 * @return Keyword id
				 */
				uint32_t keyword() const;

				/**
				 * Returns the id of the symbol of this token.
				 * @return Symbol id
				 */
				uint32_t symbol() const;

				/**
				 * Returns the string value of this token.
				 * @return The token's string value
				 */
				const std::string& string() const;

				/**
				 * Returns the string value of this token and invalidates the token.
				 * @return The token's string value
				 */
				std::string moveString();

				/**
				 * Returns the raw data of the token.
				 * @return Raw data
				 */
				inline const std::string& raw() const;

				/**
				 * Returns whether the token is valid.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/// Holds the type of the token.
				Type type_ = TOKEN_INVALID;

				/// Holds the id of the keyword or symbol, if any.
				uint32_t id_ = invalidId;

				/// Holds the data of the token.
				std::string data_;
		};

	protected:

		/**
		 * Definition of an unordered map mapping strings to ids.
		 */
		typedef std::unordered_map<std::string, uint32_t> IdMap;

		/**
		 * Definition of an unordered set holding line remark symbols.
		 */
		typedef std::unordered_set<std::string> LineRemarks;

		/**
		 * Definition of an unordered map mapping begin remark symbols to end remark symbols.
		 */
		typedef std::unordered_map<std::string, std::string> ScopeRemarks;

		/**
		 * Definition of a character table.
		 */
		typedef std::array<uint16_t, 256> CharTable;

		/**
		 * Definition of first character types.
		 */
		enum FirstChar : uint16_t
		{
			/// Invalid.
			CHAR_INVALID = 0,
			/// Character.
			CHAR_CHARACTER = 1,
			/// Identifier.
			CHAR_IDENTIFIER = 2,
			/// Number.
			CHAR_NUMBER = 4,
			/// Integer.
			CHAR_INTEGER = 8,
			/// Keyword.
			CHAR_KEYWORD = 16,
			/// String.
			CHAR_STRING = 32,
			/// Symbol.
			CHAR_SYMBOL = 64,
			/// Remark.
			CHAR_REMARK = 128,
			/// White space.
			CHAR_SPACE = 256
		};

	public:

		/**
		 * Creates a new scanner using a stream as input.
		 * @param stream The stream to be use as input
		 * @param progress Optional resulting scanner progress in percent, with range [0, 1]
		 * @param cancel Optional scanner cancel flag
		 */
		explicit Scanner(const std::shared_ptr<std::istream>& stream, float* progress = nullptr, bool* cancel = nullptr);

		/**
		 * Creates a new scanner using a file or a memory buffer as input.
		 * @param filename The name of the file to be used as input, `buffer` must be empty
		 * @param buffer The buffer to be used as input, `filename` must be empty
		 * @param progress Optional resulting scanner progress in percent, with range [0, 1]
		 * @param cancel Optional scanner cancel flag
		 */
		inline Scanner(const std::string& filename, const std::string& buffer, float* progress = nullptr, bool* cancel = nullptr);

		/**
		 * Creates a new scanner using a file or a memory buffer as input.
		 * @param filename The name of the file to be used as input, `buffer` must be empty
		 * @param buffer The buffer to be used as input, `filename` must be empty
		 * @param progress Optional resulting scanner progress in percent, with range [0, 1]
		 * @param cancel Optional scanner cancel flag
		 */
		inline Scanner(const std::string& filename, std::string&& buffer, float* progress = nullptr, bool* cancel = nullptr);

		/**
		 * Destructs a scanner.
		 */
		virtual ~Scanner();

		/**
		 * Returns the recent token.
		 * @return Recent token
		 */
		const Token& token();

		/**
		 * Returns a line token starting at the current position.
		 * A line token does not handle remarks.
		 * @return Line token
		 */
		const Token& lineToken();

		/**
		 * Return the recent token and pops it afterwards.
		 * @return Recent token.
		 */
		Token tokenPop();

		/**
		 * Returns a lookout to the next token.
		 * @return Next token
		 */
		const Token& look();

		/**
		 * Pops the recent token.
		 */
		void pop();

		/**
		 * Returns the recent line.
		 * @return Recent line
		 */
		inline size_t line() const;

		/**
		 * Returns the recent column.
		 * @return Recent column
		 */
		inline size_t column() const;

		/**
		 * Returns the position of the scanner.
		 * @return Position of the scanner in characters
		 */
		size_t position() const;

		/**
		 * Returns the size of the scanner.
		 * @return Size of the scanner in characters
		 */
		size_t size() const;

		/**
		 * Returns the name of the input file, if the input is a file.
		 * @return The scanner's input filename, empty if the input was a buffer
		 */
		inline const std::string& filename() const;

		/**
		 * Returns whether the scanner is valid and ready to use.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Finds the next token in a given string starting from a specified position.
		 * A token is enclosed by white characters or by the borders of the given string, the length of the given string is explicitly defined by the parameter 'size'.
		 * @param pointer The pointer to the string in which the next token is to be found, must be valid
		 * @param size The length of the given string in characters, with range [1, infinity)
		 * @param start The first character within the given string that defines the first possible character of the token, with range [0, size - 1]
		 * @param tokenStart The resulting start location within the given string of the found token, with range [start, strlen(pointer) - 1]
		 * @param tokenLength The resulting length of the found token, with range [1, strlen(pointer) - start]
		 * @return True, if a second token may follow; False, if the token is the last token
		 */
		static bool findNextToken(const char* pointer, const size_t size, const size_t start, size_t& tokenStart, size_t& tokenLength);

		/**
		 * Finds the next token in a given string starting from a specified position.
		 * A token is enclosed by white characters or by the borders of the given string, the end is identified by a null character.
		 * @param pointer The pointer to the string in which the next token is to be found, can be nullptr
		 * @param start The first character within the given string that defines the first possible character of the token, with range [0, strlen(pointer)]
		 * @param tokenStart The resulting start location within the given string of the found token, with range [start, strlen(pointer) - 1]
		 * @param tokenLength The resulting length of the found token, with range [1, strlen(pointer) - start]
		 * @return True, if a second token may follow; False, if the token is the last token
		 */
		static bool findNextToken(const char* pointer, const size_t start, size_t& tokenStart, size_t& tokenLength);

		/**
		 * Returns whether a given character is a white space character.
		 * A white space character can be one of the following:
		 * <pre>
		 * ' ', '\\t', '\\n', or '\\r'
		 * </pre>
		 * @param character The character to be checked
		 * @return True, if so
		 */
		static inline bool isWhitespace(const char& character);

	protected:

		/**
		 * Creates a new scanner.
		 * The scanner may forward an entire progress state, if the pointer value is defined.<br>
		 * Beware: Make sure that the value exists during the whole scanning timer!<br>
		 * Further, the scanner may be canceled by an explicit flag.<br>
		 * In the case the scanner is canceled an end of file token is returned.<br>
		 * Beware: As for the progress value, also the cancel object must exist during the whole scanning progress, if provided
		 * @param progress Optional progress parameter to forward the scanning progress with range [0, 1], use nullptr if the progress state is not necessary
		 * @param cancel Optional cancel state to cancel the scanner progress by setting the flag to 'true', use nullptr if the cancel state is not necessary
		 */
		Scanner(float* progress, bool* cancel);

		/**
		 * Returns one character.
		 * @param offset Offset to the recent position
		 * @return Character
		 */
		uint8_t get(const size_t offset = 0);

		/**
		 * Returns data of a specified size starting at the recent position.
		 * Beware: Make sure that enough pending buffer is available
		 * @param size Size of the data to receive
		 * @return Requested data
		 */
		std::string data(const size_t size) const;

		/**
		 * Returns data of a specified size starting at the offset position.
		 * Beware: Make sure that enough pending buffer is available
		 * @param offset Start position relative to the current position
		 * @param size Size of the data to receive
		 * @return Requested data
		 */
		std::string data(const size_t offset, const size_t size) const;

		/**
		 * Consumes one or more character.
		 * @param chars Number of characters to consume
		 */
		void consume(const size_t chars = 1);

		/**
		 * Refills the intermediate buffer.
		 * @return True, if the buffer could be refilled with new characters
		 */
		bool refillIntermediateBuffer();

		/**
		 * Returns the keyword id of a given string.
		 * @param data Data to convert to a keyword
		 * @return Id of the identifier, otherwise invalidId
		 */
		uint32_t keywordId(const std::string& data) const;

		/**
		 * Returns the symbol id of a given string.
		 * @param data Data to convert to a symbol
		 * @return Id of the symbol, otherwise invalidId
		 */
		uint32_t symbolId(const std::string& data) const;

		/**
		 * Sets whether the keywords are case sensitive or not.
		 * As default all keywords are case sensitive.<br>
		 * Beware: This property has to be set before the first keyword is registered!
		 * @param caseSensitive True, if all keywords will be case sensitive
		 */
		void setKeywordProperty(const bool caseSensitive);

		/**
		 * Registers a new keyword.
		 * @param keyword New keyword
		 * @param id Id of the keyword
		 */
		void registerKeyword(const std::string& keyword, const uint32_t id);

		/**
		 * Registers a new symbol.
		 * @param symbol New symbol
		 * @param id Id of the symbol
		 */
		void registerSymbol(const std::string& symbol, const uint32_t id);

		/**
		 * Registers a line remark symbol.
		 * @param lineRemark Line remark symbol
		 */
		void registerLineRemark(const std::string& lineRemark);

		/**
		 * Registers a scope remark symbol.
		 * @param begin Begin remark symbol
		 * @param end End remark symbol
		 */
		void registerScopeRemark(const std::string& begin, const std::string& end);

		/**
		 * Registers a white space character.
		 * @param character White space character to register
		 * @return True, if succeeded
		 */
		bool registerWhiteSpaceCharacter(const uint8_t character);

		/**
		 * Reads and returns the next token.
		 * @param consumeBytes Determines whether the scanner consume the read characters.
		 * @return New token
		 */
		virtual Token readToken(const bool consumeBytes = true);

		/**
		 * Reads white space.
		 * @param crossLines Determines whether the white space can be separated over several lines
		 * @return Next not-white-space character
		 */
		uint8_t readWhiteSpace(bool crossLines = true);

		/**
		 * Discards non white space and jumps to the first white space position.
		 * @return Discarded elements
		 */
		std::string discardNonWhiteSpace();

		/**
		 * Reads remark comments.
		 * @return True, if a comment was read
		 */
		bool readRemark();

		/**
		 * Reads a line remark comment.
		 * @return True, if a comment was read
		 */
		bool readLineRemark();

		/**
		 * Reads a scope remark comment.
		 * @return True, if a comment was read
		 */
		bool readScopeRemark();

		/**
		 * Tries to read a character as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readCharacter(Token& token, const bool consumeBytes);

		/**
		 * Tries to read a identifier as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readIdentifier(Token& token, const bool consumeBytes);

		/**
		 * Tries to read an integer as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readInteger(Token& token, const bool consumeBytes);

		/**
		 * Tries to read a keyword as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readKeyword(Token& token, const bool consumeBytes);

		/**
		 * Tries to read a remaining line as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readLine(Token& token, const bool consumeBytes);

		/**
		 * Tries to read a number as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readNumber(Token& token, const bool consumeBytes);

		/**
		 * Tries to read a string as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readString(Token& token, const bool consumeBytes);

		/**
		 * Tries to read a symbol as next token.
		 * @param token Returning token
		 * @param consumeBytes Determines whether the scanner consumes the read characters
		 * @return True, if succeeded
		 */
		bool readSymbol(Token& token, const bool consumeBytes);

	private:

		/**
		 * Returns one character from the extra buffer.
		 * @param offset Offset inside the recent extra buffer
		 * @return Character
		 */
		uint8_t getExtra(const size_t offset = 0);

		/**
		 * Refills the extra buffer.
		 * @param minIndex Minimal index of the character needed inside the extra buffer
		 * @return True, if enough characters could be read
		 */
		bool refillExtraBuffer(const size_t minIndex);

		/**
		 * Creates a file input stream or a string input stream depending on the given input.
		 * @param filename The name of the file to be used as input, `buffer` must be empty
		 * @param buffer The buffer to be used as input, `filename` must be empty
		 */
		static inline std::shared_ptr<std::istream> createInputStream(const std::string& filename, std::string&& buffer);

		/**
		 * Creates a file input stream or a string input stream depending on the given input.
		 * @param filename The name of the file to be used as input, `buffer` must be empty
		 * @param buffer The buffer to be used as input, `filename` must be empty
		 */
		static inline std::shared_ptr<std::istream> createInputStream(const std::string& filename, const std::string& buffer);

	protected:

		/// Recent token.
		Token recentToken_;

		/// Next token.
		Token nextToken_;

		/// The input stream from which the scanner receives the data.
		std::shared_ptr<std::istream> stream_;

		/// The name of the input file, if the input is a file.
		std::string filename_;

		/// The scanner's progress in percent, with range [0, 1].
		float* progress_ = nullptr;

		/// Cancel flag.
		bool* cancel_ = nullptr;

		/// Local intermediate buffer.
		Memory intermediateBuffer_;

		/// The current pointer inside the intermediate buffer.
		uint8_t* intermediateBufferPointer_ = nullptr;

		/// Number of remaining characters in the intermediate buffer.
		size_t intermediateBufferSize_ = 0;

		/// Local extra buffer, used if the intermediate buffer is too small.
		Memory extraBuffer_;

		/// Pointer inside the extra buffer.
		uint8_t* extraBufferPointer_ = nullptr;

		/// Number of remaining characters inside the extra buffer.
		size_t extraBufferSize_ = 0;

		/// Holds the current line.
		size_t line_ = 1;

		/// Holds the current column.
		size_t column_ = 1;

		/// Holds the current position of the scanner.
		size_t position_ = 0;

		/// Map mapping keyword strings to identifier ids.
		IdMap keywordMap_;

		/// Determines whether all keywords are case sensitive.
		bool keywordsAreCaseSensitive_ = true;

		/// Map mapping symbol strings to symbol ids.
		IdMap symbolMap_;

		/// Registered line remarks.
		LineRemarks lineRemarks_;

		/// Length of the maximal line remark.
		size_t maximalLengthLineRemarks_ = 0;

		/// Scope remarks.
		ScopeRemarks scopeRemarks_;

		/// Length of the maximal scope remarks.
		size_t maximalLengthScopeRemarks_ = 0;

		/// Table holding the definition of allowed first characters.
		CharTable firstCharTable_;

		/// Table holding the definition of allowed following characters.
		CharTable followingCharTable_;

		/// Table holding the definition of not allowed following characters.
		CharTable invalidCharTable_;

		/// Definition of the minimum intermediate buffer size.
		static constexpr size_t minBufferSize_ = 2048;

		/// Definition of the maximum intermediate buffer size.
		static constexpr size_t maxBufferSize_ = 8192;
};

inline Scanner::Token::Type Scanner::Token::type() const
{
	return type_;
}

inline bool Scanner::Token::isType(const Type type) const
{
	return type_ == type;
}

inline bool Scanner::Token::isCharacter() const
{
	return type_ == TOKEN_CHARACTER;
}

inline bool Scanner::Token::isIdentifier() const
{
	return type_ == TOKEN_IDENTIFIER;
}

inline bool Scanner::Token::isInteger() const
{
	return type_ == TOKEN_INTEGER;
}

inline bool Scanner::Token::isNumber() const
{
	return type_ == TOKEN_NUMBER;
}

inline bool Scanner::Token::isIntegerOrNumber() const
{
	return type_ == TOKEN_INTEGER || type_ == TOKEN_NUMBER;
}

inline bool Scanner::Token::isLine() const
{
	return type_ == TOKEN_LINE;
}

inline bool Scanner::Token::isKeyword() const
{
	return type_ == TOKEN_KEYWORD;
}

inline bool Scanner::Token::isString() const
{
	return type_ == TOKEN_STRING;
}

inline bool Scanner::Token::isSymbol() const
{
	return type_ == TOKEN_SYMBOL;
}

inline bool Scanner::Token::isEndOfFile() const
{
	return type_ == TOKEN_END_OF_FILE;
}

inline const std::string& Scanner::Token::raw() const
{
	return data_;
}

inline Scanner::Token::operator bool() const
{
	return type_ != TOKEN_INVALID;
}

inline Scanner::Scanner(const std::string& filename, const std::string& buffer, float* progress, bool* cancel) :
	Scanner(createInputStream(filename, buffer), progress, cancel)
{
	ocean_assert(!filename.empty() || !buffer.empty());

	if (!filename.empty() && stream_)
	{
		filename_ = filename;
	}
}

inline Scanner::Scanner(const std::string& filename, std::string&& buffer, float* progress, bool* cancel) :
	Scanner(createInputStream(filename, std::move(buffer)), progress, cancel)
{
	ocean_assert(!filename.empty() || stream_);

	if (!filename.empty() && stream_)
	{
		filename_ = filename;
	}
}

size_t Scanner::line() const
{
	return line_;
}

size_t Scanner::column() const
{
	return column_;
}

inline const std::string& Scanner::filename() const
{
	return filename_;
}

inline bool Scanner::isValid() const
{
	return bool(stream_);
}

inline bool Scanner::isWhitespace(const char& character)
{
	return character == ' ' || character== '\t' || character == '\n' || character == '\r';
}

inline std::shared_ptr<std::istream> Scanner::createInputStream(const std::string& filename, std::string&& buffer)
{
	ocean_assert(!filename.empty() || !buffer.empty());

	if (!filename.empty())
	{
		return std::shared_ptr<std::istream>(new std::ifstream(filename.c_str(), std::ios_base::binary));
	}

	return std::shared_ptr<std::istream>(new std::istringstream(std::move(buffer)));
}

inline std::shared_ptr<std::istream> Scanner::createInputStream(const std::string& filename, const std::string& buffer)
{
	ocean_assert(!filename.empty() || !buffer.empty());

	if (!filename.empty())
	{
		return std::shared_ptr<std::istream>(new std::ifstream(filename.c_str(), std::ios_base::binary));
	}

	return std::shared_ptr<std::istream>(new std::istringstream(buffer));
}

}

}

#endif // META_OCEAN_IO_SCANNER_H
