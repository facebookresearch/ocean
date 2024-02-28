// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanblob/OceanBlob.h"

#include "ocean/unifiedfeatures/FeatureContainer.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanBlob
{

/**
 * The definition of the feature container for the Ocean Harris Detector
 * @ingroup unifiedfeaturesoceanblob
 */
class OCEAN_UNIFIEDFEATURES_OB_EXPORT OBFeatureContainer final : public FeatureContainer
{
	public:
		/**
		 * Default constructor
		 */
		inline OBFeatureContainer();

		/**
		 * Constructor
		 * @param blobFeatures The Blob features (with only keypoints) in the form of BlobFeatures the Ocean Blob detector has produced.
		 * @param integralFrame The integral image used in blob detection. This image is stored here to share with the generator.
		 * @param detectorFrame Pointer to the frame used in detection. This is to make sure the frame used in descriptor generator is the same as the one in detector.
		 */
		inline OBFeatureContainer(CV::Detector::Blob::BlobFeatures&& blobFeatures, Frame&& integralFrame, const Frame* detectorFrame);

		/**
		 * Returns the keypoints detected by blob detector in the form of CV::Detector::Blob::BlobFeatures that are stored in this container
		 * @return A reference to the Blob corners
		 */
		inline const CV::Detector::Blob::BlobFeatures& blobFeatures() const;

		/**
		 * Returns the size of this container
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
		static inline std::string nameOBFeatureContainer();

		/**
		 * Creates an empty instance of this feature container
		 * @return The pointer to new instance
		 */
		static inline std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& /* ignored */);

		/**
		 * Checks whether the provided frame was used to initialize the detector.
		 * If the frame was the same frame that used for detection, then we can skip reinitializing the detector during descriptor generation.
		 * @param frame The frame to check
		 * @return True, if the provided frame was exactly the frame that was used for detection; otherwise, false
		 */
		inline bool checkIfFrameWasUsedForDetection(const Frame& frame) const;

		/**
		 * Returns the integral frame used in blob detection.
		 * Only useful when the Frame in generator is the same as the 'detectorFrame_'.
		 * @return A reference to the integral frame
		 */
		inline const Frame& integralFrame() const;

	protected:
		/**
		 * @sa FeatureContainer::convertFeaturesToKeypoints()
		 */
		Keypoints convertFeaturesToKeypoints() const override;

	private:
		/// Contains key points from the blob detector.
		CV::Detector::Blob::BlobFeatures blobFeatures_;

		/// Integral image used in detection. Will be passed to descriptor generation.
		Frame integralFrame_;

		/// Pointer to the frame that was used for detection, if any.
		/// If this is available, it allows us to skip SIFT pyramid recomputation during descriptor generation.
		const Frame* detectorFrame_;
};

inline OBFeatureContainer::OBFeatureContainer() :
	OBFeatureContainer(CV::Detector::Blob::BlobFeatures(), Frame(), nullptr)
{
	// Nothing else to do.
}

inline OBFeatureContainer::OBFeatureContainer(CV::Detector::Blob::BlobFeatures&& blobFeatures, Frame&& integralFrame, const Frame* detectorFrame) :
	FeatureContainer(nameOceanBlobLibrary(), nameOBFeatureContainer(), FeatureCategory::KEYPOINTS, /* featureVersion */ 0u),
	blobFeatures_(std::move(blobFeatures)),
	integralFrame_(std::move(integralFrame)),
	detectorFrame_(detectorFrame)
{
	// Nothing else to do.
}

inline const CV::Detector::Blob::BlobFeatures& OBFeatureContainer::blobFeatures() const
{
	return blobFeatures_;
}

inline std::string OBFeatureContainer::nameOBFeatureContainer()
{
	return "OBFeatureContainer";
}

inline std::shared_ptr<UnifiedObject> OBFeatureContainer::create(const std::shared_ptr<Parameters>& /* ignored */)
{
	return std::make_shared<OBFeatureContainer>();
}

inline bool OBFeatureContainer::checkIfFrameWasUsedForDetection(const Frame& frame) const
{
	return detectorFrame_ == &frame;
}

inline const Frame& OBFeatureContainer::integralFrame() const
{
	return integralFrame_;
}


} // namespace OceanBlob

} // namespace UnifiedFeatures

} // namespace Ocean
