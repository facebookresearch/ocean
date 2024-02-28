// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/deeppatchuint8/DeepPatchUint8.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace DeepPatchUint8
{

/**
 * Definition of the descriptor container that will hold the float-based deep descriptors
 * @ingroup unifiedfeaturesdeeppatchuint8
 */
struct OCEAN_UNIFIEDFEATURES_DP8_EXPORT DP8FeatureContainer final : public FeatureContainer
{
	public:

		/**
		 * Default constructor which creates an empty container
		 */
		inline DP8FeatureContainer();

		/**
		 * Constructor
		 * @param keypoints The keypoints that will be stored in this container
		 */
		 inline DP8FeatureContainer(std::vector<perception::features::Keypoint>&& keypoints);

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
		static inline std::string nameDP8FeatureContainer();

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
		std::vector<perception::features::Keypoint> keypoints_;
};

inline size_t DP8FeatureContainer::size() const
{
	return keypoints_.size();
}

inline DP8FeatureContainer::DP8FeatureContainer() :
	DP8FeatureContainer(std::vector<perception::features::Keypoint>())
{
	// Nothing else to do.
}

inline DP8FeatureContainer::DP8FeatureContainer(std::vector<perception::features::Keypoint>&& keypoints) :
	FeatureContainer(nameDeepPatchUint8Library(), nameDP8FeatureContainer(), FeatureCategory::KEYPOINTS, /* version */ 0u),
	keypoints_(std::move(keypoints))
{
	// Nothing else to do.
}

inline std::string DP8FeatureContainer::nameDP8FeatureContainer()
{
	return "DP8FeatureContainer";
}

inline std::shared_ptr<UnifiedObject> DP8FeatureContainer::create(const std::shared_ptr<Parameters>& /* parameters */)
{
	return std::make_shared<DP8FeatureContainer>();
}

} // namespace DeepPatchUint8

} // namespace UnifiedFeatures

} // namespace Ocean
