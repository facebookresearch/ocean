// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/boltharris/BoltHarris.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace BoltHarris
{

/**
 * Definition of a wrapper for the reference implementation of Bolt Harris detector
 * @ingroup unifiedfeaturesboltharris
 */
class OCEAN_UNIFIEDFEATURES_BH_EXPORT BHFeatureDetector final : public FeatureDetector
{
	public:

		/**
		 * Definition of parameters for the detector
		 */
		class BHParameters : public UnifiedFeatures::Parameters
		{
			public:

				/**
				 * Default constructor
				 */
				BHParameters() = default;

			public:

				/// The threshold that the underlying detector will use, range: [0, infinity)
				unsigned int threshold = 1000000u;

				/// The maximum number of corners to detect, range: [0, infinity)
				size_t maxCornersToExtract = 500;

				/// Indicates whether edges or corners should be extracted
				bool extractEdges = true;
		};

	public:

		/**
		 * Default constructor
		 */
		inline BHFeatureDetector();

		/**
		 * Constructor
		 * @param parameters The parameters that should be used for the detection
		 */
		inline explicit BHFeatureDetector(BHParameters&& parameters);

		/**
		 * @sa FeatureDetector::detectFeatures()
		 */
		Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker = nullptr) const override;

		/**
		 * Returns the name of this object
		 * @return The name of this object
		 */
		static inline std::string nameBHFeatureDetector();

		/**
		 * Creates an instance of this feature container
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters that should be used for the detection
		BHParameters parameters_;
};

inline BHFeatureDetector::BHFeatureDetector() :
	BHFeatureDetector(BHParameters())
{
	// Nothing else to do.
}

inline BHFeatureDetector::BHFeatureDetector(BHParameters&& parameters) :
	FeatureDetector(nameBoltHarrisLibrary(), nameBHFeatureDetector(), FeatureCategory::KEYPOINTS, { FrameType::FORMAT_Y8 }),
	parameters_(parameters)
{
	// Nothing else to do.
}

inline std::string BHFeatureDetector::nameBHFeatureDetector()
{
	return "BHFeatureDetector";
}

inline std::shared_ptr<UnifiedObject> BHFeatureDetector::create(const std::shared_ptr<Parameters>& parameters)
{
	if (parameters == nullptr)
	{
		ocean_assert(false && "The initialization requires parameters!");
		return nullptr;
	}

	const std::shared_ptr<BHParameters> bhParametersPtr = std::static_pointer_cast<BHParameters>(parameters);
	ocean_assert(parameters == nullptr || bhParametersPtr != nullptr);

	BHParameters bhParameters = bhParametersPtr != nullptr ? *bhParametersPtr : BHParameters();

	return std::make_shared<BHFeatureDetector>(std::move(bhParameters));
}

} // namespace BoltHarris

} // namespace UnifiedFeatures

} // namespace Ocean
