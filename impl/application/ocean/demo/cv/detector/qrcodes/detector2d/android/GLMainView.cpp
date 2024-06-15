/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/qrcodes/detector2d/android/GLMainView.h"

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

void GLMainView::initializeQRCode(const std::string& inputMedium, const std::string& resolution)
{
	std::vector<std::wstring> commandLines;

	commandLines.push_back(String::toWString(inputMedium));
	commandLines.push_back(String::toWString(resolution));

	detector2DWrapper_ = Detector2DWrapper(commandLines);

	if (detector2DWrapper_.frameMedium())
	{
		pixelImage_->setDevice_T_camera(detector2DWrapper_.frameMedium()->device_T_camera());
	}

	if (!setBackgroundMedium(pixelImage_, true))
	{
		Log::error() << "Failed to set the background medium";
	}

	startThread();
}

void GLMainView::threadRun()
{
	double resultingAlignerPerformance;

	while (shouldThreadStop() == false)
	{
		// We check whether the platform-independent detector has some new image to process

		Frame resultingFrame;
		std::vector<std::string> messages;
		detector2DWrapper_.detectAndDecode(resultingFrame, resultingAlignerPerformance, messages);

		if (resultingFrame.isValid())
		{
			// We received a frame from the detector
			// so we forward the result to the render by updating the visual content of the pixel image

			// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
			// however, this demo application focuses on the usage of platform independent code and not on performance
			// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

			pixelImage_->setPixelImage(std::move(resultingFrame));

			Log::info() << resultingAlignerPerformance * 1000.0 << "ms";

			if (messages.empty())
			{
				Log::info() << "---";
			}
			else
			{
				Log::info() << "Found " << messages.size() << " codes:";

				for (const std::string& message : messages)
				{
					Log::info() << message;
				}
			}
		}
		else
		{
			Thread::sleep(1u);
		}
	}
}

jboolean Java_com_meta_ocean_app_demo_cv_detector_qrcodes_detector2d_android_Detector2DActivity_initializeQRCode(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	GLMainView::get<GLMainView>().initializeQRCode(inputMediumValue, resolutionValue);

	return true;
}
