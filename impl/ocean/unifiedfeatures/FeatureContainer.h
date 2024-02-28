// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/UnifiedObject.h"
#include "ocean/unifiedfeatures/Keypoint.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace Thrift
{

/// Forward declaration
class FeatureContainer;

} // namespace Thrift

/**
 * Definition of the generic interface for features containers
 * @ingroup unifiedfeatures
 */
class OCEAN_UNIFIEDFEATURES_EXPORT FeatureContainer : public UnifiedObject
{
	public:

		/**
		 * Default destructor
		 */
		virtual ~FeatureContainer() = default;

		/**
		 * Returns the feature category of the container
		 * @return The feature category
		 */
		inline FeatureCategory featureCategory() const;

		/**
		 * Converts the abstracts features to keypoints
		 * If the feature category is `FeatureCategory::KEYPOINTS` all features will converted to `Keypoints`, for other feature categories the result will be empty list
		 * @return The list of keypoints
		 */
		inline const Keypoints& keypoints() const;

		/**
		 * Returns the number of features stored in this container
		 * @return The number of features
		 */
		virtual size_t size() const = 0;

		/**
		 * Converts the contents of this container to Thrift data structures
		 * @param thriftFeatureContainer The Thrift data structure that will be filled
		 * @return True on success, otherwise false
		 */
		virtual bool toThrift(Thrift::FeatureContainer& thriftFeatureContainer) const = 0;

		/**
		 * Converts a Thrift data structure back to the hidden data structures of this container
		 * @param thriftFeatureContainer The Thrift data structure that will be used to fill this container
		 * @return True on success, otherwise false
		 */
		virtual bool fromThrift(const Thrift::FeatureContainer& thriftFeatureContainer) = 0;

		/**
		 * Draws this container into a frame
		 * @param frame The frame into which the features will be drawn, must be valid
		 * @param offset An optional offset that is applied to the features before drawing them
		 */
		virtual void drawFeatures(Frame& frame, const Vector2& offset = Vector2(Scalar(0), Scalar(0))) const;

	protected:

		/**
		 * Constructor
		 * @param libraryName The name of the library that this feature container belongs to
		 * @param featureName The name of the implementation of these features, must be valid
		 * @param featureCategory The type of the features that will be stored in this container
		 * @param featureVersion Optional version of the underlying implementation of these features, range: [0, infinity)
		 */
		inline FeatureContainer(const std::string& libraryName, const std::string& featureName, const FeatureCategory featureCategory, const unsigned int featureVersion = 0u);

		/**
		 * Converts the features into keypoints
		 * Classes that implemented this interface can choose to override this function if their feature category is `FeatureCategory::KEYPOINTS`
		 * @return The features that have been converted into keypoints
		 */
		inline virtual Keypoints convertFeaturesToKeypoints() const;

	protected:

		/// The type of the features that will be stored in this container
		const FeatureCategory featureCategory_;

		/// The features that have been converted into keypoints
		mutable std::shared_ptr<Keypoints> keypoints_;
};

inline FeatureCategory FeatureContainer::featureCategory() const
{
	return featureCategory_;
}

inline const Keypoints& FeatureContainer::keypoints() const
{
	if (keypoints_ == nullptr)
	{
		Keypoints keypoints = convertFeaturesToKeypoints();
		keypoints_ = std::make_shared<Keypoints>(std::move(keypoints));
	}

	ocean_assert(keypoints_ != nullptr);
	ocean_assert(featureCategory() == FeatureCategory::KEYPOINTS || keypoints_->size() == 0);

	return *keypoints_;
}

inline FeatureContainer::FeatureContainer(const std::string& libraryName, const std::string& featureName, const FeatureCategory featureCategory, const unsigned int featureVersion) :
	UnifiedObject(libraryName, featureName, featureVersion),
	featureCategory_(featureCategory)
{
	// Nothing else to do.
}

inline Keypoints FeatureContainer::convertFeaturesToKeypoints() const
{
	return Keypoints();
}

} // namespace UnifiedFeatures

} // namespace Ocean
