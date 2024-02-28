// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/base/Base.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Numeric.h"
#include "ocean/unifiedfeatures/DescriptorMatcher.h"

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftParameters.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * Definition of a descriptor matcher for VLFeat SIFT descriptors
 * @ingroup unifiedfeaturesvlfeatsift
 */
class OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT VlfeatSiftBruteForceDescriptorMatcher final : public DescriptorMatcher
{
	protected:

		/**
		 * Definition of a history of best matches
		 */
		class BestMatchHistory
		{
			public:

				/**
				 * Default constructor
				 */
				BestMatchHistory() = default;

				/**
				 * Adds a new match to the history
				 * @param index The index of the descriptor that has been matched, must be `< Index32(-1)`
				 * @param distance The distance of the matched descriptor, range: [0, infinity)
				 * @return True if the match was added to the history of best matches, otherwise false
				 */
				inline bool addMatch(const Index32 index, const float distance);

				/**
				 * Returns the index (and distance) of the i-th best match that has been recorded
				 * @param bestIndex The index of the i-th best match, range: [0, 2)
				 * @param distance The optionally returning distance of the i-th best match, will be ignored if set to `nullptr`
				 */
				inline Index32 bestMatch(unsigned int bestIndex, float* distance = nullptr) const;

				/**
				 * Returns if the ratio between the first and the second best match is signifcant
				 * @param ratio The ratio between the best and the second best match so that it is considered significant such that `distances_[0] < distances_[1] * ratio`, ratio: (0, infinity)
				 * @return True if the distance ratio is not exceeded, otherwise false
				 */
				inline bool isSignificantMatch(const float ratio) const;

				/**
				 * Returns the distance ratio between the best and the second best match
				 */
				inline float distanceRatio() const;

			protected:

				/// Indices of the best matches
				Index32 indices_[2] = { Index32(-1), Index32(-1) };

				/// Distances of the best matches
				float distances_[2] = { NumericF::maxValue(), NumericF::maxValue() };
		};

	public:

		/**
		 * Default constructor
		 */
		inline VlfeatSiftBruteForceDescriptorMatcher();

		/**
		 * Constructor
		 * @param parameters The parameters that should be used for the detection
		 */
		inline explicit VlfeatSiftBruteForceDescriptorMatcher(const VlfeatSiftParameters& parameters);

		/**
		 * Matches two descriptor containers
		 * @sa DescriptorMatcher::matchDescriptors()
		 */
		Result matchDescriptors(const DescriptorContainer& descriptorContainer0, const DescriptorContainer& descriptorContainer1, IndexPairs32& correspondences, Worker* worker = nullptr) const override;

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		inline static std::string nameVlfeatSiftBruteForceDescriptorMatcher();

		/**
		 * Creates an instance of this descriptor matcher
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters that should be used for descriptor generation
		VlfeatSiftParameters parameters_;
};

inline bool VlfeatSiftBruteForceDescriptorMatcher::BestMatchHistory::addMatch(const Index32 index, const float distance)
{
	ocean_assert(distance >= 0.0f);

	if (index == indices_[0] || index == indices_[1])
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (distance >= distances_[1])
	{
		return false;
	}

	if (distance < distances_[0])
	{
		indices_[1] = indices_[0];
		distances_[1] = distances_[0];

		indices_[0] = index;
		distances_[0] = distance;
	}
	else if (distance < distances_[1])
	{
		indices_[1] = index;
		distances_[1] = distance;
	}

	ocean_assert(distances_[0] <= distances_[1]);

	return true;
}

inline Index32 VlfeatSiftBruteForceDescriptorMatcher::BestMatchHistory::bestMatch(unsigned int bestIndex, float* distance) const
{
	ocean_assert(bestIndex < 2u);

	if (distance != nullptr)
	{
		*distance = distances_[bestIndex];
	}

	return indices_[bestIndex];
}

bool VlfeatSiftBruteForceDescriptorMatcher::BestMatchHistory::isSignificantMatch(const float ratio) const
{
	ocean_assert(ratio > 0.0f);
	return distances_[0] < (distances_[1] * ratio);
}

inline float VlfeatSiftBruteForceDescriptorMatcher::BestMatchHistory::distanceRatio() const
{
	if (indices_[0] == Index32(-1))
	{
		ocean_assert(indices_[1] == Index32(-1));
		return 0.0f;
	}
	else if (indices_[0] != Index32(-1) && indices_[1] == Index32(-1))
	{
		// Only a single match
		return NumericF::maxValue();
	}

	ocean_assert(indices_[0] != Index32(-1) && indices_[1] != Index32(-1));
	ocean_assert(distances_[0] >= 0.0f && distances_[1] >= 0.0f);

	return distances_[1] > 0.0f ? distances_[0] / distances_[1] : NumericF::maxValue();
}

inline VlfeatSiftBruteForceDescriptorMatcher::VlfeatSiftBruteForceDescriptorMatcher() :
	VlfeatSiftBruteForceDescriptorMatcher(VlfeatSiftParameters())
{
	// Nothing else to do.
}

inline VlfeatSiftBruteForceDescriptorMatcher::VlfeatSiftBruteForceDescriptorMatcher(const VlfeatSiftParameters& parameters) :
	DescriptorMatcher(nameVlfeatSiftLibrary(), nameVlfeatSiftBruteForceDescriptorMatcher(), /* border */ 0u),
	parameters_(parameters)
{
	// Nothing else to do.
}

inline std::string VlfeatSiftBruteForceDescriptorMatcher::nameVlfeatSiftBruteForceDescriptorMatcher()
{
	return "VlfeatSiftBruteForceDescriptorMatcher";
}

inline std::shared_ptr<UnifiedObject> VlfeatSiftBruteForceDescriptorMatcher::create(const std::shared_ptr<Parameters>& parameters)
{
	VlfeatSiftParameters* siftParameters = dynamic_cast<VlfeatSiftParameters*>(parameters.get());

	if (siftParameters)
	{
		return std::make_shared<VlfeatSiftBruteForceDescriptorMatcher>(*siftParameters);
	}

	return std::make_shared<VlfeatSiftBruteForceDescriptorMatcher>();
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
