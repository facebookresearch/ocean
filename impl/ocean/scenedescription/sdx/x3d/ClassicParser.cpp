/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ClassicParser.h"
#include "ocean/scenedescription/sdx/x3d/Factory.h"
#include "ocean/scenedescription/sdx/x3d/X3DScene.h"

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

ClassicParser::ClassicParser(const std::string& filename, float* progress, bool* cancel) :
	scanner_(filename, progress, cancel),
	scenePtr_(nullptr)
{
	// nothing to do here
}

SDXSceneRef ClassicParser::parse(const Library& library, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (engine.isNull() || scanner_.isValid() == false)
	{
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

	const IO::File file(scanner_.filename());
	const std::string lowerFileExtension = String::toLower(file.extension());

	scenePtr_ = new X3DScene(file(), library, engine);
	if (scenePtr_ == nullptr)
	{
		throw OutOfMemoryException();
	}

	SceneRef scene(library.nodeManager().registerNode(scenePtr_));

	SDXSceneRef sdxScene(scene);
	MultiNode::Values values;

	try
	{
		const ClassicScanner::Token& header = scanner_.lineToken();

		if (lowerFileExtension == "wrl")
		{
			if (header.line() != "#VRML V2.0 utf8")
			{
				throw OceanException("Invalid file header: VRML files with header version \"#VRML V2.0 utf8\" are supported only!");
			}
		}
		else if (lowerFileExtension == "x3dv")
		{
			if (header.line() != "#X3D V3.0 utf8")
			{
				throw OceanException("Invalid file header: X3D files with header version \"#X3D V3.0 utf8\" are supported only!");
			}
		}
		else if (lowerFileExtension == "ox3dv")
		{
			if (header.line() != "#OX3D V3.0 utf8")
			{
				throw OceanException("Invalid file header: Ocean-X3D files with header version \"#OX3D V3.0 utf8\" are supported only!");
			}
		}

		scanner_.pop();

		while (scanner_.token().isEndOfFile() == false)
		{
			if (scanner_.token().isKeyword(ClassicScanner::KEYWORD_ROUTE))
			{
				scanner_.pop();
				parseRoute();
			}
			else if (scanner_.token().isKeyword(ClassicScanner::KEYWORD_META))
			{
				scanner_.pop();
				parseMeta();
			}
			else if (scanner_.token().isKeyword(ClassicScanner::KEYWORD_PROFILE))
			{
				scanner_.pop();
				parseProfile();
			}
			else
			{
				NodeRef node = parseNode();

				if (node)
				{
					values.emplace_back(std::move(node));
				}
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << "Error in \"" << scanner_.filename() << "\", line " << scanner_.line() << ", column " << scanner_.column() << ": " << exception.what();
	}

	sdxScene->field<MultiNode>("children").setValues(values, sceneTimestamp_);
	sdxScene->initialize(sceneTimestamp_);

	return sdxScene;
}

NodeRef ClassicParser::parseNode()
{
	Token token = scanner_.token();

	std::string name_;

	if (token.isKeyword())
	{
		if (token.isKeyword(ClassicScanner::KEYWORD_NULL))
		{
			scanner_.pop();
			return NodeRef();
		}

		if (token.isKeyword(ClassicScanner::KEYWORD_DEF) == false)
		{
			throw OceanException(std::string("Unexpected keyword \"") + token.raw() + std::string("\"."));
		}

		scanner_.pop();
		token = scanner_.tokenPop();

		if (token.isIdentifier() == false)
		{
			throw OceanException(std::string("Expected a node name after 'DEF', got \"") + token.raw() + std::string("\" instead."));
		}

		name_ = token.identifier();
		token = scanner_.tokenPop();
	}

	if (token.isIdentifier() == false)
	{
		throw OceanException(std::string("Expected a node type, got \"") + token.raw() + std::string("\" instead."));
	}

	std::string nodeType = token.identifier();
	scanner_.pop();

	SDXNodeRef node(Factory::createNode(nodeType, scenePtr_->environment()));

	if (node.isNull())
	{
		if (skipNode(false))
		{
			Log::warning() << "Skipped unknown x3d node \"" << nodeType << "\" in line " << scanner_.line() << ".";
			return NodeRef(node);
		}

		throw OceanException(std::string("Could not skip the unknown x3d node type \"") + nodeType + std::string("\"."));
	}

	if (name_.empty() == false)
	{
		node->setName(name_);
	}

	token = scanner_.tokenPop();
	if (token.isSymbol(ClassicScanner::SYMBOL_NODE_BEGIN) == false)
	{
		throw OceanException(std::string("Expected a node begin symbol '{', got \"") + token.raw() + std::string("\" instead."));
	}

	try
	{
		while (true)
		{
			token = scanner_.tokenPop();

			if (!token)
			{
				Log::error() << "Error in \"" << scanner_.filename() << "\", line " << scanner_.line() << ", column " << scanner_.column() << ": Invalid token \"" << token.raw() << "\", trying to parse next objects...";
				continue;
			}

			if (token.isSymbol(ClassicScanner::SYMBOL_NODE_END))
			{
				break;
			}

			if (token.isIdentifier() == false)
			{
				if (token.isKeyword())
				{
					if (token.isKeyword(ClassicScanner::KEYWORD_ROUTE))
					{
						parseRoute();
						continue;
					}
					else if (node->isDynamic())
					{
						addDynamicField(token, node);
					}
				}
				else
				{
					throw OceanException(std::string("Expected a field name, got \"") + token.raw() + std::string("\" instead."));
				}

				token = scanner_.tokenPop();
			}

			if (token.isIdentifier())
			{
				if (node->hasField(token.identifier()))
				{
					Field& field = node->field(token.identifier());
					parseField(field);

					continue;
				}
				else if (node->isDynamic())
				{
					DynamicNodeRef dynamicNode(node);
					ocean_assert(dynamicNode);

					if (dynamicNode->hasDynamicField(token.identifier()))
					{
						Field& field = dynamicNode->dynamicField(token.identifier());
						parseField(field);

						continue;
					}
				}
			}

			throw OceanException(std::string("The node \"") + nodeType + std::string("\" does not have a field \"") + token.raw() + std::string("\"."));
		}
	}
	catch(const Exception& exception)
	{
		Log::error() << "Error in \"" << scanner_.filename() << "\", line " << scanner_.line() << ", column " << scanner_.column() << ": " << exception.what();

		if (skipNode(true))
		{
			Log::warning() << "Skipped x3d node \"" << nodeType << "\", line " << scanner_.line() << ", column " << scanner_.column() << ".";
			return NodeRef(node);
		}

		throw OceanException(std::string("Could not skip the unknown x3d node type \"") + nodeType + std::string("\"."));
	}

	return NodeRef(node);
}

void ClassicParser::addDynamicField(const Token& token, const NodeRef& node)
{
	ocean_assert(token.isKeyword());

	SmartObjectRef<DynamicNode, Node> dynamicNode(node);
	ocean_assert(dynamicNode);

	const Token nameToken(scanner_.token());

	if (nameToken.isIdentifier() == false)
	{
		throw OceanException(std::string("Tried to parse a dynamic field node \"") + token.raw() + std::string("\" and got an invalid field name \"") + nameToken.raw() + std::string("\"."));
	}

	switch (token.keyword())
	{
		case ClassicScanner::KEYWORD_MF_BOOL:
			dynamicNode->addField(nameToken.identifier(), MultiBool());
			break;

		case ClassicScanner::KEYWORD_MF_COLOR:
			dynamicNode->addField(nameToken.identifier(), MultiColor());
			break;

		case ClassicScanner::KEYWORD_MF_FLOAT:
			dynamicNode->addField(nameToken.identifier(), MultiFloat());
			break;

		case ClassicScanner::KEYWORD_MF_INT32:
			dynamicNode->addField(nameToken.identifier(), MultiInt());
			break;

		case ClassicScanner::KEYWORD_MF_MATRIX_3F:
			dynamicNode->addField(nameToken.identifier(), MultiMatrix3());
			break;

		case ClassicScanner::KEYWORD_MF_MATRIX_4F:
			dynamicNode->addField(nameToken.identifier(), MultiMatrix4());
			break;

		case ClassicScanner::KEYWORD_MF_NODE:
			dynamicNode->addField(nameToken.identifier(), MultiNode());
			break;

		case ClassicScanner::KEYWORD_MF_ROTATION:
			dynamicNode->addField(nameToken.identifier(), MultiRotation());
			break;

		case ClassicScanner::KEYWORD_MF_STRING:
			dynamicNode->addField(nameToken.identifier(), MultiString());
			break;

		case ClassicScanner::KEYWORD_MF_TIME:
			dynamicNode->addField(nameToken.identifier(), MultiTime());
			break;

		case ClassicScanner::KEYWORD_MF_VEC_2F:
			dynamicNode->addField(nameToken.identifier(), MultiVector2());
			break;

		case ClassicScanner::KEYWORD_MF_VEC_3F:
			dynamicNode->addField(nameToken.identifier(), MultiVector3());
			break;

		case ClassicScanner::KEYWORD_MF_VEC_4F:
			dynamicNode->addField(nameToken.identifier(), MultiVector4());
			break;

		case ClassicScanner::KEYWORD_SF_BOOL:
			dynamicNode->addField(nameToken.identifier(), SingleBool());
			break;

		case ClassicScanner::KEYWORD_SF_COLOR:
			dynamicNode->addField(nameToken.identifier(), SingleColor());
			break;

		case ClassicScanner::KEYWORD_SF_FLOAT:
			dynamicNode->addField(nameToken.identifier(), SingleFloat());
			break;

		case ClassicScanner::KEYWORD_SF_INT32:
			dynamicNode->addField(nameToken.identifier(), SingleInt());
			break;

		case ClassicScanner::KEYWORD_SF_MATRIX_3F:
			dynamicNode->addField(nameToken.identifier(), SingleMatrix3());
			break;

		case ClassicScanner::KEYWORD_SF_MATRIX_4F:
			dynamicNode->addField(nameToken.identifier(), SingleMatrix4());
			break;

		case ClassicScanner::KEYWORD_SF_NODE:
			dynamicNode->addField(nameToken.identifier(), SingleNode());
			break;

		case ClassicScanner::KEYWORD_SF_ROTATION:
			dynamicNode->addField(nameToken.identifier(), SingleRotation());
			break;

		case ClassicScanner::KEYWORD_SF_STRING:
			dynamicNode->addField(nameToken.identifier(), SingleString());
			break;

		case ClassicScanner::KEYWORD_SF_TIME:
			dynamicNode->addField(nameToken.identifier(), SingleTime());
			break;

		case ClassicScanner::KEYWORD_SF_VEC_2F:
			dynamicNode->addField(nameToken.identifier(), SingleVector2(Vector2(0, 0)));
			break;

		case ClassicScanner::KEYWORD_SF_VEC_3F:
			dynamicNode->addField(nameToken.identifier(), SingleVector3());
			break;

		case ClassicScanner::KEYWORD_SF_VEC_4F:
			dynamicNode->addField(nameToken.identifier(), SingleVector4());
			break;

		default:
			throw OceanException(std::string("Tried to parse dynamic field \"") + nameToken.identifier() + std::string("\" and got an unknown field type \"") + token.raw() + std::string("\""));
	}
}

void ClassicParser::parseField(Field& field)
{
	if (field.is0D())
	{
		switch (field.type())
		{
			case Field::TYPE_BOOLEAN:
				parseField(static_cast<SingleBool&>(field));
				break;

			case Field::TYPE_COLOR:
				parseField(static_cast<SingleColor&>(field));
				break;

			case Field::TYPE_FLOAT:
				parseField(static_cast<SingleFloat&>(field));
				break;

			case Field::TYPE_INT:
				parseField(static_cast<SingleInt&>(field));
				break;

			case Field::TYPE_MATRIX3:
				parseField(static_cast<SingleMatrix3&>(field));
				break;

			case Field::TYPE_MATRIX4:
				parseField(static_cast<SingleMatrix4&>(field));
				break;

			case Field::TYPE_NODE:
				parseField(static_cast<SingleNode&>(field));
				break;

			case Field::TYPE_ROTATION:
				parseField(static_cast<SingleRotation&>(field));
				break;

			case Field::TYPE_STRING:
				parseField(static_cast<SingleString&>(field));
				break;

			case Field::TYPE_TIME:
				parseField(static_cast<SingleTime&>(field));
				break;

			case Field::TYPE_VECTOR2:
				parseField(static_cast<SingleVector2&>(field));
				break;

			case Field::TYPE_VECTOR3:
				parseField(static_cast<SingleVector3&>(field));
				break;

			case Field::TYPE_VECTOR4:
				parseField(static_cast<SingleVector4&>(field));
				break;

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
				parseField(static_cast<MultiBool&>(field));
				break;

			case Field::TYPE_COLOR:
				parseField(static_cast<MultiColor&>(field));
				break;

			case Field::TYPE_FLOAT:
				parseField(static_cast<MultiFloat&>(field));
				break;

			case Field::TYPE_INT:
				parseField(static_cast<MultiInt&>(field));
				break;

			case Field::TYPE_MATRIX3:
				parseField(static_cast<MultiMatrix3&>(field));
				break;

			case Field::TYPE_MATRIX4:
				parseField(static_cast<MultiMatrix4&>(field));
				break;

			case Field::TYPE_NODE:
				parseField(static_cast<MultiNode&>(field));
				break;

			case Field::TYPE_ROTATION:
				parseField(static_cast<MultiRotation&>(field));
				break;

			case Field::TYPE_STRING:
				parseField(static_cast<MultiString&>(field));
				break;

			case Field::TYPE_TIME:
				parseField(static_cast<MultiTime&>(field));
				break;

			case Field::TYPE_VECTOR2:
				parseField(static_cast<MultiVector2&>(field));
				break;

			case Field::TYPE_VECTOR3:
				parseField(static_cast<MultiVector3&>(field));
				break;

			case Field::TYPE_VECTOR4:
				parseField(static_cast<MultiVector3&>(field));
				break;

			default:
				ocean_assert(false && "Missing implementation");
				throw MissingImplementationException();
		}
	}
}

void ClassicParser::parseField(SingleBool& field)
{
	try
	{
		field.setValue(parseBool(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFBool field: ") + message);
	}
}

void ClassicParser::parseField(SingleColor& field)
{
	try
	{
		const float red = max(0.0f, min(float(parseFloat()), 1.0f));
		const float green = max(0.0f, min(float(parseFloat()), 1.0f));
		const float blue = max(0.0f, min(float(parseFloat()), 1.0f));

		field.setValue(RGBAColor(red, green, blue), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFColor field: ") + message);
	}
}

void ClassicParser::parseField(SingleFloat& field)
{
	try
	{
		field.setValue(parseFloat(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFFloat field: ") + message);
	}
}

void ClassicParser::parseField(SingleInt& field)
{
	const Token token(scanner_.tokenPop());

	if (token.isInteger())
	{
		field.setValue(token.integer(), sceneTimestamp_);
	}
	else
	{
		throw OceanException(std::string("Could not parse a SFInt32 field, got \"") + token.raw() + std::string("\" instead"));
	}
}

void ClassicParser::parseField(SingleMatrix3& field)
{
	Scalar matrixValues[9];

	try
	{
		for (unsigned int n = 0u; n < 9u; n++)
		{
			matrixValues[n] = parseFloat();
		}

		SquareMatrix3 transposedMatrix(matrixValues);
		field.setValue(transposedMatrix.transposed(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a SFMatrix3f field: ") + std::string(exception.what()));
	}
}

void ClassicParser::parseField(SingleMatrix4& field)
{
	Scalar matrixValues[16];

	try
	{
		for (unsigned int n = 0u; n < 16u; n++)
		{
			matrixValues[n] = parseFloat();
		}

		SquareMatrix4 transposedMatrix(matrixValues);
		field.setValue(transposedMatrix.transposed(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a SFMatrix4f field: ") + std::string(exception.what()));
	}
}

void ClassicParser::parseField(SingleNode& field)
{
	if (scanner_.token().isKeyword(ClassicScanner::KEYWORD_USE))
	{
		scanner_.pop();
		const Token token(scanner_.tokenPop());

		if (token.isIdentifier() == false)
		{
			throw OceanException(std::string("Expected a node name after 'USE', got \"") + token.raw() + std::string("\" instead."));
		}

		bool found = false;
		NodeRefs nodes(scenePtr_->environment()->library()->nodes(token.identifier()));

		for (NodeRefs::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
		{
			SDXNodeRef node(*i);
			ocean_assert(node);

			if (node->sceneId() == scenePtr_->sceneId())
			{
				field.setValue(node);
				found = true;
				break;
			}
		}

		if (!found)
		{
			Log::warning() << "Failed to USE the node \"" << token.identifier() << "\" as it does not exist.";
		}
	}
	else
	{
		field.setValue(parseNode(), sceneTimestamp_);
	}
}

void ClassicParser::parseField(SingleRotation& field)
{
	try
	{
		field.setValue(parseRotation(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFRotation field: ") + message);
	}
}

void ClassicParser::parseField(SingleString& field)
{
	try
	{
		field.setValue(parseString(), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFString field: ") + message);
	}
}

void ClassicParser::parseField(SingleTime& field)
{
	try
	{
		field.setValue(Timestamp(parseFloat()), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFTime field: ") + message);
	}
}

void ClassicParser::parseField(SingleVector2& field)
{
	try
	{
		const float x = float(parseFloat());
		const float y = float(parseFloat());

		field.setValue(Vector2(x, y), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFVec2f field: ") + message);
	}
}

void ClassicParser::parseField(SingleVector3& field)
{
	try
	{
		const Scalar x = float(parseFloat());
		const Scalar y = float(parseFloat());
		const Scalar z = float(parseFloat());

		field.setValue(Vector3(x, y, z), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFVec3f field: ") + message);
	}
}

void ClassicParser::parseField(SingleVector4& field)
{
	try
	{
		const Scalar x = float(parseFloat());
		const Scalar y = float(parseFloat());
		const Scalar z = float(parseFloat());
		const Scalar w = float(parseFloat());

		field.setValue(Vector4(x, y, z, w), sceneTimestamp_);
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a SFVec4f field: ") + message);
	}
}

void ClassicParser::parseField(MultiBool& field)
{
	Token token(scanner_.token());

	MultiBool::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				values.emplace_back(parseBool());
			}
		}
		else
		{
			values.emplace_back(parseBool());
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFBool field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiColor& field)
{
	Token token(scanner_.token());

	MultiColor::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				const float red = max(0.0f, min(float(parseFloat()), 1.0f));
				const float green = max(0.0f, min(float(parseFloat()), 1.0f));
				const float blue = max(0.0f, min(float(parseFloat()), 1.0f));

				values.emplace_back(red, green, blue);
			}
		}
		else
		{
			const float red = max(0.0f, min(float(parseFloat()), 1.0f));
			const float green = max(0.0f, min(float(parseFloat()), 1.0f));
			const float blue = max(0.0f, min(float(parseFloat()), 1.0f));

			values.emplace_back(red, green, blue);
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFColor field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiFloat& field)
{
	Token token(scanner_.token());

	MultiFloat::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				values.emplace_back(parseFloat());
			}
		}
		else
		{
			values.emplace_back(parseFloat());
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFFloat field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiInt& field)
{
	Token token(scanner_.token());

	MultiInt::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				if (token.isInteger() == false)
				{
					throw OceanException(std::string("The token is not a integer, got \"") + token.raw() + std::string("\" instead"));
				}

				values.emplace_back(token.integer());
				scanner_.pop();
			}
		}
		else
		{
			if (token.isInteger() == false)
			{
				throw OceanException(std::string("The token is not a integer, got \"") + token.raw() + std::string("\" instead"));
			}

			values.emplace_back(token.integer());
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFInt32 field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiMatrix3& field)
{
	Token token(scanner_.token());

	MultiMatrix3::Values values;
	Scalar matrixValues[9];

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				for (unsigned int n = 0u; n < 9u; n++)
				{
					matrixValues[n] = parseFloat();
				}

				SquareMatrix3 transposedMatrix(matrixValues);
				values.emplace_back(transposedMatrix.transposed());
			}
		}
		else
		{
			for (unsigned int n = 0u; n < 9u; n++)
			{
				matrixValues[n] = parseFloat();
			}

			SquareMatrix3 transposedMatrix(matrixValues);
			values.emplace_back(transposedMatrix.transposed());
		}
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a MFMatrix3f field: ") + std::string(exception.what()));
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiMatrix4& field)
{
	Token token(scanner_.token());

	MultiMatrix4::Values values;
	Scalar matrixValues[16];

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				for (unsigned int n = 0u; n < 16u; n++)
				{
					matrixValues[n] = parseFloat();
				}

				SquareMatrix4 transposedMatrix(matrixValues);
				values.emplace_back(transposedMatrix.transposed());
			}
		}
		else
		{
			for (unsigned int n = 0u; n < 16u; n++)
			{
				matrixValues[n] = parseFloat();
			}

			SquareMatrix4 transposedMatrix(matrixValues);
			values.emplace_back(transposedMatrix.transposed());
		}
	}
	catch (const Exception& exception)
	{
		throw OceanException(std::string("Could not parse a MFMatrix4f field: ") + std::string(exception.what()));
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiNode& field)
{
	MultiNode::Values values;

	const size_t fieldBeginLine = scanner_.line();
	const size_t fieldBeginColumn = scanner_.column();

	try
	{
		if (scanner_.token().isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				if (scanner_.token().isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				if (scanner_.token().isKeyword(ClassicScanner::KEYWORD_USE))
				{
					scanner_.pop();
					const Token token(scanner_.tokenPop());

					if (token.isIdentifier() == false)
					{
						throw OceanException(std::string("Expected a node name after 'USE', got \"") + token.raw() + std::string("\" instead."));
					}

					NodeRef node;
					NodeRefs nodes(scenePtr_->environment()->library()->nodes(token.identifier()));
					for (NodeRefs::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
					{
						SDXNodeRef found(*i);
						ocean_assert(found);

						if (found->sceneId() == scenePtr_->sceneId())
						{
							node = found;
							break;
						}
					}

					if (node)
					{
						values.emplace_back(std::move(node));
					}
					else
					{
						Log::warning() << "Failed to USE the node \"" << token.identifier() << "\" as it does not exist.";
					}
				}
				else
				{
					NodeRef node(parseNode());

					if (node)
					{
						values.emplace_back(std::move(node));
					}
				}
			}
		}
		else
		{
			if (scanner_.token().isKeyword(ClassicScanner::KEYWORD_USE))
			{
				scanner_.pop();
				const Token token(scanner_.tokenPop());

				if (token.isIdentifier() == false)
				{
					throw OceanException(std::string("Expected a node name after 'USE', got \"") + token.raw() + std::string("\" instead."));
				}

				NodeRef node;
				NodeRefs nodes(scenePtr_->environment()->library()->nodes(token.identifier()));
				for (NodeRefs::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
				{
					SDXNodeRef found(*i);
					ocean_assert(found);

					if (found->sceneId() == scenePtr_->sceneId())
					{
						node = found;
						break;
					}
				}

				if (node)
				{
					values.emplace_back(std::move(node));
				}
				else
				{
					Log::warning() << "Failed to USE the node \"" << token.identifier() << "\" as it does not exist.";
				}
			}
			else
			{
				values.emplace_back(parseNode());
			}
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFNode field (") + String::toAString(fieldBeginLine) + std::string(", ") + String::toAString(fieldBeginColumn) + std::string("): ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiRotation& field)
{
	Token token(scanner_.token());

	MultiRotation::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				values.emplace_back(parseRotation());
			}
		}
		else
		{
			values.emplace_back(parseRotation());
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFRotation field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiString& field)
{
	Token token(scanner_.token());

	MultiString::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				values.emplace_back(parseString());
			}
		}
		else
		{
			values.emplace_back(parseString());
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFString field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiTime& field)
{
	Token token(scanner_.token());

	MultiTime::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				values.emplace_back(double(parseFloat()));
			}
		}
		else
		{
			values.emplace_back(double(parseFloat()));
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFTime field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiVector2& field)
{
	Token token(scanner_.token());

	MultiVector2::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				const Scalar x = parseFloat();
				const Scalar y = parseFloat();

				values.emplace_back(x, y);
			}
		}
		else
		{
			const Scalar x = parseFloat();
			const Scalar y = parseFloat();

			values.emplace_back(x, y);
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFVec2f field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiVector3& field)
{
	Token token(scanner_.token());

	MultiVector3::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				const Scalar x = parseFloat();
				const Scalar y = parseFloat();
				const Scalar z = parseFloat();

				values.emplace_back(x, y, z);
			}
		}
		else
		{
			const Scalar x = parseFloat();
			const Scalar y = parseFloat();
			const Scalar z = parseFloat();

			values.emplace_back(x, y, z);
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFVec3f field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseField(MultiVector4& field)
{
	Token token(scanner_.token());

	MultiVector4::Values values;

	try
	{
		if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_BEGIN))
		{
			scanner_.pop();

			while (true)
			{
				token = scanner_.token();

				if (token.isSymbol(ClassicScanner::SYMBOL_FIELD_END))
				{
					scanner_.pop();
					break;
				}

				const Scalar x = parseFloat();
				const Scalar y = parseFloat();
				const Scalar z = parseFloat();
				const Scalar w = parseFloat();

				values.emplace_back(x, y, z, w);
			}
		}
		else
		{
			const Scalar x = parseFloat();
			const Scalar y = parseFloat();
			const Scalar z = parseFloat();
			const Scalar w = parseFloat();

			values.emplace_back(x, y, z, w);
		}
	}
	catch (const Exception& exception)
	{
		std::string message(exception.what());
		throw OceanException(std::string("Could not parse a MFVec4f field: ") + message);
	}

	field.setValues(values, sceneTimestamp_);
}

void ClassicParser::parseRoute()
{
	// start node
	Token token(scanner_.tokenPop());
	if (!token.isIdentifier())
	{
		throw OceanException(std::string("Failed to parse a ROUTE statement, expected a start node, got \"") + token.raw() + std::string("\" instead"));
	}

	const std::string startNodeName = token.identifier();

	// dot between start node and start field
	token = scanner_.tokenPop();
	if (!token.isSymbol(ClassicScanner::SYMBOL_DOT))
	{
		throw OceanException(std::string("Failed to parse a ROUTE statement, expected a \".\" between start node and start field, got \"") + token.raw() + std::string("\" instead"));
	}

	// start field
	token = scanner_.tokenPop();
	if (!token.isIdentifier())
	{
		throw OceanException(std::string("Failed to parse a ROUTE statement, expected a start field, got \"") + token.raw() + std::string("\" instead"));
	}

	std::string startFieldName = token.identifier();

	// TO keyword
	token = scanner_.tokenPop();
	if (!token.isKeyword(ClassicScanner::KEYWORD_TO))
	{
		throw OceanException(std::string("Failed to parse a ROUTE statement, expected a \"TO\" keyword, got \"") + token.raw() + std::string("\" instead"));
	}

	// target node
	token = scanner_.tokenPop();
	if (!token.isIdentifier())
	{
		throw OceanException(std::string("Failed to parse a ROUTE statement, expected a target node, got \"") + token.raw() + std::string("\" instead"));
	}

	const std::string targetNodeName = token.identifier();

	// dot between target node and target field
	token = scanner_.tokenPop();
	if (!token.isSymbol(ClassicScanner::SYMBOL_DOT))
	{
		throw OceanException(std::string("Failed to parse a ROUTE statement, expected a \".\" between target node and target field, got \"") + token.raw() + std::string("\" instead"));
	}

	// target field
	token = scanner_.tokenPop();
	if (!token.isIdentifier())
	{
		throw OceanException(std::string("Failed to parse a ROUTE statement, expected a target field, got \"") + token.raw() + std::string("\" instead"));
	}

	std::string targetFieldName = token.identifier();

	try
	{
		const X3DNodeRef startNode = scenePtr_->environment()->library()->nodeManager().node(startNodeName, scenePtr_->sceneId());
		if (startNode.isNull())
		{
			throw OceanException(std::string("The start node \"") + startNodeName + std::string("\" of ROUTE ") + startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		startFieldName = startNode->originalFieldName(startFieldName);

		if (startNode->hasField(startFieldName) == false)
		{
			throw OceanException(std::string("The start field \"") + startFieldName + std::string("\" of ROUTE ") + startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		const NodeRef targetNode = scenePtr_->environment()->library()->nodeManager().node(targetNodeName, scenePtr_->sceneId());
		if (targetNode.isNull())
		{
			throw OceanException(std::string("The target node \"") + targetNodeName + std::string("\" of ROUTE ") + startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		targetFieldName = targetNode->originalFieldName(targetFieldName);

		if (targetNode->hasField(targetFieldName) == false)
		{
			throw OceanException(std::string("The target field \"") + targetFieldName + std::string("\" of ROUTE ") + startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" does not exist."));
		}

		const Field& startField = startNode->field(startFieldName);
		const Field& targetField = targetNode->field(targetFieldName);

		if (startField.type() != targetField.type() || startField.dimension() != targetField.dimension())
		{
			throw OceanException(std::string("The fields of ROUTE ") + startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" have different types."));
		}

		if ((startNode->fieldAccessType(startFieldName) & Node::ACCESS_GET) == 0)
		{
			throw OceanException(std::string("The start field \"") + startFieldName + std::string("\" of ROUTE ") + startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" must be an output field."));
		}

		 if ((targetNode->fieldAccessType(targetFieldName) & Node::ACCESS_SET) == 0)
		 {
			 throw OceanException(std::string("The target field \"") + targetFieldName + std::string("\" of ROUTE ") + startNodeName + std::string(".") + startFieldName + std::string(" TO ") + targetNodeName + std::string(".") + targetFieldName + std::string(" must be an input field."));
		 }

		startNode->addConnection(startFieldName, targetNode->id(), targetFieldName);
	}
	catch(const Exception& exception)
	{
		Log::warning() << "Failed to establish the ROUTE in line " << scanner_.line() << ": " << exception.what();
	}
}

void ClassicParser::parseMeta()
{
	Token token(scanner_.tokenPop());
	if (!token.isString())
	{
		throw OceanException(std::string("Failed to parse a META statement, expected a quoted string as key, got \"") + token.raw() + std::string("\" instead"));
	}

	token = scanner_.tokenPop();
	if (!token.isString())
	{
		throw OceanException(std::string("Failed to parse a META statement, expected a quoted string as value, got \"") + token.raw() + std::string("\" instead"));
	}
}

void ClassicParser::parseProfile()
{
	Token token(scanner_.tokenPop());
	if (!token.isIdentifier())
	{
		throw OceanException(std::string("Failed to parse a PROFILE statement, expected a profile name, got \"") + token.raw() + std::string("\" instead"));
	}
}

bool ClassicParser::parseBool()
{
	const Token token(scanner_.tokenPop());

	if (token.isKeyword(ClassicScanner::KEYWORD_TRUE))
	{
		return true;
	}

	if (token.isKeyword(ClassicScanner::KEYWORD_FALSE))
	{
		return false;
	}

	throw OceanException(std::string("Could not parse a boolean, got \"") + token.raw() + std::string("\" instead"));
}

Scalar ClassicParser::parseFloat()
{
	const Token token(scanner_.tokenPop());

	if (token.isNumber())
	{
		return token.number();
	}

	if (token.isInteger())
	{
		return Scalar(token.integer());
	}

	throw OceanException(std::string("The token is not a float, got \"") + token.raw() + std::string("\" instead"));
}

std::string ClassicParser::parseString()
{
	Token token(scanner_.tokenPop());

	if (token.isString())
	{
		// ensuring that \" is replaced to "

		return String::replace(token.moveString(), "\\\"", "\"", false /*onlyFirstOccurrence*/);
	}

	if (token.isIdentifier())
	{
		return token.identifier();
	}

	throw OceanException(std::string("The token is not a string, got \"") + token.raw() + std::string("\" instead"));
}

Rotation ClassicParser::parseRotation()
{
	const Scalar x = parseFloat();
	const Scalar y = parseFloat();
	const Scalar z = parseFloat();
	const Scalar a = parseFloat();

	Vector3 axis(x, y, z);
	if (axis.normalize())
	{
		return Rotation(axis, a);
	}

	Log::warning() << "Invalid rotation axis in \"" << scanner_.filename() << "\" in line: " << scanner_.line() << ", using default axis instead.";
	return Rotation();
}

bool ClassicParser::skipNode(const bool nodeStartedAlready)
{
	Token token(scanner_.tokenPop());

	if (!nodeStartedAlready && !token.isSymbol(ClassicScanner::SYMBOL_NODE_BEGIN))
	{
		return false;
	}

	unsigned int nodes = 1u;
	while (nodes != 0u)
	{
		try
		{
			token = scanner_.tokenPop();

			if (token.isEndOfFile())
			{
				return false;
			}
			else if (token.isSymbol(ClassicScanner::SYMBOL_NODE_BEGIN))
			{
				++nodes;
			}
			else if (token.isSymbol(ClassicScanner::SYMBOL_NODE_END))
			{
				--nodes;
			}
		}
		catch(...)
		{
			// nothing to do here
		}
	}

	return true;
}

}

}

}

}
