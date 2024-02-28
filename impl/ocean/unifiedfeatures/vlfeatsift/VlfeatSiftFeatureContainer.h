// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSift.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

#include "vl/covdet.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * The definition of the feature container for the VLFeat SIFT Detector
 * @ingroup unifiedfeaturesvlfeatsift
 */
class OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT VlfeatSiftFeatureContainer : public FeatureContainer
{
	public:

		typedef std::unique_ptr<VlCovDet, void (*)(VlCovDet*)> VlfeatCovariantDetector;

		/**
		 * Default constructor
		 */
		inline VlfeatSiftFeatureContainer();

		/**
		 * Constructor
		 * @param detector Detector object that owns the memory for the extracted features
		 * @param detectorFrame Pointer to the frame pixel data that was used when initializing the detector
		 */
		inline VlfeatSiftFeatureContainer(VlfeatCovariantDetector&& detector, const void* detectorFrame);

		/**
		 * Returns the detector object containing features.
		 * This detector object should NOT be directly used for detection -- it is only kept around to cache the already computed VLFeat image pyramid for subsequent descriptor generation, if applicable.
		 * @return A reference to the detector
		 */
		inline const VlfeatCovariantDetector& detector() const;

		/**
		 * Checks whether the provided frame was used to initialize the detector.
		 * If the frame was the same frame that used for detection, then we can skip reinitializing the detector during descriptor generation.
		 * @param frame The frame to check
		 * @return True, if the provided frame was exactly the frame that was used for detection; otherwise, false
		 */
		inline bool checkIfFrameWasUsedForDetection(const Frame& frame) const;

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
		static inline std::string nameVlfeatSiftFeatureContainer();

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

	private:

		/// Extracts scale and orientation parameters from a feature's affine transformation
		static void computeScaleAndOrientationForFeature(const VlCovDetFeature& feature, float& scale, float& orientation);

		/// The VLFeat SIFT detector which contains features
		VlfeatCovariantDetector detector_;

		/// Pointer to the frame pixel data that was used for detection, if any.
		/// If this is available, it allows us to skip SIFT pyramid recomputation during descriptor generation.
		const void* detectorFrame_;
};

// Use a default detection type of VL_COVDET_METHOD_DOG; we don't actually use this detection type in the container.
inline VlfeatSiftFeatureContainer::VlfeatSiftFeatureContainer() :
	VlfeatSiftFeatureContainer(VlfeatCovariantDetector(vl_covdet_new(VL_COVDET_METHOD_DOG), &vl_covdet_delete), nullptr)
{
	// Nothing else to do.
}

inline VlfeatSiftFeatureContainer::VlfeatSiftFeatureContainer(VlfeatCovariantDetector&& detector, const void* detectorFrame) :
	FeatureContainer(nameVlfeatSiftLibrary(), nameVlfeatSiftFeatureContainer(), FeatureCategory::KEYPOINTS, /* featureVersion */ 0u),
	detector_(std::move(detector)),
	detectorFrame_(detectorFrame)
{
	// Nothing else to do.
}

inline const VlfeatSiftFeatureContainer::VlfeatCovariantDetector& VlfeatSiftFeatureContainer::detector() const
{
	return detector_;
}

inline bool VlfeatSiftFeatureContainer::checkIfFrameWasUsedForDetection(const Frame& frame) const
{
	return detectorFrame_ == frame.constdata<void>();
}

inline std::string VlfeatSiftFeatureContainer::nameVlfeatSiftFeatureContainer()
{
	return "VlfeatSiftFeatureContainer";
}

inline std::shared_ptr<UnifiedObject> VlfeatSiftFeatureContainer::create(const std::shared_ptr<Parameters>& /* ignored */)
{
	return std::make_shared<VlfeatSiftFeatureContainer>();
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
