// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/DescriptorContainer.h"
#include "ocean/unifiedfeatures/FeatureContainer.h"
#include "ocean/unifiedfeatures/FeatureDetector.h"
#include "ocean/unifiedfeatures/Keypoint.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace Thrift
{

/// Forward declaration
class ContainerPair;

} // namespace Thrift

/**
 * This class is a collection of utitility functions for the unified feature interface.
 * @ingroup unifiedfeatures
 */
class OCEAN_UNIFIEDFEATURES_EXPORT Utilities
{
	public:

		/**
		 * Draws a single keypoint
		 * @param frame The frame into which the keypoint will be drawn, must be valid
		 * @param keypoint The keypoint that will be drawn
		 * @param offset Optional pixel offset that is applied to the keypoints
		 */
		static void drawKeypoint(Frame& frame, const Keypoint& keypoint, const Vector2& offset = Vector2(Scalar(0), Scalar(0)));

		/**
		 * Draws keypoints
		 * @param frame The frame into which the keypoints will be drawn, must be valid
		 * @param keypoints A pointer to the keypoints that will be drawn, must be valid and have at least `keypointSize` elements
		 * @param keypointsSize The number of keypoints that will be drawn, range: [0, infinity)
		 * @param offset Optional pixel offset that is applied to the keypoints
		 */
		static void drawKeypoints(Frame& frame, const Keypoint* keypoints, const size_t keypointsSize, const Vector2& offset = Vector2(Scalar(0), Scalar(0)));

		/**
		 * Draws a feature container
		 * This function will draw a feature container based on its feature type.
		 * @param frame The frame into which the feature container will be drawn
		 * @param featureContainer The feature container that will be drawn
		 * @sa FeatureContainer::draw()
		 */
		static void drawFeatureContainer(Frame& frame, const FeatureContainer& featureContainer);

		/**
		 * Draws a feature correspondences
		 * This function will draw the specified feature containers and the correspondences between them
		 * @param frame The frame into which the feature container will be drawn
		 * @param featureContainer0 The first feature container that will be drawn
		 * @param featureContainer1 The second feature container that will be drawn
		 * @param correspondences The correspondences between the first and the second feature containers
		 * @param offset0 Optional pixel offset that is applied to the keypoints of the first container
		 * @param offset1 Optional pixel offset that is applied to the keypoints of the second container
		 */
		static void drawFeatureCorrespondences(Frame& frame, const FeatureContainer& featureContainer0, const FeatureContainer& featureContainer1, const IndexPairs32& correspondences, const Vector2& offset0 = Vector2(Scalar(0), Scalar(0)), const Vector2& offset1 = Vector2(Scalar(0), Scalar(0)));

		/**
		 * Converts a feature and descriptor container into Thrift data structures
		 * @param featureContainer The feature container that will converted to Thrift
		 * @param descriptorContainer The descriptor container that will converted to Thrift
		 * @param thriftContainerPair The resulting Thrift container that will be filled
		 * @return True if the conversion was successful, otherwise false
		 */
		static bool toThrift(const FeatureContainer& featureContainer, const DescriptorContainer& descriptorContainer, Thrift::ContainerPair& thriftContainerPair);

		/**
		 * Converts a thrift data structure back into a feature and a descriptor container
		 * @param thriftContainerPair The Thrift data structure that will be used to fill the feature and descriptor containers
		 * @param featureContainer The resulting feature container, may be `nullptr` if the function doesn't return `true`
		 * @param descriptorContainer The resulting descriptor container, may be `nullptr` if the function doesn't return `true`
		 * @return True if the conversion was successful, otherwise false
		 */
		static bool fromThrift(const Thrift::ContainerPair& thriftContainerPair, std::shared_ptr<FeatureContainer>& featureContainer, std::shared_ptr<DescriptorContainer>& descriptorContainer);
};

} // namespace UnifiedFeatures

} // namespace Ocean
