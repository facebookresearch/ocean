/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_STEREOBULLSEYEDETECTOR_H
#define OCEAN_CV_DETECTOR_BULLSEYES_STEREOBULLSEYEDETECTOR_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

#include "ocean/cv/detector/bullseyes/Bullseye.h"
#include "ocean/cv/detector/bullseyes/MonoBullseyeDetector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/FisheyeEpipolarGeometry.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * This class implements a stereo detector for bullseye patterns.
 * @ingroup cvdetectorbullseyes
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT StereoBullseyeDetector
{
	public:

		/**
		 * Definition of a pair of bullseyes.
		 */
		using BullseyePair = std::pair<Bullseye, Bullseye>;

		/**
		 * Definition of a vector holding bullseye pairs.
		 */
		using BullseyePairs = std::vector<BullseyePair>;

		/**
		 * Definition of a pair of vectors of bullseyes, one from each camera.
		 */
		using BullseyeGroup = std::array<Bullseyes, 2>;

		/**
		 * This class holds the most important parameters for the stereo detector.
		 * Currently, this class inherits all parameters from the monocular detector.
		 */
		class Parameters : public MonoBullseyeDetector::Parameters
		{
			public:

				/**
				 * Returns the default parameters for the stereo detector.
				 * @return The default parameters
				 */
				static Parameters defaultParameters();
		};

	protected:

		/// An alias for the fisheye epipolar geometry.
		using EpipolarGeometry = Geometry::FisheyeEpipolarGeometry;

	public:

		/**
		 * Detects bullseyes in a pair of stereo frames.
		 * @param cameras The camera profiles for the stereo pair, must contain exactly 2 valid cameras
		 * @param yFrames The stereo frames in which bullseyes will be detected, must contain exactly 2 valid 8-bit grayscale frames
		 * @param world_T_device The transformation from the device coordinate system to the world coordinate system, must be valid
		 * @param device_T_cameras The transformations from each camera coordinate system to the device coordinate system, must contain exactly 2 valid transformations
		 * @param bullseyePairs The resulting pairs of detected bullseyes, one from each camera
		 * @param bullseyeCenters The resulting 3D positions of the triangulated bullseye centers in world coordinates
		 * @param parameters The parameters for the detector, must be valid
		 * @param worker Optional worker to distribute the computation
		 * @return True, if succeeded
		 */
		static bool detectBullseyes(const SharedAnyCameras& cameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, const MonoBullseyeDetector::Parameters& parameters = MonoBullseyeDetector::Parameters::defaultParameters(), Worker* worker = nullptr);

	protected:

		/**
		 * Matches bullseyes detected in two stereo frames using epipolar geometry.
		 * @param cameras The camera profiles for the stereo pair, must contain exactly 2 valid cameras
		 * @param yFrames The stereo frames in which bullseyes will be detected, must contain exactly 2 valid 8-bit grayscale frames
		 * @param world_T_device The transformation from the device coordinate system to the world coordinate system, must be valid
		 * @param device_T_cameras The transformations from each camera coordinate system to the device coordinate system, must contain exactly 2 valid transformations
		 * @param epipolarGeometry The epipolar geometry describing the relationship between the two stereo cameras, must be valid
		 * @param bullseyeGroup The bullseyes detected in both frames, bullseyeGroup[0] contains bullseyes from the first frame, bullseyeGroup[1] contains bullseyes from the second frame
		 * @param maxDistanceToEpipolarLine The maximum allowed distance from a bullseye to its epipolar line, in pixels, with range (0, infinity)
		 * @param bullseyePairs The resulting matched bullseye pairs
		 * @return True, if succeeded
		 */
		static bool matchBullseyes(const SharedAnyCameras& cameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const EpipolarGeometry& epipolarGeometry, const BullseyeGroup& bullseyeGroup, const Scalar maxDistanceToEpipolarLine, BullseyePairs& bullseyePairs);

		/**
		 * Triangulates matched bullseye pairs to compute their 3D positions.
		 * @param cameras The camera profiles for the stereo pair, must contain exactly 2 valid cameras
		 * @param world_T_device The transformation from the device coordinate system to the world coordinate system, must be valid
		 * @param device_T_cameras The transformations from each camera coordinate system to the device coordinate system, must contain exactly 2 valid transformations
		 * @param epipolarGeometry The epipolar geometry describing the relationship between the two stereo cameras, must be valid
		 * @param candidates The candidate bullseye pairs to triangulate
		 * @param bullseyePairs The resulting validated bullseye pairs
		 * @param bullseyeCenters The resulting 3D positions of the bullseye centers in world coordinates
		 * @param reprojectionErrorsA The resulting reprojection errors for camera A, in pixels, one value per validated bullseye pair
		 * @param reprojectionErrorsB The resulting reprojection errors for camera B, in pixels, one value per validated bullseye pair
		 * @return True, if succeeded
		 */
		static bool triangulateBullseyes(const SharedAnyCameras& cameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const EpipolarGeometry& epipolarGeometry, const BullseyePairs& candidates, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, Scalars& reprojectionErrorsA, Scalars& reprojectionErrorsB);
};

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_STEREOBULLSEYEDETECTOR_H
