/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SPHERICAL_ENVIRONMENT_H
#define META_OCEAN_TRACKING_SPHERICAL_ENVIRONMENT_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Callback.h"

#include "ocean/cv/advanced/PanoramaFrame.h"

#include "ocean/geometry/Estimator.h"
#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a spherical environment based on a panorama frame.
 * The environment can be extended by new camera frame with unknown orientation as long as the orientation offset between successive frames is not too large.<br>
 * Further, the environment can be used to determine the orientation of a given camera frame capturing an already known area.<br>
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT SphericalEnvironment : public CV::Advanced::PanoramaFrame
{
	protected:

		/**
		 * Definition of a vector holding 8 bit characters.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Definition of a vector holding buffers.
		 */
		typedef std::vector<Buffer> Buffers;

		/**
		 * Definition of a map mapping indices.
		 */
		typedef std::unordered_map<uint32_t, uint32_t> IndexMap;

		/**
		 * Definition of a map mapping unique frame-feature-point-ids to unique frame-feature-point-ids.
		 */
		typedef std::multimap<uint64_t, uint64_t> FeaturePointMap;

		/**
		 * Forward declaration of a base class for all optimization classes.
		 */
		class CameraData;

		/**
		 * Forward declaration of the data class allowing to optimize the intrinsic and extrinsic camera parameters.
		 */
		class CameraOrientationsData;

		/**
		 * Forward declaration of the data class allowing to find an initial camera field of view.
		 */
		class CameraFovData;

		/**
		 * Forward declaration of a data class allowing to optimize a 2x2 transformation table.
		 */
		class TransformationTableData2x2;

	public:

		/**
		 * Definition of a callback function allowing to determine an update mask for a current camera frame.
		 * Parameter 0: The frame pyramid of the previous frame
		 * Parameter 1: The frame pyramid of the current frame
		 * Parameter 2: The camera profile of the previous frame
		 * Parameter 3: The camera profile of the current frame
		 * Parameter 4: The camera orientation of the previous frame
		 * Parameter 5: The camera orientation of the current frame
		 * Parameter 6: The mask value for static image content
		 * Parameter 7: The resulting update mask frame
		 * Parameter 8: An optional worker object
		 * Return value: True, if the camera frame can be used to update the panorama frame
		 */
		typedef Callback<bool, const CV::FramePyramid&, const CV::FramePyramid&, const PinholeCamera&, const PinholeCamera&, const SquareMatrix3&, const SquareMatrix3&, const uint8_t, Frame&, Worker*> FrameCallback;

		/**
		 * Definition of a vector holding groups of pairs between camera pose ids and image point ids.
		 */
		typedef std::vector<IndexPairs32> PoseImagePointPairGroups;

	public:

		/**
		 * Creates an invalid spherical environment object.
		 */
		SphericalEnvironment() = default;

		/**
		 * Creates a new spherical environment object.
		 * @param panoramaDimensionWidth The width of the entire panorama frame (the maximal possible size) representing horizontal 360 degrees, in pixel with range [1, infinity)
		 * @param panoramaDimensionHeight The height of the entire panorama frame (the maximal possible size) representing vertical 180 degrees, in pixel with range [1, infinity)
		 * @param maskValue The mask value defining the 8 bit pixel value of valid pixels
		 * @param frameMode The update mode of this panorama frame
		 */
		inline SphericalEnvironment(const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const uint8_t maskValue, const UpdateMode frameMode);

		/**
		 * Adds a new camera frame to the panorama frame for which the orientation is unknown.
		 * The frame may be either the first frame of a sequence of frame or a subsequent frame in the sequence as long as the frame content of successive frames does not change too much.<br>
		 * The first frame is assigned with the default camera orientation, the orientation of successive frames is determined automatically.<br>
		 * @param pinholeCamera The camera profile of the current frame
		 * @param frame The camera frame which will be added
		 * @param approximationBinSize Optional width of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param fineAdjustmentEstimator A robust estimator type, if the given camera frame will be distorted up to a small scale so that it fits with the already existing image content in the environment
		 * @param optimizeCamera True, to optimize the given camera profile, False to use the given camera profile
		 * @param worker Optional worker object to distribute the computation
		 * @param orientation Optional the resulting orientation of the given camera frame, with respect to the default orientation of the first camera frame
		 * @param optimizedCamera Optional resulting optimized camera profile
		 * @param frameCallback Optional callback function that is invoked before the panorama frame will be updated and which allows to define a frame mask separating the frame in static and dynamic image areas, only static image content is used to extend the environment
		 * @return True, if succeeded
		 */
		bool extendEnvironment(const PinholeCamera& pinholeCamera, const Frame& frame, const unsigned int approximationBinSize = 20u, const Geometry::Estimator::EstimatorType fineAdjustmentEstimator = Geometry::Estimator::ET_INVALID, const bool optimizeCamera = false, Worker* worker = nullptr, SquareMatrix3* orientation = nullptr, PinholeCamera* optimizedCamera = nullptr, const FrameCallback& frameCallback = FrameCallback());

		/**
		 * Determines the precise orientation of a given camera frame.
		 * @param pinholeCamera The camera profile of the current frame, must be valid
		 * @param orientation The rough orientation of the given frame, in relation to the spherical environment, must be valid
		 * @param frame The camera frame which will be added, must be valid
		 * @param mask An optional mask defining valid and invalid pixels in the given frame, may be invalid if all pixels are valid
		 * @param estimator The estimator type which is applied to determine the precise orientation
		 * @param optimizedOrientation The resulting precise orientation best matching to the given camera frame
		 * @param optimizedCamera Optional resulting optimized camera profile, nullptr if the given camera profile is not optimized internally
		 * @param fineAdjustment Optional resulting transformation lookup table with relative offsets providing a fine adjustment for the camera frame
		 * @param approximationBinSize Optional width of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool optimizeOrientation(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Frame& frame, const Frame& mask, const Geometry::Estimator::EstimatorType estimator, SquareMatrix3& optimizedOrientation, PinholeCamera* optimizedCamera = nullptr, LookupTable* fineAdjustment = nullptr, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr);

		/**
		 * Clears the panorama frame and allows to set a new first camera frame.
		 * @see PanoramaFrame::clear().
		 */
		virtual void clear();

		/**
		 * Determines point correspondences between two camera frames captured with individual locations (describable by a homography) by application of a pyramid-based patch tracking approach.
		 * The given homography can be a full 8-DOF homography including e.g., scale and projection.
		 * @param sourceFrame The source frame for which the correspondences will be determined, must be valid
		 * @param targetFrame The second frame for which the correspondences will be determine, with same pixel format as the first frame, must be valid
		 * @param homography The known homography between both frames, transforming points defined in the first frame to points defined in the second frame (pointTargetFrame = H * pointSourceFrame), must be valid
		 * @param sourcePoints The resulting points of the correspondences, defined in the source frame
		 * @param targetPoints The resulting points of the correspondences, defined in the target frame, one for each source point
		 * @param patchSize The size of the patches to be used for tracking, possible values are 7, 15 or 31
		 * @param maximalDistance Maximal expected distance (radius) between two tracked correspondences in pixel, with range [1, infinity)
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param downsamplingMode The downsampling mode that is applied to create the pyramid layers
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determinePointCorrespondencesHomography(const Frame& sourceFrame, const Frame& targetFrame, const SquareMatrix3& homography, Vectors2& sourcePoints, Vectors2& targetPoints, const unsigned int patchSize, const unsigned int maximalDistance = 32u, const unsigned int coarsestLayerRadius = 8u, const CV::FramePyramid::DownsamplingMode downsamplingMode = CV::FramePyramid::DM_FILTER_14641, Worker* worker = nullptr);

		/**
		 * Determines point correspondences between two camera frames captured with individual locations (describable by a homography) by application of a pyramid-based patch tracking approach.
		 * The given homography can be a full 8-DOF homography including e.g., scale and projection.
		 * @param sourceFramePyramid The frame pyramid of the source frame for which the correspondences will be determined, should hold enough layers, must be valid
		 * @param targetFrame The second frame for which the correspondences will be determine, with same pixel format as the first frame, must be valid
		 * @param homography The known homography between both frames, transforming points defined in the first frame to points defined in the second frame (pointTargetFrame = H * pointSourceFrame), must be valid
		 * @param sourcePointCandidates The candidate of source points out of which the resulting correspondences will be created, defined in the source frame
		 * @param validSourcePoints The resulting points of the correspondences, defined in the source frame and a subset of sourcePointCandidates
		 * @param validTargetPoints The resulting points of the correspondences, defined in the target frame, one for each source point
		 * @param validSourcePointIndices The resulting indices of the candidates of the source points which actually has been used as correspondences, one for each correspondence
		 * @param patchSize The size of the patches to be used for tracking, possible values are 7, 15 or 31
		 * @param maximalDistance Maximal expected distance (radius) between two tracked correspondences in pixel, with range [1, infinity)
		 * @param coarsestLayerRadius The search radius on the coarsest layer, with range [2, infinity)
		 * @param downsamplingMode The downsampling mode that is applied to create the pyramid layers
		 * @param worker Optional worker object to distribute the computation
		 * @param usedPointCandidates Optional resulting number of point candidates that have been used (e.g., that are located inside the intersection of both frames), with range [0, sourcePointCandidates.size()]
		 * @return True, if succeeded
		 */
		static bool determinePointCorrespondencesHomography(const CV::FramePyramid& sourceFramePyramid, const Frame& targetFrame, const SquareMatrix3& homography, const Vectors2& sourcePointCandidates, Vectors2& validSourcePoints, Vectors2& validTargetPoints, Indices32& validSourcePointIndices, const unsigned int patchSize, const unsigned int maximalDistance = 32u, const unsigned int coarsestLayerRadius = 8u, const CV::FramePyramid::DownsamplingMode downsamplingMode = CV::FramePyramid::DM_FILTER_14641, Worker* worker = nullptr, size_t* usedPointCandidates = nullptr);

		/**
		 * This function determines a mask for image areas not matching with the common homography between two successive camera frames.
		 * An image point does not match with the common homography if the point has a distance larger than 3 pixels to the expected homography point.<br>
		 * @param previousFramePyramid The frame pyramid of the previous camera frame
		 * @param currentFramePyramid The frame pyramid of the current camera frame
		 * @param previousCamera The camera profile of the previous camera frame
		 * @param currentCamera The camera profile of the current camera frame
		 * @param previousOrientation The orientation of the previous camera frame, in relation to the coordinate system of the panorama frame
		 * @param currentOrientation The orientation of the current camera frame, in relation to the coordinate system of the panorama frame
		 * @param maskValue The mask value for static image content
		 * @param currentMask The resulting mask for the current camera frame separating static and dynamic image content
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if a valid mask could be determined
		 */
		static bool nonHomographyMask(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const PinholeCamera& previousCamera, const PinholeCamera& currentCamera, const SquareMatrix3& previousOrientation, const SquareMatrix3& currentOrientation, const uint8_t maskValue, Frame& currentMask, Worker* worker = nullptr);

		/**
		 * Optimizes the camera profile for a given set of camera frames with known orientations so that the offset between corresponding points in the individual camera frames becomes as small as possible.
		 * @param pinholeCamera The camera profile which will be optimized
		 * @param frames The individual camera frame
		 * @param orientations The orientations of the individual camera frames, one orientation for each frame
		 * @param optimizedCamera The resulting optimized camera profile
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if a valid mask could be determined
		 */
		static bool optimizeCamera(const PinholeCamera& pinholeCamera, const Frames& frames, const SquareMatrices3& orientations, PinholeCamera& optimizedCamera, Worker* worker = nullptr);

		/**
		 * Optimizes the camera profile for a given set of image points from individual camera frames so that the offset between the corresponding points becomes as small as possible.
		 * @param pinholeCamera The camera profile which will be optimized
		 * @param orientations The orientations of the individual camera frames, one orientation for each frame
		 * @param imagePoints The entire set of image points holding all image points from all image frames
		 * @param orientationImagePointPairGroups Groups of pairs combining indices from the set of image points with indices of unique feature points, one set of pairs for each camera frame
		 * @param optimizedCamera The resulting camera profile with ideal field of view
		 * @param optimizedOrientations The camera orientations matching with the new camera profile
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged robust (depending on estimator) pixel error for the given initial parameters
		 * @param finalError Optional resulting averaged robust (depending on estimator) pixel error for the final optimized parameters
		 * @param intermediateErrors Optional resulting intermediate (improving) errors
		 * @return True, if the camera profile could be optimized
		 */
		static bool optimizeCamera(const PinholeCamera& pinholeCamera, const SquareMatrices3& orientations, const ImagePoints& imagePoints, const PoseImagePointPairGroups& orientationImagePointPairGroups, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedOrientations, const unsigned int iterations = 20u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr);

		/**
		 * Determines the initial field of view for a set of camera frames with known orientation and a corresponding set of unique features observed in several individual frames.
		 * @param width The width of the camera profile in pixel, with range [1, infinity)
		 * @param height The height of the camera profile in pixel, with range [1, infinity)
		 * @param orientations The known orientations of the individual camera frames
		 * @param imagePoints The entire set of image points observed in individual camera frames
		 * @param orientationImagePointPairGroups Groups of pairs combining indices from the set of image points with indices of unique feature points, one set of pairs for each camera frame
		 * @param optimizedCamera The resulting camera profile with ideal field of view
		 * @param optimizedOrientations The camera orientations matching with the new camera profile
		 * @param lowerFovX The lower bound of the possible horizontal field of view
		 * @param upperFovX The upper bound of the possible horizontal field of view
		 * @param steps The number of steps in which the defined angle range is subdivided
		 * @return True, if succeeded
		 */
		static bool findInitialFieldOfView(const unsigned int width, const unsigned int height, const SquareMatrices3& orientations, const ImagePoints& imagePoints, const PoseImagePointPairGroups& orientationImagePointPairGroups, PinholeCamera& optimizedCamera, SquareMatrices3& optimizedOrientations, const Scalar lowerFovX = Numeric::deg2rad(40), const Scalar upperFovX = Numeric::deg2rad(90), const unsigned int steps = 10u);

		/**
		 * Determines a 2x2 transformation table (a lookup table) transforming a set of points (defined in the domain of the transformation table) to another set of points.
		 * The resulting transformation provides relative transformation offset from one point set to the other point set: points1 = transformation(points0).<br>
		 * @param width The width of the resulting lookup table, with range [1, infinity)
		 * @param height The height of the resulting lookup table, with range [1, infinity)
		 * @param points0 The set of points lying inside the resulting lookup table, the transformation will provide corrected positions for these points, with ranges [0, width)x[0, height)
		 * @param points1 The set of points corresponding to the first set of points but having the correct positions, with ranges (-infinity, infinity)x(-infinity, infinity)
		 * @param transformation0to1 The resulting transformation which will transform points0 to points1
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, in relation to the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, in relation to the defined estimator
		 * @return True, if succeeded
		 */
		static bool determineTransformationTable2x2(const unsigned int width, const unsigned int height, const Vectors2& points0, const Vectors2& points1, LookupCorner2<Vector2>& transformation0to1, const unsigned int iterations = 20u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);

	protected:

		/**
		 * Interpolates square image patches with sub-pixel position and stores the image content as patch buffer.
		 * @param frame The frame in which the patches are located, must be valid
		 * @param positions The center positions of the individual patches, with ranges [tSize / 2, frame.width() - tSize / 2 - 1)x[tSize / 2, frame.height() - tSize / 2 - 1)
		 * @param worker Optional worker object to distribute the computation
		 * @return The buffer storing all interpolated patches consecutively
		 * @tparam tSize The size of the image patches (tSize x tSize)
		 */
		template <unsigned int tSize>
		static Buffer interpolateSquarePatches(const Frame& frame, const Vectors2& positions, Worker* worker = nullptr);

		/**
		 * Interpolates square image patches with sub-pixel position and stores the image content as patch buffer.
		 * @param frame The frame in which the patches are located with 8 bit per channel
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param positions The center positions of the individual patches, with ranges [tSize / 2, width - tSize / 2 - 1)x[tSize / 2, height - tSize / 2 - 1)
		 * @param result The buffer receiving the resulting interpolated patches
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of image channels, with range [1, infinity)
		 * @tparam tSize The size of the image patches (tSize x tSize), with range [1, infinity) must be odd
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void interpolateSquarePatches8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const Vectors2& positions, uint8_t* result, Worker* worker = nullptr);

		/**
		 * Interpolates a subset of square image patches with sub-pixel position and stores the image content as patch buffer.
		 * @param frame The frame in which the patches are located with 8 bit per channel
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param positions The center positions of the individual patches, with ranges [tSize / 2, width - tSize / 2 - 1)x[tSize / 2, height - tSize / 2 - 1)
		 * @param result The buffer receiving the resulting interpolated patches
		 * @param firstPosition The first position to be handled
		 * @param numberPositions The number of positions to be handled
		 * @tparam tChannels The number of image channels, with range [1, infinity)
		 * @tparam tSize The size of the image patches (tSize x tSize), with range [1, infinity) must be odd
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void interpolateSquarePatches8BitPerChannelSubset(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const Vector2* positions, uint8_t* result, const unsigned int firstPosition, const unsigned int numberPositions);

		/**
		 * Finds unique bijective point correspondences between two given camera frames from a set of already detected unique feature points.
		 * @param camera0 The camera profile of the first frame, must be valid
		 * @param camera1 The camera profile of the second frame, must be valid
		 * @param pixelFormat The pixel of the given buffers storing interpolated image patches
		 * @param orientation0 The orientation of the first frame, must be valid
		 * @param orientation1 The orientation of the second frame, must be valid
		 * @param buffer0 The buffer storing interpolated image patches from the first frame, matching with the given image points
		 * @param buffer1 The buffer storing interpolated image patches form the second frame, matching with the given image points
		 * @param points0 The center positions of the image patches in the first frame
		 * @param points1 The center positions of the image patches in the second frame
		 * @param distribution0 The point distribution of the image points from the first frame
		 * @param distribution1 The point distribution of the image points from the second frame
		 * @param worker Optional worker object to distribute the computation
		 * @return A set if unique bijective point correspondences between the first and second frame
		 * @tparam tSize The size of the image patches (tSize x tSize), with range [1, infinity) must be odd
		 * @see findBijectiveCorrespondences8BitPerChannel().
		 */
		template < unsigned int tSize>
		static IndexPairs32 findBijectiveCorrespondences(const PinholeCamera& camera0, const PinholeCamera& camera1, const FrameType::PixelFormat pixelFormat, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const Buffer& buffer0, const Buffer& buffer1, const Vectors2& points0, const Vectors2& points1, const Geometry::SpatialDistribution::DistributionArray& distribution0, const Geometry::SpatialDistribution::DistributionArray& distribution1, Worker* worker);

		/**
		 * Finds unique bijective point correspondences between two given camera frames with eight bit per channel from a set of already detected unique feature points.
		 * @param camera0 The camera profile of the first frame, must be valid
		 * @param camera1 The camera profile of the second frame, must be valid
		 * @param orientation0 The orientation of the first frame, must be valid
		 * @param orientation1 The orientation of the second frame, must be valid
		 * @param datas0 The buffer storing interpolated image patches from the first frame, matching with the given image points
		 * @param datas1 The buffer storing interpolated image patches form the second frame, matching with the given image points
		 * @param points0 The center positions of the image patches in the first frame
		 * @param points1 The center positions of the image patches in the second frame
		 * @param distribution0 The point distribution of the image points from the first frame
		 * @param distribution1 The point distribution of the image points from the second frame
		 * @param worker Optional worker object to distribute the computation
		 * @return A set if unique bijective point correspondences between the first and second frame
		 * @tparam tChannels The number of image channels, with range [1, infinity)
		 * @tparam tSize The size of the image patches (tSize x tSize), with range [1, infinity) must be odd
		 * @see findBijectiveCorrespondences().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static IndexPairs32 findBijectiveCorrespondences8BitPerChannel(const PinholeCamera& camera0, const PinholeCamera& camera1, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const uint8_t* datas0, const uint8_t* datas1, const Vectors2& points0, const Vectors2& points1, const Geometry::SpatialDistribution::DistributionArray& distribution0, const Geometry::SpatialDistribution::DistributionArray& distribution1, Worker* worker);

		/**
		 * Finds bidirectional point correspondences between two given camera frames with eight bit per channel from a set of already detected unique feature points.
		 * @param camera0 The camera profile of the first frame, must be valid
		 * @param camera1 The camera profile of the second frame, must be valid
		 * @param orientation0 The orientation of the first frame, must be valid
		 * @param orientation1 The orientation of the second frame, must be valid
		 * @param datas0 The buffer storing interpolated image patches from the first frame, matching with the given image points
		 * @param datas1 The buffer storing interpolated image patches form the second frame, matching with the given image points
		 * @param points0 The center positions of the image patches in the first frame
		 * @param points1 The center positions of the image patches in the second frame
		 * @param distribution0 The point distribution of the image points from the first frame
		 * @param distribution1 The point distribution of the image points from the second frame
		 * @param worker Optional worker object to distribute the computation
		 * @return A set if unique bijective point correspondences between the first and second frame
		 * @tparam tChannels The number of image channels, with range [1, infinity)
		 * @tparam tSize The size of the image patches (tSize x tSize), with range [1, infinity) must be odd
		 * @see findBidirectionalCorrespondences8BitPerChannelSubset().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static IndexPairs32 findBidirectionalCorrespondences8BitPerChannel(const PinholeCamera& camera0, const PinholeCamera& camera1, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const uint8_t* datas0, const uint8_t* datas1, const Vectors2& points0, const Vectors2& points1, const Geometry::SpatialDistribution::DistributionArray& distribution0, const Geometry::SpatialDistribution::DistributionArray& distribution1, Worker* worker);

		/**
		 * Finds subsets of bidirectional point correspondences between two given camera frames with eight bit per channel from a set of already detected unique feature points.
		 * @param camera0 The camera profile of the first frame, must be valid
		 * @param camera1 The camera profile of the second frame, must be valid
		 * @param orientation0 The orientation of the first frame, must be valid
		 * @param orientation1 The orientation of the second frame, must be valid
		 * @param datas0 The buffer storing interpolated image patches from the first frame, matching with the given image points
		 * @param datas1 The buffer storing interpolated image patches form the second frame, matching with the given image points
		 * @param points0 The center positions of the image patches in the first frame
		 * @param points1 The center positions of the image patches in the second frame
		 * @param distribution0 The point distribution of the image points from the first frame
		 * @param distribution1 The point distribution of the image points from the second frame
		 * @param lock Optional lock for multi-thread execution
		 * @param results The resulting unique bijective point correspondences between the first and second frame
		 * @param firstPoint The first point to be handled
		 * @param numberPoints The number of points to be handled
		 * @tparam tChannels The number of image channels, with range [1, infinity)
		 * @tparam tSize The size of the image patches (tSize x tSize), with range [1, infinity) must be odd
		 * @see findBidirectionalCorrespondences8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void findBidirectionalCorrespondences8BitPerChannelSubset(const PinholeCamera* camera0, const PinholeCamera* camera1, const SquareMatrix3* orientation0, const SquareMatrix3* orientation1, const uint8_t* datas0, const uint8_t* datas1, const Vectors2* points0, const Vectors2* points1, const Geometry::SpatialDistribution::DistributionArray* distribution0, const Geometry::SpatialDistribution::DistributionArray* distribution1, Lock* lock, IndexPairs32* results, const unsigned int firstPoint, const unsigned int numberPoints);

		/**
		 * Finds a corresponding patch for a given patch from a second set of patches.
		 * @param data0 The image patch for which a second best matching patch has to be found
		 * @param datas1 The set of image patches from which the best matching patch has to be found
		 * @param indices The indices of possible patch candidates from the set of image patches
		 * @return The index of the image patch best matching to the given patch
		 * @tparam tChannels The number of image channels, with range [1, infinity)
		 * @tparam tSize The size of the image patches (tSize x tSize), with range [1, infinity) must be odd
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static unsigned int findCorrespondingPoint8BitPerChannel(const uint8_t* data0, const uint8_t* datas1, const Indices32& indices);

		/**
		 * Determines a set of corresponding image points representing the same unique feature point (only observed in individual camera frames).
		 * @param correspondences All possible correspondences from which the set will be extracted
		 * @param minSiblings The minimal number of image points which are necessary to define one unique feature point (the minimal number of frames in which the feature point has been observed), 0u if any number is welcome
		 * @return The set of unique feature points
		 */
		static std::vector<Indices64> determineFeaturePointsFromPointCloud(const FeaturePointMap& correspondences, const unsigned int minSiblings = 0u);

		/**
		 * Determines all sibling image point correspondences for a given correspondence.
		 * @param correspondences The set of all correspondences
		 * @param iStart The current correspondence for which all sibling correspondences are determined
		 * @param siblings The resulting unique ids of all sibling correspondences which have been found
		 * @param usedSet A set of unique correspondences which have been used/marked already
		 * @param frameSet A map of frame indices counting the number of image points for each frame to valid that a frame holds more than one image point
		 * @return The iterator of the next correspondence to be investigated
		 */
		static FeaturePointMap::const_iterator determineSiblings(const FeaturePointMap& correspondences, const FeaturePointMap::const_iterator& iStart, UnorderedIndexSet64& siblings, UnorderedIndexSet64& usedSet, IndexMap& frameSet);

		/**
		 * Creates a unique ids for a given frame index and points index located in the frame.
		 * @param frameIndex The index of the frame
		 * @param pointIndex The index of the point ion the frame
		 * @return Resulting unique feature point id
		 */
		static inline uint64_t uniqueFeaturePointId(const uint32_t frameIndex, const uint32_t pointIndex);

		/**
		 * Extracts the frame index of a unique feature point id.
		 * @param id The unique feature point id
		 * @return The frame index of the id
		 */
		static inline uint32_t frameIndex(const uint64_t id);

		/**
		 * Extracts the point index of a unique feature point id.
		 * @param id The unique feature point id
		 * @return The point index of the id
		 */
		static inline uint32_t pointIndex(const uint64_t id);

	protected:

		/// The initial orientation of the first camera frame.
		SquareMatrix3 initialOrientation_ = SquareMatrix3(true);

		/// The orientation of the camera of the most recent frame.
		SquareMatrix3 previousOrientation_ = SquareMatrix3(false);

		/// The camera profile of the most recent frame.
		PinholeCamera previousCamera_;

		/// The frame pyramid of the most recent frame.
		CV::FramePyramid previousFramePyramid_;
};

inline SphericalEnvironment::SphericalEnvironment(const unsigned int dimensionWidth, const unsigned int dimensionHeight, const uint8_t maskValue, const UpdateMode updateMode) :
	CV::Advanced::PanoramaFrame(dimensionWidth, dimensionHeight, maskValue, updateMode),
	initialOrientation_(true),
	previousOrientation_(false)
{
	// nothing to do here
}

inline uint64_t SphericalEnvironment::uniqueFeaturePointId(const uint32_t frameIndex, const uint32_t pointIndex)
{
	return uint64_t(frameIndex) << 32ull | uint64_t(pointIndex);
}

inline uint32_t SphericalEnvironment::frameIndex(const uint64_t id)
{
	return uint32_t(id >> 32ull);
}

inline uint32_t SphericalEnvironment::pointIndex(const uint64_t id)
{
	return uint32_t(id & 0xFFFFFFFFull);
}

}

}

#endif // META_OCEAN_TRACKING_SPHERICAL_ENVIRONMENT_H
