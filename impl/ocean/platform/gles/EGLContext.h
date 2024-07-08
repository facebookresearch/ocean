/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GLES_EGL_CONTEXT_H
#define META_OCEAN_PLATFORM_GLES_EGL_CONTEXT_H

#include "ocean/platform/gles/GLES.h"

#ifndef __APPLE__
	#include <EGL/egl.h>
	#include <EGL/eglext.h>
#endif // __APPLE__

namespace Ocean
{

namespace Platform
{

namespace GLES
{

/**
 * This class encapsulates an EGL context.
 * @ingroup platformgles
 */
class OCEAN_PLATFORM_GLES_EXPORT EGLContext
{
	public:

#ifndef __APPLE__

		/**
		 * Definition of a configuration attribute pair combining attribute (first) and value (second).
		 */
		typedef std::pair<EGLint, EGLint> ConfigAttributePair;

		/**
		 * Definition of a vector holding configuration pairs.
		 */
		typedef std::vector<ConfigAttributePair> ConfigAttributePairs;

	public:

		/**
		 * Creates a new uninitialized context object.
		 * @see initialize().
		 */
		EGLContext();

		/**
		 * Move constructor.
		 * @param context The context to be moved
		 */
		EGLContext(EGLContext&& context);

		/**
		 * Disabled copy consturctor.
		 * @param context The context which whould be copied
		 */
		EGLContext(const EGLContext& context) = delete;

		/**
		 * Destructs the context object and releases all remaining EGL resources.
		 * @see release().
		 */
		~EGLContext();

		/**
		 * Returns the major version of this egl context.
		 * @return The context's major version
		 */
		inline EGLint majorVersion() const;

		/**
		 * Returns the minor version of this egl context.
		 * @return The context's minor version
		 */
		inline EGLint minorVersion() const;

		/**
		 * Returns the display id of this egl context.
		 * @return The context's display id
		 */
		inline const EGLDisplay& display() const;

		/**
		 * Returns the configuration of the egl context.
		 * @return The context's configuration
		 */
		inline const EGLConfig& config() const;

		/**
		 * Returns the context id of this egl context.
		 * @return The context's context id
		 */
		inline const ::EGLContext& context() const;

		/**
		 * Initializes this context object.
		 * @param configAttributePairs The configuration attributes the resulting context must support.
		 * @param shareContext Optional share context
		 * @see release().
		 */
		bool initialize(const ConfigAttributePairs& configAttributePairs = ConfigAttributePairs(), EGLContext* shareContext = nullptr);

		/**
		 * Releases this context.
		 * @see initialize().
		 */
		void release();

		/**
		 * Returns whether this context is valid (whether the context is initialized) and whether it can be used.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns whether this context is valid (whether the context is initialized) and whether it can be used.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param context The context to be moved
		 */
		EGLContext& operator=(EGLContext&& context);

		/**
		 * Disabled copy operator.
		 * @param context The context which would be copied
		 */
		EGLContext& operator=(const EGLContext& context) = delete;

		/**
		 * Translates an EGL error to a string.
		 * @param errorValue The error value to translate
		 * @return The translated error value
		 */
		static const char* translateError(const EGLint errorValue);

	protected:

		/// The major EGL version.
		EGLint majorVersion_;

		/// The minor EGL version.
		EGLint minorVersion_;

		/// The EGL display.
		EGLDisplay display_;

		/// The EGL config.
		EGLConfig config_;

		/// The EGL context.
		::EGLContext context_;

		/// The dummy surface.
		EGLSurface dummySurface_;

#endif // __APPLE__

};

#ifndef __APPLE__

inline EGLint EGLContext::majorVersion() const
{
	return majorVersion_;
}

inline EGLint EGLContext::minorVersion() const
{
	return minorVersion_;
}

inline const EGLDisplay& EGLContext::display() const
{
	return display_;
}

inline const EGLConfig& EGLContext::config() const
{
	return config_;
}

inline const ::EGLContext& EGLContext::context() const
{
	return context_;
}

inline EGLContext::operator bool() const
{
return isValid();
}

#endif // __APPLE__

}

}

}

#endif // META_OCEAN_PLATFORM_GLES_EGL_CONTEXT_H
