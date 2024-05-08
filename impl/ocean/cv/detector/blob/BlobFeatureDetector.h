/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_FEATURE_DETECTOR_BLOB_H
#define META_OCEAN_CV_DETECTOR_FEATURE_DETECTOR_BLOB_H

#include "ocean/cv/detector/blob/Blob.h"
#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
namespace Test { namespace TestCV { namespace TestDetector { class TestBlobFeatureDetector; } } }

namespace CV
{

namespace Detector
{

namespace Blob
{

// Forward declaration.
class BlobFeature;

/**
 * This class implements a detector for Blob features.
 * @ingroup cvdetectorblob
 */
class OCEAN_CV_DETECTOR_BLOB_EXPORT BlobFeatureDetector
{
	friend class Ocean::Test::TestCV::TestDetector::TestBlobFeatureDetector;

	public:

		/**
		 * Definition of different sampling densities.
		 */
		enum SamplingDense
		{
			// The most dense sampling.
			SAMPLING_DENSE = 0,
			// A normal sampling.
			SAMPLING_NORMAL = 1,
			// A sparse sampling.
			SAMPLING_SPARSE = 2
		};

	private:

		/**
		 * This class implements a Blob response map.
		 */
		class OCEAN_CV_DETECTOR_BLOB_EXPORT ResponseMap
		{
			public:

				/**
				 * Creates a new response map used to detect features in an entire image.
				 * @param frameWidth Width of the original frame in pixel
				 * @param frameHeight Height of the original frame in pixel
				 * @param samplingStep Sampling step for this response map in pixel in relation to the original frame size
				 * @param filterIndex Index of the filter to be used 1 is a filter of size 9x9, 2: 15x15, 3: 21x21 ...
				 * @param explicitOffset Explicit horizontal and vertical offset of the first sampling position to optimize the filter positions
				 */
				ResponseMap(const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int samplingStep, const unsigned int filterIndex, const unsigned int explicitOffset);

				/**
				 * Creates a new response map used to detect features in an image subset only.
				 * @param frameWidth Width of the original frame in pixel
				 * @param frameHeight Height of the original frame in pixel
				 * @param subframeLeft Horizontal start position of the image subset to be used (in pixel)
				 * @param subframeTop Vertical start position of the image subset to be used (in pixel)
				 * @param subframeWidth Width of the image subset to be used (in pixel)
				 * @param subframeHeight Height of the image subset to be used (in pixel)
				 * @param samplingStep Sampling step for this response map in pixel in relation to the original frame size
				 * @param filterIndex Index of the filter to be used 1 is a filter of size 9x9, 2: 15x15, 3: 21x21 ...
				 * @param explicitOffset Explicit horizontal and vertical offset of the first sampling position to optimize the filter positions
				 */
				ResponseMap(const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const unsigned int samplingStep, const unsigned int filterIndex, const unsigned int explicitOffset);

				/**
				 * Returns the width of the original frame in pixel.
				 * @return Original frame width
				 */
				inline unsigned int frameWidth() const;

				/**
				 * Returns the height of the original frame in pixel.
				 * @return Original frame height
				 */
				inline unsigned int frameHeight() const;

				/**
				 * Returns the width of the response map in pixel.
				 * Each pixel of this map is a filter result.
				 * @return Response width
				 */
				inline unsigned int responseWidth() const;

				/**
				 * Returns the height of the response map in pixel.
				 * Each pixel of this map is a filter result.
				 * @return Response height
				 */
				inline unsigned int responseHeight() const;

				/**
				 * Returns the sampling step of this map.
				 * @return Sampling step
				 */
				inline unsigned int samplingStep() const;

				/**
				 * Returns the size of the quadratic filter (horizontal or vertical).
				 * @return Filter size in pixel
				 */
				inline unsigned int filterSize() const;

				/**
				 * Returns the area of the used filter which is the square of the full filer size.
				 * @return Filter area
				 */
				inline unsigned int filterArea() const;

				/**
				 * Returns the index of the used filter.
				 * A 9x9 filter has an index of 1, 15x15 an index of 2, 21x21 and index of 3, 195x195 and index of 32.
				 * @return Filter index
				 */
				inline unsigned int filterIndex() const;

				/**
				 * Returns the horizontal position of the first response value in relation to the original frame.
				 * @return First response position
				 */
				inline unsigned int responseFirstX() const;

				/**
				 * Returns the vertical position of the first response value in relation to the original frame.
				 * @return First response position
				 */
				inline unsigned int responseFirstY() const;

				/**
				 * Returns the filter response of this map.
				 * @return Filter response
				 */
				inline const Scalar* filterResponse() const;

				/**
				 * Returns the Laplacian signs of this map.
				 * @return Laplacian signs
				 */
				inline const unsigned char* laplacianSign() const;

				/**
				 * Calculates the coordinate of a specified response value for the original frame dimension.
				 * @param responseX Horizontal response coordinate
				 * @param responseY Vertical response coordinate
				 * @param frameX Resulting horizontal frame coordinate
				 * @param frameY Resulting vertical frame coordinate
				 * @return True, if succeeded
				 */
				bool calculateOriginalPosition(const unsigned int responseX, const unsigned int responseY, unsigned int& frameX, unsigned int& frameY) const;

				/**
				 * Calculates the coordinate of a specified response value for the original frame dimension with floating point precision.
				 * @param responseX Horizontal response coordinate
				 * @param responseY Vertical response coordinate
				 * @param frameX Resulting horizontal frame coordinate
				 * @param frameY Resulting vertical frame coordinate
				 * @return True, if succeeded
				 */
				bool calculateOriginalPosition(const Scalar responseX, const Scalar responseY, Scalar& frameX, Scalar& frameY) const;

				/**
				 * Returns the coordinate of a specified original value for the response dimension.
				 * @param frameX Horizontal coordinate inside the original frame
				 * @param frameY Vertical coordinate inside the original frame
				 * @param responseX Resulting horizontal response coordinate
				 * @param responseY Resulting vertical response coordinate
				 * @return True, if succeeded
				 */
				bool calculateResponsePosition(const unsigned int frameX, const unsigned int frameY, unsigned int& responseX, unsigned int& responseY) const;

				/**
				 * Calculates the fast Hessian determinant for this response map.
				 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
				 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries
				 * @param worker Optional worker object used for computation distribution, nullptr if no worker is defined
				 */
				void filter(const uint32_t* linedIntegralImage, Worker* worker = nullptr);

				/**
				 * Calculates the parameters of the response map by the definition of the original frame size, the filter size, sampling step and explicit offset value.
				 * The size of a response map will be smaller than the size of the original frame depending of the filter border, the sampling step and the sub-image dimension.
				 * Beware: Because of the combination of different filter sizes, response layers and sampling steps each filtering point must be at an odd position (for x and y) in relation to the original frame dimension.
				 * @param frameWidth Original frame width in pixel
				 * @param frameHeight Original frame height in pixel
				 * @param subframeLeft Horizontal start position of the sub image
				 * @param subframeTop Vertical start position of the sub image
				 * @param subframeWidth Width of the sub image in pixel
				 * @param subframeHeight Height of the sub image in pixel
				 * @param filterSize Size of the quadratic filter in pixel
				 * @param samplingStep Sampling step in pixel, at least 1
				 * @param explicitOffset Explicit sampling offset
				 * @param responseWidth Resulting width of the response map in pixel
				 * @param responseHeight Resulting height of the response map in pixel
				 * @param responseFirstX Resulting horizontal start position of the response map in relation to the original frame
				 * @param responseFirstY Resulting vertical start position of the response map in relation to the original frame
				 * @param filterOffsetX Resulting horizontal offset of the filter in relation to the original frame
				 * @param filterOffsetY Resulting vertical offset of the filter in relation to the original frame
				 * @return True, if the parameters result in a valid response map
				 */
				static bool calculateResponseParameters(const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const unsigned int filterSize, const unsigned int samplingStep, const unsigned int explicitOffset, unsigned int& responseWidth, unsigned int& responseHeight, unsigned int& responseFirstX, unsigned int& responseFirstY, unsigned int& filterOffsetX, unsigned int& filterOffsetY);

				/**
				 * Returns whether this response map holds at least 3 response values in each axis.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			private:

				/**
				 * Calculates the fast Hessian determinant for a subset of this response map.
				 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries
				 * @param firstResponseRow Specifies the index of the first row to be filtered
				 * @param numberResponseRows Specifies the number of rows to be handled, with start point as specified
				 */
				void filterSubset(const uint32_t* linedIntegralImage, const unsigned int firstResponseRow, const unsigned int numberResponseRows);

			private:

				/// Width of the original frame in pixel.
				unsigned int mapFrameWidth;

				/// Height of the original frame in pixel.
				unsigned int mapFrameHeight;

				/// Width of the response map in pixel.
				unsigned int mapResponseWidth;

				/// Height of the response map in pixel.
				unsigned int mapResponseHeight;

				/// Sampling step of the response map in relation to the original frame.
				unsigned int mapSamplingStep;

				/// Index of the filter.
				unsigned int mapFilterIndex;

				/// Size of the quadratic filter (horizontal or vertical) in pixel.
				unsigned int mapFilterSize;

				/// Horizontal start position of the response map in relation to the original frame (filter half, explicit offset and sub-image offset).
				unsigned int mapResponseFirstX;

				/// Vertical start position of the response map in relation to the original frame (filter half, explicit offset and sub-image offset).
				unsigned int mapResponseFirstY;

				/// Horizontal offset of the filter in relation to the original frame (explicit offset and sub-image offset).
				unsigned int mapFilterOffsetX;

				/// Vertical offset of the filter in relation to the original frame (explicit offset and sub-image offset).
				unsigned int mapFilterOffsetY;

				/// Response map values.
				Frame mapFilterResponseFrame;

				/// Response map holding Laplacian sign values.
				Frame mapLaplacianSignFrame;
		};

	public:

		/**
		 * Detects Blob features inside a frame and may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries, must be valid
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param samplingDense Sampling dense to be used for feature detection
		 * @param threshold Lower bound for detected feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is defined
		 * @return Number of detected features
		 * @see BlobFeatureDetector::calculateOrientation(), BlobFeatureDetector::calculateDescriptor(), IntegralImage::createLinedImage().
		 */
		static inline unsigned int detectFeatures(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker = nullptr);

		/**
		 * Detects Blob features inside a frame and may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * This function detects Blob features inside one layer only. Thus all Blob features will have the same scale corresponding to the specified layer.<br>
		 * Thus, a non maximum suppression is processed in a 2D neighborhood instead of a 3D neighborhood.<br>
		 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries, must be valid
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param layer Filter layer to be used, the layer 1 uses 9x9 filter, layer 2 uses 15x15, ...
		 * @param samplingDense Sampling dense to be used for feature detection
		 * @param threshold Lower bound for detected feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is defined
		 * @return Number of detected features
		 * @see BlobFeatureDetector::calculateOrientation(), BlobFeatureDetector::calculateDescriptor(), IntegralImage::createLinedImage().
		 */
		static unsigned int detectFeatures(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const unsigned int layer, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker = nullptr);

		/**
		 * Detects Blob features inside a frame and may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries, must be valid
		 * @param width The width of the original frame in pixel, with range [1, infinity)
		 * @param height The height of the original frame in pixel, with range [1, infinity)
		 * @param subFrameLeft Horizontal position of the sub frame to detect features in, with range [0, width)
		 * @param subFrameTop Vertical position of the sub frame to detect features in, with range [0, height)
		 * @param subFrameWidth Width of the sub frame to detect features in, with range [1, width - subFrameLeft]
		 * @param subFrameHeight Height of the sub frame to detect features in, with range [1, height - subFrameTop]
		 * @param samplingDense Sampling dense to be used for feature detection
		 * @param threshold Lower bound for detected feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is defined
		 * @return Number of detected features
		 * @see BlobFeatureDetector::calculateOrientation(), BlobFeatureDetector::calculateDescriptor(), IntegralImage::createLinedImage().
		 */
		static unsigned int detectFeatures(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker = nullptr);

		/**
		 * Detects Blob features inside a frame and may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param frame The frame to detect Blob features in, will be converted internally if the pixel format and pixel origin do not match the internal requirements, must be valid
		 * @param samplingDense Sampling dense to be used for feature detection
		 * @param threshold Lower bound for detected feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is defined
		 * @return Number of detected features
		 * @see BlobFeatureDetector::calculateOrientation(), BlobFeatureDetector::calculateDescriptor(), IntegralImage::createLinedImage().
		 */
		static unsigned int detectFeatures(const Frame& frame, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker = nullptr);

		/**
		 * Detects Blob features inside a frame and may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param frame The frame to detect Blob features in, will be converted internally if the pixel format and pixel origin do not match the internal requirements, must be valid
		 * @param subFrameLeft Horizontal position of the sub frame to detect features in, with range [0, frame.width())
		 * @param subFrameTop Vertical position of the sub frame to detect features in, with range [0, frame.height())
		 * @param subFrameWidth Width of the sub frame to detect features in, with range [1, frame.width() - subFrameLeft]
		 * @param subFrameHeight Height of the sub frame to detect features inn, with range [1, frame.height() - subFrameTop]
		 * @param samplingDense Sampling dense to be used for feature detection
		 * @param threshold Lower bound for detected feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is defined
		 * @return Number of detected features
		 * @see BlobFeatureDetector::calculateOrientation(), BlobFeatureDetector::calculateDescriptor(), IntegralImage::createLinedImage().
		 */
		static unsigned int detectFeatures(const Frame& frame, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker = nullptr);

	private:

		/**
		 * Finds extremes inside a 3D neighborhood and may use an optional worker object to distribute the computation load.
		 * Candidates for a extreme can be on the middle response map only.<br>
		 * The low (sampler filter size) and high (larger filter size) response layers are used for comparison only.
		 * @param low Response map with a smaller filter than used for the middle layer
		 * @param middle Response map with possible extreme candidates
		 * @param high Response map with a larger filter than used for the middle layer
		 * @param threshold Lower bound for feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is defined
		 */
		static inline void findExtremes(const ResponseMap& low, const ResponseMap& middle, const ResponseMap& high, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker = nullptr);

		/**
		 * Finds extremes inside a 2D neighborhood and may use an optional worker object to distribute the computation load.
		 * @param map Response map holding the filter responses
		 * @param threshold Lower bound for feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is defined
		 */
		static inline void findExtremes(const ResponseMap& map, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker = nullptr);

		/**
		 * Finds extremes inside a 3D neighborhood in a specified response map subset
		 * Candidates for a extreme can be on the middle response map only.<br>
		 * The low (sampler filter size) and high (larger filter size) response layers are used for comparison only.
		 * @param low Response map with a smaller filter than used for the middle layer
		 * @param middle Response map with possible extreme candidates
		 * @param high Response map with a larger filter than used for the middle layer
		 * @param threshold Lower bound for feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param lock Optional lock if this function is executed distributed within several threads
		 * @param firstHighRow Specifies the index of the first row (inside the high response map) to be handled
		 * @param numberHighRows Specifies the number of rows to be handled (inside the high response map), with start point as specified
		 */
		static void findExtremesSubset(const ResponseMap* low, const ResponseMap* middle, const ResponseMap* high, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures* features, Lock* lock, const unsigned int firstHighRow, const unsigned int numberHighRows);

		/**
		 * Finds extremes inside a 2D neighborhood.
		 * @param map Response map holding the filter responses
		 * @param threshold Lower bound for feature strength
		 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param features Resulting features each holding a valid interpolated 2D position, a strength value and Laplacian sign
		 * @param lock Optional lock if this function is executed distributed within several threads
		 * @param firstResponseRow Specifies the index of the first response row to be handled
		 * @param numberResponseRows Specifies the number of response rows to be handled
		 */
		static void findExtremesSubset(const ResponseMap* map, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures* features, Lock* lock, const unsigned int firstResponseRow, const unsigned int numberResponseRows);

		/**
		 * Determines the exact feature position of an already extracted feature inside a 3D space.
		 * @param low Response map with a smaller filter than used for the middle layer
		 * @param middle Response map of the middle layer
		 * @param high Response map with a larger filter than used for the middle layer
		 * @param xHigh Horizontal feature position in the high layer
		 * @param yHigh Vertical feature position in the high layer
		 * @param iterations Number of iterations which can be used to determine the exact position, if more iterations are necessary the feature will be discarded
		 * @param distortionState Distortion state of the feature point
		 * @param features The features receiving the new feature
		 */
		static void determineExactFeaturePosition(const ResponseMap& low, const ResponseMap& middle, const ResponseMap& high, const unsigned int xHigh, const unsigned int yHigh, const unsigned int iterations, const BlobFeature::DistortionState distortionState, BlobFeatures& features);

		/**
		 * Determines the exact feature position of an already extracted feature inside a 2D space.
		 * @param map Response map the feature has been found inside.
		 * @param top The top response value of the feature (x, y - 1)
		 * @param middle Center response value of the feature (x, y)
		 * @param bottom The bottom response value of the feature (x, y + 1)
		 * @param x Initial and inaccurate horizontal position of the feature
		 * @param y Initial and inaccurate vertical position of the feature
		 * @param iterations Number of iterations which can be used to determine the exact position, if more iterations are necessary the feature will be discarded
		 * @param distortionState Distortion state of the feature point
		 * @param features The features receiving the new feature
		 */
		static void determineExactFeaturePosition(const ResponseMap& map, const Scalar* top, const Scalar* middle, const Scalar* bottom, const unsigned int x, const unsigned int y, const unsigned int iterations, const BlobFeature::DistortionState distortionState, BlobFeatures& features);

	private:

		/// Definition of all sampling steps for different sampling densities.
		static const unsigned int densitiesSamplingStep[3][10];

		/// Definition of all explicit sampling offsets for different sampling densities.
		static const unsigned int densitiesSamplingOffsets[3][10];
};

inline unsigned int BlobFeatureDetector::ResponseMap::frameWidth() const
{
	return mapFrameWidth;
}

inline unsigned int BlobFeatureDetector::ResponseMap::frameHeight() const
{
	return mapFrameHeight;
}

inline unsigned int BlobFeatureDetector::ResponseMap::responseWidth() const
{
	return mapResponseWidth;
}

inline unsigned int BlobFeatureDetector::ResponseMap::responseHeight() const
{
	return mapResponseHeight;
}

inline unsigned int BlobFeatureDetector::ResponseMap::samplingStep() const
{
	return mapSamplingStep;
}

inline unsigned int BlobFeatureDetector::ResponseMap::filterSize() const
{
	return mapFilterSize;
}

inline unsigned int BlobFeatureDetector::ResponseMap::filterArea() const
{
	return mapFilterSize * mapFilterSize;
}

inline unsigned int BlobFeatureDetector::ResponseMap::filterIndex() const
{
	return mapFilterIndex;
}

inline unsigned int BlobFeatureDetector::ResponseMap::responseFirstX() const
{
	return mapResponseFirstX;
}

inline unsigned int BlobFeatureDetector::ResponseMap::responseFirstY() const
{
	return mapResponseFirstY;
}

inline const Scalar* BlobFeatureDetector::ResponseMap::filterResponse() const
{
	return mapFilterResponseFrame.constdata<Scalar>();
}

inline const unsigned char* BlobFeatureDetector::ResponseMap::laplacianSign() const
{
	return mapLaplacianSignFrame.constdata<uint8_t>();
}

inline BlobFeatureDetector::ResponseMap::operator bool() const
{
	return mapResponseWidth >= 3 && mapResponseHeight >= 3;
}

inline unsigned int BlobFeatureDetector::detectFeatures(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker)
{
	return detectFeatures(linedIntegralImage, width, height, 0u, 0u, width, height, samplingDense, threshold, frameIsUndistorted, features, worker);
}

inline void BlobFeatureDetector::findExtremes(const ResponseMap& low, const ResponseMap& middle, const ResponseMap& high, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker)
{
	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&BlobFeatureDetector::findExtremesSubset, &low, &middle, &high, threshold, frameIsUndistorted, &features, &lock, 0u, 0u), 0u, high.responseHeight() - 2u, 7u, 8u);
	}
	else
	{
		findExtremesSubset(&low, &middle, &high, threshold, frameIsUndistorted, &features, nullptr, 0u, high.responseHeight() - 2u);
	}
}

inline void BlobFeatureDetector::findExtremes(const ResponseMap& map, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker)
{
	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::createStatic(&BlobFeatureDetector::findExtremesSubset, &map, threshold, frameIsUndistorted, &features, &lock, 0u, 0u), 0u, map.responseHeight(), 5u, 6u, 20u);
	}
	else
	{
		findExtremesSubset(&map, threshold, frameIsUndistorted, &features, nullptr, 0u, map.responseHeight());
	}
}

}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_FEATURE_DETECTOR_BLOB_H
