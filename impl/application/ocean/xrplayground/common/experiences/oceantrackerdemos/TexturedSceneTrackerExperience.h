// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_TEXTURED_SCENE_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_TEXTURED_SCENE_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Group.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/tracking/maptexturing/TextureGenerator.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience for a scene tracker.
 * @ingroup xrplayground
 */
class TexturedSceneTrackerExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/**
		 * Definition of individual texture processor states.
		 */
		enum ProcessorState : uint32_t
		{
			/// The processor is currently stopped.
			PS_STOPPED = 0u,
			/// The processor is currently idling.
			PS_IDLE,
			/// The processor is updating the mesh.
			PS_UPDATING_MESH,
			/// The processor has updated the mesh.
			PS_MESH_UPDATED,
			/// The processor is exporting the mesh.
			PS_MESH_EXPORTING,
			/// The processor is processing the current frame.
			PS_PROCESS_FRAME
		};

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * Definition of a class holding a mesh.
		 */
		class MeshRenderingObject
		{
			public:

				/**
				 * Creates a new mesh rendering object.
				 * @param engine The rendering engine to be used
				 */
				MeshRenderingObject(const Rendering::Engine& engine);

				/**
				 * Updates the rendering object with a textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param textureCoordinates The texture coordinates, one for each vetex
				 * @param triangleFaces The triangle faces
				 * @param textureFrame The texture
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors2& textureCoordinates, const Rendering::TriangleFaces& triangleFaces, Frame&& textureFrame);

				/**
				 * Updates the rendering object with a non-textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param perVertexNormals The texture coordinates, one for each vetex
				 * @param triangleFaces The triangle faces
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const Rendering::TriangleFaces& triangleFaces);

				/**
				 * Updates the rendering object with a non-textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param perVertexNormals The texture coordinates, one for each vetex
				 * @param triangleFaceIndices The indices of the triangle faces
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const Indices32& triangleFaceIndices);

				/**
				 * Updates the rendering object with a non-textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param triangleFaces The triangle faces
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces);

				/**
				 * Returns the object's Transform node.
				 * @return The Transform node
				 */
				inline Rendering::TransformRef transform() const;

			protected:

				/// The VertexSet object.
				Rendering::VertexSetRef vertexSet_;

				/// The Triangles object.
				Rendering::TrianglesRef triangles_;

				/// The FrameTexture2D object for textured meshes.
				Rendering::FrameTexture2DRef texture_;

				/// The Material object for non-textured meshes.
				Rendering::MaterialRef material_;

				/// The transform object holding the geometry.
				Rendering::TransformRef transform_;
		};

		/**
		 * Definition of an unordered map mapping unique mesh ids to mesh objects.
		 */
		typedef std::unordered_map<Index32, MeshRenderingObject> MeshRenderingObjectMap;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~TexturedSceneTrackerExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Starts the scanning and texturing process.
		 * @return True, if succeeded
		 */
		bool start();

		/**
		 * Stops the scanning and texturing process.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Exports the current mesh.
		 * @param path The full path where to save the mesh
		 * @return True, if succeeded
		 */
		bool exportMesh(const std::string& path);

		/**
		 * Creates a new GravityAndHeadingNativeExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		TexturedSceneTrackerExperience() = default;

		/**
		 * The thread's run function.
		 */
		void threadRun() override;

		/**
		 * Event function for new tracking samples from the scene tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from scene tracker
		 */
		void onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Shows the user interaction on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 */
		void showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface);

		/**
		 * Unloads the user interaction on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 */
		void unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface);

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	protected:

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The scene tracker providing access to be used in this experience.
		Devices::SceneTracker6DOFRef sceneTracker6DOF_;

		/// The object id of the scene tracker.
		Devices::SceneTracker6DOF::ObjectId objectId_ = Devices::SceneTracker6DOF::invalidObjectId();

		/// The subscription object for sample events from the scene tracker.
		Devices::Measurement::SampleEventSubscription sceneTrackerSampleEventSubscription_;

		/// The rendering group node holding the rendering content.
		Rendering::GroupRef renderingGroup_;

		/// The most recent scene element to process, nullptr otherwise.
		Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement_;

		/// The recent transformation between camera and world.
		HomogenousMatrix4 world_T_recentCamera_ = HomogenousMatrix4(false);

		/// The map mapping mesh ids to mesh objects.
		MeshRenderingObjectMap meshRenderingObjectMap_;

		/// The timestamp when the last scene element was updated.
		Timestamp recentSceneElementTimestamp_;

		/// The timestamp when the last rendering object was updated.
		Timestamp renderingObjectTimestamp_;

		/// The frame medium of the video background.
		Media::FrameMediumRef frameMedium_;

		/// The texture generator.
		Tracking::MapTexturing::TextureGenerator textureGenerator_;

		/// The timestamp of the last frame which has been processed.
		Timestamp lastProcessedFrameTimestamp_;

		/// The current processor state.
		ProcessorState processorState_ = PS_STOPPED;

		/// The experience's lock.
		Lock lock_;
};

inline Rendering::TransformRef TexturedSceneTrackerExperience::MeshRenderingObject::transform() const
{
	return transform_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_TEXTURED_SCENE_TRACKER_EXPERIENCE_H
