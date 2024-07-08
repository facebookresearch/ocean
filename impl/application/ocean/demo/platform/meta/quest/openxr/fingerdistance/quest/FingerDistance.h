/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_FINGERDISTANCE_QUEST_FINGER_DISTANCE_H
#define META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_FINGERDISTANCE_QUEST_FINGER_DISTANCE_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

using namespace Ocean;
using namespace Platform::Meta::Quest::OpenXR::Application;

/**
 * Implements a specialization of the VRNativeApplicationAdvanced.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class FingerDistance final : public VRNativeApplicationAdvanced
{
	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit FingerDistance(struct android_app* androidApp);

	protected:

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRNativeApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

		/**
		 * Returns the center transformation between both finger tips.
		 * @param leftFingerTip The position of the left finger tip, in world coordinates
		 * @param rightFingerTip The position of the right finger tip, in world coordinates
		 * @return The resulting transformation transformation the center between both fingers top world, optimized so that x-axis is pointing to the right of the text, and y-axis upwards of the text, invalid in case of an error
		 */
		static HomogenousMatrix4 centerTransformation(const Vector3& leftFingerTip, const Vector3& rightFingerTip);

		/**
		 * Returns a string with the distance.
		 * @param distance The distance, in meter, with range [0, infinity)
		 * @return The resulting distance string
		 */
		static std::string distanceString(const Scalar distance);

	protected:

		/// The rendering Group object for all rendering objects.
		Rendering::GroupRef renderingGroup_;

		/// The rendering Transform object of the sphere for the left finger tip.
		Rendering::TransformRef renderingTransformFingerTipLeft_;

		/// The rendering Transform object of the sphere for the right finger tip.
		Rendering::TransformRef renderingTransformFingerTipRight_;

		/// The rendering Transform object of the cylinder between both finger tips.
		Rendering::TransformRef renderingTransformCylinder_;

		/// The rendering Transform object for the distance text.
		Rendering::TransformRef renderingTransformTextDistance_;

		/// The rendering Text object for the distance text.
		Rendering::TextRef renderingTextDistance_;
};

#endif // META_OCEAN_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_FINGERDISTANCE_QUEST_FINGER_DISTANCE_H
