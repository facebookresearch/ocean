/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/linedetector/android/GLMainView.h"

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

void GLMainView::initializeLineDetector(const std::string& inputMedium, const std::string& resolution)
{
	std::vector<std::wstring> commandLines;

	commandLines.emplace_back(String::toWString(inputMedium));
	commandLines.emplace_back(String::toWString(resolution));

	lineDetectorWrapper_ = LineDetectorWrapper(commandLines);

	if (lineDetectorWrapper_.frameMedium())
	{
		pixelImage_->setDevice_T_camera(lineDetectorWrapper_.frameMedium()->device_T_camera());
	}

	if (!setBackgroundMedium(pixelImage_, true /*adjustFov*/))
	{
		Log::error() << "Failed to set the background medium";
	}

	startThread();
}

void GLMainView::threadRun()
{
	Frame resultingDetectorFrame;
	double resultingDetectorPerformance;

	double sumPerformance = 0.0;
	unsigned int performanceCounter = 0u;

	while (shouldThreadStop() == false)
	{
		// we check whether the platform independent detector has some new image to process

		if (lineDetectorWrapper_.detectNewFrame(resultingDetectorFrame, resultingDetectorPerformance) && resultingDetectorFrame.isValid())
		{
			// we received an augmented frame from the detector
			// so we forward the result to the render by updating the visual content of the pixel image

			// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
			// however, this demo application focuses on the usage of platform independent code and not on performance
			// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

			pixelImage_->setPixelImage(std::move(resultingDetectorFrame));
			resultingDetectorFrame = Frame();

			sumPerformance += resultingDetectorPerformance;
			++performanceCounter;

			if (performanceCounter >= 10u)
			{
				Log::info() << "Average performance: " << (sumPerformance / double(performanceCounter)) * 1000.0 << "ms";

				performanceCounter = 0u;
				sumPerformance = 0.0;
			}
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
