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
#include "ocean/math/Matrix.h"
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
				 * Creates a new parameter object with default parameters.
				 */
				Parameters() = default;

				/**
				 * Returns the default parameters for the stereo detector.
				 * @return The default parameters
				 */
				static Parameters defaultParameters();

				/**
				 * Returns the maximum allowed distance from a point to the epipolar line.
				 * @return The maximum distance in pixels, with range [0, infinity)
				 */
				Scalar maxDistanceToEpipolarLine() const;

				/**
				 * Sets the maximum allowed distance from a point to the epipolar line.
				 * @param distance The maximum distance in pixels, with range [0, infinity)
				 * @return True if the value was valid and set successfully, otherwise false
				 */
				bool setMaxDistanceToEpipolarLine(const Scalar distance);

			protected:

				/// When matching points, this defines the maximum allowed distance from a point to the epipolar line of the other point, in pixels, with range [0, infinity)
				Scalar maxDistanceToEpipolarLine_ = Scalar(5);
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
		static bool detectBullseyes(const SharedAnyCameras& cameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, const Parameters& parameters = Parameters::defaultParameters(), Worker* worker = nullptr);

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
		 * Returns an invalid (arbitrarily large) matching cost value used to indicate that two bullseyes cannot be matched.
		 * This value is large enough to prevent matching but not so large as to cause numerical issues.
		 * @return The invalid matching cost value
		 */
		constexpr static Scalar invalidMatchingCost();

		/**
		 * Computes the matching cost between two bullseyes from different stereo cameras.
		 * The cost is based on epipolar geometry constraints and radius similarity, with both components
		 * transformed using a sigmoid function to produce smooth, bounded costs in the range [0, 1].
		 * Lower costs indicate better matches.
		 * @param bullseyeA The bullseye from the first camera, must be valid
		 * @param bullseyeB The bullseye from the second camera, must be valid
		 * @param epipolarGeometry The epipolar geometry describing the relationship between the two cameras, must be valid
		 * @param maxSqrDistance The squared maximum allowed distance from a point to its matching epipolar line, in pixels squared, with range (0, infinity)
		 * @param cameraB_s_cameraA The scale factor relating measurements in camera B to camera A (e.g., width_B / width_A), with range (0, infinity)
		 * @return The matching cost value, with range [0, 1], where lower values indicate better matches
		 */
		static Scalar computeBullseyeMatchingCost(const Bullseye& bullseyeA, const Bullseye& bullseyeB, const EpipolarGeometry& epipolarGeometry, const Scalar maxSqrDistance, const Scalar cameraB_s_cameraA);

		/**
		 * Computes a cost matrix containing matching costs between all pairs of bullseyes from two cameras.
		 * Each element (i, j) in the matrix represents the matching cost between bullseye i from camera A and bullseye j from camera B.
		 * The cost matrix can be used with an assignment solver to find optimal bullseye correspondences.
		 * @param bullseyesA The bullseyes detected in the first camera, must not be empty
		 * @param bullseyesB The bullseyes detected in the second camera, must not be empty
		 * @param epipolarGeometry The epipolar geometry describing the relationship between the two cameras, must be valid
		 * @param maxSqrDistanceToEpipolarLine The squared maximum allowed distance from a point to its epipolar line, in pixels squared, with range (0, infinity)
		 * @param cameraB_s_cameraA The scale factor relating measurements in camera B to camera A (e.g., width_B / width_A), with range (0, infinity)
		 * @param costMatrix The resulting cost matrix with dimensions [bullseyesA.size() x bullseyesB.size()]
		 * @return True if the cost matrix was successfully computed
		 */
		static bool computeBullseyeMatchingCostMatrix(const Bullseyes& bullseyesA, const Bullseyes& bullseyesB, const EpipolarGeometry& epipolarGeometry, const Scalar maxSqrDistanceToEpipolarLine, const Scalar cameraB_s_cameraA, Matrix& costMatrix);

		/**
		 * Triangulates a single matched bullseye pair to compute its 3D position in world coordinates.
		 * This function casts rays from both camera centers through the bullseye positions and finds their nearest point
		 * to determine the 3D location. It also computes reprojection errors to assess triangulation quality.
		 * @param cameraA The camera profile for the first camera, must be valid
		 * @param cameraB The camera profile for the second camera, must be valid
		 * @param world_T_cameraA The transformation from camera A coordinate system to world coordinate system, must be valid
		 * @param world_T_cameraB The transformation from camera B coordinate system to world coordinate system, must be valid
		 * @param bullseyeA The bullseye from camera A, must be valid and within camera A's field of view
		 * @param bullseyeB The bullseye from camera B, must be valid and within camera B's field of view
		 * @param bullseyeCenter The resulting 3D position of the bullseye center in world coordinates
		 * @param reprojectionErrorA The resulting reprojection error for camera A, in pixels
		 * @param reprojectionErrorB The resulting reprojection error for camera B, in pixels
		 * @return True if triangulation succeeded and the 3D point is in front of both cameras, otherwise false
		 */
		static bool triangulateBullseye(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Bullseye& bullseyeA, const Bullseye& bullseyeB, Vector3& bullseyeCenter, Scalar& reprojectionErrorA, Scalar& reprojectionErrorB);

		/**
		 * Triangulates matched bullseye pairs to compute their 3D positions.
		 * @param cameras The camera profiles for the stereo pair, must contain exactly 2 valid cameras
		 * @param world_T_device The transformation from the device coordinate system to the world coordinate system, must be valid
		 * @param device_T_cameras The transformations from each camera coordinate system to the device coordinate system, must contain exactly 2 valid transformations
		 * @param candidates The candidate bullseye pairs to triangulate
		 * @param bullseyePairs The resulting validated bullseye pairs
		 * @param bullseyeCenters The resulting 3D positions of the bullseye centers in world coordinates
		 * @param reprojectionErrorsA The resulting reprojection errors for camera A, in pixels, one value per validated bullseye pair
		 * @param reprojectionErrorsB The resulting reprojection errors for camera B, in pixels, one value per validated bullseye pair
		 * @return True, if succeeded
		 */
		static bool triangulateBullseyes(const SharedAnyCameras& cameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const BullseyePairs& candidates, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, Scalars& reprojectionErrorsA, Scalars& reprojectionErrorsB);
};

constexpr Scalar StereoBullseyeDetector::invalidMatchingCost()
{
	// Arbitrarily large value that doesn't cause numerical issues.
	return Scalar(1000);
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_STEREOBULLSEYEDETECTOR_H
