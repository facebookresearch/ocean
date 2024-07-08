/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_PARSER_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_PARSER_H

#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdl/obj/OBJScanner.h"

#include "ocean/scenedescription/SDLScene.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

// Forward declaration
class OBJScene;

/**
 * This class implements a obj parser able to parse obj files.
 * @ingroup scenegrpahobj
 */
class OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT OBJParser
{
	protected:

		/**
		 * Definition of a scanner token.
		 */
		typedef OBJScanner::Token Token;

	public:

		/**
		 * Creates a new parser for obj files.
		 * @param filename Name of the file to parse
		 * @param progress Optional progress state receiving the parser progress recurrently
		 */
		explicit OBJParser(const std::string& filename, float* progress = nullptr);

		/**
		 * Parses the given file and returns a node reference to the entire scene.
		 * @return Parsed scene
		 * @param cancel Cancel state allows the cancelation of a load process while loading process hasn't finished, if the cancel state is used the load process stops if the value is set to True
		 */
		SDLSceneRef parse(bool* cancel = nullptr);

	protected:

		/**
		 * Parses a vertex.
		 * @param scene Scene receiving the parsed vertex
		 */
		void parseVertex(OBJScene& scene);

		/**
		 * Parses a normal.
		 * @param scene Scene receiving the parsed normal
		 */
		void parseNormal(OBJScene& scene);

		/**
		 * Parses a parameter.
		 * @param scene Scene receiving the parsed parameter
		 */
		void parseParameter(OBJScene& scene);

		/**
		 * Parses a texture coordinate.
		 * @param scene Scene receiving the parsed texture coordinate
		 */
		void parseTextureCoordinate(OBJScene& scene);

		/**
		 * Parses a new face.
		 * @param scene Scene receiving the parsed face
		 */
		void parseFace(OBJScene& scene);

		/**
		 * Parses a group.
		 * @param scene Scene receiving the parsed group
		 */
		void parseGroup(OBJScene& scene);

		/**
		 * Parses a line.
		 * @param scene Scene receiving the parsed line
		 */
		void parseLine(OBJScene& scene);

		/**
		 * Parses a mtllib statement.
		 * @param scene Scene receiving the parsed material library
		 */
		void parseMtlLib(OBJScene& scene);

		/**
		 * Parses a object name statement.
		 * @param scene Scene receiving the parsed object name
		 */
		void parseObject(OBJScene& scene);

		/**
		 * Parses a point statement.
		 * @param scene Scene receiving the parsed point
		 */
		void parsePoint(OBJScene& scene);

		/**
		 * Parses a usemtl statement.
		 * @param scene Scene receiving the parsed material
		 */
		void parseUseMtl(OBJScene& scene);

		/**
		 * Parses a smoothing factor.
		 * @param scene Scene receiving the parsed smoothing factor
		 */
		void parseSmoothingFactor(OBJScene& scene);

		/**
		 * Parses a float value.
		 * @return Parsed float value
		 */
		Scalar parseFloat();

	protected:

		/// Scanner scanning obj token.
		OBJScanner scanner_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_PARSER_H
