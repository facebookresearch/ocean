// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanblob/OBFeatureContainer.h"
#include "ocean/unifiedfeatures/oceanblob/OBParameters.h"
#include "ocean/unifiedfeatures/oceanblob/OceanBlob.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

/**
 * Definition of a wrapper for the Ocean Blob detector
 * @ingroup unifiedfeaturesoceanblob
 */
class OCEAN_UNIFIEDFEATURES_OB_EXPORT OBFeatureDetector final : public FeatureDetector
{
	public:
		/**
	     * Default constructor
	     */
	    inline OBFeatureDetector();

	    /**
	     * Constructor
	     * @param parameters The parameters that should be used for the detection
	     */
		inline OBFeatureDetector(const OBParameters& parameters);

		/**
	     * @sa FeatureContainer::detectFeatures()
	     */
	    Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker = nullptr) const override;

	    /**
	     * Returns the name of this object
	     * @return The name
	     */
	    static inline std::string nameOBFeatureDetector();

	    /**
	     * Creates an instance of this feature detector
	     * @param parameters Optional parameters to create this detector
	     * @return The pointer to new instance
	     */
	    static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:
		/// The parameters that should be used for the detection
	    OBParameters parameters_;
};

inline OBFeatureDetector::OBFeatureDetector() :
	OBFeatureDetector(OBParameters())
{
	// Nothing else to do.
}

inline OBFeatureDetector::OBFeatureDetector(const OBParameters& parameters) :
	FeatureDetector(nameOceanBlobLibrary(), nameOBFeatureDetector(), FeatureCategory::KEYPOINTS, {FrameType::FORMAT_Y8}),
	parameters_(parameters)
{
	// Nothing else to do.
}

inline std::string OBFeatureDetector::nameOBFeatureDetector()
{
	return "OBFeatureDetector";
}

inline std::shared_ptr<UnifiedObject> OBFeatureDetector::create(const std::shared_ptr<Parameters>& parameters)
{
	// This should allow to make use of the parameters
	ocean_assert_and_suppress_unused(parameters == nullptr, parameters);

	return std::make_shared<OBFeatureDetector>();
}

} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
