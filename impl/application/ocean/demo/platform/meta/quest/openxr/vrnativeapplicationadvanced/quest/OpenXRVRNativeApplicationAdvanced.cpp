/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/vrnativeapplicationadvanced/quest/OpenXRVRNativeApplicationAdvanced.h"

OpenXRVRNativeApplicationAdvanced::OpenXRVRNativeApplicationAdvanced(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	// nothing to do here
}

void OpenXRVRNativeApplicationAdvanced::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	if (passthrough_.isValid() && !passthrough_.isStarted())
	{
		if (!passthrough_.start())
		{
			Log::error() << "Failed to start passthrough";
		}
	}

	if (vrHandVisualizer_.isValid())
	{
		vrHandVisualizer_.setTransparency(0.0f);

		if (predictedDisplayTime >= handVisualizationModeTimestamp_)
		{
			if (handVisualizationModeTimestamp_.isValid())
			{
				const std::vector<VRHandVisualizer::RenderMode> renderModes =
				{
					VRHandVisualizer::RM_MESH,
					VRHandVisualizer::RM_JOINTS,
					VRHandVisualizer::RM_WIREFRAME,
					VRHandVisualizer::RenderMode(VRHandVisualizer::RM_JOINTS | VRHandVisualizer::RM_WIREFRAME),
					VRHandVisualizer::RM_ALL
				};

				renderModeIndex_ = (renderModeIndex_ + 1) % renderModes.size();

				vrHandVisualizer_.setRenderMode(renderModes[renderModeIndex_]);
			}

			handVisualizationModeTimestamp_ = predictedDisplayTime + 2.0;
		}
	}
}
