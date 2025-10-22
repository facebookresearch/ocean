/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/calibration/picturetaker/android/GLMainView.h"

#include "ocean/base/DateTime.h"

#include "ocean/io/File.h"

#include "ocean/io/image/Image.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/ResourceManager.h"
#include "ocean/platform/android/Utilities.h"

#include "ocean/rendering/PerspectiveView.h"

#include <fstream>

using namespace Ocean;

const bool GLMainView::instanceRegistered_ = GLMainView::registerInstanceFunction(GLMainView::createInstance);

GLMainView::GLMainView()
{
	// nothing to do here
}

GLMainView::~GLMainView()
{
	release();
}

bool GLMainView::selectCamera(const std::string& cameraName)
{
	if (liveVideo_)
	{
		return false;

	}

	liveVideo_ = Media::Manager::get().newMedium(cameraName);

	if (!liveVideo_)
	{
		Log::error() << "Failed to access input medium '" << cameraName << "'";
		return false;
	}

	selectedCameraName_ = cameraName;

	return true;
}

bool GLMainView::startCamera(const std::string& resolution, const std::string& directory)
{
	unsigned int preferredWidth = 0u;
	unsigned int preferredHeight = 0u;
	if (Media::Utilities::parseResolution(resolution, preferredWidth, preferredHeight))
	{
		if (liveVideo_->setPreferredFrameDimension(preferredWidth, preferredHeight))
		{
			Log::debug() << "Set preferred resolution " << preferredWidth << "x" << preferredHeight;
		}
		else
		{
			Log::error() << "Failed to set preferred resolution " << preferredWidth << "x" << preferredHeight;
		}
	}
	else
	{
		Log::warning() << "Failed to parse resolution '" << resolution << "'";
	}

	if (!liveVideo_->start())
	{
		Log::error() << "Failed to start input medium '" << liveVideo_->url() << "'";
		return false;
	}

	videoStabilization_ = liveVideo_->videoStabilization() ? 1 : 0;

	if (!setBackgroundMedium(liveVideo_, false /*adjustFov=*/))
	{
		Log::error() << "Failed to set the background medium";
	}

	directory_ = IO::Directory(directory + DateTime::localStringDate('-') + "_" + DateTime::localStringTime(false, '-'));

	if (!directory_.exists() && !directory_.create())
	{
		Log::error() << "Failed to create directory '" << directory_() << "'";
		return false;
	}

	selectedResolution_ = resolution;

	return true;
}

bool GLMainView::setFocus(const float focus)
{
	if (focus < 0.0f || focus > 1.0f)
	{
		return false;
	}

	if (!liveVideo_)
	{
		return false;
	}

	currentFocus_ = focus;

	return liveVideo_->setFocus(focus);
}

bool GLMainView::setVideoStabilization(const bool enabled)
{
	if (!liveVideo_)
	{
		return false;
	}

	if (!liveVideo_->setVideoStabilization(enabled))
	{
		return false;
	}

	videoStabilization_ = enabled ? 1 : 0;

	return true;
}

bool GLMainView::videoStabilization() const
{
	return videoStabilization_ == 1;
}

bool GLMainView::takePicture()
{
	if (!liveVideo_)
	{
		return false;
	}

	const FrameRef frame = liveVideo_->frame();

	if (!frame)
	{
		return false;
	}

	if (!directory_.isValid())
	{
		return false;
	}

	// Write settings file on first image capture
	if (!settingsFileWritten_)
	{
		const IO::File settingsFile = directory_ + IO::File("camera_settings.txt");

		std::ofstream settingsStream(settingsFile());

		if (settingsStream.is_open())
		{
			settingsStream << "Camera: " << liveVideo_->url() << std::endl;
			settingsStream << "Resolution: " << frame->width() << "x" << frame->height() << std::endl;
			settingsStream << "Focus: " << currentFocus_ << std::endl;

			if (videoStabilization_ == 1)
			{
				settingsStream << "Video Stabilization: Enabled" << std::endl;
			}
			else if (videoStabilization_ == 0)
			{
				settingsStream << "Video Stabilization: Disabled" << std::endl;
			}
			else if (videoStabilization_ == -1)
			{
				settingsStream << "Video Stabilization: Unknown" << std::endl;
			}

			Log::info() << "Wrote camera settings to '" << settingsFile() << "'";
			settingsFileWritten_ = true;
		}
		else
		{
			Log::error() << "Failed to write camera settings file";
		}
	}

	const IO::File filename = directory_ + IO::File("image_" + String::toAString(frame->width()) + "x" + String::toAString(frame->height()) + "_" + String::toAString(pictureCounter_++, 3u) + ".png");

	if (!IO::Image::Comfort::writeImage(*frame, filename(), true))
	{
		Log::error() << "Failed to write the picture";
		return false;
	}

	Log::info() << "Wrote picture to '" << filename() << "'";

	return true;
}

std::vector<std::string> GLMainView::availableResolutions()
{
	if (!liveVideo_)
	{
		Log::error() << "No camera selected";
		ocean_assert(false && "This should never happen!");

		return std::vector<std::string>();
	}

	const Media::LiveVideo::StreamConfigurations streamConfigurations = liveVideo_->supportedStreamConfigurations(Media::LiveVideo::ST_FRAME);

	std::vector<std::string> result;

	for (const Media::LiveVideo::StreamConfiguration& streamConfiguration : streamConfigurations)
	{
		std::string resolution = String::toAString(streamConfiguration.width_) + "x" + String::toAString(streamConfiguration.height_);

		if (!hasElement(result, resolution))
		{
			result.emplace_back(std::move(resolution));
		}
	}

	return result;
}

std::vector<std::string> GLMainView::availableCameras()
{
	const Media::Library::Definitions definitions = Media::Manager::get().selectableMedia();

	std::vector<std::string> result;
	result.reserve(definitions.size());

	for (const Media::Library::Definition& definition : definitions)
	{
		result.emplace_back(definition.url());
	}

	return result;
}

bool GLMainView::initialize()
{
	if (!GLFrameView::initialize())
	{
		return false;
	}

	setFovX(Numeric::deg2rad(55));

	return true;
}

bool GLMainView::render()
{
	if (liveVideo_ && background_)
	{
		const Rotation view_R_background(background_->orientation());

		if (Numeric::isNotEqualEps(view_R_background.angle()))
		{
#ifdef OCEAN_DEBUG
			const Scalar absRotationAngle = Numeric::abs(view_R_background.angle());
			const Vector3 axis = view_R_background.axis();

			ocean_assert(Numeric::isEqual(absRotationAngle, Numeric::pi_2()) || Numeric::isEqual(absRotationAngle, Numeric::pi_2() * Scalar(3)));
			ocean_assert(axis == Vector3(0, 0, 1) || axis == Vector3(0, 0, -1));
#endif // OCEAN_DEBUG
		}

		const bool isNotRoated = Numeric::isEqualEps(view_R_background.angle());

		const PinholeCamera& camera = background_->camera();

		if (camera.isValid())
		{
			const Scalar backgroundFovX = isNotRoated ? camera.fovX() : camera.fovY();

			ocean_assert(framebuffer_);
			const Rendering::PerspectiveViewRef view = framebuffer_->view();

			if (view)
			{
				constexpr Scalar borderAngle = Numeric::deg2rad(2);
				view->setFovX(backgroundFovX + borderAngle);
			}
		}
	}

	return GLFrameView::render();
}

jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_startCamera(JNIEnv* env, jobject javaThis, jstring resolution)
{
	jobject currentActivity = Platform::Android::NativeInterfaceManager::get().currentActivity();

	if (currentActivity == nullptr)
	{
		Log::error() << "Current activity not set";
		return false;
	}

	std::string directory;
	if (!Platform::Android::ResourceManager::getExternalFilesDirectory(env, currentActivity, directory))
	{
		Log::error() << "Failed to determine the external file directory";
		return false;
	}

	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	return GLMainView::get<GLMainView>().startCamera(resolutionValue, directory);
}

jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_takePicture(JNIEnv* env, jobject javaThis)
{
	return GLMainView::get<GLMainView>().takePicture();
}

jobjectArray Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_availableCameras(JNIEnv* env, jobject javaThis)
{
	const std::vector<std::string> cameras = GLMainView::availableCameras();

	return Platform::Android::Utilities::toJavaStringArray(env, cameras);
}

jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_selectCamera(JNIEnv* env, jobject javaThis, jstring cameraName)
{
	const std::string cameraNameValue(Platform::Android::Utilities::toAString(env, cameraName));

	return GLMainView::get<GLMainView>().selectCamera(cameraNameValue);
}

jobjectArray Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_availableResolutions(JNIEnv* env, jobject javaThis)
{
	const std::vector<std::string> resolutions = GLMainView::get<GLMainView>().availableResolutions();

	return Platform::Android::Utilities::toJavaStringArray(env, resolutions);
}

jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_setFocus(JNIEnv* env, jobject javaThis, jfloat focus)
{
	return GLMainView::get<GLMainView>().setFocus(float(focus));
}

jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_setVideoStabilization(JNIEnv* env, jobject javaThis, jboolean enabled)
{
	return GLMainView::get<GLMainView>().setVideoStabilization(bool(enabled));
}

jboolean Java_com_meta_ocean_app_demo_cv_calibration_picturetaker_android_PictureTakerActivity_videoStabilization(JNIEnv* env, jobject javaThis)
{
	return GLMainView::get<GLMainView>().videoStabilization();
}
