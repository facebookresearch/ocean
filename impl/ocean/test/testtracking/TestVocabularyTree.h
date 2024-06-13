/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTACKING_TEST_VOCABULARY_TREE_H
#define META_OCEAN_TEST_TESTTACKING_TEST_VOCABULARY_TREE_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/tracking/VocabularyTree.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements a test for the VocabularyTree class.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestVocabularyTree
{
	public:

		/**
		 * Definition of individual descriptor types.
		 */
		enum DescriptorType
		{
			/// Descriptor based on binary data.
			DT_BINARY,
			/// Descriptor based on floats.
			DT_FLOAT
		};

	protected:

		/// The number of elements/bytes of a binary descriptor.
		static constexpr unsigned int binaryDescriptorElements_ = 32u;

		/// The number of elements of a float descriptor.
		static constexpr unsigned int floatDescriptorElements_ = 128u;

		/**
		 * Definition of a binary descriptor.
		 */
		using BinaryDescriptor = std::array<uint8_t, binaryDescriptorElements_>;

		/**
		 * Definition of a float descriptor.
		 */
		using FloatDescriptor = std::array<float, floatDescriptorElements_>;

		/**
		 * Helper class to determine data types for individual descriptor types.
		 * @tparam tDescriptorType The type of the descriptor to be sued
		 */
		template <DescriptorType tDescriptorType>
		class TypeHelper
		{
			// nothing to do here
		};

	public:

		/**
		 * Invokes all existing tests for VocabularyTree.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the function determining the means for clusters of binary descriptors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testDetermineClustersMeanForBinaryDescriptor(const double testDuration, Worker& worker);

		/**
		 * Tests the function determining the means for clusters of float descriptors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testDetermineClustersMeanForFloatDescriptor(const double testDuration, Worker& worker);

		/**
		 * Tests the constructor.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tDescriptorType The descriptor type to be used
		 */
		template <DescriptorType tDescriptorType>
		static bool testConstructor(const double testDuration, Worker& worker);

		/**
		 * Tests manual matching accessing the leafs.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tDescriptorType The descriptor type to be used
		 */
		template <DescriptorType tDescriptorType>
		static bool testMatchingViaLeafs(const double testDuration, Worker& worker);

		/**
		 * Tests descriptor matching.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tDescriptorType The descriptor type to be used
		 */
		template <DescriptorType tDescriptorType>
		static bool testMatchingDescriptors(const double testDuration, Worker& worker);

		/**
		 * Tests descriptor matching with forest.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tDescriptorType The descriptor type to be used
		 */
		template <DescriptorType tDescriptorType>
		static bool testMatchingDescriptorsWithForest(const double testDuration, Worker& worker);

	protected:

		/**
		 * Separates the individual bits of binary descriptor into individual integer values.
		 * @param descriptor The binary descriptor to be separated
		 * @return The resulting integer values, one for each bit
		 */
		static Indices32 separateBinaryDescriptor(const BinaryDescriptor& descriptor);
};

template <>
class OCEAN_TEST_TRACKING_EXPORT TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_BINARY>
{
	public:

		/**
		 * The readable name of the descriptor type.
		 */
		static constexpr const char* name_ = "Binary";

		/**
		 * Definition of a descriptor data type.
		 */
		using Descriptor = BinaryDescriptor;

		/**
		 * Definition of a vector holding descriptors.
		 */
		using Descriptors = std::vector<Descriptor>;

		/**
		 * Definition of the data type for the distance between two descriptors.
		 */
		using DistanceType = unsigned int;

		/**
		 * Determines the (hamming) distance between two binary descriptors.
		 * @param descriptorA The first descriptor
		 * @param descriptorB The second descriptor
		 * @return The resulting distance
		 */
		static DistanceType determineDistance(const BinaryDescriptor& descriptorA, const BinaryDescriptor& descriptorB);

		/**
		 * Randomizes a descriptor.
		 * @param descriptor The descriptor to be randomized
		 * @param randomGenerator The random generator object to be used
		 */
		static void randomizeDescriptor(BinaryDescriptor& descriptor, RandomGenerator& randomGenerator);

		/**
		 * Applies a minor random modification to a given descriptor.
		 * @param descriptor The descriptor to modify
		 * @param randomGenerator The random generator object to be used
		 * @return The modified descriptor
		 */
		static BinaryDescriptor modifyDescriptor(const BinaryDescriptor& descriptor, RandomGenerator& randomGenerator);

		/**
		 * Returns a bunch of descriptor epsilons which can be used for testing.
		 * @param numberEpsilons The number of epsilon value to return, with range [2, numberBits]
		 * @return The descriptor epsilons
		 */
		static std::vector<DistanceType> descriptorEpsilons(const unsigned int numberEpsilons);

		/**
		 * Definition of the vocabulary tree data type.
		 */
		using VocabularyTree = Tracking::VocabularyTree<Descriptor, DistanceType, determineDistance>;

		/**
		 * The function pointer to the cluster mean function.
		 */
		static constexpr const VocabularyTree::ClustersMeanFunction clusterMeanFunction_ = &VocabularyTree::determineClustersMeanForBinaryDescriptor<binaryDescriptorElements_ * 8u>;
};

template <>
class OCEAN_TEST_TRACKING_EXPORT TestVocabularyTree::TypeHelper<TestVocabularyTree::DT_FLOAT>
{
	public:

		/**
		 * The readable name of the descriptor type.
		 */
		static constexpr const char* name_ = "Float";

		/**
		 * Definition of a descriptor data type.
		 */
		using Descriptor = FloatDescriptor;

		/**
		 * Definition of a vector holding descriptors.
		 */
		using Descriptors = std::vector<Descriptor>;

		/**
		 * Definition of the data type for the distance between two descriptors.
		 */
		using DistanceType = float;

		/**
		 * Determines the square distance between two float descriptors.
		 * @param descriptorA The first descriptor
		 * @param descriptorB The second descriptor
		 * @return The resulting square distance
		 */
		static float determineDistance(const FloatDescriptor& descriptorA, const FloatDescriptor& descriptorB);

		/**
		 * Randomizes a descriptor.
		 * @param descriptor The descriptor to be randomized
		 * @param randomGenerator The random generator object to be used
		 */
		static void randomizeDescriptor(FloatDescriptor& descriptor, RandomGenerator& randomGenerator);

		/**
		 * Applies a minor random modification to a given descriptor.
		 * @param descriptor The descriptor to modify
		 * @param randomGenerator The random generator object to be used
		 * @return The modified descriptor
		 */
		static FloatDescriptor modifyDescriptor(const FloatDescriptor& descriptor, RandomGenerator& randomGenerator);

		/**
		 * Returns a bunch of descriptor epsilons which can be used for testing.
		 * @param numberEpsilons The number of epsilon value to return, with range [2, numberBits]
		 * @return The descriptor epsilons
		 */
		static std::vector<DistanceType> descriptorEpsilons(const unsigned int numberEpsilons);

		/**
		 * Definition of the vocabulary tree data type.
		 */
		using VocabularyTree = Tracking::VocabularyTree<Descriptor, DistanceType, determineDistance>;

		/**
		 * The function pointer to the cluster mean function.
		 */
		static constexpr const VocabularyTree::ClustersMeanFunction clusterMeanFunction_ = &VocabularyTree::determineClustersMeanForFloatDescriptor<floatDescriptorElements_>;
};

}

}

}

#endif // META_OCEAN_TEST_TESTTACKING_TEST_VOCABULARY_TREE_H
