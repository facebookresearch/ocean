// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_HELPERS_H
#define META_OCEAN_MEDIA_VRS_HELPERS_H

#include "ocean/media/vrs/VRS.h"

#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This class implements helper functions for components outside of Media::VRS.
 * The helper class is necessary to avoid any non-Ocean header file dependencies.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT Helpers
{
	public:

		/**
		 * Reads hand tracking data from a VRS file.
		 * The resulting tracking data can be used for e.g., Keyboard Tracking.
		 * @param vrsFile The VRS file from which the data will be read
		 * @param map_world_T_leftHandJoints The poses of the left hand joints
 		 * @param map_world_T_rightHandJoints The poses of the right hand joints
		 * @return True, if the VRS file contained hand tracking data
		 */
		static bool readHandTrackingFromVRS(const std::string& vrsFile, std::map<double, HomogenousMatricesD4>& map_world_T_leftHandJoints, std::map<double, HomogenousMatricesD4>& map_world_T_rightHandJoints);
};

}

}

}

#endif // META_OCEAN_MEDIA_VRS_HELPERS_H
