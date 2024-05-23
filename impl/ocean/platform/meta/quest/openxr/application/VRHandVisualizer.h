/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_HAND_VISUALIZER_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_HAND_VISUALIZER_H

#include "ocean/platform/meta/quest/openxr/application/Application.h"

#include "ocean/platform/meta/quest/application/VRVisualizer.h"

#include "ocean/platform/meta/quest/openxr/HandPoses.h"

#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/ShaderProgram.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/VertexSet.h"

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
 * This class implements helper functions allowing to visualize the hand mesh used by hand tracking (Nimble) in an Ocean-based VR application (e.g., VRNativeApplication).
 * @see VRNativeApplication.
 * @ingroup platformmetaquestopenxrapplication
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT VRHandVisualizer : public Quest::Application::VRVisualizer
{
	public:

		/**
		 * Definition of individual render modes.
		 */
		enum RenderMode : uint32_t
		{
			/// An invalid render mode.
			RM_INVALID = 0u,
			/// The joints will be rendered as small coordinate systems.
			RM_JOINTS = 1u << 0u,
			/// The hand mesh will be rendered as a wireframe.
			RM_WIREFRAME = 1u << 1u,
			/// The hand mesh will be rendred.
			RM_MESH = 1u << 2u,
			/// The default rendering mode.
			RM_DEFAULT = RM_MESH,
			/// The render mode using all existing render modes.
			RM_ALL = RM_JOINTS | RM_WIREFRAME | RM_MESH,
		};

		/**
		 * This class implements a scoped state object allowing to reset all states of a visualizer.
		 * The state can be stored locally or states can be pushed onto a stack.
		 * @see pushState(), popState().
		 */
		class ScopedState
		{
			friend class VRHandVisualizer;

			protected:

				/// The default hand transparency.
				static constexpr Scalar defaultTransparency_ = Scalar(0.8);

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

				/// The color to be used when rendering the hands.
				RGBAColor handColor_ = RGBAColor(0.7f, 0.7f, 0.7f);

				/// The transparency when this object was created.
				Scalar transparency_ = Scalar(0.6);

				/// The render mode when this object was created.
				RenderMode renderMode_ = RM_DEFAULT;
		};

	protected:

		/// The shader part for the platform.
		static const char* partPlatform_;

		/// The shader part with the vertex shader.
		static const char* partVertexShaderTexture_;

		/// The shader part with the fragment shader.
		static const char* partFragmentShader_;

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
		 */
		inline VRHandVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer);

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
		~VRHandVisualizer() override;

		/**
		 * Shows the hands.
		 * @see hide().
		 */
		inline void show();

		/**
		 * Hides the hands.
		 * @see show().
		 */
		inline void hide();

		/**
		 * Returns whether the hands are shown.
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
		 * Sets the hand's render mode.
		 * @param renderMode The render mode to be set
		 * @return True, if succeeded
		 * @see renderMode().
		 */
		bool setRenderMode(const RenderMode renderMode);

		/**
		 * Returns the hand's render mode.
		 * @return The mode which is used to render the hands.
		 * @see setRenderMode().
		 */
		RenderMode renderMode() const;

		/**
		 * Visualizes the hands in relation to the given HandPoses' base space.
		 * Note: you need to call `handPoses.update()` before calling this function
		 * @param handPoses An instance of the hand poses object, must be valid
		 * @return True, if succeeded
		 */
		bool visualizeHands(const HandPoses& handPoses);

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
		 * Releases the hand visualizer and all associated resources explicitly.
		 */
		void release();

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
		 * Visualizes the joints of the hands.
		 * @param handPoses The hand poses object providing the necessary information to render the hands
		 * @return True, if succeeded
		 */
		bool visalizeJoints(const HandPoses& handPoses);

		/**
		 * Visualizes the wireframe of the hands.
		 * @param handPoses The hand poses object providing the necessary information to render the hands
		 * @return True, if succeeded
		 */
		bool visualizeWireframe(const HandPoses& handPoses);

		/**
		 * Visualizes the mesh of the hands.
		 * @param handPoses The hand poses object providing the necessary information to render the hands
		 * @return True, if succeeded
		 */
		bool visualizeMesh(const HandPoses& handPoses);

	protected:

		/// The render mode to be used.
		RenderMode renderMode_ = RM_DEFAULT;

		/// The scene object of the renderer;
		Rendering::SceneRef scene_;

		/// The rendering Transform node for rendering the joints.
		Rendering::TransformRef transformJoints_;

		/// The rendering VertexSet object for rendering the joints.
		Rendering::VertexSetRef vertexSetJoints_;

		/// The rendering Group node for rendering the wireframe.
		Rendering::GroupRef groupWireFrame_;

		/// The rendering Group node for rendering the mesh.
		Rendering::GroupRef groupMesh_;

		/// The rendering Geometry nodes for rendering the left and right hand mesh.
		Rendering::GeometryRef geometriesMesh_[HandPoses::numberHands_];

		/// The rendering ShaderProgram objects for rendering the left and right hand mesh.
		Rendering::ShaderProgramRef shaderProgramsMesh_[HandPoses::numberHands_];

		/// The color to be used when rendering the hands.
		RGBAColor handColor_ = RGBAColor(0.7f, 0.7f, 0.7f);

		/// The transparency which is used to render the hands, with range [0, 1]
		Scalar transparency_ = ScopedState::defaultTransparency_;

		/// True, if the hands are visualized; False, if the hands are hidden.
		std::atomic<bool> isShown_ = true;

		/// A stack of visualization states.
		ScopedStates stateStack_;
};

inline VRHandVisualizer::VRHandVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer) :
	VRVisualizer(engine, framebuffer)
{
	// nothhing to do here
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
		release();

		renderMode_ = vrHandVisualizer.renderMode_;
		vrHandVisualizer.renderMode_ = RM_DEFAULT;

		scene_ = std::move(vrHandVisualizer.scene_);

		transformJoints_ = std::move(vrHandVisualizer.transformJoints_);
		vertexSetJoints_ = std::move(vrHandVisualizer.vertexSetJoints_);

		groupWireFrame_ = std::move(vrHandVisualizer.groupWireFrame_);

		groupMesh_ = std::move(vrHandVisualizer.groupMesh_);

		for (size_t handIndex = 0; handIndex < HandPoses::numberHands_; ++handIndex)
		{
			geometriesMesh_[handIndex] = std::move(vrHandVisualizer.geometriesMesh_[handIndex]);
			shaderProgramsMesh_[handIndex] = std::move(vrHandVisualizer.shaderProgramsMesh_[handIndex]);
		}

		handColor_ = vrHandVisualizer.handColor_;
		vrHandVisualizer.handColor_ = RGBAColor(0.7f, 0.7f, 0.7f);

		transparency_ = vrHandVisualizer.transparency_;
		vrHandVisualizer.transparency_ = ScopedState::defaultTransparency_;

		isShown_ = bool(vrHandVisualizer.isShown_);
		vrHandVisualizer.isShown_ = true;

		stateStack_ = std::move(vrHandVisualizer.stateStack_);

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

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_VR_HAND_VISUALIZER_H
