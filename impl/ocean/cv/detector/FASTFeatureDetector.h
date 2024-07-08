/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_FAST_FEATURE_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_FAST_FEATURE_DETECTOR_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/FASTFeature.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/NonMaximumSuppression.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements a FAST feature detector.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT FASTFeatureDetector
{
	public:

		/**
		 * Definition of a boolean enum for frame un-/distortion properties (to improve code readability).
		 */
		enum FrameDistortion : bool
		{
			/// The provided frame is distorted so that all resulting feature locations are distorted.
			FD_FRAME_IS_DISTORTED = false,
			/// The provided frame is undistorted so that all resulting feature locations are also undistorted.
			FD_FRAME_IS_UNDISTORTED = true,
		};

		/**
		 * Definition of a boolean enum for scoring methods (to improve code readability).
		 */
		enum ScoringMethod : bool
		{
			/// The feature's strength will be based on an approximated scoring.
			SM_APPROXIMATED = false,
			/// The feature's strength will be based on a precise scoring.
			SM_PRECISE = true
		};

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_DETECTOR_EXPORT Comfort
		{
			public:

				/**
				 * Detects FAST features inside a given frame and can distribute the computation to several CPU cores.
				 * This function detects (standard) FAST features.
				 * @param frame The frame to detect FAST features in, will be converted internally if the pixel format and pixel origin matches the internal requirements
				 * @param threshold FAST intensity threshold
				 * @param frameIsUndistorted True, if the original input frame is undistorted and thus the 2D feature position will be undistorted too
				 * @param preciseScoring State defining whether an additional precise scoring is applied for each feature point
				 * @param features Resulting FAST features
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static inline bool detectFeatures(const Frame& frame, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, Worker* worker = nullptr);

				/**
				 * Detects FAST features inside a narrow area of a given frame and can distribute the computation to several CPU cores.
				 * This function detects (standard) FAST features.
				 * @param frame The frame to detect FAST features in, will be converted internally if the pixel format and pixel origin matches the internal requirements
				 * @param subRegionLeft Horizontal start position of the sub-region, in pixel, with range [0, width - 1]
				 * @param subRegionTop Vertical start position of the sub-region, in pixel, with range [0, height - 1]
				 * @param subRegionWidth Width of the sub-region, in pixel, with range [1, infinity)
				 * @param subRegionHeight Height of the sub-region, in pixel, with range [1, infinity)
				 * @param threshold The minimal intensity threshold for all detected features, with range [1, 255]
				 * @param frameIsUndistorted True, if the frame is undistorted and thus the 2D feature position will be undistorted too
				 * @param preciseScoring True to apply an additional precise scoring for each detected feature point; False, to use the approximated feature strength
				 * @param features The resulting FAST features
				 * @param worker Optional worker object to distribute the computation
				 * @return True, if succeeded
				 */
				static bool detectFeatures(const Frame& frame, const unsigned int subRegionLeft, const unsigned int subRegionTop, const unsigned int subRegionWidth, const unsigned int subRegionHeight, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, Worker* worker = nullptr);
		};

	private:

		/**
		 * Definition of a maximum suppression object holding integer strength parameters.
		 */
		typedef NonMaximumSuppression<uint32_t> NonMaximumSuppressionVote;

	public:

		/**
		 * Detects FAST features inside a given frame.
		 * @param yFrame The 8-bit grayscale frame in which the FAST feature has been detected, must be valid
		 * @param width The width of the frame in pixel, with range [9, infinity)
		 * @param height The height of the frame in pixel, with range [9, infinity)
		 * @param threshold The minimal intensity threshold for all detected features, with range [1, 255]
		 * @param frameIsUndistorted True, if the frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param preciseScoring True to apply an additional precise scoring for each detected feature point; False, to use the approximated feature strength
		 * @param features The resulting FAST features
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void detectFeatures(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Detects FAST features inside a sub-region of a given frame.
		 * @param yFrame The 8-bit grayscale frame in which the FAST feature has been detected, must be valid
		 * @param width The width of the frame in pixel, with range [9, infinity)
		 * @param height The height of the frame in pixel, with range [9, infinity)
		 * @param subRegionLeft Horizontal start position of the sub-region, in pixel, with range [0, width - 1]
		 * @param subRegionTop Vertical start position of the sub-region, in pixel, with range [0, height - 1]
		 * @param subRegionWidth Width of the sub-region, in pixel, with range [1, infinity)
		 * @param subRegionHeight Height of the sub-region, in pixel, with range [1, infinity)
		 * @param threshold The minimal intensity threshold for all detected features, with range [1, 255]
		 * @param frameIsUndistorted True, if the frame is undistorted and thus the 2D feature position will be undistorted too
		 * @param preciseScoring True to apply an additional precise scoring for each detected feature point; False, to use the approximated feature strength
		 * @param features The resulting FAST features
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static void detectFeatures(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int subRegionLeft, const unsigned int subRegionTop, const unsigned int subRegionWidth, const unsigned int subRegionHeight, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, const unsigned int framePaddingElements, Worker* worker = nullptr);

	private:

		/**
		 * Detects candidates of FAST features inside a sub region of a given frame.
		 * @param yFrame The 8-bit grayscale frame in which the FAST feature has been detected, must be valid
		 * @param width The width of the frame in pixel, with range [7, infinity)
		 * @param height The height of the frame in pixel, with range [7, infinity)
		 * @param threshold Detection threshold defining the lower boundary of the binary search
		 * @param nonMaximumSuppression Non maximum suppression buffer holding all votes stronger than the specified threshold
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param firstRow The first row to be handled, with range [0, height - 1]
		 * @param numberRows The number of rows to be handled, with range [1, height - firstRow]
		 */
		static void detectFeatureCandidatesSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, NonMaximumSuppressionVote* nonMaximumSuppression, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies a more precise feature scoring using a binary threshold search.
		 * This function may use a worker object to distribute the computational load if preferred.<br>
		 * @param yFrame The 8-bit grayscale frame in which the FAST feature has been detected, must be valid
		 * @param width The width of the frame in pixel, with range [7, infinity)
		 * @param height The height of the frame in pixel, with range [7, infinity)
		 * @param threshold Detection threshold defining the lower boundary of the binary search
		 * @param features Already detected features receiving the more precise score
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to be used for computation distribution
		 */
		static void scoreFeaturesPrecise(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, FASTFeatures& features, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Applies a more precise feature scoring using a binary threshold search to a subset of the given features.
		 * @param yFrame The 8-bit grayscale frame in which the FAST feature has been detected, must be valid
		 * @param width The width of the frame in pixel, with range [7, infinity)
		 * @param height The height of the frame in pixel, with range [7, infinity)
		 * @param threshold Detection threshold defining the lower boundary of the binary search
		 * @param features Already detected features receiving the more precise score
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param firstFeature First feature to be handled, with range [0, features->size() - 1]
		 * @param numberFeatures Number of features to be handled, with range [1, features->size() - firstFeature]
		 */
		static void scoreFeaturesPreciseSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, FASTFeatures* features, const unsigned int framePaddingElements, const unsigned int firstFeature, const unsigned int numberFeatures);

		/**
		 * Applies a more precise feature scoring using a binary threshold search for one feature.
		 * @param yFrame The grayscale frame in which the FAST feature has been detected, must be valid
		 * @param width The width of the frame in pixel, with range [7, infinity)
		 * @param height The height of the frame in pixel, with range [7, infinity)
		 * @param threshold Detection threshold defining the lower boundary of the binary search
		 * @param feature The already detected feature for which the more precise score will be determined (and updated)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 */
		static void scoreFeaturePrecise(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, FASTFeature& feature, const unsigned int framePaddingElements);
};

inline bool FASTFeatureDetector::Comfort::detectFeatures(const Frame& frame, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, Worker* worker)
{
	constexpr unsigned int subRegionLeft = 0u;
	constexpr unsigned int subRegionTop = 0u;

	return detectFeatures(frame, subRegionLeft, subRegionTop, frame.width(), frame.height(), threshold, frameIsUndistorted, preciseScoring, features, worker);
}

inline void FASTFeatureDetector::detectFeatures(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, const unsigned int framePaddingElements, Worker* worker)
{
	constexpr unsigned int subRegionLeft = 0u;
	constexpr unsigned int subRegionTop = 0u;

	detectFeatures(yFrame, width, height, subRegionLeft, subRegionTop, width, height, threshold, frameIsUndistorted, preciseScoring, features, framePaddingElements, worker);
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_FAST_FEATURE_DETECTOR_H
