/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/XMLConfig.h"

#include "ocean/base/String.h"

#include <tinyxml2.h>

namespace Ocean
{

namespace IO
{

XMLConfig::XMLValue::XMLValue(XMLConfig* config, tinyxml2::XMLNode* node) :
	Value(),
	xmlNode_(node),
	xmlConfig_(config)
{
	valueType_ = TYPE_UNDEFINED;
}

XMLConfig::XMLValue::~XMLValue()
{
	// nothing to do here
}

unsigned int XMLConfig::XMLValue::values() const
{
	if (xmlNode_ == nullptr)
	{
		return 0u;
	}

	unsigned int count = 0u;
	tinyxml2::XMLElement* element = xmlNode_->FirstChildElement();

	while (element)
	{
		++count;
		element = element->NextSiblingElement();
	}

	return count;
}

unsigned int XMLConfig::XMLValue::values(const std::string& name) const
{
	if (xmlNode_ == nullptr || name.empty())
	{
		return 0u;
	}

	unsigned int count = 0u;
	tinyxml2::XMLElement* element = xmlNode_->FirstChildElement(name.c_str());

	while (element)
	{
		++count;
		element = element->NextSiblingElement(name.c_str());
	}

	return count;
}

bool XMLConfig::XMLValue::exist(const std::string& name) const
{
	if (xmlNode_ == nullptr || name.empty())
	{
		return false;
	}

	return xmlNode_->FirstChildElement(name.c_str()) != nullptr;
}

XMLConfig::XMLValue& XMLConfig::XMLValue::value(const unsigned int index, std::string& name)
{
	name.clear();

	if (xmlConfig_ == nullptr || xmlNode_ == nullptr)
	{
		return nullValue();
	}

	unsigned int count = 0u;
	tinyxml2::XMLElement* element = xmlNode_->FirstChildElement();

	while (element)
	{
		if (count == index)
		{
			if (element->Name())
			{
				name = std::string(element->Name());
			}

			xmlConfig_->xmlIntermediateValues_.push_back(new XMLValue(xmlConfig_, element));
			return *xmlConfig_->xmlIntermediateValues_.back();
		}

		++count;
		element = element->NextSiblingElement();
	}

	return nullValue();
}

bool XMLConfig::XMLValue::value(const unsigned int index, std::string& name, Value** value)
{
	name.clear();

	if (xmlConfig_ == nullptr || xmlNode_ == nullptr)
	{
		return false;
	}

	unsigned int count = 0u;
	tinyxml2::XMLElement* element = xmlNode_->FirstChildElement();

	while (element)
	{
		if (count == index)
		{
			if (element->Name())
			{
				name = std::string(element->Name());
			}

			xmlConfig_->xmlIntermediateValues_.push_back(new XMLValue(xmlConfig_, element));
			*value = xmlConfig_->xmlIntermediateValues_.back();
			return true;
		}

		++count;
		element = element->NextSiblingElement();
	}

	return false;
}

XMLConfig::XMLValue& XMLConfig::XMLValue::value(const std::string& name, const unsigned int index)
{
	if (xmlConfig_ == nullptr || xmlNode_ == nullptr || name.empty())
	{
		return nullValue();
	}

	unsigned int count = 0u;
	tinyxml2::XMLElement* element = xmlNode_->FirstChildElement(name.c_str());

	while (element)
	{
		if (count == index)
		{
			xmlConfig_->xmlIntermediateValues_.push_back(new XMLValue(xmlConfig_, element));
			return *xmlConfig_->xmlIntermediateValues_.back();
		}

		++count;
		element = element->NextSiblingElement(name.c_str());
	}

	return nullValue();
}

bool XMLConfig::XMLValue::value(const std::string& name, const unsigned int index, Value** value)
{
	ocean_assert(value);

	if (xmlConfig_ == nullptr || xmlNode_ == nullptr || name.empty())
	{
		return false;
	}

	unsigned int count = 0u;
	tinyxml2::XMLElement* element = xmlNode_->FirstChildElement(name.c_str());

	while (element)
	{
		if (count == index)
		{
			xmlConfig_->xmlIntermediateValues_.push_back(new XMLValue(xmlConfig_, element));
			*value = xmlConfig_->xmlIntermediateValues_.back();
			return true;
		}

		++count;
		element = element->NextSiblingElement(name.c_str());
	}

	return false;
}

XMLConfig::XMLValue& XMLConfig::XMLValue::add(const std::string& name)
{
	if (xmlConfig_ == nullptr || xmlNode_ == nullptr || name.empty())
	{
		return nullValue();
	}

	tinyxml2::XMLElement* element = xmlNode_->GetDocument()->NewElement(name.c_str());
	xmlNode_->InsertEndChild(element);

	xmlConfig_->xmlIntermediateValues_.push_back(new XMLValue(xmlConfig_, element));
	return *xmlConfig_->xmlIntermediateValues_.back();
}

std::string XMLConfig::XMLValue::attribute(const std::string& name) const
{
	if (xmlConfig_ == nullptr || xmlNode_ == nullptr || name.empty())
	{
		return std::string();
	}

	tinyxml2::XMLElement* element = dynamic_cast<tinyxml2::XMLElement*>(xmlNode_);
	if (element == nullptr)
	{
		return std::string();
	}

	const char* value = element->Attribute(name.c_str());
	if (value == nullptr)
	{
		return std::string();
	}

	return std::string(value);
}

bool XMLConfig::XMLValue::operator()(const bool value) const
{
	if (xmlNode_ == nullptr)
	{
		return value;
	}

	const tinyxml2::XMLText* text = firstText(xmlNode_);

	if (!text || text->Value() == nullptr)
	{
		return value;
	}

	const std::string stringValue(text->Value());

	bool bValue;
	if (!String::isBoolean(stringValue, false, true, &bValue))
	{
		return value;
	}

	return bValue;
}

int XMLConfig::XMLValue::operator()(const int value) const
{
	if (xmlNode_ == nullptr)
	{
		return value;
	}

	const tinyxml2::XMLText* text = firstText(xmlNode_);

	if (!text || text->Value() == nullptr)
	{
		return value;
	}

	const std::string stringValue(text->Value());

	int iValue;
	if (!String::isInteger32(stringValue, &iValue))
	{
		return value;
	}

	return iValue;
}

double XMLConfig::XMLValue::operator()(const double value) const
{
	if (xmlNode_ == nullptr)
	{
		return value;
	}

	const tinyxml2::XMLText* text = firstText(xmlNode_);

	if (!text || text->Value() == nullptr)
	{
		return value;
	}

	const std::string stringValue(text->Value());

	double dValue;
	if (!String::isNumber(stringValue, true, &dValue))
	{
		return value;
	}

	return dValue;
}

std::string XMLConfig::XMLValue::operator()(const std::string& value) const
{
	if (xmlNode_ == nullptr || xmlNode_->ToElement() == nullptr || xmlNode_->ToElement()->GetText() == nullptr)
	{
		return value;
	}

	return std::string(xmlNode_->ToElement()->GetText());
}

std::vector<bool> XMLConfig::XMLValue::operator()(const std::vector<bool>& value) const
{
	ocean_assert(false && "Not supported!");
	return value;
}

std::vector<int> XMLConfig::XMLValue::operator()(const std::vector<int>& value) const
{
	ocean_assert(false && "Not supported!");
	return value;
}

std::vector<double> XMLConfig::XMLValue::operator()(const std::vector<double>& value) const
{
	ocean_assert(false && "Not supported!");
	return value;
}

std::vector<std::string> XMLConfig::XMLValue::operator()(const std::vector<std::string>& value) const
{
	ocean_assert(false && "Not supported!");
	return value;
}

bool XMLConfig::XMLValue::operator=(const bool value)
{
	if (xmlNode_ == nullptr || xmlNode_->ToElement() == nullptr)
	{
		return false;
	}

	tinyxml2::XMLText* text = firstText(xmlNode_);

	if (text)
	{
		text->SetValue(value ? "true" : "false");
	}
	else
	{
		ocean_assert(xmlNode_->GetDocument());
		tinyxml2::XMLText* newText = xmlNode_->GetDocument()->NewText(value ? "true" : "false");

		xmlNode_->InsertEndChild(newText);
	}

	return true;
}

bool XMLConfig::XMLValue::operator=(const int value)
{
	if (xmlNode_ == nullptr || xmlNode_->ToElement() == nullptr)
	{
		return false;
	}

	tinyxml2::XMLText* text = firstText(xmlNode_);

	if (text)
	{
		text->SetValue(String::toAString(value).c_str());
	}
	else
	{
		ocean_assert(xmlNode_->GetDocument());
		tinyxml2::XMLText* newText = xmlNode_->GetDocument()->NewText(String::toAString(value).c_str());

		xmlNode_->InsertEndChild(newText);
	}

	return true;
}

bool XMLConfig::XMLValue::operator=(const double value)
{
	if (xmlNode_ == nullptr || xmlNode_->ToElement() == nullptr)
	{
		return false;
	}

	tinyxml2::XMLText* text = firstText(xmlNode_);

	if (text)
	{
		text->SetValue(String::toAString(value, 15).c_str());
	}
	else
	{
		ocean_assert(xmlNode_->GetDocument());
		tinyxml2::XMLText* newText = xmlNode_->GetDocument()->NewText(String::toAString(value, 15).c_str());

		xmlNode_->InsertEndChild(newText);
	}

	return true;
}

bool XMLConfig::XMLValue::operator=(const std::string& value)
{
	if (xmlNode_ == nullptr || xmlNode_->ToElement() == nullptr)
	{
		return false;
	}

	tinyxml2::XMLText* text = firstText(xmlNode_);

	if (text)
	{
		text->SetValue(value.c_str());
	}
	else
	{
		ocean_assert(xmlNode_->GetDocument());
		tinyxml2::XMLText* newText = xmlNode_->GetDocument()->NewText(value.c_str());

		xmlNode_->InsertEndChild(newText);
	}

	return true;
}

bool XMLConfig::XMLValue::operator=(const std::vector<bool>& /*values*/)
{
	ocean_assert(false && "Not supported!");
	return false;
}

bool XMLConfig::XMLValue::operator=(const std::vector<int>& /*values*/)
{
	ocean_assert(false && "Not supported!");
	return false;
}

bool XMLConfig::XMLValue::operator=(const std::vector<double>& /*values*/)
{
	ocean_assert(false && "Not supported!");
	return false;
}

bool XMLConfig::XMLValue::operator=(const std::vector<std::string>& /*values*/)
{
	ocean_assert(false && "Not supported!");
	return false;
}

XMLConfig::XMLValue& XMLConfig::XMLValue::operator[](const std::string& name)
{
	Value* xmlValue = nullptr;
	if (value(name, 0u, &xmlValue))
	{
		return dynamic_cast<XMLValue&>(*xmlValue);
	}

	return add(name);
}

XMLConfig::XMLValue::operator bool() const
{
	return xmlConfig_ != nullptr && xmlNode_ != nullptr;
}

XMLConfig::XMLConfig() :
	inputStream_(inputFileStream_)
{
	// nothing to do here
}

XMLConfig::XMLConfig(const std::string& filename, const bool read) :
	xmlDocument_(new tinyxml2::XMLDocument()),
	filename_(filename),
	inputStream_(inputFileStream_)
{
	inputFileStream_.open(filename.c_str(), std::ios::binary);

	if (read)
	{
		XMLConfig::read();
	}
}

XMLConfig::XMLConfig(std::istream& inputStream, const bool read) :
	xmlDocument_(new tinyxml2::XMLDocument()),
	inputStream_(inputStream)
{
	if (read)
	{
		XMLConfig::read();
	}
}

XMLConfig::~XMLConfig()
{
	for (XMLValues::iterator i = xmlIntermediateValues_.begin(); i != xmlIntermediateValues_.end(); ++i)
	{
		delete *i;
	}

	delete xmlDocument_;
}

bool XMLConfig::setFilename(const std::string& filename, const bool read)
{
	// check whether the configuration object has been initialized with an explicit input stream
	if (&inputFileStream_ != &inputStream_)
	{
		return false;
	}

	inputFileStream_.open(filename.c_str(), std::ios::binary);

	if (inputFileStream_.bad())
	{
		return false;
	}

	filename_ = filename;

	if (read)
	{
		return XMLConfig::read();
	}

	return true;
}

bool XMLConfig::read()
{
	if (inputStream_.bad())
	{
		return false;
	}

	xmlDocument_->DeleteChildren();

	ocean_assert(xmlDocument_);

	inputStream_.seekg(0, std::istream::end);
	const std::streampos streamPosition = inputStream_.tellg();

	// check whether the file does not exist
	if (streamPosition == std::streampos(-1))
	{
		return true;
	}

	const std::size_t length = size_t(streamPosition);
	inputStream_.seekg(0, std::istream::beg);

	if (length == 0)
	{
		return true;
	}

	std::vector<unsigned char> buffer(length);
	inputStream_.read((char*)buffer.data(), length);

	const tinyxml2::XMLError xmlOpenResult = xmlDocument_->Parse((char*)buffer.data(), buffer.size());

	if (xmlOpenResult != tinyxml2::XML_SUCCESS)
	{
		std::string errorName, errorStr;

#if defined(TINYXML2_MAJOR_VERSION) && TINYXML2_MAJOR_VERSION >= 6
		if (xmlDocument_->ErrorName())
		{
			errorName = std::string(xmlDocument_->ErrorName());
		}
		if (xmlDocument_->ErrorStr())
		{
			errorStr = std::string(xmlDocument_->ErrorStr());
		}
#else
		if (xmlDocument_->GetErrorStr1())
		{
			errorName = std::string(xmlDocument_->GetErrorStr1());
		}
		if (xmlDocument_->GetErrorStr2())
		{
			errorStr = std::string(xmlDocument_->GetErrorStr2());
		}
#endif

		if (!errorName.empty())
		{
			Log::warning() << "Failed to read XML file \"" << filename_ << "\": " << errorName;

			if (!errorStr.empty())
			{
				Log::warning() << "Detailed problem: " << errorStr;
			}
		}
		else
		{
			ocean_assert(errorStr.empty());
		}

		return false;
	}

	return true;
}

bool XMLConfig::write()
{
	if (filename_.empty())
	{
		return false;
	}

	ocean_assert(&inputFileStream_ == &inputStream_);

	// we have to give up the file stream as otherwise tinyxml could not access the file
	inputFileStream_.close();

	ocean_assert(xmlDocument_);
	const tinyxml2::XMLError xmlOpenResult = xmlDocument_->SaveFile(filename_.c_str());

	// we will cover the file again
	inputFileStream_.open(filename_.c_str(), std::ios::binary);

	if (inputFileStream_.bad())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (xmlOpenResult != tinyxml2::XML_SUCCESS)
	{
		std::string errorName, errorStr;

#if defined(TINYXML2_MAJOR_VERSION) && TINYXML2_MAJOR_VERSION >= 6
		if (xmlDocument_->ErrorName())
		{
			errorName = std::string(xmlDocument_->ErrorName());
		}
		if (xmlDocument_->ErrorStr())
		{
			errorStr = std::string(xmlDocument_->ErrorStr());
		}
#else
		if (xmlDocument_->GetErrorStr1())
		{
			errorName = std::string(xmlDocument_->GetErrorStr1());
		}
		if (xmlDocument_->GetErrorStr2())
		{
			errorStr = std::string(xmlDocument_->GetErrorStr2());
		}
#endif

		if (!errorName.empty())
		{
			Log::warning() << "Failed to write XML file \"" << filename_ << "\": " << errorName;

			if (!errorStr.empty())
			{
				Log::warning() << "Detailed problem: " << errorStr;
			}
		}
		else
		{
			ocean_assert(errorStr.empty());
		}

		return false;
	}

	return true;
}

unsigned int XMLConfig::values() const
{
	ocean_assert(xmlDocument_);

	unsigned int count = 0u;
	const tinyxml2::XMLElement* element = xmlDocument_->FirstChildElement();

	while (element)
	{
		++count;
		element = element->NextSiblingElement();
	}

	return count;
}

unsigned int XMLConfig::values(const std::string& name) const
{
	if (name.empty())
	{
		return 0u;
	}

	ocean_assert(xmlDocument_);

	unsigned int count = 0u;
	const tinyxml2::XMLElement* element = xmlDocument_->FirstChildElement(name.c_str());

	while (element)
	{
		++count;
		element = element->NextSiblingElement(name.c_str());
	}

	return count;
}

bool XMLConfig::exist(const std::string& name) const
{
	if (name.empty())
	{
		return false;
	}

	ocean_assert(xmlDocument_);
	return xmlDocument_->FirstChildElement(name.c_str()) != nullptr;
}

XMLConfig::XMLValue& XMLConfig::value(const unsigned int index, std::string& name)
{
	ocean_assert(xmlDocument_);

	XMLValue value(this, xmlDocument_);
	return value.value(index, name);
}

bool XMLConfig::value(const unsigned int index, std::string& name, Value** value)
{
	ocean_assert(value);
	ocean_assert(xmlDocument_);

	XMLValue xmlValue(this, xmlDocument_);
	return xmlValue.value(index, name, value);
}

XMLConfig::XMLValue& XMLConfig::value(const std::string& name, const unsigned int index)
{
	ocean_assert(xmlDocument_);

	XMLValue value(this, xmlDocument_);
	return value.value(name, index);
}

bool XMLConfig::value(const std::string& name, const unsigned int index, Value** value)
{
	ocean_assert(value);
	ocean_assert(xmlDocument_);

	XMLValue xmlValue(this, xmlDocument_);
	return xmlValue.value(name, index, value);
}

XMLConfig::XMLValue& XMLConfig::add(const std::string& name)
{
	if (name.empty())
	{
		return nullValue();
	}

	ocean_assert(xmlDocument_);

	tinyxml2::XMLElement* newElement = xmlDocument_->NewElement(name.c_str());
	ocean_assert(newElement);

	xmlDocument_->InsertEndChild(newElement);

	xmlIntermediateValues_.push_back(new XMLValue(this, newElement));
	return *xmlIntermediateValues_.back();
}

XMLConfig::XMLValue& XMLConfig::operator[](const std::string& name)
{
	ocean_assert(xmlDocument_);

	Value* xmlValue = nullptr;
	if (value(name, 0u, &xmlValue))
	{
		return dynamic_cast<XMLValue&>(*xmlValue);
	}

	return add(name);
}

bool XMLConfig::isEmpty() const
{
	ocean_assert(xmlDocument_);
	return xmlDocument_->FirstChildElement() != nullptr;
}

const tinyxml2::XMLText* XMLConfig::firstText(const tinyxml2::XMLNode* xmlNode)
{
	if (xmlNode == nullptr)
	{
		return nullptr;
	}

	const tinyxml2::XMLText* text = xmlNode->ToText();

	if (text)
	{
		return text;
	}

	const tinyxml2::XMLNode* node = xmlNode->FirstChild();

	while (node)
	{
		text = node->ToText();

		if (text)
		{
			return text;
		}

		node = node->NextSibling();
	}

	return nullptr;
}

tinyxml2::XMLText* XMLConfig::firstText(tinyxml2::XMLNode* xmlNode)
{
	if (xmlNode == nullptr)
	{
		return nullptr;
	}

	tinyxml2::XMLText* text = xmlNode->ToText();

	if (text)
	{
		return text;
	}

	tinyxml2::XMLNode* node = xmlNode->FirstChild();

	while (node)
	{
		text = node->ToText();

		if (text)
		{
			return text;
		}

		node = node->NextSibling();
	}

	return nullptr;
}

XMLConfig::XMLValue& XMLConfig::nullValue()
{
	static XMLConfig::XMLValue value(nullptr, nullptr);
	return value;
}

} // namespace IO

} // namespace Ocean
