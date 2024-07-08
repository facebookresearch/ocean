/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_INDEX_BUFFER_H
#define META_OCEAN_PLATFORM_GL_INDEX_BUFFER_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/Buffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements an OpenGL index buffer object.
 * This buffer represents a GL_ELEMENT_ARRAY_BUFFER.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT IndexBuffer : public Buffer
{
	public:

		/**
		 * Creates a new index buffer object.
		 */
		inline IndexBuffer();

		/**
		 * Creates a new index buffer object with associated context.
		 * @param context The associated context of the new index buffer object
		 */
		explicit inline IndexBuffer(Context& context);

		/**
		 * Sets or changes the data of this index buffer object.
		 * @param data The data to set
		 * @param size The number of elements to set
		 * @return True, if succeeded
		 */
		bool setBufferData(const unsigned int* data, const size_t size);

		/**
		 * Binds this index buffer object.
		 * @return True, if succeeded
		 */
		bool bind() const;

		/**
		 * Assign operator which does not create a new copy of an assocated OpenGL object.
		 * @param buffer Index buffer object to assign
		 * @return Reference to this object
		 */
		IndexBuffer& operator=(const IndexBuffer& buffer);
};

inline IndexBuffer::IndexBuffer() :
	Buffer()
{
	// nothing to do here
}

inline IndexBuffer::IndexBuffer(Context& context) :
	Buffer(context)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_INDEX_BUFFER_H
