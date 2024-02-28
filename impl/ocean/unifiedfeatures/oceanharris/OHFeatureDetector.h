// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanharris/OceanHarris.h"
#include "ocean/unifiedfeatures/oceanharris/OHFeatureContainer.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/detector/HarrisCorner.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanHarris
{

/**
 * Definition of a wrapper for the Ocean Harris detector
 * @ingroup unifiedfeaturesoceanharris
 */
class OCEAN_UNIFIEDFEATURES_OH_EXPORT OHFeatureDetector final : public FeatureDetector
{
	public:

		/**
		 * Definition of parameters for the detector
		 */
		class OHParameters : public UnifiedFeatures::Parameters
		{
			public:

				/**
				 * Default constructor
				 */
				OHParameters() = default;

			public:

				/// The threshold of the Harris corner detector
				unsigned int threshold = 20u;

				/// Indicates whether sub-pixel interpolation should be used
				bool determineExactPosition = true;
		};

	public:

		/**
		 * Default constructor
		 */
		inline OHFeatureDetector();

		/**
		 * Constructor
		 * @param parameters The parameters that should be used for the detection
		 */
		inline explicit OHFeatureDetector(OHParameters&& parameters);

		/**
		 * @sa FeatureContainer::detectFeatures()
		 */
		Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker = nullptr) const override;

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameOHFeatureDetector();

		/**
		 * Creates an instance of this feature detector
		 * @param parameters Optional parameters to create this detector
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters that should be used for the detection
		OHParameters parameters_;
};

inline OHFeatureDetector::OHFeatureDetector() :
	OHFeatureDetector(OHParameters())
{
	// Nothing else to do.
}

inline OHFeatureDetector::OHFeatureDetector(OHParameters&& parameters) :
	FeatureDetector(nameOceanHarrisLibrary(), nameOHFeatureDetector(), FeatureCategory::KEYPOINTS, { FrameType::FORMAT_Y8 }),
	parameters_(std::move(parameters))
{
	// Nothing else to do.
}

inline std::string OHFeatureDetector::nameOHFeatureDetector()
{
	return "OHFeatureDetector";
}

inline std::shared_ptr<UnifiedObject> OHFeatureDetector::create(const std::shared_ptr<Parameters>& parameters)
{
	// This should allow to make use of the parameters
	ocean_assert_and_suppress_unused(parameters == nullptr, parameters);

	return std::make_shared<OHFeatureDetector>();
}

} // namespace OceanHarris

} // namespace UnifiedFeatures

} // namespace Ocean
