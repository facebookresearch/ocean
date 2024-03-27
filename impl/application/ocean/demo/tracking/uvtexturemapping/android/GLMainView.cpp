// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/uvtexturemapping/android/GLMainView.h"

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

void GLMainView::initializeUVTextureMapping(const std::string& inputMedium, const std::string& pattern, const std::string& resolution)
{
	std::vector<std::wstring> commandLines;

	commandLines.push_back(String::toWString(inputMedium));
	commandLines.push_back(String::toWString(pattern));
	commandLines.push_back(String::toWString(resolution));

	uvTextureMappingWrapper_ = UVTextureMappingWrapper(commandLines);

	const Media::FrameMediumRef oldBackgroundMedium = backgroundMedium();
	if (pixelImage_ && oldBackgroundMedium)
	{
		pixelImage_->setDevice_T_camera(oldBackgroundMedium->device_T_camera());
	}

	setBackgroundMedium(pixelImage_, true);

	startThread();
}

void GLMainView::threadRun()
{
	Frame resultingTrackerFrame;
	double resultingTrackerPerformance;

	while (shouldThreadStop() == false)
	{
		// we check whether the platform independent tracker has some new image to process

		if (uvTextureMappingWrapper_.trackNewFrame(resultingTrackerFrame, resultingTrackerPerformance) && resultingTrackerFrame.isValid())
		{
			// we received an augmented frame from the tracker
			// so we forward the result to the render by updating the visual content of the pixel image

			// **NOTE** copying the resulting RGB frame and forwarding the frame to the renderer costs some performance
			// however, this demo application focuses on the usage of platform independent code and not on performance
			// @see ocean_app_shark for a high performance implementation of an Augmented Realty application (even more powerful)

			pixelImage_->setPixelImage(resultingTrackerFrame);

			Log::info() << resultingTrackerPerformance * 1000.0 << "ms";
		}
		else
		{
			Thread::sleep(1u);
		}
	}

}

jboolean Java_com_facebook_ocean_app_demo_tracking_uvtexturemapping_android_UVTextureMappingActivity_initializeUVTextureMapping(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring pattern, jstring resolution)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string patternValue(Platform::Android::Utilities::toAString(env, pattern));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	GLMainView::get<GLMainView>().initializeUVTextureMapping(inputMediumValue, patternValue, resolutionValue);

	return true;
}