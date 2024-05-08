/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_XML_PARSER_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_XML_PARSER_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DScene.h"

#include "ocean/io/Scanner.h"

#include "ocean/scenedescription/SDXNode.h"
#include "ocean/scenedescription/Field0D.h"
#include "ocean/scenedescription/Field1D.h"
#include "ocean/scenedescription/SDXScene.h"

#include <tinyxml2.h>

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a XML parser for X3D.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT XMLParser
{
	protected:

		/**
		 * This class implements a helper scanner for the XML X3D file format.
		 */
		class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT XMLScanner : virtual public IO::Scanner
		{
			public:

				/**
				 * Creates a new XML scanner.
				 * @param data The data to be parsed
				 * @see BufferScanner::BufferScanner().
				 */
				explicit XMLScanner(const std::string& data);
		};

	public:

		/**
		 * Creates a new XML parser.
		 * @param filename Name of the file to parse
		 * @param progress Optional parser progress state recurrently receiving the parsing progress if defined
		 * @param cancel Optional cancel state to stop the progress during process
		 */
		explicit XMLParser(const std::string& filename, float* progress = nullptr, bool* cancel = nullptr);

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
		 * @param xmlNode XML node to be parsed
		 * @return Resulting X3D node
		 */
		NodeRef parseNode(const tinyxml2::XMLElement* xmlNode);

		/**
		 * Parses a new node.
		 * @param parent The parent X3D node
		 * @param xmlNode XML node to be parsed
		 */
		void parseNodeField(NodeRef& parent, const tinyxml2::XMLElement* xmlNode);

		/**
		 * Parses a route statement
		 * @param xmlNode XML node to be parsed
		 */
		void parseRoute(const tinyxml2::XMLElement* xmlNode);

		/**
		 * Adds a dynamic field.
		 * @param parent The parent X3D node
		 * @param xmlNode XML node to be parsed
		 */
		void addDynamicField(NodeRef& parent, const tinyxml2::XMLElement* xmlNode);

		/**
		 * Parses a field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(Field& field, const std::string& value);

		/**
		 * Parses a SFBool field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleBool& field, const std::string& value);

		/**
		 * Parses a SFColor field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleColor& field, const std::string& value);

		/**
		 * Parses a SFFloat field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleFloat& field, const std::string& value);

		/**
		 * Parses a SFInt32 field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleInt& field, const std::string& value);

		/**
		 * Parses a SFMatrix3 field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleMatrix3& field, const std::string& value);

		/**
		 * Parses a SFMatrix4 field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleMatrix4& field, const std::string& value);

		/**
		 * Parses a SFRotation field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleRotation& field, const std::string& value);

		/**
		 * Parses a SFString field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleString& field, const std::string& value);

		/**
		 * Parses a SFTime field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleTime& field, const std::string& value);

		/**
		 * Parses a SFVec2f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleVector2& field, const std::string& value);

		/**
		 * Parses a SFVec3f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleVector3& field, const std::string& value);

		/**
		 * Parses a SFVec4f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(SingleVector4& field, const std::string& value);

		/**
		 * Parses a MFBool field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiBool& field, const std::string& value);

		/**
		 * Parses a MFColor field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiColor& field, const std::string& value);

		/**
		 * Parses a MFFloat field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiFloat& field, const std::string& value);

		/**
		 * Parses a MFInt32 field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiInt& field, const std::string& value);

		/**
		 * Parses a MFMatrix3f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiMatrix3& field, const std::string& value);

		/**
		 * Parses a MFMatrix4f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiMatrix4& field, const std::string& value);

		/**
		 * Parses a MFRotation field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiRotation& field, const std::string& value);

		/**
		 * Parses a MFString field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiString& field, const std::string& value);

		/**
		 * Parses a MFTime field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiTime& field, const std::string& value);

		/**
		 * Parses a MFVec2f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiVector2& field, const std::string& value);

		/**
		 * Parses a MFVec3f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiVector3& field, const std::string& value);

		/**
		 * Parses a MFVec4f field.
		 * @param field The field receiving the parsed data
		 * @param value The value to be parsed
		 * @return True, if succeeded
		 */
		bool parseValueField(MultiVector4& field, const std::string& value);

		/**
		 * Parses a bool value.
		 * @param scanner The scanner to be used for parsing
		 * @return Parsed bool value
		 */
		bool parseBool(IO::Scanner& scanner);

		/**
		 * Parses a float value.
		 * @param scanner The scanner to be used for parsing
		 * @return Parsed float value
		 */
		Scalar parseFloat(IO::Scanner& scanner);

		/**
		 * Parses an integer value.
		 * @param scanner The scanner to be used for parsing
		 * @return Parsed float value
		 */
		int parseInt(IO::Scanner& scanner);

		/**
		 * Parses a string value.
		 * @param scanner The scanner to be used for parsing
		 * @return Parsed string value
		 */
		std::string parseString(IO::Scanner& scanner);

		/**
		 * Parses a rotation value.
		 * @param scanner The scanner to be used for parsing
		 * @return Parsed rotation value
		 */
		Rotation parseRotation(IO::Scanner& scanner);

		/**
		 * The lookup function for node field.
		 * @param parent The parent node in that the field has to be found
		 * @param nodeType Type of the child node for that the corresponding field has to be found
		 * @return Resulting field name
		 */
		static std::string fieldNameLookup(const NodeRef& parent, const std::string& nodeType);

	protected:

		/// XML parser.
		tinyxml2::XMLDocument xmlDocument_;

		/// XML file opening result.
		tinyxml2::XMLError xmlOpenResult_ = tinyxml2::XML_SUCCESS;

		/// X3D scene holding the parsed scene.
		X3DScene* scene_ = nullptr;

		/// Scene filename.
		std::string sceneFilename_;

		/// Scene timestamp.
		Timestamp sceneTimestamp_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_XML_PARSER_H
