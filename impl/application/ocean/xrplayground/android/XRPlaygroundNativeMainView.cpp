// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

// clang-format off
// @nolint

#include "application/ocean/xrplayground/android/XRPlaygroundNativeMainView.h"

#include "application/ocean/xrplayground/common/PrototypeDevices.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/media/LiveVideo.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Resource.h"
#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Android
{

const bool XRPlaygroundNativeMainView::instanceRegistered_ = XRPlaygroundNativeMainView::registerInstanceFunction(XRPlaygroundNativeMainView::createInstance);

XRPlaygroundNativeMainView::XRPlaygroundNativeMainView() :
	cameraPermissionGranted_(false)
{
	// nothing to do here
}

XRPlaygroundNativeMainView::~XRPlaygroundNativeMainView()
{
	release();
}

bool XRPlaygroundNativeMainView::initialize()
{
	Log::debug() << "XRPlaygroundNativeMainView::initialize()";

	if (!GLView::initialize())
	{
		return false;
	}

	if (!PrototypeDevices::registerPrototypeDevices())
	{
		Log::error() << "At least one prototype device could not be registered";
	}

	RandomI::initialize();

	Platform::Android::ResourceManager::get().initialize(Platform::Android::NativeInterfaceManager::get().virtualMachine(), Platform::Android::NativeInterfaceManager::get().currentActivity());

	ocean_assert(Platform::Android::ResourceManager::get().isValid());
	const std::string externalDirectoryName = Platform::Android::ResourceManager::get().externalFilesDirectory();

	Log::info() << "Using external directory: " << externalDirectoryName;

	// experience assets
	const std::string temporaryAssetTargetDirectory = externalDirectoryName + "/xrplayground/assets";
	if (Platform::Android::ResourceManager::get().copyAssets(temporaryAssetTargetDirectory, /* createDirectory */ true))
	{
		const IO::Directory assetDirectory(temporaryAssetTargetDirectory);

		if (assetDirectory.exists())
		{
			IO::FileResolver::get().addReferencePath(assetDirectory);

			loadContent(temporaryAssetTargetDirectory + "/instructions.x3dv", false);
		}
		else
		{
			Log::error() << "Failed to create the asset directory";
		}
	}
	else
	{
		Log::error() << "Failed to copy scene asset files";
	}

	int versionCode = -1;
	std::string versionName;

	if (Platform::Android::Utilities::manifestVersion(Platform::Android::NativeInterfaceManager::get().virtualMachine(), Platform::Android::NativeInterfaceManager::get().currentActivity(), versionCode, versionName))
	{
		versionName += ", " + String::toAString(versionCode);
	}
	else
	{
		Log::error() << "Failed to determine version";
	}

	if (!xrPlaygroundCommon_.initialize(versionName))
	{
		return false;
	}

	if (cameraPermissionGranted_)
	{
		// the permission was granted before we initialized the rendering engine

		setupCamera();
	}

	return true;
}

bool XRPlaygroundNativeMainView::release()
{
	Log::debug() << "XRPlaygroundNativeMainView::release()";

	xrPlaygroundCommon_.unloadContent();

	return GLView::release();
}

bool XRPlaygroundNativeMainView::resize(const int width, const int height)
{
	const Scalar aspectRatio = height != 0u ? Scalar(width) / Scalar(height) : Scalar(0);

	xrPlaygroundCommon_.resize(width, height, aspectRatio);

	return true;
}

bool XRPlaygroundNativeMainView::render()
{
	xrPlaygroundCommon_.preRender(Interaction::UserInterface());

	xrPlaygroundCommon_.render();

	return true;
}

bool XRPlaygroundNativeMainView::loadContent(const std::string& filename, const bool replace)
{
	return xrPlaygroundCommon_.loadContent(filename, replace);
}

bool XRPlaygroundNativeMainView::unloadContent()
{
	xrPlaygroundCommon_.unloadContent();

	return true;
}

bool XRPlaygroundNativeMainView::startVRSRecording()
{
	if (xrPlaygroundCommon_.isVRSRecordingActive())
	{
		Log::warning() << "VRS recording already active";
		return false;
	}

	const std::string externalDirectoryName = Platform::Android::ResourceManager::get().externalFilesDirectory();

	const IO::Directory directory = IO::Directory(externalDirectoryName) + IO::Directory("VRSRecordings");

	const IO::File vrsFilename(directory + IO::File("XRPlayground_Recording__" + DateTime::localStringDate('-') + "__" + DateTime::localStringTime(false, '-') + ".vrs"));

	return xrPlaygroundCommon_.startVRSRecording(vrsFilename());
}

bool XRPlaygroundNativeMainView::stopVRSRecording()
{
	if (!xrPlaygroundCommon_.isVRSRecordingActive())
	{
		Log::warning() << "VRS recording not active";
		return false;
	}

	return xrPlaygroundCommon_.stopVRSRecording();
}

void XRPlaygroundNativeMainView::onPermissionGranted(const std::string& permission)
{
	GLView::onPermissionGranted(permission);

	if (permission == "android.permission.CAMERA")
	{
		cameraPermissionGranted_ = true;

		if (xrPlaygroundCommon_.isInitialized())
		{
			setupCamera();
		}
	}
}

void XRPlaygroundNativeMainView::onTouchDown(const float x, const float y)
{
	const Vector2 screenPosition = Vector2(Scalar(x), Scalar(y));
	xrPlaygroundCommon_.touchEventStarted(Interaction::UserInterface(), screenPosition);
}

void XRPlaygroundNativeMainView::onTouchMove(const float x, const float y)
{
	const Vector2 screenPosition = Vector2(Scalar(x), Scalar(y));
	xrPlaygroundCommon_.touchEventMoved(Interaction::UserInterface(), screenPosition);
}

void XRPlaygroundNativeMainView::onTouchUp(const float x, const float y)
{
	const Vector2 screenPosition = Vector2(Scalar(x), Scalar(y));
	xrPlaygroundCommon_.touchEventStopped(Interaction::UserInterface(), screenPosition);
}

bool XRPlaygroundNativeMainView::setupCamera()
{
	const Media::FrameMediumRef liveVideo = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);

	if (liveVideo)
	{
		liveVideo->setPreferredFrameDimension(1280u, 720u);
		liveVideo->start();

		return xrPlaygroundCommon_.setBackgroundFrameMedium(liveVideo);
	}

	return false;
}

}

}

}
