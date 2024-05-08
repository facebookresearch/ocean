/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_MOTION_H
#define META_OCEAN_CV_MOTION_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/PixelPosition.h"
#include "ocean/cv/SumAbsoluteDifferences.h"
#include "ocean/cv/SumSquareDifferences.h"
#include "ocean/cv/ZeroMeanSumSquareDifferences.h"

#include "ocean/base/ShiftVector.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

// Forward declaration.
template <typename TMetric> class Motion;

/**
 * Definition of a Motion class that applies sum absolute difference calculations as metric.
 * @see MotionSSD, MotionZeroMeanSSD, Motion.
 * @ingroup cv
 */
typedef Motion<SumAbsoluteDifferences> MotionSAD;

/**
 * Definition of a Motion class that applies sum square difference calculations as metric.
 * @see MotionSAD, MotionZeroMeanSSD, Motion.
 * @ingroup cv
 */
typedef Motion<SumSquareDifferences> MotionSSD;

/**
 * Definition of a Motion class that applies zero-mean sum square difference calculations as metric.
 * @see MotionSAD, MotionSSD, Motion.
 * @ingroup cv
 */
typedef Motion<ZeroMeanSumSquareDifferences> MotionZeroMeanSSD;

/**
 * This class implements patch-based motion techniques.
 * @tparam TMetric The metric that is applied for measurements with pixel accuracy
 * @see MotionSAD, MotionSSD, MotionZeroMeanSSD.
 * @ingroup cv
 */
template <typename TMetric = SumSquareDifferences>
class Motion
{
	public:

		/**
		 * Tracks a set of given points between two frames with pixel accuracy.
		 * Actually, this function simply creates two frame pyramids and invokes the corresponding function needing frame pyramid as parameters.<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * If a point is near the frame border, a mirrored image patch is applied
		 * @param previousFrame The previous frame in which the previous points are located, must be valid
		 * @param currentFrame The current frame, with same pixel format and pixel orientation as the previous frame, must be valid
		 * @param previousPoints The points that are located in the previous frame (the points to be tracked from the previous frame to the current frame), with ranges [0, previousFrame.width())x[0, previousFrame.height())
		 * @param roughPoints Rough locations of the previous points in the current frame (if known), otherwise simply provide the previous points again, one for each previous point, with ranges [0, currentFrame.width())x[0, currentFrame.height())
		 * @param currentPoints Resulting current points, that have been tracked from the previous frame to the current frame, with ranges [0, currentFrame.width())x[0, currentFrame.height())
		 * @param maximalOffset Maximal expected offset between two corresponding points in pixel, defined in the domain of previous/current frame, with range [1, infinity)
		 * @param coarsestLayerRadiusX The search radius on the coarsest pyramid layer in horizontal direction, in pixel, with range [0, infinity)
		 * @param coarsestLayerRadiusY The search radius on the coarsest pyramid layer in vertical direction, in pixel, with range [0, infinity)
		 * @param downsamplingMode The down sampling mode that is applied to create the pyramid layers
		 * @param worker Optional worker object to distribute the computation
		 * @param metricResults Optional resulting matching quality of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the results do not matter
		 * @return True, if succeeded
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 */
		template <unsigned int tPatchSize>
		static bool trackPointsInPyramidMirroredBorder(const Frame& previousFrame, const Frame& currentFrame, const PixelPositions& previousPoints, const PixelPositions& roughPoints, PixelPositions& currentPoints, const unsigned int maximalOffset, const unsigned int coarsestLayerRadiusX, const unsigned int coarsestLayerRadiusY, const FramePyramid::DownsamplingMode downsamplingMode = FramePyramid::DM_FILTER_14641, Worker* worker = nullptr, std::vector<uint32_t>* metricResults = nullptr, std::vector<uint32_t>* metricIdentityResults = nullptr);

		/**
		 * Tracks a set of given points between two frame pyramids, with pixel accuracy.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.
		 * @param previousPyramid The frame pyramid of the previous frame in which the previous points are located, must be valid
		 * @param currentPyramid The frame pyramid of the current frame, with same pixel format and pixel origin as the previous frame pyramid, must be valid
		 * @param previousPoints The points that are located in the previous frame (the points to be tracked from the previous frame to the current frame), with ranges [0, previousPyramid.finestWidth())x[0, previousPyramid.finestHeight())
		 * @param roughPoints Rough locations of the previous points in the current frame (if known), otherwise simply provide the previous points again, one for each previous point, with ranges [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight())
		 * @param currentPoints Resulting current points, that have been tracked from the previous frame to the current frame, with ranges [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight())
		 * @param coarsestLayerRadiusX The search radius on the coarsest pyramid layer in horizontal direction, in pixel, with range [0, infinity)
		 * @param coarsestLayerRadiusY The search radius on the coarsest pyramid layer in vertical direction, in pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param metricResults Optional resulting matching quality of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the results do not matter
		 * @return True, if succeeded
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 */
		template <unsigned int tPatchSize>
		static bool trackPointsInPyramidMirroredBorder(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const PixelPositions& previousPoints, const PixelPositions& roughPoints, PixelPositions& currentPoints, const unsigned int coarsestLayerRadiusX, const unsigned int coarsestLayerRadiusY, Worker* worker = nullptr, std::vector<uint32_t>* metricResults = nullptr, std::vector<uint32_t>* metricIdentityResults = nullptr);

		/**
		 * Determines the motion for one given point between two frames by application of an image patch.
		 * Patch pixels outside the frame are mirrored into the frame before comparison.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, must be valid
		 * @param width0 Width of the first frame in pixel, with range [tPatchSize/2, infinity)
		 * @param height0 Height of the first frame in pixel, with range [tPatchSize/2, infinity)
		 * @param width1 Width of the second frame in pixel, with range [tPatchSize/2, infinity)
		 * @param height1 Height of the second frame in pixel, with range [tPatchSize/2, infinity)
		 * @param position0 The position in the first frame, with range [0, width - 1]x[0, height - 1]
		 * @param radiusX The search radius in horizontal direction, in pixel, with range [0, width - 1]
		 * @param radiusY The search radius in vertical direction, in pixel, with range [0, height - 1]
		 * @param frame0PaddingElements The number of padding elements at the end of each row of the first frame, in elements, with range [0, infinity)
		 * @param frame1PaddingElements The number of padding elements at the end of each row of the second frame, in elements, with range [0, infinity)
		 * @param rough1 The optional rough guess of the point in the second frame, an invalid position if unknown
		 * @param metricResult Optional resulting matching quality of the applied metric, nullptr if the result does not matter
		 * @param metricIdentityResult Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the result does not matter
		 * @return Best matching position in the second frame
		 * @tparam tChannels The number of frame channels, with range [1u, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static PixelPosition pointMotionInFrameMirroredBorder(const uint8_t* const frame0, const uint8_t* const frame1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const PixelPosition& position0, const unsigned int radiusX, const unsigned int radiusY, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const PixelPosition& rough1 = PixelPosition(), uint32_t* const metricResult = nullptr, uint32_t* const metricIdentityResult = nullptr);

		/**
		 * Determines the motion for one given point between two frames by application of an image patch.
		 * Patch pixels outside the frame are mirrored into the frame before comparison.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, must be valid
		 * @param channels The number of frame channels, with range [1, 4]
		 * @param width0 Width of the first frame in pixel, with range [tPatchSize/2, infinity)
		 * @param height0 Height of the first frame in pixel, with range [tPatchSize/2, infinity)
		 * @param width1 Width of the second frame in pixel, with range [tPatchSize/2, infinity)
		 * @param height1 Height of the second frame in pixel, with range [tPatchSize/2, infinity)
		 * @param position0 The position in the first frame, with range [0, width - 1]x[0, height - 1]
		 * @param radiusX The search radius in horizontal direction, in pixel, with range [0, width - 1]
		 * @param radiusY The search radius in vertical direction, in pixel, with range [0, height - 1]
		 * @param frame0PaddingElements The number of padding elements at the end of each row of the first frame, in elements, with range [0, infinity)
		 * @param frame1PaddingElements The number of padding elements at the end of each row of the second frame, in elements, with range [0, infinity)
		 * @param rough1 The optional rough guess of the point in the second frame, an invalid position if unknown
		 * @param metricResult Optional resulting matching quality of the applied metric, nullptr if the result does not matter
		 * @param metricIdentityResult Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the result does not matter
		 * @return Best matching position in the second frame
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 */
		template <unsigned int tPatchSize>
		static inline PixelPosition pointMotionInFrameMirroredBorder(const uint8_t* const frame0, const uint8_t* const frame1, const unsigned int channels, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const PixelPosition& position0, const unsigned int radiusX, const unsigned int radiusY, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const PixelPosition& rough1 = PixelPosition(), uint32_t* const metricResult = nullptr, uint32_t* const metricIdentityResult = nullptr);

	protected:

		/**
		 * Tracks a subset of given points between two frame pyramids.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * @param previousPyramid Previous frame pyramid
		 * @param currentPyramid Current frame pyramid, with same frame type as the previous frame
		 * @param numberLayers The number of pyramid layers that will be used for tracking, with range [1, min(pyramids->layers(), coarsest layer that match with the patch size)]
		 * @param previousPoints A set of points that are located in the previous frame
		 * @param roughPoints The rough points in the current frame (if known), otherwise the prevousPoints may be provided
		 * @param currentPoints Resulting current points, that have been tracking between the two points
		 * @param coarsestLayerRadiusX The search radius on the coarsest pyramid layer in horizontal direction, in pixel, with range [0, infinity)
		 * @param coarsestLayerRadiusY The search radius on the coarsest pyramid layer in vertical direction, in pixel, with range [0, infinity)
		 * @param metricResults Optional resulting results of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting results of the applied metric in both frames at the same previous position, nullptr if the results do not matter
		 * @param firstPoint The first point to be handled, with range [0, numberPoints - 1]
		 * @param numberPoints The number of points to handled, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 */
		template <unsigned int tPatchSize>
		static void trackPointsInPyramidMirroredBorderSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const unsigned int numberLayers, const PixelPositions* previousPoints, const PixelPositions* roughPoints, PixelPositions* currentPoints, const unsigned int coarsestLayerRadiusX, const unsigned int coarsestLayerRadiusY, uint32_t* metricResults, uint32_t* metricIdentityResults, const unsigned int firstPoint, const unsigned int numberPoints);
};

template <typename TMetric>
template <unsigned int tPatchSize>
bool Motion<TMetric>::trackPointsInPyramidMirroredBorder(const Frame& previousFrame, const Frame& currentFrame, const PixelPositions& previousPoints, const PixelPositions& roughPoints, PixelPositions& currentPoints, const unsigned int maximalOffset, const unsigned int coarsestLayerRadiusX, const unsigned int coarsestLayerRadiusY, const FramePyramid::DownsamplingMode downsamplingMode, Worker* worker, std::vector<uint32_t>* metricResults, std::vector<uint32_t>* metricIdentityResults)
{
	static_assert(tPatchSize % 2u == 1u, "Invalid image patch size, must be odd!");
	static_assert(tPatchSize >= 3u, "Invalid image patch size!");

	ocean_assert(previousFrame && currentFrame);

	ocean_assert(previousFrame.frameType().pixelFormat() == currentFrame.frameType().pixelFormat());
	ocean_assert(previousFrame.frameType().pixelOrigin() == currentFrame.frameType().pixelOrigin());

	ocean_assert(previousPoints.size() == roughPoints.size());

	const unsigned int idealLayers = FramePyramid::idealLayers(previousFrame.width(), previousFrame.height(), (tPatchSize / 2u) * 4u, (tPatchSize / 2u) * 4u, 2u, maximalOffset);

	if (idealLayers == 0u)
	{
		return false;
	}

	const FramePyramid previousPyramid(previousFrame, downsamplingMode, idealLayers, false /*copyFirstLayer*/, worker);
	const FramePyramid currentPyramid(currentFrame, downsamplingMode, idealLayers, false /*copyFirstLayer*/, worker);

	return trackPointsInPyramidMirroredBorder<tPatchSize>(previousPyramid, currentPyramid, previousPoints, roughPoints, currentPoints, coarsestLayerRadiusX, coarsestLayerRadiusY, worker, metricResults, metricIdentityResults);
}

template <typename TMetric>
template <unsigned int tPatchSize>
bool Motion<TMetric>::trackPointsInPyramidMirroredBorder(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const PixelPositions& previousPoints, const PixelPositions& roughPoints, PixelPositions& currentPoints, const unsigned int coarsestLayerRadiusX, const unsigned int coarsestLayerRadiusY, Worker* worker, std::vector<uint32_t>* metricResults, std::vector<uint32_t>* metricIdentityResults)
{
	static_assert(tPatchSize % 2u == 1u, "Invalid image patch size, must be odd!");
	static_assert(tPatchSize >= 3u, "Invalid image patch size, must be larger than 2!");

	ocean_assert(previousPyramid.frameType().pixelFormat() == currentPyramid.frameType().pixelFormat());
	ocean_assert(previousPyramid.frameType().pixelOrigin() == currentPyramid.frameType().pixelOrigin());

	const unsigned int idealLayers = CV::FramePyramid::idealLayers(previousPyramid.finestWidth(), previousPyramid.finestHeight(), (tPatchSize / 2u) * 4u, (tPatchSize / 2u) * 4u, 2u);
	const unsigned int numberLayers = std::min(std::min(previousPyramid.layers(), currentPyramid.layers()), idealLayers);

	if (numberLayers == 0u)
	{
		return false;
	}

	currentPoints.resize(previousPoints.size());

	if (metricResults)
	{
		metricResults->resize(previousPoints.size());
	}

	if (metricIdentityResults)
	{
		metricIdentityResults->resize(previousPoints.size());
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&Motion::trackPointsInPyramidMirroredBorderSubset<tPatchSize>, &previousPyramid, &currentPyramid, numberLayers, &previousPoints, &roughPoints, &currentPoints, coarsestLayerRadiusX, coarsestLayerRadiusY, metricResults ? metricResults->data() : nullptr, metricIdentityResults ? metricIdentityResults->data() : nullptr, 0u, 0u), 0u, (unsigned int)(previousPoints.size()));
	}
	else
	{
		trackPointsInPyramidMirroredBorderSubset<tPatchSize>(&previousPyramid, &currentPyramid, numberLayers, &previousPoints, &roughPoints, &currentPoints, coarsestLayerRadiusX, coarsestLayerRadiusY, metricResults ? metricResults->data() : nullptr, metricIdentityResults ? metricIdentityResults->data() : nullptr, 0u, (unsigned int)(previousPoints.size()));
	}

	return true;
}

template <typename TMetric>
template <unsigned int tChannels, unsigned int tPatchSize>
PixelPosition Motion<TMetric>::pointMotionInFrameMirroredBorder(const uint8_t* const frame0, const uint8_t* const frame1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const PixelPosition& position0, const unsigned int radiusX, const unsigned int radiusY, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const PixelPosition& rough1, uint32_t* const metricResult, uint32_t* const metricIdentityResult)
{
	static_assert(tChannels != 0u, "Invalid number of data channels!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size of the image patch, must be odd!");

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(frame0 != nullptr && frame1 != nullptr);
	ocean_assert(radiusX != 0u || radiusY != 0u);

	ocean_assert(width0 >= tPatchSize_2 && height0 >= tPatchSize_2);
	ocean_assert(width1 >= tPatchSize_2 && height1 >= tPatchSize_2);

	ocean_assert(position0.x() < width0);
	ocean_assert(position0.y() < height0);

	const PixelPosition position1((rough1 && rough1.x() < width1 && rough1.y() < height1) ? rough1 : position0);
	ocean_assert(position1.x() < width1);
	ocean_assert(position1.y() < height1);

	const unsigned int leftCenter1 = (unsigned int)(max(0, int(position1.x() - radiusX)));
	const unsigned int topCenter1 = (unsigned int)(max(0, int(position1.y() - radiusY)));

	const unsigned int rightCenter1 = min(position1.x() + radiusX, width1 - 1u);
	const unsigned int bottomCenter1 = min(position1.y() + radiusY, height1 - 1u);

	ocean_assert(leftCenter1 < width1 && leftCenter1 <= rightCenter1 && rightCenter1 < width1);
	ocean_assert(topCenter1 < height1 && topCenter1 <= bottomCenter1 && bottomCenter1 < height1);

	PixelPosition bestPosition;
	uint32_t bestMetric = uint32_t(-1);
	uint32_t bestSqrDistance = uint32_t(-1);

	// check whether position0 is entirely inside the frame (including the patch with size tPatchSize x tPatchSize)
	if ((position0.x() >= tPatchSize_2 && position0.y() >= tPatchSize_2 && position0.x() + tPatchSize_2 < width0 && position0.y() + tPatchSize_2 < height0))
	{
		for (unsigned int y1 = topCenter1; y1 <= bottomCenter1; ++y1)
		{
			for (unsigned int x1 = leftCenter1; x1 <= rightCenter1; ++x1)
			{
				// check whether we can use the fast metric function

				const uint32_t metric = (x1 - tPatchSize_2 < width1 - (tPatchSize - 1u) && y1 - tPatchSize_2 < height1 - (tPatchSize - 1u)) ?
									TMetric::template patch8BitPerChannel<tChannels, tPatchSize>(frame0, frame1, width0, width1, position0.x(), position0.y(), x1, y1, frame0PaddingElements, frame1PaddingElements) :
									TMetric::template patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(frame0, frame1, width0, height0, width1, height1, position0.x(), position0.y(), x1, y1, frame0PaddingElements, frame1PaddingElements);

				const PixelPosition position(x1, y1);

				if (metric < bestMetric || (metric == bestMetric && position1.sqrDistance(position) < bestSqrDistance))
				{
					bestMetric = metric;
					bestPosition = position;

					bestSqrDistance = position1.sqrDistance(position);
				}

				if (metricIdentityResult && x1 == position1.x() && y1 == position1.y())
				{
					*metricIdentityResult = metric;
				}
			}
		}
	}
	else
	{
		for (unsigned int y1 = topCenter1; y1 <= bottomCenter1; ++y1)
		{
			for (unsigned int x1 = leftCenter1; x1 <= rightCenter1; ++x1)
			{
				const uint32_t metric = TMetric::template patchMirroredBorder8BitPerChannel<tChannels, tPatchSize>(frame0, frame1, width0, height0, width1, height1, position0.x(), position0.y(), x1, y1, frame0PaddingElements, frame1PaddingElements);

				const PixelPosition position(x1, y1);

				if (metric < bestMetric || (metric == bestMetric && position1.sqrDistance(position) < bestSqrDistance))
				{
					bestMetric = metric;
					bestPosition = position;

					bestSqrDistance = position1.sqrDistance(position);
				}

				if (metricIdentityResult && x1 == position1.x() && y1 == position1.y())
				{
					*metricIdentityResult = metric;
				}
			}
		}
	}

	ocean_assert(bestMetric != uint32_t(-1) && bestPosition.isValid());

	if (metricResult)
	{
		*metricResult = bestMetric;
	}

	ocean_assert(abs(int(bestPosition.x()) - int(position1.x())) <= int(radiusX));
	ocean_assert(abs(int(bestPosition.y()) - int(position1.y())) <= int(radiusY));

	return bestPosition;
}

template <typename TMetric>
template <unsigned int tPatchSize>
inline PixelPosition Motion<TMetric>::pointMotionInFrameMirroredBorder(const uint8_t* const frame0, const uint8_t* const frame1, const unsigned int channels, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const PixelPosition& position0, const unsigned int radiusX, const unsigned int radiusY, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const PixelPosition& rough1, uint32_t* const metricResult, uint32_t* const metricIdentityResult)
{
	ocean_assert(channels >= 1u);

	switch (channels)
	{
		case 1u:
			return pointMotionInFrameMirroredBorder<1u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, position0, radiusX, radiusY, frame0PaddingElements, frame1PaddingElements, rough1, metricResult, metricIdentityResult);

		case 2u:
			return pointMotionInFrameMirroredBorder<2u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, position0, radiusX, radiusY, frame0PaddingElements, frame1PaddingElements, rough1, metricResult, metricIdentityResult);

		case 3u:
			return pointMotionInFrameMirroredBorder<3u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, position0, radiusX, radiusY, frame0PaddingElements, frame1PaddingElements, rough1, metricResult, metricIdentityResult);

		case 4u:
			return pointMotionInFrameMirroredBorder<4u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, position0, radiusX, radiusY, frame0PaddingElements, frame1PaddingElements, rough1, metricResult, metricIdentityResult);
	}

	ocean_assert(false && "Invalid pixel format!");
	return rough1;
}

template <typename TMetric>
template <unsigned int tPatchSize>
void Motion<TMetric>::trackPointsInPyramidMirroredBorderSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const unsigned int numberLayers, const PixelPositions* previousPoints, const PixelPositions* roughPoints, PixelPositions* currentPoints, const unsigned int coarsestLayerRadiusX, const unsigned int coarsestLayerRadiusY, uint32_t* metricResults, uint32_t* metricIdentityResults, const unsigned int firstPoint, const unsigned int numberPoints)
{
	static_assert(tPatchSize % 2u == 1u, "Invalid image patch size!");
	static_assert(tPatchSize >= 3u, "Invalid image patch size!");

	ocean_assert(previousPyramid && currentPyramid);
	ocean_assert(previousPoints && roughPoints && currentPoints);

	ocean_assert(*previousPyramid && *currentPyramid);

	ocean_assert(previousPyramid->frameType().pixelFormat() == currentPyramid->frameType().pixelFormat());
	ocean_assert(previousPyramid->frameType().pixelOrigin() == currentPyramid->frameType().pixelOrigin());

	ocean_assert(previousPyramid->layers() >= 1u && currentPyramid->layers() >= 1u);

	ocean_assert(previousPoints->size() == roughPoints->size());
	ocean_assert(currentPoints->size() == previousPoints->size());

	ocean_assert(coarsestLayerRadiusX != 0u || coarsestLayerRadiusY != 0u);

	ocean_assert(firstPoint + numberPoints <= previousPoints->size());

	ocean_assert(numberLayers >= 1u);
	ocean_assert(numberLayers <= previousPyramid->layers());
	ocean_assert(numberLayers <= currentPyramid->layers());

	ocean_assert(previousPyramid->layer(numberLayers - 1u).width() >= tPatchSize / 2u);
	ocean_assert(previousPyramid->layer(numberLayers - 1u).height() >= tPatchSize / 2u);

	ShiftVector<PixelPosition> intermediateRoughPoints(firstPoint, numberPoints);

	const unsigned int lowestCurrentWidth = currentPyramid->layer(numberLayers - 1u).width();
	const unsigned int lowestCurrentHeight = currentPyramid->layer(numberLayers - 1u).height();
	const unsigned int lowestLayerFactor = currentPyramid->sizeFactor(numberLayers - 1u);
	ocean_assert(lowestCurrentWidth >= 1u && lowestCurrentHeight >= 1u);

	const unsigned int channels = previousPyramid->frameType().channels();
	ocean_assert(channels >= 1u && channels <= 4u);

	for (unsigned int n = firstPoint; n < firstPoint + numberPoints; ++n)
	{
		const PixelPosition& roughPoint = (*roughPoints)[n];

		const unsigned int x = min((roughPoint.x() + lowestLayerFactor / 2u) / lowestLayerFactor, lowestCurrentWidth - 1u);
		const unsigned int y = min((roughPoint.y() + lowestLayerFactor / 2u) / lowestLayerFactor, lowestCurrentHeight - 1u);

		intermediateRoughPoints[n] = PixelPosition(x, y);
	}

	unsigned int layerRadiusX = coarsestLayerRadiusX;
	unsigned int layerRadiusY = coarsestLayerRadiusY;

	for (unsigned int layerIndex = numberLayers - 1u; layerIndex < numberLayers; --layerIndex)
	{
		const Frame& previousFrame = (*previousPyramid)[layerIndex];
		const Frame& currentFrame = (*currentPyramid)[layerIndex];

		const unsigned int previousWidth = previousFrame.width();
		const unsigned int previousHeight = previousFrame.height();

		const unsigned int currentWidth = currentFrame.width();
		const unsigned int currentHeight = currentFrame.height();

		for (unsigned int i = firstPoint; i < firstPoint + numberPoints; ++i)
		{
			ocean_assert(intermediateRoughPoints[i].x() < currentWidth && intermediateRoughPoints[i].y() < currentHeight);

			PixelPosition& intermediateRoughPoint = intermediateRoughPoints[i];

			uint32_t* const metricResult = metricResults ? metricResults + i : nullptr;
			uint32_t* const metricIdentityResult = (layerIndex == 0u && metricIdentityResults) ? metricIdentityResults + i : nullptr;

			ocean_assert(layerIndex < 31u);
			const unsigned int layerFactor = 1u << layerIndex;

			const PixelPosition previousPosition(min(((*previousPoints)[i].x() + layerFactor / 2u) / layerFactor, previousWidth - 1u), min(((*previousPoints)[i].y() + layerFactor / 2u) / layerFactor, previousHeight - 1u));

			if (previousPosition.x() < previousWidth && previousPosition.y() < previousHeight)
			{
				const PixelPosition position(pointMotionInFrameMirroredBorder<tPatchSize>(previousFrame.constdata<uint8_t>(), currentFrame.constdata<uint8_t>(), channels, previousWidth, previousHeight, currentWidth, currentHeight, previousPosition, layerRadiusX, layerRadiusY, previousFrame.paddingElements(), currentFrame.paddingElements(), intermediateRoughPoint, metricResult, metricIdentityResult));

				ocean_assert(position.x() < currentWidth && position.y() < currentHeight);

				// if we are back on the finest layer, than we store the tracked point as final result
				if (layerIndex == 0u)
				{
					ocean_assert(i < currentPoints->size());
					(*currentPoints)[i] = position;
				}
				else
				{
					// we store the tracked point as rough intermediate point for the next finer layer
					const unsigned int higherWidth = currentPyramid->layer(layerIndex - 1u).width();
					const unsigned int higherHeight = currentPyramid->layer(layerIndex - 1u).height();

					intermediateRoughPoint = PixelPosition(min(position.x() * 2u, higherWidth - 1u), min(position.y() * 2u, higherHeight - 1u));
				}
			}
			else
			{
				// if we are back on the finest layer, than we store the rough point as final result, otherwise we guess the rough intermediate point for the next layer
				if (layerIndex == 0u)
				{
					ocean_assert(i < currentPoints->size());
					(*currentPoints)[i] = intermediateRoughPoint;
				}
				else
				{
					intermediateRoughPoint = PixelPosition((intermediateRoughPoint.x() * 2u + layerFactor / 2u) / layerFactor, (intermediateRoughPoint.y() * 2u + layerFactor / 2u) / layerFactor);
				}
			}
		}

		// all layers expect the coarsest layer will apply a search radius of 2 pixels (a search region of 5x5)

		layerRadiusX = 2u;
		layerRadiusY = 2u;
	}
}

}

}

#endif // META_OCEAN_CV_MOTION_H
