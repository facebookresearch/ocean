/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_PASSTHROUGH_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_PASSTHROUGH_H

#include "ocean/platform/meta/quest/openxr/OpenXR.h"

#include "ocean/platform/openxr/Session.h"

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

/**
 * This class implements a wrapper for passthrough.
 * This object uses the Android feature 'com.oculus.feature.PASSTHROUGH'.
 * @ingroup platformmetaquestopenxr
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT Passthrough final
{
	public:

		/**
		 * Definition of an unordered set holding strings.
		 */
		typedef std::unordered_set<std::string> StringSet;

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
				 * @param passthrough The visualizer to which this new object belongs
				 */
				ScopedState(Passthrough& passthrough);

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
				Passthrough* passthrough_ = nullptr;

				/// True, if the passthrough service was actively running when this object was created.
				bool wasRunning_ = false;
		};

		/**
		 * Definition of a vector holding state objects.
		 */
		typedef std::vector<ScopedState> ScopedStates;

	public:

		/**
		 * Default constructor.
		 */
		Passthrough() = default;

		/**
		 * Destructs this passthrough object and releases all associated resources.
		 */
		~Passthrough();

		/**
		 * Move constructor.
		 * @param passthrough The object to be moved
		 */
		inline Passthrough(Passthrough&& passthrough);

		/**
		 * Initializes this passthrough object.
		 * @param session The OpenXR session to be used, must be valid
		 * @param xrPassthroughLayerPurposeFB The purpose to be used
		 * @return True, if succeeded
		 */
		inline bool initialize(const Platform::OpenXR::Session& session, const XrPassthroughLayerPurposeFB& xrPassthroughLayerPurposeFB = XR_PASSTHROUGH_LAYER_PURPOSE_RECONSTRUCTION_FB);

		/**
		 * Initializes this passthrough object.
		 * @param xrInstance The OpenXR instance to be used, must be valid
		 * @param xrSession The OpenXR session to be used, must be valid
		 * @param xrPassthroughLayerPurposeFB The purpose to be used
		 * @return True, if succeeded
		 */
		bool initialize(const XrInstance& xrInstance, const XrSession& xrSession, const XrPassthroughLayerPurposeFB& xrPassthroughLayerPurposeFB = XR_PASSTHROUGH_LAYER_PURPOSE_RECONSTRUCTION_FB);

		/**
		 * Starts passthrough.
		 * @return True, if succeeded
		 */
		bool start();

		/**
		 * Pauses passthrough.
		 * @return True, if succeeded
		 */
		bool pause();

		/**
		 * Returns whether passthrough is started.
		 * @return True, if so
		 */
		bool isStarted() const;

		/**
		 * Explicitly releases the passthrough object and all associated resources.
		 */
		void release();

		/**
		 * Returns the OpenXR passthrough layer.
		 * @return The passthrough's layer
		 */
		inline XrPassthroughLayerFB xrPassthroughLayerFB() const;

		/**
		 * Returns the lock of the passthrough.
		 * @return The passthrough's lock.
		 */
		inline Lock& lock() const;

		/**
		 * Returns whether this object holds initialized and valid OpenXR hand trackers.
		 * @return True, if so
		 */
		inline bool isValid() const;

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
		 * Move operator.
		 * @param passthrough The object to be moved
		 * @return Reference to this object
		 */
		Passthrough& operator=(Passthrough&& passthrough);

		/**
		 * Returns the names of the necessary OpenXR extensions necessary for hand tracking.
		 * @return The necessary OpenXR extensions
		 */
		static const StringSet& necessaryOpenXRExtensionNames();

	protected:

		/**
		 * Disabled copy constructor.
		 */
		Passthrough(const Passthrough&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		Passthrough& operator=(const Passthrough&) = delete;

	protected:

		/// The OpenXR instance.
		XrInstance xrInstance_ = XR_NULL_HANDLE;

		/// The OpenXR session.
		XrSession xrSession_ = XR_NULL_HANDLE;

		/// The OpenXR passthrough handle.
		XrPassthroughFB xrPassthroughFB_ = XR_NULL_HANDLE;

		/// The OpenXR passthrough layer handle.
		XrPassthroughLayerFB xrPassthroughLayerFB_ = XR_NULL_HANDLE;

		/// The OpenXR passthrough create function.
		PFN_xrCreatePassthroughFB xrCreatePassthroughFB_ = nullptr;

		/// The OpenXR passthrough destroy function.
		PFN_xrDestroyPassthroughFB xrDestroyPassthroughFB_ = nullptr;

		/// The OpenXR passthrough start function.
		PFN_xrPassthroughStartFB xrPassthroughStartFB_ = nullptr;

		/// The OpenXR passthrough pause function.
		PFN_xrPassthroughPauseFB xrPassthroughPauseFB_ = nullptr;

		/// The OpenXR passthrough create layer function.
		PFN_xrCreatePassthroughLayerFB xrCreatePassthroughLayerFB_ = nullptr;

		/// The OpeXR passthrough destroy layer function.
		PFN_xrDestroyPassthroughLayerFB xrDestroyPassthroughLayerFB_ = nullptr;

		/// The OpenXR passthrough resume layer function.
		PFN_xrPassthroughLayerResumeFB xrPassthroughLayerResumeFB_ = nullptr;

		/// The OpenXR passthrough set style function.
		PFN_xrPassthroughLayerSetStyleFB xrPassthroughLayerSetStyleFB_ = nullptr;

		/// True, if passthrough is currently started.
		bool isStarted_ = false;

		/// A stack of visualization states.
		ScopedStates stateStack_;

		/// The lock object.
		mutable Lock lock_;
};

inline Passthrough::Passthrough(Passthrough&& passthrough)
{
	*this = std::move(passthrough);
}

bool Passthrough::initialize(const Platform::OpenXR::Session& session, const XrPassthroughLayerPurposeFB& xrPassthroughLayerPurposeFB)
{
	ocean_assert(session.isValid());

	return initialize(session.xrInstance(), session, xrPassthroughLayerPurposeFB);
}

inline XrPassthroughLayerFB Passthrough::xrPassthroughLayerFB() const
{
	return xrPassthroughLayerFB_;
}

inline Lock& Passthrough::lock() const
{
	return lock_;
}

inline bool Passthrough::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return xrInstance_ != XR_NULL_HANDLE;
}

inline void Passthrough::pushState()
{
	const ScopedLock scopedLock(lock_);

	stateStack_.emplace_back(*this);
}

inline void Passthrough::popState()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!stateStack_.empty());

	stateStack_.pop_back();
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_PASSTHROUGH_H
