/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_BUFFER_H
#define META_OCEAN_PLATFORM_GL_BUFFER_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/ContextAssociated.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements the base class for all buffers.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT Buffer : public ContextAssociated
{
	public:

		/**
		 * Destructs a buffer object.
		 */
		virtual ~Buffer();

		/**
		 * Returns the size (the number of elements) this buffer holds.
		 * @return The number of elements
		 */
		inline size_t size() const;

		/**
		 * Releases this vertex buffer object.
		 * @return True, if succeeded
		 */
		bool release();

		/**
		 * Returns whether this object holds a valid vertex buffer.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Creates a new vertex buffer object.
		 */
		inline Buffer();

		/**
		 * Creates a new buffer object with associated context.
		 * @param context The associated context of the new buffer object
		 */
		explicit inline Buffer(Context& context);

	protected:

		/// The id of the vertex buffer.
		GLuint bufferId;

		/// The number of elements the vertex buffer holds.
		size_t bufferSize;
};

inline Buffer::Buffer() :
	ContextAssociated(),
	bufferId(0u),
	bufferSize(0)
{
	// nothing to do here
}

inline Buffer::Buffer(Context& context) :
	ContextAssociated(context),
	bufferId(0u),
	bufferSize(0)
{
	// nothing to do here
}

inline size_t Buffer::size() const
{
	return bufferSize;
}

inline Buffer::operator bool() const
{
	return bufferId != 0u;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_BUFFER_H
