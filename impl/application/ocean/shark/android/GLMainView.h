/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_GL_MAIN_VIEW_H
#define FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_GL_MAIN_VIEW_H

#include "application/ocean/shark/android/SharkAndroid.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/platform/android/application/GLFrameView.h"

#include "ocean/scenedescription/SceneDescription.h"

namespace Ocean
{

namespace Shark
{

namespace Android
{

/**
 * This class implements the main view of the shark viewer for android platforms.
 * @ingroup sharkandroid
 */
class GLMainView : public Platform::Android::Application::GLFrameView
{
	friend class Singleton<GLMainView>;

	private:

		/**
		 * Definition of a vector holding scene description scene ids.
		 */
		using SceneIds = std::vector<SceneDescription::SceneId>;

		/**
		 * Definition of a pair holding a scene filename and a replacement state.
		 */
		using SceneFilenamePair = std::pair<std::string, bool>;

		/**
		 * Definition of a vector holding scene filename pair objects.
		 */
		using SceneFilenamePairs = std::vector<SceneFilenamePair>;

	public:

		/**
		 * Initializes the view.
		 * @see GLFrameView.
		 */
		bool initialize() override;

		/**
		 * Releases the view.
		 * @return True, if succeeded
		 */
		bool release() override;

		/**
		 * Loads or adds a new scene.
		 * @param filename Filename of the scene to be added
		 * @param replace State determining whether already existing scenes will be replaced or not
		 * @return True, if succeeded
		 */
		bool loadScene(const std::string& filename, const bool replace);

		/**
		 * Unload all permanent scenes.
		 */
		bool unloadScenes();

		/**
		 * Renders the next frame.
		 * @see GLFrameView.
		 */
		bool render() override;

		/**
		 * Creates an instance of this object.
		 * @return The instance
		 */
		static inline Platform::Android::Application::GLView* createInstance();

	private:

		/**
		 * Creates a new main view object.
		 */
		GLMainView() = default;

		/**
		 * Destructs a main view object.
		 */
		~GLMainView() override;

	private:

		/// The pending scenes that will be loaded after the view has been initialized.
		SceneFilenamePairs pendingScenes_;

		/// Scene description ids of all loaded permanent scenes.
		SceneIds permanentSceneIds_;

		/// True, if the view has been initialized.
		bool initialized_ = false;

		/// High performance statistics objects.
		HighPerformanceStatistic performance_;

		/// Static helper variable ensuring that the instance function will be registered.
		const static bool instanceRegistered_;
};

inline Platform::Android::Application::GLView* GLMainView::createInstance()
{
	return new GLMainView();
}

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_SHARK_ANDROID_GL_MAIN_VIEW_H
