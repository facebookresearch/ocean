/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYEDETECTORSTEREO_H
#define OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYEDETECTORSTEREO_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"
#include "ocean/cv/detector/bullseyes/Bullseye.h"
#include "ocean/cv/detector/bullseyes/BullseyeDetectorMono.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"
#include "ocean/base/Utilities.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Matrix.h"
#include "ocean/math/Vector3.h"

#include <array>

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
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT BullseyeDetectorStereo
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
		class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT Parameters : public BullseyeDetectorMono::Parameters
		{
			public:

				/**
				 * Creates a new parameter object with default parameters.
				 */
				Parameters() = default;

				/**
				 * Returns the maximum frame width before downscaling is applied.
				 * Input frames wider than this value will be downscaled for efficiency.
				 * @return The maximum frame width in pixels, with range [1, infinity)
				 */
				unsigned int maxFrameWidth() const noexcept;

				/**
				 * Sets the maximum frame width before downscaling is applied.
				 * Input frames wider than this value will be downscaled for efficiency.
				 * @param maxFrameWidth The maximum frame width in pixels, with range [1, infinity)
				 */
				void setMaxFrameWidth(unsigned int maxFrameWidth) noexcept;

				/**
				 * Returns the default parameters for the stereo detector.
				 * @return The default parameters
				 */
				static Parameters defaultParameters();

			protected:

				/// The maximum frame width before downscaling is applied, with range [1, infinity)
				unsigned int maxFrameWidth_ = 720u;
		};

	protected:

		/**
		 * This class represents a candidate bullseye match between two stereo cameras.
		 * A candidate includes the triangulated 3D center position and the reprojection errors for
		 * both cameras. The indices of the matched bullseyes are stored separately as keys in the
		 * CandidateMap. Candidates are used during the matching process before final validation and
		 * selection.
		 */
		class Candidate
		{
			public:

				/**
				 * Creates a default (invalid) candidate.
				 */
				Candidate() = default;

				/**
				 * Creates a new candidate with the specified parameters.
				 * The indices of the matched bullseyes are not stored in the candidate itself,
				 * but rather as keys in the CandidateMap that holds this candidate.
				 * @param center The triangulated 3D center position in world coordinates
				 * @param reprojectionErrorA The reprojection error for camera A, in pixels
				 * @param reprojectionErrorB The reprojection error for camera B, in pixels
				 */
				Candidate(const Vector3& center, const Scalar reprojectionErrorA, const Scalar reprojectionErrorB);

				/**
				 * Returns whether this candidate is valid.
				 * @return True if the candidate has valid indices and center position, otherwise false
				 */
				bool isValid() const;

				/**
				 * Returns the triangulated 3D center position.
				 * @return The 3D center position in world coordinates
				 */
				const Vector3& center() const;

				/**
				 * Returns the reprojection error for camera A.
				 * @return The reprojection error in pixels
				 */
				Scalar reprojectionErrorA() const;

				/**
				 * Returns the reprojection error for camera B.
				 * @return The reprojection error in pixels
				 */
				Scalar reprojectionErrorB() const;

				/**
				 * Returns an invalid bullseye center value used as a sentinel for uninitialized positions.
				 * @return The invalid center position
				 */
				static Vector3 invalidBullseyeCenter();

			protected:

				/// The triangulated 3D center position in world coordinates
				Vector3 center_ = invalidBullseyeCenter();

				/// The reprojection error for camera A, in pixels
				Scalar reprojectionErrorA_ = Numeric::minValue();

				/// The reprojection error for camera B, in pixels
				Scalar reprojectionErrorB_ = Numeric::minValue();
		};

		/**
		 * Definition of an unordered map holding candidate bullseyes.
		 * The key is a pair of indices (indexA, indexB) where indexA corresponds to a bullseye in camera A
		 * and indexB corresponds to a bullseye in camera B. The value is the Candidate object containing
		 * the triangulated 3D position and reprojection errors for this match.
		 */
		using CandidateMap = std::unordered_map<IndexPair32, Candidate, PairHash>;

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
		 * Extracts and validates candidate bullseye matches between two stereo cameras.
		 * This function triangulates all possible bullseye pairs and returns those that produce valid 3D positions.
		 * The returned map uses index pairs (indexA, indexB) as keys, where indexA corresponds to a bullseye in camera A
		 * and indexB corresponds to a bullseye in camera B. The values are Candidate objects containing the triangulated
		 * 3D positions and reprojection errors for each matched pair.
		 * @param cameraA The camera profile for the first camera, must be valid
		 * @param cameraB The camera profile for the second camera, must be valid
		 * @param world_T_cameraA The transformation from camera A coordinate system to world coordinate system, must be valid
		 * @param world_T_cameraB The transformation from camera B coordinate system to world coordinate system, must be valid
		 * @param bullseyesA The bullseyes detected in camera A, may be empty
		 * @param bullseyesB The bullseyes detected in camera B, may be empty
		 * @return The map of valid candidate matches; may be empty if no valid matches were found
		 */
		static CandidateMap extractBullseyeCandidates(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Bullseyes& bullseyesA, const Bullseyes& bullseyesB);

		/**
		 * Extracts the final bullseye pairs and their 3D centers from candidate matches.
		 * This function uses the candidate map to retrieve the original bullseye objects and their triangulated positions.
		 * @param cameraA The camera profile for the first camera, must be valid
		 * @param cameraB The camera profile for the second camera, must be valid
		 * @param bullseyesA The bullseyes detected in camera A, must not be empty
		 * @param bullseyesB The bullseyes detected in camera B, must not be empty
		 * @param candidateMap The map of candidate matches with index pairs as keys and triangulated positions as values, must not be empty
		 * @param bullseyePairs The resulting validated bullseye pairs
		 * @param bullseyeCenters The resulting 3D positions of the bullseye centers in world coordinates
		 * @return True if extraction succeeded, otherwise false
		 */
		static bool extractBullseyes(const AnyCamera& cameraA, const AnyCamera& cameraB, const Bullseyes& bullseyesA, const Bullseyes& bullseyesB, const CandidateMap& candidateMap, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters);

		/**
		 * Computes a cost matrix for matching bullseyes based on candidate triangulation results.
		 * The cost matrix dimensions are (numBullseyesA x numBullseyesB), where each element (i, j)
		 * contains the cost of matching bullseye i from camera A with bullseye j from camera B.
		 * Costs are based on the triangulation quality (reprojection errors) and geometric consistency
		 * (comparing bullseye radii across cameras using the camera scale factor).
		 * @param cameraA The camera profile for the first camera, must be valid
		 * @param cameraB The camera profile for the second camera, must be valid
		 * @param bullseyesA The bullseyes detected in camera A, must not be empty
		 * @param bullseyesB The bullseyes detected in camera B, must not be empty
		 * @param candidateMap The map of candidate matches to evaluate, must not be empty
		 * @param costMatrix The resulting cost matrix with dimensions (numBullseyesA x numBullseyesB)
		 * @return True if the cost matrix was successfully computed, otherwise false
		 */
		static bool computeCostMatrix(const AnyCamera& cameraA, const AnyCamera& cameraB, const Bullseyes& bullseyesA, const Bullseyes& bullseyesB, const CandidateMap& candidateMap, Matrix& costMatrix);

		/**
		 * Returns an invalid (arbitrarily large) matching cost value used to indicate that two bullseyes cannot be matched.
		 * This value is large enough to prevent matching but not so large as to cause numerical issues.
		 * @return The invalid matching cost value
		 */
		constexpr static Scalar invalidMatchingCost();

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
		 * Downscales frames and cameras if they exceed the maximum frame width.
		 * For frames that don't exceed the threshold, the original frame and camera are used (no copy is made).
		 * @param cameras The input camera profiles, must contain exactly 2 valid cameras
		 * @param yFrames The input frames, must contain exactly 2 valid 8-bit grayscale frames
		 * @param maxFrameWidth The maximum allowed frame width before downscaling is applied, with range [1, infinity)
		 * @param downscaledCameras The resulting cameras (downscaled or original), will contain exactly 2 cameras
		 * @param downscaledYFrames The resulting frames (downscaled or referencing original), will contain exactly 2 frames
		 * @param scaleFactors The scale factors applied to each frame (1.0 if no downscaling), will contain exactly 2 values
		 * @param worker Optional worker to distribute the computation
		 * @return True if successful, false on error
		 */
		static bool downscaleFramesAndCameras(const SharedAnyCameras& cameras, const Frames& yFrames, unsigned int maxFrameWidth, SharedAnyCameras& downscaledCameras, Frames& downscaledYFrames, std::array<Scalar, 2>& scaleFactors, Worker* worker);
};

constexpr Scalar BullseyeDetectorStereo::invalidMatchingCost()
{
	// Arbitrarily large value that doesn't cause numerical issues.
	return Scalar(1000);
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYEDETECTORSTEREO_H
