// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanorb/OceanOrb.h"

#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/ORBFeature.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanOrb
{

/**
 * Definition of a descriptor generator for Ocean ORB descriptors
 * @ingroup unifiedfeaturesoceanorb
 */
class OCEAN_UNIFIEDFEATURES_OO_EXPORT OODescriptorGenerator final : public DescriptorGenerator
{
	public:

		/**
		 * Default constructor
		 */
		inline OODescriptorGenerator();

		/**
		 * Constructor
		 * @param useSubLayers True, if two additional sub layers for descriptors are use, create three descriptors per feature point; otherwise, only one layer is used
		 */
		inline OODescriptorGenerator(const bool useSubLayers);

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
		static inline std::string nameOODescriptorGenerator();

		/**
		 * Creates an instance of this descriptor generator
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// Indicates if two additional sub layers for descriptors are use, create three descriptors per feature point; otherwise, only one layer is used
		bool useSubLayers_;
};

inline OODescriptorGenerator::OODescriptorGenerator() :
	OODescriptorGenerator(/* useSubLayers */ false)
{
	// Nothing else to do.
}

inline OODescriptorGenerator::OODescriptorGenerator(const bool useSubLayers) :
	DescriptorGenerator(nameOceanOrbLibrary(), nameOODescriptorGenerator(), /* border, cf. https://fburl.com/diffusion/a7q5qytj */ 31u, DescriptorCategory::BINARY_DESCRIPTOR, { FrameType::FORMAT_Y8 }),
	useSubLayers_(useSubLayers)
{
	// Nothing else to do.
}

inline std::string OODescriptorGenerator::nameOODescriptorGenerator()
{
	return "OODescriptorGenerator";
}

inline std::shared_ptr<UnifiedObject> OODescriptorGenerator::create(const std::shared_ptr<Parameters>& /* parameters */)
{
	return std::make_shared<OODescriptorGenerator>();
}

} // namespace OceanOrb

} // namespace UnifiedFeatures

} // namespace Ocean
