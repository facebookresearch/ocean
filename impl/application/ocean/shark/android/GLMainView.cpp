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
	{
		return false;
	}

	ocean_assert(viewInitialized == false);
	viewInitialized = true;

	const ScopedLock scopedLock(lock_);

	for (SceneFilenamePairs::const_iterator i = viewPendingScenes.begin(); i != viewPendingScenes.end(); ++i)
	{
		loadScene(i->first, i->second);
	}

	return true;
}

bool GLMainView::release()
{
	const ScopedLock scopedLock(lock_);

	Log::info() << "Performance: " << viewPerformance.averageMseconds() << "ms / frame";

	Timestamp timestampNow(true);

	Log::info() << "Render iterations " << renderingIterations_;
	Log::info() << "Real performance: " << String::toAString(1000.0 * double(timestampNow - renderingStartTimestamp_) / max(1.0, double(renderingIterations_)), 8) << "ms / frame";

	unloadScenes();

	return GLFrameView::release();
}

bool GLMainView::loadScene(const std::string& filename, const bool replace)
{
	const ScopedLock scopedLock(lock_);

	if (!viewInitialized)
	{
		if (replace)
		{
			viewPendingScenes = SceneFilenamePairs(1, SceneFilenamePair(filename, replace));
		}
		else
		{
			viewPendingScenes.push_back(SceneFilenamePair(filename, replace));
		}

		Log::info() << "The scene \"" << filename << "\" will be loaded after the view has been initialized.";

		return true;
	}

	if (framebuffer_.isNull() || engine_.isNull())
	{
		Log::error() << "Failed to load scene file: No valid framebuffer!";
		return false;
	}

	try
	{
		if (replace)
		{
			unloadScenes();
		}

		const IO::File file(filename);

		const SceneDescription::SceneRef scene = SceneDescription::Manager::get().load(filename, engine_, Timestamp(true));

		if (scene)
		{
			if (scene->descriptionType() == SceneDescription::TYPE_PERMANENT)
			{
				const SceneDescription::SDXSceneRef sdxScene(scene);
				ocean_assert(sdxScene);

				viewPermanentSceneIds.push_back(sdxScene->sceneId());

				framebuffer_->addScene(sdxScene->renderingScene());
			}
			else
			{
				SceneDescription::SDLSceneRef sdlScene(scene);
				ocean_assert(sdlScene);

				const Rendering::SceneRef renderingScene(sdlScene->apply(engine_));

				if (renderingScene)
				{
					framebuffer_->addScene(renderingScene);
				}
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
	const ScopedLock scopedLock(lock_);

	bool result = true;

	try
	{
		for (SceneIds::const_iterator i = viewPermanentSceneIds.begin(); i != viewPermanentSceneIds.end(); ++i)
		{
			if (SceneDescription::Manager::get().unload(*i))
			{
				Log::info() << "Successfully unloaded scene with id: " << *i;
			}
			else
			{
				Log::error() << "Failed to unload scene with id: " << *i;
				result = false;
			}
		}

		viewPermanentSceneIds.clear();

		if (framebuffer_)
		{
			framebuffer_->clearScenes();
		}
	}
	catch(...)
	{
		Log::error() << "Uncaught exception occured!";
	}

	return result;
}

bool GLMainView::render()
{
	const ScopedLock scopedLock(lock_);

	if (engine_.isNull() || framebuffer_.isNull())
	{
		return false;
	}

	if (renderingStartTimestamp_.isInvalid())
	{
		renderingStartTimestamp_.toNow();
	}

	++renderingIterations_;

	try
	{
		if (adjustFovXToBackground_)
		{
			const Rendering::PerspectiveViewRef perspectiveView(framebuffer_->view());

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
		Timestamp updateTimestamp = SceneDescription::Manager::get().preUpdate(framebuffer_->view(), Timestamp(true));

		// Applying the pre update function of the interaction plugins
		// updateTimestamp = Interaction::Manager::get().preUpdate(engine_, updateTimestamp);

		// Applying updates on all scene descriptions
		SceneDescription::Manager::get().update(framebuffer_->view(), updateTimestamp);

		// Updates the rendering engine
		engine_->update(updateTimestamp);

		// Applying post update function of the interaction plugins
		// Interaction::Manager::get().postUpdate(engine_, updateTimestamp);

		// Rendering the current frame
		framebuffer_->render();

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
