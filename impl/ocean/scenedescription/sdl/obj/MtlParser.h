/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MTL_PARSER_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MTL_PARSER_H

#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdl/obj/Material.h"
#include "ocean/scenedescription/sdl/obj/MtlScanner.h"

#include <vector>

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

/**
 * This class implements a obj material parser able to parse mtl files.
 * @ingroup scenedescriptionsdlobj
 */
class OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT MtlParser
{
	public:

		/**
		 * Definition of a vector holding materials.
		 */
		typedef std::vector<Material> Materials;

	protected:

		/**
		 * Definition of a scanner token.
		 */
		typedef MtlScanner::Token Token;

	public:

		/**
		 * Creates a new obj mtl parser.
		 * @param filename Name of the mtl file to parse
		 */
		MtlParser(const std::string& filename);

		/**
		 * Parses the mtl file and returns all parsed material objects.
		 * @return The resulting materials
		 * @param cancel Cancel state allows the cancelation of a load process while loading process hasn't finished, if the cancel state is used the load process stops if the value is set to True
		 */
		Materials parse(bool* cancel = nullptr);

	protected:

		/**
		 * Parses a obj material.
		 * @param materials The materials to which the parsed material object will be added
		 */
		void parseMaterial(Materials& materials);

		/**
		 * Parses the ambient color.
		 * @param material Material receiving the ambient color
		 */
		void parseAmbientColor(Material& material);

		/**
		 * Parses the dissolve value.
		 * @param material Material receiving the dissolve value
		 */
		void parseDissolve(Material& material);

		/**
		 * Parses the diffuse color.
		 * @param material Material receiving the diffuse color
		 */
		void parseDiffuseColor(Material& material);

		/**
		 * Parses the ambient texture name.
		 * @param material Material receiving the ambient texture name
		 */
		void parseAmbientTexture(Material& material);

		/**
		 * Parses the diffuse texture name.
		 * @param material Material receiving the diffuse texture name
		 */
		void parseDiffuseTexture(Material& material);

		/**
		 * Parses the emissive color.
		 * @param material Material receiving the emissive color
		 */
		void parseEmissiveColor(Material& material);

		/**
		 * Parses the optical density value.
		 * @param material Material receiving the density value
		 */
		void parseOpticalDensity(Material& material);

		/**
		 * Parses the illumination model.
		 * @param material Material receiving the illumination model
		 */
		void parseIlluminationModel(Material& material);

		/**
		 * Parses the spcular color.
		 * @param material Material receiving the specular color
		 */
		void parseSpecularColor(Material& material);

		/**
		 * Parses the specular highlight.
		 * @param material Meterial receiving the specular highlight value
		 */
		void parseSpecularHighlight(Material& material);

		/**
		 * Parses the transmission filter value.
		 * @param material Meterial receiving the specular highlight value
		 */
		void parseTransmissionFilter(Material& material);

		/**
		 * Parses the transparency value.
		 * @param material Meterial receiving the transparency value
		 */
		void parseTransparency(Material& material);

		/**
		 * Parses an unsupported keyword.
		 */
		void parseUnsupportedKeyword();

		/**
		 * Parses a float value.
		 * @return Parsed float value
		 */
		Scalar parseFloat();

	protected:

		/// Scanner able to scan obj mtl token.
		MtlScanner scanner_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_MTL_PARSER_H
