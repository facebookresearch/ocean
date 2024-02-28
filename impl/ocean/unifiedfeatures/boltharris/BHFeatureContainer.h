// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/boltharris/BoltHarris.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace BoltHarris
{

/**
 * The definition of the feature container that this detector will generate
 * @ingroup boltharris
 */
struct OCEAN_UNIFIEDFEATURES_BH_EXPORT BHFeatureContainer final : public FeatureContainer
{
	public:

		/**
		 * Default constructor
		 */
		inline BHFeatureContainer();

		/**
		 * Constructor
		 * @param xy The x- and y-coordinates of the detected Harris corners, size must be: `xy.size() == 2 * cornerResponse.size()`
		 * @param cornerResponse The strength values of the detector Harris corners, size must be: `xy.size() == 2 * cornerResponse.size()`
		 */
		inline BHFeatureContainer(std::vector<int>&& xy, std::vector<int>&& cornerResponse);

		/**
		 * @sa FeatureDetector::size()
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
		static inline std::string nameBHFeatureContainer();

		/**
		 * Creates an instance of this descriptor container
		 * @return A pointer to the new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/**
		 * @sa FeatureDetector::convertFeaturesToKeypoints()
		 */
		Keypoints convertFeaturesToKeypoints() const override;

	protected:

		/// The x- and y-coordinates of the detected Harris corners: `{ x0, y0, x1, y1, ... }`
		std::vector<int> xy_;

		/// he strength values of the detector Harris corners: `{ s0, s1, s2, ... }`
		std::vector<int> cornerResponse_;
};

inline BHFeatureContainer::BHFeatureContainer() :
	BHFeatureContainer(std::vector<int>(), std::vector<int>())
{
	// Nothing else to do.
}

inline BHFeatureContainer::BHFeatureContainer(std::vector<int>&& xy, std::vector<int>&& cornerResponse) :
	FeatureContainer(nameBoltHarrisLibrary(), nameBHFeatureContainer(), FeatureCategory::KEYPOINTS, /* featureVersion */ 0u),
	xy_(std::move(xy)),
	cornerResponse_(std::move(cornerResponse))
{
	// Nothing else to do.
}

inline std::string BHFeatureContainer::nameBHFeatureContainer()
{
	return "BHFeatureContainer";
}

inline std::shared_ptr<UnifiedObject> BHFeatureContainer::create(const std::shared_ptr<Parameters>& /* parameters */)
{
	return std::make_shared<BHFeatureContainer>();
}

} // namespace BoltHarris

} // namespace UnifiedFeatures

} // namespace Ocean
