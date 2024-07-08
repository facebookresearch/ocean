/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Config.h"

#include "ocean/base/Exception.h"

namespace Ocean
{

Config::ValueType Config::Value::type() const
{
	return valueType_;
}

bool Config::Value::isMultiType() const
{
	return valueType_ == TYPE_MULTI_BOOL || valueType_ == TYPE_MULTI_INT || valueType_ == TYPE_MULTI_NUMBER || valueType_ == TYPE_MULTI_STRING;
}

unsigned int Config::Value::values() const
{
	return 0u;
}

unsigned int Config::Value::values(const std::string& /*name*/) const
{
	return 0u;
}

bool Config::Value::exist(const std::string& /*name*/) const
{
	return false;
}

Config::Value& Config::Value::value(const unsigned int /*index*/, std::string& name)
{
	name = std::string();
	return nullValue();
}

bool Config::Value::value(const unsigned int /*index*/, std::string& /*name*/, Value** /*value*/)
{
	return false;
}

Config::Value& Config::Value::value(const std::string& /*name*/, const unsigned int /*index*/)
{
	return nullValue();
}

bool Config::Value::value(const std::string& /*name*/, const unsigned int /*index*/, Value** /*value*/)
{
	return false;
}

Config::Value& Config::Value::add(const std::string& /*name*/)
{
	return nullValue();
}

bool Config::Value::operator()(const bool value) const
{
	return value;
}

int Config::Value::operator()(const int value) const
{
	return value;
}

double Config::Value::operator()(const double value) const
{
	return value;
}

std::string Config::Value::operator()(const std::string& value) const
{
	return value;
}

std::vector<bool> Config::Value::operator()(const std::vector<bool>& value) const
{
	return value;
}

std::vector<int> Config::Value::operator()(const std::vector<int>& value) const
{
	return value;
}

std::vector<double> Config::Value::operator()(const std::vector<double>& value) const
{
	return value;
}

std::vector<std::string> Config::Value::operator()(const std::vector<std::string>& value) const
{
	return value;
}

bool Config::Value::operator=(const bool /*value*/)
{
	return false;
}

bool Config::Value::operator=(const int /*value*/)
{
	return false;
}

bool Config::Value::operator=(const double /*value*/)
{
	return false;
}

bool Config::Value::operator=(const std::string& /*value*/)
{
	return false;
}

bool Config::Value::operator=(const std::vector<bool>& /*values*/)
{
	return false;
}

bool Config::Value::operator=(const std::vector<int>& /*values*/)
{
	return false;
}

bool Config::Value::operator=(const std::vector<double>& /*values*/)
{
	return false;
}

bool Config::Value::operator=(const std::vector<std::string>& /*values*/)
{
	return false;
}

Config::Value& Config::Value::operator[](const std::string& /*name*/)
{
	return nullValue();
}

Config::Config()
{
	// nothing to do here
}

Config::~Config()
{
	// nothing to do here
}

bool Config::read()
{
	return false;
}

bool Config::write()
{
	return false;
}

unsigned int Config::values() const
{
	return 0u;
}

unsigned int Config::values(const std::string& /*name*/) const
{
	return 0u;
}

bool Config::exist(const std::string& /*name*/) const
{
	return false;
}

Config::Value& Config::value(const unsigned int /*index*/, std::string& /*name*/)
{
	return nullValue();
}

bool Config::value(const unsigned int /*index*/, std::string& /*name*/, Value** /*value*/)
{
	return false;
}

Config::Value& Config::value(const std::string& /*name*/, const unsigned int /*index*/)
{
	return nullValue();
}

bool Config::value(const std::string& /*name*/, const unsigned int /*index*/, Value** /*value*/)
{
	return false;
}

Config::Value& Config::operator[](const std::string& /*name*/)
{
	return nullValue();
}

Config::Value& Config::add(const std::string& /*name*/)
{
	return nullValue();
}

Config::Value& Config::nullValue()
{
	static Config::Value value;
	return value;
}

} // namespace Ocean
