/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_IMAGE_ALIGNMENT_DENSE_H
#define META_OCEAN_TRACKING_IMAGE_ALIGNMENT_DENSE_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements functions allowing dense image alignment.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT HomographyImageAlignmentDense
{
	public:

		/**
		 * Definition of an abstract base class allowing to store consistency data.
		 */
		class ConsistencyData
		{
			friend class ObjectRef<ConsistencyData>;

			public:

				/**
				 * Returns whether this object currently does not hold any consistency information.
				 * @return True, if so
				 */
				virtual bool isEmpty() = 0;

			protected:

				/**
				 * Protected default constructor.
				 */
				inline ConsistencyData();

				/**
				 * Disabled copy constructor.
				 * @param consistencyData Object which would be copied
				 */
				ConsistencyData(const ConsistencyData& consistencyData) = delete;

				/**
				 * Destructs this object.
				 */
				virtual ~ConsistencyData() = default;

				/**
				 * Disabled copy operator.
				 * @param consistencyData Object which would be copied
				 * @return Reference to this object
				 */
				ConsistencyData& operator=(const ConsistencyData& consistencyData) = delete;
		};

		/**
		 * Definition of an object reference holding a consistency data object.
		 */
		typedef ObjectRef<ConsistencyData> ConsistencyDataRef;

	protected:

		// Forward declaration.
		template <unsigned int tChannels>
		class HomographyProvider;

		// Forward declaration.
		template <unsigned int tChannels>
		class DenseAdditiveHomographyProvider;

		// Forward declaration.
		template <unsigned int tChannels>
		class DenseInverseCompositionalHomographyProvider;

	public:

		/**
		 * Optimizes the alignment between two images within a specified sub-region regarding a homography by application of an iterative additive delta Levenberg-Marquardt optimization approach.
		 * The resulting homography converts points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint), the given rough homography must be defined accordingly.<br>
		 * The performance of this function can be improved further if this function is invoked successively for the same template frame with identical parameters but with individual current frames.<br>
		 * In this case, an empty consistency data object must be provided for the first call, stored outside and must again be provided for the successive calls.
		 * @param templateFrame The template frame defining the reference to which the current frame will be aligned, must be valid
		 * @param templateSubRegion The sub-region defined within the template frame from which the visual content is used for the alignment, image content outside the sub-region is not investigated, must be valid
		 * @param currentFrame The current frame for which the alignment will be determine, must have the same pixel format and pixel origin as the template frame
		 * @param roughHomography An already rough homography defining the alignment between template and current frame, the homography transforms points defined in the template frame to points defined in the current frame
		 * @param homographyParameters The number of parameters used to define the homography, with range [8, 9]
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @param homography The resulting optimized homography for the specified template frame and current frame, with 1 in the lower right corner
		 * @param iterations The number of optimization iterations that will be applied at most, with range [1, infinity)
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting initial averaged squared error between template and current frame within the sub-region
		 * @param finalError Optional resulting final averaged squared error between template and current frame within the sub-region
		 * @param intermediateErrors Optional resulting intermediate averaged squared errors, on error for each improving optimization iteration
		 * @param abort Optional abort statement allowing to abort the alignment at any time; set the value True to abort the alignment
		 * @param externalConsistencyData Optional abstract consistency data object that may be used to improve the overall performance for several successive optimization calls for individual current frames but with the same template frame
		 * @see optimizeAlignmentInverseCompositional(), optimizeAlignmentMultiResolution().
		 */
		static bool optimizeAlignmentAdditive(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& roughHomography, const unsigned int homographyParameters, const bool zeroMean, SquareMatrix3& homography, const unsigned int iterations = 20u, Scalar lambda = 10, const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr, ConsistencyDataRef* externalConsistencyData = nullptr, bool* abort = nullptr);

		/**
		 * Optimizes the alignment between two images within a specified sub-region regarding a homography by application of an iterative inverse compositional delta Levenberg-Marquardt optimization approach.
		 * The resulting homography converts points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint), the given rough homography must be defined accordingly.<br>
		 * The performance of this function can be improved further if this function is invoked successively for the same template frame with identical parameters but with individual current frames.<br>
		 * In this case, an empty consistency data object must be provided for the first call, stored outside and must again be provided for the successive calls.
		 * @param templateFrame The template frame defining the reference to which the current frame will be aligned, must be valid
		 * @param templateSubRegion The sub-region defined within the template frame from which the visual content is used for the alignment, image content outside the sub-region is not investigated, must be valid
		 * @param currentFrame The current frame for which the alignment will be determine, must have the same pixel format and pixel origin as the template frame
		 * @param roughHomography An already rough homography defining the alignment between template and current frame, the homography transforms points defined in the template frame to points defined in the current frame
		 * @param homographyParameters The number of parameters used to define the homography, with range [8, 9]
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @param homography The resulting optimized homography for the specified template frame and current frame, with 1 in the lower right corner
		 * @param iterations The number of optimization iterations that will be applied at most, with range [1, infinity)
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting initial averaged squared error between template and current frame within the sub-region
		 * @param finalError Optional resulting final averaged squared error between template and current frame within the sub-region
		 * @param intermediateErrors Optional resulting intermediate averaged squared errors, on error for each improving optimization iteration
		 * @param abort Optional abort statement allowing to abort the alignment at any time; set the value True to abort the alignment
		 * @param externalConsistencyData Optional abstract consistency data object that may be used to improve the overall performance for several successive optimization calls for individual current frames but with the same template frame
		 * @see optimizeAlignmentAdditive(), optimizeAlignmentMultiResolution().
		 */
		static bool optimizeAlignmentInverseCompositional(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& roughHomography, const unsigned int homographyParameters, const bool zeroMean, SquareMatrix3& homography, const unsigned int iterations = 20u, Scalar lambda = 10, const Scalar lambdaFactor = 10, Scalar* initialError = nullptr, Scalar* finalError = nullptr, Scalars* intermediateErrors = nullptr, ConsistencyDataRef* externalConsistencyData = nullptr, bool* abort = nullptr);

		/**
		 * Optimizes the alignment between two images within a specified sub-region regarding a homography by applying a multi-resolution (coarse to fine approach) Gauss-Newton or a Levenberg-Marquardt optimization approach.
		 * The resulting homography converts points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint), the given rough homography must be defined accordingly.<br>
		 * The performance of this function can be improved further if this function is invoked successively for the same template frame with identical parameters but with individual current frames.<br>
		 * In this case, an empty consistency data object must be provided for the first call, stored outside and must again be provided for the successive calls.
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region defined in the template frames, must be valid
		 * @param currentFrame The current frame, must have the same pixel format, pixel origin and layer number as the pyramid of the template frame, must be valid
		 * @param numberPyramidLayers The number of pyramid layers to be used, with range [1, infinity)
		 * @param homographyParameters The number of parameters representing the homography with range [8, 9]
		 * @param additiveAlignment True, to apply the additive delta optimization; False, to apply the inverse compositional delta optimization
		 * @param levenbergMarquardtOptimization True, to apply the Levenberg-Marquardt optimization; False, to apply Gauss-Newton
		 * @param zeroMean True, to apply a zero-mean optimization (color intensities will be subtracted from the mean color intensity); False, to use the color intensities directly
		 * @param roughHomography The rough homography already known between the template frame and the current frame (currentPoint = H * templatePoint)
		 * @param homography The resulting precise homography
		 * @param coarseIterations The number of optimization iterations that will be applied at most on the coarsest pyramid layer, with range [1, infinity)
		 * @param fineIterations The number of optimization iterations that will be applied at most on the finest pyramid layer, the layers between coarsest and finest layer will be handled by a linear interpolation between both iteration values, with range [1, infinity)
		 * @param downsamplingMode The down-sampling mode that is applied to create the pyramid layers
		 * @return True, if succeeded
		 * @see optimizeAlignmentAdditive(), optimizeAlignmentInverseCompositional().
		 */
		static bool optimizeAlignmentMultiResolution(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const unsigned int numberPyramidLayers, const unsigned int homographyParameters, const bool additiveAlignment, const bool levenbergMarquardtOptimization, const bool zeroMean, const SquareMatrix3& roughHomography, SquareMatrix3& homography, const unsigned int coarseIterations = 20u, const unsigned int fineIterations = 4u, const CV::FramePyramid::DownsamplingMode downsamplingMode = CV::FramePyramid::DM_FILTER_14641);

		/**
		 * Optimizes the alignment between two images within a specified sub-region regarding a homography by applying a multi-resolution (coarse to fine approach) Gauss-Newton or a Levenberg-Marquardt optimization approach.
		 * The resulting homography converts points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint), the given rough homography must be defined accordingly.<br>
		 * The performance of this function can be improved further if this function is invoked successively for the same template frame with identical parameters but with individual current frames.<br>
		 * In this case, an empty consistency data object must be provided for the first call, stored outside and must again be provided for the successive calls.
		 * @param templateFramePyramid The pyramid frame of the template frame
		 * @param templateSubRegions The multi-resolution sub-regions defined in the template frames (one sub-region for each pyramid-layer)
		 * @param currentFramePyramid The pyramid frame of the current frame, must have the same pixel format, pixel origin and layer number as the pyramid of the template frame
		 * @param layers The number of pyramid layers to be used for the alignment, with range [1, min(templateFramePyramid.layers(), currentFramePyramid.layers()]
		 * @param homographyParameters The number of parameters representing the homography with range [8, 9]
		 * @param additiveAlignment True, to apply the additive delta optimization; False, to apply the inverse compositional delta optimization
		 * @param levenbergMarquardtOptimization True, to apply the Levenberg-Marquardt optimization; False, to apply Gauss-Newton
		 * @param zeroMean True, to apply a zero-mean optimization (color intensities will be subtracted from the mean color intensity); False, to use the color intensities directly
		 * @param roughHomography The rough homography already known between the template frame and the current frame (currentPoint = H * templatePoint)
		 * @param homography The resulting precise homography
		 * @param coarseIterations The number of optimization iterations that will be applied at most on the coarsest pyramid layer, with range [1, infinity)
		 * @param fineIterations The number of optimization iterations that will be applied at most on the finest pyramid layer, the layers between coarsest and finest layer will be handled by a linear interpolation between both iteration values, with range [1, infinity)
		 * @param consistencyDatas The optional consistency data objects to improve the optimization performance, either no objects or one object per pyramid layer
		 * @return True, if succeeded
		 * @see optimizeAlignmentAdditive(), optimizeAlignmentInverseCompositional().
		 */
		static bool optimizeAlignmentMultiResolution(const CV::FramePyramid& templateFramePyramid, const std::vector<CV::SubRegion>& templateSubRegions, const CV::FramePyramid& currentFramePyramid, const unsigned int layers, const unsigned int homographyParameters, const bool additiveAlignment, const bool levenbergMarquardtOptimization, const bool zeroMean, const SquareMatrix3& roughHomography, SquareMatrix3& homography, const unsigned int coarseIterations = 20u, const unsigned int fineIterations = 4u, std::vector<Tracking::HomographyImageAlignmentDense::ConsistencyDataRef>* consistencyDatas = nullptr);

	protected:

		/**
		 * Determines the current error for a given homography between a current frame and a template frame within a sub-region of the template frame.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).<br>
		 * We keep this implementation for debugging and for better understanding of the overall algorithm.
		 * @param templateFrame The template frame for which the error is determined, with 8 bit per pixel and 1-4 channels
		 * @param templateSubRegion The sub-region defined within the template frame
		 * @param currentFrame The current frame for which the error is determined, must have the same pixel format and pixel origin as the template frame
		 * @param homography The homography for which the error is determined, transforming points defined in the template frame to points defined in the current frame, with 1 in the lower right corner
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @return The resulting average squared error for all valid pixel correspondences
		 */
		static Scalar slowDetermineError(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homography, const bool zeroMean);

		/**
		 * Determines the current error for a given homography between a current frame and a template frame within a sub-region of the template frame.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).<br>
		 * We keep this implementation for debugging and for better understanding of the overall algorithm.
		 * @param templateFrame The template frame for which the error is determined, with 8 bit per pixel and 1-4 channels
		 * @param templateSubRegion The sub-region defined within the template frame
		 * @param currentFrame The current frame for which the error is determined, must have the same pixel format and pixel origin as the template frame
		 * @param homography The homography for which the error is determined, transforming points defined in the template frame to points defined in the current frame, with 1 in the lower right corner
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @return The resulting average squared error for all valid pixel correspondences
		 * @tparam tChannels The number of channels both frames have, with range [1, 4]
		 */
		template <unsigned int tChannels>
		static Scalar slowDetermineError8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homography, const bool zeroMean);

		/**
		 * Determines the 8x8 or 9x9 Hessian matrix and the 8x1 or 9x1 Jacobian-Error vector for a given homography between a current frame and a template frame within a sub-region of the template frame.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).<br>
		 * We keep this implementation for debugging and for better understanding of the overall algorithm.
		 * @param templateFrame The template frame for which the Hessian and Jacobian is determined, with 8 bit per pixel and 1-4 channels
		 * @param templateSubRegion The sub-region defined within the template frame
		 * @param currentFrame The current frame for which the Hessian and Jacobian is determined, must have the same pixel format and pixel origin as the template frame
		 * @param homography The homography for which the Hessian and Jacobian is determined, transforming points defined in the template frame to points defined in the current frame, with 1 in the lower right corner
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @param hessian The resulting Hessian matrix
		 * @param jacobianError The resulting Jacobian-Error matrix
		 * @return True, if succeeded
		 * @tparam tParameters The number of parameters describing the homography and used to optimize it, with range [8, 9]
		 */
		template <unsigned int tParameters>
		static bool slowDetermineHessianAndErrorJacobian(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homography, const bool zeroMean, Matrix& hessian, Matrix& jacobianError);

		/**
		 * Determines the 8x8 or 9x9 Hessian matrix and the 8x1 or 9x1 Jacobian-Error vector for a given homography between a current frame and a template frame within a sub-region of the template frame.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).<br>
		 * We keep this implementation for debugging and for better understanding of the overall algorithm.
		 * @param templateFrame The template frame for which the Hessian and Jacobian is determined, with 8 bit per pixel and 1-4 channels
		 * @param templateSubRegion The sub-region defined within the template frame
		 * @param currentFrame The current frame for which the Hessian and Jacobian is determined, must have the same pixel format and pixel origin as the template frame
		 * @param homography The homography for which the Hessian and Jacobian is determined, transforming points defined in the template frame to points defined in the current frame, with 1 in the lower right corner
		 * @param hessian The resulting Hessian matrix
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @param jacobianError The resulting Jacobian-Error matrix
		 * @return True, if succeeded
		 * @tparam tParameters The number of parameters describing the homography and used to optimize it, with range [8, 9]
		 * @tparam tChannels The number of channels both frames have, with range [1, 4]
		 */
		template <unsigned int tParameters, unsigned int tChannels>
		static bool slowDetermineHessianAndErrorJacobian8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& currentFrame, const SquareMatrix3& homography, const bool zeroMean, Matrix& hessian, Matrix& jacobianError);

		/**
		 * Determines the mean color intensities in the corresponding sub-regions of the current transformed frame and also optional in the template frame while optional a mask frame defines valid and invalid pixels (not counting for error determination).
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region in which the mean color intensities are determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedCurrentSubFrameMask Optional mask that has been created during the creation of the sub-region of the transformed current frame, nullptr to avoid the usage of any mask which is more efficient
		 * @param templateMeans The resulting mean color intensities in the template frame, if 'tDetermineTemplateMeans' is True
		 * @param currentMeans The resulting mean color intensities in the current transformed frame
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tDetermineTemplateMeans True, to determine the mean intensities for the current frame and the template frame; False, to determine the mean intensities for the current frame only
		 */
		template <unsigned int tChannels, bool tDetermineTemplateMeans>
		static inline bool determineMeans8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrame, const Frame& transformedCurrentSubFrameMask, Scalar* templateMeans, Scalar* currentMeans, Worker* worker);

		/**
		 * Determines the color intensity error within a sub-region between a template frame and a transformed current frame while optional a mask frame defines valid and invalid pixels (not counting for error determination).
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The frame data of the template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrameData The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedCurrentSubFrameMaskData Optional mask that has been created during the creation of the sub-region of the transformed current frame, nullptr to avoid the usage of a mask which is more efficient
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param worker Optional worker object to distribute the computation
		 * @return The average squared error for each pixel in the sub-region
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tChannels, bool tUseMeans>
		static inline Scalar determineError8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrameData, const Frame& transformedCurrentSubFrameMaskData, const Scalar* templateMeans, const Scalar* currentMeans, Worker* worker);

		/**
		 * Determines the Hessian matrix and the product of transposed Jacobian matrix and error vector for a template frame and current transformed frame within a sub-region while optional a mask frame defines valid and invalid pixels (not counting for error determination).
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedCurrentSubFrameMask Optional mask that has been created during the creation of the sub-region of the transformed current frame, nullptr to avoid the usage of any mask which is more efficient
		 * @param transformedBoundingBoxLeft The left border of the bounding box of the transformed sub-region in pixel, with range [0, currentFrame.width())
		 * @param transformedBoundingBoxTop the top border of the bounding box of the transformed sub-region in pixel, with range [0, currentFrame.height())
		 * @param transformedBoundingBoxWidth The width of the bounding box of the transformed sub-region in pixel, with range [1, currentFrame.width() - transformedBoundingBoxLeft]
		 * @param transformedBoundingBoxHeight The height of the bounding box of the transformed sub-region in pixel, with range [1, currentFrame.height() - transformedBoundingBoxTop]
		 * @param gradientCurrentFrame The gradient filter responses of the current frame restricted to the location and size of the transformed bounding box, must be valid
		 * @param homography The homography to be used to transform the current frame with
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param hessian The resulting Hessian matrix, with dimension (tParameters x tParameters)
		 * @param jacobianError The resulting product of transposed Jacobian matrix and error vector, with dimension (tParameters x 1)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tParameters The number of parameters that are used to optimize the homography, with range [8, 9]
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
		static inline void determineHessianAndErrorJacobian8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrame, const Frame& transformedCurrentSubFrameMask, const unsigned int transformedBoundingBoxLeft, const unsigned int transformedBoundingBoxTop, const unsigned int transformedBoundingBoxWidth, const unsigned int transformedBoundingBoxHeight, const Frame& gradientCurrentFrame, const SquareMatrix3& homography, const Scalar* templateMeans, const Scalar* currentMeans, Matrix& hessian, Matrix& jacobianError, Worker* worker);

		/**
		 * Determines the product of transposed Jacobian matrix and error vector for the inverse compositional optimization approach for a template frame and current transformed frame within a sub-region while optional a mask frame defines valid and invalid pixels (not counting for error determination).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedCurrentSubFrameMask Optional mask that has been created during the creation of the sub-region of the transformed current frame, nullptr to avoid the usage of any mask which is more efficient
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param jacobianRows The already determine (and constant) Jacobian rows for each pixel and channel of the template sub-region, each row has dimension (1 x tParameters)
		 * @param jacobianError The resulting product of transposed Jacobian matrix and error vector, with dimension (tParameters x 1)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tParameters The number of parameters that are used to optimize the homography, with range [8, 9]
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
		static inline void determineErrorJacobianInverseCompositional8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrame, const Frame& transformedCurrentSubFrameMask, const Scalar* templateMeans, const Scalar* currentMeans, const Scalar* jacobianRows, Matrix& jacobianError, Worker* worker);

		/**
		 * Determines the mean color intensities in a subset of the corresponding sub-regions of the current transformed frame and also optional in the template frame.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region in which the mean color intensities are determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param templateMeans The resulting sum of color intensities in the template frame, if 'tDetermineTemplateMeans' is True, must be initialized with 0
		 * @param currentMeans The resulting sum of color intensities in the current transformed frame, must be initialized with 0
		 * @param templateMeansDenominator The number of pixels that have been used to determine the sum of intensities in the template frame, must be initialized with 0
		 * @param currentMeansDenominator The number of pixels that have been used to determine the sum of intensities in the current transformed frame, must be initialized with 0
		 * @param lock The lock object necessary if this function is executed on multiple threads in parallel, nullptr otherwise
		 * @param threads The number of threads on which this function is executed in parallel
		 * @param threadIndex The index of the current thread, with range [0, threads)
		 * @param unused An unused parameter must be 1
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tDetermineTemplateMeans True, to determine the mean intensities for the current frame and the template frame; False, to determine the mean intensities for the current frame only
		 * @see determineMeans8BitPerChannel().
		 */
		template <unsigned int tChannels, bool tDetermineTemplateMeans>
		static void determineMeans8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, Scalar* templateMeans, Scalar* currentMeans, unsigned int* templateMeansDenominator, unsigned int* currentMeansDenominator, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused);

		/**
		 * Determines the mean color intensities in a subset of the corresponding sub-regions of the current transformed frame and also optional in the template frame.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region in which the mean color intensities are determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedCurrentSubFrameMask The mask that has been created during the creation of the sub-region of the transformed current frame
		 * @param templateMeans The resulting sum of color intensities in the template frame, if 'tDetermineTemplateMeans' is True, must be initialized with 0
		 * @param currentMeans The resulting sum of color intensities in the current transformed frame, must be initialized with 0
		 * @param templateMeansDenominator The number of pixels that have been used to determine the sum of intensities in the template frame, must be initialized with 0
		 * @param currentMeansDenominator The number of pixels that have been used to determine the sum of intensities in the current transformed frame, must be initialized with 0
		 * @param lock The lock object necessary if this function is executed on multiple threads in parallel, nullptr otherwise
		 * @param threads The number of threads on which this function is executed in parallel
		 * @param threadIndex The index of the current thread, with range [0, threads)
		 * @param unused An unused parameter must be 1
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tDetermineTemplateMeans True, to determine the mean intensities for the current frame and the template frame; False, to determine the mean intensities for the current frame only
		 * @see determineMeans8BitPerChannel().
		 */
		template <unsigned int tChannels, bool tDetermineTemplateMeans>
		static void determineMeansMask8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Frame* transformedCurrentSubFrameMask, Scalar* templateMeans, Scalar* currentMeans, unsigned int* templateMeansDenominator, unsigned int* currentMeansDenominator, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused);

		/**
		 * Determines the color intensity error in a subset of a sub-region between a template frame and a transformed current frame.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param squaredError The resulting sum of squared errors, must be initialized with 0
		 * @param errorDenominators The number of pixels that have been used to determine the error, must be initialized with 0
		 * @param lock The lock object necessary if this function is executed on multiple threads in parallel, nullptr otherwise
		 * @param threads The number of threads on which this function is executed in parallel
		 * @param threadIndex The index of the current thread, with range [0, threads)
		 * @param unused An unused parameter must be 1
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tChannels, bool tUseMeans>
		static void determineError8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Scalar* templateMeans, const Scalar* currentMeans, Scalar* squaredError, unsigned int* errorDenominators, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused);

		/**
		 * Determines the color intensity error in a subset of a sub-region between a template frame and a transformed current frame while a mask frame defines valid and invalid pixels (not counting for error determination).
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedCurrentSubFrameMask The mask that has been created during the creation of the sub-region of the transformed current frame
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param squaredError The resulting sum of squared errors, must be initialized with 0
		 * @param errorDenominators The number of pixels that have been used to determine the error, must be initialized with 0
		 * @param lock The lock object necessary if this function is executed on multiple threads in parallel, nullptr otherwise
		 * @param threads The number of threads on which this function is executed in parallel
		 * @param threadIndex The index of the current thread, with range [0, threads)
		 * @param unused An unused parameter must be 1
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tChannels, bool tUseMeans>
		static void determineErrorMask8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Frame* transformedCurrentSubFrameMask, const Scalar* templateMeans, const Scalar* currentMeans, Scalar* squaredError, unsigned int* errorDenominators, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused);

		/**
		 * Determines the subset-Hessian matrix and the subset-product of transposed Jacobian matrix and error vector for a template frame and current transformed frame within a sub-region.
		 * The homography transformed points defined in the template frame to points defined in the current frame (currenttPoint = H * templatePoint).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedBoundingBoxLeft The left border of the bounding box of the transformed sub-region in pixel, with range [0, currentFrame.width())
		 * @param transformedBoundingBoxTop the top border of the bounding box of the transformed sub-region in pixel, with range [0, currentFrame.height())
		 * @param transformedBoundingBoxWidth The width of the bounding box of the transformed sub-region in pixel, with range [1, currentFrame.width() - transformedBoundingBoxLeft]
		 * @param transformedBoundingBoxHeight The height of the bounding box of the transformed sub-region in pixel, with range [1, currentFrame.height() - transformedBoundingBoxTop]
		 * @param gradientCurrentFrame The gradient filter responses of the current frame restricted to the location and size of the transformed bounding box, must be valid
		 * @param homography The homography to be used to transform the current frame with
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param hessian The resulting Hessian matrix, with dimension (tParameters x tParameters), must be initialized with 0
		 * @param jacobianError The resulting product of transposed Jacobian matrix and error vector, with dimension (tParameters x 1), must be initialized with 0
		 * @param lock The lock object necessary if this function is executed on multiple threads in parallel, nullptr otherwise
		 * @param threads The number of threads on which this function is executed in parallel
		 * @param threadIndex The index of the current thread, with range [0, threads)
		 * @param unused An unused parameter must be 1
		 * @tparam tParameters The number of parameters that are used to optimize the homography, with range [8, 9]
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
		static void determineHessianAndErrorJacobian8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const unsigned int transformedBoundingBoxLeft, const unsigned int transformedBoundingBoxTop, const unsigned int transformedBoundingBoxWidth, const unsigned int transformedBoundingBoxHeight, const Frame* gradientCurrentFrame, const SquareMatrix3* homography, const Scalar* templateMeans, const Scalar* currentMeans, Matrix* hessian, Matrix* jacobianError, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused);

		/**
		 * Determines the subset of the product of transposed Jacobian matrix and error vector for the inverse compositional optimization approach for a template frame and current transformed frame within a sub-region while optional a mask frame defines valid and invalid pixels (not counting for error determination).
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param jacobianRows The already determine (and constant) Jacobian rows for each pixel and channel of the template sub-region, each row has dimension (1 x tParameters)
		 * @param jacobianError The resulting product of transposed Jacobian matrix and error vector, with dimension (tParameters x 1), must be initialized with 0
		 * @param lock The lock object necessary if this function is executed on multiple threads in parallel, nullptr otherwise
		 * @param threads The number of threads on which this function is executed in parallel
		 * @param threadIndex The index of the current thread, with range [0, threads)
		 * @param unused An unused parameter must be 1
		 * @tparam tParameters The number of parameters that are used to optimize the homography, with range [8, 9]
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
		static void determineErrorJacobianInverseCompositional8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Scalar* templateMeans, const Scalar* currentMeans, const Scalar* jacobianRows, Matrix* jacobianError, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused);

		/**
		 * Determines the subset-Hessian matrix and the subset-product of transposed Jacobian matrix and error vector for a template frame and current transformed frame within a sub-region.
		 * @param templateFrame The template frame, must be valid
		 * @param templateSubRegion The sub-region for which the error is determined, defined in the template frame
		 * @param transformedCurrentSubFrame The sub-region from the transformed current frame, needs to be transformed with the homography with size and location equal to the sub-region
		 * @param transformedCurrentSubFrameMask The mask that has been created during the creation of the sub-region of the transformed current frame
		 * @param transformedBoundingBoxLeft The left border of the bounding box of the transformed sub-region in pixel, with range [0, currentFrame.width())
		 * @param transformedBoundingBoxTop the top border of the bounding box of the transformed sub-region in pixel, with range [0, currentFrame.height())
		 * @param transformedBoundingBoxWidth The width of the bounding box of the transformed sub-region in pixel, with range [1, currentFrame.width() - transformedBoundingBoxLeft]
		 * @param transformedBoundingBoxHeight The height of the bounding box of the transformed sub-region in pixel, with range [1, currentFrame.height() - transformedBoundingBoxTop]
		 * @param gradientCurrentFrame The gradient filter responses of the current frame restricted to the location and size of the transformed bounding box, must be valid
		 * @param current_H_template The homography converting the template image to the current image, must be valid
		 * @param templateMeans The mean color intensities in the sub-region of the template frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param currentMeans The mean color intensities in the current transformed frame, must be defined if 'tUseMeans' is True; otherwise nullptr
		 * @param hessian The resulting Hessian matrix, with dimension (tParameters x tParameters), must be initialized with 0
		 * @param jacobianError The resulting product of transposed Jacobian matrix and error vector, with dimension (tParameters x 1), must be initialized with 0
		 * @param lock The lock object necessary if this function is executed on multiple threads in parallel, nullptr otherwise
		 * @param threads The number of threads on which this function is executed in parallel
		 * @param threadIndex The index of the current thread, with range [0, threads)
		 * @param unused An unused parameter must be 1
		 * @tparam tParameters The number of parameters that are used to optimize the homography, with range [8, 9]
		 * @tparam tChannels The number of data channels of the provided frames
		 * @tparam tUseMeans True, if the mean color intensities are used to determine the errors; False, otherwise
		 */
		template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
		static void determineHessianAndErrorJacobianMask8BitPerChannelSubset(const Frame* templateFrame, const CV::SubRegion* templateSubRegion, const Frame* transformedCurrentSubFrame, const Frame* transformedCurrentSubFrameMask, const unsigned int transformedBoundingBoxLeft, const unsigned int transformedBoundingBoxTop, const unsigned int transformedBoundingBoxWidth, const unsigned int transformedBoundingBoxHeight, const Frame* gradientCurrentFrame, const SquareMatrix3* current_H_template, const Scalar* templateMeans, const Scalar* currentMeans, Matrix* hessian, Matrix* jacobianError, Lock* lock, const unsigned int threads, const unsigned int threadIndex, const unsigned int unused);
};

inline HomographyImageAlignmentDense::ConsistencyData::ConsistencyData()
{
	// nothing to do here
}

template <unsigned int tChannels, bool tDetermineTemplateMeans>
inline bool HomographyImageAlignmentDense::determineMeans8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrame, const Frame& transformedCurrentSubFrameMask, Scalar* templateMeans, Scalar* currentMeans, Worker* worker)
{
	ocean_assert(templateFrame.isValid() && templateFrame.channels() == tChannels && templateFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(!transformedCurrentSubFrameMask.isValid() || (transformedCurrentSubFrameMask.width() == transformedCurrentSubFrame.width() && transformedCurrentSubFrameMask.height() == transformedCurrentSubFrame.height()));

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		currentMeans[n] = Scalar(0);

		if constexpr (tDetermineTemplateMeans)
		{
			templateMeans[n] = Scalar(0);
		}
	}

	unsigned int templateMeansDenominator = 0u;
	unsigned int currentMeansDenominator = 0u;

	if (worker)
	{
		Lock lock;

		if (transformedCurrentSubFrameMask.isValid())
		{
			worker->executeFunction(Worker::Function::createStatic(&determineMeansMask8BitPerChannelSubset<tChannels, tDetermineTemplateMeans>, &templateFrame, &templateSubRegion, &transformedCurrentSubFrame, &transformedCurrentSubFrameMask, templateMeans, currentMeans, &templateMeansDenominator, &currentMeansDenominator, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&determineMeans8BitPerChannelSubset<tChannels, tDetermineTemplateMeans>, &templateFrame, &templateSubRegion, &transformedCurrentSubFrame, templateMeans, currentMeans, &templateMeansDenominator, &currentMeansDenominator, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
	}
	else
	{
		if (transformedCurrentSubFrameMask.isValid())
		{
			determineMeansMask8BitPerChannelSubset<tChannels, tDetermineTemplateMeans>(&templateFrame, &templateSubRegion, &transformedCurrentSubFrame, &transformedCurrentSubFrameMask, templateMeans, currentMeans, &templateMeansDenominator, &currentMeansDenominator, nullptr, 1u, 0u, 1u);
		}
		else
		{
			determineMeans8BitPerChannelSubset<tChannels, tDetermineTemplateMeans>(&templateFrame, &templateSubRegion, &transformedCurrentSubFrame, templateMeans, currentMeans, &templateMeansDenominator, &currentMeansDenominator, nullptr, 1u, 0u, 1u);
		}
	}

	if (currentMeansDenominator == 0u)
	{
		return false;
	}

	const Scalar invCurrentDenominator = Scalar(1) / Scalar(currentMeansDenominator);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		currentMeans[n] *= invCurrentDenominator;
	}

	if constexpr (tDetermineTemplateMeans)
	{
		const Scalar invTemplateDenominator = Scalar(1) / Scalar(templateMeansDenominator);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			templateMeans[n] *= invTemplateDenominator;
		}
	}

	return true;
}

template <unsigned int tChannels, bool tUseMeans>
inline Scalar HomographyImageAlignmentDense::determineError8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrame, const Frame& transformedCurrentSubFrameMask, const Scalar* templateMeans, const Scalar* currentMeans, Worker* worker)
{
	ocean_assert(templateFrame.isValid() && templateFrame.channels() == tChannels && templateFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	Scalar errorValue = Scalar(0);
	unsigned int errorDenominator = 0u;

	if (worker)
	{
		Lock lock;

		if (transformedCurrentSubFrameMask.isValid())
		{
			worker->executeFunction(Worker::Function::createStatic(&determineErrorMask8BitPerChannelSubset<tChannels, tUseMeans>, &templateFrame, &templateSubRegion, &transformedCurrentSubFrame, &transformedCurrentSubFrameMask, templateMeans, currentMeans, &errorValue, &errorDenominator, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&determineError8BitPerChannelSubset<tChannels, tUseMeans>, &templateFrame, &templateSubRegion, &transformedCurrentSubFrame, templateMeans, currentMeans, &errorValue, &errorDenominator, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
	}
	else
	{
		if (transformedCurrentSubFrameMask.isValid())
		{
			determineErrorMask8BitPerChannelSubset<tChannels, tUseMeans>(&templateFrame, &templateSubRegion, &transformedCurrentSubFrame, &transformedCurrentSubFrameMask, templateMeans, currentMeans, &errorValue, &errorDenominator, nullptr, 1u, 0u, 1u);
		}
		else
		{
			determineError8BitPerChannelSubset<tChannels, tUseMeans>(&templateFrame, &templateSubRegion, &transformedCurrentSubFrame, templateMeans, currentMeans, &errorValue, &errorDenominator, nullptr, 1u, 0u, 1u);
		}
	}

	if (errorDenominator == 0u)
	{
		return Numeric::maxValue();
	}

	return errorValue / Scalar(errorDenominator * tChannels);
}

template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
inline void HomographyImageAlignmentDense::determineHessianAndErrorJacobian8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrame, const Frame& transformedCurrentSubFrameMask, const unsigned int transformedBoundingBoxLeft, const unsigned int transformedBoundingBoxTop, const unsigned int transformedBoundingBoxWidth, const unsigned int transformedBoundingBoxHeight, const Frame& gradientCurrentFrame, const SquareMatrix3& homography, const Scalar* templateMeans, const Scalar* currentMeans, Matrix& hessian, Matrix& jacobianError, Worker* worker)
{
	hessian = Matrix(tParameters, tParameters, false);
	jacobianError = Matrix(tParameters, 1, false);

	if (worker)
	{
		Lock lock;

		if (transformedCurrentSubFrameMask.isValid())
		{
			worker->executeFunction(Worker::Function::createStatic(&determineHessianAndErrorJacobianMask8BitPerChannelSubset<tParameters, tChannels, tUseMeans>, &templateFrame, &templateSubRegion, &transformedCurrentSubFrame, &transformedCurrentSubFrameMask, transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, &gradientCurrentFrame, &homography, templateMeans, currentMeans, &hessian, &jacobianError, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&determineHessianAndErrorJacobian8BitPerChannelSubset<tParameters, tChannels, tUseMeans>, &templateFrame, &templateSubRegion, &transformedCurrentSubFrame, transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, &gradientCurrentFrame, &homography, templateMeans, currentMeans, &hessian, &jacobianError, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
	}
	else
	{
		if (transformedCurrentSubFrameMask.isValid())
		{
			determineHessianAndErrorJacobianMask8BitPerChannelSubset<tParameters, tChannels, tUseMeans>(&templateFrame, &templateSubRegion, &transformedCurrentSubFrame, &transformedCurrentSubFrameMask, transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, &gradientCurrentFrame, &homography, templateMeans, currentMeans, &hessian, &jacobianError, nullptr, 1u, 0u, 1u);
		}
		else
		{
			determineHessianAndErrorJacobian8BitPerChannelSubset<tParameters, tChannels, tUseMeans>(&templateFrame, &templateSubRegion, &transformedCurrentSubFrame, transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, &gradientCurrentFrame, &homography, templateMeans, currentMeans, &hessian, &jacobianError, nullptr, 1u, 0u, 1u);
		}
	}
}

template <unsigned int tParameters, unsigned int tChannels, bool tUseMeans>
inline void HomographyImageAlignmentDense::determineErrorJacobianInverseCompositional8BitPerChannel(const Frame& templateFrame, const CV::SubRegion& templateSubRegion, const Frame& transformedCurrentSubFrame, const Frame& transformedCurrentSubFrameMask, const Scalar* templateMeans, const Scalar* currentMeans, const Scalar* jacobianRows, Matrix& jacobianError, Worker* worker)
{
	jacobianError = Matrix(tParameters, 1, false);

	if (worker)
	{
		Lock lock;

		if (transformedCurrentSubFrameMask.isValid())
		{
			ocean_assert(false && "**TODO**");
			//worker->executeFunction(Worker::Function::createStatic(&determineHessianAndErrorJacobianMaskInverseCompositional8BitPerChannelSubset<tParameters, tChannels, tUseMeans>, &templateFrame, &templateSubRegion, transformedCurrentSubFrameData, transformedCurrentSubFrameMaskData, transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, gradientCurrentData, &homography, templateMeans, currentMeans, &hessian, &jacobianError, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
		else
		{
			worker->executeFunction(Worker::Function::createStatic(&determineErrorJacobianInverseCompositional8BitPerChannelSubset<tParameters, tChannels, tUseMeans>, &templateFrame, &templateSubRegion, &transformedCurrentSubFrame, templateMeans, currentMeans, jacobianRows, &jacobianError, &lock, worker->threads(), 0u, 0u), 0u, worker->threads());
		}
	}
	else
	{
		if (transformedCurrentSubFrameMask.isValid())
		{
			ocean_assert(false && "**TODO**");
			//determineHessianAndErrorJacobianMaskInverseCompositional8BitPerChannelSubset<tParameters, tChannels, tUseMeans>(&templateFrame, &templateSubRegion, transformedCurrentSubFrameData, transformedCurrentSubFrameMaskData, transformedBoundingBoxLeft, transformedBoundingBoxTop, transformedBoundingBoxWidth, transformedBoundingBoxHeight, gradientCurrentData, &homography, templateMeans, currentMeans, &hessian, &jacobianError, nullptr, 1u, 0u, 1u);
		}
		else
		{
			determineErrorJacobianInverseCompositional8BitPerChannelSubset<tParameters, tChannels, tUseMeans>(&templateFrame, &templateSubRegion, &transformedCurrentSubFrame, templateMeans, currentMeans, jacobianRows, &jacobianError, nullptr, 1u, 0u, 1u);
		}
	}
}

}

}

#endif // META_OCEAN_TRACKING_IMAGE_ALIGNMENT_DENSE_H
