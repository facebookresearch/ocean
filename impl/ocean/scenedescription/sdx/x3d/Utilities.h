/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_UTILITIES_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_UTILITIES_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/Vector3.h"

#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements utility functions.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Utilities
{
	public:

		/**
		 * Writes the header of a X3D file.
		 * @param stream The output stream to which the header will be written, must be valid
		 * @return True, if succeeded
		 */
		static bool writeHeader(std::ostream& stream);

		/**
		 * Writes a shape with a PointSet geometry to an output stream.
		 * @param stream The output stream to which the shape will be written, must be valid
		 * @param vertices The 3D vertices for the PointSet, at least one
		 * @param emissiveColor Optional emissive color of the point's material, invalid otherwise
		 * @param colors Optional per-vertex colors, one for each vertex or empty
		 * @param indentation Optional indentation
		 * @param vertexPrecision The precision of the vertices, with range [0, infinity)
		 * @param name Optional name of the shape
		 * @return True, if succeeded
		 */
		static bool writePointSet(std::ostream& stream, const Vectors3& vertices, const RGBAColor& emissiveColor = RGBAColor(false), const RGBAColors& colors = RGBAColors(), const std::string& indentation = std::string(), const unsigned int vertexPrecision = 6u, const std::string& name = std::string());

		/**
		 * Writes a shape with an IndexdFaceSet geometry to an output stream.
		 * @param stream The output stream to which the shape will be written, must be valid
		 * @param vertices The 3D vertices for the IndexdFaceSet, at least three
		 * @param triangleFaces  The triangle faces to write, at least one
		 * @param color Optional per-mesh color, invalid to not define a mesh color
		 * @param perVertexNormals Optional per-vertex normals, one for each vertex or empty
		 * @param perVertexColors Optional per-vertex colors, one for each vertex or empty
		 * @param perVertexTextureCoordinates Optional per-vertex texture coordinates, one for each vertex or empty
		 * @param textureUrl The URL of the image texture, empty otherwise
		 * @param indentation Optional indentation
		 * @param vertexPrecision The precision of the vertices, with range [0, infinity)
		 * @param name Optional name of the shape
		 * @return True, if succeeded
		 */
		static bool writeIndexedFaceSet(std::ostream& stream, const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces, const RGBAColor& color = RGBAColor(false), const Vectors3& perVertexNormals = Vectors3(), const RGBAColors& perVertexColors = RGBAColors(), const Vectors2& perVertexTextureCoordinates = Vectors2(), const std::string& textureUrl = std::string(), const std::string& indentation = std::string(), const unsigned int vertexPrecision = 6u, const std::string& name = std::string());

		/**
		 * Writes a coordinate system to an output stream.
		 * This function actually writes a Group node containing several shapes likes Cylinders and Cones.<br>
		 * The length of each axis is one unit.
		 * @param stream The output stream to which the coordinate system will be written, must be valid
		 * @param world_T_coordinateSystem The transformation between coordinate system and world, must be valid
		 * @param xAxisColor The color of the x-axis
		 * @param yAxisColor The color of the y-axis
		 * @param zAxisColor The color of the z-axis
		 * @param indentation Optional indentation
		 * @param name The optional name of the Group node
		 * @return True, if succeeded
		 */
		static bool writeCoordinateSystem(std::ostream& stream, const HomogenousMatrix4& world_T_coordinateSystem, const RGBAColor& xAxisColor = RGBAColor(1.0f, 0.0f, 0.0f), const RGBAColor& yAxisColor = RGBAColor(0.0f, 1.0f, 0.0f), const RGBAColor& zAxisColor = RGBAColor(0.0f, 0.0f, 1.0f), const std::string& indentation = std::string(), const std::string& name = std::string());

		/**
		 * Writes coordinate systems to an output stream.
		 * @param stream The output stream to which the coordinate system will be written, must be valid
		 * @param world_T_coordinateSystems The transformations between coordinate systems and world, at least one
		 * @param xAxisColor The color of the x-axis
		 * @param yAxisColor The color of the y-axis
		 * @param zAxisColor The color of the z-axis
		 * @param indentation Optional indentation
		 * @param name A unique name which can be used to define & reference coordinate systems, must be valid
		 * @return True, if succeeded
		 */
		static bool writeCoordinateSystems(std::ostream& stream, const HomogenousMatrices4& world_T_coordinateSystems, const RGBAColor& xAxisColor = RGBAColor(1.0f, 0.0f, 0.0f), const RGBAColor& yAxisColor = RGBAColor(0.0f, 1.0f, 0.0f), const RGBAColor& zAxisColor = RGBAColor(0.0f, 0.0f, 1.0f), const std::string& indentation = std::string(), const std::string& name = "OCEAN_COORDINATE_SYSTEM");

		/**
		 * Writes a Cylinder to an output stream.
		 * This function actually writes a Transform node containing the Cylinder.
		 * @param stream The output stream to which the cylinder will be written, must be valid
		 * @param world_T_cylinder The transformation between cylinder and world, must be valid
		 * @param height The height of the cylinder, with range [0, infinity)
		 * @param radius The radius of the cylinder, with range [0, infinity)
		 * @param color The color of the cylinder
		 * @param indentation Optional indentation
		 * @param name The optional name of the Transform node
		 * @return True, if succeeded
		 */
		static bool writeCylinder(std::ostream& stream, const HomogenousMatrix4& world_T_cylinder, const Scalar height, const Scalar radius, const RGBAColor& color, const std::string& indentation = std::string(), const std::string& name = std::string());

		/**
		 * Writes a Cone to an output stream.
		 * This function actually writes a Transform node containing the Cone.
		 * @param stream The output stream to which the cone will be written, must be valid
		 * @param world_T_cone The transformation between cone and world, must be valid
		 * @param height The height of the cone, with range [0, infinity)
		 * @param bottomRadius The bottom radius of the cone, with range [0, infinity)
		 * @param color The color of the cone
		 * @param indentation Optional indentation
		 * @param name The optional name of the Transform node
		 * @return True, if succeeded
		 */
		static bool writeCone(std::ostream& stream, const HomogenousMatrix4& world_T_cone, const Scalar height, const Scalar bottomRadius, const RGBAColor& color, const std::string& indentation = std::string(), const std::string& name = std::string());

		/**
		 * Writes a Text to an output stream.
		 * This function actually writes a Transform node containing the Text.
		 * @param stream The output stream to which the text will be written, must be valid
		 * @param world_T_text The transformation between text and world, must be valid
		 * @param text The text string, must be valid
		 * @param size The text size, with range (0, infinity)
		 * @param indentation Optional indentation
		 * @param name The optional name of the Transform node
		 * @return True, if succeeded
		 */
		static bool writeText(std::ostream& stream, const HomogenousMatrix4& world_T_text, const std::string& text, const Scalar size, const std::string& indentation = std::string(), const std::string& name = std::string());

	protected:

		/**
		 * Writes the start of a Transform node to an output stream.
		 * Afterwards the 'children' field as to be written followed by a closing bracket for the Transform node.
		 * @param stream The output stream to which the node will be written, must be valid
		 * @param world_T_transform The transformation between the Transform node and the world, must be valid
		 * @param indentation Optional indentation
		 * @param name The optional name of the Transform node
		 * @return True, if succeeded
		 */
		static bool writeTransformNodeStart(std::ostream& stream, const HomogenousMatrix4& world_T_transform, const std::string& indentation = std::string(), const std::string& name = std::string());
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_UTILITIES_H
