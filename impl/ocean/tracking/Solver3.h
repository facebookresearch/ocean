/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SOLVER_3_H
#define META_OCEAN_TRACKING_SOLVER_3_H

#include "ocean/tracking/Tracking.h"
#include "ocean/tracking/Database.h"

#include "ocean/base/ShiftVector.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/SubRegion.h"

#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"
#include "ocean/geometry/NonLinearOptimizationOrientation.h"
#include "ocean/geometry/NonLinearOptimizationPose.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a Structure From Motion solver for unconstrained 3D object points and unconstrained 6-DOF camera poses.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT Solver3
{
	public:

		/**
		 * Definition of individual camera motion types.
		 */
		enum CameraMotion
		{
			/// Invalid camera motion.
			CM_INVALID = 0,
			/// Static camera.
			CM_STATIC = (1 << 0),
			/// Rotational camera motion (panning or tilting).
			CM_ROTATIONAL = (1 << 1),
			/// Translational camera motion.
			CM_TRANSLATIONAL = (1 << 2),
			/// Tiny rotational camera motion.
			CM_ROTATIONAL_TINY = CM_ROTATIONAL | (1 << 3),
			/// Moderate rotational camera motion.
			CM_ROTATIONAL_MODERATE = CM_ROTATIONAL | (1 << 4),
			/// Significant rotational camera motion.
			CM_ROTATIONAL_SIGNIFICANT = CM_ROTATIONAL | (1 << 5),
			/// Tiny translational camera motion.
			CM_TRANSLATIONAL_TINY = CM_TRANSLATIONAL | (1 << 6),
			/// Moderate translational camera motion.
			CM_TRANSLATIONAL_MODERATE = CM_TRANSLATIONAL | (1 << 7),
			/// Significant translational camera motion.
			CM_TRANSLATIONAL_SIGNIFICANT = CM_TRANSLATIONAL | (1 << 8),
			/// An unknown (arbitrary) camera motion with rotational and translational motion.
			CM_UNKNOWN = CM_ROTATIONAL | CM_TRANSLATIONAL | (1 << 9)
		};

		/**
		 * Definition of individual methods to determine the accuracy of object points.
		 */
		enum AccuracyMethod
		{
			/// Invalid method.
			AM_INVALID,
			/// Determination of the minimal absolute cosine values between the mean observation direction and each observation direction.
			AM_MEAN_DIRECTION_MIN_COSINE,
			/// Determination of the mean absolute cosine value between the mean observation direction and each observation direction.
			AM_MEAN_DIRECTION_MEAN_COSINE,
			/// Determination of the median absolute cosine value between the mean observation direction and each observation direction.
			AM_MEAN_DIRECTION_MEDIAN_COSINE
		};

		/**
		 * Definition of a class allowing to define a relative threshold with lower and upper boundary for individual reference values.
		 */
		class RelativeThreshold
		{
			public:

				/**
				 * Creates a new threshold object.
				 * @param lowerBoundary The lower boundary of the resulting threshold value
				 * @param factor The factor which will be applied to an external reference values to defined an individual threshold
				 * @param upperBoundary The upper boundary of the resulting threshold value
				 */
				inline RelativeThreshold(const unsigned int lowerBoundary, const Scalar factor, const unsigned int upperBoundary);

				/**
				 * Returns the lower boundary of this object.
				 * @return Lower boundary
				 */
				inline unsigned int lowerBoundary() const;

				/**
				 * Returns the factor of this object.
				 * @return Relative factor
				 */
				inline Scalar factor() const;

				/**
				 * Returns the upper boundary of this object.
				 * @return Upper boundary
				 */
				inline unsigned int upperBoundary() const;

				/**
				 * Returns the relative threshold for a given reference value.
				 * @param value The reference value for which the individual threshold will be determined
				 * @return The relative threshold: min[minmax(lowerBoundary(), value * factor(), upperBoundary()), value]
				 */
				inline unsigned int threshold(const unsigned int value) const;

				/**
				 * Returns whether for a given reference value a valid relative threshold can be determined.
				 * @param value The reference value which will be checked
				 * @param threshold Optional resulting relative threshold for the given reference value: min[minmax(lowerBoundary(), value * factor(), upperBoundary()), value]
				 * @return True, if value >= lowerBoundary()
				 */
				inline bool hasValidThreshold(const unsigned int value, unsigned int* threshold = nullptr) const;

				/**
				 * Returns whether for a given reference value a valid relative threshold can be determined.
				 * @param value The reference value which will be checked
				 * @param threshold Optional resulting relative threshold for the given reference value: min[minmax(max[lowerBoundary(), tLowerBoundary], value * factor(), upperBoundary()), value]
				 * @return True, if value >= max(lowerBoundary(), tLowerBoundary)
				 * @tparam tLowerBoundary An explicit lower boundary which is checked in parallel to the internal lower boundary
				 */
				template <unsigned int tLowerBoundary>
				inline bool hasValidThreshold(const unsigned int value, unsigned int* threshold = nullptr) const;

			protected:

				/// The lower boundary of the relative threshold.
				unsigned int thresholdLowerBoundary;

				/// The factor defining the relative threshold.
				Scalar thresholdFactor;

				/// The upper boundary of the relative threshold.
				unsigned int thresholdUpperBoundary;
		};

		/**
		 * This class implements an accessor for groups of pairs of object point ids and image points.
		 * Each group represents one camera pose, while the pairs within the groups represent the visual information in the individual camera poses (camera frames).<br>
		 * The groups of pairs have the following structure, each group (camera pose) can have an arbitrary number of pairs:
		 * <pre>
		 * poseIndex_0 -> (objectPointId_0, imagePoint_0)
		 *             -> (objectPointId_1, imagePoint_4)
		 *             -> (objectPointId_5, imagePoint_9)
		 *
		 * poseIndex_1 -> (objectPointId_0, imagePoint_2)
		 *
		 * poseIndex_2 -> (objectPointId_2, imagePoint_3)
		 *                (objectPointId_1, imagePoint_8)
		 *
		 * poseIndex_3 -> (objectPointId_9, imagePoint_5)
		 * </pre>
		 */
		class PoseToObjectPointIdImagePointCorrespondenceAccessor : public Geometry::NonLinearOptimization::PoseGroupsAccessor
		{
			public:

				/**
				 * Copy constructor.
				 * @param accessor The accessor object to copy
				 */
				inline PoseToObjectPointIdImagePointCorrespondenceAccessor(const PoseToObjectPointIdImagePointCorrespondenceAccessor& accessor);

				/**
				 * Move constructor.
				 * @param accessor The accessor object to move
				 */
				inline PoseToObjectPointIdImagePointCorrespondenceAccessor(PoseToObjectPointIdImagePointCorrespondenceAccessor&& accessor) noexcept;

				/**
				 * Creates a new accessor object and extracts the necessary information from a given database.
				 * Beware: Due to the given threshold of minimal visible object points the number of resulting groups can be smaller than the number of given pose ids.<br>
				 * @param database The database providing all information of this accessor
				 * @param poseIds The ids of the camera poses which will be used to extract the individual groups, at most one group for each provided camera pose (can be less due to the defined threshold of minimal visible object points)
				 * @param objectPointIds The ids of the object points for which the individual correspondences will be determined, if an object point is not visible in a given camera pose the associated correspondence (between object point id and image point) will not be part of the associated group
				 * @param minimalVisibleObjectPoints The minimal number of object points (from the set of given object point ids) which must be visible in a camera pose so that a group will be created for that pose)
				 * @param validPoseIndices Optional resulting indices of valid pose ids (the indices of the specified pose ids for which a corresponding groups has been created)
				 * @param usedObjectPointIndices Optional resulting indices of the used object points (the indices of the specified object point ids which are used at least in one group)
				 */
				PoseToObjectPointIdImagePointCorrespondenceAccessor(const Database& database, const Indices32& poseIds, const Indices32& objectPointIds, const unsigned int minimalVisibleObjectPoints = 10u, Indices32* validPoseIndices = nullptr, Indices32* usedObjectPointIndices = nullptr);

				/**
				 * Assign operator.
				 * @param accessor The accessor object to assign
				 * @return Reference to this object
				 */
				inline PoseToObjectPointIdImagePointCorrespondenceAccessor& operator=(const PoseToObjectPointIdImagePointCorrespondenceAccessor& accessor);

				/**
				 * Move operator.
				 * @param accessor The accessor object to move
				 * @return Reference to this object
				 */
				inline PoseToObjectPointIdImagePointCorrespondenceAccessor& operator=(PoseToObjectPointIdImagePointCorrespondenceAccessor&& accessor) noexcept;
		};

		/**
		 * This class implements an accessor for groups of pairs of pose indices (not pose ids) and image points.
		 * Each group represents one object point, while the pairs within the groups represent the visual information in the individual camera poses (camera frames).<br>
		 * The groups of pairs have the following structure, each group (object point) can have an arbitrary number of pairs:
		 * <pre>
		 * objectPointIndex_0 -> (poseIndex_0, imagePoint_0)
		 *                    -> (poseIndex_1, imagePoint_4)
		 *                    -> (poseIndex_5, imagePoint_9)
		 *
		 * objectPointIndex_1 -> (poseIndex_0, imagePoint_2)
		 *
		 * objectPointIndex_2 -> (poseIndex_2, imagePoint_3)
		 *                       (poseIndex_1, imagePoint_8)
		 *
		 * objectPointIndex_3 -> (poseIndex_9, imagePoint_5)
		 * </pre>
		 */
		class ObjectPointToPoseImagePointCorrespondenceAccessor : public Geometry::NonLinearOptimization::ObjectPointGroupsAccessor
		{
			public:

				/**
				 * Copy constructor.
				 * @param accessor The accessor object to copy
				 */
				inline ObjectPointToPoseImagePointCorrespondenceAccessor(const ObjectPointToPoseImagePointCorrespondenceAccessor& accessor);

				/**
				 * Move constructor.
				 * @param accessor The accessor object to move
				 */
				inline ObjectPointToPoseImagePointCorrespondenceAccessor(ObjectPointToPoseImagePointCorrespondenceAccessor&& accessor) noexcept;

				/**
				 * Creates a new accessor object by moving a subset of a given accessor object.
				 * @param accessor The accessor object from which the subset will be moved
				 * @param validGroupIndices The indices of the valid groups which will be copied
				 */
				inline ObjectPointToPoseImagePointCorrespondenceAccessor(ObjectPointToPoseImagePointCorrespondenceAccessor&& accessor, const Indices32& validGroupIndices);

				/**
				 * Creates a new accessor object and extracts the necessary information from a given database.
				 * This constructor focuses on the provided pose ids and tries to use as much object points as possible as long as the provided thresholds do not violate any condition.<br>
				 * Beware: Due to the given threshold of minimal visible observations per key frame the number of resulting groups can be smaller than the number of given object points ids.<br>
				 * The pose indices of the individual (group) pairs provide the index in relation to the given set of pose ids, not the pose id itself.<br>
				 * @param database The database providing all information of this accessor
				 * @param poseIds The ids of the camera poses for which the individual correspondences will be determined, if an object is not visible in a given camera pose the associated correspondence (between camera pose index and image point) will not be part of the associated group
				 * @param objectPointCandidateIds The ids of the object point candidates which will be used to extract the individual groups, at most one group for each provided candidate (can be less due to the defined threshold of minimal observations per keyframes)
				 * @param minimalObservationsInKeyframes The minimal number of observations (from the set of given pose ids) which must exist for one object point so that a group will be created for that object point, with range [1, infinity)
				 * @param validObjectPoints Optional resulting indices of valid object point ids (the indices of the specified object point ids for which a corresponding groups has been created)
				 */
				ObjectPointToPoseImagePointCorrespondenceAccessor(const Database& database, const Indices32& poseIds, const Indices32& objectPointCandidateIds, const unsigned int minimalObservationsInKeyframes = 2u, Indices32* validObjectPoints = nullptr);

				/**
				 * Creates a new accessor object and extracts the necessary information from a given database.
				 * This constructor focuses on the provided object points and guarantees to create one group for each object point.<br>
				 * However, each group may have less pairs as specified by the thresholds if an object point does not provided enough observations.<br>
				 * The pose indices of the individual (group) pairs provide the index in relation to the given set of pose ids, not the pose id itself.
				 * @param database The database providing all information of this accessor
				 * @param lowerPoseId The id of the lowest pose which can be used as possible key frame
				 * @param upperPoseId The id of the last pose which can be used as possible key frame, with range [lowerPoseId, infinity)
				 * @param objectPointIds The ids of the object points for which groups will be created
				 * @param minimalObservationsInKeyframes The minimal number of observations which should be found for each object points, if an object point does not have so many observations less observations will be used, with range [2, infinity)
				 * @param minimalKeyFrames The minimal number of key frames which will be used
				 * @param usedKeyFrameIds Optional resulting ids of all used key frames
				 */
				ObjectPointToPoseImagePointCorrespondenceAccessor(const Database& database, const Index32 lowerPoseId, const Index32 upperPoseId, const Indices32& objectPointIds, const unsigned int minimalObservationsInKeyframes = 2u, const unsigned int minimalKeyFrames = 2u, Indices32* usedKeyFrameIds = nullptr);

				/**
				 * Assign operator.
				 * @param accessor The accessor to assign
				 * @return The reference to this object
				 */
				inline ObjectPointToPoseImagePointCorrespondenceAccessor& operator=(const ObjectPointToPoseImagePointCorrespondenceAccessor& accessor);

				/**
				 * Move operator.
				 * @param accessor The accessor to move
				 * @return The reference to this object
				 */
				inline ObjectPointToPoseImagePointCorrespondenceAccessor& operator=(ObjectPointToPoseImagePointCorrespondenceAccessor&& accessor) noexcept;

		};

	protected:

		/**
		 * Definition of a map mapping 32 bit indices to 32 bit indices.
		 */
		typedef std::map<unsigned int, unsigned int> IndexMap32;

		/**
		 * Definition of a shift vector holding groups of image points.
		 */
		typedef ShiftVector<Vectors2> ImagePointGroups;

		/**
		 * This class implements an accessor providing access to observation pairs (the observation of a projected object points in camera poses/frames) for a set of object points.
		 * This accessor is designed for specific object points which are all visible in a some camera poses/frames.<br>
		 * The accessor provides the following structure:
		 * <pre>
		 * objectpoint_0 -> (observation_0, imagepoint_a)
		 *               -> (observation_1, imagepoint_b)
		 *               -> (observation_2, imagepoint_c)
		 *
		 * objectpoint_1 -> (observation_0, imagepoint_d)
		 *               -> (observation_1, imagepoint_e)
		 *               -> (observation_2, imagepoint_f)
		 * </pre>
		 * Thus, we have n object points which are all visible in m camera poses.
		 */
		class ObjectPointToPoseIndexImagePointCorrespondenceAccessor : public Geometry::NonLinearOptimization::ObjectPointGroupsAccessor
		{
			public:

				/**
				 * Copy constructor.
				 * @param accessor The accessor to copy
				 */
				inline ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const ObjectPointToPoseIndexImagePointCorrespondenceAccessor& accessor);

				/**
				 * Move constructor.
				 * @param accessor The accessor to move
				 */
				inline ObjectPointToPoseIndexImagePointCorrespondenceAccessor(ObjectPointToPoseIndexImagePointCorrespondenceAccessor&& accessor) noexcept;

				/**
				 * Creates a new accessor object.
				 * @param imagePointGroups The image point groups from which the individual image points are extracted
				 * @param posesSubsetBlock Indices defining a subset of the image point groups while the indices are defined as if the image groups start with the frame index 0
				 */
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const std::vector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock);

				/**
				 * Creates a new accessor object.
				 * @param imagePointGroups The image point groups from which the individual image points are extracted
				 * @param posesSubsetBlock Indices defining a subset of the image point groups while the indices are defined as if the image groups start with the frame index 0
				 */
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const ShiftVector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock);

				/**
				 * Creates a new accessor object.
				 * @param imagePointGroups The image point groups from which the individual image points are extracted
				 * @param posesSubsetBlock Indices defining a subset of the image point groups while the indices are defined as if the image groups start with the frame index 0
				 * @param objectPointsSubset Indices defining a subset of the object points (within the image point groups)
				 */
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const std::vector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock, const Indices32& objectPointsSubset);

				/**
				 * Creates a new accessor object.
				 * @param imagePointGroups The image point groups from which the individual image points are extracted
				 * @param posesSubsetBlock Indices defining a subset of the image point groups while the indices are defined as if the image groups start with the frame index 0
				 * @param objectPointsSubset Indices defining a subset of the object points (within the image point groups)
				 */
				ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const ShiftVector<Vectors2>& imagePointGroups, const Indices32& posesSubsetBlock, const Indices32& objectPointsSubset);

				/**
				 * Assign operator.
				 * @param accessor The accessor to assign
				 * @return Reference to this object
				 */
				inline ObjectPointToPoseIndexImagePointCorrespondenceAccessor& operator=(const ObjectPointToPoseIndexImagePointCorrespondenceAccessor& accessor);

				/**
				 * Move operator.
				 * @param accessor The accessor to move
				 * @return Reference to this object
				 */
				inline ObjectPointToPoseIndexImagePointCorrespondenceAccessor& operator=(ObjectPointToPoseIndexImagePointCorrespondenceAccessor&& accessor) noexcept;
		};

		/**
		 * Definition of a pair combining a pose id and an error parameter.
		 */
		typedef std::pair<Index32, Scalar> PoseErrorPair;

		/**
		 * Definition of a vector holding pose error pairs.
		 */
		typedef std::vector<PoseErrorPair> PoseErrorPairs;

	public:

		/**
		 * Determines the initial positions of 3D object points in a database if no camera poses or structure information is known.
		 * Feature points are tracked from frame to frame within a defined camera frame range as long as the number of tracked points fall under a defined threshold.<br>
		 * Key frames are selected from this range of (tracked) camera frames with representative geometry information.<br>
		 * This function can be configured so that (perfectly) static image points located in all frames at the same position are identified not used for calculations.<br>
		 * Static image points can be located (visible) at static logos (bands) in the frames so that these image points must not be used.
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points
		 * @param pinholeCamera The pinhole camera profile which will be applied
		 * @param randomGenerator a random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrame The index of the frame from which the algorithm will start, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range (lowerFrame, infinity)
		 * @param maximalStaticImagePointFilterRatio The maximal ratio between (perfectly) static image points and the overall number of image points so that these static image points will be filtered and not used, with ratio [0, 1), 0 to avoid any filtering
		 * @param initialObjectPoints The resulting initial 3D positions of object points that could be extracted
		 * @param initialObjectPointIds The resulting ids of the resulting object points, one id for each resulting object point
		 * @param pointsThreshold The threshold of image points which must be visible in each camera frame
		 * @param minimalKeyFrames The minimal number of keyframes that will be extracted
		 * @param maximalKeyFrames The maximal number of keyframes that will be extracted
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @param usedPoseIds Optional resulting ids of all camera poses which have been used to determine the initial object points
		 * @param finalSqrError Optional resulting final average error
		 * @param finalImagePointDistance Optional resulting final average distance between the individual image points and the center of these image points
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineInitialObjectPointsFromSparseKeyFrames(const Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const Scalar maximalStaticImagePointFilterRatio, Vectors3& initialObjectPoints, Indices32& initialObjectPointIds, const RelativeThreshold& pointsThreshold = RelativeThreshold(20u, Scalar(0.5), 100u), const unsigned int minimalKeyFrames = 3u, const unsigned int maximalKeyFrames = 10u, const Scalar maximalSqrError = Scalar(3.5 * 3.5), Indices32* usedPoseIds = nullptr, Scalar* finalSqrError = nullptr, Scalar* finalImagePointDistance = nullptr, bool* abort = nullptr);

		/**
		 * Determines the initial positions of 3D object points in a database if no camera poses or structure information is known.
		 * Feature points are tracked from frame to frame within a defined camera frame range as long as the number of tracked points fall under a defined threshold.<br>
		 * The entire range of frames with tracked points are use to determine the locations of the 3D object points.<br>
		 * This function can be configured so that (perfectly) static image points located in all frames at the same position are identified not used for calculations.<br>
		 * Static image points can be located (visible) at static logos (bands) in the frames so that these image points must not be used.
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points
		 * @param pinholeCamera The pinhole camera profile which will be applied
		 * @param randomGenerator a random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrame The index of the frame from which the algorithm will start, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range (lowerFrame, infinity)
		 * @param regionOfInterest Optional region of interest defining a specific area in the start frame so that the object points lying in the region are handled with higher priority, an invalid region to avoid any special region of interest handling
		 * @param maximalStaticImagePointFilterRatio The maximal ratio between (perfectly) static image points and the overall number of image points so that these static image points will be filtered and not used, with ratio [0, 1), 0 to avoid any filtering
		 * @param initialObjectPoints The resulting initial 3D positions of object points that could be extracted
		 * @param initialObjectPointIds The resulting ids of the resulting object points, one id for each resulting object point
		 * @param pointsThreshold The threshold of image points which must be visible in each camera frame
		 * @param minimalTrackedFramesRatio The minimal number of frames that finally have been tracked (the entire range of frames in which the object points are visible) defined as ratio of the entire frame range, with range (0, 1], does not have any meaning if no start frame or region of interest is defined
		 * @param minimalKeyFrames The minimal number of keyframes that will be extracted
		 * @param maximalKeyFrames The maximal number of keyframes that will be extracted
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @param usedPoseIds Optional resulting ids of all camera poses which have been used to determine the initial object points
		 * @param finalSqrError Optional resulting final average error
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineInitialObjectPointsFromDenseFrames(const Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const CV::SubRegion& regionOfInterest, const Scalar maximalStaticImagePointFilterRatio, Vectors3& initialObjectPoints, Indices32& initialObjectPointIds, const RelativeThreshold& pointsThreshold = RelativeThreshold(20u, Scalar(0.5), 100u), const Scalar minimalTrackedFramesRatio = Scalar(0.1), const unsigned int minimalKeyFrames = 3u, const unsigned int maximalKeyFrames = 10u, const Scalar maximalSqrError = Scalar(3.5 * 3.5), Indices32* usedPoseIds = nullptr, Scalar* finalSqrError = nullptr, bool* abort = nullptr);

		/**
		 * Determines the initial positions of 3D object points in a database if no camera poses or structure information is known.
		 * Feature points are tracked from frame to frame within a defined camera frame range as long as the number of tracked points fall under a defined threshold.<br>
		 * Key frames are selected from this range of (tracked) camera frames with representative geometry information.<br>
		 * This function internally applies several individual iterations beginning from individual start frames so that the best result within the entire frame range is returned.<br>
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points
		 * @param steps The number of steps that are applied within the defined frame range, with range [1, upperFrame - lowerFrame + 1]
		 * @param pinholeCamera The pinhole camera profile which will be applied
		 * @param randomGenerator A random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range (lowerFrame, infinity)
		 * @param maximalStaticImagePointFilterRatio The maximal ratio between (perfectly) static image points and the overall number of image points so that these static image points will be filtered and not used, with ratio [0, 1), 0 to avoid any filtering
		 * @param initialObjectPoints The resulting initial 3D positions of object points that could be extracted
		 * @param initialObjectPointIds The resulting ids of the resulting object points, one id for each resulting object point
		 * @param pointsThreshold The threshold of image points which must be visible in each camera frame
		 * @param minimalKeyFrames The minimal number of keyframes that will be extracted
		 * @param maximalKeyFrames The maximal number of keyframes that will be extracted
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @param usedPoseIds Optional resulting ids of all camera poses which have been used to determine the initial object points
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineInitialObjectPointsFromSparseKeyFramesBySteps(const Database& database, const unsigned int steps, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const Scalar maximalStaticImagePointFilterRatio, Vectors3& initialObjectPoints, Indices32& initialObjectPointIds, const RelativeThreshold& pointsThreshold = RelativeThreshold(20u, Scalar(0.5), 100u), const unsigned int minimalKeyFrames = 2u, const unsigned int maximalKeyFrames = 10u, const Scalar maximalSqrError = Scalar(3.5 * 3.5), Indices32* usedPoseIds = nullptr, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the initial object point positions for a set of key frames (image point groups) observing the unique object points in individual camera poses.
		 * This function applies a RANSAC mechanism randomly selecting individual start key frames (pairs of image points).<br>
		 * The key frames (image point groups) provide the following topology:<br>
		 * For n unique object points visible in m individual frames we have n object points (op) and n * m overall image points (ip):
		 * <pre>
		 *                  op_1,   op_2,   op_3,   op_4,   ..., op_n
		 * sparse_pose_0 -> ip_1_1, ip_1_2, ip_1_3, ip_1_4, ..., ip_1_n
		 * sparse_pose_1 -> ip_2_1, ip_2_2, ip_2_3, ip_2_4, ..., ip_2_n
		 * sparse_pose_2 -> ip_3_1, ip_3_2, ip_3_3, ip_3_4, ..., ip_3_n
		 * sparse_pose_3 -> ip_4_1, ip_4_2, ip_4_3, ip_4_4, ..., ip_4_n
		 * ...
		 * sparse pose_m -> ip_m_1, ip_m_2, ip_m_3, ip_m_4, ..., ip_y_n
		 * </pre>
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param imagePointGroups Key frames of image points, all points in one group are located in the same camera frame and the individual points correspond to the same unique object points
		 * @param randomGenerator A random generator object
		 * @param validPoses The resulting poses that could be determined
		 * @param validPoseIndices The indices of resulting valid poses in relation to the given image point groups
		 * @param objectPoints The resulting object points that could be determined
		 * @param validObjectPointIndices The indices of resulting valid object points in relation to the given image point groups
		 * @param iterations The number of RANSAC iterations trying to find a better result than already determined
		 * @param minimalValidObjectPoints The threshold of object points that must be valid
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @param database Optional database holding the image points from the imagePointGroups to validate the resulting 3D object positions even for camera poses not corresponding to the provided groups of image points; if defined also 'keyFrameIds' and 'objectPointIds' must be defined
		 * @param keyFrameIds Optional ids of the individual keyframes to which the set of image point groups from 'imagePointGroups' belong, each key frame id corresponds with one group of image points, if defined also 'database' and 'objectPointIds' must be defined
		 * @param objectPointIds Optional ids of the individual object points which projections are provided as groups of image points in 'imagePointGroups', if defined also 'database' and 'keyFrameIds' must be defined
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 * @see determineInitialObjectPointsFromDenseFramesRANSAC().
		 */
		static bool determineInitialObjectPointsFromSparseKeyFramesRANSAC(const PinholeCamera& pinholeCamera, const Database::ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, HomogenousMatrices4& validPoses, Indices32& validPoseIndices, Vectors3& objectPoints, Indices32& validObjectPointIndices, const unsigned int iterations = 20u, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(10u, Scalar(0.3), 20u), const Scalar maximalSqrError = Scalar(3.5 * 3.5), const Database* database = nullptr, const Indices32* keyFrameIds = nullptr, const Indices32* objectPointIds = nullptr, bool* abort = nullptr);

		/**
		 * Determines the initial object point positions for a set of frames (image point groups) observing the unique object points in individual camera poses.
		 * This function applies a RANSAC mechanism randomly selecting individual start key frames (pairs of image points).<br>
		 * The key frames (image point groups) provide the following topology:<br>
		 * For n unique object points visible in m individual frames we have n object points (op) and n * m overall image points (ip):
		 * <pre>
		 *                 op_1,   op_2,   op_3,   op_4,   ..., op_n
		 * ...
		 * dense_pose_2 -> ip_3_1, ip_3_2, ip_3_3, ip_3_4, ..., ip_3_n
		 * dense_pose_3 -> ip_4_1, ip_4_2, ip_4_3, ip_4_4, ..., ip_4_n
		 * ...
		 * </pre>
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param imagePointGroups Frames of image points, all points in one group are located in the same camera frame and the individual points correspond to the same unique object points
		 * @param randomGenerator A random generator object
		 * @param validPoses The resulting poses that could be determined
		 * @param validPoseIds The ids of resulting valid poses, one id for each resulting valid pose (the order of the ids is arbitrary)
		 * @param objectPoints The resulting object points that could be determined
		 * @param validObjectPointIndices The indices of resulting valid object points in relation to the given image point groups
		 * @param iterations The number of RANSAC iterations trying to find a better result than already determined
		 * @param minimalValidObjectPoints The threshold of object points that must be valid
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 * @see determineInitialObjectPointsFromSparseKeyFramesRANSAC().
		 */
		static bool determineInitialObjectPointsFromDenseFramesRANSAC(const PinholeCamera& pinholeCamera, const ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, HomogenousMatrices4& validPoses, Indices32& validPoseIds, Vectors3& objectPoints, Indices32& validObjectPointIndices, const unsigned int iterations = 20u, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(10u, Scalar(0.3), 20u), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the initial object point positions for a set of key frames (image point groups) observing unique object points.
		 * This function starts with two explicit key frames (image point groups) and then tries to find more matching key frames (image point groups).<br>
		 * The set of given image points should not contain image points located (visible) at a static logo in the frame as these points may violate the pose determination algorithms.<br>
		 * The key frames (image point groups) provide the following topology:<br>
		 * For n unique object points visible in m individual key frames we have n object points (op) and n * m overall image points (ip):
		 * <pre>
		 *                  op_1,   op_2,   op_3,   op_4,   ..., op_n
		 * sparse_pose_0 -> ip_1_1, ip_1_2, ip_1_3, ip_1_4, ..., ip_1_n
		 * sparse_pose_1 -> ip_2_1, ip_2_2, ip_2_3, ip_2_4, ..., ip_2_n
		 * sparse_pose_2 -> ip_3_1, ip_3_2, ip_3_3, ip_3_4, ..., ip_3_n
		 * sparse_pose_3 -> ip_4_1, ip_4_2, ip_4_3, ip_4_4, ..., ip_4_n
		 * ...
		 * sparse pose_m -> ip_m_1, ip_m_2, ip_m_3, ip_m_4, ..., ip_y_n
		 * </pre>
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param imagePointGroups Key frames (groups) of image points, all points in one key frame (group) are located in the same camera key frame and the individual points correspond to the same unique object points
		 * @param randomGenerator A random generator object
		 * @param firstGroupIndex The index of the first key frame (image point group) which is applied as the first stereo frame, with range [0, imagePointGroups.size())
		 * @param secondGroupIndex The index of the second key frame (image point group) which is applied as the second stereo frame, with range [0, imagePointGroups.size()), with firstGroupIndex != secondGroupIndex
		 * @param poses The resulting poses that could be determined
		 * @param validPoseIndices The indices of resulting valid poses in relation to the given image point groups
		 * @param objectPoints The resulting object points that could be determined
		 * @param validObjectPointIndices The indices of resulting valid object points in relation to the given image point groups
		 * @param minimalValidObjectPoints The minimal number of valid object points which must be reached
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @return True, if succeeded
		 * @see determineInitialObjectPointsFromFrameRange().
		 */
		static bool determineInitialObjectPointsFromSparseKeyFrames(const PinholeCamera& pinholeCamera, const Database::ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, const unsigned int firstGroupIndex, const unsigned int secondGroupIndex, HomogenousMatrices4& poses, Indices32& validPoseIndices, Vectors3& objectPoints, Indices32& validObjectPointIndices, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(10u, Scalar(0.3), 20u), const Scalar maximalSqrError = Scalar(3.5 * 3.5));

		/**
		 * Determines the initial object point positions for a set of image point groups (covering a range of image frames) observing the unique object points in individual frames.
		 * This function starts with two explicit frames (image point groups) and then tries to find more matching frames (image point groups).<br>
		 * The set of given image points should not contain image points located (visible) at a static logo in the frame as these points may violate the pose determination algorithms.<br>
		 * All frames (image point groups) within the frame range provide the following topology:<br>
		 * For n unique object points visible in m individual frames we have n object points (op) and n * m overall image points (ip):
		 * <pre>
		 *                 op_1,   op_2,   op_3,   op_4,   ..., op_n
		 * ...
		 * dense_pose_2 -> ip_3_1, ip_3_2, ip_3_3, ip_3_4, ..., ip_3_n
		 * dense_pose_3 -> ip_4_1, ip_4_2, ip_4_3, ip_4_4, ..., ip_4_n
		 * ...
		 * </pre>
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param imagePointGroups Frames (groups) of image points, all points in one frame (group) are located in the same camera frame and the individual points correspond to the same unique object points
		 * @param randomGenerator A random generator object
		 * @param firstGroupIndex The index of the first frame (image point group) which is applied as the first stereo frame, with range [imagePointGroups.firstIndex(), imagePointGroups.lastIndex()]
		 * @param secondGroupIndex The index of the second frame (image point group) which is applied as the second stereo frame, with range [imagePointGroups.firstIndex(), imagePointGroups.lastIndex()], with firstGroupIndex != secondGroupIndex
		 * @param validPoses The resulting poses that could be determined
		 * @param validPoseIds The ids of resulting valid poses, one id for each valid resulting pose (the order of the ids is arbitrary)
		 * @param totalSqrError The resulting sum of square pixel errors for all valid poses
		 * @param objectPoints The resulting object points that could be determined
		 * @param validObjectPointIndices The indices of resulting valid object points in relation to the given image point groups, with range [5, infinity)
		 * @param minimalValidObjectPoints The minimal number of valid object points which must be reached
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @return True, if succeeded
		 * @see determineInitialObjectPointsFromKeyFrames().
		 */
		static bool determineInitialObjectPointsFromDenseFrames(const PinholeCamera& pinholeCamera, const ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, const unsigned int firstGroupIndex, const unsigned int secondGroupIndex, HomogenousMatrices4& validPoses, Indices32& validPoseIds, Scalar& totalSqrError, Vectors3& objectPoints, Indices32& validObjectPointIndices, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(10u, Scalar(0.3), 20u), const Scalar maximalSqrError = Scalar(3.5 * 3.5));

		/**
		 * Optimizes the positions of already known initial 3D object points when a given database holds neither valid 3D positions or valid 6DOF poses.
		 * The optimization is done by a bundle adjustment between the camera poses of distinct keyframes and the given 3D object points, however the optimized camera poses are not provided.<br>
		 * This function can optimize a subset of the given initial object points to allow more camera poses (camera frames) to be involved.<br>
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrame The index of the frame from which the algorithm will start, in this frame the specified initial object points must all be visible, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param initialObjectPoints The already known initial 3D positions of object points
		 * @param initialObjectPointIds The ids of the already known object points, one id for each given initial object point
		 * @param optimizedObjectPoints The resulting optimized 3D positions of the given initial object points
		 * @param optimizedObjectPointIds The resulting ids of the optimized object points, one id for each optimized object point
		 * @param minimalObjectPoints The minimal number of object points that will be optimized (the higher the number the less camera poses may be used as some object points may not be visible in all camera frames), with range [5, initialObjectPoints.size()); however, tue to pose inaccuracies the algorithm finally may use less object points
		 * @param minimalKeyFrames The minimal number of keyframes that will be used, with range [2, maximalKeyFrames]
		 * @param maximalKeyFrames The maximal number of keyframes that will be used, with range [minimalKeyFrames, upperFrame - lowerFrame + 1]; however, due to pose inaccuracies the algorithm finally may use more keyframes
		 * @param maximalSqrError The maximal squared projection error for a 3D object point, points with larger error are excluded after a first optimization iteration
		 * @param usedPoseIds Optional resulting ids of all camera poses which have been used to optimized the object points
		 * @param initialSqrError Optional resulting initial average squared error
		 * @param finalSqrError Optional resulting final average squared error
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool optimizeInitialObjectPoints(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const Vectors3& initialObjectPoints, const Indices32& initialObjectPointIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, const unsigned int minimalObjectPoints = 5u, const unsigned int minimalKeyFrames = 3u, const unsigned int maximalKeyFrames = 10u, const Scalar maximalSqrError = Scalar(3.5 * 3.5), Indices32* usedPoseIds = nullptr, Scalar* initialSqrError = nullptr, Scalar* finalSqrError = nullptr, bool* abort = nullptr);

		/**
		 * Determines the positions of new object points from a database within a specified frame range.
		 * Only camera frames with valid camera poses are used to determine the new object points.<br>
		 * This function extracts a subset of representative camera poses and triangulates image points from individual camera poses to determine new 3D object points.<br>
		 * Object points in the database with valid 3D positions are not investigated.<br>
		 * @param database The database defining the topology of 3D object points, corresponding 2D image points and corresponding camera poses
		 * @param camera The camera profile defining the projection, must be valid
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param newObjectPoints The resulting positions of new object points
		 * @param newObjectPointIds The resulting ids of the new object points, each id corresponds with a positions from 'newObjectPoints'
		 * @param minimalKeyFrames The minimal number of key frames which must be valid for a 3D object point, with range [minimalKeyFrames, upperFrame - lowerFrame + 1]
		 * @param maximalKeyFrames The maximal number of key frames which will be use to determine the 3D object point positions, with range [minimalKeyFrames, upperFrame - lowerFrame + 1]
		 * @param maximalSqrError The maximal squared error between a projected 3D object point and an image point so that the combination of object point and image point count as valid
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const unsigned int lowerFrame, const unsigned int upperFrame, Vectors3& newObjectPoints, Indices32& newObjectPointIds, const unsigned int minimalKeyFrames = 3u, const unsigned int maximalKeyFrames = 10u, const Scalar maximalSqrError = Scalar(3.5 * 3.5), Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the positions of a set of (currently unknown) object points.
		 * Only camera frames with valid camera pose are used to determined the new object points.<br>
		 * @param database The database form which the object point, image point and pose information is extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraMotion The motion of the camera, can be CM_ROTATIONAL or CM_TRANSLATIONAL
		 * @param unknownObjectPointIds The ids of all (currently unknown) object points for which a 3D position will be determined, must all be valid
		 * @param newObjectPoints The resulting 3D location of the new object points
		 * @param newObjectPointIds The ids of the resulting new object points, one id for each resulting new object point
		 * @param randomGenerator Random generator object to be used for creating random numbers, must be defined
		 * @param newObjectPointObservations Optional resulting number of observations for each determined 3D object point, one number for each resulting 3D object point location
		 * @param minimalObservations The minimal number of observations for each new object points which are necessary to determine the 3D location
		 * @param useAllObservations True, to use all observations (with valid camera pose) to determine the 3D locations; False, to apply a RANSAC mechanism taking a subset of all observations to determine the 3D locations
		 * @param estimator The robust estimator which is applied during optimization of each individual new 3D location, must be defined
		 * @param ransacMaximalSqrError The maximal squared projection error between a new 3D object point and the corresponding image points for the RANSAC mechanism
		 * @param averageRobustError The (average) robust error for a new 3D object point after optimization of the 3D location
		 * @param maximalSqrError The maximal error for a new valid 3D object point after optimization of the 3D location
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded and not aborted
		 */
		static bool determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, const Indices32& unknownObjectPointIds, Vectors3& newObjectPoints, Indices32& newObjectPointIds, RandomGenerator& randomGenerator, Indices32* newObjectPointObservations = nullptr, const unsigned int minimalObservations = 2u, const bool useAllObservations = true, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar averageRobustError = Scalar(3.5 * 3.5), const Scalar maximalSqrError = Numeric::maxValue(), Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the positions of all (currently unknown) object points.
		 * Only camera frames with valid camera pose are used to determined the locations of the new object points.<br>
		 * All unknown object points with more or equal observations (in valid poses) than specified will be handled.<br>
		 * However, the number of resulting object points with valid 3D position may be smaller than the maximal possible number due to e.g., the defined maximal error parameters.<br>
		 * @param database The database form which the object point, image point and pose information is extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraMotion The motion of the camera, can be CM_ROTATIONAL or CM_TRANSLATIONAL
		 * @param newObjectPoints The resulting 3D location of the new object points
		 * @param newObjectPointIds The ids of the resulting new object points, one id for each resulting new object point
		 * @param randomGenerator Random generator object to be used for creating random numbers, must be defined
		 * @param newObjectPointObservations Optional resulting number of observations (with valid camera poses) for each determined 3D object point, one number for each resulting 3D object point location
		 * @param minimalObjectPointPriority The minimal priority value of the resulting unknown object points
		 * @param minimalObservations The minimal number of observations (with valid camera poses) for each new object points which are necessary to determine the 3D location, with range [2, infinity)
		 * @param useAllObservations True, to use all observations (with valid camera pose) to determine the 3D locations; False, to apply a RANSAC mechanism taking a subset of all observations to determine the 3D locations
		 * @param estimator The robust estimator which is applied during optimization of each individual new 3D location, must be defined
		 * @param ransacMaximalSqrError The maximal squared projection error between a new 3D object point and the corresponding image points for the RANSAC mechanism
		 * @param averageRobustError The (average) robust error for a new 3D object point after optimization of the 3D location
		 * @param maximalSqrError The maximal error for a new valid 3D object point after optimization of the 3D location
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded and not aborted
		 */
		static inline bool determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, Vectors3& newObjectPoints, Indices32& newObjectPointIds, RandomGenerator& randomGenerator, Indices32* newObjectPointObservations = nullptr, const Scalar minimalObjectPointPriority = Scalar(-1), const unsigned int minimalObservations = 10u, const bool useAllObservations = true, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar averageRobustError = Scalar(3.5 * 3.5), const Scalar maximalSqrError = Numeric::maxValue(), Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the positions of (currently unknown) object points which are visible in specified poses (the poses are specified by a lower and upper frame range).
		 * Only camera frames with valid camera pose are used to determined the locations of the new object points.<br>
		 * All unknown object points with more or equal observations (in valid poses) than specified will be handled.<br>
		 * However, the number of resulting object points with valid 3D position may be small than the maximal possible number due to e.g., the defined maximal error parameters.<br>
		 * @param database The database form which the object point, image point and pose information is extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraMotion The motion of the camera, can be CM_ROTATIONAL or CM_TRANSLATIONAL
		 * @param lowerPoseId The lower id of the camera pose in which the unknown object points can/must be visible
		 * @param upperPoseId The upper id of the camera pose in which the unknown object points can/must be visible, with range [lowerPoseId, infinity)
		 * @param newObjectPoints The resulting 3D location of the new object points
		 * @param newObjectPointIds The ids of the resulting new object points, one id for each resulting new object point
		 * @param randomGenerator Random generator object to be used for creating random numbers, must be defined
		 * @param newObjectPointObservations Optional resulting number of observations (with valid camera poses) for each determined 3D object point, one number for each resulting 3D object point location
		 * @param minimalObjectPointPriority The minimal priority value of the resulting unknown object points
		 * @param minimalObservations The minimal number of observations (with valid camera poses) for each new object points which are necessary to determine the 3D location
		 * @param useAllObservations True, to use all observations (with valid camera pose) to determine the 3D locations; False, to apply a RANSAC mechanism taking a subset of all observations to determine the 3D locations
		 * @param estimator The robust estimator which is applied during optimization of each individual new 3D location, must be defined
		 * @param ransacMaximalSqrError The maximal squared projection error between a new 3D object point and the corresponding image points for the RANSAC mechanism
		 * @param averageRobustError The (average) robust error for a new 3D object point after optimization of the 3D location
		 * @param maximalSqrError The maximal error for a new valid 3D object point after optimization of the 3D location
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded and not aborted
		 * @tparam tVisibleInAllPoses True, if the object points must be visible in all poses (frames) of the specified pose range; False, if the object point can be visible in any poses (frames) within the specified pose range
		 */
		template <bool tVisibleInAllPoses>
		static inline bool determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, const Index32 lowerPoseId, const Index32 upperPoseId, Vectors3& newObjectPoints, Indices32& newObjectPointIds, RandomGenerator& randomGenerator, Indices32* newObjectPointObservations = nullptr, const Scalar minimalObjectPointPriority = Scalar(-1), const unsigned int minimalObservations = 10u, const bool useAllObservations = true, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar averageRobustError = Scalar(3.5 * 3.5), const Scalar maximalSqrError = Numeric::maxValue(), Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Optimizes a set of 3D object points (having a quite good accuracy already) without optimizing the camera poses concurrently.
		 * The database must hold the valid initial 3D object positions, the image point positions and must hold valid camera poses.<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param cameraMotion The motion of the camera, CM_ROTATIONAL if the camera poses do not have a translational part, CM_TRANSLATIONAL otherwise
		 * @param objectPointIds The ids of the object points for which the positions will be optimized (all points must have already initial 3D positions)
		 * @param optimizedObjectPoints The resulting positions of the optimized object points
		 * @param optimizedObjectPointIds The ids of the optimized object points, one id for each positions in 'optimizedObjectPoints'
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param maximalRobustError The maximal error between a projected object point and the individual image points; beware the error must be defined w.r.t. the selected estimator
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 * @see optimizeObjectPointsWithVariablePoses().
		 */
		static bool optimizeObjectPointsWithFixedPoses(const Database& database, const PinholeCamera& pinholeCamera, const CameraMotion cameraMotion, const Indices32& objectPointIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, unsigned int minimalObservations = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar maximalRobustError = Scalar(3.5 * 3.5), Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Optimizes 3D object points (having a quite good accuracy already) and optimizes the camera poses concurrently.
		 * The optimization is based on a bundle adjustment for camera poses and object points minimizing the projection error between projected object points and image points located in the camera frames.<br>
		 * Representative key frames with valid camera poses are selected and all object points visible in these key frames will be optimized as long as the object points can be observed in more key frames than the defined threshold 'minimalObservations'.<br>
		 * However, the number of observations for each individual object point and the ids of the key frames in which the object points are visible can be arbitrary (as long as the defined thresholds hold).<br>
		 * The database must hold the valid initial 3D object positions, the image point positions and must hold valid camera poses.<br>
		 * Beware: Neither any pose nor any object point in the database will be updated, use the resulting optimized object point locations to update the database!<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param optimizedObjectPoints The resulting positions of the optimized object points
		 * @param optimizedObjectPointIds The ids of the optimized object points, one id for each positions in 'optimizedObjectPoints'
		 * @param optimizedKeyFramePoses Optional resulting camera poses, one for each keyframe which has been used during optimization, nullptr if not of interest
		 * @param optimizedKeyFramePoseIds Optional resulting ids of the camera poses which have been used as key frame during optimization, one for each 'optimizedKeyFramePoses', nullptr if not of interest
		 * @param minimalKeyFrames The minimal number of key frames (with valid poses) which are necessary for the optimization, with range [2, maximalkeyFrames]
		 * @param maximalKeyFrames The maximal number of key frames (with valid poses) which will be used for the optimization, with range [minimalKeyFrames, infinity)
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized, with range [minimalKeyFrames, infinity)
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param iterations The number of optimization iterations which will be applied, with range [1, infinity)
		 * @param initialRobustError Optional the initial average robust error before optimization
		 * @param finalRobustError Optional the final average robust error after optimization
		 * @return True, if succeeded
		 * @see optimizeObjectPointsWithFixedPoses().
		 */
		static bool optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses = nullptr, Indices32* optimizedKeyFramePoseIds = nullptr, const unsigned int minimalKeyFrames = 3u, const unsigned int maximalKeyFrames = 20u, const unsigned int minimalObservations = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const unsigned int iterations = 50u, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr);

		/**
		 * Optimizes 3D object points (having a quite good accuracy already) and optimizes the camera poses concurrently.
		 * The optimization is based on a bundle adjustment for camera poses and object points minimizing the projection error between projected object points and image points located in the camera frames.<br>
		 * Representative key frames with valid camera poses must be provided and all object points visible in these key frames will be optimized as long as the object points can be observed in more key frames than the defined threshold 'minimalObservations'.<br>
		 * However, the number of observations for each individual object point and the ids of the key frames in which the object points are visible can be arbitrary (as long as the defined thresholds hold).<br>
		 * The database must hold the valid initial 3D object positions, the image point positions and must hold valid camera poses.<br>
		 * Beware: Neither any pose nor any object point in the database will be updated, use the resulting optimized object point locations to update the database!<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param keyFrameIds The ids of all poses defining representative key frames for the optimization, at least two
		 * @param optimizedObjectPoints The resulting positions of the optimized object points, at least one
		 * @param optimizedObjectPointIds The ids of the optimized object points, one id for each positions in 'optimizedObjectPoints'
		 * @param optimizedKeyFramePoses Optional resulting optimized camera poses, one for each key frame id
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized, with range [minimalKeyFrames, infinity)
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param iterations The number of optimization iterations which will be applied, with range [1, infinity)
		 * @param initialRobustError Optional the initial average robust error before optimization
		 * @param finalRobustError Optional the final average robust error after optimization
		 * @return True, if succeeded
		 * @see optimizeObjectPointsWithFixedPoses().
		 */
		static bool optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& keyFrameIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses = nullptr, const unsigned int minimalObservations = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const unsigned int iterations = 50u, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr);

		/**
		 * Optimizes 3D object points (having a quite good accuracy already) and optimizes the camera poses concurrently.
		 * The optimization is based on a bundle adjustment for camera poses and object points minimizing the projection error between projected object points and image points located in the camera frames.<br>
		 * Representative key frames with valid camera poses must be provided, further a set of object point ids must be provided which should be used for optimization, the object points visible in the key frames will be optimized as long as the object points can be observed in more key frames than the defined threshold 'minimalObservations'.<br>
		 * However, the number of observations for each individual object point and the ids of the key frames in which the object points are visible can be arbitrary (as long as the defined thresholds hold).<br>
		 * The database must hold the valid initial 3D object positions, the image point positions and must hold valid camera poses.<br>
		 * Beware: Neither any pose nor any object point in the database will be updated, use the resulting optimized object point locations to update the database!<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param keyFrameIds The ids of all poses defining representative key frames for the optimization, at least two
		 * @param objectPointIds The ids of the object points which will be optimized (may be a subset only), at least one
		 * @param optimizedObjectPoints The resulting positions of the optimized object points
		 * @param optimizedObjectPointIds The ids of the optimized object points, one id for each positions in 'optimizedObjectPoints'
		 * @param optimizedKeyFramePoses Optional resulting optimized camera poses, one for each key frame id
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized, with range [minimalKeyFrames, infinity)
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param iterations The number of optimization iterations which will be applied, with range [1, infinity)
		 * @param initialRobustError Optional the initial average robust error before optimization
		 * @param finalRobustError Optional the final average robust error after optimization
		 * @return True, if succeeded
		 * @see optimizeObjectPointsWithFixedPoses().
		 */
		static bool optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& keyFrameIds, const Indices32& objectPointIds, Vectors3& optimizedObjectPoints, Indices32& optimizedObjectPointIds, HomogenousMatrices4* optimizedKeyFramePoses = nullptr, const unsigned int minimalObservations = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const unsigned int iterations = 50u, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr);

		static bool optimizeObjectPointsWithVariablePoses(const Database& database, const PinholeCamera& pinholeCamera, const Index32 lowerPoseId, const Index32 upperPoseId, const Indices32& objectPointIds, Indices32& usedKeyFrameIds, Vectors3& optimizedObjectPoints, const unsigned int minimalObservations = 10u, const unsigned int minimalKeyFrames = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const unsigned int iterations = 50u, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr);

		/**
		 * Supposes pure rotational camera motion for a given database with stable camera poses determined by initial but stable object points.
		 * If the camera profile is not well approximated during determination of the camera poses and the initial 3D object points the camera motion may contain translational motion although in reality the motion is only rotational.<br>
		 * Especially, if the camera comes with a significant distortion the motion determination may go wrong.<br>
		 * Therefore, this function supposes sole rotational camera motion, determined the new 3D object points locations, selected a set of suitable keyframes best representing the entire number of valid camera poses, optimizes the camera's field of view and the distortion parameter.<br>
		 * If the projection error between 3D object points and 2D image points falls below a defined threshold (should be strong), than the camera motion can be expected to provide only rotational parts.<br>
		 * Beware: Valid object points (with valid location) not visible within the specified frame range will not be investigated.
		 * @param database The database providing a set initial 3D object points visible in several valid camera poses
		 * @param pinholeCamera The pinhole camera profile which has been used to determine the camera poses and 3D object point locations in the given database
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param findInitialFieldOfView True, to apply a determination of the initial field of view of the camera (should be done if the camera's field of view is not known)
		 * @param optimizationStrategy The optimization strategy for the camera parameters which will be applied, OS_INVALID to avoid any optimization of the camera parameters
		 * @param optimizedCamera The resulting optimized camera profile with adjusted field of view and distortion parameters
		 * @param optimizedDatabase The resulting database with optimized camera poses and 3D object point locations
		 * @param minimalObservations The minimal number of observations an object points must have so that it will be investigated to measure whether the camera motion is pure rotational
		 * @param minimalKeyframes The minimal number of key frames (with valid poses) which are necessary for the determination/optimization, with range [2, minimalKeyFrames)
		 * @param maximalKeyframes The maximal number of key frames (with valid poses) which will be used for the optimization, with range [minimalKeyFrames, upperFrame - lowerFrame + 1]
		 * @param lowerFovX The lower threshold border for the final (ideal) horizontal field of view of the camera profile, with range (0, upperFovX],
		 * @param upperFovX The upper threshold border for the final (ideal) horizontal field of view of the camera profile, with range [lowerFoVX, PI)
		 * @param maximalSqrError The maximal average projection error between the 3D object points and the 2D image points so that a correspondence counts as valid
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param finalMeanSqrError Optional resulting final mean squared pose error (averaged)
		 * @return True, if the camera motion is pure rotational
		 * @see optimizeCamera().
		 */
		static bool supposeRotationalCameraMotion(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrame, const unsigned int upperFrame, const bool findInitialFieldOfView, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, Database& optimizedDatabase, const unsigned int minimalObservations = 0u, const unsigned int minimalKeyframes = 3u, const unsigned int maximalKeyframes = 20u, const Scalar lowerFovX = Numeric::deg2rad(20), const Scalar upperFovX = Numeric::deg2rad(140), const Scalar maximalSqrError = (1.5 * 1.5), Worker* worker = nullptr, bool* abort = nullptr, Scalar* finalMeanSqrError = nullptr);

		/**
		 * Optimizes the camera profile for a given database with stable camera poses determined by initial but stable object points.
		 * This function selected a representative subset of the valid poses within the specified frame range and considers all object points visible in the subset of camera frames.<br>
		 * The resulting optimized database (with optimized object point locations) invalidates all object point locations of object points not visible in the selected subset of camera frames.<br>
		 * Therefore, this function should be invoked after the initial set of stable object points are determined but before the database stores too many object points (which would get lost).<br>
		 * Further, this function supposes a translational (and optional rotational) camera motion.<br>
		 * @param database The database providing a set initial 3D object points visible in several valid camera poses
		 * @param pinholeCamera The pinhole camera profile which has been used to determine the camera poses and 3D object point locations in the given database
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param findInitialFieldOfView True, to apply a determination of the initial field of view of the camera (should be done if the camera's field of view is not known)
		 * @param optimizationStrategy The optimization strategy for the camera parameters which will be applied, OS_INVALID to avoid any optimization of the camera parameters
		 * @param optimizedCamera The resulting optimized camera profile with adjusted field of view and distortion parameters
		 * @param optimizedDatabase The resulting database with optimized camera poses and 3D object point locations
		 * @param minimalObservationsInKeyframes The minimal number of observations an object point must have under all selected keyframes so that it will be investigated to optimized the camera profile and so that this object point will be optimized
		 * @param minimalKeyframes The minimal number of key frames (with valid poses) which are necessary for the determination/optimization, with range [2, minimalKeyFrames)
		 * @param maximalKeyframes The maximal number of key frames (with valid poses) which will be used for the optimization, with range [minimalKeyFrames, upperFrame - lowerFrame + 1]
		 * @param lowerFovX The lower threshold border for the final (ideal) horizontal field of view of the camera profile, with range (0, upperFovX],
		 * @param upperFovX The upper threshold border for the final (ideal) horizontal field of view of the camera profile, with range [lowerFoVX, PI)
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param finalMeanSqrError Optional resulting final mean squared pose error (averaged)
		 * @return True, if the camera profile and the 3D object point locations and the camera poses in the given database could be optimized
		 * @see supposeRotationalCameraMotion().
		 */
		static bool optimizeCamera(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrame, const unsigned int upperFrame, const bool findInitialFieldOfView, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, Database& optimizedDatabase, const unsigned int minimalObservationsInKeyframes = 2u, const unsigned int minimalKeyframes = 3u, const unsigned int maximalKeyframes = 20u, const Scalar lowerFovX = Numeric::deg2rad(20), const Scalar upperFovX = Numeric::deg2rad(140), Worker* worker = nullptr, bool* abort = nullptr, Scalar* finalMeanSqrError = nullptr);

		/**
		 * Optimizes 3D object points (having a quite good accuracy already) and optimizes the camera poses and camera profile concurrently.
		 * The optimization is based on a bundle adjustment for camera poses and object points minimizing the projection error between projected object points and image points located in the camera frames.<br>
		 * Representative key frames with valid camera poses are selected and all object points visible in these key frames will be optimized as long as the object points can be observed in more key frames than the defined threshold 'minimalObservations'.<br>
		 * However, the number of observations for each individual object point and the ids of the key frames in which the object points are visible can be arbitrary (as long as the defined thresholds hold).<br>
		 * The database must hold the valid initial 3D object positions, the image point positions and must hold valid camera poses.<br>
		 * Beware: Neither any pose nor any object point in the database will be updated, use the resulting optimized object point locations to update the database!<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param optimizationStrategy The optimization strategy for the camera parameters which will be applied, OS_INVALID to avoid any optimization of the camera parameters
		 * @param optimizedCamera The resulting optimized camera profile with adjusted field of view and distortion parameters
		 * @param optimizedObjectPoints The resulting positions of the optimized object points
		 * @param optimizedObjectPointIds The ids of the optimized object points, one id for each positions in 'optimizedObjectPoints'
		 * @param optimizedKeyFramePoses Optional resulting camera poses, one for each keyframe which has been used during optimization, nullptr if not of interest
		 * @param optimizedKeyFramePoseIds Optional resulting ids of the camera poses which have been used as key frame during optimization, one for each 'optimizedKeyFramePoses', nullptr if not of interest
		 * @param minimalKeyFrames The minimal number of key frames (with valid poses) which are necessary for the optimization, with range [2, maximalkeyFrames]
		 * @param maximalKeyFrames The maximal number of key frames (with valid poses) which will be used for the optimization, with range [minimalKeyFrames, infinity)
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized, with range [minimalKeyFrames, infinity)
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param iterations The number of optimization iterations which will be applied, with range [1, infinity)
		 * @param initialRobustError Optional the initial average robust error before optimization
		 * @param finalRobustError Optional the final average robust error after optimization
		 * @return True, if succeeded
		 * @see optimizeObjectPointsWithFixedPoses().
		 */
		static bool optimizeCameraWithVariableObjectPointsAndPoses(const Database& database, const PinholeCamera& pinholeCamera, const PinholeCamera::OptimizationStrategy optimizationStrategy, PinholeCamera& optimizedCamera, Vectors3* optimizedObjectPoints = nullptr, Indices32* optimizedObjectPointIds = nullptr, HomogenousMatrices4* optimizedKeyFramePoses = nullptr, Indices32* optimizedKeyFramePoseIds = nullptr, const unsigned int minimalKeyFrames = 3u, const unsigned int maximalKeyFrames = 20u, const unsigned int minimalObservations = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const unsigned int iterations = 50u, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr);

		/**
		 * Optimizes 3D object points (having a quite good accuracy already) and optimizes the camera poses and camera profile concurrently.
		 * The optimization is based on a bundle adjustment for camera poses and object points minimizing the projection error between projected object points and image points located in the camera frames.<br>
		 * Representative key frames with valid camera poses must be provided and all object points visible in these key frames will be optimized as long as the object points can be observed in more key frames than the defined threshold 'minimalObservations'.<br>
		 * However, the number of observations for each individual object point and the ids of the key frames in which the object points are visible can be arbitrary (as long as the defined thresholds hold).<br>
		 * The database must hold the valid initial 3D object positions, the image point positions and must hold valid camera poses.<br>
		 * Beware: Neither any pose nor any object point in the database will be updated, use the resulting optimized object point locations to update the database!<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param optimizationStrategy The optimization strategy for the camera parameters which will be applied, OS_INVALID to avoid any optimization of the camera parameters
		 * @param keyFrameIds The ids of all poses defining representative key frames for the optimization, at least two
		 * @param optimizedCamera The resulting optimized camera profile with adjusted field of view and distortion parameters
		 * @param optimizedObjectPoints The resulting positions of the optimized object points, at least one
		 * @param optimizedObjectPointIds The ids of the optimized object points, one id for each positions in 'optimizedObjectPoints'
		 * @param optimizedKeyFramePoses Optional resulting optimized camera poses, one for each key frame id
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized, with range [minimalKeyFrames, infinity)
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param iterations The number of optimization iterations which will be applied, with range [1, infinity)
		 * @param initialRobustError Optional the initial average robust error before optimization
		 * @param finalRobustError Optional the final average robust error after optimization
		 * @return True, if succeeded
		 * @see optimizeObjectPointsWithFixedPoses().
		 */
		static bool optimizeCameraWithVariableObjectPointsAndPoses(const Database& database, const PinholeCamera& pinholeCamera, const PinholeCamera::OptimizationStrategy optimizationStrategy, const Indices32& keyFrameIds, PinholeCamera& optimizedCamera, Vectors3* optimizedObjectPoints = nullptr, Indices32* optimizedObjectPointIds = nullptr, HomogenousMatrices4* optimizedKeyFramePoses = nullptr, const unsigned int minimalObservations = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const unsigned int iterations = 50u, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr);

		/**
		 * Optimizes 3D object points (having a quite good accuracy already) and optimizes the camera poses and camera profile concurrently.
		 * The optimization is based on a bundle adjustment for camera poses and object points minimizing the projection error between projected object points and image points located in the camera frames.<br>
		 * Representative key frames with valid camera poses must be provided, further a set of object point ids must be provided which should be used for optimization, the object points visible in the key frames will be optimized as long as the object points can be observed in more key frames than the defined threshold 'minimalObservations'.<br>
		 * However, the number of observations for each individual object point and the ids of the key frames in which the object points are visible can be arbitrary (as long as the defined thresholds hold).<br>
		 * The database must hold the valid initial 3D object positions, the image point positions and must hold valid camera poses.<br>
		 * Beware: Neither any pose nor any object point in the database will be updated, use the resulting optimized object point locations to update the database!<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param optimizationStrategy The optimization strategy for the camera parameters which will be applied, OS_INVALID to avoid any optimization of the camera parameters
		 * @param keyFrameIds The ids of all poses defining representative key frames for the optimization, at least two
		 * @param objectPointIds The ids of the object points which will be optimized (may be a subset only), at least one
		 * @param optimizedCamera The resulting optimized camera profile
		 * @param optimizedObjectPoints The resulting positions of the optimized object points
		 * @param optimizedObjectPointIds Optional resulting ids of the optimized object points, one id for each positions in 'optimizedObjectPoints', nullptr if not of interest
		 * @param optimizedKeyFramePoses Optional resulting optimized camera poses, one for each key frame id
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized, with range [minimalKeyFrames, infinity)
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param iterations The number of optimization iterations which will be applied, with range [1, infinity)
		 * @param initialRobustError Optional the initial average robust error before optimization
		 * @param finalRobustError Optional the final average robust error after optimization
		 * @return True, if succeeded
		 * @see optimizeObjectPointsWithFixedPoses().
		 */
		static bool optimizeCameraWithVariableObjectPointsAndPoses(const Database& database, const PinholeCamera& pinholeCamera, const PinholeCamera::OptimizationStrategy optimizationStrategy, const Indices32& keyFrameIds, const Indices32& objectPointIds, PinholeCamera& optimizedCamera, Vectors3* optimizedObjectPoints = nullptr, Indices32* optimizedObjectPointIds = nullptr, HomogenousMatrices4* optimizedKeyFramePoses = nullptr, const unsigned int minimalObservations = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const unsigned int iterations = 50u, Scalar* initialRobustError = nullptr, Scalar* finalRobustError = nullptr);

		/**
		 * Updates the camera poses of the database depending on valid 2D/3D points correspondences within a range of camera frames.
		 * The camera poses will be set to invalid if no valid pose can be determined (e.g., if not enough valid point correspondences are known for a specific camera frame).<br>
		 * Pose determination starts at a specified frame and moves to higher and lower frame indices afterwards.<br>
		 * Poses from successive frames are applied as initial guess for a new frame.<br>
		 * The resulting poses will have either a sole rotational motion or a rotational and translational motion, this depends on the defined camera motion.<br>
		 * @param database The database from which the point correspondences are extracted and which receives the determined camera poses
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraMotion The motion of the camera, use CM_UNKNOWN if the motion is unknown so that 6-DOF poses will be determined
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrame The index of the frame from which the algorithm will start, in this frame the specified initial object points must all be visible, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera pose, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a pose counts as valid, with range (0, infinity)
		 * @param finalAverageError Optional resulting average final error for all valid poses, the error depends on the selected robust estimator
		 * @param validPoses Optional resulting number of valid poses
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if all poses have been updated (the poses may be invalid)
		 */
		static bool updatePoses(Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar maximalRobustError = Scalar(3.5 * 3.5), Scalar* finalAverageError = nullptr, size_t* validPoses = nullptr, bool* abort = nullptr);

		/**
		 * Updates the camera poses of the database depending on valid 2D/3D points correspondences within a range of camera frames.
		 * The camera poses will be set to invalid if no valid pose can be determined (e.g., if not enough valid point correspondences are known for a specific camera frame).<br>
		 * If a worker is provided every pose is determined independently.<br>
		 * The resulting poses will have either a sole rotational motion or a rotational and translational motion, this depends on the defined camera motion.<br>
		 * @param database The database from which the point correspondences are extracted and which receives the determined camera poses
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraMotion The motion of the camera, use CM_UNKNOWN if the motion is unknown so that 6-DOF poses will be determined
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera pose, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a pose counts as valid, with range (0, infinity)
		 * @param finalAverageError Optional resulting average final error for all valid poses, the error depends on the selected robust estimator
		 * @param validPoses Optional resulting number of valid poses
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if all poses have been updated (the poses may be invalid)
		 */
		static bool updatePoses(Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar maximalRobustError = Scalar(3.5 * 3.5), Scalar* finalAverageError = nullptr, size_t* validPoses = nullptr, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * Determines the camera poses depending on valid 2D/3D points correspondence within a range of camera frames.
		 * The camera poses will be set to invalid if no valid pose can be determined (e.g., if not enough valid point correspondences are known for a specific camera frame).<br>
		 * The resulting poses will have either a sole rotational motion or a rotational and translational motion, this depends on the defined camera motion.<br>
		 * @param database The database from which the point correspondences are extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraMotion The motion of the camera, use CM_UNKNOWN if the motion is unknown so that 6-DOF poses will be determined
		 * @param priorityObjectPointIds Optional ids of the object points for which the poses will be optimized with higher priority, may be zero so that all object points are investigated with the same priority
		 * @param solePriorityPoints True, to apply only the priority object points for pose determination, has no meaning if no priority points are provided
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera pose, with range [5, infinity)
		 * @param poses The resulting determined poses starting with the lower frame and ending with the upper frame
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a pose counts as valid, with range (0, infinity)
		 * @param finalAverageError Optional resulting average final error for all valid poses, the error depends on the selected robust estimator
		 * @param worker Optional worker object to distribute the computation
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if all poses have been determined (some poses may be invalid)
		 */
		static bool determinePoses(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, const IndexSet32& priorityObjectPointIds, const bool solePriorityPoints, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, ShiftVector<HomogenousMatrix4>& poses, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar ransacMaximalSqrError = Scalar(3.5 * 3.5), const Scalar maximalRobustError = Scalar(3.5 * 3.5), Scalar* finalAverageError = nullptr, Worker* worker = nullptr, bool* abort = nullptr);

		/**
		 * This functions tracks image points (defined by their object points) from one frame to the sibling frames as long as the number of tracked points fall below a specified number or as long as a minimal number of sibling frames has been processed.
		 * Thus, this function supports two individual termination conditions: either the specification of a minimal number of tracked points or the specification of the minimal number of used sibling frames (with at least one tracked point).<br>
		 * If the number of tracked object points exceeds 'maximalTrackedObjectPoints' we select the most 'interesting' (by taking object points widely spread over the start frame) object points and remove the remaining.<br>
		 * The tracking is applied forward and backward starting at a specific frame.<br>
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points, object point positions and camera poses may be invalid as this information is not used
		 * @param objectPointIds The ids of the initial object points defining the image points which will be tracked, each object point should have a corresponding image point
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrame The index of the frame from which the algorithm will start, in this frame the specified initial object points must all be visible, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalTrackedObjectPoints One of two termination conditions: The minimal number of tracked points, with range [1, objectPointIds.size()], must be 0 if minimalTrackedFrames is not 0
		 * @param minimalTrackedFrames One of two termination conditions: The minimal number of tracked frames, with range [1, upperFrame - lowerFrame + 1u], must be 0 if minimalTrackedObjectPoints is not 0
		 * @param maximalTrackedObjectPoints The maximal number of tracked points, with range [minimalTrackedObjectPoints, objectPointIds.size()]
		 * @param trackedObjectPointIds The resulting ids of the tracked object points, one id for each tracked object point
		 * @param trackedImagePointGroups The resulting groups of tracked image point, one groups for each camera frame, one image point for each object point
		 * @param trackedValidIndices Optional resulting indices of the given object point ids that could be tracked
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool trackObjectPoints(const Database& database, const Indices32& objectPointIds, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const unsigned int minimalTrackedObjectPoints, const unsigned int minimalTrackedFrames, const unsigned int maximalTrackedObjectPoints, Indices32& trackedObjectPointIds, ImagePointGroups& trackedImagePointGroups, Indices32* trackedValidIndices = nullptr, bool* abort = nullptr);

		/**
		 * This functions tracks two individual groups (disjoined) image points (defined by their object points) from one frame to the sibling frames as long as the number of tracked points fall below a specified number.
		 * The tracking is applied forward and backward starting at a specific frame<br>.
		 * First, the priority points will be tracked as long as possible which defined the tracking range for the remaining points.<br>
		 * Afterwards, the remaining points will be tracked as long as possible but not outside the frame range which results from the tracking of the priority points.<br>
		 * Last, the results of both groups will be joined to one large set of tracked object points, first the priority object points, followed by the remaining object points.<br>
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points, object point positions and camera poses may be invalid as this information is not used
		 * @param priorityObjectPointIds The ids of the initial priority object points defining the first group of image points which will be tracked, each object point should have a corresponding image point
		 * @param remainingObjectPointIds The ids of the initial remaining object points defining the second group of image points which will be tracked, each object point should have a corresponding image point
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrame The index of the frame from which the algorithm will start, in this frame the specified initial object points must all be visible, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalTrackedPriorityObjectPoints The minimal number of tracked priority points, with range [1, priorityObjectPointIds.size())
		 * @param minimalRemainingFramesRatio The minimal number of frames in which remaining points must be tracked (must be visible) defined as a ratio of the number of frames in which the priority points are visible, with range (0, 1]
		 * @param maximalTrackedPriorityObjectPoints The maximal number of tracked priority points, with range [minimalTrackedPriorityObjectPoints, priorityObjectPointIds.size()]
		 * @param maximalTrackedRemainingObjectPoints The maximal number of tracked remaining points, with range [minimalTrackedRemainingObjectPoints, remainingObjectPointIds.size()]
		 * @param trackedObjectPointIds The resulting ids of the tracked object points, one id for each tracked object point
		 * @param trackedImagePointGroups The resulting groups of tracked image point, one groups for each camera frame, one image point for each object point
		 * @param trackedValidPriorityIndices Optional resulting indices of the given priority object point ids that could be tracked
		 * @param trackedValidRemainingIndices Optional resulting indices of the given remaining object point ids that could be tracked
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @return True, if succeeded
		 */
		static bool trackObjectPoints(const Database& database, const Indices32& priorityObjectPointIds, const Indices32& remainingObjectPointIds, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame, const unsigned int minimalTrackedPriorityObjectPoints, const Scalar minimalRemainingFramesRatio, const unsigned int maximalTrackedPriorityObjectPoints, const unsigned int maximalTrackedRemainingObjectPoints, Indices32& trackedObjectPointIds, ImagePointGroups& trackedImagePointGroups, Indices32* trackedValidPriorityIndices = nullptr, Indices32* trackedValidRemainingIndices = nullptr, bool* abort = nullptr);

		/**
		 * This function tracks a group of object points from one frame to both (if available) neighbor frames and counts the minimal number of tracked points.
		 * Use this function to measure the scene complexity at a specific frame.<br>
		 * The less object points can be tracked the more complex the scene.
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points, object point positions and camera poses may be invalid as this information is not used
		 * @param objectPointIds The ids of the object points which will be tracked, each object point should have a corresponding image point
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrame The index of the frame from which the algorithm will start, in this frame the specified initial object points must all be visible, with range [lowerFrame, upperFrame]
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @return The resulting tracked object points (the object points visible in the range of three image frames)
		 */
		static Indices32 trackObjectPointsToNeighborFrames(const Database& database, const Indices32& objectPointIds, const unsigned int lowerFrame, const unsigned int startFrame, const unsigned int upperFrame);

		/**
		 * Determines a set of representative camera poses from a given database within a specified frame range.
		 * Only valid camera poses from the database will be investigated.<br>
		 * @param database The database from which the representative camera poses are extracted
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param numberRepresentative The number of representative poses that will be determined
		 * @return The ids of the representative camera poses
		 */
		static Indices32 determineRepresentativePoses(const Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, const size_t numberRepresentative);

		/**
		 * Determines a set of representative camera poses from a given database from a set of given camera poses.
		 * @param database The database from which the representative camera poses are extracted
		 * @param poseIds The camera pose ids from which the representative camera poses are extracted, all poses must be valid
		 * @param numberRepresentative The number of representative poses that will be determined
		 * @return The ids of the representative camera poses
		 */
		static Indices32 determineRepresentativePoses(const Database& database, const Indices32& poseIds, const size_t numberRepresentative);

		/**
		 * Determines the camera 6-DOF pose for a specific camera frame.
		 * @param database The database from which the object point and image point correspondences are extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param frameId The id of the frame for which the camera pose will be determined
		 * @param roughPose Optional a rough camera pose to speedup the computation and accuracy
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera pose, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @param correspondences Optional resulting number of 2D/3D point correspondences which were available
		 * @return The resulting camera pose, an invalid pose if no pose can be determined
		 */
		static inline HomogenousMatrix4 determinePose(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const HomogenousMatrix4& roughPose = HomogenousMatrix4(false), const unsigned int minimalCorrespondences = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr, unsigned int* correspondences = nullptr);

		/**
		 * Determines the camera 6-DOF pose for a specific camera frame.
		 * @param database The database from which the object point and image point correspondences are extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param frameId The id of the frame for which the camera pose will be determined
		 * @param roughPose Optional a rough camera pose to speedup the computation and accuracy
		 * @param priorityObjectPointIds Ids of object points for which the poses will be optimized
		 * @param solePriorityPoints True, to apply only the priority object points for pose determination
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera pose, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @param correspondences Optional resulting number of 2D/3D point correspondences which were available
		 * @return The resulting camera pose, an invalid pose if no pose can be determined
		 */
		static inline HomogenousMatrix4 determinePose(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const IndexSet32& priorityObjectPointIds, const bool solePriorityPoints, const HomogenousMatrix4& roughPose = HomogenousMatrix4(false), const unsigned int minimalCorrespondences = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr, unsigned int* correspondences = nullptr);

		/**
		 * Determines the camera 6-DOF pose for a specific camera frame.
		 * @param database The database from which the image points are extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param frameId The id of the frame for which the camera pose will be determined
		 * @param objectPoints The object points which are all visible in the specified frame
		 * @param objectPointIds The ids of the object points, one id for each object points
		 * @param roughPose Optional a rough camera pose to speedup the computation and accuracy
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @return The resulting camera pose, an invalid pose if no pose can be determined
		 */
		static inline HomogenousMatrix4 determinePose(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<Index32>& objectPointIds, const HomogenousMatrix4& roughPose = HomogenousMatrix4(false), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr);

		/**
		 * Determines the camera 6-DOF pose for a set of object point and image point correspondences.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param objectPoints The object points which are visible in a frame
		 * @param imagePoints The image points which are projections of the given object points, one image point corresponds to one object point
		 * @param roughPose Optional a rough camera pose to speedup the computation and accuracy
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal robust squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @param validIndices Optional resulting indices of the valid point correspondences
		 * @return The resulting camera pose, an invalid pose if no pose can be determined
		 */
		static inline HomogenousMatrix4 determinePose(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const HomogenousMatrix4& roughPose = HomogenousMatrix4(false), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr, Indices32* validIndices = nullptr);

		/**
		 * Determines the camera 6-DOF pose for a set of object point and image point correspondences.
		 * The point correspondences are separated to a set of priority correspondences and remaining correspondences ensuring that the pose mainly matches for the priority point correspondences.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param objectPoints The object points which are visible in a frame, first all priority object points followed by the remaining object points
		 * @param imagePoints The image points which are projections of the given object points, one image point corresponds to one object point
		 * @param priorityCorrespondences The number of priority point correspondences
		 * @param roughPose Optional a rough camera pose to speedup the computation and accuracy
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal robust squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @return The resulting camera pose, an invalid pose if no pose can be determined
		 */
		static inline HomogenousMatrix4 determinePose(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const size_t priorityCorrespondences, const HomogenousMatrix4& roughPose = HomogenousMatrix4(false), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr);

		/**
		 * Determines the camera 3-DOF orientation (as the camera has rotational motion only) for a specific camera frame.
		 * @param database The database from which the object point and image point correspondences are extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param frameId The id of the frame for which the camera orientation will be determined
		 * @param roughOrientation Optional a rough camera orientation to speedup the computation and accuracy
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera orientation, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear orientation optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @param correspondences Optional resulting number of 2D/3D point correspondences which were available
		 * @return The resulting camera orientation, an invalid orientation if no orientation can be determined
		 */
		static inline SquareMatrix3 determineOrientation(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const SquareMatrix3& roughOrientation = SquareMatrix3(false), const unsigned int minimalCorrespondences = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr, unsigned int* correspondences = nullptr);

		/**
		 * Determines the camera 3-DOF orientation (as the camera has rotational motion only) for a specific camera frame.
		 * @param database The database from which the object point and image point correspondences are extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param frameId The id of the frame for which the camera orientation will be determined
		 * @param priorityObjectPointIds Ids of object points for which the poses will be optimized
		 * @param solePriorityPoints True, to apply only the priority object points for pose determination
		 * @param roughOrientation Optional a rough camera orientation to speedup the computation and accuracy
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera orientation, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear orientation optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @param correspondences Optional resulting number of 2D/3D point correspondences which were available
		 * @return The resulting camera orientation, an invalid orientation if no orientation can be determined
		 */
		static inline SquareMatrix3 determineOrientation(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const IndexSet32& priorityObjectPointIds, const bool solePriorityPoints, const SquareMatrix3& roughOrientation = SquareMatrix3(false), const unsigned int minimalCorrespondences = 10u, const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr, unsigned int* correspondences = nullptr);

		/**
		 * Determines the camera 3-DOF orientation (as the camera has rotational motion only) for a specific camera frame.
		 * @param database The database from which the image points are extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param frameId The id of the frame for which the camera orientation will be determined
		 * @param objectPoints The object points which are all visible in the specified frame
		 * @param objectPointIds The ids of the object points, one id for each object points
		 * @param numberObjectPoints The number of given object points, with range [5, infinity)
		 * @param roughOrientation Optional a rough camera orientation to speedup the computation and accuracy
		 * @param estimator The robust estimator which is applied for the non-linear orientation optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @return The resulting camera orientation, an invalid orientation if no orientation can be determined
		 */
		static inline SquareMatrix3 determineOrientation(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const ObjectPoint* objectPoints, const Index32* objectPointIds, const size_t numberObjectPoints, const SquareMatrix3& roughOrientation = SquareMatrix3(false), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr);

		/**
		 * Determines the camera 3-DOF orientation for a set of object point and image point correspondences.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param objectPoints The object points which are visible in a frame
		 * @param imagePoints The image points which are projections of the given object points, one image point corresponds to one object point
		 * @param roughOrientation Optional a rough camera orientation to speedup the computation and accuracy
		 * @param estimator The robust estimator which is applied for the non-linear orientation optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal robust squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @param validIndices Optional resulting indices of the valid point correspondences
		 * @return The resulting camera orientation, an invalid orientation if no orientation can be determined
		 */
		static inline SquareMatrix3 determineOrientation(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const SquareMatrix3& roughOrientation = SquareMatrix3(false), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr, Indices32* validIndices = nullptr);

		/**
		 * Determines the camera 3-DOF orientation for a set of object point and image point correspondences.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param objectPoints The object points which are visible in a frame, first all priority object points followed by the remaining object points
		 * @param imagePoints The image points which are projections of the given object points, one image point corresponds to one object point
		 * @param priorityCorrespondences The number of priority point correspondences
		 * @param roughOrientation Optional a rough camera orientation to speedup the computation and accuracy
		 * @param estimator The robust estimator which is applied for the non-linear orientation optimization
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalSqrError The maximal robust squared pixel error between image point and projected object points for the RANSAC algorithm, with range (0, infinity)
		 * @param finalRobustError Optional resulting final average robust error, in relation to the defined estimator
		 * @return The resulting camera orientation, an invalid orientation if no orientation can be determined
		 */
		static inline SquareMatrix3 determineOrientation(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const size_t priorityCorrespondences, const SquareMatrix3& roughOrientation = SquareMatrix3(false), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_SQUARE, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Scalar* finalRobustError = nullptr);

		/**
		 * Determines valid poses for a range of camera frames while for each frame a group of image points is given which correspond to the given object points.
		 * Two individual camera poses must be known within the range of camera frames.<br>
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPoints The object points with known locations, each object point has a corresponding image point in the groups of image points
		 * @param imagePointGroups The groups of image points, each set of image points corresponds to the object points, each group of image points represents one camera pose (the observed object points respectively)
		 * @param randomGenerator Random number generator
		 * @param cameraMotion The motion of the camera, use CM_UNKNOWN if the motion is unknown so that 6-DOF poses will be determined
		 * @param firstValidPoseIndex The index of the frame for which the first pose is known, with range [imagePointGroups.firstIndex(), imagePointGroups.lastIndex()]
		 * @param firstValidPose The first known pose, must be valid
		 * @param secondValidPoseIndex The index of the frame for which the second pose is known, with range [imagePointGroups.firstIndex(), imagePointGroups.lastIndex()] with firstValidPoseIndex != secondValidPoseIndex
		 * @param secondValidPose The second known pose, must be valid
		 * @param minimalValidCorrespondenceRatio The minimal ratio of valid correspondences (w.r.t. the given object points), if the number of valid correspondences is too low the pose is not valid, with range (0, 1]
		 * @param maximalSqrError The maximal pixel error between a projected object point and the corresponding image point so that the correspondence is valid
		 * @param validObjectPointIndices Optional resulting indices of the object points which are all valid in all determined valid poses
		 * @param poses Optional resulting valid poses (corresponding to poseIds)
		 * @param poseIds Optional resulting ids of all valid poses, each id has a corresponding resulting pose (however the ids themselves have no order)
		 * @param totalSqrError Optional resulting sum of square pixel errors for all valid poses
		 * @return The number of valid poses
		 */
		static size_t determineValidPoses(const AnyCamera& camera, const Vectors3& objectPoints, const ImagePointGroups& imagePointGroups, RandomGenerator& randomGenerator, const CameraMotion cameraMotion, const unsigned int firstValidPoseIndex, const HomogenousMatrix4& firstValidPose, const unsigned int secondValidPoseIndex, const HomogenousMatrix4& secondValidPose, const Scalar minimalValidCorrespondenceRatio = Scalar(1), const Scalar maximalSqrError = Scalar(3.5 * 3.5), Indices32* validObjectPointIndices = nullptr, HomogenousMatrices4* poses = nullptr, Indices32* poseIds = nullptr, Scalar* totalSqrError = nullptr);

		/**
		 * Determines the camera motion from the camera poses within a specified frame range covering only valid poses.
		 * @param database The database from which the camera pose are taken
		 * @param pinholeCamera The pinhole camera profile which is applied
		 * @param lowerFrame The index of the frame defining the lower border of the camera frames which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of the camera frames which will be investigated, with range [lowerFrame, infinity)
		 * @param onlyVisibleObjectPoints True, to use only object points which are visible within the defined frame range; False, to use all object points
		 * @param worker Optional worker object to distribute the computation
		 * @param minimalTinyTranslationObservationAngle The minimal angle of observation rays for 3D object points so that the motion contains a tiny translational motion, with range (0, PI/2)
		 * @param minimalModerateTranslationObservationAngle The minimal angle of observation rays for 3D object points so that the motion contains a moderate translational motion, with range (minimalTinyTranslationObservationAngle, PI/2)
		 * @param minimalSignificantTranslationObservationAngle The minimal angle of observation rays for 3D object points so that the motion contains a significant translational motion, with range (minimalSignificantTranslationObservationAngle, PI/2)
		 * @param minimalTinyRotationAngle The minimal angle between the viewing directions so that the motion contains a tiny rotational motion, with range (0, PI/2)
		 * @param minimalModerateRotationAngle The minimal angle between the viewing directions so that the motion contains a moderate rotational motion, with range (minimalTinyRotationAngle, PI/2)
		 * @param minimalSignificantRotationAngle The minimal angle between the viewing directions so that the motion contains a significant rotational motion, with range (minimalSignificantRotationAngle, PI/2)
		 * @return The resulting motion of the camera
		 */
		static CameraMotion determineCameraMotion(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrame, const unsigned int upperFrame, const bool onlyVisibleObjectPoints = true, Worker* worker = nullptr, const Scalar minimalTinyTranslationObservationAngle = Numeric::deg2rad(Scalar(0.15)), const Scalar minimalModerateTranslationObservationAngle = Numeric::deg2rad(1), const Scalar minimalSignificantTranslationObservationAngle = Numeric::deg2rad(5), const Scalar minimalTinyRotationAngle = Numeric::deg2rad(Scalar(0.25)), const Scalar minimalModerateRotationAngle = Numeric::deg2rad(5), const Scalar minimalSignificantRotationAngle = Numeric::deg2rad(10));

		/**
		 * Measures the accuracy of a 3D object point in combination with a set of camera poses and image points (the projections of the object point).
		 * The accuracy of the point can be determined by individual methods, while the basic idea is to use the angles between the individual observation rays of the object point.<br>
		 * @param pinholeCamera The pinhole camera profile which is applied
		 * @param poses The camera poses in which the object point is visible
		 * @param imagePoints The individual image points in the individual camera frames
		 * @param observations The number of observations (pairs of camera poses and image points)
		 * @param accuracyMethod The method which is applied to determine the accuracy, must be valid
		 * @return The resulting accuracy parameter depending on the specified method
		 */
		static Scalar determineObjectPointAccuracy(const PinholeCamera& pinholeCamera, const HomogenousMatrix4* poses, const Vector2* imagePoints, const size_t observations, const AccuracyMethod accuracyMethod);

		/**
		 * Measures the accuracy of several 3D object points.
		 * This methods extracts the 3D object point locations from the given database.<br>
		 * The accuracy of the points can be determined by individual methods, while the basic idea is to use the angles between the individual observation rays of the object points.<br>
		 * @param database The database providing the location of the 3D object points, the camera poses and the image point positions.<br>
		 * @param pinholeCamera The pinhole camera profile which is applied
		 * @param objectPointIds The ids of the object points for which the accuracies will be determined, each object point must be valid
		 * @param accuracyMethhod The method which is applied to determine the accuracy, must be valid
		 * @param lowerFrame Optional index of the frame defining the lower border of camera poses which will be investigated, -1 if no lower and no upper border is defined
		 * @param upperFrame Optional index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity), -1 if also 'lowerFrame' is -1
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting accuracy parameters depending on the specified method, one parameter of each object point, an accuracy of -1 indicates an invalid point (e.g., due to too less measurements)
		 */
		static Scalars determineObjectPointsAccuracy(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& objectPointIds, const AccuracyMethod accuracyMethhod, const unsigned int lowerFrame = (unsigned int)(-1), const unsigned int upperFrame = (unsigned int)(-1), Worker* worker = nullptr);

		/**
		 * Determines the projection errors of a 3D object point in combination with a set of camera poses and image points (the projections of the object point).
		 * @param camera The camera profile defining the projection, must be valid
		 * @param objectPoint The 3D object point for which the quality will be measured
		 * @param world_T_cameras The camera poses in which the object point is visible
		 * @param imagePoints The individual image points in the individual camera frames
		 * @param minimalSqrError Optional resulting minimal (best) projection error for the object point
		 * @param averageSqrError Optional resulting averaged projection error for the object point
		 * @param maximalSqrError Optional resulting maximal (worst) projection error for the object point
		 */
		static void determineProjectionErrors(const AnyCamera& camera, const Vector3& objectPoint, const ConstIndexedAccessor<HomogenousMatrix4>& world_T_cameras, const ConstIndexedAccessor<Vector2>& imagePoints, Scalar* minimalSqrError = nullptr, Scalar* averageSqrError = nullptr, Scalar* maximalSqrError = nullptr);

		/**
		 * Determines the accuracy of a camera pose for all valid object points visible in the frame by measuring the projection error between the projected object points and their corresponding image points.
		 * @param database The database providing the locations of the 3D object points, the camera poses and the image points
		 * @param pinholeCamera The pinhole camera profile which is applied
		 * @param poseId The id of the camera frame for which the accuracy of the pose will be determined
		 * @param useDistortionParameters True, to apply the distortion parameter of the camera
		 * @param validCorrespondences Optional resulting number of valid pose correspondences
		 * @param minimalSqrError Optional resulting minimal (best) projection error for the pose
		 * @param averageSqrError Optional resulting averaged projection error for the pose
		 * @param maximalSqrError Optional resulting maximal (worst) projection error for the pose
		 * @return True, if the database holds a valid pose for the specified camera frame and at least one valid point correspondence
		 */
		static bool determineProjectionError(const Database& database, const PinholeCamera& pinholeCamera, const Index32 poseId, const bool useDistortionParameters, unsigned int* validCorrespondences = nullptr, Scalar* minimalSqrError = nullptr, Scalar* averageSqrError = nullptr, Scalar* maximalSqrError = nullptr);

		/**
		 * Determines the averaged and maximal squared pixel errors between the projections of individual 3D object points and their corresponding image points in individual camera frames.
		 * @param database The database from which the camera poses, the object points and the image points are extracted
		 * @param pinholeCamera The pinhole camera profile which is applied
		 * @param objectPointIds The ids of all object points for which the maximal squared pixel errors are determined
		 * @param useDistortionParameters True, to use the distortion parameters of the camera to distort the projected object points
		 * @param lowerFrame Optional index of the frame defining the lower border of camera poses which will be investigated, -1 if no lower and no upper border is defined
		 * @param upperFrame Optional index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity), -1 if also 'lowerFrame' is -1
		 * @param minimalSqrErrors Optional resulting minimal squared pixel errors, one error for each given object point id, invalid object points or object points without corresponding observation receive Numeric::maxValue() as error
		 * @param averagedSqrErrors Optional resulting averaged pixel errors, one error for each given object point id, invalid object points or object points without corresponding observation receive Numeric::maxValue() as error
		 * @param maximalSqrErrors Optional resulting maximal squared pixel errors, one error for each given object point id, invalid object points or object points without corresponding observation receive Numeric::maxValue() as error
		 * @param observations Optional resulting observations for each object point, one number of observations for each given object point id
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determineProjectionErrors(const Database& database, const PinholeCamera& pinholeCamera, const Indices32& objectPointIds, const bool useDistortionParameters, const unsigned int lowerFrame = (unsigned int)(-1), const unsigned int upperFrame = (unsigned int)(-1), Scalar* minimalSqrErrors = nullptr, Scalar* averagedSqrErrors = nullptr, Scalar* maximalSqrErrors = nullptr, unsigned int* observations = nullptr, Worker* worker = nullptr);

		/**
		 * Determines the individual cosine values between the mean coordinate axis of a range of poses and the coordinate axis of the individual poses.
		 * The specified range of camera pose must cover a range with valid poses.<br>
		 * @param database The database providing the camera poses
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param xOrientations The resulting cosine values for the poses' xAxis, one for each camera pose
		 * @param yOrientations The resulting cosine values for the poses' yAxis, one for each camera pose
		 * @param zOrientations The resulting cosine values for the poses' zAxis, one for each camera pose
		 */
		static void determinePosesOrientation(const Database& database, const unsigned int lowerFrame, const unsigned int upperFrame, Scalar* xOrientations, Scalar* yOrientations, Scalar* zOrientations);

		/**
		 * Determines the number of valid correspondences between image points and object points for each frame within a specified frame range.
		 * @param database The database providing the 3D object points, the 2D image points and the topology between image and object points
		 * @param needValidPose True, if the pose must be valid so that the number of valid correspondences will be determined, otherwise the number of correspondences will be zero
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences Optional resulting minimal number of correspondences for all frames within the defined frame range
		 * @param averageCorrespondences Optional resulting averaged number of correspondences for all frames within the defined frame range
		 * @param medianCorrespondences Optional resulting median of all correspondences for all frames within the defined frame range
		 * @param maximalCorrespondences Optional resulting maximal number correspondences for all frames within the defined frame range
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determineNumberCorrespondences(const Database& database, const bool needValidPose, const unsigned int lowerFrame, const unsigned int upperFrame, unsigned int* minimalCorrespondences = nullptr, Scalar* averageCorrespondences = nullptr, unsigned int* medianCorrespondences = nullptr, unsigned int* maximalCorrespondences = nullptr, Worker* worker = nullptr);

		/**
		 * Determines a 3D plane best fitting to a set of given 3D object points.
		 * @param objectPoints The object points for which the best matching plane will be determined, at least 3
		 * @param randomGenerator Random number generator
		 * @param plane The resulting 3D plane
		 * @param minimalValidObjectPoints The minimal number of valid object points so that a valid plane will be determined
		 * @param estimator The robust estimator which will be applied to determine the 3D plane
		 * @param finalError Optional resulting final error
		 * @param validIndices Optional resulting indices of all valid object points
		 * @return True, if succeeded
		 */
		static inline bool determinePlane(const ConstIndexedAccessor<Vector3>& objectPoints, RandomGenerator& randomGenerator, Plane3& plane, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(3u, Scalar(0.5), 20u), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_HUBER, Scalar* finalError = nullptr, Indices32* validIndices = nullptr);

		/**
		 * Determines a 3D plane best fitting to a set of given 3D object point ids.
		 * @param database The database holding the 3D object point locations
		 * @param objectPointIds The ids of the object points for which the best matching plane will be determined, at least 3, must have valid locations in the database
		 * @param randomGenerator Random number generator
		 * @param plane The resulting 3D plane
		 * @param minimalValidObjectPoints The minimal number of valid object points so that a valid plane will be determined
		 * @param estimator The robust estimator which will be applied to determine the 3D plane
		 * @param finalError Optional resulting final error
		 * @param validIndices Optional resulting indices of all valid object points
		 * @return True, if succeeded
		 */
		static inline bool determinePlane(const Database& database, const Indices32& objectPointIds, RandomGenerator& randomGenerator, Plane3& plane, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(3u, Scalar(0.5), 20u), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_HUBER, Scalar* finalError = nullptr, Indices32* validIndices = nullptr);

		/**
		 * Determines a 3D plane best fitting to a set of given 3D object point ids which are specified by a given sub-region in the camera frame.
		 * @param database The database holding the 3D object point locations
		 * @param frameIndex The index of the frame in which the plane is visible for which the given sub-region defines the area of image points for which the corresponding object points define the 3D plane, the pose must be valid
		 * @param subRegion The sub-region which defines the plane area in the camera frame
		 * @param randomGenerator Random number generator
		 * @param plane The resulting 3D plane
		 * @param minimalValidObjectPoints The minimal number of valid object points so that a valid plane will be determined
		 * @param estimator The robust estimator which will be applied to determine the 3D plane
		 * @param finalError Optional resulting final error
		 * @param usedObjectPointIds Optional resulting ids of the used object points
		 * @return True, if succeeded
		 */
		static bool determinePlane(const Database& database, const Index32 frameIndex, const CV::SubRegion& subRegion, RandomGenerator& randomGenerator, Plane3& plane, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(3u, Scalar(0.5), 20u), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_HUBER, Scalar* finalError = nullptr, Indices32* usedObjectPointIds = nullptr);

		/**
		 * Determines a 3D plane best fitting to image points in a specified sub-region in a specified frame and best fitting to this area visible in a specified frame range.
		 * @param database The database holding the 3D object point locations
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param lowerFrameIndex The index of the frame defining the lower border of camera poses which will be investigated
		 * @param subRegionFrameIndex The index of the frame for which the sub-region is specified
		 * @param upperFrameIndex The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param subRegion The sub-region defining the area in the image frame for which the 3D plane will be determined
		 * @param randomGenerator The random number generator object
		 * @param plane The resulting 3D plane best fitting for the given data
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param minimalValidObjectPoints The minimal number of valid 3D points in relation to the 3D object points which are projected into the sub-region in the sub-region frame
		 * @param medianDistanceFactor The factor with which the median distance between the initial 3D plane and the initial 3D object points is multiplied to determine the maximal distance between the finial plane and any 3D object point which defines the plane, with range (0, infinity)
		 * @param estimator The robust estimator used to determine the initial plane for the sub-region frame
		 * @param finalError Optional resulting final square error
		 * @param usedObjectPointIds Optional resulting ids of all 3D object points which have been used to determine the 3D plane
		 * @return True, if succeeded
		 */
		static bool determinePlane(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int lowerFrameIndex, const unsigned int subRegionFrameIndex, const unsigned int upperFrameIndex, const CV::SubRegion& subRegion, RandomGenerator& randomGenerator, Plane3& plane, const bool useDistortionParameters, const RelativeThreshold& minimalValidObjectPoints = RelativeThreshold(3u, Scalar(0.5), 20u), const Scalar medianDistanceFactor = Scalar(6), const Geometry::Estimator::EstimatorType estimator = Geometry::Estimator::ET_HUBER, Scalar* finalError = nullptr, Indices32* usedObjectPointIds = nullptr);

		/**
		 * Determines a 3D plane perpendicular to the camera with specified distance to the camera.
		 * This function may be used for e.g., rotational camera motion as e.g., initial guess.
		 * @param database The database holding the 3D object point locations
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param frameIndex The index of the frame in which the given image point is visible
		 * @param imagePoint The image point to which (to the viewing ray respectively) the resulting plane will be perpendicular, must lie inside the camera frame
		 * @param distance The distance of the plane to the camera, with range (0, infinity)
		 * @param plane The resulting 3D plane best fitting for the given data
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param pointOnPlane Optional resulting 3D intersection point of resulting plane and the viewing ray of the provided image point
		 * @return True, if succeeded
		 */
		static bool determinePerpendicularPlane(const Database& database, const PinholeCamera& pinholeCamera, const unsigned int frameIndex, const Vector2& imagePoint, const Scalar distance, Plane3& plane, const bool useDistortionParameters, Vector3* pointOnPlane = nullptr);

		/**
		 * Determines a 3D plane perpendicular to the camera with specified distance to the camera.
		 * This function may be used for e.g., rotational camera motion as e.g., initial guess.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param pose The pose of the camera, must be valid
		 * @param imagePoint The image point to which (to the viewing ray respectively) the resulting plane will be perpendicular, must lie inside the camera frame
		 * @param distance The distance of the plane to the camera, with range (0, infinity)
		 * @param plane The resulting 3D plane best fitting for the given data
		 * @param useDistortionParameters True, to use the distortion parameters of the camera
		 * @param pointOnPlane Optional resulting 3D intersection point of resulting plane and the viewing ray of the provided image point
		 * @return True, if succeeded
		 */
		static bool determinePerpendicularPlane(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& pose, const Vector2& imagePoint, const Scalar distance, Plane3& plane, const bool useDistortionParameters, Vector3* pointOnPlane = nullptr);

		/**
		 * Removes very far object points from the database if the amount of object points does not exceed a specified ratio (compared to the remaining object points).
		 * Optimization functions for camera poses or bundle adjustment functions may fail if the database holds a large set of dense object points and a small number of very sparse object points.<br>
		 * Thus, this function can be used to improve the 'quality' of a database.
		 * @param database The database from which the very sparse object points will be removed
		 * @param minimalBoundingBoxDiagonal the minimal size of the diagonal of the bounding box of the object points so that the database can be modified, with range (0, infinity)
		 * @param medianFactor The factor which is multiplied with the median distance between the median object point and the object points of the database to identify very sparse (very far) object points
		 * @param maximalSparseObjectPointRatio The maximal ratio between the very spars object points and the entire number of object points so that the database will be modified
		 * @return True, if at least one very sparse object point has been removed from the database
		 */
		static bool removeSparseObjectPoints(Database& database, const Scalar minimalBoundingBoxDiagonal = Scalar(1e+7), const Scalar medianFactor = Scalar(100), const Scalar maximalSparseObjectPointRatio = Scalar(0.05));

		/**
		 * Removes all valid 3D object points (and their corresponding 2D image points) from the database which are at least in one frame not in front of the camera while having an existing 2D image point as observation.
		 * @param database The database from which the 3D object points will be removed
		 * @param removedObjectPointIds Optional resulting ids of all object points which have been removed, nullptr if not of interest
		 * @return The number of removed 3D object points
		 */
		static size_t removeObjectPointsNotInFrontOfCamera(Database& database, Indices32* removedObjectPointIds = nullptr);

		/**
		 * Removes any 3D object point (and it's corresponding 2D image points) from the database with less then a specified number of observations.
		 * @param database The database from which the 3D object points will be removed
		 * @param minimalNumberObservations The minimal number of observations a 3D object point must have to stay in the database, with range [1, infinity)
		 * @param removedObjectPointIds Optional resulting ids of all object points which have been removed, nullptr if not of interest
		 * @return The number of removed 3D object points
		 */
		static size_t removeObjectPointsWithoutEnoughObservations(Database& database, const size_t minimalNumberObservations, Indices32* removedObjectPointIds = nullptr);

		/**
		 * Removes any 3D object point (and it's corresponding 2D image points) from the database if all their corresponding camera poses are located within a too small bounding box.
		 * The bounding box is determined based on the translational parts of the camera poses.
		 * @param database The database from which the 3D object points will be removed
		 * @param minimalBoxDiagonal The minimal diagonal of the bounding box of all camera poses of supporting an object point to stay in the database
		 * @param removedObjectPointIds Optional resulting ids of all object points which have been removed, nullptr if not of interest
		 * @return The number of removed 3D object points
		 */
		static size_t removeObjectPointsWithSmallBaseline(Database& database, const Scalar minimalBoxDiagonal, Indices32* removedObjectPointIds = nullptr);

		/**
		 * Translates a camera motion value to a string providing the detailed motion as readable string.
		 * @param cameraMotion The camera motion for which a readable string is requested
		 * @return The readable string of the camera motion
		 */
		static std::string translateCameraMotion(const CameraMotion cameraMotion);

	protected:

		/**
		 * Determines a subset of perfectly static image points which may be image points located (visible) at static logos in the frames.
		 * @param imagePointGroups Groups of image points where each group holds the projection of the same 3D object points
		 * @param objectPointIds The ids of the object points which have the corresponding projected image points in the groups of image points
		 * @param maximalStaticImagePointFilterRatio The maximal ratio of static image points in relation to the entire number of image points in each group, with range [0, 1]
		 * @return The number of static image points that have been removed
		 */
		static size_t filterStaticImagePoints(ImagePointGroups& imagePointGroups, Indices32& objectPointIds, const Scalar maximalStaticImagePointFilterRatio);

		/**
		 * Determines the initial positions of 3D object points in a database if no camera poses or structure information is known.
		 * This functions processes a subset of pre-defined start frames from which the point tracking starts.<br>
		 * @param database The database defining the topology of 3D object points and corresponding 2D image points
		 * @param pinholeCamera The pinhole camera profile which will be applied
		 * @param randomGenerator A random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param startFrames The entire set of start frames from which a subset will be processed
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated
		 * @param maximalStaticImagePointFilterRatio The maximal ratio between (perfectly) static image points and the overall number of image points so that these static image points will be filtered and not used, with ratio [0, 1), 0 to avoid any filtering
		 * @param initialObjectPoints The resulting initial 3D positions of object points that could be extracted
		 * @param initialObjectPointIds The resulting ids of the resulting object points, one id for each resulting object point
		 * @param initialPoseIds The resulting ids of all camera poses which have been used to determine the resulting initial object points
		 * @param initialPointDistance The resulting distance between the image points which have been used to determine the initial object points, which is a measure for the reliability of the resulting 3D object points
		 * @param pointsThreshold The threshold of image points which must be visible in each camera frame
		 * @param minimalKeyFrames The minimal number of keyframes that will be extracted
		 * @param maximalKeyFrames The maximal number of keyframes that will be extracted
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @param lock The lock object which must be defined if this function is executed in parallel on several threads, otherwise nullptr
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param numberThreads The number of threads on which this function is executed in parallel, with range [1, infinity)
		 * @param threadIndex The index of the thread on which this function is executed
		 * @param numberThreadsOne Must be 1
		 */
		static void determineInitialObjectPointsFromSparseKeyFramesByStepsSubset(const Database* database, const PinholeCamera* pinholeCamera, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const Indices32* startFrames, const unsigned int upperFrame, const Scalar maximalStaticImagePointFilterRatio, Vectors3* initialObjectPoints, Indices32* initialObjectPointIds, Indices32* initialPoseIds, Scalar* initialPointDistance, const RelativeThreshold* pointsThreshold, const unsigned int minimalKeyFrames, const unsigned int maximalKeyFrames, const Scalar maximalSqrError, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne);

		/**
		 * Determines the initial object point positions for a set of frames (image point groups) observing the unique object points in individual camera poses by a RANSAC algorithm.
		 * This function applies a RANSAC mechanism randomly selecting individual start key frames (pairs of image points).<br>
		 * The key frames (image point groups) provide the following topology:<br>
		 * For n unique object points visible in m individual frames we have n object points (op) and n * m overall image points (ip):
		 * <pre>
		 *                 op_1,   op_2,   op_3,   op_4,   ..., op_n
		 * ...
		 * dense_pose_2 -> ip_3_1, ip_3_2, ip_3_3, ip_3_4, ..., ip_3_n
		 * dense_pose_3 -> ip_4_1, ip_4_2, ip_4_3, ip_4_4, ..., ip_4_n
		 * ...
		 * </pre>
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param imagePointGroups Frames of image points, all points in one group are located in the same camera frame and the individual points correspond to the same unique object points
		 * @param randomGenerator A random generator object
		 * @param validPoses The resulting poses that could be determined
		 * @param validPoseIds The ids of resulting valid poses, one id for each resulting valid pose (the order of the ids is arbitrary)
		 * @param objectPoints The resulting object points that could be determined
		 * @param validObjectPointIndices The indices of resulting valid object points in relation to the given image point groups
		 * @param totalError The resulting total error of the best RANSAC iteration
		 * @param minimalValidObjectPoints The threshold of object points that must be valid
		 * @param maximalSqrError The maximal square distance between an image points and a projected object point
		 * @param remainingIterations The number of RANSAC iterations that still need to be applied
		 * @param lock The lock object which must be defined if this function is executed in parallel on several threads, otherwise nullptr
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param firstIteration The first RANSAC iteration to apply, has no meaning as 'remainingIterations' is used instead
		 * @param numberIterations The number of RANSAC iterations to apply, has no meaning as 'remainingIterations' is used instead
		 * @see determineInitialObjectPointsFromDenseFramesRANSAC().
		 */
		static void determineInitialObjectPointsFromDenseFramesRANSACSubset(const PinholeCamera* pinholeCamera, const ImagePointGroups* imagePointGroups, RandomGenerator* randomGenerator, HomogenousMatrices4* validPoses, Indices32* validPoseIds, Vectors3* objectPoints, Indices32* validObjectPointIndices, Scalar* totalError, const RelativeThreshold* minimalValidObjectPoints, const Scalar maximalSqrError, unsigned int* remainingIterations, Lock* lock, bool* abort, unsigned int firstIteration, unsigned int numberIterations);

		/**
		 * Updates a subset of the camera poses depending on valid 2D/3D points correspondences within a range of camera frames.
		 * The camera poses will be set to invalid if no valid pose can be determined (e.g., if not enough valid point correspondences are known for a specific camera frame).<br>
		 * @param database The database from which the point correspondences are extracted and which receives the determined camera poses
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera pose, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a pose counts as valid, with range (0, infinity)
		 * @param totalError The resulting accumulated total error for all poses
		 * @param validPoses The resulting number of valid poses
		 * @param lock The lock object which must be defined if this function is executed in parallel on several individual threads
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param numberThreads The overall number of threads which are used in parallel
		 * @param threadIndex The index of the thread executing this function, with range [0, numberThreads)
		 * @param numberThreadsOne Must be 1
		 */
		static void updatePosesSubset(Database* database, const AnyCamera* camera, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, size_t* validPoses, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne);

		/**
		 * Updates a subset of the camera orientations (as the camera has rotational motion only) depending on valid 2D/3D points correspondences within a range of camera frames.
		 * The camera orientations (their poses respectively) will be set to invalid if no valid orientation can be determined (e.g., if not enough valid point correspondences are known for a specific camera frame).<br>
		 * @param database The database from which the point correspondences are extracted and which receives the determined camera orientations (the 6-DOF poses with zero translation)
		 * @param camera The camera profile defining the projection, must be valid
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera orientations, with range [5, infinity)
		 * @param estimator The robust estimator which is applied for the non-linear orientation optimization
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a orientation counts as valid, with range (0, infinity)
		 * @param totalError The resulting accumulated total error for all poses (orientations)
		 * @param validPoses The resulting number of valid poses (orientations)
		 * @param lock The lock object which must be defined if this function is executed in parallel on several individual threads
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param numberThreads The overall number of threads which are used in parallel
		 * @param threadIndex The index of the thread executing this function, with range [0, numberThreads)
		 * @param numberThreadsOne Must be 1
		 */
		static void updateOrientationsSubset(Database* database, const AnyCamera* camera, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, size_t* validPoses, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne);

		/**
		 * Determines a subset of the camera poses depending on valid 2D/3D points correspondences within a range of camera frames.
		 * The camera poses will be set to invalid if no valid pose can be determined (e.g., if not enough valid point correspondences are known for a specific camera frame).<br>
		 * @param database The database from which the point correspondences are extracted and which receives the determined camera poses
		 * @param camera The camera profile defining the projection, must be valid
		 * @param priorityObjectPointIds Optional ids of the object points for which the poses will be optimized, may be zero so that all object points are investigated with the same priority
		 * @param solePriorityPoints True, to apply only the priority object points for pose determination
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera pose, with range [5, infinity)
		 * @param poses The resulting determined poses starting with the lower frame and ending with the upper frame
		 * @param estimator The robust estimator which is applied for the non-linear pose optimization
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a pose counts as valid, with range (0, infinity)
		 * @param totalError The resulting accumulated total error for all poses
		 * @param lock The lock object which must be defined if this function is executed in parallel on several individual threads
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param numberThreads The overall number of threads which are used in parallel
		 * @param threadIndex The index of the thread executing this function, with range [0, numberThreads)
		 * @param numberThreadsOne Must be 1
		 */
		static void determinePosesSubset(const Database* database, const AnyCamera* camera, const IndexSet32* priorityObjectPointIds, const bool solePriorityPoints, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, ShiftVector<HomogenousMatrix4>* poses, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne);

		/**
		 * Determines a subset of the camera orientations (as the camera has rotational motion only) depending on valid 2D/3D points correspondences within a range of camera frames.
		 * The camera orientations (their poses respectively) will be set to invalid if no valid orientation can be determined (e.g., if not enough valid point correspondences are known for a specific camera frame).<br>
		 * @param database The database from which the point correspondences are extracted and which receives the determined camera orientations (the 6-DOF poses with zero translation)
		 * @param camera The camera profile defining the projection, must be valid
		 * @param priorityObjectPointIds Optional ids of the object points for which the poses will be optimized, may be zero so that all object points are investigated with the same priority
		 * @param solePriorityPoints True, to apply only the priority object points for pose determination
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalCorrespondences The minimal number of 2D/3D points correspondences which are necessary to determine a valid camera orientations, with range [5, infinity)
		 * @param poses The resulting determined poses starting with the lower frame and ending with the upper frame
		 * @param estimator The robust estimator which is applied for the non-linear orientation optimization
		 * @param ransacMaximalSqrError The maximal squared pixel error between image point and projected object points for RANSAC iterations, with range (0, infinity)
		 * @param minimalValidCorrespondenceRatio The ratio of the minimal number of valid correspondences (the valid correspondences will be determined from a RANSAC iteration), with range [0, 1]
		 * @param maximalRobustError The maximal average robust pixel error between image point and projected object points so that a orientation counts as valid, with range (0, infinity)
		 * @param totalError The resulting accumulated total error for all poses (orientations)
		 * @param lock The lock object which must be defined if this function is executed in parallel on several individual threads
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param numberThreads The overall number of threads which are used in parallel
		 * @param threadIndex The index of the thread executing this function, with range [0, numberThreads)
		 * @param numberThreadsOne Must be 1
		 */
		static void determineOrientationsSubset(const Database* database, const AnyCamera* camera, const IndexSet32* priorityObjectPointIds, const bool solePriorityPoints, RandomGenerator* randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalCorrespondences, ShiftVector<HomogenousMatrix4>* poses, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar ransacMaximalSqrError, const Scalar maximalRobustError, Scalar* totalError, Lock* lock, bool* abort, const unsigned int numberThreads, const unsigned int threadIndex, const unsigned int numberThreadsOne);

		/**
		 * Determines the semi-precise location of 3D object points and the camera poses for a sole rotational camera motion.
		 * The locations and the camera poses may not match with a pure rotational camera motion before.<br>
		 * Only object points with an already valid location will receive a precise location matching to the rotational motion.<br>
		 * Only valid camera poses will receive a precise pose matching to the rotational motion.
		 * @param database The database providing already known locations of 3D object points (may not match with a sole rotational camera motion), already known valid camera poses (may also not match with a sole rotational camera motion)
		 * @param pinholeCamera The pinhole camera profile defined the projection
		 * @param randomGenerator Random generator object
		 * @param lowerFrame The index of the frame defining the lower border of camera poses which will be investigated
		 * @param upperFrame The index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized, with range [0, infinity)
		 * @param relocatedObjectPointIds Optional resulting ids of all object points which have been relocated
		 * @return True, if succeeded
		 */
		static bool updateDatabaseToRotationalMotion(Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int lowerFrame, const unsigned int upperFrame, const unsigned int minimalObservations, IndexSet32* relocatedObjectPointIds);

		/**
		 * Determines the positions of new object points from a database within a specified frame range.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param database The database from which the object point and image point correspondences are extracted
		 * @param objectPointsData The data holding groups of pose ids and image point ids for each individual object point
		 * @param randomGenerator Random generator object to be used for creating random numbers, must be defined
		 * @param maximalSqrError The maximal squared error between a projected 3D object point and an image point so that the combination of object point and image point count as valid
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param lock The lock object which must be defined if this function is invoked in parallel
		 * @param newObjectPoints The resulting positions of new object points
		 * @param newObjectPointIds The resulting ids of the new object points, each id corresponds with a positions from 'newObjectPoints'
		 * @param firstObjectPoint The first object point to be handled, with range [0, numberObjectPoints)
		 * @param numberObjectPoints The number of object points to be handled, with range [0, objectPointData->size()]
		 */
		static void determineUnknownObjectPointsSubset(const AnyCamera* camera, const Database* database, const Database::PoseImagePointTopologyGroups* objectPointsData, RandomGenerator* randomGenerator, const Scalar maximalSqrError, bool* abort, Lock* lock, Vectors3* newObjectPoints, Indices32* newObjectPointIds, unsigned int firstObjectPoint, unsigned int numberObjectPoints);

		/**
		 * Determines the positions of a subset of (currently unknown) object points.
		 * @param database The database form which the object point, image point and pose information is extracted
		 * @param camera The camera profile defining the projection, must be valid
		 * @param cameraMotion The motion of the camera, can be CM_ROTATIONAL or CM_TRANSLATIONAL
		 * @param objectPointIds The ids of all (currently unknown) object points for which a 3D position will be determined, must all be valid
		 * @param newObjectPoints The resulting 3D location of the new object points
		 * @param newObjectPointIds The ids of the resulting new object points, one id for each resulting new object point
		 * @param newObjectPointObservations Optional resulting number of observations for each resulting new object point, one number for each resulting new object point
		 * @param randomGenerator Random generator object to be used for creating random numbers, must be defined
		 * @param minimalObservations The minimal number of observations for each new object points which are necessary to determine the 3D location
		 * @param useAllObservations True, to use all observations (with valid camera pose) to determine the 3D locations; False, to apply a RANSAC mechanism taking a subset of all observations to determine the 3D locations
		 * @param estimator The robust estimator which is applied during optimization of each individual new 3D location, must be defined
		 * @param ransacMaximalSqrError The maximal squared projection error between a new 3D object point and the corresponding image points for the RANSAC mechanism
		 * @param averageRobustError The (average) robust error for a new 3D object point after optimization of the 3D location
		 * @param maximalSqrError The maximal error for a new valid 3D object point after optimization of the 3D location
		 * @param look Lock object which must be defined if this function is executed in parallel on individual threads
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param firstObjectPoint First object point to be handled
		 * @param numberObjectPoints Number of object points to be handled
		 */
		static void determineUnknownObjectPointsSubset(const Database* database, const AnyCamera* camera, const CameraMotion cameraMotion, const Index32* objectPointIds, Vectors3* newObjectPoints, Indices32* newObjectPointIds, Indices32* newObjectPointObservations, RandomGenerator* randomGenerator, const unsigned int minimalObservations, const bool useAllObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar ransacMaximalSqrError, const Scalar averageRobustError, const Scalar maximalSqrError, Lock* look, bool* abort, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints);

		/**
		 * Optimizes a subset of a set of 3D object points which have a quite good accuracy already without optimizing the camera poses concurrently.
		 * The database must hold the valid initial 3D object positions and must hold valid camera poses.<br>
		 * @param database The database from which the initial 3D object point positions and the individual camera poses (in which the object points are visible) are extracted
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param cameraMotion The motion of the camera, CM_ROTATIONAL if the camera poses do not have a translational part, CM_TRANSLATIONAL otherwise
		 * @param objectPointIds The ids of the object points for which the positions will be optimized (all points must have already initial 3D positions)
		 * @param optimizedObjectPoints The resulting positions of the optimized object points
		 * @param optimizedObjectPointIds The ids of the optimized object points, one id for each positions in 'optimizedObjectPoints'
		 * @param minimalObservations The minimal number of observations a 3D object point must have so that the position of the object point will be optimized
		 * @param estimator The robust estimator which is applied to determine the projection error between 3D object point positions and the image points in individual camera frames
		 * @param maximalRobustError The maximal error between a projected object point and the individual image points; beware the error must be defined w.r.t. the selected estimator
		 * @param look Optional lock object ensuring a safe distribution of the computation, must be defined if this function is executed in parallel
		 * @param abort Optional abort statement allowing to stop the execution; True, if the execution has to stop
		 * @param firstObjectPoint First object point to be handled
		 * @param numberObjectPoints The number of object points to be handled
		 */
		static void optimizeObjectPointsWithFixedPosesSubset(const Database* database, const PinholeCamera* pinholeCamera, const CameraMotion cameraMotion, const Index32* objectPointIds, Vectors3* optimizedObjectPoints, Indices32* optimizedObjectPointIds, const unsigned int minimalObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar maximalRobustError, Lock* look, bool* abort, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints);

		/**
		 * Measures the accuracy of a subset of several 3D object points.
		 * @param database The database providing the location of the 3D object points, the camera poses and the image point positions.<br>
		 * @param pinholeCamera The pinhole camera profile which is applied
		 * @param objectPointIds The ids of the object points for which the accuracies will be determined, each object point must be valid
		 * @param accuracyMethhod The method which is applied to determine the accuracy
		 * @param lowerFrame Optional index of the frame defining the lower border of camera poses which will be investigated, -1 if no lower and no upper border is defined
		 * @param upperFrame Optional index of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity), -1 if also 'lowerFrame' is -1
		 * @param values The resulting accuracy parameters depending on the specified method, one parameter of each object point
		 * @param firstObjectPoint First object point to be handled
		 * @param numberObjectPoints The number of object points to be handled
		 * @see measureObjectPointsAccuracy().
		 */
		static void determineObjectPointsAccuracySubset(const Database* database, const PinholeCamera* pinholeCamera, const Index32* objectPointIds, const AccuracyMethod accuracyMethhod, const unsigned int lowerFrame, const unsigned int upperFrame, Scalar* values, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints);

		/**
		 * Determines the maximal squared pixel errors between the projections of a subset of individual 3D object points and their corresponding image points in individual camera frames.
		 * @param database The database from which the camera poses, the object points and the image points are extracted
		 * @param pinholeCamera The pinhole camera profile which is applied
		 * @param objectPointIds The ids of all object points for which the maximal squared pixel errors are determined
		 * @param posesIF The inverted and flipped poses of all camera frames which will be investigated, the poses can be valid or invalid, the first pose is the camera pose for the frame with id 'lowerPoseId'
		 * @param lowerPoseId The id of the first provided pose
		 * @param upperPoseId The id of the last provided pose, thus posesIF must store (upperPoseId - lowerPoseId + 1) poses
		 * @param useDistortionParameters True, to use the distortion parameters of the camera to distort the projected object points
		 * @param minimalSqrErrors Optional resulting minimal squared pixel errors, one error for each given object point id, invalid object points or object points without corresponding observation receive Numeric::maxValue() as error
		 * @param averagedSqrErrors Optional resulting averaged pixel errors, one error for each given object point id, invalid object points or object points without corresponding observation receive Numeric::maxValue() as error
		 * @param maximalSqrErrors Optional resulting maximal squared pixel errors, one error for each given object point id, invalid object points or object points without corresponding observation receive Numeric::maxValue() as error
		 * @param observations Optional resulting observations for each object point, one number of observations for each given object point id
		 * @param firstObjectPoint The first object point to handle
		 * @param numberObjectPoints The number of object points to handle
		 */
		static void determineProjectionErrorsSubset(const Database* database, const PinholeCamera* pinholeCamera, const Index32* objectPointIds, const HomogenousMatrix4* posesIF, const Index32 lowerPoseId, const unsigned int upperPoseId, const bool useDistortionParameters, Scalar* minimalSqrErrors, Scalar* averagedSqrErrors, Scalar* maximalSqrErrors, unsigned int* observations, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints);

		/**
		 * Determines the average distance between the center of a set of given points and each of the points.
		 * @param points The set of points for which the average distance will be determined
		 * @param size The number of points in the set, with range [1, infinity)
		 * @return The average distance
		 */
		static Scalar averagePointDistance(const Vector2* points, const size_t size);
};

inline Solver3::RelativeThreshold::RelativeThreshold(const unsigned int lowerBoundary, const Scalar factor, const unsigned int upperBoundary) :
	thresholdLowerBoundary(lowerBoundary),
	thresholdFactor(factor),
	thresholdUpperBoundary(upperBoundary)
{
	// nothing to do here
}

inline unsigned int Solver3::RelativeThreshold::lowerBoundary() const
{
	return thresholdLowerBoundary;
}

inline Scalar Solver3::RelativeThreshold::factor() const
{
	return thresholdFactor;
}

inline unsigned int Solver3::RelativeThreshold::upperBoundary() const
{
	return thresholdUpperBoundary;
}

inline unsigned int Solver3::RelativeThreshold::threshold(const unsigned int value) const
{
	return min(minmax<unsigned int>(thresholdLowerBoundary, (unsigned int)Numeric::round32(Scalar(value) * thresholdFactor), thresholdUpperBoundary), value);
}

inline bool Solver3::RelativeThreshold::hasValidThreshold(const unsigned int value, unsigned int* threshold) const
{
	const unsigned int result = min(minmax<unsigned int>(thresholdLowerBoundary, (unsigned int)Numeric::round32(Scalar(value) * thresholdFactor), thresholdUpperBoundary), value);

	if (value < thresholdLowerBoundary)
		return false;

	ocean_assert(result <= value);
	ocean_assert(result >= thresholdLowerBoundary);
	ocean_assert(result <= thresholdUpperBoundary);

	if (threshold)
		*threshold = result;

	return true;
}

template <unsigned int tLowerBoundary>
inline bool Solver3::RelativeThreshold::hasValidThreshold(const unsigned int value, unsigned int* threshold) const
{
	const unsigned int result = min(minmax<unsigned int>(max(thresholdLowerBoundary, tLowerBoundary), (unsigned int)Numeric::round32(Scalar(value) * thresholdFactor), thresholdUpperBoundary), value);

	if (value < tLowerBoundary || value < thresholdLowerBoundary)
		return false;

	ocean_assert(result <= value);
	ocean_assert(result >= thresholdLowerBoundary);
	ocean_assert(result <= thresholdUpperBoundary);

	if (threshold)
		*threshold = result;

	return true;
}

inline Solver3::PoseToObjectPointIdImagePointCorrespondenceAccessor::PoseToObjectPointIdImagePointCorrespondenceAccessor(const PoseToObjectPointIdImagePointCorrespondenceAccessor& accessor) :
	PoseGroupsAccessor(accessor.elementGroups_)
{
	// nothing to do here
}

inline Solver3::PoseToObjectPointIdImagePointCorrespondenceAccessor::PoseToObjectPointIdImagePointCorrespondenceAccessor(PoseToObjectPointIdImagePointCorrespondenceAccessor&& accessor) noexcept :
	PoseGroupsAccessor(std::move(accessor.elementGroups_))
{
	// nothing to do here
}

inline Solver3::PoseToObjectPointIdImagePointCorrespondenceAccessor& Solver3::PoseToObjectPointIdImagePointCorrespondenceAccessor::operator=(const PoseToObjectPointIdImagePointCorrespondenceAccessor& accessor)
{
	PoseGroupsAccessor::operator=(accessor);
	return *this;
}

inline Solver3::PoseToObjectPointIdImagePointCorrespondenceAccessor& Solver3::PoseToObjectPointIdImagePointCorrespondenceAccessor::operator=(PoseToObjectPointIdImagePointCorrespondenceAccessor&& accessor) noexcept
{
	PoseGroupsAccessor::operator=(std::move(accessor));
	return *this;
}

inline Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor::ObjectPointToPoseImagePointCorrespondenceAccessor(const ObjectPointToPoseImagePointCorrespondenceAccessor& accessor) :
	ObjectPointGroupsAccessor(accessor.elementGroups_)
{
	// nothing to do here
}

inline Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor::ObjectPointToPoseImagePointCorrespondenceAccessor(ObjectPointToPoseImagePointCorrespondenceAccessor&& accessor) noexcept :
	ObjectPointGroupsAccessor(std::move(accessor.elementGroups_))
{
	// nothing to do here
}

inline Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor::ObjectPointToPoseImagePointCorrespondenceAccessor(ObjectPointToPoseImagePointCorrespondenceAccessor&& accessor, const Indices32& validGroupIndices)
{
	elementGroups_.reserve(validGroupIndices.size());

	for (Indices32::const_iterator i = validGroupIndices.begin(); i != validGroupIndices.end(); ++i)
	{
		ocean_assert(*i < accessor.elementGroups_.size());
		elementGroups_.emplace_back(std::move(accessor.elementGroups_[*i]));
	}
}

inline Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor& Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor::operator=(const ObjectPointToPoseImagePointCorrespondenceAccessor& accessor)
{
	ObjectPointGroupsAccessor::operator=(accessor);
	return *this;
}

inline Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor& Solver3::ObjectPointToPoseImagePointCorrespondenceAccessor::operator=(ObjectPointToPoseImagePointCorrespondenceAccessor&& accessor) noexcept
{
	ObjectPointGroupsAccessor::operator=(std::move(accessor));
	return *this;
}

inline Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(const ObjectPointToPoseIndexImagePointCorrespondenceAccessor& accessor) :
	ObjectPointGroupsAccessor(accessor.elementGroups_)
{
	// nothing to do here
}

inline Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::ObjectPointToPoseIndexImagePointCorrespondenceAccessor(ObjectPointToPoseIndexImagePointCorrespondenceAccessor&& accessor) noexcept :
	ObjectPointGroupsAccessor(std::move(accessor.elementGroups_))
{
	// nothing to do here
}

inline Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor& Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::operator=(const ObjectPointToPoseIndexImagePointCorrespondenceAccessor& accessor)
{
	ObjectPointGroupsAccessor::operator=(accessor);
	return *this;
}

inline Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor& Solver3::ObjectPointToPoseIndexImagePointCorrespondenceAccessor::operator=(ObjectPointToPoseIndexImagePointCorrespondenceAccessor&& accessor) noexcept
{
	ObjectPointGroupsAccessor::operator=(std::move(accessor));
	return *this;
}

inline bool Solver3::determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, Vectors3& newObjectPoints, Indices32& newObjectPointIds, RandomGenerator& randomGenerator, Indices32* newObjectPointObservations, const Scalar minimalObjectPointPriority, const unsigned int minimalObservations, const bool useAllObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar ransacMaximalSqrError, const Scalar averageRobustError, const Scalar maximalSqrError, Worker* worker, bool* abort)
{
	ocean_assert(cameraMotion != CM_INVALID);

	const Indices32 invalidObjectPointIds = database.objectPointIds<false, true>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), nullptr, minimalObjectPointPriority);

	return determineUnknownObjectPoints(database, camera, cameraMotion, invalidObjectPointIds, newObjectPoints, newObjectPointIds, randomGenerator, newObjectPointObservations, minimalObservations, useAllObservations, estimator, ransacMaximalSqrError, averageRobustError, maximalSqrError, worker, abort);
}

template <bool tVisibleInAllPoses>
inline bool Solver3::determineUnknownObjectPoints(const Database& database, const AnyCamera& camera, const CameraMotion cameraMotion, const Index32 lowerPoseId, const Index32 upperPoseId, Vectors3& newObjectPoints, Indices32& newObjectPointIds, RandomGenerator& randomGenerator, Indices32* newObjectPointObservations, const Scalar minimalObjectPointPriority, const unsigned int minimalObservations, const bool useAllObservations, const Geometry::Estimator::EstimatorType estimator, const Scalar ransacMaximalSqrError, const Scalar averageRobustError, const Scalar maximalSqrError, Worker* worker, bool* abort)
{
	ocean_assert(cameraMotion != CM_INVALID);
	ocean_assert(lowerPoseId <= upperPoseId);

	const Indices32 invalidObjectPointIds = database.objectPointIds<false, true, tVisibleInAllPoses>(lowerPoseId, upperPoseId, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), minimalObjectPointPriority);

	return determineUnknownObjectPoints(database, camera, cameraMotion, invalidObjectPointIds, newObjectPoints, newObjectPointIds, randomGenerator, newObjectPointObservations, minimalObservations, useAllObservations, estimator, ransacMaximalSqrError, averageRobustError, maximalSqrError, worker, abort);
}

inline HomogenousMatrix4 Solver3::determinePose(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const HomogenousMatrix4& roughPose, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError, unsigned int* correspondences)
{
	ocean_assert(camera.isValid());

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	database.imagePointsObjectPoints<false, false>(frameId, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
	ocean_assert(imagePoints.size() == objectPoints.size());

	if (correspondences != nullptr)
	{
		*correspondences = (unsigned int)imagePoints.size();
	}

	// check whether enough points correspondences could be found
	if (imagePoints.size() < minimalCorrespondences)
	{
		return HomogenousMatrix4(false);
	}

	return determinePose(camera, randomGenerator, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), roughPose, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
}

inline HomogenousMatrix4 Solver3::determinePose(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const IndexSet32& priorityObjectPointIds, const bool solePriorityPoints, const HomogenousMatrix4& roughPose, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError, unsigned int* correspondences)
{
	ocean_assert(camera.isValid());

	ocean_assert(!priorityObjectPointIds.empty());

	Vectors2 priorityImagePoints, remainingImagePoints;
	Vectors3 priorityObjectPoints, remainingObjectPoints;
	database.imagePointsObjectPoints<false, false>(frameId, priorityObjectPointIds, priorityImagePoints, priorityObjectPoints, remainingImagePoints, remainingObjectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
	ocean_assert(priorityImagePoints.size() == priorityObjectPoints.size());
	ocean_assert(remainingImagePoints.size() == remainingObjectPoints.size());

	if (solePriorityPoints)
	{
		if (correspondences != nullptr)
		{
			*correspondences = (unsigned int)priorityImagePoints.size();
		}

		if (priorityImagePoints.size() < minimalCorrespondences)
		{
			return HomogenousMatrix4(false);
		}

		return determinePose(camera, randomGenerator, ConstArrayAccessor<Vector3>(priorityObjectPoints), ConstArrayAccessor<Vector2>(priorityImagePoints), roughPose, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
	}
	else
	{
		if (correspondences != nullptr)
		{
			*correspondences = (unsigned int)(priorityImagePoints.size() + remainingImagePoints.size());
		}

		// check whether enough points correspondences could be found
		if (priorityImagePoints.size() + remainingImagePoints.size() < minimalCorrespondences)
		{
			return HomogenousMatrix4(false);
		}

		const size_t priorityCorrespondences = priorityImagePoints.size();

		priorityImagePoints.insert(priorityImagePoints.end(), remainingImagePoints.begin(), remainingImagePoints.end());
		priorityObjectPoints.insert(priorityObjectPoints.end(), remainingObjectPoints.begin(), remainingObjectPoints.end());

		return determinePose(camera, randomGenerator, ConstArrayAccessor<Vector3>(priorityObjectPoints), ConstArrayAccessor<Vector2>(priorityImagePoints), priorityCorrespondences, roughPose, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
	}
}

inline HomogenousMatrix4 Solver3::determinePose(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<Index32>& objectPointIds, const HomogenousMatrix4& roughPose, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError)
{
	const ScopedConstMemoryAccessor<Index32> scopedObjectPointIdMemoryAccessor(objectPointIds);

	Indices32 validIndices;
	const Vectors2 imagePoints = database.imagePointsFromObjectPoints<false>(frameId, scopedObjectPointIdMemoryAccessor.data(), scopedObjectPointIdMemoryAccessor.size(), validIndices);
	ocean_assert(scopedObjectPointIdMemoryAccessor.size() == validIndices.size());

	return determinePose(camera, randomGenerator, objectPoints, ConstArrayAccessor<ImagePoint>(imagePoints), roughPose, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
}

inline HomogenousMatrix4 Solver3::determinePose(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const HomogenousMatrix4& roughPose, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError, Indices32* validIndices)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(minimalValidCorrespondenceRatio >= 0 && minimalValidCorrespondenceRatio <= 1);

	HomogenousMatrix4 previousPose(roughPose);

	// **TODO** as now the p3p works fine, and as we have the refinement step we can skip the explicit refinement if p3p is used

	Indices32 internalValidIndices;
	if (!previousPose.isValid() || minimalValidCorrespondenceRatio < 1)
	{
		Geometry::RANSAC::p3p(camera, objectPoints, imagePoints, randomGenerator, previousPose, 5u, true, 50u, maximalSqrError, &internalValidIndices);
	}

	// check whether we did not receive enough valid correspondences from the RANSAC, however if the difference is 2 we accept the pose as in this case the ratio may provide wrong results
	if (minimalValidCorrespondenceRatio < 1 && Scalar(internalValidIndices.size()) < Scalar(objectPoints.size()) * minimalValidCorrespondenceRatio && objectPoints.size() - internalValidIndices.size() > 2)
	{
		return HomogenousMatrix4(false);
	}

	HomogenousMatrix4 currentPose(false);
	if (previousPose.isValid())
	{
		if (minimalValidCorrespondenceRatio < 1 && internalValidIndices.size() != objectPoints.size())
		{
			Geometry::NonLinearOptimizationPose::optimizePose(camera, previousPose, ConstIndexedAccessorSubsetAccessor<Vector3, Index32>(objectPoints, internalValidIndices), ConstIndexedAccessorSubsetAccessor<Vector2, Index32>(imagePoints, internalValidIndices),currentPose, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError);

			if (validIndices)
			{
				*validIndices = std::move(internalValidIndices);
			}
		}
		else
		{
			Geometry::NonLinearOptimizationPose::optimizePose(camera, previousPose, objectPoints, imagePoints, currentPose, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError);

			if (validIndices)
			{
				*validIndices = createIndices(objectPoints.size(), 0u);
			}
		}
	}

	return currentPose;
}

inline HomogenousMatrix4 Solver3::determinePose(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const size_t priorityCorrespondences, const HomogenousMatrix4& roughPose, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(minimalValidCorrespondenceRatio >= 0 && minimalValidCorrespondenceRatio <= 1);

	HomogenousMatrix4 previousPose(roughPose);

	// **TODO** as now the p3p works fine, and as we have the refinement step we can skip the explicit refinement if p3p is used

	Indices32 validIndices;
	if (!previousPose.isValid() || minimalValidCorrespondenceRatio < 1)
	{
		Geometry::RANSAC::p3p(camera, objectPoints, imagePoints, randomGenerator, previousPose, 5u, true, 50u, maximalSqrError, &validIndices);
	}

	// check whether we did not receive enough valid correspondences from the RANSAC, however if the difference is 2 we accept the pose as in this case the ratio may provide wrong results
	if (minimalValidCorrespondenceRatio < 1 && Scalar(validIndices.size()) < Scalar(objectPoints.size()) * minimalValidCorrespondenceRatio && objectPoints.size() - validIndices.size() > 2)
	{
		return HomogenousMatrix4(false);
	}

	// for priority image points we take a sigma of 1
	// for remaining image points we take a sigma identical to the number of priority values (at least 1 and at most 10)

	const SquareMatrix2 priorityInvertedCovariance(1, 0, 0, 1);

	const Scalar sigmaRemaining = minmax(Scalar(1), Scalar(priorityCorrespondences), Scalar(10));
	const Scalar sigmaRemainingInvSqr(1 / (sigmaRemaining * sigmaRemaining));
	const SquareMatrix2 remainingInvertedCovariance(sigmaRemainingInvSqr, 0, 0, sigmaRemainingInvSqr);

	ocean_assert(priorityInvertedCovariance == Geometry::Utilities::covarianceMatrix(Vector2(1, 0), 1, Vector2(0, 1), 1).inverted());
	ocean_assert(remainingInvertedCovariance == Geometry::Utilities::covarianceMatrix(Vector2(1, 0), sigmaRemaining, Vector2(0, 1), sigmaRemaining).inverted());

	HomogenousMatrix4 currentPose(false);
	if (previousPose.isValid())
	{
		if (minimalValidCorrespondenceRatio < 1 && validIndices.size() != objectPoints.size())
		{
			Matrix invertedCovariances(validIndices.size(), 2);

			for (size_t n = 0; n < validIndices.size(); ++n)
			{
				const Index32 index = validIndices[n];

				if (index < priorityCorrespondences)
				{
					priorityInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
				}
				else
				{
					remainingInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
				}
			}

			Geometry::NonLinearOptimizationPose::optimizePose(camera, previousPose, ConstIndexedAccessorSubsetAccessor<Vector3, Index32>(objectPoints, validIndices), ConstIndexedAccessorSubsetAccessor<Vector2, Index32>(imagePoints, validIndices), currentPose, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError, &invertedCovariances);
		}
		else
		{
			Matrix invertedCovariances(2 * objectPoints.size(), 2);

			for (size_t n = 0; n < priorityCorrespondences; ++n)
			{
				priorityInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
			}

			for (size_t n = priorityCorrespondences; n < objectPoints.size(); ++n)
			{
				remainingInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
			}

			Geometry::NonLinearOptimizationPose::optimizePose(camera, previousPose, objectPoints, imagePoints, currentPose, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError, &invertedCovariances);
		}
	}

	return currentPose;
}

inline SquareMatrix3 Solver3::determineOrientation(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const SquareMatrix3& roughOrientation, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError, unsigned int* correspondences)
{
	ocean_assert(camera.isValid());

	Vectors2 imagePoints;
	Vectors3 objectPoints;
	database.imagePointsObjectPoints<false, false>(frameId, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
	ocean_assert(imagePoints.size() == objectPoints.size());

	if (correspondences != nullptr)
	{
		*correspondences = (unsigned int)imagePoints.size();
	}

	// check whether enough points correspondences could be found
	if (imagePoints.size() < minimalCorrespondences)
	{
		return SquareMatrix3(false);
	}

	return determineOrientation(camera, randomGenerator, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(imagePoints), roughOrientation, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
}

inline SquareMatrix3 Solver3::determineOrientation(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const IndexSet32& priorityObjectPointIds, const bool solePriorityPoints, const SquareMatrix3& roughOrientation, const unsigned int minimalCorrespondences, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError, unsigned int* correspondences)
{
	ocean_assert(camera.isValid());
	ocean_assert(!priorityObjectPointIds.empty());

	Vectors2 priorityImagePoints, remainingImagePoints;
	Vectors3 priorityObjectPoints, remainingObjectPoints;
	database.imagePointsObjectPoints<false, false>(frameId, priorityObjectPointIds, priorityImagePoints, priorityObjectPoints, remainingImagePoints, remainingObjectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));
	ocean_assert(priorityImagePoints.size() == priorityObjectPoints.size());
	ocean_assert(remainingImagePoints.size() == remainingObjectPoints.size());

	if (solePriorityPoints)
	{
		if (correspondences != nullptr)
		{
			*correspondences = (unsigned int)priorityImagePoints.size();
		}

		// check whether enough points correspondences could be found
		if (priorityImagePoints.size() < minimalCorrespondences)
		{
			return SquareMatrix3(false);
		}

		return determineOrientation(camera, randomGenerator, ConstArrayAccessor<Vector3>(priorityObjectPoints), ConstArrayAccessor<Vector2>(priorityImagePoints), roughOrientation, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
	}
	else
	{
		if (correspondences != nullptr)
		{
			*correspondences = (unsigned int)(priorityImagePoints.size() + remainingImagePoints.size());
		}

		// check whether enough points correspondences could be found
		if (priorityImagePoints.size() + remainingImagePoints.size() < minimalCorrespondences)
		{
			return SquareMatrix3(false);
		}

		const size_t priorityCorrespondences = priorityImagePoints.size();

		priorityImagePoints.insert(priorityImagePoints.end(), remainingImagePoints.begin(), remainingImagePoints.end());
		priorityObjectPoints.insert(priorityObjectPoints.end(), remainingObjectPoints.begin(), remainingObjectPoints.end());

		return determineOrientation(camera, randomGenerator, ConstArrayAccessor<Vector3>(priorityObjectPoints), ConstArrayAccessor<Vector2>(priorityImagePoints), priorityCorrespondences, roughOrientation, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
	}
}

inline SquareMatrix3 Solver3::determineOrientation(const Database& database, const AnyCamera& camera, RandomGenerator& randomGenerator, const unsigned int frameId, const ObjectPoint* objectPoints, const Index32* objectPointIds, const size_t numberObjectPoints, const SquareMatrix3& roughOrientation, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError)
{
	ocean_assert(camera.isValid());

	Indices32 validIndices;
	const Vectors2 imagePoints = database.imagePointsFromObjectPoints<false>(frameId, objectPointIds, numberObjectPoints, validIndices);
	ocean_assert(numberObjectPoints == validIndices.size());

	return determineOrientation(camera, randomGenerator, ConstArrayAccessor<Vector3>(objectPoints, numberObjectPoints), ConstArrayAccessor<Vector2>(imagePoints), roughOrientation, estimator, minimalValidCorrespondenceRatio, maximalSqrError, finalRobustError);
}

inline SquareMatrix3 Solver3::determineOrientation(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const SquareMatrix3& roughOrientation, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError, Indices32* validIndices)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(minimalValidCorrespondenceRatio >= 0 && minimalValidCorrespondenceRatio <= 1);

	SquareMatrix3 previousOrientation(roughOrientation);

	Indices32 internalValidIndices;
	if (previousOrientation.isNull() || minimalValidCorrespondenceRatio < 1)
	{
		Geometry::RANSAC::orientation(camera, objectPoints, imagePoints, randomGenerator, previousOrientation, 5u, 50u, maximalSqrError, nullptr, &internalValidIndices);
	}

	// check whether we do not receive enough valid correspondences from the RANSAC, however if the difference is 2 we accept the pose as in this case the ratio may provide wrong results
	if (minimalValidCorrespondenceRatio < 1 && Scalar(internalValidIndices.size()) < Scalar(objectPoints.size()) * minimalValidCorrespondenceRatio && objectPoints.size() - internalValidIndices.size() > 2)
	{
		return SquareMatrix3(false);
	}

	SquareMatrix3 currentOrientation(false);
	if (!previousOrientation.isNull())
	{
		if (minimalValidCorrespondenceRatio < 1)
		{
			Geometry::NonLinearOptimizationOrientation::optimizeOrientation(camera, previousOrientation, ConstIndexedAccessorSubsetAccessor<Vector3, unsigned int>(objectPoints, internalValidIndices), ConstIndexedAccessorSubsetAccessor<Vector2, unsigned int>(imagePoints, internalValidIndices), currentOrientation, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError);

			if (validIndices != nullptr)
			{
				*validIndices = std::move(internalValidIndices);
			}
		}
		else
		{
			Geometry::NonLinearOptimizationOrientation::optimizeOrientation(camera, previousOrientation, objectPoints, imagePoints, currentOrientation, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError);

			if (validIndices != nullptr)
			{
				*validIndices = createIndices(objectPoints.size(), 0u);
			}
		}
	}

	return currentOrientation;
}

inline SquareMatrix3 Solver3::determineOrientation(const AnyCamera& camera, RandomGenerator& randomGenerator, const ConstIndexedAccessor<ObjectPoint>& objectPoints, const ConstIndexedAccessor<ImagePoint>& imagePoints, const size_t priorityCorrespondences, const SquareMatrix3& roughOrientation, const Geometry::Estimator::EstimatorType estimator, const Scalar minimalValidCorrespondenceRatio, const Scalar maximalSqrError, Scalar* finalRobustError)
{
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints.size() == imagePoints.size());
	ocean_assert(minimalValidCorrespondenceRatio >= 0 && minimalValidCorrespondenceRatio <= 1);

	SquareMatrix3 previousOrientation(roughOrientation);

	Indices32 validIndices;
	if (previousOrientation.isNull() || minimalValidCorrespondenceRatio < 1)
	{
		Geometry::RANSAC::orientation(camera, objectPoints, imagePoints, randomGenerator, previousOrientation, 5u, 50u, maximalSqrError, nullptr, &validIndices);
	}

	// check whether we do not receive enough valid correspondences from the RANSAC, however if the difference is 2 we accept the pose as in this case the ratio may provide wrong results
	if (minimalValidCorrespondenceRatio < 1 && Scalar(validIndices.size()) < Scalar(objectPoints.size()) * minimalValidCorrespondenceRatio && objectPoints.size() - validIndices.size() > 2)
	{
		return SquareMatrix3(false);
	}

	// for priority image points we take a sigma of 1
	// for remaining image points we take a sigma identical to the number of priority values (at least 1 and at most 10)

	const SquareMatrix2 priorityInvertedCovariance(1, 0, 0, 1);

	const Scalar sigmaRemaining = minmax(Scalar(1), Scalar(priorityCorrespondences), Scalar(10));
	const Scalar sigmaRemainingInvSqr(1 / (sigmaRemaining * sigmaRemaining));
	const SquareMatrix2 remainingInvertedCovariance(sigmaRemainingInvSqr, 0, 0, sigmaRemainingInvSqr);

	ocean_assert(priorityInvertedCovariance == Geometry::Utilities::covarianceMatrix(Vector2(1, 0), 1, Vector2(0, 1), 1).inverted());
	ocean_assert(remainingInvertedCovariance == Geometry::Utilities::covarianceMatrix(Vector2(1, 0), sigmaRemaining, Vector2(0, 1), sigmaRemaining).inverted());

	SquareMatrix3 currentOrientation(false);
	if (!previousOrientation.isNull())
	{
		if (minimalValidCorrespondenceRatio < 1)
		{
			Vectors3 subsetObjectPoints;
			Vectors2 subsetImagePoints;

			subsetObjectPoints.reserve(validIndices.size());
			subsetImagePoints.reserve(validIndices.size());

			Matrix invertedCovariances(validIndices.size(), 2);

			for (size_t n = 0; n < validIndices.size(); ++n)
			{
				const Index32 index = validIndices[n];

				if (index < priorityCorrespondences)
				{
					priorityInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
				}
				else
				{
					remainingInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
				}

				subsetObjectPoints.push_back(objectPoints[index]);
				subsetImagePoints.push_back(imagePoints[index]);
			}

			Geometry::NonLinearOptimizationOrientation::optimizeOrientation(camera, previousOrientation, ConstArrayAccessor<Vector3>(subsetObjectPoints), ConstArrayAccessor<Vector2>(subsetImagePoints), currentOrientation, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError, &invertedCovariances);
		}
		else
		{
			Matrix invertedCovariances(2 * objectPoints.size(), 2);

			for (size_t n = 0; n < priorityCorrespondences; ++n)
			{
				priorityInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
			}

			for (size_t n = priorityCorrespondences; n < objectPoints.size(); ++n)
			{
				remainingInvertedCovariance.copyElements(invertedCovariances[2 * n], false);
			}

			Geometry::NonLinearOptimizationOrientation::optimizeOrientation(camera, previousOrientation, objectPoints, imagePoints, currentOrientation, 20u, estimator, Scalar(0.001), Scalar(5), nullptr, finalRobustError, &invertedCovariances);
		}
	}

	return currentOrientation;
}

inline bool Solver3::determinePlane(const ConstIndexedAccessor<Vector3>& objectPoints, RandomGenerator& randomGenerator, Plane3& plane, const RelativeThreshold& minimalValidObjectPoints, const Geometry::Estimator::EstimatorType estimator, Scalar* finalError, Indices32* validIndices)
{
	ocean_assert(objectPoints.size() >= 3);
	return Geometry::RANSAC::plane(objectPoints, randomGenerator, plane, 100u, Scalar(0.1), minimalValidObjectPoints.threshold((unsigned int)objectPoints.size()), estimator, finalError, validIndices);
}

inline bool Solver3::determinePlane(const Database& database, const Indices32& objectPointIds, RandomGenerator& randomGenerator, Plane3& plane, const RelativeThreshold& minimalValidObjectPoints, const Geometry::Estimator::EstimatorType estimator, Scalar* finalError, Indices32* validIndices)
{
	ocean_assert(objectPointIds.size() >= 3);

	return determinePlane(ConstArrayAccessor<Vector3>(database.objectPoints<false>(objectPointIds)), randomGenerator, plane, minimalValidObjectPoints, estimator, finalError, validIndices);
}

}

}

#endif // META_OCEAN_TRACKING_SOLVER_3_H
