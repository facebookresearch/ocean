// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_MOTION_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_MOTION_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"
#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"
#include "ocean/cv/advanced/AdvancedZeroMeanSumSquareDifferences.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/Motion.h"
#include "ocean/cv/PixelPosition.h"
#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Box2.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

// Forward declaration.
template <typename TMetricInteger, typename TMetricFloat> class AdvancedMotion;

/**
 * Definition of an AdvancedMotion class that applies sum square difference calculations as metric.
 * @see AdvancedMotionZeroMeanSSD, AdvancedMotion.
 * @ingroup cvadvanced
 */
typedef AdvancedMotion<SumSquareDifferences, AdvancedSumSquareDifferences> AdvancedMotionSSD;

/**
 * Definition of an AdvancedMotion class that applies zero-mean sum square difference calculations as metric.
 * @see AdvancedMotionSSD, AdvancedMotion.
 * @ingroup cvadvanced
 */
typedef AdvancedMotion<ZeroMeanSumSquareDifferences, AdvancedZeroMeanSumSquareDifferences> AdvancedMotionZeroMeanSSD;

/**
 * This class implements advanced motion techniques (mainly with sub-pixel accuracy or binary masks) allowing to determine the motion (movement) of individual image points between two frames.
 * @tparam TMetricInteger The metric that is applied for measurements with pixel accuracy
 * @tparam TMetricFloat The metric that is applied for measurement with sub-pixel accuracy
 * @see AdvancedMotionSSD, AdvancedMotionZeroMeanSSD.
 * @ingroup cvadvanced
 */
template <typename TMetricInteger = SumSquareDifferences, typename TMetricFloat = AdvancedSumSquareDifferences>
class AdvancedMotion
{
	public:

		/**
		 * Definition of a vector holding metric results.
		 */
		typedef std::vector<uint32_t> MetricResults;

	public:

		/**
		 * Tracks a set of given points between two frames, with sub-pixel accuracy.
		 * Actually, this function simply creates two frame pyramids and invokes the corresponding function needing frame pyramid as parameters.<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * If a point is near the frame border, a mirrored image patch is applied.
		 * @param previousFrame The previous frame in which the previous points are located, must be valid
		 * @param currentFrame The current frame, with same pixel format and pixel orientation as the previous frame, must be valid
		 * @param previousPoints The points that are located in the previous frame (the points to be tracked from the previous frame to the current frame), with ranges [0, previousFrame.width())x[0, previousFrame.height())
		 * @param roughPoints Rough locations of the previous points in the current frame (if known), otherwise simply provide the previous points again, one for each previous point, with ranges [0, currentFrame.width())x[0, currentFrame.height())
		 * @param currentPoints Resulting current points, that have been tracked from the previous frame to the current frame, with ranges [0, currentFrame.width())x[0, currentFrame.height())
		 * @param maximalOffset Maximal expected offset between two corresponding points in pixel, defined in the domain of previous/current frame, with range [1, infinity)
		 * @param coarsestLayerRadius The search radius on the coarsest layer in pixel, with range [2, infinity)
		 * @param downsamplingMode The downsampling mode that is applied to create the pyramid layers
		 * @param subPixelIterations The number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param metricResults Optional resulting matching quality of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the results do not matter
		 * @return True, if succeeded
		 * @tparam tSize Size of the image patch that is used to determine the motion, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 * @see trackPointsSubPixelMask().
		 */
		template <unsigned int tSize>
		static bool trackPointsSubPixelMirroredBorder(const Frame& previousFrame, const Frame& currentFrame, const Vectors2& previousPoints, const Vectors2& roughPoints, Vectors2& currentPoints, const unsigned int maximalOffset, const unsigned int coarsestLayerRadius, const FramePyramid::DownsamplingMode downsamplingMode = FramePyramid::DM_FILTER_14641, const unsigned int subPixelIterations = 4u, Worker* worker = nullptr, MetricResults* metricResults = nullptr, MetricResults* metricIdentityResults = nullptr);

		/**
		 * Tracks a set of given points between two frame pyramids, with sub-pixel accuracy.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * This function can apply a larger search radius on the coarsest pyramid layer than on all other layers.<br>
		 * The search radius on the intermediate layers and on the finest layer is always 2.
		 * @param previousPyramid The frame pyramid of the previous frame in which the previous points are located, must be valid
		 * @param currentPyramid The frame pyramid of the current frame, with same pixel format and pixel origin as the previous frame pyramid, must be valid
		 * @param previousPoints The points that are located in the previous frame (the points to be tracked from the previous frame to the current frame), with ranges [0, previousPyramid.finestWidth())x[0, previousPyramid.finestHeight())
		 * @param roughPoints Rough locations of the previous points in the current frame (if known), otherwise simply provide the previous points again, one for each previous point, with ranges [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight())
		 * @param currentPoints Resulting current points, that have been tracked from the previous frame to the current frame, with ranges [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight())
		 * @param coarsestLayerRadius The search radius on the coarsest layer in pixel, with range [2, infinity)
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param metricResults Optional resulting matching quality of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the results do not matter
		 * @return True, if succeeded
		 * @tparam tSize Size of the image patch that is used to determine the motion, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 * @see trackPointsSubPixelMask().
		 */
		template <unsigned int tSize>
		inline static bool trackPointsSubPixelMirroredBorder(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const Vectors2& previousPoints, const Vectors2& roughPoints, Vectors2& currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations = 4u, Worker* worker = nullptr, MetricResults* metricResults = nullptr, MetricResults* metricIdentityResults = nullptr);

		/**
		 * Tracks a set of given points between two frame pyramids, with sub-pixel accuracy.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * This function can apply a larger search radius on the coarsest pyramid layer than on all other layers.<br>
		 * The search radius on the intermediate layers and on the finest layer is always 2.<br>
		 * This function has two template parameters: a) the number of frame channels and b) the patch size, both known at compile time.<br>
		 * @param previousPyramid The frame pyramid of the previous frame in which the previous points are located, with pixel format matching with the number of frame channels 'tChannels', must be valid
		 * @param currentPyramid The frame pyramid of the current frame, with same pixel format and pixel origin as the previous frame pyramid, must be valid
		 * @param previousPoints The points that are located in the previous frame (the points to be tracked from the previous frame to the current frame), with ranges [0, previousPyramid.finestWidth())x[0, previousPyramid.finestHeight())
		 * @param roughPoints Rough locations of the previous points in the current frame (if known), otherwise simply provide the previous points again, one for each previous point, with ranges [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight())
		 * @param currentPoints Resulting current points, that have been tracked from the previous frame to the current frame, with ranges [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight())
		 * @param coarsestLayerRadius The search radius on the coarsest layer in pixel, with range [2, infinity)
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param metricResults Optional resulting matching quality of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the results do not matter
		 * @return True, if succeeded
		 * @tparam tChannels The number of channels the frame pyramids have, with range [1, 4]
		 * @tparam tSize Size of the image patch that is used to determine the motion, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 * @see trackPointsSubPixelMask().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		inline static bool trackPointsSubPixelMirroredBorder(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const Vectors2& previousPoints, const Vectors2& roughPoints, Vectors2& currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations = 4u, Worker* worker = nullptr, MetricResults* metricResults = nullptr, MetricResults* metricIdentityResults = nullptr);

		/**
		 * Tracks a set of given points between two frame pyramids with pixel accuracy while each pyramid layer can contain valid and invalid pixels specified by two individual (pyramid) masks.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * Patch pixels outside the any of the frames will be treated as invalid mask pixels and thus will not be considered.
		 * @param previousPyramid The frame pyramid of the previous frame which will be used for the motion detection, must be valid
		 * @param currentPyramid The frame pyramid of the current frame which will be used for the motion detection, with same pixel origin as the first frame (pyramid), must be valid
		 * @param previousMaskPyramid The corresponding 8 bit mask pyramid of the previous frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the previous frame (pyramid), must be valid
		 * @param currentMaskPyramid The corresponding 8 bit mask pyramid of the current frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the current frame (pyramid), must be valid
		 * @param previousPoints The positions to be tracked in the current frame, with range [0, previousPyramid.finestWidth())x[0, previousPyramid.finestHeight())
		 * @param roughCurrentPoints Already known positions of the tracked points in the current frame (pyramid) one for each previous point, with range [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight()), zero locations if no rough location is known
		 * @param currentPoints The resulting positions of the tracked points located in the current frame, one for each previous point
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tSize The size of the square patch which is used for tracking in pixel, with range [1, min('finestWidth', 'finestHeight') * 2], must be odd
		 * @see trackPointsSubPixelMask().
		 */
		template <unsigned int tSize>
		inline static bool trackPointsMask(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const FramePyramid& previousMaskPyramid, const FramePyramid& currentMaskPyramid, const PixelPositions& previousPoints, const PixelPositions& roughCurrentPoints, PixelPositions& currentPoints, const unsigned int coarsestLayerRadius, Worker* worker = nullptr);

		/**
		 * Tracks a set of given points between two frame pyramids with sub-pixel accuracy while each pyramid layer can contain valid and invalid pixels specified by two individual (pyramid) masks.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * Patch pixels outside the any of the frames will be treated as invalid mask pixels and thus will not be considered.
		 * @param previousPyramid The frame pyramid of the previous frame which will be used for the motion detection, must be valid
		 * @param currentPyramid The frame pyramid of the current frame which will be used for the motion detection, with same pixel origin as the first frame (pyramid), must be valid
		 * @param previousMaskPyramid The corresponding 8 bit mask pyramid of the previous frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the previous frame (pyramid), must be valid
		 * @param currentMaskPyramid The corresponding 8 bit mask pyramid of the current frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the current frame (pyramid), must be valid
		 * @param previousPoints The positions to be tracked in the current frame, with range [0, previousPyramid.finestWidth())x[0, previousPyramid.finestHeight())
		 * @param roughCurrentPoints Already known positions of the tracked points in the current frame (pyramid) one for each previous point, with range [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight()), zero locations if no rough location is known
		 * @param currentPoints The resulting positions of the tracked points located in the current frame, one for each previous point
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tSize The size of the square patch which is used for tracking in pixel, with range [1, min('finestWidth', 'finestHeight') * 2], must be odd
		 * @return True, if succeeded
		 * @see trackPointsMask().
		 */
		template <unsigned int tSize>
		inline static bool trackPointsSubPixelMask(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const FramePyramid& previousMaskPyramid, const FramePyramid& currentMaskPyramid, const Vectors2& previousPoints, const Vectors2& roughCurrentPoints, Vectors2& currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations = 4u, Worker* worker = nullptr);

		/**
		 * Tracks a set of arbitrary (unknown) points between two frame pyramids with sub-pixel accuracy.
		 * An optional sub-region can be specified shrinking the tracking area.<br>
		 * Further, the arbitrary points can be separated into individual bin arrays allowing to distribute the points over the image area.<br>
		 * The points are tracked bidirectional, thus the points are tracked from the previous to the current and from the current to the previous frame.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * The number of pyramid layers on which points are tracked can be specified. Both pyramids need to contain at least this number of layers.
		 * If a point is near the frame border, a mirrored image patch is applied.
		 * @param previousPyramid Previous frame pyramid
		 * @param nextPyramid Next frame pyramid, with same frame type as the previous frame
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousImagePoints Resulting set of points that are located in the previous frame
		 * @param nextImagePoints Resulting points in the next frame, each point corresponds to one previous point (by index)
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param previousSubRegion An optional sub-region in that the points are tracked only
		 * @param horizontalBins Optional horizontal bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param verticalBins Optional vertical bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param strength Minimal strength parameter of the tracked feature points
		 * @param worker Optional worker object to distribute the computation
		 * @param trackingLayers Number of pyramid layers on which points are tracked
		 * @return True, if succeeded
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 */
		template <unsigned int tSize>
		static bool trackArbitraryPointsBidirectionalSubPixelMirroredBorder(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError = Scalar(0.9 * 0.9), const SubRegion& previousSubRegion = SubRegion(), const unsigned int horizontalBins = 0u, const unsigned int verticalBins = 0u, const unsigned int strength = 30u, Worker* worker = nullptr, const unsigned int trackingLayers = 1u);

		/**
		 * Tracks a set of arbitrary (unknown) points between two frame pyramids with sub-pixel accuracy.
		 * An optional sub-region can be specified shrinking the tracking area.<br>
		 * Further, the arbitrary points can be separated into individual bin arrays allowing to distribute the points over the image area.<br>
		 * The points are tracked bidirectional, thus the points are tracked from the previous to the current and from the current to the previous frame.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * The number of pyramid layers on which points are tracked can be specified.
		 * @param previousFrame Previous frame
		 * @param nextFrame Next frame, with same frame type as the previous frame
		 * @param maximalOffset Maximal offset between two corresponding points, in pixel
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousImagePoints Resulting set of points that are located in the previous frame
		 * @param nextImagePoints Resulting points in the next frame, each point corresponds to one previous point (by index)
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param previousSubRegion An optional sub-region in that the points are tracked only
		 * @param horizontalBins Optional horizontal bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param verticalBins Optional vertical bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param strength Minimal strength parameter of the tracked feature points
		 * @param downsamplingMode The downsampling mode that is applied to create the pyramid layers
		 * @param worker Optional worker object to distribute the computation
		 * @param trackingLayers Number of pyramid layers on which points are tracked
		 * @return True, if succeeded
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 */
		template <unsigned int tSize>
		static bool trackArbitraryPointsBidirectionalSubPixelMirroredBorder(const Frame& previousFrame, const Frame& nextFrame, const unsigned int maximalOffset, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError = Scalar(0.9 * 0.9), const SubRegion& previousSubRegion = SubRegion(), const unsigned int horizontalBins = 0u, const unsigned int verticalBins = 0u, const unsigned int strength = 30u, const FramePyramid::DownsamplingMode downsamplingMode = FramePyramid::DM_FILTER_14641, Worker* worker = nullptr, const unsigned int trackingLayers = 1u);

		/**
		 * Tracks a set of given points between two frame pyramids with sub-pixel accuracy.
		 * The points are tracked bidirectional, thus the points are tracked from the previous pyramid to the next pyramid and from the next pyramid back to the previous pyraimd.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.
		 * @param previousPyramid The previous frame pyramid, must be valid
		 * @param nextPyramid The next frame pyramid, with same pixel format and pixel origin as the previous pyramid, must be valid
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousImagePoints A set of points that are located in the previous frame, this set may be reduced as some points may be discarded (if no validIndices parameter is specified)
		 * @param nextImagePoints Resulting points in the next frame, each point corresponds to one previous point
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param worker Optional worker object to distribute the computation
		 * @param validIndices Optional vector receiving the indices of the valid point correspondences. Beware: If this parameter is defined, the resulting previousPoints and nextImagePoints have to be explicitly filtered by application of the indices
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 * @see trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations().
		 */
		template <unsigned int tSize>
		static bool trackPointsBidirectionalSubPixelMirroredBorder(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError = Scalar(0.9 * 0.9), Worker* worker = nullptr, Indices32* validIndices = nullptr, const unsigned int subPixelIterations = 4u);

		/**
		 * Tracks a set of given points between two frame pyramids with sub-pixel accuracy.
		 * The points are tracked bidirectional, thus the points are tracked from the previous to the current and from the current to the previous frame.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * This function has two template parameters: a) the number of frame channels and b) the patch size, both known at compile time.
		 * @param previousPyramid Previous frame pyramid with pixel format matching with the number of frame channels 'tChannels', must be valid
		 * @param nextPyramid Next frame pyramid, with same frame type as the previous frame, with same frame type as the previous pyramid
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousImagePoints A set of points that are located in the previous frame, this set may be reduced as some points may be discarded (if no validIndices parameter is specified)
		 * @param nextImagePoints Resulting points in the next frame, each point corresponds to one previous point
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param worker Optional worker object to distribute the computation
		 * @param validIndices Optional vector receiving the indices of the valid point correspondences. Beware: If this parameter is defined, the resulting previousPoints and nextImagePoints have to be explicitly filtered by application of the indices
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of channels both frame pyramids have, with range [1, 4]
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 * @see trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static bool trackPointsBidirectionalSubPixelMirroredBorder(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError = Scalar(0.9 * 0.9), Worker* worker = nullptr, Indices32* validIndices = nullptr, const unsigned int subPixelIterations = 4u);

		/**
		 * Tracks a set of given points between two frame pyramids with sub-pixel accuracy.
		 * The points are tracked bidirectional, thus the points are tracked from the previous to the current and from the current to the previous frame.<br>
		 * This function uses a rough guess for the next image points allowing to improve the tracking quality as long as the rough guess is better than no guess.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.
		 * @param previousPyramid Previous frame pyramid, must be valid
		 * @param nextPyramid Next frame pyramid, with same pixel format and pixel orientation as the previous frame, must be valid
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousImagePoints A set of points that are located in the previous frame, this set may be reduced as some points may be discarded (if no validIndices parameter is specified)
		 * @param roughNextImagePoints The rough locations of the previous image points in the current next frame, one for each previous image point
		 * @param nextImagePoints Resulting points in the next frame, each point corresponds to one previous point
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param worker Optional worker object to distribute the computation
		 * @param validIndices Optional vector receiving the indices of the valid point correspondences. Beware: If this parameter is defined, the resulting previousPoints and nextImagePoints have to be explicitly filtered by application of the indices
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 * @see trackPointsBidirectionalSubPixelMirroredBorder().
		 */
		template <unsigned int tSize>
		static bool trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, const Vectors2& roughNextImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError = Scalar(0.9 * 0.9), Worker* worker = nullptr, Indices32* validIndices = nullptr, const unsigned int subPixelIterations = 4u);

		/**
		 * Tracks a set of given points between two frame pyramids with sub-pixel accuracy.
		 * The points are tracked bidirectional, thus the points are tracked from the previous to the current and from the current to the previous frame.<br>
		 * This function uses a rough guess for the next image points allowing to improve the tracking quality as long as the rough guess is better than no guess.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * This function has two template parameters: a) the number of frame channels and b) the patch size, both known at compile time.
		 * @param previousPyramid Previous frame pyramid with pixel format matching with the number of frame channels 'tChannels', must be valid
		 * @param nextPyramid Next frame pyramid, with same pixel format and pixel orientation as the previous frame, must be valid
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousImagePoints A set of points that are located in the previous frame, this set may be reduced as some points may be discarded (if no validIndices parameter is specified)
		 * @param roughNextImagePoints The rough locations of the previous image points in the current next frame, one for each previous image point
		 * @param nextImagePoints Resulting points in the next frame, each point corresponds to one previous point
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param worker Optional worker object to distribute the computation
		 * @param validIndices Optional vector receiving the indices of the valid point correspondences. Beware: If this parameter is defined, the resulting previousPoints and nextImagePoints have to be explicitly filtered by application of the indices
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of channels both frame pyramids have, with range [1, 4]
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 * @see trackPointsBidirectionalSubPixelMirroredBorder().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static bool trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, const Vectors2& roughNextImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError = Scalar(0.9 * 0.9), Worker* worker = nullptr, Indices32* validIndices = nullptr, const unsigned int subPixelIterations = 4u);

		/**
		 * Tracks a set of given points between two frames with sub-pixel accuracy.
		 * The points are tracked bidirectional, thus the points are tracked from the previous frame to the next frame and from the next frame back to the previous frame.<br>
		 * Point correspondences with an inaccurate bidirectional tracking are discarded.<br>
		 * If a point is near the frame border, a mirrored image patch is applied.
		 * @param previousFrame The previous frame, must be valid
		 * @param nextFrame The next frame, with same pixel format and pixel origin as the previous frame, must be valid
		 * @param maximalOffset Maximal offset between two corresponding points, in pixel
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param previousImagePoints A set of points that are located in the previous frame, this set may be reduced as some points may be discarded
		 * @param nextImagePoints Resulting points in the next frame, each point corresponds to one previous point
		 * @param maximalSqrError Maximal square distance between forward and backward tracking for a valid point
		 * @param worker Optional worker object to distribute the computation
		 * @param downsamplingMode The downsampling mode that is applied to create the pyramid layers
		 * @param validIndices Optional vector receiving the indices of the valid point correspondences
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @return True, if succeeded
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 */
		template <unsigned int tSize>
		static bool trackPointsBidirectionalSubPixelMirroredBorder(const Frame& previousFrame, const Frame& nextFrame, const unsigned int maximalOffset, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError = Scalar(0.9 * 0.9), const FramePyramid::DownsamplingMode downsamplingMode = FramePyramid::DM_FILTER_14641, Worker* worker = nullptr, Indices32* validIndices = nullptr, const unsigned int subPixelIterations = 4u);

		/**
		 * Detects and tracks reliable arbitrary reference points between two frames.
		 * The reference points are distributed into an array to receive wide spread points.
		 * @param previousPyramid Frame pyramid of the previous image
		 * @param currentPyramid Frame pyramid of the current image
		 * @param previousReferencePoints Resulting reliable reference points inside the previous frame
		 * @param currentReferencePoints Resulting and tracked reliable reference points inside the current frame
		 * @param horizontalBins Number of horizontal bins to subdivide the specified bounding box
		 * @param verticalBins Number of vertical bins to subdivide the specified bounding box
		 * @param boundingBox Optional bounding box to shrink the search area of reliable tracking points
		 * @param maskFrame Optional mask frame allowing to use only reference points outside a given mask
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tSize Size of the tracking patch in pixels
		 */
		template <unsigned int tSize>
		static bool trackReliableReferencePoints(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, Vectors2& previousReferencePoints, Vectors2& currentReferencePoints, const unsigned int horizontalBins = 16u, const unsigned int verticalBins = 16u, const PixelBoundingBox& boundingBox = PixelBoundingBox(), const Frame& maskFrame = Frame(), Worker* worker = nullptr);

		/**
		 * Determines the motion for one given point between two frames with sub-pixel accuracy by application of an image patch.
		 * Patch pixels outside the frame are mirrored into the frame before compared.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, must be valid
		 * @param width0 Width of the first frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param height0 Height of the first frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param width1 Width of the second frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param height1 Height of the second frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param frame0PaddingElements The number of padding elements at the end of each first frame row, in elements, with range [0, infinity)
		 * @param frame1PaddingElements The number of padding elements at the end of each second frame row, in elements, with range [0, infinity)
		 * @param position0 Position in the first frame, with range [0, width)x[0, height)
		 * @param radiusX The search radius in horizontal direction, in pixel, with range [0, width - 1]
		 * @param radiusY The search radius in vertical direction, in pixel, with range [0, height - 1]
		 * @param rough1 The optional rough guess of the point in the second frame, (Numeric::maxValue(), Numeric::maxValue()) if unknown
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param metricResult Optional resulting matching quality of the applied metric, nullptr if the result does not matter
		 * @param metricIdentityResult Optional resulting matching quality of the applied metric between both frames at the identity location (the previous and rough position), nullptr if the result does not matter
		 * @return Best matching position in the second frame
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static Vector2 pointMotionInFrameSubPixelMirroredBorder(const uint8_t* frame0, const uint8_t* frame1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const Vector2& position0, const unsigned int radiusX, const unsigned int radiusY, const Vector2& rough1 = Vector2(Numeric::maxValue(), Numeric::maxValue()), const unsigned int subPixelIterations = 4u, uint32_t* metricResult = nullptr, uint32_t* metricIdentityResult = nullptr);

		/**
		 * Determines the motion for one given point between two frames with pixel accuracy while each frame can contain valid and invalid pixels specified by two individual masks.
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * Patch pixels outside the any of the frames will be treated as invalid mask pixels and thus will not be considered.
		 * @param frame0 The first frame which will be used for the motion detection, must be valid
		 * @param frame1 The second frame which will be used for the motion detection, with same pixel origin as the first frame, must be valid
		 * @param mask0 The corresponding 8 bit mask of the first frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same frame dimension and pixel origin as the first frame, must be valid
		 * @param mask1 The corresponding 8 bit mask of the second frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same frame dimension and pixel origin as the second frame, must be valid
		 * @param width0 The width of the first frame in pixel, with range [1, infinity)
		 * @param height0 The height of the first frame in pixel, with range [1, infinity)
		 * @param width1 The width of the second frame in pixel, with range [1, infinity)
		 * @param height1 The height of the second frame in pixel, with range [1, infinity)
		 * @param position0 The position of the point to be tracked in the first frame, with range [0, width0)x[0, height0)
		 * @param radius The search radius in pixel, with range [1, infinity)
		 * @param rough1 The already rough known position of the resulting point in the second frame, with range [0, width1)x[0, height1), or and invalid position if now rough guess exists
		 * @return The resulting location of the provided point in the second frame
		 * @tparam tChannels The number of data channel each frame has, with range [1, infinity)
		 * @tparam tSize The size of the square patch which is used for tracking in pixel, with range [1, min(min(width0, width1), min(height0, height1)) * 2], must be odd
		 * @see pointMotionSubPixelMask().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static PixelPosition pointMotionMask(const unsigned char* frame0, const unsigned char* frame1, const unsigned char* mask0, const unsigned char* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const PixelPosition& position0, const unsigned int radius, const PixelPosition& rough1 = PixelPosition());

		/**
		 * Determines the motion for one given point between two frames with sub-pixel accuracy while each frame can contain valid and invalid pixels specified by two individual masks.
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * Patch pixels outside any of the frames will be treated as invalid mask pixels and thus will not be considered.
		 * @param frame0 The first frame which will be used for the motion detection, must be valid
		 * @param frame1 The second frame which will be used for the motion detection, with same pixel origin as the first frame, must be valid
		 * @param mask0 The corresponding 8 bit mask of the first frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same frame dimension and pixel origin as the first frame, must be valid
		 * @param mask1 The corresponding 8 bit mask of the second frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same frame dimension and pixel origin as the second frame, must be valid
		 * @param width0 The width of the first frame in pixel, with range [1, infinity)
		 * @param height0 The height of the first frame in pixel, with range [1, infinity)
		 * @param width1 The width of the second frame in pixel, with range [1, infinity)
		 * @param height1 The height of the second frame in pixel, with range [1, infinity)
		 * @param position0 The position of the point to be tracked in the first frame, with range [0, width0)x[0, height0)
		 * @param radiusX The search radius in horizontal direction, in pixel, with range [0, infinity)
		 * @param radiusY The search radius in vertical direction, in pixel, with range [0, infinity)
		 * @param rough1 The already rough known position of the resulting point in the second frame, with range [0, width1)x[0, height1), or and invalid position if now rough guess exists
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @return The resulting location of the provided point in the second frame
		 * @tparam tChannels The number of data channel each frame has, with range [1, infinity)
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 * @see pointMotionMask().
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static Vector2 pointMotionInFrameSubPixelMask(const uint8_t* const frame0, const uint8_t* const frame1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const Vector2& position0, const unsigned int radiusX, const unsigned int radiusY, const Vector2& rough1 = Vector2(Numeric::maxValue(), Numeric::maxValue()), const unsigned int subPixelIterations = 4u);

	private:

		/**
		 * Determines the motion for one given point between two frames with sub-pixel accuracy by application of an image patch.
		 * Patch pixels outside the frame are mirrored into the frame before compared.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, must be valid
		 * @param channels The number of frame channels, with range [1, 4]
		 * @param width0 Width of the first frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param height0 Height of the first frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param width1 Width of the second frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param height1 Height of the second frame in pixel, with range [tPatchSize / 2u, infinity)
		 * @param frame0PaddingElements The number of padding elements at the end of each first frame row, in elements, with range [0, infinity)
		 * @param frame1PaddingElements The number of padding elements at the end of each second frame row, in elements, with range [0, infinity)
		 * @param position0 Position in the first frame, with range [0, width)x[0, height)
		 * @param radiusX The search radius in horizontal direction, in pixel, with range [0, width - 1]
		 * @param radiusY The search radius in vertical direction, in pixel, with range [0, height - 1]
		 * @param rough1 The optional rough guess of the point in the second frame, (Numeric::maxValue(), Numeric::maxValue()) if unknown
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param metricResult Optional resulting result of the applied metric, nullptr if the result does not matter
		 * @param metricIdentityResult Optional resulting result of the applied metric in both frames at the same previous position, nullptr if the results do not matter
		 * @return Best matching position in the second frame
		 * @tparam tPatchSize The size of the square patch (the edge length) in pixel, with range [3, infinity), must be odd, recommended is 5, 7, 15, 31, or 63
		 */
		template <unsigned int tPatchSize>
		inline static Vector2 pointMotionInFrameSubPixelMirroredBorder(const uint8_t* frame0, const uint8_t* frame1, const unsigned int channels, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const Vector2& position0, const unsigned int radiusX, const unsigned int radiusY, const Vector2& rough1 = Vector2(Numeric::maxValue(), Numeric::maxValue()), const unsigned int subPixelIterations = 4u, uint32_t* metricResult = nullptr, uint32_t* metricIdentityResult = nullptr);

		/**
		 * Determines the motion for one given point between two frames by application of an image patch.
		 * Patch pixels outside the frame are mirrored into the frame before compared.
		 * @param buffer0 The buffer containing the first image patch as one continuous memory block, must be valid
		 * @param frame1 The second image, with same pixel format as the buffer, must be valid
		 * @param width1 Width of the second frame in pixel, with range [tSize, infinity)
		 * @param height1 Height of the second frame in pixel, with range [tSize, infinity)
		 * @param frame1PaddingElements The number of padding elements at the end of each second image row, in elements, with range [0, infinity)
		 * @param position1 Rough position in the second frame
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param metricResult Optional resulting result of the applied metric, nullptr if the result does not matter
		 * @return Best matching position in the second frame
		 * @tparam tChannels Number of channels the frame holds
		 * @tparam tSize Size of the image patch that is used to determine the motion, must be odd
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static Vector2 pointMotionSubPixelMirroredBorder(const uint8_t* buffer0, const uint8_t* frame1, const unsigned int width1, const unsigned int height1, const unsigned int frame1PaddingElements, const Vector2& position1, const unsigned int subPixelIterations, unsigned int* metricResult = nullptr);

		/**
		 * Tracks a subset of given points between two frame pyramids with sub-pixel accuracy.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * If a point is near the frame border, a mirrored image patch is applied.
		 * @param previousPyramid The previous frame pyramid, must be valid
		 * @param nextPyramid The next frame pyramid, with same pixel format and pixel orientation as the previous frame pyramid, must be valid
		 * @param numberLayers The number of pyramid layers that will be used for tracking, with range [1, min(pyramids->validLayers(), 'coarsest layer that match with the patch size')]
		 * @param previousPoints A set of points that are located in the previous frame
		 * @param roughNextPoints The rough points located in the finest pyramid layer of the next pyramid (if known), nullptr if unknown
		 * @param nextPoints Resulting tracked points located in the finest layer of the next pyramid
		 * @param coarsestLayerRadius The search radius on the coarsest pyramid layer, with range [2, infinity)
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param metricResults Optional resulting results of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting results of the applied metric in both frames at the same previous position, nullptr if the results do not matter
		 * @param firstPoint First point to be applied
		 * @param numberPoints Number of points to be applied
		 * @tparam tSize The size of the applied image patches in pixel, with range [1, infinity), must be odd
		 */
		template <unsigned int tSize>
		static void trackPointsSubPixelMirroredBorderSubset(const FramePyramid* previousPyramid, const FramePyramid* nextPyramid, const unsigned int numberLayers, const Vectors2* previousPoints, const Vectors2* roughNextPoints, Vectors2* nextPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, unsigned int* metricResults, unsigned int* metricIdentityResults, const unsigned int firstPoint, const unsigned int numberPoints);

		/**
		 * Tracks a subset of given points between two frame pyramids with sub-pixel accuracy.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * If a point is near the frame border, a mirrored image patch is applied.<br>
		 * This function has two template parameters: a) the number of frame channels and b) the patch size, both known at compile time.<br>
		 * @param previousPyramid Previous frame pyramid, must be valid
		 * @param currentPyramid Current frame pyramid, with same pixel format and pixel orientation as the previous frame pyramid, must be valid
		 * @param numberLayers The number of pyramid layers that will be used for tracking, with range [1, min(pyramids->validLayers(), 'coarsest layer that match with the patch size')]
		 * @param previousPoints A set of points that are located in the previous frame
		 * @param roughPoints The rough points in the current frame (if known), otherwise the prevousPoints may be provided
		 * @param currentPoints Resulting current points, that have been tracking between the two points
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param metricResults Optional resulting results of the applied metric, nullptr if the results do not matter
		 * @param metricIdentityResults Optional resulting results of the applied metric in both frames at the same previous position, nullptr if the results do not matter
		 * @param firstPoint First point to be applied
		 * @param numberPoints Number of points to be applied
		 * @tparam tChannels The number of channels both frame pyramids have, with range [1, 4]
		 * @tparam tSize Defines the size of the applied image patches in pixel, must be odd
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void trackPointsSubPixelMirroredBorderSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const unsigned int numberLayers, const Vectors2* previousPoints, const Vectors2* roughPoints, Vectors2* currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, unsigned int* metricResults, unsigned int* metricIdentityResults, const unsigned int firstPoint, const unsigned int numberPoints);


		/**
		 * Tracks a subset of a given set of points between two frame pyramids with pixel accuracy while each pyramid layer can contain valid and invalid pixels specified by two individual (pyramid) masks.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * Patch pixels outside the any of the frames will be treated as invalid mask pixels and thus will not be considered.
		 * @param previousPyramid The frame pyramid of the previous frame which will be used for the motion detection, must be valid
		 * @param currentPyramid The frame pyramid of the current frame which will be used for the motion detection, with same pixel origin as the first frame (pyramid), must be valid
		 * @param previousMaskPyramid The corresponding 8 bit mask pyramid of the previous frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the previous frame (pyramid), must be valid
		 * @param currentMaskPyramid The corresponding 8 bit mask pyramid of the current frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the current frame (pyramid), must be valid
		 * @param numberLayers The number of pyramid layers that will be used for tracking, with range [1, min('validLayers', 'coarsest layer that match with the patch size')]
		 * @param previousPoints The positions to be tracked in the current frame, with range [0, previousPyramid.finestWidth())x[0, previousPyramid.finestHeight())
		 * @param roughCurrentPoints Already known positions of the tracked points in the current frame (pyramid) one for each previous point, with range [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight()), zero locations if no rough location is known
		 * @param currentPoints The resulting positions of the tracked points located in the current frame, one for each previous point
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param firstPoint The first point to be handled, with range [0, previousPoints->size())
		 * @param numberPoints the number of points to be handled, with range [1, previousPoints->size()]
		 * @tparam tSize The size of the square patch which is used for tracking in pixel, with range [1, min('finestWidth', 'finestHeight') * 2], must be odd
		 * @see trackPointsSubPixelMask().
		 */
		template <unsigned int tSize>
		static void trackPointsMaskSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const FramePyramid* previousMaskPyramid, const FramePyramid* currentMaskPyramid, const unsigned int numberLayers, const PixelPositions* previousPoints, const PixelPositions* roughCurrentPoints, PixelPositions* currentPoints, const unsigned int coarsestLayerRadius, const unsigned int firstPoint, const unsigned int numberPoints);

		/**
		 * Tracks a subset of a given set of points between two frame pyramids with sub-pixel accuracy while each pyramid layer can contain valid and invalid pixels specified by two individual (pyramid) masks.
		 * The points are tracked unidirectional (from the previous frame to the current frame).<br>
		 * The motion is determined by application of an image patch centered around the point to be tracked.<br>
		 * Patch pixels outside the any of the frames will be treated as invalid mask pixels and thus will not be considered.
		 * @param previousPyramid The frame pyramid of the previous frame which will be used for the motion detection, must be valid
		 * @param currentPyramid The frame pyramid of the current frame which will be used for the motion detection, with same pixel origin as the first frame (pyramid), must be valid
		 * @param previousMaskPyramid The corresponding 8 bit mask pyramid of the previous frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the previous frame (pyramid), must be valid
		 * @param currentMaskPyramid The corresponding 8 bit mask pyramid of the current frame identifying valid pixels, pixel values not equal to 0x00 are valid, with same layer number, frame dimension and pixel origin as the current frame (pyramid), must be valid
		 * @param numberLayers The number of pyramid layers that will be used for tracking, with range [1, min('validLayers', 'coarsest layer that match with the patch size')]
		 * @param previousPoints The positions to be tracked in the current frame, with range [0, previousPyramid.finestWidth())x[0, previousPyramid.finestHeight())
		 * @param roughCurrentPoints Already known positions of the tracked points in the current frame (pyramid) one for each previous point, with range [0, currentPyramid.finestWidth())x[0, currentPyramid.finestHeight()), zero locations if no rough location is known
		 * @param currentPoints The resulting positions of the tracked points located in the current frame, one for each previous point
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, with range [1, infinity)
		 * @param firstPoint The first point to be handled, with range [0, previousPoints->size())
		 * @param numberPoints the number of points to be handled, with range [1, previousPoints->size()]
		 * @tparam tSize The size of the square patch which is used for tracking in pixel, with range [1, min('finestWidth', 'finestHeight') * 2], must be odd
		 * @see trackPointsSubPixelMask().
		 */
		template <unsigned int tSize>
		static void trackPointsSubPixelMaskSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const FramePyramid* previousMaskPyramid, const FramePyramid* currentMaskPyramid, const unsigned int numberLayers, const Vectors2* previousPoints, const Vectors2* roughCurrentPoints, Vectors2* currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, const unsigned int firstPoint, const unsigned int numberPoints);
};

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsSubPixelMirroredBorder(const Frame& previousFrame, const Frame& currentFrame, const Vectors2& previousPoints, const Vectors2& roughPoints, Vectors2& currentPoints, const unsigned int maximalOffset, const unsigned int coarsestLayerRadius, const FramePyramid::DownsamplingMode downsamplingMode, const unsigned int subPixelIterations, Worker* worker, MetricResults* metricResults, MetricResults* metricIdentityResults)
{
	static_assert(tSize % 2u == 1u, "Invalid image patch size, must be odd!");
	static_assert(tSize >= 3u, "Invalid image patch size, must be larger than 2!");

	ocean_assert(previousFrame && currentFrame);

	ocean_assert(previousFrame.frameType().pixelFormat() == currentFrame.frameType().pixelFormat());
	ocean_assert(previousFrame.frameType().pixelOrigin() == currentFrame.frameType().pixelOrigin());

	ocean_assert(previousPoints.size() == roughPoints.size());

	ocean_assert(maximalOffset >= 1u);
	ocean_assert(subPixelIterations >= 1u);

	const unsigned int idealLayers = FramePyramid::idealLayers(previousFrame.width(), previousFrame.height(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u, maximalOffset, coarsestLayerRadius);
	ocean_assert(idealLayers >= 1u);

	if (idealLayers == 0u)
	{
		return false;
	}

	const FramePyramid previousPyramid(previousFrame, idealLayers, worker, downsamplingMode);
	const FramePyramid currentPyramid(currentFrame, idealLayers, worker, downsamplingMode);

	return trackPointsSubPixelMirroredBorder<tSize>(previousPyramid, currentPyramid, previousPoints, roughPoints, currentPoints, coarsestLayerRadius, subPixelIterations, worker, metricResults, metricIdentityResults);
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
inline bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsSubPixelMirroredBorder(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const Vectors2& previousPoints, const Vectors2& roughPoints, Vectors2& currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, Worker* worker, MetricResults* metricResults, MetricResults* metricIdentityResults)
{
	static_assert(tSize % 2u == 1u, "Invalid image patch size, must be odd!");
	static_assert(tSize >= 3u, "Invalid image patch size, must be larger than 2!");

	ocean_assert(previousPyramid.frameType().pixelFormat() == currentPyramid.frameType().pixelFormat());
	ocean_assert(previousPyramid.frameType().pixelOrigin() == currentPyramid.frameType().pixelOrigin());

	ocean_assert(previousPoints.size() == roughPoints.size());

	ocean_assert(subPixelIterations >= 1u);

	const unsigned int idealLayers = CV::FramePyramid::idealLayers(previousPyramid.finestWidth(), previousPyramid.finestHeight(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u);
	const unsigned int numberLayers = min(min(previousPyramid.validLayers(), currentPyramid.validLayers()), idealLayers);

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
		worker->executeFunction(Worker::Function::createStatic(&AdvancedMotion::trackPointsSubPixelMirroredBorderSubset<tSize>, &previousPyramid, &currentPyramid, numberLayers, &previousPoints, &roughPoints, &currentPoints, coarsestLayerRadius, subPixelIterations, metricResults ? metricResults->data() : nullptr, metricIdentityResults ? metricIdentityResults->data() : nullptr, 0u, 0u), 0u, (unsigned int)(previousPoints.size()));
	}
	else
	{
		trackPointsSubPixelMirroredBorderSubset<tSize>(&previousPyramid, &currentPyramid, numberLayers, &previousPoints, &roughPoints, &currentPoints, coarsestLayerRadius, subPixelIterations, metricResults ? metricResults->data() : nullptr, metricIdentityResults ? metricIdentityResults->data() : nullptr, 0u, (unsigned int)(previousPoints.size()));
	}

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tSize>
inline bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsSubPixelMirroredBorder(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const Vectors2& previousPoints, const Vectors2& roughPoints, Vectors2& currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, Worker* worker, MetricResults* metricResults, MetricResults* metricIdentityResults)
{
	static_assert(tSize % 2u == 1u, "Invalid image patch size, must be odd!");
	static_assert(tSize >= 3u, "Invalid image patch size, must be larger than 2!");

	ocean_assert(FrameType::arePixelFormatsCompatible(previousPyramid.frameType().pixelFormat(), currentPyramid.frameType().pixelFormat()));
	ocean_assert(previousPyramid.frameType().pixelOrigin() == currentPyramid.frameType().pixelOrigin());

	ocean_assert(previousPyramid.frameType().channels() == tChannels);
	ocean_assert(currentPyramid.frameType().channels() == tChannels);

	ocean_assert(previousPoints.size() == roughPoints.size());

	ocean_assert(subPixelIterations >= 1u);

	const unsigned int idealLayers = CV::FramePyramid::idealLayers(previousPyramid.finestWidth(), previousPyramid.finestHeight(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u);
	const unsigned int numberLayers = min(min(previousPyramid.validLayers(), currentPyramid.validLayers()), idealLayers);

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
		worker->executeFunction(Worker::Function::createStatic(&AdvancedMotion::trackPointsSubPixelMirroredBorderSubset<tChannels, tSize>, &previousPyramid, &currentPyramid, numberLayers, &previousPoints, &roughPoints, &currentPoints, coarsestLayerRadius, subPixelIterations, metricResults ? metricResults->data() : nullptr, metricIdentityResults ? metricIdentityResults->data() : nullptr, 0u, 0u), 0u, (unsigned int)(previousPoints.size()));
	}
	else
	{
		trackPointsSubPixelMirroredBorderSubset<tChannels, tSize>(&previousPyramid, &currentPyramid, numberLayers, &previousPoints, &roughPoints, &currentPoints, coarsestLayerRadius, subPixelIterations, metricResults ? metricResults->data() : nullptr, metricIdentityResults ? metricIdentityResults->data() : nullptr, 0u, (unsigned int)(previousPoints.size()));
	}

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
inline bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsMask(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const FramePyramid& previousMaskPyramid, const FramePyramid& currentMaskPyramid, const PixelPositions& previousPoints, const PixelPositions& roughCurrentPoints, PixelPositions& currentPoints, const unsigned int coarsestLayerRadius, Worker* worker)
{
	static_assert(tSize % 2u == 1u, "Invalid image patch size, must be odd!");
	static_assert(tSize >= 3u, "Invalid image patch size, must be larger than 2!");

	ocean_assert(previousPyramid.frameType().pixelFormat() == currentPyramid.frameType().pixelFormat());
	ocean_assert(previousPyramid.frameType().pixelOrigin() == currentPyramid.frameType().pixelOrigin());

	ocean_assert(previousMaskPyramid.frameType().pixelFormat() == currentMaskPyramid.frameType().pixelFormat());
	ocean_assert(previousMaskPyramid.frameType().pixelOrigin() == currentMaskPyramid.frameType().pixelOrigin());

	const unsigned int idealLayers = CV::FramePyramid::idealLayers(previousPyramid.finestWidth(), previousPyramid.finestHeight(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u);
	const unsigned int numberLayers = min(min(previousPyramid.validLayers(), currentPyramid.validLayers()), idealLayers);

	if (numberLayers == 0u)
	{
		return false;
	}

	currentPoints.resize(previousPoints.size());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&AdvancedMotion::trackPointsMaskSubset<tSize>, &previousPyramid, &currentPyramid, &previousMaskPyramid, &currentMaskPyramid, numberLayers, &previousPoints, &roughCurrentPoints, &currentPoints, coarsestLayerRadius, 0u, 0u), 0u, (unsigned int)(previousPoints.size()));
	}
	else
	{
		trackPointsMaskSubset<tSize>(&previousPyramid, &currentPyramid, &previousMaskPyramid, &currentMaskPyramid, numberLayers, &previousPoints, &roughCurrentPoints, &currentPoints, coarsestLayerRadius, 0u, (unsigned int)(previousPoints.size()));
	}

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
inline bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsSubPixelMask(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, const FramePyramid& previousMaskPyramid, const FramePyramid& currentMaskPyramid, const Vectors2& previousPoints, const Vectors2& roughCurrentPoints, Vectors2& currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, Worker* worker)
{
	static_assert(tSize % 2u == 1u, "Invalid image patch size, must be odd!");
	static_assert(tSize >= 3u, "Invalid image patch size, must be larger than 2!");

	ocean_assert(previousPyramid.frameType().pixelFormat() == currentPyramid.frameType().pixelFormat());
	ocean_assert(previousPyramid.frameType().pixelOrigin() == currentPyramid.frameType().pixelOrigin());

	ocean_assert(previousMaskPyramid.frameType().pixelFormat() == currentMaskPyramid.frameType().pixelFormat());
	ocean_assert(previousMaskPyramid.frameType().pixelOrigin() == currentMaskPyramid.frameType().pixelOrigin());

	const unsigned int idealLayers = CV::FramePyramid::idealLayers(previousPyramid.finestWidth(), previousPyramid.finestHeight(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u);
	const unsigned int numberLayers = min(min(previousPyramid.validLayers(), currentPyramid.validLayers()), idealLayers);

	if (numberLayers == 0u)
	{
		return false;
	}

	currentPoints.resize(previousPoints.size());

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&AdvancedMotion::trackPointsSubPixelMaskSubset<tSize>, &previousPyramid, &currentPyramid, &previousMaskPyramid, &currentMaskPyramid, numberLayers, &previousPoints, &roughCurrentPoints, &currentPoints, coarsestLayerRadius, subPixelIterations, 0u, 0u), 0u, (unsigned int)(previousPoints.size()));
	}
	else
	{
		trackPointsSubPixelMaskSubset<tSize>(&previousPyramid, &currentPyramid, &previousMaskPyramid, &currentMaskPyramid, numberLayers, &previousPoints, &roughCurrentPoints, &currentPoints, coarsestLayerRadius, subPixelIterations, 0u, (unsigned int)previousPoints.size());
	}

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackArbitraryPointsBidirectionalSubPixelMirroredBorder(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError, const SubRegion& previousSubRegion, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int strength, Worker* worker, const unsigned int trackingLayers)
{
	ocean_assert(previousPyramid && nextPyramid);

	ocean_assert(previousImagePoints.empty() && nextImagePoints.empty());

	if (!previousPyramid || previousPyramid.frameType() != nextPyramid.frameType())
	{
		return false;
	}

	const unsigned int maximalTrackingLayers = min(trackingLayers, min(previousPyramid.validLayers(), nextPyramid.validLayers()));

	for (unsigned int n = 0u; n < maximalTrackingLayers; ++n)
	{
		const Frame previousLayer(previousPyramid[n], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		if (previousLayer.width() < tSize || previousLayer.height() < tSize)
		{
			break;
		}

		Frame yFrame;
		if (!FrameConverter::Comfort::convert(previousLayer, FrameType(previousLayer, FrameType::FORMAT_Y8), yFrame, FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		if (yFrame.width() == 0u || yFrame.height() == 0u)
		{
			return false;
		}

		const Scalar layerFactor = Scalar((unsigned int)(1 << n));
		const Scalar invLayerFactor = Scalar(1) / layerFactor;

		// Calculate bounding box:
		Box2 boundingBox = previousSubRegion.boundingBox().isValid() ? previousSubRegion.boundingBox() * invLayerFactor : Box2();
		if (!boundingBox.isValid())
		{
			boundingBox = Box2(0, 0, Scalar(yFrame.width()), Scalar(yFrame.height()));
		}

		// Calculate clip window by intersecting bounding box with image borders:
		unsigned int windowLeft, windowTop, windowWidth, windowHeight;
		if (!boundingBox.box2integer(yFrame.width(), yFrame.height(), windowLeft, windowTop, windowWidth, windowHeight))
		{
			continue;
		}

		ocean_assert(windowWidth >= 1u && windowWidth <= yFrame.width());
		ocean_assert(windowHeight >= 1u && windowHeight <= yFrame.height());

		Detector::HarrisCorners corners;
		if (!Detector::HarrisCornerDetector::detectCorners(yFrame, windowLeft, windowTop, windowWidth, windowHeight, strength, true, corners, true, worker))
		{
			return false;
		}

		// If first run went bad, we try again with lowered expectations:
		if (n == 0u && corners.size() < 50)
		{
			corners.clear();
			if (!Detector::HarrisCornerDetector::detectCorners(yFrame, windowLeft, windowTop, windowWidth, windowHeight, strength / 2u, true, corners, true, worker))
			{
				return false;
			}
		}

		// If second run went bad, we try once more with even lower expectations:
		if (n == 0u && corners.size() < 20)
		{
			corners.clear();
			if (!Detector::HarrisCornerDetector::detectCorners(yFrame, windowLeft, windowTop, windowWidth, windowHeight, strength / 4u, true, corners, true, worker))
			{
				return false;
			}
		}

		if (corners.empty())
		{
			continue;
		}

		// Restrict corners to those lying inside sub regions:
		Detector::HarrisCorners cornersSubRegion;

		// if no sub-area filter is provided, use all corners
		if (previousSubRegion.isEmpty())
		{
			cornersSubRegion = std::move(corners);
		}
		else
		{
			cornersSubRegion.reserve(corners.size());

			for (Detector::HarrisCorners::const_iterator i = corners.begin(); i != corners.end(); ++i)
			{
				if (previousSubRegion.isInside(i->observation() * layerFactor))
				{
					cornersSubRegion.push_back(*i);
				}
			}
		}

		if (cornersSubRegion.empty())
		{
			continue;
		}

		std::sort(cornersSubRegion.begin(), cornersSubRegion.end()); // Sort by corner strength in descending order
		Vectors2 smallPreviousImagePoints = Detector::HarrisCorner::corners2imagePoints(cornersSubRegion);

		if (!smallPreviousImagePoints.empty() && horizontalBins != 0u && verticalBins != 0u)
		{
			smallPreviousImagePoints = Geometry::SpatialDistribution::distributeAndFilter(smallPreviousImagePoints.data(), smallPreviousImagePoints.size(), Scalar(windowLeft), Scalar(windowTop), Scalar(windowWidth), Scalar(windowHeight), horizontalBins, verticalBins);
		}

		if (smallPreviousImagePoints.empty())
		{
			continue;
		}

		// Create sub pyramid using fast frame data referencing:
		const FramePyramid previousSmall = FramePyramid(previousPyramid, false, n, previousPyramid.validLayers() - n);
		const FramePyramid nextSmall = FramePyramid(nextPyramid, false, n, nextPyramid.validLayers() - n);

		Vectors2 smallNextImagePoints;

		// Find corresponding points in next frame:
		if (trackPointsBidirectionalSubPixelMirroredBorder<tSize>(previousSmall, nextSmall, coarsestLayerRadius, smallPreviousImagePoints, smallNextImagePoints, maximalSqrError, worker))
		{
			for (unsigned int i = 0u; i < smallPreviousImagePoints.size(); ++i)
			{
				previousImagePoints.push_back(smallPreviousImagePoints[i] * layerFactor);
				nextImagePoints.push_back(smallNextImagePoints[i] * layerFactor);
			}
		}
	}

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackArbitraryPointsBidirectionalSubPixelMirroredBorder(const Frame& previousFrame, const Frame& nextFrame, const unsigned int maximalOffset, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError, const SubRegion& previousSubRegion, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int strength, const FramePyramid::DownsamplingMode downsamplingMode, Worker* worker, const unsigned int trackingLayers)
{
	ocean_assert(previousFrame && nextFrame);

	if (!previousFrame || previousFrame.frameType() != nextFrame.frameType())
	{
		return false;
	}

	const unsigned int layers = min(trackingLayers, FramePyramid::idealLayers(previousFrame.width(), previousFrame.height(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u, maximalOffset, coarsestLayerRadius));

	if (layers == 0u)
	{
		return false;
	}

	const FramePyramid previousPyramid(previousFrame, layers, worker, downsamplingMode);
	const FramePyramid nextPyramid(nextFrame, layers, worker, downsamplingMode);

	return trackArbitraryPointsBidirectionalSubPixelMirroredBorder<tSize>(previousPyramid, nextPyramid, coarsestLayerRadius, previousImagePoints, nextImagePoints, maximalSqrError, previousSubRegion, horizontalBins, verticalBins, strength, worker, trackingLayers);
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsBidirectionalSubPixelMirroredBorder(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError, Worker* worker, Indices32* validIndices, const unsigned int subPixelIterations)
{
	ocean_assert(previousPyramid && nextPyramid);

	ocean_assert(!previousImagePoints.empty() && nextImagePoints.empty());
	ocean_assert(!validIndices || validIndices->empty());

	if (previousImagePoints.empty())
	{
		return true;
	}

	if (!previousPyramid || !previousPyramid.frameType().isPixelFormatCompatible(nextPyramid.frameType().pixelFormat()) || previousPyramid.frameType().pixelOrigin() != nextPyramid.frameType().pixelOrigin())
	{
		return false;
	}

	Vectors2 previousPointCandidates(std::move(previousImagePoints));
	ocean_assert(previousImagePoints.empty());

	// forward point motion
	Vectors2 nextPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tSize>(previousPyramid, nextPyramid, previousPointCandidates, previousPointCandidates, nextPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	// backward point motion
	Vectors2 backwardsPreviousPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tSize>(nextPyramid, previousPyramid, nextPointCandidates, nextPointCandidates, backwardsPreviousPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	ocean_assert(previousPointCandidates.size() == nextPointCandidates.size());
	ocean_assert(previousPointCandidates.size() == backwardsPreviousPointCandidates.size());

	previousImagePoints = Vectors2();
	previousImagePoints.reserve(previousPointCandidates.size());

	nextImagePoints.clear();
	nextImagePoints.reserve(previousPointCandidates.size());

	if (validIndices != nullptr)
	{
		validIndices->clear();
		validIndices->reserve(previousPointCandidates.size());

		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));
			const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

			previousImagePoints.push_back(previousPointCandidates[n]);
			nextImagePoints.push_back(nextImagePoint);

			// identify point pairs with almost identical point motion
			if (sqrDistance <= maximalSqrError && nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
			{
				validIndices->emplace_back(Index32(n));
			}
		}
	}
	else
	{
		// identify point pairs with almost identical point motion
		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));

			if (sqrDistance <= maximalSqrError)
			{
				const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

				if (nextImagePoint.x() >= Scalar(0) && nextImagePoint.y() >= Scalar(0) && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
				{
					previousImagePoints.push_back(previousPointCandidates[n]);
					nextImagePoints.push_back(nextImagePoint);
				}
			}
		}
	}

	ocean_assert(previousImagePoints.size() == nextImagePoints.size());

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsBidirectionalSubPixelMirroredBorder(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError, Worker* worker, Indices32* validIndices, const unsigned int subPixelIterations)
{
	ocean_assert(previousPyramid && nextPyramid);
	ocean_assert(previousPyramid.frameType().channels() == tChannels);
	ocean_assert(nextPyramid.frameType().channels() == tChannels);

	ocean_assert(!previousImagePoints.empty() && nextImagePoints.empty());
	ocean_assert(!validIndices || validIndices->empty());

	if (previousImagePoints.empty())
	{
		return true;
	}

	if (!previousPyramid || previousPyramid.frameType() != nextPyramid.frameType())
	{
		return false;
	}

	Vectors2 previousPointCandidates(std::move(previousImagePoints));
	ocean_assert(previousImagePoints.empty());

	// forward point motion
	Vectors2 nextPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tChannels, tSize>(previousPyramid, nextPyramid, previousPointCandidates, previousPointCandidates, nextPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	// backward point motion
	Vectors2 backwardsPreviousPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tChannels, tSize>(nextPyramid, previousPyramid, nextPointCandidates, nextPointCandidates, backwardsPreviousPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	ocean_assert(previousPointCandidates.size() == nextPointCandidates.size());
	ocean_assert(previousPointCandidates.size() == backwardsPreviousPointCandidates.size());

	previousImagePoints = Vectors2();
	previousImagePoints.reserve(previousPointCandidates.size());

	nextImagePoints.clear();
	nextImagePoints.reserve(previousPointCandidates.size());

	if (validIndices)
	{
		validIndices->clear();
		validIndices->reserve(previousPointCandidates.size());

		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));
			const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

			previousImagePoints.push_back(previousPointCandidates[n]);
			nextImagePoints.push_back(nextImagePoint);

			// identify point pairs with almost identical point motion
			if (sqrDistance <= maximalSqrError && nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
			{
				validIndices->push_back((unsigned int)(n));
			}
		}
	}
	else
	{
		// identify point pairs with almost identical point motion
		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));

			if (sqrDistance <= maximalSqrError)
			{
				const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

				if (nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
				{
					previousImagePoints.push_back(previousPointCandidates[n]);
					nextImagePoints.push_back(nextImagePoint);
				}
			}
		}
	}

	ocean_assert(previousImagePoints.size() == nextImagePoints.size());

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, const Vectors2& roughNextImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError, Worker* worker, Indices32* validIndices, const unsigned int subPixelIterations)
{
	ocean_assert(previousPyramid && nextPyramid);
	ocean_assert(previousPyramid.frameType().pixelFormat() == nextPyramid.frameType().pixelFormat() && previousPyramid.frameType().pixelOrigin() == nextPyramid.frameType().pixelOrigin());

	ocean_assert(!previousImagePoints.empty() && nextImagePoints.empty());
	ocean_assert(!validIndices || validIndices->empty());

	ocean_assert(&previousImagePoints != &roughNextImagePoints);

	if (previousImagePoints.empty())
	{
		return true;
	}

	if (!previousPyramid || previousPyramid.frameType().pixelFormat() != nextPyramid.frameType().pixelFormat() || previousPyramid.frameType().pixelOrigin() != nextPyramid.frameType().pixelOrigin())
	{
		return false;
	}

	Vectors2 previousPointCandidates(std::move(previousImagePoints));
	ocean_assert(previousImagePoints.empty());

	// forward point motion
	Vectors2 nextPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tSize>(previousPyramid, nextPyramid, previousPointCandidates, roughNextImagePoints, nextPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	// backward point motion
	Vectors2 backwardsPreviousPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tSize>(nextPyramid, previousPyramid, nextPointCandidates, previousPointCandidates, backwardsPreviousPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	previousImagePoints = Vectors2();
	previousImagePoints.reserve(previousPointCandidates.size());

	nextImagePoints.clear();
	nextImagePoints.reserve(previousPointCandidates.size());

	if (validIndices)
	{
		validIndices->clear();
		validIndices->reserve(previousPointCandidates.size());

		// identify point pairs with almost identical point motion
		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));
			const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

			previousImagePoints.push_back(previousPointCandidates[n]);
			nextImagePoints.push_back(nextImagePoint);

			if (sqrDistance <= maximalSqrError && nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
			{
				validIndices->push_back((unsigned int)(n));
			}
		}
	}
	else
	{
		// identify point pairs with almost identical point motion
		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));

			if (sqrDistance <= maximalSqrError)
			{
				const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

				if (nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
				{
					previousImagePoints.push_back(previousPointCandidates[n]);
					nextImagePoints.push_back(nextImagePoint);
				}
			}
		}
	}

	ocean_assert(previousImagePoints.size() == nextImagePoints.size());

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations(const CV::FramePyramid& previousPyramid, const CV::FramePyramid& nextPyramid, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, const Vectors2& roughNextImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError, Worker* worker, Indices32* validIndices, const unsigned int subPixelIterations)
{
	ocean_assert(previousPyramid && nextPyramid);
	ocean_assert(FrameType::arePixelFormatsCompatible(previousPyramid.frameType().pixelFormat(), nextPyramid.frameType().pixelFormat()));
	ocean_assert(previousPyramid.frameType().pixelOrigin() == nextPyramid.frameType().pixelOrigin());
	ocean_assert(previousPyramid.frameType().channels() == tChannels);
	ocean_assert(nextPyramid.frameType().channels() == tChannels);

	ocean_assert(!previousImagePoints.empty() && nextImagePoints.empty());
	ocean_assert(!validIndices || validIndices->empty());

	ocean_assert(&previousImagePoints != &roughNextImagePoints);

	if (previousImagePoints.empty())
	{
		return true;
	}

	if (!previousPyramid
			|| !nextPyramid
			|| !FrameType::arePixelFormatsCompatible(previousPyramid.frameType().pixelFormat(), nextPyramid.frameType().pixelFormat())
			|| previousPyramid.frameType().pixelOrigin() != nextPyramid.frameType().pixelOrigin()
			|| previousPyramid.frameType().channels() != tChannels)
	{
		return false;
	}

	Vectors2 previousPointCandidates(std::move(previousImagePoints));
	ocean_assert(previousImagePoints.empty());

	// forward point motion
	Vectors2 nextPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tChannels, tSize>(previousPyramid, nextPyramid, previousPointCandidates, roughNextImagePoints, nextPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	// backward point motion
	Vectors2 backwardsPreviousPointCandidates(previousPointCandidates.size());
	if (!trackPointsSubPixelMirroredBorder<tChannels, tSize>(nextPyramid, previousPyramid, nextPointCandidates, previousPointCandidates, backwardsPreviousPointCandidates, coarsestLayerRadius, subPixelIterations, worker))
	{
		return false;
	}

	previousImagePoints = Vectors2();
	previousImagePoints.reserve(previousPointCandidates.size());

	nextImagePoints.clear();
	nextImagePoints.reserve(previousPointCandidates.size());

	if (validIndices)
	{
		validIndices->clear();
		validIndices->reserve(previousPointCandidates.size());

		// identify point pairs with almost identical point motion
		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));
			const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

			previousImagePoints.push_back(previousPointCandidates[n]);
			nextImagePoints.push_back(nextImagePoint);

			if (sqrDistance <= maximalSqrError && nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
			{
				validIndices->push_back((unsigned int)(n));
			}
		}
	}
	else
	{
		// identify point pairs with almost identical point motion
		for (size_t n = 0; n < previousPointCandidates.size(); ++n)
		{
			const Scalar sqrDistance(previousPointCandidates[n].sqrDistance(backwardsPreviousPointCandidates[n]));

			if (sqrDistance <= maximalSqrError)
			{
				const Vector2 nextImagePoint(nextPointCandidates[n] + (previousPointCandidates[n] - backwardsPreviousPointCandidates[n]) * Scalar(0.5));

				if (nextImagePoint.x() >= 0 && nextImagePoint.y() >= 0 && nextImagePoint.x() < Scalar(nextPyramid.finestWidth()) && nextImagePoint.y() < Scalar(nextPyramid.finestHeight()))
				{
					previousImagePoints.push_back(previousPointCandidates[n]);
					nextImagePoints.push_back(nextImagePoint);
				}
			}
		}
	}

	ocean_assert(previousImagePoints.size() == nextImagePoints.size());

	return true;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsBidirectionalSubPixelMirroredBorder(const Frame& previousFrame, const Frame& nextFrame, const unsigned int maximalOffset, const unsigned int coarsestLayerRadius, Vectors2& previousImagePoints, Vectors2& nextImagePoints, const Scalar maximalSqrError, const FramePyramid::DownsamplingMode downsamplingMode, Worker* worker, Indices32* validIndices, const unsigned int subPixelIterations)
{
	ocean_assert(previousFrame.isValid() && nextFrame.isValid());

	if (!previousFrame || !previousFrame.isPixelFormatCompatible(nextFrame.pixelFormat()) || previousFrame.pixelOrigin() != nextFrame.pixelOrigin())
	{
		return false;
	}

	const unsigned int previousLayers = FramePyramid::idealLayers(previousFrame.width(), previousFrame.height(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u, maximalOffset, coarsestLayerRadius);
	const unsigned int nextLayers = FramePyramid::idealLayers(nextFrame.width(), nextFrame.height(), (tSize / 2u) * 4u, (tSize / 2u) * 4u, 2u, maximalOffset, coarsestLayerRadius);
	ocean_assert(previousLayers >= 1u && nextLayers >= 1u);

	const unsigned int layers = std::min(previousLayers, nextLayers);

	if (layers == 0u)
	{
		return false;
	}

	const FramePyramid previousPyramid(previousFrame, layers, worker, downsamplingMode);
	const FramePyramid nextPyramid(nextFrame, layers, worker, downsamplingMode);

	return trackPointsBidirectionalSubPixelMirroredBorder<tSize>(previousPyramid, nextPyramid, coarsestLayerRadius, previousImagePoints, nextImagePoints, maximalSqrError, worker, validIndices, subPixelIterations);
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
bool AdvancedMotion<TMetricInteger, TMetricFloat>::trackReliableReferencePoints(const FramePyramid& previousPyramid, const FramePyramid& currentPyramid, Vectors2& previousReferencePoints, Vectors2& currentReferencePoints, const unsigned int horizontalBins, const unsigned int verticalBins, const PixelBoundingBox& boundingBox, const Frame& maskFrame, Worker* worker)
{
	ocean_assert(previousReferencePoints.empty());
	ocean_assert(currentReferencePoints.empty());

	ocean_assert(&previousReferencePoints != &currentReferencePoints);

	ocean_assert(horizontalBins >= 1u);
	ocean_assert(verticalBins >= 1u);

	const Frame previousFrame(previousPyramid.finestLayer(), Frame::temporary_ACM_USE_KEEP_LAYOUT);

	const unsigned int width = previousFrame.width();
	const unsigned int height = previousFrame.height();

	const unsigned int areaLeft = boundingBox ? boundingBox.left() : 0u;
	const unsigned int areaTop = boundingBox ? boundingBox.top() : 0u;
	const unsigned int areaWidth = boundingBox ? boundingBox.width() : width;
	const unsigned int areaHeight = boundingBox ? boundingBox.height() : height;

	ocean_assert(areaLeft + areaWidth <= width);
	ocean_assert(areaTop + areaHeight <= height);

	ocean_assert(!maskFrame.isValid() || maskFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8));

	Detector::HarrisCorners features;
	features.reserve(5000);
	Detector::HarrisCornerDetector::detectCorners(previousFrame, areaLeft, areaTop, areaWidth, areaHeight, 1u, true, features, false, worker);
	ocean_assert(!features.empty());

	if (features.empty())
	{
		return false;
	}

	std::sort(features.begin(), features.end());
	const Vectors2 allPreviousReferencePoints(CV::Detector::HarrisCorner::corners2imagePoints(features));

	const Geometry::SpatialDistribution::DistributionArray distribution(Geometry::SpatialDistribution::distributeToArray(allPreviousReferencePoints.data(), (unsigned int)allPreviousReferencePoints.size(), Scalar(areaLeft), Scalar(areaTop), Scalar(areaWidth), Scalar(areaHeight), horizontalBins, verticalBins));

	const uint8_t* const maskData = maskFrame ? maskFrame.constdata<uint8_t>() : nullptr;
	const unsigned int maskStrideElements = maskFrame ? maskFrame.strideElements() : 0u;

	previousReferencePoints.reserve(distribution.bins());
	for (unsigned int n = 0u; n < distribution.bins(); ++n)
	{
		const Indices32& indices = distribution[n];

		if (!indices.empty())
		{
			ocean_assert(indices.front() < allPreviousReferencePoints.size());
			const Vector2& point = allPreviousReferencePoints[indices.front()];

			const unsigned int xPosition = (unsigned int)(point.x());
			const unsigned int yPosition = (unsigned int)(point.y());

			ocean_assert(xPosition < width);
			ocean_assert(yPosition < height);

			if (maskData == nullptr || maskData[yPosition * maskStrideElements + xPosition] == 0xFFu)
			{
				previousReferencePoints.push_back(point);
			}
		}
	}

	if (previousReferencePoints.empty())
	{
		return false;
	}

	return trackPointsSubPixelMirroredBorder<tSize>(previousPyramid, currentPyramid, previousReferencePoints, previousReferencePoints, currentReferencePoints, 2u, 4u, worker);
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tPatchSize>
inline Vector2 AdvancedMotion<TMetricInteger, TMetricFloat>::pointMotionInFrameSubPixelMirroredBorder(const uint8_t* frame0, const uint8_t* frame1, const unsigned int channels, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const Vector2& position0, const unsigned int radiusX, const unsigned int radiusY, const Vector2& rough1, const unsigned int subPixelIterations, uint32_t* metricResult, uint32_t* metricIdentityResult)
{
	ocean_assert(channels >= 1u);

	switch (channels)
	{
		case 1u:
			return pointMotionInFrameSubPixelMirroredBorder<1u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, frame0PaddingElements, frame1PaddingElements, position0, radiusX, radiusY, rough1, subPixelIterations, metricResult, metricIdentityResult);

		case 2u:
			return pointMotionInFrameSubPixelMirroredBorder<2u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, frame0PaddingElements, frame1PaddingElements, position0, radiusX, radiusY, rough1, subPixelIterations, metricResult, metricIdentityResult);

		case 3u:
			return pointMotionInFrameSubPixelMirroredBorder<3u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, frame0PaddingElements, frame1PaddingElements, position0, radiusX, radiusY, rough1, subPixelIterations, metricResult, metricIdentityResult);

		case 4u:
			return pointMotionInFrameSubPixelMirroredBorder<4u, tPatchSize>(frame0, frame1, width0, height0, width1, height1, frame0PaddingElements, frame1PaddingElements, position0, radiusX, radiusY, rough1, subPixelIterations, metricResult, metricIdentityResult);
	}

	ocean_assert(false && "Invalid pixel format!");
	return rough1;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
void AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsSubPixelMirroredBorderSubset(const FramePyramid* previousPyramid, const FramePyramid* nextPyramid, const unsigned int numberLayers, const Vectors2* previousPoints, const Vectors2* roughNextPoints, Vectors2* nextPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, unsigned int* metricResults, unsigned int* metricIdentityResults, const unsigned int firstPoint, const unsigned int numberPoints)
{
	static_assert(tSize % 2u == 1u, "Invalid patch size, must be odd!");

	ocean_assert(previousPyramid != nullptr && nextPyramid != nullptr);
	ocean_assert(previousPoints != nullptr && nextPoints != nullptr);

	ocean_assert(previousPyramid->isValid() && nextPyramid->isValid());

	ocean_assert(previousPyramid->frameType().pixelFormat() == nextPyramid->frameType().pixelFormat());
	ocean_assert(previousPyramid->frameType().pixelOrigin() == nextPyramid->frameType().pixelOrigin());

	ocean_assert(roughNextPoints == nullptr || previousPoints->size() == roughNextPoints->size());
	ocean_assert(nextPoints->size() == previousPoints->size());

	ocean_assert(firstPoint + numberPoints <= previousPoints->size());

	ocean_assert(numberLayers >= 1u);
	ocean_assert(numberLayers <= previousPyramid->validLayers());
	ocean_assert(numberLayers <= nextPyramid->validLayers());

	ocean_assert(previousPyramid->layer(numberLayers - 1u).width() >= tSize / 2u);
	ocean_assert(previousPyramid->layer(numberLayers - 1u).height() >= tSize / 2u);

	ShiftVector<Vector2> intermediateRoughNextPoints(firstPoint, numberPoints);

	const Scalar coarsetsWidthNextPyramid = Scalar(nextPyramid->layer(numberLayers - 1u).width());
	const Scalar coarsetsHeightNextPyramid = Scalar(nextPyramid->layer(numberLayers - 1u).height());
	const Scalar coarsetsLayerFactorNextPyramid = Scalar(1) / Scalar(nextPyramid->sizeFactor(numberLayers - 1u));
	ocean_assert(coarsetsWidthNextPyramid >= 1u && coarsetsHeightNextPyramid >= 1u);

	const unsigned int channels = previousPyramid->frameType().channels();
	ocean_assert(channels >= 1u && channels <= 4u);

	for (unsigned int n = firstPoint; n < firstPoint + numberPoints; ++n)
	{
		const Vector2& roughNextPoint = roughNextPoints != nullptr ? (*roughNextPoints)[n] : (*previousPoints)[n];

		const Scalar x = min(roughNextPoint.x() * coarsetsLayerFactorNextPyramid, coarsetsWidthNextPyramid - Scalar(1));
		const Scalar y = min(roughNextPoint.y() * coarsetsLayerFactorNextPyramid, coarsetsHeightNextPyramid - Scalar(1));

		intermediateRoughNextPoints[n] = Vector2(x, y);
	}

	for (unsigned int layerIndex = numberLayers - 1u; layerIndex < numberLayers; --layerIndex)
	{
		const Frame previousLayer((*previousPyramid)[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);
		const Frame nextLayer((*nextPyramid)[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		const uint8_t* const previousLayerData = previousLayer.constdata<uint8_t>();
		const uint8_t* const nextLayerData = nextLayer.constdata<uint8_t>();

		const unsigned int previousLayerPaddingElements = previousLayer.paddingElements();
		const unsigned int nextLayerPaddingElements = nextLayer.paddingElements();

		const unsigned int previousLayerWidth = previousLayer.width();
		const unsigned int previousLayerHeight = previousLayer.height();

		const unsigned int nextLayerWidth = nextLayer.width();
		const unsigned int nextLayerHeight = nextLayer.height();

		if (layerIndex == 0u)
		{
			//  we apply a sub-pixel accurate tracking on the finest pyramid layer

			const unsigned int layerRadiusX = numberLayers == 1u ? coarsestLayerRadius : 2u;
			const unsigned int layerRadiusY = numberLayers == 1u ? coarsestLayerRadius : 2u;

			for (unsigned int pointIndex = firstPoint; pointIndex < firstPoint + numberPoints; ++pointIndex)
			{
				const Vector2& previousPosition = (*previousPoints)[pointIndex];
				ocean_assert(previousPosition.x() >= Scalar(0) && previousPosition.y() >= Scalar(0));
				ocean_assert(previousPosition.x() < Scalar(previousLayerWidth) && previousPosition.y() < Scalar(previousLayerHeight));

				const Vector2& intermediateRoughNextPoint = intermediateRoughNextPoints[pointIndex];
				ocean_assert(intermediateRoughNextPoint.x() >= Scalar(0) && intermediateRoughNextPoint.y() >= Scalar(0));
				ocean_assert(intermediateRoughNextPoint.x() < Scalar(nextLayerWidth) && intermediateRoughNextPoint.y() < Scalar(nextLayerHeight));

				unsigned int* const metricResult = metricResults ? metricResults + pointIndex : nullptr;
				unsigned int* const metricIdentityResult = metricIdentityResults ? metricIdentityResults + pointIndex : nullptr;

				const Vector2 nextPoint = pointMotionInFrameSubPixelMirroredBorder<tSize>(previousLayerData, nextLayerData, channels, previousLayerWidth, previousLayerHeight, nextLayerWidth, nextLayerHeight, previousLayerPaddingElements, nextLayerPaddingElements, previousPosition, layerRadiusX, layerRadiusY, intermediateRoughNextPoint, subPixelIterations, metricResult, metricIdentityResult);

				ocean_assert(nextPoint.x() >= 0 && nextPoint.x() < Scalar(nextLayerWidth));
				ocean_assert(nextPoint.y() >= 0 && nextPoint.y() < Scalar(nextLayerHeight));

				ocean_assert(pointIndex < nextPoints->size());
				(*nextPoints)[pointIndex] = nextPoint;
			}
		}
		else // otherwise we apply a pixel accurate determination
		{
			ocean_assert(layerIndex > 0u);

			const unsigned int layerRadius = (layerIndex == numberLayers - 1u) ? coarsestLayerRadius : 2u;

			const Scalar layerFactor = Scalar(1) / Scalar(1u << layerIndex);

			const Scalar finerNextLayerWidth1 = Scalar((*nextPyramid)[layerIndex - 1u].width()) - Scalar(1);
			const Scalar finerNextLayerHeight1 = Scalar((*nextPyramid)[layerIndex - 1u].height()) - Scalar(1);

			for (unsigned int pointIndex = firstPoint; pointIndex < firstPoint + numberPoints; ++pointIndex)
			{
				ocean_assert(intermediateRoughNextPoints[pointIndex].x() >= Scalar(0) && intermediateRoughNextPoints[pointIndex].y() >= Scalar(0));
				ocean_assert(intermediateRoughNextPoints[pointIndex].x() < Scalar(nextLayerWidth) && intermediateRoughNextPoints[pointIndex].y() < Scalar(nextLayerHeight));

				const PixelPosition intermediateRoughNextPoint(Numeric::round32(intermediateRoughNextPoints[pointIndex].x()), Numeric::round32(intermediateRoughNextPoints[pointIndex].y()));
				ocean_assert(intermediateRoughNextPoint.x() < nextLayerWidth && intermediateRoughNextPoint.y() < nextLayerHeight);

				unsigned int* const metricResult = metricResults ? metricResults + pointIndex : nullptr;

				const Vector2& previousPointFinestLayer = (*previousPoints)[pointIndex];

				const PixelPosition previousPoint(std::min(Numeric::round32(previousPointFinestLayer.x() * layerFactor), int(previousLayerWidth - 1u)), std::min(Numeric::round32(previousPointFinestLayer.y() * layerFactor), int(previousLayerHeight - 1u)));

				ocean_assert(previousPoint.x() < previousLayerWidth && previousPoint.y() < previousLayerHeight);
				if (previousPoint.x() < previousLayerWidth && previousPoint.y() < previousLayerHeight)
				{
					const PixelPosition nextPoint = Motion<TMetricInteger>::template pointMotionInFrameMirroredBorder<tSize>(previousLayerData, nextLayerData, channels, previousLayerWidth, previousLayerHeight, nextLayerWidth, nextLayerHeight, previousPoint, layerRadius, layerRadius, previousLayerPaddingElements, nextLayerPaddingElements, intermediateRoughNextPoint, metricResult);

					ocean_assert(nextPoint.x() < nextLayerWidth && nextPoint.y() < nextLayerHeight);

					intermediateRoughNextPoints[pointIndex] = Vector2(min(Scalar(nextPoint.x() * 2u), finerNextLayerWidth1), min(Scalar(nextPoint.y() * 2u), finerNextLayerHeight1));

					ocean_assert(intermediateRoughNextPoints[pointIndex].x() >= 0 && intermediateRoughNextPoints[pointIndex].x() <= finerNextLayerWidth1);
					ocean_assert(intermediateRoughNextPoints[pointIndex].y() >= 0 && intermediateRoughNextPoints[pointIndex].y() <= finerNextLayerHeight1);
				}
				else
				{
					ocean_assert(false && "This should never happen!");

					intermediateRoughNextPoints[pointIndex] = Vector2(previousPointFinestLayer.x() * layerFactor * Scalar(2), previousPointFinestLayer.y() * layerFactor * Scalar(2));

					ocean_assert(intermediateRoughNextPoints[pointIndex].x() >= 0 && intermediateRoughNextPoints[pointIndex].x() < finerNextLayerWidth1);
					ocean_assert(intermediateRoughNextPoints[pointIndex].y() >= 0 && intermediateRoughNextPoints[pointIndex].y() < finerNextLayerHeight1);
				}
			}
		}
	}
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tSize>
void AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsSubPixelMirroredBorderSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const unsigned int numberLayers, const Vectors2* previousPoints, const Vectors2* roughPoints, Vectors2* currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, unsigned int* metricResults, unsigned int* metricIdentityResults, const unsigned int firstPoint, const unsigned int numberPoints)
{
	static_assert(tSize % 2u == 1u, "Invalid patch size, must be odd!");

	ocean_assert(previousPyramid && currentPyramid);
	ocean_assert(previousPoints && roughPoints && currentPoints);

	ocean_assert(*previousPyramid && *currentPyramid);

	ocean_assert(FrameType::arePixelFormatsCompatible(previousPyramid->frameType().pixelFormat(), currentPyramid->frameType().pixelFormat()));
	ocean_assert(previousPyramid->frameType().pixelOrigin() == currentPyramid->frameType().pixelOrigin());

	ocean_assert(previousPyramid->frameType().channels() == tChannels);
	ocean_assert(currentPyramid->frameType().channels() == tChannels);

	ocean_assert(previousPoints->size() == roughPoints->size());
	ocean_assert(currentPoints->size() == previousPoints->size());

	ocean_assert(firstPoint + numberPoints <= previousPoints->size());

	ocean_assert(numberLayers >= 1u);
	ocean_assert(numberLayers <= previousPyramid->validLayers());
	ocean_assert(numberLayers <= currentPyramid->validLayers());

	ocean_assert(previousPyramid->layer(numberLayers - 1u).width() >= tSize / 2u);
	ocean_assert(previousPyramid->layer(numberLayers - 1u).height() >= tSize / 2u);

	ShiftVector<Vector2> intermediateRoughPoints(firstPoint, numberPoints);

	const Scalar lowestCurrentWidth = Scalar(currentPyramid->layer(numberLayers - 1u).width());
	const Scalar lowestCurrentHeight = Scalar(currentPyramid->layer(numberLayers - 1u).height());
	const Scalar lowestLayerFactor = Scalar(1) / Scalar(currentPyramid->sizeFactor(numberLayers - 1u));
	ocean_assert(lowestCurrentWidth >= 1u && lowestCurrentHeight >= 1u);

	const unsigned int channels = previousPyramid->frameType().channels();
	ocean_assert_and_suppress_unused(channels >= 1u && channels <= 4u, channels);

	for (unsigned int n = firstPoint; n < firstPoint + numberPoints; ++n)
	{
		const Vector2& roughPoint = (*roughPoints)[n];

		const Scalar x = min(roughPoint.x() * lowestLayerFactor, lowestCurrentWidth - 1);
		const Scalar y = min(roughPoint.y() * lowestLayerFactor, lowestCurrentHeight - 1);

		intermediateRoughPoints[n] = Vector2(x, y);
	}

	for (int l = int(numberLayers) - 1; l >= 0; --l)
	{
		const Frame previousFrame((*previousPyramid)[l], Frame::temporary_ACM_USE_KEEP_LAYOUT);
		const Frame currentFrame((*currentPyramid)[l], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		const uint8_t* const previousFrameData = previousFrame.constdata<uint8_t>();
		const uint8_t* const currentFrameData = currentFrame.constdata<uint8_t>();

		const unsigned int previousFramePaddingElements = previousFrame.paddingElements();
		const unsigned int currentFramePaddingElements = currentFrame.paddingElements();

		const unsigned int previousWidth = previousFrame.width();
		const unsigned int previousHeight = previousFrame.height();

		const unsigned int currentWidth = currentFrame.width();
		const unsigned int currentHeight = currentFrame.height();

		// if the finest layer is reached we apply a subpixel accurate determination
		if (l == 0)
		{
			const unsigned int layerRadiusX = numberLayers == 1u ? coarsestLayerRadius : 2u;
			const unsigned int layerRadiusY = numberLayers == 1u ? coarsestLayerRadius : 2u;

			for (unsigned int i = firstPoint; i < firstPoint + numberPoints; ++i)
			{
				ocean_assert(intermediateRoughPoints[i].x() >= Scalar(0) && intermediateRoughPoints[i].y() >= Scalar(0));
				ocean_assert(intermediateRoughPoints[i].x() < Scalar(currentWidth) && intermediateRoughPoints[i].y() < Scalar(currentHeight));

				const Vector2& intermediateRoughPoint = intermediateRoughPoints[i];

				unsigned int* const metricResult = metricResults ? metricResults + i : nullptr;
				unsigned int* const metricIdentityResult = metricIdentityResults ? metricIdentityResults + i : nullptr;

				const Vector2& previousPosition = (*previousPoints)[i];

				ocean_assert(previousPosition.x() >= Scalar(0) && previousPosition.y() >= Scalar(0));
				ocean_assert(previousPosition.x() < Scalar(previousWidth) && previousPosition.y() < Scalar(previousHeight));

				const Vector2 position(pointMotionInFrameSubPixelMirroredBorder<tChannels, tSize>(previousFrameData, currentFrameData, previousWidth, previousHeight, currentWidth, currentHeight, previousFramePaddingElements, currentFramePaddingElements, previousPosition, layerRadiusX, layerRadiusY, intermediateRoughPoint, subPixelIterations, metricResult, metricIdentityResult));

				ocean_assert(position.x() >= 0 && position.x() < Scalar(currentWidth));
				ocean_assert(position.y() >= 0 && position.y() < Scalar(currentHeight));

				ocean_assert(i < currentPoints->size());
				(*currentPoints)[i] = position;
			}
		}
		else // otherwise we apply a pixel accurate determination
		{
			ocean_assert(l > 0);

			const unsigned int layerRadiusX = (l == int(numberLayers) - 1) ? coarsestLayerRadius : 2u;
			const unsigned int layerRadiusY = (l == int(numberLayers) - 1) ? coarsestLayerRadius : 2u;

			for (unsigned int i = firstPoint; i < firstPoint + numberPoints; ++i)
			{
				ocean_assert(intermediateRoughPoints[i].x() >= Scalar(0) && intermediateRoughPoints[i].y() >= Scalar(0));
				ocean_assert(intermediateRoughPoints[i].x() < Scalar(currentWidth) && intermediateRoughPoints[i].y() < Scalar(currentHeight));

				const PixelPosition intermediateRoughPoint(Numeric::round32(intermediateRoughPoints[i].x()), Numeric::round32(intermediateRoughPoints[i].y()));

				unsigned int* const metricResult = metricResults ? metricResults + i : nullptr;

				const Scalar layerFactor = Scalar(1) / Scalar((unsigned int)(1 << l));
				const PixelPosition previousPosition(min(Numeric::round32((*previousPoints)[i].x() * layerFactor), int(previousWidth) - 1),
													 min(Numeric::round32((*previousPoints)[i].y() * layerFactor), int(previousHeight) - 1));

				if (previousPosition.x() < previousWidth && previousPosition.y() < previousHeight)
				{
					const PixelPosition position(Motion<TMetricInteger>::template pointMotionInFrameMirroredBorder<tChannels, tSize>(previousFrameData, currentFrameData, previousWidth, previousHeight, currentWidth, currentHeight, previousPosition, layerRadiusX, layerRadiusY, previousFramePaddingElements, currentFramePaddingElements, intermediateRoughPoint, metricResult));

					ocean_assert(position.x() < currentWidth && position.y() < currentHeight);

					const Scalar higherWidth = Scalar((*currentPyramid)[l - 1].width());
					const Scalar higherHeight = Scalar((*currentPyramid)[l - 1].height());

					intermediateRoughPoints[i] = Vector2(min(Scalar(position.x() * 2u), higherWidth - 1), min(Scalar(position.y() * 2u), higherHeight - 1));

					ocean_assert(intermediateRoughPoints[i].x() >= 0 && intermediateRoughPoints[i].x() < Scalar(higherWidth));
					ocean_assert(intermediateRoughPoints[i].y() >= 0 && intermediateRoughPoints[i].y() < Scalar(higherHeight));
				}
				else
				{
					intermediateRoughPoints[i] = Vector2((*previousPoints)[i].x() * layerFactor * Scalar(2), (*previousPoints)[i].y() * layerFactor * Scalar(2));

					ocean_assert(intermediateRoughPoints[i].x() >= 0 && intermediateRoughPoints[i].x() < Scalar(currentFrame.width() * 2u));
					ocean_assert(intermediateRoughPoints[i].y() >= 0 && intermediateRoughPoints[i].y() < Scalar(currentFrame.height() * 2u));
				}
			}
		}
	}
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
void AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsMaskSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const FramePyramid* previousMaskPyramid, const FramePyramid* currentMaskPyramid, const unsigned int numberLayers, const PixelPositions* previousPoints, const PixelPositions* roughCurrentPoints, PixelPositions* currentPoints, const unsigned int coarsestLayerRadius, const unsigned int firstPoint, const unsigned int numberPoints)
{
	static_assert(tSize % 2u == 1u, "Invalid patch size, must be odd!");

	ocean_assert(previousPyramid && currentPyramid && previousMaskPyramid && currentMaskPyramid);
	ocean_assert(previousPoints && roughCurrentPoints && currentPoints);

	ocean_assert(*previousPyramid && *currentPyramid && *previousMaskPyramid && *currentMaskPyramid);

	ocean_assert(previousPyramid->frameType().pixelFormat() == currentPyramid->frameType().pixelFormat());
	ocean_assert(previousPyramid->frameType().pixelOrigin() == currentPyramid->frameType().pixelOrigin());

	ocean_assert(previousMaskPyramid->frameType().pixelFormat() == currentMaskPyramid->frameType().pixelFormat());
	ocean_assert(previousMaskPyramid->frameType().pixelOrigin() == currentMaskPyramid->frameType().pixelOrigin());

	ocean_assert(roughCurrentPoints->empty() || previousPoints->size() == roughCurrentPoints->size());
	ocean_assert(currentPoints->size() == previousPoints->size());

	ocean_assert(firstPoint + numberPoints <= previousPoints->size());

	ocean_assert(numberLayers >= 1u);
	ocean_assert(numberLayers <= previousPyramid->validLayers());
	ocean_assert(numberLayers <= currentPyramid->validLayers());
	ocean_assert(numberLayers <= previousMaskPyramid->validLayers());
	ocean_assert(numberLayers <= currentMaskPyramid->validLayers());

	ocean_assert(previousPyramid->layer(numberLayers - 1u).width() >= tSize / 2u);
	ocean_assert(previousPyramid->layer(numberLayers - 1u).height() >= tSize / 2u);

	ShiftVector<PixelPosition> intermediateRoughPoints(firstPoint, numberPoints);

	const unsigned int lowestCurrentWidth = currentPyramid->layer(numberLayers - 1u).width();
	const unsigned int lowestCurrentHeight = currentPyramid->layer(numberLayers - 1u).height();
	const unsigned int lowestLayerFactor = currentPyramid->sizeFactor(numberLayers - 1u);
	ocean_assert(lowestCurrentWidth >= 1u && lowestCurrentHeight >= 1u);

	const unsigned int channels = previousPyramid->frameType().channels();
	ocean_assert(channels >= 1u && channels <= 4u);

	for (unsigned int n = firstPoint; n < firstPoint + numberPoints; ++n)
	{
		const PixelPosition& roughPoint = roughCurrentPoints->empty() ? (*previousPoints)[n] : (*roughCurrentPoints)[n];

		const unsigned int x = min((roughPoint.x() + lowestLayerFactor / 2u) / lowestLayerFactor, lowestCurrentWidth - 1u);
		const unsigned int y = min((roughPoint.y() + lowestLayerFactor / 2u) / lowestLayerFactor, lowestCurrentHeight - 1u);

		intermediateRoughPoints[n] = PixelPosition(x, y);
	}

	for (int l = int(numberLayers) - 1; l >= 0; --l)
	{
		const unsigned int layerRadius = (l == int(numberLayers) - 1) ? coarsestLayerRadius : 2u;

		const LegacyFrame& previousFrame = (*previousPyramid)[l];
		const LegacyFrame& currentFrame = (*currentPyramid)[l];

		const LegacyFrame& previousMask = (*previousMaskPyramid)[l];
		const LegacyFrame& currentMask = (*currentMaskPyramid)[l];

		const uint8_t* const previousData = previousFrame.constdata<uint8_t>();
		const uint8_t* const currentData = currentFrame.constdata<uint8_t>();

		const uint8_t* const previousMaskData = previousMask.constdata<uint8_t>();
		const uint8_t* const currentMaskData = currentMask.constdata<uint8_t>();

		ocean_assert(previousFrame.width() == previousMask.width());
		ocean_assert(previousFrame.height() == previousMask.height());

		ocean_assert(currentFrame.width() == previousMask.width());
		ocean_assert(currentFrame.height() == previousMask.height());

		const unsigned int previousWidth = previousFrame.width();
		const unsigned int previousHeight = previousFrame.height();

		const unsigned int currentWidth = currentFrame.width();
		const unsigned int currentHeight = currentFrame.height();

		for (unsigned int i = firstPoint; i < firstPoint + numberPoints; ++i)
		{
			ocean_assert(intermediateRoughPoints[i].x() < currentWidth && intermediateRoughPoints[i].y() < currentHeight);

			PixelPosition& intermediateRoughPoint = intermediateRoughPoints[i];

			const unsigned int layerFactor = (unsigned int)(1 << l);
			const PixelPosition previousPosition(min(((*previousPoints)[i].x() + layerFactor / 2u) / layerFactor, previousWidth - 1u),
													min(((*previousPoints)[i].y() + layerFactor / 2u) / layerFactor, previousHeight - 1u));

			if (previousPosition.x() < previousWidth && previousPosition.y() < previousHeight)
			{
				const PixelPosition position(pointMotionMask<tSize>(previousData, currentData, previousMaskData, currentMaskData, channels, previousWidth, previousHeight, currentWidth, currentHeight, previousPosition, layerRadius, intermediateRoughPoint));

				ocean_assert(position.x() < currentWidth && position.y() < currentHeight);

				// if we are back on the finest layer, than we store the tracked point as final result
				if (l == 0)
				{
					ocean_assert(i < currentPoints->size());
					(*currentPoints)[i] = position;
				}
				else
				{
					// we store the tracked point as rough intermediate point for the next finer layer
					const unsigned int higherWidth = currentPyramid->layer(l - 1).width();
					const unsigned int higherHeight = currentPyramid->layer(l - 1).height();

					intermediateRoughPoint = PixelPosition(min(position.x() * 2u, higherWidth - 1u), min(position.y() * 2u, higherHeight - 1u));
				}
			}
			else
			{
				// if we are back on the finest layer, than we store the rough point as final result, otherwise we guess the rough intermediate point for the next layer
				if (l == 0)
				{
					ocean_assert(i < currentPoints->size());
					(*currentPoints)[i] = intermediateRoughPoint;
				}
				else
				{
					intermediateRoughPoint = PixelPosition((intermediateRoughPoint.x() * 2u + layerFactor / 2u) / layerFactor,
															(intermediateRoughPoint.y() * 2u + layerFactor / 2u) / layerFactor);
				}
			}
		}
	}
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tSize>
void AdvancedMotion<TMetricInteger, TMetricFloat>::trackPointsSubPixelMaskSubset(const FramePyramid* previousPyramid, const FramePyramid* currentPyramid, const FramePyramid* previousMaskPyramid, const FramePyramid* currentMaskPyramid, const unsigned int numberLayers, const Vectors2* previousPoints, const Vectors2* roughCurrentPoints, Vectors2* currentPoints, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, const unsigned int firstPoint, const unsigned int numberPoints)
{
	static_assert(tSize % 2u == 1u, "Invalid patch size, must be odd!");

	ocean_assert(previousPyramid && currentPyramid);
	ocean_assert(previousMaskPyramid && currentMaskPyramid);
	ocean_assert(previousPoints && roughCurrentPoints && currentPoints);

	ocean_assert(*previousPyramid && *currentPyramid);
	ocean_assert(*previousMaskPyramid && *currentMaskPyramid);

	ocean_assert(previousPyramid->frameType().pixelFormat() == currentPyramid->frameType().pixelFormat());
	ocean_assert(previousPyramid->frameType().pixelOrigin() == currentPyramid->frameType().pixelOrigin());

	ocean_assert(previousMaskPyramid->frameType().pixelFormat() == currentMaskPyramid->frameType().pixelFormat());
	ocean_assert(previousMaskPyramid->frameType().pixelOrigin() == currentMaskPyramid->frameType().pixelOrigin());

	ocean_assert(roughCurrentPoints->empty() || previousPoints->size() == roughCurrentPoints->size());
	ocean_assert(currentPoints->size() == previousPoints->size());

	ocean_assert(firstPoint + numberPoints <= previousPoints->size());

	ocean_assert(numberLayers >= 1u);
	ocean_assert(numberLayers <= previousPyramid->validLayers());
	ocean_assert(numberLayers <= currentPyramid->validLayers());
	ocean_assert(numberLayers <= previousMaskPyramid->validLayers());
	ocean_assert(numberLayers <= currentMaskPyramid->validLayers());

	ocean_assert(previousPyramid->layer(numberLayers - 1u).width() >= tSize / 2u);
	ocean_assert(previousPyramid->layer(numberLayers - 1u).height() >= tSize / 2u);

	ShiftVector<Vector2> intermediateRoughPoints(firstPoint, numberPoints);

	const Scalar lowestCurrentWidth = Scalar(currentPyramid->layer(numberLayers - 1u).width());
	const Scalar lowestCurrentHeight = Scalar(currentPyramid->layer(numberLayers - 1u).height());
	const Scalar lowestLayerFactor = Scalar(1) / Scalar(currentPyramid->sizeFactor(numberLayers - 1u));
	ocean_assert(lowestCurrentWidth >= 1u && lowestCurrentHeight >= 1u);

	const unsigned int channels = previousPyramid->frameType().channels();
	ocean_assert(channels >= 1u && channels <= 4u);

	for (unsigned int n = firstPoint; n < firstPoint + numberPoints; ++n)
	{
		const Vector2& roughPoint = roughCurrentPoints->empty() ? (*previousPoints)[n] : (*roughCurrentPoints)[n];

		const Scalar x = min(roughPoint.x() * lowestLayerFactor, lowestCurrentWidth - 1);
		const Scalar y = min(roughPoint.y() * lowestLayerFactor, lowestCurrentHeight - 1);

		intermediateRoughPoints[n] = Vector2(x, y);
	}

	for (int l = int(numberLayers) - 1; l >= 0; --l)
	{
		const LegacyFrame& previousFrame = (*previousPyramid)[l];
		const LegacyFrame& currentFrame = (*currentPyramid)[l];

		const LegacyFrame& previousMask = (*previousMaskPyramid)[l];
		const LegacyFrame& currentMask = (*currentMaskPyramid)[l];

		const uint8_t* const previousData = previousFrame.constdata<uint8_t>();
		const uint8_t* const currentData = currentFrame.constdata<uint8_t>();

		const uint8_t* const previousMaskData = previousMask.constdata<uint8_t>();
		const uint8_t* const currentMaskData = currentMask.constdata<uint8_t>();

		ocean_assert(previousFrame.width() == previousMask.width());
		ocean_assert(previousFrame.height() == previousMask.height());

		ocean_assert(currentFrame.width() == previousMask.width());
		ocean_assert(currentFrame.height() == previousMask.height());

		const unsigned int previousWidth = previousFrame.width();
		const unsigned int previousHeight = previousFrame.height();

		const unsigned int currentWidth = currentFrame.width();
		const unsigned int currentHeight = currentFrame.height();

		// if the finest layer is reached we apply a subpixel accurate determination
		if (l == 0)
		{
			const unsigned int layerRadius = numberLayers == 1u ? coarsestLayerRadius : 2u;

			for (unsigned int i = firstPoint; i < firstPoint + numberPoints; ++i)
			{
				ocean_assert(intermediateRoughPoints[i].x() >= Scalar(0) && intermediateRoughPoints[i].y() >= Scalar(0));
				ocean_assert(intermediateRoughPoints[i].x() < Scalar(currentWidth) && intermediateRoughPoints[i].y() < Scalar(currentHeight));

				const Vector2& intermediateRoughPoint = intermediateRoughPoints[i];

				const Vector2& previousPosition = (*previousPoints)[i];

				ocean_assert(previousPosition.x() >= Scalar(0) && previousPosition.y() >= Scalar(0));
				ocean_assert(previousPosition.x() < Scalar(previousWidth) && previousPosition.y() < Scalar(previousHeight));

				const Vector2 position(pointMotionInFrameSubPixelMask<tSize>(previousData, currentData, previousMaskData, currentMaskData, channels, previousWidth, previousHeight, currentWidth, currentHeight, previousPosition, layerRadius, intermediateRoughPoint, subPixelIterations));

				ocean_assert(i < currentPoints->size());
				(*currentPoints)[i] = position;
			}
		}
		else // otherwise we apply a pixel accurate determination
		{
			ocean_assert(l > 0);

			const unsigned int layerRadius = (l == int(numberLayers) - 1) ? coarsestLayerRadius : 2u;

			for (unsigned int i = firstPoint; i < firstPoint + numberPoints; ++i)
			{
				ocean_assert(intermediateRoughPoints[i].x() >= Scalar(0) && intermediateRoughPoints[i].y() >= Scalar(0));
				ocean_assert(intermediateRoughPoints[i].x() < Scalar(currentWidth) && intermediateRoughPoints[i].y() < Scalar(currentHeight));

				const PixelPosition intermediateRoughPoint(Numeric::round32(intermediateRoughPoints[i].x()), Numeric::round32(intermediateRoughPoints[i].y()));

				const Scalar layerFactor = Scalar(1) / Scalar((unsigned int)(1 << l));
				const PixelPosition previousPosition(min(Numeric::round32((*previousPoints)[i].x() * layerFactor), int(previousWidth) - 1),
														min(Numeric::round32((*previousPoints)[i].y() * layerFactor), int(previousHeight) - 1));

				if (previousPosition.x() < previousWidth && previousPosition.y() < previousHeight)
				{
					const PixelPosition position(pointMotionMask<tSize>(previousData, currentData, previousMaskData, currentMaskData, channels, previousWidth, previousHeight, currentWidth,
																													currentHeight, previousPosition, layerRadius, intermediateRoughPoint));

					ocean_assert(position.x() < currentWidth && position.y() < currentHeight);

					const Scalar higherWidth = Scalar((*currentPyramid)[l - 1].width());
					const Scalar higherHeight = Scalar((*currentPyramid)[l - 1].height());

					intermediateRoughPoints[i] = Vector2(min(Scalar(position.x() * 2u), higherWidth - 1), min(Scalar(position.y() * 2u), higherHeight - 1));

					ocean_assert(intermediateRoughPoints[i].x() >= 0 && intermediateRoughPoints[i].x() < Scalar(higherWidth));
					ocean_assert(intermediateRoughPoints[i].y() >= 0 && intermediateRoughPoints[i].y() < Scalar(higherHeight));
				}
				else
				{
					intermediateRoughPoints[i] = Vector2((*previousPoints)[i].x() * layerFactor * Scalar(2), (*previousPoints)[i].y() * layerFactor * Scalar(2));

					ocean_assert(intermediateRoughPoints[i].x() >= 0 && intermediateRoughPoints[i].x() < Scalar(currentFrame.width() * 2u));
					ocean_assert(intermediateRoughPoints[i].y() >= 0 && intermediateRoughPoints[i].y() < Scalar(currentFrame.height() * 2u));
				}
			}
		}
	}
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tSize>
PixelPosition AdvancedMotion<TMetricInteger, TMetricFloat>::pointMotionMask(const unsigned char* frame0, const unsigned char* frame1, const unsigned char* mask0, const unsigned char* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const PixelPosition& position0, const unsigned int radius, const PixelPosition& rough1)
{
	static_assert(tChannels != 0u, "Invalid number of data channels!");
	static_assert(tSize % 2u == 1u, "Invalid size of the image patch, must be odd!");

	ocean_assert(frame0 && frame1);
	ocean_assert(radius != 0u);

	ocean_assert(width0 >= tSize / 2u && height0 >= tSize / 2u);
	ocean_assert(width1 >= tSize / 2u && height1 >= tSize / 2u);

	ocean_assert(position0.x() < width0);
	ocean_assert(position0.y() < height0);

	const PixelPosition position1((rough1.x() < width1 && rough1.y() < height1) ? rough1 : position0);
	ocean_assert(position1.x() < width1);
	ocean_assert(position1.y() < height1);

	const unsigned int leftCenter1 = max(0, int(position1.x() - radius));
	const unsigned int topCenter1 = max(0, int(position1.y() - radius));

	const unsigned int rightCenter1 = min(position1.x() + radius, width1 - 1u);
	const unsigned int bottomCenter1 = min(position1.y() + radius, height1 - 1u);

	ocean_assert(leftCenter1 < width1);
	ocean_assert(topCenter1 < height1);

	ocean_assert(leftCenter1 <= rightCenter1);
	ocean_assert(topCenter1 <= bottomCenter1);

	ocean_assert(rightCenter1 < width1);
	ocean_assert(bottomCenter1 < height1);

	const unsigned int sizeHalf = tSize / 2u;

	unsigned int metricMin = (unsigned int)(-1);
	unsigned int sqrDistanceMin = (unsigned int)(-1);
	PixelPosition minPosition;

	for (unsigned int y1 = topCenter1; y1 <= bottomCenter1; ++y1)
	{
		for (unsigned int x1 = leftCenter1; x1 <= rightCenter1; ++x1)
		{
			const unsigned int metric = AdvancedZeroMeanSumSquareDifferences::determine8BitPerChannelPartialTemplate<tChannels>(frame0, frame1, mask0, mask1, width0, height0, width1, height1, tSize, tSize, position0.x() - sizeHalf, position0.y() - sizeHalf, x1 - sizeHalf, y1 - sizeHalf).first;

			const PixelPosition position(x1, y1);

			if (metric < metricMin || (metric == metricMin && position1.sqrDistance(position)) < sqrDistanceMin)
			{
				metricMin = metric;
				minPosition = position;
				sqrDistanceMin = position1.sqrDistance(position);
			}
		}
	}

	ocean_assert(metricMin != (unsigned int)(-1) && minPosition);

	ocean_assert(abs(int(minPosition.x()) - int(position1.x())) <= int(radius));
	ocean_assert(abs(int(minPosition.y()) - int(position1.y())) <= int(radius));

	return minPosition;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tPatchSize>
Vector2 AdvancedMotion<TMetricInteger, TMetricFloat>::pointMotionInFrameSubPixelMask(const uint8_t* frame0, const uint8_t* frame1, const uint8_t* mask0, const uint8_t* mask1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const Vector2& position0, const unsigned int radiusX, const unsigned int radiusY, const Vector2& rough1, const unsigned int subPixelIterations)
{
	static_assert(tChannels != 0u, "Invalid number of data channels!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size of the image patch, must be odd!");

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	constexpr unsigned int frame0PaddingElements = 0u;
	constexpr unsigned int frame1PaddingElements = 0u;
	constexpr unsigned int mask0PaddingElements = 0u;
	constexpr unsigned int mask1PaddingElements = 0u;

	ocean_assert(frame0 != nullptr && frame1 != nullptr);
	ocean_assert(mask0 != nullptr && mask1 != nullptr);

	ocean_assert(radiusX != 0u || radiusY != 0u);

	ocean_assert_and_suppress_unused(width0 >= tPatchSize_2 && height0 >= tPatchSize_2, tPatchSize_2);
	ocean_assert_and_suppress_unused(width1 >= tPatchSize_2 && height1 >= tPatchSize_2, tPatchSize_2);

	ocean_assert(position0.x() >= Scalar(0) && position0.x() < width0);
	ocean_assert(position0.y() >= Scalar(0) && position0.y() < height0);

	const Vector2 position1(rough1.x() != Numeric::maxValue() ? rough1 : position0);

	ocean_assert(position1.x() >= Scalar(0) && position1.x() < width1);
	ocean_assert(position1.y() >= Scalar(0) && position1.y() < height1);

	const Scalar leftCenter1 = max(Scalar(0), position1.x() - Scalar(radiusX));
	const Scalar topCenter1 = max(Scalar(0), position1.y() - Scalar(radiusY));

	const Scalar rightCenter1 = min(position1.x() + Scalar(radiusX), Scalar(width1 - 1u));
	const Scalar bottomCenter1 = min(position1.y() + Scalar(radiusY), Scalar(height1 - 1u));

	uint32_t bestMetric = uint32_t(-1);
	Scalar bestSqrDistance = Numeric::maxValue();
	Vector2 bestPosition(Numeric::minValue(), Numeric::minValue());

	for (Scalar y1 = topCenter1; y1 < bottomCenter1 + Numeric::eps(); ++y1)
	{
		for (Scalar x1 = leftCenter1; x1 < rightCenter1 + Numeric::eps(); ++x1)
		{
			const uint32_t candidateMetric = AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate<tChannels, tPatchSize>(frame0, frame1, mask0, mask1, width0, height0, width1, height1, position0.x(), position0.y(), x1, y1, frame0PaddingElements, frame1PaddingElements, mask0PaddingElements, mask1PaddingElements).first;

			const Vector2 candidatePosition(x1, y1);

			if (candidateMetric < bestMetric || (candidateMetric == bestMetric && position1.sqrDistance(candidatePosition) < bestSqrDistance))
			{
				bestMetric = candidateMetric;
				bestPosition = candidatePosition;

				bestSqrDistance = position1.sqrDistance(candidatePosition);
			}
		}
	}

	ocean_assert(bestMetric != Numeric::maxValue());

	ocean_assert(abs(int(bestPosition.x()) - int(position1.x())) <= int(radiusX));
	ocean_assert(abs(int(bestPosition.y()) - int(position1.y())) <= int(radiusY));

	constexpr unsigned int subPixelSampels = 8u;

	const Vector2 steps[subPixelSampels] =
	{
		Vector2(-1, -1),
		Vector2(0, -1),
		Vector2(1, -1),
		Vector2(-1, 0),
		Vector2(1, 0),
		Vector2(-1, 1),
		Vector2(0, 1),
		Vector2(1, 1)
	};

	Scalar offset = Scalar(0.5);
	Vector2 iterationPosition1(bestPosition);

	for (unsigned int n = 0u; n < subPixelIterations; ++n)
	{
		Vector2 betterPosition1(iterationPosition1);

		for (unsigned int i = 0u; i < subPixelSampels; ++i)
		{
			const Vector2 candidatePosition1(iterationPosition1.x() + steps[i].x() * offset, iterationPosition1.y() + steps[i].y() * offset);

			const uint32_t candidateMetric = AdvancedSumSquareDifferencesBase::patchWithMask8BitPerChannelTemplate<tChannels, tPatchSize>(frame0, frame1, mask0, mask1, width0, height0, width1, height1, position0.x(), position0.y(), candidatePosition1.x(), candidatePosition1.y(), frame0PaddingElements, frame1PaddingElements, mask0PaddingElements, mask1PaddingElements).first;

			if (candidateMetric < bestMetric)
			{
				bestMetric = candidateMetric;
				betterPosition1 = candidatePosition1;
			}
		}

		iterationPosition1 = betterPosition1;
		offset *= Scalar(0.5);
	}

	return iterationPosition1;
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tPatchSize>
Vector2 AdvancedMotion<TMetricInteger, TMetricFloat>::pointMotionInFrameSubPixelMirroredBorder(const uint8_t* frame0, const uint8_t* frame1, const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const unsigned int frame0PaddingElements, const unsigned int frame1PaddingElements, const Vector2& position0, const unsigned int radiusX, const unsigned int radiusY, const Vector2& rough1, const unsigned int subPixelIterations, uint32_t* metricResult, uint32_t* metricIdentityResult)
{
	static_assert(tChannels != 0u, "Invalid number of data channels!");
	static_assert(tPatchSize % 2u == 1u, "Invalid size of the image patch, must be odd!");

	constexpr unsigned int tPatchSize_2 = tPatchSize / 2u;

	ocean_assert(frame0 != nullptr && frame1 != nullptr);

	// **TODO** correct this assert due to the mirrored patch and correct the documentation -> also interpolateSquareMirroredBorder()...
	ocean_assert(width0 >= tPatchSize && height0 >= tPatchSize);
	ocean_assert(width1 >= tPatchSize && height1 >= tPatchSize);

	ocean_assert(position0.x() >= Scalar(0) && position0.x() < Scalar(width0));
	ocean_assert(position0.y() >= Scalar(0) && position0.y() < Scalar(height0));

	const PixelPosition position1(rough1.x() != Numeric::maxValue() ? PixelPosition::vector2pixelPosition(rough1) : PixelPosition::vector2pixelPosition(position0));

	const unsigned int leftCenter1 = (unsigned int)(max(0, int(position1.x() - radiusX)));
	const unsigned int topCenter1 = (unsigned int)(max(0, int(position1.y() - radiusY)));

	const unsigned int rightCenter1 = min(position1.x() + radiusX, width1 - 1u);
	const unsigned int bottomCenter1 = min(position1.y() + radiusY, height1 - 1u);

	// first, we determine a buffer containing the first (interpolated) image patch

	uint8_t buffer0[tPatchSize * tPatchSize * tChannels];
	uint8_t buffer1[tPatchSize * tPatchSize * tChannels];

	const unsigned int x0 = (unsigned int)(position0.x());
	const unsigned int y0 = (unsigned int)(position0.y());

	if (x0 - tPatchSize_2 < width0 - tPatchSize && y0 - tPatchSize_2 < height0 - tPatchSize)
	{
		ocean_assert(x0 >= tPatchSize_2 && x0 < width0 - (tPatchSize_2 + 1u) && y0 >= tPatchSize_2 && y0 < height0 - (tPatchSize_2 + 1u));
		AdvancedFrameInterpolatorBilinear::interpolateSquarePatch8BitPerChannel<tChannels, tPatchSize>(frame0, width0, frame0PaddingElements, buffer0, position0);
	}
	else
	{
		ocean_assert(!(x0 >= tPatchSize_2 && x0 < width0 - (tPatchSize_2 + 1u) && y0 >= tPatchSize_2 && y0 < height0 - (tPatchSize_2 + 1u)));
		AdvancedFrameInterpolatorBilinear::interpolateSquareMirroredBorder8BitPerChannel<tChannels, tPatchSize>(frame0, width0, height0, frame0PaddingElements, buffer0, position0);
	}

	PixelPosition bestPosition;
	uint32_t bestMetric = uint32_t(-1);
	unsigned int bestSqrDistance = (unsigned int)(-1);

	for (unsigned int y1 = topCenter1; y1 <= bottomCenter1; ++y1)
	{
		for (unsigned int x1 = leftCenter1; x1 <= rightCenter1; ++x1)
		{
			uint32_t candidateMetric;

			if (x1 - tPatchSize_2 < width1 - tPatchSize && y1 - tPatchSize_2 < height1 - tPatchSize)
			{
				candidateMetric = TMetricInteger::template patchBuffer8BitPerChannel<tChannels, tPatchSize>(frame1, width1, x1, y1, frame1PaddingElements, buffer0);
			}
			else
			{
				constexpr unsigned int buffer1PaddingElements = 0u;

				FrameConverter::patchFrameMirroredBorder<uint8_t, tChannels>(frame1, buffer1, width1, height1, x1, y1, tPatchSize, frame1PaddingElements, buffer1PaddingElements); // **TODO** for performance improvements: use patch/patch mirrored border metric

				candidateMetric = TMetricInteger::template buffer8BitPerChannel<tChannels, tPatchSize * tPatchSize>(buffer0, buffer1);
			}

			const PixelPosition position(x1, y1);

			if (candidateMetric < bestMetric || (candidateMetric == bestMetric && position1.sqrDistance(position) < bestSqrDistance))
			{
				bestMetric = candidateMetric;
				bestPosition = position;

				bestSqrDistance = position1.sqrDistance(position);
			}

			if (metricIdentityResult && x1 == position1.x() && y1 == position1.y())
			{
				*metricIdentityResult = candidateMetric;
			}
		}
	}

	ocean_assert(bestMetric != (unsigned int)(-1) && bestPosition);

	ocean_assert(abs(int(bestPosition.x()) - int(position1.x())) <= int(radiusX));
	ocean_assert(abs(int(bestPosition.y()) - int(position1.y())) <= int(radiusY));

	if (metricResult)
	{
		*metricResult = bestMetric;
	}

	return pointMotionSubPixelMirroredBorder<tChannels, tPatchSize>(buffer0, frame1, width1, height1, frame1PaddingElements, Vector2(Scalar(bestPosition.x()), Scalar(bestPosition.y())), subPixelIterations, metricResult);
}

template <typename TMetricInteger, typename TMetricFloat>
template <unsigned int tChannels, unsigned int tSize>
Vector2 AdvancedMotion<TMetricInteger, TMetricFloat>::pointMotionSubPixelMirroredBorder(const uint8_t* buffer0, const uint8_t* frame1, const unsigned int width1, const unsigned int height1, const unsigned int frame1PaddingElements, const Vector2& position1, const unsigned int subPixelIterations, unsigned int* metricResult)
{
	static_assert(tChannels != 0u, "Invalid number of data channels!");
	static_assert(tSize % 2u == 1u, "Invalid size of the image patch, must be odd!");

	ocean_assert(buffer0 != nullptr && frame1 != nullptr);

	ocean_assert(width1 >= tSize && height1 >= tSize);

	ocean_assert(position1.x() >= 0 && position1.x() < Scalar(width1));
	ocean_assert(position1.y() >= 0 && position1.y() < Scalar(height1));

	unsigned int metricBest = (unsigned int)(-1);

	if (metricResult)
	{
		metricBest = *metricResult;

#ifdef OCEAN_DEBUG
		const bool result = metricBest == TMetricFloat::template patchMirroredBorderBuffer8BitPerChannel<tChannels, tSize>(frame1, width1, height1, position1.x(), position1.y(), frame1PaddingElements, buffer0);
		ocean_assert_and_suppress_unused(result, result);
#endif
	}
	else
	{
		const unsigned int x1 = (unsigned int)position1.x();
		const unsigned int y1 = (unsigned int)position1.y();

		if (x1 - (tSize / 2u) < width1 - tSize && y1 - (tSize / 2u) < height1 - tSize)
		{
			ocean_assert(x1 >= (tSize / 2u) && y1 >= (tSize / 2u) && x1 < width1 - (tSize / 2u + 1u) && y1 < height1 - (tSize / 2u + 1u));
			metricBest = TMetricFloat::template patchBuffer8BitPerChannel<tChannels, tSize>(frame1, width1, position1.x(), position1.y(), frame1PaddingElements, buffer0);
		}
		else
		{
			ocean_assert(!(x1 >= (tSize / 2u) && y1 >= (tSize / 2u) && x1 < width1 - (tSize / 2u + 1u) && y1 < height1 - (tSize / 2u + 1u)));
			metricBest = TMetricFloat::template patchMirroredBorderBuffer8BitPerChannel<tChannels, tSize>(frame1, width1, height1, position1.x(), position1.y(), frame1PaddingElements, buffer0);
		}
	}

	const Vector2 steps[8] =
	{
		Vector2(-1, -1),
		Vector2(0, -1),
		Vector2(1, -1),
		Vector2(-1, 0),
		Vector2(1, 0),
		Vector2(-1, 1),
		Vector2(0, 1),
		Vector2(1, 1)
	};

	Scalar offset = Scalar(0.5);
	Vector2 iterationPosition1(position1);

	for (unsigned int n = 0u; n < subPixelIterations; ++n)
	{
		Vector2 betterPosition1(iterationPosition1);

		// make 8 sample calculations

		for (unsigned int i = 0u; i < 8u; ++i)
		{
			const Vector2 testPosition1(iterationPosition1.x() + steps[i].x() * offset, iterationPosition1.y() + steps[i].y() * offset);

			if (testPosition1.x() >= 0 && testPosition1.x() < Scalar(width1) && testPosition1.y() >= 0 && testPosition1.y() < Scalar(height1))
			{
				const unsigned int x1 = (unsigned int)(testPosition1.x());
				const unsigned int y1 = (unsigned int)(testPosition1.y());

				const unsigned int metric = (x1 - (tSize / 2u) < width1 - tSize && y1 - (tSize / 2u) < height1 - tSize) ?
									TMetricFloat::template patchBuffer8BitPerChannel<tChannels, tSize>(frame1, width1, testPosition1.x(), testPosition1.y(), frame1PaddingElements, buffer0) :
									TMetricFloat::template patchMirroredBorderBuffer8BitPerChannel<tChannels, tSize>(frame1, width1, height1, testPosition1.x(), testPosition1.y(), frame1PaddingElements, buffer0);

				if (metric < metricBest)
				{
					metricBest = metric;
					betterPosition1 = testPosition1;
				}

			}
		}

		iterationPosition1 = betterPosition1;
		offset *= Scalar(0.5);
	}

	if (metricResult)
	{
		*metricResult = metricBest;
	}

	ocean_assert(iterationPosition1.x() >= 0 && iterationPosition1.y() >= 0);
	ocean_assert(iterationPosition1.x() < Scalar(width1) && iterationPosition1.y() < Scalar(height1));

	return iterationPosition1;
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_MOTION_H
