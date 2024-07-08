/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_CONTEXT_ASSOCIATED_H
#define META_OCEAN_PLATFORM_GL_CONTEXT_ASSOCIATED_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/Context.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements the base class for all object which have an associated context.
 * Objects derived from this class need a valid associated context before they can be used or applied.<br>
 * Once an associated context has been set it cannot be changed anymore.<br>
 * @see setContext().
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT ContextAssociated
{
	public:

		/**
		 * Destructs this object.
		 */
		virtual ~ContextAssociated();

		/**
		 * Sets the context of this object.
		 * The context must not be set more than once!<br>
		 * Beware: Do not call any function of this framebuffer unless a valid context has been set!<br>
		 * @param context The context to set
		 * @return True, if succeeded
		 */
		virtual bool setContext(Context& context);

		/**
		 * Returns the associated context of this object.
		 * Beware: Ensure that this context has a valid associated context before calling this function.<br>
		 * @return The context
		 * @see hasContext().
		 */
		inline Context& context();

		/**
		 * Returns whether this object has a valid associated context.
		 * @return True, if so
		 */
		inline bool hasContext() const;

	protected:

		/**
		 * Creates a new object without associated context.
		 */
		ContextAssociated() = default;

		/**
		 * Creates a new object with given associated context.
		 * @param context The associated context
		 */
		explicit inline ContextAssociated(Context& context);

	protected:

		/// The OpenGL context which is associated with this framebuffer.
		Context* associatedContext_ = nullptr;
};

inline ContextAssociated::ContextAssociated(Context& context) :
	associatedContext_(&context)
{
	// nothing to do here
}

inline Context& ContextAssociated::context()
{
	ocean_assert(associatedContext_ != nullptr);

	return *associatedContext_;
}

inline bool ContextAssociated::hasContext() const
{
	return associatedContext_ != nullptr;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_CONTEXT_ASSOCIATED_H
