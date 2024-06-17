/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "SharkViewController.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Thread.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"

#include "ocean/scenedescription/Manager.h"

#include "ocean/platform/apple/Resource.h"

using namespace Ocean;

@interface SharkViewController ()
{
	/// The rendering engine.
	Rendering::EngineRef engine_;

	/// The framebuffer in which the result will be rendered.
	Rendering::FramebufferRef framebuffer_;

	/// The optional permanent scene description which may realize interaction and tracking logic.
	SceneDescription::SDXSceneRef sceneDescription_;
}

@end

@implementation SharkViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

	RandomI::initialize();

	// we aquire any existing rendering engine using OpenGL ES (use individual param for other graphics API)
	engine_ = Rendering::Manager::get().engine("", Rendering::Engine::API_OPENGLES);

	// we ensure that we have a valid rendering engine before we proceed
	if (engine_.isNull())
	{
		return;
	}

	// we create a framebuffer in which we will draw the content
	framebuffer_ = engine_->createFramebuffer();

	// we create a view with perspective projection model
	Rendering::PerspectiveViewRef renderingView = engine_->factory().createPerspectiveView();

	if (renderingView.isNull())
	{
		return;
	}

	// we set the initial horizontal viewing angle of the view (however, this fov will be adjusted later so that it perfectly fits with the video-background)
	renderingView->setFovX(Numeric::deg2rad(35));
	// we define the background as black (however, due to the video background its color will not be visible)
	renderingView->setBackgroundColor(RGBAColor(0, 0, 0));

	// we connect the view with the framebuffer
	framebuffer_->setView(renderingView);


	// we create a live video medium for our background (and e.g., for the tracker aquired by "BACKGROUND" in the TrackerTransform node)
	// as the name of the camera device may change we take the first one
	Media::FrameMediumRef liveVideo = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);

	if (liveVideo)
	{
		// as we have a valid video object we now can create the corresponding background, which will be added to the view
		Rendering::UndistortedBackgroundRef background = engine_->factory().createUndistortedBackground();

		if (background.isNull())
		{
			return;
		}

		const HomogenousMatrix4 device_T_display = framebuffer_->device_T_display();
		ocean_assert(device_T_display.isValid());

		const HomogenousMatrix4 display_T_camera = device_T_display.inverted() * HomogenousMatrix4(liveVideo->device_T_camera());

		Quaternion display_R_camera(display_T_camera.rotation());

		if ((display_R_camera * Vector3(0, 0, 1)) * Vector3(0, 0, 1) < 0)
		{
			// the camera is pointing towards the opposite direction of the display (e.g., user-facing camera)
			display_R_camera = Quaternion(Vector3(0, 1, 0), Numeric::pi()) * display_R_camera;
		}

		background->setOrientation(display_R_camera);

		// we define a preferred frame dimension (however, the camera device may provide a different dimension if this dimension is not suitable/available)
		liveVideo->setPreferredFrameDimension(1280, 720u);

		// we start the camera device
		liveVideo->start();

		// we connect the background to the live video
		background->setMedium(liveVideo);

		// we connect the background to the view
		renderingView->addBackground(background);
	}


	// now as we have set up the Augmented Reality environment we finally may load a scene
	// in our case the scene file defines the tracking logic so that we do not have to take care about any camera tracking

	// we seek for the resource file containing the scene
	const std::wstring path = Platform::Apple::Resource::resourcePath(L"dinosaur", L"ox3dv");

	if (path.empty())
	{
		return;
	}

	// we load the scene and create an abstract scene representation
	SceneDescription::SceneRef newScene = SceneDescription::Manager::get().load(String::toAString(path), engine_, Timestamp(true));

	if (newScene.isNull())
	{
		return;
	}

	if (newScene->descriptionType() == SceneDescription::TYPE_PERMANENT)
	{
		// a permanent scene is a scene, which should exist as long as the scene is rendererd (i.e., a scene description such as VRML97 or X3D - due to the interaction/animation logic defined in such scene descriptions)

		sceneDescription_ = newScene;
		ocean_assert(sceneDescription_);

		// the rendering scene object of the permanent scene description object can be added to the rendering framebuffer
		framebuffer_->addScene(sceneDescription_->renderingScene());
	}
	else
	{
		ocean_assert(newScene->descriptionType() == SceneDescription::TYPE_TRANSIENT);

		// a transient scene is a scene not defining any animation or interaction logic, thus, after creating a rendering scene graph we may throw the scene description away

		SceneDescription::SDLSceneRef sdlScene(newScene);
		ocean_assert(sdlScene);

		// we have a transient scene description object, thus we have to apply the description to the rendering engine explicitly
		Rendering::SceneRef renderingScene = sdlScene->apply(engine_);

		// if the created rendering scene object is valid, it may be added to the rendering framebuffer
		if (renderingScene)
		{
			framebuffer_->addScene(renderingScene);
		}
	}

	// we explicitly set the priority of the main thread below than normal as iOS seems to reduce the priority of the remaining threads otherwise
	Thread::setThreadPriority(Thread::PRIORTY_BELOW_NORMAL);
}

- (void)update
{
	if (framebuffer_ && engine_)
	{
		const Timestamp currentTimestamp(true);

		// we apply a pre-update for all scene description objects 'interested' in this event, as a result we determine the ideal timestamp for the update event
		const Timestamp updateTimestamp = SceneDescription::Manager::get().preUpdate(framebuffer_->view(), currentTimestamp);

		// now we update the scene description objects as well as the rendering engine with the timestamp as received by the pre-update
		// therefore, we can ensure that e.g., the most recent tracking result matches with the video frame used for rendering the background
		SceneDescription::Manager::get().update(framebuffer_->view(), updateTimestamp);

		engine_->update(updateTimestamp);
	}
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	if (framebuffer_.isNull())
	{
		return;
	}

	Rendering::PerspectiveViewRef renderingView = framebuffer_->view();

	if (renderingView.isNull() || self.view.bounds.size.height == 0)
	{
		return;
	}

	const float scale = float([UIScreen mainScreen].scale);

	const float widthPixels = float(rect.size.width) * scale;
	const float heightPixels = float(rect.size.height) * scale;

	ocean_assert(widthPixels == NumericF::floor(widthPixels));
	ocean_assert(heightPixels == NumericF::floor(heightPixels));

	if (widthPixels < 1.0f || heightPixels < 1.0f)
	{
		return;
	}

	const unsigned int viewportWidth = (unsigned int)(widthPixels);
	const unsigned int viewportHeight = (unsigned int)(heightPixels);

	framebuffer_->setViewport(0u, 0u, viewportWidth, viewportHeight);

	const Scalar aspect = Scalar(viewportWidth) / Scalar(viewportHeight);
	renderingView->setAspectRatio(aspect);

	// now we determine the ideal field of view for our device (the field of view that will be rendered should be slightly smaller than the field of view of the camera)
	const Scalar idealFovX = renderingView->idealFovX();

	if (idealFovX != renderingView->fovX())
	{
		renderingView->setFovX(idealFovX);
		Log::info() << "The field of view has been adjusted to " << Numeric::rad2deg(idealFovX) << " degree.";
	}

	// we simply invoke the rendering of the framebuffer
	framebuffer_->render();
}

@end
