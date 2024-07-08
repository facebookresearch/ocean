/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_SHADER_PROGRAM_H
#define META_OCEAN_PLATFORM_GL_SHADER_PROGRAM_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/ContextAssociated.h"

#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements an OpenGL shader program.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT ShaderProgram : public ContextAssociated
{
	public:

		/**
		 * Creates a new shader program object.
		 */
		inline ShaderProgram();

		/**
		 * Creates a new shader program with associated context.
		 * @param context The associated context of the shader program
		 */
		explicit inline ShaderProgram(Context& context);

		/**
		 * Destructs a shader program object.
		 */
		virtual ~ShaderProgram();

		/**
		 * Returns the id of the shader program.
		 * @return The shader program's id, 0 if the shader has not been defined or could not be created
		 */
		inline GLuint id() const;

		/**
		 * Creates a new shader program by the code of the vertex shader and fragment shader.
		 * The shader code will be compiled and linked afterwards.<br>
		 * @param vertexCode The code of the vertex shader
		 * @param fragmentCode The code of the fragment shader
		 * @param errorMessage Optional resulting error message
		 * @return True, if succeeded
		 */
		bool createProgram(const std::string& vertexCode, const std::string& fragmentCode, std::string* errorMessage = nullptr);

		/**
		 * Binds this shader program.
		 * @return True, if succeeded
		 */
		bool bind();

		/**
		 * Sets the value of a uniform parameter of this shader program.
		 * @param uniformName The name of the uniform parameter
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		bool setUniform(const std::string& uniformName, const int value);

		/**
		 * Sets the value of a uniform parameter of this shader program.
		 * @param uniformName The name of the uniform parameter
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		bool setUniform(const std::string& uniformName, const float value);

		/**
		 * Sets the value of a uniform parameter of this shader program.
		 * @param uniformName The name of the uniform parameter
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		bool setUniform(const std::string& uniformName, const VectorF2& value);

		/**
		 * Sets the value of a uniform parameter of this shader program.
		 * @param uniformName The name of the uniform parameter
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		bool setUniform(const std::string& uniformName, const VectorF3& value);

		/**
		 * Sets the value of a uniform parameter of this shader program.
		 * @param uniformName The name of the uniform parameter
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		bool setUniform(const std::string& uniformName, const VectorF4& value);

		/**
		 * Sets the value of a uniform parameter of this shader program.
		 * @param uniformName The name of the uniform parameter
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		bool setUniform(const std::string& uniformName, const SquareMatrixF4& value);

		/**
		 * Releases this shader program.
		 * @return True, if succeeded
		 */
		bool release();

		/**
		 * Returns whether this object holds a valid shader program (which could be created, compiled and linked successfully).
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Creates either a vertex or a fragment shader.
		 * @param code The code of the shader
		 * @param shaderId The resulting id of the shader, must be 0
		 * @param shaderType The type of the shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
		 * @param errorMessage Optional resulting error message
		 * @return True, if succeeded
		 */
		bool createShader(const std::string& code, GLuint& shaderId, const GLenum shaderType, std::string* errorMessage = nullptr);

	protected:

		/// The id of the shader program.
		GLuint programId;

		/// The id of the vertex shader.
		GLuint programVertexShaderId;

		/// The id of the fragment shader.
		GLuint programFragmentShaderId;
};

inline ShaderProgram::ShaderProgram() :
	ContextAssociated(),
	programId(0u),
	programVertexShaderId(0u),
	programFragmentShaderId(0u)
{
	// nothing to do here
}

inline ShaderProgram::ShaderProgram(Context& context) :
	ContextAssociated(context),
	programId(0u),
	programVertexShaderId(0u),
	programFragmentShaderId(0u)
{
	// nothing to do here
}

inline GLuint ShaderProgram::id() const
{
	return programId;
}

inline ShaderProgram::operator bool() const
{
	ocean_assert(programId == 0u || (programVertexShaderId != 0u || programFragmentShaderId != 0u));
	return programId != 0u;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_SHADER_PROGRAM_H
