/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/OBJParser.h"
#include "ocean/scenedescription/sdl/obj/MtlParser.h"
#include "ocean/scenedescription/sdl/obj/OBJScene.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

OBJParser::OBJParser(const std::string& filename, float* progress) :
	scanner_(filename, progress)
{
	// nothing to do here
}

SDLSceneRef OBJParser::parse(bool* cancel)
{
	OBJScene* scene = new OBJScene(scanner_.filename());

	try
	{
		while ((!cancel || !*cancel) && scanner_.token().isEndOfFile() == false)
		{
			if (scanner_.token().isKeyword() == false)
			{
				throw OceanException(std::string("Expected a keyword like 'v', 'n' or 'f', got \"") + scanner_.token().raw() + std::string("\" instead."));
			}

			switch (scanner_.token().keyword())
			{
				case OBJScanner::KEYWORD_F:
					parseFace(*scene);
					break;

				case OBJScanner::KEYWORD_G:
					parseGroup(*scene);
					break;

				case OBJScanner::KEYWORD_L:
					parseLine(*scene);
					break;

				case OBJScanner::KEYWORD_MTLLIB:
					parseMtlLib(*scene);
					break;

				case OBJScanner::KEYWORD_O:
					parseObject(*scene);
					break;

				case OBJScanner::KEYWORD_P:
					parsePoint(*scene);
					break;

				case OBJScanner::KEYWORD_USEMTL:
					parseUseMtl(*scene);
					break;

				case OBJScanner::KEYWORD_S:
					parseSmoothingFactor(*scene);
					break;

				case OBJScanner::KEYWORD_V:
					parseVertex(*scene);
					break;

				case OBJScanner::KEYWORD_VN:
					parseNormal(*scene);
					break;

				case OBJScanner::KEYWORD_VP:
					parseParameter(*scene);
					break;

				case OBJScanner::KEYWORD_VT:
					parseTextureCoordinate(*scene);
					break;

				default:
					throw NotImplementedException(std::string("The keyword \"") + scanner_.token().raw() + std::string("\"is not supported."));
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << "Could not parse scene \"" << scanner_.filename() << "\". Line: " << scanner_.line() << ", column: " << scanner_.column() << ". " << exception.what();
	}

	return SDLSceneRef(scene);
}

void OBJParser::parseVertex(OBJScene& scene)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_V));
	scanner_.pop();

	const Scalar x(parseFloat());
	const Scalar y(parseFloat());
	const Scalar z(parseFloat());

	scene.addVertex(Vector3(x, y, z));

	// a fourth value represents 'w' (a homogenous coordinate)
	// three additional values represent vertex colors

	unsigned int skippedValues = 0u;

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		if (scanner_.token().isIntegerOrNumber())
		{
			scanner_.tokenPop();
			++skippedValues;
		}
		else
		{
			break;
		}
	}

	if (skippedValues == 2u)
	{
		throw OceanException("Keyword 'v' expects either 3 values, 4 values (homogenous coordinates), or 6 values");
	}
}

void OBJParser::parseNormal(OBJScene& scene)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_VN));
	scanner_.pop();

	const Scalar x(parseFloat());
	const Scalar y(parseFloat());
	const Scalar z(parseFloat());

	scene.addNormal(Vector3(x, y, z).normalizedOrZero());
}

void OBJParser::parseParameter(OBJScene& /*scene*/)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_VP));
	scanner_.pop();

	// this implementation does not use the parameter values
	parseFloat();
	parseFloat();
	parseFloat();
}

void OBJParser::parseTextureCoordinate(OBJScene& scene)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_VT));
	scanner_.pop();

	const Scalar x(parseFloat());
	Vector2 textureCoordinate(x, 0);

	if (scanner_.token().isIntegerOrNumber())
	{
		textureCoordinate.y() = scanner_.tokenPop().integerOrNumber();

		if (scanner_.token().isIntegerOrNumber())
		{
			scanner_.pop();
		}
	}

	scene.addTextureCoordinate(textureCoordinate);
}

void OBJParser::parseFace(OBJScene& scene)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_F));
	scanner_.pop();

	Rendering::VertexIndices vertexIndices;
	Rendering::VertexIndices textureIndices;
	Rendering::VertexIndices normalIndices;

	int index;
	while (scanner_.token().isInteger())
	{
		// vertex index
		index = scanner_.token().integer();

		if (index < 0)
		{
			index = int(scene.vertices().size()) + index + 1;
		}

		if (index == 0)
		{
			throw OceanException("\'0\' is an invalid vertex index.");
		}

		if (index > int(scene.vertices().size()))
		{
			throw OceanException(std::string("Invalid vertex index \"") + String::toAString(index) + std::string("\" there are ") + String::toAString(scene.vertices().size()) + std::string(" vertices defined only."));
		}

		vertexIndices.emplace_back((unsigned int)(index - 1));
		scanner_.pop();

		if (scanner_.token().isSymbol(OBJScanner::SYMBOL_SLASH))
		{
			scanner_.pop();

			// vertex texture coordinate
			if (scanner_.token().isInteger())
			{
				index = scanner_.token().integer();

				if (index < 0)
				{
					index = int(scene.textureCoordinates().size()) + index + 1;
				}

				if (index == 0)
				{
					throw OceanException("\'0\' is an invalid vertex index.");
				}

				if (index > int(scene.textureCoordinates().size()))
				{
					throw OceanException(std::string("Invalid texture coordinate index \"") + String::toAString(index) + std::string("\" there are ") + String::toAString(scene.textureCoordinates().size()) + std::string(" texture coordinates defined only."));
				}

				textureIndices.emplace_back((unsigned int)(index - 1));

				scanner_.pop();
			}

			if (scanner_.token().isSymbol(OBJScanner::SYMBOL_SLASH))
			{
				scanner_.pop();
			}
			else
			{
				continue;
			}

			// vertex normal
			if (scanner_.token().isInteger() == false)
			{
				throw OceanException(std::string("Expected an index of a normal, got \"") + scanner_.token().raw() + std::string("\" instead."));
			}

			index = scanner_.token().integer();

			if (index < 0)
			{
				index = int(scene.normals().size()) + index + 1;
			}

			if (index == 0)
			{
				throw OceanException("\'0\' is an invalid vertex index.");
			}

			if (index > int(scene.normals().size()))
			{
				throw OceanException(std::string("Invalid normal index \"") + String::toAString(index) + std::string("\" there are ") + String::toAString(scene.normals().size()) + std::string(" normals defined only."));
			}

			normalIndices.emplace_back((unsigned int)(index - 1));

			scanner_.pop();
		}
	}

	if (normalIndices.size() != vertexIndices.size() && normalIndices.empty() == false)
	{
		throw OceanException("Invalid number of normal indices.");
	}

	if (textureIndices.size() != vertexIndices.size() && textureIndices.empty() == false)
	{
		throw OceanException("Invalid number of texture coordinate indices.");
	}

	scene.addFace(OBJScene::Face(vertexIndices, normalIndices, textureIndices));
}

void OBJParser::parseGroup(OBJScene& /*scene*/)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_G));

	Token token(scanner_.lineToken());
	scanner_.pop();

	// this implementation parses the group but does not use it
}

void OBJParser::parseLine(OBJScene& /*scene*/)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_L));

	Token token(scanner_.lineToken());
	scanner_.pop();

	Log::warning() << "Lines are not supported.";

	// this implementation parses the line but does not use ist
}

void OBJParser::parseMtlLib(OBJScene& scene)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_MTLLIB));

	Token token(scanner_.lineToken());

	IO::Files resolved(IO::FileResolver::get().resolve(IO::File(token.line()), IO::File(scene.filename())));

	bool fileFound = false;
	for (unsigned int n = 0; n < resolved.size(); n++)
	{
		if (resolved[n].exists())
		{
			fileFound = true;

			MtlParser mtlParser(resolved[n]());
			MtlParser::Materials materials(mtlParser.parse());

			if (materials.empty() == false)
			{
				scene.setMaterials(std::move(materials));
				break;
			}
		}
	}

	if (!fileFound)
	{
		Log::warning() << "Could not resolve the defined material file: \"" << token.line() << "\".";
		Log::warning() << "Possible path would have been:";

		for (IO::Files::const_iterator i = resolved.begin(); i != resolved.end(); ++i)
		{
			Log::warning() << (*i)();
		}
	}

	scanner_.pop();
}

void OBJParser::parseObject(OBJScene& /*scene*/)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_O));

	Token token(scanner_.lineToken());
	scanner_.pop();
}

void OBJParser::parsePoint(OBJScene& /*scene*/)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_P));

	Token token(scanner_.lineToken());
	scanner_.pop();

	Log::warning() << "Points are not supported.";

	// this implementation parses the points but does not use ist
}

void OBJParser::parseUseMtl(OBJScene& scene)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_USEMTL));

	const Token token(scanner_.lineToken());
	scene.setCurrentMaterial(token.line());

	scanner_.pop();
}

void OBJParser::parseSmoothingFactor(OBJScene& /*scene*/)
{
	ocean_assert(scanner_.token().isKeyword(OBJScanner::KEYWORD_S));
	scanner_.pop();

	Token token(scanner_.tokenPop());

	// accepting an integer, a float or 'off' as token
	if (!token.isNumber() && !token.isInteger() && !token.isKeyword(OBJScanner::KEYWORD_OFF))
	{
		throw OceanException(std::string("The token is not a float or the keyword 'off', got \"") + token.raw() + std::string("\" instead."));
	}
}

Scalar OBJParser::parseFloat()
{
	Token token(scanner_.tokenPop());

	if (token.isIntegerOrNumber())
	{
		return token.integerOrNumber();
	}

	throw OceanException(std::string("The token is not a float, got \"") + token.raw() + std::string("\" instead."));
}

}

}

}

}
