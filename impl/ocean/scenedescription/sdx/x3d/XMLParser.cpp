/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/XMLParser.h"
#include "ocean/scenedescription/sdx/x3d/Factory.h"

#include "ocean/base/String.h"

#include "ocean/io/File.h"

#include "ocean/scenedescription/DynamicNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

XMLParser::XMLScanner::XMLScanner(const std::string& data) :
	Scanner(std::string(), data, nullptr, nullptr)
{
	registerWhiteSpaceCharacter(',');
}

XMLParser::XMLParser(const std::string& filename, float* /*progress*/, bool* /*cancel*/) :
	sceneFilename_(filename)
{
	xmlOpenResult_ = xmlDocument_.LoadFile(filename.c_str());
}

SDXSceneRef XMLParser::parse(const Library& library, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (engine.isNull() || xmlOpenResult_ != tinyxml2::XML_SUCCESS)
	{
#if TINYXML2_MAJOR_VERSION >= 6 || TIXML2_MAJOR_VERSION >= 6
		const char* const errorName = xmlDocument_.ErrorName();
		const char* const errorDescription = xmlDocument_.ErrorStr();
#else
		const char* const errorName = xmlDocument_.GetErrorStr1();
		const char* const errorDescription = xmlDocument_.GetErrorStr2();
#endif

		const std::string errorNameStr(errorName ? errorName : "");
		const std::string errorDescriptionStr(errorDescription ? errorDescription : "");

		if (!errorNameStr.empty())
		{
			Log::error() << "Failed to open the X3D file \"" << sceneFilename_ << "\": " << errorNameStr;

			if (!errorDescriptionStr.empty())
			{
				Log::error() << "Detailed problem: " << errorDescriptionStr;
			}
		}
		else
		{
			ocean_assert(errorDescriptionStr.empty());
		}

		return SDXSceneRef();
	}

	if (timestamp.isInvalid())
	{
		sceneTimestamp_.toNow();
	}
	else
	{
		sceneTimestamp_ = timestamp;
	}

	const IO::File file(sceneFilename_);
	const std::string lowerFileExtension = String::toLower(file.extension());

	scene_ = new X3DScene(file(), library, engine);
	if (scene_ == nullptr)
	{
		throw OutOfMemoryException();
	}

	SceneRef scene(library.nodeManager().registerNode(scene_));

	SDXSceneRef sdxScene(scene);
	MultiNode::Values values;

	const tinyxml2::XMLElement* xmlNodeX3D = xmlDocument_.FirstChildElement("X3D");
	if (xmlNodeX3D == nullptr)
	{
		Log::error() << "The X3D file \"" << sceneFilename_ << "\" does not contain an X3D node.";
		return SceneRef();
	}

	const tinyxml2::XMLElement* xmlNodeScene = xmlNodeX3D->FirstChildElement("Scene");
	if (xmlNodeScene == nullptr)
	{
		Log::error() << "The X3D file \"" << sceneFilename_ << "\" does not contain an Scene node.";
		return SceneRef();
	}

	const tinyxml2::XMLElement* xmlNode = xmlNodeScene->FirstChildElement();

	try
	{
		/*const ClassicScanner::Token& header = scanner.lineToken();

		if (lowerFileExtension == "wrl")
		{
			if (header.line() != "#VRML V2.0 utf8")
				throw OceanException("Invalid file header: VRML files with header version \"#VRML V2.0 utf8\" are supported only!");
		}
		else if (lowerFileExtension == "x3dv")
		{
			if (header.line() != "#X3D V3.0 utf8")
				throw OceanException("Invalid file header: X3D files with header version \"#X3D V3.0 utf8\" are supported only!");
		}
		else if (lowerFileExtension == "ox3dv")
		{
			if (header.line() != "#OX3D V3.0 utf8")
				throw OceanException("Invalid file header: Ocean-X3D files with header version \"#OX3D V3.0 utf8\" are supported only!");
		}

		scanner.pop();*/

		while (xmlNode)
		{
			/*if (scanner.token().isKeyword(ClassicScanner::KEYWORD_META))
			{
				scanner.pop();
				parseMeta();
			}
			else if (scanner.token().isKeyword(ClassicScanner::KEYWORD_PROFILE))
			{
				scanner.pop();
				parseProfile();
			}
			else*/
			{
				NodeRef node = parseNode(xmlNode);

				if (node)
				{
					values.emplace_back(std::move(node));
				}

				xmlNode = xmlNode->NextSiblingElement();
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << "Error in \"" << sceneFilename_ << "\": " << exception.what();
	}

	sdxScene->field<MultiNode>("children").setValues(values, sceneTimestamp_);
	sdxScene->initialize(sceneTimestamp_);

	return sdxScene;
}

NodeRef XMLParser::parseNode(const tinyxml2::XMLElement* xmlNode)
{
	ocean_assert(xmlNode);

	if (!xmlNode)
	{
		return NodeRef();
	}

	const std::string nodeType(xmlNode->Name());

	if (nodeType == "ROUTE")
	{
		parseRoute(xmlNode);
		return NodeRef();
	}

	SDXNodeRef node(Factory::createNode(nodeType, scene_->environment()));
	if (node.isNull())
	{
		Log::warning() << "Skipped unknown x3d node \"" << nodeType << "\".";
		return NodeRef(node);
	}

	std::string name_;

	// Attributes
	try
	{
		const tinyxml2::XMLAttribute* xmlAttribute = xmlNode->FirstAttribute();

		while (xmlAttribute)
		{
			const std::string attributeName(xmlAttribute->Name());

			if (attributeName == "DEF" && xmlAttribute->Value())
			{
				name_ = xmlAttribute->Value();
			}
			else if (attributeName == "USE" && xmlAttribute->Value())
			{
				const std::string useName(xmlAttribute->Value());
				const NodeRefs nodes(scene_->environment()->library()->nodes(useName));

				for (NodeRefs::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
				{
					SDXNodeRef localNode(*i);
					ocean_assert(localNode);

					if (localNode->sceneId() == scene_->sceneId())
					{
						return NodeRef(localNode);
					}
				}

				Log::error() << "Failed USE the specified node \"" << useName << "\" as it has not been defined.";
				return NodeRef();
			}
			else
			{
				if (node->hasField(attributeName) && xmlAttribute->Value())
				{
					Field& field = node->field(attributeName);
					parseValueField(field, xmlAttribute->Value());
				}
				else
				{
					Log::warning() << "Unknown node field \"" << attributeName << "\".";
				}
			}

			xmlAttribute = xmlAttribute->Next();
		}

		if (name_.empty() == false)
		{
			node->setName(name_);
		}
	}
	catch(const Exception& exception)
	{
		if (name_.empty())
		{
			Log::error() << "Error in " << nodeType << " node: " << exception.what();
		}
		else
		{
			Log::error() << "Error in " << nodeType << " node \"" << name_ << "\": " << exception.what();
		}

		return NodeRef(node);
	}

	// Children
	try
	{
		const tinyxml2::XMLElement* xmlChild = xmlNode->FirstChildElement();

		while (xmlChild)
		{
			const std::string childName(xmlChild->Name());

			if (childName == "field" && node->isDynamic())
			{
				try
				{
					addDynamicField(node, xmlChild);
				}
				catch(const Exception& exception)
				{
					if (name_.empty())
					{
						Log::error() << "Error in " << nodeType << " node: " << exception.what();
					}
					else
					{
						Log::error() << "Error in " << nodeType << " node \"" << name_ << "\": " << exception.what();
					}
				}
			}
			else
			{
				try
				{
					parseNodeField(node, xmlChild);
				}
				catch(const Exception& exception)
				{
					Log::warning() << exception.what();
				}
			}

			xmlChild = xmlChild->NextSiblingElement();
		}
	}
	catch(const Exception& exception)
	{
		if (name_.empty())
		{
			Log::error() << "Error in " << nodeType << " node: " << exception.what();
		}
		else
		{
			Log::error() << "Error in " << nodeType << " node \"" << name_ << "\": " << exception.what();
		}

		return NodeRef(node);
	}

	return NodeRef(node);
}

void XMLParser::parseNodeField(NodeRef& parent, const tinyxml2::XMLElement* xmlNode)
{
	ocean_assert(parent && xmlNode);

	const std::string nodeType(xmlNode->Name());
	const std::string fieldName(fieldNameLookup(parent, nodeType));

	ocean_assert(parent->hasField(fieldName));
	Field& field = parent->field(fieldName);
	ocean_assert(field.type() == Field::TYPE_NODE);

	if (field.is0D())
	{
		const NodeRef node = parseNode(xmlNode);
		ocean_assert(node);

		if (node)
		{
			static_cast<SingleNode&>(field).setValue(node, sceneTimestamp_);
		}
	}
	else
	{
		const NodeRef node = parseNode(xmlNode);
		ocean_assert(node);

		if (node)
		{
			static_cast<MultiNode&>(field).values().push_back(node);
			field.setTimestamp(sceneTimestamp_);
		}
	}
}

void XMLParser::parseRoute(const tinyxml2::XMLElement* xmlNode)
{
	ocean_assert(xmlNode);

	const char* fromNode = xmlNode->Attribute("fromNode");
	const char* fromField = xmlNode->Attribute("fromField");
	const char* toNode = xmlNode->Attribute("toNode");
	const char* toField = xmlNode->Attribute("toField");

	if (!fromNode || !fromField || !toNode || !toField)
	{
		Log::warning() << "Failed to parse a ROUTE statement: At least one of the four necessary node or field information is missing.";
		return;
	}

	std::string startNodeName(fromNode);
	std::string startFieldName(fromField);
	std::string targetNodeName(toNode);
	std::string targetFieldName(toField);

	try
	{
		const X3DNodeRef startNode = scene_->environment()->library()->nodeManager().node(startNodeName, scene_->sceneId());
		if (startNode.isNull())
		{
			throw OceanException(std::string("The start node \"") + startNodeName + std::string("\" of ROUTE ")
				+ startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		startFieldName = startNode->originalFieldName(startFieldName);

		if (startNode->hasField(startFieldName) == false)
		{
			throw OceanException(std::string("The start field \"") + startFieldName + std::string("\" of ROUTE ")
				+ startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		const NodeRef targetNode = scene_->environment()->library()->nodeManager().node(targetNodeName, scene_->sceneId());
		if (targetNode.isNull())
		{
			throw OceanException(std::string("The target node \"") + targetNodeName + std::string("\" of ROUTE ")
				+ startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		targetFieldName = targetNode->originalFieldName(targetFieldName);

		if (targetNode->hasField(targetFieldName) == false)
		{
			throw OceanException(std::string("The target field \"") + targetFieldName + std::string("\" of ROUTE ")
				+ startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		const Field& startField = startNode->field(startFieldName);
		const Field& targetField = targetNode->field(targetFieldName);

		if (startField.type() != targetField.type() || startField.dimension() != targetField.dimension())
		{
			throw OceanException(std::string("The fields of ROUTE ")
				+ startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" have different types."));
		}

		if ((startNode->fieldAccessType(startFieldName) & Node::ACCESS_GET) == 0)
		{
			throw OceanException(std::string("The start field \"") + startFieldName + std::string("\" of ROUTE ")
				+ startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" must be an output field."));
		}

		 if ((targetNode->fieldAccessType(targetFieldName) & Node::ACCESS_SET) == 0)
		 {
			 throw OceanException(std::string("The target field \"") + targetFieldName + std::string("\" of ROUTE ")
				+ startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" must be an input field."));
		 }

		startNode->addConnection(startFieldName, targetNode->id(), targetFieldName);
	}
	catch(const Exception& exception)
	{
		Log::warning() << "Failed to establish a ROUTE: " << exception.what();
	}
}

void XMLParser::addDynamicField(NodeRef& parent, const tinyxml2::XMLElement* xmlNode)
{
	ocean_assert(parent && xmlNode && parent->isDynamic());
	ocean_assert(xmlNode->Name() && std::string(xmlNode->Name()) == "field");

	const char* fieldName = xmlNode->Attribute("name");
	const char* fieldType = xmlNode->Attribute("type");
	const char* fieldValue = xmlNode->Attribute("value");

	if (fieldName == nullptr || fieldType == nullptr)
	{
		throw OceanException("Invalid user-defined field: need valid field name and field type.");
	}

	const std::string type(fieldType);
	const std::string name(fieldName);

	const DynamicNodeRef dynamicNode(parent);

	if (type == "SFBool")
		dynamicNode->addField(name, SingleBool());
	else if (type == "MFBool")
		dynamicNode->addField(name, MultiBool());
	else if (type == "SFColor")
		dynamicNode->addField(name, SingleColor());
	else if (type == "MFColor")
		dynamicNode->addField(name, MultiColor());
	else if (type == "SFFloat")
		dynamicNode->addField(name, SingleFloat());
	else if (type == "MFFloat")
		dynamicNode->addField(name, MultiFloat());
	else if (type == "SFInt32")
		dynamicNode->addField(name, SingleInt());
	else if (type == "MFInt32")
		dynamicNode->addField(name, MultiInt());
	else if (type == "SFMatrix3f")
		dynamicNode->addField(name, SingleMatrix3());
	else if (type == "MFMatrix3f")
		dynamicNode->addField(name, MultiMatrix3());
	else if (type == "SFMatrix4f")
		dynamicNode->addField(name, SingleMatrix4());
	else if (type == "MFMatrix4f")
		dynamicNode->addField(name, MultiMatrix4());
	else if (type == "SFRotation")
		dynamicNode->addField(name, SingleRotation());
	else if (type == "MFRotation")
		dynamicNode->addField(name, MultiRotation());
	else if (type == "SFString")
		dynamicNode->addField(name, SingleString());
	else if (type == "MFString")
		dynamicNode->addField(name, MultiString());
	else if (type == "SFTime")
		dynamicNode->addField(name, SingleTime());
	else if (type == "MFTime")
		dynamicNode->addField(name, MultiTime());
	else if (type == "SFVec2f")
		dynamicNode->addField(name, SingleVector2());
	else if (type == "MFVec2f")
		dynamicNode->addField(name, MultiVector2());
	else if (type == "SFVec3f")
		dynamicNode->addField(name, SingleVector3());
	else if (type == "MFVec3f")
		dynamicNode->addField(name, MultiVector3());
	else if (type == "SFVec4f")
		dynamicNode->addField(name, SingleVector4());
	else if (type == "MFVec4f")
		dynamicNode->addField(name, MultiVector4());
	else
		throw OceanException(std::string("Tried to parse dynamic field \"") + name + std::string("\" and got an unknown field type \"") + type + std::string("\""));

	if (fieldValue)
	{
		Field& dynamicField = dynamicNode->dynamicField(name);
		parseValueField(dynamicField, std::string(fieldValue));
	}
}

bool XMLParser::parseValueField(Field& field, const std::string& value)
{
	if (field.is0D())
	{
		switch (field.type())
		{
			case Field::TYPE_BOOLEAN:
				return parseValueField(static_cast<SingleBool&>(field), value);

			case Field::TYPE_COLOR:
				return parseValueField(static_cast<SingleColor&>(field), value);

			case Field::TYPE_FLOAT:
				return parseValueField(static_cast<SingleFloat&>(field), value);

			case Field::TYPE_INT:
				return parseValueField(static_cast<SingleInt&>(field), value);

			case Field::TYPE_MATRIX3:
				return parseValueField(static_cast<SingleMatrix3&>(field), value);

			case Field::TYPE_MATRIX4:
				return parseValueField(static_cast<SingleMatrix4&>(field), value);

			case Field::TYPE_ROTATION:
				return parseValueField(static_cast<SingleRotation&>(field), value);

			case Field::TYPE_STRING:
				return parseValueField(static_cast<SingleString&>(field), value);

			case Field::TYPE_TIME:
				return parseValueField(static_cast<SingleTime&>(field), value);

			case Field::TYPE_VECTOR2:
				return parseValueField(static_cast<SingleVector2&>(field), value);

			case Field::TYPE_VECTOR3:
				return parseValueField(static_cast<SingleVector3&>(field), value);

			case Field::TYPE_VECTOR4:
				return parseValueField(static_cast<SingleVector4&>(field), value);

			default:
				ocean_assert(false && "Missing implementation");
				throw MissingImplementationException();
		}
	}
	else
	{
		ocean_assert(field.is1D());

		switch (field.type())
		{
			case Field::TYPE_BOOLEAN:
				return parseValueField(static_cast<MultiBool&>(field), value);

			case Field::TYPE_COLOR:
				return parseValueField(static_cast<MultiColor&>(field), value);

			case Field::TYPE_FLOAT:
				return parseValueField(static_cast<MultiFloat&>(field), value);

			case Field::TYPE_INT:
				return parseValueField(static_cast<MultiInt&>(field), value);

			case Field::TYPE_MATRIX3:
				return parseValueField(static_cast<MultiMatrix3&>(field), value);

			case Field::TYPE_MATRIX4:
				return parseValueField(static_cast<MultiMatrix4&>(field), value);

			case Field::TYPE_ROTATION:
				return parseValueField(static_cast<MultiRotation&>(field), value);

			case Field::TYPE_STRING:
				return parseValueField(static_cast<MultiString&>(field), value);

			case Field::TYPE_TIME:
				return parseValueField(static_cast<MultiTime&>(field), value);

			case Field::TYPE_VECTOR2:
				return parseValueField(static_cast<MultiVector2&>(field), value);

			case Field::TYPE_VECTOR3:
				return parseValueField(static_cast<MultiVector3&>(field), value);

			case Field::TYPE_VECTOR4:
				return parseValueField(static_cast<MultiVector3&>(field), value);

			default:
				ocean_assert(false && "Missing implementation");
				throw MissingImplementationException();
		}
	}
}

bool XMLParser::parseValueField(SingleBool& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		field.setValue(parseBool(scanner), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFBool field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleColor& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);

		const float red = max(0.0f, min(float(parseFloat(scanner)), 1.0f));
		const float green = max(0.0f, min(float(parseFloat(scanner)), 1.0f));
		const float blue = max(0.0f, min(float(parseFloat(scanner)), 1.0f));

		field.setValue(RGBAColor(red, green, blue), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFColor field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleFloat& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		field.setValue(parseFloat(scanner), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFFloat field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleInt& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		field.setValue(parseInt(scanner), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFInt32 field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleMatrix3& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		Scalar matrixValues[9];

		for (unsigned int n = 0; n < 9; n++)
			matrixValues[n] = parseFloat(scanner);

		SquareMatrix3 transposedMatrix(matrixValues);
		field.setValue(transposedMatrix.transposed(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a SFMatrix3f field: ") + std::string(exception.what()));
	}

	return true;
}

bool XMLParser::parseValueField(SingleMatrix4& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		Scalar matrixValues[16];

		for (unsigned int n = 0; n < 16; n++)
			matrixValues[n] = parseFloat(scanner);

		SquareMatrix4 transposedMatrix(matrixValues);
		field.setValue(transposedMatrix.transposed(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a SFMatrix4f field: ") + std::string(exception.what()));
	}

	return true;
}

bool XMLParser::parseValueField(SingleRotation& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		field.setValue(parseRotation(scanner), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFRotation field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleString& field, const std::string& value)
{
	try
	{
		const std::string timString = String::trim(value);
		if (!timString.empty() && timString[0] != '\"')
			field.setValue(value, sceneTimestamp_);
		else
		{
			XMLScanner scanner(value);
			field.setValue(parseString(scanner), sceneTimestamp_);
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFString field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleTime& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		field.setValue(Timestamp(parseFloat(scanner)), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFTime field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleVector2& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);

		const float x = float(parseFloat(scanner));
		const float y = float(parseFloat(scanner));

		field.setValue(Vector2(x, y), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFVec2f field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleVector3& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);

		const Scalar x = float(parseFloat(scanner));
		const Scalar y = float(parseFloat(scanner));
		const Scalar z = float(parseFloat(scanner));

		field.setValue(Vector3(x, y, z), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFVec3f field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(SingleVector4& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);

		const Scalar x = float(parseFloat(scanner));
		const Scalar y = float(parseFloat(scanner));
		const Scalar z = float(parseFloat(scanner));
		const Scalar w = float(parseFloat(scanner));

		field.setValue(Vector4(x, y, z, w), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFVec4f field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiBool& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiBool::Values values;

		while (!scanner.token().isEndOfFile())
			values.push_back(parseBool(scanner));

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFBool field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiColor& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiColor::Values values;

		while (!scanner.token().isEndOfFile())
		{
			const float red = max(0.0f, min(float(parseFloat(scanner)), 1.0f));
			const float green = max(0.0f, min(float(parseFloat(scanner)), 1.0f));
			const float blue = max(0.0f, min(float(parseFloat(scanner)), 1.0f));

			values.push_back(RGBAColor(red, green, blue));
		}

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFColor field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiFloat& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiFloat::Values values;

		while (!scanner.token().isEndOfFile())
			values.push_back(parseFloat(scanner));

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFFloat field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiInt& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiInt::Values values;

		while (!scanner.token().isEndOfFile())
			values.push_back(parseInt(scanner));

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFInt32 field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiMatrix3& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiMatrix3::Values values;
		Scalar matrixValues[9];

		while (!scanner.token().isEndOfFile())
		{
			for (unsigned int n = 0; n < 9; n++)
				matrixValues[n] = parseFloat(scanner);

			SquareMatrix3 transposedMatrix(matrixValues);
			values.push_back(transposedMatrix.transposed());
		}

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a MFMatrix3f field: ") + std::string(exception.what()));
	}

	return true;
}

bool XMLParser::parseValueField(MultiMatrix4& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiMatrix4::Values values;
		Scalar matrixValues[16];

		while (!scanner.token().isEndOfFile())
		{
			for (unsigned int n = 0; n < 16; n++)
				matrixValues[n] = parseFloat(scanner);

			SquareMatrix4 transposedMatrix(matrixValues);
			values.push_back(transposedMatrix.transposed());
		}

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a MFMatrix4f field: ") + std::string(exception.what()));
	}

	return true;
}

bool XMLParser::parseValueField(MultiRotation& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiRotation::Values values;

		while (!scanner.token().isEndOfFile())
			values.push_back(parseRotation(scanner));

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFRotation field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiString& field, const std::string& value)
{
	try
	{
		MultiString::Values values;

		const std::string timString = String::trim(value);
		if (!timString.empty() && timString[0] != '\"')
			values.push_back(value);
		else
		{
			XMLScanner scanner(value);

			while (!scanner.token().isEndOfFile())
			{
				values.push_back(parseString(scanner));
			}
		}

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFString field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiTime& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiTime::Values values;

		while (!scanner.token().isEndOfFile())
			values.push_back(Timestamp(parseFloat(scanner)));

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFTime field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiVector2& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiVector2::Values values;

		while (!scanner.token().isEndOfFile())
		{
			const Scalar x = parseFloat(scanner);
			const Scalar y = parseFloat(scanner);

			values.push_back(Vector2(x, y));
		}

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFVec2f field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiVector3& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiVector3::Values values;

		while (!scanner.token().isEndOfFile())
		{
			const Scalar x = parseFloat(scanner);
			const Scalar y = parseFloat(scanner);
			const Scalar z = parseFloat(scanner);

			values.push_back(Vector3(x, y, z));
		}

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFVec3f field: ") + message);
	}

	return true;
}

bool XMLParser::parseValueField(MultiVector4& field, const std::string& value)
{
	try
	{
		XMLScanner scanner(value);
		MultiVector4::Values values;

		while (!scanner.token().isEndOfFile())
		{
			const Scalar x = parseFloat(scanner);
			const Scalar y = parseFloat(scanner);
			const Scalar z = parseFloat(scanner);
			const Scalar w = parseFloat(scanner);

			values.push_back(Vector4(x, y, z, w));
		}

		field.setValues(values, sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFVec4f field: ") + message);
	}

	return true;
}

bool XMLParser::parseBool(IO::Scanner& scanner)
{
	const XMLScanner::Token token(scanner.tokenPop());

	if (token.isIdentifier())
	{
		if (token.identifier() == "TRUE")
			return true;

		if (token.identifier() == "FALSE")
			return false;
	}

	throw OceanException(std::string("Could not parse a boolean, got \"") + token.raw() + std::string("\" instead"));
}

Scalar XMLParser::parseFloat(IO::Scanner& scanner)
{
	const XMLScanner::Token token(scanner.tokenPop());

	if (token.isNumber())
		return token.number();
	if (token.isInteger())
		return Scalar(token.integer());

	throw OceanException(std::string("The token is not a float, got \"") + token.raw() + std::string("\" instead"));
}

int XMLParser::parseInt(IO::Scanner& scanner)
{
	const XMLScanner::Token token(scanner.tokenPop());

	if (token.isInteger())
		return token.integer();

	throw OceanException(std::string("The token is not an integer, got \"") + token.raw() + std::string("\" instead"));
}

std::string XMLParser::parseString(IO::Scanner& scanner)
{
	const XMLScanner::Token token(scanner.tokenPop());

	if (token.isString())
		return token.string();
	if (token.isIdentifier())
		return token.identifier();

	throw OceanException(std::string("The token is not a string, got \"") + token.raw() + std::string("\" instead"));
}

Rotation XMLParser::parseRotation(IO::Scanner& scanner)
{
	const Scalar x = parseFloat(scanner);
	const Scalar y = parseFloat(scanner);
	const Scalar z = parseFloat(scanner);
	const Scalar a = parseFloat(scanner);

	Vector3 axis(x, y, z);
	if (axis.normalize())
		return Rotation(axis, a);

	Log::warning() << "Invalid rotation axis in \"" << sceneFilename_ << ", using default axis instead.";
	return Rotation();
}

std::string XMLParser::fieldNameLookup(const NodeRef& parent, const std::string& nodeType)
{
	static std::map<std::string, std::string> lookupMap;
	if (lookupMap.empty())
	{
		lookupMap["Appearance"] = "appearance";
		lookupMap["Box"] = "geometry";
		lookupMap["Color"] = "color";
		lookupMap["Cone"] = "geometry";
		lookupMap["Coordinate"] = "coord";
		lookupMap["Cylinder"] = "geometry";
		lookupMap["ImageTexture"] = "texture";
		lookupMap["IndexedFaceSet"] = "geometry";
		lookupMap["LiveVideoTexture"] = "texture";
		lookupMap["Material"] = "material";
		lookupMap["MovieTexture"] = "texture";
		lookupMap["MultiTexture"] = "texture";
		lookupMap["MultiTextureTransform"] = "textureTransform";
		lookupMap["Normal"] = "normal";
		lookupMap["PackagedShader"] = "shaders";
		lookupMap["PhantomTextureCoordinate"] = "texCoord";
		lookupMap["PointSet"] = "geometry";
		lookupMap["Sphere"] = "geometry";
		lookupMap["StreamTexture"] = "texture";
		lookupMap["TextureProperties"] = "textureProperties";
		lookupMap["TextureTransform"] = "textureTransform";
	}

	const std::map<std::string, std::string>::const_iterator i = lookupMap.find(nodeType);
	if (i == lookupMap.end())
	{
		if (!parent->hasField("children"))
			throw OceanException(std::string("Failed to add the node type \"") + nodeType + std::string("\" to the parent node \"") + parent->type() + std::string("\""));

		return std::string("children");
	}

	const std::string& fieldName = i->second;
	ocean_assert(!fieldName.empty());

	if (!parent->hasField(fieldName))
		throw OceanException(std::string("Failed to add the node type \"") + nodeType + std::string("\" to the parent node type \"") + parent->type() + std::string("\""));

	return fieldName;
}

}

}

}

}
