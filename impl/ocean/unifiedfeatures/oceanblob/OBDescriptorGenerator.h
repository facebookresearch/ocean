// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanblob/OBParameters.h"
#include "ocean/unifiedfeatures/oceanblob/OceanBlob.h"

#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

/**
 * Definition of a descriptor generator for Ocean ORB descriptors
 * @ingroup unifiedfeaturesoceanblob
 */
class OCEAN_UNIFIEDFEATURES_OB_EXPORT OBDescriptorGenerator final : public DescriptorGenerator
{
	public:

		/**
		 * Default constructor
		 */
		inline OBDescriptorGenerator();

		/**
		 * Constructor
		 * @param parameters The parameters to be used
		 */
		inline OBDescriptorGenerator(const OBParameters& parameters);

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
		static inline std::string nameOBDescriptorGenerator();

		/**
		 * Creates an instance of this descriptor generator
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// Parameters for detector.
		OBParameters parameters_;
};

inline OBDescriptorGenerator::OBDescriptorGenerator() :
	OBDescriptorGenerator(OBParameters())
{
	// Nothing else to do.
}

inline OBDescriptorGenerator::OBDescriptorGenerator(const OBParameters& parameters) :
	DescriptorGenerator(nameOceanBlobLibrary(), nameOBDescriptorGenerator(), /* border, cf. https://fburl.com/diffusion/a7q5qytj */ 0u, DescriptorCategory::FLOAT_DESCRIPTOR, {FrameType::FORMAT_Y8}),
	parameters_(parameters)
{
	// Nothing else to do.
}

inline std::string OBDescriptorGenerator::nameOBDescriptorGenerator()
{
	return "OBDescriptorGenerator";
}

inline std::shared_ptr<UnifiedObject> OBDescriptorGenerator::create(const std::shared_ptr<Parameters>& /* parameters */)
{
	return std::make_shared<OBDescriptorGenerator>();
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
