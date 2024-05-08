/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UTILITIES_H
#define META_OCEAN_TRACKING_MAPBUILDING_UTILITIES_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/DescriptorHandling.h"
#include "ocean/tracking/mapbuilding/Unified.h"

#include "ocean/io/Bitstream.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements utility functions.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT Utilities : public DescriptorHandling
{
	public:

		/**
		 * Writes a descriptor map to an output stream.
		 * @param unifiedDescriptorMap The descriptor map to write
		 * @param outputStream The output stream to which the map will be written
		 * @return True, if succeeded
		 */
		static bool writeDescriptorMap(const UnifiedDescriptorMap& unifiedDescriptorMap, IO::OutputBitstream& outputStream);

		/**
		 * Reads a descriptor map from an output stream.
		 * @param inputStream The stream from which the map will be read
		 * @param unifiedDescriptorMap The resulting descriptor map
		 * @return True, if succeeded
		 */
		static bool readDescriptorMap(IO::InputBitstream& inputStream, std::shared_ptr<UnifiedDescriptorMap>& unifiedDescriptorMap);
};

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_UTILITIES_H
