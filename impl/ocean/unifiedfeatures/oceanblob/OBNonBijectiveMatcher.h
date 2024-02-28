// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanblob/OceanBlob.h"

#include "ocean/base/Worker.h"
#include "ocean/math/Math.h"
#include "ocean/unifiedfeatures/DescriptorMatcher.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

/**
 * Definition of a simple matcher for Ocean Blob features
 * @ingroup unifiedfeaturesoceanblob
 */
class OCEAN_UNIFIEDFEATURES_OB_EXPORT OBNonBijectiveMatcher final : public DescriptorMatcher
{
	public:

		/**
		 * Definition of parameters for the matcher
		 */
		class OBBMParameters : public Parameters
		{
			public:

				/**
				 * Constructor
				 */
				OBBMParameters() = default;
			public:

			    /// Matching threshold of the matcher.
				Ocean::Scalar matchingThreshold_ = 0.1f;
				/// Early rejection in the matcher.
				bool earlyRejection_ = false;
		};

		/**
		 * Default constructor
		 */
		inline OBNonBijectiveMatcher();

		/**
		 * Constructor
		 * @param parameters The parameters of this matcher
		 */
		inline OBNonBijectiveMatcher(OBBMParameters&& parameters);

		/**
		 * Matches two descriptor containers
		 * @sa DescriptorMatcher::matchDescriptors()
		 */
		Result matchDescriptors(const DescriptorContainer& descriptorContainer0, const DescriptorContainer& descriptorContainer1, IndexPairs32& correspondences, Worker* worker = nullptr) const override;

		/**
		 * Returns the name of this matcher
		 * @return The name of this matcher
		 */
		static inline std::string nameOBNonBijectiveMatcher();

		/**
		 * Creates an instance of this descriptor matcher
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:
		/// The parameters of this matcher
		OBBMParameters parameters_;
};

inline OBNonBijectiveMatcher::OBNonBijectiveMatcher() :
	OBNonBijectiveMatcher(OBBMParameters())
{
	// Nothing else to do.
}

inline OBNonBijectiveMatcher::OBNonBijectiveMatcher(OBBMParameters&& parameters) :
	DescriptorMatcher(nameOceanBlobLibrary(), nameOBNonBijectiveMatcher(), /* descriptorMatcherVersion */ 0u),
	parameters_(std::move(parameters))
{
	// Nothing else to do.
}

inline std::string OBNonBijectiveMatcher::nameOBNonBijectiveMatcher()
{
	return "OBNonBijectiveMatcher";
}

inline std::shared_ptr<UnifiedObject> OBNonBijectiveMatcher::create(const std::shared_ptr<Parameters>& parameters)
{
	const std::shared_ptr<OBBMParameters> obParametersPtr = std::static_pointer_cast<OBBMParameters>(parameters);
	OBBMParameters ooParameters = obParametersPtr != nullptr ? *obParametersPtr : OBBMParameters();

	return std::make_shared<OBNonBijectiveMatcher>(std::move(ooParameters));
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
