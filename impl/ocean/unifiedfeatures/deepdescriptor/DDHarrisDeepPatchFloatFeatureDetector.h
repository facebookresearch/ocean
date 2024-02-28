// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/deepdescriptor/DeepDescriptor.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/io/File.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

/**
 * Definition of the Deep Patch Float detector and descriptor
 * @ingroup unifiedfeaturesdeepdescriptor
 */
class OCEAN_UNIFIEDFEATURES_DD_EXPORT DDHarrisDeepPatchFloatFeatureDetector final : public FeatureDetector
{
	public:

		/**
		 * Definition of parameters for the detector
		 */
		class DDParameters : public Parameters
		{
			public:

				/**
				 * Constructor
				 * @param deepPatchFloatModelFilename The file location from where model for the Deep Patch Float descriptor can loaded, must be valid
				 */
				inline DDParameters(const std::string& deepPatchFloatModelFilename);

			public:

				/// The file location from where model for the Deep Patch Float descriptor can loaded, must be valid
				std::string deepPatchFloatModelFilename_;
		};

	protected:

		/**
		 * Definition of a descriptor generator to make the deep descriptors accessible
		 * This descriptor generator is only available in combination with the enclosing feature detector. It is, by itself not useful.
		 */
		class IntegratedHelperDescriptorGenerator final : public DescriptorGenerator
		{
			friend DDHarrisDeepPatchFloatFeatureDetector;

			public:

				/**
				 * Default constructor
				 */
				inline IntegratedHelperDescriptorGenerator();

				/**
				 * @sa DescriptorGenerator::generateDescriptors()
				 */
				Result generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptors, Worker* worker = nullptr) const override;

			protected:

				/**
				 * Stores descriptors in this instance
				 * This function will be called from the feature detector and will store
				 * @param deepPatchFloatDescriptors The descriptors that will be stored in this instance
				 */
				inline void setDescriptors(DeepPatchFloatDescriptors&& deepPatchFloatDescriptors);

			protected:

				// The deep-patch-float descriptors
				mutable DeepPatchFloatDescriptors deepPatchFloatDescriptors_;
		};

	public:

		/**
		 * Creates an invalid detector
		 */
		DDHarrisDeepPatchFloatFeatureDetector() = delete;

		/**
		 * Constructor
		 * @param parameters The parameters for this detector
		 */
		inline explicit DDHarrisDeepPatchFloatFeatureDetector(DDParameters&& parameters);

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
		static inline std::string nameDDHarrisDeepPatchFloatFeatureDetector();

		/**
		 * Creates an instance of this descriptor generator
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters of this detector
		DDParameters parameters_;

		/// The instance of the descriptor generator that will manage the actual descriptors
		mutable std::shared_ptr<IntegratedHelperDescriptorGenerator> descriptorGenerator_;

		/// The deep patch float pipeline
		std::shared_ptr<perception::features::DeepDescriptorPipeline> deepDescriptorFloatPipeline_;
};

inline DDHarrisDeepPatchFloatFeatureDetector::DDParameters::DDParameters(const std::string& deepPatchFloatModelFilename) :
	deepPatchFloatModelFilename_(deepPatchFloatModelFilename)
{
	// Nothing else to do
}

inline DDHarrisDeepPatchFloatFeatureDetector::IntegratedHelperDescriptorGenerator::IntegratedHelperDescriptorGenerator() :
	DescriptorGenerator(nameDeepDescriptorLibrary(), nameDDHarrisDeepPatchFloatFeatureDetector() + "IntegratedHelperDescriptorGenerator", /* border, cf. https://fburl.com/diffusion/b6h2fugt */ 20u, DescriptorCategory::BINARY_DESCRIPTOR, { FrameType::FORMAT_Y8 })
{
	// Nothing else to do
}

inline void DDHarrisDeepPatchFloatFeatureDetector::IntegratedHelperDescriptorGenerator::setDescriptors(DeepPatchFloatDescriptors&& deepPatchFloatDescriptors)
{
	deepPatchFloatDescriptors_ = std::move(deepPatchFloatDescriptors);
}

inline DDHarrisDeepPatchFloatFeatureDetector::DDHarrisDeepPatchFloatFeatureDetector(DDParameters&& parameters) :
	FeatureDetector(nameDeepDescriptorLibrary(), nameDDHarrisDeepPatchFloatFeatureDetector(), FeatureCategory::KEYPOINTS, { FrameType::FORMAT_Y8 }, /* detectorVersion */ 0u),
	parameters_(parameters),
	descriptorGenerator_(std::make_shared<IntegratedHelperDescriptorGenerator>())
{
	deepDescriptorFloatPipeline_ = std::make_shared<perception::features::DeepDescriptorPipeline>();
}

inline std::shared_ptr<DescriptorGenerator> DDHarrisDeepPatchFloatFeatureDetector::getDescriptorGenerator()
{
	ocean_assert(descriptorGenerator_ != nullptr);
	return descriptorGenerator_;
}

inline std::string DDHarrisDeepPatchFloatFeatureDetector::nameDDHarrisDeepPatchFloatFeatureDetector()
{
	return "DDHarrisDeepPatchFloatFeatureDetector";
}

inline std::shared_ptr<UnifiedObject> DDHarrisDeepPatchFloatFeatureDetector::create(const std::shared_ptr<Parameters>& parameters)
{
	if (parameters == nullptr)
	{
		ocean_assert(false && "This feature detector always requires parameters for the initialization");
		return nullptr;
	}

	const std::shared_ptr<DDParameters> ddParametersPtr = std::dynamic_pointer_cast<DDParameters>(parameters);
	if (ddParametersPtr == nullptr)
	{
		ocean_assert(false && "Incompatible parameters!");
		return nullptr;
	}

	ocean_assert(IO::File(ddParametersPtr->deepPatchFloatModelFilename_).exists());

	DDParameters ddParameters = *ddParametersPtr;
	return std::make_shared<DDHarrisDeepPatchFloatFeatureDetector>(std::move(ddParameters));
}

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
