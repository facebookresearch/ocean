/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_MAP_MERGING_H
#define META_OCEAN_TRACKING_MAPBUILDING_MAP_MERGING_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/DescriptorHandling.h"
#include "ocean/tracking/mapbuilding/Unified.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/tracking/Database.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements merging algorithms and functionalities for maps.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT MapMerging : public DescriptorHandling
{
	public:

		/**
		 * Executes bundle adjustment in a given database.
		 * @param database The database in which the bundle adjustment will be executed
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param randomGenerator The random generator object to be used
		 * @param iterations The number of optimization iterations, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool bundleAdjustment(Database& database, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int iterations);

		/**
		 * Closes the loop(s) in a database and merges all corresponding 3D object points.
		 * @param database The database in which the loops will be closed
		 * @param freakMap The map mapping object points to descriptors
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param minimalNumberValidCorrespondences The minimal number of valid correspondences between 3D object points and (not associated) 2D image points so that the correspondences are considered to be valid and thus a closed loop, with range [3, infinity)
		 * @param maximalNumberOverlappingObjectPointInPosePair The maximal number of 3D object points which can be visible in both frames so that both frames are still considered for loop closing, with range [0, infinity)
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param iterationsWithoutImprovements The number of loop searching iterations without any further loop improvement until the search for further loops stops, with range [1, infinity)
		 * @return The number of merged object points
		 */
		static size_t closeLoops(Database& database, FreakMultiDescriptorMap256& freakMap, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const unsigned int minimalNumberValidCorrespondences, const unsigned int maximalNumberOverlappingObjectPointInPosePair = 50u, const unsigned int maximalDescriptorDistance = 64u, const unsigned int iterationsWithoutImprovements = 100u);

		/**
		 * Merges individual 3D object points in a database.
		 * Object points with are not visible in the same frame (not known to be visible) will be merged if the projection error of both object points are below a threshold.
		 * @param database The database in which the 3D object points will be merged
		 * @param freakMap The map mapping object points to descriptors
		 * @param pinholeCamera The pinhole camera profile to be used, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param maximalProjectionError The maximal projection error between a projected 3D object point and a 2D image point to count as corresponding, in pixel, with range [0, infinity)
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptors are still considered to match, with range [0, infinity)
		 * @param iterationsWithoutImprovements The number of searching iterations without any further improvement until the search for further loops stops, with range [1, infinity)
		 * @return The number of merged object points
		 */
		static size_t mergeObjectPoints(Database& database, FreakMultiDescriptorMap256& freakMap, const PinholeCamera& pinholeCamera, RandomGenerator& randomGenerator, const Scalar maximalProjectionError, const unsigned int maximalDescriptorDistance = 64u, const unsigned int iterationsWithoutImprovements = 100u);

		/**
		 * Merges two maps together.
		 * @param sourceCamera The camera profile used for the source database
		 * @param sourceDatabase The source database which will be merged into the target database
		 * @param sourceDescriptorMap The descriptor map for the source database
		 * @param targetCamera The camera profile used for the target database
		 * @param targetDatabase The target database in which the source database will be merged
		 * @param targetDescriptorMap the descriptor map for the target database
		 * @param randomGenerator The random generator to be used
		 * @param minimalNumberCorrespondingFeaturesPerPose The minimal number of feature correspondences between the source and target database in one frame so that both frames (and their feature points) count as corresponding and will be merged, with range [3, infinity)
		 * @param minimalNumberCorrespondingPoses The minimal number of corresponding poses between source and target database so that a set of candidate correspondences count as valid, with range [1, infinity)
		 * @param iterationsWithoutImprovements The number of searching iterations without any further improvement until the search for further improvements stops, with range [1, infinity)
		 * @param maximalNumberImprovements The maximal number of merging improvements, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool mergeMaps(const PinholeCamera& sourceCamera, const Database& sourceDatabase, const UnifiedDescriptorMap& sourceDescriptorMap, const PinholeCamera& targetCamera, Database& targetDatabase, UnifiedDescriptorMap& targetDescriptorMap, RandomGenerator& randomGenerator, const unsigned int minimalNumberCorrespondingFeaturesPerPose = 50u, const unsigned int minimalNumberCorrespondingPoses = 20u, const unsigned int iterationsWithoutImprovements = 100u, const unsigned int maximalNumberImprovements = (unsigned int)(-1));
};

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_MAP_MERGING_H
