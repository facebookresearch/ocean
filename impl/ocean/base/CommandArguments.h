/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_COMMAND_ARGUMENTS_H
#define META_OCEAN_BASE_COMMAND_ARGUMENTS_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/String.h"
#include "ocean/base/Triple.h"
#include "ocean/base/Value.h"

#include <unordered_map>

namespace Ocean
{

/**
 * This class implements a manager for command arguments.
 * Arguments can be configured, parsed, and acquired.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT CommandArguments
{
	public:

		/**
		 * Definition of a string with either `char` or `wchar_t`.
		 */
		template <typename T>
		using ArgumentT = std::basic_string<T>;

		/**
		 * Definition of a vector holding strings with either `char` or `wchar_t`.
		 */
		template <typename T>
		using ArgumentsT = std::vector<ArgumentT<T>>;

		/**
		 * This class implements a simple singleton holding the raw application's command arguments.
		 */
		class OCEAN_BASE_EXPORT Manager : public Singleton<Manager>
		{
			friend class Singleton<Manager>;

			public:

				/**
				 * Sets/registers the command arguments of the application.
				 * @param arguments The arguments to set, ensure that the pointer is valid as long as the application exists, must be valid
				 * @param size The number of provided command arguments, with range [1, infinity)
				 */
				bool setRawArguments(const char* const* arguments, const size_t size);

				/**
				 * Sets/registers the command arguments of the application.
				 * @param arguments The arguments to set, ensure that the pointer is valid as long as the application exists, must be valid
				 * @param size The number of provided command arguments, with range [1, infinity)
				 */
				bool setRawArguments(const wchar_t* const* arguments, const size_t size);

				/**
				 * Returns the command arguments of the application.
				 * @return The application's command arguments, nullptr if the arguments have not been set via setArguments().
				 * @tparam TChar The character type, `char` or `wchar_t`
				 * @see setArguments().
				 */
				template <typename TChar = char>
				const TChar* const* rawArguments() const;

				/**
				 * Returns the number of command arguments of the application.
				 * @return The application's number of command arguments, 0 if arguments have not been set via setArguments().
				 * @see setArguments().
				 */
				size_t size() const;

			protected:

				/**
				 * Protected default constructor.
				 */
				Manager();

			protected:

				/// The command arguments, nullptr if no arguments are defined.
				const char* const* argumentsChar_ = nullptr;

				/// The command arguments, nullptr if no arguments are defined.
				const wchar_t* const* argumentsWchar_ = nullptr;

				/// The number of command arguments, with range [0, infinity).
				size_t size_ = 0;

				/// The manager's lock.
				mutable Lock lock_;
		};

	protected:

		/**
		 * This class defines a named value with long and short name, with default parameter and description.
		 */
		class Parameter
		{
			public:

				/**
				 * Creates a new parameter object.
				 * @param longName The long name of the parameter, must be valid
				 * @param shortName The short name of the parameter, can be empty
				 * @param description The optional description of the parameter, can be empty
				 * @param defaultValue The optional default value, invalid otherwise
				 */
				Parameter(const std::string& longName, const std::string& shortName, const std::string& description, const Value& defaultValue);

				/**
				 * Returns the long name of this parameter
				 * @return The parameter's long name
				 */
				inline const std::string& longName() const;

				/**
				 * Returns the short name of this parameter
				 * @return The parameter's short name, can be empty
				 */
				inline const std::string& shortName() const;

				/**
				 * Returns the description of this parameter.
				 * @return The parameter's description
				 */
				inline const std::string& description() const;

				/**
				 * Returns the default value of this parameter.
				 * @return The parameter's default name, can be invalid
				 */
				inline const Value& defaultValue() const;

			protected:

				/// The long name of the parameter.
				std::string longName_;

				/// The short name of the parameter, can be empty.
				std::string shortName_;

				/// The description of this parameter, can be empty.
				std::string description_;

				/// The default value of this parameter, can be invalid.
				Value defaultValue_;
		};

		/**
		 * Definition of a map mapping long parameter names to parameter objects.
		 */
		typedef std::map<std::string, Parameter> ParameterMap;

		/**
		 * Definition of a map mapping short parameter names to long parameter names.
		 */
		typedef std::unordered_map<std::string, std::string> ShortToLongMap;

		/**
		 * Definition of a map mapping long parameter names to values.
		 */
		typedef std::unordered_map<std::string, Value> ValueMap;

	public:

		/**
		 * Creates a new object.
		 */
		CommandArguments();

		/**
		 * Creates a new object with a description of the application
		 * @param applicationDescription A description text for the application using these command arguments.
		 */
		CommandArguments(const std::string& applicationDescription);

		/**
		 * Registers a new named parameter which can be parsed as command argument.
		 * @param longName The long name of the parameter, must be valid, must start with a alphabetic character
		 * @param shortName Optional short name of the parameter, can be empty, must start with a alphabetic character if defined
		 * @param description Optional description of the parameter, can be empty
		 * @param defaultValue Optional default value of the parameter, can be invalid
		 * @return True, if the named parameter did not exist before
		 */
		bool registerParameter(const std::string& longName, const std::string& shortName = std::string(), const std::string& description = std::string(), const Value& defaultValue = Value());

		/**
		 * Registers nameless parameters for the summary.
		 * @param description The description for nameless parameters, must not be empty
		 * @return True, if succeeded
		 */
		bool registerNamelessParameters(std::string&& description);

		/**
		 * Parses A given command line.
		 * The entire command line may contain several commands separated by space characters.<br>
		 * Command elements which contain space characters must be surrounded by quotation marks.<br>
		 * However, all surrounding quotation marks will be removed during the parsing process.<br>
		 * The first argument should not be the filename (and path) of the executable.
		 * @param commandLine The command line to be parsed, must be valid
		 * @return True, if succeeded
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		inline bool parse(const TChar* commandLine);

		/**
		 * Parses the several command arguments.
		 * @param arguments The arguments to be parsed, an be nullptr if `size == 0`
		 * @param size The number of arguments, with size [0, infinity)
		 * @param skipFirstArgument True, to skip the first argument (as this is commonly the path of the executable); False, to parse the first parameter as well
		 * @return True, if succeeded
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		bool parse(const TChar* const* arguments, const size_t size, const bool skipFirstArgument = true);

		/**
		 * Parses the command arguments already separated into individual arguments.
		 * @param separatedArguments The individual arguments to be parsed
		 * @return True, if succeeded
		 */
		bool parse(const ArgumentsT<char>& separatedArguments);

		/**
		 * Parses the command arguments already separated into individual arguments.
		 * @param separatedArguments The individual arguments to be parsed
		 * @return True, if succeeded
		 */
		bool parse(const ArgumentsT<wchar_t>& separatedArguments);

		/**
		 * Returns the value of a specific parameter which has been parsed.
		 * Below an example of how to use this function:
		 * @code
		 * CommandArguments commandArguments;
		 * commandArguments.registerParameter("input", "i", "The input file");
		 *
		 * const Value inputFile = commandArguments.value<std::string>("input");
		 * if (inputFile.isString())
		 * {
		 *     const std::string inputFileString = inputFile.stringValue();
		 *     // ...
		 * }
		 * @endcode
		 * @param longName The long name of the parameter for which the value will be returned, must be valid
		 * @param allowDefaultValue True, to return the default value in case no actual value has been parsed; False, to return an invalid value if no actual value has been parsed
		 * @param namelessValueIndex Optional explicit index of a nameless value which is used in case the named value does not exist, -1 to avoid using a nameless value
		 * @return The value of the parameter, an invalid value if the value has not been parsed
		 */
		Value value(const std::string& longName, const bool allowDefaultValue = true, const size_t namelessValueIndex = size_t(-1)) const;

		/**
		 * Returns the value of a specific parameter which has been parsed.
		 * Below an example of how to use this function:
		 * @code
		 * CommandArguments commandArguments;
		 * commandArguments.registerParameter("input", "i", "The input file");
		 * commandArguments.registerParameter("factor", "f", "The factor parameter");
		 *
		 * const std::string inputFile = commandArguments.value<std::string>("input", std::string(), false);
		 * const double factor = commandArguments.value<double>("factor", -1.0, false);
		 * @endcode
		 * @param longName The long name of the parameter for which the value will be returned, must be valid
		 * @param invalidValue The resulting value in case the parameter was not parsed
		 * @param allowDefaultValue True, to return the default value in case no actual value has been parsed; False, to return an invalid value if no actual value has been parsed
		 * @param namelessValueIndex Optional explicit index of a nameless value which is used in case the named value does not exist, -1 to avoid using a nameless value
		 * @return The value of the parameter, the invalid value if the value has not been parsed
		 */
		template <typename T>
		T value(const std::string& longName, const T& invalidValue, const bool allowDefaultValue, const size_t namelessValueIndex = size_t(-1)) const;

		/**
		 * Checks whether a specific parameter value has been parsed, or whether a default value is defined.
		 * @param longName The long name of the parameter value to check, must be valid
		 * @param value Optional resulting value of the parameter
		 * @param allowDefaultValue True, to allow a default value to count as parsed value; False, to check for actually parsed values only
		 * @param namelessValueIndex Optional explicit index of a nameless value which is used in case the named value does not exist, -1 to avoid using a nameless value
		 * @return True, if succeeded
		 */
		bool hasValue(const std::string& longName, Value* value = nullptr, const bool allowDefaultValue = true, const size_t namelessValueIndex = size_t(-1)) const;

		/**
		 * Checks whether a specific parameter value has been parsed with specific data type, or whether a default value is defined.
		 * @param longName The long name of the parameter value to check, must be valid
		 * @param value The resulting value of the parameter
		 * @param allowDefaultValue True, to allow a default value to count as parsed value; False, to check for actually parsed values only
		 * @param namelessValueIndex Optional explicit index of a nameless value which is used in case the named value does not exist, -1 to avoid using a nameless value
		 * @return True, if succeeded
		 * @tparam T The data type of the value, 'Value' by default, can also be one of these specific values 'bool', 'int32_t', 'double' (while integers are accepted as well), 'std::string'
		 */
		template <typename T>
		bool hasValue(const std::string& longName, T& value, const bool allowDefaultValue = true, const size_t namelessValueIndex = size_t(-1)) const;

		/**
		 * Returns all nameless values which have been parsed.
		 * @return The nameless values, in order as they have been parsed, if any
		 */
		inline const std::vector<std::string>& namelessValues() const;

		/**
		 * Creates a summary of all possible command arguments.
		 * @return The string containing the summary
		 */
		std::string makeSummary();

		/**
		 * Parses the command line and returns the individual command elements.
		 * The entire command line may contain several commands separated by space characters.<br>
		 * Command elements which contain space characters must be surrounded by quotation marks.<br>
		 * However, all surrounding quotation marks will be removed during the parsing process.<br>
		 * The first argument should not be the filename (and path) of the executable.
		 * @param commandLine Command line to be parsed
		 * @return The separated command arguments
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		static ArgumentsT<TChar> separateArguments(const TChar* commandLine);

	protected:

		/**
		 * Returns whether a given string is the start of a long parameter name (whether it starts with "--").
		 * The string must be zero terminated.
		 * @param parameter The string to be checked, must be valid
		 * @return True, if so
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		bool isLongParameter(const TChar* parameter);

		/**
		 * Returns whether a given string is the start of a short parameter name (whether it starts with "--").
		 * The string must be zero terminated.
		 * @param parameter The string to be checked, must be valid
		 * @return True, if so
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		bool isShortParameter(const TChar* parameter);

		/**
		 * Returns a dash character.
		 * @return Dash character
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		static inline TChar dashCharacter();

		/**
		 * Returns a quote character.
		 * @return Quote character
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		static inline TChar quoteCharacter();

		/**
		 * Returns a space character.
		 * @return Space character
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		static inline TChar spaceCharacter();

		/**
		 * Returns a backslash character.
		 * @return Backslash character
		 * @tparam TChar The character type, `char` or `wchar_t`
		 */
		template <typename TChar>
		static inline TChar backslashCharacter();

	protected:

		/// The description text for the application using these command arguments.
		std::string applicationDescription_;

		/// The map mapping short parameter names to long parameter names.
		ShortToLongMap shortToLongMap_;

		/// The map mapping long parameter names to parameter objects.
		ParameterMap parameterMap_;

		/// The map mapping long parameter names to values.
		ValueMap valueMap_;

		/// The vector of values without name.
		std::vector<std::string> namelessValues_;

		/// Optional description for nameless parameters.
		std::string descriptionNamelessParameters_;
};

inline const std::string& CommandArguments::Parameter::longName() const
{
	return longName_;
}

inline const std::string& CommandArguments::Parameter::shortName() const
{
	return shortName_;
}

inline const std::string& CommandArguments::Parameter::description() const
{
	return description_;
}

inline const Value& CommandArguments::Parameter::defaultValue() const
{
	return defaultValue_;
}

template <typename TChar>
inline bool CommandArguments::parse(const TChar* commandLine)
{
	if (commandLine == nullptr)
	{
		return false;
	}

	return parse(separateArguments(commandLine));
}

template <>
inline bool CommandArguments::parse<wchar_t>(const wchar_t* commandLine)
{
	if (commandLine == nullptr)
	{
		return false;
	}

	const ArgumentsT<wchar_t> wSeparatedArugments = separateArguments(commandLine);

	ArgumentsT<char> separatedArugments;
	separatedArugments.reserve(wSeparatedArugments.size());

	for (const ArgumentT<wchar_t>& argument : wSeparatedArugments)
	{
		separatedArugments.emplace_back(String::toAString(argument));
	}

	return parse(separatedArugments);
}

template <typename TChar>
bool CommandArguments::parse(const TChar* const* arguments, const size_t size, const bool skipFirstArgument)
{
	if (size == 0)
	{
		return true;
	}

	if (arguments == nullptr)
	{
		ocean_assert(false && "Invalid arguments!");
		return false;
	}

	ArgumentsT<char> separatedArguments;
	separatedArguments.reserve(size);

	const size_t nStart = skipFirstArgument ? 1 : 0;

	for (size_t n = nStart; n < size; ++n)
	{
		separatedArguments.emplace_back(String::toAString(arguments[n]));
	}

	return parse(separatedArguments);
}

template <typename T>
T CommandArguments::value(const std::string& longName, const T& invalidValue, const bool allowDefaultValue, const size_t namelessValueIndex) const
{
	T validValue;
	if (hasValue<T>(longName, validValue, allowDefaultValue, namelessValueIndex))
	{
		return validValue;
	}

	return invalidValue;
}

template <>
inline bool CommandArguments::hasValue(const std::string& longName, bool& boolValue, const bool allowDefaultValue, const size_t namelessValueIndex) const
{
	Value value;
	if (!hasValue(longName, &value, allowDefaultValue, namelessValueIndex))
	{
		return false;
	}

	if (!value.isBool())
	{
		return false;
	}

	boolValue = value.boolValue();

	return true;
}

template <>
inline bool CommandArguments::hasValue(const std::string& longName, int32_t& intValue, const bool allowDefaultValue, const size_t namelessValueIndex) const
{
	Value value;
	if (!hasValue(longName, &value, allowDefaultValue, namelessValueIndex))
	{
		return false;
	}

	if (!value.isInt())
	{
		return false;
	}

	intValue = value.intValue();

	return true;
}

template <>
inline bool CommandArguments::hasValue(const std::string& longName, double& doubleValue, const bool allowDefaultValue, const size_t namelessValueIndex) const
{
	Value value;
	if (!hasValue(longName, &value, allowDefaultValue, namelessValueIndex))
	{
		return false;
	}

	if (!value.isFloat64(true /*allowIntAndFloat*/))
	{
		return false;
	}

	doubleValue = value.float64Value(true /*allowIntAndFloat*/);

	return true;
}

template <>
inline bool CommandArguments::hasValue(const std::string& longName, std::string& stringValue, const bool allowDefaultValue, const size_t namelessValueIndex) const
{
	Value value;
	if (!hasValue(longName, &value, allowDefaultValue, namelessValueIndex))
	{
		return false;
	}

	if (!value.isString())
	{
		return false;
	}

	stringValue = value.stringValue();

	return true;
}

inline const std::vector<std::string>& CommandArguments::namelessValues() const
{
	return namelessValues_;
}

template <typename TChar>
CommandArguments::ArgumentsT<TChar> CommandArguments::separateArguments(const TChar* commandLine)
{
	ArgumentsT<TChar> applicationArguments;

	if (!commandLine)
	{
		return applicationArguments;
	}

	ArgumentT<TChar> line(commandLine);

	while (!line.empty())
	{
		const typename ArgumentT<TChar>::size_type posQuote = line.find(quoteCharacter<TChar>());
		const typename ArgumentT<TChar>::size_type posSpace = line.find(spaceCharacter<TChar>());

		if (posQuote == 0)
		{
			// pase string command

			typename ArgumentT<TChar>::size_type posEndQuote = line.find(quoteCharacter<TChar>(), 1);

			while (posEndQuote != ArgumentT<TChar>::npos)
			{
				if (line[posEndQuote - 1] == backslashCharacter<TChar>())
				{
					posEndQuote = line.find(quoteCharacter<TChar>(), posEndQuote + 1);
				}
				else
				{
					applicationArguments.push_back(line.substr(1, posEndQuote - 1));
					line = line.substr(posEndQuote + 1);
					break;
				}
			}

			if (posEndQuote == ArgumentT<TChar>::npos)
			{
				Log::error() << "Invalid string command: " << String::toAString(line) << "The end quote is missing!";
				return applicationArguments;
			}

			continue;
		}

		// parse string-less command
		if (posSpace == ArgumentT<TChar>::npos)
		{
			ocean_assert(line.length() != 0);

			applicationArguments.push_back(line);
			break;
		}

		if (posSpace == 0)
		{
			line = line.substr(1);
		}
		else
		{
			applicationArguments.push_back(line.substr(0, posSpace));
			line = line.substr(posSpace + 1);
		}
	}

	return applicationArguments;
}

template <typename TChar>
bool CommandArguments::isLongParameter(const TChar* parameter)
{
	ocean_assert(parameter != nullptr);

	// we expect two '-' followed by at least one alphabetic character

	return parameter[0] == dashCharacter<TChar>() && parameter[1] == dashCharacter<TChar>() && parameter[2] != dashCharacter<TChar>() && std::isalpha(int(parameter[2])) != 0;
}

template <typename TChar>
bool CommandArguments::isShortParameter(const TChar* parameter)
{
	ocean_assert(parameter != nullptr);

	// we expect one '-' followed by at least one alphabetic character

	return parameter[0] == dashCharacter<TChar>() && std::isalpha(int(parameter[1])) != 0;
}

template <>
inline char CommandArguments::dashCharacter<char>()
{
	return '-';
}

template <>
inline wchar_t CommandArguments::dashCharacter<wchar_t>()
{
	return L'-';
}

template <>
inline char CommandArguments::quoteCharacter<char>()
{
	return '"';
}

template <>
inline wchar_t CommandArguments::quoteCharacter<wchar_t>()
{
	return L'"';
}

template <>
inline char CommandArguments::spaceCharacter<char>()
{
	return ' ';
}

template <>
inline wchar_t CommandArguments::spaceCharacter<wchar_t>()
{
	return L' ';
}

template <>
inline char CommandArguments::backslashCharacter<char>()
{
	return '\\';
}

template <>
inline wchar_t CommandArguments::backslashCharacter<wchar_t>()
{
	return L'\\';
}

}

#endif // META_OCEAN_BASE_COMMAND_ARGUMENTS_H
