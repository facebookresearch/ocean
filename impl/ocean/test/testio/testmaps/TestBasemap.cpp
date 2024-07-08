/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testmaps/TestBasemap.h"

#include "ocean/network/HTTPSClient.h"

#include <regex>
#include <string>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestMaps
{

using namespace IO::Maps;

bool TestBasemap::test(const double testDuration)
{
	Log::info() << "Basemap test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testTileFromPBFData() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMissingFeatures() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Basemap test succeeded.";
	}
	else
	{
		Log::info() << "Basemap test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBasemap, TileFromPBFData)
{
	EXPECT_TRUE(TestBasemap::testTileFromPBFData());
}

TEST(TestBasemap, MissingFeatures)
{
	EXPECT_TRUE(TestBasemap::testMissingFeatures());
}

#endif // OCEAN_USE_GTEST

bool TestBasemap::testTileFromPBFData()
{
	Log::info() << "Tile from PBF data test:";
	Log::info() << " ";

	bool allSucceeded = true;

	// hard-coded location in Seattle downtown

	constexpr double latitude = 47.608666414402364;
	constexpr double longitude = -122.33377211776998;

	constexpr static unsigned int tileLevel = 16u;

	const Basemap::TileIndexPair tileIndexPair = Basemap::Tile::calculateTile(tileLevel, latitude, longitude);

	if (tileIndexPair.isValid())
	{
		std::string url;
		if (getTileUrl(tileLevel, tileIndexPair, url))
		{
			std::vector<uint8_t> buffer;
			if (httpRequest(url, buffer))
			{
				const Basemap::SharedTile tile = Basemap::newTileFromPBFData(tileLevel, tileIndexPair, buffer.data(), buffer.size());

				if (tile)
				{
					size_t numberBuildings = 0;
					size_t numberBuildingsWithDefaultHeight = 0;
					size_t numberBuildingsWithCorrectHeight = 0;

					size_t numberRoads = 0;
					size_t numberTransits = 0;
					size_t numberUnknowns = 0;

					size_t numberLandCover = 0;
					size_t numberLandUse = 0;
					size_t numberWater = 0;

					for (const Basemap::SharedObject& object : tile->objects())
					{
						switch (object->objectType())
						{
							case Basemap::Object::OT_BUILDING:
							{
								++numberBuildings;

								const Basemap::Building& building = (const Basemap::Building&)(*object);

								if (building.height() > 3.42)
								{
									++numberBuildingsWithCorrectHeight;
								}
								else if (building.height() == 3.42) // the default size in case the building height is provided but without "correct" height
								{
									++numberBuildingsWithDefaultHeight;
								}

								break;
							}

							case Basemap::Object::OT_ROAD:
							{
								++numberRoads;
								break;
							}

							case Basemap::Object::OT_TRANSIT:
							{
								++numberTransits;
								break;
							}

							case Basemap::Object::OT_LAND_COVER:
							{
								++numberLandCover;
								break;
							}

							case Basemap::Object::OT_LAND_USE:
							{
								++numberLandUse;
								break;
							}

							case Basemap::Object::OT_WATER:
							{
								++numberWater;
								break;
							}

							case Basemap::Object::OT_UNKNOWN:
							{
								++numberUnknowns;
								break;
							}
						}
					}

					if (numberBuildings < 150 || numberBuildings > 200) // 165 expected
					{
						allSucceeded = false;
					}

					if (numberBuildings > 0)
					{
						if (double(numberBuildingsWithCorrectHeight) / double(numberBuildings) < 0.75) // we want at least 75% buildings with correct height
						{
							allSucceeded = false;
						}
					}

					if (numberBuildings != numberBuildingsWithCorrectHeight + numberBuildingsWithDefaultHeight)
					{
						allSucceeded = false;
					}

					if (numberRoads < 25 || numberRoads > 45) // 32 expected
					{
						allSucceeded = false;
					}

					if (numberTransits < 1 || numberTransits > 5) // 1 expected
					{
						allSucceeded = false;
					}

					if (numberUnknowns != 0)
					{
						allSucceeded = false;
					}

					Log::info() << "The tile contains:";
					Log::info() << "Buildings: " << numberBuildings << ", with correct height: " << numberBuildingsWithCorrectHeight << ", with default height: " << numberBuildingsWithDefaultHeight;
					Log::info() << "Roads: " << numberRoads;
					Log::info() << "Transits: " << numberTransits;
					Log::info() << "Land use: " << numberLandUse;
					Log::info() << "Land cover: " << numberLandCover;
					Log::info() << "Water: " << numberWater;
					Log::info() << "Unknowns: " << numberUnknowns;
				}
				else
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}
	else
	{
		allSucceeded = false;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestBasemap::testMissingFeatures()
{
	Log::info() << "Testing missing features:";
	Log::info() << " ";

	// several hardcoded locations at which we download tiles
	const Vectors2 locations =
	{
		Vector2(47.62730662770595, -122.34617316300015),
		Vector2(47.65422427341229, -122.4132419989625),
		Vector2(47.667117352982295, -122.39385988389796),
		Vector2(47.61310633078666, -122.20027801696367),
		Vector2(47.57373181158289, -122.35174565691246),
		Vector2(47.59609591968932, -122.3326965269414),
		Vector2(47.584568395303954, -122.33058858575951),
		Vector2(47.44981990967438, -122.30791461255407),
		Vector2(47.42537651442893, -121.41230665979982),
		Vector2(33.9804691394831, -118.45652225965496),
		Vector2(34.008523784117855, -118.49893084870432),
		Vector2(33.81247503014904, -117.91981105911344),
		Vector2(-34.65926110008644, -58.52334891746847),
		Vector2(-77.83845287337849, 166.78560379154496),
		Vector2(-43.01860519560414, 171.60345002697844),
		Vector2(52.39773321574908, 13.070069118851501),
		Vector2(51.49960052218123, -0.14080983236905248)
	};

	unsigned int numberTiles = 0u;
	unsigned int numberTilesSucceeded = 0u;

	for (const Vector2& location : locations)
	{
		const double latitude = location.x();
		const double longitude = location.y();

		for (const unsigned int tileLevel : {15u, 16u, 17u, 18u})
		{
			++ numberTiles;

			const Basemap::TileIndexPair tileIndexPair = Basemap::Tile::calculateTile(tileLevel, latitude, longitude);

			if (tileIndexPair.isValid())
			{
				std::string url;
				if (getTileUrl(tileLevel, tileIndexPair, url))
				{
					std::vector<uint8_t> buffer;
					if (httpRequest(url, buffer))
					{
						const Basemap::SharedTile tile = Basemap::newTileFromPBFData(tileLevel, tileIndexPair, buffer.data(), buffer.size());

						if (tile)
						{
							++numberTilesSucceeded;
						}
					}
				}
			}
		}
	}

	const bool allSucceeded = numberTiles == numberTilesSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded to load " << numberTilesSucceeded << " tiles";
	}
	else
	{
		Log::info() << "Validation: FAILED to load " << (numberTiles - numberTilesSucceeded) << " of " << numberTiles << " tiles";
	}

	return allSucceeded;
}

bool TestBasemap::httpRequest(const std::string& url, std::vector<uint8_t>& response)
{
	return Network::HTTPSClient::httpsGetRequest(url, response) && !response.empty();
}

bool TestBasemap::getTileUrl(unsigned int level, const Basemap::TileIndexPair& tileIndexPair, std::string& url)
{
	const std::string styleUrl = IO::Maps::Basemap::styleUrl();
	std::vector<uint8_t> styleBuffer;

	if (httpRequest(styleUrl, styleBuffer))
	{
		std::string urlTemplate;
		if (IO::Maps::Basemap::extractTileUrlTemplate(reinterpret_cast<const char*>(styleBuffer.data()), styleBuffer.size(), urlTemplate))
		{
			std::regex pattern("external.xx.fbcdn.net");

			if (regex_search(urlTemplate, pattern))
			{
				urlTemplate = std::regex_replace(urlTemplate, pattern, "facebook.com");

				IO::Maps::Basemap::constructTileUrl(urlTemplate, level, tileIndexPair, url);
				return true;
			}
		}
	}

	return false;
}

} // namespace TestMaps

} // namespace TestIO

} // namespace Test

} // namespace Ocean
