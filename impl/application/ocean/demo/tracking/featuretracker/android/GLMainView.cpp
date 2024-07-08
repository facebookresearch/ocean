/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/featuretracker/android/GLMainView.h"

#include "ocean/base/Frame.h"
#include "ocean/base/String.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

const bool GLMainView::instanceRegistered_ = GLMainView::registerInstanceFunction(GLMainView::createInstance);

GLMainView::GLMainView()
{
	pixelImage_ = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);
}

GLMainView::~GLMainView()
{
	stopThread();
	release();
}

void GLMainView::initializeFeatureTracker(const std::string& inputMedium, const std::string& pattern, const std::string& resolution)
{
	const std::vector<std::wstring> commandLines =
	{
		L"-i",
		String::toWString(inputMedium),
		L"-p",
		String::toWString(pattern),
		L"-r",
		String::toWString(resolution)
	};

	featureTrackerWrapper_ = FeatureTrackerWrapper(commandLines);

	const Media::FrameMediumRef inputFrameMedium = featureTrackerWrapper_.inputMedium();
	if (pixelImage_ && inputFrameMedium)
	{
		pixelImage_->setDevice_T_camera(inputFrameMedium->device_T_camera());
	}

	if (!setBackgroundMedium(pixelImage_, true /*adjustFov*/))
	{
		Log::error() << "Failed to set the background medium";
	}

	startThread();
}

void GLMainView::threadRun()
{
	double resultingTrackerPerformance;

	while (shouldThreadStop() == false)
	{
		// we check whether the platform independent tracker has some new image to process

		Frame resultingTrackerFrame;

		if (featureTrackerWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance) && resultingTrackerFrame.isValid())
		{
			// we received an augmented frame from the tracker
			// so we forward the result to the render by updating the visual content of the pixel image

			// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
			// however, this demo application focuses on the usage of platform independent code and not on performance
			// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

			pixelImage_->setPixelImage(std::move(resultingTrackerFrame));

			Log::info() << resultingTrackerPerformance * 1000.0 << "ms";
		}
		else
		{
			Thread::sleep(1u);
		}
	}

}

jboolean Java_com_meta_ocean_app_demo_tracking_featuretracker_android_FeatureTrackerActivity_initializeFeatureTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring pattern, jstring resolution)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string patternValue(Platform::Android::Utilities::toAString(env, pattern));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	GLMainView::get<GLMainView>().initializeFeatureTracker(inputMediumValue, patternValue, resolutionValue);

	return true;
}
