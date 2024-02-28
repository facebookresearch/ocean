// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OBJECT_CAPTURE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OBJECT_CAPTURE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/Triangles.h"

namespace vogon
{
	/// Forward-declaration
	class MeshBase;
}

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to create meshes with Object Capture.
 * @ingroup xrplayground
 */
class ObjectCaptureExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~ObjectCaptureExperience() override;

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
		 * Starts the scanning process.
		 * @param objectSize Indicator for the object size that will be captured, valid values: 0 - small, 1 - medium, 2 - large
		 * @return True, if succeeded
		 */
		bool start(const unsigned int objectSize);

		/**
		 * Stops the scanning process.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Exports the latest mesh to a local file on device.
		 * @param filename The name of the exported file, must be valid
		 * @return True, if succeeded
		 */
		bool exportMesh(const std::string& filename);

		/**
		 * Creates a new GravityAndHeadingNativeExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		ObjectCaptureExperience() = default;

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

		/**
		 * The thread run function.
		 */
		void threadRun() override;

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

		/**
		 * Extracts mesh information from a Vogon mesh
		 * @param vogonMesh The mesh object from which the vertices and other mesh information will be extracted
		 * @param vertices The resulting list of mesh vertices
		 * @param triangleFaces The resulting list of triangle indices
		 * @param vertexNormals The resulting list of vertex normals
		 * @param vertexColors The optionally resulting list of vertex colors, will be ignored if `nullptr`
		 * @return True on success, otherwise false
		 */
		static bool extractFromVogonMesh(const vogon::MeshBase& vogonMesh, Vectors3& vertices, Rendering::TriangleFaces& triangleFaces, Vectors3& vertexNormals, RGBAColors* vertexColors = nullptr);

	protected:

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The scene tracker providing access to be used in this experience.
		Devices::SceneTracker6DOFRef sceneTracker6DOF_;

		/// The frame medium used for tracking.
		Media::FrameMediumRef frameMedium_;

		/// The object id of the scene tracker.
		Devices::SceneTracker6DOF::ObjectId objectId_ = Devices::SceneTracker6DOF::invalidObjectId();

		/// The subscription object for sample events from the scene tracker.
		Devices::Measurement::SampleEventSubscription sceneTrackerSampleEventSubscription_;

		/// The rendering group node holding the rendering content.
		Rendering::GroupRef renderingGroup_;

		/// The recent triangle faces extracted from VOGON.
		Rendering::TriangleFaces recentTriangleFaces_;

		/// The recent vertices extractd from VOGON.
		Vectors3 recentVertices_;

	 	/// The recent per-vertex normals.
		Vectors3 recentPerVertexNormals_;

		/// The recent per-vertex colors;
		RGBAColors recentPerVertexColors_;

		/// The final texture atlas that object capture provided
		Frame finalTextureAtlas_;

		/// The final texture coordinates that object capture provided (one per mesh vertex)
		Vectors2 finalTextureCoordinates_;

		/// True, object capture finished the reconstruction
		std::atomic<bool> isFinalReconstruction_ = false;

		/// The experience's lock.
		Lock lock_;

		/// Indicator for the object size that will be captured: 0 - small, 1 - medium, 2 - large
		unsigned int objectSize_ = 1u;

		/// True, if the mesh generation is active.
		std::atomic<bool> isActive_ = false;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OBJECT_CAPTURE_H
