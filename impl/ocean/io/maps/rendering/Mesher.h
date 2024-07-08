/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_MAPS_RENDERING_MESHER_H
#define META_OCEAN_IO_MAPS_RENDERING_MESHER_H

#include "ocean/io/maps/rendering/Rendering.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/io/maps/Basemap.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace IO
{

namespace Maps
{

namespace Rendering
{

/**
 * This class implements functions to creates meshes from map objects.
 * @ingroup iomapsrendering
 */
class OCEAN_IO_MAPS_RENDERING_EXPORT Mesher
{
	public:

		/**
		 * This class defines a mesh.
		 */
		class Mesh
		{
			public:

				/**
				 * Default constructor.
				 */
				Mesh() = default;

				/**
				 * Creates a new mesh object.
				 * @param vertices The vertices of the mesh
				 * @param perVertexNormals The per-vertex normals of the mesh
				 * @param triangleFaces The triangle faces of the mesh, three consecutive indices define one triangle
				 */
				inline Mesh(Vectors3&& vertices, Vectors3&& perVertexNormals, Indices32&& triangleFaces);

				/**
				 * Returns the vertices of this mesh.
				 * @return The mesh's vertices
				 */
				inline const Vectors3& vertices() const;

				/**
				 * Returns the per-vertex normals of this mesh.
				 * @return The mesh's per-vertex normals
				 */
				inline const Vectors3& perVertexNormals() const;

				/**
				 * Returns the triangle faces of this mesh.
				 * @return The mesh's triangle faces, three consecutive indices define one triangle
				 */
				inline const Indices32& triangleFaces() const;

				/**
				 * Returns whether this mesh holds at least one valid triangle.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The vertices of the mesh.
				Vectors3 vertices_;

				/// The per-vertex normals of the mesh.
				Vectors3 perVertexNormals_;

				/// The triangle faces of the mesh, three consecutive indices define one triangle.
				Indices32 triangleFaces_;
		};

	public:

		/**
		 * Creates a triangle object containing all buildings of a tile.
		 * @param objects The tile objects, objects not being a building are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param volumetric True, to create a volumetic building if the building's height is known; False, to create a flat object
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Mesh createBuildings(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const bool volumetric, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Creates a triangle object containing one building.
		 * @param building The building for which the triangle object will be created
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param volumetric True, to create a volumetic building if the building's height is known; False, to create a flat object
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Mesh createBuilding(const Basemap::Building& building, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, const bool volumetric, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Creates a triangle object containing all roads of a tile.
		 * @param objects The tile objects, objects not being a road are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the roads, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @param roadWidthMap The map mapping road type to road widths in meter
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Mesh createRoads(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset = Scalar(0), const Basemap::Road::RoadWidthMap& roadWidthMap = Basemap::Road::defaultRoadWidthMap());

		/**
		 * Creates a triangle object containing all transits of a tile.
		 * @param objects The tile objects, objects not being a transit are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the transits, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Mesh createTransits(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Creates a triangle object containing all waters of a tile.
		 * @param objects The tile objects, objects not being a water are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Mesh createWaters(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Creates a triangle object containing all land uses of a tile.
		 * @param objects The tile objects, objects not being a land use are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Mesh createLandUses(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Creates a triangle object containing all land covers of a tile.
		 * @param objects The tile objects, objects not being a land cover are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Mesh createLandCovers(const Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Adds vertices for a building forming a 3D object.
		 * @param building The building for which the vertices will be added
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param volumetric True, to create a volumetic building if the building's height is known; False, to create a flat object
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool addBuilding(const Basemap::Building& building, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const bool volumetric, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Adds vertices for a road forming a flat band.
		 * @param road The road for which the vertices will be added
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @param roadWidthMap The map mapping road type to road widths in meter
		 * @return True, if succeeded
		 */
		static bool addRoad(const Basemap::Road& road, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset = Scalar(0), const Basemap::Road::RoadWidthMap& roadWidthMap = Basemap::Road::defaultRoadWidthMap());

		/**
		 * Adds vertices for a transit forming a flat band.
		 * @param transit The transit for which the vertices will be added
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool addTransit(const Basemap::Transit& transit, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Adds vertices for a water forming a flat geometry.
		 * @param water The water for which the vertices will be added
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool addWater(const Basemap::Water& water, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Adds vertices for a land use forming a flat geometry.
		 * @param landUse The land use for which the vertices will be added
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool addLandUse(const Basemap::LandUse& landUse, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset = Scalar(0));

		/**
		 * Adds vertices for a land cover forming a flat geometry.
		 * @param landCover The land cover for which the vertices will be added
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param groundPlaneOffset The offset of the ground plane in metric y-space, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool addLandCover(const Basemap::LandCover& landCover, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar groundPlaneOffset = Scalar(0));

	 	 /**
		 * Adds vertices for a polygon to form a 3D object.
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param outerPolygon The outer polygon of the object, with at least two points
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param normalizedHeight The height of the 3D object in normalized space, with range [0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param normalizedGroundPlaneOffset The offset of the ground plane in normalized y-space, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool addPolygon(const CV::PixelPositionI& origin, const CV::PixelPositionsI& outerPolygon, const Scalar pixelPositionNormalization, const Scalar normalizedHeight, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar normalizedGroundPlaneOffset);

	protected:

		/**
		 * Adds vertices for a line string to form a flat band.
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param lineString The line string to form the flat band from, with at least two points
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param normalizedWidth The width of the band in normalized space, with range (0, infinity)
		 * @param vertices The vertices to which the new vertices will be added
		 * @param normals The normals to which the new normals will be added
		 * @param triangleFaces The triangle faces to which the new triangle faces will be added, three consecutive indices define one triangle
		 * @param normalizedGroundPlaneOffset The offset of the ground plane in normalized y-space, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool addLineString(const CV::PixelPositionI& origin, const CV::PixelPositionsI& lineString, const Scalar pixelPositionNormalization, const Scalar normalizedWidth, Vectors3& vertices, Vectors3& normals, Indices32& triangleFaces, const Scalar normalizedGroundPlaneOffset);
};

inline Mesher::Mesh::Mesh(Vectors3&& vertices, Vectors3&& perVertexNormals, Indices32&& triangleFaces) :
	vertices_(std::move(vertices)),
	perVertexNormals_(std::move(perVertexNormals)),
	triangleFaces_(std::move(triangleFaces))
{
	ocean_assert(vertices_.size() == perVertexNormals_.size());
	ocean_assert(triangleFaces_.size() % 3 == 0);
}

inline const Vectors3& Mesher::Mesh::vertices() const
{
	return vertices_;
}

inline const Vectors3& Mesher::Mesh::perVertexNormals() const
{
	return perVertexNormals_;
}

inline const Indices32& Mesher::Mesh::triangleFaces() const
{
	return triangleFaces_;
}

inline bool Mesher::Mesh::isValid() const
{
	return !triangleFaces_.empty() && triangleFaces_.size() % 3 == 0;
}

}

}

}

}

#endif // META_OCEAN_IO_MAPS_RENDERING_MESHER_H
