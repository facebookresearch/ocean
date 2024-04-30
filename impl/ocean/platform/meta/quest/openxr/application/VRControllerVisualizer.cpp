// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

			Vector3 translationOffset(0, 0, 0);

			switch (deviceType_)
			{
				case Device::DT_QUEST:
					translationOffset = Vector3(0, 0, Scalar(0.0525));
					break;

				case Device::DT_QUEST_2:
				case Device::DT_QUEST_3:
				case Device::DT_QUEST_PRO:
				case Device::DT_VENTURA:
					translationOffset = Vector3(0, 0, Scalar(0.055));
					break;

				case Device::DT_UNKNOWN:
					ocean_assert(false && "Unknown device type!");
					break;
			}

			baseSpace_T_controllerAim *= HomogenousMatrix4(translationOffset);
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
