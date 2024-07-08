/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_NATIVE_APPLICATION_ADVANCED_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_NATIVE_APPLICATION_ADVANCED_H

#include "ocean/platform/meta/quest/openxr/application/Application.h"
#include "ocean/platform/meta/quest/openxr/application/VRControllerVisualizer.h"
#include "ocean/platform/meta/quest/openxr/application/VRHandVisualizer.h"
#include "ocean/platform/meta/quest/openxr/application/VRNativeApplication.h"

#include "ocean/platform/meta/quest/openxr/HandPoses.h"
#include "ocean/platform/meta/quest/openxr/Passthrough.h"

#include "ocean/rendering/Scene.h"

#include "ocean/scenedescription/SceneDescription.h"

#include <queue>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

/**
 * This class implements an advanced Oculus (VR) application using Ocean's scene graph rendering pipline (Rendering::GLESceneGraph).
 * In addition to `VRNativeApplication`, this provides the following features
 * - Controller rendering
 * - Hand rendering
 * - Importers for 3D file formats
 * - Passthrough
 * @ingroup platformmetaquestopenxrapplication
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT VRNativeApplicationAdvanced : public VRNativeApplication
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

		/**
		 * Loads a new 3D model file to the scene.
		 * Once the scene is loaded, the onModelLoaded() event function will be called.
		 * @param modelFilename The filename of the 3D model to be loaded, must be valid
		 * @param world_T_model The transformation between model and world to be set after the model is loded, must be valid
		 * @see removeModel(), onModelLoaded().
		 */
		void loadModel(std::string modelFilename, const HomogenousMatrix4& world_T_model = HomogenousMatrix4(true));

		/**
		 * Removes a 3D model file from the scene.
		 * Once the scene is removed, the onModelRemoved() event function will be called.
		 * @param modelFilename The filename of the 3D model to be removed, must be valid
		 * @see loadModel(), onModelRemoved().
		 */
		void removeModel(std::string modelFilename);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param nativeApplication Application object which would have been copied
		 */
		VRNativeApplicationAdvanced(const VRNativeApplicationAdvanced& nativeApplication) = delete;

		/**
		 * Returns the names of the necessary OpenXR extensions the application needs.
		 * @see NativeApplication::necessaryOpenXRExtensionNames().
		 */
		StringSet necessaryOpenXRExtensionNames() const override;

		/**
		 * Removes all 3D scene models from the scenegraph which are queued to be removed.
		 */
		virtual void handleModelRemoveQueue();

		/**
		 * Removes a loaded 3D scene model from the scenegraph.
		 * @param sceneIdPair The pair combining the scene id and rendering object id of a model to be removed, must be valid
		 * @param filename The filename of the 3D model to be removed, must be valid
		 */
		virtual void invokeRemoveModel(const SceneIdPair& sceneIdPair, const std::string& filename);

		/**
		 * Loads all 3D scene models into the scenegraph which are queued to be loaded.
		 * @param predictedDisplayTime The timestamp which will be used for rendering, must be valid
		 */
		virtual void handleModelLoadQueue(const Timestamp& predictedDisplayTime);

		/**
		 * Event function called whenever the session is ready, when the session state changed to XR_SESSION_STATE_READY.
		 * @see VRNativeApplication::onOpenXRSessionReady().
		 */
		void onOpenXRSessionReady() override;

		/**
		 * Event function called whenever the session is stopping, when the session state changed to XR_SESSION_STATE_STOPPING.
		 * @see RNativeApplication::onOpenXRSessionStopping().
		 */
		void onOpenXRSessionStopping() override;

		/**
		 * The event function which is called when all resources should finally be released.
		 * @see NativeApplication::onReleaseResources().
		 */
		void onReleaseResources() override;

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRNativeApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRNativeApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Event function allows to add custom compositor layers at the very back.
		 * @see VRNativeApplication::onAddCompositorBackLayers().
		 */
		void onAddCompositorBackLayers(XrCompositorLayerUnions& xrCompositorLayerUnions) override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplication::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

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

		/// The passhrough object.
		Passthrough passthrough_;

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

#ifdef OCEAN_PLATFORM_QUEST_OPENXR_APPLICATION_USE_EXTERNAL_RESOURCES

/**
 * Registers external resources.
 * @ingroup platformmetaquestopenxrapplication
 */
void VRNativeApplicationAdvanced_registerExternalResources();

/**
 * Un-registers external resources.
 * @ingroup platformmetaquestopenxrapplication
 */
void VRNativeApplicationAdvanced_unregisterExternalResources();

#endif

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_NATIVE_APPLICATION_ADVANCED_H
