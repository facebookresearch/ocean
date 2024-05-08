/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_SHADER_H
#define META_OCEAN_RENDERING_GLES_SHADER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"

#include "ocean/base/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

// Forward declaration.
class GLESShader;

/**
 * Definition of an object reference for shader containers.
 * @ingroup renderinggles
 */
typedef Ocean::ObjectRef<GLESShader> GLESShaderRef;

/**
 * This class is the base class for all OpenGL ES shader container.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESShader
{
	public:

		/**
		 * Creates a new shader.
		 */
		GLESShader() = default;

		/**
		 * Destructs a vertex shader object.
		 */
		virtual ~GLESShader();

		/**
		 * Returns the id of this OpenGL ES shader.
		 * @return OpenGL ES shader id
		 */
		inline GLuint id() const;

		/**
		 * Compiles the shader using the given shader code.
		 * @param type The type of the shader, e.g., GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.
		 * @param code Shader code defining the shader
		 * @param message Returning compiler error or warning message.
		 * @return True, if the compilation was successfully
		 */
		bool compile(const GLenum type, const std::string& code, std::string& message);

		/**
		 * Compiles the shader using the given shader code.
		 * @param type The type of the shader, e.g., GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.
		 * @param codes Shader code defining the shader, can be composed of several individual code blocks
		 * @param lengths The lengths of the individual code fragments, in bytes
		 * @param message Returning compiler error or warning message.
		 * @return True, if the compilation was successfully
		 */
		bool compile(const GLenum type, const std::vector<const GLchar*>& codes, const std::vector<GLint>& lengths, std::string& message);

		/**
		 * Returns whether this shader holds no valid code.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether this shader has been compiled successfully.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// OpenGL ES shader id.
		GLuint id_ = 0u;

		/// OpenGL ES shader type.
		GLenum type_ = 0u;
};

inline GLuint GLESShader::id() const
{
	return id_;
}

inline bool GLESShader::isNull() const
{
	return id_ == 0u;
}

inline GLESShader::operator bool() const
{
	return id_ != 0u;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_SHADER_H
