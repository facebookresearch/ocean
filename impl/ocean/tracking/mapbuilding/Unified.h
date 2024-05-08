/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_H
#define META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/DescriptorHandling.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptor.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptorMap.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptors.h"
#include "ocean/tracking/mapbuilding/UnifiedMatching.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/Octree.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/tracking/Database.h"
#include "ocean/tracking/VocabularyTree.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This is the base class for all descriptor extractors.
 * @ingroup trackingmapbuilding
 */
class UnifiedDescriptorExtractor
{
	public:

		/**
		 * Destructs the object.
		 */
		virtual ~UnifiedDescriptorExtractor() = default;

		/**
		 * Determines the descriptor for a given image point and adds the descriptor to the map for the corresponding object point.
		 * @param yFramePyramid The image pyramid from which the feature point will be extracted, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param imagePoint The 2D image point of the feature point
		 * @param objectPointId The id of the object point to which the descriptor will be added, must be valid
		 * @param unifiedDescriptorMap The unified descriptor map to which the descriptor will be added
		 * @return True, if succeeded
		 */
		virtual bool createAndAddDescriptor(const CV::FramePyramid& yFramePyramid, const AnyCamera& anyCamera, const Vector2& imagePoint, const Index32& objectPointId, UnifiedDescriptorMap& unifiedDescriptorMap) const = 0;

		/**
		 * Creates a new descriptor map for this extractor.
		 * @return The new descriptor map
		 */
		virtual std::shared_ptr<UnifiedDescriptorMap> createUnifiedDescriptorMap() const = 0;
};

/**
 * This class implements the descriptor extractor for FREAK Multi descriptors with 32 bytes or 256 bits.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT UnifiedDescriptorExtractorFreakMultiDescriptor256 : public UnifiedDescriptorExtractor
{
	public:

		/**
		 * Determines the descriptor for a given image point and adds the descriptor to the map for the corresponding object point.
		 * @see UnifiedDescriptorExtractor::createAndAddDescriptor().
		 */
		bool createAndAddDescriptor(const CV::FramePyramid& yFramePyramid, const AnyCamera& anyCamera, const Vector2& imagePoint, const Index32& objectPointId, UnifiedDescriptorMap& unifiedDescriptorMap) const override;

		/**
		 * Creates a new descriptor map for this extractor.
		 * @return The new descriptor map
		 */
		std::shared_ptr<UnifiedDescriptorMap> createUnifiedDescriptorMap() const override;
};

/**
 * This class implements a helper class for FREAK Multi descriptors with 32 bytes or 256 bits.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT UnifiedHelperFreakMultiDescriptor256 : public DescriptorHandling
{
	public:

		/// The binary descriptor.
		typedef UnifiedDescriptor::BinaryDescriptor<256u> BinaryDescriptor256;

		/// The binary descriptors.
		typedef UnifiedDescriptor::BinaryDescriptors<256u> BinaryDescriptors256;

		/// Definition of the distance value.
		typedef unsigned int BinaryDescriptorDistance;

		/// Definition of a vocabulary forest.
		typedef Tracking::VocabularyForest<BinaryDescriptor256, BinaryDescriptorDistance, UnifiedDescriptorT<BinaryDescriptor256>::determineDistance> BinaryVocabularyForest;

		/// Definition of a vocabulary tree.
		typedef BinaryVocabularyForest::TVocabularyTree BinaryVocabularyTree;

		/// Definition of a descriptor for object points.
		typedef CV::Detector::FREAKDescriptors32 ObjectPointDescriptor;

		/**
		 * Initializes the data structures for guided and unguided matching.
		 * @param objectPoints The 3D object points for matching, may be adjusted internally
		 * @param objectPointIds The ids of the 3D object points, one for each object point, may be adjusted internally
		 * @param descriptorMap The map mapping object point ids to their corresponding descriptors
		 * @param randomGenerator The random generator to be used
		 * @param objectPointDescriptors The resulting descriptors for the 3D object points
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 * @param objectPointDescriptorsForest The resulting forest with object point descriptors
		 * @param objectPointOctree The resulting octree for the 3D object points
		 * @return @return True, if succeeded
		 */
		static bool initialize(Vectors3& objectPoints, Indices32& objectPointIds, const UnifiedDescriptorMap& descriptorMap, RandomGenerator& randomGenerator, BinaryDescriptors256& objectPointDescriptors, Indices32& objectPointIndices, BinaryVocabularyForest& objectPointDescriptorsForest, Geometry::Octree& objectPointOctree);

		/**
		 * Initializes the data structures for guided and unguided matching.
		 * @param database The database holding the topology of the features
		 * @param descriptorMap The map mapping object point ids to their corresponding descriptors
		 * @param randomGenerator The random generator to be used
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the 3D object points, one for each object point
		 * @param objectPointDescriptors The resulting descriptors for the 3D object points
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 * @param objectPointDescriptorsForest The resulting forest with object point descriptors
		 * @param objectPointOctree The resulting octree for the 3D object points
		 * @return @return True, if succeeded
		 */
		static bool initialize(const Tracking::Database& database, const UnifiedDescriptorMap& descriptorMap, RandomGenerator& randomGenerator, Vectors3& objectPoints, Indices32& objectPointIds, BinaryDescriptors256& objectPointDescriptors, Indices32& objectPointIndices, BinaryVocabularyForest& objectPointDescriptorsForest, Geometry::Octree& objectPointOctree);

		/**
		 * Initializes the data structures for guided and unguided matching.
		 * @param database The database holding the topology of the features
		 * @param descriptorMap The map mapping object point ids to their corresponding descriptors
		 * @param randomGenerator The random generator to be used
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the 3D object points, one for each object point
		 * @param objectPointDescriptors The resulting descriptors for the 3D object points
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 * @param objectPointDescriptorsForest The resulting forest with object point descriptors
		 * @param objectPointOctree The resulting octree for the 3D object points
		 * @param unifiedUnguidedMatching The resulting unified unguided matching object
		 * @param unifiedGuidedMatching The resulting unified guided matching object
		 * @return @return True, if succeeded
		 */
		static bool initialize(const Tracking::Database& database, const UnifiedDescriptorMap& descriptorMap, RandomGenerator& randomGenerator, Vectors3& objectPoints, Indices32& objectPointIds, BinaryDescriptors256& objectPointDescriptors, Indices32& objectPointIndices, BinaryVocabularyForest& objectPointDescriptorsForest, Geometry::Octree& objectPointOctree, std::unique_ptr<UnifiedUnguidedMatchingFreakMultiFeatures256Group>& unifiedUnguidedMatching, std::unique_ptr<UnifiedGuidedMatchingFreakMultiDescriptor256Group>& unifiedGuidedMatching);

		/**
		 * Extracts vocabulary descriptors from a descriptor feature map.
		 * This function mainly serializes the descriptors from the map and e.g., separates multi-level/multi-layer descriptors into individual descriptors.
		 * @param descriptorMap The descriptor map containing the features to serialize
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of all 3D object points
		 * @param objectPointIndices The resulting indices of 3D object points, allowing to map extracted vocabulary descriptors to object points (e.g., because the given object point descriptor may be multi-level or/and multi-view descriptors
		 * @param vocabularyDescriptors The resulting descriptors for the vocabulary tree, must be single-view, single-level descriptors
		 */
		static bool extractVocabularyDescriptorsFromMap(const UnifiedDescriptorMap& descriptorMap, Vectors3& objectPoints, Indices32& objectPointIds, Indices32& objectPointIndices, UnifiedDescriptor::ByteDescriptors<32u>& vocabularyDescriptors);
};

/**
 * This class implements a helper class for float descriptors.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT UnifiedHelperFloatSingleLevelMultiView128
{
	public:

		/**
		 * Extracts vocabulary descriptors from a descriptor feature map.
		 * This function mainly serializes the descriptors from the map and e.g., separates multi-level/multi-layer descriptors into individual descriptors.
		 * @param descriptorMap The descriptor map containing the features to serialize
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of all 3D object points
		 * @param objectPointIndices The resulting indices of 3D object points, allowing to map extracted vocabulary descriptors to object points (e.g., because the given object point descriptor may be multi-level or/and multi-view descriptors
		 * @param vocabularyDescriptors The resulting descriptors for the vocabulary tree, must be single-view, single-level descriptors
		 */
		static bool extractVocabularyDescriptorsFromMap(const UnifiedDescriptorMap& descriptorMap, Vectors3& objectPoints, Indices32& objectPointIds, Indices32& objectPointIndices, UnifiedDescriptor::FloatDescriptors<128u>& vocabularyDescriptors);
};

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_POSE_ESTIMATION_H
