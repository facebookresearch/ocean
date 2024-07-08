/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_FEATURE_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_FEATURE_DETECTOR_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class provides an abstraction for visual features and strong feature points.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT FeatureDetector
{
	protected:

		/**
		 * This class implements a vector extension holding an additional intensity parameter.
		 */
		class IntensityVector2 : public Vector2
		{
			public:

				/**
				 * Creates a new intensity vector object with undefined vector elements.
				 * Beware: The elemtns are neither zero nor a specific value!
				 * @see VectorT2::VectorT2()
				 */
				inline IntensityVector2();

				/**
				 * Creates a new intensity vector by the given postion and intensity value.
				 * @param position Vector position
				 * @param intensity Vector intensity
				 */
				inline IntensityVector2(const Vector2& position, const int intensity);

				/**
				 * Returns the intensity value of this object.
				 * @return Intensity value
				 */
				inline int intensity() const;

				/**
				 * Compares two intensity objects.
				 * @param object Second object to compare
				 * @return True, if the intensity of the left object is higher than the intensity of the right object
				 */
				inline bool operator<(const IntensityVector2& object) const;

			private:

				/// Intensity value.
				int vectorIntensity;
		};

		/**
		 * Definition of a vector holding intensity vector objects.
		 */
		typedef std::vector<IntensityVector2> IntensityVectors2;

	public:

		/**
		 * Determines the points in an 8bit grayscale image with highest Harris corner response votes.
		 * @param yFrame The 8bit grayscale frame in which the Harris corner respondes have to be determined, must be valid
		 * @param width The width of the frame in pixel, with range [5, infinity)
		 * @param height The height of the frame in pixel, with range [5, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each row of yFrame, in elements, with range [0, infinity)
		 * @param positions Given 2D positions inside the frame that have to be ordered according their response intensities
		 * @param maximalPoints Number of maximal points that have to be returned
		 * @param minSqrDistance Minimal squared distance between the points to be accepted
		 * @param harrisThreshold Minimal threshold for all Harris responses (points below this threshold will be skipped)
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting feature points
		 */
		static Vectors2 filterStrongHarrisPoints(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vectors2& positions, const unsigned int maximalPoints, const Scalar minSqrDistance, const int harrisThreshold = 0, Worker* worker = nullptr);

		/**
		 * Determines strong feature points in a given image, optional a sub-region can be specified in that the points are detected.
		 * @param yFrame The frame in which the feature points are detected, must have pixel format FORMAT_Y, must be valid
		 * @param subRegion Optional sub-region specifying a small image area in that the points are detected, an invalid sub-region to use the entire frame
		 * @param horizontalBins Optional horizontal bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param verticalBins Optional vertical bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param strength Minimal aiming strength parameter of the tracked feature points, this value will be weakened if too less feature points can be detected
		 * @param worker Optional worker object to distribute the computation
		 * @param strengths Optional resulting strength values individual for each point
		 * @return The resulting feature points, feature points with high strength value first
		 */
		static inline Vectors2 determineHarrisPoints(const Frame& yFrame, const SubRegion& subRegion = SubRegion(), const unsigned int horizontalBins = 0u, const unsigned int verticalBins = 0u, const unsigned int strength = 30u, Worker* worker = nullptr, std::vector<int>* strengths = nullptr);

		/**
		 * Determines strong feature points in a given image, optional a sub-region can be specified in that the points are detected.
		 * @param yFrame The 8 bit grayscale frame (Y8 pixel format) in which the feature points are detected, must be valid
		 * @param width The width of the given frame in pixel, with range [7, infinity)
		 * @param height The height of the given frame in pixel, with range [7, infinity)
		 * @param yFramePaddingElements The number of padding elements at the end of each row of yFrame, in elements, with range [0, infinity)
		 * @param subRegion Optional sub-region specifying a small image area in that the points are detected
		 * @param horizontalBins Optional horizontal bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param verticalBins Optional vertical bins that can be used to distribute the tracked points into array bins (in each bin there will be at most one point)
		 * @param strength Minimal aiming strength parameter of the tracked feature points, this value will be weakened if too less feature points can be detected
		 * @param worker Optional worker object to distribute the computation
		 * @param strengths Optional resulting strength values individual for each point
		 * @return The resulting feature points, feature points with high strength value first
		 */
		static Vectors2 determineHarrisPoints(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const SubRegion& subRegion, const unsigned int horizontalBins = 0u, const unsigned int verticalBins = 0u, const unsigned int strength = 30u, Worker* worker = nullptr, std::vector<int>* strengths = nullptr);
};

inline FeatureDetector::IntensityVector2::IntensityVector2() :
	Vector2(),
	vectorIntensity(NumericT<int>::minValue())
{
	// nothing to do here
}

inline FeatureDetector::IntensityVector2::IntensityVector2(const Vector2& position, const int intensity) :
	Vector2(position),
	vectorIntensity(intensity)
{
	// nothing to do here
}

inline int FeatureDetector::IntensityVector2::intensity() const
{
	return vectorIntensity;
}

inline bool FeatureDetector::IntensityVector2::operator<(const IntensityVector2& object) const
{
	return vectorIntensity > object.vectorIntensity;
}

inline Vectors2 FeatureDetector::determineHarrisPoints(const Frame& yFrame, const SubRegion& subRegion, const unsigned int horizontalBins, const unsigned int verticalBins, const unsigned int strength, Worker* worker, std::vector<int>* strengths)
{
	if (!yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format!");
		return Vectors2();
	}

	return determineHarrisPoints(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), subRegion, horizontalBins, verticalBins, strength, worker, strengths);
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_FEATURE_DETECTOR_H
