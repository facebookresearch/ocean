// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSift.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftParameters.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * Definition of a descriptor generator for VLFeat SIFT descriptors
 * @ingroup unifiedfeaturesvlfeatsift
 */
class OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT VlfeatSiftDescriptorGenerator final : public DescriptorGenerator
{
	public:

		/**
		 * Default constructor
		 */
		inline VlfeatSiftDescriptorGenerator();

		/**
		 * Constructor
		 * @param parameters The parameters that should be used for the detection
		 */
		inline explicit VlfeatSiftDescriptorGenerator(const VlfeatSiftParameters& parameters);

		/**
		 * @sa DescriptorGenerator::isSupportedFeatureDetector()
		 */
		bool isSupportedFeatureDetector(const FeatureDetector& featureDetector) const override;

		/**
		 * @sa DescriptorGenerator::generateDescriptors()
		 */
		Result generateDescriptors(const Frame& frame, const FeatureContainer& keypoints, std::shared_ptr<DescriptorContainer>& descriptors, Worker* worker = nullptr) const override;

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameVlfeatSiftDescriptorGenerator();

		/**
		 * Creates an instance of this descriptor generator
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters that should be used for descriptor generation
		VlfeatSiftParameters parameters_;
};

inline VlfeatSiftDescriptorGenerator::VlfeatSiftDescriptorGenerator() :
	VlfeatSiftDescriptorGenerator(VlfeatSiftParameters())
{
	// Nothing else to do.
}

inline VlfeatSiftDescriptorGenerator::VlfeatSiftDescriptorGenerator(const VlfeatSiftParameters& parameters) :
	DescriptorGenerator(nameVlfeatSiftLibrary(), nameVlfeatSiftDescriptorGenerator(), /* border */ 0u, DescriptorCategory::FLOAT_DESCRIPTOR, { FrameType::FORMAT_Y8, FrameType::FORMAT_F32 }),
	parameters_(parameters)
{
	// Nothing else to do.
}

inline std::string VlfeatSiftDescriptorGenerator::nameVlfeatSiftDescriptorGenerator()
{
	return "VlfeatSiftDescriptorGenerator";
}

inline std::shared_ptr<UnifiedObject> VlfeatSiftDescriptorGenerator::create(const std::shared_ptr<Parameters>& parameters)
{
	VlfeatSiftParameters* siftParameters = dynamic_cast<VlfeatSiftParameters*>(parameters.get());

	if (siftParameters)
	{
		return std::make_shared<VlfeatSiftDescriptorGenerator>(*siftParameters);
	}

	return std::make_shared<VlfeatSiftDescriptorGenerator>();
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
