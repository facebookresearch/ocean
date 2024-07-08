/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/IndexedFaceSet.h"
#include "ocean/scenedescription/sdx/x3d/Coordinate.h"
#include "ocean/scenedescription/sdx/x3d/TextureCoordinate.h"
#include "ocean/scenedescription/sdx/x3d/PhantomTextureCoordinate.h"

#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

IndexedFaceSet::IndexedFaceSet(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	X3DComposedGeometryNode(environment),
	convex_(true),
	creaseAngle_(0)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createTriangles();
}

IndexedFaceSet::NodeSpecification IndexedFaceSet::specifyNode()
{
	NodeSpecification specification("IndexedFaceSet");

	registerField(specification, "set_colorIndex", setColorIndex_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "set_coordIndex", setCoordIndex_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "set_normalIndex", setNormalIndex_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "set_texCoordIndex", setTexCoordIndex_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "colorIndex", colorIndex_, ACCESS_NONE);
	registerField(specification, "convex", convex_, ACCESS_NONE);
	registerField(specification, "coordIndex", coordIndex_, ACCESS_NONE);
	registerField(specification, "creaseAngle", creaseAngle_, ACCESS_NONE);
	registerField(specification, "normalIndex", normalIndex_, ACCESS_NONE);
	registerField(specification, "texCoordIndex", texCoordIndex_, ACCESS_NONE);

	X3DComposedGeometryNode::registerFields(specification);

	return specification;
}

void IndexedFaceSet::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DComposedGeometryNode::onInitialize(scene, timestamp);

	apply();
}

void IndexedFaceSet::onFieldChanged(const std::string& /*fieldName*/)
{
	ocean_assert(false && "Missing implementation.");
}

bool IndexedFaceSet::onFieldChanging(const std::string& /*fieldName*/, const Field& /*field*/)
{
	ocean_assert(false && "Missing implementation.");
	return false;
}

void IndexedFaceSet::apply()
{
	if (renderingObject_.isNull())
	{
		renderingObject_ = engine()->factory().createTriangles();
	}

	Rendering::TrianglesRef renderingTriangles(renderingObject_);
	if (renderingTriangles.isNull() || renderingVertexSet_.isNull())
	{
		return;
	}

	if (convex_.value() == false)
	{
		Log::warning() << "IndexedFaceSet supports convex polygons only, however the mesh is interpreted as convex.";
	}

	if (coord_.value().isNull() || coord_.value()->hasField("point") == false)
	{
		Log::warning() << "IndexedFaceSet holds no coordinates.";
		return;
	}

	const MultiVector3::Values& points = coord_.value()->field<MultiVector3>("point").values();

	const bool hasColors = bool(color_.value());
	const MultiColor::Values* colors = nullptr;
	if (hasColors)
	{
		colors = &color_.value()->field<MultiColor>("color").values();
	}

	const bool hasNormals = bool(normal_.value());
	const MultiVector3::Values* normals = nullptr;
	if (hasNormals)
	{
		normals = &normal_.value()->field<MultiVector3>("vector").values();
	}

	const bool hasTextureCoordinates = bool(texCoord_.value());
	const MultiVector2::Values* texCoords = nullptr;
	const MultiVector3::Values* phantomTexCoords = nullptr;

	if (hasTextureCoordinates)
	{
		const Field& field = texCoord_.value()->field("point");

		if (field.isType(Field::TYPE_VECTOR2, 1))
		{
			texCoords = &texCoord_.value()->field<MultiVector2>("point").values();
		}
		else if (field.isType(Field::TYPE_VECTOR3, 1))
		{
			phantomTexCoords = &texCoord_.value()->field<MultiVector3>("point").values();
		}
	}


	const MultiInt::Values& coordinateIndices = coordIndex_.values();
	const MultiInt::Values& colorIndices = colorIndex_.values();
	const MultiInt::Values& normalIndices = normalIndex_.values();
	const MultiInt::Values& texCoordIndices = texCoordIndex_.values();

	Rendering::Vertices renderingVertices;
	Rendering::Normals renderingNormals;
	Rendering::TextureCoordinates renderingTextureCoordinates;
	Rendering::Vertices renderingPhantomTextureCoordinates;
	RGBAColors renderingColors;

	unsigned int numberPolygonVertices = 0u;

	bool useColors = false;
	bool useTextureCoordinates = false;

	try
	{
		// check whether all coordinate indices are valid
		for (MultiInt::Values::const_iterator i = coordinateIndices.begin(); i != coordinateIndices.end(); ++i)
		{
			if (*i == -1)
			{
				if (numberPolygonVertices < 3u)
				{
					throw OceanException("A valid face/polygon must hold at least three vertex indices.");
				}

				numberPolygonVertices = 0;
			}
			else
			{
				++numberPolygonVertices;

				if (*i < -1 || Rendering::VertexIndex(*i) >= points.size())
				{
					std::string message(std::string("Invalid coordinate index \"") + String::toAString(*i)
							+ std::string("\", there are only ") + String::toAString(points.size()) + std::string(" vertices defined."));

					throw OceanException(message);
				}
			}
		}

		if (numberPolygonVertices > 0 && numberPolygonVertices < 3)
		{
			throw OceanException("A valid face must hold at least three vertex indices.");
		}

		if (hasColors && colorPerVertex_.value())
		{
			if (colorIndices.size() < coordinateIndices.size())
			{
				throw OceanException("Not enough defined color indices, there have to be at least as many indices as coordinate indices.");
			}
		}

		if (hasTextureCoordinates && texCoordIndices.empty() == false)
		{
			if (texCoordIndices.size() < coordinateIndices.size())
			{
				throw OceanException("Not enough defined texture coordinate indices, there have to be at least as many indices as coordinate indices.");
			}
		}

		Rendering::VertexIndex first;
		unsigned int numberPolygons = 0;

		for (unsigned int n = 0u; n < coordinateIndices.size(); n++)
		{
			if (coordinateIndices[n] == -1)
			{
				continue;
			}

			numberPolygonVertices = 0;

			if (ccw_.value())
			{
				first = n;
				renderingVertices.emplace_back(points[coordinateIndices[first]]);
				renderingVertices.emplace_back(points[coordinateIndices[first + 1]]);
				renderingVertices.emplace_back(points[coordinateIndices[first + 2]]);

				numberPolygonVertices += 3;
				n += 3;

				while (coordinateIndices.size() > n && coordinateIndices[n] != -1)
				{
					renderingVertices.emplace_back(points[coordinateIndices[first]]);
					renderingVertices.emplace_back(points[coordinateIndices[n - 1]]);
					renderingVertices.emplace_back(points[coordinateIndices[n]]);
					++numberPolygonVertices;
					++n;
				}
			}
			else /// ccw.value() == false
			{
				first = n;
				renderingVertices.emplace_back(points[coordinateIndices[first]]);
				renderingVertices.emplace_back(points[coordinateIndices[first + 2]]);
				renderingVertices.emplace_back(points[coordinateIndices[first + 1]]);

				numberPolygonVertices += 3;
				n += 3;

				while (coordinateIndices.size() > n && coordinateIndices[n] != -1)
				{
					renderingVertices.emplace_back(points[coordinateIndices[first]]);
					renderingVertices.emplace_back(points[coordinateIndices[n]]);
					renderingVertices.emplace_back(points[coordinateIndices[n - 1]]);
					++numberPolygonVertices;
					++n;
				}
			}

			if (hasNormals)
			{
				ocean_assert(normals);

				if (normalPerVertex_.value())
				{
					if (normalIndices.empty())
					{
						// If the normalIndex field is empty, the coordIndex field is used to choose normals from the X3DNormalNode node.
						// If the greatest index in the coordIndex field is N, then there shall be N+1 normals in the X3DNormalNode node.
						addNormalsPerVertex(renderingNormals, *normals, coordIndex_.values(), first, n);
					}
					else
					{
						// If the normalIndex field is not empty, normals are applied to each vertex of the IndexedFaceSet in exactly the same manner that the coordIndex field is used to choose coordinates for each vertex from the Coordinate node.
						// The normalIndex field shall contain at least as many indices as the coordIndex field, and shall contain end-of-face markers (-1) in exactly the same places as the coordIndex  field.
						// If the greatest index in the indexIndex field is N, then there shall be N+1 normals in the X3DNormalNode node.
						addNormalsPerVertex(renderingNormals, *normals, normalIndex_.values(), first, n);
					}
				}
				else // normal per face
				{
					// If the normalIndex field is empty, the normals in the X3DNormalNode node are applied to each face of the IndexedFaceSet in order.
					// There shall be at least as many normals in the X3DNormalNode node as there are faces.
					if (normalIndices.empty())
					{
						if (normals->size() < numberPolygons)
						{
							throw OceanException("Not enough defined normal values.");
						}

						renderingNormals.insert(renderingNormals.end(), 3 * (numberPolygonVertices - 2), (*normals)[numberPolygons]);
					}
					else
					{
						if (normalIndices.size() < numberPolygons)
						{
							throw OceanException("Not enough defined normal indices.");
						}

						const int normalIndexValue = normalIndices[numberPolygons];

						if (normalIndexValue < 0)
						{
							throw OceanException(std::string("Invalid normal index \"") + String::toAString(normalIndexValue) + std::string("\""));
						}

						if (normals->size() <= (unsigned int)(normalIndexValue))
						{
							throw OceanException(std::string("Invalid normal index \"") + String::toAString(normalIndexValue)
									+ std::string("\", there are only ") + String::toAString(normals->size()) + std::string(" normals defined."));
						}

						renderingNormals.insert(renderingNormals.end(), 3 * (numberPolygonVertices - 2), (*normals)[(unsigned int)(normalIndexValue)]);
					}
				}
			}
			// no normals
			else
			{
				unsigned int index1 = first + 1u;
				unsigned int index2 = first + 2u;

				if (ccw_.value() == false)
				{
					index1 = first + 2u;
					index2 = first + 1u;
				}

				const Rendering::Vertex& v0 = points[coordinateIndices[first]];
				const Rendering::Vertex& v1 = points[coordinateIndices[index1]];
				const Rendering::Vertex& v2 = points[coordinateIndices[index2]];

				Rendering::Normal normal((v1 - v0).cross(v2 - v0));

				if (normal.normalize())
				{
					renderingNormals.insert(renderingNormals.end(), 3 * (numberPolygonVertices - 2), normal);
				}
				else
				{
					Log::warning() << "IndexedFaceSet: Could not calculate a valid normal.";
					renderingNormals.insert(renderingNormals.end(), 3 * (numberPolygonVertices - 2), Rendering::Normal(0, 0, 1));
				}
			}

			if (hasColors)
			{
				ocean_assert(colors);

				if (colorPerVertex_.value())
				{
					if (colorIndices.empty())
					{
						// If the colorIndex field is empty, the coordIndex field is used to choose colors from the X3DColorNode node.
						// If the greatest index in the coordIndex field is N, then there shall be N+1 colors in the X3DColorNode node.
						useColors = addColorsPerVertex(renderingColors, *colors, coordIndex_.values(), first, n);
					}
					else
					{
						// If the colorIndex field is not empty, colors are applied to each vertex of the IndexedFaceSet in exactly the same manner that the coordIndex field is used to choose coordinates for each vertex from the Coordinate node.
						// The colorIndex field shall contain at least as many indices as the coordIndex field, and shall contain end-of-face markers (-1) in exactly the same places as the coordIndex  field.
						// If the greatest index in the indexIndex field is N, then there shall be N+1 colors in the X3DColorNode node.
						useColors = addColorsPerVertex(renderingColors, *colors, colorIndex_.values(), first, n);
					}
				}
				else // color per face
				{
					if (colorIndices.empty())
					{
						if (colors->size() < numberPolygons)
						{
							throw OceanException("Not enough defined color values.");
						}

						renderingColors.insert(renderingColors.end(), 3 * (numberPolygonVertices - 2), (*colors)[numberPolygons]);
					}
					else
					{
						if (colorIndices.size() < numberPolygons)
						{
							throw OceanException("Not enough defined color indices.");
						}

						const int colorIndexValue = colorIndices[numberPolygons];

						if (colorIndexValue < 0)
						{
							throw OceanException(std::string("Invalid color index \"") + String::toAString(colorIndexValue) + std::string("\""));
						}

						if (colors->size() <= (unsigned int)(colorIndexValue))
						{
							throw OceanException(std::string("Invalid color index \"") + String::toAString(colorIndexValue)
									+ std::string("\", there are only ") + String::toAString(colors->size()) + std::string(" colors defined."));
						}

						renderingColors.insert(renderingColors.end(), 3 * (numberPolygonVertices - 2), (*colors)[(unsigned int)(colorIndexValue)]);
					}
				}
			}

			if (hasTextureCoordinates)
			{
				ocean_assert(texCoords || phantomTexCoords);

				if (texCoordIndices.empty())
				{
					if (texCoords)
					{
						ocean_assert(!phantomTexCoords);
						useTextureCoordinates = addTextureCoordinatesPerVertex(renderingTextureCoordinates, *texCoords, coordIndex_.values(), first, n);
					}
					else
					{
						ocean_assert(!texCoords);
						useTextureCoordinates = addPhantomTextureCoordinatesPerVertex(renderingPhantomTextureCoordinates, *phantomTexCoords, coordIndex_.values(), first, n);
					}
				}
				else
				{
					if (texCoords)
					{
						ocean_assert(!phantomTexCoords);
						useTextureCoordinates = addTextureCoordinatesPerVertex(renderingTextureCoordinates, *texCoords, texCoordIndex_.values(), first, n);
					}
					else
					{
						ocean_assert(!texCoords);
						useTextureCoordinates = addPhantomTextureCoordinatesPerVertex(renderingPhantomTextureCoordinates, *phantomTexCoords, texCoordIndex_.values(), first, n);
					}
				}
			}

			++numberPolygons;
		}
	}
	catch(const Exception& exception)
	{
		Log::error() << exception.what();
		renderingObject_.release();
		return;
	}

	if (renderingVertices.empty())
	{
		renderingObject_.release();
		return;
	}

	Rendering::TriangleFaces faces;
	faces.reserve(renderingVertices.size() / 3);

	for (unsigned int n = 0u; n < renderingVertices.size(); n += 3u)
	{
		faces.emplace_back(n);
	}

	if (hasNormals == false && normalPerVertex_.value() && creaseAngle_.value() > 0)
	{
		Rendering::TriangleFace::calculateSmoothedPerVertexNormals(faces, renderingVertices, renderingNormals, creaseAngle_.value());
	}

	ocean_assert(renderingNormals.empty() || renderingNormals.size() == renderingVertices.size());
	ocean_assert(useTextureCoordinates == false || renderingTextureCoordinates.empty() || renderingTextureCoordinates.size() == renderingVertices.size());
	ocean_assert(useColors == false || renderingColors.empty() || renderingColors.size() == renderingVertices.size());

	renderingVertexSet_->setVertices(renderingVertices);
	renderingVertexSet_->setNormals(renderingNormals);

	if (useTextureCoordinates)
	{
		if (!renderingTextureCoordinates.empty())
		{
			renderingVertexSet_->setTextureCoordinates(renderingTextureCoordinates, 0);
		}
		else
		{
			try
			{
				renderingVertexSet_->setPhantomTextureCoordinates(renderingPhantomTextureCoordinates, 0);

				if (texCoord_.value()->hasField("reference"))
				{
					renderingVertexSet_->setPhantomTextureCoordinateSystem(texCoord_.value()->field<SingleString>("reference").value());
				}
			}
			catch(const Exception& exception)
			{
				Log::warning() << exception.what();
			}
		}
	}

	if (useColors || (colorPerVertex_.value() == false && renderingColors.size() == renderingVertices.size()))
	{
		renderingVertexSet_->setColors(renderingColors);
	}

	renderingTriangles->setVertexSet(renderingVertexSet_);
	renderingTriangles->setFaces(faces);
}

void IndexedFaceSet::addNormalsPerVertex(Rendering::Normals& renderingNormals, const MultiVector3::Values& normals, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex) const
{
	ocean_assert(normalPerVertex_.value());

	// has been checked before
	ocean_assert(endIndex <= indices.size());

	if (indices[endIndex] != -1)
	{
		throw OceanException("Vertex indices does not match with normal indices.");
	}

	if (ccw_.value())
	{
		ocean_assert(firstIndex + 3 <= endIndex);
		for (unsigned int i = firstIndex; i < firstIndex + 3; ++i)
		{
			const int index = indices[i];

			if (index < 0)
			{
				throw OceanException(std::string("Invalid normal index \"") + String::toAString(indices[i]) + std::string("\"."));
			}

			if (normals.size() <= (unsigned int)(index))
			{
				throw OceanException(std::string("Invalid normal index \"") + String::toAString(index)
					+ std::string("\", there are only ") + String::toAString(normals.size()) + std::string(" normals defined."));
			}

			renderingNormals.emplace_back(normals[(unsigned int)(index)]);
		}

		for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
		{
			const int index0 = indices[firstIndex];
			const int index1 = indices[i - 1];
			const int index2 = indices[i];

			if (index0 < 0 || index1 < 0 || index2 < 0)
			{
				throw OceanException(std::string("Invalid (negative) normal index."));
			}

			if (normals.size() <= (unsigned int)(index0) || normals.size() <= (unsigned int)(index1) || normals.size() <= (unsigned int)(index2))
			{
				throw OceanException(std::string("Invalid normal index, there are only ") + String::toAString(normals.size()) + std::string(" normals defined."));
			}

			renderingNormals.emplace_back(normals[(unsigned int)(index0)]);
			renderingNormals.emplace_back(normals[(unsigned int)(index1)]);
			renderingNormals.emplace_back(normals[(unsigned int)(index2)]);
		}
	}
	else // ccw.value() == false
	{
		ocean_assert(firstIndex + 3 <= endIndex);

		const int firstIndex0 = indices[firstIndex];
		const int firstIndex1 = indices[firstIndex + 2];
		const int firstIndex2 = indices[firstIndex + 1];

		if (firstIndex0 < 0 || firstIndex1 < 0 || firstIndex2 < 0)
		{
			throw OceanException(std::string("Invalid (negative) normal index."));
		}

		if (normals.size() <= (unsigned int)(firstIndex0) || normals.size() <= (unsigned int)(firstIndex1) || normals.size() <= (unsigned int)(firstIndex2))
		{
			throw OceanException(std::string("Invalid normal index, there are only ") + String::toAString(normals.size()) + std::string(" normals defined."));
		}

		renderingNormals.emplace_back(normals[(unsigned int)(firstIndex0)]);
		renderingNormals.emplace_back(normals[(unsigned int)(firstIndex1)]);
		renderingNormals.emplace_back(normals[(unsigned int)(firstIndex2)]);

		for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
		{
			const int index0 = indices[firstIndex];
			const int index1 = indices[i];
			const int index2 = indices[i - 1];

			if (index0 < 0 || index1 < 0 || index2 < 0)
			{
				throw OceanException(std::string("Invalid (negative) normal index."));
			}

			if (normals.size() <= (unsigned int)(index0) || normals.size() <= (unsigned int)(index1) || normals.size() <= (unsigned int)(index2))
			{
				throw OceanException(std::string("Invalid normal index, there are only ") + String::toAString(normals.size()) + std::string(" normals defined."));
			}

			renderingNormals.emplace_back(normals[(unsigned int)(index0)]);
			renderingNormals.emplace_back(normals[(unsigned int)(index1)]);
			renderingNormals.emplace_back(normals[(unsigned int)(index2)]);
		}
	}
}

bool IndexedFaceSet::addColorsPerVertex(RGBAColors& renderingColors, const MultiColor::Values& colors, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex)
{
	try
	{
		ocean_assert(colorPerVertex_.value());

		// has been checked before
		ocean_assert(endIndex <= indices.size());

		if (indices[endIndex] != -1)
		{
			throw OceanException("Vertex indices does not match with color indices.");
		}

		if (ccw_.value())
		{
			ocean_assert(firstIndex + 3 <= endIndex);
			for (unsigned int i = firstIndex; i < firstIndex + 3; ++i)
			{
				const int index = indices[i];

				if (index < 0)
				{
					throw OceanException(std::string("Invalid color index \"") + String::toAString(indices[i]) + std::string("\"."));
				}

				if (colors.size() <= (unsigned int)(index))
				{
					throw OceanException(std::string("Invalid color index \"") + String::toAString(index)
						+ std::string("\", there are only ") + String::toAString(colors.size()) + std::string(" colors defined."));
				}

				renderingColors.emplace_back(colors[(unsigned int)(index)]);
			}

			for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
			{
				const int index0 = indices[firstIndex];
				const int index1 = indices[i - 1];
				const int index2 = indices[i];

				if (index0 < 0 || index1 < 0 || index2 < 0)
				{
					throw OceanException(std::string("Invalid (negative) color index."));
				}

				if (colors.size() <= (unsigned int)(index0) || colors.size() <= (unsigned int)(index1) || colors.size() <= (unsigned int)(index2))
				{
					throw OceanException(std::string("Invalid color index, there are only ") + String::toAString(colors.size()) + std::string(" colors defined."));
				}

				renderingColors.emplace_back(colors[(unsigned int)(index0)]);
				renderingColors.emplace_back(colors[(unsigned int)(index1)]);
				renderingColors.emplace_back(colors[(unsigned int)(index2)]);
			}
		}
		else // ccw.value() == false
		{
			ocean_assert(firstIndex + 3 <= endIndex);

			const int firstIndex0 = indices[firstIndex];
			const int firstIndex1 = indices[firstIndex + 2];
			const int firstIndex2 = indices[firstIndex + 1];

			if (firstIndex0 < 0 || firstIndex1 < 0 || firstIndex2 < 0)
			{
				throw OceanException(std::string("Invalid (negative) color index."));
			}

			if (colors.size() <= (unsigned int)(firstIndex0) || colors.size() <= (unsigned int)(firstIndex1) || colors.size() <= (unsigned int)(firstIndex2))
			{
				throw OceanException(std::string("Invalid color index, there are only ") + String::toAString(colors.size()) + std::string(" colors defined."));
			}

			renderingColors.emplace_back(colors[(unsigned int)(firstIndex0)]);
			renderingColors.emplace_back(colors[(unsigned int)(firstIndex1)]);
			renderingColors.emplace_back(colors[(unsigned int)(firstIndex2)]);

			for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
			{
				const int index0 = indices[firstIndex];
				const int index1 = indices[i];
				const int index2 = indices[i - 1];

				if (index0 < 0 || index1 < 0 || index2 < 0)
				{
					throw OceanException(std::string("Invalid (negative) color index."));
				}

				if (colors.size() <= (unsigned int)(index0) || colors.size() <= (unsigned int)(index1) || colors.size() <= (unsigned int)(index2))
				{
					throw OceanException(std::string("Invalid color index, there are only ") + String::toAString(colors.size()) + std::string(" colors defined."));
				}

				renderingColors.emplace_back(colors[(unsigned int)(index0)]);
				renderingColors.emplace_back(colors[(unsigned int)(index1)]);
				renderingColors.emplace_back(colors[(unsigned int)(index2)]);
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
		renderingColors.clear();

		return false;
	}

	return true;
}

bool IndexedFaceSet::addTextureCoordinatesPerVertex(Rendering::TextureCoordinates& renderingTextureCoordinates, const MultiVector2::Values& textureCoordinates, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex)
{
	try
	{
		// has been checked before
		ocean_assert(endIndex <= indices.size());

		if (indices.size() > endIndex && indices[endIndex] != -1)
		{
			throw OceanException("Vertex indices does not match with texture coordinate indices.");
		}

		if (ccw_.value())
		{
			ocean_assert(firstIndex + 3 <= endIndex);
			for (unsigned int i = firstIndex; i < firstIndex + 3; ++i)
			{
				const int index = indices[i];

				if (index < 0)
				{
					throw OceanException(std::string("Invalid texture coordinate index \"") + String::toAString(indices[i]) + std::string("\"."));
				}

				if (textureCoordinates.size() <= (unsigned int)(index))
				{
					throw OceanException(std::string("Invalid texture coordinates index \"") + String::toAString(index)
						+ std::string("\", there are only ") + String::toAString(textureCoordinates.size()) + std::string(" texture coordinates defined."));
				}

				renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(index)]);
			}

			for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
			{
				const int index0 = indices[firstIndex];
				const int index1 = indices[i - 1];
				const int index2 = indices[i];

				if (index0 < 0 || index1 < 0 || index2 < 0)
				{
					throw OceanException(std::string("Invalid (negative) texture coordinate index."));
				}

				if (textureCoordinates.size() <= (unsigned int)(index0) || textureCoordinates.size() <= (unsigned int)(index1) || textureCoordinates.size() <= (unsigned int)(index2))
				{
					throw OceanException(std::string("Invalid texture coordinate index, there are only ") + String::toAString(textureCoordinates.size()) + std::string(" textureCoordinates defined."));
				}

				renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(index0)]);
				renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(index1)]);
				renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(index2)]);
			}
		}
		else // ccw.value() == false
		{
			ocean_assert(firstIndex + 3 <= endIndex);

			const int firstIndex0 = indices[firstIndex];
			const int firstIndex1 = indices[firstIndex + 2];
			const int firstIndex2 = indices[firstIndex + 1];

			if (firstIndex0 < 0 ||firstIndex1 < 0 || firstIndex2 < 0)
			{
				throw OceanException(std::string("Invalid (negative) texture coordinate index."));
			}

			if (textureCoordinates.size() <= (unsigned int)(firstIndex0) || textureCoordinates.size() <= (unsigned int)(firstIndex1) || textureCoordinates.size() <= (unsigned int)(firstIndex2))
			{
				throw OceanException(std::string("Invalid texture coordinate index, there are only ") + String::toAString(textureCoordinates.size()) + std::string(" texture coordinates defined."));
			}

			renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(firstIndex0)]);
			renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(firstIndex1)]);
			renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(firstIndex2)]);

			for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
			{
				const int index0 = indices[firstIndex];
				const int index1 = indices[i];
				const int index2 = indices[i - 1];

				if (index0 < 0 || index1 < 0 || index2 < 0)
				{
					throw OceanException(std::string("Invalid (negative) texture coordinate index."));
				}

				if (textureCoordinates.size() <= (unsigned int)(index0) || textureCoordinates.size() <= (unsigned int)(index1) || textureCoordinates.size() <= (unsigned int)(index2))
				{
					throw OceanException(std::string("Invalid texture coordinate index, there are only ") + String::toAString(textureCoordinates.size()) + std::string(" texture coordinates defined."));
				}

				renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(index0)]);
				renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(index1)]);
				renderingTextureCoordinates.emplace_back(textureCoordinates[(unsigned int)(index2)]);
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
		renderingTextureCoordinates.clear();

		return false;
	}

	return true;
}

bool IndexedFaceSet::addPhantomTextureCoordinatesPerVertex(Rendering::Vertices& renderingPhantomTextureCoordinates, const MultiVector3::Values& phantomTextureCoordinates, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex)
{
	try
	{
		// has been checked before
		ocean_assert(endIndex <= indices.size());

		if (indices.size() > endIndex && indices[endIndex] != -1)
		{
			throw OceanException("Vertex indices does not match with phantom texture coordinate indices.");
		}

		if (ccw_.value())
		{
			ocean_assert(firstIndex + 3 <= endIndex);
			for (unsigned int i = firstIndex; i < firstIndex + 3; ++i)
			{
				const int index = indices[i];

				if (index < 0)
				{
					throw OceanException(std::string("Invalid phantom texture coordinate index \"") + String::toAString(indices[i]) + std::string("\"."));
				}

				if (phantomTextureCoordinates.size() <= (unsigned int)(index))
				{
					throw OceanException(std::string("Invalid texture coordinates index \"") + String::toAString(index)
						+ std::string("\", there are only ") + String::toAString(phantomTextureCoordinates.size()) + std::string(" phantom texture coordinates defined."));
				}

				renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(index)]);
			}

			for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
			{
				const int index0 = indices[firstIndex];
				const int index1 = indices[i - 1];
				const int index2 = indices[i];

				if (index0 < 0 || index1 < 0 || index2 < 0)
				{
					throw OceanException(std::string("Invalid (negative) phantom texture coordinate index."));
				}

				if (phantomTextureCoordinates.size() <= (unsigned int)(index0) || phantomTextureCoordinates.size() <= (unsigned int)(index1) || phantomTextureCoordinates.size() <= (unsigned int)(index2))
				{
					throw OceanException(std::string("Invalid phantom texture coordinate index, there are only ") + String::toAString(phantomTextureCoordinates.size()) + std::string(" phantom texture coordinates defined."));
				}

				renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(index0)]);
				renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(index1)]);
				renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(index2)]);
			}
		}
		else // ccw.value() == false
		{
			ocean_assert(firstIndex + 3 <= endIndex);

			const int firstIndex0 = indices[firstIndex];
			const int firstIndex1 = indices[firstIndex + 2];
			const int firstIndex2 = indices[firstIndex + 1];

			if (firstIndex0 < 0 || firstIndex1 < 0 || firstIndex2 < 0)
			{
				throw OceanException(std::string("Invalid (negative) texture coordinate index."));
			}

			if (phantomTextureCoordinates.size() <= (unsigned int)(firstIndex0) || phantomTextureCoordinates.size() <= (unsigned int)(firstIndex1) || phantomTextureCoordinates.size() <= (unsigned int)(firstIndex2))
			{
				throw OceanException(std::string("Invalid phantom texture coordinate index, there are only ") + String::toAString(phantomTextureCoordinates.size()) + std::string(" phantom texture coordinates defined."));
			}

			renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(firstIndex0)]);
			renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(firstIndex1)]);
			renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(firstIndex2)]);

			for (unsigned int i = firstIndex + 3; i < endIndex; ++i)
			{
				const int index0 = indices[firstIndex];
				const int index1 = indices[i];
				const int index2 = indices[i - 1];

				if (index0 < 0 || index1 < 0 || index2 < 0)
				{
					throw OceanException(std::string("Invalid (negative) texture coordinate index."));
				}

				if (phantomTextureCoordinates.size() <= (unsigned int)(index0) || phantomTextureCoordinates.size() <= (unsigned int)(index1) || phantomTextureCoordinates.size() <= (unsigned int)(index2))
				{
					throw OceanException(std::string("Invalid phantom texture coordinate index, there are only ") + String::toAString(phantomTextureCoordinates.size()) + std::string(" phantom texture coordinates defined."));
				}

				renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(index0)]);
				renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(index1)]);
				renderingPhantomTextureCoordinates.emplace_back(phantomTextureCoordinates[(unsigned int)(index2)]);
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
		renderingPhantomTextureCoordinates.clear();

		return false;
	}

	return true;
}

size_t IndexedFaceSet::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
