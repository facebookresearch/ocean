/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_VERTEX_ARRAY_H
#define META_OCEAN_PLATFORM_GL_VERTEX_ARRAY_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/ContextAssociated.h"


namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements a vertex array object.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT VertexArray : public ContextAssociated
{
	public:

		/**
		 * Creates a new vertex array object.
		 */
		inline VertexArray();

		/**
		 * Creates a new vertex array object with associated context.
		 * @param context The associated context of the new vertex array object
		 */
		explicit inline VertexArray(Context& context);

		/**
		 * Destructs this vertex array object.
		 */
		virtual ~VertexArray();

		/**
		 * Binds this vertex array object (and creates it if it does not exist).
		 * @return True, if succeeded
		 */
		bool bind();

		/**
		 * Unbinds this vertex array object (unbinds any/the current vertex object).
		 * @return True, if succeeded
		 */
		bool unbind();

		/**
		 * Releases this vertex array object.
		 * @return True, if succeeded
		 */
		bool release();

		/**
		 * Returns whether this object holds a valid vertex buffer.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Assign operator which does not create a new copy of an associated OpenGL object.
		 * @param object Vertex array object to assign
		 * @return Reference to this object
		 */
		VertexArray& operator=(const VertexArray& object);

	protected:

		/// The id of the vertex array.
		GLuint arrayId;
};

inline VertexArray::VertexArray() :
	ContextAssociated(),
	arrayId(0u)
{
	// nothing to do here
}

inline VertexArray::VertexArray(Context& context) :
	ContextAssociated(context),
	arrayId(0u)
{
	// nothing to do here
}

inline VertexArray::operator bool() const
{
	return arrayId != 0u;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_VERTEX_ARRAY_H
