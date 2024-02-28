// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_INPUT_H
#define META_OCEAN_PLATFORM_META_AVATARS_INPUT_H

#include "ocean/platform/meta/avatars/Avatars.h"

#include "ocean/math/HomogenousMatrix4.h"

#include <ovrAvatar2/tracking/Body.h>
#include <ovrAvatar2/tracking/LipSync.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * This class provides input containers and functions for Avatars.
 * @ingroup platformmetaavatars
 */
class OCEAN_PLATFORM_META_AVATARS_EXPORT Input
{
	public:

		/**
		 * Definition of a class combining the input states for headset tracking, hand tracking, controller tracking, controller states, and lipsync.
		 */
		class BodyTrackingData
		{
			public:

				/// The transformation between this avatar at the remote headset and the remote headset's world, invalid to hide the local avatar at the remote side.
				HomogenousMatrixF4 remoteHeadsetWorld_T_remoteAvatar_ = HomogenousMatrixF4(false);

				/// The input tracking state.
				ovrAvatar2InputTrackingState avatarInputTrackingState_ = {};

				/// The input control state.
				ovrAvatar2InputControlState avatarInputControlState_ = {};

				/// The hand tracking state.
				ovrAvatar2HandTrackingState avatarHandTrackingState_ = {};

				/// The lip sync state.
				ovrAvatar2LipSyncState avatarLipSyncState_ = {};
		};

		/**
		 * Definition of a shared pointer holding a BodyTrackingData object.
		 */
		typedef std::shared_ptr<BodyTrackingData> SharedBodyTrackingData;

	public:

		/**
		 * Creates a body tracking data object for a local user on non-headset platform.
		 * The resulting body tracking data is mainly based on the user's head pose.
		 * @param world_T_head The transformation between head and world, must be valid
		 * @return The resulting body tracking data object
		 */
		static SharedBodyTrackingData createBodyTrackingData(const HomogenousMatrix4& world_T_head);
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_INPUT_H
