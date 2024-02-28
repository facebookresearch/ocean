// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_MAPS_HANDLER_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_MAPS_HANDLER_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "ocean/io/maps/Basemap.h"

#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Triangles.h"

#include "ocean/network/tigon/TigonRequest.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements helper functions to handle map elements e.g., from basemap.
 * @ingroup xrplayground
 */
class MapsHandler
{
	public:

		/**
		 * Creates the rendering objects of one tile.
		 * @param engine The rendering engine to be used
		 * @param tile The map tile to be rendered
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param volumetric True, to create a volumetic building if the building's height is known; False, to create a flat object
		 * @return The Transform object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TransformRef createTile(const Rendering::Engine& engine, const IO::Maps::Basemap::Tile& tile, const Scalar targetSize, const bool volumetric);

		/**
		 * Creates a triangle object containing all buildings of a tile.
		 * @param engine The rendering engine to be used
		 * @param objects The tile objects, objects not being a building are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @param volumetric True, to create a volumetic building if the building's height is known; False, to create a flat object
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TrianglesRef createBuildings(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const bool volumetric);

		/**
		 * Creates a triangle object containing one building.
		 * @param engine The rendering engine to be used
		 * @param building The building for which the triangle object will be created
		 * @param origin The explicit origin in the layer domain, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixelPositionNormalization The normalization function to be applied to each coordinate of the line string, with range (0, infinity)
		 * @param metricNormalization The normalization function to be applied to each metric value, with range (0, infinity)
		 * @param volumetric True, to create a volumetic building if the building's height is known; False, to create a flat object
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TrianglesRef createBuilding(const Rendering::Engine& engine, const IO::Maps::Basemap::Building& building, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, const bool volumetric);

		/**
		 * Creates a triangle object containing all roads of a tile.
		 * @param engine The rendering engine to be used
		 * @param objects The tile objects, objects not being a road are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the roads, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TrianglesRef createRoads(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize);

		/**
		 * Creates a triangle object containing all transits of a tile.
		 * @param engine The rendering engine to be used
		 * @param objects The tile objects, objects not being a transit are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the transits, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TrianglesRef createTransits(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize);

		/**
		 * Creates a triangle object containing all waters of a tile.
		 * @param engine The rendering engine to be used
		 * @param objects The tile objects, objects not being a building are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TrianglesRef createWaters(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize);

		/**
		 * Creates a triangle object containing all land uses of a tile.
		 * @param engine The rendering engine to be used
		 * @param objects The tile objects, objects not being a building are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TrianglesRef createLandUses(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize);

		/**
		 * Creates a triangle object containing all land covers of a tile.
		 * @param engine The rendering engine to be used
		 * @param objects The tile objects, objects not being a building are ignored
		 * @param titleMetricExtent The metric extent of the tile owning the buildings, in meter, with range (0, infinity)
		 * @param targetSize The target size of the tile in the rendering domain, with range (0, infinity)
		 * @return The Triangles object containing all rendering element, invalid in case of an error
		 */
		static Rendering::TrianglesRef createLandCovers(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize);

		/**
		 * Downloads a new map tile.
		 * @param level The detail level of the tile to download, with range [1, 22]
		 * @param latitude The latitude location inside the tile, in degree, with range [-180, 180]
		 * @param longitude The longitude location inside the tile, in degree, with range [-90, 90]
		 * @return The map tile, nullptr if the tile could not be accessed
		 */
		static std::future<Network::Tigon::TigonRequest::TigonResponse> downloadTile(const unsigned int level, const double latitude, const double longitude);

		/**
		 * Downloads a new map tile.
		 * @param level The detail level of the tile to download, with range [1, 22]
		 * @param latitudeIndex
		 * @param longitudeIndex
		 * @return The map tile, nullptr if the tile could not be accessed
		 */
		static std::future<Network::Tigon::TigonRequest::TigonResponse> downloadTile(const unsigned int level, const unsigned int latitudeIndex, const unsigned int longitudeIndex);

		/**
		 * Temporary workaround.
		 *
		 * Downloads a new map tile.
		 * @param level The detail level of the tile to download, with range [1, 22]
		 * @param latitude The latitude location inside the tile, in degree, with range [-180, 180]
		 * @param longitude The longitude location inside the tile, in degree, with range [-90, 90]
		 * @return The map tile, nullptr if the tile could not be accessed
		 */
		static IO::Maps::Basemap::SharedTile downloadTileDirect(const unsigned int level, const double latitude, const double longitude);
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_MAPS_HANDLER_H
