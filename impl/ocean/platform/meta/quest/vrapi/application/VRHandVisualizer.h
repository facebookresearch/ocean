// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_HAND_VISUALIZER_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_HAND_VISUALIZER_H

#include "ocean/platform/meta/quest/vrapi/application/Application.h"

#include "ocean/math/Vector4.h"

#include "ocean/platform/meta/quest/application/VRVisualizer.h"

#include "ocean/platform/meta/quest/vrapi/HandPoses.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/VertexSet.h"

#include <VrApi_Types.h>

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
 * This class implements helper functions allowing to visualize the hand mesh used by hand tracking (Nimble) in an Ocean-based VR application (e.g., VRNativeApplication).
 * Usage example:
 * <pre>
 * class YourApplication : public VRNativeApplication
 * {
 *      ...
 *      VRHandVisualizer handVisualizer_;
 *      ...
 * };
 *
 * YourApplication::onVrModeEntered()
 * {
 *      ...
 *      handVisualizer_ = VRHandVisualizer(engine_, framebuffer_, ovrMobile_);
 *      ...
 * }
 *
 * YourApplication::onVrModeLeft()
 * {
 *      ...
 *      handVisualizer_ = VRHandVisualizer();
 *      ...
 * }
 * </pre>
 * @see VRNativeApplication.
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT VRHandVisualizer : public Quest::Application::VRVisualizer
{
	public:

		/**
		 * This class implements a scoped state object allowing to reset all states of a visualizer.
		 * The state can be stored locally or states can be pushed onto a stack.
		 * @see pushState(), popState().
		 */
		class ScopedState
		{
			public:

				/**
				 * Default constructor.
				 */
				ScopedState() = default;

				/**
				 * Creates a new state object.
				 * @param vrHandVisualizer The visualizer to which this new object belongs
				 */
				ScopedState(VRHandVisualizer& vrHandVisualizer);

				/**
				 * Move constructor.
				 * @param scopedState The state object to be moved
				 */
				ScopedState(ScopedState&& scopedState);

				/**
				 * Destructs this object and releases the state.
				 */
				~ScopedState();

				/**
				 * Explicitly releases this state.
				 * The properties of the owning visualizer will be reset to the situation when the state was created.
				 */
				void release();

				/**
				 * Move operator.
				 * @param scopedState The state object to be moved
				 * @return Reference to this object
				 */
				ScopedState& operator=(ScopedState&& scopedState);

			protected:

				/**
				 * Disabled copy constructor.
				 */
				ScopedState(const ScopedState&) = delete;

				/**
				 * Disabled assign operator.
				 * @return The reference to this object
				 */
				ScopedState& operator=(const ScopedState&) = delete;

			protected:

				/// The visualizer to which this state object belongs.
				VRHandVisualizer* vrHandVisualizer_ = nullptr;

				/// True, if the hands where shown when this object was created.
				bool wasShown_ = false;

				/// The transparency when this object was created.
				Scalar transparency_ = Scalar(0.6);
		};

	protected:

		/// The default hand transparency.
		static constexpr Scalar defaultTransparency_ = Scalar(0.6);

		/// Typedef for blend indices (4-tuple)
		typedef VectorT4<Index32> BlendIndex4;

		/// Typedef for vectors of blend indices
		typedef std::vector<BlendIndex4> BlendIndices4;

		/// Indetifier for invalid blend indices
		static constexpr Index32 invalidBlendIndex = Index32(-1);

		/**
		 * Identifier for the different hands
		 */
		enum HandType : uint32_t
		{
			/// Identifier of the left hand
			CT_LEFT = 0u,
			/// Identifier of the right hand
			CT_RIGHT
		};

		/**
		 * Collection of raw mesh information
		 */
		class HandMesh
		{
			public:

				/**
				 * Default constructor
				 */
				 HandMesh() = default;

				/**
				 * Constructor to create an instance of a hand mesh
				 * @param vertices The vertices of the mesh that will be stored, at least 3
				 * @param normals The normals of the mesh that will be stored, at least 3
				 * @param triangles The vertex indices that define the triangles of the mesh, at least 1
				 * @param textureCoordinates The texture coordinates for each of the mesh, must be valid and have the same number of elements as there are vertices
				 * @param blendIndices The indices (4-tuples) of the hand skeleton joints (cf. `HandPoses::Joints`) that will be used to interpolate the locations of the vertices of the mesh (skeletal animation), size must equal the number of vertices, unused elements of the tuples must be set to `invalidBlendIndex`
				 * @param blendWeights The weights used to interpolate the locations of the mesh vertices (4-tuples, values must be >= 0 and sum to 1), size must be equal to the number of vertices
				 * @sa HandPoses::Joints
				 */
				inline HandMesh(Vectors3&& vertices, Vectors3&& normals, Rendering::TriangleFaces&& triangles, Rendering::TextureCoordinates&& textureCoordinates, BlendIndices4&& blendIndices, Vectors4&& blendWeights);

				/**
				 * Returns true if this instance stores mesh data and if data is of the expected size.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				* Returns a const reference to the stored vertices of the mesh
				* @return The vertices
				*/
				inline const Vectors3& vertices() const;

				/**
				* Returns a const reference to the normals of the mesh
				* @return The normals
				*/
				inline const Vectors3& normals() const;

				/**
				* Returns a const reference to the triangles of the mesh
				* @return The triangles
				*/
				inline const Rendering::TriangleFaces& triangles() const;

				/**
				* Returns a const reference to the texture coordinates of the mesh
				* @return The texture coordinates
				*/
				inline const Rendering::TextureCoordinates& textureCoordinates() const;

				/**
				* Returns a const reference to the blend indices of the mesh
				* @return The blend indices
				*/
				inline const BlendIndices4& blendIndices() const;

				/**
				* Returns a const reference to the blend weights of the mesh
				* @return The blend weights
				*/
				inline const Vectors4& blendWeights() const;

			protected:

				/// The vertices of the mesh
				Vectors3 vertices_;

				/// The normals of the mesh
				Vectors3 normals_;

				/// The triangles of the mesh
				Rendering::TriangleFaces triangles_;

				/// The texture coordinates of the mesh
				Rendering::TextureCoordinates textureCoordinates_;

				/// The blend indices of the mesh
				BlendIndices4 blendIndices_;

				/// The blend weights of the mesh
				Vectors4 blendWeights_;
		};

		/**
		 * Definition of a vector holding state objects.
		 */
		typedef std::vector<ScopedState> ScopedStates;

	public:

		/**
		 * Default constructor, creates a new invalid visualizer.
		 */
		VRHandVisualizer() = default;

		/**
		 * Creates a new hand visualizer and initializes the object with a given rendering engine and associated framebuffer
		 * Rendering engine and framebuffer are necessary so that the rendering objects (like Scene, Transform, Texture2D) can be created and attached to the existing rendering objects.
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 * @param ovr The ovrMobile object, must be valid
		 */
		inline VRHandVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, ovrMobile* ovr);

		/**
		 * Move constructor
		 * @param vrHandVisualizer Another instance of the hand visualizer that will be moved to this instance
		 */
		inline VRHandVisualizer(VRHandVisualizer&& vrHandVisualizer);

		/**
		 * Disable the copy constructor
		 * @param vrHandVisualizer Another instance of the hand visualizer that would have been copied to this instance
		 */
		inline VRHandVisualizer(const VRHandVisualizer& vrHandVisualizer) = delete;

		/**
		 * Destructor
		 */
		inline ~VRHandVisualizer();

		/**
		 * Shows the controllers.
		 * @see hide().
		 */
		inline void show();

		/**
		 * Hides the controllers.
		 * @see show().
		 */
		inline void hide();

		/**
		 * Returns whether the controllers are shown.
		 * @return True, if succeeded
		 */
		inline bool isShown() const;

		/**
		 * Sets the hand's transparency.
		 * @param transparency The transparency to be used when rendering the hands, 0 is fully opaque, 1 is fully transparent, with range [0, 1]
		 * @return True, if succeeded
		 * @see transparency().
		 */
		bool setTransparency(const Scalar transparency);

		/**
		 * Returns the hand's transparency.
		 * @return The transparency which is used when rendering the hands, with range [0, 1]
		 * @see setTransparency().
		 */
		Scalar transparency() const;

		/**
		 * Visualizes the hand meshes at the location that the hand tracking (Nimble) reports (defined in relation to the world).
		 * Note: you need to call `handPoses.update()` before calling this function
		 * @param handPoses An instance of the hand poses object, must be valid
		 * @param show True, to show the hands; False, to hide the hands
		 */
		void visualizeHandsInWorld(const HandPoses& handPoses, const bool show = true);

		/**
		 * Pushes a new configuration state to the stack.
		 * Each push needs to be balanced with a pop.
		 * @see popState().
		 */
		inline void pushState();

		/**
		 * Pops the most recent state from the stack and resets the visualizer's configuration accordingly.
		 */
		inline void popState();

		/**
		 * Disable the copy-assignment operator
		 * @param vrHandVisualizer Another instance of the hand visualizer that would have been copied to this instance
		 * @return A reference to this instance of the hand visualizer that would have been returned
		 */
		inline VRHandVisualizer& operator=(VRHandVisualizer& vrHandVisualizer) = delete;

		/**
		 * Move-assignment operator
		 * @param vrHandVisualizer Another instance of the hand visualizer that will be moved to this instance
		 * @return A reference to this instance of the hand visualizer
		 */
		inline VRHandVisualizer& operator=(VRHandVisualizer&& vrHandVisualizer);

	protected:

		/**
		 * Loads the hand mesh from VrApi
		 * @param ovr The OVR SDK object, must be valid
		 * @return True if the hand meshes were loaded successfully
		 */
		bool loadHandMeshes(ovrMobile* ovr);

	protected:

		/// The scene object of the renderer;
		Rendering::SceneRef scene_;

		/// Mesh of the left hand
		HandMesh leftHandMesh_;

		/// Mesh of the right hand
		HandMesh rightHandMesh_;

		/// The geometry object of the left hand
		Rendering::GeometryRef leftHandMeshGeometry_;

		/// The geometry object of the right hand
		Rendering::GeometryRef rightHandMeshGeometry_;

		/// The vertex set of the left hand (required to animate the mesh)
		Rendering::VertexSetRef leftVertexSet_;

		/// The vertex set of the right hand (required to animate the mesh)
		Rendering::VertexSetRef rightVertexSet_;

		/// The rendering Material node associated with the left hand.
		Rendering::MaterialRef leftMaterial_;

		/// The rendering Material node associated with the right hand.
		Rendering::MaterialRef rightMaterial_;

		/// The transparency which is used to render the hands, with range [0, 1]
		Scalar transparency_ = defaultTransparency_;

		/// The inverse of the left hand rest poses (or bind matrices)
		HomogenousMatrices4 leftJointRestPoses_T_rootJoint_;

		/// The inverse of the right hand rest poses (or bind matrices)
		HomogenousMatrices4 rightJointRestPoses_T_rootJoint_;

		/// True, if the controllers are visualized; False, if the controllers are hidden.
		std::atomic<bool> isShown_ = true;

		/// A stack of visualization states.
		ScopedStates stateStack_;
};

inline VRHandVisualizer::HandMesh::HandMesh(Vectors3&& vertices, Vectors3&& normals, Rendering::TriangleFaces&& triangles, Rendering::TextureCoordinates&& textureCoordinates, BlendIndices4&& blendIndices, Vectors4&& blendWeights) :
	vertices_(std::move(vertices)),
	normals_(std::move(normals)),
	triangles_(std::move(triangles)),
	textureCoordinates_(std::move(textureCoordinates)),
	blendIndices_(std::move(blendIndices)),
	blendWeights_(std::move(blendWeights))
{
	// nothing else to do
}

inline bool VRHandVisualizer::HandMesh::isValid() const
{
	return !vertices_.empty()
		&& vertices_.size() == normals_.size()
		&& vertices_.size() == textureCoordinates_.size()
		&& vertices_.size() == blendIndices_.size()
		&& vertices_.size() == blendWeights_.size()
		&& !triangles_.empty();
}

inline const Vectors3& VRHandVisualizer::HandMesh::vertices() const
{
	return vertices_;
}

inline const Vectors3& VRHandVisualizer::HandMesh::normals() const
{
	return normals_;
}

inline const Rendering::TriangleFaces& VRHandVisualizer::HandMesh::triangles() const
{
	return triangles_;
}

inline const Rendering::TextureCoordinates& VRHandVisualizer::HandMesh::textureCoordinates() const
{
	return textureCoordinates_;
}

inline const VRHandVisualizer::BlendIndices4& VRHandVisualizer::HandMesh::blendIndices() const
{
	return blendIndices_;
}

inline const Vectors4& VRHandVisualizer::HandMesh::blendWeights() const
{
	return blendWeights_;
}

inline VRHandVisualizer::VRHandVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, ovrMobile* ovr) :
	VRVisualizer(engine, framebuffer),
	isShown_(true)
{
	if (!loadHandMeshes(ovr))
	{
		ocean_assert(false && "This should never happen!");
	}
}

inline VRHandVisualizer::VRHandVisualizer(VRHandVisualizer&& vrHandVisualizer) :
	VRHandVisualizer()
{
	*this = std::move(vrHandVisualizer);
}

inline VRHandVisualizer::~VRHandVisualizer()
{
	if (scene_)
	{
		framebuffer_->removeScene(scene_);
	}
}

inline void VRHandVisualizer::show()
{
	isShown_ = true;
}

inline void VRHandVisualizer::hide()
{
	isShown_ = false;
}

inline bool VRHandVisualizer::isShown() const
{
	return isShown_;
}

inline void VRHandVisualizer::pushState()
{
	const ScopedLock scopedLock(lock_);

	stateStack_.emplace_back(*this);
}

inline void VRHandVisualizer::popState()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!stateStack_.empty());

	stateStack_.pop_back();
}

inline VRHandVisualizer& VRHandVisualizer::operator=(VRHandVisualizer&& vrHandVisualizer)
{
	if (this != &vrHandVisualizer)
	{
		scene_ = std::move(vrHandVisualizer.scene_);

		leftHandMesh_ = std::move(vrHandVisualizer.leftHandMesh_);
		rightHandMesh_ = std::move(vrHandVisualizer.rightHandMesh_);

		leftHandMeshGeometry_ = std::move(vrHandVisualizer.leftHandMeshGeometry_);
		rightHandMeshGeometry_ = std::move(vrHandVisualizer.rightHandMeshGeometry_);

		leftVertexSet_ = std::move(vrHandVisualizer.leftVertexSet_);
		rightVertexSet_ = std::move(vrHandVisualizer.rightVertexSet_);

		leftMaterial_ = std::move(vrHandVisualizer.leftMaterial_);
		rightMaterial_ = std::move(vrHandVisualizer.rightMaterial_);

		transparency_ = vrHandVisualizer.transparency_;
		vrHandVisualizer.transparency_ = defaultTransparency_;

		leftJointRestPoses_T_rootJoint_ = std::move(vrHandVisualizer.leftJointRestPoses_T_rootJoint_);
		rightJointRestPoses_T_rootJoint_ = std::move(vrHandVisualizer.rightJointRestPoses_T_rootJoint_);

		isShown_ = bool(vrHandVisualizer.isShown_);
		vrHandVisualizer.isShown_ = true;

		VRVisualizer::operator=(std::move(vrHandVisualizer));
	}

	return *this;
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_HAND_VISUALIZER_H
