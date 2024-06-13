/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/maps/Basemap.h"

#include "ocean/io/JSONConfig.h"

#include <vtzero/vector_tile.hpp>

#include <sstream>
#include <regex>

namespace Ocean
{

namespace IO
{

namespace Maps
{

Basemap::Road::RoadType Basemap::Road::translateRoadType(const std::string& roadType)
{
	const static RoadTypeMap roadTypeMap =
	{
		{"access", RT_ACCESS},
		{"alley", RT_ALLEY},
		{"bridleway", RT_BRIDLEWAY},
		{"crossing", RT_CROSSING},
		{"crosswalk", RT_CROSSWALK},
		{"cycleway", RT_CYCLEWAY},
		{"driveway", RT_DRIVEWAY},
		{"footway", RT_FOOTWAY},
		{"highway", RT_HIGHWAY},
		{"highway_link", RT_HIGHWAY_LINK},
		{"link", RT_LINK},
		{"living_street", RT_LIVING_STREET},
		{"local", RT_LOCAL},
		{"motorway", RT_MOTORWAY},
		{"motorway_link", RT_MOTORWAY_LINK},
		{"parking_aisle", RT_PARKING_AISLE},
		{"path", RT_PATH},
		{"pedestrian", RT_PEDESTRIAN},
		{"primary", RT_PRIMARY},
		{"primary_link", RT_PRIMARY_LINK},
		{"raceway", RT_RACEWAY},
		{"residential", RT_RESIDENTIAL},
		{"road", RT_ROAD},
		{"secondary", RT_SECONDARY},
		{"secondary_link", RT_SECONDARY_LINK},
		{"service", RT_SERVICE},
		{"sidewalk", RT_SIDEWALK},
		{"steps", RT_STEPS},
		{"tertiary", RT_TERTIARY},
		{"tertiary_link", RT_TERTIARY_LINK},
		{"track", RT_TRACK},
		{"trunk", RT_TRUNK},
		{"trunk_link", RT_TRUNK_LINK},
		{"unclassified", RT_UNCLASSIFIED}
	};

	ocean_assert(roadTypeMap.size() == RT_END - 1u);

	const RoadTypeMap::const_iterator iType = roadTypeMap.find(roadType);

	if (iType == roadTypeMap.cend())
	{
		Log::warning() << "Unknown basemap road class: " << roadType;
		return RT_UNKNOWN;
	}

	return iType->second;
}

Basemap::Road::RoadWidthMap Basemap::Road::defaultRoadWidthMap()
{
	RoadWidthMap roadWidthMap =
	{
		{RT_LINK, 4.0f},
		{RT_LIVING_STREET, 4.0f},
		{RT_LOCAL, 4.0f},
		{RT_HIGHWAY, 9.0f},
		{RT_HIGHWAY_LINK, 4.0f},
		{RT_MOTORWAY, 9.0f},
		{RT_MOTORWAY_LINK, 4.0f},
		{RT_PRIMARY, 9.0f},
		{RT_PRIMARY_LINK, 4.0f},
		{RT_RESIDENTIAL, 4.0},
		{RT_ROAD, 3.0f},
		{RT_SECONDARY, 6.0},
		{RT_SECONDARY_LINK, 4.0f},
		{RT_SERVICE, 3.0f},
		{RT_TERTIARY, 6.0},
		{RT_TERTIARY_LINK, 4.0f},
		{RT_TRACK, 4.0f},
		{RT_TRUNK, 9.0f},
		{RT_TRUNK_LINK, 4.0f},
		{RT_UNCLASSIFIED, 4.0f},
		{RT_END, 2.0f} // default width for all unspecified streets
	};

	ocean_assert(roadWidthMap.size() <= RT_END);

	return roadWidthMap;
}

CV::PixelBoundingBoxI Basemap::Building::boundingBox() const
{
	CV::PixelBoundingBoxI result;

	for (const PixelPositionsI& outerPolygon : outerPolygons_)
	{
		for (const PixelPositionI& position : outerPolygon)
		{
			result += position;
		}
	}

	for (const PixelPositionsI& lineString : lineStrings_)
	{
		for (const PixelPositionI& position : lineString)
		{
			result += position;
		}
	}

	return result;
}

Basemap::Water::WaterType Basemap::Water::translateWaterType(const std::string& waterType)
{
	const static WaterTypeMap waterTypeMap =
	{
		{"canal", WT_CANAL},
		{"dock", WT_DOCK},
		{"human_made", WT_HUMAN_MADE},
		{"lake", WT_LAKE},
		{"inland", WT_INLAND},
		{"ocean", WT_OCEAN},
		{"pond", WT_POND},
		{"reservoir", WT_RESERVOIR},
		{"river", WT_RIVER},
		{"stream", WT_STREAM},
		{"water", WT_WATER}
	};

	ocean_assert(waterTypeMap.size() == WT_END - 1u);

	const WaterTypeMap::const_iterator iType = waterTypeMap.find(waterType);

	if (iType == waterTypeMap.cend())
	{
		Log::warning() << "Unknown basemap water class: " << waterType;
		return WT_UNKNOWN;
	}

	return iType->second;
}

bool Basemap::TileIndexPair::isLocationClose(const TileIndexPair& tileIndexPair, const double latitudeFraction, const double longitudeFraction, const double maxFraction) const
{
	ocean_assert(isValid());

	ocean_assert(tileIndexPair.isValid());
	ocean_assert(latitudeFraction >= 0.0 && latitudeFraction <= 1.0);
	ocean_assert(longitudeFraction >= 0.0 && longitudeFraction <= 1.0);
	ocean_assert(maxFraction >= 0.0 && maxFraction < 1.0);

	if (*this == tileIndexPair)
	{
		// both tiles are identical
		return true;
	}

	const int tileIndexOffsetLatitude = int(tileIndexPair.latitudeIndex() - latitudeIndex_);
	const int tileIndexOffsetLongitude = int(tileIndexPair.longitudeIndex() - longitudeIndex_);

	if (tileIndexOffsetLatitude <= -2 || tileIndexOffsetLatitude >= 2 || tileIndexOffsetLongitude <= -2 || tileIndexOffsetLongitude >= 2)
	{
		// there is at least one entire tile between both tiles
		return false;
	}

	double adjustedLatitudeFraction = 0.0;

	if (tileIndexOffsetLatitude > 0)
	{
		adjustedLatitudeFraction = latitudeFraction;
	}
	else if (tileIndexOffsetLatitude < 0)
	{
		adjustedLatitudeFraction = 1.0 - latitudeFraction;
	}

	double adjustedLongitudeFraction = 0.0;

	if (tileIndexOffsetLongitude > 0)
	{
		adjustedLongitudeFraction = longitudeFraction;
	}
	else if (tileIndexOffsetLongitude < 0)
	{
		adjustedLongitudeFraction = 1.0 - longitudeFraction;
	}

	ocean_assert(adjustedLatitudeFraction >= 0.0 && adjustedLatitudeFraction <= 1.0);
	ocean_assert(adjustedLongitudeFraction >= 0.0 && adjustedLongitudeFraction <= 1.0);

	return adjustedLatitudeFraction <= maxFraction && adjustedLongitudeFraction <= maxFraction;
}

Basemap::TileIndexPairs Basemap::TileIndexPair::createNeighborhoodTiles(const TileIndexPair& tileIndexPair, const unsigned int numberTilesOnLevel, const unsigned int maxDistance)
{
	ocean_assert(tileIndexPair.isValid());
	ocean_assert(tileIndexPair.isInside(numberTilesOnLevel));

	TileIndexPairs neighborhoodTiles;
	neighborhoodTiles.reserve(9);

	const unsigned int beginLatitudeIndex = (unsigned int)(std::max(0, int(tileIndexPair.latitudeIndex_) - int(maxDistance)));
	const unsigned int endLatitudeIndex = std::min(tileIndexPair.latitudeIndex() + maxDistance + 1u, numberTilesOnLevel);

	const unsigned int beginLongitudeIndex = (unsigned int)(std::max(0, int(tileIndexPair.longitudeIndex_) - int(maxDistance)));
	const unsigned int endLongitudeIndex = std::min(tileIndexPair.longitudeIndex() + maxDistance + 1u, numberTilesOnLevel);

	for (unsigned int laIndex = beginLatitudeIndex; laIndex < endLatitudeIndex; ++laIndex)
	{
		for (unsigned int loIndex = beginLongitudeIndex; loIndex < endLongitudeIndex; ++loIndex)
		{
			neighborhoodTiles.emplace_back(laIndex, loIndex);
		}
	}

	ocean_assert(neighborhoodTiles.size() >= 4 && neighborhoodTiles.size() <= 9);
	return neighborhoodTiles;
}

bool Basemap::Tile::parsePBFData(const void* data, const size_t size)
{
	if (!isValid())
	{
		ocean_assert(false && "Invalid tile!");
		return false;
	}

	if (data == nullptr || size == 0)
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

	try
	{
		const vtzero::vector_tile vectorTile((const char*)(data), size);

		const size_t numberLayers = vectorTile.count_layers();

		GeometryHandlerPoints geometryHandlerPoints;
		GeometryHandlerLineStrings geometryHandlerLineStrings;
		GeometryHandlerLinePolygons geometryHandlerPolygons;

		PixelPositionsI points;
		PixelPositionGroupsI lineStrings;
		PixelPositionGroupsI outerPolygons;
		PixelPositionGroupsI innerPolygons;

		for (size_t n = 0; n < numberLayers; ++n)
		{
			vtzero::layer layer = vectorTile.get_layer(n);

			const std::string layerName = layer.name().to_string();

			Log::debug() << " ";
			Log::debug() << "Layer name: " << layerName;

			const LayerType layerType = translateLayerName(layerName);

			if (layerType == LT_UNKNOWN)
			{
				continue;
			}

			const unsigned int layerExtent = layer.extent();

			if (layerExtent == 0u)
			{
				ocean_assert(false && "Invalid layer extent!");
				continue;
			}

			Log::debug() << "Fatures:";

			while (vtzero::feature vtzeroFeature = layer.next_feature())
			{
#ifdef OCEAN_DEBUG
				while (vtzero::property property = vtzeroFeature.next_property())
				{
					if (property.value().type() == vtzero::property_value_type::string_value)
					{
						Log::debug() << "Property " << property.key().to_string() << ", " << property.value().string_value().to_string();
					}
					else
					{
						Log::debug() << "Property " << property.key().to_string() << ", with type " << int(property.value().type());
					}
				}

				vtzeroFeature.reset_property();
#endif

				points.clear();
				lineStrings.clear();
				outerPolygons.clear();
				innerPolygons.clear();

				const vtzero::GeomType vtzeroGeometryType = vtzeroFeature.geometry_type();

				switch (vtzeroGeometryType)
				{
					case vtzero::GeomType::UNKNOWN:
						continue;

					case vtzero::GeomType::POINT:
					{
						geometryHandlerPoints.reset();
						vtzero::decode_point_geometry(vtzeroFeature.geometry(), geometryHandlerPoints);

						std::swap(points, geometryHandlerPoints.points_);

						break;
					}

					case vtzero::GeomType::LINESTRING:
					{
						geometryHandlerLineStrings.reset();
						vtzero::decode_linestring_geometry(vtzeroFeature.geometry(), geometryHandlerLineStrings);

						std::swap(lineStrings, geometryHandlerLineStrings.lineStrings_);

						break;
					}

					case vtzero::GeomType::POLYGON:
					{
						geometryHandlerPolygons.reset();
						vtzero::decode_polygon_geometry(vtzeroFeature.geometry(), geometryHandlerPolygons);

						std::swap(outerPolygons, geometryHandlerPolygons.outerPolygons_);
						std::swap(innerPolygons, geometryHandlerPolygons.innerPolygons_);

						break;
					}
				}

				Log::debug() << "Points: " << points.size() << ", line strings: " << lineStrings.size() << ", polygons: " << outerPolygons.size() << " + " << innerPolygons.size();

				SharedObject object;

				switch (layerType)
				{
					case LT_BUILDING:
					{
						ocean_assert(points.empty());

						object = parseBuilding(vtzeroFeature, std::move(outerPolygons), std::move(innerPolygons), std::move(lineStrings), layerExtent);
						break;
					}

					case LT_ROAD:
					{
						if (!lineStrings.empty())
						{
							ocean_assert(points.empty());
							ocean_assert(outerPolygons.empty());
							ocean_assert(innerPolygons.empty());

							object = parseRoad(vtzeroFeature, std::move(lineStrings), layerExtent);
						}
						else
						{
							ocean_assert(!outerPolygons.empty());

							Log::debug() << "Skipped road as it is not composed of line strings";
						}

						break;
					}

					case LT_TRANSIT:
					{
						if (!lineStrings.empty())
						{
							ocean_assert(outerPolygons.empty());
							ocean_assert(innerPolygons.empty());
							ocean_assert(points.empty());

							object = parseTransit(vtzeroFeature, std::move(lineStrings), layerExtent);
						}
						else
						{
							// transit locations - e.g., bus stops, currently not handled

							ocean_assert(!outerPolygons.empty() || !points.empty());

							Log::debug() << "Skipping transit location";
						}

						break;
					}

					case LT_WATER:
					{
						ocean_assert(points.empty());
						ocean_assert(lineStrings.empty());

						object = parseWater(vtzeroFeature, std::move(outerPolygons), std::move(innerPolygons), layerExtent);
						break;
					}

					case LT_LAND_USE:
					{
						ocean_assert(points.empty());

						object = parseLandUse(vtzeroFeature, std::move(outerPolygons), std::move(innerPolygons), std::move(lineStrings), layerExtent);
						break;
					}

					case LT_LAND_COVER:
					{
						ocean_assert(points.empty());

						object = parseLandCover(vtzeroFeature, std::move(outerPolygons), std::move(innerPolygons), std::move(lineStrings), layerExtent);
						break;
					}

					default:
						break;
				}

				if (object)
				{
					objects_.emplace_back(std::move(object));
				}
			}
		}

		return true;
	}
	catch (...)
	{
		// nothing to do here
	}

	return false;
}

void Basemap::Tile::tileCoordinate2GPSLocation(const PixelPositionI& position, const unsigned int layerExtent, double& latitude, double& longitude) const
{
	ocean_assert(isValid());
	tileCoordinate2GPSLocation(level_, tileIndexPair_, position, layerExtent, latitude, longitude);
}

double Basemap::Tile::metricExtent(const double earthRadius) const
{
	ocean_assert(isValid());
	ocean_assert(earthRadius > NumericD::eps());

	double latitude = NumericD::minValue();
	double longitude = NumericD::minValue();
	tileCoordinate2GPSLocation(PixelPositionI(2048, 2048), 4096u, latitude, longitude);

	const double clampedLatitude = minmax(minLatitude, latitude, maxLatitude);

	ocean_assert(numberTiles(level_) >= 1u);
	return NumericD::cos(NumericD::deg2rad(clampedLatitude)) * earthRadius * NumericD::pi2() / double(numberTiles(level_));
}

void Basemap::Tile::clear()
{
	objects_.clear();
}

Basemap::TileIndexPair Basemap::Tile::calculateTile(const unsigned int level, const double latitude, const double longitude, double* latitudeTileFraction, double* longitudeTileFraction)
{
	ocean_assert(level >= 1u && level < 22u);
	ocean_assert(latitude >= -90.0 && latitude <= 90.0);
	ocean_assert(longitude >= -180.0 && longitude <= 180.0);

	const double latitudeTileNormalized = calculateNormalizedTileLatitude(latitude);
	const double longitudeTileNormalized = calculateNormalizedTileLongitude(longitude);

	const unsigned int tiles = numberTiles(level);

	unsigned int latitudeIndex = (unsigned int)(double(tiles) * latitudeTileNormalized);
	unsigned int longitudeIndex = (unsigned int)(double(tiles) * longitudeTileNormalized);

	ocean_assert(latitudeIndex < tiles);
	ocean_assert(longitudeIndex < tiles);

	if (latitudeTileFraction)
	{
		const double tileFactor = double(tiles) * latitudeTileNormalized;

		const double fraction = tileFactor - NumericD::floor(tileFactor);
		ocean_assert(fraction >= 0.0 && fraction < 1.0);

		*latitudeTileFraction = fraction;
	}

	if (longitudeTileFraction)
	{
		const double tileFactor = double(tiles) * longitudeTileNormalized;

		const double fraction = tileFactor - NumericD::floor(tileFactor);
		ocean_assert(fraction >= 0.0 && fraction < 1.0);

		*longitudeTileFraction = fraction;
	}

	return TileIndexPair(latitudeIndex, longitudeIndex);
}

void Basemap::Tile::calculateTileFractions(const unsigned int level, const double latitude, const double longitude, const TileIndexPair& tileIndexPair, double& latitudeTileFraction, double& longitudeTileFraction)
{
	ocean_assert(level >= 1u && level < 22u);
	ocean_assert(latitude >= -90.0 && latitude <= 90.0);
	ocean_assert(longitude >= -180.0 && longitude <= 180.0);
	ocean_assert(tileIndexPair.isValid());

	const unsigned int tiles = numberTiles(level);

	ocean_assert(tileIndexPair.latitudeIndex() < tiles);
	ocean_assert(tileIndexPair.longitudeIndex() < tiles);

	const double latitudeTileNormalized = calculateNormalizedTileLatitude(latitude);
	const double longitudeTileNormalized = calculateNormalizedTileLongitude(longitude);

	latitudeTileFraction = double(tiles) * latitudeTileNormalized - double(tileIndexPair.latitudeIndex());
	longitudeTileFraction = double(tiles) * longitudeTileNormalized - double(tileIndexPair.longitudeIndex());
}

void Basemap::Tile::tileCoordinate2GPSLocation(const unsigned int level, const TileIndexPair& tileIndexPair, const PixelPositionI& position, const unsigned int layerExtent, double& latitude, double& longitude)
{
	ocean_assert(level >= 1u && level <= 22u);
	ocean_assert(tileIndexPair.isValid());
	ocean_assert(tileIndexPair.latitudeIndex() < numberTiles(level) && tileIndexPair.longitudeIndex() < numberTiles(level));

	ocean_assert(position.isValid());
	ocean_assert(layerExtent >= 1u);

	const double invLayerExtent = 1.0 / double(layerExtent);

	const unsigned int tiles = numberTiles(level);

	longitude = double(360.0) * (double(tileIndexPair.longitudeIndex()) + double(position.x()) * invLayerExtent) / double(tiles) - 180.0;

	const double yFactor = 0.5 - (double(tileIndexPair.latitudeIndex()) + double(position.y()) * invLayerExtent) / double(tiles);
	latitude = 90.0 - 360.0 * NumericD::atan(NumericD::exp(-yFactor * NumericD::pi2())) / NumericD::pi();

	ocean_assert(latitude >= -90.0 && latitude <= 90.0);
	ocean_assert(longitude >= -180.0 && longitude <= 180.0);
}

double Basemap::Tile::calculateNormalizedTileLatitude(const double latitude)
{
	ocean_assert(latitude >= -90.0 && latitude <= 90.0);

	const double clampedLatitude = minmax(minLatitude, latitude, maxLatitude);

	const double sinLatitude = NumericD::sin(NumericD::deg2rad(clampedLatitude)); // WGS-84 elipsolid
	ocean_assert(NumericD::isNotEqualEps(1.0 - sinLatitude));

	double latitudeTileNormalized = (0.5 - NumericD::log((1.0 + sinLatitude) / (1.0 - sinLatitude)) / (NumericD::pi2() * 2.0));
	if (latitudeTileNormalized >= 1.0)
	{
		latitudeTileNormalized -= 1.0;
	}

	ocean_assert(latitudeTileNormalized >= 0.0 && latitudeTileNormalized < 1.0);

	return latitudeTileNormalized;
}

double Basemap::Tile::calculateNormalizedTileLongitude(const double longitude)
{
	ocean_assert(longitude >= -180.0 && longitude <= 180.0);

	double longitudeTileNormalized = (180.0 + longitude) / 360.0;
	if (longitudeTileNormalized >= 1.0)
	{
		longitudeTileNormalized -= 1.0;
	}

	ocean_assert(longitudeTileNormalized >= 0.0 && longitudeTileNormalized < 1.0);

	return longitudeTileNormalized;
}

void Basemap::GeometryHandlerPoints::points_begin(uint32_t count)
{
	ocean_assert(count >= 1u);
	ocean_assert(points_.empty());

	points_.reserve(count);
}

void Basemap::GeometryHandlerPoints::points_point(vtzero::point point)
{
	points_.emplace_back(point.x, point.y);
}

void Basemap::GeometryHandlerPoints::points_end()
{
	// nothing to do here
}

void Basemap::GeometryHandlerPoints::reset()
{
	points_.clear();
}

void Basemap::GeometryHandlerLinePolygons::ring_begin(uint32_t count)
{
	ocean_assert(count >= 1u);
	ocean_assert(intermediatePolygons_.empty());

	intermediatePolygons_.reserve(count);
}

void Basemap::GeometryHandlerLinePolygons::ring_point(vtzero::point point)
{
	intermediatePolygons_.emplace_back(point.x, point.y);
}

void Basemap::GeometryHandlerLinePolygons::ring_end(vtzero::ring_type ringType)
{
	ocean_assert(intermediatePolygons_.size() >= 2);
	ocean_assert(intermediatePolygons_.front() == intermediatePolygons_.back());

	switch (ringType)
	{
		case vtzero::ring_type::outer:
			intermediatePolygons_.pop_back();
			outerPolygons_.emplace_back(std::move(intermediatePolygons_));
			ocean_assert(intermediatePolygons_.empty());
			break;

		case vtzero::ring_type::inner:
			intermediatePolygons_.pop_back();
			innerPolygons_.emplace_back(std::move(intermediatePolygons_));
			ocean_assert(intermediatePolygons_.empty());
			break;

		case vtzero::ring_type::invalid:
			intermediatePolygons_.clear();
			break;

		default:
			ocean_assert(false && "Invalid type!");
	}
}

void Basemap::GeometryHandlerLinePolygons::reset()
{
	intermediatePolygons_.clear();
	outerPolygons_.clear();
	innerPolygons_.clear();
}

void Basemap::GeometryHandlerLineStrings::linestring_begin(uint32_t count)
{
	ocean_assert(count >= 1u);

	lineStrings_.emplace_back();
	lineStrings_.back().reserve(count);
}

void Basemap::GeometryHandlerLineStrings::linestring_point(vtzero::point point)
{
	lineStrings_.back().emplace_back(point.x, point.y);
}

void Basemap::GeometryHandlerLineStrings::linestring_end()
{
	// nothing to do here
}

void Basemap::GeometryHandlerLineStrings::reset()
{
	lineStrings_.clear();
}

Basemap::SharedTile Basemap::newTileFromPBFData(const unsigned int level, const TileIndexPair& tileIndexPair, const void* data, const size_t size)
{
	if (level < 1u || level > 22u)
	{
		ocean_assert(false && "Invalid level!");
		return nullptr;
	}

	const unsigned int tiles = Tile::numberTiles(level);

	if (!tileIndexPair.isValid() || tileIndexPair.latitudeIndex() >= tiles || tileIndexPair.longitudeIndex() >= tiles)
	{
		ocean_assert(false && "Invalid tile indices!");
		return nullptr;
	}

	if (data == nullptr || size == 0)
	{
		ocean_assert(false && "Invalid data!");
		return nullptr;
	}

	Tile newTile(level, tileIndexPair);
	ocean_assert(newTile.isValid());

	if (!newTile.parsePBFData(data, size))
	{
		Log::error() << "Failed to parse the given PBF data";
		return nullptr;
	}

	return std::make_shared<Tile>(std::move(newTile));
}

Basemap::SharedObject Basemap::parseBuilding(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent)
{
	if (outerPolygons.empty() && innerPolygons.empty() && lineStrings.empty())
	{
		return nullptr;
	}

	ocean_assert((outerPolygons.empty() && innerPolygons.empty()) || lineStrings.empty());

	double buildingHeight = -1.0;

	while (vtzero::property vtzeroProperty = vtzeroFeature.next_property())
	{
		ocean_assert(vtzeroProperty.valid());

		if (vtzeroProperty.key().compare("building_height") == 0)
		{
			ocean_assert(buildingHeight == -1.0);

			double value;
			if (numberFromPropertyValue(vtzeroProperty.value(), value))
			{
				buildingHeight = value;

				// no additional properties are of interest
				break;
			}
		}
	}

	return std::make_shared<Building>(std::move(outerPolygons), std::move(innerPolygons), std::move(lineStrings), Scalar(buildingHeight), layerExtent);
}

Basemap::SharedObject Basemap::parseRoad(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent)
{
	if (lineStrings.empty())
	{
		return nullptr;
	}

	std::string name;
	Road::RoadType roadType = Road::RT_UNKNOWN;

	while (vtzero::property vtzeroProperty = vtzeroFeature.next_property())
	{
		ocean_assert(vtzeroProperty.valid());

		if (vtzeroProperty.key().compare("name") == 0)
		{
			if (vtzeroProperty.value().type() == vtzero::property_value_type::string_value)
			{
				ocean_assert(name.empty());
				name = vtzeroProperty.value().string_value().to_string();
			}
		}

		if (vtzeroProperty.key().compare("class") == 0)
		{
			ocean_assert(roadType == Road::RT_UNKNOWN);

			if (vtzeroProperty.value().type() == vtzero::property_value_type::string_value)
			{
				const vtzero::data_view value = vtzeroProperty.value().string_value();

				roadType = Road::translateRoadType(std::string(value));
			}
		}
	}

	if (roadType == Road::RT_UNKNOWN)
	{
		return nullptr;
	}

	return std::make_shared<Road>(roadType, std::move(name), std::move(lineStrings), layerExtent);
}

Basemap::SharedObject Basemap::parseTransit(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent)
{
	if (lineStrings.empty())
	{
		return nullptr;
	}

	std::string name;
	Transit::TransitType transitType = Transit::TT_UNKNOWN;

	while (vtzero::property vtzeroProperty = vtzeroFeature.next_property())
	{
		ocean_assert(vtzeroProperty.valid());

		if (vtzeroProperty.key().compare("name") == 0)
		{
			if (vtzeroProperty.value().type() == vtzero::property_value_type::string_value)
			{
				ocean_assert(name.empty());
				name = vtzeroProperty.value().string_value().to_string();
			}
		}

		if (vtzeroProperty.key().compare("class") == 0)
		{
			ocean_assert(transitType == Transit::TT_UNKNOWN);

			if (vtzeroProperty.value().type() == vtzero::property_value_type::string_value)
			{
				const vtzero::data_view value = vtzeroProperty.value().string_value();

				if (value.compare("aerialway") == 0)
				{
					transitType = Transit::TT_AERIALWAY;
				}
				else if (value.compare("railway") == 0)
				{
					transitType = Transit::TT_RAILWAY;
				}
				else if (value.compare("ferry_route") == 0)
				{
					transitType = Transit::TT_FERRY;
				}
				else
				{
					ocean_assert(false && "Invalid type!");
				}
			}
		}
	}

	if (transitType == Transit::TT_UNKNOWN)
	{
		return nullptr;
	}

	return std::make_shared<Transit>(transitType, std::move(name), std::move(lineStrings), layerExtent);
}

Basemap::SharedObject Basemap::parseWater(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, const unsigned int layerExtent)
{
	if (outerPolygons.empty() && innerPolygons.empty())
	{
		return nullptr;
	}

	Water::WaterType waterType = Water::WT_UNKNOWN;

	while (vtzero::property vtzeroProperty = vtzeroFeature.next_property())
	{
		ocean_assert(vtzeroProperty.valid());

		if (vtzeroProperty.key().compare("class") == 0)
		{
			ocean_assert(waterType == Water::WT_UNKNOWN);

			if (vtzeroProperty.value().type() == vtzero::property_value_type::string_value)
			{
				const vtzero::data_view value = vtzeroProperty.value().string_value();

				waterType = Water::translateWaterType(std::string(value));
			}
		}
	}

	if (waterType == Water::WT_UNKNOWN)
	{
		return nullptr;
	}

	return std::make_shared<Water>(waterType, std::move(outerPolygons), std::move(innerPolygons), layerExtent);
}

Basemap::SharedObject Basemap::parseLandUse(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent)
{
	if (outerPolygons.empty() && innerPolygons.empty())
	{
		return nullptr;
	}

	LandUse::LandUseType landUseType = LandUse::LUT_UNKNOWN;

	while (vtzero::property vtzeroProperty = vtzeroFeature.next_property())
	{
		ocean_assert(vtzeroProperty.valid());

		if (vtzeroProperty.key().compare("class") == 0)
		{
			ocean_assert(landUseType == LandUse::LUT_UNKNOWN);

			if (vtzeroProperty.value().type() == vtzero::property_value_type::string_value)
			{
				const vtzero::data_view value = vtzeroProperty.value().string_value();

				if (value.compare("airport") == 0)
				{
					landUseType = LandUse::LUT_AIRPORT;
				}
				else if (value.compare("amusement") == 0)
				{
					landUseType = LandUse::LUT_AMUSEMENT;
				}
				else if (value.compare("education") == 0)
				{
					landUseType = LandUse::LUT_EDUCATION;
				}
				else if (value.compare("greenspace") == 0)
				{
					landUseType = LandUse::LUT_GREENSPACE;
				}
				else if (value.compare("land") == 0)
				{
					landUseType = LandUse::LUT_LAND;
				}
				else if (value.compare("national_park") == 0)
				{
					landUseType = LandUse::LUT_NATIONAL_PARK;
				}
				else if (value.compare("plaza") == 0)
				{
					landUseType = LandUse::LUT_PLAZA;
				}
				else if (value.compare("recreation") == 0)
				{
					landUseType = LandUse::LUT_RECREATION;
				}
				else
				{
					Log::warning() << "Unknown land use type: " << value.to_string();
				}
			}
		}
	}

	return std::make_shared<LandUse>(landUseType, std::move(outerPolygons), std::move(innerPolygons), std::move(lineStrings), layerExtent);
}

Basemap::SharedObject Basemap::parseLandCover(vtzero::feature& vtzeroFeature, PixelPositionGroupsI&& outerPolygons, PixelPositionGroupsI&& innerPolygons, PixelPositionGroupsI&& lineStrings, const unsigned int layerExtent)
{
	if (outerPolygons.empty() && innerPolygons.empty())
	{
		return nullptr;
	}

	LandCover::LandCoverType landCoverType = LandCover::LCT_UNKNOWN;

	while (vtzero::property vtzeroProperty = vtzeroFeature.next_property())
	{
		ocean_assert(vtzeroProperty.valid());

		if (vtzeroProperty.key().compare("class") == 0)
		{
			ocean_assert(landCoverType == LandCover::LCT_UNKNOWN);

			if (vtzeroProperty.value().type() == vtzero::property_value_type::string_value)
			{
				const vtzero::data_view value = vtzeroProperty.value().string_value();

				if (value.compare("grass") == 0)
				{
					landCoverType = LandCover::LCT_GRASS;
				}
				else if (value.compare("sand") == 0)
				{
					landCoverType = LandCover::LCT_SAND;
				}
				else if (value.compare("paved") == 0)
				{
					landCoverType = LandCover::LCT_PAVED;
				}
				else
				{
					Log::warning() << "Unknown land use type: " << value.to_string();
				}
			}
		}
	}

	return std::make_shared<LandCover>(landCoverType, std::move(outerPolygons), std::move(innerPolygons), std::move(lineStrings), layerExtent);
}

bool Basemap::numberFromPropertyValue(const vtzero::property_value& vtzeroPropertyValue, double& value)
{
	switch (vtzeroPropertyValue.type())
	{
		case vtzero::property_value_type::double_value:
			value = vtzeroPropertyValue.double_value();
			return true;

		case vtzero::property_value_type::string_value:
			return String::isNumber(vtzeroPropertyValue.string_value().to_string(), true, &value);

		case vtzero::property_value_type::float_value:
			value = double(vtzeroPropertyValue.float_value());
			return true;

		case vtzero::property_value_type::int_value:
			value = double(vtzeroPropertyValue.int_value());
			return true;

		case vtzero::property_value_type::sint_value:
			value = double(vtzeroPropertyValue.sint_value());
			return true;

		case vtzero::property_value_type::uint_value:
			value = double(vtzeroPropertyValue.uint_value());
			return true;

		default:
			break;
	}

	return false;
}

Basemap::LayerType Basemap::translateLayerName(const std::string& layerName)
{
	const static LayerTypeMap layerTypeMap =
	{
		{"airport", LT_AIRPORT},
		{"aoi", LT_AREA_OF_INTEREST},
		{"bathymetry", LT_BATHYMETRY},
		{"building", LT_BUILDING},
		{"building_label", LT_BUILDING_LABEL},
		{"border", LT_BORDER},
		{"indoor", LT_INDOOR},
		{"indoor_label", LT_INDOOR_LABEL},
		{"land_cover", LT_LAND_COVER},
		{"land_use", LT_LAND_USE},
		{"landuse_label", LT_LAND_USE_LABEL},
		{"landmark_point", LT_LANDMARK_POINT},
		{"natural", LT_NATURAL},
		{"natural_label", LT_NATURAL_LABEL},
		{"parking", LT_PARKING},
		{"placename", LT_PLACENAME},
		{"place_label", LT_PLACE_LABEL},
		{"poi", LT_POI},
		{"road", LT_ROAD},
		{"transit", LT_TRANSIT},
		{"transit_point", LT_TRANSIT_POINT},
		{"tree_point", LT_TREE_POINT},
		{"water", LT_WATER},
		{"waterway", LT_WATERWAY},
		{"water_label", LT_WATER_LABEL},
		{"water_line", LT_WATER_LINE},
		{"water_offset", LT_WATER_OFFSET},
		{"wave", LT_WAVE},
	};

	ocean_assert(layerTypeMap.size() == LT_END - 1u);

	const LayerTypeMap::const_iterator iType = layerTypeMap.find(layerName);

	if (iType == layerTypeMap.cend())
	{
		Log::warning() << "Unknown layer type: " << layerName;
		return LT_UNKNOWN;
	}

	return iType->second;
}

const std::string& Basemap::styleUrl()
{
	static std::string url = "https://facebook.com/maps/vt/style/canterbury_1_0/";
	return url;
}

bool Basemap::extractTileUrlTemplate(const char* styleData, const size_t styleSize, std::string& urlTemplate)
{
	ocean_assert(styleData != nullptr && styleSize != 0);

	if (styleData == nullptr || styleSize == 0)
	{
		return false;
	}

	const std::string styleString(styleData, styleSize);

	std::shared_ptr<std::stringstream> jsonStream = std::make_shared<std::stringstream>(styleString);

	JSONConfig json(jsonStream);

	if (json.exist("sources"))
	{
		JSONConfig::JSONValue& sourceValue = json.value("sources", 0);

		if (sourceValue.exist("facebook"))
		{
			JSONConfig::JSONValue& facebookValue = sourceValue.value("facebook", 0);

			if (facebookValue.exist("tiles"))
			{
				JSONConfig::JSONValue& tilesValue = facebookValue.value("tiles", 0);

				urlTemplate = tilesValue(std::string());

				return urlTemplate.length() > 0;
			}
		}
	}

	return false;
}

bool Basemap::constructTileUrl(const std::string urlTemplate, const unsigned int level, const TileIndexPair& tileIndexPair, std::string& url)
{
	if (level < 1u || level > 22u)
	{
		ocean_assert(false && "Invalid level!");
		return false;
	}

	const unsigned int tiles = Tile::numberTiles(level);

	if (!tileIndexPair.isValid() || tileIndexPair.latitudeIndex() >= tiles || tileIndexPair.longitudeIndex() >= tiles)
	{
		ocean_assert(false && "Invalid tile indices!");
		return false;
	}

	const std::regex reX("\\{x\\}");
	const std::regex reY("\\{y\\}");
	const std::regex reZ("\\{z\\}");
	if (!std::regex_search(urlTemplate, reX) || ! std::regex_search(urlTemplate, reY) || ! std::regex_search(urlTemplate, reZ))
	{
		ocean_assert(false && "Invalid URL template!");
		return false;
	}

	std::string tempUrl = std::regex_replace(urlTemplate, reX, std::to_string(tileIndexPair.longitudeIndex()));
	tempUrl = std::regex_replace(tempUrl, reY, std::to_string(tileIndexPair.latitudeIndex()));
	url = std::regex_replace(tempUrl, reZ, std::to_string(level));
	return true;
}

}

}

}
