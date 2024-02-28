// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/vlfeatsift/VlfeatSift.h"

#include "vl/covdet.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace VlfeatSift
{

/**
 * Definition of parameters for the VLFeat SIFT feature detector and descriptor generator
 * @ingroup unifiedfeaturesvlfeatsift
 */
class OCEAN_UNIFIEDFEATURES_VLFEAT_SIFT_EXPORT VlfeatSiftParameters final : public UnifiedFeatures::Parameters
{
	public:

		/**
		 * Default constructor
		 */
		VlfeatSiftParameters() = default;

		/// Base octave to use in the image pyramid, with 0 being the first octave.
		/// A value of -1 allow for 1 level of upsampling; values lower than this likely are not useful.
		int firstOctave = -1;

		/// Number of suboctaves per octave, with range [1, infinity).
		unsigned int octaveResolution = 3;

		/// Maximum number of features to detect.
		/// Note that the VLFeat implementation may return slightly more than this many features by extracting multiple orientations per feature.
		unsigned int maximumNumberFeatures = 3000u;

		/// Feature detection method.
		VlCovDetMethod detectionMethod = VL_COVDET_METHOD_DOG;

		/// Detection parameter. Whether to compute the affine shape for features.
		/// Affine features have potentially better resilience to perspective distortions, but they are more costly to compute.
		bool computeAffineShape = false;

		/// Detection parameter. If true, the orientation is fixed to zero for all features.
		bool upright = false;

		/// Detection parameter. Threshold on the Difference-of-Gaussian local maxima during detection, with range (0, infinity).
		double peakThreshold = 0.05 / 3.0;

		/// Detection parameter. Threshold on the Difference of Gaussian edge response during detection, with range (0, infinity).
		double edgeThreshold = 12.0;

		/// Description parameter. Whether to apply Domain Size Pooling.
		/// DSP calculates the average descriptor over a range of keypoint window sizes.
		/// It has shown to have better performance than SIFT, but it is much costlier to compute.
		/// See "Domain-Size Pooling in Local Descriptors and Network Architectures" by Dong and Soatto (CVPR 2015).
		bool useDomainSizePooling = false;

		/// Description parameter. Number of scales to use for Domain Size Pooling, with range (1, infinity).
		/// A value <=1 to 1 will implicitly disable DSP.
		unsigned int domainSizePoolingNumberScales = 10u;

		/// Description parameter. Minimum feature scale for Domain Size Pooling, with range (0, domainSizePoolingMaximumScale).
		double domainSizePoolingMinimumScale = 1.0 / 6.0;

		/// Description parameter. Maximum feature scale for Domain Size Pooling, with range (domainSizePoolingMinimumScale, infinity).
		double domainSizePoolingMaximumScale = 3.0;

		/// Minimum matching ratio between the distances of the first and second best matches, range: (0, infinity)
		float matchingRatio = 0.8f;

		/// Maximum distance that matches are allowed to have, range: (0, infinity)
		float maximumMatchingDistance = 0.75f;
};

} // namespace VlfeatSift

} // namespace UnifiedFeatures

} // namespace Ocean
