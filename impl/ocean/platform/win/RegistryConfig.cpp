/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/RegistryConfig.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

RegistryConfig::RegistryValue::RegistryValue()
{
	ocean_assert(valueType_ == TYPE_INVALID);
	valueType_ = TYPE_UNDEFINED;
}

RegistryConfig::RegistryValue::RegistryValue(const HKEY key)
{
	valueType_ = TYPE_GROUP;

	Registry::Names names = Registry::keys(key);

	for (Registry::Names::const_iterator i = names.begin(); i != names.end(); ++i)
	{
		ocean_assert(subValues_.find(*i) == subValues_.end());

		HKEY subKey = Registry::openKey(key, *i);
		ocean_assert(subKey != nullptr);

		subValues_[*i] = RegistryValue(subKey);

		Registry::closeKey(subKey);
	}

	names = Registry::values(key);

	for (Registry::Names::const_iterator i = names.begin(); i != names.end(); ++i)
	{
		ocean_assert(subValues_.find(*i) == subValues_.end());

		subValues_[*i] = RegistryValue(key, *i);
	}
}

RegistryConfig::RegistryValue::RegistryValue(const HKEY key, const std::string name)
{
	Registry::ValueType type = Registry::valueType(key, name);

	switch (type)
	{
		case Registry::VALUE_INTEGER32:
			string_ = String::toAString(Registry::value(key, name, 0));
			valueType_ = TYPE_INT;
			break;

		case Registry::VALUE_STRING:
			string_ = Registry::value(key, name, "");
			valueType_ = TYPE_STRING;
			break;

		default:
			valueType_ = TYPE_UNDEFINED;
	};
}

RegistryConfig::RegistryValue::~RegistryValue()
{
	// nothing to do here
}

unsigned int RegistryConfig::RegistryValue::values() const
{
	return (unsigned int)(subValues_.size());
}

unsigned int RegistryConfig::RegistryValue::values(const std::string& name) const
{
	return subValues_.find(name) != subValues_.end() ? 1u : 0u;
}

bool RegistryConfig::RegistryValue::exist(const std::string& name) const
{
	return subValues_.find(name) != subValues_.end();
}

RegistryConfig::RegistryValue& RegistryConfig::RegistryValue::value(const unsigned int index, std::string& name)
{
	Values::iterator i = subValues_.begin();

	unsigned int n = 0u;
	while (n != index)
	{
		if (i == subValues_.end())
		{
			return nullValue();
		}

		++i;
		++n;
	}

	if (i == subValues_.end())
	{
		return nullValue();
	}

	name = i->first;
	return i->second;
}

bool RegistryConfig::RegistryValue::value(const unsigned int index, std::string& name, Value** value)
{
	ocean_assert(value);

	Values::iterator i = subValues_.begin();

	unsigned int n = 0u;
	while (n != index)
	{
		if (i == subValues_.end())
		{
			return false;
		}

		++i;
		++n;
	}

	if (i == subValues_.end())
	{
		return false;
	}

	name = i->first;
	*value = &i->second;
	return true;
}

RegistryConfig::RegistryValue& RegistryConfig::RegistryValue::value(const std::string& name, const unsigned int index)
{
	Values::iterator i = subValues_.find(name);
	if (i == subValues_.end() || index != 0u)
	{
		return nullValue();
	}

	return i->second;
}

bool RegistryConfig::RegistryValue::value(const std::string& name, const unsigned int index, Value** value)
{
	ocean_assert(value);

	Values::iterator i = subValues_.find(name);
	if (i == subValues_.end() || index != 0u)
	{
		return false;
	}

	*value = &i->second;
	return true;
}

bool RegistryConfig::RegistryValue::operator()(const bool value) const
{
	if (valueType_ == TYPE_BOOL)
	{
		return boolValue();
	}

	if (valueType_ == TYPE_INT)
	{
		return integerValue() != 0u;
	}

	return value;
}

int RegistryConfig::RegistryValue::operator()(const int value) const
{
	if (valueType_ == TYPE_INT)
	{
		return integerValue();
	}

	return value;
}

double RegistryConfig::RegistryValue::operator()(const double value) const
{
	if (valueType_ == TYPE_NUMBER)
	{
		return numberValue();
	}

	if (valueType_ == TYPE_INT)
	{
		return double(integerValue());
	}

	if (valueType_ == TYPE_STRING)
	{
		return double(atof(string_.c_str()));
	}

	return value;
}

std::string RegistryConfig::RegistryValue::operator()(const std::string& value) const
{
	if (valueType_ == TYPE_STRING)
	{
		return string_;
	}

	return value;
}

bool RegistryConfig::RegistryValue::operator=(const bool value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_BOOL;
	string_ = value ? "true" : "false";

	return true;
}

bool RegistryConfig::RegistryValue::operator=(const int value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_INT;
	string_ = String::toAString(value);

	return true;
}

bool RegistryConfig::RegistryValue::operator=(const double value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_NUMBER;
	string_ = String::toAString(value);

	return true;
}

bool RegistryConfig::RegistryValue::operator=(const std::string& value)
{
	if (valueType_ == TYPE_GROUP || valueType_ == TYPE_INVALID)
	{
		return false;
	}

	valueType_ = TYPE_STRING;
	string_ = value;

	return true;
}

RegistryConfig::RegistryValue& RegistryConfig::RegistryValue::operator[](const std::string& name)
{
	if ((valueType_ != TYPE_GROUP && valueType_ != TYPE_UNDEFINED) || name.empty())
	{
		return nullValue();
	}

	valueType_ = TYPE_GROUP;

	Values::iterator i = subValues_.find(name);
	if (i == subValues_.end())
	{
		i = subValues_.insert(std::make_pair(name, RegistryValue())).first;
	}

	ocean_assert(i != subValues_.end());

	return i->second;
}

bool RegistryConfig::RegistryValue::boolValue() const
{
	ocean_assert(string_ == "true" || string_ == "false");
	return string_ == "true";
}

int RegistryConfig::RegistryValue::integerValue() const
{
	for (unsigned int n = 0; n < string_.length(); ++n)
	{
		ocean_assert(string_[n] >= '0' && string_[n] <= '9' || string_[n] == '-');
	}

	if (string_.empty())
	{
		return 0;
	}

	return atoi(string_.c_str());
}

double RegistryConfig::RegistryValue::numberValue() const
{
	if (string_.empty())
	{
		return 0.0;
	}

	return atof(string_.c_str());
}

bool RegistryConfig::RegistryValue::writeToRegistry(const HKEY key, const std::string& name) const
{
	if (key == nullptr)
	{
		return false;
	}

	switch (valueType_)
	{
		case TYPE_BOOL:
			return Registry::setValue(key, name, boolValue());

		case TYPE_INT:
			return Registry::setValue(key, name, integerValue());

		case TYPE_NUMBER:
		case TYPE_STRING:
			return Registry::setValue(key, name, string_);

		case TYPE_GROUP:
		{
			HKEY subKey = Registry::openKey(key, name);

			bool oneSucceeded = false;

			for (Values::const_iterator i = subValues_.begin(); i != subValues_.end(); ++i)
			{
				const RegistryValue& value = dynamic_cast<const RegistryValue&>(i->second);
				if (value.writeToRegistry(subKey, i->first))
				{
					oneSucceeded = true;
				}
			}

			Registry::closeKey(subKey);

			return oneSucceeded;
		}

		case TYPE_INVALID:
		case TYPE_UNDEFINED:
		case TYPE_MULTI_BOOL:
		case TYPE_MULTI_INT:
		case TYPE_MULTI_NUMBER:
		case TYPE_MULTI_STRING:
			break;
	};

	return false;
}

RegistryConfig::RegistryConfig(const std::string& path, const Registry::RootType rootType) :
	rootType_(rootType),
	path_(path)
{
	read();
}

RegistryConfig::~RegistryConfig()
{
	if (double(Timestamp(true) - writeTimestamp()) > 1.0)
	{
		write();
	}
}

bool RegistryConfig::read()
{
	values_.clear();

	HKEY key = Registry::openKey(rootType_, path_);
	if (key == nullptr)
	{
		return false;
	}

	Registry::Names names = Registry::keys(key);

	for (Registry::Names::const_iterator i = names.begin(); i != names.end(); ++i)
	{
		ocean_assert(values_.find(*i) == values_.end());

		HKEY subKey = Registry::openKey(key, *i);
		ocean_assert(subKey != nullptr);

		values_[*i] = RegistryValue(subKey);

		Registry::closeKey(subKey);
	}

	names = Registry::values(key);

	for (Registry::Names::const_iterator i = names.begin(); i != names.end(); ++i)
	{
		ocean_assert(values_.find(*i) == values_.end());

		values_[*i] = RegistryValue(key, *i);
	}

	Registry::closeKey(key);

	return true;
}

bool RegistryConfig::write()
{
	HKEY key = Registry::openKey(rootType_, path_);
	if (key == nullptr)
	{
		return false;
	}

	bool oneSucceeded = false;

	for (Values::const_iterator i = values_.begin(); i != values_.end(); ++i)
	{
		const RegistryValue& value = dynamic_cast<const RegistryValue&>(i->second);
		if (value.writeToRegistry(key, i->first))
		{
			oneSucceeded = true;
		}
	}

	Registry::closeKey(key);

	if (oneSucceeded)
	{
		writeTimestamp_.toNow();
	}

	return oneSucceeded;
}

unsigned int RegistryConfig::values() const
{
	return (unsigned int)(values_.size());
}

unsigned int RegistryConfig::values(const std::string& name) const
{
	return values_.find(name) != values_.end() ? 1u : 0u;
}

bool RegistryConfig::exist(const std::string& name) const
{
	return values_.find(name) != values_.end();
}

RegistryConfig::RegistryValue& RegistryConfig::value(const unsigned int index, std::string& name)
{
	Values::iterator i = values_.begin();

	unsigned int n = 0u;
	while (n != index)
	{
		if (i == values_.end())
		{
			return nullValue();
		}

		++i;
		++n;
	}

	name = i->first;
	return i->second;
}

bool RegistryConfig::value(const unsigned int index, std::string& name, Value** value)
{
	ocean_assert(value);

	Values::iterator i = values_.begin();

	unsigned int n = 0u;
	while (n != index)
	{
		if (i == values_.end())
		{
			return false;
		}

		++i;
		++n;
	}

	name = i->first;
	*value = &i->second;
	return true;
}

RegistryConfig::RegistryValue& RegistryConfig::value(const std::string& name, const unsigned int index)
{
	Values::iterator i = values_.find(name);
	if (i == values_.end() || index != 0u)
	{
		return nullValue();
	}

	return i->second;
}

bool RegistryConfig::value(const std::string& name, const unsigned int index, Value** value)
{
	ocean_assert(value);

	Values::iterator i = values_.find(name);
	if (i == values_.end() || index != 0u)
	{
		return false;
	}

	*value = &i->second;
	return true;
}

RegistryConfig::RegistryValue& RegistryConfig::operator[](const std::string& name)
{
	if (name.empty())
	{
		return nullValue();
	}

	Values::iterator i = values_.find(name);
	if (i == values_.end())
	{
		i = values_.insert(std::make_pair(name, RegistryValue())).first;
	}

	ocean_assert(i != values_.end());

	return i->second;
}

RegistryConfig::RegistryValue& RegistryConfig::nullValue()
{
	static RegistryConfig::RegistryValue value;
	return value;
}

} // namespace Win

} // namespace Platform

} // namespace Ocean
