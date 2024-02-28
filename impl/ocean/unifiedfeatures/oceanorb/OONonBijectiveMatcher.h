// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanorb/OceanOrb.h"

#include "ocean/unifiedfeatures/DescriptorMatcher.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

/**
 * Definition of a simple matcher for Ocean ORB features
 * @ingroup unifiedfeaturesoceanorb
 */
class OCEAN_UNIFIEDFEATURES_OO_EXPORT OONonBijectiveMatcher final : public DescriptorMatcher
{
	public:

		/**
		 * Definition of parameters for the matcher
		 */
		class OOParameters : public Parameters
		{
			public:

				/**
				 * Constructor
				 */
				OOParameters() = default;

			public:

				/// The percentage (in relation to the number of descriptor bits) of the maximal hamming distance so that two descriptors count as similar, with range [0, 1]
				float matchingThreshold_ = 0.15f;
		};

	public:

		/**
		 * Default constructor
		 */
		inline OONonBijectiveMatcher();

		/**
		 * Constructor
		 * @param parameters The parameters of this matcher
		 */
		inline OONonBijectiveMatcher(OOParameters&& parameters);

		/**
		 * Matches two descriptor containers
		 * @sa DescriptorMatcher::matchDescriptors()
		 */
		Result matchDescriptors(const DescriptorContainer& descriptorContainer0, const DescriptorContainer& descriptorContainer1, IndexPairs32& correspondences, Worker* worker = nullptr) const override;

		/**
		 * Returns the name of this matcher
		 * @return The name of this matcher
		 */
		static inline std::string nameOONonBijectiveMatcher();

		/**
		 * Creates an instance of this descriptor matcher
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters of this matcher
		OOParameters parameters_;
};

inline OONonBijectiveMatcher::OONonBijectiveMatcher() :
	OONonBijectiveMatcher(OOParameters())
{
	// Nothing else to do.
}

inline OONonBijectiveMatcher::OONonBijectiveMatcher(OOParameters&& parameters) :
	DescriptorMatcher(nameOceanOrbLibrary(), nameOONonBijectiveMatcher(), /* descriptorMatcherVersion */ 0u),
	parameters_(std::move(parameters))
{
	// Nothing else to do.
}

inline std::string OONonBijectiveMatcher::nameOONonBijectiveMatcher()
{
	return "OONonBijectiveMatcher";
}

inline std::shared_ptr<UnifiedObject> OONonBijectiveMatcher::create(const std::shared_ptr<Parameters>& parameters)
{
	const std::shared_ptr<OOParameters> ooParametersPtr = std::static_pointer_cast<OOParameters>(parameters);
	OOParameters ooParameters = ooParametersPtr != nullptr ? *ooParametersPtr : OOParameters();

	return std::make_shared<OONonBijectiveMatcher>(std::move(ooParameters));
}

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
