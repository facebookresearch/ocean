// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/platform/meta/quest/vrapi/application/Application.h"
#include "ocean/platform/meta/quest/vrapi/application/VRNativeApplication.h"

#include "ocean/platform/meta/quest/application/VRVisualizer.h"

#include <insightsdk/internal/mr_api_internal.h>
#include <insightsdk/mr_api.h>

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
 * This class implements helper functions allowing to visualize the passthrough mesh from the MR SDK.
 * @see VRNativeApplication
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT MRPassthroughVisualizer : public Quest::Application::VRVisualizer
{
	public:

		/**
		 * This class implements a scoped state object allowing to reset all passthrough states of a visualizer.
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
				 * @param mrPassthroughVisualizer The visualizer to which this new object belongs
				 */
				ScopedState(MRPassthroughVisualizer& mrPassthroughVisualizer);

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
				MRPassthroughVisualizer* mrPassthroughVisualizer_ = nullptr;

				/// True, if the passthrough service was actively running when this object was created.
				bool wasRunning_ = false;
		};

	protected:

		/**
		 * Definition of passthrough styles
		 */
		struct PassthroughStyle
		{
			/// A human-readable name of the style
			std::string name;

			/// The parameters of this style
			MrPassthroughStyle style;

			/**
			 * Definition of color map parameters
			 */
			union
			{
				MrPassthroughColorMapMonoToMono mono;
				MrPassthroughColorMapMonoToRgba rgba;
				MrPassthroughColorMapContrastControl hands;
			} colorMap;
		};

		/// Definition of a vector of passthrough styles
		typedef std::vector<PassthroughStyle> PassthroughStyles;

		/**
		 * Definition of custom shaders available for Passthrough styles.
		 */
		struct CustomShader
		{
			/// A human-readable name of the shader
			std::string name;

			/// The data structure that manages the shader program
			MrShaderProgram program;

			/// Indicates whether this is shader is require on this platform.
			bool required;
		};

		/// A vector of custom shaders
		typedef std::vector<CustomShader> CustomShaders;

		/**
		 * Definition of a vector holding state objects.
		 */
		typedef std::vector<ScopedState> ScopedStates;

	public:

		/**
		 * Default constructor, creates a new invalid visualizer.
		 */
		MRPassthroughVisualizer() = default;

		/**
		 * Creates a valid passthrough visualizer
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 * @param ovr The ovrMobile object, must be valid
		 * @param java A pointer to the ovrJava object, must be valid
		 */
		inline MRPassthroughVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, ovrMobile* ovr, const ovrJava* java);

		/**
		 * Move constructor
		 * @param mrPassthroughVisualizer Another instance of the MR passthrough visualizer that will be moved to this instance
		 */
		inline MRPassthroughVisualizer(MRPassthroughVisualizer&& mrPassthroughVisualizer);

		/**
		 * Disabled the copy constructor
		 * @param mrPassthroughVisualizer Another instance of the MR passthrough visualizer that would have been copied to this instance
		 */
		MRPassthroughVisualizer(const MRPassthroughVisualizer& mrPassthroughVisualizer) = delete;

		/**
		 * Destructor
		 */
		inline ~MRPassthroughVisualizer();

		/**
		 * Returns whether this object holds valid data.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Indicates if the passthrough service is actively running and provides composition layers
		 * @return True if it is running, otherwise false
		 */
		inline bool isPassthroughRunning() const;

		/**
		 * Pauses the passthrough service without closing the connection to the service
		 * @return True if the service is paused, otherwise false
		 */
		bool pausePassthrough();

		/**
		 * Resumes a paused passthrough service
		 * @return True if the service has resumed, otherwise false
		 */
		bool resumePassthrough();

		/**
		 * Indicates that the app has entered VR mode and allows the final initialization of the passthrough service
		 * Call this function in the override of `VRNativeApplication::onVrModeEntered()` that is closest to your application
		 * @return True if the initialization was successful, otherwise false
		 * @sa VRNativeApplication::onVrModeEntered()
		 */
		bool enterVrMode();

		/**
		 * Indicates to the passthrough service that the VR mode has been left and that it needs to be stopped
		 * Call this function in the override of `VRNativeApplication::onVrModeLeft()` that is closest to your application
		 * @return True if the deinitialization was successful
		 * @sa VRNativeApplication::onVrModeLeft()
		 */
		bool leaveVrMode();

		/**
		 * Triggers the passthrough service to process the next frame
		 * @return True if the operation was successful, otherwise false
		 */
		bool update();

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
		 * Triggers the passthrough service to render the passthrough information
		 * @param renderTimestamp The estimated time when this frame will be displayed, must be valid
		 * @return A custom layer holding the information required to render passthrough using time warp
		 * @sa VRNativeApplication::addCustomLayer()
		 */
		VRNativeApplication::CustomLayer render(const Timestamp renderTimestamp);

		/**
		 * Move-assignment operator
		 * @param mrPassthroughVisualizer Another instance of the MR passthrough visualizer that will be moved to this instance
		 * @return A reference to this instance of the MR passthrough visualizer
		 */
		MRPassthroughVisualizer& operator=(MRPassthroughVisualizer&& mrPassthroughVisualizer);

		/**
		 * Disable the copy-assignment operator
		 * @param mrPassthroughVisualizer Another instance of the MR passthrough visualizer that would have been copied to this instance
		 * @return A reference to this instance of the MR passthrough visualizer
		 */
		MRPassthroughVisualizer& operator=(const MRPassthroughVisualizer& mrPassthroughVisualizer) = delete;

	protected:

		/**
		 * Initializes the passthrough styles
		 */
		void initializePassthroughStyles();

		/**
		 * Loads the selected passthrough style and shaders
		 */
		void applyUpdatedPassthroughStyle();

		/**
		 * Translates return codes of the MR Passthrough API to human-readable strings
		 * @param mrResult The return code that will be translated to a human-readable string
		 */
		static std::string toString(const MrResult mrResult);

		/**
		 * Initializes the session with the MR Passthrough API
		 * @param mrSession The MR Passthrough session object that will be returned, only valid if the function returns true
		 * @param mrPassthroughFeature Returns the MR Passthrough feature object
		 * @param mrCompositionLayer Returns the composition layer that the passthrough service requires
		 * @param java A pointer to the ovrJava object, must be valid
		 * @param ovr The ovrMobile object, must be valid
		 * @return True if the session was successfully initialized
		 */
		static bool createMrSession(MrSession& mrSession, MrPassthroughFeature& mrPassthroughFeature, MrCompositionLayer& mrCompositionLayer, const ovrJava* java, ovrMobile* ovr);

		/**
		 * Closes a MR Passthrough session
		 * @param mrSession The object of the MR Passthrough session that will be closed, must be valid
		 * @param mrPassthroughFeature The MR Passthrough feature object that belongs to the session, must be valid
		 * @return True if the session has been closed successfully
		 */
		static bool destroyMrSession(MrSession& mrSession, MrPassthroughFeature& mrPassthroughFeature);

		/**
		 * Recreates the composition layer
		 * @param mrSession The object of the MR Passthrough session, must be valid
		 * @param mrCompositionLayer The composition layer that the passthrough service will recreate, must be valid
		 * @return True if the composition layer was successfully recreated, otherwise false
		 */
		static bool recreateCompositionLayer(MrSession& mrSession, MrCompositionLayer& mrCompositionLayer);

	protected:

		/// The ovrMobile object, must be valid
		ovrMobile* ovrMobile_ = nullptr;

		/// The OVR Java object from the main application, must be valid
		const ovrJava* ovrJava_ = nullptr;

		/// The MR session object
		MrSession mrSession_ = XR_NULL_HANDLE;

		/// MR passthrough feature
		MrPassthroughFeature mrPassthroughFeature_ = XR_NULL_HANDLE;

		/// MR composition layer for passthrough
		MrCompositionLayer mrCompositionLayer_ = XR_NULL_HANDLE;

		/// The available passthrough styles
		PassthroughStyles passthroughStyles_;

		/// The available custom shaders
		CustomShaders customShaders_;

		/// The index of the select passthrough style
		size_t passthroughStyleIndex_ = 0;

		/// The previously selected passthrough style
		int lastAppliedCustomShaderIndex_ = -1;

		/// The selected custom shader, if applicable
		int customShaderStyleStartIndex_ = -1;

		/// A stack of visualization states.
		ScopedStates stateStack_;
};

inline MRPassthroughVisualizer::MRPassthroughVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer, ovrMobile* ovr, const ovrJava* java) :
	VRVisualizer(engine, framebuffer),
	ovrMobile_(ovr),
	ovrJava_(java),
	mrSession_(XR_NULL_HANDLE),
	mrPassthroughFeature_(XR_NULL_HANDLE),
	mrCompositionLayer_(XR_NULL_HANDLE)
{
	ocean_assert(ovrMobile_ != nullptr);
	ocean_assert(ovrJava_ != nullptr);

	initializePassthroughStyles();
}

inline MRPassthroughVisualizer::MRPassthroughVisualizer(MRPassthroughVisualizer&& mrPassthroughVisualizer) :
	MRPassthroughVisualizer()
{
	*this = std::move(mrPassthroughVisualizer);
}

inline MRPassthroughVisualizer::~MRPassthroughVisualizer()
{
	destroyMrSession(mrSession_, mrPassthroughFeature_);
}

inline bool MRPassthroughVisualizer::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return VRVisualizer::isValid() && ovrMobile_ != nullptr && ovrJava_ != nullptr && mrSession_ != XR_NULL_HANDLE && mrPassthroughFeature_ != XR_NULL_HANDLE;
}

inline bool MRPassthroughVisualizer::isPassthroughRunning() const
{
	const ScopedLock scopedLock(lock_);

	if (mrPassthroughFeature_ != XR_NULL_HANDLE)
	{
		return MrPassthroughIsRunning(mrPassthroughFeature_);
	}

	return false;
}

inline void MRPassthroughVisualizer::pushState()
{
	const ScopedLock scopedLock(lock_);

	stateStack_.emplace_back(*this);
}

inline void MRPassthroughVisualizer::popState()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!stateStack_.empty());

	stateStack_.pop_back();
}

} // namespace Application

} // namespace VrApi

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
