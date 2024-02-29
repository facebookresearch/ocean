// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/qt/XRPlaygroundMain.h"
#include "application/ocean/xrplayground/qt/MainWidget.h"

#include "application/ocean/xrplayground/common/PrototypeDevices.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"

#include "ocean/devices/mapbuilding/MapBuilding.h"
#include "ocean/devices/pattern/Pattern.h"

#include "ocean/platform/meta/Login.h"

#include "ocean/rendering/glescenegraph/qt/QT.h"

#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdx/x3d/X3d.h"

#ifdef __APPLE__
	#include "ocean/media/avfoundation/AVFoundation.h"
	#include "ocean/media/imageio/ImageIO.h"
#endif

#include <QtWidgets/QApplication>

using namespace Ocean;
using namespace Ocean::XRPlayground::QT;

int main(int argc, char *argv[])
{
	QApplication qtApplication(argc, argv);

	Messenger::get().setOutputType(Messenger::MessageOutput(Messenger::OUTPUT_DEBUG_WINDOW | Messenger::OUTPUT_QUEUED));

	CommandArguments commandArguments;
	commandArguments.registerParameter("content", "c", "The scene file or experience to load");
	commandArguments.registerParameter("vrs", "v", "The VRS recording to load");
	commandArguments.registerParameter("userId", "u", "The user id to be used for logging into FB services");
	commandArguments.registerParameter("userToken", "t", "The user token to be used for logging into FB services");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, argc);

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	const Value contentValue = commandArguments.value("content");
	const std::string content = contentValue.isString() ? contentValue.stringValue() : "";

	const Value vrsValue = commandArguments.value("vrs");
	const std::string vrsRecording = vrsValue.isString() ? vrsValue.stringValue() : "";

	std::string userId;
	std::string userToken;

	const Value userTokenValue = commandArguments.value("userToken");

	if (userTokenValue.isString())
	{
		userToken = userTokenValue.stringValue();
	}

	const Value userIdValue = commandArguments.value("userId");

	if (userIdValue.isFloat64(true) || userIdValue.isString())
	{
		userId = userIdValue.isString() ? userIdValue.stringValue() : String::toAString(int64_t(userIdValue.float64Value(true)));
	}

	Platform::Meta::Login::get().setLogin(Platform::Meta::Login::LT_FACEBOOK, std::move(userId), std::move(userToken));

	XRPlayground::PrototypeDevices::registerPrototypeDevices();

#ifdef __APPLE__
	Media::ImageIO::registerImageIOLibrary();
	Media::AVFoundation::registerAVFLibrary();
#endif

	Devices::Pattern::registerPatternLibrary();
	Devices::MapBuilding::registerMapBuildingLibrary();

	Rendering::GLESceneGraph::QT::registerGLESceneGraphEngine();

	SceneDescription::SDL::OBJ::registerOBJLibrary();
	SceneDescription::SDL::Assimp::registerAssimpLibrary();
	SceneDescription::SDX::X3D::registerX3DLibrary();

	int result = 0;

	{
		MainWidget mainWidget(content, vrsRecording);
		mainWidget.show();

		result = qtApplication.exec();
	}

	SceneDescription::SDX::X3D::unregisterX3DLibrary();
	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();

	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();

	Devices::MapBuilding::unregisterMapBuildingLibrary();
	Devices::Pattern::unregisterPatternLibrary();

#ifdef __APPLE__
	Media::AVFoundation::unregisterAVFLibrary();
	Media::ImageIO::unregisterImageIOLibrary();
#endif

	return result;
}
