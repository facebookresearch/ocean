/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/similaritytracker/android/GLMainView.h"

#include "ocean/base/Frame.h"
#include "ocean/base/String.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

const bool GLMainView::viewInstanceRegistered = GLMainView::registerInstanceFunction(GLMainView::createInstance);

GLMainView::GLMainView() :
	recentTouchPosition(Numeric::minValue(), Numeric::maxValue())
{
	viewPixelImage = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);
}

GLMainView::~GLMainView()
{
	stopThread();
	release();
}

void GLMainView::initializeSimilarityTracker(const std::string& inputMedium, const std::string& resolution)
{
	std::vector<std::wstring> commandLines;

	commandLines.push_back(String::toWString(inputMedium));
	commandLines.push_back(String::toWString(resolution));

	viewSimilarityTracker = SimilarityTrackerWrapper(commandLines);

	const Media::FrameMediumRef oldBackgroundMedium = backgroundMedium();
	if (viewPixelImage && oldBackgroundMedium)
	{
		viewPixelImage->setDevice_T_camera(oldBackgroundMedium->device_T_camera());
	}

	setBackgroundMedium(viewPixelImage, true);

	startThread();
}

void GLMainView::threadRun()
{
	double resultingTrackerPerformance;

	while (shouldThreadStop() == false)
	{
		// we check whether the platform independent tracker has some new image to process

		const Vector2 localRecentTouchPosition = recentTouchPosition;
		Frame resultingTrackerFrame;

		if (viewSimilarityTracker.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance, localRecentTouchPosition) && resultingTrackerFrame.isValid())
		{
			// we received an augmented frame from the tracker
			// so we forward the result to the render by updating the visual content of the pixel image

			// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
			// however, this demo application focuses on the usage of platform independent code and not on performance
			// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

			viewPixelImage->setPixelImage(resultingTrackerFrame);

			Log::info() << resultingTrackerPerformance * 1000.0 << "ms";

			recentTouchPosition = Vector2(Numeric::minValue(), Numeric::minValue());
		}
		else
		{
			Thread::sleep(1u);
		}
	}

}

void GLMainView::onTouchDown(const float x, const float y)
{
	Scalar xFrame, yFrame;

	if (screen2frame(x, y, xFrame, yFrame))
	{
		recentTouchPosition = Vector2(xFrame, yFrame);
	}
}

jboolean Java_com_meta_ocean_app_demo_tracking_similaritytracker_android_SimilarityTrackerActivity_initializeSimilarityTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	GLMainView::get<GLMainView>().initializeSimilarityTracker(inputMediumValue, resolutionValue);

	return true;
}
