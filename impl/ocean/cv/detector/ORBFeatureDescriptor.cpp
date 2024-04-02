// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/detector/ORBFeatureDescriptor.h"

#include "ocean/cv/detector/ORBSamplingPattern.h"
#include "ocean/cv/detector/ORBFeatureOrientation.h"
#include "ocean/cv/detector/FASTFeatureDetector.h"
#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/IntegralImage.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

void ORBFeatureDescriptor::detectReferenceFeaturesAndDetermineDescriptors(const FramePyramid& framePyramid, ORBFeatures& featurePoints, const bool useHarrisFeatures, const unsigned int featureThreshold, Worker* worker)
{
	ocean_assert(framePyramid.isValid());
	ocean_assert(featurePoints.empty());
	ocean_assert(featureThreshold <= 255u);

	// we acquire the integral image for the finest image resolution, and reuse it for lower image resolutions as well
	Frame linedIntegralFrame(FrameType(framePyramid.finestWidth() + 1u, framePyramid.finestHeight() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));

	for (unsigned int i = 0u; i < framePyramid.layers(); ++i)
	{
		const Frame layer(framePyramid[i], Frame::temporary_ACM_USE_KEEP_LAYOUT);
		ocean_assert(FrameType::formatIsGeneric(layer.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

		ORBFeatures orbFeatures;

		if (useHarrisFeatures)
		{
			HarrisCorners harrisCorners;
			HarrisCornerDetector::detectCorners(layer.constdata<uint8_t>(), layer.width(), layer.height(), layer.paddingElements(), featureThreshold, true, harrisCorners, true, worker);

			orbFeatures = ORBFeature::features2ORBFeatures(harrisCorners, layer.width(), layer.height());
		}
		else
		{
			FASTFeatures featurePointsFAST;
			FASTFeatureDetector::detectFeatures(layer.constdata<uint8_t>(), layer.width(), layer.height(), featureThreshold, true, true, featurePointsFAST, layer.paddingElements(), worker);

			orbFeatures = ORBFeature::features2ORBFeatures(featurePointsFAST, layer.width(), layer.height());
		}

		if (!orbFeatures.empty())
		{
			ocean_assert(linedIntegralFrame.isContinuous());

			uint32_t* const linedIntegralFrameData = linedIntegralFrame.data<uint32_t>();
			CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(layer.constdata<uint8_t>(), linedIntegralFrameData, layer.width(), layer.height(), layer.paddingElements(), linedIntegralFrame.paddingElements());

			ORBFeatureOrientation::determineFeatureOrientation(linedIntegralFrameData, layer.width(), layer.height(), orbFeatures, worker);

			determineDescriptors(linedIntegralFrameData, layer.width(), layer.height(), orbFeatures, true, worker);

			if (i != 0u)
			{
				const unsigned int factor = 1u << i;

				for (unsigned int j = 0u; j < orbFeatures.size(); ++j)
				{
					const Vector2 observation = orbFeatures[j].observation() + Vector2(Scalar(0.5), Scalar(0.5));
					orbFeatures[j].setObservation(observation * Scalar(factor), orbFeatures[j].distortionState());
				}
			}

			featurePoints.insert(featurePoints.end(), orbFeatures.begin(), orbFeatures.end());
		}
	}
}

void ORBFeatureDescriptor::determineDescriptorsSubset(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeature* featurePoints, const bool useSublayers, const unsigned int firstFeaturePoint, const unsigned int numberFeaturePoints)
{
	ocean_assert(linedIntegralFrame != nullptr && featurePoints != nullptr);
	ocean_assert(width > 0u && height > 0u);

	constexpr unsigned int linedIntegralFramePaddingElements = 0u;

	const ORBSamplingPattern& samplingPattern = ORBSamplingPattern::get();

	const size_t numberDescriptorBitset = sizeof(ORBDescriptor::DescriptorBitset) * 8;

	const unsigned int numberLayers = useSublayers ? 3u : 1u;

	const Scalar factors[3] = {Scalar(1), Scalar(0.7071067811865475244), Scalar(1.4142135623730950)}; // 1, 1/sqrt(2), sqrt(2)
	const unsigned int patchSizes[3] = {5u, 3u, 7u};

	for (unsigned int subLayer = 0u; subLayer < numberLayers; ++subLayer)
	{
		const Scalar factor = factors[subLayer];
		const unsigned int patchSize = patchSizes[subLayer];
		//const Scalar strengthThreshold = Scalar(patchSize * patchSize * 255u) * Scalar(0.05);

		for (unsigned int i = firstFeaturePoint; i < firstFeaturePoint + numberFeaturePoints; ++i)
		{
			const Scalar x = Scalar(featurePoints[i].observation().x() + Scalar(0.5));
			const Scalar y = Scalar(featurePoints[i].observation().y() + Scalar(0.5));
			const ORBSamplingPattern::IntensityComparisons& lookupTable = samplingPattern.samplingPatternByAngle(featurePoints[i].orientation());

			ORBDescriptor descriptor;
			//unsigned int strengthCounter = 0u;

			for (size_t j = 0u; j < numberDescriptorBitset; ++j)
			{
				const Scalar intensity1 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, Vector2(x + lookupTable[j].x1() * factor, y + lookupTable[j].y1() * factor), CV::PC_CENTER, patchSize, patchSize);
				const Scalar intensity2 = CV::FrameInterpolatorBilinear::patchIntensitySum1Channel(linedIntegralFrame, width, height, linedIntegralFramePaddingElements, Vector2(x + lookupTable[j].x2() * factor, y + lookupTable[j].y2() * factor), CV::PC_CENTER, patchSize, patchSize);

				if (intensity1 < intensity2)
				{
					descriptor[j] = 1;
				}

				/*if (Numeric::abs(intensity1 - intensity2) > strengthThreshold)
				{
					++strengthCounter;
				}*/
			}

			featurePoints[i].setDescriptorType(ORBFeature::FDT_ORIENTED);
			featurePoints[i].addDescriptor(descriptor);

			// feature strength is currently not used
			//featurePoints[i].setStrength(Scalar(strengthCounter) * Scalar(0.00390625)); // / 256
		}
	}
}

void ORBFeatureDescriptor::determineNonBijectiveCorrespondencesSubset(const ORBFeature* forwardFeatures, const size_t numberForwardFeatures, const ORBFeature* backwardFeatures, const size_t numberBackwardFeatures, const float threshold, IndexPairs32* correspondences, Lock* lock, const unsigned int firstIndex, const unsigned int numberIndices)
{
	ocean_assert(forwardFeatures != nullptr && backwardFeatures != nullptr);
	ocean_assert(numberForwardFeatures >= 1 && numberBackwardFeatures >= 1);

	ocean_assert_and_suppress_unused(firstIndex + numberIndices <= size_t(numberForwardFeatures), numberForwardFeatures);
	ocean_assert(threshold >= 0.0f && threshold <= 1.0f);

	ocean_assert(correspondences != nullptr);

	const size_t numberDescriptorBitset = sizeof(ORBDescriptor::DescriptorBitset) * 8;
	const unsigned maximalHammingDistance = (unsigned int)(float(numberDescriptorBitset) * threshold + 0.5f);

	IndexPairs32 localMatches;
	localMatches.reserve(numberIndices);

	for (unsigned int nForward = firstIndex; nForward < firstIndex + numberIndices; nForward++)
	{
		unsigned int bestMatchingBackwardFeature = (unsigned int)(-1);
		unsigned int bestHammingDistance = maximalHammingDistance + 1u;

		const ORBDescriptors& forwardDescriptors = forwardFeatures[nForward].descriptors();

		for (size_t nBackward = 0; nBackward < numberBackwardFeatures; ++nBackward)
		{
			const ORBDescriptors& backwardDescriptors = backwardFeatures[nBackward].descriptors();

			for (size_t nForwardDescriptor = 0; nForwardDescriptor < forwardDescriptors.size(); ++nForwardDescriptor)
			{
				const ORBDescriptor& forwardDescriptor = forwardDescriptors[nForwardDescriptor];

				for (size_t nBackwardDescriptor = 0; nBackwardDescriptor < backwardDescriptors.size(); ++nBackwardDescriptor)
				{
					const ORBDescriptor& backwardDescriptor = backwardDescriptors[nBackwardDescriptor];

					const unsigned int hammingDistance = calculateHammingDistance(forwardDescriptor, backwardDescriptor);

					if (hammingDistance < bestHammingDistance)
					{
						bestMatchingBackwardFeature = (unsigned int)nBackward;
						bestHammingDistance = hammingDistance;
					}
				}
			}
		}

		if (bestMatchingBackwardFeature != (unsigned int)(-1))
		{
			ocean_assert(bestHammingDistance <= maximalHammingDistance);
			localMatches.emplace_back(nForward, bestMatchingBackwardFeature);
		}
	}

	const OptionalScopedLock scopedLock(lock);
	correspondences->insert(correspondences->end(), localMatches.cbegin(), localMatches.cend());
}

}

}

}
