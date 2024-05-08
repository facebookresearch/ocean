/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_VOCABULAR_TREE_H
#define META_OCEAN_TRACKING_VOCABULAR_TREE_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Memory.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Tracking
{

// Forward declaration.
class VocabularyStructure;

/**
 * Definition of a shared pointer holding a VocabularyStructure object.
 * @see VocabularyStructure.
 * @ingroup tracking
 */
using SharedVocabularyStructure = std::shared_ptr<VocabularyStructure>;

/**
 * This class implements the base class for all Vocabulary objects.
 * @ingroup tracking
 */
class VocabularyStructure
{
	public:

		/**
		 * Returns an invalid matching index.
		 * @return The index of an invalid match
		 */
		static constexpr Index32 invalidMatchIndex();

		/**
		 * Definition of individual strategies to initialize the clustering of each tree node.
		 */
		enum InitializationStrategy : uint32_t
		{
			/// An invalid strategy.
			IS_INVALID = 0u,
			/// All initial clusters are chosen randomly.
			IS_PURE_RANDOM,
			/// The initial first cluster is chosen randomly, the remaining clusters are chosen with largest distance to each other.
			IS_LARGEST_DISTANCE
		};

		/**
		 * Definition of individual matching modes for descriptors.
		 */
		enum MatchingMode : uint32_t
		{
			/// An invalid matching mode.
			MM_INVALID = 0u,
			/// Only descriptor from the first best tree leaf are considered for matching (the second+ leaf with identical best distance is skipped).
			MM_FIRST_BEST_LEAF,
			/// All descriptors from all best tree leafs are considered for matching (all leafs with identical best distances are considered).
			MM_ALL_BEST_LEAFS,
			/// All descriptors from all tree leafs are considered for matching which are within a 1% distance to the best leaf.
			MM_ALL_GOOD_LEAFS_1,
			/// All descriptors from all tree leafs are considered for matching which are within a 2% distance to the best leaf.
			MM_ALL_GOOD_LEAFS_2
		};

		/**
		 * This class implements a simple container holding the index pairs of matching descriptors and their distance.
		 * @tparam TDistance The data type of the distance measure between two descriptors, e.g., 'unsigned int', 'float'
		 */
		template <typename TDistance>
		class Match
		{
			public:

				/**
				 * Default constructor for an invalid match.
				 */
				Match() = default;

				/**
				 * Creates a new match object.
				 * @param candidateDescriptorIndex The index of the candidate descriptor, with range [0, infinity)
				 * @param queryDescriptorIndex The index of the query descriptor, with range [0, infinity)
				 * @param distance The distance between both descriptors, with range [0, infinity]
				 */
				inline Match(const Index32 candidateDescriptorIndex, const Index32 queryDescriptorIndex, const TDistance distance);

				/**
				 * Returns the index of the candidate descriptor.
				 * @return The candidate descriptor's index, with range [0, infinity)
				 */
				inline Index32 candidateDescriptorIndex() const;

				/**
				 * Returns the index of the query descriptor.
				 * @return The query descriptor's index, with range [0, infinity)
				 */
				inline Index32 queryDescriptorIndex() const;

				/**
				 * Returns the distance between both descriptors.
				 * @return The distance, with range [0, infinity]
				 */
				inline TDistance distance() const;

				/**
				 * Returns whether this match is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The index of the candidate descriptor, with range [0, infinity).
				Index32 candidateDescriptorIndex_ = invalidMatchIndex();

				/// The index of the query descriptor, with range [0, infinity).
				Index32 queryDescriptorIndex_ = invalidMatchIndex();

				/// The distance between both descriptors, with range [0, infinity].
				TDistance distance_ = NumericT<TDistance>::maxValue();
		};

		/**
		 * Definition of a vector holding matches.
		 * @ingroup TDistance The data type of the distance measure between two descriptors, e.g., 'unsigned int', 'float'
		 */
		template <typename TDistance>
		using Matches = std::vector<Match<TDistance>>;

		/**
		 * This class stores construction parameters for a VocabularyStructure.
		 */
		class Parameters
		{
			public:

				/**
				 * Default constructor.
				 */
				Parameters() = default;

				/**
				 * Creates a new parameters object.
				 * @param maximalNumberClustersPerLevel The maximal number of clusters each tree level can have with range [2, infinity)
				 * @param maximalDescriptorsPerLeaf The maximal number of descriptors each leaf can have, with range [1, infinity)
				 * @param maximalLevels The maximal number of tree levels, at tree will never have more level regardless what has been specified in 'maximalNumberClustersPerLevel' or 'maximalDescriptorsPerLeaf'
				 * @param initializationStrategy The initialization strategy for initial clusters
				 */
				inline Parameters(const unsigned int maximalNumberClustersPerLevel, const unsigned int maximalDescriptorsPerLeaf, const unsigned int maximalLevels = (unsigned int)(-1), const InitializationStrategy initializationStrategy = IS_LARGEST_DISTANCE);

				/**
				 * Returns whether this object holds valid parameters.
				 * @return True, if so
				 */
				inline bool isValid() const;

			public:

				/// The maximal number of clusters each tree level can have with range [2, infinity).
				unsigned int maximalNumberClustersPerLevel_ = 10u;

				/// The maximal number of descriptors each leaf can have, with range [1, infinity).
				unsigned int maximalDescriptorsPerLeaf_ = 40u;

				/// The maximal number of tree levels, at tree will never have more level regardless what has been specified in 'maximalNumberClustersPerLevel_' or 'maximalDescriptorsPerLeaf_'.
				unsigned int maximalLevels_ = (unsigned int)(-1);

				/// The initialization strategy for initial clusters.
				InitializationStrategy initializationStrategy_ = IS_LARGEST_DISTANCE;
		};

	public:

		/**
		 * Destructs this object.
		 */
		virtual ~VocabularyStructure() = default;

	protected:

		/**
		 * Default constructor.
		 */
		VocabularyStructure() = default;

		/**
		 * Returns the lookup table which separates the bits of a byte into 8 individual bytes.
		 * The lookup table can be used e.g., during the calculation of the mean descriptor of several descriptors.
		 * @return The lookup table with 256 * 8 entries
		 */
		static inline std::vector<uint8_t> generateBitSeparationLookup8();
};

/**
 * This class implements a Vocabulary Tree for feature descriptors.
 * Trees will not own the memory of the provided descriptors.<br>
 * Tree descriptors have to be provided as one memory array and trees store indices to these descriptors only.<br>
 * The tree descriptors need to exist as long as the corresponding tree exists.
 * @tparam TDescriptor The data type of the descriptors for which the tree will be created
 * @tparam TDistance The data type of the distance measure between two descriptors, e.g., 'unsigned int', 'float'
 * @tparam tDistanceFunction The pointers to the function able to calculate the distance between two descriptors
 * @see VocabularyForest
 * @ingroup tracking
 */
template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
class VocabularyTree : public VocabularyStructure
{
	public:

		/**
		 * The descriptor type of this tree.
		 */
		typedef TDescriptor Descriptor;

		/**
		 * The distance data type of this tree.
		 */
		typedef TDistance Distance;

		/**
		 * The data type of the sum of distances, uint64_t for uint32_t, and float for float.
		 */
		typedef typename NextLargerTyper<TDistance>::TypePerformance TSumDistances;

		/**
		 * The pointer to the function determining the distance between two descriptors of this tree.
		 */
		static constexpr TDistance(*distanceFunction)(const TDescriptor&, const TDescriptor&) = tDistanceFunction;

		/**
		 * Definition of a vector holding descriptors.
		 */
		typedef std::vector<TDescriptor> TDescriptors;

		/**
		 * Definition of a vector holding distances.
		 */
		typedef std::vector<TDistance> TDistances;

		/**
		 * Definition of a tree node which is just an alias for the tree (the root node).
		 */
		typedef VocabularyTree<TDescriptor, TDistance, tDistanceFunction> Node;

		/**
		 * Definition of a vector holding tree nodes.
		 */
		typedef std::vector<Node*> Nodes;

		/**
		 * Definition of a vector holding constant tree nodes.
		 */
		typedef std::vector<const Node*> ConstNodes;

		/**
		 * Definition of a Match object using the distance data type of this tree.
		 */
		using Match = Match<TDistance>;

		/**
		 * Definition of a vector holding Match objects.
		 */
		using Matches = Matches<TDistance>;

		/**
		 * Definition of a function pointer allowing to determine the mean descriptors for individual clusters.
		 * @see determineClustersMeanForBinaryDescriptor(), determineClustersMeanForFloatDescriptor().
		 */
		using ClustersMeanFunction = TDescriptors(*)(const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, const Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, Worker* worker);

		/**
		 * Definition of a function pointer to a function allowing to return individual descriptors from a multi-descriptor.
		 * A feature point can be described with a multi-descriptor if the feature point has been seen from individual distances (to be scale invariant to some extent).
		 * First function parameter is the multi-descriptor, second parameter is the index of the actual descriptor to return, returns nullptr if the index is out of range
		 */
		template <typename TMultiDescriptor>
		using MultiDescriptorFunction = const TDescriptor*(*)(const TMultiDescriptor&, const size_t);

		/**
		 * Definition of a function pointer to a function allowing to return individual multi-descriptors from a group of multi-descriptors.
		 * A feature point can be described with a group of multi-descriptors if the feature point has been seen from individual locations or at individual moments in time (e.g., day vs. night).
		 * First function parameter is the multi-descriptor group, second parameter is the index of the actual multi-descriptor to return, returns nullptr if the index is out of range
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor>
		using MultiDescriptorGroupFunction = const TMultiDescriptor*(*)(const TMultiDescriptorGroup&, const size_t);

		/**
		 * Definition of a class which holds reusable data for internal use.
		 * This object can avoid reallocating memory when calling a matching function several times in a row.<br>
		 * Simply define this object outside of the loop and provide the object as parameter, e.g.,
		 * @code
		 * ReusableData reusableData;
		 * for (const TDescriptor& queryDescriptor : queryDescriptors)
		 * {
		 *     const Index32 index = vocabularyTree.matchDescriptor(candidateDescriptors, queryDescriptor, nullptr, reusableData);
		 *     ...
		 * }
		 * @endcode
		 */
		class ReusableData
		{
			friend class VocabularyTree<TDescriptor, TDistance, tDistanceFunction>;

			public:

				/**
				 * Creates a new object.
				 */
				ReusableData() = default;

			protected:

				/// The internal reusable data.
				mutable std::vector<const Indices32*> internalData_;
		};

	public:

		/**
		 * Creates an empty vocabulary tree.
		 */
		VocabularyTree() = default;

		/**
		 * Move constructor.
		 * @param vocabularyTree The vocabulary tree to be moved
		 */
		VocabularyTree(VocabularyTree&& vocabularyTree);

		/**
		 * Creates a new tree for a known descriptors.
		 * The given descriptors must not change afterwards, the descriptors must exist as long as the tree exists.
		 * @param treeDescriptors The descriptors for which the new tree will be created, must be valid
		 * @param numberTreeDescriptors The number of tree descriptors, with range [1, infinity)
		 * @param clustersMeanFunction The function allowing to determine the mean descriptors for individual clusters, must be valid
		 * @param parameters The parameters used to construct the tree, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @param randomGenerator Optional explicit random generator object
		 */
		VocabularyTree(const TDescriptor* treeDescriptors, const size_t numberTreeDescriptors, const ClustersMeanFunction& clustersMeanFunction, const Parameters& parameters = Parameters(), Worker* worker = nullptr, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Destructs the vocabulary tree.
		 */
		~VocabularyTree();

		/**
		 * Determines the leaf best matching with a given descriptor.
		 * Actually this function returns the tree's descriptors within the leaf.
		 * @param descriptor The descriptor for which the best leaf will be determined
		 * @return The indices of the descriptors within the leaf
		 */
		const Indices32& determineBestLeaf(const TDescriptor& descriptor) const;

		/**
		 * Determines the leafs best matching with a given descriptor.
		 * Actually this function returns the tree's descriptors within the leafs.
		 * @param descriptor The descriptor for which the best leafs will be determined
		 * @param leafs The resulting groups of indices of the descriptors within the leafs, must be empty when calling
		 * @param distanceEpsilon The epsilon distance between the currently best node and node candidates
		 */
		void determineBestLeafs(const TDescriptor& descriptor, std::vector<const Indices32*>& leafs, const TDistance distanceEpsilon = TDistance(0)) const;

		/**
		 * Matches a query descriptor with all candidate descriptors in this tree.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryDescriptor The query descriptor for which the best matching tree candidate descriptor will be determined
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor& queryDescriptor, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches a query multi-descriptor with all candidate descriptors in this tree.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptor The first single descriptor of the query multi-descriptor for which the best matching tree candidate descriptor will be determined
		 * @param numberQuerySingleDescriptors The number of single descriptors within the multi-descriptor, with range [1, infinity)
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor* queryMultiDescriptor, const size_t numberQuerySingleDescriptors, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches a query multi-descriptor with all candidate descriptors in this tree.
		 * A feature point can be described with a multi-descriptor if the feature point has been seen from individual distances (to be scale invariant to some extent).
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptor The query multi-descriptor for which the best matching tree candidate descriptor will be determined
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptor, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TMultiDescriptor& queryMultiDescriptor, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches a query group of multi-descriptors with all candidate descriptors in this tree.
		 * A feature point can be described with a group of multi-descriptors if the feature point has been seen from individual locations or at individual moments in time (e.g., day vs. night).
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptorGroup The query group of multi-descriptors (all representing one query feature) for which the best matching tree candidate descriptor will be determined
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam TMultiDescriptorGroup The data type of the multi-descriptor group
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorGroupFunction The function pointer to a static function allowing to access one multi-descriptor of a group of multi-descriptors, must be valid
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor, MultiDescriptorGroupFunction<TMultiDescriptorGroup, TMultiDescriptor> tMultiDescriptorGroupFunction, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchMultiDescriptorGroup(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup& queryMultiDescriptorGroup, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches several query descriptors with all candidate descriptors in this tree.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryDescriptors The query descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryDescriptors == 0'
		 * @param numberQueryDescriptors The number of given query descriptors, with range [0, infinity)
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		void matchDescriptors(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const size_t numberQueryDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker = nullptr) const;

		/**
		 * Matches several query multi-descriptors with all candidate descriptors in this tree.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptors The query multi-descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryMultiDescriptors == 0'
		 * @param numberQueryMultiDescriptors The number of given query multi-descriptors, with range [0, infinity)
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TMultiDescriptor The data type of a multi-descriptor
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptor, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		void matchMultiDescriptors(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const size_t numberQueryMultiDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker = nullptr) const;

		/**
		 * Matches several query groups of multi-descriptors with all candidate descriptors in this tree.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptorGroups The query groups of multi-descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryMultiDescriptors == 0'
		 * @param numberQueryMultiDescriptorGroups The number of given query groups of multi-descriptors, with range [0, infinity)
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TMultiDescriptorGroup The data type of the multi-descriptor group
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorGroupFunction The function pointer to a static function allowing to access one multi-descriptor of a group of multi-descriptors, must be valid
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor, MultiDescriptorGroupFunction<TMultiDescriptorGroup, TMultiDescriptor> tMultiDescriptorGroupFunction, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		void matchMultiDescriptorGroups(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const size_t numberQueryMultiDescriptorGroups, const TDistance maximalDistance, Matches& matches, Worker* worker = nullptr) const;

		/**
		 * Returns the descriptor representing this tree/node.
		 * @return The node's descriptor
		 */
		inline const TDescriptor& nodeDescriptor() const;

		/**
		 * Returns all indices of descriptors which belong to this tree/node.
		 * @return The node's descriptors, empty in non-leaf nodes
		 */
		inline const Indices32& descriptorIndices() const;

		/**
		 * Returns all child nodes of this node/tree.
		 * @return The node's child nodes, empty for leaf nodes
		 */
		inline const Nodes& childNodes() const;

		/**
		 * Move operator.
		 * @param vocabularyTree The vocabulary tree to be moved
		 * @return Reference to this object
		 */
		VocabularyTree& operator=(VocabularyTree&& vocabularyTree);

		/**
		 * Disabled assign operator.
		 * @param vocabularyTree The vocabulary tree to be copied
		 * @return Reference to this object
		 */
		VocabularyTree& operator=(const VocabularyTree& vocabularyTree) = delete;

		/**
		 * Determines a binary mean descriptor for each cluster.
		 * @param numberClusters The number of clusters, with range [1, infinity)
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of the clusters, must be valid
		 * @param descriptorIndices The indices of the individual descriptors wrt. the given descriptors, must be valid
		 * @param clusterIndicesForDescriptors The indices of the clusters to which each individual descriptor belongs (a lookup table with cluster indices), one for each descriptor
		 * @param numberDescriptorIndices The number of provided descriptor indices (the number of descriptors which are part of all clusters), must be valid
		 * @param worker Optional worker to distribute the computation
		 * @return The resulting mean descriptors, one for each cluster
		 * @tparam tSize The number of bits per binary descriptor
		 */
		template <unsigned int tSize>
		static TDescriptors determineClustersMeanForBinaryDescriptor(const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, const Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, Worker* worker);

		/**
		 * Determines a float mean descriptor for each cluster.
		 * @param numberClusters The number of clusters, with range [1, infinity)
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of the clusters, must be valid
		 * @param descriptorIndices The indices of the individual descriptors wrt. the given descriptors, must be valid
		 * @param clusterIndicesForDescriptors The indices of the clusters to which each individual descriptor belongs (a lookup table with cluster indices), one for each descriptor
		 * @param numberDescriptorIndices The number of provided descriptor indices (the number of descriptors which are part of all clusters), must be valid
		 * @param worker Optional worker to distribute the computation
		 * @return The resulting mean descriptors, one for each cluster
		 * @tparam tSize The number of elements per float descriptor
		 */
		template <unsigned int tSize>
		static TDescriptors determineClustersMeanForFloatDescriptor(const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, const Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, Worker* worker);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param vocabularyTree The vocabulary tree to be copied
		 */
		VocabularyTree(const VocabularyTree& vocabularyTree) = delete;

		/**
		 * Creates a new intermediate tree node.
		 * @param level The current node level, with range [0, infinity)
		 * @param nodeDescriptor The descriptor of the new node
		 * @param parameters The parameters used to construct the tree node, must be valid
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of this new tree node, must be valid
		 * @param reusableDescriptorIndicesInput The indices of the descriptors for which the new node will be created, must be valid
		 * @param reusableDescriptorIndicesOutput Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be re-used internally, must be valid
		 * @param numberDescriptorsIndices The number of given indices in 'reusableDescriptorIndicesInput', with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param reusableClusterIndicesForDescriptors Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be reused internally, must be valid
		 * @param clustersMeanFunction The function allowing to determine the mean descriptors for individual clusters, must be valid
		 * @param worker Optional worker object to distribute the computation
		 */
		VocabularyTree(const unsigned int level, const TDescriptor& nodeDescriptor, const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, const size_t numberDescriptorsIndices, RandomGenerator& randomGenerator, Index32* reusableClusterIndicesForDescriptors, const ClustersMeanFunction& clustersMeanFunction, Worker* worker);

		/**
		 * Creates a new leaf node.
		 * @param level The level of the leaf node, with range [0, infinity)
		 * @param nodeDescriptor The descriptor of the leaf node
		 * @param descriptorIndices The indices of the descriptors which represent the leaf node, must be valid
		 * @param numberDescriptorsIndices The number of given descriptor indices, with range [1, infinity)
		 */
		VocabularyTree(const unsigned int level, const TDescriptor& nodeDescriptor, Index32* descriptorIndices, size_t numberDescriptorsIndices);

		/**
		 * Creates the new child nodes.
		 * @param childNodeLevel The level of the child nodes, with range [1, infinity)
		 * @param parameters The parameters used to construct the tree node, must be valid
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of this new tree node, must be valid
		 * @param clusterCenters The descriptors of the individual child nodes (the centers of the clusters, one for each child node, must be valid
		 * @param clusterSizes The sizes of the individual clusters, one for each cluster, must be valid
		 * @param numberClusters The number of clusters and thus the number of child nodes, with range [1, infinity)
		 * @param reusableDescriptorIndicesInput The indices of the descriptors for which the new node will be created, must be valid
		 * @param reusableDescriptorIndicesOutput Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be re-used internally, must be valid
		 * @param numberDescriptorsIndices The number of given indices in 'reusableDescriptorIndicesInput', with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param reusableClusterIndicesForDescriptors Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be reused internally, must be valid
		 * @param clustersMeanFunction The function allowing to determine the mean descriptors for individual clusters, must be valid
		 * @param worker Optional worker object to distribute the computation
		 */
		void createChildNodes(const unsigned int childNodeLevel, const Parameters& parameters, const TDescriptor* treeDescriptors, const TDescriptor* clusterCenters, const unsigned int* clusterSizes, const size_t numberClusters, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, const size_t numberDescriptorsIndices, RandomGenerator& randomGenerator, Index32* reusableClusterIndicesForDescriptors, const ClustersMeanFunction& clustersMeanFunction, Worker* worker);

		/**
		 * Distributes several descriptors into individual clusters.
		 * @param parameters The parameters used to construct the tree, must be valid
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of the clusters, must be valid
		 * @param reusableDescriptorIndicesInput The indices of the descriptors for which the new clusters will be created, must be valid
		 * @param reusableDescriptorIndicesOutput Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be re-used internally, must be valid
		 * @param numberDescriptorsIndices The number of provided indices of the tree descriptors, with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param clusterIndicesForDescriptors The resulting cluster indices to which the descriptors have been assigned, must be valid
		 * @param clustersMeanFunction The function allowing to determine the mean descriptors for individual clusters, must be valid
		 * @param clusterSizes Optional resulting sizes of the individual resulting clusters
		 * @param worker Optional worker object to distribute the computation
		 * @return The descriptors of the centers of the resulting clusters
		 */
		TDescriptors clusterDescriptors(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, size_t numberDescriptorsIndices, RandomGenerator& randomGenerator, Index32* clusterIndicesForDescriptors, const ClustersMeanFunction& clustersMeanFunction, Indices32* clusterSizes = nullptr, Worker* worker = nullptr);

		/**
		 * Determines the initial clusters based on specified initialization strategy.
		 * @param parameters The parameters used to construct the tree, must be valid
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of the clusters, must be valid
		 * @param reusableDescriptorIndicesInput The indices of the descriptors for which the new clusters will be created, must be valid
		 * @param reusableDescriptorIndicesOutput Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be re-used internally, must be valid
		 * @param numberDescriptorsIndices The number of provided indices of the tree descriptors, with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @return The descriptors of the centers of the initial clusters
		 */
		TDescriptors initialClusters(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, size_t numberDescriptorsIndices, RandomGenerator& randomGenerator);

		/**
		 * Determines the initial clusters based on the largest distance between each other.
		 * The first cluster is selected randomly, the following clusters are selected so that they have the largest distance to the existing clusters.
		 * @param parameters The parameters used to construct the tree, must be valid
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of the clusters, must be valid
		 * @param descriptorIndices The indices of the tree descriptors for which the new clusters will be determined, must be valid
		 * @param reusableIndices Memory block of indices with same size as 'descsriptorIndices' which can be re-used internally, must be valid
		 * @param numberDescriptorsIndices The number of provided indices of the tree descriptors, with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @return The descriptors of the centers of the initial clusters
		 */
		TDescriptors initialClustersLargestDistance(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* descriptorIndices, Index32* reusableIndices, const size_t numberDescriptorsIndices, RandomGenerator& randomGenerator) const;

		/**
		 * Determines the initial cluster based on a pure random choice.
		 * @param parameters The parameters used to construct the tree, must be valid
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of the clusters, must be valid
		 * @param descriptorIndices The indices of the tree descriptors for which the new clusters will be determined, must be valid
		 * @param numberDescriptorsIndices The number of provided indices of the tree descriptors, with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @return The descriptors of the centers of the initial clusters
		 */
		TDescriptors initialClustersPureRandom(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* descriptorIndices, const size_t numberDescriptorsIndices, RandomGenerator& randomGenerator) const;

		/**
		 * Assigns descriptors to clusters.
		 * @param clusterCenters The centers of the clusters to which the descriptors will be assigned, must be valid
		 * @param numberClusters The number of clusters, with range [1, infinity)
		 * @param treeDescriptors The descriptors of the entire tree from which some will belong to the new clusters, must be valid
		 * @param descriptorIndices The indices of the descriptors which need to be assigned, must be valid
		 * @param clusterIndicesForDescriptors The resulting cluster indices to which the descriptors have been assigned, must be valid
		 * @param numberDescriptorIndices The number of given indices of descriptors which will be assigned to the clusters, with range [1, infinity)
		 * @param sumDistances Optional resulting sum of distances between all assigned descriptors and their corresponding cluster centers, nullptr if not of interest
		 * @param worker Optional worker to distribute the computation
		 */
		static Indices32 assignDescriptorsToClusters(const TDescriptor* clusterCenters, const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, TSumDistances* sumDistances = nullptr, Worker* worker = nullptr);

		/**
		 * Creates a subset of the new child nodes.
		 * @param childNodeLevel The level of the child nodes, with range [1, infinity)
		 * @param parameters The parameters used to construct the tree node, must be valid
		 * @param treeDescriptors The descriptors of the entire tree from which some will be part of this new tree node, must be valid
		 * @param clusterCenters The descriptors of the individual child nodes (the centers of the clusters, one for each child node, must be valid
		 * @param clusterSizes The sizes of the individual clusters, one for each cluster, must be valid
		 * @param numberClusters The number of clusters and thus the number of child nodes, with range [1, infinity)
		 * @param reusableDescriptorIndicesInput The indices of the descriptors for which the new node will be created, must be valid
		 * @param reusableDescriptorIndicesOutput Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be re-used internally, must be valid
		 * @param numberDescriptorsIndices The number of given indices in 'reusableDescriptorIndicesInput', with range [1, infinity)
		 * @param randomGenerator The random generator to be used
		 * @param reusableClusterIndicesForDescriptors Memory block of indices with same size as 'reusableDescriptorIndicesInput' which can be reused internally, must be valid
		 * @param clustersMeanFunction The function allowing to determine the mean descriptors for individual clusters, must be valid
		 * @param subsetFirstCluster The index of the first cluster (the first child node) to be handled, with range [0, numberClusters - 1]
		 * @param subsetNumberClusters The number of clusters (child nodes) to be handled, with range [1, numberClusters - subsetFirstCluster]
		 */
		void createChildNodesSubset(const unsigned int childNodeLevel, const Parameters* parameters, const TDescriptor* treeDescriptors, const TDescriptor* clusterCenters, const unsigned int* clusterSizes, const size_t numberClusters, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, const size_t numberDescriptorsIndices, RandomGenerator* randomGenerator, Index32* reusableClusterIndicesForDescriptors, const ClustersMeanFunction* clustersMeanFunction, const unsigned int subsetFirstCluster, const unsigned int subsetNumberClusters);

		/**
		 * Assigns a subset of descriptors to clusters.
		 * @param clusterCenters The centers of the clusters to which the descriptors will be assigned, must be valid
		 * @param numberClusters The number of clusters, with range [1, infinity)
		 * @param treeDescriptors The descriptors of the entire tree from which some will belong to the new clusters, must be valid
		 * @param descriptorIndices The indices of the descriptors which need to be assigned, must be valid
		 * @param clusterIndicesForDescriptors The resulting cluster indices to which the descriptors have been assigned, must be valid
		 * @param clusterSizes The sizes of the individual clusters, one for each cluster, must be valid
		 * @param sumDistances Optional resulting sum of distances between all assigned descriptors and their corresponding cluster centers, nullptr if not of interest
		 * @param lock Optional lock when executed in multiple threads in parallel, nullptr otherwise
		 * @param firstDescriptorIndex The first descriptor index to be handled, with range [0, infinity)
		 * @param numberDescriptorIndices The number of descriptor indices to be handled, with range [1, infinity)
		 */
		static void assignDescriptorsToClustersSubset(const TDescriptor* clusterCenters, const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, Index32* clusterIndicesForDescriptors, Index32* clusterSizes, TSumDistances* sumDistances, Lock* lock, const unsigned int firstDescriptorIndex, const unsigned int numberDescriptorIndices);

		/**
		 * Matches a subset of several query descriptors with all tree candidate descriptors.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryDescriptors The query descriptors for which the best matching tree candidate descriptors will be determined, must be valid
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param lock Optional lock when executed in multiple threads in parallel, nullptr otherwise
		 * @param firstQueryDescriptor The index of the first query descriptor to be handled, with range [0, infinity)
		 * @param numberQueryDescriptors The number of query descriptors to be handled, with range [1, infinity)
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <MatchingMode tMatchingMode>
		void matchDescriptorsSubset(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryDescriptor, const unsigned int numberQueryDescriptors) const;

		/**
		 * Matches a subset of several query multi-descriptors with all tree candidate descriptors.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptors The query multi-descriptors for which the best matching tree candidate descriptors will be determined, must be valid
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param lock Optional lock when executed in multiple threads in parallel, nullptr otherwise
		 * @param firstQueryMultiDescriptor The index of the first query multi-descriptor to be handled, with range [0, infinity)
		 * @param numberQueryMultiDescriptors The number of query multi-descriptors to be handled, with range [1, infinity)
		 * @tparam TMultiDescriptor The data type of a multi-descriptor
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptor, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode>
		void matchMultiDescriptorsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptor, const unsigned int numberQueryMultiDescriptors) const;

		/**
		 * Matches a subset of several query groups of multi-descriptors with all candidate descriptors in this tree.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptorGroups The query groups of multi-descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryMultiDescriptors == 0'
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param lock Optional lock when executed in multiple threads in parallel, nullptr otherwise
		 * @param firstQueryMultiDescriptorGroup The index of the first query group of multi-descriptors to be handled, with range [0, infinity)
		 * @param numberQueryMultiDescriptorGroups The number of query groups of multi-descriptors to be handled, with range [1, infinity)
		 * @tparam TMultiDescriptorGroup The data type of the multi-descriptor group
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorGroupFunction The function pointer to a static function allowing to access one multi-descriptor of a group of multi-descriptors, must be valid
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor, MultiDescriptorGroupFunction<TMultiDescriptorGroup, TMultiDescriptor> tMultiDescriptorGroupFunction, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode>
		void matchMultiDescriptorGroupsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptorGroup, const unsigned int numberQueryMultiDescriptorGroups) const;

	protected:

		/// The node's level.
		unsigned int level_ = 0u;

		/// The node's descriptor.
		TDescriptor nodeDescriptor_;

		/// The indices of the descriptors which are part of this node.
		Indices32 descriptorIndices_;

		/// The child-nodes of this node.
		Nodes childNodes_;
};

/**
 * This class implements a Vocabulary Forest holding several Vocabulary Trees.
 * Using several trees with individual clustering can increase the probability to determine the correct descriptor.
 * @tparam TDescriptor The data type of the descriptors for which the tree will be created
 * @tparam TDistance The data type of the distance measure between two descriptors, e.g., 'unsigned int', 'float'
 * @tparam tDistanceFunction The pointers to the function able to calculate the distance between two descriptors
 * @see VocabularyTree
 * @ingroup tracking
 */
template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
class VocabularyForest : public VocabularyStructure
{
	public:

		/**
		 * The descriptor type of this forest.
		 */
		typedef TDescriptor Descriptor;

		/**
		 * The distance data type of this forest.
		 */
		typedef TDistance Distance;

		/**
		 * The pointer to the function determining the distance between two descriptors of this forest.
		 */
		static constexpr TDistance(*distanceFunction)(const TDescriptor&, const TDescriptor&) = tDistanceFunction;

		/**
		 * Definition of a vector holding descriptors.
		 */
		typedef std::vector<TDescriptor> TDescriptors;

		/**
		 * Definition of a vector holding distances.
		 */
		typedef std::vector<TDistance> TDistances;

		/**
		 * Definition of the Vocabulary Tree object.
		 */
		typedef VocabularyTree<TDescriptor, TDistance, tDistanceFunction> TVocabularyTree;

		/**
		 * Definition of a Match object using the distance data type of this tree.
		 */
		using Match = Match<TDistance>;

		/**
		 * Definition of a vector holding Match objects.
		 */
		using Matches = Matches<TDistance>;

		/**
		 * Definition of a function pointer allowing to determine the mean descriptors for individual clusters.
		 * @see determineClustersMeanForBinaryDescriptor().
		 */
		using ClustersMeanFunction = TDescriptors(*)(const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, const Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, Worker* worker);

		/**
		 * Definition of a function pointer to a function allowing to return individual descriptors from a multi-descriptor.
		 * A feature point can be described with a multi-descriptor if the feature point has been seen from individual distances (to be scale invariant to some extent).
		 * First function parameter is the multi-descriptor, second parameter is the index of the actual descriptor to return, returns nullptr if the index is out of range
		 */
		template <typename TMultiDescriptor>
		using MultiDescriptorFunction = const TDescriptor*(*)(const TMultiDescriptor&, const size_t);

		/**
		 * Definition of a function pointer to a function allowing to return individual multi-descriptors from a group of multi-descriptors.
		 * A feature point can be described with a group of multi-descriptors if the feature point has been seen from individual locations or at individual moments in time (e.g., day vs. night).
		 * First function parameter is the multi-descriptor group, second parameter is the index of the actual multi-descriptor to return, returns nullptr if the index is out of range
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor>
		using MultiDescriptorGroupFunction = const TMultiDescriptor*(*)(const TMultiDescriptorGroup&, const size_t);

		/**
		 * Re-definition of the ReusableData object.
		 */
		using ReusableData = typename TVocabularyTree::ReusableData;

	protected:

		/**
		 * Definition of a vector holding VocabularyTree objects.
		 */
		typedef std::vector<TVocabularyTree> TVocabularyTrees;

	public:

		/**
		 * Creates a new empty forest without trees.
		 */
		VocabularyForest() = default;

		/**
		 * Creates a new forest with several trees for given descriptors.
		 * The given descriptors must not change afterwards, the descriptors must exist as long as the forest exists.
		 * @param numberTrees The number of trees to be created within the forest, with range [1, infinity)
		 * @param treeDescriptors The descriptors for which the new tree will be created, must be valid
		 * @param numberTreeDescriptors The number of tree descriptors, with range [1, infinity)
		 * @param clustersMeanFunction The function allowing to determine the mean descriptors for individual clusters, must be valid
		 * @param parameters The parameters used to construct the forest, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @param randomGenerator Optional explicit random generator object
		 */
		VocabularyForest(const size_t numberTrees, const TDescriptor* treeDescriptors, const size_t numberTreeDescriptors, const ClustersMeanFunction& clustersMeanFunction, const Parameters& parameters = Parameters(), Worker* worker = nullptr, RandomGenerator* randomGenerator = nullptr);

		/**
		 * Matches a query descriptor with all candidate descriptors in this forest.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryDescriptor The query descriptor for which the best matching tree candidate descriptor will be determined
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam tMatchingMode The mode which is used for matching
		 * @see VocabularyTree::matchDescriptor().
		 */
		template <MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor& queryDescriptor, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches a query multi-descriptor with all candidate descriptors in this forest.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptor The first single descriptor of the query multi-descriptor for which the best matching tree candidate descriptor will be determined
		 * @param numberQuerySingleDescriptors The number of single descriptors within the multi-descriptor, with range [1, infinity)
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam tMatchingMode The mode which is used for matching
		 * @see VocabularyTree::matchMultiDescriptor().
		 */
		template <MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor* queryMultiDescriptor, const size_t numberQuerySingleDescriptors, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches a query multi-descriptor with all candidate descriptors in this forest.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptor The query multi-descriptor for which the best matching tree candidate descriptor will be determined
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 * @see VocabularyTree::matchMultiDescriptor().
		 */
		template <typename TMultiDescriptor, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TMultiDescriptor& queryMultiDescriptor, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches a query group of multi-descriptors with all candidate descriptors in this forest.
		 * A feature point can be described with a group of multi-descriptors if the feature point has been seen from individual locations or at individual moments in time (e.g., day vs. night).
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptorGroup The query group of multi-descriptors (all representing one query feature) for which the best matching tree candidate descriptor will be determined
		 * @param distance Optional resulting distance, nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 * @return The index of the matched tree candidate descriptor, with range [0, 'numberTreeDescriptors' - 1], invalidMatchIndex() if no match could be determined
		 * @tparam TMultiDescriptorGroup The data type of the multi-descriptor group
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorGroupFunction The function pointer to a static function allowing to access one multi-descriptor of a group of multi-descriptors, must be valid
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor, MultiDescriptorGroupFunction<TMultiDescriptorGroup, TMultiDescriptor> tMultiDescriptorGroupFunction, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		Index32 matchMultiDescriptorGroup(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup& queryMultiDescriptorGroup, TDistance* distance = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Matches several query descriptors with all candidate descriptors in this forest.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryDescriptors The query descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryDescriptors == 0'
		 * @param numberQueryDescriptors The number of given query descriptors, with range [0, infinity)
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tMatchingMode The mode which is used for matching
		 * @see VocabularyTree::matchDescriptors().
		 */
		template <MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		void matchDescriptors(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const size_t numberQueryDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker = nullptr) const;

		/**
		 * Matches several query multi-descriptors with all candidate descriptors in this forest.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptors The query multi-descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryMultiDescriptors == 0'
		 * @param numberQueryMultiDescriptors The number of given query multi-descriptors, with range [0, infinity)
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TMultiDescriptor The data type of a multi-descriptor
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 * @see VocabularyTree::matchMultiDescriptors().
		 */
		template <typename TMultiDescriptor, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		void matchMultiDescriptors(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const size_t numberQueryMultiDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker = nullptr) const;

		/**
		 * Matches several query groups of multi-descriptors with all candidate descriptors in this forest.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptorGroups The query groups of multi-descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryMultiDescriptors == 0'
		 * @param numberQueryMultiDescriptorGroups The number of given query groups of multi-descriptors, with range [0, infinity)
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param worker Optional worker object to distribute the computation
		 * @tparam TMultiDescriptorGroup The data type of the multi-descriptor group
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorGroupFunction The function pointer to a static function allowing to access one multi-descriptor of a group of multi-descriptors, must be valid
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor, MultiDescriptorGroupFunction<TMultiDescriptorGroup, TMultiDescriptor> tMultiDescriptorGroupFunction, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode = MM_FIRST_BEST_LEAF>
		void matchMultiDescriptorGroups(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const size_t numberQueryMultiDescriptorGroups, const TDistance maximalDistance, Matches& matches, Worker* worker = nullptr) const;

	protected:

		/**
		 * Matches a subset of several query descriptors with all forest candidate descriptors.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryDescriptors The query descriptors for which the best matching tree candidate descriptors will be determined, must be valid
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param lock Optional lock when executed in multiple threads in parallel, nullptr otherwise
		 * @param firstQueryDescriptor The index of the first query descriptor to be handled, with range [0, infinity)
		 * @param numberQueryDescriptors The number of query descriptors to be handled, with range [1, infinity)
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <MatchingMode tMatchingMode>
		void matchDescriptorsSubset(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryDescriptor, const unsigned int numberQueryDescriptors) const;

		/**
		 * Matches a subset of several query multi-descriptors with all forest candidate descriptors.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptors The query multi-descriptors for which the best matching tree candidate descriptors will be determined, must be valid
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param lock Optional lock when executed in multiple threads in parallel, nullptr otherwise
		 * @param firstQueryMultiDescriptor The index of the first query multi-descriptor to be handled, with range [0, infinity)
		 * @param numberQueryMultiDescriptors The number of query multi-descriptors to be handled, with range [1, infinity)
		 * @tparam TMultiDescriptor The data type of a multi-descriptor
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptor, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode>
		void matchMultiDescriptorsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptor, const unsigned int numberQueryMultiDescriptors) const;

		/**
		 * Matches a subset of several query groups of multi-descriptors with all candidate descriptors in this forest.
		 * @param candidateDescriptors The entire set of tree candidate descriptors which have been used to create the tree, from which the best matching descriptor will be determined, must be valid
		 * @param queryMultiDescriptorGroups The query groups of multi-descriptors for which the best matching tree candidate descriptors will be determined, can be nullptr if 'numberQueryMultiDescriptors == 0'
		 * @param maximalDistance The maximal distance between two matching descriptors, with range [0, infinity)
		 * @param matches The resulting matches
		 * @param lock Optional lock when executed in multiple threads in parallel, nullptr otherwise
		 * @param firstQueryMultiDescriptorGroup The index of the first query group of multi-descriptors to be handled, with range [0, infinity)
		 * @param numberQueryMultiDescriptorGroups The number of query groups of multi-descriptors to be handled, with range [1, infinity)
		 * @tparam TMultiDescriptorGroup The data type of the multi-descriptor group
		 * @tparam TMultiDescriptor The data type of the multi-descriptor
		 * @tparam tMultiDescriptorGroupFunction The function pointer to a static function allowing to access one multi-descriptor of a group of multi-descriptors, must be valid
		 * @tparam tMultiDescriptorFunction The function pointer to a static function allowing to access one single-descriptor of a multi-descriptor, must be valid
		 * @tparam tMatchingMode The mode which is used for matching
		 */
		template <typename TMultiDescriptorGroup, typename TMultiDescriptor, MultiDescriptorGroupFunction<TMultiDescriptorGroup, TMultiDescriptor> tMultiDescriptorGroupFunction, MultiDescriptorFunction<TMultiDescriptor> tMultiDescriptorFunction, MatchingMode tMatchingMode>
		void matchMultiDescriptorGroupsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptorGroup, const unsigned int numberQueryMultiDescriptorGroups) const;

	protected:

		/// The trees of this forest.
		TVocabularyTrees vocabularyTrees_;
};

constexpr Index32 VocabularyStructure::invalidMatchIndex()
{
	return Index32(-1);
}

template <typename TDistance>
inline VocabularyStructure::Match<TDistance>::Match(const Index32 candidateDescriptorIndex, const Index32 queryDescriptorIndex, const TDistance distance) :
	candidateDescriptorIndex_(candidateDescriptorIndex),
	queryDescriptorIndex_(queryDescriptorIndex),
	distance_(distance)
{
	// nothing to do here
}

template <typename TDistance>
inline Index32 VocabularyStructure::Match<TDistance>::candidateDescriptorIndex() const
{
	return candidateDescriptorIndex_;
}

template <typename TDistance>
inline Index32 VocabularyStructure::Match<TDistance>::queryDescriptorIndex() const
{
	return queryDescriptorIndex_;
}

template <typename TDistance>
inline TDistance VocabularyStructure::Match<TDistance>::distance() const
{
	return distance_;
}

template <typename TDistance>
inline bool VocabularyStructure::Match<TDistance>::isValid() const
{
	return candidateDescriptorIndex_ != invalidMatchIndex() && queryDescriptorIndex_ != invalidMatchIndex();
}

inline VocabularyStructure::Parameters::Parameters(const unsigned int maximalNumberClustersPerLevel, const unsigned int maximalDescriptorsPerLeaf, const unsigned int maximalLevels, const InitializationStrategy initializationStrategy) :
	maximalNumberClustersPerLevel_(maximalNumberClustersPerLevel),
	maximalDescriptorsPerLeaf_(maximalDescriptorsPerLeaf),
	maximalLevels_(maximalLevels),
	initializationStrategy_(initializationStrategy)
{
	// nothing to do here
}

inline bool VocabularyStructure::Parameters::isValid() const
{
	return maximalNumberClustersPerLevel_ >= 2u && maximalDescriptorsPerLeaf_ >= 1u && maximalLevels_ >= 1u && initializationStrategy_ != IS_INVALID;
}

inline std::vector<uint8_t> VocabularyStructure::generateBitSeparationLookup8()
{
	std::vector<uint8_t> lookup(256 * 8);

	for (unsigned int n = 0u; n < 256u; ++n)
	{
		uint8_t* const lookupValues = lookup.data() + n * 8u;

		for (unsigned int i = 0; i < 8u; ++i)
		{
			if (n & (1 << i))
			{
				lookupValues[i] = 1u;
			}
			else
			{
				lookupValues[i] = 0u;
			}
		}
	}

	return lookup;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::VocabularyTree(VocabularyTree<TDescriptor, TDistance, tDistanceFunction>&& VocabularyTree)
{
	static_assert(tDistanceFunction != nullptr, "Invalid distance function!");

	*this = std::move(VocabularyTree);
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::VocabularyTree(const TDescriptor* treeDescriptors, const size_t numberTreeDescriptors, const ClustersMeanFunction& clustersMeanFunction, const Parameters& parameters, Worker* worker, RandomGenerator* randomGenerator)
{
	static_assert(tDistanceFunction != nullptr, "Invalid distance function!");

	ocean_assert(parameters.isValid());

	ocean_assert(numberTreeDescriptors > 0);

	Indices32 reusableDescriptorIndicesInput = Ocean::createIndices<Index32>(numberTreeDescriptors, 0u);
	Indices32 reusableDescriptorIndicesOutput(numberTreeDescriptors);
	Indices32 reusableClusterIndicesForDescriptors(numberTreeDescriptors);

	RandomGenerator localRandomGenerator(randomGenerator);

	memset(&nodeDescriptor_, 0, sizeof(nodeDescriptor_));


	if (numberTreeDescriptors < parameters.maximalNumberClustersPerLevel_)
	{
		*this = VocabularyTree<TDescriptor, TDistance, tDistanceFunction>(0u, treeDescriptors[0], reusableDescriptorIndicesInput.data(), reusableDescriptorIndicesInput.size());
	}
	else
	{
		*this = VocabularyTree<TDescriptor, TDistance, tDistanceFunction>(0u, nodeDescriptor_, parameters, treeDescriptors, reusableDescriptorIndicesInput.data(), reusableDescriptorIndicesOutput.data(), reusableDescriptorIndicesInput.size(), localRandomGenerator, reusableClusterIndicesForDescriptors.data(), clustersMeanFunction, worker);
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::VocabularyTree(const unsigned int level, const TDescriptor& nodeDescriptor, const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, const size_t numberDescriptorIndices, RandomGenerator& randomGenerator, Index32* reusableClusterIndicesForDescriptors, const ClustersMeanFunction& clustersMeanFunction, Worker* worker) :
	level_(level),
	nodeDescriptor_(nodeDescriptor)
{
	static_assert(tDistanceFunction != nullptr, "Invalid distance function!");

	ocean_assert(parameters.isValid());
	ocean_assert(reusableDescriptorIndicesInput != nullptr && reusableDescriptorIndicesOutput != nullptr);

	const unsigned int childLevel = level + 1u;
	ocean_assert(childLevel < parameters.maximalLevels_);

	ocean_assert(parameters.maximalNumberClustersPerLevel_ <= numberDescriptorIndices);

	Indices32 clusterSizes;
	const TDescriptors clusterCenters = clusterDescriptors(parameters, treeDescriptors, reusableDescriptorIndicesInput, reusableDescriptorIndicesOutput, numberDescriptorIndices, randomGenerator, reusableClusterIndicesForDescriptors, clustersMeanFunction, &clusterSizes, worker);
	ocean_assert(clusterCenters.size() == clusterSizes.size());

	// now, we swap the reusable input and output index buffers

	Index32* const swappedReusableDescriptorIndicesInput = reusableDescriptorIndicesOutput;
	Index32* const swappedReusableDescriptorIndicesOutput = reusableDescriptorIndicesInput;

	createChildNodes(childLevel, parameters, treeDescriptors, clusterCenters.data(), clusterSizes.data(), clusterCenters.size(), swappedReusableDescriptorIndicesInput, swappedReusableDescriptorIndicesOutput, numberDescriptorIndices, randomGenerator, reusableClusterIndicesForDescriptors, clustersMeanFunction, worker);
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::VocabularyTree(const unsigned int level, const TDescriptor& nodeDescriptor, Index32* descriptorIndices, size_t numberDescriptorsIndices) :
	level_(level),
	nodeDescriptor_(nodeDescriptor)
{
	static_assert(tDistanceFunction != nullptr, "Invalid distance function!");

	ocean_assert(descriptorIndices != nullptr);
	ocean_assert(numberDescriptorsIndices != 0);

	descriptorIndices_ = Indices32(descriptorIndices, descriptorIndices + numberDescriptorsIndices);
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::~VocabularyTree()
{
	for (VocabularyTree* childNode : childNodes_)
	{
		delete childNode;
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::createChildNodes(const unsigned int childNodeLevel, const Parameters& parameters, const TDescriptor* treeDescriptors, const TDescriptor* clusterCenters, const unsigned int* clusterSizes, const size_t numberClusters, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, size_t numberDescriptorsIndices, RandomGenerator& randomGenerator, Index32* reusableClusterIndicesForDescriptors, const ClustersMeanFunction& clustersMeanFunction, Worker* worker)
{
	ocean_assert(clusterCenters != nullptr && clusterSizes != nullptr);
	ocean_assert(numberClusters >= 1);
	ocean_assert(reusableDescriptorIndicesInput != nullptr && reusableDescriptorIndicesOutput != nullptr);

	ocean_assert(childNodes_.empty());
	childNodes_.resize(numberClusters, nullptr);

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::createChildNodesSubset, childNodeLevel, &parameters, treeDescriptors, clusterCenters, clusterSizes, numberClusters, reusableDescriptorIndicesInput, reusableDescriptorIndicesOutput, numberDescriptorsIndices, &randomGenerator, reusableClusterIndicesForDescriptors, &clustersMeanFunction, 0u, 0u), 0u, (unsigned int)(numberClusters));
	}
	else
	{
		createChildNodesSubset(childNodeLevel, &parameters, treeDescriptors, clusterCenters, clusterSizes, numberClusters, reusableDescriptorIndicesInput, reusableDescriptorIndicesOutput, numberDescriptorsIndices, &randomGenerator, reusableClusterIndicesForDescriptors, &clustersMeanFunction, 0u, (unsigned int)(numberClusters));
	}

#ifdef OCEAN_DEBUG
	for (Node* childNode : childNodes_)
	{
		ocean_assert(childNode != nullptr);
	}
#endif
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
const Indices32& VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::determineBestLeaf(const TDescriptor& descriptor) const
{
	const Node* bestChildNode;
	TDistance bestDistance;

	const Node* node = this;

	while (true)
	{
		ocean_assert(node != nullptr);

		bestChildNode = nullptr;
		bestDistance = NumericT<TDistance>::maxValue();

		for (const Node* childNode : node->childNodes_)
		{
			ocean_assert(childNode != nullptr);

			const TDistance distance = tDistanceFunction(descriptor, childNode->nodeDescriptor_);

			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestChildNode = childNode;
			}
		}

		if (bestChildNode == nullptr)
		{
			return node->descriptorIndices_;
		}

		node = bestChildNode;
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::determineBestLeafs(const TDescriptor& descriptor, std::vector<const Indices32*>& leafs, const TDistance distanceEpsilon) const
{
	ocean_assert(leafs.empty());

	TDistance bestDistance;

	ConstNodes bestNodes;
	bestNodes.reserve(16);

	ConstNodes nodes;
	nodes.reserve(16);

	nodes.emplace_back(this);

	while (!nodes.empty())
	{
		const Node* node = nodes.back();
		nodes.pop_back();

		ocean_assert(node != nullptr);

		bestNodes.clear();
		bestDistance = NumericT<TDistance>::maxValue();

		for (const Node* childNode : node->childNodes_)
		{
			ocean_assert(childNode != nullptr);

			const TDistance distance = tDistanceFunction(descriptor, childNode->nodeDescriptor_);

			if (distance < bestDistance)
			{
				if (distance + distanceEpsilon < bestDistance)
				{
					// we have a significant improvements
					bestNodes.clear();
				}

				bestDistance = distance;
				bestNodes.emplace_back(childNode);
			}
			else if (distance + distanceEpsilon <= bestDistance)
			{
				bestNodes.emplace_back(childNode);
			}
		}

		if (bestNodes.empty())
		{
			ocean_assert(bestDistance == NumericT<TDistance>::maxValue());

			leafs.emplace_back(&node->descriptorIndices_);
		}
		else
		{
			if (bestNodes.size() == 1)
			{
				nodes.emplace_back(bestNodes.front());
			}
			else
			{
				nodes.insert(nodes.cend(), bestNodes.cbegin(), bestNodes.cend());
			}
		}
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor& queryDescriptor, TDistance* distance, const ReusableData& reusableData) const
{
	ocean_assert(candidateDescriptors != nullptr);

	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	if constexpr (tMatchingMode == MM_FIRST_BEST_LEAF)
	{
		const Indices32& candidateIndices = determineBestLeaf(queryDescriptor);

		for (const Index32& candidateIndex : candidateIndices)
		{
			const TDistance candidateDistance = tDistanceFunction(candidateDescriptors[candidateIndex], queryDescriptor);

			if (candidateDistance < bestDistance)
			{
				bestDistance = candidateDistance;
				bestCandidateDescriptorIndex = candidateIndex;
			}
		}
	}
	else
	{
		ocean_assert(tMatchingMode == MM_ALL_BEST_LEAFS || tMatchingMode == MM_ALL_GOOD_LEAFS_1 || tMatchingMode == MM_ALL_GOOD_LEAFS_2);

		TDistance distanceEpsilon = TDistance(0);

		if constexpr (tMatchingMode == MM_ALL_GOOD_LEAFS_1)
		{
			if (std::is_floating_point<TDistance>::value)
			{
				distanceEpsilon = TDistance(0.25);
			}
			else
			{
				distanceEpsilon = TDistance((sizeof(TDescriptor) * 8 * 1 + 50) / 100);
			}
		}
		else if constexpr (tMatchingMode == MM_ALL_GOOD_LEAFS_2)
		{
			if (std::is_floating_point<TDistance>::value)
			{
				distanceEpsilon = TDistance(0.5);
			}
			else
			{
				distanceEpsilon = TDistance((sizeof(TDescriptor) * 8 * 2 + 50) / 100);
			}
		}

		reusableData.internalData_.clear();

		determineBestLeafs(queryDescriptor, reusableData.internalData_, distanceEpsilon);

		for (const Indices32* candidateLeaf : reusableData.internalData_)
		{
			for (const Index32& candidateIndex : *candidateLeaf)
			{
				const TDistance candidateDistance = tDistanceFunction(candidateDescriptors[candidateIndex], queryDescriptor);

				if (candidateDistance < bestDistance)
				{
					bestDistance = candidateDistance;
					bestCandidateDescriptorIndex = candidateIndex;
				}
			}
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor* queryMultyDescriptor, const size_t numberQuerySingleDescriptors, TDistance* distance, const ReusableData& reusableData) const
{
	ocean_assert(candidateDescriptors != nullptr);
	ocean_assert(queryMultyDescriptor != nullptr && numberQuerySingleDescriptors >= 1);

	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	for (size_t nQuerySingle = 0; nQuerySingle < numberQuerySingleDescriptors; ++nQuerySingle)
	{
		TDistance candidateDistance;
		const Index32 candidateIndex = matchDescriptor<tMatchingMode>(candidateDescriptors, queryMultyDescriptor[nQuerySingle], &candidateDistance, reusableData);

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestCandidateDescriptorIndex = candidateIndex;
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptor, const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TMultiDescriptor& queryMultiDescriptor, TDistance* distance, const ReusableData& reusableData) const
{
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	ocean_assert(candidateDescriptors != nullptr);

	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	size_t nQueryIndex = 0;

	while (const TDescriptor* queryDescriptor = tMultiDescriptorFunction(queryMultiDescriptor, nQueryIndex++))
	{
		ocean_assert(queryDescriptor != nullptr);

		TDistance candidateDistance;
		const Index32 candidateIndex = matchDescriptor<tMatchingMode>(candidateDescriptors, *queryDescriptor, &candidateDistance, reusableData);

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestCandidateDescriptorIndex = candidateIndex;
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptorGroup, typename TMultiDescriptor, const TMultiDescriptor*(*tMultiDescriptorGroupFunction)(const TMultiDescriptorGroup&, const size_t), const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroup(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup& queryMultiDescriptorGroup, TDistance* distance, const ReusableData& reusableData) const
{
	static_assert(tMultiDescriptorGroupFunction != nullptr, "Invalid function!");
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	ocean_assert(candidateDescriptors != nullptr);

	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	size_t nQueryIndex = 0;

	while (const TMultiDescriptor* queryMultiDescriptor = tMultiDescriptorGroupFunction(queryMultiDescriptorGroup, nQueryIndex++))
	{
		ocean_assert(queryMultiDescriptor != nullptr);

		TDistance candidateDistance;
		const Index32 candidateIndex = matchMultiDescriptor<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, *queryMultiDescriptor, &candidateDistance, reusableData);

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestCandidateDescriptorIndex = candidateIndex;
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchDescriptors(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const size_t numberQueryDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker) const
{
	matches.clear();

	ocean_assert(candidateDescriptors != nullptr);
	if (numberQueryDescriptors == 0)
	{
		return;
	}

	ocean_assert(queryDescriptors != nullptr);

	if (worker && numberQueryDescriptors >= 50)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchDescriptorsSubset<tMatchingMode>, candidateDescriptors, queryDescriptors, maximalDistance, &matches, &lock, 0u, 0u), 0u, (unsigned int)(numberQueryDescriptors), 5u, 6u, 50u);
	}
	else
	{
		matchDescriptorsSubset<tMatchingMode>(candidateDescriptors, queryDescriptors, maximalDistance, &matches, nullptr, 0u, (unsigned int)(numberQueryDescriptors));
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptor, const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptors(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const size_t numberQueryMultiDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker) const
{
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	matches.clear();

	ocean_assert(candidateDescriptors != nullptr);
	if (numberQueryMultiDescriptors == 0)
	{
		return;
	}

	ocean_assert(queryMultiDescriptors != nullptr);

	if (worker && numberQueryMultiDescriptors >= 50)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorsSubset<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>, candidateDescriptors, queryMultiDescriptors, maximalDistance, &matches, &lock, 0u, 0u), 0u, (unsigned int)(numberQueryMultiDescriptors), 5u, 6u, 50u);
	}
	else
	{
		matchMultiDescriptorsSubset<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptors, maximalDistance, &matches, nullptr, 0u, (unsigned int)(numberQueryMultiDescriptors));
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptorGroup, typename TMultiDescriptor, const TMultiDescriptor*(*tMultiDescriptorGroupFunction)(const TMultiDescriptorGroup&, const size_t), const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroups(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const size_t numberQueryMultiDescriptorGroups, const TDistance maximalDistance, Matches& matches, Worker* worker) const
{
	static_assert(tMultiDescriptorGroupFunction != nullptr, "Invalid function!");
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	matches.clear();

	ocean_assert(candidateDescriptors != nullptr);
	if (numberQueryMultiDescriptorGroups == 0)
	{
		return;
	}

	ocean_assert(queryMultiDescriptorGroups != nullptr);

	if (worker && numberQueryMultiDescriptorGroups >= 50)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroupsSubset<TMultiDescriptorGroup, TMultiDescriptor, tMultiDescriptorGroupFunction, tMultiDescriptorFunction, tMatchingMode>, candidateDescriptors, queryMultiDescriptorGroups, maximalDistance, &matches, &lock, 0u, 0u), 0u, (unsigned int)(numberQueryMultiDescriptorGroups), 5u, 6u, 50u);
	}
	else
	{
		matchMultiDescriptorGroupsSubset<TMultiDescriptorGroup, TMultiDescriptor, tMultiDescriptorGroupFunction, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptorGroups, maximalDistance, &matches, nullptr, 0u, (unsigned int)(numberQueryMultiDescriptorGroups));
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
inline const TDescriptor& VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::nodeDescriptor() const
{
	return nodeDescriptor_;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
inline const Indices32& VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::descriptorIndices() const
{
	return descriptorIndices_;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
inline const typename VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::Nodes& VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::childNodes() const
{
	return childNodes_;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
typename VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::TDescriptors VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::clusterDescriptors(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, size_t numberDescriptorsIndices, RandomGenerator& randomGenerator, Index32* reusableClusterIndicesForDescriptors, const ClustersMeanFunction& clustersMeanFunction, Indices32* clusterSizes, Worker* worker)
{
	ocean_assert(parameters.isValid());
	ocean_assert(parameters.maximalNumberClustersPerLevel_ <= numberDescriptorsIndices);
	ocean_assert(reusableDescriptorIndicesInput != nullptr && reusableDescriptorIndicesOutput != nullptr);

	TDescriptors clusterCenters = initialClusters(parameters, treeDescriptors, reusableDescriptorIndicesInput, reusableDescriptorIndicesOutput, numberDescriptorsIndices, randomGenerator);
	const unsigned int numberClusters = (unsigned int)(clusterCenters.size());
	ocean_assert(numberClusters >= 1u && numberClusters * parameters.maximalDescriptorsPerLeaf_ <= numberDescriptorsIndices + parameters.maximalDescriptorsPerLeaf_);

	TSumDistances previousSumDistances = NumericT<TSumDistances>::maxValue();
	Indices32 internalClusterSizes;

	while (true)
	{
		if (!internalClusterSizes.empty()) // not in the first iteration
		{
			clusterCenters = clustersMeanFunction(numberClusters, treeDescriptors, reusableDescriptorIndicesInput, reusableClusterIndicesForDescriptors, numberDescriptorsIndices, worker);
		}

		TSumDistances sumDistances = NumericT<TSumDistances>::maxValue();
		internalClusterSizes = assignDescriptorsToClusters(clusterCenters.data(), numberClusters, treeDescriptors, reusableDescriptorIndicesInput, reusableClusterIndicesForDescriptors, numberDescriptorsIndices, &sumDistances, worker);
		ocean_assert(sumDistances != NumericT<TSumDistances>::maxValue());

		if (sumDistances >= previousSumDistances)
		{
			// we reached the optimal clustering
			break;
		}

		previousSumDistances = sumDistances;
	}

	ocean_assert(clusterCenters.size() >= 1);
	ocean_assert(internalClusterSizes.size() == clusterCenters.size());

	// now we move the descriptor indices based on the new clusters

	bool atLeastOneClusterEmpty = internalClusterSizes.front() == 0u;

	std::vector<Index32*> newDescriptorIndicesPerCluster;
	newDescriptorIndicesPerCluster.reserve(numberClusters);

	newDescriptorIndicesPerCluster.emplace_back(reusableDescriptorIndicesOutput);
	for (unsigned int nCluster = 1u; nCluster < numberClusters; ++nCluster)
	{
		newDescriptorIndicesPerCluster.emplace_back(newDescriptorIndicesPerCluster.back() + internalClusterSizes[nCluster - 1u]);

		if (internalClusterSizes[nCluster] == 0u)
		{
			atLeastOneClusterEmpty = true;
		}
	}

	for (size_t nDescriptor = 0; nDescriptor < numberDescriptorsIndices; ++nDescriptor)
	{
		const Index32& descriptorIndex = reusableDescriptorIndicesInput[nDescriptor];

		const Index32& clusterIndex = reusableClusterIndicesForDescriptors[descriptorIndex];

		*newDescriptorIndicesPerCluster[clusterIndex]++ = descriptorIndex;
	}

#ifdef OCEAN_DEBUG
	const Index32* debugNewDescriptorIndicesPerCluster = reusableDescriptorIndicesOutput;
	for (unsigned int nCluster = 0u; nCluster < numberClusters; ++nCluster)
	{
		debugNewDescriptorIndicesPerCluster += internalClusterSizes[nCluster];
		ocean_assert(newDescriptorIndicesPerCluster[nCluster] == debugNewDescriptorIndicesPerCluster);
	}
#endif

	if (atLeastOneClusterEmpty)
	{
		// we remove all empty clusters while keeping the order

		TDescriptors newClusterCenters;
		newClusterCenters.reserve(clusterCenters.size());

		Indices32 newInternalClusterSizes;
		newInternalClusterSizes.reserve(clusterCenters.size());

		for (size_t n = 0; n < clusterCenters.size(); ++n)
		{
			if (internalClusterSizes[n] != 0u)
			{
				newClusterCenters.emplace_back(clusterCenters[n]);
				newInternalClusterSizes.emplace_back(internalClusterSizes[n]);
			}
		}

		clusterCenters = std::move(newClusterCenters);
		internalClusterSizes = std::move(newInternalClusterSizes);
	}

	ocean_assert(internalClusterSizes.size() == clusterCenters.size());

	if (clusterSizes != nullptr)
	{
		*clusterSizes = std::move(internalClusterSizes);
	}

	return clusterCenters;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
typename VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::TDescriptors VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::initialClusters(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, size_t numberDescriptorsIndices, RandomGenerator& randomGenerator)
{
	ocean_assert(parameters.isValid());
	ocean_assert(parameters.maximalNumberClustersPerLevel_ <= numberDescriptorsIndices);
	ocean_assert(reusableDescriptorIndicesInput != nullptr);

	switch (parameters.initializationStrategy_)
	{
		case IS_LARGEST_DISTANCE:
			return initialClustersLargestDistance(parameters, treeDescriptors, reusableDescriptorIndicesInput, reusableDescriptorIndicesOutput, numberDescriptorsIndices, randomGenerator);

		default:
			ocean_assert(parameters.initializationStrategy_ == IS_PURE_RANDOM);
			return initialClustersPureRandom(parameters, treeDescriptors, reusableDescriptorIndicesInput, numberDescriptorsIndices, randomGenerator);
	};
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
typename VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::TDescriptors VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::initialClustersLargestDistance(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* descriptorIndices, Index32* reusableIndices, const size_t numberDescriptorsIndices, RandomGenerator& randomGenerator) const
{
	ocean_assert(parameters.isValid());
	ocean_assert(parameters.maximalNumberClustersPerLevel_ <= numberDescriptorsIndices);
	ocean_assert(treeDescriptors != nullptr && descriptorIndices != nullptr && reusableIndices != nullptr);
	ocean_assert(numberDescriptorsIndices >= 1);

	// the first cluster is selected randomly
	// afterwards, we determine the descriptors all having the largest distance too all existing clusters and we selected one of these descriptors as new cluster
	// we repeat this process until we have enough clusters

	const unsigned int maximalClusters = std::min(parameters.maximalNumberClustersPerLevel_, (unsigned int)(numberDescriptorsIndices + parameters.maximalDescriptorsPerLeaf_ - 1u) / parameters.maximalDescriptorsPerLeaf_);

	TDescriptors clusterCenters;
	clusterCenters.reserve(maximalClusters);

	clusterCenters.emplace_back(treeDescriptors[descriptorIndices[RandomI::random(randomGenerator, (unsigned int)(numberDescriptorsIndices) - 1u)]]);

	for (unsigned int nCluster = 1u; nCluster < maximalClusters; ++nCluster)
	{
		TDistance worstDistance = NumericT<TDistance>::minValue();
		unsigned int numberSameDistances = 0u;

		// now, we randomly select a descriptor with largest distance

		for (size_t nDescriptor = 0; nDescriptor < numberDescriptorsIndices; ++nDescriptor)
		{
			const Index32& descriptorIndex = descriptorIndices[nDescriptor];

			const TDescriptor& descriptor = treeDescriptors[descriptorIndex];

			TDistance localBestDistance = NumericT<TDistance>::maxValue();

			for (const TDescriptor& clusterCenter : clusterCenters)
			{
				const TDistance distance = tDistanceFunction(descriptor, clusterCenter);

				if (distance < localBestDistance)
				{
					localBestDistance = distance;
				}
			}

			if (localBestDistance > worstDistance)
			{
				worstDistance = localBestDistance;

				reusableIndices[0u] = descriptorIndex;
				numberSameDistances = 1u;
			}
			else if (localBestDistance == worstDistance)
			{
				reusableIndices[numberSameDistances++] = descriptorIndex;
			}
		}

		if (worstDistance == TDistance(0)) // other threshold
		{
			break;
		}

		if (numberSameDistances == 0u)
		{
			break;
		}

		const unsigned int randomIndex = RandomI::random(randomGenerator, numberSameDistances - 1u);

		const Index32& randomDescriptorIndex = reusableIndices[randomIndex];

		clusterCenters.emplace_back(treeDescriptors[randomDescriptorIndex]);
	}

	return clusterCenters;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
typename VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::TDescriptors VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::initialClustersPureRandom(const Parameters& parameters, const TDescriptor* treeDescriptors, Index32* descriptorIndices, const size_t numberDescriptorsIndices, RandomGenerator& randomGenerator) const
{
	ocean_assert(parameters.isValid());
	ocean_assert(parameters.maximalNumberClustersPerLevel_ <= numberDescriptorsIndices);
	ocean_assert(treeDescriptors != nullptr && descriptorIndices != nullptr);
	ocean_assert(numberDescriptorsIndices >= 1);

	const unsigned int maximalClusters = std::min(parameters.maximalNumberClustersPerLevel_, (unsigned int)(numberDescriptorsIndices + parameters.maximalDescriptorsPerLeaf_ - 1u) / parameters.maximalDescriptorsPerLeaf_);

	// first, we select the cluster centers randomly

	UnorderedIndexSet32 initialCenterIndices;
	initialCenterIndices.reserve(maximalClusters);

	while ((unsigned int)(initialCenterIndices.size()) < maximalClusters)
	{
		initialCenterIndices.emplace(RandomI::random(randomGenerator, (unsigned int)(numberDescriptorsIndices) - 1u));
	}

	TDescriptors clusterCenters;
	clusterCenters.reserve(maximalClusters);

	for (const Index32& randomIndex : initialCenterIndices)
	{
		const Index32& descriptorIndex = descriptorIndices[randomIndex];

		clusterCenters.emplace_back(treeDescriptors[descriptorIndex]);
	}

	return clusterCenters;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
VocabularyTree<TDescriptor, TDistance, tDistanceFunction>& VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::operator=(VocabularyTree<TDescriptor, TDistance, tDistanceFunction>&& VocabularyTree)
{
	if (this != &VocabularyTree)
	{
		nodeDescriptor_ = VocabularyTree.nodeDescriptor_;
		descriptorIndices_ = std::move(VocabularyTree.descriptorIndices_);
		childNodes_ = std::move(VocabularyTree.childNodes_);
	}

	return *this;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <unsigned int tSize>
typename VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::TDescriptors VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::determineClustersMeanForBinaryDescriptor(const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, const Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, Worker* /*worker todo*/)
{
	static_assert(tSize >= 1u && tSize % 8u == 0u, "Invalid descriptor size!");

#ifndef OCEAN_DONT_USE_LOOKUP_TABLE_IN_VOCABULARY_TREE
	static const std::vector<uint8_t> lookup(generateBitSeparationLookup8());
#endif

	constexpr unsigned int tBytes = tSize / 8u;

	ocean_assert(numberClusters >= 1u);
	ocean_assert(treeDescriptors != nullptr && descriptorIndices != nullptr);
	ocean_assert(numberDescriptorIndices >= 1u);
	ocean_assert(clusterIndicesForDescriptors != nullptr);

	Indices32 meanDescriptorsSum(numberClusters * tSize, 0u);
	Indices32 numberDescriptorsInClusters(numberClusters, 0u);

	for (size_t nDescriptor = 0; nDescriptor < numberDescriptorIndices; ++nDescriptor)
	{
		const Index32& descriptorIndex = descriptorIndices[nDescriptor];

		ocean_assert(clusterIndicesForDescriptors[descriptorIndex] < numberClusters);

		++numberDescriptorsInClusters[clusterIndicesForDescriptors[descriptorIndex]];
		Index32* meanDescriptor = meanDescriptorsSum.data() + clusterIndicesForDescriptors[descriptorIndex] * tSize;

		const uint8_t* const descriptor = (const uint8_t*)(treeDescriptors + descriptorIndex);

		for (unsigned int nByte = 0u; nByte < tBytes; ++nByte)
		{
#ifdef OCEAN_DONT_USE_LOOKUP_TABLE_IN_VOCABULARY_TREE
			for (unsigned int nBit = 0u; nBit < 8u; ++nBit)
			{
				if (descriptor[nByte] & (1u << nBit))
				{
					(*meanDescriptor)++;
				}

				++meanDescriptor;
			}
#else
			const uint8_t* lookupValues = lookup.data() + descriptor[nByte] * 8;

			for (unsigned int nBit = 0u; nBit < 8u; ++nBit)
			{
				*meanDescriptor++ += lookupValues[nBit];
			}
#endif // OCEAN_DONT_USE_LOOKUP_TABLE_IN_VOCABULARY_TREE
		}
	}

	TDescriptors meanDescriptors(numberClusters);

	for (unsigned int nCluster = 0u; nCluster < numberClusters; ++nCluster)
	{
		const unsigned int* meanDescriptorSum = meanDescriptorsSum.data() + nCluster * tSize;

		uint8_t* meanDescriptor = (uint8_t*)(meanDescriptors.data() + nCluster);

		if (numberDescriptorsInClusters[nCluster] != 0u)
		{
			for (unsigned int nByte = 0u; nByte < tBytes; ++nByte)
			{
				uint8_t byte = 0u;

				for (unsigned int nBit = 0u; nBit < 8u; ++nBit)
				{
					const unsigned int meanBit = (*meanDescriptorSum + numberDescriptorsInClusters[nCluster] / 2u) / numberDescriptorsInClusters[nCluster];
					ocean_assert(meanBit == 0u || meanBit == 1u);

					byte = byte | uint8_t(meanBit << nBit);

					++meanDescriptorSum;
				}

				meanDescriptor[nByte] = byte;
			}
		}
		else
		{
			// forcing the mean descriptor to be zero in case default constructor does not create a zero descriptor
			memset(meanDescriptor, 0, tBytes);
		}
	}

	return meanDescriptors;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <unsigned int tSize>
typename VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::TDescriptors VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::determineClustersMeanForFloatDescriptor(const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, const Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, Worker* /*worker*/)
{
	static_assert(tSize >= 1u, "Invalid descriptor size!");

	// **TODO** add multi-core implementation

	ocean_assert(numberClusters >= 1u);
	ocean_assert(treeDescriptors != nullptr && descriptorIndices != nullptr);
	ocean_assert(numberDescriptorIndices >= 1u);
	ocean_assert(clusterIndicesForDescriptors != nullptr);

	std::vector<float> meanDescriptorsSum(numberClusters * tSize, 0.0);
	Indices32 numberDescriptorsInClusters(numberClusters, 0u);

	for (size_t nDescriptor = 0; nDescriptor < numberDescriptorIndices; ++nDescriptor)
	{
		const Index32& descriptorIndex = descriptorIndices[nDescriptor];

		ocean_assert(clusterIndicesForDescriptors[descriptorIndex] < numberClusters);

		++numberDescriptorsInClusters[clusterIndicesForDescriptors[descriptorIndex]];
		float* meanDescriptor = meanDescriptorsSum.data() + clusterIndicesForDescriptors[descriptorIndex] * tSize;

		const float* const descriptor = (const float*)(treeDescriptors + descriptorIndex);

		for (unsigned int nElement = 0u; nElement < tSize; ++nElement)
		{
			meanDescriptor[nElement] += descriptor[nElement];
		}
	}

	TDescriptors meanDescriptors(numberClusters);

	for (unsigned int nCluster = 0u; nCluster < numberClusters; ++nCluster)
	{
		const float* meanDescriptorSum = meanDescriptorsSum.data() + nCluster * tSize;

		float* meanDescriptor = (float*)(meanDescriptors.data() + nCluster);

		if (numberDescriptorsInClusters[nCluster] != 0u)
		{
			const float invDescriptorsInCluster = 1.0f / float(numberDescriptorsInClusters[nCluster]);

			for (unsigned int nElement = 0u; nElement < tSize; ++nElement)
			{
				meanDescriptor[nElement] = meanDescriptorSum[nElement] * invDescriptorsInCluster;
			}
		}
		else
		{
			memset(meanDescriptor, 0, sizeof(float) * tSize);

#ifdef OCEAN_DEBUG
			for (unsigned int nElement = 0u; nElement < tSize; ++nElement)
			{
				ocean_assert(meanDescriptor[nElement] == 0.0f);
			}
#endif
		}
	}

	return meanDescriptors;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
Indices32 VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::assignDescriptorsToClusters(const TDescriptor* clusterCenters, const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, Index32* clusterIndicesForDescriptors, const size_t numberDescriptorIndices, TSumDistances* sumDistances, Worker* worker)
{
	TSumDistances localSumDistances = TSumDistances(0);
	Indices32 clusterSizes(numberClusters, 0u);

	if (worker != nullptr && numberDescriptorIndices * numberClusters >= 50000)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::assignDescriptorsToClustersSubset, clusterCenters, numberClusters, treeDescriptors, descriptorIndices, clusterIndicesForDescriptors, clusterSizes.data(), &localSumDistances, &lock, 0u, 0u), 0u, (unsigned int)(numberDescriptorIndices));
	}
	else
	{
		assignDescriptorsToClustersSubset(clusterCenters, numberClusters, treeDescriptors, descriptorIndices, clusterIndicesForDescriptors, clusterSizes.data(), &localSumDistances, nullptr, 0u, (unsigned int)(numberDescriptorIndices));
	}

	if (sumDistances != nullptr)
	{
		*sumDistances = localSumDistances;
	}

	return clusterSizes;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::createChildNodesSubset(const unsigned int childNodeLevel, const Parameters* parameters, const TDescriptor* treeDescriptors, const TDescriptor* clusterCenters, const unsigned int* clusterSizes, const size_t numberClusters, Index32* reusableDescriptorIndicesInput, Index32* reusableDescriptorIndicesOutput, const size_t numberDescriptorsIndices, RandomGenerator* randomGenerator, Index32* reusableClusterIndicesForDescriptors, const ClustersMeanFunction* clustersMeanFunction, const unsigned int subsetFirstCluster, const unsigned int subsetNumberClusters)
{
	ocean_assert(childNodeLevel >= 1u);
	ocean_assert(parameters != nullptr && parameters->isValid());
	ocean_assert(clusterCenters != nullptr && clusterSizes != nullptr);
	ocean_assert(numberClusters >= 1u && subsetNumberClusters >= 1);
	ocean_assert(reusableDescriptorIndicesInput != nullptr && reusableDescriptorIndicesOutput != nullptr);

	unsigned int descriptorClusterOffset = 0u;

	for (unsigned int nCluster = 0u; nCluster < subsetFirstCluster; ++nCluster)
	{
		descriptorClusterOffset += clusterSizes[nCluster];
	}

	for (unsigned int nCluster = subsetFirstCluster; nCluster < subsetFirstCluster + subsetNumberClusters; ++nCluster)
	{
		ocean_assert(nCluster < childNodes_.size());

		const size_t subsetNumberDescriptorIndices = clusterSizes[nCluster];

		if (subsetNumberDescriptorIndices > 0)
		{
			const TDescriptor& nodeDescriptor = clusterCenters[nCluster];

			ocean_assert_and_suppress_unused(descriptorClusterOffset + subsetNumberDescriptorIndices <= numberDescriptorsIndices, numberDescriptorsIndices);

			Index32* const subsetReusableDescriptorIndicesInput = reusableDescriptorIndicesInput + descriptorClusterOffset;
			Index32* const subsetReusableDescriptorIndicesOutput = reusableDescriptorIndicesOutput + descriptorClusterOffset;

			Node* childNode = nullptr;

			if (numberClusters == 1 || subsetNumberDescriptorIndices <= parameters->maximalDescriptorsPerLeaf_ || childNodeLevel + 1u >= parameters->maximalLevels_)
			{
				childNode = new Node(childNodeLevel, nodeDescriptor, subsetReusableDescriptorIndicesInput, subsetNumberDescriptorIndices);
			}
			else
			{
				constexpr Worker* noWorker = nullptr; // we apply a worker in the lowest level only

				childNode = new Node(childNodeLevel, nodeDescriptor, *parameters, treeDescriptors, subsetReusableDescriptorIndicesInput, subsetReusableDescriptorIndicesOutput, subsetNumberDescriptorIndices, *randomGenerator, reusableClusterIndicesForDescriptors, *clustersMeanFunction, noWorker);
			}

			ocean_assert(childNodes_[nCluster] == nullptr);
			childNodes_[nCluster] = childNode;

			descriptorClusterOffset += clusterSizes[nCluster];
		}
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::assignDescriptorsToClustersSubset(const TDescriptor* clusterCenters, const unsigned int numberClusters, const TDescriptor* treeDescriptors, const Index32* descriptorIndices, Index32* clusterIndicesForDescriptors, Index32* clusterSizes, TSumDistances* sumDistances, Lock* lock, const unsigned int firstDescriptorIndex, const unsigned int numberDescriptorIndices)
{
	ocean_assert(clusterCenters != nullptr && numberClusters >= 1u);
	ocean_assert(treeDescriptors != nullptr && descriptorIndices != nullptr);
	ocean_assert(clusterIndicesForDescriptors != nullptr);
	ocean_assert(clusterSizes != nullptr);
	ocean_assert(numberDescriptorIndices >= 1u);

	Indices32 localClusterSizes(numberClusters, 0u);
	TSumDistances localSumDistances = TSumDistances(0);

	TDistance bestDistance;
	unsigned int bestCluster;

	for (unsigned int nDescriptor = firstDescriptorIndex; nDescriptor < firstDescriptorIndex + numberDescriptorIndices; ++nDescriptor)
	{
		const Index32& descriptorIndex = descriptorIndices[nDescriptor];
		const TDescriptor& descriptor = treeDescriptors[descriptorIndex];

		bestDistance = NumericT<TDistance>::maxValue();
		bestCluster = (unsigned int)(-1);

		for (unsigned int nCluster = 0u; nCluster < numberClusters; ++nCluster)
		{
			const TDistance distance = tDistanceFunction(clusterCenters[nCluster], descriptor);

			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestCluster = nCluster;
			}
		}

		ocean_assert(bestCluster < numberClusters);

		clusterIndicesForDescriptors[descriptorIndex] = bestCluster;

		++localClusterSizes[bestCluster];

		localSumDistances += bestDistance;
	}

	const OptionalScopedLock scopedLock(lock);

	for (unsigned int n = 0u; n < numberClusters; ++n)
	{
		clusterSizes[n] += localClusterSizes[n];
	}

	if (sumDistances)
	{
		*sumDistances += localSumDistances;
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchDescriptorsSubset(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryDescriptor, const unsigned int numberQueryDescriptors) const
{
	ocean_assert(candidateDescriptors != nullptr);
	ocean_assert(matches != nullptr);
	ocean_assert(numberQueryDescriptors >= 1u);

	ReusableData reusableData;

	Matches localMatches;
	localMatches.reserve(numberQueryDescriptors);

	for (unsigned int nQuery = firstQueryDescriptor; nQuery < firstQueryDescriptor + numberQueryDescriptors; ++nQuery)
	{
		TDistance distance = NumericT<TDistance>::maxValue();
		const Index32 matchingCandidateIndex = matchDescriptor<tMatchingMode>(candidateDescriptors, queryDescriptors[nQuery], &distance, reusableData);

		if (distance <= maximalDistance)
		{
			ocean_assert(matchingCandidateIndex != invalidMatchIndex());

			localMatches.emplace_back(matchingCandidateIndex, nQuery, distance);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	matches->insert(matches->cend(), localMatches.cbegin(), localMatches.cend());
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptor, const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptor, const unsigned int numberQueryMultiDescriptors) const
{
	ocean_assert(candidateDescriptors != nullptr);
	ocean_assert(matches != nullptr);
	ocean_assert(numberQueryMultiDescriptors >= 1u);

	ReusableData reusableData;

	Matches localMatches;
	localMatches.reserve(numberQueryMultiDescriptors);

	for (unsigned int nQuery = firstQueryMultiDescriptor; nQuery < firstQueryMultiDescriptor + numberQueryMultiDescriptors; ++nQuery)
	{
		TDistance distance = NumericT<TDistance>::maxValue();
		const Index32 matchingCandidateIndex = matchMultiDescriptor<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptors[nQuery], &distance, reusableData);

		if (distance <= maximalDistance)
		{
			ocean_assert(matchingCandidateIndex != invalidMatchIndex());

			localMatches.emplace_back(matchingCandidateIndex, nQuery, distance);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	matches->insert(matches->cend(), localMatches.cbegin(), localMatches.cend());
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptorGroup, typename TMultiDescriptor, const TMultiDescriptor*(*tMultiDescriptorGroupFunction)(const TMultiDescriptorGroup&, const size_t), const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyTree<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroupsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptorGroup, const unsigned int numberQueryMultiDescriptorGroups) const
{
	ocean_assert(candidateDescriptors != nullptr);
	ocean_assert(matches != nullptr);
	ocean_assert(numberQueryMultiDescriptorGroups >= 1u);

	ReusableData reusableData;

	Matches localMatches;
	localMatches.reserve(numberQueryMultiDescriptorGroups);

	for (unsigned int nQuery = firstQueryMultiDescriptorGroup; nQuery < firstQueryMultiDescriptorGroup + numberQueryMultiDescriptorGroups; ++nQuery)
	{
		TDistance distance = NumericT<TDistance>::maxValue();
		const Index32 matchingCandidateIndex = matchMultiDescriptorGroup<TMultiDescriptorGroup, TMultiDescriptor, tMultiDescriptorGroupFunction, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptorGroups[nQuery], &distance, reusableData);

		if (distance <= maximalDistance)
		{
			ocean_assert(matchingCandidateIndex != invalidMatchIndex());

			localMatches.emplace_back(matchingCandidateIndex, nQuery, distance);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	matches->insert(matches->cend(), localMatches.cbegin(), localMatches.cend());
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::VocabularyForest(const size_t numberTrees, const TDescriptor* treeDescriptors, const size_t numberDescriptors, const ClustersMeanFunction& clustersMeanFunction, const Parameters& parameters, Worker* worker, RandomGenerator* randomGenerator)
{
	ocean_assert(numberTrees >= 1u);

	for (size_t n = 0; n < numberTrees; ++n)
	{
		vocabularyTrees_.emplace_back(treeDescriptors, numberDescriptors, clustersMeanFunction, parameters, worker, randomGenerator);
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor& queryDescriptor, TDistance* distance, const ReusableData& reusableData) const
{
	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	for (const VocabularyTree<TDescriptor, TDistance, tDistanceFunction>& vocabularyTree : vocabularyTrees_)
	{
		TDistance candidateDistance;
		const Index32 candidateDescriptorIndex = vocabularyTree.template matchDescriptor<tMatchingMode>(candidateDescriptors, queryDescriptor, &candidateDistance, reusableData);

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestCandidateDescriptorIndex = candidateDescriptorIndex;
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TDescriptor* queryMultiDescriptor, const size_t numberQuerySingleDescriptors, TDistance* distance, const ReusableData& reusableData) const
{
	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	for (const TVocabularyTree& vocabularyTree : vocabularyTrees_)
	{
		TDistance candidateDistance;
		const Index32 candidateDescriptorIndex = vocabularyTree.template matchMultiDescriptor<tMatchingMode>(candidateDescriptors, queryMultiDescriptor, numberQuerySingleDescriptors, &candidateDistance, reusableData);

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestCandidateDescriptorIndex = candidateDescriptorIndex;
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptor, const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptor(const TDescriptor* candidateDescriptors, const TMultiDescriptor& queryMultiDescriptor, TDistance* distance, const ReusableData& reusableData) const
{
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	for (const TVocabularyTree& vocabularyTree : vocabularyTrees_)
	{
		TDistance candidateDistance;
		const Index32 candidateDescriptorIndex = vocabularyTree.template matchMultiDescriptor<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptor, &candidateDistance, reusableData);

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestCandidateDescriptorIndex = candidateDescriptorIndex;
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptorGroup, typename TMultiDescriptor, const TMultiDescriptor*(*tMultiDescriptorGroupFunction)(const TMultiDescriptorGroup&, const size_t), const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
Index32 VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroup(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup& queryMultiDescriptorGroup, TDistance* distance, const ReusableData& reusableData) const
{
	static_assert(tMultiDescriptorGroupFunction != nullptr, "Invalid function!");
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	Index32 bestCandidateDescriptorIndex = invalidMatchIndex();
	TDistance bestDistance = NumericT<TDistance>::maxValue();

	for (const TVocabularyTree& vocabularyTree : vocabularyTrees_)
	{
		TDistance candidateDistance;
		const Index32 candidateDescriptorIndex = vocabularyTree.template matchMultiDescriptorGroup<TMultiDescriptorGroup, TMultiDescriptor, tMultiDescriptorGroupFunction, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptorGroup, &candidateDistance, reusableData);

		if (candidateDistance < bestDistance)
		{
			bestDistance = candidateDistance;
			bestCandidateDescriptorIndex = candidateDescriptorIndex;
		}
	}

	if (distance != nullptr)
	{
		*distance = bestDistance;
	}

	return bestCandidateDescriptorIndex;
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchDescriptors(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const size_t numberQueryDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker) const
{
	matches.clear();

	ocean_assert(candidateDescriptors != nullptr);
	if (numberQueryDescriptors == 0)
	{
		return;
	}

	ocean_assert(queryDescriptors != nullptr);

	if (worker && numberQueryDescriptors >= 50)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchDescriptorsSubset<tMatchingMode>, candidateDescriptors, queryDescriptors, maximalDistance, &matches, &lock, 0u, 0u), 0u, (unsigned int)(numberQueryDescriptors), 5u, 6u, 50u);
	}
	else
	{
		matchDescriptorsSubset<tMatchingMode>(candidateDescriptors, queryDescriptors, maximalDistance, &matches, nullptr, 0u, (unsigned int)(numberQueryDescriptors));
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptor, const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptors(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const size_t numberQueryMultiDescriptors, const TDistance maximalDistance, Matches& matches, Worker* worker) const
{
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	matches.clear();

	ocean_assert(candidateDescriptors != nullptr);
	if (numberQueryMultiDescriptors == 0)
	{
		return;
	}

	ocean_assert(queryMultiDescriptors != nullptr);

	if (worker && numberQueryMultiDescriptors >= 50)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorsSubset<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>, candidateDescriptors, queryMultiDescriptors, maximalDistance, &matches, &lock, 0u, 0u), 0u, (unsigned int)(numberQueryMultiDescriptors), 5u, 6u, 50u);
	}
	else
	{
		matchMultiDescriptorsSubset<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptors, maximalDistance, &matches, nullptr, 0u, (unsigned int)(numberQueryMultiDescriptors));
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptorGroup, typename TMultiDescriptor, const TMultiDescriptor*(*tMultiDescriptorGroupFunction)(const TMultiDescriptorGroup&, const size_t), const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroups(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const size_t numberQueryMultiDescriptorGroups, const TDistance maximalDistance, Matches& matches, Worker* worker) const
{
	static_assert(tMultiDescriptorGroupFunction != nullptr, "Invalid function!");
	static_assert(tMultiDescriptorFunction != nullptr, "Invalid function!");

	matches.clear();

	ocean_assert(candidateDescriptors != nullptr);
	if (numberQueryMultiDescriptorGroups == 0)
	{
		return;
	}

	ocean_assert(queryMultiDescriptorGroups != nullptr);

	if (worker && numberQueryMultiDescriptorGroups >= 50)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroupsSubset<TMultiDescriptorGroup, TMultiDescriptor, tMultiDescriptorGroupFunction, tMultiDescriptorFunction, tMatchingMode>, candidateDescriptors, queryMultiDescriptorGroups, maximalDistance, &matches, &lock, 0u, 0u), 0u, (unsigned int)(numberQueryMultiDescriptorGroups), 5u, 6u, 50u);
	}
	else
	{
		matchMultiDescriptorGroupsSubset<TMultiDescriptorGroup, TMultiDescriptor, tMultiDescriptorGroupFunction, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptorGroups, maximalDistance, &matches, nullptr, 0u, (unsigned int)(numberQueryMultiDescriptorGroups));
	}
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchDescriptorsSubset(const TDescriptor* candidateDescriptors, const TDescriptor* queryDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryDescriptor, const unsigned int numberQueryDescriptors) const
{
	ocean_assert(candidateDescriptors != nullptr);
	ocean_assert(matches != nullptr);
	ocean_assert(numberQueryDescriptors >= 1u);

	ReusableData reusableData;

	Matches localMatches;
	localMatches.reserve(numberQueryDescriptors);

	for (unsigned int nQuery = firstQueryDescriptor; nQuery < firstQueryDescriptor + numberQueryDescriptors; ++nQuery)
	{
		TDistance distance = NumericT<TDistance>::maxValue();
		const Index32 matchingCandidateIndex = matchDescriptor<tMatchingMode>(candidateDescriptors, queryDescriptors[nQuery], &distance, reusableData);

		if (distance <= maximalDistance)
		{
			ocean_assert(matchingCandidateIndex != invalidMatchIndex());

			localMatches.emplace_back(matchingCandidateIndex, nQuery, distance);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	matches->insert(matches->cend(), localMatches.cbegin(), localMatches.cend());
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptor, const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptor* queryMultiDescriptors, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptor, const unsigned int numberQueryMultiDescriptors) const
{
	ocean_assert(candidateDescriptors != nullptr);
	ocean_assert(matches != nullptr);
	ocean_assert(numberQueryMultiDescriptors >= 1u);

	ReusableData reusableData;

	Matches localMatches;
	localMatches.reserve(numberQueryMultiDescriptors);

	for (unsigned int nQuery = firstQueryMultiDescriptor; nQuery < firstQueryMultiDescriptor + numberQueryMultiDescriptors; ++nQuery)
	{
		TDistance distance = NumericT<TDistance>::maxValue();
		const Index32 matchingCandidateIndex = matchMultiDescriptor<TMultiDescriptor, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptors[nQuery], &distance, reusableData);

		if (distance <= maximalDistance)
		{
			ocean_assert(matchingCandidateIndex != invalidMatchIndex());

			localMatches.emplace_back(matchingCandidateIndex, nQuery, distance);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	matches->insert(matches->cend(), localMatches.cbegin(), localMatches.cend());
}

template <typename TDescriptor, typename TDistance, TDistance(*tDistanceFunction)(const TDescriptor&, const TDescriptor&)>
template <typename TMultiDescriptorGroup, typename TMultiDescriptor, const TMultiDescriptor*(*tMultiDescriptorGroupFunction)(const TMultiDescriptorGroup&, const size_t), const TDescriptor*(*tMultiDescriptorFunction)(const TMultiDescriptor&, const size_t), VocabularyStructure::MatchingMode tMatchingMode>
void VocabularyForest<TDescriptor, TDistance, tDistanceFunction>::matchMultiDescriptorGroupsSubset(const TDescriptor* candidateDescriptors, const TMultiDescriptorGroup* queryMultiDescriptorGroups, const TDistance maximalDistance, Matches* matches, Lock* lock, const unsigned int firstQueryMultiDescriptorGroup, const unsigned int numberQueryMultiDescriptorGroups) const
{
	ocean_assert(candidateDescriptors != nullptr);
	ocean_assert(matches != nullptr);
	ocean_assert(numberQueryMultiDescriptorGroups >= 1u);

	ReusableData reusableData;

	Matches localMatches;
	localMatches.reserve(numberQueryMultiDescriptorGroups);

	for (unsigned int nQuery = firstQueryMultiDescriptorGroup; nQuery < firstQueryMultiDescriptorGroup + numberQueryMultiDescriptorGroups; ++nQuery)
	{
		TDistance distance = NumericT<TDistance>::maxValue();
		const Index32 matchingCandidateIndex = matchMultiDescriptorGroup<TMultiDescriptorGroup, TMultiDescriptor, tMultiDescriptorGroupFunction, tMultiDescriptorFunction, tMatchingMode>(candidateDescriptors, queryMultiDescriptorGroups[nQuery], &distance, reusableData);

		if (distance <= maximalDistance)
		{
			ocean_assert(matchingCandidateIndex != invalidMatchIndex());

			localMatches.emplace_back(matchingCandidateIndex, nQuery, distance);
		}
	}

	const OptionalScopedLock scopedLock(lock);

	matches->insert(matches->cend(), localMatches.cbegin(), localMatches.cend());
}

}

}

#endif // META_OCEAN_TRACKING_VOCABULAR_TREE_H
