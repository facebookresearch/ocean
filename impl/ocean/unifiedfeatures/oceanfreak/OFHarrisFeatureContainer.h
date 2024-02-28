// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanfreak/OceanFreak.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

#include "ocean/cv/detector/HarrisCorner.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

/**
 * The definition of the feature container for the Ocean Harris Detector
 * @ingroup unifiedfeaturesoceanharris
 */
class OCEAN_UNIFIEDFEATURES_OF_EXPORT OFHarrisFeatureContainer final : public FeatureContainer
{
	public:

		/**
		 * Default constructor
		 */
		inline OFHarrisFeatureContainer();

		/**
		 * Constructor
		 * @param harrisCorners The Harris corners the Ocean Harris detector has produced, must have the same size as `harrisCornerPyramidLevels`
		 * @param harrisCornerPyramidLevels Holds for each Harris corner the level index of the pyramid level where it was extracted, must have the same size as `harrisCorners`
		 */
		inline OFHarrisFeatureContainer(CV::Detector::HarrisCorners&& harrisCorners, Indices32&& harrisCornerPyramidLevels);

		/**
		 * Returns the Harris corners that are stored in this container
		 * @return A reference to the Harris corners
		 */
		inline const CV::Detector::HarrisCorners& harrisCorners() const;

		/**
		 * Returns the pyramid levels of the Harris corners
		 * @return The pyramid levels
		 */
		inline const Indices32& harrisCornerPyramidLevels() const;

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
		static inline std::string nameOFHarrisFeatureContainer();

		/**
		 * Creates an instance of this feature container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/**
		 * @sa FeatureContainer::convertFeaturesToKeypoints()
		 */
		Keypoints convertFeaturesToKeypoints() const override;

	protected:

		/// The Harris corners the Ocean Harris detector has produced
		CV::Detector::HarrisCorners harrisCorners_;

		/// The indices of the pyramid layers that these keypoints have been detected on
		Indices32 harrisCornerPyramidLevels_;
};

inline OFHarrisFeatureContainer::OFHarrisFeatureContainer() :
	OFHarrisFeatureContainer(CV::Detector::HarrisCorners(), Indices32())
{
	// Nothing else to do.
}

inline OFHarrisFeatureContainer::OFHarrisFeatureContainer(CV::Detector::HarrisCorners&& harrisCorners, Indices32&& harrisCornerPyramidLevels) :
	FeatureContainer(nameOceanFreakLibrary(), nameOFHarrisFeatureContainer(), FeatureCategory::KEYPOINTS, /* featureVersion */ 0u),
	harrisCorners_(std::move(harrisCorners)),
	harrisCornerPyramidLevels_(std::move(harrisCornerPyramidLevels))
{
	// Nothing else to do.
}

inline const CV::Detector::HarrisCorners& OFHarrisFeatureContainer::harrisCorners() const
{
	return harrisCorners_;
}

inline const Indices32& OFHarrisFeatureContainer::harrisCornerPyramidLevels() const
{
	return harrisCornerPyramidLevels_;
}

inline std::string OFHarrisFeatureContainer::nameOFHarrisFeatureContainer()
{
	return "OFHarrisFeatureContainer";
}

inline std::shared_ptr<UnifiedObject> OFHarrisFeatureContainer::create(const std::shared_ptr<Parameters>& parameters)
{
	ocean_assert_and_suppress_unused(parameters == nullptr && "This container does not have any parameters", parameters);

	return std::make_shared<OFHarrisFeatureContainer>();
}

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
