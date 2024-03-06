// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/fbmessengercodes/android/GLMainView.h"

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

void GLMainView::initializeFBMessengerCodes(const std::string& inputMedium, const std::string& resolution, const std::string& pixelFormat, const std::string& badge, const std::string bullseye, const std::string& classifier)
{
	std::vector<std::wstring> commandLines;

	commandLines.push_back(String::toWString(inputMedium));
	commandLines.push_back(String::toWString(resolution));
	commandLines.push_back(String::toWString(pixelFormat));
	commandLines.push_back(String::toWString(badge));
	commandLines.push_back(String::toWString(bullseye));
	commandLines.push_back(String::toWString(classifier));

	viewFBMessengerCodesWrapper = FBMessengerCodesWrapper(commandLines);

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
	Frame resultingAlignerFrame;
	double resultingAlignerPerformance;

	while (shouldThreadStop() == false)
	{
		// We check whether the platform-independent detector has some new image to process

		std::vector<std::string> messages;
		viewFBMessengerCodesWrapper.detectAndDecode(resultingAlignerFrame, resultingAlignerPerformance, messages);

		if (resultingAlignerFrame.isValid())
		{
			// We received a frame from the detector
			// so we forward the result to the render by updating the visual content of the pixel image

			// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
			// however, this demo application focuses on the usage of platform independent code and not on performance
			// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

			viewPixelImage->setPixelImage(resultingAlignerFrame);

			Log::info() << resultingAlignerPerformance * 1000.0 << "ms";

			Log::info() << (messages.empty() ? std::string("---") : (std::string("Found ") + String::toAString(messages.size()) + " codes"));
		}
		else
		{
			Thread::sleep(1u);
		}
	}
}

jboolean Java_com_meta_ocean_app_demo_cv_detector_fbmessengercodes_android_FBMessengerCodesActivity_initializeFBMessengerCodes(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution, jstring pixelFormat, jstring badge, jstring bullseye, jstring classifier)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));
	const std::string pixelFormatValue(Platform::Android::Utilities::toAString(env, pixelFormat));
	const std::string badgeValue(Platform::Android::Utilities::toAString(env, badge));
	const std::string bullseyeValue(Platform::Android::Utilities::toAString(env, bullseye));
	const std::string classifierValue(Platform::Android::Utilities::toAString(env, classifier));

	GLMainView::get<GLMainView>().initializeFBMessengerCodes(inputMediumValue, resolutionValue, pixelFormatValue, badgeValue, bullseyeValue, classifierValue);

	return true;
}
