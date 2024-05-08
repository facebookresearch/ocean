// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "application/ocean/demo/cv/detector/linedetector/android/GLMainView.h"

#include "ocean/base/String.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

const bool GLMainView::viewInstanceRegistered = GLMainView::registerInstanceFunction(GLMainView::createInstance);

GLMainView::GLMainView()
{
	viewPixelImage = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);
}

GLMainView::~GLMainView()
{
	stopThread();
	release();
}

void GLMainView::initializeLineDetector(const std::string& inputMedium, const std::string& resolution)
{
	std::vector<std::wstring> commandLines;

	commandLines.push_back(String::toWString(inputMedium));
	commandLines.push_back(String::toWString(resolution));

	viewLineDetectorWrapper = LineDetectorWrapper(commandLines);

	const Media::FrameMediumRef oldBackgroundMedium = backgroundMedium();

	if (viewPixelImage && oldBackgroundMedium)
	{
		viewPixelImage->setDevice_T_camera(oldBackgroundMedium->device_T_camera());
	}

	setBackgroundMedium(viewPixelImage, true /*adjustFov*/);

	startThread();
}

void GLMainView::threadRun()
{
	Frame resultingDetectorFrame;
	double resultingDetectorPerformance;

	while (shouldThreadStop() == false)
	{
		// we check whether the platform independent detector has some new image to process

		if (viewLineDetectorWrapper.detectNewFrame(resultingDetectorFrame, resultingDetectorPerformance) && resultingDetectorFrame.isValid())
		{
			// we received an augmented frame from the detector
			// so we forward the result to the render by updating the visual content of the pixel image

			// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
			// however, this demo application focuses on the usage of platform independent code and not on performance
			// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

			viewPixelImage->setPixelImage(resultingDetectorFrame);

			Log::info() << resultingDetectorPerformance * 1000.0 << "ms";
		}
		else
		{
			Thread::sleep(1u);
		}
	}

}

jboolean Java_com_meta_ocean_app_demo_cv_detector_linedetector_android_LineDetectorActivity_initializeLineDetector(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	GLMainView::get<GLMainView>().initializeLineDetector(inputMediumValue, resolutionValue);

	return true;
}
