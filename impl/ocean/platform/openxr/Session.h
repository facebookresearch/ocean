/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_OPENXR_SESSION_H
#define META_OCEAN_PLATFORM_OPENXR_SESSION_H

#include "ocean/platform/openxr/OpenXR.h"
#include "ocean/platform/openxr/Instance.h"
#include "ocean/platform/openxr/Utilities.h"

#include "ocean/base/Lock.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

/**
 * This class wraps an OpenXR session.
 * A session represents an application’s intention to display XR content to the user.
 * @ingroup platformopenxr
 */
class OCEAN_PLATFORM_OPENXR_EXPORT Session final
{
	public:

		/**
		 * Definition of a vector holding color spaces.
		 */
		typedef std::vector<XrColorSpaceFB> XrColorSpacesFB;

	public:

		/**
		 * Default constructor creating an invalid session.
		 */
		Session() = default;

		/**
		 * Move constructor.
		 * @param session The session to be moved
		 */
		inline Session(Session&& session);

		/**
		 * Destructs the session and releases all associated resources.
		 */
		~Session();

		/**
		 * Initializes the session.
		 * @param instance The instance for which the session will be created, must be valid
		 * @param xrGraphicsBinding The pointer to the graphics binding struct, must be valid
		 * @param width The recommended width of the view configuration used for this session, in pixel, with range [1, infinity)
		 * @param height The recommended height of the view configuration used for this session, in pixel, with range [1, infinity)
		 * @return True, if succeeded
		 */
		inline bool initialize(const Instance& instance, const void* xrGraphicsBinding, const unsigned int width, const unsigned int height);

		/**
		 * Initializes the session.
		 * @param xrInstance The instance for which the session will be created, must be valid
		 * @param xrSystemId The runtime's identifier, must be valid
		 * @param xrGraphicsBinding The pointer to the graphics binding struct, must be valid
		 * @param width The recommended width of the view configuration used for this session, in pixel, with range [1, infinity)
		 * @param height The recommended height of the view configuration used for this session, in pixel, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool initialize(const XrInstance& xrInstance, const XrSystemId& xrSystemId, const void* xrGraphicsBinding, const unsigned int width, const unsigned int height);

		/**
		 * Explicitly releases the session and all associated resources.
		 * @see initialize().
		 */
		void release();

		/**
		 * Begins this session.
		 * After this function successfully returns, the session is considered to be running.
		 * The session should be stopped with end().
		 * @param xrViewConfigurationType The view configuration to be used, must be valid
		 * @return True, if succeeded
		 * @see isRunning(), end().
		 */
		bool begin(const XrViewConfigurationType xrViewConfigurationType);

		/**
		 * Ends this session.
		 * @return True, if succeeded
		 * @see isRunning(), begin().
		 */
		bool end();

		/**
		 * Returns whether this session is currently running.
		 * @return True, if so
		 * @see begin().
		 */
		inline bool isRunning() const;

		/**
		 * Returns the recommended width of the view configuration used for this session.
		 * @return The session's view configuration width, in pixel, with range [1, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the recommended height of the view configuration used for this session.
		 * @return The session's view configuration height, in pixel, with range [1, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Creates a space associated with this session.
		 * @param xrReferenceSpaceType The type of the space to create, must be valid
		 * @return The resulting space object
		 */
		inline ScopedXrSpace createSpace(const XrReferenceSpaceType xrReferenceSpaceType) const;

		/**
		 * Creates an action space for a pose action.
		 * @param xrAction The action for which the space will be created, must be a pose action, must be valid
		 * @param xrSubactionPath Optional subaction path for the action space, XR_NULL_PATH otherwise
		 * @return The resulting space object
		 */
		inline ScopedXrSpace createActionSpace(const XrAction& xrAction, const XrPath& xrSubactionPath = XR_NULL_PATH) const;

		/**
		 * Creates an action space for a pose action.
		 * @param xrAction The action for which the space will be created, must be a pose action, must be valid
		 * @param subactionPath Optional subaction path for the action space, empty otherwise
		 * @return The resulting space object
		 */
		inline ScopedXrSpace createActionSpace(const XrAction& xrAction, const std::string& subactionPath) const;

		/**
		 * Waits for the next frame that needs to be rendered.
		 * @param predictedDisplayTime The resulting predicted display time of the next frame
		 * @param predictedDisplayPeriod Optional resulting predicted display preriod of the next frame, nullptr if not of interest
		 * @return True, if the next frame needs to be rendered; False, to skip the next frame
		 */
		bool nextFrame(XrTime& predictedDisplayTime, XrDuration* predictedDisplayPeriod = nullptr);

		/**
		 * Sets the color space of this session.
		 * Needs extension: XR_FB_COLOR_SPACE_EXTENSION_NAME.
		 * @param xrColorSpaceFB The color space to set
		 * @return True, if succeeded
		 */
		inline bool setColorSpace(const XrColorSpaceFB xrColorSpaceFB);

		/**
		 * Translates an OpenXR result associated with this instance into a readable string.
		 * @param xrResult The OpenXR result to translate
		 * @return The translated result
		 */
		std::string translateResult(const XrResult xrResult) const;

		/**
		 * Returns the OpenXR instance associated with this session.
		 * @return The associated OpenXR instance, XR_NULL_HANDLE if unknown
		 */
		inline XrInstance xrInstance() const;

		/**
		 * Returns the OpenXR runtime's identifier.
		 * @return The runtime's identifier, XR_NULL_SYSTEM_ID if unknown
		 */
		inline XrSystemId xrSystemId() const;

		/**
		 * Returns whether this object holds a valid OpenXR instance.
		 * @return True, if so
		 * @see initialize().
		 */
		bool isValid() const;

		/**
		 * Move operator.
		 * @param session The session to be moved
		 * @return Reference to this object
		 */
		Session& operator=(Session&& session);

		/**
		 * Returns the wrapped OpenXR session.
		 * @return The actual session, nullptr if not initialized
		 * @see isValid().
		 */
		operator XrSession() const;

		/**
		 * Returns whether this object holds a valid OpenXR session.
		 * @return True, if so
		 * @see isValid().
		 */
		explicit inline operator bool() const;

		/**
		 * Creates a space associated with this session.
		 * @param xrInstance The OpenXR instance which created the session, must be valid
		 * @param xrSession The OpenXR session to which the space will be associated, must be valid
		 * @param xrReferenceSpaceType The type of the space to create, must be valid
		 * @return The resulting space object
		 */
		static ScopedXrSpace createSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrReferenceSpaceType xrReferenceSpaceType);

		/**
		 * Creates an action space for a pose action.
		 * @param xrInstance The OpenXR instance which created the session, must be valid
		 * @param xrSession The OpenXR session to which the space will be associated, must be valid
		 * @param xrAction The action for which the space will be created, must be a pose action, must be valid
		 * @param xrSubactionPath Optional subaction path for the action space, XR_NULL_PATH otherwise
		 * @return The resulting space object
		 */
		static ScopedXrSpace createActionSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrAction& xrAction, const XrPath& xrSubactionPath = XR_NULL_PATH);

		/**
		 * Creates an action space for a pose action.
		 * @param xrInstance The OpenXR instance which created the session, must be valid
		 * @param xrSession The OpenXR session to which the space will be associated, must be valid
		 * @param xrAction The action for which the space will be created, must be a pose action, must be valid
		 * @param subactionPath Optional subaction path for the action space, empty otherwise
		 * @return The resulting space object
		 */
		static inline ScopedXrSpace createActionSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrAction& xrAction, const std::string& subactionPath);

		/**
		 * Determines the available color spaces.
		 * Needs extension: XR_FB_COLOR_SPACE_EXTENSION_NAME.
		 * @param xrInstance The OpenXR instance for which the color spaces will be determined, must be valid
		 * @param xrSession The OpenXR session for which the color spaces will be determined, must be valid
		 * @param xrColorSpacesFB The resulting color spaces
		 * @return True, if succeeded
		 */
		static bool determineExistingColorSpaces(const XrInstance& xrInstance, const XrSession& xrSession, XrColorSpacesFB& xrColorSpacesFB);

		/**
		 * Sets the color space of a session.
		 * Needs extension: XR_FB_COLOR_SPACE_EXTENSION_NAME.
		 * @param xrInstance The OpenXR instance for which the color space will be set, must be valid
		 * @param xrSession The OpenXR session for which the color space will be set, must be valid
		 * @param xrColorSpaceFB The color space to set
		 * @return True, if succeeded
		 */
		static bool setColorSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrColorSpaceFB xrColorSpaceFB);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		Session(const Session&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		Session& operator=(const Session&) = delete;

	protected:

		/// The OpenXR instance for which the session is created.
		XrInstance xrInstance_ = XR_NULL_HANDLE;

		/// The actual OpenXR session.
		XrSession xrSession_ = XR_NULL_HANDLE;

		/// The identifier for the runtime.
		XrSystemId xrSystemId_ = XR_NULL_SYSTEM_ID;

		/// True, if the session is running.
		bool isRunning_ = false;

		/// The recommended width of the view configuration used for this session, in pixel, with range [1, infinity).
		unsigned int width_ = 0u;

		/// The recommended height of the view configuration used for this session, in pixel, with range [1, infinity)
		unsigned int height_ = 0u;

		/// The session's lock.
		mutable Lock lock_;
};

inline Session::Session(Session&& session)
{
	*this = std::move(session);
}

inline bool Session::initialize(const Instance& instance, const void* xrGraphicsBinding, const unsigned int width, const unsigned int height)
{
	ocean_assert(instance.isValid());
	ocean_assert(xrGraphicsBinding != nullptr);

	return initialize(instance, instance.xrSystemId(), xrGraphicsBinding, width, height);
}

inline bool Session::isRunning() const
{
	const ScopedLock scopedLock(lock_);

	return isRunning_;
}

inline unsigned int Session::width() const
{
	const ScopedLock scopedLock(lock_);

	return width_;
}

inline unsigned int Session::height() const
{
	const ScopedLock scopedLock(lock_);

	return height_;
}

inline ScopedXrSpace Session::createSpace(const XrReferenceSpaceType xrReferenceSpaceType) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	return createSpace(xrInstance_, xrSession_, xrReferenceSpaceType);
}

inline ScopedXrSpace Session::createActionSpace(const XrAction& xrAction, const XrPath& xrSubactionPath) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	return createActionSpace(xrInstance_, xrSession_, xrAction, xrSubactionPath);
}

inline ScopedXrSpace Session::createActionSpace(const XrAction& xrAction, const std::string& subactionPath) const
{
	return createActionSpace(xrInstance_, xrSession_, xrAction, Utilities::translatePath(xrInstance_, subactionPath));
}

inline bool Session::setColorSpace(const XrColorSpaceFB xrColorSpaceFB)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	return setColorSpace(xrInstance_, xrSession_, xrColorSpaceFB);
}

inline XrInstance Session::xrInstance() const
{
	const ScopedLock scopedLock(lock_);

	return xrInstance_;
}

inline XrSystemId Session::xrSystemId() const
{
	const ScopedLock scopedLock(lock_);

	return xrSystemId_;
}

inline Session::operator bool() const
{
	return isValid();
}

ScopedXrSpace Session::createActionSpace(const XrInstance& xrInstance, const XrSession& xrSession, const XrAction& xrAction, const std::string& subactionPath)
{
	return createActionSpace(xrInstance, xrSession, xrAction, Utilities::translatePath(xrInstance, subactionPath));
}

}

}

}

#endif // META_OCEAN_PLATFORM_OPENXR_SESSION_H
