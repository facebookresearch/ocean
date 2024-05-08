/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_VERTEX_BUFFER_H
#define META_OCEAN_PLATFORM_GL_VERTEX_BUFFER_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/Buffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements a vertex buffer.
 * This buffer represents a GL_ARRAY_BUFFER.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT VertexBuffer : public Buffer
{
	public:

		/**
		 * Creates a new vertex buffer object.
		 */
		inline VertexBuffer();

		/**
		 * Creates a new vertex buffer object with associated context.
		 * @param context The associated context of the new vertex buffer object
		 */
		explicit inline VertexBuffer(Context& context);

		/**
		 * Sets or changes the data of this vertex buffer object.
		 * @param data The data to set
		 * @param size The number of elements to set
		 * @return True, if succeeded
		 */
		bool setBufferData(const double* data, const size_t size);

		/**
		 * Sets or changes the data of this vertex buffer object.
		 * @param data The data to set
		 * @param size The number of elements to set
		 * @return True, if succeeded
		 */
		bool setBufferData(const float* data, const size_t size);

		/**
		 * Binds this vertex buffer object to a specified shader program and attribute.
		 * @param programId The id of the shader program
		 * @param attributeName The name of the shader's attribute
		 * @param elements The number of elements to bind
		 * @return True, if succeeded
		 */
		bool bindToProgram(const GLuint programId, const std::string& attributeName, const unsigned int elements) const;

		/**
		 * Assign operator which does not create a new copy of an assocated OpenGL object.
		 * @param buffer Vertex buffer object to assign
		 * @return Reference to this object
		 */
		VertexBuffer& operator=(const VertexBuffer& buffer);
};

inline VertexBuffer::VertexBuffer() :
	Buffer()
{
	// nothing to do here
}

inline VertexBuffer::VertexBuffer(Context& context) :
	Buffer(context)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_VERTEX_BUFFER_H
