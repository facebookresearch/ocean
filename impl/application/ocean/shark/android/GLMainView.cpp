/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/shark/android/GLMainView.h"

#include "ocean/base/String.h"

// #include "ocean/interaction/Manager.h"

#include "ocean/io/File.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/View.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/Scene.h"

namespace Ocean
{

namespace Shark
{

namespace Android
{

const bool GLMainView::viewInstanceRegistered = GLMainView::registerInstanceFunction(GLMainView::createInstance);

GLMainView::GLMainView() :
	viewInitialized(false)
{
	// nothing to do here
}

GLMainView::~GLMainView()
{
	release();
}

bool GLMainView::initialize()
{
	if (!GLFrameView::initialize())
		return false;

	ocean_assert(viewInitialized == false);
	viewInitialized = true;

	const ScopedLock scopedLock(lock);

	for (SceneFilenamePairs::const_iterator i = viewPendingScenes.begin(); i != viewPendingScenes.end(); ++i)
		loadScene(i->first, i->second);

	return true;
}

bool GLMainView::release()
{
	const ScopedLock scopedLock(lock);

	Log::info() << "Performance: " << viewPerformance.averageMseconds() << "ms / frame";

	Timestamp timestampNow(true);

	Log::info() << "Render iterations " << viewRenderingIterations;
	Log::info() << "Real performance: " << String::toAString(1000.0 * double(timestampNow - viewRenderingStartTimestamp) / max(1.0, double(viewRenderingIterations)), 8) << "ms / frame";

	unloadScenes();

	return GLFrameView::release();
}

bool GLMainView::loadScene(const std::string& filename, const bool replace)
{
	const ScopedLock scopedLock(lock);

	if (!viewInitialized)
	{
		if (replace)
			viewPendingScenes = SceneFilenamePairs(1, SceneFilenamePair(filename, replace));
		else
			viewPendingScenes.push_back(SceneFilenamePair(filename, replace));

		Log::info() << "The scene \"" << filename << "\" will be loaded after the view has been initialized.";

		return true;
	}

	if (viewFramebuffer.isNull() || viewEngine.isNull())
	{
		Log::error() << "Failed to load scene file: No valid framebuffer!";
		return false;
	}

	try
	{
		if (replace)
			unloadScenes();

		const IO::File file(filename);

		// currently not necessary:
		//if (Interaction::Manager::get().isSupported(file.extension()))
		//	if (Interaction::Manager::get().load(viewEngine, Timestamp(true), filename))
		//		return true;

		const SceneDescription::SceneRef scene = SceneDescription::Manager::get().load(filename, viewEngine, Timestamp(true));
		if (scene)
		{
			if (scene->descriptionType() == SceneDescription::TYPE_PERMANENT)
			{
				const SceneDescription::SDXSceneRef sdxScene(scene);
				ocean_assert(sdxScene);

				viewPermanentSceneIds.push_back(sdxScene->sceneId());

				viewFramebuffer->addScene(sdxScene->renderingScene());
			}
			else
			{
				SceneDescription::SDLSceneRef sdlScene(scene);
				ocean_assert(sdlScene);

				const Rendering::SceneRef renderingScene(sdlScene->apply(viewEngine));

				if (renderingScene)
					viewFramebuffer->addScene(renderingScene);
				else
				{
					Log::error() << "Failed to apply the scene description to the rendering engine";
					return false;
				}
			}
		}
		else
		{
			Log::error() << "Could not load scene description file";
			return false;
		}
	}
	catch(const Exception& exception)
	{
		Log::error() << exception.what();
		return false;
	}
	catch(...)
	{
		Log::error() << "Uncaught exception occured!";
		return false;
	}

	return true;
}

bool GLMainView::unloadScenes()
{
	const ScopedLock scopedLock(lock);

	bool result = true;

	try
	{
		for (SceneIds::const_iterator i = viewPermanentSceneIds.begin(); i != viewPermanentSceneIds.end(); ++i)
			if (SceneDescription::Manager::get().unload(*i))
				Log::info() << "Successfully unloaded scene with id: " << *i;
			else
			{
				Log::error() << "Failed to unload scene with id: " << *i;
				result = false;
			}

		viewPermanentSceneIds.clear();

		if (viewFramebuffer)
			viewFramebuffer->clearScenes();
	}
	catch(...)
	{
		Log::error() << "Uncaught exception occured!";
	}

	return result;
}

bool GLMainView::render()
{
	const ScopedLock scopedLock(lock);

	if (viewEngine.isNull() || viewFramebuffer.isNull())
		return false;

	if (viewRenderingStartTimestamp.isInvalid())
		viewRenderingStartTimestamp.toNow();

	++viewRenderingIterations;

	try
	{
		if (adjustFovXToBackground)
		{
			const Rendering::PerspectiveViewRef perspectiveView(viewFramebuffer->view());
			if (perspectiveView)
			{
				bool validCamera = false;
				const Scalar idealFovX = perspectiveView->idealFovX(&validCamera);

				if (validCamera)
				{
					perspectiveView->setFovX(idealFovX);
				}
			}
		}

		// starts performance measurement
		viewPerformance.start();

		// Applying specific pre-updates on all scene descriptions necessary before the interaction plugins are applied
		Timestamp updateTimestamp = SceneDescription::Manager::get().preUpdate(viewFramebuffer->view(), Timestamp(true));

		// Applying the pre update function of the interaction plugins
		// updateTimestamp = Interaction::Manager::get().preUpdate(viewEngine, updateTimestamp);

		// Applying updates on all scene descriptions
		SceneDescription::Manager::get().update(viewFramebuffer->view(), updateTimestamp);

		// Updates the rendering engine
		viewEngine->update(updateTimestamp);

		// Applying post update function of the interaction plugins
		// Interaction::Manager::get().postUpdate(viewEngine, updateTimestamp);

		// Rendering the current frame
		viewFramebuffer->render();

		// stops the performance measurement
		viewPerformance.stop();

		return true;
	}
	catch(const Exception& exception)
	{
		Log::error() << exception.what();
	}
	catch(...)
	{
		Log::error() << "Uncaught exception occured during rendering!";
	}

	return false;
}

}

}

}
