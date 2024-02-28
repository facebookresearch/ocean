// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanharris/OceanHarris.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

#include "ocean/cv/detector/HarrisCorner.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanHarris
{

/**
 * The definition of the feature container for the Ocean Harris Detector
 * @ingroup unifiedfeaturesoceanharris
 */
class OCEAN_UNIFIEDFEATURES_OH_EXPORT OHFeatureContainer final : public FeatureContainer
{
	public:

		/**
		 * Default constructor
		 */
		inline OHFeatureContainer();

		/**
		 * Constructor
		 * @param harrisCorners The Harris corners the Ocean Harris detector has produced
		 */
		inline OHFeatureContainer(CV::Detector::HarrisCorners&& harrisCorners);

		/**
		 * Returns the Harris corners that are stored in this container
		 * @return A reference to the Harris corners
		 */
		inline const CV::Detector::HarrisCorners& harrisCorners() const;

		/**
		 * @sa FeatureContainer::size()
		 */
		size_t size() const override;

		/**
		 * Converts the contents of this container to Thrift data structures
		 * @sa FeatureContainer::toThrift()
		 */
		bool toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const override;

		/**
		 * Converts a Thrift data structure back to the hidden data structures of this container
		 * @sa FeatureContainer::fromThrift()
		 */
		bool fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer) override;

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameOHFeatureContainer();

		/**
		 * Creates an empty instance of this feature container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& /* ignored */);

	protected:

		/**
		 * @sa FeatureContainer::convertFeaturesToKeypoints()
		 */
		Keypoints convertFeaturesToKeypoints() const override;

	public:

		/// The Harris corners the Ocean Harris detector has produced
		CV::Detector::HarrisCorners harrisCorners_;
};

inline OHFeatureContainer::OHFeatureContainer() :
	OHFeatureContainer(CV::Detector::HarrisCorners())
{
	// Nothing else to do.
}

inline OHFeatureContainer::OHFeatureContainer(CV::Detector::HarrisCorners&& harrisCorners) :
	FeatureContainer(nameOceanHarrisLibrary(), nameOHFeatureContainer(), FeatureCategory::KEYPOINTS, /* featureVersion */ 0u),
	harrisCorners_(std::move(harrisCorners))
{
	// Nothing else to do.
}

inline const CV::Detector::HarrisCorners& OHFeatureContainer::harrisCorners() const
{
	return harrisCorners_;
}

inline std::string OHFeatureContainer::nameOHFeatureContainer()
{
	return "OHFeatureContainer";
}

inline std::shared_ptr<UnifiedObject> OHFeatureContainer::create(const std::shared_ptr<Parameters>& /* ignored */)
{
	return std::make_shared<OHFeatureContainer>();
}

} // namespace OceanHarris

} // namespace UnifiedFeatures

} // namespace Ocean
