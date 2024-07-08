/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_MAPS_BASEMAP_H
#define META_OCEAN_IO_MAPS_BASEMAP_H

#include "ocean/io/maps/Maps.h"

#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"

#include <vtzero/feature.hpp>
#include <vtzero/geometry.hpp>

namespace Ocean
{

namespace IO
{

namespace Maps
{

/**
 * This class implements an parser of basemap data.
 * @ingroup iomaps
 */
class OCEAN_IO_MAPS_EXPORT Basemap
{
	public:

		/**
		 * Definition of a location with signed pixel precision.
		 */
		typedef CV::PixelPositionI PixelPositionI;

		/**
		 * Definition of a vector holding locations with signed pixel precision.
		 */
		typedef CV::PixelPositionsI PixelPositionsI;

		/**
		 * Definition of groups of pixel positions.
		 */
		typedef std::vector<PixelPositionsI> PixelPositionGroupsI;

		/**
		 * This class is the base class for all map objects.
		 * Objects are extracted from layers, layer coordinates are given with pixel precision.
		 */
		class Object
		{
			public:

				/**
				 * Definition of individual object types.
				 */
				enum ObjectType : uint32_t
				{
					/// The object type is unknown.
					OT_UNKNOWN = 0u,
					/// The object is a building.
					OT_BUILDING,
					/// The object is a land cover.
					OT_LAND_COVER,
					/// The object is a land use.
					OT_LAND_USE,
					/// The object is a road.
					OT_ROAD,
					/// The object is a transit.
					OT_TRANSIT,
					/// The object is a water.
					OT_WATER
				};

			public:

				/**
				 * Returns the type of this object.
				 * @return The object's type
				 */
				inline ObjectType objectType() const;

				/**
				 * Returns the extent of the layer in which this object is defined.
				 * @return The layer's extent, in pixels, with range [1, infinity), 0 if invalid
				 */
				inline unsigned int layerExtent() const;

				/**
				 * Converts the coordinate defined in the owning layer of this object to a target domain with individual extent.
				 * @param coordinate The coordinate located in the layer to which this object belongs, with range (-infinity, infinity)x(-infinity, infinity)
				 * @param targetExtent The extent of the target domain, with sub-pixel precision, with range [1, infinity)
				 * @return The resulting converted coordinate
				 */
				inline Vector2 vectorFromCoordinate(const PixelPositionI& coordinate, const Scalar targetExtent) const;

			protected:

				/**
				 * Creates a new object.
				 * @param objectType The type of the new object, must be valid
				 * @param layerExtent The extent of the layer to which this object belongs, in pixel, with range [1, infinity)
				 */
				inline Object(const ObjectType objectType, const unsigned int layerExtent);

			protected:

				/// The object's type.
				ObjectType objectType_ = OT_UNKNOWN;

				/// The extent of the layer to which this object belongs, in pixel, with range [1, infinity), 0 if invalid
				unsigned int layerExtent_ = 0u;
		};

		/**
		 * This class implements a road object.
		 */
		class Road final : public Object
		{
			public:

				/**
				 * Definition of individual road types.
				 */
				enum RoadType : uint32_t
				{
					/// An unknown road type.
					RT_UNKNOWN = 0u,
					/// A road to provide access.
					RT_ACCESS,
					/// The road is an alley.
					RT_ALLEY,
					/// The road is a bridleway (e.g., mainly used by horses).
					RT_BRIDLEWAY,
					/// The road is a crossing.
					RT_CROSSING,
					/// A cycleway.
					RT_CYCLEWAY,
					/// The road is a crosswalk.
					RT_CROSSWALK,
					/// The road is a driveway.
					RT_DRIVEWAY,
					/// The road is a footway.
					RT_FOOTWAY,
					/// A highway.
					RT_HIGHWAY,
					/// A highway link.
					RT_HIGHWAY_LINK,
					/// A link.
					RT_LINK,
					/// A living street.
					RT_LIVING_STREET,
					/// A local street.
					RT_LOCAL,
					/// A motorway.
					RT_MOTORWAY,
					/// A motorway link.
					RT_MOTORWAY_LINK,
					/// The road is a parking aisle.
					RT_PARKING_AISLE,
					/// The road is a path.
					RT_PATH,
					/// A pedestrian road.
					RT_PEDESTRIAN,
					/// A residential road.
					RT_RESIDENTIAL,
					/// A primary road.
					RT_PRIMARY,
					/// A primary link road.
					RT_PRIMARY_LINK,
					/// The road is a raceway.
					RT_RACEWAY,
					/// A default road.
					RT_ROAD,
					/// A secondary road.
					RT_SECONDARY,
					/// A secondary link.
					RT_SECONDARY_LINK,
					/// A service road.
					RT_SERVICE,
					/// The road is a sidewalk.
					RT_SIDEWALK,
					/// A path/road with steps.
					RT_STEPS,
					/// A tertiary road.
					RT_TERTIARY,
					/// A tertiary link.
					RT_TERTIARY_LINK,
					/// A track.
					RT_TRACK,
					/// A trunk road.
					RT_TRUNK,
					/// A trunk link.
					RT_TRUNK_LINK,
					/// An unclassified road.
					RT_UNCLASSIFIED,
					/// Exclusive end value.
					RT_END
				};

				/**
				 * Definition of an unordered map mapping road types to road widths.
				 */
				typedef std::unordered_map<RoadType, float> RoadWidthMap;

			protected:

				/**
				 * Definition of an unordered map mapping road type strings to road type values.
				 */
				typedef std::unordered_map<std::string, RoadType> RoadTypeMap;

			public:

				/**
				 * Creates a new road.
				 * @param roadType The type of the new road, must be valid
				 * @param name The name of the road, if known
				 * @param lineStrings The line strings defining the shape of the road, at least one line string
				 * @param layerExtent The extent of the layer to which this road belongs, in pixel, with range [1, infinity)
				 */
				inline Road(const RoadType roadType, std::string&& name, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

				/**
				 * Returns the type of this road.
				 * @return The road's type
				 */
				inline RoadType roadType() const;

				/**
				 * Returns the line strings defining the shape of the road in the domain of the layer to which this road belongs.
				 * @return The line strings
				 */
				inline const PixelPositionGroupsI& lineStrings() const;

				/**
				 * Translates the string of a road type to a value.
				 * @param roadType The string of the road type to translate
				 * @return The resulting value of the road type, RT_UNKNOWN if unknown
				 */
				static RoadType translateRoadType(const std::string& roadType);

				/**
				 * Returns the default map for road widths.
				 * The default road width is specified for RT_END.
				 * @return The map mapping road types to road widths, in meter.
				 */
				static RoadWidthMap defaultRoadWidthMap();

			protected:

				/// The type of this road.
				RoadType roadType_;

				/// The name of this road, if known.
				std::string name_;

				/// The line strings defining the shape of the road in the domain of the layer to which this road belongs.
				PixelPositionGroupsI lineStrings_;
		};

		/**
		 * This class implements a transit object.
		 */
		class Transit final : public Object
		{
			public:

				/**
				 * Definition of individual transit types.
				 */
				enum TransitType : uint32_t
				{
					/// An unknown transit type.
					TT_UNKNOWN = 0u,
					/// Aerial transit way e.g., for planes.
					TT_AERIALWAY,
					/// A ferry.
					TT_FERRY,
					/// A railway.
					TT_RAILWAY
				};

				/**
				 * Creates a new transit object.
				 * @param transitType The type of the new transit object, must be valid
				 * @param name The name of the transit, if known
				 * @param lineStrings The line strings defining the shape of the transit, at least one line string
				 * @param layerExtent The extent of the layer to which this transit belongs, in pixel, with range [1, infinity)
				 */
				inline Transit(const TransitType transitType, std::string&& name, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

				/**
				 * Returns the type of this transit.
				 * @return The transit's type
				 */
				inline TransitType transitType() const;

				/**
				 * Returns the line strings defining the shape of the transit in the domain of the layer to which this transit belongs.
				 * @return The line strings
				 */
				inline const PixelPositionGroupsI& lineStrings() const;

			protected:

				/// The type of this transit.
				TransitType transitType_;

				/// The name of this transit, if known.
				std::string name_;

				/// The line strings defining the shape of the transit in the domain of the layer to which this transit belongs.
				PixelPositionGroupsI lineStrings_;
		};

		/**
		 * This class implements an object composed of inner and outer polygons.
		 */
		class InnerOuterPolygonsObject : public Object
		{
			public:

				/**
				 * Returns the individual outer polygons of this building.
				 * @return The building's outer polygons, can be empty
				 */
				inline const PixelPositionGroupsI& outerPolygons() const;

				/**
				 * Returns the individual inner poloyons of this building.
				 * @return The building's inner polygons, can be empty
				 */
				inline const PixelPositionGroupsI& innerPolygons() const;

			protected:

				/**
				 * Creates a new object.
				 * @param objectType The type of the new object, must be valid
				 * @param outerPolygons The building's outer polygons, can be empty
				 * @param innerPolygons The building's inner polygons, can be empty
				 * @param layerExtent The extent of the layer to which this building belongs, in pixel, with range [1, infinity)
				 */
				inline InnerOuterPolygonsObject(const ObjectType objectType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, const unsigned int layerExtent);

			protected:

				/// The individual outer polygons of this building.
				PixelPositionGroupsI outerPolygons_;

				/// The individual inner pologyons of this building.
				PixelPositionGroupsI innerPolygons_;
		};

		/**
		 * This class implements a building object.
		 */
		class OCEAN_IO_MAPS_EXPORT Building final : public InnerOuterPolygonsObject
		{
			public:

				/**
				 * Creates a new building.
				 * @param outerPolygons The building's outer polygons, can be empty
				 * @param innerPolygons The building's inner polygons, can be empty
				 * @param lineStrings The building's line strings, can be empty
				 * @param height The building's height, in meter, with range [0, infinity), -1 if unknown
				 * @param layerExtent The extent of the layer to which this building belongs, in pixel, with range [1, infinity)
				 */
				inline Building(PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const Scalar height, const unsigned int layerExtent);

				/**
				 * Returns the height of the building.
				 * @return The building's height, in meter, with range [0, infinity), -1 if unknown
				 */
				inline Scalar height() const;

				/**
				 * The individual line strings of this building.
				 * @return The building's line strings, can be empty
				 */
				inline const PixelPositionGroupsI& lineStrings() const;

				/**
				 * Returns the pixel bounding box entirely enclosing this building.
				 * @return The bounding box
				 */
				CV::PixelBoundingBoxI boundingBox() const;

			protected:

				/// The height of the bulding in meter, with range [0, infinity), -1 if unknown
				Scalar height_;

				/// The individual line strings of this building.
				PixelPositionGroupsI lineStrings_;
		};

		/**
		 * This class implements a water object.
		 */
		class Water final : public InnerOuterPolygonsObject
		{
			public:

				/**
				 * Definition of individual water types.
				 */
				enum WaterType : uint32_t
				{
					/// An unknown water type.
					WT_UNKNOWN = 0u,
					/// The water is a canal.
					WT_CANAL,
					/// The water is a dock.
					WT_DOCK,
					/// The water is human made.
					WT_HUMAN_MADE,
					/// An inland water.
					WT_INLAND,
					/// The water is a lake.
					WT_LAKE,
					/// The water is an ocean.
					WT_OCEAN,
					/// The water is a pond.
					WT_POND,
					/// The water is a reservoir.
					WT_RESERVOIR,
					/// The water is a river.
					WT_RIVER,
					/// The water is a stream.
					WT_STREAM,
					/// The water without further specification.
					WT_WATER,
					/// Exclusive end value.
					WT_END
				};

			protected:

				/**
				 * Definition of an unordered map mapping water type strings to water type values.
				 */
				typedef std::unordered_map<std::string, WaterType> WaterTypeMap;

			public:

				/**
				 * Creates a new water.
				 * @param waterType The type of the water
				 * @param outerPolygons The water's outer polygons, can be empty
				 * @param innerPolygons The water's inner polygons, can be empty
				 * @param layerExtent The extent of the layer to which this water belongs, in pixel, with range [1, infinity)
				 */
				inline Water(const WaterType waterType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, const unsigned int layerExtent);

				/**
				 * Returns the type of the water.
				 * @return The water's type
				 */
				inline WaterType waterType() const;

				/**
				 * Translates the string of a water type to a value.
				 * @param waterType The string of the water type to translate
				 * @return The resulting value of the water type, WT_UNKNOWN if unknown
				 */
				static WaterType translateWaterType(const std::string& waterType);

			protected:

				/// The type of the water.
				WaterType waterType_ = WT_UNKNOWN;
		};

		/**
		 * This class implements a land use object.
		 */
		class LandUse final : public InnerOuterPolygonsObject
		{
			public:

				/**
				 * Definition of individual land use types.
				 */
				enum LandUseType : uint32_t
				{
					/// An unknown land use type.
					LUT_UNKNOWN = 0u,
					/// The land use is an airport.
					LUT_AIRPORT,
					/// The land is used for amusement.
					LUT_AMUSEMENT,
					/// The land use is education.
					LUT_EDUCATION,
					/// The land use is a green space.
					LUT_GREENSPACE,
					/// The land use is a land.
					LUT_LAND,
					/// The land use is a national park.
					LUT_NATIONAL_PARK,
					/// The land use is a plaza.
					LUT_PLAZA,
					/// The land use is a recreation.
					LUT_RECREATION
				};

			public:

				/**
				 * Creates a new land use.
				 * @param landUseType The type of the land use
				 * @param outerPolygons The land use's outer polygons, can be empty
				 * @param innerPolygons The land use's inner polygons, can be empty
				 * @param lineStrings The building's line strings, can be empty
				 * @param layerExtent The extent of the layer to which this land use belongs, in pixel, with range [1, infinity)
				 */
				inline LandUse(const LandUseType landUseType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

				/**
				 * Returns the type of the land use.
				 * @return The land use's type
				 */
				inline LandUseType landUseType() const;

				/**
				 * Returns the line strings defining the shape of the transit in the domain of the layer to which this transit belongs.
				 * @return The line strings
				 */
				inline const PixelPositionGroupsI& lineStrings() const;

			protected:

				/// The type of the land use.
				LandUseType landUseType_ = LUT_UNKNOWN;

				/// The individual line strings of this building.
				PixelPositionGroupsI lineStrings_;
		};

		/**
		 * This class implements a land use object.
		 */
		class LandCover final : public InnerOuterPolygonsObject
		{
			public:

				/**
				 * Definition of individual land use types.
				 */
				enum LandCoverType : uint32_t
				{
					/// An unknown land cover type.
					LCT_UNKNOWN = 0u,
					/// The land is covered with grass.
					LCT_GRASS,
					/// The land is paved.
					LCT_PAVED,
					/// The land is coverd with sand.
					LCT_SAND
				};

			public:

				/**
				 * Creates a new land cover.
				 * @param landCoverType The type of the land cover
				 * @param outerPolygons The land use's outer polygons, can be empty
				 * @param innerPolygons The land use's inner polygons, can be empty
				 * @param lineStrings The building's line strings, can be empty
				 * @param layerExtent The extent of the layer to which this land use belongs, in pixel, with range [1, infinity)
				 */
				inline LandCover(const LandCoverType landCoverType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

				/**
				 * Returns the type of the land use.
				 * @return The land use's type
				 */
				inline LandCoverType landCoverType() const;

				/**
				 * Returns the line strings defining the shape of the transit in the domain of the layer to which this transit belongs.
				 * @return The line strings
				 */
				inline const PixelPositionGroupsI& lineStrings() const;

			protected:

				/// The type of the land cover.
				LandCoverType landCoverType_ = LCT_UNKNOWN;

				/// The individual line strings of this building.
				PixelPositionGroupsI lineStrings_;
		};

		/**
		 * Definition of a shared pointer holding an Object.
		 */
		typedef std::shared_ptr<Object> SharedObject;

		/**
		 * Definition of a vector holding shared objects.
		 */
		typedef std::vector<SharedObject> SharedObjects;

		/// Forward declaration.
		class TileIndexPair;

		/**
		 * Definition of a vector holding tile index pairs.
		 */
		typedef std::vector<TileIndexPair> TileIndexPairs;

		/**
		 * This class holds the tile indics in latitude and longitude direction.
		 */
		class TileIndexPair
		{
			public:

				/**
				 * Default constructor creating an invalid pair of indices.
				 */
				TileIndexPair() = default;

				/**
				 * Creates an new tile index pair.
				 * @param latitudeIndex The tile index in latitude direction, with range [0, infinity)
				 * @param longitudeIndex The tile index in longitude direction, with range [0, infinity)
				 */
				inline TileIndexPair(const unsigned int latitudeIndex, const unsigned int longitudeIndex);

				/**
				 * Returns the tile index in latitude direction.
				 * @return The tile index in latitude direction, with range [0, infinity)
				 */
				inline unsigned int latitudeIndex() const;

				/**
				 * Returns the tile index in longitude direction.
				 * @return The tile index in longitude direction, with range [0, infinty)
				 */
				inline unsigned int longitudeIndex() const;

				/**
				 * Returns whether this tile index pair is within the range of a maximal number of tiles.
				 * @param numberTiles The number of tiles in the level in which this tile pair is defined, with range [2, infinity)
				 * @return True, if so
				 */
				inline bool isInside(const unsigned int numberTiles) const;

				/**
				 * Returns whether a given location is close to this tile.
				 * The location is defined in a normalized tile fractions for a given tile.
				 * @param tileIndexPair The given tile in which the location is located, can be the same tile as this tile, must be valid
				 * @param latitudeFraction The normalized tile fraction in latitude direction defining the actual location in the `tileIndexPair` tile, with range [0, 1], 0 for the north edge, 1 for the south edge
				 * @param longitudeFraction The normalized tile fraction in longitude direction defining the actual location in the `tileIndexPair` tile, with range [0, 1], 0 for the west edge, 1 for the east edge
				 * @param maxFraction The maximal fraction between the given location and the border of this tile so that the given location counts as close, with range [0, 1], 0 to allow locations at the direct border only, 1 to allow locations with a one-tile distance
				 * @return True, if so
				 */
				bool isLocationClose(const TileIndexPair& tileIndexPair, const double latitudeFraction, const double longitudeFraction, const double maxFraction = 0.2) const;

				/**
				 * Returns whether the this object holds valid tile indices.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether two tile index pair objects hold the same indices.
				 * @param other The second tile pair object to compare
				 * @return True, if so
				 */
				inline bool operator==(const TileIndexPair& other) const;

				/**
				 * Returns whether two tile index pair objects hold not the same indices.
				 * @param other The second tile pair object to compare
				 * @return True, if so
				 */
				inline bool operator!=(const TileIndexPair& other) const;

				/**
				 * Hash function.
				 * @param tileIndexPair Tile index pair for which the hash will be determined
				 * @return The hash value
				 */
				inline size_t operator()(const TileIndexPair& tileIndexPair) const;

				/**
				 * Returns the tile index pairs of all neighboring tiles for a given center tile.
				 * @param tileIndexPair The center tile for which the neighboring tiles will be returned, must be valid
				 * @param numberTilesOnLevel The number of tiles in latitude and longitude direction on the tile's detail level, with range [max(tileIndexPair.latitudeIndex_, tileIndexPair.longitudeIndex_), infinity)
				 * @param maxDistance The maximal distance between a neighboring tile and the center tile in latitude or longitude direction so that it counts as neibhoring, 0 returns only the center tile, 1 returns the 9-neighborhood, 2 returns the 25-neighborhood, with range [0, infinity)
				 * @return The tiles in the neighborhood of the given center tile (always including the center tile)
				 */
				static TileIndexPairs createNeighborhoodTiles(const TileIndexPair& tileIndexPair, const unsigned int numberTilesOnLevel, const unsigned int maxDistance);

			protected:

				/// The tile index in latitude direction, with range [0, infinity).
				unsigned int latitudeIndex_ = (unsigned int)(-1);

				/// The tile index in longitude direction, with range [0, infinity).
				unsigned int longitudeIndex_ = (unsigned int)(-1);
		};

		/**
		 * Definition of a set holding tile index pairs.
		 */
		typedef std::unordered_set<TileIndexPair, TileIndexPair> TileIndexPairSet;

		/**
		 * This class stores the information belonging to one map tile.
		 */
		class OCEAN_IO_MAPS_EXPORT Tile
		{
			public:

				/**
				 * Default constructor creating an invalid tile.
				 */
				Tile() = default;

				/**
				 * Default move constructor.
				 * @param tile Tile to be moved
				 */
				Tile(Tile&& tile) = default;

				/**
				 * Creates a new valid tile object.
				 * @param level The detail level, with range [1, 22]
				 * @param tileIndexPair The tile index pair defining the location of this tile, with range [0, numberTiles(level) - 1]x[0, numberTiles(level) - 1]
				 */
				inline Tile(const unsigned int level, const TileIndexPair& tileIndexPair);

				/**
				 * Returns the detail level of this tile.
				 * @return The tile's detail level, with range [1, 22], 0 if invalid
				 */
				inline unsigned int level() const;

				/**
				 * Returns the tile index pair definining the tile's location.
				 * @return The tile's index pair, with range [0, numberTiles(level()) - 1]x[0, numberTiles(level()) - 1]
				 */
				inline const TileIndexPair& tileIndexPair() const;

				/**
				 * Returns all objects of this tile.
				 * @return The tile's objects
				 */
				inline const SharedObjects& objects() const;

				/**
				 * Parses a buffer containing the tile information as pbf file.
				 * Previously exsiting map objects will not be removed before the new map objects are added during parsing.
				 * @param data The data of the buffer, must be valid
				 * @param size The size of the buffer, in bytes, with range [1, infinity)
				 * @return True, if succeeded
				 */
				bool parsePBFData(const void* data, const size_t size);

				/**
				 * Returns the approximated GPS location of a position in this tile.
				 * @param position The position in this tile, in pixels, with range (-infinity, infinity)x(-infinity, infinity), must be valid
				 * @param layerExtent The extent of the layer in which the position is located, with range [1, infinity)
				 * @param latitude The resulting latitude coordinate of the GPS location of the given tile position, in degree, with range [-90, 90]
				 * @param longitude The resulting latitude coordinate of the GPS location of the given tile position, in degree, with range [-180, 180]
				 */
				void tileCoordinate2GPSLocation(const PixelPositionI& position, const unsigned int layerExtent, double& latitude, double& longitude) const;

				/**
				 * Returns the metric extent of this tile.
				 * @param earthRadius The radius of the earth, in meters, with range (0, infinity)
				 * @return The approximated extent of this tile at a tile's latitude, in horizontal and vertical direction, in meter, with range (0, infinity)
				 */
				double metricExtent(const double earthRadius = 6378135.0) const;

				/**
				 * Removes all map objects, the level and tile information is untought.
				 */
				void clear();

				/**
				 * Returns whether this tile is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Default move operator.
				 * @param tile Tile to be moved
				 * @return Reference to this object
				 */
				Tile& operator=(Tile&& tile) = default;

				/**
				 * Returns the number of tiles in horizontal and vertical direction for a given detail level.
				 * The number of levels is determined by 2 ^ level .
				 * @param level The detail level, with range [1, 22]
				 * @return The number of tiles in horizontal and vertical direction, with range [2, 4,194,304]
				 */
				static constexpr inline unsigned int numberTiles(const unsigned int level);

				/**
				 * Calculates the tile in which a given GPS coordinate is located at a specified detail level.
				 * @param level The detail level, with range [1, 22]
				 * @param latitude The latitude value of the GPS coordinate, in degree, with range [-90, 90]
				 * @param longitude The longitude value of the GPS coordinate, in degree, with range [-180, 180]
				 * @param latitudeTileFraction Optional resulting fraction in latitude direction providing the preceise location within the tile, with range [0, 1), 0 for the north edge of the tile, 1 for the south edege of the tile
				 * @param longitudeTileFraction Optional resulting fraction in longitude direction providing the preceise location within the tile, with range [0, 1), 0 for the west edge of the tile, 1 for the east edege of the tile
				 * @return The resulting tile index pair, with range [0, numberTiles(level) - 1]x[0, numberTiles(level) - 1]
				 */
				static TileIndexPair calculateTile(const unsigned int level, const double latitude, const double longitude, double* latitudeTileFraction = nullptr, double* longitudeTileFraction = nullptr);

				/**
				 * Calculates the tile fraction ini latitude and longitude for a given GPS coordinate in relation to a given tile.
				 * @param level The detail level, with range [1, 22]
				 * @param latitude The latitude value of the GPS coordinate, in degree, with range [-90, 90]
				 * @param longitude The longitude value of the GPS coordinate, in degree, with range [-180, 180]
				 * @param tileIndexPair The tile index pair specifying the tile for which the fractions will be determined, must be valid
				 * @param latitudeTileFraction Optional resulting fraction in latitude direction providing the preceise location within the tile, with range [0, 1), 0 for the north edge of the tile, 1 for the south edege of the tile
				 * @param longitudeTileFraction Optional resulting fraction in longitude direction providing the preceise location within the tile, with range [0, 1), 0 for the west edge of the tile, 1 for the east edege of the tile
				 */
				static void calculateTileFractions(const unsigned int level, const double latitude, const double longitude, const TileIndexPair& tileIndexPair, double& latitudeTileFraction, double& longitudeTileFraction);

				/**
				 * Returns the approximated GPS location of a position in a tile.
				 * @param level The detail level, with range [1, 22]
				 * @param tileIndexPair The tile index pair defining the tile, must be valid
				 * @param position The position in the tile, in pixels, with range (-infinity, infinity)x(-infinity, infinity), must be valid
				 * @param layerExtent The extent of the layer in which the position is located, with range [1, infinity)
				 * @param latitude The resulting latitude coordinate of the GPS location of the given tile position, in degree, with range [-90, 90]
				 * @param longitude The resulting latitude coordinate of the GPS location of the given tile position, in degree, with range [-180, 180]
				 */
				static void tileCoordinate2GPSLocation(const unsigned int level, const TileIndexPair& tileIndexPair, const PixelPositionI& position, const unsigned int layerExtent, double& latitude, double& longitude);

			protected:

				/**
				 * Calculates the tile in which a given GPS coordinate is located in latitude direction with sub-tile accuracy in normalized tile space.
				 * The actual tile index can be determined by:
				 * <pre>
				 * latitudeTileIndex = floor(latitudeTileNormalized * numberTiles(level))
				 * </pre>
				 * @param latitude The latitude value of the GPS coordinate, in degree, with range [-90, 90]
				 * @return The resulting normalized tile in latitude direction in which the GPS location is located, with range [0, 1)
				 **/
				static double calculateNormalizedTileLatitude(const double latitude);

				/**
				 * Calculates the tile in which a given GPS coordinate is located in longitude direction with sub-tile accuracy in normalized tile space.
				 * The actual tile index can be determined by:
				 * <pre>
				 * longitudeTileIndex = floor(longitudeTileNormalized * numberTiles(level))
				 * </pre>
				 * @param longitude The longitude value of the GPS coordinate, in degree, with range [-180, 180]
				 * @return The resulting normalized tile in longitude direction which the GPS locaiton is located, withr ange [0, 1)
				 **/
				static double calculateNormalizedTileLongitude(const double longitude);

			protected:

				/// The detail level of this tile, with range [1, 22], 0 if invalid
				unsigned int level_ = 0u;

				/// The tile index pair of the tile in latitude/vertical direction within the detail level, with range [0, numberTiles(level_) - 1]x[0, numberTiles(level_) - 1], otherwise invalid
				TileIndexPair tileIndexPair_;

				/// The map objects of in this tile.
				SharedObjects objects_;
		};

		/**
		 * Definition of a shared pointer holding a Tile.
		 */
		 typedef std::shared_ptr<Tile> SharedTile;

	protected:

		/**
		 * Geometry handler for points.
		 */
		class GeometryHandlerPoints
		{
			public:

				/**
				 * This is called once at the beginning with the number of points.
				 * For a point geometry, this will be 1, for multipoint geometries this will be larger.
				 * @param count The number of points, with range [1, infinity)
				 */
				void points_begin(uint32_t count);

				/**
				 * This is called once for each point.
				 * @param point The new point
				 */
				void points_point(vtzero::point point);

				/**
				 * This is called once at the end.
				 */
				void points_end();

				/**
				 * Resets the handler so that it can be reused.
				 */
				void reset();

			public:

				/// The gathered points of this handler.
				PixelPositionsI points_;
		};

		/**
		 * Geometry handler for polygons.
		 */
		class GeometryHandlerLinePolygons
		{
			public:

				/**
				 * This is called at the beginning of each ring with the number of points in this ring.
				 * For a simple polygon with only one outer ring, this function will only be called once, if there are inner rings or if this is a multipolygon, it will be called several times.
				 * @param count The number of points in the next polygon (first and last point are identical), with range [2, infinity)
				 */
				void ring_begin(uint32_t count);

				/**
				 * This is called once for each point.
				 * @param point The new point
				 */
				void ring_point(vtzero::point point);

				/**
				 * This is called at the end of each ring.
				 * The parameter tells you whether the ring is an outer or inner ring or whether the ring was invalid (if the area is 0).
				 */
				void ring_end(vtzero::ring_type ringType);

				/**
				 * Resets the handler so that it can be reused.
				 */
				void reset();

			public:

				/// The points of the current active polygon.
				PixelPositionsI intermediatePolygons_;

				/// The individual outer polygons.
				PixelPositionGroupsI outerPolygons_;

				/// the indvidual inner polygons.
				PixelPositionGroupsI innerPolygons_;
		};

		/**
		 * Geometry handler for line strings.
		 */
		class GeometryHandlerLineStrings
		{
			public:

				/**
				 * This is called at the beginning of each linestring with the number of points in this linestring.
				 * For a simple linestring this function will only be called once, for a multilinestring it will be called several times.
				 * @param count The number of points in the next line string, with range [1, infinity)
				 */
				void linestring_begin(uint32_t count);

				/**
				 * This is called once for each point.
				 * @param point The new point
				 */
				void linestring_point(vtzero::point point);

				/**
				 * This is called at the end of each linestring.
				 */
				void linestring_end();

				/**
				 * Resets the handler so that it can be reused.
				 */
				void reset();

			public:

				/// The individual line strings.
				PixelPositionGroupsI lineStrings_;
		};

		/**
		 * Definition of individual layer types.
		 */
		enum LayerType : uint32_t
		{
			/// The layer type is unknown.
			LT_UNKNOWN = 0u,
			/// The layer holds information about an airport (point, line, & polygon airport features).
			LT_AIRPORT,
			/// The layer holds areas of interest (polygonal areas of interest).
			LT_AREA_OF_INTEREST,
			/// The layer holds bathymetry information (depth polygons for oceans).
			LT_BATHYMETRY,
			/// The layer holds buildings (polygonal structures).
			LT_BUILDING,
			/// The layer holds labels of buildings (label point centroid for polygonal structures).
			LT_BUILDING_LABEL,
			/// The layer holds a border.
			LT_BORDER,
			/// The layer is an indoor layer (floor plans for meta offices, malls, airports).
			LT_INDOOR,
			/// The layer holds indoor label information (labels for indoor features).
			LT_INDOOR_LABEL,
			/// The layer holds land cover information (polygons for physical land features).
			LT_LAND_COVER,
			/// The layer holds land use information (mostly polygons for land usages).
			LT_LAND_USE,
			/// The layer holds labels of land use (label point centroid for polygonal land usages).
			LT_LAND_USE_LABEL,
			/// The layer holds landmark point information.
			LT_LANDMARK_POINT,
			/// The layer holds natural areas.
			LT_NATURAL,
			/// The layer holds labels of natural areas.
			LT_NATURAL_LABEL,
			/// The layer holds parking information (experimental parking layer, point & polygon).
			LT_PARKING,
			/// The layer holds labels of places (point features for cities, neighborhoods).
			LT_PLACENAME,
			/// The layer holds place labels.
			LT_PLACE_LABEL,
			/// The layer holds a point of interest.
			LT_POI,
			/// The layer holds road data (linear features for roads, sidewalks).
			LT_ROAD,
			/// The layer holds transit information (aerial, rail, ferry; linear features).
			LT_TRANSIT,
			/// The layer holds transit point information (aerial, rail, ferry stations).
			LT_TRANSIT_POINT,
			/// The layer holds tree point information (point features for trees).
			LT_TREE_POINT,
			/// The layer holds water information (polygonal water features).
			LT_WATER,
			/// The layer holds waterway information (linear water features).
			LT_WATERWAY,
			/// The layer holds water label information (label point centroid for polygonal water).
			LT_WATER_LABEL,
			/// The layer holds water line information.
			LT_WATER_LINE,
			/// The layer holds water offset information (polygons used for creating a shadow effect on inland water features).
			LT_WATER_OFFSET,
			/// The layer holds wave information (points used to show a wave icon over water).
			LT_WAVE,
			/// Exclusive end value.
			LT_END
		};

		/**
		 * Definition of an unordered map mapping layer type strings to layer type values.
		 */
		typedef std::unordered_map<std::string, LayerType> LayerTypeMap;

		/// The minimal latitude angle, in degree.
		static constexpr double minLatitude = -85.05112878;

		/// The maximal latitude angle, in degree.
		static constexpr double maxLatitude = 85.05112878;

	public:

		/**
		 * Creates a new tile based on given PBF data.
		 * @param level The detail level, with range [1, 22]
		 * @param tileIndexPair The tile index pair defining the location of the tile, with range [0, numberTiles(level) - 1]x[0, numberTiles(level) - 1]
		 * @param data The data of the buffer, must be valid
		 * @param size The size of the buffer, in bytes, with range [1, infinity)
		 * @return The resulting tile, invalid if the given PBF data could not be parsed
		 */
		static SharedTile newTileFromPBFData(const unsigned int level, const TileIndexPair& tileIndexPair, const void* data, const size_t size);

		/**
		 * Returns the url for downloading the map style data
		 * @return The url for downloading the map style data
		 */
		static const std::string& styleUrl();

		/**
		 * Gets the url template for downloading a map tile.
		 * @param styleData The style data, must be valid
		 * @param styleSize The size of the style data buffer, in bytes, with range [1, infinity)
		 * @param urlTemplate The resulting url template
		 * @return True, if succeeded
		 */
		static bool extractTileUrlTemplate(const char* styleData, const size_t styleSize, std::string& urlTemplate);

		/**
		 * Constructs the url for downloading a map tile.
		 * @param urlTemplate The url template, must be valid
		 * @param level The detail level, with range [1, 22]
		 * @param tileIndexPair The tile index pair defining the location of the tile, with range [0, numberTiles(level) - 1]x[0, numberTiles(level) - 1]
		 * @param url The resulting url
		 * @return True, if succeeded
		 */
		static bool constructTileUrl(const std::string urlTemplate, const unsigned int level, const TileIndexPair& tileIndexPair, std::string& url);

	protected:

		/**
		 * Parses a building feature.
		 * @param vtzeroFeature The building feature, must be valid
		 * @param outerPolygons The outer polygons of the building, can be empty
		 * @param innerPolygons The inner polygons of the building, can be empty
		 * @param lineStrings The line strings of the building, can be empty
		 * @param layerExtent The extent of the layer in which the building is located, in pixels, with range [1, infinity)
		 * @return The resulting building object, nullptr if the feature could not be parsed
		 */
		static SharedObject parseBuilding(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

		/**
		 * Parses a road feature.
		 * @param vtzeroFeature The building feature, must be valid
		 * @param lineStrings The line strings of the transit, at least one
		 * @param layerExtent The extent of the layer in which the transit is located, in pixels, with range [1, infinity)
		 * @return The resulting road object, nullptr if the feature could not be parsed
		 */
		static SharedObject parseRoad(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

		/**
		 * Parses a transit feature.
		 * @param vtzeroFeature The building feature, must be valid
		 * @param lineStrings The line strings of the transit, at least one
		 * @param layerExtent The extent of the layer in which the transit is located, in pixels, with range [1, infinity)
		 * @return The resulting transit object, nullptr if the feature could not be parsed
		 */
		static SharedObject parseTransit(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

		/**
		 * Parses a water feature.
		 * @param vtzeroFeature The building feature, must be valid
		 * @param outerPolygons The outer polygons of the building, can be empty
		 * @param innerPolygons The inner polygons of the building, can be empty
		 * @param layerExtent The extent of the layer in which the building is located, in pixels, with range [1, infinity)
		 * @return The resulting water object, nullptr if the feature could not be parsed
		 */
		static SharedObject parseWater(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, const unsigned int layerExtent);

		/**
		 * Parses a land use feature.
		 * @param vtzeroFeature The building feature, must be valid
		 * @param outerPolygons The outer polygons of the building, can be empty
		 * @param innerPolygons The inner polygons of the building, can be empty
		 * @param lineStrings The line strings of the building, can be empty
		 * @param layerExtent The extent of the layer in which the building is located, in pixels, with range [1, infinity)
		 * @return The resulting land use object, nullptr if the feature could not be parsed
		 */
		static SharedObject parseLandUse(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

		/**
		 * Parses a land cover feature.
		 * @param vtzeroFeature The building feature, must be valid
		 * @param outerPolygons The outer polygons of the building, can be empty
		 * @param innerPolygons The inner polygons of the building, can be empty
		 * @param lineStrings The line strings of the building, can be empty
		 * @param layerExtent The extent of the layer in which the building is located, in pixels, with range [1, infinity)
		 * @return The resulting land cover object, nullptr if the feature could not be parsed
		 */
		static SharedObject parseLandCover(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent);

		/**
		 * Returns the value of a property value as number.
		 * @param propertyValue The property value from which the number will be extracted
		 * @param value The resulting number value
		 * @return True, if succeeded
		 */
		static bool numberFromPropertyValue(const vtzero::property_value& propertyValue, double& value);

		/**
		 * Translates the name of a layer to the corresponding layer type.
		 * @param layerName The name of the layer, must be valid
		 * @return The resulting layer type, LT_UNKNOWN if unknown
		 */
		static LayerType translateLayerName(const std::string& layerName);
};

inline Basemap::Object::Object(const ObjectType objectType, const unsigned int layerExtent) :
	objectType_(objectType),
	layerExtent_(layerExtent)
{
	ocean_assert(objectType != OT_UNKNOWN);
}

inline unsigned int Basemap::Object::layerExtent() const
{
	return layerExtent_;
}

inline Vector2 Basemap::Object::vectorFromCoordinate(const PixelPositionI& coordinate, const Scalar targetExtent) const
{
	ocean_assert(layerExtent_ >= 1u);
	ocean_assert(coordinate.isValid() && targetExtent > Numeric::eps());

	return Vector2(Scalar(coordinate.x()), Scalar(coordinate.y())) * Scalar(targetExtent) / Scalar(layerExtent_);
}

inline Basemap::Object::ObjectType Basemap::Object::objectType() const
{
	return objectType_;
}

inline Basemap::Road::Road(const RoadType roadType, std::string&& name, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent) :
	Object(OT_ROAD, layerExtent),
	roadType_(roadType),
	name_(std::move(name)),
	lineStrings_(std::move(lineStrings))
{
	ocean_assert(!lineStrings_.empty());
}

inline Basemap::Road::RoadType Basemap::Road::roadType() const
{
	return roadType_;
}

inline const Basemap::PixelPositionGroupsI& Basemap::Road::lineStrings() const
{
	return lineStrings_;
}

inline Basemap::Transit::Transit(const TransitType transitType, std::string&& name, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent) :
	Object(OT_TRANSIT, layerExtent),
	transitType_(transitType),
	name_(std::move(name)),
	lineStrings_(std::move(lineStrings))
{
	ocean_assert(!lineStrings_.empty());
}

inline Basemap::Transit::TransitType Basemap::Transit::transitType() const
{
	return transitType_;
}

inline const Basemap::PixelPositionGroupsI& Basemap::Transit::lineStrings() const
{
	return lineStrings_;
}

inline Basemap::InnerOuterPolygonsObject::InnerOuterPolygonsObject(const ObjectType objectType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, const unsigned int layerExtent) :
	Object(objectType, layerExtent),
	outerPolygons_(std::move(outerPolygons)),
	innerPolygons_(std::move(innerPolygons))
{
	// nothing to do here
}

inline const Basemap::PixelPositionGroupsI& Basemap::InnerOuterPolygonsObject::outerPolygons() const
{
	return outerPolygons_;
}

inline const Basemap::PixelPositionGroupsI& Basemap::InnerOuterPolygonsObject::innerPolygons() const
{
	return innerPolygons_;
}

inline Basemap::Building::Building(PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const Scalar height, const unsigned int layerExtent) :
	InnerOuterPolygonsObject(OT_BUILDING, std::move(outerPolygons), std::move(innerPolygons), layerExtent),
	height_(height),
	lineStrings_(std::move(lineStrings))
{
	// nothing to do here
}

inline Scalar Basemap::Building::height() const
{
	return height_;
}

inline const Basemap::PixelPositionGroupsI& Basemap::Building::lineStrings() const
{
	return lineStrings_;
}

inline Basemap::Water::Water(const WaterType waterType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, const unsigned int layerExtent) :
	InnerOuterPolygonsObject(OT_WATER, std::move(outerPolygons), std::move(innerPolygons), layerExtent),
	waterType_(waterType)
{
	// nothing to do here
}

inline Basemap::Water::WaterType Basemap::Water::waterType() const
{
	return waterType_;
}

inline Basemap::LandUse::LandUse(const LandUseType landUseType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent) :
	InnerOuterPolygonsObject(OT_LAND_USE, std::move(outerPolygons), std::move(innerPolygons), layerExtent),
	landUseType_(landUseType),
	lineStrings_(std::move(lineStrings))
{
	// nothing to do here
}

inline Basemap::LandUse::LandUseType Basemap::LandUse::landUseType() const
{
	return landUseType_;
}

inline const Basemap::PixelPositionGroupsI& Basemap::LandUse::lineStrings() const
{
	return lineStrings_;
}

inline Basemap::LandCover::LandCover(const LandCoverType landCoverType, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent) :
	InnerOuterPolygonsObject(OT_LAND_COVER, std::move(outerPolygons), std::move(innerPolygons), layerExtent),
	landCoverType_(landCoverType),
	lineStrings_(std::move(lineStrings))
{
	// nothing to do here
}

inline Basemap::LandCover::LandCoverType Basemap::LandCover::landCoverType() const
{
	return landCoverType_;
}

inline const Basemap::PixelPositionGroupsI& Basemap::LandCover::lineStrings() const
{
	return lineStrings_;
}

inline Basemap::TileIndexPair::TileIndexPair(const unsigned int latitudeIndex, const unsigned int longitudeIndex) :
	latitudeIndex_(latitudeIndex),
	longitudeIndex_(longitudeIndex)
{
	ocean_assert(isValid());
}

inline unsigned int Basemap::TileIndexPair::latitudeIndex() const
{
	return latitudeIndex_;
}

inline unsigned int Basemap::TileIndexPair::longitudeIndex() const
{
	return longitudeIndex_;
}

inline bool Basemap::TileIndexPair::isInside(const unsigned int numberTiles) const
{
	return latitudeIndex_ < numberTiles && longitudeIndex_ < numberTiles;
}

inline bool Basemap::TileIndexPair::isValid() const
{
	return latitudeIndex_ != (unsigned int)(-1) && longitudeIndex_ != (unsigned int)(-1);
}

inline bool Basemap::TileIndexPair::operator==(const TileIndexPair& other) const
{
	return latitudeIndex_ == other.latitudeIndex_ && longitudeIndex_ == other.longitudeIndex_;
}

inline bool Basemap::TileIndexPair::operator!=(const TileIndexPair& other) const
{
	return !(*this == other);
}

inline size_t Basemap::TileIndexPair::operator()(const TileIndexPair& tileIndexPair) const
{
	return size_t(tileIndexPair.latitudeIndex_) ^ size_t(tileIndexPair.longitudeIndex_);
}

inline Basemap::Tile::Tile(const unsigned int level, const TileIndexPair& tileIndexPair) :
	level_(level),
	tileIndexPair_(tileIndexPair)
{
	ocean_assert(isValid());
}

inline unsigned int Basemap::Tile::level() const
{
	return level_;
}

inline const Basemap::TileIndexPair& Basemap::Tile::tileIndexPair() const
{
	return tileIndexPair_;
}

inline const Basemap::SharedObjects& Basemap::Tile::objects() const
{
	return objects_;
}

inline bool Basemap::Tile::isValid() const
{
	return level_ >= 1u && level_ <= 22u && tileIndexPair_.isValid() && tileIndexPair_.latitudeIndex() < numberTiles(level_) && tileIndexPair_.longitudeIndex() < numberTiles(level_);
}

constexpr inline unsigned int Basemap::Tile::numberTiles(const unsigned int level)
{
	ocean_assert(level >= 1u && level < 22u);

	return 1u << level;
}

}

}

}

#endif // META_OCEAN_IO_MAPS_BASEMAP_H
