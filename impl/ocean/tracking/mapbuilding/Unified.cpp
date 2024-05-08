/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/mapbuilding/Unified.h"
#include "ocean/tracking/mapbuilding/PoseEstimationT.h"

#include "ocean/base/WorkerPool.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

bool UnifiedDescriptorExtractorFreakMultiDescriptor256::createAndAddDescriptor(const CV::FramePyramid& yFramePyramid, const AnyCamera& anyCamera, const Vector2& imagePoint, const Index32& objectPointId, UnifiedDescriptorMap& unifiedDescriptorMap) const
{
	ocean_assert(yFramePyramid.isValid() && yFramePyramid.frameType().isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(yFramePyramid.finestWidth() == anyCamera.width());
	ocean_assert(yFramePyramid.finestHeight() == anyCamera.height());

	if (unifiedDescriptorMap.descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256)
	{
		ocean_assert(false && "Other descriptors not yet supported!");
		return false;
	}

	UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256& unifiedDescriptorMapFreak256 = (UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256&)(unifiedDescriptorMap);

	DescriptorHandling::FreakMultiDescriptor256 newFreakDescriptor;
	if (DescriptorHandling::computeFreakDescriptor(yFramePyramid, anyCamera, imagePoint, newFreakDescriptor))
	{
		DescriptorHandling::FreakMultiDescriptors256& existingFreakDescriptors = unifiedDescriptorMapFreak256.descriptorMap()[objectPointId];

		bool similarDescriptorExists = false;

		for (const DescriptorHandling::FreakMultiDescriptor256& existingFreakFeature : existingFreakDescriptors)
		{
			if (existingFreakFeature.distance(newFreakDescriptor) <= 10u)
			{
				similarDescriptorExists = true;
				break;
			}
		}

		if (!similarDescriptorExists)
		{
			existingFreakDescriptors.emplace_back(newFreakDescriptor);
			return true;
		}
	}

	return false;
}

std::shared_ptr<UnifiedDescriptorMap> UnifiedDescriptorExtractorFreakMultiDescriptor256::createUnifiedDescriptorMap() const
{
	return std::make_shared<UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>();
}

bool UnifiedHelperFreakMultiDescriptor256::initialize(Vectors3& objectPoints, Indices32& objectPointIds, const UnifiedDescriptorMap& descriptorMap, RandomGenerator& randomGenerator, BinaryDescriptors256& objectPointDescriptors, Indices32& objectPointIndices, BinaryVocabularyForest& objectPointDescriptorsForest, Geometry::Octree& objectPointOctree)
{
	if (descriptorMap.descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256)
	{
		ocean_assert(false && "Descriptor map does not match!");
		return false;
	}

	const Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256* specializedDescriptorMap = dynamic_cast<const Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256*>(&descriptorMap);
	ocean_assert(specializedDescriptorMap != nullptr);

	if (specializedDescriptorMap == nullptr)
	{
		return false;
	}

	const Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& descriptorMapFreak256 = specializedDescriptorMap->descriptorMap();

	objectPointDescriptors.clear();
	objectPointIndices.clear();

	objectPointDescriptors.reserve(objectPointIds.size() * 40);
	objectPointIndices.reserve(objectPointDescriptors.capacity());

	for (size_t n = 0; n < objectPointIds.size(); /*noop*/)
	{
		const Index32& objectPointId = objectPointIds[n];

		const UnifiedGuidedMatchingFreakMultiLevelDescriptor256::UnorderedDescriptorMap<ObjectPointDescriptor>::const_iterator i = descriptorMapFreak256.find(objectPointId);
		if (i != descriptorMapFreak256.cend())
		{
			const FreakMultiDescriptors256& descriptors = i->second;

			for (const FreakMultiDescriptor256& multiDescriptor : descriptors)
			{
				for (unsigned int t = 0u; t < multiDescriptor.descriptorLevels(); ++t)
				{
					const auto& layerDescriptor = multiDescriptor.data()[t];
					static_assert(sizeof(layerDescriptor) == 32, "Invalid size!");

					BinaryDescriptor256 targetObjectPointDescriptor;
					memcpy(&targetObjectPointDescriptor, &layerDescriptor, sizeof(layerDescriptor));

					objectPointDescriptors.emplace_back(targetObjectPointDescriptor);

					objectPointIndices.emplace_back(Index32(n));
				}
			}

			++n;
		}
		else
		{
			objectPointIds[n] = objectPointIds.back();
			objectPointIds.pop_back();

			objectPoints[n] = objectPoints.back();
			objectPoints.pop_back();
		}
	}

#ifdef OCEAN_DEBUG
	for (const Vector3& objectPoint : objectPoints)
	{
		ocean_assert(objectPoint != Database::invalidObjectPoint());

		if (Numeric::isInf(objectPoint.x()))
		{
			ocean_assert(objectPoint != Database::invalidObjectPoint());
		}

		ocean_assert(!Numeric::isInf(objectPoint.x()) && !Numeric::isInf(objectPoint.y()) && !Numeric::isInf(objectPoint.z()));
	}
#endif

	const BinaryVocabularyTree::ClustersMeanFunction clustersMeanFunction = &BinaryVocabularyTree::determineClustersMeanForBinaryDescriptor<sizeof(BinaryDescriptor256) * 8>;

	objectPointDescriptorsForest = BinaryVocabularyForest(2, objectPointDescriptors.data(), objectPointDescriptors.size(), clustersMeanFunction, BinaryVocabularyForest::Parameters(), WorkerPool::get().scopedWorker()(), &randomGenerator);
	objectPointOctree = Geometry::Octree(objectPoints.data(), objectPoints.size(), Geometry::Octree::Parameters(40u, true));

	return true;
}

bool UnifiedHelperFreakMultiDescriptor256::initialize(const Tracking::Database& database, const UnifiedDescriptorMap& descriptorMap, RandomGenerator& randomGenerator, Vectors3& objectPoints, Indices32& objectPointIds, BinaryDescriptors256& objectPointDescriptors, Indices32& objectPointIndices, BinaryVocabularyForest& objectPointDescriptorsForest, Geometry::Octree& objectPointOctree)
{
	objectPointIds.clear();
	objectPoints.clear();

	objectPointIds = database.objectPointIds<false, false>(Tracking::Database::invalidObjectPoint(), &objectPoints);

	return initialize(objectPoints, objectPointIds, descriptorMap, randomGenerator, objectPointDescriptors, objectPointIndices, objectPointDescriptorsForest, objectPointOctree);
}

bool UnifiedHelperFreakMultiDescriptor256::initialize(const Tracking::Database& database, const UnifiedDescriptorMap& descriptorMap, RandomGenerator& randomGenerator, Vectors3& objectPoints, Indices32& objectPointIds, BinaryDescriptors256& objectPointDescriptors, Indices32& objectPointIndices, BinaryVocabularyForest& objectPointDescriptorsForest, Geometry::Octree& objectPointOctree, std::unique_ptr<UnifiedUnguidedMatchingFreakMultiFeatures256Group>& unifiedUnguidedMatching, std::unique_ptr<UnifiedGuidedMatchingFreakMultiDescriptor256Group>& unifiedGuidedMatching)
{
	if (!initialize(database, descriptorMap, randomGenerator, objectPoints, objectPointIds, objectPointDescriptors, objectPointIndices, objectPointDescriptorsForest, objectPointOctree))
	{
		return false;
	}

	if (descriptorMap.descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256)
	{
		return false;
	}

	const Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256* specializedDescriptorMap = dynamic_cast<const Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256*>(&descriptorMap);
	ocean_assert(specializedDescriptorMap != nullptr);

	if (specializedDescriptorMap == nullptr)
	{
		return false;
	}

	const Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& descriptorMapFreak256 = specializedDescriptorMap->descriptorMap();

	unifiedUnguidedMatching = std::make_unique<UnifiedUnguidedMatchingFreakMultiFeatures256Group>(objectPoints.data(), objectPointDescriptors.data(), objectPoints.size(), objectPointIndices.data(), objectPointDescriptorsForest);
	unifiedGuidedMatching = std::make_unique<UnifiedGuidedMatchingFreakMultiDescriptor256Group>(objectPoints.data(), objectPoints.size(), objectPointOctree, objectPointIds.data(), descriptorMapFreak256);

	return true;
}

bool UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap(const UnifiedDescriptorMap& descriptorMap, Vectors3& objectPoints, Indices32& objectPointIds, Indices32& objectPointIndices, UnifiedDescriptor::ByteDescriptors<32u>& vocabularyDescriptors)
{
	if (descriptorMap.descriptorType() != UnifiedDescriptor::DT_FREAK_MULTI_LEVEL_MULTI_VIEW_256)
	{
		ocean_assert(false && "Descriptor map does not match!");
		return false;
	}

	const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256* specializedDescriptorMap = dynamic_cast<const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256*>(&descriptorMap);
	ocean_assert(specializedDescriptorMap != nullptr);

	if (specializedDescriptorMap == nullptr)
	{
		return false;
	}

	const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap& descriptorMapFreak256 = specializedDescriptorMap->descriptorMap();

	vocabularyDescriptors.clear();
	objectPointIndices.clear();

	vocabularyDescriptors.reserve(objectPointIds.size() * 40);
	objectPointIndices.reserve(vocabularyDescriptors.capacity());

	for (size_t n = 0; n < objectPointIds.size(); /*noop*/)
	{
		const Index32& objectPointId = objectPointIds[n];

		const UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap::const_iterator iObjectPointDescriptor = descriptorMapFreak256.find(objectPointId);

		if (iObjectPointDescriptor != descriptorMapFreak256.cend())
		{
			const CV::Detector::FREAKDescriptors32& multiViewDescriptors = iObjectPointDescriptor->second;

			for (const CV::Detector::FREAKDescriptor32& multiDescriptor : multiViewDescriptors)
			{
				for (unsigned int t = 0u; t < multiDescriptor.descriptorLevels(); ++t)
				{
					const CV::Detector::FREAKDescriptor32::SinglelevelDescriptorData& layerDescriptor = multiDescriptor.data()[t];
					static_assert(sizeof(layerDescriptor) == 32, "Invalid size!");

					vocabularyDescriptors.emplace_back();
					memcpy(&vocabularyDescriptors.back(), &layerDescriptor, sizeof(layerDescriptor));

					objectPointIndices.emplace_back(Index32(n));
				}
			}

			++n;
		}
		else
		{
			objectPointIds[n] = objectPointIds.back();
			objectPointIds.pop_back();

			objectPoints[n] = objectPoints.back();
			objectPoints.pop_back();
		}
	}

	return true;

}

bool UnifiedHelperFloatSingleLevelMultiView128::extractVocabularyDescriptorsFromMap(const UnifiedDescriptorMap& unifiedDescriptorMap, Vectors3& objectPoints, Indices32& objectPointIds, Indices32& objectPointIndices, UnifiedDescriptor::FloatDescriptors<128u>& vocabularyDescriptors)
{
	if (unifiedDescriptorMap.descriptorType() != UnifiedDescriptor::DT_FLOAT_SINGLE_LEVEL_MULTI_VIEW_128)
	{
		ocean_assert(false && "Descriptor map does not match!");
		return false;
	}

	const UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>* specializedUnifiedDescriptorMap = dynamic_cast<const UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>*>(&unifiedDescriptorMap);
	ocean_assert(specializedUnifiedDescriptorMap != nullptr);

	if (specializedUnifiedDescriptorMap == nullptr)
	{
		return false;
	}

	const UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>::DescriptorMap& descriptorMap = specializedUnifiedDescriptorMap->descriptorMap();

	vocabularyDescriptors.clear();
	objectPointIndices.clear();

	vocabularyDescriptors.reserve(objectPointIds.size() * 40);
	objectPointIndices.reserve(vocabularyDescriptors.capacity());

	for (size_t n = 0; n < objectPointIds.size(); /*noop*/)
	{
		const Index32& objectPointId = objectPointIds[n];

		const UnifiedDescriptorMapFloatSingleLevelMultiViewDescriptor<128u>::DescriptorMap::const_iterator iObjectPointDescriptor = descriptorMap.find(objectPointId);

		if (iObjectPointDescriptor != descriptorMap.cend())
		{
			const UnifiedDescriptor::FloatDescriptors<128u>& multiViewDescriptors = iObjectPointDescriptor->second;

			for (const UnifiedDescriptor::FloatDescriptor<128u>& multiDescriptor : multiViewDescriptors)
			{
				vocabularyDescriptors.emplace_back();
				memcpy(&vocabularyDescriptors.back(), multiDescriptor.data(), sizeof(UnifiedDescriptor::FloatDescriptor<128u>));

				objectPointIndices.emplace_back(Index32(n));
			}

			++n;
		}
		else
		{
			objectPointIds[n] = objectPointIds.back();
			objectPointIds.pop_back();

			objectPoints[n] = objectPoints.back();
			objectPoints.pop_back();
		}
	}

	return true;
}

}

}

}
