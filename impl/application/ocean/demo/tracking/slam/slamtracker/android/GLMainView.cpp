/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/slam/slamtracker/android/GLMainView.h"

#include "ocean/base/Frame.h"
#include "ocean/base/String.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/ResourceManager.h"
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

void GLMainView::initializeSLAMTracker(const std::string& inputMedium, const std::string& resolution, const std::string& deviceModel)
{
	JavaVM* javaVM = Platform::Android::NativeInterfaceManager::get().virtualMachine();
	jobject activity = Platform::Android::NativeInterfaceManager::get().currentActivity();

	if (Platform::Android::ResourceManager::get().initialize(javaVM, activity))
	{
		Platform::Android::ResourceManager::UniqueScopedResource scopedResource = Platform::Android::ResourceManager::get().accessAsset("camera_calibration.json");

		if (scopedResource)
		{
			if (IO::CameraCalibrationManager::get().registerCalibrations(scopedResource->data(), scopedResource->size()))
			{
				Log::info() << "Successfully loaded camera calibration file";

				IO::CameraCalibrationManager::get().setDeviceVersion(deviceModel);
			}
		}
	}

	std::vector<std::wstring> commandLines;

	commandLines.emplace_back(L"--input");
	commandLines.emplace_back(String::toWString(inputMedium));

	if (!resolution.empty())
	{
		commandLines.emplace_back(L"--resolution");
		commandLines.emplace_back(String::toWString(resolution));
	}

	slamTracker_ = std::make_unique<SLAMTrackerWrapper>(commandLines);

	if (slamTracker_->frameMedium())
	{
		pixelImage_->setDevice_T_camera(slamTracker_->frameMedium()->device_T_camera());
	}

	if (!setBackgroundMedium(pixelImage_, true))
	{
		Log::error() << "Failed to set the background medium";
	}

	startThread();
}

void GLMainView::threadRun()
{
	while (shouldThreadStop() == false)
	{
		// we check whether the platform independent tracker has some new image to process

		Frame outputFrame;
		if (slamTracker_->trackNewFrame(outputFrame) && outputFrame.isValid())
		{

			pixelImage_->setPixelImage(std::move(outputFrame));
		}
		else
		{
			Thread::sleep(1u);
		}
	}
}

bool GLMainView::startRecording()
{
	if (slamTracker_)
	{
		return slamTracker_->startRecording();
	}

	Log::error() << "SLAM tracker not initialized";
	return false;
}

bool GLMainView::stopRecording()
{
	if (slamTracker_)
	{
		return slamTracker_->stopRecording();
	}

	Log::error() << "SLAM tracker not initialized";
	return false;
}

jboolean Java_com_meta_ocean_app_demo_tracking_slam_slamtracker_android_SLAMTrackerActivity_initializeSLAMTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	std::string deviceModel;
	if (!Platform::Android::Utilities::deviceModel(env, deviceModel))
	{
		Log::error() << "Failed to determine device model";
	}

	GLMainView::get<GLMainView>().initializeSLAMTracker(inputMediumValue, resolutionValue, deviceModel);

	return true;
}

jboolean Java_com_meta_ocean_app_demo_tracking_slam_slamtracker_android_SLAMTrackerActivity_startRecording(JNIEnv* env, jobject javaThis)
{
	return GLMainView::get<GLMainView>().startRecording();
}

jboolean Java_com_meta_ocean_app_demo_tracking_slam_slamtracker_android_SLAMTrackerActivity_stopRecording(JNIEnv* env, jobject javaThis)
{
	return GLMainView::get<GLMainView>().stopRecording();
}
