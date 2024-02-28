// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/deeppatchuint8/DeepPatchUint8.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/io/File.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

/**
 * Definition of the DeepPatchUint8 detector and descriptor
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
class OCEAN_UNIFIEDFEATURES_DP8_EXPORT DP8FeatureDetector final : public FeatureDetector
{
	public:

		/**
		 * Definition of parameters for the detector
		 */
		class DP8Parameters : public Parameters
		{
			public:

				/**
				 * Constructor
				 * @param modelFilename The file location from where model for the DeepPatchUint8 descriptor can loaded, must be valid
				 */
				inline DP8Parameters(const std::string& modelFilename);

			public:

				/// The file location from where model for the DeepPatchUint8 descriptor can loaded, must be valid
				std::string modelFilename_;
		};

	protected:

		/**
		 * Definition of a descriptor generator to make the deep descriptors accessible
		 * This descriptor generator is only available in combination with the enclosing feature detector. It is, by itself not useful.
		 */
		class DP8DescriptorGenerator final : public DescriptorGenerator
		{
			friend DP8FeatureDetector;

			public:

				/**
				 * Default constructor
				 */
				inline DP8DescriptorGenerator();

				/**
				 * @sa DescriptorGenerator::generateDescriptors()
				 */
				Result generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptors, Worker* worker = nullptr) const override;

			protected:

				/**
				 * Stores descriptors in this instance
				 * This function will be called from the feature detector and will store
				 * @param descriptors The descriptors that will be stored in this instance
				 */
				inline void setDescriptors(DP8Descriptors96&& descriptors);

			protected:

				// The DeepPatchUint8 descriptors
				mutable DP8Descriptors96 descriptors_;
		};

	public:

		/**
		 * Creates an invalid detector
		 */
		DP8FeatureDetector() = delete;

		/**
		 * Constructor
		 * @param parameters The parameters for this detector
		 */
		inline explicit DP8FeatureDetector(DP8Parameters&& parameters);

		/**
		 * @sa FeatureDetector::detectFeatures()
		 */
		FeatureDetector::Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker = nullptr) const override;

		/**
		 * Returns the descriptor generator that is part of this detector
		 * @return The pointer to the instance of the descriptor generator
		 */
		inline std::shared_ptr<DescriptorGenerator> getDescriptorGenerator();

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameDP8FeatureDetector();

		/**
		 * Creates an instance of this descriptor generator
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters of this detector
		DP8Parameters parameters_;

		/// The instance of the descriptor generator that will manage the actual descriptors
		mutable std::shared_ptr<DP8DescriptorGenerator> descriptorGenerator_;

		/// The perception pipeline for the deep descriptors
		std::shared_ptr<perception::features::DeepDescriptorPipeline> deepDescriptorPipeline_;
};

inline DP8FeatureDetector::DP8Parameters::DP8Parameters(const std::string& modelFilename) :
	modelFilename_(modelFilename)
{
	// Nothing else to do
}

inline DP8FeatureDetector::DP8DescriptorGenerator::DP8DescriptorGenerator() :
	DescriptorGenerator(nameDeepPatchUint8Library(), nameDP8FeatureDetector() + "DP8DescriptorGenerator", /* border, cf. https://fburl.com/diffusion/b6h2fugt */ 20u, DescriptorCategory::BINARY_DESCRIPTOR, { FrameType::FORMAT_Y8 })
{
	// Nothing else to do
}

inline void DP8FeatureDetector::DP8DescriptorGenerator::setDescriptors(DP8Descriptors96&& descriptors)
{
	descriptors_ = std::move(descriptors);
}

inline DP8FeatureDetector::DP8FeatureDetector(DP8Parameters&& parameters) :
	FeatureDetector(nameDeepPatchUint8Library(), nameDP8FeatureDetector(), FeatureCategory::KEYPOINTS, { FrameType::FORMAT_Y8 }, /* detectorVersion */ 0u),
	parameters_(parameters),
	descriptorGenerator_(std::make_shared<DP8DescriptorGenerator>())
{
	deepDescriptorPipeline_ = std::make_shared<perception::features::DeepDescriptorPipeline>();
}

inline std::shared_ptr<DescriptorGenerator> DP8FeatureDetector::getDescriptorGenerator()
{
	ocean_assert(descriptorGenerator_ != nullptr);
	return descriptorGenerator_;
}

inline std::string DP8FeatureDetector::nameDP8FeatureDetector()
{
	return "DP8FeatureDetector";
}

inline std::shared_ptr<UnifiedObject> DP8FeatureDetector::create(const std::shared_ptr<Parameters>& parameters)
{
	if (parameters == nullptr)
	{
		ocean_assert(false && "This feature detector always requires parameters for the initialization");
		return nullptr;
	}

	const std::shared_ptr<DP8Parameters> ddParametersPtr = std::dynamic_pointer_cast<DP8Parameters>(parameters);
	if (ddParametersPtr == nullptr)
	{
		ocean_assert(false && "Incompatible parameters!");
		return nullptr;
	}

	ocean_assert(IO::File(ddParametersPtr->modelFilename_).exists());

	DP8Parameters ddParameters = *ddParametersPtr;
	return std::make_shared<DP8FeatureDetector>(std::move(ddParameters));
}

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
