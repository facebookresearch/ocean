/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef  META_OCEAN_PLATFORM_META_QUEST_OPENXR_HAND_GESTURES_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_HAND_GESTURES_H

#include "ocean/platform/meta/quest/openxr/OpenXR.h"
#include "ocean/platform/meta/quest/openxr/HandPoses.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

/**
 * This class implements hand gestures functionalities.
 * @ingroup platformmetaquestopenxr
 */
class HandGestures
{
	public:

		/**
		 * Determines the minimal and maximal angle between consecutive bones.
		 * @param jointPoints The 3D locations of the hand joints, at least 3
		 * @param jointIndices The indices of consecutive hand joints (e.g., of a finger), two indices define a bone, at least 3
		 * @param minAngle Optional resulting minimal angle between all consecutive bones, in radian, with range [0, maxAngle]
		 * @param maxAngle Optional resulting maximal angle between all consecutive bones, in radian, with range [minAngle, PI]
		 * @param medianAngle Optional resulting median angle between all consecutive bones, in radian, with range [minAngle, maxAngle]
		 * @return True, if succeeded
		 */
		static bool determineBoneAngles(const Vectors3& jointPoints, const Indices32& jointIndices, Scalar* minAngle = nullptr, Scalar* maxAngle = nullptr, Scalar* medianAngle = nullptr);

		/**
		 * Determines the distance between two finger joints of a hand.
		 * @param jointPoints The 3D locations of the hand joints, at least 3
		 * @param joint0 The index of the first joint, with range [0, jointPoints.size() - 1]
		 * @param joint1 The index of the second joint, with range [0, jointPoints.size() - 1]
		 * @param position0 Optional The resulting position of the first joint, nullptr if not of interest
		 * @param position1 Optional The resulting position of the second joint, nullptr if not of interest
		 * @return The resulting distance, Numeric::maxValue() otherwise
		 */
		static Scalar distanceBetweenFingerJoints(const Vectors3& jointPoints, const Index32 joint0, const Index32 joint1, Vector3* position0 = nullptr, Vector3* position1 = nullptr);

		/**
		 * Returns whether a hand is pinching (whether thumb and index finger tips are together).
		 * @param jointPositions The positions of the joints of a hand, defined in world, either XR_HAND_JOINT_COUNT_EXT positions (or zero to fail)
		 * @param position Optional resulting pinching position (position between thumb and index finger tip, in world), nullptr of not of interest
		 * @param maxDistance The maximal distance between both finger tips to count as pinching, in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandPinching(const Vectors3& jointPositions, Vector3* position = nullptr, const Scalar maxDistance = Scalar(0.025));

		/**
		 * Returns whether a hand is making a 'tunnel' gesture (whether thumb and index finger tips are together and forming a round tunnel with thumb and index finger).
		 * @param jointPositions The positions of the joints of a hand, defined in world, either XR_HAND_JOINT_COUNT_EXT positions (or zero to fail)
		 * @param isLeft True, if the hand is a left hand; False, if the hand is a right hand
		 * @param world_T_tunnelCenter The resulting transformation between the tunnel center and the world, with x-axis to the right (parallel to the line between the index knuckle and thumb tip), and z-axis outside the tunnel (towards the user)
		 * @param maxTipDistance The maximal distance between both finger tips to count as pinching, in meter, with range [0, infinity)
		 * @param minTunnelDistance The minimal distance between opposite tunnel bones (to ensure a round tunnel), in meters, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandTunneling(const Vectors3& jointPositions, const bool isLeft, HomogenousMatrix4& world_T_tunnelCenter, const Scalar maxTipDistance = Scalar(0.025), const Scalar minTunnelDistance = Scalar(0.05));

		/**
		 * Returns whether a hand is making a pointing gesture with the index finger.
		 * @param jointPositions The positions of the joints of a hand, defined in world, either XR_HAND_JOINT_COUNT_EXT positions (or zero to fail)
		 * @param worldRay The resulting pointing ray along the index finger, in world
		 * @param maxIndexAngle The maximal angle between bones of the index finger, in radian, with range [0, PI)
		 * @param minAngle The minimal angle between bones in all other fingers, in radian, with range [0, PI)
		 * @param minTipDistance The minimal distance between index fingertip and all other fingertips, in meter, with range [0, infinity)
		 * @param maxTipDistance The maximal distance between fingertips (not including the index finger), in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandPointing(const Vectors3& jointPositions, Line3& worldRay, const Scalar maxIndexAngle = Numeric::deg2rad(15), const Scalar minAngle = Numeric::deg2rad(35), const Scalar minTipDistance = Scalar(0.03), const Scalar maxTipDistance = Scalar(0.08));

		/**
		 * Returns whether a hand is making a spreading gesture (whether the hand is flat and all fingers are spread away from each other).
		 * @param jointPositions The positions of the joints of a hand, defined in world, either XR_HAND_JOINT_COUNT_EXT positions (or zero to fail)
		 * @param isLeft True, if the hand is a left hand; False, if the hand is a right hand
		 * @param world_T_hand The resulting transformation between hand and world, with origin in the center of the hand, with y-axis along the middle finger, with z-axis upwards away from the hand, and x-axis to the right
		 * @param maxAngle The maximal angle between bones of the fingers, in radian, with range [0, PI)
		 * @param minTipDistance The minimal distance between all fingertips, in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandSpreading(const Vectors3& jointPositions, const bool isLeft, HomogenousMatrix4& world_T_hand, const Scalar maxAngle = Numeric::deg2rad(20), const Scalar minTipDistance = Scalar(0.025));

		/**
		 * Returns whether a hand is making a grab gesture (whether the hand is grabbing a ball like object).
		 * @param jointPositions The positions of the joints of a hand, defined in world, either XR_HAND_JOINT_COUNT_EXT positions (or zero to fail)
		 * @param isLeft True, if the hand is a left hand; False, if the hand is a right hand
		 * @param world_T_hand The resulting transformation between hand and world, with origin in the center of the hand, with y-axis along the middle finger, with z-axis upwards away from the hand, and x-axis to the right
		 * @param minAngle The minimal median angle between bones of the fingers, in radian, with range [0, maxAngle)
		 * @param maxAngle The maximal median angle between bones of the fingers, in radian, with range (minAngle, PI)
		 * @param minTipDistance The minimal distance between all fingertips and the thumb, in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandGrabbing(const Vectors3& jointPositions, const bool isLeft, HomogenousMatrix4& world_T_hand, const Scalar minAngle = Numeric::deg2rad(25), const Scalar maxAngle = Numeric::deg2rad(65), const Scalar minTipDistance = Scalar(0.04));
};

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_HAND_GESTURES_H
