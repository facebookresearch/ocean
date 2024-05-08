/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_MAPS_UTILITIES_H
#define META_OCEAN_IO_MAPS_UTILITIES_H

#include "ocean/io/maps/Maps.h"
#include "ocean/io/maps/Basemap.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace IO
{

namespace Maps
{

/**
 * This class implements several utility functions for maps.
 * @ingroup iomaps
 */
class OCEAN_IO_MAPS_EXPORT Utilities
{
	public:

		/**
		 * Draws one basemap tile into an image.
		 * @param tile The tile to draw, must be valid
		 * @param imageExtent The image size in horizontal and vertical direction
		 * @return The resulting image
		 */
		static Frame drawToImage(const Basemap::Tile& tile, const unsigned int imageExtent = 4096u);
};

}

}

}

#endif // META_OCEAN_IO_MAPS_UTILITIES_H
