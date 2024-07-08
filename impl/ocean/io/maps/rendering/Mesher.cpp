/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/maps/rendering/Mesher.h"

#include "ocean/math/Line2.h"

#include "ocean/cv/segmentation/PixelContour.h"
#include "ocean/cv/segmentation/Triangulation.h"

namespace Ocean
{

namespace IO
{

namespace Maps
{

namespace Rendering
{

using namespace CV::Segmentation;

Mesher::Mesh Mesher::createBuildings(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const bool volumetric, const Scalar groundPlaneOffset)
{
	Vectors3 vertices;
	vertices.reserve(objects.size() * 40);

	Vectors3 normals;
	normals.reserve(vertices.capacity());

	Indices32 triangleFaces;
	triangleFaces.reserve(vertices.capacity());

	for (const Basemap::SharedObject& object : objects)
	{
		ocean_assert(object);

		const CV::PixelPositionI origin(object->layerExtent() / 2, object->layerExtent() / 2);

		const Scalar pixelPositionNormalization = Scalar(targetSize) / Scalar(object->layerExtent());

		const Scalar metricNormalization = Scalar(double(targetSize) / titleMetricExtent);

		switch (object->objectType())
		{
			case Basemap::Object::OT_BUILDING:
			{
				const Basemap::Building* building = reinterpret_cast<Basemap::Building*>(object.get());
				ocean_assert(building != nullptr);

				addBuilding(*building, origin, pixelPositionNormalization, metricNormalization, vertices, normals, triangleFaces, volumetric, groundPlaneOffset);
				break;
			}

			default:
				break;
		}
	}

	if (triangleFaces.empty())
	{
		return Mesh();
	}

	return Mesh(std::move(vertices), std::move(normals), std::move(triangleFaces));
}

Mesher::Mesh Mesher::createBuilding(const Basemap::Building& building, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, const bool volumetric, const Scalar groundPlaneOffset)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	const Basemap::PixelPositionGroupsI& outerPolygons = building.outerPolygons();

	if (outerPolygons.empty())
	{
		return Mesh();
	}

	const Scalar normalizedHeight = volumetric ? std::max(Scalar(0), building.height()) * metricNormalization : Scalar(0);
	const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

	Vectors3 vertices;
	vertices.reserve(outerPolygons.size() * 20);

	Vectors3 normals;
	normals.reserve(vertices.capacity());

	Indices32 triangleFaces;
	triangleFaces.reserve(outerPolygons.size() * 30);

	for (const Basemap::PixelPositionsI& outerPolygon : outerPolygons)
	{
		addPolygon(origin, outerPolygon, pixelPositionNormalization, normalizedHeight, vertices, normals, triangleFaces, normalizedGroundPlaneOffset);
	}

	return Mesh(std::move(vertices), std::move(normals), std::move(triangleFaces));
}

Mesher::Mesh Mesher::createRoads(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset, const Basemap::Road::RoadWidthMap& roadWidthMap)
{
	Vectors3 vertices;
	vertices.reserve(objects.size() * 40);

	Vectors3 normals;
	normals.reserve(vertices.capacity());

	Indices32 triangleFaces;
	triangleFaces.reserve(vertices.capacity());

	for (const Basemap::SharedObject& object : objects)
	{
		ocean_assert(object);

		const CV::PixelPositionI origin(object->layerExtent() / 2, object->layerExtent() / 2);

		const Scalar pixelPositionNormalization = Scalar(targetSize) / Scalar(object->layerExtent());

		const Scalar metricNormalization = Scalar(double(targetSize) / titleMetricExtent);

		const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

		switch (object->objectType())
		{
			case Basemap::Object::OT_ROAD:
			{
				const Basemap::Road* road = reinterpret_cast<Basemap::Road*>(object.get());
				ocean_assert(road != nullptr);

				addRoad(*road, origin, pixelPositionNormalization, metricNormalization, vertices, normals, triangleFaces, normalizedGroundPlaneOffset, roadWidthMap);
				break;
			}

			default:
				break;
		}
	}

	if (triangleFaces.empty())
	{
		return Mesh();
	}

	return Mesh(std::move(vertices), std::move(normals), std::move(triangleFaces));
}

Mesher::Mesh Mesher::createTransits(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset)
{
	Vectors3 vertices;
	vertices.reserve(objects.size() * 40);

	Vectors3 normals;
	normals.reserve(vertices.capacity());

	Indices32 triangleFaces;
	triangleFaces.reserve(vertices.capacity());

	for (const Basemap::SharedObject& object : objects)
	{
		ocean_assert(object);

		const CV::PixelPositionI origin(object->layerExtent() / 2, object->layerExtent() / 2);

		const Scalar pixelPositionNormalization = Scalar(targetSize) / Scalar(object->layerExtent());

		const Scalar metricNormalization = Scalar(double(targetSize) / titleMetricExtent);

		const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

		switch (object->objectType())
		{
			case Basemap::Object::OT_TRANSIT:
			{
				const Basemap::Transit* transit = reinterpret_cast<Basemap::Transit*>(object.get());
				ocean_assert(transit != nullptr);

				addTransit(*transit, origin, pixelPositionNormalization, metricNormalization, vertices, normals, triangleFaces, normalizedGroundPlaneOffset);
				break;
			}

			default:
				break;
		}
	}

	if (triangleFaces.empty())
	{
		return Mesh();
	}

	return Mesh(std::move(vertices), std::move(normals), std::move(triangleFaces));
}

Mesher::Mesh Mesher::createWaters(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset)
{
	Vectors3 vertices;
	vertices.reserve(objects.size() * 40);

	Vectors3 normals;
	normals.reserve(vertices.capacity());

	Indices32 triangleFaces;
	triangleFaces.reserve(vertices.capacity());

	for (const Basemap::SharedObject& object : objects)
	{
		ocean_assert(object);

		const CV::PixelPositionI origin(object->layerExtent() / 2, object->layerExtent() / 2);

		const Scalar pixelPositionNormalization = Scalar(targetSize) / Scalar(object->layerExtent());

		const Scalar metricNormalization = Scalar(double(targetSize) / titleMetricExtent);

		switch (object->objectType())
		{
			case Basemap::Object::OT_WATER:
			{
				const Basemap::Water* water = reinterpret_cast<Basemap::Water*>(object.get());
				ocean_assert(water != nullptr);

				addWater(*water, origin, pixelPositionNormalization, metricNormalization, vertices, normals, triangleFaces, groundPlaneOffset);
				break;
			}

			default:
				break;
		}
	}

	if (triangleFaces.empty())
	{
		return Mesh();
	}

	return Mesh(std::move(vertices), std::move(normals), std::move(triangleFaces));
}

Mesher::Mesh Mesher::createLandUses(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset)
{
	Vectors3 vertices;
	vertices.reserve(objects.size() * 40);

	Vectors3 normals;
	normals.reserve(vertices.capacity());

	Indices32 triangleFaces;
	triangleFaces.reserve(vertices.capacity());

	for (const Basemap::SharedObject& object : objects)
	{
		ocean_assert(object);

		const CV::PixelPositionI origin(object->layerExtent() / 2, object->layerExtent() / 2);

		const Scalar pixelPositionNormalization = Scalar(targetSize) / Scalar(object->layerExtent());

		const Scalar metricNormalization = Scalar(double(targetSize) / titleMetricExtent);

		switch (object->objectType())
		{
			case Basemap::Object::OT_LAND_USE:
			{
				const Basemap::LandUse* landUse = reinterpret_cast<Basemap::LandUse*>(object.get());
				ocean_assert(landUse != nullptr);

				addLandUse(*landUse, origin, pixelPositionNormalization, metricNormalization, vertices, normals, triangleFaces, groundPlaneOffset);
				break;
			}

			default:
				break;
		}
	}

	if (triangleFaces.empty())
	{
		return Mesh();
	}

	return Mesh(std::move(vertices), std::move(normals), std::move(triangleFaces));
}

Mesher::Mesh Mesher::createLandCovers(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset)
{
	Vectors3 vertices;
	vertices.reserve(objects.size() * 40);

	Vectors3 normals;
	normals.reserve(vertices.capacity());

	Indices32 triangleFaces;
	triangleFaces.reserve(vertices.capacity());

	for (const Basemap::SharedObject& object : objects)
	{
		ocean_assert(object);

		const CV::PixelPositionI origin(object->layerExtent() / 2, object->layerExtent() / 2);

		const Scalar pixelPositionNormalization = Scalar(targetSize) / Scalar(object->layerExtent());

		const Scalar metricNormalization = Scalar(double(targetSize) / titleMetricExtent);

		switch (object->objectType())
		{
			case Basemap::Object::OT_LAND_COVER:
			{
				const Basemap::LandCover* landCover = reinterpret_cast<Basemap::LandCover*>(object.get());
				ocean_assert(landCover != nullptr);

				addLandCover(*landCover, origin, pixelPositionNormalization, metricNormalization, vertices, normals, triangleFaces, groundPlaneOffset);
				break;
			}

			default:
				break;
		}
	}

	if (triangleFaces.empty())
	{
		return Mesh();
	}

	return Mesh(std::move(vertices), std::move(normals), std::move(triangleFaces));
}

bool Mesher::addBuilding(const Basemap::Building& building, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const bool volumetric, const Scalar groundPlaneOffset)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	ocean_assert(vertices.size() == normals.size());

	bool allSucceeded = true;

	const Scalar normalizedHeight = volumetric ? std::max(Scalar(0), building.height()) * metricNormalization : Scalar(0);
	const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

	for (const Basemap::PixelPositionsI& outerPolygon : building.outerPolygons())
	{
		if (!addPolygon(origin, outerPolygon, pixelPositionNormalization, normalizedHeight, vertices, normals, triangleFaces, normalizedGroundPlaneOffset))
		{
			allSucceeded = false;
		}
	}

	ocean_assert(vertices.size() == normals.size());

	return allSucceeded;
}

bool Mesher::addRoad(const Basemap::Road& road, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset, const Basemap::Road::RoadWidthMap& roadWidthMap)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	ocean_assert(vertices.size() == normals.size());

	bool allSucceeded = true;

	const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

	for (const Basemap::PixelPositionsI& lineString : road.lineStrings())
	{
		Basemap::Road::RoadWidthMap::const_iterator iRoad = roadWidthMap.find(road.roadType());

		if (iRoad == roadWidthMap.cend())
		{
			iRoad = roadWidthMap.find(Basemap::Road::RT_END);
			ocean_assert(iRoad != roadWidthMap.cend() && "RT_END needs to be defined for default roads");
		}

		ocean_assert(iRoad != roadWidthMap.cend());
		if (iRoad != roadWidthMap.cend())
		{
			ocean_assert(iRoad->second > 0.0f);

			const Scalar normalizedWidth = Scalar(iRoad->second) * metricNormalization;

			if (!addLineString(origin, lineString, pixelPositionNormalization, normalizedWidth, vertices, normals, triangleFaces, normalizedGroundPlaneOffset))
			{
				allSucceeded = false;
			}
		}
	}

	ocean_assert(vertices.size() == normals.size());

	return allSucceeded;
}

bool Mesher::addTransit(const Basemap::Transit& transit, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	ocean_assert(vertices.size() == normals.size());

	bool allSucceeded = true;

	const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

	for (const Basemap::PixelPositionsI& lineString : transit.lineStrings())
	{
		Scalar normalizedWidth = Scalar(10) * metricNormalization; // 10 meter

		switch (transit.transitType())
		{
			case Basemap::Transit::TT_RAILWAY:
				normalizedWidth = Scalar(5) * metricNormalization; // 5 meter
				break;

			default:
				break;
		}

		if (!addLineString(origin, lineString, pixelPositionNormalization, normalizedWidth, vertices, normals, triangleFaces, normalizedGroundPlaneOffset))
		{
			allSucceeded = false;
		}
	}

	ocean_assert(vertices.size() == normals.size());

	return allSucceeded;
}

bool Mesher::addWater(const Basemap::Water& water, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	ocean_assert(vertices.size() == normals.size());

	bool allSucceeded = true;

	const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

	for (const Basemap::PixelPositionsI& outerPolygon : water.outerPolygons())
	{
		if (!addPolygon(origin, outerPolygon, pixelPositionNormalization, 0, vertices, normals, triangleFaces, normalizedGroundPlaneOffset))
		{
			allSucceeded = false;
		}
	}

	ocean_assert(vertices.size() == normals.size());

	return allSucceeded;
}

bool Mesher::addLandUse(const Basemap::LandUse& landUse, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	ocean_assert(vertices.size() == normals.size());

	bool allSucceeded = true;

	const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

	for (const Basemap::PixelPositionsI& outerPolygon : landUse.outerPolygons())
	{
		if (!addPolygon(origin, outerPolygon, pixelPositionNormalization, 0, vertices, normals, triangleFaces, normalizedGroundPlaneOffset))
		{
			allSucceeded = false;
		}
	}

	ocean_assert(vertices.size() == normals.size());

	return allSucceeded;
}

bool Mesher::addLandCover(const Basemap::LandCover& landCover, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	ocean_assert(vertices.size() == normals.size());

	bool allSucceeded = true;

	const Scalar normalizedGroundPlaneOffset = groundPlaneOffset * metricNormalization;

	for (const Basemap::PixelPositionsI& outerPolygon : landCover.outerPolygons())
	{
		if (!addPolygon(origin, outerPolygon, pixelPositionNormalization, 0, vertices, normals, triangleFaces, normalizedGroundPlaneOffset))
		{
			allSucceeded = false;
		}
	}

	ocean_assert(vertices.size() == normals.size());

	return allSucceeded;
}

bool Mesher::addPolygon(const CV::PixelPositionI& origin, const CV::PixelPositionsI& outerPolygon, const Scalar pixelPositionNormalization, const Scalar normalizedHeight, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar normalizedGroundPlaneOffset)
{
	ocean_assert(!outerPolygon.empty());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(normalizedHeight >= 0);

	ocean_assert(vertices.size() == normals.size());

	Index32 vertexStartIndex = Index32(vertices.size());

	const PixelContourI pixelContour(outerPolygon);
	const Triangulation::IndexTriangles indexTriangles = Triangulation::triangulate(pixelContour);

	if (indexTriangles.empty())
	{
		return false;
	}

	// first, we keep all vertices in pixel space to ensure that we can determine a valid normal (with 32 bit floats)

	// top faces

	for (const CV::PixelPositionI& pixelPosition : outerPolygon)
	{
		const Vector2 vertex = (pixelPosition - origin).vector();

		vertices.emplace_back(vertex.x(), normalizedGroundPlaneOffset + normalizedHeight, vertex.y());
	}

	for (const Triangulation::IndexTriangle& indexTriangle : indexTriangles)
	{
		const CV::PixelPositionI direction01(outerPolygon[indexTriangle[1]] - outerPolygon[indexTriangle[0]]);
		const CV::PixelPositionI direction02(outerPolygon[indexTriangle[2]] - outerPolygon[indexTriangle[0]]);

		const int crossProduct = direction01.x() * direction02.y() - direction01.y() * direction02.x();

		if (crossProduct < 0)
		{
			triangleFaces.emplace_back(vertexStartIndex + indexTriangle[0u]);
			triangleFaces.emplace_back(vertexStartIndex + indexTriangle[1u]);
			triangleFaces.emplace_back(vertexStartIndex + indexTriangle[2u]);
		}
		else
		{
			triangleFaces.emplace_back(vertexStartIndex + indexTriangle[0u]);
			triangleFaces.emplace_back(vertexStartIndex + indexTriangle[2u]);
			triangleFaces.emplace_back(vertexStartIndex + indexTriangle[1u]);
		}
	}

	normals.insert(normals.cend(), outerPolygon.size(), Vector3(0, 1, 0));

	if (normalizedHeight > Numeric::eps())
	{
		// side faces

		for (Index32 nPlus = 0u; nPlus < outerPolygon.size(); ++nPlus)
		{
			// we need four vertices for each side face, as each face has an own normal

			const Index32 nMinus = Index32(Ocean::modulo(int(nPlus) - 1, int(outerPolygon.size())));
			ocean_assert(nMinus < outerPolygon.size());

			const Vector3 vertexRoofMinus = vertices[vertexStartIndex + nMinus];
			const Vector3 vertexRoofPlus = vertices[vertexStartIndex + nPlus];

			const Vector3 vertexFloorMinus = Vector3(vertexRoofMinus.x(), normalizedGroundPlaneOffset, vertexRoofMinus.z());
			const Vector3 vertexFloorPlus = Vector3(vertexRoofPlus.x(), normalizedGroundPlaneOffset, vertexRoofPlus.z());

			const Index32 currentVertexIndex = Index32(vertices.size());

			vertices.emplace_back(vertexRoofMinus);
			vertices.emplace_back(vertexRoofPlus);
			vertices.emplace_back(vertexFloorMinus);
			vertices.emplace_back(vertexFloorPlus);

			Vector3 normal;

			if (pixelContour.isCounterClockwise())
			{
				triangleFaces.emplace_back(currentVertexIndex + 0u);
				triangleFaces.emplace_back(currentVertexIndex + 2u);
				triangleFaces.emplace_back(currentVertexIndex + 3u);

				triangleFaces.emplace_back(currentVertexIndex + 0u);
				triangleFaces.emplace_back(currentVertexIndex + 2u);
				triangleFaces.emplace_back(currentVertexIndex + 1u);

				normal = Vector3(0, -1, 0).cross(vertexRoofPlus - vertexRoofMinus).normalizedOrZero();
				ocean_assert(normal.isUnit());
			}
			else
			{
				triangleFaces.emplace_back(currentVertexIndex + 1u);
				triangleFaces.emplace_back(currentVertexIndex + 3u);
				triangleFaces.emplace_back(currentVertexIndex + 2u);

				triangleFaces.emplace_back(currentVertexIndex + 1u);
				triangleFaces.emplace_back(currentVertexIndex + 2u);
				triangleFaces.emplace_back(currentVertexIndex + 0u);

				normal = Vector3(0, -1, 0).cross(vertexRoofMinus - vertexRoofPlus).normalizedOrZero();
				ocean_assert(normal.isUnit());
			}

			normals.insert(normals.cend(), 4, normal);
		}
	}

	// now, we convert the vertices from pixel to normalized space

	for (Index32 nVertex = vertexStartIndex; nVertex < Index32(vertices.size()); ++nVertex)
	{
		Vector3& vertex = vertices[nVertex];

		vertex = Vector3(vertex.x() * pixelPositionNormalization, vertex.y(), vertex.z() * pixelPositionNormalization);
	}

	ocean_assert(vertices.size() == normals.size());

	return true;
}

bool Mesher::addLineString(const CV::PixelPositionI& origin, const CV::PixelPositionsI& lineString, const Scalar pixelPositionNormalization, const Scalar normalizedWidth, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, Scalar normalizedGroundPlaneOffset)
{
	ocean_assert(lineString.size() >= 2);
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(normalizedWidth > Numeric::eps());

	if (lineString.size() < 2)
	{
		return false;
	}

	/*
	 * The line string needs to be expected with the width.
	 * We determine the actual vertex positions by intersecting the borders of consecutive segments.
	 * Line string positions are depected by X
	 * The left/right vertices are depectied by L/R
	 *
	 *  L--------------------------------L
	 *  |
	 *  |    X    - - - - - - - - - - -  X
	 *  |    |
	 *  |         R----------------------R
	 *  |    |    |
	 *  |         |
	 *  |    |    |
	 *  |         |
	 *  |    |    |
	 *  L    X    R
	 */

	ocean_assert(pixelPositionNormalization > 0);

	const Scalar width_2 = (normalizedWidth / pixelPositionNormalization) * Scalar(0.5);

	ocean_assert(vertices.size() == normals.size());

	Index32 currentVertexIndex = Index32(vertices.size());

	const Vector2 firstPosition = (lineString.front() - origin).vector();
	Vector2 currentPosition = (lineString[1] - origin).vector();

	const Vector2 direction = (currentPosition - firstPosition).normalizedOrZero();
	ocean_assert(direction.isUnit());

	Vector2 perpendicular = direction.perpendicular();
	ocean_assert(perpendicular.isUnit());

	Vector2 firstPositionMinus = firstPosition - perpendicular * width_2;
	Vector2 firstPositionPlus = firstPosition + perpendicular * width_2;

	Line2 borderMinus(firstPositionMinus, direction);
	Line2 borderPlus(firstPositionPlus, direction);

	if (lineString.front() == lineString.back())
	{
		if (lineString.size() == 2)
		{
			return false;
		}

		// we have a loop

		const Vector2 lastPosition = (lineString[lineString.size() - 2] - origin).vector();

		const Vector2 lastDirection = (firstPosition - lastPosition).normalizedOrZero();
		ocean_assert(lastDirection.isUnit());

		const Vector2 lastPerpendicular = lastDirection.perpendicular();
		ocean_assert(lastPerpendicular.isUnit());

		const Vector2 lastPositionMinus = lastPosition - lastPerpendicular * width_2;
		const Vector2 lastPositionPlus = lastPosition + lastPerpendicular * width_2;

		const Line2 lastBorderMinus(lastPositionMinus, lastDirection);
		const Line2 lastBorderPlus(lastPositionPlus, lastDirection);

		borderMinus.intersection(lastBorderMinus, firstPositionMinus); // updating currentPositionMinus to the intersection if not parallel
		borderPlus.intersection(lastBorderPlus, firstPositionPlus);
	}

	vertices.emplace_back(firstPositionMinus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, firstPositionMinus.y() * pixelPositionNormalization); // the two start vertices
	vertices.emplace_back(firstPositionPlus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, firstPositionPlus.y() * pixelPositionNormalization);

	for (size_t n = 1; n < lineString.size() - 1; ++n)
	{
		ocean_assert(n + 1 < lineString.size());
		const Vector2 nextPosition = (lineString[n + 1] - origin).vector();

		const Vector2 nextDirection = (nextPosition - currentPosition).normalizedOrZero();
		ocean_assert(nextDirection.isUnit());

		perpendicular = nextDirection.perpendicular();
		ocean_assert(perpendicular.isUnit());

		Vector2 currentPositionMinus = currentPosition - perpendicular * width_2;
		Vector2 currentPositionPlus = currentPosition + perpendicular * width_2;

		const Line2 nextBorderMinus(currentPositionMinus, nextDirection);
		const Line2 nextBorderPlus(currentPositionPlus, nextDirection);

		borderMinus.intersection(nextBorderMinus, currentPositionMinus); // updating currentPositionMinus to the intersection if not parallel
		borderPlus.intersection(nextBorderPlus, currentPositionPlus);

		vertices.emplace_back(currentPositionMinus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, currentPositionMinus.y() * pixelPositionNormalization); // the two intermediate vertices
		vertices.emplace_back(currentPositionPlus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, currentPositionPlus.y() * pixelPositionNormalization);

		triangleFaces.emplace_back(currentVertexIndex + 0u);
		triangleFaces.emplace_back(currentVertexIndex + 1u);
		triangleFaces.emplace_back(currentVertexIndex + 3u);

		triangleFaces.emplace_back(currentVertexIndex + 0u);
		triangleFaces.emplace_back(currentVertexIndex + 3u);
		triangleFaces.emplace_back(currentVertexIndex + 2u);

		currentVertexIndex += 2u;

		currentPosition = nextPosition;

		borderMinus = nextBorderMinus;
		borderPlus = nextBorderPlus;
	}

	if (lineString.front() == lineString.back())
	{
		vertices.emplace_back(firstPositionMinus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, firstPositionMinus.y() * pixelPositionNormalization); // the two end vertices
		vertices.emplace_back(firstPositionPlus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, firstPositionPlus.y() * pixelPositionNormalization);
	}
	else
	{
		const Vector2 lastPosition = (lineString.back() - origin).vector();

		const Vector2 lastPositionMinus = lastPosition - perpendicular * width_2;
		const Vector2 lastPositionPlus = lastPosition + perpendicular * width_2;

		vertices.emplace_back(lastPositionMinus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, lastPositionMinus.y() * pixelPositionNormalization); // the two end vertices
		vertices.emplace_back(lastPositionPlus.x() * pixelPositionNormalization, normalizedGroundPlaneOffset, lastPositionPlus.y() * pixelPositionNormalization);
	}

	triangleFaces.emplace_back(currentVertexIndex + 0u);
	triangleFaces.emplace_back(currentVertexIndex + 1u);
	triangleFaces.emplace_back(currentVertexIndex + 3u);

	triangleFaces.emplace_back(currentVertexIndex + 0u);
	triangleFaces.emplace_back(currentVertexIndex + 3u);
	triangleFaces.emplace_back(currentVertexIndex + 2u);

	normals.insert(normals.cend(), lineString.size() * 2, Vector3(0, 1, 0));
	ocean_assert(vertices.size() == normals.size());

	return true;
}

}

}

}

}
