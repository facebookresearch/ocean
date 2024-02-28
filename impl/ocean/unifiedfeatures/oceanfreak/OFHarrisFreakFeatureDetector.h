// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanfreak/OceanFreak.h"
#include "ocean/unifiedfeatures/oceanfreak/OFDescriptorContainer.h"
#include "ocean/unifiedfeatures/oceanfreak/OFHarrisFeatureContainer.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

/// Forward declaration
template <size_t tDescriptorSizeBytes>
class OFHarrisFreakFeatureDetectorT;

/// Typedef for the Ocean Harris features with Ocean FREAK descriptors (32 bytes)
typedef OFHarrisFreakFeatureDetectorT<32> OFHarrisFreakFeatureDetector32;

/// Typedef for the Ocean Harris features with Ocean FREAK descriptors (64 bytes)
typedef OFHarrisFreakFeatureDetectorT<64> OFHarrisFreakFeatureDetector64;

/**
 * Definition of a feature detector for Ocean Harris corners in combination with Ocean FREAK descriptors
 * This class wraps the function `CV::Detector::FREAKDescriptor::extractHarrisCornersAndComputeDescriptors()`, which
 * extracts Harris corners and, simultaneously, computes FREAK descriptors for these corners.
 *
 * This kind of function is made usable in this framework, by splitting the above function into a feature detector and a
 * separate descriptor generator that are used together. The following example outlines the suggested workflow:
 * @code
 * std::shared_ptr<OFHarrisFreakFeatureDetector32> detector = std::make_shared<OFHarrisFreakFeatureDetector32>(...parameters...);
 * std::shared_ptr<OFHarrisFreakFeatureDetector32::OceanFreakDescriptorGenerator> descriptorGenerator = detector->getDescriptorGenerator();
 *
 * while (...)
 * {
 *     Frame frame = ...;
 *
 *     // Detects features and computes descriptors and stores the descriptors in `descriptorGenerator`
 *     std::shared_ptr<FeatureContainer> featureContainer;
 *     detector->detectFeatures(frame, featureContainer);
 *
 *     // Creates the descriptor container based on the descriptors stored in the previous step
 *     std::shared_ptr<DescriptorContainer> descriptorContainer;
 *     descriptorGenerator->generateDescriptors(frame, featureContainer, descriptorContainer);
 *
 *     ...
 * }
 * @endcode
 *
 * @ingroup unifiedfeaturesoceanfreak
 */
template <size_t tDescriptorSizeBytes>
class OCEAN_UNIFIEDFEATURES_OF_EXPORT OFHarrisFreakFeatureDetectorT final : public FeatureDetector
{
	public:

		/// Short-hand definition of the currently selected type of Freak descriptor
		typedef CV::Detector::FREAKDescriptorT<tDescriptorSizeBytes> FreakDescriptor;

		/// Short-hand definition of a vector of the currently selected type of Freak descriptor
		typedef std::vector<FreakDescriptor> FreakDescriptors;

		/**
		 * Definition of parameters for the detector
		 */
		class OFParameters : public Parameters
		{
			public:

				/**
				 * Constructor
				 * @param cameraDerivativeFunctor The functor that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray, must be valid
				 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths) , range: (0, infinity)
				 */
				OFParameters(const std::shared_ptr<typename FreakDescriptor::CameraDerivativeFunctor>& cameraDerivativeFunctor, const float inverseFocalLength);

			public:

				/// This value determines the last layer of the frame pyramid for which corners and descriptors will be computed, range: [0, maxFrameArea)
				unsigned int minFrameArea = 50u * 50u;

				/// This value determines the first layer of the frame pyramid for which corners and descriptors will be computed, range: (minFrameArea, infinity)
				unsigned int maxFrameArea = 640u * 480u;

				/// Expected number of Harris cornes if the resolution of the image were 640 x 480 pixels. The actual number of expected corners is scaled to the size first layer in the image pyramid that is used for the extraction and then distributed over the range of pyramid layers that is used, range: [1, infinity)
				unsigned int expectedHarrisCorners640x480 = 1000u;

				/// Scale factor that determines the rate with which the number of corners is reduced as the function climbs through the image pyramid , range: [0, 1)
				Scalar harrisCornersReductionScale = Scalar(0.4);

				/// Threshold value for the Harris corner detector, range: [0, 512]
				unsigned int harrisCornerThreshold = 1u;

				/// The inverse focal length (assumes identical vertical and horizontal focal lengths) , range: (0, infinity)
				float inverseFocalLength;

				/// A functor that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray
				std::shared_ptr<typename FreakDescriptor::CameraDerivativeFunctor> cameraDerivativeFunctor;

				/// If true, all invalid descriptors (and corresponding corners and entries of pyramid levels) will be removed, otherwise all results will be remain as-is
				bool removeInvalid = true;

				/// Minimum distance in pixels from the image border (same value on all levels of the pyramid) that all Harris corners must have in order to be accepted, otherwise they will be discarded, range: [0, min(yFrame.width(), yFrame.height())/2)
				Scalar border = Scalar(20);

				/// If true, force the subpixel interpolation to determine the exact position of the extracted Harris corners
				bool determineExactHarrisCornerPositions = true;

				/// If true the original input frame is undistorted and all extracted 2D feature positions will be marked as undistorted, too
				bool yFrameIsUndistorted = false;
		};

	protected:

		/**
		 * Definition of a descriptor generator to make the FREAK descriptors accessible
		 * This descriptor generator is only available in combination with the enclosing feature detector. It is, by itself not useful.
		 */
		class IntegratedHelperDescriptorGenerator final : public DescriptorGenerator
		{
			friend OFHarrisFreakFeatureDetectorT;

			public:

				/**
				 * Default constructor
				 */
				IntegratedHelperDescriptorGenerator();

				/**
				 * @sa DescriptorGenerator::generateDescriptors()
				 */
				Result generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptors, Worker* worker = nullptr) const override;

			protected:

				/**
				 * Stores descriptors in this instance
				 * This function will be called from the feature detector and will store
				 * @param freakDescriptors The FREAK descriptors that will be stored in this instance
				 */
				void setFreakDescriptors(FreakDescriptors&& freakDescriptors);

			protected:

				// The FREAK descriptors
				mutable FreakDescriptors freakDescriptors_;
		};

	public:

		/**
		 * Creates an invalid detector
		 */
		OFHarrisFreakFeatureDetectorT() = delete;

		/**
		 * Constructor
		 * @param parameters The parameters for this detector
		 */
		explicit OFHarrisFreakFeatureDetectorT(OFParameters&& parameters);

		/**
		 * @sa FeatureDetector::detectFeatures()
		 */
		FeatureDetector::Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker = nullptr) const override;

		/**
		 * Returns the descriptor generator that is part of
		 * @return The pointer to the instance of the descriptor generator
		 */
		std::shared_ptr<DescriptorGenerator> getDescriptorGenerator();

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static std::string nameOFHarrisFreakFeatureDetector();

		/**
		 * Creates an instance of this feature detector
		 * @return The pointer to new instance
		 */
		static std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters of this detector
		OFParameters parameters_;

		/// The instance of the descriptor generator that will manage the actual descriptors
		mutable std::shared_ptr<IntegratedHelperDescriptorGenerator> descriptorGenerator_;
};

template <size_t tDescriptorSizeBytes>
OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::OFParameters::OFParameters(const std::shared_ptr<typename FreakDescriptor::CameraDerivativeFunctor>& cameraDerivativeFunctor, const float focalLengthInverse) :
	inverseFocalLength(focalLengthInverse),
	cameraDerivativeFunctor(cameraDerivativeFunctor)
{
	ocean_assert(cameraDerivativeFunctor != nullptr);
	ocean_assert(inverseFocalLength > 0.0f);
}

template <size_t tDescriptorSizeBytes>
OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::IntegratedHelperDescriptorGenerator::IntegratedHelperDescriptorGenerator() :
	DescriptorGenerator(nameOceanFreakLibrary(), "OceanFreakHarrisDescriptorGenerator" + String::toAString(tDescriptorSizeBytes), /* border, cf. https://fburl.com/diffusion/b6h2fugt */ 20u, DescriptorCategory::BINARY_DESCRIPTOR, { FrameType::FORMAT_Y8 })
{
	// Nothing else to do
}

template <size_t tDescriptorSizeBytes>
void OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::IntegratedHelperDescriptorGenerator::setFreakDescriptors(FreakDescriptors&& freakDescriptors)
{
	freakDescriptors_ = std::move(freakDescriptors);
}

template <size_t tDescriptorSizeBytes>
OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::OFHarrisFreakFeatureDetectorT(OFParameters&& parameters) :
	FeatureDetector(nameOceanFreakLibrary(), nameOFHarrisFreakFeatureDetector(), FeatureCategory::KEYPOINTS, { FrameType::FORMAT_Y8 }, /* detectorVersion */ 0u),
	parameters_(parameters),
	descriptorGenerator_(std::make_shared<IntegratedHelperDescriptorGenerator>())
{
	// Nothing else to do.
}

template <size_t tDescriptorSizeBytes>
std::shared_ptr<DescriptorGenerator> OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::getDescriptorGenerator()
{
	ocean_assert(descriptorGenerator_ != nullptr);
	return descriptorGenerator_;
}

template <size_t tDescriptorSizeBytes>
std::string OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::nameOFHarrisFreakFeatureDetector()
{
	return "OFHarrisFreakFeatureDetector" + String::toAString(tDescriptorSizeBytes);
}

template <size_t tDescriptorSizeBytes>
std::shared_ptr<UnifiedObject> OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::create(const std::shared_ptr<Parameters>& parameters)
{
	if (parameters == nullptr)
	{
		ocean_assert(false && "The initialization requires parameters!");
		return nullptr;
	}

	const std::shared_ptr<OFParameters> ofParametersPtr = std::static_pointer_cast<OFParameters>(parameters);

	if (ofParametersPtr == nullptr)
	{
		ocean_assert(false && "Incompatible or missing parameters");
		return nullptr;
	}

	OFParameters ofParameters = *ofParametersPtr;
	return std::make_shared<OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>>(std::move(ofParameters));
}

template <size_t tDescriptorSizeBytes>
DescriptorGenerator::Result OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::IntegratedHelperDescriptorGenerator::generateDescriptors(const Frame& /* frame - ignored */, const FeatureContainer& /* featureContainer - ignored */, std::shared_ptr<DescriptorContainer>& descriptorContainer, Worker* /* worker - ignored */) const
{
	descriptorContainer = std::make_shared<OFDescriptorContainerT<tDescriptorSizeBytes>>(std::move(freakDescriptors_));

	return Result::SUCCESS;
}

template <size_t tDescriptorSizeBytes>
FeatureDetector::Result OFHarrisFreakFeatureDetectorT<tDescriptorSizeBytes>::detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	ocean_assert(parameters_.cameraDerivativeFunctor != nullptr);
	ocean_assert(parameters_.minFrameArea < parameters_.maxFrameArea);
	ocean_assert(parameters_.expectedHarrisCorners640x480 != 0u);
	ocean_assert(parameters_.harrisCornersReductionScale >= 0 && parameters_.harrisCornersReductionScale < Scalar(1));
	ocean_assert(parameters_.harrisCornerThreshold >= 0u && parameters_.harrisCornerThreshold <= 512u);
	ocean_assert(parameters_.inverseFocalLength > 0);
	ocean_assert(parameters_.cameraDerivativeFunctor != nullptr);
	ocean_assert(parameters_.border >= Scalar(20));
	ocean_assert(descriptorGenerator_ != nullptr);

	CV::Detector::HarrisCorners harrisCorners;
	Indices32 harrisCornerPyramidLevels;
	FreakDescriptors freakDescriptors;

	if (CV::Detector::FREAKDescriptorT<tDescriptorSizeBytes>::extractHarrisCornersAndComputeDescriptors(frame, parameters_.maxFrameArea, parameters_.minFrameArea, parameters_.expectedHarrisCorners640x480, parameters_.harrisCornersReductionScale, parameters_.harrisCornerThreshold, parameters_.inverseFocalLength, *parameters_.cameraDerivativeFunctor, harrisCorners, harrisCornerPyramidLevels, freakDescriptors, parameters_.removeInvalid, parameters_.border, parameters_.determineExactHarrisCornerPositions, parameters_.yFrameIsUndistorted, worker))
	{
		// TODO Apply a lock during the assignment in order to keep features and descriptors in sync
		ocean_assert(harrisCorners.size() == harrisCornerPyramidLevels.size());
		ocean_assert(harrisCorners.size() == freakDescriptors.size());

		featureContainer = std::make_shared<OFHarrisFeatureContainer>(std::move(harrisCorners), std::move(harrisCornerPyramidLevels));
		descriptorGenerator_->setFreakDescriptors(std::move(freakDescriptors));
	}

	return Result::SUCCESS;
}

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
