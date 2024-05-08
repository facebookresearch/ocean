/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_SHADER_PROGRAM_H
#define META_OCEAN_RENDERING_GLES_SHADER_PROGRAM_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"
#include "ocean/rendering/glescenegraph/GLESShader.h"

#include "ocean/base/ObjectRef.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"

#include "ocean/rendering/ShaderProgram.h"

#include <unordered_map>

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

// Forward declaration.
class GLESShaderProgram;

// Forward declaration.
class GLESFramebuffer;

/**
 * Definition of an object reference for shader program containers.
 * @ingroup renderinggles
 */
typedef SmartObjectRef<GLESShaderProgram> GLESShaderProgramRef;

/**
 * This class implements a container for an OpenGL ES shader program.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESShaderProgram :
	public ShaderProgram,
	public GLESAttribute
{
	public:

		/**
		 * Definition of a pair combining a GL shader type and shader code parts.
		 */
		typedef std::pair<GLenum, std::vector<const char*>> ShaderCodePair;

		/**
		 * Definition of a vector holding shader code pairs.
		 */
		typedef std::vector<ShaderCodePair> ShaderCodePairs;

	protected:

		/**
		 * This class implements a temporary store for shader parameters.
		 * The temporary storage is necessary to allow setting parameters from a different thread than the rendering thread.
		 * @tparam T The data type of the parameters
		 */
		template <typename T>
		class Parameters
		{
			public:

				/**
				 * Definition of a map mapping parameter names to parameter values.
				 */
				typedef std::unordered_map<std::string, T> ValueMap;

			public:

				/**
				 * Setting the value of a parameter.
				 * @param name The name of the parameter, must be valid
				 * @param value The value to be set
				 */
				void setValue(const std::string& name, T&& value);

				/**
				 * Setting the value of a parameter.
				 * @param name The name of the parameter, must be valid
				 * @param value The value to be set
				 */
				void setValue(const std::string& name, const T& value);

				/**
				 * Binds all parameters to a shader program.
				 * @param id The id of the shader program to which the values bill be bound, must be valid
				 */
				void bindValues(const GLuint id) const;

			protected:

				/// All values.
				ValueMap valueMap_;
		};

		/**
		 * Definition of a map mapping sampler indices to textures.
		 */
		typedef std::unordered_map<SamplerIndex, TextureRef> SamplerMap;

	public:

		/**
		 * Creates a new OpenGL ES shader program container.
		 */
		GLESShaderProgram();

		/**
		 * Destructs an OpenGL ES shader program container.
		 */
		~GLESShaderProgram() override;

		/**
		 * Returns the OpenGL ES shader program id.
		 * @return OpenGL ES program id
		 */
		inline GLuint id() const;

		/**
		 * Links a vertex and a fragment shader.
		 * @param programType The type of the shader program, must be valid
		 * @param vertexShader Vertex shader to link
		 * @param fragmentShader Fragment shader to link
		 * @param message Returning linker error or warning message.
		 * @return True, if succeeded
		 */
		bool link(const ProgramType programType, const GLESShaderRef& vertexShader, const GLESShaderRef& fragmentShader, std::string& message);

		/**
		 * Links several shaders.
		 * @param programType The type of the shader program, must be valid
		 * @param shaders The shaders to link
		 * @param message Returning linker error or warning message.
		 * @return True, if succeeded
		 */
		bool link(const ProgramType programType, const std::vector<GLESShaderRef>& shaders, std::string& message);

		/**
		 * Compiles and links a vertex and a fragment shader.
		 * @param programType The type of the shader program, must be valid
		 * @param vertexShaderCode Vertex shader code to compile and link, can be composed of several individual code blocks
		 * @param fragmentShaderCode Fragment shader code to compile and link, can be composed of several individual code blocks
		 * @param message Returning compiler or linker error or warning message.
		 * @return True, if succeeded
		 */
		bool compileAndLink(const ProgramType programType, const std::vector<const char*>& vertexShaderCode, const std::vector<const char*>& fragmentShaderCode, std::string& message);

		/**
		 * Compiles and links shaders.
		 * @param programType The type of the shader program, must be valid
		 * @param shaderCodePairs The pairs of shader types and shader codes, the shader codes can be composed of several individual code blocks
		 * @param message Returning compiler or linker error or warning message.
		 * @return True, if succeeded
		 */
		bool compileAndLink(const ProgramType programType, const ShaderCodePairs& shaderCodePairs, std::string& message);

		/**
		 * Uses the shader and binds the given projection and model matrices as OpenGL uniforms.
		 * @param projection The projection matrix used for this node
		 * @param camera_T_model The transformation between model and camera (aka Modelview matrix), must be valid
		 * @param camera_T_world The transformation between world and camera,(aka View matrix) must be valid
		 * @param normalMatrix Normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix
		 */
		void bind(const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix) const;

		/**
		 * Uses the shader and binds the given projection and model matrices as OpenGL uniforms.
		 * @param framebuffer The framebuffer which initiated the rendering process
		 * @param projection The projection matrix used for this node
		 * @param camera_T_model The transformation between model and camera (aka Modelview matrix), must be valid
		 * @param camera_T_world The transformation between world and camera,(aka View matrix) must be valid
		 * @param normalMatrix Normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix
		 */
		void bind(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix);

		/**
		 * Returns the shader type necessary to render an object with this attribute.
		 * @see GLESAttribute::necessaryShader().
		 */
		ProgramType necessaryShader() const override;

		/**
		 * Binds all (custom) attributes of this shader program.
		 * @see GLESAttribute::bindAttribute().
		 */
		void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) override;

		/**
		 * Sets the shading code (a custom shader, not a shader from GLESProgramManager).
		 * @see ShaderProgram::setShader().
		 */
		bool setShader(const ShaderLanguage shaderLanguage, const FilenamePairs& filenamePairs, std::string& errorMessage) override;

		/**
		 * Sets the shading code (a custom shader, not a shader from GLESProgramManager).
		 * @see ShaderProgram::setShader().
		 */
		bool setShader(const ShaderLanguage shaderLanguage, const std::string& vertexShaderCode, const std::string& fragmentShaderCode, std::string& errorMessage) override;

		/**
		 * Sets the shading code.
		 * @see ShaderProgram::setShader().
		 */
		bool setShader(const ShaderLanguage shaderLanguage, const std::vector<const char*>& vertexShaderCode, const std::vector<const char*>& fragmentShaderCode, std::string& errorMessage) override;

		/**
		 * Sets the shader code.
		 * @see ShaderProgram::setShader().
		 */
		bool setShader(const ShaderLanguage shaderLanguage, const CodePairs& codePairs, std::string& errorMessage) override;

		/**
		 * Returns whether this shader program has a specific parameter.
		 * @see ShaderProgram::existParameter().
		 */
		bool existParameter(const std::string& name) const override;

		/**
		 * Returns the type of a specific parameter.
		 * @see ShaderProgram::parameterType().
		 */
		ParameterType parameterType(const std::string& name) const override;

		/**
		 * Returns the number of sampler parameters.
		 * @see ShaderProgram::samplerNumber().
		 */
		unsigned int samplerNumber() const override;

		/**
		 * Returns the index of a registered texture sample object.
		 * @see ShaderProgram::samplerIndex().
		 */
		SamplerIndex samplerIndex(const TextureRef& sampler) const override;

		/**
		 * Returns the number of elements of a specific parameter.
		 * @see ShaderProgram::parameterElements().
		 */
		unsigned int parameterElements(const std::string& name) const override;

		/**
		 * Sets the texture of a specified sampler.
		 * @see ShaderProgram::setSampler().
		 */
		bool setSampler(const SamplerIndex index, const TextureRef& texture) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const double value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const float value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const float* value, const unsigned int elements) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const int value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const unsigned int value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const HomogenousMatrix4& value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const SquareMatrix3& value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const SquareMatrix4& value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const Vector2& value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const Vector3& value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const Vector4& value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const HomogenousMatrices4& value) override;

		/**
		 * Sets a parameter by a given parameter name.
		 * @see ShaderProgram::setParameter().
		 */
		bool setParameter(const std::string& name, const SquareMatrices3& value) override;

		/**
		 * Returns whether this program holds a valid and successfully compiled and linked shader code.
		 * @see ShaderProgram::isCompiled().
		 */
		bool isCompiled() const override;

		/**
		 * Translates the OpenGL shader type to a readable string.
		 * @param shaderType The shader type to translate
		 * @return The readable string
		 */
		static std::string translateShaderType(const GLenum shaderType);

	protected:

		/**
		 * Releases the shader program.
		 */
		void release();

	protected:

		/// OpenGL ES shader program id.
		GLuint id_;

		/// The type of the shader program.
		ProgramType programType_;

		/// The shaders.
		std::vector<GLESShaderRef> shaders_;

		/// The map of texture samplers.
		SamplerMap samplers_;

		/// The map for floating point values.
		Parameters<float> parametersFloat_;

		/// The map for arrays of floating point value.
		Parameters<std::vector<float>> parametersFloats_;

		/// The map of integer values.
		Parameters<int> parametersInt_;

		/// The map of integer values.
		Parameters<unsigned int> parametersUnsignedInt_;

		/// The map of HomogenousMatrix4 values.
		Parameters<HomogenousMatrix4> parametersHomogenousMatrix4_;

		/// The map of SquareMatrix3 values.
		Parameters<SquareMatrix3> parametersSquareMatrix3_;

		/// The map of SquareMatrix4 values.
		Parameters<SquareMatrix4> parametersSquareMatrix4_;

		/// The map of Vector2 values.
		Parameters<Vector2> parametersVector2_;

		/// The map of Vector3 values.
		Parameters<Vector3> parametersVector3_;

		/// The map of Vector4 values.
		Parameters<Vector4> parametersVector4_;

		/// The map of HomogenousMatrices4 values.
		Parameters<HomogenousMatrices4> parametersHomogenousMatrices4_;

		/// The map of SquareMatrices3 values.
		Parameters<SquareMatrices3> parametersSquareMatrices3_;
};

template <typename T>
void GLESShaderProgram::Parameters<T>::setValue(const std::string& name, const T& value)
{
	valueMap_[name] = value;
}

template <typename T>
void GLESShaderProgram::Parameters<T>::setValue(const std::string& name, T&& value)
{
	valueMap_.emplace(name, std::move(value));
}

template <>
inline void GLESShaderProgram::Parameters<std::vector<float>>::bindValues(const GLuint id) const
{
	for (typename ValueMap::const_iterator i = valueMap_.cbegin(); i != valueMap_.cend(); ++i)
	{
		const std::string& name = i->first;
		const std::vector<float>& value = i->second;

		const GLint locationId = glGetUniformLocation(id, name.c_str());
		if (locationId != -1)
		{
			setUniform(locationId, value.data(), value.size());
		}
	}
}

template <typename T>
void GLESShaderProgram::Parameters<T>::bindValues(const GLuint id) const
{
	for (typename ValueMap::const_iterator i = valueMap_.cbegin(); i != valueMap_.cend(); ++i)
	{
		const std::string& name = i->first;
		const T& value = i->second;

		const GLint locationId = glGetUniformLocation(id, name.c_str());
		if (locationId != -1)
		{
			setUniform(locationId, value);
		}
	}
}

inline GLuint GLESShaderProgram::id() const
{
	return id_;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_SHADER_PROGRAM_H
