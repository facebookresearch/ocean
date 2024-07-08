/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/MtlParser.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

MtlParser::MtlParser(const std::string& filename) :
	scanner_(filename)
{
	// nothing to do here
}

MtlParser::Materials MtlParser::parse(bool* cancel)
{
	Materials materials;

	if (scanner_.isValid() == false)
	{
		Log::warning() << "Failed to load material file: \"" << scanner_.filename() << "\".";
	}

	try
	{
		while ((!cancel || !*cancel) && scanner_.token().isEndOfFile() == false)
		{
			if (scanner_.token().isKeyword(MtlScanner::KEYWORD_NEWMTL) == false)
			{
				throw OceanException(std::string("Expected a 'newmtl' keyword, got \"") + scanner_.token().raw() + std::string("\" instead."));
			}

			parseMaterial(materials);
		}
	}
	catch(const Exception& exception)
	{
		Log::error() << "Could not parse material file \"" << scanner_.filename() << "\". Line: " << scanner_.line() << ", column: " << scanner_.column() << ". " << exception.what();
	}

	return materials;
}

void MtlParser::parseMaterial(Materials& materials)
{
	Material material;

	const Token token(scanner_.lineToken());

	std::string materialName = token.line();

	material.setName(materialName);

	scanner_.pop();

	while (scanner_.token().isEndOfFile() == false)
	{
		if (scanner_.token().isKeyword() == false)
		{
			Log::warning() << "Expected a keyword defining a material in file: \"" << scanner_.filename() << ":" << scanner_.line() << ", got \"" << scanner_.token().raw() << "\" instead, trying to skip this unsupported token.";
			parseUnsupportedKeyword();
			continue;
		}

		switch (scanner_.token().keyword())
		{
			case MtlScanner::KEYWORD_D:
				parseDissolve(material);
				break;

			case MtlScanner::KEYWORD_ILLUM:
				parseIlluminationModel(material);
				break;

			case MtlScanner::KEYWORD_MAP_KA:
				parseAmbientTexture(material);
				break;

			case MtlScanner::KEYWORD_MAP_KD:
				parseDiffuseTexture(material);
				break;

			case MtlScanner::KEYWORD_NI:
				parseOpticalDensity(material);
				break;

			case MtlScanner::KEYWORD_KA:
				parseAmbientColor(material);
				break;

			case MtlScanner::KEYWORD_KD:
				parseDiffuseColor(material);
				break;

			case MtlScanner::KEYWORD_KE:
				parseEmissiveColor(material);
				break;

			case MtlScanner::KEYWORD_KS:
				parseSpecularColor(material);
				break;

			case MtlScanner::KEYWORD_NS:
				parseSpecularHighlight(material);
				break;

			case MtlScanner::KEYWORD_TF:
				parseTransmissionFilter(material);
				break;

			case MtlScanner::KEYWORD_TR:
				parseTransparency(material);
				break;

			case MtlScanner::KEYWORD_NEWMTL:
				materials.emplace_back(std::move(material));
				return;

			default:
				Log::warning() << "The keyword \"" << scanner_.token().raw() << "\"is not supported, trying to skip it.";
				parseUnsupportedKeyword();
				break;
		}
	}

	materials.emplace_back(std::move(material));
}

void MtlParser::parseAmbientColor(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_KA));
	scanner_.pop();

	const float red = max(0.0f, min(1.0f, float(parseFloat())));
	const float green = max(0.0f, min(1.0f, float(parseFloat())));
	const float blue = max(0.0f, min(1.0f, float(parseFloat())));

	material.setAmbientColor(RGBAColor(red, green, blue));
}

void MtlParser::parseDissolve(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_D));
	scanner_.pop();

	const float dissolve = max(0.0f, min(1.0f, float(parseFloat())));

	material.setTransparency(1 - dissolve);
}

void MtlParser::parseDiffuseColor(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_KD));
	scanner_.pop();

	const float red = max(0.0f, min(1.0f, float(parseFloat())));
	const float green = max(0.0f, min(1.0f, float(parseFloat())));
	const float blue = max(0.0f, min(1.0f, float(parseFloat())));

	material.setDiffuseColor(RGBAColor(red, green, blue));
}

void MtlParser::parseAmbientTexture(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_MAP_KA));

	const Token token(scanner_.lineToken());

	if (material.textureName().empty())
	{
		material.setTextureName(token.line());
	}
	else if (material.textureName() != token.line())
	{
		Log::warning() << "The defined ambient texture is different from the already defined one.";
	}

	scanner_.pop();
}

void MtlParser::parseDiffuseTexture(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_MAP_KD));

	const Token token(scanner_.lineToken());

	if (!material.textureName().empty() && material.textureName() != token.line())
	{
		Log::warning() << "The defined diffuse texture is different from the already defined one.";
	}

	material.setTextureName(token.line());

	scanner_.pop();
}

void MtlParser::parseEmissiveColor(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_KE));
	scanner_.pop();

	const float red = max(0.0f, min(1.0f, float(parseFloat())));
	const float green = max(0.0f, min(1.0f, float(parseFloat())));
	const float blue = max(0.0f, min(1.0f, float(parseFloat())));

	material.setEmissiveColor(RGBAColor(red, green, blue));
}

void MtlParser::parseOpticalDensity(Material& /*material*/)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_NI));
	scanner_.pop();

	const Token token(scanner_.tokenPop());
	if (token.isIntegerOrNumber() == false)
	{
		throw OceanException(std::string("Expected a number as optical density value, got \"") + token.raw() + std::string("\" instead."));
	}
}

void MtlParser::parseIlluminationModel(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_ILLUM));
	scanner_.pop();

	const Token token(scanner_.tokenPop());
	if (token.isInteger() == false)
	{
		throw OceanException(std::string("Expected integer as illumination model, got \"") + token.raw() + std::string("\" instead."));
	}

	material.setIlluminationModel(Material::IlluminationModel(token.integer()));
}

void MtlParser::parseSpecularColor(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_KS));
	scanner_.pop();

	const float red = max(0.0f, min(1.0f, float(parseFloat())));
	const float green = max(0.0f, min(1.0f, float(parseFloat())));
	const float blue = max(0.0f, min(1.0f, float(parseFloat())));

	material.setSpecularColor(RGBAColor(red, green, blue));
}

void MtlParser::parseSpecularHighlight(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_NS));
	scanner_.pop();

	/**
	 * Ns exponent
	 * Specifies the specular exponent for the current material.  This defines the focus of the specular highlight.
	 * "exponent" is the value for the specular exponent.  A high exponent results in a tight, concentrated highlight.
	 * Ns values normally range from 0 to 1000.
	 */

	material.setSpecularExponent(parseFloat());
}

void MtlParser::parseTransmissionFilter(Material& /*material*/)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_TF));
	scanner_.pop();

	/**
	 * To specify the transmission filter of the current material, you can use the "Tf" statement, the "Tf spectral" statement, or the "Tf xyz" statement.
	 *
	 * Any light passing through the object is filtered by the transmission filter, which only allows the specifiec colors to pass through.
	 * For example, Tf 0 1 0 allows all the green to pass through and filters out all the red and blue.
	 *
	 * "r g b" are the values for the red, green, and blue components of the atmosphere.  The g and b arguments are optional.
	 * If only r is specified, then g, and b are assumed to be equal to r.  The r g b values are normally in the range of 0.0 to 1.0.
	 * Values outside this range increase or decrease the relectivity accordingly.
	 */

	Scalar redTransmissionValue = parseFloat();

	Scalar greenTransmissionValue = redTransmissionValue;
	Scalar blueTransmissionValue = redTransmissionValue;

	if (scanner_.token().isIntegerOrNumber())
	{
		greenTransmissionValue = parseFloat();

		if (scanner_.token().isIntegerOrNumber())
		{
			blueTransmissionValue = parseFloat();
		}
	}

	if (redTransmissionValue != 1 || greenTransmissionValue != 1 || blueTransmissionValue != 1)
	{
		Log::warning() << "To date a transmission filter value with \"Tf 1, 1, 1\" is supported only.";
	}
}

void MtlParser::parseTransparency(Material& material)
{
	ocean_assert(scanner_.token().isKeyword(MtlScanner::KEYWORD_TR));
	scanner_.pop();

	const float transparency = max(0.0f, min(1.0f, float(parseFloat())));

	material.setTransparency(1 - transparency);
}

void MtlParser::parseUnsupportedKeyword()
{
	scanner_.pop();
	scanner_.lineToken();
	scanner_.pop();
}

Scalar MtlParser::parseFloat()
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
