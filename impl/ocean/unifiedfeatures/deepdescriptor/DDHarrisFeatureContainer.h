// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/deepdescriptor/DeepDescriptor.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

#include <perception/features/deep/Deep.h>

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepDescriptor
{

/**
 * Definition of the descriptor container that will hold the float-based deep descriptors
 * @ingroup unifiedfeaturesdeepdescriptor
 */
struct OCEAN_UNIFIEDFEATURES_DD_EXPORT DDHarrisFeatureContainer final : public FeatureContainer
{
	public:

		/**
		 * Default constructor which creates an empty container
		 */
		inline DDHarrisFeatureContainer();

		/**
		 * Constructor
		 * @param perceptionKeypoints The keypoints that will be stored in this container
		 */
		 inline DDHarrisFeatureContainer(std::vector<perception::features::Keypoint>&& perceptionKeypoints);

		/**
		 * Returns the number of descriptors stored in this instance
		 * @sa DescriptorContainer::size()
		 */
		inline size_t size() const override;

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
		static inline std::string nameDDHarrisFeatureContainer();

		/**
		 * Creates an instance of this container
		 * @param parameters Optional parameters to initialize this container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/**
		 * @sa FeatureContainer::convertFeaturesToKeypoints()
		 */
		Keypoints convertFeaturesToKeypoints() const override;

	public:

		// The keypoints that are stored in this container
		std::vector<perception::features::Keypoint> perceptionKeypoints_;
};

inline size_t DDHarrisFeatureContainer::size() const
{
	return perceptionKeypoints_.size();
}

inline DDHarrisFeatureContainer::DDHarrisFeatureContainer() :
	DDHarrisFeatureContainer(std::vector<perception::features::Keypoint>())
{
	// Nothing else to do.
}

inline DDHarrisFeatureContainer::DDHarrisFeatureContainer(std::vector<perception::features::Keypoint>&& perceptionKeypoints) :
	FeatureContainer(nameDeepDescriptorLibrary(), nameDDHarrisFeatureContainer(), FeatureCategory::KEYPOINTS, /* version */ 0u),
	perceptionKeypoints_(std::move(perceptionKeypoints))
{
	// Nothing else to do.
}

inline std::string DDHarrisFeatureContainer::nameDDHarrisFeatureContainer()
{
	return "DDHarrisFeatureContainer";
}

inline std::shared_ptr<UnifiedObject> DDHarrisFeatureContainer::create(const std::shared_ptr<Parameters>& /* parameters */)
{
	return std::make_shared<DDHarrisFeatureContainer>();
}

} // namespace DeepDescriptor

} // namespace UnifiedFeatures

} // namespace Ocean
