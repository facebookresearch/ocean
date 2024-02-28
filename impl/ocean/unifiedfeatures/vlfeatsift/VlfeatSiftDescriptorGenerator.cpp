// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftDescriptorGenerator.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverterY8.h"

#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/vlfeatsift/Utilities.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftDescriptorContainer.h"
#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSiftFeatureContainer.h"

#include "vl/sift.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

DescriptorGenerator::Result VlfeatSiftDescriptorGenerator::generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptors, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!isSupportedPixelFormat(frame.pixelFormat()))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	// Potentially cast the Y8 frame to float32, and/or make a contiguous copy of the frame if necessary.
	std::unique_ptr<Frame> float32FramePtr;
	const Frame& float32Frame = Utilities::frameAsContiguousF32(frame, float32FramePtr, worker);

	const unsigned int domainSizePoolingNumberScales = parameters_.useDomainSizePooling ? max(parameters_.domainSizePoolingNumberScales, 1u) : 1u;
	float domainSizePoolingMinimumScale = 1.0f;
	float domainSizePoolingScaleStep = 0.0f;

	if (domainSizePoolingNumberScales > 1u)
	{
		domainSizePoolingMinimumScale = parameters_.domainSizePoolingMinimumScale;
		domainSizePoolingScaleStep = (parameters_.domainSizePoolingMaximumScale - domainSizePoolingMinimumScale) / domainSizePoolingNumberScales;

		if (domainSizePoolingMinimumScale < 0.0f || NumericF::isEqualEps(domainSizePoolingMinimumScale) || domainSizePoolingScaleStep <= 0.0f)
		{
			ocean_assert(false && "Invalid Domain Size Pooling parameters");
			return Result::GENERAL_ERROR;
		}
	}

	// Create a dummy SIFT filter required for descriptor generation.
	// The parameters here are just minimal parameters required for creating the object.
	// Only the description parameters (magnification, Gaussian window size, and normalization threshold) are actually used, and we take the VLFeat defaults.
	std::unique_ptr<VlSiftFilt, void (*)(VlSiftFilt*)> siftFilter(vl_sift_new(16, 16, 1, 3, 0), &vl_sift_delete);
	if (!siftFilter)
	{
		ocean_assert(false && "Failed to create SIFT filter");
		return Result::GENERAL_ERROR;
	}

	// If the existing feature container is already a VLFeat SIFT container, then it already holds the features in our required format, plus a VLFeat detector instance.
	// Otherwise, we'll need to copy the features.
	std::unique_ptr<VlfeatSiftFeatureContainer> localVlfeatSiftFeatureContainer;

	const VlfeatSiftFeatureContainer* vlfeatSiftFeatureContainer = dynamic_cast<const VlfeatSiftFeatureContainer*>(&featureContainer);

	if (!vlfeatSiftFeatureContainer)
	{
		localVlfeatSiftFeatureContainer = std::make_unique<VlfeatSiftFeatureContainer>();
		vlfeatSiftFeatureContainer = localVlfeatSiftFeatureContainer.get();
		ocean_assert(vlfeatSiftFeatureContainer);

		const VlfeatSiftFeatureContainer::VlfeatCovariantDetector& detector = vlfeatSiftFeatureContainer->detector();

		// Copy over the keypoints.
		for (const Keypoint& keypoint : featureContainer.keypoints())
		{
			VlCovDetFeature feature;

			// VLFeat uses a top-left pixel coordinate of (-0.5, -0.5).
			feature.frame.x = keypoint.observation().x() - 0.5f;
			feature.frame.y = keypoint.observation().y() - 0.5f;

			// Note that the suboctave (feature.s) and scores are unused.
			feature.o = keypoint.octave();

			// Either the feature has a full affine transform, or we can derive one from the scale and orientation (if available).
			const std::shared_ptr<const SquareMatrixF2> affineTransformImageFromCanonical = keypoint.affineTransformImageFromCanonical();

			if (affineTransformImageFromCanonical != nullptr)
			{
				feature.frame.a11 = (*affineTransformImageFromCanonical)(0, 0);
				feature.frame.a12 = (*affineTransformImageFromCanonical)(0, 1);
				feature.frame.a21 = (*affineTransformImageFromCanonical)(1, 0);
				feature.frame.a22 = (*affineTransformImageFromCanonical)(1, 1);
			}
			else
			{
				// If the scale is unavailable, features will be extracted only at the octave resolution.
				const float scale = keypoint.hasScale() ? keypoint.scale() : 1.0f;

				feature.frame.a11 = scale;
				feature.frame.a12 = scale;

				if (keypoint.hasOrientation())
				{
					feature.frame.a11 *= NumericF::cos(keypoint.orientation());
					feature.frame.a12 *= -NumericF::sin(keypoint.orientation());
				}

				feature.frame.a21 = -feature.frame.a12;
				feature.frame.a22 = feature.frame.a11;
			}

			vl_covdet_append_feature(detector.get(), &feature);
		}
	}

	const VlfeatSiftFeatureContainer::VlfeatCovariantDetector& detector = vlfeatSiftFeatureContainer->detector();

	// If the image used for detection is exactly the same as the image here, then we can skip VLFeat's internal image processing.
	// Otherwise, we will need to set up the image for processing, again.
	if (!vlfeatSiftFeatureContainer->checkIfFrameWasUsedForDetection(frame))
	{
		vl_covdet_set_first_octave(detector.get(), parameters_.firstOctave);
		vl_covdet_set_octave_resolution(detector.get(), parameters_.octaveResolution);

		vl_covdet_put_image(detector.get(), float32Frame.constdata<float>(), float32Frame.width(), float32Frame.height());
	}

	// Compute the SIFT descriptors
	const size_t numberKeypoints = vlfeatSiftFeatureContainer->size();
	const VlCovDetFeature* features = vl_covdet_get_features(detector.get());

	/// Source image patch sampling radius.
	/// The total number of samples used in a patch is (2 * patchResolution + 1)^2.
	constexpr vl_size kPatchResolution = 15;

	constexpr double kPatchRelativeExtent = 7.5; // radius in pixels at the feature's octave, used for patch sampling
	constexpr double kPatchRelativeSmoothing = 1.0; // smoothing value used in sampling, relative to the patch frame (unit circle)
	const double kSigma = 2.0; // Gaussian window size used for descriptor calculation (default mentioned in VLFeat docs)

	constexpr vl_size kPatchSideLength = 2 * kPatchResolution + 1;
	constexpr vl_size kPatchArea = kPatchSideLength * kPatchSideLength;

	float patch[kPatchArea];
	float patchGradient[2 * kPatchArea]; // 2-channel interleaved gradient magnitude and angle

	typename VlfeatSiftDescriptorContainer::SiftDescriptors computedDescriptors;
	computedDescriptors.reserve(numberKeypoints);

	for (size_t i = 0; i < numberKeypoints; ++i)
	{
		typename VlfeatSiftDescriptorContainer::SiftDescriptors scaledDescriptors(domainSizePoolingNumberScales);

		for (size_t level = 0; level < domainSizePoolingNumberScales; ++level)
		{
			const float domainSizePoolingScale = domainSizePoolingMinimumScale + domainSizePoolingScaleStep * level;

			VlFrameOrientedEllipse scaledFrame = features[i].frame;
			scaledFrame.a11 *= domainSizePoolingScale;
			scaledFrame.a12 *= domainSizePoolingScale;
			scaledFrame.a21 *= domainSizePoolingScale;
			scaledFrame.a22 *= domainSizePoolingScale;

			vl_covdet_extract_patch_for_frame(detector.get(), patch, kPatchResolution, kPatchRelativeExtent, kPatchRelativeSmoothing, scaledFrame);

			vl_imgradient_polar_f(patchGradient, patchGradient + 1, 2, 2 * kPatchSideLength, patch, kPatchSideLength, kPatchSideLength, kPatchSideLength);

			vl_sift_calc_raw_descriptor(siftFilter.get(), patchGradient, scaledDescriptors[level].descriptorLevels().front().data(), kPatchSideLength, kPatchSideLength, kPatchResolution, kPatchResolution, kSigma, 0);
		}

		// If domain size pooling is enabled, take the element-wise average of the computed descriptors.
		if (domainSizePoolingNumberScales > 1u)
		{
			typename VlfeatSiftDescriptorContainer::SiftDescriptor& descriptor = computedDescriptors.emplace_back();
			const size_t numberElements = descriptor.descriptorLevels().front().size();

			for (size_t j = 0; j < numberElements; ++j)
			{
				float total = 0.0f;
				for (const auto& scaledDescriptor : scaledDescriptors)
				{
					total += scaledDescriptor.descriptorLevels().front()[j];
				}

				descriptor.descriptorLevels().front()[j] = total / domainSizePoolingNumberScales;
			}
		}
		else
		{
			ocean_assert(scaledDescriptors.size() == 1);
			computedDescriptors.emplace_back(scaledDescriptors.front());
		}

		// Apply RootSIFT.
		typename VlfeatSiftDescriptorContainer::SiftDescriptor& descriptor = computedDescriptors.back();
		float l1Norm = 0.0f;

		static_assert(VlfeatSiftDescriptorContainer::SiftDescriptor::levels() == 1, "Invalid level number!");

		for (const float& element : descriptor.descriptorLevels().front())
		{
			ocean_assert(element >= 0.0f);
			l1Norm += element;
		}

		if (!NumericF::isEqualEps(l1Norm))
		{
			for (float& element : descriptor.descriptorLevels().front())
			{
				element = NumericF::sqrt(element / l1Norm);
			}
		}
		else
		{
			ocean_assert(false && "Extracted a zero-length descriptor");
		}

#ifdef OCEAN_DEBUG
		float sqrLength = 0.0f;

		for (float& element : descriptor.descriptorLevels().front())
		{
			sqrLength += NumericF::sqr(element);
		}

		ocean_assert(NumericF::isWeakEqual(NumericF::sqrt(sqrLength), 1.0f));
#endif // OCEAN_DEBUG
	}

	descriptors = std::make_shared<VlfeatSiftDescriptorContainer>(std::move(computedDescriptors));

	return Result::SUCCESS;
}

bool VlfeatSiftDescriptorGenerator::isSupportedFeatureDetector(const FeatureDetector& featureDetector) const
{
	return featureDetector.featureCategory() == FeatureCategory::KEYPOINTS;
}

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
