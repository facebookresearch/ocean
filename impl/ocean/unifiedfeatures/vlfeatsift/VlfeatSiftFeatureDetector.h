// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/base/Frame.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSift.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftParameters.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftFeatureContainer.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * Definition of a wrapper for the VLFeat SIFT detector
 * @ingroup unifiedfeaturesvlfeatsift
 */
class OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT VlfeatSiftFeatureDetector final : public FeatureDetector
{
	public:

		/**
		 * Default constructor
		 */
		inline VlfeatSiftFeatureDetector();

		/**
		 * Constructor
		 * @param parameters The parameters that should be used for the detection
		 */
		inline VlfeatSiftFeatureDetector(const VlfeatSiftParameters& parameters);

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameVlfeatSiftFeatureDetector();

		/**
		 * @sa FeatureContainer::detectFeatures()
		 */
		Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker = nullptr) const override;

		/**
		 * Creates an instance of this feature detector
		 * @param parameters Optional parameters to create this detector
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters that should be used for the detection
		VlfeatSiftParameters parameters_;
};

inline VlfeatSiftFeatureDetector::VlfeatSiftFeatureDetector() :
	VlfeatSiftFeatureDetector(VlfeatSiftParameters())
{
	// Nothing else to do.
}

inline VlfeatSiftFeatureDetector::VlfeatSiftFeatureDetector(const VlfeatSiftParameters& parameters) :
	FeatureDetector(nameVlfeatSiftLibrary(), nameVlfeatSiftFeatureDetector(), FeatureCategory::KEYPOINTS, { FrameType::FORMAT_Y8, FrameType::FORMAT_F32 }, /* detectorVersion */ 0u),
	parameters_(parameters)
{
	// Nothing else to do.
}

inline std::string VlfeatSiftFeatureDetector::nameVlfeatSiftFeatureDetector()
{
	return "VlfeatSiftFeatureDetector";
}

inline std::shared_ptr<UnifiedObject> VlfeatSiftFeatureDetector::create(const std::shared_ptr<Parameters>& parameters)
{
	VlfeatSiftParameters* siftParameters = dynamic_cast<VlfeatSiftParameters*>(parameters.get());

	if (siftParameters)
	{
		return std::make_shared<VlfeatSiftFeatureDetector>(*siftParameters);
	}

	return std::make_shared<VlfeatSiftFeatureDetector>();
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
