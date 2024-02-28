// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_DETECTOR_ORB_FEATURE_ORIENTATION_H
#define META_OCEAN_CV_DETECTOR_ORB_FEATURE_ORIENTATION_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/ORBFeature.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class implements all necessary functions to calculate the orientation of an ORB features.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT ORBFeatureOrientation
{
	public:

		/**
		 * Determines the orientation for all given feature points based on the intensity centroid of a patch around the feature positions.
		 * A 31 x 31 pixel patch is used to calculate the orientation.
		 * The determined orientations are set to the given feature points.
		 * @param linedIntegralFrame Pointer to the lined integral frame of the frame where the feature points were detected; must be an integral frame for an 8 bit frame
		 * @param width The width of the original frame in pixel (not the width of the lined-integral frame), with range [1, infinity)
		 * @param height The height of the original frame in pixel (not the height of the lined-integral frame), with range [1, infinity)
		 * @param featurePoints Feature points to determine the orientation for
		 * @param worker Optional worker object to distribute the computation to several CPU cores
		 */
		static void determineFeatureOrientation(const unsigned int* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeatures& featurePoints, Worker* worker = nullptr);

	protected:

		/**
		 * Determines the orientation for all given feature points based on the intensity centroid of a patch around the feature positions.
		 * A 31 x 31 pixel patch is used to calculate the orientation.
		 * The determined orientations are set to the given feature points.
		 * @param linedIntegralFrame Pointer to the lined integral frame of the frame where the feature points were detected; must be an integral frame for an 8 bit frame
		 * @param width The width of the original frame in pixel (not the width of the lined-integral frame), with range [1, infinity)
		 * @param height The height of the original frame in pixel (not the height of the lined-integral frame), with range [1, infinity)
		 * @param featurePoints Feature points to determine the orientation for
		 * @param startIndex First index of the feature point vector to be handled
		 * @param range Number of feature points to be handled
		 */
		static void determineFeatureOrientationSubset(const unsigned int* linedIntegralFrame, const unsigned int width, const unsigned int height, ORBFeatures* featurePoints, const unsigned int startIndex, const unsigned int range);

		/**
		 * Determines the orientation for a given feature point based on the intensity centroid of a patch around the feature position.
		 * A 31 x 31 pixel patch is used to calculate the orientation.
		 * @param linedIntegralFrame Pointer to the lined integral frame of the frame where the feature points were detected; must be an integral frame for an 8 bit frame
		 * @param width The width of the original frame in pixel (not the width of the lined-integral frame), with range [1, infinity)
		 * @param height The height of the original frame in pixel (not the height of the lined-integral frame), with range [1, infinity)
		 * @param observation 2D observation position of the feature
		 * @return Orientation angle in radian with range [0, 2*PI)
		 */
		static Scalar determineFeatureOrientation31(const unsigned int* linedIntegralFrame, const unsigned int width, const unsigned int height, const Vector2& observation);
};

}

}

}

#endif // META_OCEAN_CV_DETECTOR_ORB_FEATURE_ORIENTATION_H
