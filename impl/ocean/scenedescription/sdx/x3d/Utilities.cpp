/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Utilities.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

bool Utilities::writeHeader(std::ostream& stream)
{
	if (stream.bad())
	{
		return false;
	}

	stream << "#X3D V3.0 utf8\n\n#Created with Ocean's X3D scene description utilities\n\n";

	return stream.good();
}

bool Utilities::writePointSet(std::ostream& stream, const Vectors3& vertices, const RGBAColor& emissiveColor, const RGBAColors& colors, const std::string& indentation, const unsigned int vertexPrecision, const std::string& name)
{
	if (stream.bad())
	{
		return false;
	}

	if (vertices.empty() || (!colors.empty() && vertices.size() != colors.size()))
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	if (name.empty())
	{
		stream << indentation << "Shape\n";
	}
	else
	{
		stream << indentation << "DEF " << name << " Shape\n";
	}

	stream << indentation << "{\n" << indentation << "\tgeometry PointSet\n" << indentation << "\t{\n";
	stream << indentation << "\t\tcoord Coordinate\n" << indentation << "\t\t{\n" << indentation << "\t\t\tpoint\n" << indentation << "\t\t\t[\n";

	const std::string valueIndentation = indentation + "\t\t\t\t";

	for (const Vector3& vertex : vertices)
	{
		stream << valueIndentation << String::toAString(vertex.x(), vertexPrecision) << " " << String::toAString(vertex.y(), vertexPrecision) << " " << String::toAString(vertex.z(), vertexPrecision) << ",\n";
	}

	stream << indentation << "\t\t\t]\n" << indentation << "\t\t}\n";

	if (!colors.empty())
	{
		stream << "\n" << indentation << "\t\tcolor Color\n" << indentation << "\t\t{\n" << indentation << "\t\t\tcolor\n" << indentation << "\t\t\t[\n";

		for (const RGBAColor& color : colors)
		{
			stream << valueIndentation << color.red() << " " << color.green() << " " << color.blue() << ",\n";
		}

		stream << indentation << "\t\t\t]\n" << indentation << "\t\t}\n";
	}

	stream << indentation << "\t}\n";

	if (emissiveColor.isValid())
	{
		stream << "\n" << indentation << "\tappearance Appearance\n" << indentation << "\t{\n";

		stream << indentation << "\t\tmaterial Material\n" << indentation << "\t\t{\n";
		stream << indentation << "\t\t\temissiveColor " << emissiveColor.red() << " " << emissiveColor.green() << " " << emissiveColor.blue() << "\n";

		if (!emissiveColor.isOpaque())
		{
			stream << indentation << "\t\ttransparency " << 1.0f - emissiveColor.alpha() << "\n";
		}

		stream << indentation << "\t\t}\n";
		stream << indentation << "\t}\n";
	}

	stream << indentation << "}\n";

	return stream.good();
}

bool Utilities::writeIndexedFaceSet(std::ostream& stream, const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces, const RGBAColor& color, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Vectors2& perVertexTextureCoordinates, const std::string& textureUrl, const std::string& indentation, const unsigned int vertexPrecision, const std::string& name)
{
	if (stream.bad())
	{
		return false;
	}

	if (vertices.empty() || (!perVertexNormals.empty() && vertices.size() != perVertexNormals.size()) || (!perVertexColors.empty() && vertices.size() != perVertexColors.size()) || (!perVertexTextureCoordinates.empty() && vertices.size() != perVertexTextureCoordinates.size()))
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	if (name.empty())
	{
		stream << indentation << "Shape\n";
	}
	else
	{
		stream << indentation << "DEF " << name << " Shape\n";
	}

	stream << indentation << "{\n" << indentation << "\tgeometry IndexedFaceSet\n" << indentation << "\t{\n";
	stream << indentation << "\t\tcoord Coordinate\n" << indentation << "\t\t{\n" << indentation << "\t\t\tpoint\n" << indentation << "\t\t\t[\n";

	const std::string valueIndentationSmall = indentation + "\t\t\t";
	const std::string valueIndentation = indentation + "\t\t\t\t";

	for (const Vector3& vertex : vertices)
	{
		stream << valueIndentation << String::toAString(vertex.x(), vertexPrecision) << " " << String::toAString(vertex.y(), vertexPrecision) << " " << String::toAString(vertex.z(), vertexPrecision) << ",\n";
	}

	stream << indentation << "\t\t\t]\n" << indentation << "\t\t}\n";

	if (!perVertexNormals.empty())
	{
		stream << "\n" << indentation << "\t\tcoordIndex\n" << indentation << "\t\t[\n";

		for (const Rendering::TriangleFace& triangleFace : triangleFaces)
		{
			stream << valueIndentationSmall << triangleFace[0] << " " << triangleFace[1] << " " << triangleFace[2] << " -1,\n";
		}

		stream << indentation << "\t\t]\n";
	}

	if (!perVertexNormals.empty())
	{
		stream << "\n" << indentation << "\t\tnormal Normal\n" << indentation << "\t\t{\n" << indentation << "\t\t\tvector\n" << indentation << "\t\t\t[\n";

		for (const Vector3& normal : perVertexNormals)
		{
			stream << valueIndentation << String::toAString(normal.x(), vertexPrecision) << " " << String::toAString(normal.y(), vertexPrecision) << " " << String::toAString(normal.z(), vertexPrecision) << ",\n";
		}

		stream << indentation << "\t\t\t]\n" << indentation << "\t\t}\n";

		stream << "\n" << indentation << "\t\tnormalIndex\n" << indentation << "\t\t[\n";

		for (const Rendering::TriangleFace& triangleFace : triangleFaces)
		{
			stream << valueIndentationSmall << triangleFace[0] << " " << triangleFace[1] << " " << triangleFace[2] << " -1,\n";
		}

		stream << indentation << "\t\t]\n";
	}

	if (!perVertexTextureCoordinates.empty())
	{
		stream << "\n" << indentation << "\t\ttexCoord TextureCoordinate\n" << indentation << "\t\t{\n" << indentation << "\t\t\tpoint\n" << indentation << "\t\t\t[\n";

		for (const Vector2& textureCoordinate : perVertexTextureCoordinates)
		{
			stream << valueIndentation << String::toAString(textureCoordinate.x(), vertexPrecision) << " " << String::toAString(textureCoordinate.y(), vertexPrecision) << ",\n";
		}

		stream << indentation << "\t\t\t]\n" << indentation << "\t\t}\n";
	}

	if (!perVertexColors.empty())
	{
		stream << "\n" << indentation << "\t\tcolor Color\n" << indentation << "\t\t{\n" << indentation << "\t\t\tcolor\n" << indentation << "\t\t\t[\n";

		for (const RGBAColor& perVertexColor : perVertexColors)
		{
			stream << valueIndentation << perVertexColor.red() << " " << perVertexColor.green() << " " << perVertexColor.blue() << ",\n";
		}

		stream << indentation << "\t\t\t]\n" << indentation << "\t\t}\n";

		stream << "\n" << indentation << "\t\tcolorIndex\n" << indentation << "\t\t[\n";

		for (const Rendering::TriangleFace& triangleFace : triangleFaces)
		{
			stream << valueIndentationSmall << triangleFace[0] << " " << triangleFace[1] << " " << triangleFace[2] << " -1,\n";
		}

		stream << indentation << "\t\t]\n";
	}

	stream << indentation << "\t}\n";

	if (color.isValid() || !textureUrl.empty())
	{
		stream << "\n" << indentation << "\tappearance Appearance\n" << indentation << "\t{\n";

		if (color.isValid())
		{
			stream << indentation << "\t\tmaterial Material\n" << indentation << "\t\t{\n";
			stream << indentation << "\t\t\tdiffuseColor " << color.red() << " " << color.green() << " " << color.blue() << "\n";

			if (!color.isOpaque())
			{
				stream << indentation << "\t\ttransparency " << 1.0f - color.alpha() << "\n";
			}

			stream << indentation << "\t\t}\n";
		}

		if (!textureUrl.empty())
		{
			stream << indentation << "\t\ttexture ImageTexture\n" << indentation << "\t\t{\n";
			stream << indentation << "\t\t\turl \"" << textureUrl << "\"\n";
			stream << indentation << "\t\t}\n";
		}

		stream << indentation << "\t}\n";
	}

	stream << indentation << "}\n";

	return stream.good();
}

bool Utilities::writeCoordinateSystem(std::ostream& stream, const HomogenousMatrix4& world_T_coordinateSystem, const RGBAColor& xAxisColor, const RGBAColor& yAxisColor, const RGBAColor& zAxisColor, const std::string& indentation, const std::string& name)
{
	if (stream.bad())
	{
		return false;
	}

	if (!writeTransformNodeStart(stream, world_T_coordinateSystem, indentation, name))
	{
		return false;
	}

	stream << indentation << "\tchildren\n" << indentation << "\t[\n";

	const std::string innerIndentation = indentation + "\t\t";

	stream << innerIndentation << "# X-axis\n";

	if (!writeCylinder(stream, HomogenousMatrix4(Vector3(Scalar(0.45), 0, 0), Rotation(0, 0, 1, -Numeric::pi_2())), Scalar(0.9), Scalar(0.05), xAxisColor, innerIndentation))
	{
		return false;
	}

	stream << "\n";

	if (!writeCone(stream, HomogenousMatrix4(Vector3(Scalar(0.95), 0, 0), Rotation(0, 0, 1, -Numeric::pi_2())), Scalar(0.1), Scalar(0.1), xAxisColor, innerIndentation))
	{
		return false;
	}

	stream << "\n";


	stream << innerIndentation << "# Y-axis\n";

	if (!writeCylinder(stream, HomogenousMatrix4(Vector3(0, Scalar(0.45), 0)), Scalar(0.9), Scalar(0.05), yAxisColor, innerIndentation))
	{
		return false;
	}

	stream << "\n";

	if (!writeCone(stream, HomogenousMatrix4(Vector3(0, Scalar(0.95), 0)), Scalar(0.1), Scalar(0.1), yAxisColor, innerIndentation))
	{
		return false;
	}

	stream << "\n";


	stream << innerIndentation << "# Z-axis\n";

	if (!writeCylinder(stream, HomogenousMatrix4(Vector3(0, 0, Scalar(0.45)), Rotation(1, 0, 0, Numeric::pi_2())), Scalar(0.9), Scalar(0.05), zAxisColor, innerIndentation))
	{
		return false;
	}

	stream << "\n";

	if (!writeCone(stream, HomogenousMatrix4(Vector3(0, 0, Scalar(0.95)), Rotation(1, 0, 0, Numeric::pi_2())), Scalar(0.1), Scalar(0.1), zAxisColor, innerIndentation))
	{
		return false;
	}

	stream << indentation << "\t]\n" << indentation << "}\n";

	return stream.good();
}

bool Utilities::writeCoordinateSystems(std::ostream& stream, const HomogenousMatrices4& world_T_coordinateSystems, const RGBAColor& xAxisColor, const RGBAColor& yAxisColor, const RGBAColor& zAxisColor, const std::string& indentation, const std::string& name)
{
	if (stream.bad())
	{
		return false;
	}

	if (world_T_coordinateSystems.empty() || name.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	for (size_t n = 0; n < world_T_coordinateSystems.size(); ++n)
	{
		const HomogenousMatrix4& world_T_coordinateSystem = world_T_coordinateSystems[n];

		if (n != 0)
		{
			stream << "\n";
		}

		if (!writeTransformNodeStart(stream, world_T_coordinateSystem, indentation, name))
		{
			return false;
		}

		stream << indentation << "\tchildren\n" << indentation << "\t[\n";

		if (n == 0)
		{
			if (!writeCoordinateSystem(stream, HomogenousMatrix4(true), xAxisColor, yAxisColor, zAxisColor, indentation + "\t\t", name))
			{
				return false;
			}
		}
		else
		{
			stream << indentation << "\t\tUSE " << name << "\n";
		}

		stream << indentation << "\t]\n" << indentation << "}\n";
	}

	return stream.good();
}

bool Utilities::writeCylinder(std::ostream& stream, const HomogenousMatrix4& world_T_cylinder, const Scalar height, const Scalar radius, const RGBAColor& color, const std::string& indentation, const std::string& name)
{
	if (stream.bad())
	{
		return false;
	}

	if (height < 0 || radius < 0)
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	if (!writeTransformNodeStart(stream, world_T_cylinder, indentation, name))
	{
		return false;
	}

	stream << indentation << "\tchildren Shape\n" << indentation << "\t{\n";
	stream << indentation << "\t\tgeometry Cylinder\n" << indentation << "\t\t{\n";
	stream << indentation << "\t\t\theight " << height << "\n";
	stream << indentation << "\t\t\tradius " << radius << "\n";
	stream << indentation << "\t\t}\n\n";

	stream << indentation << "\t\tappearance Appearance\n" << indentation << "\t\t{\n";
	stream << indentation << "\t\t\tmaterial Material\n" << indentation << "\t\t\t{\n";
	stream << indentation << "\t\t\t\tdiffuseColor " << color.red() << " " << color.green() << " " << color.blue() << "\n";

	if (!color.isOpaque())
	{
		stream << indentation << "\t\t\t\ttransparency " << 1.0f - color.alpha() << "\n";
	}

	stream << indentation << "\t\t\t}\n" << indentation << "\t\t}\n" << indentation << "\t}\n" << indentation << "}\n";

	return stream.good();
}

bool Utilities::writeCone(std::ostream& stream, const HomogenousMatrix4& world_T_cone, const Scalar height, const Scalar bottomRadius, const RGBAColor& color, const std::string& indentation, const std::string& name)
{
	if (stream.bad())
	{
		return false;
	}

	if (height < 0 || bottomRadius < 0)
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	if (!writeTransformNodeStart(stream, world_T_cone, indentation, name))
	{
		return false;
	}

	stream << indentation << "\tchildren Shape\n" << indentation << "\t{\n";
	stream << indentation << "\t\tgeometry Cone\n" << indentation << "\t\t{\n";
	stream << indentation << "\t\t\theight " << height << "\n";
	stream << indentation << "\t\t\tbottomRadius " << bottomRadius << "\n";
	stream << indentation << "\t\t}\n\n";

	stream << indentation << "\t\tappearance Appearance\n" << indentation << "\t\t{\n";
	stream << indentation << "\t\t\tmaterial Material\n" << indentation << "\t\t\t{\n";
	stream << indentation << "\t\t\t\tdiffuseColor " << color.red() << " " << color.green() << " " << color.blue() << "\n";

	if (!color.isOpaque())
	{
		stream << indentation << "\t\t\t\ttransparency " << 1.0f - color.alpha() << "\n";
	}

	stream << indentation << "\t\t\t}\n" << indentation << "\t\t}\n" << indentation << "\t}\n" << indentation << "}\n";

	return stream.good();
}

bool Utilities::writeText(std::ostream& stream, const HomogenousMatrix4& world_T_text, const std::string& text, const Scalar size, const std::string& indentation, const std::string& name)
{
	if (stream.bad())
	{
		return false;
	}

	if (text.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	if (!writeTransformNodeStart(stream, world_T_text, indentation, name))
	{
		return false;
	}

	stream << indentation << "\tchildren Shape\n" << indentation << "\t{\n";
	stream << indentation << "\t\tgeometry Text\n" << indentation << "\t\t{\n";
	stream << indentation << "\t\t\tstring [\"" << text << "\"]" << "\n";
	stream << indentation << "\t\t\tfontStyle FontStyle\n" << indentation << "\t\t\t{\n";
	stream << indentation << "\t\t\t\tjustify [\"MIDDLE\", \"MIDDLE\"]\n";
	stream << indentation << "\t\t\t\tsize " << size << "\n";

	stream << indentation << "\t\t\t}\n" << indentation << "\t\t}\n" << indentation << "\t}\n" << indentation << "}\n";

	return stream.good();
}

bool Utilities::writeTransformNodeStart(std::ostream& stream, const HomogenousMatrix4& world_T_transform, const std::string& indentation, const std::string& name)
{
	if (!world_T_transform.isValid())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	if (name.empty())
	{
		stream << indentation << "Transform\n";
	}
	else
	{
		stream << indentation << "DEF " << name << " Transform\n";
	}

	stream << indentation << "{\n";

	bool placedTransformation = false;

	const Vector3 translation = world_T_transform.translation();

	if (!translation.isNull())
	{
		stream << indentation << "\ttranslation " << translation.x() << " " << translation.y() << " " << translation.z() << "\n";
		placedTransformation = true;
	}

	const Quaternion quaternion = world_T_transform.rotation();

	if (quaternion != Quaternion(true))
	{
		const Rotation rotation(quaternion);

		stream << indentation << "\trotation " << rotation.axis().x() << " " << rotation.axis().y() << " " << rotation.axis().z() << " " << rotation.angle() << "\n";
		placedTransformation = true;
	}

	const Vector3 scale = world_T_transform.scale();

	if (scale != Vector3(1, 1, 1))
	{
		stream << indentation << "\tscale " << scale.x() << " " << scale.y() << " " << scale.z() << "\n";
		placedTransformation = true;
	}

	if (placedTransformation)
	{
		stream << "\n";
	}

	return true;
}

}

}

}

}
