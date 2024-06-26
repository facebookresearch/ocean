/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_TEST_TESTIO_TESTMAPS_TEST_BASEMAP_H
#define FACEBOOK_OCEAN_TEST_TESTIO_TESTMAPS_TEST_BASEMAP_H

#include "ocean/test/testio/testmaps/TestIOMaps.h"

#include "ocean/io/maps/Basemap.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestMaps
{

/**
 * This class implements a test for the Basemap class.
 * @ingroup testiomaps
 */
class OCEAN_TEST_IO_MAPS_EXPORT TestBasemap
{
	public:

		/**
		 * Tests all functions of the Basemap class.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the newTileFromPBFData functions.
		 * @return True, if succeeded
		 */
		static bool testTileFromPBFData();

		/**
		 * Tests whether the impoter does not read all features.
		 * @return True, if succeeded
		 */
		static bool testMissingFeatures();

	protected:

		/**
		 * Applies an HTTP get request.
		 * @param url The URL of the request
		 * @param response The resulting response
		 * @return True, if succeeded
		 */
		static bool httpRequest(const std::string& url, std::vector<uint8_t>& response);

		/**
		 * Gets the url for a specific tile.
		 * @param level The detail level, with range [1, 22]
		 * @param tileIndexPair The pair with tile indices, must be valid
		 * @param url The resulting url
		 */
		static bool getTileUrl(unsigned int level, const IO::Maps::Basemap::TileIndexPair& tileIndexPair, std::string& url);
};

}

}

}

}

#endif // FACEBOOK_OCEAN_TEST_TESTIO_TESTMAPS_TEST_BASEMAP_H
