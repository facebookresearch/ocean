/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/CommandArguments.h"

namespace Ocean
{

CommandArguments::Manager::Manager()
{
	// nothing to do here
}

bool CommandArguments::Manager::setRawArguments(const char* const* arguments, const size_t size)
{
	const ScopedLock scopedLock(lock_);

	if (size_ != 0)
	{
		ocean_assert(false && "Arguments already registered!");
		return false;
	}

	if (arguments == nullptr || size == 0)
	{
		return false;
	}

	ocean_assert(argumentsChar_ == nullptr && argumentsWchar_ == nullptr);

	argumentsChar_ = arguments;
	size_ = size;

	return true;
}

bool CommandArguments::Manager::setRawArguments(const wchar_t* const* arguments, const size_t size)
{
	const ScopedLock scopedLock(lock_);

	if (size_ != 0)
	{
		ocean_assert(false && "Arguments already registered!");
		return false;
	}

	if (arguments == nullptr || size == 0)
	{
		return false;
	}

	ocean_assert(argumentsChar_ == nullptr && argumentsWchar_ == nullptr);

	argumentsWchar_ = arguments;
	size_ = size;

	return true;
}

template <>
const char* const* CommandArguments::Manager::rawArguments<char>() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(argumentsWchar_ == nullptr && "Wrong data type!");

	return argumentsChar_;
}

template <>
const wchar_t* const* CommandArguments::Manager::rawArguments<wchar_t>() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(argumentsChar_ == nullptr && "Wrong data type!");

	return argumentsWchar_;
}

size_t CommandArguments::Manager::size() const
{
	const ScopedLock scopedLock(lock_);

	return size_;
}

CommandArguments::Parameter::Parameter(const std::string& longName, const std::string& shortName, const std::string& description, const Value& defaultValue) :
	longName_(longName),
	shortName_(shortName),
	description_(description),
	defaultValue_(defaultValue)
{
	// nothing to do here
}

CommandArguments::CommandArguments()
{
	// nothing to do here
}

CommandArguments::CommandArguments(const std::string& applicationDescription) :
	applicationDescription_(applicationDescription)
{
	// nothing to do here
}

bool CommandArguments::registerParameter(const std::string& longName, const std::string& shortName, const std::string& description, const Value& defaultValue)
{
	if (longName.empty())
	{
		ocean_assert(false && "The long arguments must be defined!");
		return false;
	}

	if (!std::isalpha(longName[0]))
	{
		ocean_assert(false && "Invalid long name, must start with alphabetic character");
		return false;
	}

	if (parameterMap_.find(longName) != parameterMap_.cend() || shortToLongMap_.find(longName) != shortToLongMap_.cend())
	{
		ocean_assert(false && "The long argument is already in use!");
		return false;
	}

	if (!shortName.empty())
	{
		if (!std::isalpha(shortName[0]))
		{
			ocean_assert(false && "Invalid short name, must start with alphabetic character");
			return false;
		}

		if (shortToLongMap_.find(shortName) != shortToLongMap_.cend() || parameterMap_.find(longName) != parameterMap_.cend())
		{
			ocean_assert(false && "The short argument is already in use!");
			return false;
		}
	}

	parameterMap_.insert(std::make_pair(longName, Parameter(longName, shortName, description, defaultValue)));

	if (!shortName.empty())
	{
		shortToLongMap_.insert(std::make_pair(shortName, longName));
	}

	return true;
}

bool CommandArguments::registerNamelessParameters(std::string&& description)
{
	if (!descriptionNamelessParameters_.empty())
	{
		return false;
	}

	descriptionNamelessParameters_ = std::move(description);

	return true;
}

bool CommandArguments::parse(const ArgumentsT<char>& separatedArguments)
{
	bool invalidParameter = false;

	for (size_t n = 0; n < separatedArguments.size(); ++n)
	{
		const ArgumentT<char>& argument = separatedArguments[n];

		// this is the last value

		ParameterMap::iterator iParameter = parameterMap_.end();

		if (isLongParameter<char>(argument.c_str()))
		{
			iParameter = parameterMap_.find(argument.c_str() + 2);
		}
		else if (isShortParameter<char>(argument.c_str()))
		{
			const ShortToLongMap::const_iterator iShort = shortToLongMap_.find(argument.c_str() + 1);

			if (iShort != shortToLongMap_.cend())
			{
				iParameter = parameterMap_.find(iShort->second);
			}
		}
		else
		{
			// we have a nameless argument

			namelessValues_.push_back(argument);

			continue;
		}

		if (iParameter != parameterMap_.end())
		{
			if (valueMap_.find(iParameter->first) != valueMap_.cend())
			{
				Log::warning() << "Overwriting parameter \"" << iParameter->first << "\"";
			}

			const std::string* valueArgument = nullptr;

			if (n + 1 < separatedArguments.size())
			{
				if (!isLongParameter<char>(separatedArguments[n + 1].c_str()) && !isShortParameter<char>(separatedArguments[n + 1].c_str()))
				{
					valueArgument = &separatedArguments[n + 1];
				}
			}

			if (valueArgument)
			{
				Value value;

				bool boolValue;
				int intValue;
				double numberValue;

				if (String::isBoolean(*valueArgument, false, false, &boolValue))
				{
					value = Value(boolValue);
				}
				else if (String::isInteger32(*valueArgument, &intValue))
				{
					value = Value(intValue);
				}
				else if (String::isNumber(*valueArgument, true, &numberValue))
				{
					value = Value(numberValue);
				}
				else
				{
					value = Value(*valueArgument);
				}

				valueMap_[iParameter->first] = value;

				// jumping to next argument
				++n;
			}
			else
			{
				// we set an invalid parameter to allow a parameter without value
				valueMap_[iParameter->first] = Value();
			}
		}
		else
		{
			Log::error() << "Unknown parameter \"" + argument + "\" - skipping it";

			invalidParameter = true;
		}
	}

	return !invalidParameter;
}

bool CommandArguments::parse(const ArgumentsT<wchar_t>& separatedArguments)
{
	ArgumentsT<char> cSeparatedArguments;
	cSeparatedArguments.reserve(separatedArguments.size());

	for (const ArgumentT<wchar_t>& separatedArgument : separatedArguments)
	{
		cSeparatedArguments.emplace_back(String::toAString(separatedArgument));
	}

	return parse(cSeparatedArguments);
}

Value CommandArguments::value(const std::string& longName, const bool allowDefaultValue, const size_t namelessValueIndex) const
{
	const ValueMap::const_iterator iValue = valueMap_.find(longName);

	if (iValue != valueMap_.cend())
	{
		return iValue->second;
	}

	if (allowDefaultValue)
	{
		const ParameterMap::const_iterator iParameter = parameterMap_.find(longName);

		if (iParameter != parameterMap_.cend())
		{
			return iParameter->second.defaultValue();
		}
	}

	if (namelessValueIndex < namelessValues_.size())
	{
		return Value(namelessValues_[namelessValueIndex]);
	}

	return Value();
}

bool CommandArguments::hasValue(const std::string& longName, Value* value, const bool allowDefaultValue, const size_t namelessValueIndex) const
{
	const ValueMap::const_iterator iValue = valueMap_.find(longName);

	if (iValue != valueMap_.cend())
	{
		if (value)
		{
			*value = iValue->second;
		}

		return true;
	}

	if (allowDefaultValue)
	{
		const ParameterMap::const_iterator iParameter = parameterMap_.find(longName);

		if (iParameter != parameterMap_.cend() && iParameter->second.defaultValue())
		{
			if (value)
			{
				*value = iParameter->second.defaultValue();
			}

			return true;
		}
	}

	if (namelessValueIndex < namelessValues_.size())
	{
		if (value)
		{
			*value = Value(namelessValues_[namelessValueIndex]);
		}

		return true;
	}

	return false;
}

std::string CommandArguments::makeSummary()
{
	std::string summary;

	if (!applicationDescription_.empty())
	{
		summary += "Description:\n   " + applicationDescription_ + "\n\n";
	}

	summary += "Options:\n";

	bool firstParameter = true;

	if (!descriptionNamelessParameters_.empty())
	{
		summary += "   " + descriptionNamelessParameters_ + "\n";

		firstParameter = false;
	}

	for (ParameterMap::const_iterator i = parameterMap_.cbegin(); i != parameterMap_.cend(); ++i)
	{
		if (!firstParameter)
		{
			summary += "\n";
		}

		firstParameter = false;

		ocean_assert(!i->second.longName().empty());

		if (i->second.shortName().empty())
		{
			summary += "   --" + i->second.longName() + ":\n";
		}
		else
		{
			summary += "   --" + i->second.longName() + ", or -" + i->second.shortName() + ":\n";
		}

		if (!i->second.description().empty())
		{
			summary += "     " + i->second.description() + "\n";
		}

		if (i->second.defaultValue())
		{
			summary += "     Default value: " + i->second.defaultValue().readableString() + "\n";
		}
	}

	return summary;
}

}
