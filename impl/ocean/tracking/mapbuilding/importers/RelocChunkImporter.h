// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_RELOC_CHUNK_IMPORTER_H
#define META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_RELOC_CHUNK_IMPORTER_H

#include "ocean/tracking/mapbuilding/importers/Importers.h"
#include "ocean/tracking/mapbuilding/DescriptorHandling.h"
#include "ocean/tracking/mapbuilding/Unified.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

namespace Importers
{

/**
 * This class implements an importer for reloc chunks.
 * @ingroup trackingmapbuildingimporters
 */
class OCEAN_TRACKING_MAPBUILDING_IMPORTERS_EXPORT RelocChunkImporter
{
	public:

		/**
		 * Definition of a vector holding multi-view FREAK descriptors.
		 */
		typedef std::vector<CV::Detector::FREAKDescriptors32> MultiViewFREAKDescriptors32;

		/**
		 * Definition of a float descriptor with 128 elements.
		 */
		typedef std::array<float, 128> FloatDescriptor128;

		/**
		 * Definition of a vector holding a multi-view float descriptor with 128 elements.
		 */
		typedef std::vector<FloatDescriptor128> FloatDescriptors128;

		/**
		 * Definition of a vector holding multi-view float descriptors.
		 */
		typedef std::vector<FloatDescriptors128> MultiViewFloatDescriptors128;

	public:

		/**
		 * Imports a reloc chunk (a feature map) stored in a file.
		 * @param filename The name of the file containing the reloc chunk, must be valid
		 * @param objectPoints The resulting 3D object points of the features
		 * @return True, if succeeded
		 */
		static bool importFromFile(const std::string& filename, Vectors3& objectPoints);

		/**
		 * Imports a reloc chunk (a feature map) stored in a file.
		 * @param filename The name of the file containing the reloc chunk, must be valid
		 * @param objectPoints The resulting 3D object points of the features
		 * @param objectPointIds The resulting ids of the features, one for each 3D object point
		 * @param unifiedDescriptorMap The resulting descriptor map, mapping object point ids to descriptors
		 * @return True, if succeeded
		 */
		static bool importFromFile(const std::string& filename, Vectors3& objectPoints, Indices32& objectPointIds, SharedUnifiedDescriptorMap& unifiedDescriptorMap);

		/**
		 * Deprecated.
		 *
		 * Imports a reloc chunk (a feature map) stored in a file.
		 * @param filename The name of the file containing the reloc chunk, must be valid
		 * @param objectPoints The resulting 3D object points of the features
		 * @param multiViewDescriptors The resulting multi-view FREAK descriptors, one multi-view descriptor for each object point
		 * @return True, if succeeded
		 */
		static bool importFromFile(const std::string& filename, Vectors3& objectPoints, MultiViewFREAKDescriptors32& multiViewDescriptors);

		/**
		 * Deprecated.
		 *
		 * Imports a reloc chunk (a feature map) stored in a file.
		 * @param filename The name of the file containing the reloc chunk, must be valid
		 * @param objectPoints The resulting 3D object points of the features
		 * @param multiViewDescriptors The resulting multi-view float descriptors, one multi-view descriptor for each object point
		 * @return True, if succeeded
		 */
		static bool importFromFile(const std::string& filename, Vectors3& objectPoints, MultiViewFloatDescriptors128& multiViewDescriptors);

		/**
		 * Deprecated.
		 *
		 * Imports a reloc chunk (a feature map) stored in a file.
		 * @param filename The name of the file containing the reloc chunk, must be valid
		 * @param objectPoints The resulting 3D object points of the features
		 * @param objectPointIds The resulting ids of the features, one for each 3D object point
		 * @param descriptorMap The resulting descriptor map mapping objectIds to multi-view descriptors
		 * @return True, if succeeded
		 */
		static bool importFromFile(const std::string& filename, Vectors3& objectPoints, Indices32& objectPointIds, UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& descriptorMap);
};

}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_RELOC_CHUNK_IMPORTER_H
