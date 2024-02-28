// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_NATIVE_APPLICATION_ADVANCED_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_NATIVE_APPLICATION_ADVANCED_H

#include "ocean/platform/meta/quest/vrapi/application/Application.h"
#include "ocean/platform/meta/quest/vrapi/application/VRControllerVisualizer.h"
#include "ocean/platform/meta/quest/vrapi/application/VRHandVisualizer.h"
#include "ocean/platform/meta/quest/vrapi/application/VRNativeApplication.h"

#include "ocean/platform/meta/quest/vrapi/HandPoses.h"

#include "ocean/rendering/Scene.h"

#include "ocean/scenedescription/SceneDescription.h"

#include <queue>
#include <unordered_map>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

/**
 * This class implements an advanced Quest (VR) application using Ocean's scene graph rendering pipline (Rendering::GLESceneGraph) using VrApi.
 * In addition to `VRNativeApplication`, this provides the following features
 * - Controller rendering
 * - Hand rendering
 * - Importers for 3D file formats
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT VRNativeApplicationAdvanced : public VRNativeApplication
{
	private:

		/**
		 * Definition of a pair holding a 3D model filename and an optional 6-DOF transformation.
		 */
		typedef std::pair<std::string, HomogenousMatrix4> ModelFilenamePair;

		/**
		 * Definition of a queue holding model filename pairs.
		 */
		typedef std::queue<ModelFilenamePair> ModelFilenamePairQueue;

		/**
		 * Definition of a queue holdingfilename.
		 */
		typedef std::queue<std::string> ModelFilenameQueue;

		/**
		 * Definition of a pair combining scene description and rendering scene ids.
		 */
		typedef std::pair<SceneDescription::SceneId, Rendering::ObjectId> SceneIdPair;

		/**
		 * Definition of a map mapping filenames to scene ids.
		 */
		typedef std::unordered_map<std::string, SceneIdPair> SceneFilenameMap;

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit VRNativeApplicationAdvanced(struct android_app* androidApp);

		/**
		 * Destructs this object.
		 */
		~VRNativeApplicationAdvanced() override;

	protected:

		/**
		 * Disabled copy constructor.
		 * @param nativeApplication Application object which would have been copied
		 */
		VRNativeApplicationAdvanced(const VRNativeApplicationAdvanced& nativeApplication) = delete;

		/**
		 * Loads a new 3D model file to the scene.
		 * @param modelFilename The filename of the 3D model to be loaded, must be valid
		 * @param world_T_model The transformation between model and world to be set after the model is loded, must be valid
		 * @see removeModel().
		 */
		void loadModel(std::string modelFilename, const HomogenousMatrix4& world_T_model = HomogenousMatrix4(true));

		/**
		 * Removes a 3D model file from the scene.
		 * @param modelFilename The filename of the 3D model to be removed, must be valid
		 */
		void removeModel(std::string modelFilename);

		/**
		 * The event function which is called when all resources should finally be released.
		 * @see NativeApplication::onReleaseResources().
		 */
		void onReleaseResources() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRNativeApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called when the VR mode has been entered.
		 * @see VRNativeApplication::onVrModeEntered().
		 */
		void onVrModeEntered() override;

		/**
		 * Events function called when the VR mode has been left.
		 * @see VRNativeApplication::onVrModeLeft().
		 */
		void onVrModeLeft() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const Timestamp& renderTimestamp) override;

		/**
		 * Event function called after a new 3D model has been loaded or has failed to load.
		 * @param modelFilename The filename of the model which has been loaded, will be valid
		 * @param scene The scene object of the new model, will be invalid if the model could not be loaded
		 */
		virtual void onModelLoaded(const std::string& modelFilename, const Rendering::SceneRef& scene);

		/**
		 * Event function called after a 3D model has been removed.
		 * @param modelFilename The filename of the model which has been removed, will be valid
		 */
		virtual void onModelRemoved(const std::string& modelFilename);

		/**
		 * Disabled copy operator.
		 * @param nativeApplication Application object which would have been copied
		 * @return Reference to this object
		 */
		VRNativeApplicationAdvanced& operator=(const VRNativeApplicationAdvanced& nativeApplication) = delete;

	protected:

		/// The visualizer for controllers.
		VRControllerVisualizer vrControllerVisualizer_;

		/// The visualizer for hands.
		VRHandVisualizer vrHandVisualizer_;

		/// The accessor for most recent hand poses.
		HandPoses handPoses_;

	private:

		/// The pending 3D model files which will be loaded once the access to the disk is granted.
		ModelFilenamePairQueue modelFilenameLoadQueue_;

		/// The pending 3D model files to be removed.
		ModelFilenameQueue modelFilenameRemoveQueue_;

		/// The map combining filenames with scene ids.
		SceneFilenameMap sceneFilenameMap_;

		/// The lock for the 3D model filename queue.
		Lock modelFilenameQueueLock_;
};

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_NATIVE_APPLICATION_ADVANCED_H
