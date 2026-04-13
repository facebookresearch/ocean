/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>

#include "ocean/base/RandomI.h"
#include "ocean/base/RandomGenerator.h"

#include <algorithm>
#include <cstdint>
#include <set>
#include <vector>

namespace {

// Components: RandomI + RandomGenerator
// Tests that RandomI::random32 with a RandomGenerator produces valid 32-bit values
// and that different seeds produce different sequences.
TEST(RandomIRandomGeneratorIntegrationTest, Random32WithGeneratorProducesFullRange) {
  Ocean::RandomGenerator generator1(12345u);
  Ocean::RandomGenerator generator2(12345u);

  // Same seed should produce same sequence
  const uint32_t val1a = Ocean::RandomI::random32(generator1);
  const uint32_t val1b = Ocean::RandomI::random32(generator2);
  EXPECT_EQ(val1a, val1b) << "Same seed should produce identical first value";

  const uint32_t val2a = Ocean::RandomI::random32(generator1);
  const uint32_t val2b = Ocean::RandomI::random32(generator2);
  EXPECT_EQ(val2a, val2b) << "Same seed should produce identical second value";

  // Different seed should (very likely) produce different values
  // With overwhelming probability, different seeds produce different first values
  // We generate multiple to reduce flakiness
  bool anyDifferent = false;
  Ocean::RandomGenerator genA(12345u);
  Ocean::RandomGenerator genB(99999u);
  for (int i = 0; i < 10; ++i) {
    if (Ocean::RandomI::random32(genA) != Ocean::RandomI::random32(genB)) {
      anyDifferent = true;
      break;
    }
  }
  EXPECT_TRUE(anyDifferent) << "Different seeds should produce different sequences";
}

// Components: RandomI + RandomGenerator
// Tests that RandomI::random64 with a RandomGenerator produces valid 64-bit values.
TEST(RandomIRandomGeneratorIntegrationTest, Random64WithGeneratorProducesValues) {
  Ocean::RandomGenerator generator1(42u);
  Ocean::RandomGenerator generator2(42u);

  // Same seed should produce same 64-bit sequence
  const uint64_t val1a = Ocean::RandomI::random64(generator1);
  const uint64_t val1b = Ocean::RandomI::random64(generator2);
  EXPECT_EQ(val1a, val1b) << "Same seed should produce identical 64-bit value";

  // Verify the value uses more than 32 bits (with high probability)
  // Generate several values and check that at least one has high bits set
  Ocean::RandomGenerator gen(777u);
  bool hasHighBits = false;
  for (int i = 0; i < 100; ++i) {
    uint64_t val = Ocean::RandomI::random64(gen);
    if (val > 0xFFFFFFFFull) {
      hasHighBits = true;
      break;
    }
  }
  EXPECT_TRUE(hasHighBits) << "random64 should produce values using more than 32 bits";
}

// Components: RandomI + RandomGenerator
// Tests that RandomI::random(generator, maxValue) respects the upper bound.
TEST(RandomIRandomGeneratorIntegrationTest, RandomWithGeneratorRespectsMaxValue) {
  Ocean::RandomGenerator generator(54321u);

  const unsigned int maxValue = 100u;
  for (int i = 0; i < 1000; ++i) {
    const unsigned int val = Ocean::RandomI::random(generator, maxValue);
    ASSERT_LE(val, maxValue) << "Value must be <= maxValue";
  }
}

// Components: RandomI + RandomGenerator
// Tests that RandomI::random(generator, lower, upper) for signed integers
// produces values within the specified range.
TEST(RandomIRandomGeneratorIntegrationTest, RandomSignedRangeWithGenerator) {
  Ocean::RandomGenerator generator(11111u);

  const int lower = -50;
  const int upper = 50;
  bool hasNegative = false;
  bool hasPositive = false;

  for (int i = 0; i < 1000; ++i) {
    const int val = Ocean::RandomI::random(generator, lower, upper);
    ASSERT_GE(val, lower) << "Value must be >= lower bound";
    ASSERT_LE(val, upper) << "Value must be <= upper bound";
    if (val < 0) {
      hasNegative = true;
    }
    if (val > 0) {
      hasPositive = true;
    }
  }

  EXPECT_TRUE(hasNegative) << "Should produce negative values in range [-50, 50]";
  EXPECT_TRUE(hasPositive) << "Should produce positive values in range [-50, 50]";
}

// Components: RandomI + RandomGenerator
// Tests that RandomI::random(generator, lower, upper) for unsigned integers
// produces values within the specified range.
TEST(RandomIRandomGeneratorIntegrationTest, RandomUnsignedRangeWithGenerator) {
  Ocean::RandomGenerator generator(22222u);

  const unsigned int lower = 10u;
  const unsigned int upper = 20u;

  std::set<unsigned int> observedValues;
  for (int i = 0; i < 1000; ++i) {
    const unsigned int val = Ocean::RandomI::random(generator, lower, upper);
    ASSERT_GE(val, lower) << "Value must be >= lower bound";
    ASSERT_LE(val, upper) << "Value must be <= upper bound";
    observedValues.insert(val);
  }

  // With 1000 iterations over a range of 11 values, we should see most of them
  EXPECT_GE(observedValues.size(), 8u) << "Should observe most values in [10, 20]";
}

// Components: RandomI + RandomGenerator
// Tests that the two-distinct-values function produces two different values.
TEST(RandomIRandomGeneratorIntegrationTest, TwoDistinctValuesWithGenerator) {
  Ocean::RandomGenerator generator(33333u);

  for (int trial = 0; trial < 100; ++trial) {
    unsigned int first = 0u;
    unsigned int second = 0u;
    Ocean::RandomI::random(generator, 10u, first, second);

    ASSERT_NE(first, second) << "Two distinct values must differ (trial " << trial << ")";
    ASSERT_LE(first, 10u) << "First value must be <= maxValue";
    ASSERT_LE(second, 10u) << "Second value must be <= maxValue";
  }
}

// Components: RandomI + RandomGenerator
// Tests the edge case where maxValue=1 for two distinct values (only 0 and 1 possible).
TEST(RandomIRandomGeneratorIntegrationTest, TwoDistinctValuesEdgeCaseMaxOne) {
  Ocean::RandomGenerator generator(44444u);

  for (int trial = 0; trial < 50; ++trial) {
    unsigned int first = 0u;
    unsigned int second = 0u;
    Ocean::RandomI::random(generator, 1u, first, second);

    ASSERT_NE(first, second) << "Two distinct values must differ";
    // With maxValue=1, one must be 0 and the other 1
    EXPECT_TRUE((first == 0u && second == 1u) || (first == 1u && second == 0u))
        << "With maxValue=1, values must be {0, 1}";
  }
}

// Components: RandomI + RandomGenerator
// Tests that the three-distinct-values function produces three different values.
TEST(RandomIRandomGeneratorIntegrationTest, ThreeDistinctValuesWithGenerator) {
  Ocean::RandomGenerator generator(55555u);

  for (int trial = 0; trial < 100; ++trial) {
    unsigned int first = 0u;
    unsigned int second = 0u;
    unsigned int third = 0u;
    Ocean::RandomI::random(generator, 20u, first, second, third);

    ASSERT_NE(first, second) << "first != second (trial " << trial << ")";
    ASSERT_NE(second, third) << "second != third (trial " << trial << ")";
    ASSERT_NE(first, third) << "first != third (trial " << trial << ")";
    ASSERT_LE(first, 20u);
    ASSERT_LE(second, 20u);
    ASSERT_LE(third, 20u);
  }
}

// Components: RandomI + RandomGenerator
// Tests the edge case where maxValue=2 for three distinct values (only 0, 1, 2 possible).
TEST(RandomIRandomGeneratorIntegrationTest, ThreeDistinctValuesEdgeCaseMaxTwo) {
  Ocean::RandomGenerator generator(66666u);

  for (int trial = 0; trial < 50; ++trial) {
    unsigned int first = 0u;
    unsigned int second = 0u;
    unsigned int third = 0u;
    Ocean::RandomI::random(generator, 2u, first, second, third);

    ASSERT_NE(first, second);
    ASSERT_NE(second, third);
    ASSERT_NE(first, third);

    // With maxValue=2, the three values must be a permutation of {0, 1, 2}
    std::set<unsigned int> vals = {first, second, third};
    EXPECT_EQ(vals.size(), 3u);
    EXPECT_EQ(vals.count(0u), 1u);
    EXPECT_EQ(vals.count(1u), 1u);
    EXPECT_EQ(vals.count(2u), 1u);
  }
}

// Components: RandomI + RandomGenerator (deterministic) vs RandomI (global state)
// Tests that initializing RandomI with a specific seed and using global random
// produces deterministic results, verifying the interaction between
// RandomI::initialize and the global random state.
TEST(RandomIRandomGeneratorIntegrationTest, InitializeWithSeedProducesDeterministicSequence) {
  // Initialize global state with a known seed
  Ocean::RandomI::initialize(12345u);
  std::vector<unsigned int> sequence1;
  sequence1.reserve(20);
  for (int i = 0; i < 20; ++i) {
    sequence1.push_back(Ocean::RandomI::random(1000u));
  }

  // Re-initialize with the same seed
  Ocean::RandomI::initialize(12345u);
  std::vector<unsigned int> sequence2;
  sequence2.reserve(20);
  for (int i = 0; i < 20; ++i) {
    sequence2.push_back(Ocean::RandomI::random(1000u));
  }

  ASSERT_EQ(sequence1.size(), sequence2.size());
  for (size_t i = 0; i < sequence1.size(); ++i) {
    EXPECT_EQ(sequence1[i], sequence2[i])
        << "Deterministic sequences should match at index " << i;
  }
}

// Components: RandomI (global) + RandomI range functions
// Tests that after initialization, the signed range function produces
// values across the full specified range including boundary values.
TEST(RandomIRandomGeneratorIntegrationTest, GlobalRandomSignedRangeCoverage) {
  Ocean::RandomI::initialize(77777u);

  const int lower = -5;
  const int upper = 5;
  std::set<int> observed;

  for (int i = 0; i < 2000; ++i) {
    const int val = Ocean::RandomI::random(lower, upper);
    ASSERT_GE(val, lower);
    ASSERT_LE(val, upper);
    observed.insert(val);
  }

  // Should observe all 11 values in [-5, 5] with 2000 iterations
  EXPECT_EQ(observed.size(), 11u)
      << "Should observe all values in [-5, 5] with sufficient iterations";
}

// Components: RandomI (global) + RandomI distinct value functions
// Tests that the global two-distinct-values function works correctly
// after initialization, verifying the interaction between global state
// seeding and the distinct-value generation algorithm.
TEST(RandomIRandomGeneratorIntegrationTest, GlobalTwoDistinctValues) {
  Ocean::RandomI::initialize(88888u);

  for (int trial = 0; trial < 100; ++trial) {
    unsigned int first = 0u;
    unsigned int second = 0u;
    Ocean::RandomI::random(5u, first, second);

    ASSERT_NE(first, second) << "Two distinct values must differ (trial " << trial << ")";
    ASSERT_LE(first, 5u);
    ASSERT_LE(second, 5u);
  }
}

// Components: RandomI (global) + RandomI distinct value functions
// Tests that the global three-distinct-values function works correctly.
TEST(RandomIRandomGeneratorIntegrationTest, GlobalThreeDistinctValues) {
  Ocean::RandomI::initialize(99999u);

  for (int trial = 0; trial < 100; ++trial) {
    unsigned int first = 0u;
    unsigned int second = 0u;
    unsigned int third = 0u;
    Ocean::RandomI::random(10u, first, second, third);

    ASSERT_NE(first, second);
    ASSERT_NE(second, third);
    ASSERT_NE(first, third);
    ASSERT_LE(first, 10u);
    ASSERT_LE(second, 10u);
    ASSERT_LE(third, 10u);
  }
}

} // namespace
