/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_SHADER_PROGRAM_H
#define META_OCEAN_RENDERING_SHADER_PROGRAM_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Texture.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class ShaderProgram;

/**
 * Definition of a smart object reference holding a shader program object.
 * @see SmartObjectRef, ShaderProgram.
 * @ingroup rendering
 */
typedef SmartObjectRef<ShaderProgram> ShaderProgramRef;

/**
 * This class implements a shader program attribute.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT ShaderProgram : virtual public Attribute
{
	public:

		/**
		 * Definition of individual shader languages.
		 */
		enum ShaderLanguage : uint32_t
		{
			/// An invalid shader language.
			SL_INVALID = 0u,
			/// The GLSL shader language (e.g., used by OpenGL and OpenGL ES).
			SL_GLSL,
			/// The Cg shader language (e.g., used by NVIDIA).
			SL_CG
		};

		/**
		 * Definition of individual shader types.
		 */
		enum ShaderType : uint32_t
		{
			/// An invalid shader type.
			ST_INVALID = 0u,
			/// A compute shader.
			ST_COMPUTE,
			/// A fragment shader.
			ST_FRAGMENT,
			/// A geometry shader.
			ST_GEOMETRY,
			/// A Tessellation control shader.
			ST_TESSELLATION_CONTROL,
			/// A Tessellation evaluation shader.
			ST_TESSELLATION_EVALUATION,
			/// A vertex shader.
			ST_VERTEX,
			//// A unified shader (e.g., combining vertex and fragment shader).
			ST_UNIFIED
		};

		/**
		 * Definition of a pair combining a filename with a shader type with.
		 */
		typedef std::pair<std::string, ShaderType> FilenamePair;

		/**
		 * Definition of a vector holding pairs combining filenames with shader types.
		 */
		typedef std::vector<FilenamePair> FilenamePairs;

		/**
		 * Definition of a pair combining shader code parts with a shader type with.
		 */
		typedef std::pair<std::vector<const char*>, ShaderType> CodePair;

		/**
		 * Definition of a vector holding pairs combining shader codes with shader types.
		 */
		typedef std::vector<CodePair> CodePairs;

		/**
		 * Definition of a parameter types.
		 */
		enum ParameterType : uint32_t
		{
			/// Invalid parameter.
			TYPE_INVALID = 0u,
			/// Unsupported parameter.
			TYPE_UNSUPPORTED,
			/// Boolean parameter.
			TYPE_BOOL,
			/// Float parameter.
			TYPE_FLOAT,
			/// Integer parameter.
			TYPE_INTEGER,
			/// 3x3 matrix parameter.
			TYPE_MATRIX3,
			/// 4x4 matrix parameter.
			TYPE_MATRIX4,
			/// 1D sample parameter.
			TYPE_SAMPLE1,
			/// 2D sample parameter.
			TYPE_SAMPLE2,
			/// 3D sample parameter.
			TYPE_SAMPLE3,
			/// Struct parameter.
			TYPE_STRUCT,
			/// 2D vector parameter.
			TYPE_VECTOR2,
			/// 3D vector parameter.
			TYPE_VECTOR3,
			/// 4D vector parameter.
			TYPE_VECTOR4
		};

		/**
		 * Definition of a sampler index.
		 */
		typedef unsigned int SamplerIndex;

		/**
		 * Definition of an invalid sampler index.
		 */
		static constexpr SamplerIndex invalidSamplerIndex = SamplerIndex(-1);

	protected:

		/**
		 * Definition of a vector holding sample objects.
		 */
		typedef std::vector<TextureRef> Textures;

	public:

		/**
		 * Sets the shader code.
		 * @param shaderLanguage The language of the given shaders.
		 * @param filenamePairs The pairs of filenames and shader types which are necessary to defined the shader, at least one
		 * @param errorMessage Returning error message if the code is not valid
		 * @return True, if the shader could be compiled successfully
		 */
		virtual bool setShader(const ShaderLanguage shaderLanguage, const FilenamePairs& filenamePairs, std::string& errorMessage);

		/**
		 * Sets the shader code.
		 * @param shaderLanguage The language of the given shaders.
		 * @param vertexShaderCode The code of the vertex shader, must be valid
		 * @param fragmentShaderCode The code of the fragment shader, must be valid
		 * @param errorMessage Returning error message if the code is not valid
		 * @return True, if the shader could be compiled successfully
		 */
		virtual bool setShader(const ShaderLanguage shaderLanguage, const std::string& vertexShaderCode, const std::string& fragmentShaderCode, std::string& errorMessage);

		/**
		 * Sets the shader code.
		 * @param shaderLanguage The language of the given shaders.
		 * @param vertexShaderCode The code of the vertex shader, can be composed of several individual code blocks
		 * @param fragmentShaderCode The code of the fragment shader, can be composed of several individual code blocks
		 * @param errorMessage Returning error message if the code is not valid
		 * @return True, if the shader could be compiled successfully
		 */
		virtual bool setShader(const ShaderLanguage shaderLanguage, const std::vector<const char*>& vertexShaderCode, const std::vector<const char*>& fragmentShaderCode, std::string& errorMessage);

		/**
		 * Sets the shader code.
		 * @param shaderLanguage The language of the given shaders.
		 * @param codePairs The codes of the shader, each shader can be composed of several individual code blocks
		 * @param errorMessage Returning error message if the code is not valid
		 * @return True, if the shader could be compiled successfully
		 */
		virtual bool setShader(const ShaderLanguage shaderLanguage, const CodePairs& codePairs, std::string& errorMessage);

		/**
		 * Returns whether this shader program has a specific parameter.
		 * @param name The name of the parameter to check
		 * @return True, if so
		 */
		virtual bool existParameter(const std::string& name) const;

		/**
		 * Returns the type of a specific parameter.
		 * @param name The name of the parameter to return the type for
		 * @return Parameter type
		 */
		virtual ParameterType parameterType(const std::string& name) const;

		/**
		 * Returns the number of sampler parameters.
		 * @return Number of samplers
		 */
		virtual unsigned int samplerNumber() const;

		/**
		 * Returns the index of a registered texture sample object.
		 * @param sampler The sampler to return the index for
		 * @return Index of the specified sampler, an invalid index is returned if the sampler is not registered
		 */
		virtual SamplerIndex samplerIndex(const TextureRef& sampler) const;

		/**
		 * Returns the number of elements of a specific parameter.
		 * @param name The name of the parameter to return the number of elements for
		 * @return Number of elements
		 */
		virtual unsigned int parameterElements(const std::string& name) const;

		/**
		 * Sets the texture of a specified sampler.
		 * @param index The index of the shader sampler to set
		 * @param texture The texture object to set
		 * @return True, if succeeded
		 */
		virtual bool setSampler(const SamplerIndex index, const TextureRef& texture);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const double value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const float value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value Array value to set
		 * @param elements Number of elements in the array
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const float* value, const unsigned int elements);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const int value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const unsigned int value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const HomogenousMatrix4& value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const SquareMatrix3& value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const SquareMatrix4& value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const Vector2& value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const Vector3& value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const Vector4& value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const HomogenousMatrices4& value);

		/**
		 * Sets a parameter by a given parameter name.
		 * @param name The name of the parameter to set
		 * @param value The value to set
		 * @return True, if succeeded
		 */
		virtual bool setParameter(const std::string& name, const SquareMatrices3& value);

		/**
		 * Returns whether this program holds a valid and successfully compiled and linked shader code.
		 * @return True, if so
		 */
		virtual bool isCompiled() const;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new shader program object.
		 */
		ShaderProgram();

		/**
		 * Destructs a shader program object.
		 */
		~ShaderProgram() override;

	protected:

		/// Vector holding all registered texture samples.
		Textures shaderProgramTextures;
};

}

}

#endif // META_OCEAN_RENDERING_SHADER_PROGRAM_H
