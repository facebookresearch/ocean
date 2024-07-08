/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/application/VRControllerVisualizer.h"

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

namespace Application
{

VRControllerVisualizer::VRControllerVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, const Device::DeviceType deviceType, const std::string& renderModelDirectoryName) :
	Quest::Application::VRControllerVisualizer(engine, framebuffer, deviceType, renderModelDirectoryName)
{
	// nothing to do here

	controllerAim_t_controllerModel_ = Vector3(0, 0, 0);

	switch (deviceType_)
	{
		case Device::DT_QUEST:
			controllerAim_t_controllerModel_ = Vector3(0, 0, Scalar(0.0525));
			break;

		case Device::DT_QUEST_2:
		case Device::DT_QUEST_3:
		case Device::DT_QUEST_PRO:
			controllerAim_t_controllerModel_ = Vector3(0, 0, Scalar(0.055));
			break;

		case Device::DT_UNKNOWN:
		case Device::DT_QUEST_END:
			ocean_assert(false && "Unknown device type!");
			break;
	}

#ifdef OCEAN_PLATFORM_META_QUEST_OPENXR_USE_EXTERNAL_TRANSLATION_OFFSET
	if (deviceType_ > Device::DT_QUEST_END)
	{
		ocean_assert(controllerAim_t_controllerModel_.isNull());
		controllerAim_t_controllerModel_ = VRControllerVisualizer_externalTranslationOffset(deviceType_);
	}
#endif
}

void VRControllerVisualizer::visualizeControllersInWorld(const TrackedController& trackedController, const Scalar controllerRayLength)
{
	ocean_assert(isValid());
	ocean_assert(trackedController.isValid());

	if (!isValid() || !trackedController.isValid())
	{
		return;
	}

	for (const TrackedController::ControllerType controllerType : {TrackedController::CT_LEFT, TrackedController::CT_RIGHT})
	{
		HomogenousMatrix4 baseSpace_T_controllerAim;
		if (trackedController.pose(controllerType, &baseSpace_T_controllerAim, /* baseSpace_T_controllerGrap */ nullptr) && baseSpace_T_controllerAim.isValid())
		{
			// we may need to apply a manual shift to align the aim space with the controller's origin

			ocean_assert(controllerAim_t_controllerModel_.x() != Numeric::minValue());

			baseSpace_T_controllerAim *= HomogenousMatrix4(controllerAim_t_controllerModel_);
		}
		else
		{
			baseSpace_T_controllerAim.toNull();
		}

		visualizeControllerInWorld(controllerType == TrackedController::CT_LEFT ? CT_LEFT : CT_RIGHT, baseSpace_T_controllerAim, controllerRayLength);
	}
}

}

}

}

}

}

}
