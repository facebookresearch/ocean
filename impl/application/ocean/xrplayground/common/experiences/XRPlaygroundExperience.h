// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_XRPLAYGROUND_EXPERIENCE_H
#define META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_XRPLAYGROUND_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/interaction/experiences/Experience.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line3.h"

#include "ocean/rendering/Scene.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include <VrApi_Input.h>
#endif

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the base class for all experiences in XRPlayground.
 * @ingroup xrplayground
 */
class XRPlaygroundExperience : public Interaction::Experiences::Experience
{
	protected:

		/**
		 * Definition of individual messages.
		 */
		enum MessageType : uint32_t
		{
			/// An invalid message.
			MT_INVALID = 0u,
			/// Message that camera access is necessary but failed.
			MT_CAMERA_ACCESS_FAILED,
			/// Message that the device needes a color cameras.
			MT_COLOR_CAMERA_NEEDED
		};

	public:

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/**
		 * Determines the distance between two finger bones of a hand.
		 * @param left True, to use the left hand; False, to use the right hand
		 * @param bone0 The index of the first bone, with range [0, ovrHandBone_Max - 1]
		 * @param bone1 The index of the second bone, with range [0, ovrHandBone_Max - 1]
		 * @param position0 Optional resulting position of the first bone, in world, nullptr if not of interest
		 * @param position1 Optional resulting position of the second bone, in world, nullptr if not of interest
		 * @return The resulting distance in meter, Numeric::maxValue() otherwise
		 */
		static Scalar distanceBetweenFingerBones(const bool left, const ovrHandBone bone0, const ovrHandBone bone1, Vector3* position0 = nullptr, Vector3* position1 = nullptr);

		/**
		 * Returns whether a hand is pinching (whether thumb and index finger tips are together).
		 * @param left True, to use the left hand; False, to use the right hand
		 * @param position Optional resulting pinching position (position between thumb and index finger tip, in world), nullptr of not of interest
		 * @param maxDistance The maximal distance between both finger tips to count as pinching, in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandPinching(const bool left, Vector3* position = nullptr, const Scalar maxDistance = Scalar(0.025));

		/**
		 * Returns whether a hand is making a 'tunnel' gesture (whether thumb and index finger tips are together and forming a round tunnel with thumb and index finger).
		 * @param left True, to use the left hand; False, to use the right hand
		 * @param world_T_tunnelCenter The resulting transformation between the tunnel center and the world, with x-axis to the right (parallel to the line between the index knuckle and thumb tip), and z-axis outside the tunnel (towards the user)
		 * @param maxTipDistance The maximal distance between both finger tips to count as pinching, in meter, with range [0, infinity)
		 * @param minTunnelDistance The minimal distance between opposite tunnel bones (to ensure a round tunnel), in meters, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandTunneling(const bool left, HomogenousMatrix4& world_T_tunnelCenter, const Scalar maxTipDistance = Scalar(0.025), const Scalar minTunnelDistance = Scalar(0.05));

		/**
		 * Returns whether a hand is making a pointing gesture with the index finger.
		 * @param left True, to use the left hand; False, to use the right hand
		 * @param worldRay The resulting pointing ray along the index finger, in world
		 * @param maxIndexAngle The maximal angle between bones of the index finger, in radian, with range [0, PI)
		 * @param minAngle The minimal angle between bones in all other fingers, in radian, with range [0, PI)
		 * @param minTipDistance The minimal distance between index fingertip and all other fingertips, in meter, with range [0, infinity)
		 * @param maxTipDistance The maximal distance between fingertips (not including the index finger), in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandPointing(const bool left, Line3& worldRay, const Scalar maxIndexAngle = Numeric::deg2rad(15), const Scalar minAngle = Numeric::deg2rad(35), const Scalar minTipDistance = Scalar(0.03), const Scalar maxTipDistance = Scalar(0.08));

		/**
		 * Returns whether a hand is making a spreading gesture (whether the hand is flat and all fingers are spread away from each other).
		 * @param left True, to use the left hand; False, to use the right hand
		 * @param world_T_hand The resulting transformation between hand and world, with origin in the center of the hand, with y-axis along the middle finger, with z-axis upwards away from the hand, and x-axis to the right
		 * @param maxAngle The maximal angle between bones of the fingers, in radian, with range [0, PI)
		 * @param minTipDistance The minimal distance between all fingertips, in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandSpreading(const bool left, HomogenousMatrix4& world_T_hand, const Scalar maxAngle = Numeric::deg2rad(20), const Scalar minTipDistance = Scalar(0.025));

		/**
		 * Returns whether a hand is making a grab gesture (whether the hand is grabbing a ball like object).
		 * @param left True, to use the left hand; False, to use the right hand
		 * @param world_T_hand The resulting transformation between hand and world, with origin in the center of the hand, with y-axis along the middle finger, with z-axis upwards away from the hand, and x-axis to the right
		 * @param minAngle The minimal median angle between bones of the fingers, in radian, with range [0, maxAngle)
		 * @param maxAngle The maximal median angle between bones of the fingers, in radian, with range (minAngle, PI)
		 * @param minTipDistance The minimal distance between all fingertips and the thumb, in meter, with range [0, infinity)
		 * @return True, if so
		 */
		static bool isHandGrabbing(const bool left, HomogenousMatrix4& world_T_hand, const Scalar minAngle = Numeric::deg2rad(25), const Scalar maxAngle = Numeric::deg2rad(65), const Scalar minTipDistance = Scalar(0.04));

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	protected:

		/**
		 * Event function before the experience is loaded.
		 * @see Experience::preLoad().
		 */
		bool preLoad(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Event function after the experience is unloaded.
		 * @see Experience::postUnload().
		 */
		bool postUnload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Returns the rendering Scene object holding this experience.
		 * @return The experience's rendering Scene object
		 */
		inline const Rendering::SceneRef& experienceScene() const;

		/**
		 * Shows a permanent message at a specified location.
		 * @param message The message to show, must be valid
		 * @param world_T_message The transformation between message and world, must be valid
		 */
		void showMessage(const std::string& message, const HomogenousMatrix4& world_T_message = HomogenousMatrix4(Vector3(0, 0, -2)));

		/**
		 * Shows a permanent message at a specified location.
		 * @param messageType The type of the message to show
		 * @param world_T_message The transformation between message and world, must be valid
		 */
		void showMessage(const MessageType messageType, const HomogenousMatrix4& world_T_message = HomogenousMatrix4(Vector3(0, 0, -2)));

	private:

		/// The rendering Scene object holding the experience.
		Rendering::SceneRef renderingExperienceScene_;

		/// The experience's engine.
		Rendering::EngineRef renderingExperienceEngine_;
};

inline const Rendering::SceneRef& XRPlaygroundExperience::experienceScene() const
{
	return renderingExperienceScene_;
}

}

}

#endif // META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_XRPLAYGROUND_EXPERIENCE_H
