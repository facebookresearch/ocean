/*
 * Portions Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/*
 * This file contains derived work from
 *
 * https://github.com/opencv/opencv_contrib/blob/70b5d209a85b33096312bc6498c4553c1b9112dc/modules/xfeatures2d/src/freak.cpp
 *
 * Copyright (C) 2011-2012  Signal processing laboratory 2, EPFL,
 * Kirell Benzi (kirell.benzi@epfl.ch),
 * Raphael Ortiz (raphael.ortiz@a3.epfl.ch)
 * Alexandre Alahi (alexandre.alahi@epfl.ch)
 * and Pierre Vandergheynst (pierre.vandergheynst@epfl.ch)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://github.com/opencv/opencv_contrib/blob/70b5d209a85b33096312bc6498c4553c1b9112dc/LICENSE
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef META_OCEAN_CV_DETECTOR_FREAK_DESCRIPTOR_H
#define META_OCEAN_CV_DETECTOR_FREAK_DESCRIPTOR_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/Descriptor.h"
#include "ocean/cv/detector/HarrisCorner.h"
#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/geometry/Jacobian.h"
#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/PinholeCamera.h"

#include <Eigen/Core>
#include <Eigen/Dense>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/// Forward-declaration of the descriptor class.
template <size_t tSize>
class FREAKDescriptorT;

/// Typedef for the 32-bytes long FREAK descriptor
using FREAKDescriptor32 = FREAKDescriptorT<32>;

/// Vector of 32-bytes long FREAK descriptors
using FREAKDescriptors32 = std::vector<FREAKDescriptor32>;

/// Typedef for the 64-bytes long FREAK descriptor
using FREAKDescriptor64 = FREAKDescriptorT<64>;

/// Vector of 64-bytes long FREAK descriptors
using FREAKDescriptors64 = std::vector<FREAKDescriptor64>;

/**
 * Implementation of the Fast Retina Keypoint descriptors (FREAK).
 * @tparam tSize The length of the FREAK descriptor in bytes. Set of valid values: {32, 64}
 * @ingroup cvdetector
 */
template <size_t tSize>
class FREAKDescriptorT
{
	static_assert(tSize == 32 || tSize == 64, "Invalid size!");

	public:

		/// Typedef for the selected pixel type. This might be turned into a template parameter at some point.
		using PixelType = std::uint8_t;

		/// The Jacobian of the projection matrix at a specific 3D location (ray from projection center to pixel in image plane)
		using PointJacobianMatrix2x3 = Eigen::Matrix<float, 2, 3>;

		/// Single-level FREAK descriptor.
		using SinglelevelDescriptorData = std::array<PixelType, tSize>;

		/// Multi-level FREAK descriptor data; if possible, this implementation computes the descriptor at three different scales: 1.0, 1.2599, and 1.5874, cf. `descriptorLevels()`
		using MultilevelDescriptorData = std::array<SinglelevelDescriptorData, 3>;

		/**
		 * The camera data that is required to compute the FREAK descriptor of a image point
		 */
		struct CameraDerivativeData
		{
			/// The normalized ray that points from projection center to a 2D pixel location in the image plane of  camera (this is in inverted-flipped coordinates)
			Eigen::Vector3f unprojectRayIF;

			/// The 2-by-3 Jacobian matrix of a projection matrix wrt. to the above 2D pixel location in the image plane of a camera (this is in inverted-flipped coordinates)
			PointJacobianMatrix2x3 pointJacobianMatrixIF;
		};

		/**
		 * Base class to compute the Jacobian of the camera projection matrix wrt. to a 2D point and the corresponding unprojection ray of an arbitrary camera model
		 */
		class CameraDerivativeFunctor
		{
			public:

				/**
				 * Default destructor
				 */
				virtual ~CameraDerivativeFunctor() = default;

				/**
				 * Purely virtual function to compute the camera derivative data; has to be implemented in any derived class
				 * @param point A 2D point in the image plane of a camera
				 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, supportedPyramidLevels())
				 * @return The 2x3 Jacobian matrix of the projection matrix and the unprojection ray (normalized to length 1)
				 */
				virtual FREAKDescriptorT<tSize>::CameraDerivativeData computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel = 0u) const = 0;

				/**
				 * Returns the maximum number of pyramid levels for which camera derivative data can be computed
				 * @return Number of pyramid levels for which camera derivative data can be computed, range: [1, infinity)
				 */
				virtual unsigned int supportedPyramidLevels() const = 0;
		};

		/**
		 * Functor that can be used to obtain the 2x3 Jacobian of the camera projection matrix wrt. to a 2D point and the corresponding unprojection ray of a pinhole camera
		 */
		class PinholeCameraDerivativeFunctor : public CameraDerivativeFunctor
		{
			public:

				/**
				 * Constructs a valid functor to compute pinhole camera derivative data
				 * @param pinholeCamera A pinhole camera that is defined at the finest layer of an image pyramid, must be valid
				 * @param pyramidLevels Number of pyramid levels that this functor instance will be prepared for, range: [1, infinity), note: actual supported number may be lower depending on the image resolution
				 */
				inline PinholeCameraDerivativeFunctor(const PinholeCamera& pinholeCamera, const unsigned int pyramidLevels = 1u);

				/**
				 * Computes the point Jacobian of the projection matrix and unprojection ray for a specified point
				 * @param point A 2D point in the image plane of a camera
				 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, supportedPyramidLevels())
				 * @return The 2x3 Jacobian matrix of the projection matrix and the unprojection ray (normalized to length 1)
				 */
				FREAKDescriptorT<tSize>::CameraDerivativeData computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel) const override;

				/**
				 * Returns the maximum number of pyramid levels for which camera derivative data can be computed
				 * @return Number of pyramid levels for which camera derivative data can be computed, range: [1, infinity)
				 */
				unsigned int supportedPyramidLevels() const override;

				/**
				 * Computes the point Jacobian of the projection matrix and unprojection ray for a specified point
				 * @param pinholeCamera The pinhole camera which is used to compute the 2x3 Jacobian of its projection matrix
				 * @param point A 2D point in the image plane of the camera
				 * @return The 2x3 Jacobian matrix of the projection matrix and the unprojection ray (normalized to length 1)
				 */
				static typename FREAKDescriptorT<tSize>::CameraDerivativeData computeCameraDerivativeData(const PinholeCamera& pinholeCamera, const Eigen::Vector2f& point);

			protected:

				/// The camera instance used to compute the Jacobian matrix and unprojection ray at the finest layer of an image pyramid
				PinholeCameras cameras_;
		};

		/**
		 * Functor that can be used to obtain the 2x3 Jacobian of the camera projection matrix wrt. to a 2D point and the corresponding unprojection ray of a camera
		 */
		class AnyCameraDerivativeFunctor : public CameraDerivativeFunctor
		{
			public:

				/**
				 * Constructs a valid functor to compute pinhole camera derivative data
				 * @param camera A pinhole camera that is defined at the finest layer of an image pyramid, must be valid
				 * @param pyramidLevels Number of pyramid levels that this functor instance will be prepared for, range: [1, infinity), note: actual supported number may be lower depending on the image resolution
				 */
				inline AnyCameraDerivativeFunctor(const SharedAnyCamera& camera, const unsigned int pyramidLevels = 1u);

				/**
				 * Computes the point Jacobian of the projection matrix and unprojection ray for a specified point
				 * @param point A 2D point in the image plane of a camera
				 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, supportedPyramidLevels())
				 * @return The 2x3 Jacobian matrix of the projection matrix and the unprojection ray (normalized to length 1)
				 */
				FREAKDescriptorT<tSize>::CameraDerivativeData computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel) const override;

				/**
				 * Returns the maximum number of pyramid levels for which camera derivative data can be computed
				 * @return Number of pyramid levels for which camera derivative data can be computed, range: [1, infinity)
				 */
				unsigned int supportedPyramidLevels() const override;

				/**
				 * Computes the point Jacobian of the projection matrix and unprojection ray for a specified point
				 * @param camera The camera which is used to compute the 2x3 Jacobian of its projection matrix
				 * @param point A 2D point in the image plane of the camera
				 * @return The 2x3 Jacobian matrix of the projection matrix and the unprojection ray (normalized to length 1)
				 */
				static typename FREAKDescriptorT<tSize>::CameraDerivativeData computeCameraDerivativeData(const AnyCamera& camera, const Eigen::Vector2f& point);

			protected:

				/// The camera instance used to compute the Jacobian matrix and unprojection ray at the finest layer of an image pyramid
				SharedAnyCameras cameras_;
		};

	public:

		/**
		 * Creates a new and invalid FREAK descriptor object
		 */
		FREAKDescriptorT() = default;

		/**
		 * Creates a new FREAK descriptor object by copying from an existing one
		 */
		FREAKDescriptorT(const FREAKDescriptorT<tSize>&) = default;

		/**
		 * Creates a new FREAK descriptor object that will be initialized to all zeros
		 * @param data The data of this descriptor, will be moved, must be valid
		 * @param levels The number of valid levels in the descriptor data, range: [1, 3]
		 * @param orientation The orientation of the descriptor in Radian, range: (-pi, pi]
		 */
		inline FREAKDescriptorT(MultilevelDescriptorData&& data, const unsigned int levels, const float orientation) noexcept;

		/**
		 * Returns the orientation of the descriptor in Radian
		 * @return The orientation of the descriptor in Radian, range: (-pi, pi]
		 */
		inline float orientation() const;

		/**
		 * Returns the descriptor data (writable)
		 * @return A non-const  reference to the descriptor data of this instance
		 */
		inline MultilevelDescriptorData& data();

		/**
		 * Returns the descriptor data
		 * @return A const reference to the descriptor data of this instance
		 */
		inline const MultilevelDescriptorData& data() const;

		/**
		 * Returns the number of levels stored in the multi-level descriptor
		 * @return The index of the descriptor level, range: [0, 2]
		 */
		inline unsigned int descriptorLevels() const;

		/**
		 * Returns the distance between this descriptor and a second descriptor.
		 * The resulting distance is the minimal distance between all existing level/single descriptors.
		 * @param descriptor The second descriptor, must be valid
		 * @return The distance between both descriptors (the hamming distance), with range [0, tSize * 8]
		 */
		OCEAN_FORCE_INLINE unsigned int distance(const FREAKDescriptorT<tSize>& descriptor) const;

		/**
		 * Returns true if this is a valid descriptor
		 * @return True if this is a valid descriptor, otherwise false
		 */
		inline bool isValid() const;

		/**
		 * Returns the length of this descriptor in bytes.
		 * @reutrn The descriptor's length in bytes
		 */
		static constexpr size_t size();

		/**
		 * Copy assignment operator, needs to be defined since there is a custom copy constructor.
		 * @return Reference to this object
		 */
		inline FREAKDescriptorT& operator=(const FREAKDescriptorT<tSize>&) noexcept = default;

		/**
		 * Compute a FREAK descriptor for a single point
		 * @param framePyramid Frame pyramid in which the location `point` has been defined, must be valid
		 * @param point Point defined at level `pointPyramidLevel` in `framePyramid` for which a descriptor will be computed, must be valid
		 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, framePyramid.layers() - 1)
		 * @param freakDescriptor The FREAK descriptor that will be computed for the input point, will be valid only if this function returns true
		 * @param unprojectRayIF This is the 3D vector that connects the projection center of the camera with image point `point` in the image plane, must be valid
		 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
		 * @param pointJacobianMatrix2x3 The 2-by-3 Jacobian of the camera projection matrix, cf. `Geometry::Jacobian::calculatePointJacobian2x3()`, must be valid
		 * @return True if the descriptor was successfully computed, otherwise false
		 */
		static bool computeDescriptor(const FramePyramid& framePyramid, const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, FREAKDescriptorT<tSize>& freakDescriptor, const Eigen::Vector3f& unprojectRayIF, const float inverseFocalLength, const PointJacobianMatrix2x3& pointJacobianMatrix2x3);

		/**
		 * Compute a FREAK descriptor for a single point
		 * This function requires a callback function which is used internally to determine the (normalized) ray from the
		 * camera projection center to a 2D image location in the image plane and the corresponding 2-by-3 Jacobian matrix
		 * of projection matrix wrt. to the 2D image location.
		 *
		 * Example usage:
		 *
		 * @code
		 * class YourCameraDerivativeFunctor : public CameraDerivativeFunctor
		 * {
		 *     typename FREAKDescriptorT<tSize>::CameraDerivativeData computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel)
		 *     {
		 *         FREAKDescriptorT<tSize>::CameraDerivativeData data;
		 *
		 *         data.pointJacobianMatrixIF = ... // Add your computation here
		 *         data.unprojectRayIF = ... // Add your computation here
		 *
		 *         return data;
		 *     }
		 * };
		 *
		 * YourCameraDerivativeFunctor YourCameraDerivativeFunctor;
		 * FREAKDescriptorT<tSize>::computeDescriptors(yFramePyramid, points.data(), points.size(), level, oceanFreakDescriptorsMulticore.data(), inverseFocalLengthX, yourCameraDerivativeFunctor, &worker);
		 * @endcode
		 *
		 * @param framePyramid Frame pyramid in which the location `point` has been defined, must be valid
		 * @param points A pointer to the 2D image points which are defined at level `pointPyramidLevel` in `framePyramid` for which descriptors will be computed, must be valid
		 * @param pointsSize The number of elements in `points`, range: [0, infinity)
		 * @param pointsPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, framePyramid.layers() - 1)
		 * @param freakDescriptors Pointer to the FREAK descriptors that will be computed for the input point, must be valid and have `pointsSize` elements. Final descriptors can be invalid, e.g., if they are too close to the image border
		 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
		 * @param cameraDerivativeFunctor A functor that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray.
		 * @param worker Optional worker instance for parallelization
		 */
		static inline void computeDescriptors(const FramePyramid& framePyramid, const Eigen::Vector2f* points, const size_t pointsSize, const unsigned int pointsPyramidLevel, FREAKDescriptorT<tSize>* freakDescriptors, const float inverseFocalLength, const CameraDerivativeFunctor& cameraDerivativeFunctor, Worker* worker = nullptr);

		/**
		 * Extract Harris corners from an image pyramid and compute FREAK descriptors
		 * @param yFrame The 8-bit grayscale image for which Harris corners and FREAK descriptors will be computed, must be valid
		 * @param maxFrameArea This value determines the first layer of the frame pyramid for which corners and descriptors will be computed, range: (minFrameArea, infinity)
		 * @param minFrameArea This value determines the last layer of the frame pyramid for which corners and descriptors will be computed, range: [0, maxFrameArea)
		 * @param expectedHarrisCorners640x480 Expected number of Harris corners if the resolution of the image were 640 x 480 pixels. The actual number of expected corners is scaled to the size first layer in the image pyramid that is used for the extraction and then distributed over the range of pyramid layers that is used, range: [1, infinity)
		 * @param harrisCornersReductionScale Scale factor that determines the rate with which the number of corners is reduced as the function climbs through the image pyramid, range: (0, 1)
		 * @param harrisCornerThreshold Threshold value for the Harris corner detector, range: [0, 512]
		 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths) , range: (0, infinity)
		 * @param cameraDerivativeFunctor A functor that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray
		 * @param corners The Harris corners that have been extracted from the frame pyramid, will be initialized by this function, will have the same size as `cornerPyramidLevels` and `descriptors`
		 * @param cornerPyramidLevels Will hold for each Harris corner the level index of the pyramid level where it was extracted, will have the same size as `corners` and `descriptors`
		 * @param descriptors Will hold the FREAK descriptors of each Harris corners. Descriptors may be invalid. Will have the same size as `corners` and `cornerPyramidLevels`
		 * @param removeInvalid If true, all invalid descriptors (and corresponding corners and entries of pyramid levels) will be removed, otherwise all results will be remain as-is
		 * @param border Minimum distance in pixels from the image border (same value on all levels of the pyramid) that all Harris corners must have in order to be accepted, otherwise they will be discarded, range: [0, min(yFrame.width(), yFrame.height())/2)
		 * @param determineExactHarrisCornerPositions If true, force the subpixel interpolation to determine the exact position of the extracted Harris corners
		 * @param yFrameIsUndistorted If true the original input frame is undistorted and all extracted 2D feature positions will be marked as undistorted, too
		 * @param worker Optional worker instance for parallelization
		 */
		static bool extractHarrisCornersAndComputeDescriptors(const Frame& yFrame, const unsigned int maxFrameArea, const unsigned int minFrameArea, const unsigned int expectedHarrisCorners640x480, const Scalar harrisCornersReductionScale, const unsigned int harrisCornerThreshold, const float inverseFocalLength, const CameraDerivativeFunctor& cameraDerivativeFunctor, HarrisCorners& corners, Indices32& cornerPyramidLevels, std::vector<FREAKDescriptorT<tSize>>& descriptors, const bool removeInvalid = false, const Scalar border = Scalar(20), const bool determineExactHarrisCornerPositions = false, const bool yFrameIsUndistorted = true, Worker* worker = nullptr);

	protected:

		/**
		 * Compute a FREAK descriptor for a single point
		 * @param framePyramid Frame pyramid in which the location `point` has been defined, must be valid
		 * @param points A pointer to the 2D image points which are defined at level `pointPyramidLevel` in `framePyramid` for which descriptors will be computed, must be valid
		 * @param pointsSize The number of elements in `points`, range: [1, infinity)
		 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, framePyramid.layers() - 1)
		 * @param freakDescriptors Pointer to the FREAK descriptors that will be computed for the input point, must be valid and have `pointsSize` elements. Final descriptors can be invalid, e.g., if they are too close to the image border
		 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
		 * @param cameraDerivativeFunctor A callback function that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray
		 * @param firstPoint The index of the first point that will be processed by this function, rand: [0, pointsSize)
		 * @param numberOfPoints Number of points that should be processed in this function starting at `firstIndex`, range: [1, pointsSize - firstIndex]
		 */
		static void computeDescriptorsSubset(const FramePyramid* framePyramid, const Eigen::Vector2f* points, const size_t pointsSize, const unsigned int pointPyramidLevel, FREAKDescriptorT<tSize>* freakDescriptors, const float inverseFocalLength, const CameraDerivativeFunctor* cameraDerivativeFunctor, const unsigned int firstPoint, const unsigned int numberOfPoints);

		/**
		 * Computes the transformation to deform receptive fields and the orientation of the descriptor
		 * @param framePyramid Frame pyramid in which the location `point` has been defined, must be valid
		 * @param point The point defined at level `pointPyramidLevel` in `framePyramid` for which a descriptor will be computed, must be valid
		 * @param pointPyramidLevel Level of the frame pyramid at which the input point is located, range: [0, framePyramid.layers() - 1)
		 * @param unprojectRayIF This is the 3D vector that connects the projection center of the camera with image point `point` in the image plane, must be valid and inside the image
		 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
		 * @param pointJacobianMatrix2x3 The 2-by-3 Jacobian of the camera projection matrix, cf. `Geometry::Jacobian::calculatePointJacobian2x3()`, must be valid
		 * @param deformationMatrix The deformation transformation is a 2-by-2 matrix that is computed from the Jacobian of the camera projection matrix, the unprojection ray and the (inverse of the) focal length
		 * @param orientation The orientation of this descriptor in radian, range: [-pi, pi]
		 * @return True if this is a valid descriptor, otherwise false
		 */
		static bool computeLocalDeformationMatrixAndOrientation(const FramePyramid& framePyramid, const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, const Eigen::Vector3f& unprojectRayIF, const float inverseFocalLength, const PointJacobianMatrix2x3& pointJacobianMatrix2x3, Eigen::Matrix<float, 2, 2>& deformationMatrix, float& orientation);

		/**
		 * Computes the average intensity of a cell
		 * @param framePyramidLayer Layer of a frame pyramid in which the location `(x, y)` has been defined, must be valid
		 * @param x The pixel-accurate horizontal coordinate of the cell, range: [0 + u, width - u), u = kernel_radius / 2 if `tEnableBorderChecks == true`, otherwise u = kernel_radius
		 * @param y The pixel-accurate vertical coordinate of the cell, range: [0 + v, height - v), v = kernel_radius / 2 if `tEnableBorderChecks == true`, otherwise v = kernel_radius
		 * @param kernelX Pointer to the horizontal offsets of the kernel elements (relative to `x`), must be valid and have `kernelElements` elements
		 * @param kernelY Pointer to the vertical offsets of the kernel elements (relative to `y`), must be valid and have `kernelElements` elements
		 * @param kernelElements Number of elements in the kernel, range: [1, infinity]
		 * @param averageIntensity The average intensity of the selected cell
		 * @tparam tEnableBorderChecks If true, only kernel values inside the input image will be added, otherwise the check will be disabled and the entire kernel is assumed to fit inside the image (for performance)
		 * @return True if the average intensity was successfully computed, otherwise false (e.g. location (x, y) too close to the image border)
		 */
		template <bool tEnableBorderChecks>
		static bool computeAverageCellIntensity(const Frame& framePyramidLayer, int x, int y, const int* kernelX, const int* kernelY, const size_t kernelElements, PixelType& averageIntensity);

		/**
		 * Creates a new pyramid frame for a specific pixel format (a specific number of channels) and applies a Gaussian blur before each down-size step.
		 * @param frame The frame pyramid will be built using this frame, must be valid and use 8 bits per channel
		 * @param kernelWidth Width of the Gaussian kernel that is applied before a down-size step, range: [1, infinity), value must be odd
 		 * @param kernelHeight Height of the Gaussian kernel that is applied before a down-size step, range: [1, infinity), value must be odd
 		 * @param layers The number of pyramid layers to be created, with range [1, infinity)
 		 * @param worker Optional worker object to distribute the computation
		 * @return The created frame pyramid (will be invalid in case of a failure)
 		 */
		static FramePyramid createFramePyramidWithBlur8BitsPerChannel(const Frame& frame, const unsigned int kernelWidth, const unsigned int kernelHeight, const unsigned int layers, Worker* worker = nullptr);

		/**
		 * Downsamples a frame by two applying a 1-1 filter after applying a Gaussian blur to the source layer.
		 * @param finerLayer The finer pyramid layer, must be valid
		 * @param coarserLayer The coarser pyramid layer, must be valid
		 * @param worker The optional worker to distribute the computation
		 * @param kernelWidth The width of the Gaussian kernel, in pixel, with range [1, infinity), must odd
		 * @param kernelHeight The height of the Gaussian kernel, in pixel, with range [1, infinity), must odd
		 * @param reusableFrame A reusable frame which can be used internally
		 * @return True, if succeeded
		 */
		static bool blurAndDownsampleByTwo11(const Frame& finerLayer, Frame& coarserLayer, Worker* worker, const unsigned int kernelWidth, const unsigned int kernelHeight, Frame& reusableFrame);

	private:

		/// The number of cells per keypoint that this implementation is using
		static constexpr size_t numberOfCells = 43;

		/// The pre-defined horizontal coordinates of the cells
		static const float cellsX[numberOfCells];

		/// The pre-defined vertical coordinates of the cells
		static const float cellsY[numberOfCells];

		/// The number of pre-defined pairs of cell indices that are used to compute the actual binary descriptor
		static constexpr size_t numberOfCellPairs = 512;

		/// The pre-defined pairs of cell indices that uare used to compute the actual binary descriptor (pairs have been randomly shuffled)
		static const std::uint8_t cellPairs[numberOfCellPairs][2];

		/// Number of elements in the circular kernel with radius 1
		static constexpr size_t kernelRadius1Elements = 5;

		/// The pre-defined horizontal coordinates of the circular kernel with radius 1
		static const int kernelRadius1X[kernelRadius1Elements];

		/// The pre-defined vertical coordinates of the circular kernel with radius 1
		static const int kernelRadius1Y[kernelRadius1Elements];

		/// Number of elements in the circular kernel with radius 2
		static constexpr size_t kernelRadius2Elements = 13;

		/// The pre-defined horizontal coordinates of the circular kernel with radius 2
		static const int kernelRadius2X[kernelRadius2Elements];

		/// The pre-defined vertical coordinates of the circular kernel with radius 2
		static const int kernelRadius2Y[kernelRadius2Elements];

		/// Number of elements in the circular kernel with radius 3
		static constexpr size_t kernelRadius3Elements = 29;

		/// The pre-defined horizontal coordinates of the circular kernel with radius 3
		static const int kernelRadius3X[kernelRadius3Elements];

		/// The pre-defined vertical coordinates of the circular kernel with radius 3
		static const int kernelRadius3Y[kernelRadius3Elements];

		/// Number of elements in the circular kernel with radius 7
		static constexpr size_t kernelRadius7Elements = 149;

		/// The pre-defined horizontal coordinates of the circular kernel with radius 7
		static const int kernelRadius7X[kernelRadius7Elements];

		/// The pre-defined vertical coordinates of the circular kernel with radius 7
		static const int kernelRadius7Y[kernelRadius7Elements];

	protected:

		/// The orientation of this descriptor in radian, range: [-pi, pi]
		float orientation_ = 0.0f;

		/// The actual FREAK descriptor data
		MultilevelDescriptorData data_;

		/// Number of valid levels in the multi-level descriptor data above, range: [0, 3]
		unsigned int dataLevels_ = 0u;
};

template <size_t tSize>
inline FREAKDescriptorT<tSize>::PinholeCameraDerivativeFunctor::PinholeCameraDerivativeFunctor(const PinholeCamera& pinholeCamera, const unsigned int pyramidLevels)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(pyramidLevels != 0u);

	cameras_.reserve(pyramidLevels);
	cameras_.emplace_back(pinholeCamera);

	unsigned int width = pinholeCamera.width();
	unsigned int height = pinholeCamera.height();

	for (unsigned int level = 1u; level < pyramidLevels; ++level)
	{
		width /= 2u;
		height /= 2u;

		if (width == 0u || height == 0u)
		{
			break;
		}

		cameras_.emplace_back(width, height, pinholeCamera);
	}

	cameras_.shrink_to_fit();
}

template <size_t tSize>
typename FREAKDescriptorT<tSize>::CameraDerivativeData FREAKDescriptorT<tSize>::PinholeCameraDerivativeFunctor::computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel) const
{
	ocean_assert(pointPyramidLevel < cameras_.size());
	return FREAKDescriptorT<tSize>::PinholeCameraDerivativeFunctor::computeCameraDerivativeData(cameras_[pointPyramidLevel], point);
}

template <size_t tSize>
unsigned int FREAKDescriptorT<tSize>::PinholeCameraDerivativeFunctor::supportedPyramidLevels() const
{
	return (unsigned int)(cameras_.size());
}

template <size_t tSize>
typename FREAKDescriptorT<tSize>::CameraDerivativeData FREAKDescriptorT<tSize>::PinholeCameraDerivativeFunctor::computeCameraDerivativeData(const PinholeCamera& pinholeCamera, const Eigen::Vector2f& point)
{
	const Vector3 unprojectRayIF = pinholeCamera.vectorIF(Vector2(point.x(), point.y()));
	ocean_assert(Numeric::isEqualEps((Vector3((Scalar(point.x()) - pinholeCamera.principalPointX()) * pinholeCamera.inverseFocalLengthX(), (Scalar(point.y()) - pinholeCamera.principalPointY()) * pinholeCamera.inverseFocalLengthY(), 1.0f).normalized() - unprojectRayIF).length()));

	// TODOX Revisit this when enabling camera distortions
	ocean_assert(pinholeCamera.hasDistortionParameters() == false);

	Scalar jacobianX[3];
	Scalar jacobianY[3];
	Geometry::Jacobian::calculatePointJacobian2x3(jacobianX, jacobianY, pinholeCamera, HomogenousMatrix4(true), unprojectRayIF, /* distort */ false);

	typename FREAKDescriptorT<tSize>::CameraDerivativeData data;

	data.unprojectRayIF = Eigen::Vector3f(float(unprojectRayIF.x()), float(unprojectRayIF.y()), float(unprojectRayIF.z()));

	// Note: the assignment below is row-major order but Eigen memory will be column-major. I know ...
	data.pointJacobianMatrixIF << float(jacobianX[0]), float(jacobianX[1]), float(jacobianX[2]), float(jacobianY[0]), float(jacobianY[1]), float(jacobianY[2]);
	ocean_assert(data.pointJacobianMatrixIF.IsRowMajor == false);

	return data;
}

template <size_t tSize>
inline FREAKDescriptorT<tSize>::AnyCameraDerivativeFunctor::AnyCameraDerivativeFunctor(const SharedAnyCamera& camera, const unsigned int pyramidLevels)
{
	ocean_assert(camera && camera->isValid());
	ocean_assert(pyramidLevels != 0u);

	cameras_.reserve(pyramidLevels);
	cameras_.emplace_back(camera);

	unsigned int width = camera->width();
	unsigned int height = camera->height();

	for (unsigned int level = 1u; level < pyramidLevels; ++level)
	{
		width /= 2u;
		height /= 2u;

		if (width == 0u || height == 0u)
		{
			break;
		}

		cameras_.emplace_back(cameras_.back()->clone(width, height));
	}
}

template <size_t tSize>
typename FREAKDescriptorT<tSize>::CameraDerivativeData FREAKDescriptorT<tSize>::AnyCameraDerivativeFunctor::computeCameraDerivativeData(const Eigen::Vector2f& point, const unsigned int pointPyramidLevel) const
{
	ocean_assert(pointPyramidLevel < cameras_.size());
	return FREAKDescriptorT<tSize>::AnyCameraDerivativeFunctor::computeCameraDerivativeData(*cameras_[pointPyramidLevel], point);
}

template <size_t tSize>
unsigned int FREAKDescriptorT<tSize>::AnyCameraDerivativeFunctor::supportedPyramidLevels() const
{
	return (unsigned int)(cameras_.size());
}

template <size_t tSize>
typename FREAKDescriptorT<tSize>::CameraDerivativeData FREAKDescriptorT<tSize>::AnyCameraDerivativeFunctor::computeCameraDerivativeData(const AnyCamera& camera, const Eigen::Vector2f& point)
{
	const Vector3 unprojectRayIF = camera.vectorIF(Vector2(point.x(), point.y()));

	Scalar jacobianX[3];
	Scalar jacobianY[3];
	camera.pointJacobian2x3IF(unprojectRayIF, jacobianX, jacobianY);

	typename FREAKDescriptorT<tSize>::CameraDerivativeData data;

	data.unprojectRayIF = Eigen::Vector3f(float(unprojectRayIF.x()), float(unprojectRayIF.y()), float(unprojectRayIF.z()));

	// Note: the assignment below is row-major order but Eigen memory will be column-major. I know ...
	data.pointJacobianMatrixIF << float(jacobianX[0]), float(jacobianX[1]), float(jacobianX[2]), float(jacobianY[0]), float(jacobianY[1]), float(jacobianY[2]);
	ocean_assert(data.pointJacobianMatrixIF.IsRowMajor == false);

	return data;
}

template <size_t tSize>
FREAKDescriptorT<tSize>::FREAKDescriptorT(MultilevelDescriptorData&& data, const unsigned int levels, const float orientation) noexcept :
	orientation_(orientation),
	data_(std::move(data)),
	dataLevels_(levels)
{
	ocean_assert(levels >= 1u && levels <= 3u);
	ocean_assert(NumericF::isInsideRange(-NumericF::pi(), orientation, NumericF::pi()));
}

template <size_t tSize>
float FREAKDescriptorT<tSize>::orientation() const
{
	ocean_assert(NumericF::isInsideRange(-NumericF::pi(), orientation_, NumericF::pi()));
	return orientation_;
}

template <size_t tSize>
inline typename FREAKDescriptorT<tSize>::MultilevelDescriptorData& FREAKDescriptorT<tSize>::data()
{
	return data_;
}

template <size_t tSize>
inline const typename FREAKDescriptorT<tSize>::MultilevelDescriptorData& FREAKDescriptorT<tSize>::data() const
{
	return data_;
}

template <size_t tSize>
inline unsigned int FREAKDescriptorT<tSize>::descriptorLevels() const
{
	ocean_assert(dataLevels_ <= 3u);
	return dataLevels_;
}

template <size_t tSize>
OCEAN_FORCE_INLINE unsigned int FREAKDescriptorT<tSize>::distance(const FREAKDescriptorT<tSize>& descriptor) const
{
	ocean_assert(isValid() && descriptor.isValid());

	unsigned int bestDistance = (unsigned int)(-1);

	for (unsigned int nOuter = 0u; nOuter < dataLevels_; ++nOuter)
	{
		const SinglelevelDescriptorData& outerData = data_[nOuter];

		for (unsigned int nInner = 0u; nInner < descriptor.dataLevels_; ++nInner)
		{
			const SinglelevelDescriptorData& innerData = descriptor.data_[nInner];

			const unsigned int distance = Descriptor::calculateHammingDistance<tSize * 8u>(outerData.data(), innerData.data());

			if (distance < bestDistance)
			{
				bestDistance = distance;
			}
		}
	}

	ocean_assert(bestDistance != (unsigned int)(-1));

	return bestDistance;
}

template <size_t tSize>
inline bool FREAKDescriptorT<tSize>::isValid() const
{
	return descriptorLevels() >= 1u && descriptorLevels() <= 3u && NumericF::isInsideRange(-NumericF::pi(), orientation_, NumericF::pi());
}

template <size_t tSize>
constexpr size_t FREAKDescriptorT<tSize>::size()
{
	return tSize;
}

template <size_t tSize>
inline void FREAKDescriptorT<tSize>::computeDescriptors(const FramePyramid& framePyramid, const Eigen::Vector2f* points, const size_t pointsSize, const unsigned int pointsPyramidLevel, FREAKDescriptorT<tSize>* freakDescriptors, const float inverseFocalLength, const CameraDerivativeFunctor& projectionDerivativeDataCallback, Worker* worker)
{
	ocean_assert(framePyramid.isValid());
	ocean_assert(points != nullptr && pointsSize != 0u);
	ocean_assert(pointsPyramidLevel < framePyramid.layers());
	ocean_assert(freakDescriptors != nullptr);
	ocean_assert(inverseFocalLength > 0.0f);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FREAKDescriptorT<tSize>::computeDescriptorsSubset, &framePyramid, points, pointsSize, pointsPyramidLevel, freakDescriptors, inverseFocalLength, &projectionDerivativeDataCallback, 0u, 0u), 0u, (unsigned int)pointsSize);
	}
	else
	{
		FREAKDescriptorT<tSize>::computeDescriptorsSubset(&framePyramid, points, pointsSize, pointsPyramidLevel, freakDescriptors, inverseFocalLength, &projectionDerivativeDataCallback, 0u, (unsigned int)pointsSize);
	}
}

template <size_t tSize>
const float FREAKDescriptorT<tSize>::cellsX[numberOfCells] =
{
	// clang-format off
	 0.0f,    -14.7216f,  -14.7216f,  0.0f,     14.7216f,  14.7216f,  -6.3745f,  -12.749f,   -6.3745f,   6.3745f,
	12.749f,    6.3745f,    0.0f,    -7.97392f, -7.97392f,  0.0f,      7.97392f,   7.97392f, -3.18725f, -6.3745f,
	-3.18725f,  3.18725f,   6.3745f,  3.18725f,  0.0f,     -3.67983f, -3.67983f,   0.0f,      3.67983f,  3.67983f,
	-1.4163f,  -2.8326f,   -1.4163f,  1.4163f,   2.8326f,   1.4163f,   0.0f,      -1.84049f, -1.84049f,  0.0f,
	 1.84049f,  1.84049f,   0.0f
	 // clang-format on
};

template <size_t tSize>
const float FREAKDescriptorT<tSize>::cellsY[numberOfCells] =
{
	// clang-format off
	16.9991f,   8.49895f, -8.49895f, -16.9991f,  -8.49895f,  8.49895f, 11.0406f,   0.0f,     -11.0406f,  -11.0406f,
	 0.0f,     11.0406f,   9.2071f,    4.60355f, -4.60355f, -9.2071f,  -4.60355f,  4.60355f,   5.52032f,   0.0f,
	-5.52032f, -5.52032f,  0.0f,       5.52032f,  4.25005f,  2.12445f, -2.12445f, -4.25005f,  -2.12445f,   2.12445f,
	 2.4536f,   0.0f,     -2.4536f,   -2.4536f,   0.0f,      2.4536f,   2.12445f,  1.0628f,   -1.0628f,   -2.12445f,
	-1.0628f,   1.0628f,   0.0f
	// clang-format on
};

template <size_t tSize>
const uint8_t FREAKDescriptorT<tSize>::cellPairs[numberOfCellPairs][2] =
{
	// clang-format off
	{37, 4}, {38, 4}, {12, 0}, {39,10}, {27, 7}, {37,29}, {20,16}, {33,16}, {14, 0}, {31, 3},
	{17, 4}, {24,12}, {33,22}, {31, 7}, {35,30}, {25, 6}, {34,31}, {20,19}, {22,17}, {16, 6},
	{23, 5}, {26,10}, {13, 5}, {31,17}, {17,10}, {31,28}, {22, 4}, {29,11}, {28, 2}, {29,19},
	{30, 6}, {37,10}, {31, 2}, {41,13}, {14, 7}, {15, 3}, {33, 4}, {18,17}, {23,19}, {33,28},
	{41,24}, {34,16}, { 7, 1}, {26, 5}, {36,13}, {42, 9}, {20,14}, {27,26}, {41, 6}, {40,19},
	{26, 3}, {36,29}, {23,13}, {40, 7}, {18, 0}, {28,22}, {22, 9}, {26,16}, {21,16}, {39,20},
	{ 8, 3}, {14, 1}, {12,11}, {31,25}, {29, 4}, {15, 1}, {41,22}, {35, 1}, {26, 2}, {34,14},
	{25, 1}, {34,17}, {34,29}, {16,14}, {19, 3}, {26,14}, {15, 5}, {25,17}, {25, 5}, {34,25},
	{ 6, 0}, {23,10}, {29,24}, {28,16}, {20, 3}, { 7, 4}, {25,11}, {36,24}, {27, 9}, {11,10},
	{23, 7}, {32,19}, {32,16}, {37,18}, {25,24}, {19, 1}, {22,20}, {38,14}, {41,31}, {16,10},
	{19, 6}, {16,11}, {31,20}, { 8, 0}, {14, 2}, {19, 0}, {37,13}, {34, 4}, {31,14}, { 6, 1},
	{40, 1}, {24,18}, {41, 1}, {41, 7}, {36,23}, {40,20}, {40,27}, {13, 0}, {19,12}, {42,38},
	{16, 7}, {34, 7}, { 9, 2}, {28, 4}, {11, 5}, {40,38}, {17, 2}, { 5, 0}, {19,14}, {12, 6},
	{19,17}, {40,22}, {26, 7}, {19, 5}, {19,11}, {28,26}, {12, 1}, {34, 0}, { 5, 1}, {27,16},
	{21,15}, {29,25}, {19, 8}, {32,26}, {37,17}, {11, 6}, {22, 6}, {39,27}, {41,37}, {21, 5},
	{14,11}, {31,16}, {38,28}, {16, 0}, {29,10}, {31,26}, {10, 1}, {22,13}, {10, 3}, {17, 3},
	{42,30}, { 8, 4}, {26, 6}, {22, 8}, {38,27}, {26,22}, {41,10}, {42,13}, {40,34}, {13, 7},
	{30,11}, {38,22}, {33,27}, {19,15}, {29, 7}, {31,10}, {26,15}, {13,12}, {29, 2}, { 5, 3},
	{15, 7}, {28,10}, {29,17}, {40,10}, {21, 1}, {15,10}, {37,11}, {40,13}, {26, 1}, {39,21},
	{34,21}, {40,31}, {19, 7}, {16, 5}, {40,39}, {37, 7}, {30,23}, {10, 9}, {36,30}, {38, 0},
	{18, 6}, {40,32}, {38,10}, {22, 3}, {26,19}, {18,13}, {39,22}, {35,17}, {31,19}, {18,11},
	{28,19}, {28, 0}, {37,31}, {30, 7}, {27,20}, {34,10}, {38, 3}, {37,23}, {18, 7}, {38,20},
	{25,19}, {20, 7}, {22,18}, { 7, 3}, {15, 2}, {23,12}, {26,13}, {38, 7}, {11, 1}, {20, 8},
	{33,21}, {37,36}, {17,16}, {36,35}, {41, 2}, {37,35}, {37, 2}, {15,14}, {10, 7}, {41,29},
	{ 7, 6}, {32,22}, {34,26}, {33, 2}, {38,26}, {31, 0}, {11, 3}, {24,23}, {13,11}, {41,19},
	{41,25}, {30,13}, {27,10}, {39,38}, {21, 3}, {31, 4}, {27,14}, {37,24}, {20, 2}, {25,23},
	{29, 1}, {39,28}, {17, 0}, { 7, 0}, { 9, 5}, {22, 2}, {33,32}, {27,21}, {30,25}, {41,23},
	{41,30}, {15, 9}, {22,10}, {31,22}, {29, 5}, {34,20}, {24,13}, {31,11}, {36,25}, {21,19},
	{19,13}, {30,29}, {33, 5}, { 6, 4}, { 5, 2}, { 8, 2}, {10, 2}, {25,13}, {37,19}, {28,14},
	{15, 4}, {10, 8}, {12, 5}, {14,13}, {24, 1}, {31,12}, {14,10}, {32,27}, {19,18}, {32, 4},
	{22, 1}, {39,26}, {17,14}, { 2, 1}, { 1, 0}, {35,23}, {34, 2}, {33,19}, {13, 3}, {39,16},
	{25, 2}, {41, 4}, {28, 7}, {31,21}, {26, 4}, {39,19}, {24,17}, {28,20}, {21, 8}, {25, 7},
	{34,15}, {41,36}, {16, 3}, {21,20}, {31,15}, {26,20}, {14, 5}, {38,16}, {40, 2}, {18,10},
	{27, 8}, {29,13}, {41,18}, {18,12}, {40,26}, {36, 0}, {21,14}, {22, 0}, {27, 2}, {11, 0},
	{21,10}, {20,10}, {23, 6}, {13, 4}, {28,21}, {22,16}, {25,22}, {35,24}, { 4, 0}, {31, 1},
	{32,21}, {21, 4}, {37, 6}, {15, 8}, { 8, 7}, {29,22}, {28,15}, {25,18}, {41,35}, {39,14},
	{34,12}, {23,17}, {25,10}, {39, 9}, {34,13}, {22,14}, { 7, 2}, {20, 9}, {28,11}, {10, 4},
	{40, 0}, {35,13}, {38,32}, {13, 2}, {39, 1}, { 2, 0}, {38,19}, {41,11}, {32,28}, {39,33},
	{30,17}, {16, 2}, {17, 6}, {13,10}, { 4, 1}, {10, 0}, {22,19}, { 4, 3}, {12, 7}, {26,21},
	{ 9, 0}, {19,16}, {34,28}, {16, 9}, { 9, 8}, {23, 0}, { 7, 5}, {10, 5}, {34,18}, {14, 6},
	{30, 5}, {31,18}, {20,15}, {34,22}, {35,12}, {23, 1}, {35,10}, { 9, 3}, {27,15}, {17,13},
	{37,30}, {26, 0}, {28,17}, {38,33}, {38, 5}, {16, 4}, {13, 1}, {28, 3}, { 5, 4}, {12, 2},
	{17, 9}, {31,29}, {22,11}, {40,17}, {25, 4}, {28,27}, {29, 6}, {34, 1}, {14, 8}, {32,15},
	{39,32}, { 6, 5}, {19, 4}, {18, 5}, {32,20}, {38,13}, {12,10}, {24, 0}, {22,15}, {36,18},
	{ 6, 3}, {34,23}, {33,15}, {22, 7}, {22,12}, {40,28}, {35,18}, {22, 5}, {29,23}, {37,34},
	{16,13}, {23,18}, {37,22}, {29,12}, {19, 2}, {14, 9}, {34,19}, {19,10}, {25,12}, {38,21},
	{28, 1}, {33,20}, {27, 4}, {11, 7}, {31,23}, {17, 7}, {17, 8}, {39, 8}, {40,21}, {16,15},
	{17, 5}, {30,18}, {39, 7}, {37,25}, {41,34}, {30,24}, {18, 1}, { 3, 1}, { 9, 4}, {22,21},
	{31, 5}, {40, 3}, {35,25}, {32, 2}, { 4, 2}, {38,31}, {14, 3}, {21, 9}, {17,12}, {16, 1},
	{35,29}, {23,22}, {20, 1}, {34, 3}, {17, 1}, {13, 6}, {40,14}, {17,11}, {38,17}, {40,16},
	{20, 4}, {23,11}, {12, 4}, { 3, 2}, {40,33}, {14, 4}, {21, 2}, {33,26}, {38,34}, {29,18},
	{21, 7}, {16, 8}
	// clang-format on
};

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius1X[kernelRadius1Elements] = { 0, -1, 0, 1, 0 };

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius1Y[kernelRadius1Elements] = { -1, 0, 0, 0, 1 };

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius2X[kernelRadius2Elements] = { 0, -1, 0, 1, -2, -1, 0, 1, 2, -1, 0, 1, 0 };

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius2Y[kernelRadius2Elements] = { -2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2 };

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius3X[kernelRadius3Elements] =
{
	// clang-format off
	0, -2, -1,  0,  1,  2, -2, -1,  0,  1,
	2, -3, -2, -1,  0,  1,  2,  3, -2, -1,
	0,  1,  2, -2, -1,  0,  1,  2,  0
	// clang-format on
};

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius3Y[kernelRadius3Elements] =
{
	// clang-format off
	-3, -2, -2, -2, -2, -2, -1, -1, -1, -1,
	-1,  0,  0,  0,  0,  0,  0,  0,  1,  1,
	 1,  1,  1,  2,  2,  2,  2,  2,  3
	 // clang-format on
};

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius7X[kernelRadius7Elements] =
{
	// clang-format off
	 0, -3, -2, -1,  0,  1,  2,  3, -4, -3,
	-2, -1,  0,  1,  2,  3,  4, -5, -4, -3,
	-2, -1,  0,  1,  2,  3,  4,  5, -6, -5,
	-4, -3, -2, -1,  0,  1,  2,  3,  4,  5,
	 6, -6, -5, -4, -3, -2, -1,  0,  1,  2,
	 3,  4,  5,  6, -6, -5, -4, -3, -2, -1,
	 0,  1,  2,  3,  4,  5,  6, -7, -6, -5,
	-4, -3, -2, -1,  0,  1,  2,  3,  4,  5,
	 6,  7, -6, -5, -4, -3, -2, -1,  0,  1,
	 2,  3,  4,  5,  6, -6, -5, -4, -3, -2,
	-1,  0,  1,  2,  3,  4,  5,  6, -6, -5,
	-4, -3, -2, -1,  0,  1,  2,  3,  4,  5,
	 6, -5, -4, -3, -2, -1,  0,  1,  2,  3,
	 4,  5, -4, -3, -2, -1,  0,  1,  2,  3,
	 4, -3, -2, -1,  0,  1,  2,  3,  0
	 // clang-format on
};

template <size_t tSize>
const int FREAKDescriptorT<tSize>::kernelRadius7Y[kernelRadius7Elements] =
{
	// clang-format off
	-7, -6, -6, -6, -6, -6, -6, -6, -5, -5,
	-5, -5, -5, -5, -5, -5, -5, -4, -4, -4,
	-4, -4, -4, -4, -4, -4, -4, -4, -3, -3,
	-3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
	-3, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
	 1,  1,  1,  1,  1,  2,  2,  2,  2,  2,
	 2,  2,  2,  2,  2,  2,  2,  2,  3,  3,
	 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	 3,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	 4,  4,  5,  5,  5,  5,  5,  5,  5,  5,
	 5,  6,  6,  6,  6,  6,  6,  6,  7
	 // clang-format on
};

template <size_t tSize>
bool FREAKDescriptorT<tSize>::computeDescriptor(const FramePyramid& pyramid, const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, FREAKDescriptorT<tSize>& freakDescriptor, const Eigen::Vector3f& unprojectRayIF, const float inverseFocalLength, const PointJacobianMatrix2x3& pointJacobianMatrixIF)
{
	ocean_assert(pointPyramidLevel < pyramid.layers());
	ocean_assert(inverseFocalLength > 0.0f);

	// No descriptors can be computed for points in the coarsest layer of the frame pyramid

	if (pointPyramidLevel + 1u >= pyramid.layers())
	{
		return false;
	}

	// Invalidate the descriptor for now

	freakDescriptor.dataLevels_ = 0u;
	ocean_assert(freakDescriptor.isValid() == false);

	// Compute the deformation matrix from the position of the image point (and its Jacobian, etc)

	Eigen::Matrix<float, 2, 2> cellDeformationMatrix;
	if (computeLocalDeformationMatrixAndOrientation(pyramid, point, pointPyramidLevel, unprojectRayIF, inverseFocalLength, pointJacobianMatrixIF, cellDeformationMatrix, freakDescriptor.orientation_) == false)
	{
		return false;
	}

	// Apply the deformation matrix to the locations of all cells

	float warpedCellX[FREAKDescriptorT<tSize>::numberOfCells];
	float warpedCellY[FREAKDescriptorT<tSize>::numberOfCells];

	for (size_t i = 0; i < FREAKDescriptorT<tSize>::numberOfCells; ++i)
	{
		const Eigen::Vector2f warpedCell = cellDeformationMatrix * Eigen::Vector2f(FREAKDescriptorT<tSize>::cellsX[i], FREAKDescriptorT<tSize>::cellsY[i]);
		warpedCellX[i] = warpedCell[0];
		warpedCellY[i] = warpedCell[1];
	}

	// Compute a descriptor for each intra-level:
	//
	//   2^(0/3) = 1,
	//   2^(1/3) = 1.2599,
	//   2^(2/3) = 1.5874
	//
	const float scaleFactors[3] = { 1.0f, 1.2599f, 1.5874f };
	for (size_t scaleLevel = 0; scaleLevel < 3; ++scaleLevel)
	{
		PixelType cellIntensities[FREAKDescriptorT<tSize>::numberOfCells];
		bool computationFailed = false;

		// Compute the average intensity per cell
		//
		// In order to reduce the number of if-branches, we'll split the range of cell IDs such that they are partitioned into groups with identical conditions, cf. table below:
		//
		// Cell ID:              0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42
		// Pyramid evel offsets: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		// Radii:                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
		// Check pixel in image: T, T, T, T, T, T, T, T, T, T, T, T, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F
		// for-loop ID:          |--- 0 ------------------------->| |--- 1 -------------------------->| |--- 2 -------->| |--- 3 ----------------------------->|

		unsigned int cellId = 0u;

		// Loop 0
		for (; cellId < 12u; ++cellId)
		{
			ocean_assert(pointPyramidLevel + 1u < pyramid.layers());
			const Frame& nextFramePyramidLayer = pyramid.layer(pointPyramidLevel + 1u);

			const float cellX = point[0] + scaleFactors[scaleLevel] * warpedCellX[cellId];
			const float cellY = point[1] + scaleFactors[scaleLevel] * warpedCellY[cellId];

			const int cellXi = NumericF::round32(((cellX + 0.5f) * 0.5f) - 0.5f);
			const int cellYi = NumericF::round32(((cellY + 0.5f) * 0.5f) - 0.5f);

			// Check if the (half-)radius fits into the image, radius = 3, radius / 2 = 1
			if (cellXi - 1 < 0 || cellXi + 1 >= int(nextFramePyramidLayer.width()) || cellYi - 1 < 0 || cellYi + 1 >= int(nextFramePyramidLayer.height()))
			{
				computationFailed = true;
				break;
			}

			ocean_assert(cellXi >= 0 && cellXi < int(nextFramePyramidLayer.width()) && cellYi >= 0 && cellYi < int(nextFramePyramidLayer.height()));

			if (computeAverageCellIntensity<true>(nextFramePyramidLayer, cellXi, cellYi, kernelRadius3X, kernelRadius3Y, kernelRadius3Elements, cellIntensities[cellId]) == false)
			{
				computationFailed = true;
				break;
			}
		}

		// Loop 1
		const Frame& currentFramePyramidLayer = pyramid.layer(pointPyramidLevel);

		ocean_assert(cellId == 12u || computationFailed == true);
		for (; computationFailed == false && cellId < 24u; ++cellId)
		{
			const float cellX = point[0] + scaleFactors[scaleLevel] * warpedCellX[cellId];
			const float cellY = point[1] + scaleFactors[scaleLevel] * warpedCellY[cellId];

			const int cellXi = NumericF::round32(cellX);
			const int cellYi = NumericF::round32(cellY);

			ocean_assert(cellXi >= 0 && cellXi < int(currentFramePyramidLayer.width()) && cellYi >= 0 && cellYi < int(currentFramePyramidLayer.height()));

			if (computeAverageCellIntensity<false>(currentFramePyramidLayer, cellXi, cellYi, kernelRadius3X, kernelRadius3Y, kernelRadius3Elements, cellIntensities[cellId]) == false)
			{
				computationFailed = true;
				break;
			}
		}

		// Loop 2
		ocean_assert(cellId == 24u || computationFailed == true);
		for (; computationFailed == false && cellId < 30u; ++cellId)
		{
			const float cellX = point[0] + scaleFactors[scaleLevel] * warpedCellX[cellId];
			const float cellY = point[1] + scaleFactors[scaleLevel] * warpedCellY[cellId];

			const int cellXi = NumericF::round32(cellX);
			const int cellYi = NumericF::round32(cellY);

			ocean_assert(cellXi >= 0 && cellXi < int(currentFramePyramidLayer.width()) && cellYi >= 0 && cellYi < int(currentFramePyramidLayer.height()));
			if (computeAverageCellIntensity<false>(currentFramePyramidLayer, cellXi, cellYi, kernelRadius2X, kernelRadius2Y, kernelRadius2Elements, cellIntensities[cellId]) == false)
			{
				computationFailed = true;
				break;
			}
		}

		// Loop 3
		ocean_assert(cellId == 30u || computationFailed == true);
		for (; computationFailed == false && cellId < FREAKDescriptorT<tSize>::numberOfCells; ++cellId)
		{
			const float cellX = point[0] + scaleFactors[scaleLevel] * warpedCellX[cellId];
			const float cellY = point[1] + scaleFactors[scaleLevel] * warpedCellY[cellId];

			const int cellXi = NumericF::round32(cellX);
			const int cellYi = NumericF::round32(cellY);

			ocean_assert(cellXi >= 0 && cellXi < int(currentFramePyramidLayer.width()) && cellYi >= 0 && cellYi < int(currentFramePyramidLayer.height()));

			if (computeAverageCellIntensity<false>(currentFramePyramidLayer, cellXi, cellYi, kernelRadius1X, kernelRadius1Y, kernelRadius1Elements, cellIntensities[cellId]) == false)
			{
				computationFailed = true;
				break;
			}
		}

		if (computationFailed)
		{
			break;
		}

		// Compute the binary descriptor for the current scale level

		for (size_t i = 0; i < tSize; ++i)
		{
			uint8_t partialDescriptor = 0u;

			for (size_t j = 0; j < 8; ++j)
			{
				partialDescriptor = uint8_t(partialDescriptor << 1u);

				const size_t pair = i * 8 + j;
				ocean_assert(pair < FREAKDescriptorT<tSize>::numberOfCellPairs);
				ocean_assert(FREAKDescriptorT<tSize>::cellPairs[pair][0] < FREAKDescriptorT<tSize>::numberOfCells);
				ocean_assert(FREAKDescriptorT<tSize>::cellPairs[pair][1] < FREAKDescriptorT<tSize>::numberOfCells);

				if (cellIntensities[FREAKDescriptorT<tSize>::cellPairs[pair][0]] > cellIntensities[FREAKDescriptorT<tSize>::cellPairs[pair][1]])
				{
					partialDescriptor = partialDescriptor | 1u;
				}
			}
			freakDescriptor.data_[scaleLevel][i] = partialDescriptor;
		}

		freakDescriptor.dataLevels_ = (unsigned int)(scaleLevel + 1);
	}

	ocean_assert(freakDescriptor.isValid() == false || (NumericF::isInsideRange(-NumericF::pi(), freakDescriptor.orientation_, NumericF::pi()) && freakDescriptor.dataLevels_ <= 3u));

	return freakDescriptor.isValid();
}

template <size_t tSize>
template <bool tEnableBorderChecks>
bool FREAKDescriptorT<tSize>::computeAverageCellIntensity(const Frame& framePyramidLayer, int cellX, int cellY, const int* kernelX, const int* kernelY, const size_t kernelElements, PixelType& averageIntensity)
{
	ocean_assert(framePyramidLayer.isValid());
	ocean_assert(kernelX != nullptr && kernelY != nullptr && kernelElements != 0u);

	const unsigned int width = framePyramidLayer.width();
	const unsigned int height = framePyramidLayer.height();
	const unsigned int frameStrideElements = framePyramidLayer.strideElements();
	const PixelType* frame = framePyramidLayer.constdata<PixelType>();

	if constexpr (tEnableBorderChecks)
	{
		unsigned int sum = 0u;
		unsigned int sumElements = 0u;

		for (size_t i = 0; i < kernelElements; ++i)
		{
			const int x = cellX + kernelX[i];
			const int y = cellY + kernelY[i];

			if (x >= 0 && x < int(width) && y >= 0 && y < int(height))
			{
				sum += frame[(unsigned int)y * frameStrideElements + (unsigned int)x];
				sumElements++;
			}
		}

		ocean_assert(sumElements != 0u);
		ocean_assert(float(sum) / float(sumElements) <= 255.0f);

		averageIntensity = PixelType(float(sum) / float(sumElements)); // TODOX No rounding in original. Add it here?
	}
	else
	{
		unsigned int sum = 0u;

		for (size_t i = 0; i < kernelElements; ++i)
		{
			const int x = cellX + kernelX[i];
			const int y = cellY + kernelY[i];
			ocean_assert_and_suppress_unused(x >= 0 && x < int(width) && y >= 0 && y < int(height), height);

			sum += frame[(unsigned int)y * frameStrideElements + (unsigned int)x];
		}

		ocean_assert(float(sum) / float(kernelElements) <= 255.0f);

		averageIntensity = PixelType(float(sum) / float(kernelElements)); // TODOX No rounding in original. Add it here?
	}

	return true;
}

template <size_t tSize>
void FREAKDescriptorT<tSize>::computeDescriptorsSubset(const FramePyramid* framePyramid, const Eigen::Vector2f* points, const size_t pointsSize, const unsigned int pointsPyramidLevel, FREAKDescriptorT<tSize>* freakDescriptor, const float inverseFocalLength, const CameraDerivativeFunctor* cameraDerivativeFunctor, const unsigned int firstPoint, const unsigned int numberOfPoints)
{
	ocean_assert(framePyramid != nullptr && framePyramid->isValid());
	ocean_assert(points != nullptr && pointsSize != 0u);
	ocean_assert(pointsPyramidLevel < framePyramid->layers());
	ocean_assert(freakDescriptor != nullptr);
	ocean_assert(inverseFocalLength > 0.0f);
	ocean_assert(cameraDerivativeFunctor != nullptr);
	ocean_assert_and_suppress_unused(firstPoint + numberOfPoints <= pointsSize && numberOfPoints != 0u, pointsSize);

	for (unsigned int i = firstPoint; i < firstPoint + numberOfPoints; ++i)
	{
		ocean_assert(i < pointsSize);
		const CameraDerivativeData data = cameraDerivativeFunctor->computeCameraDerivativeData(points[i], pointsPyramidLevel);
		computeDescriptor(*framePyramid, points[i], pointsPyramidLevel, freakDescriptor[i], data.unprojectRayIF, inverseFocalLength, data.pointJacobianMatrixIF);
	}
}

template <size_t tSize>
bool FREAKDescriptorT<tSize>::computeLocalDeformationMatrixAndOrientation(const FramePyramid& pyramid, const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, const Eigen::Vector3f& unprojectRayIF, const float inverseFocalLengthX, const PointJacobianMatrix2x3& projectionJacobianMatrix, Eigen::Matrix<float, 2, 2> & deformationMatrix, float& orientation)
{
	ocean_assert(pyramid.isValid());
	ocean_assert(pointPyramidLevel < pyramid.layers());
	ocean_assert(pyramid.frameType().isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(NumericF::isEqualEps(unprojectRayIF.norm() - 1.0f));
	ocean_assert(inverseFocalLengthX > 0);
	ocean_assert(projectionJacobianMatrix.IsRowMajor == false);

	// In the plane perpendicular to the unprojection ray, determine two arbitrary but perpendicular vectors

	const Eigen::Vector3f directionY(0, 1, 0);
	const Eigen::Vector3f nx = directionY.cross(unprojectRayIF).normalized() * inverseFocalLengthX;
	const Eigen::Vector3f ny = unprojectRayIF.cross(nx);

	// Compute an initial warping matrix from the perpendicular vectors

	Eigen::Matrix<float, 3, 2> N;
	N.col(0) = nx;
	N.col(1) = ny;
	const Eigen::Matrix<float, 2, 2> initialDeformationMatrix = projectionJacobianMatrix * N;

	// Make sure that the orientation kernel (radius 7) fits inside the current pyramid layer

	constexpr float cornerX[4] = {-7.0f, -7.0f, 7.0f, 7.0f};
	constexpr float cornerY[4] = {-7.0f, 7.0f, -7.0f, 7.0f};
	const Frame& framePyramidLevel = pyramid.layer(pointPyramidLevel);

	for (size_t i = 0; i < 4; ++i)
	{
		const Eigen::Vector2f warpedCorner = point + initialDeformationMatrix * Eigen::Vector2f(cornerX[i], cornerY[i]);

		const unsigned int x = (unsigned int)(NumericF::round32(warpedCorner.x()));
		const unsigned int y = (unsigned int)(NumericF::round32(warpedCorner.y()));

		if (x >= framePyramidLevel.width() || y >= framePyramidLevel.height())
		{
			return false;
		}
	}

	// Compute weighted intensity over the kernel

	int magnitudeX = 0;
	int magnitudeY = 0;
	const unsigned int strideElements = framePyramidLevel.strideElements();
	const PixelType* data = framePyramidLevel.constdata<PixelType>();

	for (size_t i = 0; i < FREAKDescriptorT<tSize>::kernelRadius7Elements; ++i)
	{
		const Eigen::Vector2f p = point + initialDeformationMatrix * Eigen::Vector2f(float(FREAKDescriptorT<tSize>::kernelRadius7X[i]), float(FREAKDescriptorT<tSize>::kernelRadius7Y[i]));

		const int u = NumericF::round32(p[0]);
		const int v = NumericF::round32(p[1]);

		ocean_assert(((unsigned int)(v) * strideElements + (unsigned int)(u)) < framePyramidLevel.size());
		const int intensity = int(data[(unsigned int)(v) * strideElements + (unsigned int)(u)]);

		// TODOX Is weighting with the relative x-/y-offsets of the kernel correct? Pixels on the border of kernel have much larger weight (up to +/-7) than ones closer to the kernel center (as low as 0 for the center)
		magnitudeX += FREAKDescriptorT<tSize>::kernelRadius7X[i] * intensity;
		magnitudeY += FREAKDescriptorT<tSize>::kernelRadius7Y[i] * intensity;
	}

	if (magnitudeX == 0 && magnitudeY == 0)
	{
		return false;
	}

	// Compute axes aligned with keypoint orientation and use them to compute the deformation matrix

	const Eigen::Vector3f gy = (nx * float(magnitudeX) + ny * float(magnitudeY)).normalized() * inverseFocalLengthX;
	const Eigen::Vector3f gx = gy.cross(unprojectRayIF);

	Eigen::Matrix<float, 3, 2> G;
	G.col(0) = gx;
	G.col(1) = gy;

	deformationMatrix = projectionJacobianMatrix * G;

	// Compute angle in image coordinates

	const Eigen::Vector2f patchY = projectionJacobianMatrix * gy;
	orientation = NumericF::atan2(patchY[1], patchY[0]);
	ocean_assert(-NumericF::pi() < orientation && orientation <= NumericF::pi());

	return true;
}

template <size_t tSize>
FramePyramid FREAKDescriptorT<tSize>::createFramePyramidWithBlur8BitsPerChannel(const Frame& frame, const unsigned int kernelWidth, const unsigned int kernelHeight, const unsigned int layers, Worker* worker)
{
	ocean_assert(frame.isValid() && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(kernelWidth != 0u && kernelWidth % 2u == 1u);
	ocean_assert(kernelHeight != 0u && kernelHeight % 2u == 1u);
	ocean_assert(layers >= 1u);

	Frame reusableFrame;

	const FramePyramid::DownsamplingFunction downsamplingFunction = std::bind(&FREAKDescriptorT<tSize>::blurAndDownsampleByTwo11, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, kernelWidth, kernelHeight, reusableFrame);

	FramePyramid framePyramid(frame, downsamplingFunction, layers, true /*copyFirstLayer*/, worker);

	if (framePyramid.layers() != layers)
	{
		return FramePyramid();
	}

	return framePyramid;
}

template <size_t tSize>
bool FREAKDescriptorT<tSize>::blurAndDownsampleByTwo11(const Frame& finerLayer, Frame& coarserLayer, Worker* worker, const unsigned int kernelWidth, const unsigned int kernelHeight, Frame& reusableFrame)
{
	ocean_assert(finerLayer.isValid());
	ocean_assert(coarserLayer.isValid());

	ocean_assert(kernelWidth >= 1u && kernelWidth % 2u == 1u);
	ocean_assert(kernelHeight >= 1u && kernelHeight % 2u == 1u);

	ocean_assert(finerLayer.numberPlanes() == 1u && finerLayer.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(finerLayer.isPixelFormatCompatible(coarserLayer.pixelFormat()));

	if (!reusableFrame.set(finerLayer.frameType(), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(reusableFrame.isValid());

	const Frame* sourceLayer = &finerLayer;

	if (kernelWidth <= finerLayer.width() && kernelHeight <= finerLayer.height())
	{
		if (!CV::FrameFilterGaussian::filter<uint8_t, uint32_t>(finerLayer.constdata<uint8_t>(), reusableFrame.data<uint8_t>(), finerLayer.width(), finerLayer.height(), finerLayer.channels(), finerLayer.paddingElements(), reusableFrame.paddingElements(), kernelWidth, kernelHeight, -1.0f, worker))
		{
			return false;
		}

		sourceLayer = &reusableFrame;
	}

	CV::FrameShrinker::downsampleByTwo8BitPerChannel11(sourceLayer->constdata<uint8_t>(), coarserLayer.data<uint8_t>(), sourceLayer->width(), sourceLayer->height(), sourceLayer->channels(), sourceLayer->paddingElements(), coarserLayer.paddingElements(), worker);

	return true;
}

template <size_t tSize>
bool FREAKDescriptorT<tSize>::extractHarrisCornersAndComputeDescriptors(const Frame& yFrame, const unsigned int maxFrameArea, const unsigned int minFrameArea, const unsigned int expectedHarrisCorners640x480, const Scalar harrisCornersReductionScale, const unsigned int harrisCornerThreshold, const float inverseFocalLength, const CameraDerivativeFunctor& cameraDerivativeFunctor, HarrisCorners& corners, Indices32& cornerPyramidLevels, std::vector<FREAKDescriptorT<tSize>>& descriptors, const bool removeInvalid, const Scalar border, const bool determineExactHarrisCornerPositions, const bool yFrameIsUndistorted, Worker* worker)
{
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<std::uint8_t, 1u>()));
	ocean_assert(minFrameArea != 0u && minFrameArea <= maxFrameArea);
	ocean_assert(expectedHarrisCorners640x480 != 0u);
	ocean_assert(harrisCornersReductionScale > Scalar(0) && harrisCornersReductionScale < Scalar(1));
	ocean_assert(harrisCornerThreshold <= 512u);
	ocean_assert(inverseFocalLength > 0.0f);
	ocean_assert(border > Scalar(0) && Scalar(2) * border < Scalar(yFrame.width()) && Scalar(2) * border < Scalar(yFrame.height()));

	corners.clear();
	cornerPyramidLevels.clear();
	descriptors.clear();

	// Determine the range of layers in the pyramid that are of interest (expressed as a range of minimum and maximum area of the frames). Note: area shrinks by a factor of 4 with each coarser layer

	const unsigned int frameArea = yFrame.width() * yFrame.height();
	const unsigned int startLayerIndex = (unsigned int)std::max(0, Numeric::round32((Numeric::log10(Scalar(frameArea) / Scalar(maxFrameArea)) / Numeric::log10(Scalar(4)))));
	const unsigned int lastLayerIndex = (unsigned int)(Numeric::log10(Scalar(frameArea) / Scalar(minFrameArea)) / Numeric::log10(Scalar(4)));
	ocean_assert(startLayerIndex <= lastLayerIndex);

	// Generate a frame pyramid (+1 extra layer)

	const FramePyramid pyramid = FREAKDescriptorT<tSize>::createFramePyramidWithBlur8BitsPerChannel(yFrame, 5u, 5u, lastLayerIndex + 2u, worker);

	if (pyramid.isValid() == false || pyramid.layers() <= lastLayerIndex || cameraDerivativeFunctor.supportedPyramidLevels() <= lastLayerIndex)
	{
		return false;
	}

	// The number of expected Harris corners is defined at a reference image size of 640x480 pixels. So, it necessary to scale this number
	// to the actual size of the first pyramid layer that will be used and then scale it such that the total number of requested points is
	// distributed over all used pyramid layers.

	const unsigned int startLayerArea = pyramid[startLayerIndex].width() * pyramid[startLayerIndex].height();

#if 0
	// Disabled but leaving it here as reference
	unsigned int expectedHarrisCornersOnLevel = (unsigned int)(Numeric::round32(Scalar(expectedHarrisCorners640x480) * Scalar(startLayerArea) / Scalar(640u * 480u)));
	expectedHarrisCornersOnLevel = (unsigned int)(Scalar(expectedHarrisCornersOnLevel) * (Scalar(1) - harrisCornersReductionScale) / (Scalar(1) - std::pow(harrisCornersReductionScale, Scalar(lastLayerIndex - startLayerIndex))));
#else
	const Scalar expectedHarrisCornersOnStartLayerF = Scalar(expectedHarrisCorners640x480) * Scalar(startLayerArea) / Scalar(640u * 480u);
	unsigned int expectedHarrisCornersOnLevel = (unsigned int)Numeric::round32(expectedHarrisCornersOnStartLayerF * (Scalar(1) - harrisCornersReductionScale) / (Scalar(1) - std::pow(harrisCornersReductionScale, Scalar(lastLayerIndex - startLayerIndex))));
#endif

	// For each layer of the pyramid, extract Harris corners and compute their descriptors

	for (unsigned int layer = startLayerIndex; layer <= lastLayerIndex; ++layer)
	{
		ocean_assert(layer + 1u < pyramid.layers());
		ocean_assert(corners.size() == descriptors.size());
		ocean_assert(corners.size() == cornerPyramidLevels.size());

		if (expectedHarrisCornersOnLevel == 0u)
		{
			break;
		}

		const Frame& pyramidLayer = pyramid[layer];

		if (pyramidLayer.width() < Scalar(2) * border + Scalar(10) || pyramidLayer.height() < Scalar(2) * border + Scalar(10))
		{
			break;
		}

		HarrisCorners harrisCornersOnLevel;
		if (CV::Detector::HarrisCornerDetector::detectCorners(pyramidLayer.constdata<uint8_t>(), pyramidLayer.width(), pyramidLayer.height(), pyramidLayer.paddingElements(), harrisCornerThreshold, yFrameIsUndistorted, harrisCornersOnLevel, determineExactHarrisCornerPositions, worker) == false)
		{
			return false;
		}

		if (harrisCornersOnLevel.empty())
		{
			continue;
		}

		// Select new corners. Make sure they are distributed approximately equal. Append them to the corresponding return value (corners)

		ocean_assert(corners.size() == descriptors.size());
		ocean_assert(corners.size() == cornerPyramidLevels.size());
		const size_t firstNewCornerIndex = corners.size();

		if (harrisCornersOnLevel.size() > expectedHarrisCornersOnLevel)
		{
			// Sort corners by the corner strength in descending order and distribute them over a regular grid of bins

			std::sort(harrisCornersOnLevel.begin(), harrisCornersOnLevel.end());

			unsigned int horizontalBins = 0u;
			unsigned int verticalBins = 0u;
			Geometry::SpatialDistribution::idealBins(pyramidLayer.width(), pyramidLayer.height(), expectedHarrisCornersOnLevel / 2u, horizontalBins, verticalBins);
			ocean_assert(horizontalBins != 0u && verticalBins != 0u);

			const HarrisCorners newCorners = Geometry::SpatialDistribution::distributeAndFilter<HarrisCorner, HarrisCorner::corner2imagePoint>(harrisCornersOnLevel.data(), harrisCornersOnLevel.size(), border, border, Scalar(pyramidLayer.width()) - Scalar(2) * border, Scalar(pyramidLayer.height()) - Scalar(2) * border, horizontalBins, verticalBins, size_t(expectedHarrisCornersOnLevel));

			corners.insert(corners.end(), newCorners.begin(), newCorners.end());
		}
		else
		{
			for (const HarrisCorner& corner : harrisCornersOnLevel)
			{
				if (corner.observation().x() >= border && corner.observation().x() < Scalar(pyramidLayer.width()) - border &&
					corner.observation().y() >= border && corner.observation().y() < Scalar(pyramidLayer.height()) - border)
				{
					corners.emplace_back(corner);
				}
			}
		}

		ocean_assert(firstNewCornerIndex <= corners.size());
		const size_t newCornersAdded = corners.size() - firstNewCornerIndex;

		if (newCornersAdded == 0)
		{
			continue;
		}

		ocean_assert(firstNewCornerIndex + newCornersAdded == corners.size());

#if defined(OCEAN_DEBUG)
		for (size_t i = firstNewCornerIndex; i < corners.size(); ++i)
		{
			ocean_assert(corners[i].observation().x() >= border && corners[i].observation().x() <= Scalar(pyramidLayer.width()) - border);
			ocean_assert(corners[i].observation().y() >= border && corners[i].observation().y() <= Scalar(pyramidLayer.height()) - border);
		}
#endif // OCEAN_DEBUG

		// Store the pyramid level of the newly detected corners

		cornerPyramidLevels.insert(cornerPyramidLevels.end(), newCornersAdded, layer);

		// Extract the locations of the detected corners for the computation of their descriptors

		std::vector<Eigen::Vector2f> observations;
		observations.reserve(newCornersAdded);

		for (size_t i = firstNewCornerIndex; i < corners.size(); ++i)
		{
			observations.emplace_back(float(corners[i].observation().x()), float(corners[i].observation().y()));
		}

		// Scale inverse focal length defined at the finest pyramid layer to current pyramid layer:
		//
		// f - focal length at finest level of the image pyramid
		// l - current pyramid level
		// scale_l = 2^l - pyramid scale
		// f_l - scaled focal length at pyramid level l
		//
		//     f_l = f / scale_l
		// <=> 1 / f_l = scale_l * (1 / f)
		const float inverseFocalLengthAtLayer = float(1u << layer) * inverseFocalLength;

		// Compute the descriptors (and directly append them to the return value)

		ocean_assert(corners.size() > descriptors.size());
		descriptors.resize(corners.size());

		ocean_assert(descriptors.begin() + size_t(firstNewCornerIndex) + observations.size() == descriptors.end());
		FREAKDescriptorT<tSize>::computeDescriptors(pyramid, observations.data(), observations.size(), layer, descriptors.data() + firstNewCornerIndex, inverseFocalLengthAtLayer, cameraDerivativeFunctor, worker);

		expectedHarrisCornersOnLevel = (unsigned int)Numeric::round32(Scalar(expectedHarrisCornersOnLevel) * harrisCornersReductionScale);

		ocean_assert(corners.size() == descriptors.size());
		ocean_assert(corners.size() == cornerPyramidLevels.size());
	}

	ocean_assert(corners.size() == descriptors.size());
	ocean_assert(corners.size() == cornerPyramidLevels.size());

	if (removeInvalid && corners.empty() == false)
	{
		size_t i = 0;
		while (i < corners.size())
		{
			if (descriptors[i].isValid())
			{
				i++;
			}
			else
			{
				ocean_assert(corners.empty() == false);
				corners[i] = corners.back();
				corners.pop_back();

				cornerPyramidLevels[i] = cornerPyramidLevels.back();
				cornerPyramidLevels.pop_back();

				descriptors[i] = descriptors.back();
				descriptors.pop_back();
			}
		}
	}

	ocean_assert(corners.size() == descriptors.size());
	ocean_assert(corners.size() == cornerPyramidLevels.size());

	return true;
}

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_DETECTOR_FREAK_DESCRIPTOR_H
