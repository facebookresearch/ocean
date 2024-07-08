/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_ORB_FEATURE_DESCRIPTOR_H
#define META_OCEAN_CV_DETECTOR_ORB_FEATURE_DESCRIPTOR_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/Descriptor.h"
#include "ocean/cv/detector/ORBFeature.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FramePyramid.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 30
	#include "ocean/cv/SSE.h"
#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	#include "ocean/cv/NEON.h"
#endif

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements all necessary functions for the ORB feature descriptor and matcher.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT ORBFeatureDescriptor
{
	public:

		/**
		 * Calculate the ORB descriptor for all given feature points.
		 * If sub layers are used, three descriptors are determined per feature. One for the unmodified frame size, one for a resizing factor of sqrt(2) and one for a resizing factor of 1/sqrt(2).
		 * @param linedIntegralFrame Pointer to the (lined) integral frame of the actual 8 bit frame for which the feature descriptors will be calculated, the integral frame has an extra line with zero pixels on the left and top of the integral frame
		 * @param width The width of the original frame in pixel (not the width of the lined-integral frame), with range [43, infinity)
		 * @param height The height of the original frame in pixel (not the height of the lined-integral frame), with range [43, infinity)
		 * @param featurePoints Feature points to calculate the descriptors for, with range [21, width - 22]x[21, height - 22] for 'useMultiLayers == false', with range [31, width - 32]x[31, height - 32] for 'useMultiLayers == true'
		 * @param useMultiLayers True, if two additional sub layers for descriptors are use, create three descriptors per feature point; otherwise, only one layer is used
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 */
		static inline void determineDescriptors(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeatures& featurePoints, const bool useMultiLayers = false, Worker* worker = nullptr);

		/**
		 * Detect feature points of the reference frame and calculate the ORB descriptors for them.
		 * Feature points are detect within individual pyramid layers separately, and for each of this feature points three descriptors are calculated.
		 * @param frame The reference frame for which the feature points will be determined, must be valid
		 * @param featurePoints The resulting feature points
		 * @param pyramidLayers The number of pyramid layers to use, with range [1, infinity)
		 * @param useHarrisFeatures True, to use Harris corners; False, to use FAST features
		 * @param featureThreshold The threshold to be used during feature detection, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 * @return True, if succeeded
		 */
		static inline bool detectReferenceFeaturesAndDetermineDescriptors(const Frame& frame, ORBFeatures& featurePoints, const unsigned int pyramidLayers, const bool useHarrisFeatures, const unsigned int featureThreshold, Worker* worker);

		/**
		 * Detect feature points in a reference frame and calculate the ORB descriptors for them.
		 * Feature points are detect within individual pyramid layers separately, and for each of this feature points three descriptors are calculated.
		 * @param framePyramid Frame pyramid of the reference frame
		 * @param featurePoints The resulting feature points
		 * @param useHarrisFeatures True, to use Harris corners; False, to use FAST features
		 * @param featureThreshold The threshold to be used during feature detection, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 */
		static void detectReferenceFeaturesAndDetermineDescriptors(const FramePyramid& framePyramid, ORBFeatures& featurePoints, const bool useHarrisFeatures, const unsigned int featureThreshold, Worker* worker);

		/**
		 * Determines feature correspondences - one backward feature point for each given forward feature point.
		 * @param forwardFeatures The forward feature points for which corresponding backward features will be determined, at least one
		 * @param backwardFeatures The backward feature points, at least one
		 * @param correspondences The resulting feature correspondences, pairs of feature indices (forward to backward indices)
		 * @param threshold The percentage (in relation to the number of descriptor bits) of the maximal hamming distance so that two descriptors count as similar, with range [0, 1]
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 */
		static inline void determineNonBijectiveCorrespondences(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, IndexPairs32& correspondences, const float threshold = 0.15f, Worker* worker = nullptr);

		/**
		 * Calculate the hamming distance between two feature descriptors (the number of different bits).
		 * @param referenceDescriptor Reference descriptor
		 * @param featureDescriptor Feature descriptor
		 * @return Hamming distance, with range [0, 256]
		 */
		static OCEAN_FORCE_INLINE unsigned int calculateHammingDistance(const ORBDescriptor& referenceDescriptor, const ORBDescriptor& featureDescriptor);

	protected:

		/**
		 * Calculate the ORB descriptor for all given feature points.
		 * If sub layers are used, three descriptors are determined per feature. One for the unmodified frame size, one for a resizing factor of sqrt(2) and one for a resizing factor of 1/sqrt(2).
		 * @param linedIntegralFrame Pointer to the (lined) integral frame of the actual 8 bit frame for which the feature descriptors will be calculated, the integral frame has an extra line with zero pixels on the left and top of the integral frame
		 * @param width The width of the original frame in pixel (not the width of the lined-integral frame), with range [43, infinity)
		 * @param height The height of the original frame in pixel (not the height of the lined-integral frame), with range [43, infinity)
		 * @param featurePoints Feature points to calculate the descriptors for, with range [21, width - 22]x[21, height - 22] for 'useMultiLayers == false', with range [31, width - 32]x[31, height - 32] for 'useMultiLayers == true'
		 * @param useMultiLayers True, if two additional sub layers for descriptors are use, create three descriptors per feature point; otherwise, only one layer is used
		 * @param firstFeaturePoint The first feature points to be handled, with range [0, size)
		 * @param numberFeaturePoints The number of feature points to be handled, with range [1, size]
		 */
		static void determineDescriptorsSubset(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeature* featurePoints, const bool useMultiLayers, const unsigned int firstFeaturePoint, const unsigned int numberFeaturePoints);

		/**
		 * Determines feature correspondences for a subset of forward feature points - one backward feature point for each given forward feature point.
		 * @param forwardFeatures The forward feature points for which corresponding backward features will be determined, must be valid
		 * @param numberForwardFeatures The number of forward feature points, with range [0, infinity)
		 * @param backwardFeatures The backward feature points, must be valid
		 * @param numberBackwardFeatures The number of backward feature points, with range [0, infinity)
		 * @param threshold The percentage (in relation to the number of descriptor bits) of the maximal hamming distance so that two descriptors count as similar, with range [0, 1]
		 * @param correspondences The resulting feature correspondences, pairs of feature indices (forward to backward indices)
		 * @param lock The lock for thread save adding of the match indices, nullptr if the function is executed single-threaded
		 * @param firstIndex First index of the feature point vector to be handled
		 * @param numberIndices Number of feature points to be handled
		 */
		static void determineNonBijectiveCorrespondencesSubset(const ORBFeature* forwardFeatures, const size_t numberForwardFeatures, const ORBFeature* backwardFeatures, const size_t numberBackwardFeatures, const float threshold, IndexPairs32* correspondences, Lock* lock, const unsigned int firstIndex, const unsigned int numberIndices);
};

inline void ORBFeatureDescriptor::determineDescriptors(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeatures& featurePoints, const bool useMultiLayers, Worker* worker)
{
	ocean_assert(linedIntegralFrame != nullptr);
	ocean_assert(width >= 43u && height >= 43u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&ORBFeatureDescriptor::determineDescriptorsSubset, linedIntegralFrame, width, height, featurePoints.data(), useMultiLayers, 0u, 0u), 0u, (unsigned int)(featurePoints.size()));
	}
	else
	{
		determineDescriptorsSubset(linedIntegralFrame, width, height, featurePoints.data(), useMultiLayers, 0u, (unsigned int)(featurePoints.size()));
	}
}

inline bool ORBFeatureDescriptor::detectReferenceFeaturesAndDetermineDescriptors(const Frame& frame, ORBFeatures& featurePoints, const unsigned int pyramidLayers, const bool useHarrisFeatures, const unsigned int featureThreshold, Worker* worker)
{
	Frame yFrame;
	if (!FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	const unsigned int maxLayerNumber = FramePyramid::idealLayers(frame.width(), frame.height(), 64u, 64u);

	if (maxLayerNumber == 0u)
	{
		return false;
	}

	const FramePyramid framePyramid(yFrame, std::min(pyramidLayers, maxLayerNumber), false /*copyFirstLayer*/, worker);

	if (!framePyramid.isValid())
	{
		return false;
	}

	detectReferenceFeaturesAndDetermineDescriptors(framePyramid, featurePoints, useHarrisFeatures, featureThreshold, worker);

	return true;
}

inline void ORBFeatureDescriptor::determineNonBijectiveCorrespondences(const ORBFeatures& forwardFeatures, const ORBFeatures& backwardFeatures, IndexPairs32& correspondences, const float threshold, Worker* worker)
{
	ocean_assert(threshold >= 0.0f && threshold <= 1.0f);

	correspondences.reserve(forwardFeatures.size());

	if (worker != nullptr)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&ORBFeatureDescriptor::determineNonBijectiveCorrespondencesSubset, forwardFeatures.data(), forwardFeatures.size(), backwardFeatures.data(), backwardFeatures.size(), threshold, &correspondences, &lock, 0u, 0u), 0u, (unsigned int)(forwardFeatures.size()));
	}
	else
	{
		determineNonBijectiveCorrespondencesSubset(forwardFeatures.data(), forwardFeatures.size(), backwardFeatures.data(), backwardFeatures.size(), threshold, &correspondences, nullptr, 0u, (unsigned int)(forwardFeatures.size()));
	}
}

OCEAN_FORCE_INLINE unsigned int ORBFeatureDescriptor::calculateHammingDistance(const ORBDescriptor& referenceDescriptor, const ORBDescriptor& featureDescriptor)
{
	static_assert(sizeof(ORBDescriptor::DescriptorBitset) == 256 / 8, "Invalid descriptor size!");

	return Descriptor::calculateHammingDistance<256>(&referenceDescriptor, &featureDescriptor);
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_ORB_FEATURE_DESCRIPTOR_H
