/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_FEATURE_MAP_H
#define META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_FEATURE_MAP_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/Unified.h"
#include "ocean/tracking/mapbuilding/UnifiedMatching.h"

#include "ocean/base/Lock.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/geometry/Octree.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

// Forward declaration.
class UnifiedFeatureMap;

/**
 * Definition of a shared pointer holding a UnifiedFeatureMap object.
 * @see UnifiedFeatureMap.
 * @ingroup trackingmapbuilding
 */
typedef std::shared_ptr<UnifiedFeatureMap> SharedUnifiedFeatureMap;

/**
 * This class implements the base class for a feature map necessary to re-localize with optimized data structures.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT UnifiedFeatureMap
{
	public:

		/**
		 * Disposes the object.
		 */
		virtual ~UnifiedFeatureMap() = default;

		/**
		 * Returns the 3D object points of the map.
		 * This function is not thread-safe.
		 * @return The map's 3D object points
		 */
		inline const Vectors3& objectPoints() const;

		/**
		 * Returns the ids object point ids, one for each 3D object point.
		 * This function is not thread-safe.
		 * @return The map's object point ids
		 */
		inline const Indices32& objectPointIds() const;

		/**
		 * Returns the descriptors of the 3D object points adjusted/optimized for the vocabulary structure.
		 * This function is not thread-safe.
		 * @return The map's vocabulary structure descriptors
		 */
		inline const UnifiedDescriptors& objectPointVocabularyDescriptors() const;

		/**
		 * Returns the indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices.
		 * This function is not thread-safe.
		 * @return The map's object point indices
		 */
		inline const Indices32& objectPointIndices() const;

		/**
		 * Returns the vocabulary forest holding the descriptors of the object points of the map.
		 * This function is not thread-safe.
		 * @return The map's vocabulary forest
		 */
		virtual const Tracking::VocabularyStructure& objectPointDescriptorsForest() const = 0;

		/**
		 * Returns the octree holding the object points of the map.
		 * This function is not thread-safe.
		 * @return The map's octree
		 */
		inline const Geometry::Octree& objectPointOctree() const;

		/**
		 * Returns the map mapping object point ids to their associated descriptors.
		 * This function is not thread-safe.
		 * @return The map's descriptor map
		 */
		inline const Tracking::MapBuilding::UnifiedDescriptorMap& descriptorMap() const;

		/**
		 * Sets or updates the feature map to be used for relocalization.
		 * @param objectPoints The 3D object points of the feature map
		 * @param objectPointIds The ids of the object points, one for each object point
		 * @param descriptorMap The map mapping object point ids to descriptors, must be valid
		 * @param randomGenerator The random generator object
		 * @return True, if succeeded
		 */
		virtual bool updateFeatureMap(Vectors3&& objectPoints, Indices32&& objectPointIds, SharedUnifiedDescriptorMap&& descriptorMap, RandomGenerator& randomGenerator) = 0;

		/**
		 * Creates the unguided matching and the guided matching object and initializes the objects with the necessary information.
		 * @param imagePoints The image points to be used, nullptr to initialize the objects without image points
		 * @param imagePointDescriptors The descriptors of the image points, nullptr to initialize the objects without image points descriptors
		 * @param unifiedUnguidedMatching The resulting unguided matching object
		 * @param unifiedGuidedMatching The resulting guided matching object
		 * @return True, if succeeded
		 */
		virtual bool createMatchingObjects(const Vector2* imagePoints, const UnifiedDescriptors* imagePointDescriptors, SharedUnifiedUnguidedMatching& unifiedUnguidedMatching, SharedUnifiedGuidedMatching& unifiedGuidedMatching) = 0;

		/**
		 * Returns whether this feature map holds at least one feature.
		 */
		inline bool isValid() const;

	protected:

		/**
		 * Default constructor creating an invalid feature map.
		 */
		UnifiedFeatureMap() = default;

	protected:

		/// The 3D object points of the map.
		Vectors3 objectPoints_;

		/// The ids object point ids, one for each 3D object point.
		Indices32 objectPointIds_;

		/// The descriptors of the 3D object points adjusted for the vocabulary structure.
		SharedUnifiedDescriptors objectPointVocabularyDescriptors_;

		/// The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices.
		Indices32 objectPointIndices_;

		/// The octree holding the object points of the map.
		Geometry::Octree objectPointOctree_;

		/// The map mapping object point ids to their associated descriptors.
		SharedUnifiedDescriptorMap descriptorMap_;

		/// The feature map's lock.
		mutable Lock lock_;
};

/**
 * This class implements a specialized feature map with specific descriptor types.
 * @tparam TImagePointDescriptor The data type of the image point descriptors
 * @tparam TObjectPointDescriptor The data type of the object point descriptors
 * @tparam TObjectPointVocabularyDescriptor The data type of the vocabulary descriptors representing the object point descriptors, must be a single-level, single-view descriptor
 * @tparam TDescriptorDistance The data type of the distance between two vocabulary descriptors, e.g., 'uint32_t' or 'float'
 * @tparam tVocabularyDistanceFunction The function pointer to a function allowing to determine the descriptor distance between two vocabulary descriptors
 * @ingroup trackingmapbuilding
 */
template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDescriptorDistance = typename UnifiedDescriptor::DistanceTyper<TObjectPointVocabularyDescriptor>::Type, TDescriptorDistance(*tVocabularyDistanceFunction)(const TObjectPointVocabularyDescriptor&, const TObjectPointVocabularyDescriptor&) = UnifiedDescriptorT<TObjectPointVocabularyDescriptor>::determineDistance>
class UnifiedFeatureMapT : public UnifiedFeatureMap
{
	public:

		/**
		 * Definition of a vocabulary forest used for unguided pose estimation.
		 */
		using VocabularyForest = Tracking::VocabularyForest<TObjectPointVocabularyDescriptor, TDescriptorDistance, tVocabularyDistanceFunction>;

		/**
		 * Definition of a vector holding vocabulary descriptors.
		 */
		using TObjectPointVocabularyDescriptors = std::vector<TObjectPointVocabularyDescriptor>;

		/**
		 * Definition of a function allowing to serialize the features from a descriptor map so that the features can be processed with a vocabulary tree/forest.
		 * @param descriptorMap The descriptor map containing the features to serialize
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of all 3D object points
		 * @param objectPointIndices The resulting indices of 3D object points, allowing to map extracted vocabulary descriptors to object points (e.g., because the given object point descriptor may be multi-level or/and multi-view descriptors
		 * @param vocabularyDescriptors The resulting descriptors for the vocabulary tree, must be single-view, single-level descriptors
		 * @return True, if succeeded
		 */
		using ExtractVocabularyDescriptorsFromMapFunction = std::function<bool(const UnifiedDescriptorMap& descriptorMap, Vectors3& objectPoints, Indices32& objectPointIds, Indices32& objectPointIndices, TObjectPointVocabularyDescriptors& vocabularyDescriptors)>;

	public:

		/**
		 * Creates a new unified feature map object.
		 * @param objectPoints The 3D object points to be used
		 * @param objectPointIds The ids of the object points, one for each object point
		 * @param descriptorMap The map containing the descriptors of the object points
		 * @param randomGenerator The random generator to be used
		 * @param clustersMeanFunction The function allowing to determine the mean descriptors for individual clusters, must be valid
		 * @param extractVocabularyDescriptorsFromMapFunction The function allowing to extract the 3D object point descriptors from the feature map and creating serialized descriptors which can be processed in the vocabulary tree, must be valid
		 */
		UnifiedFeatureMapT(Vectors3&& objectPoints, Indices32&& objectPointIds, SharedUnifiedDescriptorMap&& descriptorMap, RandomGenerator& randomGenerator, typename VocabularyForest::ClustersMeanFunction clustersMeanFunction, ExtractVocabularyDescriptorsFromMapFunction extractVocabularyDescriptorsFromMapFunction);

		/**
		 * Returns the descriptors of the object points used in the vocabulary tree.
		 * @return The vocabulary tree's descriptors
		 */
		const TObjectPointVocabularyDescriptor* objectPointVocabularyDescriptors() const;

		/**
		 * Returns the vocabulary forest holding the descriptors of the object points of the map.
		 * This function is not thread-safe.
		 * @return The map's vocabulary forest
		 */
		const VocabularyForest& objectPointDescriptorsForest() const override;

		/**
		 * Sets or updates the feature map to be used for relocalization.
		 * @see UnifiedFeatureMap::updateFeatureMap().
		 */
		bool updateFeatureMap(Vectors3&& objectPoints, Indices32&& objectPointIds, SharedUnifiedDescriptorMap&& descriptorMap, RandomGenerator& randomGenerator) override;

		/**
		 * Creates the unguided matching and the guided matching object and initializes the objects with the necessary information.
		 * @see UnifiedFeatureMap::createMatchingObjects().
		 */
		bool createMatchingObjects(const Vector2* imagePoints, const UnifiedDescriptors* imagePointDescriptors, SharedUnifiedUnguidedMatching& unifiedUnguidedMatching, SharedUnifiedGuidedMatching& unifiedGuidedMatching) override;

	protected:

		/// The function allowing to determine the mean descriptors for individual clusters.
		typename VocabularyForest::ClustersMeanFunction clustersMeanFunction_;

		/// The function allowing to extract the 3D object point descriptors from the feature map and creating serialized descriptors which can be processed in the vocabulary tree.
		ExtractVocabularyDescriptorsFromMapFunction extractVocabularyDescriptorsFromMapFunction_;

		/// The vocabulary forest holding the descriptors of the object points of the map.
		VocabularyForest objectPointDescriptorsForest_;
};

inline const Vectors3& UnifiedFeatureMap::objectPoints() const
{
	return objectPoints_;
}

inline const Indices32& UnifiedFeatureMap::objectPointIds() const
{
	return objectPointIds_;
}

inline const UnifiedDescriptors& UnifiedFeatureMap::objectPointVocabularyDescriptors() const
{
	ocean_assert(objectPointVocabularyDescriptors_ != nullptr);
	return *objectPointVocabularyDescriptors_;
}

inline const Indices32& UnifiedFeatureMap::objectPointIndices() const
{
	return objectPointIndices_;
}

inline const Geometry::Octree& UnifiedFeatureMap::objectPointOctree() const
{
	return objectPointOctree_;
}

inline const Tracking::MapBuilding::UnifiedDescriptorMap& UnifiedFeatureMap::descriptorMap() const
{
	ocean_assert(descriptorMap_ != nullptr);
	return *descriptorMap_;
}

inline bool UnifiedFeatureMap::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return !objectPoints_.empty() && objectPointVocabularyDescriptors_ != nullptr;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tVocabularyDistanceFunction)(const TObjectPointVocabularyDescriptor&, const TObjectPointVocabularyDescriptor&)>
UnifiedFeatureMapT<TImagePointDescriptor, TObjectPointDescriptor, TObjectPointVocabularyDescriptor, TDescriptorDistance, tVocabularyDistanceFunction>::UnifiedFeatureMapT(Vectors3&& objectPoints, Indices32&& objectPointIds, SharedUnifiedDescriptorMap&& descriptorMap, RandomGenerator& randomGenerator, typename VocabularyForest::ClustersMeanFunction clustersMeanFunction, ExtractVocabularyDescriptorsFromMapFunction extractVocabularyDescriptorsFromMapFunction) :
	clustersMeanFunction_(std::move(clustersMeanFunction)),
	extractVocabularyDescriptorsFromMapFunction_(std::move(extractVocabularyDescriptorsFromMapFunction))
{
	ocean_assert(clustersMeanFunction_);
	ocean_assert(extractVocabularyDescriptorsFromMapFunction_);

	const bool result = updateFeatureMap(std::move(objectPoints), std::move(objectPointIds), std::move(descriptorMap), randomGenerator);
	ocean_assert_and_suppress_unused(result, result);
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tVocabularyDistanceFunction)(const TObjectPointVocabularyDescriptor&, const TObjectPointVocabularyDescriptor&)>
inline const TObjectPointVocabularyDescriptor* UnifiedFeatureMapT<TImagePointDescriptor, TObjectPointDescriptor, TObjectPointVocabularyDescriptor, TDescriptorDistance, tVocabularyDistanceFunction>::objectPointVocabularyDescriptors() const
{
	if (!objectPointVocabularyDescriptors_)
	{
		return nullptr;
	}

	if (objectPointVocabularyDescriptors_->descriptorType() != UnifiedDescriptor::DescriptorTyper<TObjectPointVocabularyDescriptor>::type())
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	const UnifiedDescriptorsT<TObjectPointVocabularyDescriptor>* vocabularyDescriptors = dynamic_cast<const UnifiedDescriptorsT<TObjectPointVocabularyDescriptor>*>(objectPointVocabularyDescriptors_.get());

	if (vocabularyDescriptors == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return nullptr;
	}

	return vocabularyDescriptors->descriptors();
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tVocabularyDistanceFunction)(const TObjectPointVocabularyDescriptor&, const TObjectPointVocabularyDescriptor&)>
inline const typename UnifiedFeatureMapT<TImagePointDescriptor, TObjectPointDescriptor, TObjectPointVocabularyDescriptor, TDescriptorDistance, tVocabularyDistanceFunction>::VocabularyForest& UnifiedFeatureMapT<TImagePointDescriptor, TObjectPointDescriptor, TObjectPointVocabularyDescriptor, TDescriptorDistance, tVocabularyDistanceFunction>::objectPointDescriptorsForest() const
{
	return objectPointDescriptorsForest_;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tVocabularyDistanceFunction)(const TObjectPointVocabularyDescriptor&, const TObjectPointVocabularyDescriptor&)>
bool UnifiedFeatureMapT<TImagePointDescriptor, TObjectPointDescriptor, TObjectPointVocabularyDescriptor, TDescriptorDistance, tVocabularyDistanceFunction>::updateFeatureMap(Vectors3&& objectPoints, Indices32&& objectPointIds, SharedUnifiedDescriptorMap&& descriptorMap, RandomGenerator& randomGenerator)
{
	if (!extractVocabularyDescriptorsFromMapFunction_)
	{
		return false;
	}

	if (objectPoints.empty() || objectPointIds.empty() || !descriptorMap)
	{
		return false;
	}

	if (objectPoints.size() != objectPointIds.size())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	objectPointVocabularyDescriptors_ = nullptr;
	objectPointIndices_.clear();
	descriptorMap_ = nullptr;

	objectPoints_ = std::move(objectPoints);
	objectPointIds_ = std::move(objectPointIds);
	descriptorMap_ = std::move(descriptorMap);

	ocean_assert(descriptorMap_);

	TObjectPointVocabularyDescriptors objectPointVocabularyDescriptors;

	if (!extractVocabularyDescriptorsFromMapFunction_(*descriptorMap_, objectPoints_, objectPointIds_, objectPointIndices_, objectPointVocabularyDescriptors))
	{
		return false;
	}

	using UnifiedDescriptorsVocabulary = UnifiedDescriptorsT<TObjectPointVocabularyDescriptor>;

	objectPointVocabularyDescriptors_ = std::make_shared<UnifiedDescriptorsVocabulary>(std::move(objectPointVocabularyDescriptors));

	const UnifiedDescriptorsVocabulary& unifiedDescriptorsVocabulary = dynamic_cast<const UnifiedDescriptorsVocabulary&>(*objectPointVocabularyDescriptors_);

	const typename VocabularyForest::Parameters parameters;

	objectPointDescriptorsForest_ = VocabularyForest(2, unifiedDescriptorsVocabulary.descriptors(), unifiedDescriptorsVocabulary.numberDescriptors(), clustersMeanFunction_, parameters, WorkerPool::get().scopedWorker()(), &randomGenerator);

	objectPointOctree_ = Geometry::Octree(objectPoints_.data(), objectPoints_.size(), Geometry::Octree::Parameters(40u, true));

	return true;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDescriptorDistance, TDescriptorDistance(*tVocabularyDistanceFunction)(const TObjectPointVocabularyDescriptor&, const TObjectPointVocabularyDescriptor&)>
bool UnifiedFeatureMapT<TImagePointDescriptor, TObjectPointDescriptor, TObjectPointVocabularyDescriptor, TDescriptorDistance, tVocabularyDistanceFunction>::createMatchingObjects(const Vector2* imagePoints, const UnifiedDescriptors* imagePointDescriptors, SharedUnifiedUnguidedMatching& unifiedUnguidedMatching, SharedUnifiedGuidedMatching& unifiedGuidedMatching)
{
	ocean_assert((imagePoints != nullptr && imagePointDescriptors != nullptr) || (imagePoints == nullptr && imagePointDescriptors == nullptr));

	const UnifiedDescriptorMapT<TObjectPointDescriptor>* specializedDescriptorMap = dynamic_cast<const UnifiedDescriptorMapT<TObjectPointDescriptor>*>(descriptorMap_.get());

	if (specializedDescriptorMap == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	using UnifiedUnguidedMatching = UnifiedUnguidedMatchingT<TImagePointDescriptor, TObjectPointVocabularyDescriptor>;
	using UnifiedGuidedMatching = UnifiedGuidedMatchingT<TImagePointDescriptor, TObjectPointDescriptor>;

	if (imagePoints != nullptr)
	{
		const UnifiedDescriptorsT<TImagePointDescriptor>* specializedImagePointDescriptors = dynamic_cast<const UnifiedDescriptorsT<TImagePointDescriptor>*>(imagePointDescriptors);

		if (specializedImagePointDescriptors == nullptr)
		{
			return false;
		}

		unifiedUnguidedMatching = std::make_shared<UnifiedUnguidedMatching>(imagePoints, specializedImagePointDescriptors->descriptors(), specializedImagePointDescriptors->numberDescriptors(), objectPoints_.data(), objectPointVocabularyDescriptors(), objectPoints_.size(), objectPointIndices_.data(), objectPointDescriptorsForest());
		unifiedGuidedMatching = std::make_shared<UnifiedGuidedMatching>(imagePoints, specializedImagePointDescriptors->descriptors(), specializedImagePointDescriptors->numberDescriptors(), objectPoints_.data(), objectPoints_.size(), objectPointOctree_, objectPointIds_.data(), specializedDescriptorMap->descriptorMap());
	}
	else
	{
		unifiedUnguidedMatching = std::make_shared<UnifiedUnguidedMatching>(objectPoints_.data(), objectPointVocabularyDescriptors(), objectPoints_.size(), objectPointIndices_.data(), objectPointDescriptorsForest());
		unifiedGuidedMatching = std::make_shared<UnifiedGuidedMatching>(objectPoints_.data(), objectPoints_.size(), objectPointOctree_, objectPointIds_.data(), specializedDescriptorMap->descriptorMap());
	}

	return unifiedUnguidedMatching && unifiedGuidedMatching;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_FEATURE_MAP_H
