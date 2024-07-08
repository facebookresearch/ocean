/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_CLASSIC_PARSER_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_CLASSIC_PARSER_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/ClassicScanner.h"

#include "ocean/scenedescription/SDXNode.h"
#include "ocean/scenedescription/Field0D.h"
#include "ocean/scenedescription/Field1D.h"
#include "ocean/scenedescription/SDXScene.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

// Forward declaration.
class X3DScene;

/**
 * This class implements a parser for the classic x3d file format.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT ClassicParser
{
	protected:

		/**
		 * Definition of a scanner token.
		 */
		typedef ClassicScanner::Token Token;

	public:

		/**
		 * Creates a new classic parser.
		 * @param filename Name of the file to parse
		 * @param progress Optional parser progress state recurrently receiving the parsing progress if defined
		 * @param cancel Optional cancel state to stop the progress during process
		 */
		explicit ClassicParser(const std::string& filename, float* progress = nullptr, bool* cancel = nullptr);

		/**
		 * Parses the given file and returns a node reference to the entire scene.
		 * @param library The library providing all nodes
		 * @param engine Rendering engine object to create corresponding rendering object from
		 * @param timestamp Parsing timestamp all scene objects will be initialized with this timestamp
		 * @return Parsed scene
		 */
		SDXSceneRef parse(const Library& library, const Rendering::EngineRef& engine, const Timestamp timestamp = Timestamp());

	protected:

		/**
		 * Parses a new node.
		 */
		NodeRef parseNode();

		/**
		 * Adds a dynamic field.
		 * @param token The token holding the dynamic field type
		 * @param node The node receiving the dynamic field
		 */
		void addDynamicField(const Token& token, const NodeRef& node);

		/**
		 * Parses a field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(Field& field);

		/**
		 * Parses a SFBool field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleBool& field);

		/**
		 * Parses a SFColor field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleColor& field);

		/**
		 * Parses a SFFloat field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleFloat& field);

		/**
		 * Parses a SFInt32 field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleInt& field);

		/**
		 * Parses a SFMatrix3 field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleMatrix3& field);

		/**
		 * Parses a SFMatrix4 field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleMatrix4& field);

		/**
		 * Parses a SFNode field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleNode& field);

		/**
		 * Parses a SFRotation field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleRotation& field);

		/**
		 * Parses a SFString field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleString& field);

		/**
		 * Parses a SFTime field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleTime& field);

		/**
		 * Parses a SFVec2f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleVector2& field);

		/**
		 * Parses a SFVec3f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleVector3& field);

		/**
		 * Parses a SFVec4f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(SingleVector4& field);

		/**
		 * Parses a MFBool field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiBool& field);

		/**
		 * Parses a MFColor field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiColor& field);

		/**
		 * Parses a MFFloat field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiFloat& field);

		/**
		 * Parses a MFInt32 field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiInt& field);

		/**
		 * Parses a MFMatrix3f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiMatrix3& field);

		/**
		 * Parses a MFMatrix4f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiMatrix4& field);

		/**
		 * Parses a MFNode field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiNode& field);

		/**
		 * Parses a MFRotation field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiRotation& field);

		/**
		 * Parses a MFString field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiString& field);

		/**
		 * Parses a MFTime field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiTime& field);

		/**
		 * Parses a MFVec2f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiVector2& field);

		/**
		 * Parses a MFVec3f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiVector3& field);

		/**
		 * Parses a MFVec4f field.
		 * @param field The field receiving the parsed data
		 */
		void parseField(MultiVector4& field);

		/**
		 * Parses a route statement.
		 */
		void parseRoute();

		/**
		 * Parses a meta statement.
		 */
		void parseMeta();

		/**
		 * Parses a profile statement.
		 */
		void parseProfile();

		/**
		 * Parses a boolean value.
		 * @return The parsed boolean value
		 */
		bool parseBool();

		/**
		 * Parses a float value.
		 * @return Parsed float value
		 */
		Scalar parseFloat();

		/**
		 * Parses a string value.
		 * @return Parsed string value
		 */
		std::string parseString();

		/**
		 * Parses a rotation value.
		 * @return Parsed rotation value
		 */
		Rotation parseRotation();

		/**
		 * Skips a unknown x3d node.
		 * @param nodeStartedAlready True, if the node has been started with an opening break already, false otherwise
		 * @return True, if the node could be skipped successfully
		 */
		bool skipNode(const bool nodeStartedAlready);

	protected:

		/// Scanner scanning x3d token.
		ClassicScanner scanner_;

		/// X3D scene holding the parsed scene.
		X3DScene* scenePtr_ = nullptr;

		/// Scene timestamp.
		Timestamp sceneTimestamp_ = Timestamp(false);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_CLASSIC_PARSER_H
